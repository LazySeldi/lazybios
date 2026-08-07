# Fuzzing lazybios

Seven libFuzzer targets cover the untrusted bytes, bounded traversal helpers,
backend transformations, and lifecycle paths used by the library. The two
file-loader targets are POSIX-only; the other five build anywhere the Clang
libFuzzer runtime is available.

| Target | Covers |
| --- | --- |
| `fuzz_dmi_table` | All 47 structure parsers against an arbitrary DMI table, with the SMBIOS version driven by the input |
| `fuzz_entry_point` | `lazybiosParseEntry`, `lazybiosIsVersionPlus`, `lazybiosPrintSMVer` |
| `fuzz_single_file` | `lazybiosSingleFile` plus every parser, including tightly concatenated tables and valid embedded table offsets |
| `fuzz_two_files` | `lazybiosFile` plus every parser — the shape a Linux sysfs host load takes |
| `fuzz_decoders` | Every decoder helper, including the ones that format into a caller-supplied buffer |
| `fuzz_helpers` | `DMINext`, `DMIString`, all-type counting, partial-context cleanup, and unavailable `lazybiosInit` dispatches |
| `fuzz_backend_buffers` | Windows `RawSMBIOSData`, raw-buffer loading used by macOS, physical-memory entry-point scanning, and SMBIOS 2.x/3.x table-address extraction used by NetBSD |

The DMI table and entry point buffers are allocated at exactly the size the
library is told they are, so an over-read of a single byte is a hard ASan
failure rather than something that silently reads adjacent heap.

## Building

Requires Clang. The fuzz targets link a separate static, instrumented copy of
the library, so this build does not change the flags of the shared library.

```sh
cmake -B build-fuzz -DCMAKE_C_COMPILER=clang -DLAZYBIOS_BUILD_FUZZERS=ON
cmake --build build-fuzz
```

ASan and UBSan are on by default and abort on the first report
(`-fno-sanitize-recover=all`). Override with `-DLAZYBIOS_FUZZ_SANITIZERS=...`,
for example `memory` for MSan or an empty string for coverage only.

## Running

```sh
fuzz/make_corpus.sh
fuzz/run_all.sh build-fuzz 60 1048576
fuzz/run_all.sh --help
```

The arguments to `run_all.sh` are the build directory, seconds per target, and
maximum generated input size. The explicit 1 MiB limit keeps large-table
allocation and length arithmetic in the campaign; libFuzzer otherwise defaults
to relatively small generated inputs. The runner prints the selected build,
corpus, and findings directories before starting.

Each target reads seeds from a matching subdirectory under
`build-fuzz/corpus`. The seeds come from `test-dumps/`, which holds real
firmware tables from roughly 97 machines. Run a single target directly when
working on one area:

```sh
./build-fuzz/fuzz/fuzz_dmi_table build-fuzz/corpus/dmi_table \
    -max_total_time=60 -max_len=1048576
```

If a target finds a crash, timeout, or out-of-memory condition, libFuzzer prints
the saved path and exits. The runner stores that input under
`build-fuzz/artifacts/`, prefixed with the target name. Reproduce it by passing
the saved input directly:

```sh
./build-fuzz/fuzz/fuzz_dmi_table \
    build-fuzz/artifacts/dmi_table-crash-<hash>
```

## Regressions

Inputs that once crashed the library are kept under `regressions/`, seeded into
the corpora by `make_corpus.sh`, and replayed on demand:

```sh
fuzz/run_regressions.sh build-fuzz
ctest --test-dir build-fuzz --output-on-failure
```

Save the crashing input there — in the directory named after the target that
found it — whenever a fuzz finding is fixed.

## Semantic tests

Fuzzing can detect memory errors and undefined behavior, but it cannot know
whether a valid field was interpreted according to the SMBIOS specification.
`lazybios_semantic_test` contains deterministic, specification-derived checks,
including signed Type 28 temperatures, field availability, numeric
conversions, traversal contracts, backend wrappers, and free functions:

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Add a focused semantic case whenever a parser or decoder bug produces the wrong
value without crashing.

## Sanitizer variants

Run both the normal quiet build and the debug-logging build. MSan is a separate
configuration because it cannot be combined with ASan. The complete local
matrix can be configured, built, and run with:

```sh
fuzz/check_variants.sh 60 1048576
```

To build an individual MSan configuration:

```sh
cmake -S . -B build-fuzz-msan -DCMAKE_C_COMPILER=clang \
    -DLAZYBIOS_BUILD_FUZZERS=ON -DLAZYBIOS_FUZZ_SANITIZERS=memory
cmake --build build-fuzz-msan
fuzz/run_all.sh build-fuzz-msan 60 1048576
```

## Covering the debug-only paths

The instrumented library is built with `LAZYBIOS_QUIET` so its logging does not
bury the fuzzer's output. `lb_dbg()` then compiles to nothing, and anything
reached only from a `lb_dbg()` argument or condition — the entry point checksum
verification, for one — becomes dead code the fuzzer cannot enter. To cover it,
configure with `-DLAZYBIOS_FUZZ_LOGGING=ON` and send the library's chatter
somewhere else while keeping the sanitizer reports:

```sh
cmake -B build-fuzz-log -DCMAKE_C_COMPILER=clang -DLAZYBIOS_BUILD_FUZZERS=ON \
      -DLAZYBIOS_FUZZ_LOGGING=ON
cmake --build build-fuzz-log
ASAN_OPTIONS=log_path=/tmp/asan UBSAN_OPTIONS=log_path=/tmp/ubsan \
    ./build-fuzz-log/fuzz/fuzz_entry_point build-fuzz/corpus/entry_point \
    -max_total_time=60 2>/dev/null
```

## Source coverage

Coverage reports show which lines and branches the seed corpora actually
reach; the number of targets alone cannot establish that. The report script
requires `llvm-profdata` and `llvm-cov` from the same LLVM release as Clang.

```sh
cmake -S . -B build-fuzz-coverage -DCMAKE_C_COMPILER=clang \
    -DLAZYBIOS_BUILD_FUZZERS=ON -DLAZYBIOS_FUZZ_SANITIZERS= \
    -DLAZYBIOS_FUZZ_COVERAGE=ON
cmake --build build-fuzz-coverage
fuzz/coverage.sh build-fuzz-coverage
```

The HTML report is written to
`build-fuzz-coverage/coverage/html/index.html`.

The platform-neutral parts of the Windows, macOS, Linux, OpenBSD, FreeBSD,
NetBSD, SunOS (Solaris/illumos), DragonFly BSD, Haiku, BeOS, and generic
backends are fuzzed by `fuzz_backend_buffers`. This includes NetBSD's
checksum-validated SMBIOS 2.x/3.x table-address and table-length extraction.
Calls into `GetSystemFirmwareTable`, IOKit, sysfs, EFI systab, OpenBSD dmesg,
FreeBSD and DragonFly BSD `kenv`, NetBSD `sysctlbyname`, `/dev/smbios`,
`/dev/mem`, `/dev/misc/mem`, `mmap`, `pread`, `lseek`/`read`, and permission
handling remain platform integration work. Exercise them on a native machine
with:

```sh
cmake -S . -B build-host-test -DBUILD_TESTING=ON \
    -DLAZYBIOS_TEST_HOST_BACKEND=ON
cmake --build build-host-test
ctest --test-dir build-host-test --output-on-failure
```

The host test is reported as skipped when SMBIOS data is unavailable or the
current account lacks the required access.

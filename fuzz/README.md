# Fuzzing lazybios

Five libFuzzer targets covering the untrusted input the library consumes:
the SMBIOS entry point and the DMI structure table.

| Target | Covers |
| --- | --- |
| `fuzz_dmi_table` | All 47 structure parsers against an arbitrary DMI table, with the SMBIOS version driven by the input |
| `fuzz_entry_point` | `lazybiosParseEntry`, `lazybiosIsVersionPlus`, `lazybiosPrintVer` |
| `fuzz_single_file` | `lazybiosSingleFile` plus every parser, through the real file-loading path |
| `fuzz_two_files` | `lazybiosFile` plus every parser — the shape a Linux sysfs host load takes |
| `fuzz_decoders` | Every decoder helper, including the ones that format into a caller-supplied buffer |

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
fuzz/make_corpus.sh build-fuzz/corpus
./build-fuzz/fuzz/fuzz_dmi_table build-fuzz/corpus/dmi_table -max_total_time=60
```

Each target reads its seeds from the matching subdirectory: `dmi_table`,
`entry_point`, `single_file`, `two_files`, `decoders`. The seeds come from
`test-dumps/`, which holds real firmware tables from ~97 machines.

To reproduce a crash, pass the saved input file directly:

```sh
./build-fuzz/fuzz/fuzz_dmi_table crash-<hash>
```

## Regressions

Inputs that once crashed the library are kept under `regressions/`, seeded into
the corpora by `make_corpus.sh`, and replayed on demand:

```sh
fuzz/run_regressions.sh build-fuzz
```

Save the crashing input there — in the directory named after the target that
found it — whenever a fuzz finding is fixed.

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

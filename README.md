# lazybios 

### A lightweight cross-platform C library for parsing SMBIOS/DMI tables.
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://github.com/LazySeldi/lazybios/graphs/commit-activity)

---
## Features

- **SMBIOS Version Detection** - Supports SMBIOS 2.x and 3.x(and future ones).
- **Clean C API** - Simple function calls, only 3 steps to ensure memory-safety. Getters return a result set that keeps each array and its count together.
- **Fast** - The table is indexed once at load, so parsers jump straight to the structures they want instead of rescanning the whole table.
- **Library Extensions** - Built-in cJSON output supports every implemented standard SMBIOS type (0-46), plus the supported OEM structures.
- **OEM Extensions** - Initial experimental support for HP Type 204 and Dell Types 177, 212, and 218 OEM-specific structures (More coming soon). dmidecode was used as a reference for these OEM types. 
- **Zero Dependencies** - Pure C standard library, except libc.
- **Memory Safe** - Proper allocation and cleanup.
- **Cross Platform** - Host SMBIOS loading is supported on Linux, Windows, macOS, OpenBSD, FreeBSD, NetBSD, SunOS (Solaris/illumos), DragonFly BSD, Haiku, BeOS, and ReactOS.
- **Human-readable values** - Every bitfield and enum is decoded during parsing into a `decoded` member, alongside the raw encoding. Nothing to call.
- **Easy to integrate** - Works naturally from C, C++, and other languages capable of calling C APIs.
- **Always up-to-date** - Implemented against the latest published DMTF SMBIOS specification.
- **Architectures:** x86_64, ARM (32/64), RISC-V 64(Did run on RISC-V, but not heavily tested), and others.
---

## A quick look

```c
#include <lazybios/lazybios.h>

lazybiosCTX_t* ctx = lazybiosCTXNew();
if (!ctx || lazybiosInit(ctx, NULL, NULL) != 0) return 1;  /* read this machine */

ctx->Type17 = lazybiosGetType17(ctx->DMIData);      /* memory devices */
if (!ctx->Type17) { lazybiosCleanup(ctx); return 1; } /* free the old pointer if it exists */

for (size_t i = 0; i < ctx->Type17->count; i++) {
    const lazybiosType17_t* dev = &ctx->Type17->entries[i];
    if (LAZYBIOS_FIELD_STATUS(dev, size) == LAZYBIOS_FIELD_PRESENT)
        printf("%s: %u MB, %s\n", dev->device_locator,
               dev->size, dev->decoded.memory_type); /* fields in decoded are already decoded */
}

lazybiosCleanup(ctx);                               /* frees everything above */
```

Three steps: make a context, load data into it, parse what you need. The context
owns whatever it hands you, so one `lazybiosCleanup` releases all of it.

`lazybiosInit` is the only loader, and its two path arguments pick the source:

```c
lazybiosInit(ctx, NULL, NULL);                        /* this machine        */
lazybiosInit(ctx, NULL, "dump.bin");                  /* one merged file     */
lazybiosInit(ctx, "smbios_entry_point", "DMI");       /* two separate files  */
```

The file modes need no privileges and behave identically everywhere, which makes
them the practical way to test or to reproduce a report from another machine.

Want the whole table instead of a few types? `lazybiosParseAll(ctx)` fills every
member in one call, and `lazybiosParseJSONAll(ctx, root)` does the same straight
into a cJSON tree.

Every parsed field comes with three views — the raw encoding the firmware wrote,
a decoded form in `decoded`, and a `field_status` saying whether the firmware
populated it at all.

---

## Project Status
1. **Windows support:** Implemented using Windows API calls. \
   **Linux support:** Implemented via sysfs, EFI system-table discovery, and `/dev/mem`. \
   **macOS support:** Implemented using the AppleSMBIOS I/O Registry service through IOKit and CoreFoundation. \
   **OpenBSD support:** Implemented via `/var/run/dmesg.boot` discovery and validated `/dev/mem` access. \
   **FreeBSD support:** Implemented via the `hint.smbios.0.mem` kernel environment value and validated `/dev/mem` access. \
   **NetBSD support:** Implemented via `machdep.smbios` and the address-based `/dev/smbios` interface, with `/dev/mem` fallbacks. \
   **SunOS (Solaris/illumos) support:** Implemented via the offset-aware `/dev/smbios` snapshot, with a legacy `/dev/mem` fallback. \
   **DragonFly BSD support:** Implemented via the `hint.smbios.0.mem` kernel environment value and validated `/dev/mem` access. \
   **Haiku support:** Scans the legacy x86 firmware window through `/dev/misc/mem` or `/dev/mem`, with positional-read fallbacks when device mapping is unavailable. \
   **BeOS support:** Uses the same validated legacy x86 physical-memory loader and device fallbacks. \
   **ReactOS support:** Implemented using Win32 firmware-table API calls.
2. Host loading is supported on Linux, Windows, macOS, OpenBSD, FreeBSD, NetBSD, SunOS (Solaris/illumos), DragonFly BSD, Haiku, BeOS, and ReactOS. File-based parsing is available independently of the selected host backend.
3. The library is nearing a very stable status. Features are unlikely to change!

---

## About This Project
1. Bugs may exist(but very rare, since all my releases are tested against the SMBIOS tables in test-dumps folder), and contributions are welcome.  
2. There are no arbitrary version limits: the library implements structures according to the latest DMTF SMBIOS specification.  
3. All new structures follow the current latest [DMTF SMBIOS Specification](https://www.dmtf.org/sites/default/files/standards/documents/DSP0134_3.9.0.pdf).

---


## In here are all of the currently supported SMBIOS types:

<details>
  <summary>Implemented Types</summary>

| Type | Structure |
| ---: | --- |
| 0 | BIOS Information |
| 1 | System Information |
| 2 | Baseboard Information |
| 3 | Chassis Information |
| 4 | Processor Information |
| 5 | Memory Controller Information (Obsolete) |
| 6 | Memory Module Information (Obsolete) |
| 7 | Cache Information |
| 8 | Port Connector Information |
| 9 | System Slots |
| 10 | On Board Devices Information (Obsolete) |
| 11 | OEM Strings |
| 12 | System Configuration Options |
| 13 | Firmware Language Information |
| 14 | Group Associations |
| 15 | System Event Log |
| 16 | Physical Memory Array |
| 17 | Memory Device Information |
| 18 | 32-Bit Memory Error Information |
| 19 | Memory Array Mapped Address |
| 20 | Memory Device Mapped Address |
| 21 | Built-in Pointing Device |
| 22 | Portable Battery |
| 23 | System Reset |
| 24 | Hardware Security |
| 25 | System Power Controls |
| 26 | Voltage Probe |
| 27 | Cooling Device |
| 28 | Temperature Probe |
| 29 | Electrical Current Probe |
| 30 | Out-of-Band Remote Access |
| 31 | Boot Integrity Services Entry Point |
| 32 | System Boot Information |
| 33 | 64-Bit Memory Error Information |
| 34 | Management Device |
| 35 | Management Device Component |
| 36 | Management Device Threshold Data |
| 37 | Memory Channel |
| 38 | IPMI Device Information |
| 39 | System Power Supply |
| 40 | Additional Information |
| 41 | Onboard Devices Extended Information |
| 42 | Management Controller Host Interface |
| 43 | TPM Device |
| 44 | Processor Additional Information |
| 45 | Firmware Inventory Information |
| 46 | String Property |
| 128-255 | OEM-specific Structures |
| 177 | Dell BIOS Flags |
| 204 | HPE ProLiant System/Rack Locator |
| 212 | Dell Indexed I/O Access |
| 218 | Dell Token Interface |

</details>

---
## Installation
[![aur.archlinux.org.io](https://img.shields.io/aur/version/lib-lazybios)](https://aur.archlinux.org/packages/lib-lazybios)
<details>
<summary>Arch Linux</summary>

You can install lazybios from the AUR.

```shell
yay -S lib-lazybios # Or use your preferred AUR helper.
```
</details>

---
<details>
<summary>Other distros? (Community packages welcome!)</summary>

Yet to come!😞

</details>

---

<details>
<summary>Manual Building and installing with CMake.</summary>

```shell
git clone https://github.com/LazySeldi/lazybios.git

cd lazybios

mkdir build

cd build

cmake .. # Defaults to /usr/local; use -DCMAKE_INSTALL_PREFIX=/usr to override it.

make

sudo make install # Optional; installs to the configured prefix.
```
</details>

---
## Documentation

Generate the complete user guide and API reference with:

```shell
cmake --build build --target docs
```

That injects the project version from CMake, so the version lives in exactly one
place. Plain `doxygen Doxyfile` still works if you would rather not go through
CMake; it just leaves the version off the generated pages.

The entry point is `docs/html/index.html`.

### Testing and Examples

The library includes several test targets:
- `lazybios_test`: The main inspection and verification tool.
- `lazybios_oem_test`: Standalone test for OEM-specific structures.
- `lazybios_json_test`: Verification for the JSON support.
- `lazybios_semantic_test`: Specification-based semantic validation.

It's best to review the `test/` directory for integration examples.

### Heads up: When using the library you may need to read some of the current SMBIOS specification that the library uses(for example lazybios will always use the latest specification, right now the latest is [3.9.0](https://www.dmtf.org/sites/default/files/standards/documents/DSP0134_3.9.0.pdf)) Since some fields may have some quirks. It's best to review test.c for things you're stuck on.

## Fuzzing

The library parses untrusted firmware data, so seven libFuzzer targets cover
entry points, structure tables, file loaders, traversal helpers,
cleanup paths, JSON serialization, and platform-neutral backend
transformations:

```shell
cmake -B build-fuzz -DCMAKE_C_COMPILER=clang -DLAZYBIOS_BUILD_FUZZERS=ON
cmake --build build-fuzz
fuzz/run_all.sh build-fuzz 60 1048576
```

Deterministic semantic tests complement fuzzing for specification errors that
do not crash:

```shell
ctest --test-dir build --output-on-failure
```

- **[fuzz/README.md](fuzz/README.md)** - Targets, sanitizer variants, regressions, large-input campaigns, and coverage reports.

## Contributing:
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - Complete guide on how to contribute!

## Issues:
- **[ISSUES.md](ISSUES.md)** - Complete guide for opening issues!

## License

lazybios is licensed under the LGPL-2.1-or-later with a static linking exception
for open-source projects. See the [LICENSE](LICENSE) file for details and [LICENSE-EXCEPTIONS](LICENSE-EXCEPTIONS.md) file for exception details.

For open-source projects: You may statically link lazybios and distribute the
resulting binary under your own license, as long as the source code to
lazybios (including any modifications) is made available under the LGPL.

For proprietary projects: The standard LGPL terms apply.

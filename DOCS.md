# lazybios Documentation

`lazybios` is a lightweight C library for reading and parsing SMBIOS/DMI data. It exposes a small context-based API, parses SMBIOS entry point metadata, and currently implements SMBIOS Types 0 through 4.

Current public project version: `0.1.0`.

## Table of Contents

- [Project Overview](#project-overview)
- [Build and Installation](#build-and-installation)
  - [Prerequisites](#prerequisites)
  - [Build From Source](#build-from-source)
  - [CMake and pkg-config](#cmake-and-pkg-config)
  - [Logging Options](#logging-options)
- [Runtime Backends and Input Modes](#runtime-backends-and-input-modes)
  - [System Initialization](#system-initialization)
  - [Separate Dump Files](#separate-dump-files)
  - [Single Merged Dump File](#single-merged-dump-file)
  - [Test Executable](#test-executable)
- [Core Concepts](#core-concepts)
  - [Context Layout](#context-layout)
  - [DMI Data Layout](#dmi-data-layout)
  - [Memory Ownership](#memory-ownership)
  - [Sentinel Values](#sentinel-values)
  - [Version Checks](#version-checks)
- [Quick Start](#quick-start)
  - [Read From the Host System](#read-from-the-host-system)
  - [Read From Dump Files](#read-from-dump-files)
  - [Read Multiple Structures](#read-multiple-structures)
- [Public API Reference](#public-api-reference)
  - [Context Management Functions](#context-management-functions)
  - [Data Loading Functions](#data-loading-functions)
  - [Core Parsing Helper Functions](#core-parsing-helper-functions)
  - [General Helper Functions](#general-helper-functions)
- [Public Data Structures](#public-data-structures)
  - [smbios_entry_info_t](#smbios_entry_info_t)
  - [lazybiosDMI_t](#lazybiosdmi_t)
  - [lazybiosCTX_t](#lazybiosctx_t)
- [SMBIOS Type Reference](#smbios-type-reference)
  - [Type 0 BIOS Information](#type-0-bios-information)
    - [Type 0 Overview](#type-0-overview)
    - [Type 0 Fields](#type-0-fields)
    - [Type 0 Functions](#type-0-functions)
    - [Type 0 Example](#type-0-example)
    - [Type 0 Notes](#type-0-notes)
  - [Type 1 System Information](#type-1-system-information)
    - [Type 1 Overview](#type-1-overview)
    - [Type 1 Fields](#type-1-fields)
    - [Type 1 Functions](#type-1-functions)
    - [Type 1 Example](#type-1-example)
    - [Type 1 Notes](#type-1-notes)
  - [Type 2 Baseboard Information](#type-2-baseboard-information)
    - [Type 2 Overview](#type-2-overview)
    - [Type 2 Fields](#type-2-fields)
    - [Type 2 Functions](#type-2-functions)
    - [Type 2 Example](#type-2-example)
    - [Type 2 Notes](#type-2-notes)
  - [Type 3 Chassis Information](#type-3-chassis-information)
    - [Type 3 Overview](#type-3-overview)
    - [Type 3 Fields](#type-3-fields)
    - [Type 3 Functions](#type-3-functions)
    - [Type 3 Example](#type-3-example)
    - [Type 3 Notes](#type-3-notes)
  - [Type 4 Processor Information](#type-4-processor-information)
    - [Type 4 Overview](#type-4-overview)
    - [Type 4 Fields](#type-4-fields)
    - [Type 4 Functions](#type-4-functions)
    - [Type 4 Example](#type-4-example)
    - [Type 4 Notes](#type-4-notes)
- [Supported SMBIOS Type Constants](#supported-smbios-type-constants)
- [Limitations and Platform Notes](#limitations-and-platform-notes)

## Project Overview

The library is written in C and built with CMake. It reads raw SMBIOS/DMI tables from supported host backends or from files, stores the raw table data in a `lazybiosDMI_t`, and allocates parsed structures on demand.

Implemented parsers:

| SMBIOS type | Public structure | Description |
| :--- | :--- | :--- |
| Type 0 | `lazybiosType0_t` | BIOS information |
| Type 1 | `lazybiosType1_t` | System information |
| Type 2 | `lazybiosType2_t` | Baseboard information |
| Type 3 | `lazybiosType3_t` | System enclosure or chassis information |
| Type 4 | `lazybiosType4_t` | Processor information |

Key characteristics:

- Supports SMBIOS 2.x and SMBIOS 3.x entry point parsing.
- Uses Linux sysfs files under `/sys/firmware/dmi/tables/` on Linux.
- Uses `GetSystemFirmwareTable` on Windows.
- Provides file loaders for repeatable testing with dumped SMBIOS data.
- Stores Type 2, Type 3, and Type 4 results as arrays because those structures can appear more than once.

## Build and Installation

### Prerequisites

- CMake 3.15 or newer.
- A C compiler with C99 support.
- A supported runtime source:
  - Linux sysfs SMBIOS files.
  - Windows firmware table API.
  - Existing SMBIOS dump files.

### Build From Source

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

The CMake project builds:

- `lazybios`, a shared library.
- `lazybios_test`, a command-line test and inspection executable.

The repository also includes `setup.sh`, which wraps common local workflows:

```bash
./setup.sh build
./setup.sh run
./setup.sh source_test
./setup.sh source_test_valgrind
./setup.sh install
./setup.sh uninstall
./setup.sh clean
```

### CMake and pkg-config

Installed CMake consumers can use the exported package:

```cmake
find_package(lazybios REQUIRED)
target_link_libraries(your_target PRIVATE lazybios::lazybios)
```

The build also generates a `lazybios.pc` file for `pkg-config`:

```bash
pkg-config --cflags --libs lazybios
```

### Logging Options

Logging is controlled by CMake options that add private compile definitions to the library target.

| Option | Default | Effect |
| :--- | :--- | :--- |
| `LAZYBIOS_QUIET` | `OFF` | Disables lazybios log output. |
| `LAZYBIOS_DEBUG` | `OFF` | Enables debug logging in addition to normal log output. |
| Neither option | n/a | Enables normal lazybios log output. |

Example:

```bash
cmake .. -DLAZYBIOS_QUIET=ON
cmake .. -DLAZYBIOS_DEBUG=ON
```

## Runtime Backends and Input Modes

### System Initialization

`lazybiosInit(ctx)` selects behavior from `ctx->backend`, which is initialized by `lazybiosCTXNew()` at compile-time platform detection.

| Backend | Status | Behavior |
| :--- | :--- | :--- |
| `LAZYBIOS_BACKEND_LINUX` | Implemented via sysfs | Reads `/sys/firmware/dmi/tables/smbios_entry_point` and `/sys/firmware/dmi/tables/DMI`. If the DMI sysfs file cannot be opened, it attempts the `/dev/mem` backend, which currently returns failure. |
| `LAZYBIOS_BACKEND_WINDOWS` | Implemented | Uses `GetSystemFirmwareTable` with the `RSMB` provider and stores the returned DMI payload. Windows does not provide a raw SMBIOS entry point file or physical table address through this path. |
| `LAZYBIOS_BACKEND_MACOS` | Not implemented | Returns failure. |
| `LAZYBIOS_BACKEND_UNKNOWN` | Not implemented | Returns failure. |

### Separate Dump Files

Use `lazybiosFile(ctx, entry_path, dmi_path)` when the SMBIOS entry point and DMI table are stored in separate files.

Expected file layout:

- `entry_path`: raw SMBIOS entry point bytes.
- `dmi_path`: raw DMI structure table bytes.

This matches Linux sysfs naming:

```text
smbios_entry_point
DMI
```

### Single Merged Dump File

Use `lazybiosSingleFile(ctx, bin_path)` when one file contains the SMBIOS entry point first and the DMI table immediately after it.

The loader determines the entry point size from the leading bytes:

- SMBIOS 3.x entry point: 24 bytes.
- SMBIOS 2.x entry point: 31 bytes.

After the entry point, all remaining bytes are treated as DMI table data.

### Test Executable

The `lazybios_test` executable exercises the library and prints parsed data.

```bash
build/lazybios_test
build/lazybios_test --type 0
build/lazybios_test --sources test-dumps/1/smbios_entry_point test-dumps/1/DMI
build/lazybios_test --single-source test-dumps/1/merged.bin
build/lazybios_test --dump .
```

Options:

| Option | Description |
| :--- | :--- |
| `--help` | Prints command help. |
| `--type <number>` | Prints one implemented SMBIOS type. Valid implemented values are `0`, `1`, `2`, `3`, and `4`. |
| `--dump [dir]` | Dumps raw SMBIOS data to a directory. Linux writes `smbios_entry_point` and `DMI`; Windows writes only `DMI.bin`. |
| `--sources <entry> <dmi>` | Parses separate raw entry point and DMI table files. |
| `--single-source <binary>` | Parses one merged file containing entry point bytes followed by DMI table bytes. |

## Core Concepts

### Context Layout

All ordinary use starts with a `lazybiosCTX_t` from `lazybiosCTXNew()`.

The context stores:

- The selected backend.
- A pointer to the raw DMI container, `ctx->DMIData`.
- Parsed Type 0 and Type 1 pointers.
- Parsed Type 2, Type 3, and Type 4 arrays plus count fields.

### DMI Data Layout

`lazybiosDMI_t` owns the raw data used by all parsers.

```c
typedef struct {
    uint8_t *dmi_data;
    size_t dmi_len;
    uint8_t *entry_data;
    size_t entry_len;
    smbios_entry_info_t entry_info;
} lazybiosDMI_t;
```

`dmi_data` points at the raw DMI structure table. `entry_data` points at raw entry point bytes when they are available. On Windows, `entry_data` is currently `NULL` because the Windows API returns SMBIOS table data rather than the raw entry point file.

### Memory Ownership

`lazybiosCTXNew()` allocates both the context and `ctx->DMIData`.

Data loading functions allocate `ctx->DMIData->entry_data` and/or `ctx->DMIData->dmi_data`.

SMBIOS type getter functions allocate parsed structures:

- Type 0 and Type 1 return a single allocated structure.
- Type 2, Type 3, and Type 4 return allocated arrays and write the parsed count through the provided count pointer.

If parsed structures are assigned into the context fields, `lazybiosCleanup(ctx)` frees them.

If parsed structures are managed outside the context, free them with the matching free function:

| Type | Free function |
| :--- | :--- |
| Type 0 | `lazybiosFreeType0(Type0)` |
| Type 1 | `lazybiosFreeType1(Type1)` |
| Type 2 | `lazybiosFreeType2(Type2, type2_count)` |
| Type 3 | `lazybiosFreeType3(Type3, type3_count)` |
| Type 4 | `lazybiosFreeType4(Type4, type4_count)` |

Avoid calling a getter repeatedly and overwriting an existing pointer without freeing the previous result.

### Sentinel Values

The public header defines common sentinel values for missing or unavailable fields.

| Macro | Value | Use |
| :--- | :--- | :--- |
| `LAZYBIOS_NOT_FOUND_U8` | `0xFF` | Missing 8-bit field. |
| `LAZYBIOS_NOT_FOUND_U16` | `0xFFFF` | Missing 16-bit field. |
| `LAZYBIOS_NOT_FOUND_U32` | `0xFFFFFFFF` | Missing 32-bit field. |
| `LAZYBIOS_NOT_FOUND_U64` | `0xFFFFFFFFFFFFFFFFULL` | Missing 64-bit field. |
| `LAZYBIOS_NULL` | `NULL` | Null pointer alias. |
| `LAZYBIOS_NOT_FOUND_STR` | `"Not Present"` | Replacement string for missing SMBIOS strings. |

### Version Checks

Use `ISVERPLUS(DMIData, req_major, req_minor)` to check whether the parsed SMBIOS version is at least a required version.

```c
if (ISVERPLUS(ctx->DMIData, 3, 0)) {
    lazybiosPrintVer(ctx);
}
```

The first argument is a `lazybiosDMI_t *`, not a `lazybiosCTX_t *`.

## Quick Start

### Read From the Host System

```c
#include "lazybios.h"

int main(void) {
    lazybiosCTX_t *ctx = lazybiosCTXNew();
    if (!ctx) return 1;

    if (lazybiosInit(ctx) != 0) {
        lazybiosCleanup(ctx);
        return 1;
    }

    ctx->Type0 = lazybiosGetType0(ctx->Type0, ctx->DMIData);
    if (ctx->Type0) {
        /* Use ctx->Type0->vendor, ctx->Type0->version, etc. */
    }

    lazybiosCleanup(ctx);
    return 0;
}
```

### Read From Dump Files

```c
lazybiosCTX_t *ctx = lazybiosCTXNew();
if (!ctx) return 1;

if (lazybiosFile(ctx, "smbios_entry_point", "DMI") != 0) {
    lazybiosCleanup(ctx);
    return 1;
}

ctx->Type1 = lazybiosGetType1(ctx->Type1, ctx->DMIData);

lazybiosCleanup(ctx);
```

Merged dump:

```c
lazybiosCTX_t *ctx = lazybiosCTXNew();
if (!ctx) return 1;

if (lazybiosSingleFile(ctx, "merged.bin") != 0) {
    lazybiosCleanup(ctx);
    return 1;
}

lazybiosPrintVer(ctx);
lazybiosCleanup(ctx);
```

### Read Multiple Structures

Type 2, Type 3, and Type 4 can appear multiple times.

```c
ctx->Type4 = lazybiosGetType4(ctx->Type4, &ctx->type4_count, ctx->DMIData);

for (size_t i = 0; i < ctx->type4_count; i++) {
    lazybiosType4_t *cpu = &ctx->Type4[i];
    /* Use cpu->socket_designation, cpu->processor_version, etc. */
}
```

## Public API Reference

The public API is declared in `include/lazybios.h`.

### Context Management Functions

| Function | Description | Return |
| :--- | :--- | :--- |
| `lazybiosCTX_t* lazybiosCTXNew(void)` | Allocates a zeroed context, allocates `ctx->DMIData`, and sets `ctx->backend` from the platform. | Context pointer, or `NULL` on allocation failure. |
| `int lazybiosCleanup(lazybiosCTX_t* ctx)` | Frees parsed SMBIOS structures, raw DMI buffers, `ctx->DMIData`, and the context. | `0` on success, `-1` if `ctx` is `NULL`. |

### Data Loading Functions

| Function | Description | Return |
| :--- | :--- | :--- |
| `int lazybiosInit(lazybiosCTX_t* ctx)` | Initializes from the selected runtime backend. | `0` on success, `-1` on failure. |
| `int lazybiosFile(lazybiosCTX_t* ctx, const char* entry_path, const char* dmi_path)` | Loads a raw SMBIOS entry point file and a raw DMI table file. | `0` on success, `-1` on failure. |
| `int lazybiosSingleFile(lazybiosCTX_t* ctx, const char* bin_path)` | Loads a single file containing entry point bytes followed by DMI table bytes. | `0` on success, `-1` on failure. |

### Core Parsing Helper Functions

| Function | Description |
| :--- | :--- |
| `char* DMIString(const uint8_t *p, uint8_t length, uint8_t index, const uint8_t *end)` | Copies the string at `index` from an SMBIOS structure string area. The returned string is allocated and must be freed by the caller unless ownership is stored in a parsed structure. |
| `const uint8_t* DMINext(const uint8_t *ptr, const uint8_t *end)` | Finds the next SMBIOS structure by skipping the formatted section and string-set terminator. |
| `size_t lazybiosCountStructsByType(const lazybiosDMI_t* DMIData, uint8_t target_type)` | Counts DMI structures whose type byte matches `target_type`, stopping at Type 127 or the end of the buffer. |
| `int lazybiosParseEntry(lazybiosCTX_t* ctx, const uint8_t* buf)` | Parses SMBIOS 2.x or 3.x entry point metadata into `ctx->DMIData->entry_info`. |

### General Helper Functions

| Function | Description |
| :--- | :--- |
| `void lazybiosPrintVer(const lazybiosCTX_t* ctx)` | Prints the parsed SMBIOS version to stdout. SMBIOS 3.x output includes document revision. |

## Public Data Structures

### smbios_entry_info_t

Parsed metadata from the SMBIOS entry point.

| Field | Type | Description |
| :--- | :--- | :--- |
| `major` | `uint8_t` | SMBIOS major version. |
| `minor` | `uint8_t` | SMBIOS minor version. |
| `docrev` | `uint8_t` | SMBIOS document revision for SMBIOS 3.x. Set to `LAZYBIOS_NOT_FOUND_U8` for SMBIOS 2.x. |
| `table_length` | `uint32_t` | Length of the DMI structure table. |
| `table_address` | `uint64_t` | Physical address from the entry point when available. Windows sets this to `0`. |
| `n_structures` | `uint16_t` | SMBIOS 2.x structure count. SMBIOS 3.x sets this to `LAZYBIOS_NOT_FOUND_U16`. |
| `is_64bit` | `bool` | `true` for SMBIOS 3.x entry point data, `false` for SMBIOS 2.x entry point data. |

### lazybiosDMI_t

Raw DMI storage plus parsed entry point metadata.

| Field | Type | Description |
| :--- | :--- | :--- |
| `dmi_data` | `uint8_t *` | Raw DMI structure table bytes. |
| `dmi_len` | `size_t` | Length of `dmi_data`. |
| `entry_data` | `uint8_t *` | Raw entry point bytes when available. |
| `entry_len` | `size_t` | Length of `entry_data`. |
| `entry_info` | `smbios_entry_info_t` | Parsed entry point metadata. |

### lazybiosCTX_t

Top-level object for backend selection, raw DMI data, and parsed SMBIOS structures.

| Field | Type | Description |
| :--- | :--- | :--- |
| `backend` | `lazybiosBackend_t` | Selected backend. |
| `DMIData` | `lazybiosDMI_t *` | Raw SMBIOS/DMI data container. |
| `Type0` | `lazybiosType0_t *` | Parsed Type 0 structure, or `NULL`. |
| `Type1` | `lazybiosType1_t *` | Parsed Type 1 structure, or `NULL`. |
| `Type2` | `lazybiosType2_t *` | Parsed Type 2 array, or `NULL`. |
| `type2_count` | `size_t` | Number of parsed Type 2 entries. |
| `Type3` | `lazybiosType3_t *` | Parsed Type 3 array, or `NULL`. |
| `type3_count` | `size_t` | Number of parsed Type 3 entries. |
| `Type4` | `lazybiosType4_t *` | Parsed Type 4 array, or `NULL`. |
| `type4_count` | `size_t` | Number of parsed Type 4 entries. |

## SMBIOS Type Reference

## Type 0 BIOS Information

### Type 0 Overview

SMBIOS Type 0 describes BIOS and platform firmware information. `lazybios` parses the first Type 0 structure it finds.

Public structure: `lazybiosType0_t`

### Type 0 Fields

| Field | Type | SMBIOS version | Description |
| :--- | :--- | :--- | :--- |
| `vendor` | `char *` | 2.0+ | BIOS vendor string. |
| `version` | `char *` | 2.0+ | BIOS version string. |
| `release_date` | `char *` | 2.0+ | BIOS release date string. |
| `bios_starting_segment` | `uint16_t` | 2.0+ | BIOS starting segment. |
| `rom_size` | `uint32_t` | 2.0+ | ROM size in KB when the legacy ROM size byte is usable. |
| `characteristics` | `uint64_t` | 2.0+ | BIOS characteristics bit field. |
| `firmware_char_ext_bytes_count` | `size_t` | 2.1+ | Number of bytes stored in `firmware_char_ext_bytes`. |
| `firmware_char_ext_bytes` | `uint8_t *` | 2.1+ | Firmware characteristics extension bytes. |
| `platform_major_release` | `uint8_t` | 2.4+ | Platform firmware major release. |
| `platform_minor_release` | `uint8_t` | 2.4+ | Platform firmware minor release. |
| `ec_major_release` | `uint8_t` | 2.4+ | Embedded controller firmware major release. |
| `ec_minor_release` | `uint8_t` | 2.4+ | Embedded controller firmware minor release. |
| `extended_rom_size` | `uint16_t` | 3.1+ | Raw extended ROM size field. |
| `unit` | `char[5]` | Helper storage | Unit buffer populated by `lazybiosType0ExtendedROMSizeU16()`. |

### Type 0 Functions

| Function | Description |
| :--- | :--- |
| `lazybiosType0_t* lazybiosGetType0(lazybiosType0_t *Type0, lazybiosDMI_t *DMIData)` | Finds and allocates the first BIOS Information structure. Returns `NULL` if no Type 0 structure is found or allocation fails. |
| `void lazybiosType0CharacteristicsStr(uint64_t characteristics, char *buf, size_t buf_len)` | Decodes the BIOS characteristics bit field into a comma-separated string. |
| `void lazybiosType0CharacteristicsExtByte1Str(uint8_t char_ext_byte_1, char *buf, size_t buf_len)` | Decodes firmware characteristics extension byte 1. |
| `void lazybiosType0CharacteristicsExtByte2Str(uint8_t char_ext_byte_2, char *buf, size_t buf_len)` | Decodes firmware characteristics extension byte 2. |
| `uint16_t lazybiosType0ExtendedROMSizeU16(uint16_t raw, char unit[5])` | Extracts the extended ROM size value and writes `"MiB"`, `"GiB"`, or `"RES"` to `unit`. |
| `void lazybiosFreeType0(lazybiosType0_t* Type0)` | Frees a Type 0 structure and its allocated members. |

### Type 0 Example

```c
ctx->Type0 = lazybiosGetType0(ctx->Type0, ctx->DMIData);

if (ctx->Type0) {
    char buf[LAZYBIOS_DECODER_BUF_SIZE * 2];
    lazybiosType0CharacteristicsStr(ctx->Type0->characteristics, buf, sizeof(buf));
}
```

### Type 0 Notes

- Missing strings are replaced with `"Not Present"`.
- If the legacy ROM size field is `0xFF`, `rom_size` is set to `LAZYBIOS_NOT_FOUND_U32` and `extended_rom_size` is used when SMBIOS 3.1+ data is available.
- The extended ROM size helper returns the raw size portion. The unit is returned separately through the `unit` buffer.

## Type 1 System Information

### Type 1 Overview

SMBIOS Type 1 describes the overall system or product. `lazybios` parses the first Type 1 structure it finds.

Public structure: `lazybiosType1_t`

### Type 1 Fields

| Field | Type | SMBIOS version | Description |
| :--- | :--- | :--- | :--- |
| `manufacturer` | `char *` | 2.0+ | System manufacturer string. |
| `product_name` | `char *` | 2.0+ | Product name string. |
| `version` | `char *` | 2.0+ | System version string. |
| `serial_number` | `char *` | 2.0+ | System serial number string. |
| `uuid` | `uint8_t[16]` | 2.1+ | Raw UUID bytes. |
| `wake_up_type` | `uint8_t` | 2.1+ | Event that caused the system to wake up. |
| `sku_number` | `char *` | 2.4+ | System SKU number string. |
| `family` | `char *` | 2.4+ | System family string. |

### Type 1 Functions

| Function | Description |
| :--- | :--- |
| `lazybiosType1_t* lazybiosGetType1(lazybiosType1_t *Type1, lazybiosDMI_t *DMIData)` | Finds and allocates the first System Information structure. Returns `NULL` if no Type 1 structure is found or allocation fails. |
| `const char* lazybiosType1WakeupTypeStr(uint8_t wake_up_type)` | Decodes a wake-up type value such as `"Power Switch"` or `"AC Power Restored"`. |
| `void lazybiosFreeType1(lazybiosType1_t* Type1)` | Frees a Type 1 structure and its allocated members. |

### Type 1 Example

```c
ctx->Type1 = lazybiosGetType1(ctx->Type1, ctx->DMIData);

if (ctx->Type1 && ctx->Type1->wake_up_type != LAZYBIOS_NOT_FOUND_U8) {
    const char *wake = lazybiosType1WakeupTypeStr(ctx->Type1->wake_up_type);
}
```

### Type 1 Notes

- UUID bytes are stored in SMBIOS byte order. The test program prints them in the common UUID display order.
- `sku_number` and `family` are allocated as `"Not Present"` when SMBIOS 2.4+ data is unavailable.

## Type 2 Baseboard Information

### Type 2 Overview

SMBIOS Type 2 describes baseboards, modules, and motherboard-like components. Multiple Type 2 structures can exist, so `lazybios` returns an array.

Public structure: `lazybiosType2_t`

### Type 2 Fields

| Field | Type | SMBIOS version | Description |
| :--- | :--- | :--- | :--- |
| `manufacturer` | `char *` | 2.0+ | Baseboard manufacturer string. |
| `product` | `char *` | 2.0+ | Baseboard product string. |
| `version` | `char *` | 2.0+ | Baseboard version string. |
| `serial_number` | `char *` | 2.0+ | Baseboard serial number string. |
| `asset_tag` | `char *` | 2.0+ | Baseboard asset tag string. |
| `feature_flags` | `uint8_t` | 2.1+ | Baseboard feature flags. |
| `location_in_chassis` | `char *` | 2.1+ | Baseboard location in chassis string. |
| `chassis_handle` | `uint16_t` | 2.1+ | Handle of the associated chassis structure. |
| `board_type` | `uint8_t` | 2.1+ | Baseboard type enum. |
| `number_of_contained_object_handles` | `uint8_t` | 2.1+ | Number of entries in `contained_object_handles`. |
| `contained_object_handles` | `uint16_t *` | 2.1+ | Array of contained object handles. |

### Type 2 Functions

| Function | Description |
| :--- | :--- |
| `lazybiosType2_t* lazybiosGetType2(lazybiosType2_t *Type2, size_t *type2_count, lazybiosDMI_t* DMIData)` | Counts and parses all Type 2 structures into an allocated array. Writes the parsed count to `type2_count`. |
| `void lazybiosType2FeatureflagsStr(uint8_t feature_flags, char *buf, size_t buf_len)` | Decodes baseboard feature flags into a comma-separated string. |
| `const char* lazybiosType2BoardTypeStr(uint8_t board_type)` | Decodes the baseboard type enum. |
| `void lazybiosFreeType2(lazybiosType2_t *Type2, size_t type2_count)` | Frees a Type 2 array and its allocated members. |

### Type 2 Example

```c
ctx->Type2 = lazybiosGetType2(ctx->Type2, &ctx->type2_count, ctx->DMIData);

for (size_t i = 0; i < ctx->type2_count; i++) {
    lazybiosType2_t *board = &ctx->Type2[i];

    char flags[LAZYBIOS_DECODER_BUF_SIZE];
    lazybiosType2FeatureflagsStr(board->feature_flags, flags, sizeof(flags));
}
```

### Type 2 Notes

- `type2_count` should be checked before indexing `ctx->Type2`.
- The parser allocates `contained_object_handles` only when the field count is nonzero and the formatted structure has enough bytes.
- Free Type 2 data with the same count returned by `lazybiosGetType2()`.

## Type 3 Chassis Information

### Type 3 Overview

SMBIOS Type 3 describes system enclosure and chassis information. Multiple Type 3 structures can exist, so `lazybios` returns an array.

Public structure: `lazybiosType3_t`

### Type 3 Fields

| Field | Type | SMBIOS version | Description |
| :--- | :--- | :--- | :--- |
| `manufacturer` | `char *` | 2.0+ | Chassis manufacturer string. |
| `type` | `uint8_t` | 2.0+ | Chassis type byte. Bit 7 indicates chassis lock presence; bits 6:0 contain the chassis type. |
| `version` | `char *` | 2.0+ | Chassis version string. |
| `serial_number` | `char *` | 2.0+ | Chassis serial number string. |
| `asset_tag` | `char *` | 2.0+ | Chassis asset tag string. |
| `boot_up_state` | `uint8_t` | 2.1+ | Boot-up state enum. |
| `power_supply_state` | `uint8_t` | 2.1+ | Power supply state enum. |
| `thermal_state` | `uint8_t` | 2.1+ | Thermal state enum. |
| `security_status` | `uint8_t` | 2.1+ | Security status enum. |
| `oem_defined` | `uint32_t` | 2.3+ | OEM-defined data. |
| `height` | `uint8_t` | 2.3+ | Enclosure height in rack units, or `0` when unspecified. |
| `number_of_power_cords` | `uint8_t` | 2.3+ | Number of power cords. |
| `contained_element_count` | `uint8_t` | 2.3+ | Number of contained element records. |
| `contained_element_record_length` | `uint8_t` | 2.3+ | Size of each contained element record in bytes. |
| `contained_elements` | `uint8_t *` | 2.3+ | Raw contained element records, sized as count times record length. |
| `sku_number` | `char *` | 2.7+ | Chassis SKU number string. |
| `rack_type` | `uint8_t` | 3.9+ | Rack type byte. |
| `rack_height` | `uint8_t` | 3.9+ | Extended rack height byte. |

### Type 3 Functions

| Function | Description |
| :--- | :--- |
| `lazybiosType3_t* lazybiosGetType3(lazybiosType3_t *Type3, size_t *type3_count, lazybiosDMI_t *DMIData)` | Counts and parses Type 3 structures into an allocated array. Writes a count to `type3_count`. |
| `void lazybiosType3TypeStr(uint8_t type, char *buf, size_t buf_len)` | Decodes the chassis type byte, including the chassis lock bit. |
| `const char* lazybiosType3StateStr(uint8_t state)` | Decodes boot-up, power supply, and thermal state values. |
| `const char* lazybiosType3SecurityStatusStr(uint8_t security_status)` | Decodes chassis security status. |
| `void lazybiosType3ContainedElementTypeStr(uint8_t contained_elements, char *buf, size_t buf_len)` | Decodes a contained element type byte as either an SMBIOS structure type or a baseboard type. |
| `void lazybiosFreeType3(lazybiosType3_t* Type3, size_t type3_count)` | Frees a Type 3 array and its allocated members. |

### Type 3 Example

```c
ctx->Type3 = lazybiosGetType3(ctx->Type3, &ctx->type3_count, ctx->DMIData);

for (size_t i = 0; i < ctx->type3_count; i++) {
    lazybiosType3_t *chassis = &ctx->Type3[i];

    char type[LAZYBIOS_DECODER_BUF_SIZE];
    lazybiosType3TypeStr(chassis->type, type, sizeof(type));
}
```

### Type 3 Notes

- `contained_elements` stores raw records. The test executable interprets each record as type, minimum count, maximum count, and optional extra bytes.
- `lazybiosType3ContainedElementTypeStr()` delegates board-type decoding to `lazybiosType2BoardTypeStr()` when the contained element byte identifies a board type.
- Rack fields are parsed only for SMBIOS 3.9+ data.

## Type 4 Processor Information

### Type 4 Overview

SMBIOS Type 4 describes processor sockets and processors. Multiple Type 4 structures can exist, so `lazybios` returns an array.

Public structure: `lazybiosType4_t`

### Type 4 Fields

| Field | Type | SMBIOS version | Description |
| :--- | :--- | :--- | :--- |
| `socket_designation` | `char *` | 2.0+ | Processor socket designation string. |
| `processor_type` | `uint8_t` | 2.0+ | Processor type enum. |
| `processor_family` | `uint8_t` | 2.0+ | Processor family enum. |
| `processor_manufacturer` | `char *` | 2.0+ | Processor manufacturer string. |
| `processor_id` | `uint64_t` | 2.0+ | Raw processor ID bytes as a 64-bit value. |
| `processor_version` | `char *` | 2.0+ | Processor version string. |
| `voltage` | `uint8_t` | 2.0+ | Processor voltage byte. |
| `external_clock` | `uint16_t` | 2.0+ | External clock in MHz. |
| `max_speed` | `uint16_t` | 2.0+ | Maximum speed in MHz. |
| `current_speed` | `uint16_t` | 2.0+ | Current speed in MHz at boot. |
| `status` | `uint8_t` | 2.0+ | Processor socket population and CPU status byte. |
| `processor_upgrade` | `uint8_t` | 2.0+ | Processor upgrade/socket enum. |
| `l1_cache_handle` | `uint16_t` | 2.1+ | Handle for the L1 cache information structure. |
| `l2_cache_handle` | `uint16_t` | 2.1+ | Handle for the L2 cache information structure. |
| `l3_cache_handle` | `uint16_t` | 2.1+ | Handle for the L3 cache information structure. |
| `serial_number` | `char *` | 2.3+ | Processor serial number string. |
| `asset_tag` | `char *` | 2.3+ | Processor asset tag string. |
| `part_number` | `char *` | 2.3+ | Processor part number string. |
| `core_count` | `uint8_t` | 2.5+ | 8-bit core count. `0xFF` indicates that the extended field should be used when available. |
| `core_enabled` | `uint8_t` | 2.5+ | 8-bit enabled core count. `0xFF` indicates that the extended field should be used when available. |
| `thread_count` | `uint8_t` | 2.5+ | 8-bit thread count. `0xFF` indicates that the extended field should be used when available. |
| `processor_characteristics` | `uint16_t` | 2.5+ | Processor characteristics bit field. |
| `processor_family_2` | `uint16_t` | 2.5+ | Extended processor family value, used when `processor_family` is `0xFE`. |
| `core_count_2` | `uint16_t` | 3.0+ | Extended core count. |
| `core_enabled_2` | `uint16_t` | 3.0+ | Extended enabled core count. |
| `thread_count_2` | `uint16_t` | 3.0+ | Extended thread count. |
| `thread_enabled` | `uint16_t` | 3.6+ | Enabled thread count. |
| `socket_type` | `char *` | 3.8+ | Socket type string. |

### Type 4 Functions

| Function | Description |
| :--- | :--- |
| `lazybiosType4_t* lazybiosGetType4(lazybiosType4_t *Type4, size_t *type4_count, lazybiosDMI_t *DMIData)` | Counts and parses Type 4 structures into an allocated array. Writes a count to `type4_count`. |
| `const char* lazybiosType4ProcessorFamilyStr(uint16_t family)` | Decodes 8-bit or 16-bit processor family values. |
| `const char* lazybiosType4SocketTypeStr(uint8_t type)` | Decodes processor upgrade/socket type enum values. |
| `void lazybiosType4CharacteristicsStr(uint16_t characteristics, char *buf, size_t buf_len)` | Decodes processor characteristics into a string. |
| `const char* lazybiosType4TypeStr(uint8_t type)` | Decodes processor type values such as `"Central Processor"`. |
| `void lazybiosType4StatusStr(uint8_t status, char *buf, size_t buf_len)` | Decodes socket population and CPU status. |
| `void lazybiosType4VoltageStr(uint8_t voltage, char *buf, size_t buf_len)` | Decodes voltage as either a concrete voltage or supported voltage capabilities. |
| `void lazybiosFreeType4(lazybiosType4_t* Type4, size_t type4_count)` | Frees a Type 4 array and its allocated members. |

### Type 4 Example

```c
ctx->Type4 = lazybiosGetType4(ctx->Type4, &ctx->type4_count, ctx->DMIData);

for (size_t i = 0; i < ctx->type4_count; i++) {
    lazybiosType4_t *cpu = &ctx->Type4[i];

    char status[LAZYBIOS_DECODER_BUF_SIZE];
    lazybiosType4StatusStr(cpu->status, status, sizeof(status));
}
```

### Type 4 Notes

- For processor family, use `processor_family_2` when `processor_family` is `0xFE` and the extended value is present.
- For core and thread counts, when the 8-bit count is `0xFF`, check the matching 16-bit field:
  - `core_count_2`
  - `core_enabled_2`
  - `thread_count_2`
- `lazybiosType4SocketTypeStr()` decodes the `processor_upgrade` enum. The separate `socket_type` field is parsed as a string for SMBIOS 3.8+ data.

## Supported SMBIOS Type Constants

The public header defines constants for several SMBIOS structure IDs:

| Constant | Value | Parser implemented |
| :--- | :--- | :--- |
| `SMBIOS_TYPE_BIOS` | `0` | Yes |
| `SMBIOS_TYPE_SYSTEM` | `1` | Yes |
| `SMBIOS_TYPE_BASEBOARD` | `2` | Yes |
| `SMBIOS_TYPE_CHASSIS` | `3` | Yes |
| `SMBIOS_TYPE_PROCESSOR` | `4` | Yes |
| `SMBIOS_TYPE_CACHES` | `7` | No |
| `SMBIOS_TYPE_PORT_CONNECTOR` | `8` | No |
| `SMBIOS_TYPE_ONBOARD_DEVICES` | `10` | No |
| `SMBIOS_TYPE_OEM_STRINGS` | `11` | No |
| `SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY` | `16` | No |
| `SMBIOS_TYPE_MEMORY_DEVICE` | `17` | No |
| `SMBIOS_TYPE_END` | `127` | Used as table terminator |

## Limitations and Platform Notes

- Linux support currently depends on sysfs SMBIOS files. The `/dev/mem` fallback exists internally but is not implemented.
- Windows support reads DMI table data through `GetSystemFirmwareTable`; it does not preserve a raw SMBIOS entry point buffer.
- macOS is represented in the backend enum but is not implemented.
- Type 0 and Type 1 getters return only the first matching structure.
- Type 2, Type 3, and Type 4 getters allocate arrays and require count-aware cleanup.
- Fields guarded by SMBIOS version checks are filled with sentinel values or `"Not Present"` when unavailable.

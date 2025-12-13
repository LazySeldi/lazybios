# lazybios: Lightweight SMBIOS/DMI Parsing Library

## Project Overview

The lazybios library is a lightweight, cross-platform C library designed for parsing System Management BIOS (SMBIOS) and Desktop Management Interface (DMI) information from a host system. It provides a structured, easy-to-use API to extract hardware and firmware details, directly from the system's DMI tables if provided.

The library is written in C and is built using the CMake build system, ensuring portability and ease of integration into other projects.

## Key Features

The primary features of the `lazybios` library include:

*   SMBIOS Support: Comprehensive support for both SMBIOS 2.x and the newer 3.x entry points and structure formats.
*   Multi-OS Backends: Currently supports data retrieval on Linux (via `/sys/firmware/dmi/tables/` sysfs files) and Windows (via the `GetSystemFirmwareTable` API).
*   File-Based Testing: Includes a dedicated function, `lazybiosFile`, for parsing DMI table dumps from specified files, which is ideal for testing and analysis on non-native environments.
*   Structured Data: Parses raw DMI structures into well-defined C structures, starting with the Type 0 (BIOS Information) structure.
*   Extensible Design: The core parsing logic is modular, allowing for easy addition of new SMBIOS structure types (e.g., Type , Type 2, etc.).

## Build and Installation

The project uses CMake for its build system. The following steps outline how to build and install the library.

### Prerequisites

*   CMake (version 15 or higher)
*   A C compiler (e.g., GCC or Clang)

### CMake Integration

The library generates a CMake package configuration file, allowing other CMake projects to easily find and link against `lazybios`.

```cmake
find_package(lazybios REQUIRED)
target_link_libraries(your_target PRIVATE lazybios::lazybios)
```

Alternatively, it can be used via `pkg-config`:

```bash
pkg-config --cflags --libs lazybios
```

## API Reference

The public API is defined in `lazybios.h`. All functions operate on a central context object, `lazybiosCTX_t`.

### 1. Context Management

| Function                                  | Description                                                                                                                                                                                                                      |
|:------------------------------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `lazybiosCTX_t* lazybiosCTXNew(void)`     | Allocates and initializes a new `lazybiosCTX_t` context structure.                                                                                                                                                               |
| `int lazybiosCleanup(lazybiosCTX_t* ctx)` | Frees all dynamically allocated memory within the context, including DMI data and parsed structures, and then frees the context itself. Returns - on failure, and 0 on success. This must be called when done with the context. |

### 2. Initialization and Data Loading

The library must be initialized to load the DMI table data into the context.

| Function | Description                                                                                                                                                                                                                                                 |
| :--- |:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `int lazybiosInit(lazybiosCTX_t* ctx)` | Primary initialization function. Attempts to detect the operating system and use the appropriate backend (e.g., `lazybiosSysfs` `lazybiosDevMem(Not implemented yet)` on Linux, `lazybiosWindows` on Windows). Returns `0` on success, `-` on failure. |
| `int lazybiosSysfs(lazybiosCTX_t* ctx)` | Linux-specific backend. Loads SMBIOS entry point and DMI table data from the `/sys/firmware/dmi/tables/` files.                                                                                                                                         |
| `int lazybiosFile(lazybiosCTX_t* ctx, const char* entry_path, const char* dmi_path)` | File-based backend. Loads SMBIOS data from specified file paths, its very useful for testing with DMI dumps.                                                                                                                                            |

### 3. Core Parsing Helpers

These functions handle the low-level details of navigating and extracting data from the raw DMI table.

| Function | Description |
| :--- | :--- |
| `int lazybiosParseEntry(lazybiosCTX_t* ctx, const uint8_t* buf)` | Parses the raw SMBIOS Entry Point buffer (`_SM_` or `_SM3_`) and populates `ctx->entry_info`. |
| `size_t lazybiosCountStructsByType(const lazybiosCTX_t* ctx, uint8_t target_type, size_t min_length)` | Counts the number of structures of a specific type in the DMI table. |
| `char* DMIString(const uint8_t *p, uint8_t length, uint8_t index, const uint8_t *end)` | Extracts and copies a string from the string section of an SMBIOS structure based on its index. The returned string is dynamically allocated and must be freed by the caller. |
| `const uint8_t* DMINext(const uint8_t *ptr, const uint8_t *end)` | Finds the start of the next SMBIOS structure in the DMI table, correctly skipping the formatted section and the double-null terminated string section. |

### 4. Data Structures

### `lazybiosCTX_t`

The main context structure holding all state and data.

```c
typedef struct lazybios_ctx {
    lazybiosBackend_t backend;      // OS backend used for data retrieval
    uint8_t *dmi_data;              // Pointer to the raw DMI table data (must be freed by cleanup)
    size_t dmi_len;                 // Length of the raw DMI table
    smbios_entry_info_t entry_info; // Information from the SMBIOS Entry Point
    lazybiosType0_t *Type0;         // Type 0 (BIOS Information) structure
} lazybiosCTX_t;
```

### `smbios_entry_info_t`

Details extracted from the SMBIOS Entry Point structure, which determines the SMBIOS version and the location of the DMI table.

| Field | Type | Description | SMBIOS Version |
| :--- | :--- | :--- | :--- |
| `major`, `minor` | `uint8_t` | Major and Minor version of SMBIOS. | 2.0+ |
| `docrev` | `uint8_t` | Document Revision. | 3.0+ |
| `table_length` | `uint32_t` | Total length of the DMI structure table. | 2.0+ |
| `table_address` | `uint64_t` | Physical address of the DMI table. | 2.0+ |
| `n_structures` | `uint16_t` | Number of structures. | 2.x only |
| `is_64bit` | `bool` | True if the SMBIOS 3.x entry point was used. | 3.0+ |

---
### `lazybiosType0_t` (BIOS Information)

This structure holds the parsed data for SMBIOS Type 0. All string fields are dynamically allocated and must be freed via `lazybiosCleanup`.

| Field | Type        | Description                               | SMBIOS Version |
| :--- |:------------|:------------------------------------------|:---------------|
| `vendor` | `char *`    | BIOS Vendor Name.                         | 2.0+           |
| `version` | `char *`    | BIOS Version.                             | 2.0+           |
| `release_date` | `char *`    | BIOS Release Date.                        | 2.0+           |
| `bios_starting_segment` | `uint16_t`  | Starting segment of the BIOS.             | 2.0+           |
| `rom_size` | `uint8_t`   | Size of the BIOS ROM, in 64KB blocks.     | 2.0+           |
| `characteristics` | `uint64_t`  | Bit field detailing BIOS characteristics. | 2.0+           |
| `firmware_char_ext_bytes` | `uint8_t *` | Extended characteristics bytes (2.1+).    | 2.1+           |
| `platform_major_release` | `uint8_t`   | Platform Firmware Major Release.          | 2.4+           |
| `extended_rom_size` | `uint16_t`  | Extended ROM Size field.                  | 3.1+           |

### `lazybiosType0_t` Functions

| Function                                                                           | Description                                                                                                          |
|:-----------------------------------------------------------------------------------|:---------------------------------------------------------------------------------------------------------------------|
| `lazybiosType0_t* lazybiosGetType0(lazybiosCTX_t* ctx)`                            | Locates and parses the Type 0 structure. The result is stored in `ctx->bios_info`.                                   |
| `const char* lazybiosFirmwareCharacteristicsStr(uint64_t characteristics)`         | Decodes the 64-bit `characteristics` field into a human-readable, comma-separated string list of supported features. |
| `const char* lazybiosFirmwareCharacteristicsExtByteStr(uint8_t char_ext_byte_1)` | Decodes the first byte of the extended characteristics into a string list.                                           |
| `const char* lazybiosFirmwareCharacteristicsExtByte2Str(uint8_t char_ext_byte_2)`  | Decodes the second byte of the extended characteristics into a string list.                                          |
| `uint16_t lazybiosFirmwareExtendedROMSizeU16(uint16_t raw, char unit[5])`         | Decodes the extended ROM size field (SMBIOS 3.1+), returning the size and setting the unit string (`MiB`/`GiB`).     |
| `void lazybiosFreeType0(lazybiosType0_t* Type0)`                                   | Frees the only the Type 0 Structure.                                                                                 |
---

## Logging and Debugging

The library includes a simple, compile-time configurable logging system.

| Macro/Option | Description |
| :--- | :--- |
| `LAZYBIOS_QUIET` | If defined during compilation, all logging is disabled. This is the quietest mode. |
| `LAZYBIOS_DEBUG` | If defined, enables verbose debug logging (`lb_dbg`) in addition to standard error logging (`lb_log`). |
| (Neither defined) | Normal Mode. Only standard error logging (`lb_log`) is enabled, which reports critical status messages and errors. |

These options can be controlled via CMake options:

```bash
# Disable all logging
cmake .. -DLAZYBIOS_QUIET=ON

# Enable verbose debug logging
cmake .. -DLAZYBIOS_DEBUG=ON
```

## SMBIOS Type Constants

The following constants are defined in `lazybios.h` for identifying common SMBIOS structures(For now only SMBIOS_TYPE_BIOS is implemented in the rework, stay tuned for the rest of them):

| Constant | Type ID | Description |
| :--- |:--------| :--- |
| `SMBIOS_TYPE_BIOS` | 0       | BIOS Information |
| `SMBIOS_TYPE_SYSTEM` | 1       | System Information |
| `SMBIOS_TYPE_BASEBOARD` | 2       | Baseboard Information |
| `SMBIOS_TYPE_CHASSIS` | 3       | System Enclosure |
| `SMBIOS_TYPE_PROCESSOR` | 4       | Processor Information |
| `SMBIOS_TYPE_MEMORY_DEVICE` | 17      | Memory Device |
| `SMBIOS_TYPE_END` | 127     | End-of-Table Marker |

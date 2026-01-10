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
*   Extensible Design: The core parsing logic is modular, allowing for easy addition of new SMBIOS structure types (e.g., Type 1, Type 2, etc.).

## Build and Installation

The project uses CMake for its build system. The following steps outline how to build and install the library.

### Prerequisites

*   CMake (version 3.15 or higher)
*   A C compiler (e.g., GCC or Clang)
*   A Supported System: Windows through WindowsAPI , Linux through sysfs only( /dev/mem will be implemented in the future ).

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
    uint8_t *entry_data;            // Pointer to the raw entry data (must be freed by cleanup)
    size_t entry_len;               // Length of the raw entry data
    smbios_entry_info_t entry_info; // Information from the SMBIOS Entry Point
    
    lazybiosType0_t *Type0;         // Type 0 (BIOS Information) structure
    lazybiosType1_t *Type1;         // Type 1 (System Informaton) structure
    lazybiosType2_t *Type2;         // Type 2 ()
    lazybiosType3_t *Type3;
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

This structure holds the parsed data for SMBIOS Type 0.

| Field | Type        | Description                               | SMBIOS Version |
| :--- |:------------|:------------------------------------------|:---------------|
| `vendor` | `char *`    | BIOS Vendor Name.                         | 2.0+           |
| `version` | `char *`    | BIOS Version.                             | 2.0+           |
| `release_date` | `char *`    | BIOS Release Date.                        | 2.0+           |
| `bios_starting_segment` | `uint16_t`  | Starting segment of the BIOS.             | 2.0+           |
| `rom_size` | `uint8_t`   | Size of the BIOS ROM, in 64KB blocks.     | 2.0+           |
| `characteristics` | `uint64_t`  | Bit field detailing BIOS characteristics. | 2.0+           |
| `firmware_char_ext_bytes` | `uint8_t *` | Extended characteristics byte.            | 2.1+           |
| `platform_major_release` | `uint8_t`   | Platform Firmware Major Release.          | 2.4+           |
| `extended_rom_size` | `uint16_t`  | Extended ROM Size field.                  | 3.1+           |

### `lazybiosType0_t` Functions

| Function                                                                       | Description                                                                                                              |
|:-------------------------------------------------------------------------------|:-------------------------------------------------------------------------------------------------------------------------|
| `lazybiosType0_t* lazybiosGetType0(lazybiosCTX_t* ctx)`                        | Locates and parses the Type 0 structure. The result is stored in `ctx->Type0`.                                           |
| `const char* lazybiosType0CharacteristicsStr(uint64_t characteristics)`        | Decodes the 64-bit `characteristics` field into a human-readable, comma-separated string list of supported features.     |
| `const char* lazybiosType0CharacteristicsExtByteStr(uint8_t char_ext_byte_1)`  | Decodes the first byte of the extended characteristics into a string list.                                               | 
| `const char* lazybiosType0CharacteristicsExtByte2Str(uint8_t char_ext_byte_2)` | Decodes the second byte of the extended characteristics into a string list.                                              |
| `uint16_t lazybiosType0ExtendedROMSizeU16(uint16_t raw, char unit[5])`         | Decodes the extended ROM size field (SMBIOS 3.1+), returning the size and setting the unit string (`MiB`/`GiB`).         |
| `void lazybiosFreeType0(lazybiosType0_t* Type0)`                               | Frees the only the Type 0 Structure.                                                                                     |
---
### `lazybiosType1_t` (System Information)

This structure holds the parsed data for SMBIOS Type 1.

| Field           | Type          | Description                              | SMBIOS Version |
|:----------------|:--------------|:-----------------------------------------|:---------------|
| `manufacturer`  | `char *`      | System Manufacturer Name.                | 2.0+           |
| `version`       | `char *`      | System Version.                          | 2.0+           |
| `product_name`  | `char *`      | Product Name.                            | 2.0+           |
| `serial_number` | `char *`      | System Serial Number.                    | 2.0+           |
| `uuid`          | `uint8_t[16]` | System UUID array of 16.                 | 2.1+           |
| `wake_up_type`  | `uint8_t`     | Bit field detailing system wake up type. | 2.1+           |
| `sku_number`    | `char *`      | System SKU Number.                       | 2.4+           |
| `family`        | `char *`      | System Family.                           | 2.4+           |

### `lazybiosType1_t` Functions

| Function                                                 | Description                                                                                                         |
|:---------------------------------------------------------|:--------------------------------------------------------------------------------------------------------------------|
| `lazybiosType1_t* lazybiosGetType1(lazybiosCTX_t* ctx)`  | Locates and parses the Type 1 structure. The result is stored in `ctx->Type1`.                                      |
| `const char* lazybiosType1TypeStr(uint8_t wake_up_type)` | Decodes the 8-bit `wake_up_type` field into a human-readable, string of system wake up type (e.g. "Power Switch").  |
| `void lazybiosFreeType1(lazybiosType1_t* Type1)`         | Frees the only the Type 1 Structure.                                                                                |

---
### `lazybiosType2_t` (System Information)

This structure holds the parsed data for SMBIOS Type 2. The SMBIOS version requirements for this field are all 2.0+ I think, I'm not use but I will keep on looking.

| Field                                | Type         | Description                                          | SMBIOS Version |
|:-------------------------------------|:-------------|:-----------------------------------------------------|:---------------|
| `manufacturer`                       | `char *`     | Baseboard Manufacturer Name.                         | 2.0+           |
| `version`                            | `char *`     | Baseboard Version.                                   | 2.0+           |
| `product`                            | `char *`     | Product Name.                                        | 2.0+           |
| `serial_number`                      | `char *`     | Baseboard Serial Number.                             | 2.0+           |
| `asset_tag`                          | `char *`     | Baseboard Asset Tag.                                 | 2.0+           |
| `feature_flags`                      | `uint8_t`    | Bit field detailing baseboard feature flags.         | 2.0+           |
| `location_in_chassis`                | `char *`     | Location in chassis.                                 | 2.0+           |
| `chassis_handle`                     | `uint16_t`   | Chassis handle.                                      | 2.0+           |
| `board_type`                         | `uint8_t`    | Enum field detailing baseboard type.                 | 2.0+           |
| `number_of_contained_object_handles` | `uint8_t`    | Number of contained object handles.                  | 2.0+           |
| `contained_object_handles`           | `uint16_t *` | contained object handles.                            | 2.0+           |

### `lazybiosType2_t` Functions

| Function                                                          | Description                                                                                                          |
|:------------------------------------------------------------------|:---------------------------------------------------------------------------------------------------------------------|
| `lazybiosType2_t* lazybiosGetType2(lazybiosCTX_t* ctx)`           | Locates and parses the Type 2 structure. The result is stored in `ctx->Type2`.                                       |
| `const char* lazybiosType2FeatureflagsStr(uint8_t feature_flags)` | Decodes the 8-bit `feature_flags` field into a human-readable, string of baseboard feature flags (e.g. "Removable"). |
| `const char* lazybiosType2BoardTypeStr(uint8_t board_type)`       | Decodes the 8-bit `board_type` field into a human-readable, string of baseboard types (e.g. "Server Blade").         |
| `void lazybiosFreeType2(lazybiosType2_t* Type2)`                  | Frees the only the Type 2 Structure.                                                                                 |

---

### `lazybiosType3_t` (System Information)

This structure holds the parsed data for SMBIOS Type 3.

| Field                             | Type        | Description                                                                                                       | SMBIOS Version |
|:----------------------------------|:------------|:------------------------------------------------------------------------------------------------------------------|:---------------|
| `manufacturer`                    | `char *`    | Chassis Manufacturer Name.                                                                                        | 2.0+           |
| `type`                            | `uint8_t`   | Chassis Type.                                                                                                     | 2.0+           |
| `version`                         | `char *`    | Chassis Version.                                                                                                  | 2.0+           |
| `serial_number`                   | `char *`    | Chassis Serial Number.                                                                                            | 2.0+           |
| `asset_tag`                       | `char *`    | Chassis Asset Tag.                                                                                                | 2.0+           |
| `boot_up_state`                   | `uint8_t`   | Enum field detailing chassis boot up state.                                                                       | 2.1+           |
| `power_supply_state`              | `uint8_t`   | Enum field detailing chassis power supply state.                                                                  | 2.1+           |
| `thermal_state`                   | `uint8_t`   | Enum field detailing chassis thermal state.                                                                       | 2.1+           |
| `security_status`                 | `uint8_t`   | Enum field detailing chassis security status.                                                                     | 2.1+           |
| `oem_defined`                     | `uint32_t`  | Chassis OEM defined data.                                                                                         | 2.3+           |
| `height`                          | `uint8_t`   | Chassis, height of the enclosure.                                                                                 | 2.3+           |
| `number_of_power_cords`           | `uint8_t`   | Number of power cords.                                                                                            | 2.3+           |
| `contained_element_count`         | `uint8_t`   | Number of the contained element's count(n).                                                                       | 2.3+           |
| `contained_element_record_length` | `uint8_t`   | Number of the contained element's record length(m).                                                               | 2.3+           |
| `contained_elements`              | `uint8_t *` | Pointer of contained elements(size = n * m BYTEs).                                                                | 2.3+           |
| `sku_number`                      | `char *`    | Chassis SKU number.                                                                                               | 2.7+           |
| `rack_type`                       | `uint8_t`   | The type of the rack.                                                                                             | 3.9+           |
| `rack_height`                     | `uint8_t`   | The height of the rack(see test.c implementation for the decoder(a better decoder is coming next version(3.3.0)). | 3.9+           |

### `lazybiosType3_t` Functions

| Function                                                                                | Description                                                                                                                                                                           |
|:----------------------------------------------------------------------------------------|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `lazybiosType3_t* lazybiosGetType3(lazybiosCTX_t* ctx)`                                 | Locates and parses the Type 3 structure. The result is stored in `ctx->Type3`.                                                                                                        |
| `const char* lazybiosType3TypeStr(uint8_t type)`                                        | Decodes the 8-bit `type` field into a human-readable, string of chassis types (e.g. "Desktop").                                                                                       |
| `const char* lazybiosType3StateStr(uint8_t state)`                                      | Decodes the 8-bit `thermal_state, power_supply_state, boot_up_state` fields into a human-readable, string of baseboard types (e.g. "Warning").                                        |
| `const char* lazybiosType3SecurityStatusStr(uint8_t security_status)`                   | Decodes the 8-bit `security_status` fields into a human-readable, string of baseboard types (e.g. "External interface locked out").                                                   |
| `const char* lazybiosType3ContainedElementTypeStr(uint8_t contained_elements)`          | Decodes the 8-bit `contained_elements` fields into a human-readable, string of baseboard types (e.g. "None") Currently its very bad, next version(3.3.0) will implement it correctly. |
| `void lazybiosFreeType3(lazybiosType3_t* Type3)`                                        | Frees the only the Type 3 Structure.                                                                                                                                                  |

---

### `lazybiosType4_t` (Processor Information)

This structure holds the parsed data for SMBIOS Type 4.

| Field                         | Type        | Description                                                                                                    | SMBIOS Version |
|:------------------------------|:------------|:---------------------------------------------------------------------------------------------------------------|:---------------|
| `socket_designation`          | `char *`    | Processor socket designation.                                                                                  | 2.0+           |
| `processor_type`              | `uint8_t`   | Processor type (Central Processor, Math Processor, etc.).                                                      | 2.0+           |
| `processor_family`            | `uint8_t`   | Processor family (Intel 486, Pentium, Xeon, etc.).                                                             | 2.0+           |
| `processor_manufacturer`      | `char *`    | Processor manufacturer name.                                                                                   | 2.0+           |
| `processor_id`                | `uint64_t`  | Processor ID (CPU signature) as 64-bit value.                                                                  | 2.0+           |
| `processor_version`           | `char *`    | Processor version string.                                                                                      | 2.0+           |
| `voltage`                     | `uint8_t`   | Processor voltage configuration.                                                                               | 2.0+           |
| `external_clock`              | `uint16_t`  | External clock frequency in MHz.                                                                               | 2.0+           |
| `max_speed`                   | `uint16_t`  | Maximum processor speed in MHz.                                                                                | 2.0+           |
| `current_speed`               | `uint16_t`  | Current processor speed in MHz (speed at boot).                                                                | 2.0+           |
| `status`                      | `uint8_t`   | Processor status (Populated, Enabled, etc.).                                                                   | 2.0+           |
| `processor_upgrade`           | `uint8_t`   | Processor upgrade type (Socket type or upgrade capability).                                                    | 2.0+           |
| `l1_cache_handle`             | `uint16_t`  | Handle to L1 cache information structure.                                                                      | 2.1+           |
| `l2_cache_handle`             | `uint16_t`  | Handle to L2 cache information structure.                                                                      | 2.1+           |
| `l3_cache_handle`             | `uint16_t`  | Handle to L3 cache information structure.                                                                      | 2.1+           |
| `serial_number`               | `char *`    | Processor serial number.                                                                                       | 2.3+           |
| `asset_tag`                   | `char *`    | Processor asset tag.                                                                                           | 2.3+           |
| `part_number`                 | `char *`    | Processor part number.                                                                                         | 2.3+           |
| `core_count`                  | `uint8_t`   | Number of cores per socket (8-bit). 0xFF indicates core count ≥256 (use `core_count_2`).                       | 2.5+           |
| `core_enabled`                | `uint8_t`   | Number of enabled cores per socket (8-bit). 0xFF indicates enabled cores ≥256 (use `core_enabled_2`).          | 2.5+           |
| `thread_count`                | `uint8_t`   | Number of threads per socket (8-bit). 0xFF indicates thread count ≥256 (use `thread_count_2`).                 | 2.5+           |
| `processor_characteristics`   | `uint16_t`  | Processor characteristics (64-bit capable, etc.).                                                              | 2.5+           |
| `processor_family_2`          | `uint16_t`  | Extended processor family (used when `processor_family` = 0xFE).                                               | 2.6+           |
| `core_count_2`                | `uint16_t`  | Extended core count (16-bit, supports >255 cores). Use when `core_count` = 0xFF.                               | 3.0+           |
| `core_enabled_2`              | `uint16_t`  | Extended enabled core count (16-bit, supports >255 cores). Use when `core_enabled` = 0xFF.                     | 3.0+           |
| `thread_count_2`              | `uint16_t`  | Extended thread count (16-bit, supports >255 threads). Use when `thread_count` = 0xFF.                         | 3.0+           |
| `thread_enabled`              | `uint16_t`  | Number of enabled threads per socket (16-bit).                                                                 | 3.6+           |
| `socket_type`                 | `char *`    | Socket type string (e.g., "Socket LGA771").                                                                    | 3.8+           |

**Note on count fields:** When 8-bit count fields (`core_count`, `core_enabled`, `thread_count`) contain 0xFF, check the corresponding 16-bit fields (`_2`) for actual count if ≥256.

### `lazybiosType4_t` Functions

| Function                                                                        | Description                                                                                                                              |
|:--------------------------------------------------------------------------------|:-----------------------------------------------------------------------------------------------------------------------------------------|
| `lazybiosType4_t* lazybiosGetType4(lazybiosCTX_t* ctx)`                         | Locates and parses the Type 4 structure. The result is stored in `ctx->Type4`.                                                           |
| `const char* lazybiosType4ProcessorFamilyStr(uint16_t family)`                  | Decodes the processor family field (8-bit or 16-bit) into human-readable string (e.g., "Intel Xeon").                                    |
| `const char* lazybiosType4TypeStr(uint8_t type)`                                | Decodes the processor type field into human-readable string (e.g., "Central Processor").                                                 |
| `const char* lazybiosType4StatusStr(uint8_t status)`                            | Decodes the processor status field into human-readable string (e.g., "Socket Populated, CPU Enabled").                                   |
| `const char* lazybiosType4UpgradeStr(uint8_t upgrade)`                          | Decodes the processor upgrade field into human-readable string (e.g., "Socket LGA771").                                                  |
| `const char* lazybiosType4CharacteristicsStr(uint16_t characteristics)`         | Decodes the processor characteristics field into human-readable string (e.g., "64-bit Capable").                                         |
| `void lazybiosFreeType4(lazybiosType4_t* Type4)`                                | Frees the Type 4 structure and all allocated strings.                                                                                    |

---
## Logging and Debugging

The library includes a simple, compile-time configurable logging system.

| Macro/Option | Description |
| :--- | :--- |
| `LAZYBIOS_QUIET` | If defined during compilation, all logging is disabled. This is the quietest mode. |
| `LAZYBIOS_DEBUG` | If defined, enables verbose debug logging (`dbg`) in addition to standard error logging (`log`). |
| (Neither defined) | Normal Mode. Only standard error logging (`log`) is enabled, which reports critical status messages and errors. |

These options can be controlled via CMake options:

```bash
# Disable all logging
cmake .. -DLAZYBIOS_QUIET=ON

# Enable verbose debug logging
cmake .. -DLAZYBIOS_DEBUG=ON
```

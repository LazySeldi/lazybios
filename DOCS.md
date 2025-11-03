# lazybios: Your Easy-Going SMBIOS/DMI Parser

Hey there! Welcome to the documentation for **lazybios**, a simple and straightforward C library designed to read and parse your system's **SMBIOS (System Management BIOS)** or **DMI (Desktop Management Interface)** tables.

If you've ever needed to grab hardware info like your BIOS version, motherboard model, CPU details, or RAM specs directly from the system firmware tables on Linux, this library is your new best friend. It handles all the low-level parsing so you can focus on what you're building.

## ⚠️ Important Heads-Up!

Before you dive in, here are a few quick notes:

*   **Root Access is Needed:** To read the SMBIOS tables from the `/sys/firmware/dmi/tables/` files on Linux, you'll need **root privileges** (or at least read access to those files).
*   **Linux Only:** This library is built specifically for Linux systems, as it relies on the DMI table files exposed in the `/sys` filesystem.
*   **Memory Management:** We use `malloc` internally to allocate strings and data structures. **Always** remember to call `lazybios_ctx_free()` when you're done to prevent memory leaks.

## Getting Started

### 1. Context is Everything

The entire library revolves around the `lazybios_ctx_t` structure, which holds all the parsed data and internal state.

| Function | Description |
| :--- | :--- |
| `lazybios_ctx_t* lazybios_ctx_new(void)` | Creates and allocates a new `lazybios_ctx_t` structure. This is your starting point. |
| `void lazybios_ctx_free(lazybios_ctx_t* ctx)` | Frees all memory associated with the context, including all the strings and data structures populated by the library. **Use this!** |
| `int lazybios_init(lazybios_ctx_t* ctx)` | Initializes the library. It reads the SMBIOS entry point and the main DMI table, and populates the context with the raw data. Returns `0` on success, `-1` on failure. |
| `void lazybios_cleanup(lazybios_ctx_t* ctx)` | Frees internal memory used for raw data. This is called automatically by `lazybios_ctx_free()`, so you usually don't need to call it directly. |

### 2. Example Usage Flow

Here's a quick look at how you'd typically use the library( For the full library usage obviously go see test.c ):

```c
#include <stdio.h>
#include "lazybios.h"

int main() {
    // 1. Create the context
    lazybios_ctx_t *ctx = lazybios_ctx_new();
    if (!ctx) {
        fprintf(stderr, "Failed to create lazybios context.\n");
        return 1;
    }

    // 2. Initialize the library (reads the DMI tables)
    if (lazybios_init(ctx) != 0) {
        fprintf(stderr, "Failed to initialize lazybios. Are you running as root?\n");
        lazybios_ctx_free(ctx);
        return 1;
    }

    // 3. Get the data you need
    system_info_t *sys_info = lazybios_get_system_info(ctx);
    if (sys_info) {
        printf("System Manufacturer: %s\n", sys_info->manufacturer);
        printf("Product Name: %s\n", sys_info->product_name);
        printf("Serial Number: %s\n", sys_info->serial_number);
    }

    // 4. Clean up everything when you're done
    lazybios_ctx_free(ctx);
    return 0;
}
```

## Data Retrieval Functions

These functions are how you get the actual hardware information. They all take the `lazybios_ctx_t*` as an argument and return a pointer to a specific data structure, or `NULL` if the information couldn't be found (e.g., the SMBIOS structure type wasn't present).

### BIOS Information (Type 0)

| Function | Description |
| :--- | :--- |
| `bios_info_t* lazybios_get_bios_info(lazybios_ctx_t* ctx)` | Gets the BIOS vendor, version, release date, and ROM size. |

**`bios_info_t` Structure:**
```c
typedef struct {
    char *vendor;         // e.g., "American Megatrends International, LLC."
    char *version;        // e.g., "F.60"
    char *release_date;   // e.g., "05/15/2023"
    uint16_t rom_size_kb; // Size of the BIOS ROM in KB
} bios_info_t;
```

### System Information (Type 1)

| Function | Description |
| :--- | :--- |
| `system_info_t* lazybios_get_system_info(lazybios_ctx_t* ctx)` | Gets the overall system details. |

**`system_info_t` Structure:**
```c
typedef struct {
    char *manufacturer;  // e.g., "HP"
    char *product_name;  // e.g., "HP ProDesk 400 G9 SFF"
    char *version;       // e.g., "V1.0"
    char *serial_number; // The system's serial number
    char *uuid;          // The system's unique UUID
} system_info_t;
```

### Baseboard (Motherboard) Information (Type 2)

| Function | Description |
| :--- | :--- |
| `baseboard_info_t* lazybios_get_baseboard_info(lazybios_ctx_t* ctx)` | Gets the motherboard's details. |

**`baseboard_info_t` Structure:**
```c
typedef struct {
    char *manufacturer;
    char *product;       // Motherboard model
    char *version;
    char *serial_number;
    char *asset_tag;
} baseboard_info_t;
```

### Chassis (Enclosure) Information (Type 3)

| Function | Description |
| :--- | :--- |
| `chassis_info_t* lazybios_get_chassis_info(lazybios_ctx_t* ctx)` | Gets the physical enclosure details. |

**`chassis_info_t` Structure:**
```c
typedef struct {
    // These have a high chance of not being specified in some systems.
    char *asset_tag;
    char *sku;
    uint8_t type;        // Chassis type code (e.g., 7 is Tower)
    uint8_t state;       // Chassis security status
} chassis_info_t;
```

### Processor Information (Type 4)

| Function | Description |
| :--- | :--- |
| `processor_info_t* lazybios_get_processor_info(lazybios_ctx_t* ctx)` | Gets the primary CPU's details. |

**`processor_info_t` Structure (Highlights):**
```c
typedef struct {
    char *socket_designation;   
    char *version;              // CPU version
    char *serial_number;
    char *asset_tag;
    char *part_number;          
    uint16_t core_count;        // Physical core count
    uint16_t core_enabled;      // Enabled cores
    uint16_t thread_count;      // Threads per core
    uint16_t max_speed_mhz;     
    uint16_t current_speed_mhz;
    uint8_t processor_type;     // THE CPU TYPE, needs a decoder will add next time!
    uint8_t processor_family;    // CPU family, needs the lazybios_get_processor_family_string() decoder
    uint16_t processor_family2;  // Extended family for values >= 0xFE
    uint16_t characteristics;    // CPU characteristics, also needs a decoder
    uint16_t L1_cache_handle;    // THE HANDLE of the Caches not the actual size of the caches.
    uint16_t L2_cache_handle;
    uint16_t L3_cache_handle;
    uint8_t voltage;             // Voltage ( Not all systems expose these )
    uint16_t external_clock_mhz; // External Clock
    uint8_t status; // The status of the CPU will also add a decoder next time!
} processor_info_t;
```

### Cache Information (Type 7)

| Function | Description |
| :--- | :--- |
| `cache_info_t* lazybios_get_caches(lazybios_ctx_t* ctx, size_t* count)` | Returns a dynamically allocated array of `cache_info_t` structures for L1, L2, L3 caches. The total number of caches is stored in the `count` pointer. |

**`cache_info_t` Structure (Highlights):**
```c
typedef struct {
    char *socket_designation; // e.g., "L1 Cache"
    uint8_t level;            // CPU caches,0 for L1, 1 for L2, 2 for L3 - Gotta use + 1 to see L1 L2 L3, 
    uint16_t size_kb;         // Cache size in KB
    uint8_t error_correction_type; // The error correction type Single-bit ECC, Multi-Bit ECC etc
    uint8_t system_cache_type; // System cache type e.g. Unified etc
    uint8_t associativity;     // e.g. 4-way
} cache_info_t;
```

### Memory Devices (Type 17)

This one is a bit different, as a system can have multiple memory sticks.

| Function | Description |
| :--- | :--- |
| `memory_device_t* lazybios_get_memory_devices(lazybios_ctx_t* ctx, size_t* count)` | Returns a dynamically allocated array of `memory_device_t` structures. The total number of devices is stored in the `count` pointer. |

**`memory_device_t` Structure (Highlights):**
```c
typedef struct {
    char *locator;       // e.g., "DIMM A1"
    char *manufacturer;  // e.g., "Samsung"
    char *part_number;   // e.g., "M378A1K43DB2-CVF"
    uint32_t size_mb;    // Size in MB (0 means the slot is empty)
    uint16_t speed_mhz;  // Configured speed in MHz
    uint8_t memory_type; // Use helper function to translate this!
    uint8_t form_factor; // Use helper function to translate this!
    // ... other fields like serial, width, etc.
} memory_device_t;
```

## Helper Functions (The Translators)

These functions are super useful for turning the raw numeric codes from the SMBIOS tables into human-readable strings.

| Function                                                                      | Description                                                                                                     |
|:------------------------------------------------------------------------------|:----------------------------------------------------------------------------------------------------------------|
| `const char* lazybios_get_processor_family_string(uint8_t family)`            | Converts the `processor_family` code (from `processor_info_t`) into a string like "Intel Core i7" or "AMD Zen". |
| `const char* lazybios_get_memory_type_string(uint8_t type)`                   | Converts the `memory_type` code (from `memory_device_t`) into a string like "DDR4" or "LPDDR5".                 |
| `const char* lazybios_get_memory_form_factor_string(uint8_t form_factor)`     | Converts the `form_factor` code (from `memory_device_t`) into a string like "DIMM" or "SODIMM".                 |
| `void lazybios_smbios_ver(const lazybios_ctx_t* ctx)`                         | A simple utility to print the SMBIOS version to `stdout`.                                                       |
| `const char* lazybios_get_cache_type_string(uint8_t cache_type)`              | Converts the 'cache_type' code (from 'cache_info_t') into a string like "Unified", "Data" etc.                  |
| `const char* lazybios_get_cache_ecc_string(uint8_t ecc_type)`                 | Converts the 'ecc_type' code (from 'cache_info_t') into a string like "Multi-bit ECC", "Single-bit ECC" etc.    |
| `const char* lazybios_get_cache_associativity_string(uint8_t associativity)`  | Converts the 'associativity' code (from 'cache_info_t') into a string like "4-way", "Fully Associative" etc.    |
## Building and Linking

You'll need to link against the `lazybios` library. Assuming you've built and installed it using CMake, here are a few common ways to integrate it into your project.

### Method 1: System-wide Installation (Simple GCC)

If you've installed the library to a standard system path (e.g., `/usr/local/lib`), you can link directly:

```shell
gcc my_app.c -llazybios -o my_app
```

### Method 2: Using `pkg-config`

If you installed to a custom location, `pkg-config` is the most reliable way to get the correct flags:

```shell
# If installed to a non-standard path, set this first
# export PKG_CONFIG_PATH=~/.local/lib/pkgconfig

# Compile and link
gcc my_app.c $(pkg-config --cflags --libs lazybios) -o my_app
```

### Method 3: CMake Integration (The Modern Way)

If your project uses CMake, you can use `find_package` for a clean integration:

```cmake
# In your CMakeLists.txt
find_package(lazybios REQUIRED)

# Link your executable against the imported target
target_link_libraries(my_app PRIVATE lazybios::lazybios)
```

That's the gist of it! If you run into any issues or have suggestions, feel free to reach out. Happy coding!

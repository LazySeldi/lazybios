# lazybios: High Performance SMBIOS/DMI Parsing Library

Hey there! Welcome to the documentation for **lazybios**, a simple and straightforward C library designed to read and parse your system's **SMBIOS (System Management BIOS)** or **DMI (Desktop Management Interface)** tables.

If you've ever needed to grab hardware info like your BIOS version, motherboard model, CPU details, or RAM specs directly from the system firmware tables on Linux, this library is what you need. It handles all the low-level parsing so you can focus on what you're building.

## ⚠️ Important Heads-Up!

Before you dive in, here are a few quick notes:

*   **Root Access is Needed:** To read the SMBIOS tables from the `/sys/firmware/dmi/tables/` files on Linux, you'll need root privileges (or at least read access to those files).
*   **Linux Only:** This library is built specifically for Linux systems, as it relies on the DMI table files exposed in the `/sys` filesystem.
*   **Memory Management:** We use `malloc` internally to allocate strings and data structures. **Always** remember to call `lazybios_ctx_free()` when you're done to prevent memory leaks.

## Getting Started

### 1. Context is Everything

The entire library revolves around the `lazybios_ctx_t` structure, which holds all the parsed data and internal state. \
Just 3 functions you have to follow to avoid ANY memory errors, well that is until my library is fully perfected!

| Function | Description |
|:---| :--- |
| `lazybios_ctx_t* lazybiosCTXNew(void)` | Creates and allocates a new `lazybios_ctx_t` structure. This is your starting point. |
| `int lazybiosInit(lazybios_ctx_t* ctx)` | Initializes the library. It reads the SMBIOS entry point and the main DMI table, populating the context with the raw data. Returns `0` on success, `-1` on failure. |
| `void lazybiosCleanup(lazybios_ctx_t* ctx)` | Frees internal memory used for raw data. This is called automatically by `lazybios_ctx_free()`, so you usually don't need to call it directly. |


### 2. Example Usage Flow

Here's a quick look at how you'd typically use the library (for a complete example, check out `test.c`):

```c
#include <stdio.h>
#include "lazybios.h" // Or <lazybios.h> if installed to system include directories

int main() {
    // 1. Create the context
    lazybios_ctx_t *ctx = lazybiosCTXNew();
    // You don't need to check for errors since the function already does

    // 2. Initialize the library (reads the DMI tables)
    if (lazybiosInit(ctx) != 0) {
        fprintf(stderr, "Failed to initialize lazybios. Are you running as root?\n");
        lazybios_ctx_free(ctx);
        return 1;
    }
    // Even here you don't really need to but keep it just in case

    // 3. Get the data you need
    system_info_t *sys_info = lazybiosGetSystemInfo(ctx);
    if (sys_info) {
        printf("System Manufacturer: %s\n", sys_info->manufacturer);
        printf("Product Name: %s\n", sys_info->product_name);
        printf("Serial Number: %s\n", sys_info->serial_number);
    }

    // 4. Clean up everything when you're done
    lazybios_ctx_free(ctx);
    // Thats it really, you just have to follow 3 rules: create, initialize and cleanup.
    
    return 0;
}
```

## Data Retrieval Functions

These functions are how you get the actual hardware information. They all take the `lazybios_ctx_t*` as an argument and return a pointer to a specific data structure, or `NULL` if the information couldn't be found (e.g., the SMBIOS structure type wasn't present).

### BIOS Information (Type 0)

| Function | Description |
|:---| :--- |
| `bios_info_t* lazybiosGetBIOSInfo(lazybios_ctx_t* ctx)` | Gets the BIOS vendor, version, release date, and ROM size. |

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
|:---| :--- |
| `system_info_t* lazybiosGetSystemInfo(lazybios_ctx_t* ctx)` | Gets the overall system details. |

**`system_info_t` Structure:**
```c
typedef struct {
    char *manufacturer;  // e.g., "HP"
    char *product_name;  // e.g., "HP ProDesk 400 G9 SFF"
    char *version;       // e.g., "V1.0"
    char *serial_number; // The system's serial number
    char *uuid;          // Properly formatted UUID (e.g., "550e8400-e29b-41d4-a716-446655440000")
} system_info_t;
```

### Baseboard (Motherboard) Information (Type 2)

| Function | Description |
| :--- | :--- |
| `baseboard_info_t* lazybiosGetBaseboardInfo(lazybios_ctx_t* ctx)` | Gets the motherboard's details. |

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
|:---| :--- |
| `chassis_info_t* lazybiosGetChassisInfo(lazybios_ctx_t* ctx)` | Gets the physical enclosure details. |

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
|:---| :--- |
| `processor_info_t* lazybiosGetProcInfo(lazybios_ctx_t* ctx)` | Gets the primary CPU's details. |

**`processor_info_t` Structure (Highlights):**
```c
typedef struct {
    char *socket_designation;   
    char *version;              // CPU version string
    char *serial_number;
    char *asset_tag;
    char *part_number;          
    uint16_t core_count;        // Physical core count
    uint16_t core_enabled;      // Enabled cores
    uint16_t thread_count;      // Thread count
    uint16_t max_speed_mhz;     
    uint16_t current_speed_mhz;
    uint8_t processor_type;     // Use lazybiosProcTypeStr() for a human-readable string
    uint8_t processor_family;   // Use lazybiosProcFamilyStr() for a human-readable string
    uint16_t processor_family2; // Extended family for values >= 0xFE
    uint16_t characteristics;   // Use lazybiosProcCharacteristicsStr() for flags
    uint16_t L1_cache_handle;   // Handle reference to cache structures (Type 7)
    uint16_t L2_cache_handle;
    uint16_t L3_cache_handle;
    uint8_t voltage;            // Voltage (not always available)
    uint16_t external_clock_mhz;
    uint8_t status;             // Use lazybiosProcStatusStr() for status
} processor_info_t;
```

### Cache Information (Type 7)

| Function | Description |
|:---|:---|
| `cache_info_t* lazybiosGetCacheInfo(lazybios_ctx_t* ctx, size_t* count)` | Returns a dynamically allocated array of `cache_info_t` structures for L1, L2, L3 caches. The total number of caches is stored in the `count` pointer. |

**`cache_info_t` Structure:**
```c
typedef struct {
    char *socket_designation;  // e.g., "L1 Cache"
    uint8_t level;             // 0 for L1, 1 for L2, 2 for L3 (add 1 when displaying)
    uint32_t size_kb;          // Cache size in KB (now 32-bit for large caches)
    uint8_t error_correction_type; // Use lazybiosCacheECCTypeStr()
    uint8_t system_cache_type; // Use lazybiosCacheTypeStr()
    uint8_t associativity;     // Use lazybiosCacheAssociativityStr()
} cache_info_t;
```

### Port Connector Information (Type 8)

| Function | Description |
|:---|:---|
| `port_connector_info_t* lazybiosGetPortConnectorInfo(lazybios_ctx_t* ctx, size_t* count);` | Returns a dynamically allocated array of `port_connector_info_t` structures. The total number of ports is stored in the `count` pointer. |


**`port_connector_info_t` Structure:**
```c
typedef struct {
    uint16_t handle; // The handle
    char *internal_ref_designator; // The Internal Reference Designator
    uint8_t  internal_connector_type; // Use lazybiosPortConnectorTypeStr()
    char *external_ref_designator; // The External Reference Designator
    uint8_t  external_connector_type; // Use lazybiosPortConnectorTypeStr()
    uint8_t  port_type; // Use lazybiosPortTypeStr()
} port_connector_info_t;
```

### Onboard Device Information (Type 10)
| Function | Description |
| :--- |:---|
| `onboard_devices_t* lazybiosGetOnboardDeviceInfo(lazybios_ctx_t* ctx, size_t* count)` | Returns information about the onboard devices in the motherboard (e.g., Audio Controller, Ethernet). The total number of devices is stored in the `count` pointer. |

**`onboard_devices_t` Structure:**
```c
typedef struct {
    char *description_string;
    uint8_t type; // Use lazybiosOnboardDeviceTypeStr()
    bool enabled; // True if the device is enabled
} onboard_devices_t;
```

### OEM Strings Information (Type 11)
| Function | Description |
|:---|:---|
| `OEMStrings_t* lazybiosGetOEMStringInfo(lazybios_ctx_t* ctx, size_t* count)` | Returns information about OEM strings specified by the manufacturer. The total number of strings is stored in the `count` pointer. |

**`OEMStrings_t` Structure:**
```c
typedef struct {
    uint8_t string_count; // The number of strings in the array
    char *strings[];      // The array of OEM strings
} OEMStrings_t;
```

### Physical Memory Array (Type 16)

| Function | Description |
|:---| :--- |
| `physical_memory_array_t* lazybiosGetPhysicalMemArrayInfo(lazybios_ctx_t* ctx, size_t* count)` | Returns information about the physical memory array (motherboard memory controller). The total number of arrays is stored in the `count` pointer. |

**`physical_memory_array_t` Structure:**
```c
typedef struct {
    uint8_t location;          // Use lazybiosPhysicalMemArrayLocationStr()
    uint8_t use;               // Use lazybiosPhysicalMemArrayUseStr()
    uint8_t ecc_type;          // Use lazybiosPhysicalMemArrayECCStr()
    uint64_t max_capacity_kb;  // Maximum capacity in KB
    uint16_t num_devices;      // Number of memory device slots
    bool extended_capacity;    // True if using extended capacity field
} physical_memory_array_t;
```

### Memory Devices (Type 17)

| Function | Description |
| :--- | :--- |
| `memory_device_t* lazybiosGetMemDeviceInfo(lazybios_ctx_t* ctx, size_t* count)` | Returns a dynamically allocated array of `memory_device_t` structures, one for each physical memory stick/slot. The total number of devices is stored in the `count` pointer. |

**`memory_device_t` Structure (Highlights):**
```c
typedef struct {
    char *locator;       // e.g., "DIMM A1"
    char *manufacturer;  // e.g., "Samsung"
    char *part_number;   // e.g., "M378A1K43DB2-CVF"
    uint32_t size_mb;    // Size in MB (0 means the slot is empty)
    uint16_t speed_mhz;  // Configured speed in MHz
    uint8_t memory_type; // Use lazybiosMemTypeStr() to translate this!
    uint8_t form_factor; // Use lazybiosMemFormFactorStr() to translate this!
    // ... other fields like serial, width, etc.
} memory_device_t;
```

## Helper Functions

These functions convert raw numeric codes from SMBIOS tables into human-readable strings. **All helper functions are now thread-safe.**

### Processor Helpers
| Function | Description |
|:---|:---|
| `const char* lazybiosProcFamilyStr(uint8_t family)` | Converts processor family code to string (e.g., "Intel Core i7") |
| `const char* lazybiosProcTypeStr(uint8_t type)` | Converts processor type code to string (e.g., "Central Processor") |
| `const char* lazybiosProcStatusStr(uint8_t status)` | Converts processor status to string (e.g., "Enabled", "Idle") |
| `const char* lazybiosProcCharacteristicsStr(uint16_t characteristics)` | Converts processor characteristics to string (e.g., "64-bit Support", "Multi-Core") |
| `const char* lazybiosProcSocketTypeStr(uint8_t type)` | Converts processor socket type to string (e.g., "LGA1366", "AM5") |

### Cache Helpers
| Function | Description |
|:---|:---|
| `const char* lazybiosCacheTypeStr(uint8_t cache_type)` | Converts cache type to string (e.g., "Unified", "Data") |
| `const char* lazybiosCacheECCTypeStr(uint8_t ecc_type)` | Converts ECC type to string (e.g., "Single-bit ECC") |
| `const char* lazybiosCacheAssociativityStr(uint8_t assoc)` | Converts associativity to string (e.g., "8-way", "16-way") |

### Port Connector Helpers
| Function | Description |
|:---|:---|
| `const char* lazybiosPortConnectorTypeStr(uint8_t connector_type)` | Converts Port Connector type to string (e.g., "PS2", "USB") |
| `const char* lazybiosPortTypeStr(uint8_t port_type)` | Converts Port type to string (e.g., "USB", "Mouse Port") |

### On Board Devices Helpers
| Function | Description |
|:---|:---|
| `const char* lazybiosOnboardDeviceTypeStr(uint8_t type)` | Converts On Board Devices type to string (e.g., "Ethernet", "Sound") |

### Memory Helpers
| Function | Description |
|:---|:---|
| `const char* lazybiosMemTypeStr(uint8_t type)` | Converts memory type to string (e.g., "DDR4", "DDR5") |
| `const char* lazybiosMemFormFactorStr(uint8_t ff)` | Converts form factor to string (e.g., "DIMM", "SODIMM") |
| `const char* lazybiosPhysicalMemArrayLocationStr(uint8_t loc)` | Converts array location to string (e.g., "System board") |
| `const char* lazybiosPhysicalMemArrayUseStr(uint8_t use)` | Converts array use to string (e.g., "System memory") |
| `const char* lazybiosPhysicalMemArrayECCStr(uint8_t ecc)` | Converts array ECC type to string (e.g., "Multi-bit ECC") |

### Utility Functions
| Function | Description |
|:---|:---|
| `void lazybiosPrintVer(const lazybios_ctx_t* ctx)` | Prints the SMBIOS version to stdout |
| `const smbios_entry_info_t* lazybiosGetEntryInfo(const lazybios_ctx_t* ctx)` | Gets the raw SMBIOS entry point information (version, table address, etc.) |

## Linking 

You'll need to link against the `lazybios` library. Assuming you've built and installed it using CMake, here are a few common ways to integrate it into your project.

### Method 1: System-wide Installation (Simple GCC)

If you've installed the library to a standard system path (e.g., `/usr/lib`), you can link directly:

```shell
gcc my_app.c -llazybios -o my_app
```

### Method 2: Using `pkg-config`

If you installed to a custom location, `pkg-config` is the most reliable way to get the correct flags:

```shell
# If installed to a non-standard path, set this first
# export PKG_CONFIG_PATH=path/to/your/custom/location/pkgconfig # this might be unnecessary sometimes

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

To really understand how these functions work together, take a look at `test.c` it literally tests every piece of functionality.

That's it! If you run into any issues or have suggestions, feel free to reach out.

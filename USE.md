# lazybios - Usage Documentation

A complete guide on how to build, use, and integrate the lazybios SMBIOS parsing library.

## 🛠️ Building the Library

### Prerequisites
- CMake 4.0+
- C23 compatible compiler (GCC/Clang)
- Linux system with SMBIOS support

### Build Instructions
```bash
# Clone the repository
git clone https://github.com/LazySeldi/lazybios
cd lazybios

# Configure with CMake
cmake -B build

# Build the library and test executable
cmake --build build

# Run the comprehensive test (requires root for SMBIOS access)
sudo ./build/lazybios_test
```
### Build Output
```
    build/liblazybios.so - The shared library

    build/lazybios_test - Comprehensive test executable
```

## API Reference
```c
int lazybios_init(lazybios_ctx_t* ctx);
```
- Initializes the Library and exits.
- Exit code 0 means success and -1 means something went wrong.

--------------------

```c
void lazybios_cleanup(lazybios_ctx_t* ctx);
```
- Cleans up the memory that the library used.
- Should be added at the end just before the programs exit.
- Don't use the library again if you used cleanup().

----------------------

```c
void lazybios_smbios_ver(const lazybios_ctx_t* ctx);
```
- Prints the SMBIOS version to stdout
- Printed Example "SMBIOS version 2.5"

----------------------

```c
const smbios_entry_info_t* lazybios_get_entry_info(const lazybios_ctx_t* ctx);
```
- Returns SMBIOS entry information.
- Returns pointer to smbios_entry_info_t struct
- The struct contains:
```c
    uint8_t major;          // SMBIOS major version
    uint8_t minor;          // SMBIOS minor version  
    uint32_t table_length;  // DMI table length in bytes
    uint64_t table_address; // Table physical address
    bool is_64bit;          // True for SMBIOS 3.x 
```
---------------------------
```c
bios_info_t* lazybios_get_bios_info(lazybios_ctx_t* ctx);
```
- Returns BIOS Information (SMBIOS Type 0).
- Returns Pointer to bios_info_t or NULL on failure.
- The struct contains:
```c
    char *vendor;        // BIOS vendor string
    char *version;       // BIOS version string
    char *release_date;  // BIOS release date
    uint16_t rom_size_kb; // BIOS ROM size in KB
```
---------------

```c
system_info_t* lazybios_get_system_info(lazybios_ctx_t* ctx);
```
- Returns System Information (SMBIOS Type 1).
- Returns Pointer to system_info_t or NULL on failure.
- The struct contains:
```c
    char *manufacturer;  // System manufacturer
    char *product_name;  // Product name/model
    char *version;       // System version
    char *serial_number; // System serial number
```
--------------------------------

```c
chassis_info_t* lazybios_get_chassis_info(lazybios_ctx_t* ctx);
```
- Returns CHASSIS Information (SMBIOS Type 3). 
- Returns Pointer to chassis_info_t or NULL on failure.
- The struct contains:
```c
    char *asset_tag;     // Chassis asset tag
    char *sku;           // Chassis SKU number
```
------------------------

```c
processor_info_t* lazybios_get_processor_info(lazybios_ctx_t* ctx);
```
- Returns processor information (SMBIOS Type 4).
- Returns Pointer to processor_info_t or NULL on failure.
- The struct contains:
```c
    char *socket_designation;  // Physical socket name
    char *version;             // Processor version string
    char *serial_number;       // Processor serial
    char *asset_tag;           // Processor asset tag  
    char *part_number;         // Processor part number
    uint16_t core_count;       // Physical core count
    uint16_t core_enabled;     // Enabled core count
    uint16_t thread_count;     // Threads per core
    uint16_t max_speed_mhz;    // Maximum processor speed
    uint8_t processor_type;    // Processor type code
    uint8_t processor_family;  // Processor family code
    uint16_t characteristics;  // Processor features bitmap
    uint8_t voltage;           // Voltage information
    uint16_t external_clock_mhz; // External bus speed
    uint16_t l1_cache_handle;  // L1 cache handle
    uint16_t l2_cache_handle;  // L2 cache handle  
    uint16_t l3_cache_handle;  // L3 cache handle
```
-------------------------------

```c
memory_device_t* lazybios_get_memory_devices(lazybios_ctx_t* ctx, size_t* count);
```
- Returns array of memory device information (SMBIOS Type 17).
- Parameters: count - pointer to store number of memory devices.
- Needs to be provided by caller so the caller can determine how many times to run a loop to print out all of the memory devices!
- Returns Array of memory_device_t or NULL on failure.
- The struct contains:
```c
    char *locator;       // Physical slot location
    char *bank_locator;  // Bank location string
    char *manufacturer;  // Memory manufacturer
    char *serial_number; // Memory serial number
    char *part_number;   // Memory part number
    uint16_t size_mb;    // Size in MB (0 = empty slot)
    uint16_t speed_mhz;  // Memory speed in MHz
    uint8_t memory_type; // Memory type code
    uint8_t form_factor; // Form factor code
    uint8_t data_width;  // Data width in bits
```
--------------------------------

# Helper Functions

```c
const char* lazybios_get_processor_family_string(uint8_t family);
```
- Converts processor family code to human-readable string.
- Parameters: family - processor family code from processor_info_t.
- Returns: String representation of processor family
- Example:
```c
processor_info_t *cpu = get_processor_info();
printf("CPU: %s\n", get_processor_family_string(cpu->processor_family));
```
--------------------

```c
size_t lazybios_get_smbios_structure_min_length(const lazybios_ctx_t* ctx, uint8_t type);
```
- Returns minimum required length for SMBIOS structure type.
- Parameters: type - SMBIOS structure type.
- Internal use but available if needed

---------------

```c
bool lazybios_is_smbios_version_at_least(const lazybios_ctx_t* ctx, uint8_t major, uint8_t minor);
```
- Checks if current SMBIOS version meets minimum requirement.
- Parameters: major, minor - version to check against.
- Returns: true if SMBIOS version >= specified version

-------------

# Integration in Your Project
### Method 1: Direct File Inclusion In Your Project
```c
#include "lazybios.h"


// Your code here...
```

### Method 2: Link as Shared Library
```bash

# Compile your program with the library
gcc my_program.c -L. -llazybios -o my_program

# Set library path if needed
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
```

### Method 3: CMake Integration
```cmake

# In your CMakeLists.txt
add_library(lazybios SHARED IMPORTED)
set_target_properties(lazybios PROPERTIES
IMPORTED_LOCATION "/path/to/liblazybios.so"
)

# Link with your executable
add_executable(my_program my_program.c)
target_link_libraries(my_program lazybios)
```
### ⚠️ Important Notes
```
    Root Privileges: Most functions require root access to read SMBIOS tables

    Memory Management: Always call cleanup() to prevent memory leaks

    Error Checking: Check return values for NULL before accessing data

    Thread Safety: Not thread-safe - call from a single thread

    Platform: Linux only (relies on /sys/firmware/dmi/tables/)
```

### Troubleshooting
Common Issues:
```
    Permission denied - Run with sudo

    Library not found - Check LD_LIBRARY_PATH or use direct linking

    No SMBIOS data - Check if your system supports SMBIOS

    Segmentation fault - Ensure init() succeeded before calling other functions
```
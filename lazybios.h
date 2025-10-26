#ifndef LAZYBIOS_H
#define LAZYBIOS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// ===== SMBIOS Entry Point Constants =====
#define SMBIOS2_ANCHOR "_SM_"
#define SMBIOS3_ANCHOR "_SM3_"
#define SMBIOS2_ENTRY_SIZE 31
#define SMBIOS3_ENTRY_SIZE 24

// Entry point offsets
#define ENTRY_MAJOR_OFFSET       6
#define ENTRY_MINOR_OFFSET       7
#define ENTRY_TABLE_LENGTH_OFFSET 0x16
#define ENTRY_TABLE_ADDR_OFFSET  0x18
#define ENTRY_TABLE_LENGTH_OFFSET_3 0x0C
#define ENTRY_TABLE_ADDR_OFFSET_3 0x10

// ===== SMBIOS Structure Types =====
#define SMBIOS_TYPE_BIOS       0
#define SMBIOS_TYPE_SYSTEM     1
#define SMBIOS_TYPE_BASEBOARD  2
#define SMBIOS_TYPE_CHASSIS    3
#define SMBIOS_TYPE_PROCESSOR  4
#define SMBIOS_TYPE_MEMDEVICE  17
#define SMBIOS_TYPE_END        127

// ===== Version-Aware Minimum Lengths =====
#define BIOS_MIN_LENGTH_2_0     0x12
#define BIOS_MIN_LENGTH_3_0     0x18
#define SYSTEM_MIN_LENGTH_2_0   0x08
#define SYSTEM_MIN_LENGTH_3_0   0x1B
#define CHASSIS_MIN_LENGTH_2_0  0x09
#define CHASSIS_MIN_LENGTH_3_0  0x15
#define MEMORY_MIN_LENGTH_2_0   0x15
#define MEMORY_MIN_LENGTH_3_0   0x2C
#define PROC_MIN_LENGTH_2_0     0x1A
#define PROC_MIN_LENGTH_2_6     0x2A

// ===== BIOS Info (Type 0) Field Offsets =====
#define BIOS_VENDOR_OFFSET         0x04
#define BIOS_VERSION_OFFSET        0x05
#define BIOS_RELEASE_DATE_OFFSET   0x08
#define BIOS_ROM_SIZE_OFFSET       0x09

// ===== System Info (Type 1) Field Offsets =====
#define SYS_MANUFACTURER_OFFSET    0x04
#define SYS_PRODUCT_OFFSET         0x05
#define SYS_VERSION_OFFSET         0x06
#define SYS_SERIAL_OFFSET          0x07

// ===== Chassis Info (Type 3) Field Offsets =====
#define CHASSIS_ASSET_TAG_OFFSET   0x08
#define CHASSIS_SKU_OFFSET         0x19

// ===== Processor Information (Type 4) Field Offsets =====
#define PROC_SOCKET_OFFSET          0x04
#define PROC_TYPE_OFFSET            0x05
#define PROC_FAMILY_OFFSET          0x06
#define PROC_VERSION_OFFSET         0x10
#define PROC_MAX_SPEED_OFFSET       0x14
#define PROC_VOLTAGE_OFFSET         0x11
#define PROC_EXTERNAL_CLOCK_OFFSET  0x12
#define PROC_CHARACTERISTICS_OFFSET 0x26
#define PROC_CORE_COUNT_OFFSET      0x23
#define PROC_CORE_ENABLED_OFFSET    0x24
#define PROC_THREAD_COUNT_OFFSET    0x25
#define PROC_SERIAL_OFFSET_2_5      0x20

// ===== Memory Device (Type 17) Field Offsets =====
#define MEM_DEVICE_LOCATOR_OFFSET      0x10
#define MEM_DEVICE_BANK_LOCATOR_OFFSET 0x11
#define MEM_DEVICE_MANUFACTURER_OFFSET 0x17
#define MEM_DEVICE_SERIAL_OFFSET       0x18
#define MEM_DEVICE_PART_NUMBER_OFFSET  0x1A
#define MEM_DEVICE_SIZE_OFFSET         0x0C
#define MEM_DEVICE_SPEED_OFFSET        0x15
#define MEM_DEVICE_TYPE_OFFSET         0x12
#define MEM_DEVICE_FORM_FACTOR_OFFSET  0x0E
#define MEM_DEVICE_WIDTH_OFFSET        0x08

// ===== Memory Type Constants =====
#define MEMORY_TYPE_DDR3   0x18
#define MEMORY_TYPE_DDR4   0x1A
#define MEMORY_TYPE_DDR5   0x22

// ===== Comprehensive Processor Family Constants =====
// Intel Processor Families
#define INTEL_8086             0x01
#define INTEL_80286            0x02
#define INTEL_80386            0x03
#define INTEL_80486            0x04
#define INTEL_PENTIUM          0x05
#define INTEL_PENTIUM_PRO      0x06
#define INTEL_PENTIUM_II       0x07
#define INTEL_PENTIUM_III      0x08
#define INTEL_PENTIUM_4        0x09
#define INTEL_PENTIUM_M        0x0A
#define INTEL_CELERON          0x0B
#define INTEL_CORE             0x0C
#define INTEL_CORE_2           0x0D
#define INTEL_CORE_I3          0x0E
#define INTEL_CORE_I5          0x0F
#define INTEL_CORE_I7          0x10
#define INTEL_CORE_I9          0x11
#define INTEL_ATOM             0x12
#define INTEL_XEON             0xB3
#define INTEL_XEON_MP          0xB4
#define INTEL_XEON_3XXX        0xB5
#define INTEL_XEON_5XXX        0xB6
#define INTEL_XEON_7XXX        0xB7

// AMD Processor Families
#define AMD_K5                 0x18
#define AMD_K6                 0x19
#define AMD_ATHLON             0x1A
#define AMD_ATHLON_64          0x1B
#define AMD_OPTERON            0x1C
#define AMD_SEMPRON            0x1D
#define AMD_TURION             0x1E
#define AMD_PHENOM             0x1F
#define AMD_PHENOM_II          0x20
#define AMD_ATHLON_II          0x21
#define AMD_FX                 0x22
#define AMD_RYZEN_3            0x23
#define AMD_RYZEN_5            0x24
#define AMD_RYZEN_7            0x25
#define AMD_RYZEN_9            0x26
#define AMD_RYZEN_THREADRIPPER 0x27
#define AMD_EPYC               0x28

// Other Processor Families
#define ARM_CORTEX             0x30
#define ARM_APPLE_M1           0x31
#define ARM_APPLE_M2           0x32
#define IBM_POWER              0x40
#define IBM_POWERPC            0x41

// ===== General Constants =====
#define SMBIOS_HEADER_SIZE         4
#define BIOS_ROM_SIZE_MULTIPLIER   64

// ===== File Paths =====
#define SMBIOS_ENTRY "/sys/firmware/dmi/tables/smbios_entry_point"
#define DMI_TABLE    "/sys/firmware/dmi/tables/DMI"

// ===== Data Structures =====
typedef struct {
    uint8_t major;
    uint8_t minor;
    uint32_t table_length;
    uint64_t table_address;
    bool is_64bit;
} smbios_entry_info_t;

typedef struct {
    char *vendor;
    char *version;
    char *release_date;
    uint16_t rom_size_kb;
} bios_info_t;

typedef struct {
    char *manufacturer;
    char *product_name;
    char *version;
    char *serial_number;
} system_info_t;

typedef struct {
    char *asset_tag;
    char *sku;
} chassis_info_t;

typedef struct {
    char *locator;
    char *bank_locator;
    char *manufacturer;
    char *serial_number;
    char *part_number;
    uint16_t size_mb;
    uint16_t speed_mhz;
    uint8_t memory_type;
    uint8_t form_factor;
    uint8_t data_width;
} memory_device_t;

typedef struct {
    char *socket_designation;
    char *version;
    char *serial_number;
    char *asset_tag;
    char *part_number;
    uint16_t core_count;
    uint16_t core_enabled;
    uint16_t thread_count;
    uint16_t max_speed_mhz;
    uint8_t processor_type;
    uint8_t processor_family;
    uint16_t characteristics;
    uint8_t voltage;
    uint16_t external_clock_mhz;
    uint16_t l1_cache_handle;
    uint16_t l2_cache_handle;
    uint16_t l3_cache_handle;
} processor_info_t;

// ===== Context Structure =====
typedef struct lazybios_ctx {
    // Raw SMBIOS data
    uint8_t *dmi_data;
    size_t dmi_len;
    smbios_entry_info_t entry_info;

    // Cached parsed data
    bios_info_t *bios_info_ptr;
    system_info_t *system_info_ptr;
    chassis_info_t *chassis_info_ptr;
    processor_info_t *processor_info_ptr;
    memory_device_t *memory_devices_ptr;
    size_t memory_devices_count;
} lazybios_ctx_t;

// ===== Public API =====
// Context management
lazybios_ctx_t* lazybios_ctx_new(void);
void lazybios_ctx_free(lazybios_ctx_t* ctx);

// Initialization and cleanup
int lazybios_init(lazybios_ctx_t* ctx);
void lazybios_cleanup(lazybios_ctx_t* ctx);

// Version info
void lazybios_smbios_ver(const lazybios_ctx_t* ctx);
const smbios_entry_info_t* lazybios_get_entry_info(const lazybios_ctx_t* ctx);

// Basic info retrievers
bios_info_t* lazybios_get_bios_info(lazybios_ctx_t* ctx);
system_info_t* lazybios_get_system_info(lazybios_ctx_t* ctx);
chassis_info_t* lazybios_get_chassis_info(lazybios_ctx_t* ctx);
processor_info_t* lazybios_get_processor_info(lazybios_ctx_t* ctx);
memory_device_t* lazybios_get_memory_devices(lazybios_ctx_t* ctx, size_t* count);

// Helper functions
size_t lazybios_get_smbios_structure_min_length(const lazybios_ctx_t* ctx, uint8_t type);
bool lazybios_is_smbios_version_at_least(const lazybios_ctx_t* ctx, uint8_t major, uint8_t minor);
const char* lazybios_get_processor_family_string(uint8_t family);

#endif // LAZYBIOS_H
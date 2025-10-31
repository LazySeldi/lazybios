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

// ===== SMBIOS 2.x Entry Point Offsets =====
#define ENTRY2_MAJOR_OFFSET       6
#define ENTRY2_MINOR_OFFSET       7
#define ENTRY2_TABLE_LENGTH_OFFSET 0x16
#define ENTRY2_TABLE_ADDR_OFFSET  0x18

// ===== SMBIOS 3.x Entry Point Offsets (FIXED) =====
#define ENTRY3_MAJOR_OFFSET       7
#define ENTRY3_MINOR_OFFSET       8
#define ENTRY3_DOCREV_OFFSET      9
#define ENTRY3_TABLE_LENGTH_OFFSET 0x0C
#define ENTRY3_TABLE_ADDR_OFFSET  0x10

// ===== SMBIOS Structure Types =====
#define SMBIOS_TYPE_BIOS       0
#define SMBIOS_TYPE_SYSTEM     1
#define SMBIOS_TYPE_BASEBOARD  2
#define SMBIOS_TYPE_CHASSIS    3
#define SMBIOS_TYPE_PROCESSOR  4
#define SMBIOS_TYPE_MEMARRAY   16
#define SMBIOS_TYPE_MEMDEVICE  17
#define SMBIOS_TYPE_MEMARRAY_MAPPED 19
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
#define BASEBOARD_MIN_LENGTH    0x08

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
#define SYS_UUID_OFFSET            0x08

// ===== Baseboard Info (Type 2) Field Offsets =====
#define BASEBOARD_MANUFACTURER_OFFSET  0x04
#define BASEBOARD_PRODUCT_OFFSET       0x05
#define BASEBOARD_VERSION_OFFSET       0x06
#define BASEBOARD_SERIAL_OFFSET        0x07
#define BASEBOARD_ASSET_TAG_OFFSET     0x08

// ===== Chassis Info (Type 3) Field Offsets =====
#define CHASSIS_ASSET_TAG_OFFSET   0x08
#define CHASSIS_SKU_OFFSET         0x19

// ===== Processor Information (Type 4) Field Offsets =====
#define PROC_SOCKET_OFFSET          0x04
#define PROC_TYPE_OFFSET            0x05
#define PROC_FAMILY_OFFSET          0x06
#define PROC_MANUFACTURER_OFFSET    0x07
#define PROC_ID_OFFSET              0x08
#define PROC_VERSION_OFFSET         0x10
#define PROC_VOLTAGE_OFFSET         0x11
#define PROC_EXTERNAL_CLOCK_OFFSET  0x12
#define PROC_MAX_SPEED_OFFSET       0x14
#define PROC_CURRENT_SPEED_OFFSET   0x16
#define PROC_STATUS_OFFSET          0x18
#define PROC_UPGRADE_OFFSET         0x19
#define PROC_SERIAL_OFFSET_2_5      0x20
#define PROC_ASSET_TAG_OFFSET_2_5   0x21
#define PROC_PART_NUMBER_OFFSET_2_5 0x22
#define PROC_CORE_COUNT_OFFSET      0x23
#define PROC_CORE_ENABLED_OFFSET    0x24
#define PROC_THREAD_COUNT_OFFSET    0x25
#define PROC_CHARACTERISTICS_OFFSET 0x26
#define PROC_FAMILY2_OFFSET         0x28
#define PROC_CORE_COUNT2_OFFSET     0x2A
#define PROC_CORE_ENABLED2_OFFSET   0x2C
#define PROC_THREAD_COUNT2_OFFSET   0x2E

// ===== Memory Device (Type 17) Field Offsets =====
#define MEM_DEVICE_ARRAY_HANDLE_OFFSET 0x04
#define MEM_DEVICE_ERROR_HANDLE_OFFSET 0x06
#define MEM_DEVICE_TOTAL_WIDTH_OFFSET  0x08
#define MEM_DEVICE_DATA_WIDTH_OFFSET   0x0A
#define MEM_DEVICE_SIZE_OFFSET         0x0C
#define MEM_DEVICE_FORM_FACTOR_OFFSET  0x0E
#define MEM_DEVICE_SET_OFFSET          0x0F
#define MEM_DEVICE_LOCATOR_OFFSET      0x10
#define MEM_DEVICE_BANK_LOCATOR_OFFSET 0x11
#define MEM_DEVICE_TYPE_OFFSET         0x12
#define MEM_DEVICE_TYPE_DETAIL_OFFSET  0x13
#define MEM_DEVICE_SPEED_OFFSET        0x15
#define MEM_DEVICE_MANUFACTURER_OFFSET 0x17
#define MEM_DEVICE_SERIAL_OFFSET       0x18
#define MEM_DEVICE_ASSET_TAG_OFFSET    0x19
#define MEM_DEVICE_PART_NUMBER_OFFSET  0x1A
#define MEM_DEVICE_ATTRIBUTES_OFFSET   0x1B
#define MEM_DEVICE_EXT_SIZE_OFFSET     0x1C
#define MEM_DEVICE_CONF_SPEED_OFFSET   0x20
#define MEM_DEVICE_MIN_VOLTAGE_OFFSET  0x22
#define MEM_DEVICE_MAX_VOLTAGE_OFFSET  0x24
#define MEM_DEVICE_CONF_VOLTAGE_OFFSET 0x26

// ===== Memory Type Constants (Official SMBIOS Values) =====
#define MEMORY_TYPE_OTHER       0x01
#define MEMORY_TYPE_UNKNOWN     0x02
#define MEMORY_TYPE_DRAM        0x03
#define MEMORY_TYPE_EDRAM       0x04
#define MEMORY_TYPE_VRAM        0x05
#define MEMORY_TYPE_SRAM        0x06
#define MEMORY_TYPE_RAM         0x07
#define MEMORY_TYPE_ROM         0x08
#define MEMORY_TYPE_FLASH       0x09
#define MEMORY_TYPE_EEPROM      0x0A
#define MEMORY_TYPE_FEPROM      0x0B
#define MEMORY_TYPE_EPROM       0x0C
#define MEMORY_TYPE_CDRAM       0x0D
#define MEMORY_TYPE_3DRAM       0x0E
#define MEMORY_TYPE_SDRAM       0x0F
#define MEMORY_TYPE_SGRAM       0x10
#define MEMORY_TYPE_RDRAM       0x11
#define MEMORY_TYPE_DDR         0x12
#define MEMORY_TYPE_DDR2        0x13
#define MEMORY_TYPE_DDR2_FB_DIMM 0x14
#define MEMORY_TYPE_DDR3        0x18
#define MEMORY_TYPE_FBD2        0x19
#define MEMORY_TYPE_DDR4        0x1A
#define MEMORY_TYPE_LPDDR       0x1B
#define MEMORY_TYPE_LPDDR2      0x1C
#define MEMORY_TYPE_LPDDR3      0x1D
#define MEMORY_TYPE_LPDDR4      0x1E
#define MEMORY_TYPE_LPDDR_NV    0x1F
#define MEMORY_TYPE_HBM         0x20
#define MEMORY_TYPE_HBM2        0x21
#define MEMORY_TYPE_DDR5        0x22
#define MEMORY_TYPE_LPDDR5      0x23
#define MEMORY_TYPE_HBM3        0x24

// ===== Memory Form Factor Constants (Official SMBIOS Values) =====
#define MEMORY_FORM_FACTOR_OTHER     0x01
#define MEMORY_FORM_FACTOR_UNKNOWN   0x02
#define MEMORY_FORM_FACTOR_SIMM      0x03
#define MEMORY_FORM_FACTOR_SIP       0x04
#define MEMORY_FORM_FACTOR_CHIP      0x05
#define MEMORY_FORM_FACTOR_DIP       0x06
#define MEMORY_FORM_FACTOR_ZIP       0x07
#define MEMORY_FORM_FACTOR_PROP_CARD 0x08
#define MEMORY_FORM_FACTOR_DIMM      0x09
#define MEMORY_FORM_FACTOR_TSOP      0x0A
#define MEMORY_FORM_FACTOR_ROW_CHIPS 0x0B
#define MEMORY_FORM_FACTOR_RIMM      0x0C
#define MEMORY_FORM_FACTOR_SODIMM    0x0D
#define MEMORY_FORM_FACTOR_SRIMM     0x0E
#define MEMORY_FORM_FACTOR_FB_DIMM   0x0F
#define MEMORY_FORM_FACTOR_DIE       0x10
#define MEMORY_FORM_FACTOR_CAMM      0x11

// ===== General Constants =====
#define SMBIOS_HEADER_SIZE         4
#define BIOS_ROM_SIZE_MULTIPLIER   64
#define MEMORY_SIZE_UNKNOWN        0xFFFF
#define MEMORY_SIZE_EXTENDED       0x7FFF

// ===== File Paths =====
#define SMBIOS_ENTRY "/sys/firmware/dmi/tables/smbios_entry_point"
#define DMI_TABLE    "/sys/firmware/dmi/tables/DMI"

// ===== Data Structures =====
typedef struct {
    uint8_t major;
    uint8_t minor;
    uint8_t docrev;  // Only valid for SMBIOS 3.x
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
    char *uuid;
} system_info_t;

typedef struct {
    char *manufacturer;
    char *product;
    char *version;
    char *serial_number;
    char *asset_tag;
} baseboard_info_t;

typedef struct {
    char *asset_tag;
    char *sku;
    uint8_t type;
    uint8_t state;
} chassis_info_t;

typedef struct {
    char *locator;
    char *bank_locator;
    char *manufacturer;
    char *serial_number;
    char *part_number;
    uint32_t size_mb;
    uint16_t speed_mhz;
    uint8_t memory_type;
    uint8_t form_factor;
    uint16_t total_width;
    uint16_t data_width;
    bool size_extended;
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
    uint16_t current_speed_mhz;
    uint8_t processor_type;
    uint8_t processor_family;
    uint16_t processor_family2;  // Extended family for values >= 0xFE
    uint16_t characteristics;
    uint8_t voltage;
    uint16_t external_clock_mhz;
    uint8_t status;
} processor_info_t;

// ===== Context Structure =====
typedef struct lazybios_ctx {
    uint8_t *dmi_data;
    size_t dmi_len;
    smbios_entry_info_t entry_info;

    bios_info_t bios_info;
    system_info_t system_info;
    baseboard_info_t baseboard_info;
    chassis_info_t chassis_info;
    processor_info_t processor_info;

    memory_device_t *memory_devices_ptr;
    size_t memory_devices_count;
} lazybios_ctx_t;

// ===== Public API =====
lazybios_ctx_t* lazybios_ctx_new(void);
void lazybios_ctx_free(lazybios_ctx_t* ctx);

int lazybios_init(lazybios_ctx_t* ctx);
void lazybios_cleanup(lazybios_ctx_t* ctx);

void lazybios_smbios_ver(const lazybios_ctx_t* ctx);
const smbios_entry_info_t* lazybios_get_entry_info(const lazybios_ctx_t* ctx);

bios_info_t* lazybios_get_bios_info(lazybios_ctx_t* ctx);
system_info_t* lazybios_get_system_info(lazybios_ctx_t* ctx);
baseboard_info_t* lazybios_get_baseboard_info(lazybios_ctx_t* ctx);
chassis_info_t* lazybios_get_chassis_info(lazybios_ctx_t* ctx);
processor_info_t* lazybios_get_processor_info(lazybios_ctx_t* ctx);
memory_device_t* lazybios_get_memory_devices(lazybios_ctx_t* ctx, size_t* count);

size_t lazybios_get_smbios_structure_min_length(const lazybios_ctx_t* ctx, uint8_t type);
bool lazybios_is_smbios_version_at_least(const lazybios_ctx_t* ctx, uint8_t major, uint8_t minor);
const char* lazybios_get_processor_family_string(uint8_t family);
const char* lazybios_get_memory_type_string(uint8_t type);
const char* lazybios_get_memory_form_factor_string(uint8_t form_factor);

#endif // LAZYBIOS_H

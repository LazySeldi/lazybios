#ifndef LAZYBIOS_H
#define LAZYBIOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// lazybios version
#define LAZYBIOS_VER "0.3.0"
#define LAZYBIOS_MAJOR 0
#define LAZYBIOS_MINOR 3
#define LAZYBIOS_PATCH 0

// Just a little recommended buffer size for the 3 argument decoder functions
#define LAZYBIOS_DECODER_BUF_SIZE 256

// SMBIOS offsets
#define SMBIOS3_ANCHOR "_SM3_"
#define SMBIOS3_MAJOR_OFFSET 0x07
#define SMBIOS3_MINOR_OFFSET 0x08
#define SMBIOS3_DOCREV_OFFSET 0x09
#define SMBIOS3_TABLE_LENGTH 0x0C
#define SMBIOS3_TABLE_ADDRESS 0x10

#define SMBIOS2_ANCHOR "_SM_"
#define SMBIOS2_DMI_ANCHOR "_DMI_"
#define SMBIOS2_MAJOR_OFFSET 0x06
#define SMBIOS2_MINOR_OFFSET 0x07
#define SMBIOS2_TABLE_LENGTH 0x16
#define SMBIOS2_TABLE_ADDRESS 0x18
#define SMBIOS2_N_STRUCTURES 0x1C

#define SMBIOS_TYPE_BIOS 0
#define SMBIOS_TYPE_SYSTEM 1
#define SMBIOS_TYPE_BASEBOARD 2
#define SMBIOS_TYPE_CHASSIS 3
#define SMBIOS_TYPE_PROCESSOR 4
#define SMBIOS_TYPE_CACHES 7
#define SMBIOS_TYPE_PORT_CONNECTOR 8
#define SMBIOS_TYPE_ONBOARD_DEVICES 10
#define SMBIOS_TYPE_OEM_STRINGS 11
#define SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY 16
#define SMBIOS_TYPE_MEMORY_DEVICE 17
#define SMBIOS_TYPE_END 127

// Values for not found
#define LAZYBIOS_NOT_FOUND_U8 0xFF
#define LAZYBIOS_NOT_FOUND_U16 0xFFFF
#define LAZYBIOS_NOT_FOUND_U32 0xFFFFFFFF
#define LAZYBIOS_NOT_FOUND_U64 0xFFFFFFFFFFFFFFFFULL
#define LAZYBIOS_NULL NULL // Why not lol
#define LAZYBIOS_NOT_FOUND_STR "Not Present"

// Helper macros
#define ISVERPLUS(DMIData, req_major, req_minor) (((DMIData)->entry_info.major > (req_major)) || ((DMIData)->entry_info.major == (req_major) && (DMIData)->entry_info.minor >= (req_minor))) // Returns 1 if the version is equal or newer and 0 if its older

#define READSTR(len, OFFSET, field, p, end) \
	if (len > OFFSET) field = DMIString(p, len, p[OFFSET], end); \
	if (!field) field = strdup(LAZYBIOS_NOT_FOUND_STR)

#define READU8(field, len, OFFSET, p) field = (len > OFFSET) ? p[OFFSET] : LAZYBIOS_NOT_FOUND_U8;

#define READU16(field, len, OFFSET, p) \
	if (len >= OFFSET + sizeof(uint16_t)) { \
		memcpy(&field, p + OFFSET, sizeof(uint16_t)); \
	} else {  \
		field = LAZYBIOS_NOT_FOUND_U16; \
	}

#define READU32(field, len, OFFSET, p) \
	if (len >= OFFSET + sizeof(uint32_t)) { \
		memcpy(&field, p + OFFSET, sizeof(uint32_t)); \
	} else {  \
		field = LAZYBIOS_NOT_FOUND_U32; \
	}

#define READU64(field, len, OFFSET, p) \
	if (len >= OFFSET + sizeof(uint64_t)) { \
		memcpy(&field, p + OFFSET, sizeof(uint64_t)); \
	} else {  \
		field = LAZYBIOS_NOT_FOUND_U64; \
	}

// ===== General Constants =====
#define SMBIOS_HEADER_SIZE 4
#define LINUX_SYSFS_SMBIOS_ENTRY "/sys/firmware/dmi/tables/smbios_entry_point"
#define LINUX_SYSFS_DMI_TABLE "/sys/firmware/dmi/tables/DMI"
#define DEV_MEM "/dev/mem"

// ===== Data Structures =====
typedef struct {
	uint8_t major;
	uint8_t minor;
	uint8_t docrev; // Only valid for SMBIOS 3.x
	uint32_t table_length;
	uint64_t table_address;
	uint16_t n_structures;
	bool is_64bit;
} smbios_entry_info_t;

typedef struct {
	// --- (SMBIOS 2.0+) ---
	char* vendor;
	char* version;
	char* release_date;
	uint16_t bios_starting_segment;
	uint32_t rom_size;
	uint64_t characteristics; // Decoder Available

	// --- SMBIOS 2.1+  ---
	size_t firmware_char_ext_bytes_count; // Needed to find out how many of firmware characteristics extension bytes are available.
	uint8_t* firmware_char_ext_bytes; // 2 Decoders Available

	// --- SMBIOS 2.4+   ---
	uint8_t platform_major_release;
	uint8_t platform_minor_release;
	uint8_t ec_major_release;
	uint8_t ec_minor_release;

	// --- SMBIOS 3.1+  ---
	uint16_t extended_rom_size; // Decoder Available
	char unit[5]; // Uint for Extended ROM Size
} lazybiosType0_t;

typedef struct {
	// --- (SMBIOS 2.0+) ---
	char* manufacturer;
	char* product_name;
	char* version;
	char* serial_number;

	// --- (SMBIOS 2.1+) ---
	uint8_t uuid[16]; // As per DMTF specs this field is 16 BYTEs.
	uint8_t wake_up_type; // The event that caused the system to wake up.

	// --- (SMBIOS 2.4+) ---
	char* sku_number;
	char* family;
} lazybiosType1_t;

typedef struct {
	// In this field I don't really see a "Version" column, I think this type has the same fields from SMBIOS 2.x to 3.x.x , I will still do some other research to make sure its true!
	char* manufacturer;
	char* product;
	char* version;
	char* serial_number;
	char* asset_tag;
	uint8_t feature_flags;
	char* location_in_chassis;
	uint16_t chassis_handle;
	uint8_t board_type;
	uint8_t number_of_contained_object_handles; // n
	uint16_t* contained_object_handles; // DMTF says this is n WORDs, so that means we allocate number_of_contained_object_handles of memory.
} lazybiosType2_t;

typedef struct {
	// --- (SMBIOS 2.0+) ---
	char* manufacturer;
	uint8_t type;
	char* version;
	char* serial_number;
	char* asset_tag;

	// --- (SMBIOS 2.1+) ---
	uint8_t boot_up_state;
	uint8_t power_supply_state;
	uint8_t thermal_state;
	uint8_t security_status;

	// --- (SMBIOS 2.3+) ---
	uint32_t oem_defined;
	uint8_t height;
	uint8_t number_of_power_cords;
	uint8_t contained_element_count; // n
	uint8_t contained_element_record_length; // m
	uint8_t* contained_elements; // n * m BYTEs

	// --- (SMBIOS 2.7+) ---
	char* sku_number; // offset is 0x15 + n * m

	// --- (SMBIOS 3.9+) ---
	uint8_t rack_type; // offset is 0x16 + n * m
	uint8_t rack_height; // offset is 0x17 + n * m
} lazybiosType3_t;

typedef struct {
	//--- (SMBIOS 2.0+) ---
	char* socket_designation;
	uint8_t processor_type;
	uint8_t processor_family;
	char* processor_manufacturer;
	uint64_t processor_id;
	char* processor_version;
	uint8_t voltage;
	uint16_t external_clock;
	uint16_t max_speed;
	uint16_t current_speed; // This is the speed at boot btw.
	uint8_t status;
	uint8_t processor_upgrade;

	// --- (SMBIOS 2.1+)---
	uint16_t l1_cache_handle; // These 3 are the cache handles not the actual cache information btw
	uint16_t l2_cache_handle;
	uint16_t l3_cache_handle;

	// --- (SMBIOS 2.3+) ---
	char* serial_number;
	char* asset_tag;
	char* part_number;

	// --- (SMBIOS 2.5+) ---
	uint8_t core_count; // if unknown(0) we set it to 0xFF, if its already 0xFF it means the core count is greater than 256, as the DMTF docs say.
	uint8_t core_enabled; // same for this field.
	uint8_t thread_count; // same for this too.
	uint16_t processor_characteristics;
	uint16_t processor_family_2; // used only if processor_family is 0xFE

	// --- (SMBIOS 3.0+) ---
	uint16_t core_count_2; // core_count is 0xFF we use this.
	uint16_t core_enabled_2; // same for this field.
	uint16_t thread_count_2; // same for this too.

	// --- (SMBIOS 3.6+) ---
	uint16_t thread_enabled;

	// --- (SMBIOS 3.8+) ---
	char* socket_type;
} lazybiosType4_t;

typedef struct {
	//--- (SMBIOS 2.1+) ---
	uint16_t physical_memory_array_handle;
	uint16_t memory_error_information_handle;
	uint16_t total_width; // total width in bits for this mem device.
	// If there are no error-correction bits, total_width value should be equal to Data Width. If the width is unknown, the field is set to FFFFh.
	uint16_t data_width; // same thing
	uint16_t size; //  size of memory deice
	uint8_t form_factor;
	uint8_t device_set;
	char* device_locator;
	char* bank_locator;
	uint8_t memory_type;
	uint16_t type_detail;

	//--- (SMBIOS 2.3+) ---
	uint16_t speed;
	char* manufacturer;
	char* serial_number;
	char* asset_tag;
	char* part_number;

	//--- (SMBIOS 2.6+) ---
	uint8_t attributes;

	//--- (SMBIOS 2.7+) ---
	uint32_t extended_size; // use this for devices with SMBIOS 2.7 instead of the uint16_t size
	uint16_t configured_memory_speed;

	//--- (SMBIOS 2.8+) ---
	uint16_t minimum_voltage;
	uint16_t maximum_voltage;
	uint16_t configured_voltage;

	//--- (SMBIOS 3.2+) ---
	uint8_t memory_technology;
	uint16_t memory_operating_mode_capability;
	char* firmware_version;
	uint16_t module_manufacturer_id;
	uint16_t module_product_id;
	uint16_t memory_subsystem_controller_manufacturer_id;
	uint16_t memory_subsystem_controller_product_id;
	uint64_t non_volatile_size;
	uint64_t volatile_size;
	uint64_t cache_size;
	uint64_t logical_size;

	//--- (SMBIOS 3.3+) ---
	uint32_t extended_speed;
	uint32_t extended_configured_memory_speed;

	//--- (SMBIOS 3.7+) ---
	uint16_t pmic0_manufacturer_id;
	uint16_t pmic0_revision_number;
	uint16_t rcd_manufacturer_id;
	uint16_t rcd_revision_number;
} lazybiosType17_t;

typedef enum { // I'm looking to implement more OSes but right now and for a long time I'm mostly going to focus on Linux.
	LAZYBIOS_BACKEND_LINUX, // Only Sysfs Currently but /dev/mem coming some day
	LAZYBIOS_BACKEND_WINDOWS, // Using Windows API
	LAZYBIOS_BACKEND_MACOS, // Not Implemented Yet
	LAZYBIOS_BACKEND_UNKNOWN,
} lazybiosBackend_t;

typedef struct {
	uint8_t* dmi_data;
	size_t dmi_len;
	uint8_t* entry_data;
	size_t entry_len;
	smbios_entry_info_t entry_info;
} lazybiosDMI_t;

typedef struct {
	lazybiosBackend_t backend;
	lazybiosDMI_t* DMIData;

	lazybiosType0_t* Type0;
	lazybiosType1_t* Type1;

	lazybiosType2_t* Type2;
	size_t type2_count;

	lazybiosType3_t* Type3;
	size_t type3_count;

	lazybiosType4_t* Type4;
	size_t type4_count;

	lazybiosType17_t* Type17;
	size_t type17_count;
} lazybiosCTX_t;

// ===== Public API =====
lazybiosCTX_t* lazybiosCTXNew(void);

int lazybiosInit(lazybiosCTX_t* ctx);
int lazybiosFile(lazybiosCTX_t* ctx, const char* entry_path, const char* dmi_path);
int lazybiosSingleFile(lazybiosCTX_t* ctx, const char* bin_path);
int lazybiosCleanup(lazybiosCTX_t* ctx);

// Core parsing functions
char* DMIString(const uint8_t* p, uint8_t length, uint8_t index, const uint8_t* end);
const uint8_t* DMINext(const uint8_t* ptr, const uint8_t* end);
size_t lazybiosCountStructsByType(const lazybiosDMI_t* DMIData, uint8_t target_type);
int lazybiosParseEntry(lazybiosCTX_t* ctx, const uint8_t* buf);

// Basic functions
void lazybiosPrintVer(const lazybiosCTX_t* ctx);

// Type 0 + Helpers
lazybiosType0_t* lazybiosGetType0(lazybiosType0_t* Type0, lazybiosDMI_t* DMIData);
void lazybiosType0CharacteristicsStr(uint64_t characteristics, char* buf, size_t buf_len);
void lazybiosType0CharacteristicsExtByte1Str(uint8_t char_ext_byte_1, char* buf, size_t buf_len);
void lazybiosType0CharacteristicsExtByte2Str(uint8_t char_ext_byte_2, char* buf, size_t buf_len);
uint16_t lazybiosType0ExtendedROMSizeU16(uint16_t raw, char unit[5]);
void lazybiosFreeType0(lazybiosType0_t* Type0);

// Type 1 + Helpers
lazybiosType1_t* lazybiosGetType1(lazybiosType1_t* Type1, lazybiosDMI_t* DMIData);
const char* lazybiosType1WakeupTypeStr(uint8_t wake_up_type);
void lazybiosFreeType1(lazybiosType1_t* Type1);

// Type 2 + Helpers
lazybiosType2_t* lazybiosGetType2(lazybiosType2_t* Type2, size_t* type2_count, lazybiosDMI_t* DMIData);
void lazybiosType2FeatureflagsStr(uint8_t feature_flags, char* buf, size_t buf_len);
const char* lazybiosType2BoardTypeStr(uint8_t board_type);
void lazybiosFreeType2(lazybiosType2_t* Type2, size_t type2_count);

// Type 3 + Helpers
lazybiosType3_t* lazybiosGetType3(lazybiosType3_t* Type3, size_t* type3_count, lazybiosDMI_t* DMIData);
void lazybiosType3TypeStr(uint8_t type, char* buf, size_t buf_len);
const char* lazybiosType3StateStr(uint8_t state);
const char* lazybiosType3SecurityStatusStr(uint8_t security_status);
void lazybiosType3ContainedElementTypeStr(uint8_t contained_elements, char* buf, size_t buf_len);
void lazybiosFreeType3(lazybiosType3_t* Type3, size_t type3_count);

// Type 4 + Helpers
lazybiosType4_t* lazybiosGetType4(lazybiosType4_t* Type4, size_t* type4_count, lazybiosDMI_t* DMIData);
const char* lazybiosType4ProcessorFamilyStr(uint16_t family);
const char* lazybiosType4SocketTypeStr(uint8_t type);
void lazybiosType4CharacteristicsStr(uint16_t characteristics, char* buf, size_t buf_len);
const char* lazybiosType4TypeStr(uint8_t type);
void lazybiosType4StatusStr(uint8_t status, char* buf, size_t buf_len);
void lazybiosType4VoltageStr(uint8_t voltage, char* buf, size_t buf_len);
void lazybiosFreeType4(lazybiosType4_t* Type4, size_t type4_count);

// Type 17 + Helpers
lazybiosType17_t* lazybiosGetType17(lazybiosType17_t* Type17, size_t* type17_count, lazybiosDMI_t* DMIData);
const char* lazybiosType17FormFactorStr(uint8_t form_factor);
const char* lazybiosType17TypeStr(uint8_t memory_type);
void lazybiosType17TypeDetailStr(uint16_t type_detail, char* buf, size_t buf_len);
void lazybiosType17ExtendedSizeStr(uint32_t extended_size, char* buf, size_t buf_len);
const char* lazybiosType17MemoryTechnologyStr(uint8_t memory_technology);
void lazybiosType17OperatingModeCapabilityStr(uint16_t memory_operating_mode_capability, char* buf, size_t buf_len);
void lazybiosType17ModuleManufacturerIDStr(uint16_t id, char* buf, size_t buf_len); // The can be used for these fields:
// Module Manufacturer ID
// Module Product ID
// Memory Subsystem Controller Manufacturer ID
// Memory Subsystem Controller Product ID
void lazybiosType17VolatileSizeStr(uint64_t volatile_size, char* buf, size_t buf_len);
void lazybiosType17NonVolatileSizeStr(uint64_t non_volatile_size, char* buf, size_t buf_len);
void lazybiosType17CacheSizeStr(uint64_t cache_size, char* buf, size_t buf_len);
void lazybiosType17ExtendedSpeedStr(uint32_t extended_speed, char* buf, size_t buf_len);
void lazybiosType17PMIC0ManufacturerIDStr(uint16_t id, char* buf, size_t buf_len);
void lazybiosType17PMIC0RevisionStr(uint16_t revision, char* buf, size_t buf_len);
void lazybiosType17RCDManufacturerIDStr(uint16_t id, char* buf, size_t buf_len);
void lazybiosType17RCDRevisionStr(uint16_t revision, char* buf, size_t buf_len);
void lazybiosFreeType17(lazybiosType17_t* Type17, size_t type17_count);

#ifdef __cplusplus
}
#endif

#endif

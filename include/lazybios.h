/**
 * @file lazybios.h
 * @brief Public API for reading and decoding SMBIOS/DMI data.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_H
/** @brief Include guard for the lazybios public API. */
#define LAZYBIOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/** @addtogroup api_constants
 * @{
 */

// lazybios version
/** @brief Complete lazybios semantic version string. */
#define LAZYBIOS_VER "0.4.0"
/** @brief Major component of the lazybios version. */
#define LAZYBIOS_MAJOR 0
/** @brief Minor component of the lazybios version. */
#define LAZYBIOS_MINOR 4
/** @brief Patch component of the lazybios version. */
#define LAZYBIOS_PATCH 0

// Just a little recommended buffer size for the 3 argument decoder functions
/** @brief Recommended output buffer size for decoder functions. */
#define LAZYBIOS_DECODER_BUF_SIZE 256

// SMBIOS offsets
// V3
/** @brief SMBIOS 3.x entry point anchor string. */
#define SMBIOS3_ANCHOR                 "_SM3_"
/** @brief Offset of the anchor in an SMBIOS 3.x entry point. */
#define SMBIOS3_ANCHOR_OFFSET          0x00
/** @brief Offset of the checksum in an SMBIOS 3.x entry point. */
#define SMBIOS3_CHECKSUM_OFFSET        0x05
/** @brief Offset of the entry point length in an SMBIOS 3.x entry point. */
#define SMBIOS3_LENGTH_OFFSET          0x06
/** @brief Offset of the major version in an SMBIOS 3.x entry point. */
#define SMBIOS3_MAJOR_OFFSET           0x07
/** @brief Offset of the minor version in an SMBIOS 3.x entry point. */
#define SMBIOS3_MINOR_OFFSET           0x08
/** @brief Offset of the document revision in an SMBIOS 3.x entry point. */
#define SMBIOS3_DOCREV_OFFSET          0x09
/** @brief Offset of the entry point revision in an SMBIOS 3.x entry point. */
#define SMBIOS3_REVISION_OFFSET        0x0A
/** @brief Offset of the reserved byte in an SMBIOS 3.x entry point. */
#define SMBIOS3_RESERVED_OFFSET        0x0B
/** @brief Offset of the maximum table size in an SMBIOS 3.x entry point. */
#define SMBIOS3_TABLE_MAX_SIZE_OFFSET  0x0C
/** @brief Offset of the table address in an SMBIOS 3.x entry point. */
#define SMBIOS3_TABLE_ADDRESS_OFFSET   0x10
/** @brief Required length of an SMBIOS 3.x entry point. */
#define SMBIOS3_ENTRY_POINT_LENGTH     0x18
/** @brief Length of the SMBIOS 3.x anchor string in bytes. */
#define SMBIOS3_ANCHOR_SIZE            5

// V2
/** @brief SMBIOS 2.x entry point anchor string. */
#define SMBIOS2_ANCHOR                     "_SM_"
/** @brief SMBIOS 2.x intermediate DMI anchor string. */
#define SMBIOS2_INTERMEDIATE_ANCHOR        "_DMI_"
/** @brief Offset of the anchor in an SMBIOS 2.x entry point. */
#define SMBIOS2_ANCHOR_OFFSET              0x00
/** @brief Offset of the checksum in an SMBIOS 2.x entry point. */
#define SMBIOS2_CHECKSUM_OFFSET            0x04
/** @brief Offset of the entry point length in an SMBIOS 2.x entry point. */
#define SMBIOS2_LENGTH_OFFSET              0x05
/** @brief Offset of the major version in an SMBIOS 2.x entry point. */
#define SMBIOS2_MAJOR_OFFSET               0x06
/** @brief Offset of the minor version in an SMBIOS 2.x entry point. */
#define SMBIOS2_MINOR_OFFSET               0x07
/** @brief Offset of the maximum structure size in an SMBIOS 2.x entry point. */
#define SMBIOS2_MAX_STRUCTURE_SIZE_OFFSET  0x08
/** @brief Offset of the entry point revision in an SMBIOS 2.x entry point. */
#define SMBIOS2_REVISION_OFFSET            0x0A
/** @brief Offset of the formatted area in an SMBIOS 2.x entry point. */
#define SMBIOS2_FORMATTED_AREA_OFFSET      0x0B
/** @brief Offset of the intermediate anchor in an SMBIOS 2.x entry point. */
#define SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET 0x10
/** @brief Offset of the intermediate checksum in an SMBIOS 2.x entry point. */
#define SMBIOS2_INTERMEDIATE_CHECKSUM_OFFSET 0x15
/** @brief Offset of the table length in an SMBIOS 2.x entry point. */
#define SMBIOS2_TABLE_LENGTH_OFFSET        0x16
/** @brief Offset of the table address in an SMBIOS 2.x entry point. */
#define SMBIOS2_TABLE_ADDRESS_OFFSET       0x18
/** @brief Offset of the structure count in an SMBIOS 2.x entry point. */
#define SMBIOS2_STRUCTURE_COUNT_OFFSET     0x1C
/** @brief Offset of the BCD revision in an SMBIOS 2.x entry point. */
#define SMBIOS2_BCD_REVISION_OFFSET        0x1E
/** @brief Standard length of an SMBIOS 2.x entry point. */
#define SMBIOS2_ENTRY_POINT_LENGTH         0x1F
/** @brief Length of an SMBIOS 2.1-compatible entry point. */
#define SMBIOS2_ENTRY_POINT_LENGTH_V21     0x1E
/** @brief Length of the SMBIOS 2.x anchor string in bytes. */
#define SMBIOS2_ANCHOR_SIZE                4
/** @brief Length of the SMBIOS 2.x intermediate anchor string in bytes. */
#define SMBIOS2_INTERMEDIATE_ANCHOR_SIZE   5
/** @brief Length of the SMBIOS 2.x formatted area in bytes. */
#define SMBIOS2_FORMATTED_AREA_SIZE        5

// Structures
/** @brief SMBIOS structure type identifier for BIOS Information. */
#define SMBIOS_TYPE_BIOS 0
/** @brief SMBIOS structure type identifier for System Information. */
#define SMBIOS_TYPE_SYSTEM 1
/** @brief SMBIOS structure type identifier for Baseboard Information. */
#define SMBIOS_TYPE_BASEBOARD 2
/** @brief SMBIOS structure type identifier for System Enclosure or Chassis. */
#define SMBIOS_TYPE_CHASSIS 3
/** @brief SMBIOS structure type identifier for Processor Information. */
#define SMBIOS_TYPE_PROCESSOR 4
/** @brief SMBIOS structure type identifier for Cache Information. */
#define SMBIOS_TYPE_CACHES 7
/** @brief SMBIOS structure type identifier for Port Connector Information. */
#define SMBIOS_TYPE_PORT_CONNECTOR 8
/** @brief SMBIOS structure type identifier for Onboard Devices Information. */
#define SMBIOS_TYPE_ONBOARD_DEVICES 10
/** @brief SMBIOS structure type identifier for OEM Strings. */
#define SMBIOS_TYPE_OEM_STRINGS 11
/** @brief SMBIOS structure type identifier for Physical Memory Array. */
#define SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY 16
/** @brief SMBIOS structure type identifier for Memory Device. */
#define SMBIOS_TYPE_MEMORY_DEVICE 17
/** @brief SMBIOS structure type identifier marking the end of the table. */
#define SMBIOS_TYPE_END 127

// Values for not found
/** @brief Sentinel for an unavailable 8-bit value. */
#define LAZYBIOS_NOT_FOUND_U8 0xFF
/** @brief Sentinel for an unavailable 16-bit value. */
#define LAZYBIOS_NOT_FOUND_U16 0xFFFF
/** @brief Sentinel for an unavailable 32-bit value. */
#define LAZYBIOS_NOT_FOUND_U32 0xFFFFFFFF
/** @brief Sentinel for an unavailable 64-bit value. */
#define LAZYBIOS_NOT_FOUND_U64 0xFFFFFFFFFFFFFFFFULL
/** @brief Null pointer value used by the lazybios API. */
#define LAZYBIOS_NULL NULL // Why not lol
/** @brief Fallback string for an unavailable SMBIOS string field. */
#define LAZYBIOS_NOT_FOUND_STR "Not Present"

// Helper macros
// Returns 1 if the version is equal or newer and 0 if its older
/** @brief Tests whether parsed SMBIOS data meets a required major and minor version. */
#define ISVERPLUS(DMIData, req_major, req_minor) ( \
((DMIData)->entry_tag == SMBIOS_VER_3X) ? \
(((DMIData)->entry_union.v3->major_version > (req_major)) || \
((DMIData)->entry_union.v3->major_version == (req_major) && (DMIData)->entry_union.v3->minor_version >= (req_minor))) \
: \
(((DMIData)->entry_union.v2->major_version > (req_major)) || \
((DMIData)->entry_union.v2->major_version == (req_major) && (DMIData)->entry_union.v2->minor_version >= (req_minor))) \
)


/** @brief Reads and allocates an SMBIOS string field or assigns the fallback string. */
#define READSTR(len, OFFSET, field, p, end) \
	if (len > OFFSET) field = DMIString(p, len, p[OFFSET], end); \
	if (!field) field = strdup(LAZYBIOS_NOT_FOUND_STR)

/** @brief Reads an 8-bit field or assigns the unavailable-value sentinel. */
#define READU8(field, len, OFFSET, p) field = (len > OFFSET) ? p[OFFSET] : LAZYBIOS_NOT_FOUND_U8;

/** @brief Reads a 16-bit field or assigns the unavailable-value sentinel. */
#define READU16(field, len, OFFSET, p) \
	if (len >= OFFSET + sizeof(uint16_t)) { \
		memcpy(&field, p + OFFSET, sizeof(uint16_t)); \
	} else {  \
		field = LAZYBIOS_NOT_FOUND_U16; \
	}

/** @brief Reads a 32-bit field or assigns the unavailable-value sentinel. */
#define READU32(field, len, OFFSET, p) \
	if (len >= OFFSET + sizeof(uint32_t)) { \
		memcpy(&field, p + OFFSET, sizeof(uint32_t)); \
	} else {  \
		field = LAZYBIOS_NOT_FOUND_U32; \
	}

/** @brief Reads a 64-bit field or assigns the unavailable-value sentinel. */
#define READU64(field, len, OFFSET, p) \
	if (len >= OFFSET + sizeof(uint64_t)) { \
		memcpy(&field, p + OFFSET, sizeof(uint64_t)); \
	} else {  \
		field = LAZYBIOS_NOT_FOUND_U64; \
	}

// ===== General Constants =====
/** @brief Size of the formatted header shared by SMBIOS structures. */
#define SMBIOS_HEADER_SIZE 4
/** @brief Linux sysfs path for the SMBIOS entry point. */
#define LINUX_SYSFS_SMBIOS_ENTRY "/sys/firmware/dmi/tables/smbios_entry_point"
/** @brief Linux sysfs path for the DMI structure table. */
#define LINUX_SYSFS_DMI_TABLE "/sys/firmware/dmi/tables/DMI"
/** @brief Linux device path used for physical-memory SMBIOS access. */
#define DEV_MEM "/dev/mem"

/** @} */

// ===== Data Structures =====
// ===== Raw Entry Point Structures =====
/**
 * @brief Raw SMBIOS 2.x entry point layout.
 * @ingroup api_entry
 */
typedef struct {
	uint8_t  anchor[4];                  /**< SMBIOS 2.x anchor bytes. */ // "_SM_"
	uint8_t  checksum;                   /**< Checksum for the entry point bytes. */
	uint8_t  entry_point_length;         /**< Length of the entry point in bytes. */
	uint8_t  major_version;              /**< SMBIOS major version. */
	uint8_t  minor_version;              /**< SMBIOS minor version. */
	uint16_t maximum_structure_size;     /**< Size of the largest SMBIOS structure. */
	uint8_t  entry_point_revision;       /**< Entry point format revision. */
	uint8_t  formatted_area[5];          /**< Revision-specific formatted bytes. */
	uint8_t  intermediate_anchor[5];     /**< Intermediate DMI anchor bytes. */ // "_DMI_"
	uint8_t  intermediate_checksum;      /**< Checksum for the intermediate entry point. */
	uint16_t structure_table_length;     /**< Length of the DMI structure table in bytes. */
	uint32_t structure_table_address;    /**< Physical address of the DMI structure table. */
	uint16_t structure_count;            /**< Number of SMBIOS structures in the table. */
	uint8_t  bcd_revision;               /**< BCD-encoded SMBIOS revision. */
} lazybiosSMBIOS2Entry;

/**
 * @brief Raw SMBIOS 3.x entry point layout.
 * @ingroup api_entry
 */
typedef struct {
	uint8_t  anchor[5];                  /**< SMBIOS 3.x anchor bytes. */ // "_SM3_"
	uint8_t  checksum;                   /**< Checksum for the entry point bytes. */
	uint8_t  entry_point_length;         /**< Length of the entry point in bytes. */
	uint8_t  major_version;              /**< SMBIOS major version. */
	uint8_t  minor_version;              /**< SMBIOS minor version. */
	uint8_t  docrev;                     /**< SMBIOS specification document revision. */
	uint8_t  entry_point_revision;       /**< Entry point format revision. */
	uint8_t  reserved;                   /**< Reserved byte. */
	uint32_t structure_table_max_size;   /**< Maximum DMI structure table size in bytes. */
	uint64_t structure_table_address;    /**< Physical address of the DMI structure table. */
} lazybiosSMBIOS3Entry;

// Version Tag
/**
 * @brief Identifies the SMBIOS entry point layout stored in a DMI container.
 * @ingroup api_entry
 */
typedef enum {
	SMBIOS_VER_UNKNOWN = 0, /**< No supported entry point has been parsed. */
	SMBIOS_VER_2X,          /**< The SMBIOS 2.x entry point member is active. */
	SMBIOS_VER_3X,          /**< The SMBIOS 3.x entry point member is active. */
	// SMBIOS_VER_4X, -- When it's released I guess
} lazybiosSMBIOSVersionTag;

/**
 * @brief Owns raw DMI table data and its parsed SMBIOS entry point.
 * @ingroup api_entry
 */
typedef struct {
	uint8_t* dmi_data;  /**< Owned raw DMI structure table bytes. */
	size_t dmi_len;     /**< Length of dmi_data in bytes. */
	uint8_t* entry_data; /**< Owned raw SMBIOS entry point bytes. */
	size_t entry_len;    /**< Length of entry_data in bytes. */

	// The tagged union (no normalized struct!)
	lazybiosSMBIOSVersionTag entry_tag; /**< Selects the valid entry_union member. */
	union {
		lazybiosSMBIOS2Entry* v2; /**< SMBIOS 2.x view into entry_data. */
		lazybiosSMBIOS3Entry* v3; /**< SMBIOS 3.x view into entry_data. */
	} entry_union; /**< Tagged view of the parsed entry point. */
} lazybiosDMI_t;

/**
 * @brief Parsed SMBIOS Type 0 BIOS Information.
 * @ingroup api_type0
 */
typedef struct {
	// --- (SMBIOS 2.0+) ---
	char* vendor;                       /**< BIOS vendor string. */
	char* version;                      /**< BIOS version string. */
	char* release_date;                 /**< BIOS release date string. */
	uint16_t bios_starting_segment;     /**< BIOS starting address segment. */
	uint32_t rom_size;                  /**< Decoded legacy ROM size in KiB. */
	uint64_t characteristics;           /**< BIOS characteristics bit field. */ // Decoder Available

	// --- SMBIOS 2.1+  ---
	size_t firmware_char_ext_bytes_count; /**< Number of available characteristics extension bytes. */ // Needed to find out how many of firmware characteristics extension bytes are available.
	uint8_t* firmware_char_ext_bytes;     /**< Owned array of characteristics extension bytes. */ // 2 Decoders Available

	// --- SMBIOS 2.4+   ---
	uint8_t platform_major_release;     /**< Platform firmware major release. */
	uint8_t platform_minor_release;     /**< Platform firmware minor release. */
	uint8_t ec_major_release;           /**< Embedded-controller firmware major release. */
	uint8_t ec_minor_release;           /**< Embedded-controller firmware minor release. */

	// --- SMBIOS 3.1+  ---
	uint16_t extended_rom_size;         /**< Raw extended ROM size field. */ // Decoder Available
	char unit[5];                       /**< Unit text written by the extended ROM size decoder. */ // Uint for Extended ROM Size
} lazybiosType0_t;

/**
 * @brief Parsed SMBIOS Type 1 System Information.
 * @ingroup api_type1
 */
typedef struct {
	// --- (SMBIOS 2.0+) ---
	char* manufacturer;                 /**< System manufacturer string. */
	char* product_name;                 /**< System product name string. */
	char* version;                      /**< System version string. */
	char* serial_number;                /**< System serial number string. */

	// --- (SMBIOS 2.1+) ---
	uint8_t uuid[16];                   /**< Raw 16-byte system UUID. */ // As per DMTF specs this field is 16 BYTEs.
	uint8_t wake_up_type;               /**< Event that caused the system to wake. */ // The event that caused the system to wake up.

	// --- (SMBIOS 2.4+) ---
	char* sku_number;                   /**< System SKU number string. */
	char* family;                       /**< System family string. */
} lazybiosType1_t;

/**
 * @brief Parsed SMBIOS Type 2 Baseboard Information.
 * @ingroup api_type2
 */
typedef struct {
	// In this field I don't really see a "Version" column, I think this type has the same fields from SMBIOS 2.x to 3.x.x , I will still do some other research to make sure its true!
	char* manufacturer;                 /**< Baseboard manufacturer string. */
	char* product;                      /**< Baseboard product string. */
	char* version;                      /**< Baseboard version string. */
	char* serial_number;                /**< Baseboard serial number string. */
	char* asset_tag;                    /**< Baseboard asset tag string. */
	uint8_t feature_flags;              /**< Baseboard feature flags. */
	char* location_in_chassis;          /**< Physical location within the chassis. */
	uint16_t chassis_handle;            /**< Handle of the associated Type 3 structure. */
	uint8_t board_type;                 /**< Baseboard type value. */
	uint8_t number_of_contained_object_handles; /**< Number of entries in contained_object_handles. */ // n
	uint16_t* contained_object_handles; /**< Owned array of contained SMBIOS object handles. */ // DMTF says this is n WORDs, so that means we allocate number_of_contained_object_handles of memory.
} lazybiosType2_t;

/**
 * @brief Parsed SMBIOS Type 3 System Enclosure or Chassis Information.
 * @ingroup api_type3
 */
typedef struct {
	// --- (SMBIOS 2.0+) ---
	char* manufacturer;                 /**< Chassis manufacturer string. */
	uint8_t type;                       /**< Chassis type with lock-presence bit. */
	char* version;                      /**< Chassis version string. */
	char* serial_number;                /**< Chassis serial number string. */
	char* asset_tag;                    /**< Chassis asset tag string. */

	// --- (SMBIOS 2.1+) ---
	uint8_t boot_up_state;              /**< Chassis boot-up state. */
	uint8_t power_supply_state;         /**< Chassis power-supply state. */
	uint8_t thermal_state;              /**< Chassis thermal state. */
	uint8_t security_status;            /**< Chassis security status. */

	// --- (SMBIOS 2.3+) ---
	uint32_t oem_defined;               /**< OEM-defined chassis data. */
	uint8_t height;                     /**< Chassis height in rack units, or zero when unspecified. */
	uint8_t number_of_power_cords;      /**< Number of chassis power cords. */
	uint8_t contained_element_count;    /**< Number of contained-element records. */ // n
	uint8_t contained_element_record_length; /**< Length of each contained-element record. */ // m
	uint8_t* contained_elements;        /**< Owned raw contained-element records. */ // n * m BYTEs

	// --- (SMBIOS 2.7+) ---
	char* sku_number;                    /**< Chassis SKU number string. */ // offset is 0x15 + n * m

	// --- (SMBIOS 3.9+) ---
	uint8_t rack_type;                  /**< Rack enclosure type. */ // offset is 0x16 + n * m
	uint8_t rack_height;                /**< Extended rack height. */ // offset is 0x17 + n * m
} lazybiosType3_t;

/**
 * @brief Parsed SMBIOS Type 4 Processor Information.
 * @ingroup api_type4
 */
typedef struct {
	//--- (SMBIOS 2.0+) ---
	char* socket_designation;           /**< Processor socket designation string. */
	uint8_t processor_type;             /**< Processor type value. */
	uint8_t processor_family;           /**< Base processor family value. */
	char* processor_manufacturer;       /**< Processor manufacturer string. */
	uint64_t processor_id;              /**< Raw processor identification value. */
	char* processor_version;            /**< Processor version string. */
	uint8_t voltage;                    /**< Current or supported processor voltage encoding. */
	uint16_t external_clock;            /**< External clock frequency in MHz. */
	uint16_t max_speed;                 /**< Maximum processor speed in MHz. */
	uint16_t current_speed;             /**< Processor speed at system boot in MHz. */ // This is the speed at boot btw.
	uint8_t status;                     /**< Socket population and processor status. */
	uint8_t processor_upgrade;          /**< Processor upgrade or socket type value. */

	// --- (SMBIOS 2.1+)---
	uint16_t l1_cache_handle;           /**< Handle of the associated L1 Type 7 structure. */ // These 3 are the cache handles not the actual cache information btw
	uint16_t l2_cache_handle;           /**< Handle of the associated L2 Type 7 structure. */
	uint16_t l3_cache_handle;           /**< Handle of the associated L3 Type 7 structure. */

	// --- (SMBIOS 2.3+) ---
	char* serial_number;                /**< Processor serial number string. */
	char* asset_tag;                    /**< Processor asset tag string. */
	char* part_number;                  /**< Processor part number string. */

	// --- (SMBIOS 2.5+) ---
	uint8_t core_count;                 /**< Base processor core count. */ // if unknown(0) we set it to 0xFF, if its already 0xFF it means the core count is greater than 256, as the DMTF docs say.
	uint8_t core_enabled;               /**< Base enabled-core count. */ // same for this field.
	uint8_t thread_count;               /**< Base processor thread count. */ // same for this too.
	uint16_t processor_characteristics; /**< Processor characteristics bit field. */
	uint16_t processor_family_2;        /**< Extended family used when processor_family is 0xFE. */ // used only if processor_family is 0xFE

	// --- (SMBIOS 3.0+) ---
	uint16_t core_count_2;              /**< Extended core count. */ // core_count is 0xFF we use this.
	uint16_t core_enabled_2;            /**< Extended enabled-core count. */ // same for this field.
	uint16_t thread_count_2;            /**< Extended thread count. */ // same for this too.

	// --- (SMBIOS 3.6+) ---
	uint16_t thread_enabled;            /**< Number of enabled processor threads. */

	// --- (SMBIOS 3.8+) ---
	char* socket_type;                  /**< Firmware-provided processor socket type string. */
} lazybiosType4_t;

/**
 * @brief Parsed SMBIOS Type 7 Cache Information.
 * @ingroup api_type7
 */
typedef struct {
	//--- (SMBIOS 2.0+) ---
	char* socket_designation;           /**< Cache socket designation string. */
	uint16_t cache_configuration;       /**< Cache configuration bit field. */
	uint16_t maximum_cache_size;        /**< Raw 16-bit maximum cache size field. */
	uint16_t installed_size;            /**< Raw 16-bit installed cache size field. */
	uint16_t supported_sram_type;       /**< Supported SRAM type bit field. */ // Same decoder with current_sram_type
	uint16_t current_sram_type;         /**< Current SRAM type bit field. */

	//--- (SMBIOS 2.1+) ---
	uint8_t cache_speed;                /**< Cache module speed in nanoseconds. */
	uint8_t error_correction_type;      /**< Cache error-correction type value. */
	uint8_t system_cache_type;          /**< System cache type value. */
	uint8_t associativity;              /**< Cache associativity value. */

	//--- (SMBIOS 3.1+) ---
	uint32_t maximum_cache_size_2;      /**< Raw extended maximum cache size field. */ // They make this stuff so complicated bro. The decoder is going to be hell.
	uint32_t installed_cache_size_2;    /**< Raw extended installed cache size field. */ // Same thing
} lazybiosType7_t;

/**
 * @brief Parsed SMBIOS Type 17 Memory Device Information.
 * @ingroup api_type17
 */
typedef struct {
	//--- (SMBIOS 2.1+) ---
	uint16_t physical_memory_array_handle; /**< Handle of the owning physical memory array. */
	uint16_t memory_error_information_handle; /**< Handle of the associated memory error information. */
	uint16_t total_width;                /**< Total device width in bits, including error correction. */ // total width in bits for this mem device.
	// If there are no error-correction bits, total_width value should be equal to Data Width. If the width is unknown, the field is set to FFFFh.
	uint16_t data_width;                 /**< Data width in bits, excluding error correction. */ // same thing
	uint16_t size;                       /**< Raw base memory-device size field. */ //  size of memory deice
	uint8_t form_factor;                 /**< Memory-device form factor value. */
	uint8_t device_set;                  /**< Device-set association value. */
	char* device_locator;                /**< Memory-device locator string. */
	char* bank_locator;                  /**< Memory-bank locator string. */
	uint8_t memory_type;                 /**< Memory type value. */
	uint16_t type_detail;                /**< Memory type-detail bit field. */

	//--- (SMBIOS 2.3+) ---
	uint16_t speed;                      /**< Base memory speed in MT/s. */
	char* manufacturer;                 /**< Memory-device manufacturer string. */
	char* serial_number;                /**< Memory-device serial number string. */
	char* asset_tag;                    /**< Memory-device asset tag string. */
	char* part_number;                  /**< Memory-device part number string. */

	//--- (SMBIOS 2.6+) ---
	uint8_t attributes;                 /**< Memory-device attributes, including rank count. */

	//--- (SMBIOS 2.7+) ---
	uint32_t extended_size;             /**< Raw extended memory-device size field. */ // use this for devices with SMBIOS 2.7 instead of the uint16_t size
	uint16_t configured_memory_speed;   /**< Base configured memory speed in MT/s. */

	//--- (SMBIOS 2.8+) ---
	uint16_t minimum_voltage;           /**< Minimum operating voltage in mV. */
	uint16_t maximum_voltage;           /**< Maximum operating voltage in mV. */
	uint16_t configured_voltage;        /**< Configured operating voltage in mV. */

	//--- (SMBIOS 3.2+) ---
	uint8_t memory_technology;           /**< Memory technology value. */
	uint16_t memory_operating_mode_capability; /**< Memory operating-mode capability bit field. */
	char* firmware_version;              /**< Memory-device firmware version string. */
	uint16_t module_manufacturer_id;     /**< Module manufacturer identifier. */
	uint16_t module_product_id;          /**< Module product identifier. */
	uint16_t memory_subsystem_controller_manufacturer_id; /**< Memory subsystem controller manufacturer identifier. */
	uint16_t memory_subsystem_controller_product_id; /**< Memory subsystem controller product identifier. */
	uint64_t non_volatile_size;          /**< Non-volatile capacity in bytes. */
	uint64_t volatile_size;              /**< Volatile capacity in bytes. */
	uint64_t cache_size;                 /**< Cache capacity in bytes. */
	uint64_t logical_size;               /**< Logical capacity in bytes. */

	//--- (SMBIOS 3.3+) ---
	uint32_t extended_speed;             /**< Extended memory speed in MT/s. */
	uint32_t extended_configured_memory_speed; /**< Extended configured memory speed in MT/s. */

	//--- (SMBIOS 3.7+) ---
	uint16_t pmic0_manufacturer_id;      /**< PMIC0 manufacturer identifier. */
	uint16_t pmic0_revision_number;      /**< PMIC0 revision number. */
	uint16_t rcd_manufacturer_id;        /**< Register clock driver manufacturer identifier. */
	uint16_t rcd_revision_number;        /**< Register clock driver revision number. */
} lazybiosType17_t;

/**
 * @brief Selects the platform backend used to obtain SMBIOS data.
 * @ingroup api_context
 */
typedef enum { // I'm looking to implement more OSes but right now and for a long time I'm mostly going to focus on Linux.
	LAZYBIOS_BACKEND_LINUX,   /**< Linux sysfs and physical-memory backend. */ // Only Sysfs Currently but /dev/mem coming some day
	LAZYBIOS_BACKEND_WINDOWS, /**< Windows firmware-table API backend. */ // Using Windows API
	LAZYBIOS_BACKEND_MACOS,   /**< Reserved macOS backend; not implemented. */ // Not Implemented Yet
	LAZYBIOS_BACKEND_UNKNOWN, /**< Unsupported or unidentified platform backend. */
} lazybiosBackend_t;

/**
 * @brief Top-level lazybios context containing raw and parsed SMBIOS data.
 * @ingroup api_context
 */
typedef struct {
	lazybiosBackend_t backend;           /**< Selected platform backend. */
	lazybiosDMI_t* DMIData;              /**< Owned raw DMI and entry-point container. */

	lazybiosType0_t* Type0;              /**< Owned parsed Type 0 structure. */
	lazybiosType1_t* Type1;              /**< Owned parsed Type 1 structure. */

	lazybiosType2_t* Type2;              /**< Owned parsed Type 2 array. */
	size_t type2_count;                  /**< Number of entries in Type2. */

	lazybiosType3_t* Type3;              /**< Owned parsed Type 3 array. */
	size_t type3_count;                  /**< Number of entries in Type3. */

	lazybiosType4_t* Type4;              /**< Owned parsed Type 4 array. */
	size_t type4_count;                  /**< Number of entries in Type4. */

	lazybiosType7_t* Type7;              /**< Owned parsed Type 7 array. */
	size_t type7_count;                  /**< Number of entries in Type7. */

	lazybiosType17_t* Type17;            /**< Owned parsed Type 17 array. */
	size_t type17_count;                 /**< Number of entries in Type17. */
} lazybiosCTX_t;

// ===== Public API =====
/** @addtogroup api_context
 * @{
 */

/**
 * @brief Allocates and initializes a lazybios context.
 *
 * The selected backend is derived from the target platform.
 *
 * @return Newly allocated context, or NULL if allocation fails.
 */
lazybiosCTX_t* lazybiosCTXNew(void);

/**
 * @brief Loads SMBIOS data using the context's selected platform backend.
 *
 * @param ctx Context that receives the raw entry point and DMI table data.
 * @return 0 on success, or -1 on failure.
 */
int lazybiosInit(lazybiosCTX_t* ctx);
/**
 * @brief Loads an SMBIOS entry point and DMI table from separate files.
 *
 * @param ctx Context that receives the loaded data.
 * @param entry_path Path to the raw SMBIOS entry point file.
 * @param dmi_path Path to the raw DMI structure table file.
 * @return 0 on success, or -1 on failure.
 */
int lazybiosFile(lazybiosCTX_t* ctx, const char* entry_path, const char* dmi_path);
/**
 * @brief Loads SMBIOS entry point and DMI data from one merged file.
 *
 * @param ctx Context that receives the loaded data.
 * @param bin_path Path to a file containing the entry point followed by the DMI table.
 * @return 0 on success, or -1 on failure.
 */
int lazybiosSingleFile(lazybiosCTX_t* ctx, const char* bin_path);
/**
 * @brief Releases a context and all SMBIOS data owned by it.
 *
 * @param ctx Context to release.
 * @return 0 on success, or -1 if ctx is NULL.
 */
int lazybiosCleanup(lazybiosCTX_t* ctx);

/** @} */

// Core parsing functions
/** @addtogroup api_parsing
 * @{
 */

/**
 * @brief Copies a string from an SMBIOS structure's string-set.
 *
 * @param p Start of the SMBIOS structure.
 * @param length Length of the structure's formatted section.
 * @param index One-based index of the requested string.
 * @param end One-past-the-end address of the DMI table buffer.
 * @return Newly allocated string, or NULL if the string is unavailable or invalid.
 */
char* DMIString(const uint8_t* p, uint8_t length, uint8_t index, const uint8_t* end);
/**
 * @brief Locates the next SMBIOS structure in a DMI table.
 *
 * @param p Start of the current SMBIOS structure.
 * @param end One-past-the-end address of the DMI table buffer.
 * @return Pointer to the next structure, or end when no complete structure remains.
 */
const uint8_t* DMINext(const uint8_t* p, const uint8_t* end);
/**
 * @brief Counts SMBIOS structures having a specified type identifier.
 *
 * @param DMIData Raw DMI table container to inspect.
 * @param target_type SMBIOS structure type identifier to count.
 * @return Number of matching structures in the table.
 */
size_t lazybiosCountStructsByType(const lazybiosDMI_t* DMIData, uint8_t target_type);
/**
 * @brief Validates and identifies an SMBIOS entry point.
 *
 * @param ctx Context whose entry tag and tagged union are updated.
 * @param entry_buf Buffer containing an SMBIOS 2.x or 3.x entry point.
 * @param buf_len Length of entry_buf in bytes.
 * @return 0 on success, or -1 if the entry point is invalid.
 */
int lazybiosParseEntry(lazybiosCTX_t* ctx, const uint8_t* entry_buf, size_t buf_len);
// Basic functions
/**
 * @brief Prints the parsed SMBIOS version to standard output.
 *
 * @param ctx Context containing a parsed SMBIOS entry point.
 */
void lazybiosPrintVer(const lazybiosCTX_t* ctx);

/** @} */

// Type 0 + Helpers
/** @addtogroup api_type0
 * @{
 */

/**
 * @brief Parses the first SMBIOS Type 0 BIOS Information structure.
 *
 * @param Type0 Existing Type 0 pointer value; it is not dereferenced or released.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 0 structure, or NULL on failure or absence.
 */
lazybiosType0_t* lazybiosGetType0(lazybiosType0_t* Type0, lazybiosDMI_t* DMIData);
/**
 * @brief Decodes BIOS characteristics into a readable string.
 *
 * @param characteristics SMBIOS BIOS characteristics bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType0CharacteristicsStr(uint64_t characteristics, char* buf, size_t buf_len);
/**
 * @brief Decodes BIOS characteristics extension byte 1.
 *
 * @param char_ext_byte_1 SMBIOS characteristics extension byte 1.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType0CharacteristicsExtByte1Str(uint8_t char_ext_byte_1, char* buf, size_t buf_len);
/**
 * @brief Decodes BIOS characteristics extension byte 2.
 *
 * @param char_ext_byte_2 SMBIOS characteristics extension byte 2.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType0CharacteristicsExtByte2Str(uint8_t char_ext_byte_2, char* buf, size_t buf_len);
/**
 * @brief Extracts the size and unit from an extended BIOS ROM size field.
 *
 * @param raw Raw SMBIOS extended ROM size value.
 * @param unit Output array that receives "MiB", "GiB", or "RES".
 * @return Size portion of the extended ROM size field.
 */
uint16_t lazybiosType0ExtendedROMSizeU16(uint16_t raw, char unit[5]);
/**
 * @brief Releases a parsed SMBIOS Type 0 structure.
 *
 * @param Type0 Type 0 structure to release.
 */
void lazybiosFreeType0(lazybiosType0_t* Type0);

/** @} */

// Type 1 + Helpers
/** @addtogroup api_type1
 * @{
 */

/**
 * @brief Parses the first SMBIOS Type 1 System Information structure.
 *
 * @param Type1 Existing Type 1 pointer value; it is not dereferenced or released.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 1 structure, or NULL on failure or absence.
 */
lazybiosType1_t* lazybiosGetType1(lazybiosType1_t* Type1, lazybiosDMI_t* DMIData);
/**
 * @brief Decodes an SMBIOS system wake-up type.
 *
 * @param wake_up_type Raw SMBIOS wake-up type value.
 * @return Static string describing the wake-up type.
 */
const char* lazybiosType1WakeupTypeStr(uint8_t wake_up_type);
/**
 * @brief Releases a parsed SMBIOS Type 1 structure.
 *
 * @param Type1 Type 1 structure to release.
 */
void lazybiosFreeType1(lazybiosType1_t* Type1);

/** @} */

// Type 2 + Helpers
/** @addtogroup api_type2
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 2 Baseboard Information structures.
 *
 * @param Type2 Existing Type 2 array pointer value; it is not dereferenced or released.
 * @param type2_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 2 array, or NULL on failure.
 */
lazybiosType2_t* lazybiosGetType2(lazybiosType2_t* Type2, size_t* type2_count, lazybiosDMI_t* DMIData);
/**
 * @brief Decodes SMBIOS baseboard feature flags into a readable string.
 *
 * @param feature_flags Raw SMBIOS baseboard feature flags.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType2FeatureflagsStr(uint8_t feature_flags, char* buf, size_t buf_len);
/**
 * @brief Decodes an SMBIOS baseboard type.
 *
 * @param board_type Raw SMBIOS baseboard type value.
 * @return Static string describing the baseboard type.
 */
const char* lazybiosType2BoardTypeStr(uint8_t board_type);
/**
 * @brief Releases an array of parsed SMBIOS Type 2 structures.
 *
 * @param Type2 Type 2 array to release.
 * @param type2_count Number of elements in Type2.
 */
void lazybiosFreeType2(lazybiosType2_t* Type2, size_t type2_count);

/** @} */

// Type 3 + Helpers
/** @addtogroup api_type3
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 3 Chassis Information structures.
 *
 * @param Type3 Existing Type 3 array pointer value; it is not dereferenced or released.
 * @param type3_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 3 array, or NULL on failure.
 */
lazybiosType3_t* lazybiosGetType3(lazybiosType3_t* Type3, size_t* type3_count, lazybiosDMI_t* DMIData);
/**
 * @brief Decodes an SMBIOS chassis type and lock indicator.
 *
 * @param type Raw SMBIOS chassis type byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType3TypeStr(uint8_t type, char* buf, size_t buf_len);
/**
 * @brief Decodes an SMBIOS chassis state value.
 *
 * @param state Raw boot-up, power-supply, or thermal state value.
 * @return Static string describing the chassis state.
 */
const char* lazybiosType3StateStr(uint8_t state);
/**
 * @brief Decodes an SMBIOS chassis security status.
 *
 * @param security_status Raw SMBIOS security status value.
 * @return Static string describing the security status.
 */
const char* lazybiosType3SecurityStatusStr(uint8_t security_status);
/**
 * @brief Decodes the type field of a chassis contained-element record.
 *
 * @param contained_elements Raw contained-element type byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType3ContainedElementTypeStr(uint8_t contained_elements, char* buf, size_t buf_len);
/**
 * @brief Releases an array of parsed SMBIOS Type 3 structures.
 *
 * @param Type3 Type 3 array to release.
 * @param type3_count Number of elements in Type3.
 */
void lazybiosFreeType3(lazybiosType3_t* Type3, size_t type3_count);

/** @} */

// Type 4 + Helpers
/** @addtogroup api_type4
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 4 Processor Information structures.
 *
 * @param Type4 Existing Type 4 array pointer value; it is not dereferenced or released.
 * @param type4_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 4 array, or NULL on failure.
 */
lazybiosType4_t* lazybiosGetType4(lazybiosType4_t* Type4, size_t* type4_count, lazybiosDMI_t* DMIData);
/**
 * @brief Decodes an SMBIOS processor family value.
 *
 * @param family Raw 8-bit or extended 16-bit processor family value.
 * @return Static string describing the processor family.
 */
const char* lazybiosType4ProcessorFamilyStr(uint16_t family);
/**
 * @brief Decodes an SMBIOS processor upgrade or socket type.
 *
 * @param type Raw SMBIOS processor upgrade value.
 * @return Static string describing the socket type.
 */
const char* lazybiosType4SocketTypeStr(uint8_t type);
/**
 * @brief Decodes SMBIOS processor characteristics into a readable string.
 *
 * @param characteristics Raw SMBIOS processor characteristics bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType4CharacteristicsStr(uint16_t characteristics, char* buf, size_t buf_len);
/**
 * @brief Decodes an SMBIOS processor type.
 *
 * @param type Raw SMBIOS processor type value.
 * @return Static string describing the processor type.
 */
const char* lazybiosType4TypeStr(uint8_t type);
/**
 * @brief Decodes an SMBIOS processor status byte.
 *
 * @param status Raw SMBIOS processor status value.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType4StatusStr(uint8_t status, char* buf, size_t buf_len);
/**
 * @brief Decodes an SMBIOS processor voltage byte.
 *
 * @param voltage Raw SMBIOS processor voltage value.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType4VoltageStr(uint8_t voltage, char* buf, size_t buf_len);
/**
 * @brief Releases an array of parsed SMBIOS Type 4 structures.
 *
 * @param Type4 Type 4 array to release.
 * @param type4_count Number of elements in Type4.
 */
void lazybiosFreeType4(lazybiosType4_t* Type4, size_t type4_count);

/** @} */

// Type 7 + Helpers
/** @addtogroup api_type7
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 7 Cache Information structures.
 *
 * @param Type7 Existing Type 7 array pointer value; it is not dereferenced or released.
 * @param type7_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 7 array, or NULL on failure.
 */
lazybiosType7_t* lazybiosGetType7(lazybiosType7_t* Type7, size_t* type7_count, lazybiosDMI_t* DMIData);
/**
 * @brief Converts a 16-bit SMBIOS cache size field to kibibytes.
 *
 * @param raw_size Raw 16-bit maximum or installed cache size value.
 * @return Decoded cache size in KiB.
 */
uint64_t lazybiosType7CacheU16(uint16_t raw_size);
/**
 * @brief Decodes SMBIOS SRAM type flags into a readable string.
 *
 * @param sram_type Raw supported or current SRAM type bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType7SRAMTypeStr(uint16_t sram_type, char* buf, size_t buf_len);
/**
 * @brief Decodes an SMBIOS cache error-correction type.
 *
 * @param ecc_type Raw SMBIOS error-correction type value.
 * @return Static string describing the error-correction type.
 */
const char* lazybiosType7ErrorCorrectionTypeStr(uint8_t ecc_type);
/**
 * @brief Decodes an SMBIOS system cache type.
 *
 * @param cache_type Raw SMBIOS system cache type value.
 * @return Static string describing the cache type.
 */
const char* lazybiosType7SystemCacheTypeStr(uint8_t cache_type);
/**
 * @brief Decodes an SMBIOS cache associativity value.
 *
 * @param associativity Raw SMBIOS cache associativity value.
 * @return Static string describing the associativity.
 */
const char* lazybiosType7AssociativityStr(uint8_t associativity);
/**
 * @brief Decodes an SMBIOS cache configuration bit field.
 *
 * @param config Raw SMBIOS cache configuration value.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType7CacheConfigurationStr(uint16_t config, char* buf, size_t buf_len);
/**
 * @brief Converts a 32-bit SMBIOS cache size field to kibibytes.
 *
 * @param raw_size Raw 32-bit maximum or installed cache size value.
 * @return Decoded cache size in KiB.
 */
uint64_t lazybiosType7CacheU32(uint32_t raw_size);
/**
 * @brief Releases an array of parsed SMBIOS Type 7 structures.
 *
 * @param Type7 Type 7 array to release.
 * @param type7_count Number of elements in Type7.
 */
void lazybiosFreeType7(lazybiosType7_t* Type7, size_t type7_count);

/** @} */

// Type 17 + Helpers
/** @addtogroup api_type17
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 17 Memory Device structures.
 *
 * @param Type17 Existing Type 17 array pointer value; it is not dereferenced or released.
 * @param type17_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 17 array, or NULL on failure.
 */
lazybiosType17_t* lazybiosGetType17(lazybiosType17_t* Type17, size_t* type17_count, lazybiosDMI_t* DMIData);
/**
 * @brief Decodes an SMBIOS memory-device form factor.
 *
 * @param form_factor Raw SMBIOS form-factor value.
 * @return Static string describing the form factor.
 */
const char* lazybiosType17FormFactorStr(uint8_t form_factor);
/**
 * @brief Decodes an SMBIOS memory type.
 *
 * @param memory_type Raw SMBIOS memory type value.
 * @return Static string describing the memory type.
 */
const char* lazybiosType17TypeStr(uint8_t memory_type);
/**
 * @brief Decodes SMBIOS memory type-detail flags.
 *
 * @param type_detail Raw SMBIOS memory type-detail bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17TypeDetailStr(uint16_t type_detail, char* buf, size_t buf_len);
/**
 * @brief Formats an SMBIOS extended memory-device size.
 *
 * @param extended_size Raw SMBIOS extended size value.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17ExtendedSizeStr(uint32_t extended_size, char* buf, size_t buf_len);
/**
 * @brief Decodes an SMBIOS memory technology value.
 *
 * @param memory_technology Raw SMBIOS memory technology value.
 * @return Static string describing the memory technology.
 */
const char* lazybiosType17MemoryTechnologyStr(uint8_t memory_technology);
/**
 * @brief Decodes memory operating-mode capability flags.
 *
 * @param memory_operating_mode_capability Raw SMBIOS capability bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17OperatingModeCapabilityStr(uint16_t memory_operating_mode_capability, char* buf, size_t buf_len);
/**
 * @brief Formats a memory module manufacturer or product identifier.
 *
 * @param id Raw SMBIOS identifier value.
 * @param buf Output buffer that receives the formatted identifier.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17ModuleManufacturerIDStr(uint16_t id, char* buf, size_t buf_len); // The can be used for these fields:
// Module Manufacturer ID
// Module Product ID
// Memory Subsystem Controller Manufacturer ID
// Memory Subsystem Controller Product ID
/**
 * @brief Formats the volatile capacity of a memory device.
 *
 * @param volatile_size Volatile capacity in bytes.
 * @param buf Output buffer that receives the formatted size.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17VolatileSizeStr(uint64_t volatile_size, char* buf, size_t buf_len);
/**
 * @brief Formats the non-volatile capacity of a memory device.
 *
 * @param non_volatile_size Non-volatile capacity in bytes.
 * @param buf Output buffer that receives the formatted size.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17NonVolatileSizeStr(uint64_t non_volatile_size, char* buf, size_t buf_len);
/**
 * @brief Formats the cache capacity of a memory device.
 *
 * @param cache_size Cache capacity in bytes.
 * @param buf Output buffer that receives the formatted size.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17CacheSizeStr(uint64_t cache_size, char* buf, size_t buf_len);
/**
 * @brief Formats an SMBIOS extended memory speed.
 *
 * @param extended_speed Raw extended speed value in MT/s.
 * @param buf Output buffer that receives the formatted speed.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17ExtendedSpeedStr(uint32_t extended_speed, char* buf, size_t buf_len);
/**
 * @brief Formats a PMIC0 manufacturer identifier.
 *
 * @param id Raw SMBIOS PMIC0 manufacturer identifier.
 * @param buf Output buffer that receives the formatted identifier.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17PMIC0ManufacturerIDStr(uint16_t id, char* buf, size_t buf_len);
/**
 * @brief Formats a PMIC0 revision number.
 *
 * @param revision Raw SMBIOS PMIC0 revision value.
 * @param buf Output buffer that receives the formatted revision.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17PMIC0RevisionStr(uint16_t revision, char* buf, size_t buf_len);
/**
 * @brief Formats a register clock driver manufacturer identifier.
 *
 * @param id Raw SMBIOS RCD manufacturer identifier.
 * @param buf Output buffer that receives the formatted identifier.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17RCDManufacturerIDStr(uint16_t id, char* buf, size_t buf_len);
/**
 * @brief Formats a register clock driver revision number.
 *
 * @param revision Raw SMBIOS RCD revision value.
 * @param buf Output buffer that receives the formatted revision.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17RCDRevisionStr(uint16_t revision, char* buf, size_t buf_len);
/**
 * @brief Releases an array of parsed SMBIOS Type 17 structures.
 *
 * @param Type17 Type 17 array to release.
 * @param type17_count Number of elements in Type17.
 */
void lazybiosFreeType17(lazybiosType17_t* Type17, size_t type17_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

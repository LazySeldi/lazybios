/**
 * @file type17.h
 * @brief Public API for SMBIOS Type 17 Memory Device Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE17_H
#define LAZYBIOS_TYPE17_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 17 fields. */
typedef struct {
	lazybiosFieldStatus_t physical_memory_array_handle;
	lazybiosFieldStatus_t memory_error_information_handle;
	lazybiosFieldStatus_t total_width;
	lazybiosFieldStatus_t data_width;
	lazybiosFieldStatus_t size;
	lazybiosFieldStatus_t form_factor;
	lazybiosFieldStatus_t device_set;
	lazybiosFieldStatus_t device_locator;
	lazybiosFieldStatus_t bank_locator;
	lazybiosFieldStatus_t memory_type;
	lazybiosFieldStatus_t type_detail;
	lazybiosFieldStatus_t speed;
	lazybiosFieldStatus_t manufacturer;
	lazybiosFieldStatus_t serial_number;
	lazybiosFieldStatus_t asset_tag;
	lazybiosFieldStatus_t part_number;
	lazybiosFieldStatus_t attributes;
	lazybiosFieldStatus_t extended_size;
	lazybiosFieldStatus_t configured_memory_speed;
	lazybiosFieldStatus_t minimum_voltage;
	lazybiosFieldStatus_t maximum_voltage;
	lazybiosFieldStatus_t configured_voltage;
	lazybiosFieldStatus_t memory_technology;
	lazybiosFieldStatus_t memory_operating_mode_capability;
	lazybiosFieldStatus_t firmware_version;
	lazybiosFieldStatus_t module_manufacturer_id;
	lazybiosFieldStatus_t module_product_id;
	lazybiosFieldStatus_t memory_subsystem_controller_manufacturer_id;
	lazybiosFieldStatus_t memory_subsystem_controller_product_id;
	lazybiosFieldStatus_t non_volatile_size;
	lazybiosFieldStatus_t volatile_size;
	lazybiosFieldStatus_t cache_size;
	lazybiosFieldStatus_t logical_size;
	lazybiosFieldStatus_t extended_speed;
	lazybiosFieldStatus_t extended_configured_memory_speed;
	lazybiosFieldStatus_t pmic0_manufacturer_id;
	lazybiosFieldStatus_t pmic0_revision_number;
	lazybiosFieldStatus_t rcd_manufacturer_id;
	lazybiosFieldStatus_t rcd_revision_number;
} lazybiosType17FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 17 Memory Device Information.
 * @ingroup api_type17
 */
typedef struct {
	uint16_t physical_memory_array_handle;
	uint16_t memory_error_information_handle;
	uint16_t total_width;
	uint16_t data_width;
	uint16_t size;
	uint8_t form_factor;
	uint8_t device_set;
	char* device_locator;
	char* bank_locator;
	uint8_t memory_type;
	uint16_t type_detail;
	uint16_t speed;
	char* manufacturer;
	char* serial_number;
	char* asset_tag;
	char* part_number;
	uint8_t attributes;
	uint32_t extended_size;
	uint16_t configured_memory_speed;
	uint16_t minimum_voltage;
	uint16_t maximum_voltage;
	uint16_t configured_voltage;
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
	uint32_t extended_speed;
	uint32_t extended_configured_memory_speed;
	uint16_t pmic0_manufacturer_id;
	uint16_t pmic0_revision_number;
	uint16_t rcd_manufacturer_id;
	uint16_t rcd_revision_number;
	lazybiosType17FieldStatus_t field_status;
} lazybiosType17_t;

/** @addtogroup api_type17
 * @{
 */

/** @brief Parses all Type 17 structures. */
lazybiosType17_t* lazybiosGetType17(lazybiosType17_t* Type17, size_t* type17_count, lazybiosDMI_t* DMIData);
/** @brief Decodes a memory-device form factor. */
const char* lazybiosType17FormFactorStr(uint8_t form_factor);
/** @brief Decodes a memory type. */
const char* lazybiosType17TypeStr(uint8_t memory_type);
/** @brief Decodes memory type-detail flags. */
void lazybiosType17TypeDetailStr(uint16_t type_detail, char* buf, size_t buf_len);
/** @brief Formats extended memory-device size. */
void lazybiosType17ExtendedSizeStr(uint32_t extended_size, char* buf, size_t buf_len);
/** @brief Decodes memory technology. */
const char* lazybiosType17MemoryTechnologyStr(uint8_t memory_technology);
/** @brief Decodes operating-mode capabilities. */
void lazybiosType17OperatingModeCapabilityStr(uint16_t memory_operating_mode_capability, char* buf, size_t buf_len);
/** @brief Formats a module identifier. */
void lazybiosType17ModuleManufacturerIDStr(uint16_t id, char* buf, size_t buf_len);
/** @brief Formats volatile capacity. */
void lazybiosType17VolatileSizeStr(uint64_t volatile_size, char* buf, size_t buf_len);
/** @brief Formats non-volatile capacity. */
void lazybiosType17NonVolatileSizeStr(uint64_t non_volatile_size, char* buf, size_t buf_len);
/** @brief Formats cache capacity. */
void lazybiosType17CacheSizeStr(uint64_t cache_size, char* buf, size_t buf_len);
/** @brief Formats extended memory speed. */
void lazybiosType17ExtendedSpeedStr(uint32_t extended_speed, char* buf, size_t buf_len);
/** @brief Formats a PMIC0 manufacturer identifier. */
void lazybiosType17PMIC0ManufacturerIDStr(uint16_t id, char* buf, size_t buf_len);
/** @brief Formats a PMIC0 revision. */
void lazybiosType17PMIC0RevisionStr(uint16_t revision, char* buf, size_t buf_len);
/** @brief Formats an RCD manufacturer identifier. */
void lazybiosType17RCDManufacturerIDStr(uint16_t id, char* buf, size_t buf_len);
/** @brief Formats an RCD revision. */
void lazybiosType17RCDRevisionStr(uint16_t revision, char* buf, size_t buf_len);
/** @brief Releases parsed Type 17 structures. */
void lazybiosFreeType17(lazybiosType17_t* Type17, size_t type17_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type17.h
 * @brief Public API for SMBIOS Type 17 Memory Device Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE17_H
#define LAZYBIOS_TYPE17_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
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
 * @brief Decoded forms of the Type 17 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* form_factor;
	const char* memory_technology;
	const char* memory_type;
	char* type_detail;
	char* extended_size;
	char* memory_operating_mode_capability;
	char* module_manufacturer_id;
	char* non_volatile_size;
	char* volatile_size;
	char* cache_size;
	char* extended_speed;
	char* pmic0_manufacturer_id;
	char* pmic0_revision_number;
	char* rcd_manufacturer_id;
	char* rcd_revision_number;
} lazybiosType17Decoded_t;

/**
 * @brief Parsed SMBIOS Type 17 Memory Device Information.
 * @ingroup api_type17
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint16_t physical_memory_array_handle;
	uint16_t memory_error_information_handle;
	uint16_t total_width;
	uint16_t data_width;
	uint16_t size;
	uint8_t form_factor;
	uint8_t device_set;
	const char* device_locator;
	const char* bank_locator;
	uint8_t memory_type;
	uint16_t type_detail;
	uint16_t speed;
	const char* manufacturer;
	const char* serial_number;
	const char* asset_tag;
	const char* part_number;
	uint8_t attributes;
	uint32_t extended_size;
	uint16_t configured_memory_speed;
	uint16_t minimum_voltage;
	uint16_t maximum_voltage;
	uint16_t configured_voltage;
	uint8_t memory_technology;
	uint16_t memory_operating_mode_capability;
	const char* firmware_version;
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
	lazybiosType17Decoded_t decoded;
	lazybiosType17FieldStatus_t field_status;
} lazybiosType17_t;

/**
 * @brief A parsed set of SMBIOS Type 17 structures.
 * @ingroup api_type17
 */
typedef struct {
	lazybiosType17_t* entries;
	size_t count;
} lazybiosType17Array_t;

/** @addtogroup api_type17
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 17 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 17 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType17Array_t* lazybiosGetType17(const lazybiosDMI_t* DMIData);
/**
 * @brief Releases a parsed set of SMBIOS Type 17 structures.
 * @param Type17 Set to release; may be NULL.
 */
void lazybiosFreeType17(lazybiosType17Array_t* Type17);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

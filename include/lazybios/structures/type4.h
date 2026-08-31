/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type4.h
 * @brief Public API for SMBIOS Type 4 Processor Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE4_H
#define LAZYBIOS_TYPE4_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 4 fields. */
typedef struct {
	lazybiosFieldStatus_t socket_designation;
	lazybiosFieldStatus_t processor_type;
	lazybiosFieldStatus_t processor_family;
	lazybiosFieldStatus_t processor_manufacturer;
	lazybiosFieldStatus_t processor_id;
	lazybiosFieldStatus_t processor_version;
	lazybiosFieldStatus_t voltage;
	lazybiosFieldStatus_t external_clock;
	lazybiosFieldStatus_t max_speed;
	lazybiosFieldStatus_t current_speed;
	lazybiosFieldStatus_t status;
	lazybiosFieldStatus_t processor_upgrade;
	lazybiosFieldStatus_t l1_cache_handle;
	lazybiosFieldStatus_t l2_cache_handle;
	lazybiosFieldStatus_t l3_cache_handle;
	lazybiosFieldStatus_t serial_number;
	lazybiosFieldStatus_t asset_tag;
	lazybiosFieldStatus_t part_number;
	lazybiosFieldStatus_t core_count;
	lazybiosFieldStatus_t core_enabled;
	lazybiosFieldStatus_t thread_count;
	lazybiosFieldStatus_t processor_characteristics;
	lazybiosFieldStatus_t processor_family_2;
	lazybiosFieldStatus_t core_count_2;
	lazybiosFieldStatus_t core_enabled_2;
	lazybiosFieldStatus_t thread_count_2;
	lazybiosFieldStatus_t thread_enabled;
	lazybiosFieldStatus_t socket_type;
} lazybiosType4FieldStatus_t;

/**
 * @brief Decoded forms of the Type 4 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* processor_family;
	const char* processor_family_2;
	const char* processor_type;
	const char* processor_upgrade;
	char* voltage;
	char* status;
	char* processor_characteristics;
} lazybiosType4Decoded_t;

/**
 * @brief Parsed SMBIOS Type 4 Processor Information.
 * @ingroup api_type4
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* socket_designation;
	uint8_t processor_type;
	uint8_t processor_family;
	const char* processor_manufacturer;
	uint64_t processor_id;
	const char* processor_version;
	uint8_t voltage;
	uint16_t external_clock;
	uint16_t max_speed;
	uint16_t current_speed;
	uint8_t status;
	uint8_t processor_upgrade;
	uint16_t l1_cache_handle;
	uint16_t l2_cache_handle;
	uint16_t l3_cache_handle;
	const char* serial_number;
	const char* asset_tag;
	const char* part_number;
	uint8_t core_count;
	uint8_t core_enabled;
	uint8_t thread_count;
	uint16_t processor_characteristics;
	uint16_t processor_family_2;
	uint16_t core_count_2;
	uint16_t core_enabled_2;
	uint16_t thread_count_2;
	uint16_t thread_enabled;
	const char* socket_type;
	lazybiosType4Decoded_t decoded;
	lazybiosType4FieldStatus_t field_status;
} lazybiosType4_t;

/**
 * @brief A parsed set of SMBIOS Type 4 structures.
 * @ingroup api_type4
 */
typedef struct {
	lazybiosType4_t* entries;
	size_t count;
} lazybiosType4Array_t;

/** @addtogroup api_type4
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 4 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 4 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType4Array_t* lazybiosGetType4(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 4 structures.
 * @param Type4 Set to release; may be NULL.
 */
void lazybiosFreeType4(lazybiosType4Array_t* Type4);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

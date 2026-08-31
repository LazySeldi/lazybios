/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type9.h
 * @brief Public API for SMBIOS Type 9 System Slots.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE9_H
#define LAZYBIOS_TYPE9_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for a Type 9 peer group. */
typedef struct {
	lazybiosFieldStatus_t segment_group_number;
	lazybiosFieldStatus_t bus_number;
	lazybiosFieldStatus_t device_function_number;
	lazybiosFieldStatus_t data_bus_width;
} lazybiosType9PeerGroupFieldStatus_t;

/**
 * @brief Decoded forms of the encoded fields in one peer group.
 */
typedef struct {
	const char* data_bus_width;
	char* device_function_number;
} lazybiosType9PeerGroupDecoded_t;

/**
 * @brief Parsed SMBIOS Type 9 peer grouping entry.
 * @ingroup api_type9
 */
typedef struct {
	uint16_t segment_group_number;
	uint8_t bus_number;
	uint8_t device_function_number;
	uint8_t data_bus_width;
	lazybiosType9PeerGroupFieldStatus_t field_status;
	lazybiosType9PeerGroupDecoded_t decoded;
} lazybiosType9PeerGroup_t;

/** @brief Availability metadata for SMBIOS Type 9 fields. */
typedef struct {
	lazybiosFieldStatus_t slot_designation;
	lazybiosFieldStatus_t slot_type;
	lazybiosFieldStatus_t slot_data_bus_width;
	lazybiosFieldStatus_t current_usage;
	lazybiosFieldStatus_t slot_length;
	lazybiosFieldStatus_t slot_id;
	lazybiosFieldStatus_t slot_characteristics_1;
	lazybiosFieldStatus_t slot_characteristics_2;
	lazybiosFieldStatus_t segment_group_number;
	lazybiosFieldStatus_t bus_number;
	lazybiosFieldStatus_t device_function_number;
	lazybiosFieldStatus_t data_bus_width;
	lazybiosFieldStatus_t peer_grouping_count;
	lazybiosFieldStatus_t peer_groups;
	lazybiosFieldStatus_t slot_information;
	lazybiosFieldStatus_t slot_physical_width;
	lazybiosFieldStatus_t slot_pitch;
	lazybiosFieldStatus_t slot_height;
} lazybiosType9FieldStatus_t;

/**
 * @brief Decoded forms of the Type 9 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* current_usage;
	const char* data_bus_width;
	const char* slot_data_bus_width;
	const char* slot_height;
	const char* slot_length;
	const char* slot_physical_width;
	const char* slot_type;
	char* slot_characteristics_1;
	char* slot_characteristics_2;
	char* device_function_number;
} lazybiosType9Decoded_t;

/**
 * @brief Parsed SMBIOS Type 9 System Slots Information.
 * @ingroup api_type9
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* slot_designation;
	uint8_t slot_type;
	uint8_t slot_data_bus_width;
	uint8_t current_usage;
	uint8_t slot_length;
	uint16_t slot_id;
	uint8_t slot_characteristics_1;
	uint8_t slot_characteristics_2;
	uint16_t segment_group_number;
	uint8_t bus_number;
	uint8_t device_function_number;
	uint8_t data_bus_width;
	uint8_t peer_grouping_count;
	lazybiosType9PeerGroup_t* peer_groups;
	uint8_t slot_information;
	uint8_t slot_physical_width;
	uint16_t slot_pitch;
	uint8_t slot_height;
	lazybiosType9Decoded_t decoded;
	lazybiosType9FieldStatus_t field_status;
} lazybiosType9_t;

/**
 * @brief A parsed set of SMBIOS Type 9 structures.
 * @ingroup api_type9
 */
typedef struct {
	lazybiosType9_t* entries;
	size_t count;
} lazybiosType9Array_t;

/** @addtogroup api_type9
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 9 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 9 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType9Array_t* lazybiosGetType9(const lazybiosDMI_t* DMIData);
/**
 * @brief Releases a parsed set of SMBIOS Type 9 structures.
 * @param Type9 Set to release; may be NULL.
 */
void lazybiosFreeType9(lazybiosType9Array_t* Type9);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This file is part of lazybios.
 *
 * lazybios is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * lazybios is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lazybios. If not, see <https://www.gnu.org/licenses/>.
 */
/**
 * @file type15.h
 * @brief Public API for SMBIOS Type 15 System Event Log Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE15_H
#define LAZYBIOS_TYPE15_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for an SMBIOS Type 15 event-log descriptor. */
typedef struct {
	lazybiosFieldStatus_t log_type;
	lazybiosFieldStatus_t variable_data_format_type;
} lazybiosType15LogTypeDescriptorFieldStatus_t;

/**
 * @brief Decoded forms of the encoded fields in one record.
 */
typedef struct {
	const char* log_type;
	const char* variable_data_format_type;
} lazybiosType15LogTypeDescriptorDecoded_t;

/**
 * @brief Parsed SMBIOS Type 15 supported event-log type descriptor.
 * @ingroup api_type15
 */
typedef struct {
	uint8_t log_type;
	uint8_t variable_data_format_type;
	lazybiosType15LogTypeDescriptorFieldStatus_t field_status;
	lazybiosType15LogTypeDescriptorDecoded_t decoded;
} lazybiosType15LogTypeDescriptor_t;

/** @brief Availability metadata for SMBIOS Type 15 fields. */
typedef struct {
	lazybiosFieldStatus_t log_area_length;
	lazybiosFieldStatus_t log_header_start_offset;
	lazybiosFieldStatus_t log_data_start_offset;
	lazybiosFieldStatus_t access_method;
	lazybiosFieldStatus_t log_status;
	lazybiosFieldStatus_t log_change_token;
	lazybiosFieldStatus_t access_method_address;
	lazybiosFieldStatus_t log_header_format;
	lazybiosFieldStatus_t number_of_supported_log_type_descriptors;
	lazybiosFieldStatus_t length_of_each_log_type_descriptor;
	lazybiosFieldStatus_t supported_log_type_descriptors;
} lazybiosType15FieldStatus_t;

/**
 * @brief Decoded forms of the Type 15 encoded fields.
 */
typedef struct {
	const char* access_method;
	uint16_t index_address;
	uint16_t data_address;
	uint16_t gpnv_handle;
	const char* log_header_format;
	char* log_status;
} lazybiosType15Decoded_t;

/**
 * @brief Parsed SMBIOS Type 15 System Event Log Information.
 * @ingroup api_type15
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint16_t log_area_length;
	uint16_t log_header_start_offset;
	uint16_t log_data_start_offset;
	uint8_t access_method;
	uint8_t log_status;
	uint32_t log_change_token;
	uint32_t access_method_address;
	uint8_t log_header_format;
	uint8_t number_of_supported_log_type_descriptors;
	uint8_t length_of_each_log_type_descriptor;
	lazybiosType15LogTypeDescriptor_t* supported_log_type_descriptors;
	lazybiosType15Decoded_t decoded;
	lazybiosType15FieldStatus_t field_status;
} lazybiosType15_t;

/**
 * @brief A parsed set of SMBIOS Type 15 structures.
 * @ingroup api_type15
 */
typedef struct {
	lazybiosType15_t* entries;
	size_t count;
} lazybiosType15Array_t;

/** @addtogroup api_type15
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 15 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 15 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType15Array_t* lazybiosGetType15(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 15 structures.
 * @param Type15 Set to release; may be NULL.
 */
void lazybiosFreeType15(lazybiosType15Array_t* Type15);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

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
 * @file type2.h
 * @brief Public API for SMBIOS Type 2 Baseboard Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE2_H
#define LAZYBIOS_TYPE2_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 2 fields. */
typedef struct {
	lazybiosFieldStatus_t manufacturer;
	lazybiosFieldStatus_t product;
	lazybiosFieldStatus_t version;
	lazybiosFieldStatus_t serial_number;
	lazybiosFieldStatus_t asset_tag;
	lazybiosFieldStatus_t feature_flags;
	lazybiosFieldStatus_t location_in_chassis;
	lazybiosFieldStatus_t chassis_handle;
	lazybiosFieldStatus_t board_type;
	lazybiosFieldStatus_t number_of_contained_object_handles;
	lazybiosFieldStatus_t contained_object_handles;
} lazybiosType2FieldStatus_t;

/**
 * @brief Decoded forms of the Type 2 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* board_type;
	char* feature_flags;
} lazybiosType2Decoded_t;

/**
 * @brief Parsed SMBIOS Type 2 Baseboard Information.
 * @ingroup api_type2
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* manufacturer;
	const char* product;
	const char* version;
	const char* serial_number;
	const char* asset_tag;
	uint8_t feature_flags;
	const char* location_in_chassis;
	uint16_t chassis_handle;
	uint8_t board_type;
	uint8_t number_of_contained_object_handles;
	uint16_t* contained_object_handles;
	lazybiosType2Decoded_t decoded;
	lazybiosType2FieldStatus_t field_status;
} lazybiosType2_t;

/**
 * @brief A parsed set of SMBIOS Type 2 structures.
 * @ingroup api_type2
 */
typedef struct {
	lazybiosType2_t* entries;
	size_t count;
} lazybiosType2Array_t;

/** @addtogroup api_type2
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 2 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 2 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType2Array_t* lazybiosGetType2(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 2 structures.
 * @param Type2 Set to release; may be NULL.
 */
void lazybiosFreeType2(lazybiosType2Array_t* Type2);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

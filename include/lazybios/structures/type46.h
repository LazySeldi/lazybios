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
 * @file type46.h
 * @brief Public API for SMBIOS Type 46 String Property.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE46_H
#define LAZYBIOS_TYPE46_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 46 fields. */
typedef struct {
	lazybiosFieldStatus_t string_property_id;
	lazybiosFieldStatus_t string_property_value;
	lazybiosFieldStatus_t parent_handle;
} lazybiosType46FieldStatus_t;

/**
 * @brief Decoded forms of the Type 46 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* string_property_id;
} lazybiosType46Decoded_t;

/**
 * @brief Parsed SMBIOS Type 46 String Property.
 * @ingroup api_type46
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint16_t string_property_id;
	const char* string_property_value;
	uint16_t parent_handle;
	lazybiosType46Decoded_t decoded;
	lazybiosType46FieldStatus_t field_status;
} lazybiosType46_t;

/**
 * @brief A parsed set of SMBIOS Type 46 structures.
 * @ingroup api_type46
 */
typedef struct {
	lazybiosType46_t* entries;
	size_t count;
} lazybiosType46Array_t;

/** @addtogroup api_type46
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 46 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 46 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType46Array_t* lazybiosGetType46(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 46 structures.
 * @param Type46 Set to release; may be NULL.
 */
void lazybiosFreeType46(lazybiosType46Array_t* Type46);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

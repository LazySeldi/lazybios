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
 * @file type1.h
 * @brief Public API for SMBIOS Type 1 System Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE1_H
#define LAZYBIOS_TYPE1_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 1 fields. */
typedef struct {
	lazybiosFieldStatus_t manufacturer;
	lazybiosFieldStatus_t product_name;
	lazybiosFieldStatus_t version;
	lazybiosFieldStatus_t serial_number;
	lazybiosFieldStatus_t uuid;
	lazybiosFieldStatus_t wake_up_type;
	lazybiosFieldStatus_t sku_number;
	lazybiosFieldStatus_t family;
} lazybiosType1FieldStatus_t;

/**
 * @brief Decoded forms of the Type 1 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* wake_up_type;
} lazybiosType1Decoded_t;

/**
 * @brief Parsed SMBIOS Type 1 System Information.
 * @ingroup api_type1
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* manufacturer;
	const char* product_name;
	const char* version;
	const char* serial_number;
	uint8_t uuid[16];
	uint8_t wake_up_type;
	const char* sku_number;
	const char* family;
	lazybiosType1Decoded_t decoded;
	lazybiosType1FieldStatus_t field_status;
} lazybiosType1_t;

/**
 * @brief A parsed set of SMBIOS Type 1 structures.
 * @ingroup api_type1
 */
typedef struct {
	lazybiosType1_t* entries;
	size_t count;
} lazybiosType1Array_t;

/** @addtogroup api_type1
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 1 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 1 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType1Array_t* lazybiosGetType1(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 1 structures.
 * @param Type1 Set to release; may be NULL.
 */
void lazybiosFreeType1(lazybiosType1Array_t* Type1);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

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
#include "../lazybios.h"
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
 * @brief Parsed SMBIOS Type 1 System Information.
 * @ingroup api_type1
 */
typedef struct {
	char* manufacturer;
	char* product_name;
	char* version;
	char* serial_number;
	uint8_t uuid[16];
	uint8_t wake_up_type;
	char* sku_number;
	char* family;
	lazybiosType1FieldStatus_t field_status;
} lazybiosType1_t;

/** @addtogroup api_type1
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 1 System Information structures.
 * @param Type1 Existing Type 1 array pointer value; it is not dereferenced or released.
 * @param type1_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 1 array, or NULL on failure.
 */
lazybiosType1_t* lazybiosGetType1(lazybiosType1_t* Type1, size_t* type1_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an SMBIOS system wake-up type.
 * @param wake_up_type Raw SMBIOS wake-up type value.
 * @return Static string describing the wake-up type.
 */
const char* lazybiosType1WakeupTypeStr(uint8_t wake_up_type);

/**
 * @brief Releases an array of parsed SMBIOS Type 1 structures.
 * @param Type1 Type 1 array to release.
 * @param type1_count Number of elements in Type1.
 */
void lazybiosFreeType1(lazybiosType1_t* Type1, size_t type1_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

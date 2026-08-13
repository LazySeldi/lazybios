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
 * @file hp_type201.h
 * @brief Public API for HP OEM SMBIOS Type 201 Information.
 * @author LazySeldi
 *
 * @note Experimental: This HP OEM type is newly added and has not been tested
 * with real hardware dumps.
 * @warning Untested.
 */

#ifndef LAZYBIOS_OEM_HP_TYPE201_H
#define LAZYBIOS_OEM_HP_TYPE201_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for HP OEM SMBIOS Type 201 fields. */
typedef struct {
	lazybiosFieldStatus_t rack_name;
} lazybiosOemHpType201FieldStatus_t;

/**
 * @brief Parsed HP OEM SMBIOS Type 201 Information.
 * @ingroup api_hp_type201
 *
 * @note Experimental and untested.
 */
typedef struct {
	const char* rack_name;
	lazybiosOemHpType201FieldStatus_t field_status;
} lazybiosOemHpType201_t;

/** @addtogroup api_hp_type201
 * @{
 */

/**
 * @brief Parses all HP OEM SMBIOS Type 201 Information structures.
 * @param HPType201 Existing HP Type 201 array pointer value.
 * @param hp_type201_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated HP Type 201 array, or NULL on failure.
 */
lazybiosOemHpType201_t* lazybiosGetOemHpType201(lazybiosOemHpType201_t* HPType201, size_t* hp_type201_count, lazybiosDMI_t* DMIData);

/**
 * @brief Releases an array of parsed HP OEM SMBIOS Type 201 structures.
 * @param HPType201 HP Type 201 array to release.
 * @param hp_type201_count Number of elements in HPType201.
 */
void lazybiosFreeOemHpType201(lazybiosOemHpType201_t* HPType201, size_t hp_type201_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

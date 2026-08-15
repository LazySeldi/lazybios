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
 * @file hp_type204.h
 * @brief Public API for HP OEM SMBIOS Type 204 Information.
 * @author LazySeldi
 *
 * @note Experimental: This HP OEM type is newly added and has not been tested
 * with real hardware dumps.
 * @warning Untested.
 */

#ifndef LAZYBIOS_OEM_HP_TYPE204_H
#define LAZYBIOS_OEM_HP_TYPE204_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for HP OEM SMBIOS Type 204 fields. */
typedef struct {
    lazybiosFieldStatus_t rack_name;
    lazybiosFieldStatus_t enclosure_name;
    lazybiosFieldStatus_t enclosure_model;
    lazybiosFieldStatus_t enclosure_serial;
    lazybiosFieldStatus_t enclosure_bays;
    lazybiosFieldStatus_t server_bay;
    lazybiosFieldStatus_t bays_filled;
} lazybiosOemHpType204FieldStatus_t;

/**
 * @brief Parsed HP OEM SMBIOS Type 204 Information.
 * @ingroup api_hp_type204
 *
 * @note Experimental and untested.
 */
typedef struct {
    const char* rack_name;
    const char* enclosure_name;
    const char* enclosure_model;
    const char* enclosure_serial;
    uint8_t enclosure_bays;
    const char* server_bay;
    uint8_t bays_filled;
	lazybiosOemHpType204FieldStatus_t field_status;
} lazybiosOemHpType204_t;

/** @addtogroup api_hp_type204
 * @{
 */

/**
 * @brief Parses all HP OEM SMBIOS Type 204 Information structures.
 * @param HPType204 Existing HP Type 204 array pointer value.
 * @param hptype204_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated HP Type 204 array, or NULL on failure.
 */
lazybiosOemHpType204_t* lazybiosGetOemHpType204(lazybiosOemHpType204_t* HPType204, size_t* hptype204_count, lazybiosDMI_t* DMIData);

/**
 * @brief Releases an array of parsed HP OEM SMBIOS Type 204 structures.
 * @param HPType204 HP Type 204 array to release.
 * @param hptype204_count Number of elements in HPType204.
 */
void lazybiosFreeOemHpType204(lazybiosOemHpType204_t* HPType204, size_t hptype204_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

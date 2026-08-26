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
 * @file dell_type218.h
 * @brief Public API for Dell Token Interface (Dell OEM SMBIOS Type 218).
 * @author LazySeldi
 */

#ifndef LAZYBIOS_OEM_DELL_TYPE218_H
#define LAZYBIOS_OEM_DELL_TYPE218_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif



/** @brief Availability metadata for DELL OEM SMBIOS Type 218 fields. */
typedef struct {
    lazybiosFieldStatus_t command_io_address;
    lazybiosFieldStatus_t command_io_code;
    lazybiosFieldStatus_t supported_command_classes_bitmap;
    lazybiosFieldStatus_t tokens;
} lazybiosOemDellType218FieldStatus_t;

typedef struct {
    uint16_t token_id;
    uint16_t location;
    uint16_t value;
} lazybiosOemDellType218Token_t;

/**
 * @brief Parsed Dell Token Interface information.
 * @ingroup api_dell_type218
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
    uint16_t command_io_address;
    uint8_t command_io_code;
    uint32_t supported_command_classes_bitmap;
    lazybiosOemDellType218Token_t *tokens;
    size_t token_count;
    lazybiosOemDellType218FieldStatus_t field_status;
} lazybiosOemDellType218_t;

/**
 * @brief A parsed set of Dell OEM Type 218 structures.
 * @ingroup api_dell_type218
 */
typedef struct {
	lazybiosOemDellType218_t* entries;
	size_t count;
} lazybiosOemDellType218Array_t;

/** @addtogroup api_dell_type218
 * @{
 */

/**
 * @brief Parses all Dell OEM SMBIOS Type 218 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no such structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosOemDellType218Array_t* lazybiosGetOemDellType218(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of Dell OEM Type 218 structures.
 * @param DellType218 Set to release; may be NULL.
 */
void lazybiosFreeOemDellType218(lazybiosOemDellType218Array_t* DellType218);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

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
 * @file dell_type212.h
 * @brief Public API for Dell Indexed I/O Access (Dell OEM SMBIOS Type 212).
 * @author LazySeldi
 */

#ifndef LAZYBIOS_OEM_DELL_TYPE212_H
#define LAZYBIOS_OEM_DELL_TYPE212_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif



/** @brief Availability metadata for DELL OEM SMBIOS Type 212 fields. */
typedef struct {
    lazybiosFieldStatus_t index_port;
    lazybiosFieldStatus_t data_port;
    lazybiosFieldStatus_t checksum_type;
    lazybiosFieldStatus_t start_index;
    lazybiosFieldStatus_t end_index;
    lazybiosFieldStatus_t value_index;
    lazybiosFieldStatus_t tokens;
} lazybiosOemDellType212FieldStatus_t;

typedef struct {
    uint16_t token_id;
    uint8_t  location;
    uint8_t  and_mask;
    uint8_t  or_mask;
} lazybiosOemDellType212Token_t;

/**
 * @brief Parsed Dell Indexed I/O Access information.
 * @ingroup api_dell_type212
 */
typedef struct {
    uint16_t index_port;
    uint16_t data_port;
    const char* checksum_type;
    uint8_t start_index;
    uint8_t end_index;
    uint8_t value_index;
    lazybiosOemDellType212Token_t* tokens;
    size_t token_count;
    lazybiosOemDellType212FieldStatus_t field_status;
} lazybiosOemDellType212_t;

/** @addtogroup api_dell_type212
 * @{
 */

/**
 * @brief Parses all DELL OEM SMBIOS Type 212 Information structures.
 * @param DELLType212 Existing DELL Type 212 array pointer value.
 * @param delltype212_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated DELL Type 212 array, or NULL on failure.
 */
lazybiosOemDellType212_t* lazybiosGetOemDellType212(lazybiosOemDellType212_t* DELLType212, size_t* delltype212_count, lazybiosDMI_t* DMIData);

/**
 * @brief Releases an array of parsed DELL OEM SMBIOS Type 212 structures.
 * @param DELLType212 DELL Type 212 array to release.
 * @param delltype212_count Number of elements in DELLType212.
 */
void lazybiosFreeOemDellType212(lazybiosOemDellType212_t* DELLType212, size_t delltype212_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

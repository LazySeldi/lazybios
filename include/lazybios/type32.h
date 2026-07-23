/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This file is part of lazybios.
 *
 * lazybios is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * lazybios is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with lazybios. If not, see <https://www.gnu.org/licenses/>.
 */
/**
 * @file type32.h
 * @brief Public API for SMBIOS Type 32 System Boot Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE32_H
#define LAZYBIOS_TYPE32_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 32 fields. */
typedef struct {
	lazybiosFieldStatus_t reserved;
	lazybiosFieldStatus_t boot_status;
	lazybiosFieldStatus_t additional_data_size;
	lazybiosFieldStatus_t additional_data;
} lazybiosType32FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 32 System Boot Information.
 * @ingroup api_type32
 */
typedef struct {
	uint8_t reserved[6];
	uint8_t boot_status;
	size_t additional_data_size;
	uint8_t* additional_data;
	lazybiosType32FieldStatus_t field_status;
} lazybiosType32_t;

/** @addtogroup api_type32
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 32 System Boot Information structures.
 * @param Type32 Existing Type 32 array pointer value; it is not dereferenced or released.
 * @param type32_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 32 array, or NULL on failure.
 */
lazybiosType32_t* lazybiosGetType32(lazybiosType32_t* Type32, size_t* type32_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an SMBIOS Type 32 system boot status code.
 * @param boot_status Raw system boot status code.
 * @return Static string describing the system boot status.
 */
const char* lazybiosType32BootStatusStr(uint8_t boot_status);

/**
 * @brief Releases an array of parsed SMBIOS Type 32 structures.
 * @param Type32 Type 32 array to release.
 * @param type32_count Number of elements in Type32.
 */
void lazybiosFreeType32(lazybiosType32_t* Type32, size_t type32_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

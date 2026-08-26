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
 * @file type40.h
 * @brief Public API for SMBIOS Type 40 Additional Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE40_H
#define LAZYBIOS_TYPE40_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for an SMBIOS Type 40 entry. */
typedef struct {
	lazybiosFieldStatus_t entry_length;
	lazybiosFieldStatus_t referenced_handle;
	lazybiosFieldStatus_t referenced_offset;
	lazybiosFieldStatus_t string;
	lazybiosFieldStatus_t value;
} lazybiosType40EntryFieldStatus_t;

/**
 * @brief Parsed entry from an SMBIOS Type 40 Additional Information structure.
 * @ingroup api_type40
 */
typedef struct {
	uint8_t entry_length;
	uint16_t referenced_handle;
	uint8_t referenced_offset;
	const char* string;
	size_t value_length;
	uint8_t* value;
	lazybiosType40EntryFieldStatus_t field_status;
} lazybiosType40Entry_t;

/** @brief Availability metadata for SMBIOS Type 40 fields. */
typedef struct {
	lazybiosFieldStatus_t additional_information_entry_count;
	lazybiosFieldStatus_t additional_information_entries;
} lazybiosType40FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 40 Additional Information.
 * @ingroup api_type40
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t additional_information_entry_count;
	lazybiosType40Entry_t* additional_information_entries;
	lazybiosType40FieldStatus_t field_status;
} lazybiosType40_t;

/**
 * @brief A parsed set of SMBIOS Type 40 structures.
 * @ingroup api_type40
 */
typedef struct {
	lazybiosType40_t* entries;
	size_t count;
} lazybiosType40Array_t;

/** @addtogroup api_type40
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 40 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 40 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType40Array_t* lazybiosGetType40(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 40 structures.
 * @param Type40 Set to release; may be NULL.
 */
void lazybiosFreeType40(lazybiosType40Array_t* Type40);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

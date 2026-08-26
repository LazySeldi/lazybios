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
 * @file type19.h
 * @brief Public API for SMBIOS Type 19 Memory Array Mapped Address.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE19_H
#define LAZYBIOS_TYPE19_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 19 fields. */
typedef struct {
	lazybiosFieldStatus_t starting_address;
	lazybiosFieldStatus_t ending_address;
	lazybiosFieldStatus_t memory_array_handle;
	lazybiosFieldStatus_t partition_width;
	lazybiosFieldStatus_t extended_starting_address;
	lazybiosFieldStatus_t extended_ending_address;
} lazybiosType19FieldStatus_t;

/**
 * @brief Decoded forms of the Type 19 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	uint64_t ending_address;
	uint64_t starting_address;
} lazybiosType19Decoded_t;

/**
 * @brief Parsed SMBIOS Type 19 Memory Array Mapped Address.
 * @ingroup api_type19
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint32_t starting_address;
	uint32_t ending_address;
	uint16_t memory_array_handle;
	uint8_t partition_width;
	uint64_t extended_starting_address;
	uint64_t extended_ending_address;
	lazybiosType19Decoded_t decoded;
	lazybiosType19FieldStatus_t field_status;
} lazybiosType19_t;

/**
 * @brief A parsed set of SMBIOS Type 19 structures.
 * @ingroup api_type19
 */
typedef struct {
	lazybiosType19_t* entries;
	size_t count;
} lazybiosType19Array_t;

/** @addtogroup api_type19
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 19 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 19 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType19Array_t* lazybiosGetType19(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 19 structures.
 * @param Type19 Set to release; may be NULL.
 */
void lazybiosFreeType19(lazybiosType19Array_t* Type19);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

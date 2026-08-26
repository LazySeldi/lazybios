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
 * @file type7.h
 * @brief Public API for SMBIOS Type 7 Cache Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE7_H
#define LAZYBIOS_TYPE7_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 7 fields. */
typedef struct {
	lazybiosFieldStatus_t socket_designation;
	lazybiosFieldStatus_t cache_configuration;
	lazybiosFieldStatus_t maximum_cache_size;
	lazybiosFieldStatus_t installed_size;
	lazybiosFieldStatus_t supported_sram_type;
	lazybiosFieldStatus_t current_sram_type;
	lazybiosFieldStatus_t cache_speed;
	lazybiosFieldStatus_t error_correction_type;
	lazybiosFieldStatus_t system_cache_type;
	lazybiosFieldStatus_t associativity;
	lazybiosFieldStatus_t maximum_cache_size_2;
	lazybiosFieldStatus_t installed_cache_size_2;
} lazybiosType7FieldStatus_t;

/**
 * @brief Decoded forms of the Type 7 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* associativity;
	const char* error_correction_type;
	uint64_t installed_cache_size_2;
	uint64_t installed_size;
	uint64_t maximum_cache_size;
	uint64_t maximum_cache_size_2;
	const char* system_cache_type;
	char* cache_configuration;
	char* supported_sram_type;
} lazybiosType7Decoded_t;

/**
 * @brief Parsed SMBIOS Type 7 Cache Information.
 * @ingroup api_type7
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* socket_designation;
	uint16_t cache_configuration;
	uint16_t maximum_cache_size;
	uint16_t installed_size;
	uint16_t supported_sram_type;
	uint16_t current_sram_type;
	uint8_t cache_speed;
	uint8_t error_correction_type;
	uint8_t system_cache_type;
	uint8_t associativity;
	uint32_t maximum_cache_size_2;
	uint32_t installed_cache_size_2;
	lazybiosType7Decoded_t decoded;
	lazybiosType7FieldStatus_t field_status;
} lazybiosType7_t;

/**
 * @brief A parsed set of SMBIOS Type 7 structures.
 * @ingroup api_type7
 */
typedef struct {
	lazybiosType7_t* entries;
	size_t count;
} lazybiosType7Array_t;

/** @addtogroup api_type7
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 7 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 7 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType7Array_t* lazybiosGetType7(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 7 structures.
 * @param Type7 Set to release; may be NULL.
 */
void lazybiosFreeType7(lazybiosType7Array_t* Type7);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

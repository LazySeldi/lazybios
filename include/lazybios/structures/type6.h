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
 * @file type6.h
 * @brief Public API for obsolete SMBIOS Type 6 Memory Module Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE6_H
#define LAZYBIOS_TYPE6_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 6 fields. */
typedef struct {
	lazybiosFieldStatus_t socket_designation;
	lazybiosFieldStatus_t bank_connections;
	lazybiosFieldStatus_t current_speed;
	lazybiosFieldStatus_t current_memory_type;
	lazybiosFieldStatus_t installed_size;
	lazybiosFieldStatus_t enabled_size;
	lazybiosFieldStatus_t error_status;
} lazybiosType6FieldStatus_t;

/**
 * @brief Decoded forms of the Type 6 encoded fields.
 */
typedef struct {
	char* bank_connections;
	char* current_memory_type;
	char* installed_size;
	char* enabled_size;
	char* error_status;
} lazybiosType6Decoded_t;

/**
 * @brief Parsed obsolete SMBIOS Type 6 Memory Module Information.
 * @ingroup api_type6
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* socket_designation;
	uint8_t bank_connections;
	uint8_t current_speed;
	uint16_t current_memory_type;
	uint8_t installed_size;
	uint8_t enabled_size;
	uint8_t error_status;
	lazybiosType6Decoded_t decoded;
	lazybiosType6FieldStatus_t field_status;
} lazybiosType6_t;

/**
 * @brief A parsed set of SMBIOS Type 6 structures.
 * @ingroup api_type6
 */
typedef struct {
	lazybiosType6_t* entries;
	size_t count;
} lazybiosType6Array_t;

/** @addtogroup api_type6
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 6 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 6 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType6Array_t* lazybiosGetType6(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 6 structures.
 * @param Type6 Set to release; may be NULL.
 */
void lazybiosFreeType6(lazybiosType6Array_t* Type6);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

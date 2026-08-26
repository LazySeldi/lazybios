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
 * @file type13.h
 * @brief Public API for SMBIOS Type 13 Firmware Language Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE13_H
#define LAZYBIOS_TYPE13_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 13 fields. */
typedef struct {
	lazybiosFieldStatus_t installable_languages;
	lazybiosFieldStatus_t flags;
	lazybiosFieldStatus_t languages;
	lazybiosFieldStatus_t current_language;
} lazybiosType13FieldStatus_t;

/**
 * @brief Decoded forms of the Type 13 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* flags;
} lazybiosType13Decoded_t;

/**
 * @brief Parsed SMBIOS Type 13 Firmware Language Information.
 * @ingroup api_type13
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t installable_languages;
	uint8_t flags;
	const char** languages;
	const char* current_language;
	lazybiosType13Decoded_t decoded;
	lazybiosType13FieldStatus_t field_status;
} lazybiosType13_t;

/**
 * @brief A parsed set of SMBIOS Type 13 structures.
 * @ingroup api_type13
 */
typedef struct {
	lazybiosType13_t* entries;
	size_t count;
} lazybiosType13Array_t;

/** @addtogroup api_type13
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 13 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 13 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType13Array_t* lazybiosGetType13(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 13 structures.
 * @param Type13 Set to release; may be NULL.
 */
void lazybiosFreeType13(lazybiosType13Array_t* Type13);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

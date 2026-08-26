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
 * @file type35.h
 * @brief Public API for SMBIOS Type 35 Management Device Component.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE35_H
#define LAZYBIOS_TYPE35_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 35 fields. */
typedef struct {
	lazybiosFieldStatus_t description;
	lazybiosFieldStatus_t management_device_handle;
	lazybiosFieldStatus_t component_handle;
	lazybiosFieldStatus_t threshold_handle;
} lazybiosType35FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 35 Management Device Component information.
 * @ingroup api_type35
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* description;
	uint16_t management_device_handle;
	uint16_t component_handle;
	uint16_t threshold_handle;
	lazybiosType35FieldStatus_t field_status;
} lazybiosType35_t;

/**
 * @brief A parsed set of SMBIOS Type 35 structures.
 * @ingroup api_type35
 */
typedef struct {
	lazybiosType35_t* entries;
	size_t count;
} lazybiosType35Array_t;

/** @addtogroup api_type35
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 35 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 35 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType35Array_t* lazybiosGetType35(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 35 structures.
 * @param Type35 Set to release; may be NULL.
 */
void lazybiosFreeType35(lazybiosType35Array_t* Type35);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

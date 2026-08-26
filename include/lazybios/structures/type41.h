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
 * @file type41.h
 * @brief Public API for SMBIOS Type 41 Onboard Devices Extended Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE41_H
#define LAZYBIOS_TYPE41_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 41 fields. */
typedef struct {
	lazybiosFieldStatus_t reference_designation;
	lazybiosFieldStatus_t device_type_and_status;
	lazybiosFieldStatus_t device_type_instance;
	lazybiosFieldStatus_t segment_group_number;
	lazybiosFieldStatus_t bus_number;
	lazybiosFieldStatus_t device_function_number;
} lazybiosType41FieldStatus_t;

/**
 * @brief Decoded forms of the Type 41 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* device_status;
	const char* device_type;
	char* device_function_number;
} lazybiosType41Decoded_t;

/**
 * @brief Parsed SMBIOS Type 41 Onboard Devices Extended Information.
 * @ingroup api_type41
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* reference_designation;
	uint8_t device_type_and_status;
	uint8_t device_type_instance;
	uint16_t segment_group_number;
	uint8_t bus_number;
	uint8_t device_function_number;
	lazybiosType41Decoded_t decoded;
	lazybiosType41FieldStatus_t field_status;
} lazybiosType41_t;

/**
 * @brief A parsed set of SMBIOS Type 41 structures.
 * @ingroup api_type41
 */
typedef struct {
	lazybiosType41_t* entries;
	size_t count;
} lazybiosType41Array_t;

/** @addtogroup api_type41
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 41 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 41 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType41Array_t* lazybiosGetType41(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 41 structures.
 * @param Type41 Set to release; may be NULL.
 */
void lazybiosFreeType41(lazybiosType41Array_t* Type41);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

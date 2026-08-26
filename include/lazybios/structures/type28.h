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
 * @file type28.h
 * @brief Public API for SMBIOS Type 28 Temperature Probe.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE28_H
#define LAZYBIOS_TYPE28_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 28 fields. */
typedef struct {
	lazybiosFieldStatus_t description;
	lazybiosFieldStatus_t location_and_status;
	lazybiosFieldStatus_t maximum_value;
	lazybiosFieldStatus_t minimum_value;
	lazybiosFieldStatus_t resolution;
	lazybiosFieldStatus_t tolerance;
	lazybiosFieldStatus_t accuracy;
	lazybiosFieldStatus_t oem_defined;
	lazybiosFieldStatus_t nominal_value;
} lazybiosType28FieldStatus_t;

/**
 * @brief Decoded forms of the Type 28 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* location;
	const char* status;
} lazybiosType28Decoded_t;

/**
 * @brief Parsed SMBIOS Type 28 Temperature Probe information.
 * @ingroup api_type28
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* description;
	uint8_t location_and_status;
	uint16_t maximum_value;
	/** Tenths of a degree C; `INT16_MIN` (`0x8000` on the wire) means unknown. */
	int16_t minimum_value;
	uint16_t resolution;
	uint16_t tolerance;
	uint16_t accuracy;
	uint32_t oem_defined;
	uint16_t nominal_value;
	lazybiosType28Decoded_t decoded;
	lazybiosType28FieldStatus_t field_status;
} lazybiosType28_t;

/**
 * @brief A parsed set of SMBIOS Type 28 structures.
 * @ingroup api_type28
 */
typedef struct {
	lazybiosType28_t* entries;
	size_t count;
} lazybiosType28Array_t;

/** @addtogroup api_type28
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 28 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 28 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType28Array_t* lazybiosGetType28(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 28 structures.
 * @param Type28 Set to release; may be NULL.
 */
void lazybiosFreeType28(lazybiosType28Array_t* Type28);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

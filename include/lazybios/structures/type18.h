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
 * @file type18.h
 * @brief Public API for SMBIOS Type 18 32-Bit Memory Error Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE18_H
#define LAZYBIOS_TYPE18_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 18 fields. */
typedef struct {
	lazybiosFieldStatus_t error_type;
	lazybiosFieldStatus_t error_granularity;
	lazybiosFieldStatus_t error_operation;
	lazybiosFieldStatus_t vendor_syndrome;
	lazybiosFieldStatus_t memory_array_error_address;
	lazybiosFieldStatus_t device_error_address;
	lazybiosFieldStatus_t error_resolution;
} lazybiosType18FieldStatus_t;

/**
 * @brief Decoded forms of the Type 18 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* error_granularity;
	const char* error_operation;
	const char* error_type;
} lazybiosType18Decoded_t;

/**
 * @brief Parsed SMBIOS Type 18 32-Bit Memory Error Information.
 * @ingroup api_type18
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t error_type;
	uint8_t error_granularity;
	uint8_t error_operation;
	uint32_t vendor_syndrome;
	uint32_t memory_array_error_address;
	uint32_t device_error_address;
	uint32_t error_resolution;
	lazybiosType18Decoded_t decoded;
	lazybiosType18FieldStatus_t field_status;
} lazybiosType18_t;

/**
 * @brief A parsed set of SMBIOS Type 18 structures.
 * @ingroup api_type18
 */
typedef struct {
	lazybiosType18_t* entries;
	size_t count;
} lazybiosType18Array_t;

/** @addtogroup api_type18
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 18 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 18 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType18Array_t* lazybiosGetType18(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 18 structures.
 * @param Type18 Set to release; may be NULL.
 */
void lazybiosFreeType18(lazybiosType18Array_t* Type18);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

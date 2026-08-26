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
 * @file type5.h
 * @brief Public API for obsolete SMBIOS Type 5 Memory Controller Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE5_H
#define LAZYBIOS_TYPE5_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 5 fields. */
typedef struct {
	lazybiosFieldStatus_t error_detecting_method;
	lazybiosFieldStatus_t error_correcting_capability;
	lazybiosFieldStatus_t supported_interleave;
	lazybiosFieldStatus_t current_interleave;
	lazybiosFieldStatus_t maximum_memory_module_size;
	lazybiosFieldStatus_t supported_speeds;
	lazybiosFieldStatus_t supported_memory_types;
	lazybiosFieldStatus_t memory_module_voltage;
	lazybiosFieldStatus_t number_of_associated_memory_slots;
	lazybiosFieldStatus_t memory_module_configuration_handles;
	lazybiosFieldStatus_t enabled_error_correcting_capabilities;
} lazybiosType5FieldStatus_t;

/**
 * @brief Decoded forms of the Type 5 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* current_interleave;
	const char* error_detecting_method;
	const char* supported_interleave;
	char* error_correcting_capability;
	char* enabled_error_correcting_capabilities;
	char* supported_speeds;
	char* supported_memory_types;
	char* memory_module_voltage;
} lazybiosType5Decoded_t;

/**
 * @brief Parsed obsolete SMBIOS Type 5 Memory Controller Information.
 * @ingroup api_type5
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t error_detecting_method;
	uint8_t error_correcting_capability;
	uint8_t supported_interleave;
	uint8_t current_interleave;
	uint8_t maximum_memory_module_size;
	uint16_t supported_speeds;
	uint16_t supported_memory_types;
	uint8_t memory_module_voltage;
	uint8_t number_of_associated_memory_slots;
	uint16_t* memory_module_configuration_handles;
	uint8_t enabled_error_correcting_capabilities;
	lazybiosType5Decoded_t decoded;
	lazybiosType5FieldStatus_t field_status;
} lazybiosType5_t;

/**
 * @brief A parsed set of SMBIOS Type 5 structures.
 * @ingroup api_type5
 */
typedef struct {
	lazybiosType5_t* entries;
	size_t count;
} lazybiosType5Array_t;

/** @addtogroup api_type5
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 5 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 5 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType5Array_t* lazybiosGetType5(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 5 structures.
 * @param Type5 Set to release; may be NULL.
 */
void lazybiosFreeType5(lazybiosType5Array_t* Type5);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

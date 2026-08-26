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
 * @file type39.h
 * @brief Public API for SMBIOS Type 39 System Power Supply.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE39_H
#define LAZYBIOS_TYPE39_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 39 fields. */
typedef struct {
	lazybiosFieldStatus_t power_unit_group;
	lazybiosFieldStatus_t location;
	lazybiosFieldStatus_t device_name;
	lazybiosFieldStatus_t manufacturer;
	lazybiosFieldStatus_t serial_number;
	lazybiosFieldStatus_t asset_tag_number;
	lazybiosFieldStatus_t model_part_number;
	lazybiosFieldStatus_t revision_level;
	lazybiosFieldStatus_t max_power_capacity;
	lazybiosFieldStatus_t power_supply_characteristics;
	lazybiosFieldStatus_t input_voltage_probe_handle;
	lazybiosFieldStatus_t cooling_device_handle;
	lazybiosFieldStatus_t input_current_probe_handle;
} lazybiosType39FieldStatus_t;

/**
 * @brief Decoded forms of the Type 39 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* input_voltage_range_switching;
	const char* power_supply_type;
	const char* status;
	char* power_supply_characteristics;
} lazybiosType39Decoded_t;

/**
 * @brief Parsed SMBIOS Type 39 System Power Supply information.
 * @ingroup api_type39
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t power_unit_group;
	const char* location;
	const char* device_name;
	const char* manufacturer;
	const char* serial_number;
	const char* asset_tag_number;
	const char* model_part_number;
	const char* revision_level;
	uint16_t max_power_capacity;
	uint16_t power_supply_characteristics;
	uint16_t input_voltage_probe_handle;
	uint16_t cooling_device_handle;
	uint16_t input_current_probe_handle;
	lazybiosType39Decoded_t decoded;
	lazybiosType39FieldStatus_t field_status;
} lazybiosType39_t;

/**
 * @brief A parsed set of SMBIOS Type 39 structures.
 * @ingroup api_type39
 */
typedef struct {
	lazybiosType39_t* entries;
	size_t count;
} lazybiosType39Array_t;

/** @addtogroup api_type39
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 39 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 39 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType39Array_t* lazybiosGetType39(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 39 structures.
 * @param Type39 Set to release; may be NULL.
 */
void lazybiosFreeType39(lazybiosType39Array_t* Type39);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

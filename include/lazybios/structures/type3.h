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
 * @file type3.h
 * @brief Public API for SMBIOS Type 3 Chassis Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE3_H
#define LAZYBIOS_TYPE3_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 3 fields. */
typedef struct {
	lazybiosFieldStatus_t manufacturer;
	lazybiosFieldStatus_t type;
	lazybiosFieldStatus_t version;
	lazybiosFieldStatus_t serial_number;
	lazybiosFieldStatus_t asset_tag;
	lazybiosFieldStatus_t boot_up_state;
	lazybiosFieldStatus_t power_supply_state;
	lazybiosFieldStatus_t thermal_state;
	lazybiosFieldStatus_t security_status;
	lazybiosFieldStatus_t oem_defined;
	lazybiosFieldStatus_t height;
	lazybiosFieldStatus_t number_of_power_cords;
	lazybiosFieldStatus_t contained_element_count;
	lazybiosFieldStatus_t contained_element_record_length;
	lazybiosFieldStatus_t contained_elements;
	lazybiosFieldStatus_t sku_number;
	lazybiosFieldStatus_t rack_type;
	lazybiosFieldStatus_t rack_height;
} lazybiosType3FieldStatus_t;

/**
 * @brief Decoded forms of the Type 3 encoded fields.
 *
 * Each member mirrors the identically named raw field and points at a static
 * string. Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* boot_up_state;
	const char* power_supply_state;
	const char* thermal_state;
	const char* security_status;
	char* type;
	char** contained_elements;
} lazybiosType3Decoded_t;

/**
 * @brief Parsed SMBIOS Type 3 System Enclosure or Chassis Information.
 * @ingroup api_type3
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* manufacturer;
	uint8_t type;
	const char* version;
	const char* serial_number;
	const char* asset_tag;
	uint8_t boot_up_state;
	uint8_t power_supply_state;
	uint8_t thermal_state;
	uint8_t security_status;
	uint32_t oem_defined;
	uint8_t height;
	uint8_t number_of_power_cords;
	uint8_t contained_element_count;
	uint8_t contained_element_record_length;
	uint8_t* contained_elements;
	const char* sku_number;
	uint8_t rack_type;
	uint8_t rack_height;
	lazybiosType3Decoded_t decoded;
	lazybiosType3FieldStatus_t field_status;
} lazybiosType3_t;

/**
 * @brief A parsed set of SMBIOS Type 3 structures.
 * @ingroup api_type3
 */
typedef struct {
	lazybiosType3_t* entries;
	size_t count;
} lazybiosType3Array_t;

/** @addtogroup api_type3
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 3 Chassis Information structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 3 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType3Array_t* lazybiosGetType3(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 3 structures.
 * @param Type3 Set to release; may be NULL.
 */
void lazybiosFreeType3(lazybiosType3Array_t* Type3);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This file is part of lazybios.
 *
 * lazybios is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * lazybios is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with lazybios. If not, see <https://www.gnu.org/licenses/>.
 */
/**
 * @file type29.h
 * @brief Public API for SMBIOS Type 29 Electrical Current Probe.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE29_H
#define LAZYBIOS_TYPE29_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 29 fields. */
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
} lazybiosType29FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 29 Electrical Current Probe information.
 * @ingroup api_type29
 */
typedef struct {
	char* description;
	uint8_t location_and_status;
	uint16_t maximum_value;
	uint16_t minimum_value;
	uint16_t resolution;
	uint16_t tolerance;
	uint16_t accuracy;
	uint32_t oem_defined;
	uint16_t nominal_value;
	lazybiosType29FieldStatus_t field_status;
} lazybiosType29_t;

/** @addtogroup api_type29
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 29 Electrical Current Probe structures.
 * @param Type29 Existing Type 29 array pointer value; it is not dereferenced or released.
 * @param type29_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 29 array, or NULL on failure.
 */
lazybiosType29_t* lazybiosGetType29(lazybiosType29_t* Type29, size_t* type29_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes the physical location from a Type 29 location-and-status field.
 * @param location_and_status Raw Type 29 location-and-status byte.
 * @return Static string describing the current-probe location.
 */
const char* lazybiosType29LocationStr(uint8_t location_and_status);

/**
 * @brief Decodes the monitored-current status from a Type 29 location-and-status field.
 * @param location_and_status Raw Type 29 location-and-status byte.
 * @return Static string describing the monitored-current status.
 */
const char* lazybiosType29StatusStr(uint8_t location_and_status);

/**
 * @brief Releases an array of parsed SMBIOS Type 29 structures.
 * @param Type29 Type 29 array to release.
 * @param type29_count Number of elements in Type29.
 */
void lazybiosFreeType29(lazybiosType29_t* Type29, size_t type29_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

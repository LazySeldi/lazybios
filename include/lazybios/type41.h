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
 * @file type41.h
 * @brief Public API for SMBIOS Type 41 Onboard Devices Extended Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE41_H
#define LAZYBIOS_TYPE41_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
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
 * @brief Parsed SMBIOS Type 41 Onboard Devices Extended Information.
 * @ingroup api_type41
 */
typedef struct {
	char* reference_designation;
	uint8_t device_type_and_status;
	uint8_t device_type_instance;
	uint16_t segment_group_number;
	uint8_t bus_number;
	uint8_t device_function_number;
	lazybiosType41FieldStatus_t field_status;
} lazybiosType41_t;

/** @addtogroup api_type41
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 41 Onboard Devices Extended Information structures.
 * @param Type41 Existing Type 41 array pointer value; it is not dereferenced or released.
 * @param type41_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 41 array, or NULL on failure.
 */
lazybiosType41_t* lazybiosGetType41(lazybiosType41_t* Type41, size_t* type41_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes the onboard-device type from a combined Type 41 type-and-status byte.
 * @param device_type_and_status Raw Type 41 device-type and status byte.
 * @return Static string describing the onboard-device type.
 */
const char* lazybiosType41DeviceTypeStr(uint8_t device_type_and_status);

/**
 * @brief Decodes the enabled status from a combined Type 41 type-and-status byte.
 * @param device_type_and_status Raw Type 41 device-type and status byte.
 * @return Static string describing whether the onboard device is enabled.
 */
const char* lazybiosType41DeviceStatusStr(uint8_t device_type_and_status);

/**
 * @brief Formats an SMBIOS Type 41 packed PCI device/function number.
 * @param device_function_number Raw packed device/function number value.
 * @param buf Output buffer that receives the decoded value.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType41DeviceFunctionStr(uint8_t device_function_number, char* buf, size_t buf_len);

/**
 * @brief Releases an array of parsed SMBIOS Type 41 structures.
 * @param Type41 Type 41 array to release.
 * @param type41_count Number of elements in Type41.
 */
void lazybiosFreeType41(lazybiosType41_t* Type41, size_t type41_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

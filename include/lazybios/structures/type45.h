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
 * @file type45.h
 * @brief Public API for SMBIOS Type 45 Firmware Inventory Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE45_H
#define LAZYBIOS_TYPE45_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 45 fields. */
typedef struct {
	lazybiosFieldStatus_t firmware_component_name;
	lazybiosFieldStatus_t firmware_version;
	lazybiosFieldStatus_t version_format;
	lazybiosFieldStatus_t firmware_id;
	lazybiosFieldStatus_t firmware_id_format;
	lazybiosFieldStatus_t release_date;
	lazybiosFieldStatus_t manufacturer;
	lazybiosFieldStatus_t lowest_supported_firmware_version;
	lazybiosFieldStatus_t image_size;
	lazybiosFieldStatus_t characteristics;
	lazybiosFieldStatus_t state;
	lazybiosFieldStatus_t number_of_associated_components;
	lazybiosFieldStatus_t associated_component_handles;
} lazybiosType45FieldStatus_t;

/**
 * @brief Decoded forms of the Type 45 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* firmware_id_format;
	const char* state;
	const char* version_format;
	char* characteristics;
} lazybiosType45Decoded_t;

/**
 * @brief Parsed SMBIOS Type 45 Firmware Inventory Information.
 * @ingroup api_type45
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* firmware_component_name;
	const char* firmware_version;
	uint8_t version_format;
	const char* firmware_id;
	uint8_t firmware_id_format;
	const char* release_date;
	const char* manufacturer;
	const char* lowest_supported_firmware_version;
	uint64_t image_size;
	uint16_t characteristics;
	uint8_t state;
	uint8_t number_of_associated_components;
	uint16_t* associated_component_handles;
	lazybiosType45Decoded_t decoded;
	lazybiosType45FieldStatus_t field_status;
} lazybiosType45_t;

/**
 * @brief A parsed set of SMBIOS Type 45 structures.
 * @ingroup api_type45
 */
typedef struct {
	lazybiosType45_t* entries;
	size_t count;
} lazybiosType45Array_t;

/** @addtogroup api_type45
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 45 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 45 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType45Array_t* lazybiosGetType45(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 45 structures.
 * @param Type45 Set to release; may be NULL.
 */
void lazybiosFreeType45(lazybiosType45Array_t* Type45);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

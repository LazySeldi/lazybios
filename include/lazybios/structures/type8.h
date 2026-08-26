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
 * @file type8.h
 * @brief Public API for SMBIOS Type 8 Port Connector Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE8_H
#define LAZYBIOS_TYPE8_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 8 fields. */
typedef struct {
	lazybiosFieldStatus_t internal_reference_designator;
	lazybiosFieldStatus_t internal_connector_type;
	lazybiosFieldStatus_t external_reference_designator;
	lazybiosFieldStatus_t external_connector_type;
	lazybiosFieldStatus_t port_type;
} lazybiosType8FieldStatus_t;

/**
 * @brief Decoded forms of the Type 8 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* external_connector_type;
	const char* internal_connector_type;
	const char* port_type;
} lazybiosType8Decoded_t;

/**
 * @brief Parsed SMBIOS Type 8 Port Connector Information.
 * @ingroup api_type8
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* internal_reference_designator;
	uint8_t internal_connector_type;
	const char* external_reference_designator;
	uint8_t external_connector_type;
	uint8_t port_type;
	lazybiosType8Decoded_t decoded;
	lazybiosType8FieldStatus_t field_status;
} lazybiosType8_t;

/**
 * @brief A parsed set of SMBIOS Type 8 structures.
 * @ingroup api_type8
 */
typedef struct {
	lazybiosType8_t* entries;
	size_t count;
} lazybiosType8Array_t;

/** @addtogroup api_type8
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 8 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 8 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType8Array_t* lazybiosGetType8(const lazybiosDMI_t* DMIData);
/**
 * @brief Releases a parsed set of SMBIOS Type 8 structures.
 * @param Type8 Set to release; may be NULL.
 */
void lazybiosFreeType8(lazybiosType8Array_t* Type8);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

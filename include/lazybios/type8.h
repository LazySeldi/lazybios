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
 * @file type8.h
 * @brief Public API for SMBIOS Type 8 Port Connector Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE8_H
#define LAZYBIOS_TYPE8_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
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
 * @brief Parsed SMBIOS Type 8 Port Connector Information.
 * @ingroup api_type8
 */
typedef struct {
	char* internal_reference_designator;
	uint8_t internal_connector_type;
	char* external_reference_designator;
	uint8_t external_connector_type;
	uint8_t port_type;
	lazybiosType8FieldStatus_t field_status;
} lazybiosType8_t;

/** @addtogroup api_type8
 * @{
 */

/** @brief Parses all Type 8 structures. */
lazybiosType8_t* lazybiosGetType8(lazybiosType8_t* Type8, size_t* type8_count, lazybiosDMI_t* DMIData);
/** @brief Decodes a port connector type. */
const char* lazybiosType8ConnectorTypeStr(uint8_t connector_type);
/** @brief Decodes a port type. */
const char* lazybiosType8PortTypeStr(uint8_t port_type);
/** @brief Releases parsed Type 8 structures. */
void lazybiosFreeType8(lazybiosType8_t* Type8, size_t type8_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

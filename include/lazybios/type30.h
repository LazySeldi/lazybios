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
 * @file type30.h
 * @brief Public API for SMBIOS Type 30 Out-of-Band Remote Access.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE30_H
#define LAZYBIOS_TYPE30_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 30 fields. */
typedef struct {
	lazybiosFieldStatus_t manufacturer_name;
	lazybiosFieldStatus_t connections;
} lazybiosType30FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 30 Out-of-Band Remote Access information.
 * @ingroup api_type30
 */
typedef struct {
	char* manufacturer_name;
	uint8_t connections;
	lazybiosType30FieldStatus_t field_status;
} lazybiosType30_t;

/** @addtogroup api_type30
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 30 Out-of-Band Remote Access structures.
 * @param Type30 Existing Type 30 array pointer value; it is not dereferenced or released.
 * @param type30_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 30 array, or NULL on failure.
 */
lazybiosType30_t* lazybiosGetType30(lazybiosType30_t* Type30, size_t* type30_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes the inbound-connection state from a Type 30 connections field.
 * @param connections Raw Type 30 connections bit field.
 * @return Static string describing whether inbound connections are enabled.
 */
const char* lazybiosType30InboundConnectionStr(uint8_t connections);

/**
 * @brief Decodes the outbound-connection state from a Type 30 connections field.
 * @param connections Raw Type 30 connections bit field.
 * @return Static string describing whether outbound connections are enabled.
 */
const char* lazybiosType30OutboundConnectionStr(uint8_t connections);

/**
 * @brief Releases an array of parsed SMBIOS Type 30 structures.
 * @param Type30 Type 30 array to release.
 * @param type30_count Number of elements in Type30.
 */
void lazybiosFreeType30(lazybiosType30_t* Type30, size_t type30_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

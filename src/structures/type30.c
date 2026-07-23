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
 * @file type30.c
 * @brief Implements parsing and decoding for SMBIOS Type 30 Out-of-Band Remote Access.
 * @author LazySeldi
 */

//
// Type 30 ( Out-of-Band Remote Access )
//

#include "lazybios_internal.h"
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define MANUFACTURER_NAME 0x04
#define CONNECTIONS 0x05

// Connection Bits
#define INBOUND_CONNECTION_ENABLED (1U << 0)
#define OUTBOUND_CONNECTION_ENABLED (1U << 1)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 30 Out-of-Band Remote Access structures.
 *
 * @param Type30 Existing Type 30 array pointer value; it is not dereferenced or released.
 * @param type30_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 30 array, or NULL on failure.
 */
lazybiosType30_t* lazybiosGetType30(lazybiosType30_t* Type30, size_t* type30_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_OUT_OF_BAND_REMOTE_ACCESS);
	size_t index = 0;

	Type30 = calloc(count, sizeof(lazybiosType30_t));
	if (!Type30) return NULL;
	if (count == 0) {
		*type30_count = 0;
		return Type30;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_OUT_OF_BAND_REMOTE_ACCESS) {
			if (index >= count) break;
			lazybiosType30_t* current = &Type30[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, manufacturer_name, len, MANUFACTURER_NAME, p, structure_end);
			READU8(current, connections, len, CONNECTIONS, p);

			index++;
		}
		p = DMINext(p, end);
	}
	*type30_count = index;
	return Type30;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes the inbound-connection state from a Type 30 connections field.
 *
 * @param connections Raw Type 30 connections bit field.
 * @return Static string describing whether inbound connections are enabled.
 */
const char* lazybiosType30InboundConnectionStr(uint8_t connections) {
	return (connections & INBOUND_CONNECTION_ENABLED) ? "Enabled" : "Disabled";
}

/**
 * @brief Decodes the outbound-connection state from a Type 30 connections field.
 *
 * @param connections Raw Type 30 connections bit field.
 * @return Static string describing whether outbound connections are enabled.
 */
const char* lazybiosType30OutboundConnectionStr(uint8_t connections) {
	return (connections & OUTBOUND_CONNECTION_ENABLED) ? "Enabled" : "Disabled";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 30 structures.
 *
 * @param Type30 Type 30 array to release.
 * @param type30_count Number of elements in Type30.
 */
void lazybiosFreeType30(lazybiosType30_t* Type30, size_t type30_count) {
	if (!Type30) return;

	for (size_t i = 0; i < type30_count; i++) {
		free(Type30[i].manufacturer_name);
	}
	free(Type30);
}

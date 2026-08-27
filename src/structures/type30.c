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
 * @file type30.c
 * @brief Implements parsing and decoding for SMBIOS Type 30 Out-of-Band Remote Access.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>

/* File-local decoders; their results are stored in each record's `decoded`. */
static inline const char* lazybiosType30InboundConnectionStr(uint8_t connections);
static inline const char* lazybiosType30OutboundConnectionStr(uint8_t connections);

// Fields
#define MANUFACTURER_NAME 0x04
#define CONNECTIONS 0x05

// Connection Bits
#define INBOUND_CONNECTION_ENABLED (1U << 0)
#define OUTBOUND_CONNECTION_ENABLED (1U << 1)

lazybiosType30Array_t* lazybiosGetType30(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType30Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_OUT_OF_BAND_REMOTE_ACCESS);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_OUT_OF_BAND_REMOTE_ACCESS].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_OUT_OF_BAND_REMOTE_ACCESS].first;
	}
	size_t index = 0;

	if (count == 0) return out;

	out->entries = calloc(count, sizeof(*out->entries));
	if (!out->entries) {
		free(out);
		return NULL;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];
		if (len < SMBIOS_HEADER_SIZE) break;
		const uint8_t* structure_end = DMINext(p, end);

		if (type == SMBIOS_TYPE_OUT_OF_BAND_REMOTE_ACCESS) {
			if (index >= count) break;
			lazybiosType30_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READSTR(current, manufacturer_name, len, MANUFACTURER_NAME, p, structure_end);
			READU8(current, connections, len, CONNECTIONS, p);

			current->decoded.inbound_connection = lazybiosType30InboundConnectionStr(current->connections);
			current->decoded.outbound_connection = lazybiosType30OutboundConnectionStr(current->connections);

			index++;
		}
		p = structure_end;
	}
	out->count = index;
	return out;
}

static inline const char* lazybiosType30InboundConnectionStr(uint8_t connections) {
	return (connections & INBOUND_CONNECTION_ENABLED) ? "Enabled" : "Disabled";
}

static inline const char* lazybiosType30OutboundConnectionStr(uint8_t connections) {
	return (connections & OUTBOUND_CONNECTION_ENABLED) ? "Enabled" : "Disabled";
}

void lazybiosFreeType30(lazybiosType30Array_t* Type30) {
    if (!Type30) return;

    free(Type30->entries);

    free(Type30);
}

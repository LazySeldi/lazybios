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
 * @file type46.c
 * @brief Implements parsing and decoding for SMBIOS Type 46 String Property.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>

/* File-local decoders; their results are stored in each record's `decoded`. */
static inline const char* lazybiosType46StringPropertyIDStr(uint16_t string_property_id);

// Fields
#define STRING_PROPERTY_ID 0x04
#define STRING_PROPERTY_VALUE 0x06
#define PARENT_HANDLE 0x07

// String Property IDs
#define STRING_PROPERTY_ID_RESERVED 0x0000
#define STRING_PROPERTY_ID_UEFI_DEVICE_PATH 0x0001
#define STRING_PROPERTY_ID_DMTF_MAX 0x7FFF
#define STRING_PROPERTY_ID_FIRMWARE_VENDOR_MAX 0xBFFF

lazybiosType46Array_t* lazybiosGetType46(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType46Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_STRING_PROPERTY);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_STRING_PROPERTY].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_STRING_PROPERTY].first;
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

		if (type == SMBIOS_TYPE_STRING_PROPERTY) {
			if (index >= count) break;
			lazybiosType46_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READU16(current, string_property_id, len, STRING_PROPERTY_ID, p);
			READSTR(current, string_property_value, len, STRING_PROPERTY_VALUE, p, structure_end);
			READU16(current, parent_handle, len, PARENT_HANDLE, p);
			if (current->parent_handle == 0xFFFF) LAZYBIOS_MARK_ABSENT(current, parent_handle);

			current->decoded.string_property_id = lazybiosType46StringPropertyIDStr(current->string_property_id);

			index++;
		}
		p = structure_end;
	}
	out->count = index;
	return out;
}

static inline const char* lazybiosType46StringPropertyIDStr(uint16_t string_property_id) {
	if (string_property_id == STRING_PROPERTY_ID_RESERVED) return "Reserved";
	if (string_property_id == STRING_PROPERTY_ID_UEFI_DEVICE_PATH) return "UEFI Device Path";
	if (string_property_id <= STRING_PROPERTY_ID_DMTF_MAX) return "Reserved for Future DMTF Use";
	if (string_property_id <= STRING_PROPERTY_ID_FIRMWARE_VENDOR_MAX) return "Firmware Vendor-defined";
	return "OEM-defined";
}

void lazybiosFreeType46(lazybiosType46Array_t* Type46) {
    if (!Type46) return;

    free(Type46->entries);

    free(Type46);
}

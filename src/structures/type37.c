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
 * @file type37.c
 * @brief Implements parsing and decoding for SMBIOS Type 37 Memory Channel.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>
#include <string.h>

/* File-local decoders; their results are stored in each record's `decoded`. */
static inline const char* lazybiosType37ChannelTypeStr(uint8_t channel_type);

// Fields
#define CHANNEL_TYPE 0x04
#define MAXIMUM_CHANNEL_LOAD 0x05
#define MEMORY_DEVICE_COUNT 0x06
#define MEMORY_DEVICES 0x07
#define MEMORY_DEVICE_ENTRY_LENGTH 0x03

// Channel Types
#define CHANNEL_TYPE_OTHER 0x01
#define CHANNEL_TYPE_UNKNOWN 0x02
#define CHANNEL_TYPE_RAMBUS 0x03
#define CHANNEL_TYPE_SYNCLINK 0x04

lazybiosType37Array_t* lazybiosGetType37(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType37Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_MEMORY_CHANNEL);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_MEMORY_CHANNEL].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_MEMORY_CHANNEL].first;
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

		if (type == SMBIOS_TYPE_MEMORY_CHANNEL) {
			if (index >= count) break;
			lazybiosType37_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READU8(current, channel_type, len, CHANNEL_TYPE, p);
			READU8(current, maximum_channel_load, len, MAXIMUM_CHANNEL_LOAD, p);
			READU8(current, memory_device_count, len, MEMORY_DEVICE_COUNT, p);

			if (current->field_status.memory_device_count == LAZYBIOS_FIELD_PRESENT) {
				size_t memory_devices_length = (size_t)current->memory_device_count * MEMORY_DEVICE_ENTRY_LENGTH;
				if ((size_t)len >= MEMORY_DEVICES + memory_devices_length) {
					if (current->memory_device_count > 0) {
						current->memory_devices = calloc(current->memory_device_count, sizeof(lazybiosType37MemoryDevice_t));
						if (!current->memory_devices) {
							out->count = index + 1;
							lazybiosFreeType37(out);
							return NULL;
						}

						for (size_t i = 0; i < current->memory_device_count; i++) {
							const uint8_t* entry = p + MEMORY_DEVICES + (i * MEMORY_DEVICE_ENTRY_LENGTH);
							current->memory_devices[i].load = entry[0];
							memcpy(&current->memory_devices[i].handle, entry + 1, sizeof(uint16_t));
							current->memory_devices[i].field_status.load = LAZYBIOS_FIELD_PRESENT;
							current->memory_devices[i].field_status.handle = LAZYBIOS_FIELD_PRESENT;
							if (current->memory_devices[i].handle == 0xFFFF) {
								current->memory_devices[i].field_status.handle = LAZYBIOS_FIELD_ABSENT;
							}
						}
					}
					current->field_status.memory_devices = LAZYBIOS_FIELD_PRESENT;
				}
			}

			current->decoded.channel_type = lazybiosType37ChannelTypeStr(current->channel_type);

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

static inline const char* lazybiosType37ChannelTypeStr(uint8_t channel_type) {
	switch (channel_type) {
		case CHANNEL_TYPE_OTHER:
			return "Other";
		case CHANNEL_TYPE_UNKNOWN:
			return "Unknown";
		case CHANNEL_TYPE_RAMBUS:
			return "Rambus";
		case CHANNEL_TYPE_SYNCLINK:
			return "SyncLink";
		default:
			return "Undefined";
	}
}

void lazybiosFreeType37(lazybiosType37Array_t* Type37) {
    if (!Type37) return;

    for (size_t i = 0; i < Type37->count; i++) free(Type37->entries[i].memory_devices);

    free(Type37->entries);

    free(Type37);
}

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
 * @file type37.c
 * @brief Implements parsing and decoding for SMBIOS Type 37 Memory Channel.
 * @author LazySeldi
 */

//
// Type 37 ( Memory Channel )
//

#include "lazybios_internal.h"
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 37 Memory Channel structures.
 *
 * @param Type37 Existing Type 37 array pointer value; it is not dereferenced or released.
 * @param type37_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 37 array, or NULL on failure.
 */
lazybiosType37_t* lazybiosGetType37(lazybiosType37_t* Type37, size_t* type37_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_MEMORY_CHANNEL);
	size_t index = 0;

	Type37 = calloc(count, sizeof(lazybiosType37_t));
	if (!Type37) return NULL;
	if (count == 0) {
		*type37_count = 0;
		return Type37;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_MEMORY_CHANNEL) {
			if (index >= count) break;
			lazybiosType37_t* current = &Type37[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);

			READU8(current, channel_type, len, CHANNEL_TYPE, p);
			READU8(current, maximum_channel_load, len, MAXIMUM_CHANNEL_LOAD, p);
			READU8(current, memory_device_count, len, MEMORY_DEVICE_COUNT, p);

			if (current->field_status.memory_device_count == LAZYBIOS_FIELD_PRESENT) {
				size_t memory_devices_length = (size_t)current->memory_device_count * MEMORY_DEVICE_ENTRY_LENGTH;
				if ((size_t)len >= MEMORY_DEVICES + memory_devices_length) {
					if (current->memory_device_count > 0) {
						current->memory_devices = calloc(current->memory_device_count, sizeof(lazybiosType37MemoryDevice_t));
						if (!current->memory_devices) {
							lazybiosFreeType37(Type37, count);
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

			index++;
		}
		p = DMINext(p, end);
	}
	*type37_count = index;
	return Type37;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes an SMBIOS Type 37 memory-channel type.
 *
 * @param channel_type Raw memory-channel type value.
 * @return Static string describing the channel type.
 */
const char* lazybiosType37ChannelTypeStr(uint8_t channel_type) {
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 37 structures.
 *
 * @param Type37 Type 37 array to release.
 * @param type37_count Number of elements in Type37.
 */
void lazybiosFreeType37(lazybiosType37_t* Type37, size_t type37_count) {
	if (!Type37) return;

	for (size_t i = 0; i < type37_count; i++) {
		free(Type37[i].memory_devices);
	}
	free(Type37);
}

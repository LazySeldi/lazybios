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
 * @file type10.c
 * @brief Implements parsing and decoding for obsolete SMBIOS Type 10 On Board Devices Information.
 * @author LazySeldi
 */

//
// Type 10 ( On Board Devices Information, Obsolete )
//

#include "lazybios_internal.h"
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
#define DEVICES 0x04
#define DEVICE_ENTRY_SIZE 2
#define DEVICE_STATUS_MASK 0x80
#define DEVICE_TYPE_MASK 0x7F

// Device Types
#define DEVICE_TYPE_OTHER 0x01
#define DEVICE_TYPE_UNKNOWN 0x02
#define DEVICE_TYPE_VIDEO 0x03
#define DEVICE_TYPE_SCSI_CONTROLLER 0x04
#define DEVICE_TYPE_ETHERNET 0x05
#define DEVICE_TYPE_TOKEN_RING 0x06
#define DEVICE_TYPE_SOUND 0x07
#define DEVICE_TYPE_PATA_CONTROLLER 0x08
#define DEVICE_TYPE_SATA_CONTROLLER 0x09
#define DEVICE_TYPE_SAS_CONTROLLER 0x0A
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all obsolete SMBIOS Type 10 On Board Devices Information structures.
 *
 * @param Type10 Existing Type 10 array pointer value; it is not dereferenced or released.
 * @param type10_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 10 array, or NULL on failure.
 */
lazybiosType10_t* lazybiosGetType10(lazybiosType10_t* Type10, size_t* type10_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;
	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;
	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_ONBOARD_DEVICES);
	size_t index = 0;
	Type10 = calloc(count, sizeof(lazybiosType10_t));
	if (!Type10) return NULL;
	if (count == 0) {
		*type10_count = 0;
		return Type10;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];
		if (type == SMBIOS_TYPE_ONBOARD_DEVICES) {
			lazybiosType10_t* current = &Type10[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			if (len >= DEVICES && (len - DEVICES) % DEVICE_ENTRY_SIZE == 0) {
				current->device_count = (len - DEVICES) / DEVICE_ENTRY_SIZE;
				LAZYBIOS_MARK_PRESENT(current, device_count);
				if (current->device_count > 0) {
					current->devices = calloc(current->device_count, sizeof(lazybiosType10Device_t));
					if (!current->devices) {
						lazybiosFreeType10(Type10, count);
						return NULL;
					}
					for (size_t i = 0; i < current->device_count; i++) {
						size_t device_offset = DEVICES + (i * DEVICE_ENTRY_SIZE);
						current->devices[i].device_type_and_status = p[device_offset];
						LAZYBIOS_MARK_PRESENT(&current->devices[i], device_type_and_status);
						uint8_t string_number = p[device_offset + 1];
						current->devices[i].description = DMIString(p, len, string_number, structure_end);
						if (string_number == 0 || current->devices[i].description) {
							LAZYBIOS_MARK_PRESENT(&current->devices[i], description);
						} else {
							LAZYBIOS_MARK_ABSENT(&current->devices[i], description);
						}
					}
				}
				LAZYBIOS_MARK_PRESENT(current, devices);
			} else {
				LAZYBIOS_MARK_ABSENT(current, device_count);
				LAZYBIOS_MARK_ABSENT(current, devices);
			}
			index++;
		}
		p = DMINext(p, end);
	}
	*type10_count = index;
	return Type10;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes an obsolete Type 10 onboard-device type.
 *
 * @param device_type_and_status Raw combined device-type and status byte.
 * @return Static string describing the onboard-device type.
 */
const char* lazybiosType10DeviceTypeStr(uint8_t device_type_and_status) {
	switch (device_type_and_status & DEVICE_TYPE_MASK) {
		case DEVICE_TYPE_OTHER: return "Other";
		case DEVICE_TYPE_UNKNOWN: return "Unknown";
		case DEVICE_TYPE_VIDEO: return "Video";
		case DEVICE_TYPE_SCSI_CONTROLLER: return "SCSI Controller";
		case DEVICE_TYPE_ETHERNET: return "Ethernet";
		case DEVICE_TYPE_TOKEN_RING: return "Token Ring";
		case DEVICE_TYPE_SOUND: return "Sound";
		case DEVICE_TYPE_PATA_CONTROLLER: return "PATA Controller";
		case DEVICE_TYPE_SATA_CONTROLLER: return "SATA Controller";
		case DEVICE_TYPE_SAS_CONTROLLER: return "SAS Controller";
		default: return "Undefined";
	}
}

/**
 * @brief Decodes an obsolete Type 10 onboard-device status.
 *
 * @param device_type_and_status Raw combined device-type and status byte.
 * @return Static string describing whether the device is enabled.
 */
const char* lazybiosType10DeviceStatusStr(uint8_t device_type_and_status) {
	return (device_type_and_status & DEVICE_STATUS_MASK) ? "Enabled" : "Disabled";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 10 structures.
 *
 * @param Type10 Type 10 array to release.
 * @param type10_count Number of elements in Type10.
 */
void lazybiosFreeType10(lazybiosType10_t* Type10, size_t type10_count) {
	if (!Type10) return;
	for (size_t i = 0; i < type10_count; i++) {
		for (size_t j = 0; j < Type10[i].device_count; j++) free(Type10[i].devices[j].description);
		free(Type10[i].devices);
	}
	free(Type10);
}

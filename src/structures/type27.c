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
 * @file type27.c
 * @brief Implements parsing and decoding for SMBIOS Type 27 Cooling Device.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>

/* File-local decoders; their results are stored in each record's `decoded`. */
static inline const char* lazybiosType27DeviceTypeStr(uint8_t device_type_and_status);
static inline const char* lazybiosType27StatusStr(uint8_t device_type_and_status);

// Fields
#define TEMPERATURE_PROBE_HANDLE 0x04
#define DEVICE_TYPE_AND_STATUS 0x06
#define COOLING_UNIT_GROUP 0x07
#define OEM_DEFINED 0x08
#define NOMINAL_SPEED 0x0C
#define DESCRIPTION 0x0E

// Device Type and Status Masks
#define DEVICE_TYPE_MASK 0x1F
#define STATUS_MASK 0xE0
#define STATUS_SHIFT 5

// Device Types
#define DEVICE_TYPE_OTHER 0x01
#define DEVICE_TYPE_UNKNOWN 0x02
#define DEVICE_TYPE_FAN 0x03
#define DEVICE_TYPE_CENTRIFUGAL_BLOWER 0x04
#define DEVICE_TYPE_CHIP_FAN 0x05
#define DEVICE_TYPE_CABINET_FAN 0x06
#define DEVICE_TYPE_POWER_SUPPLY_FAN 0x07
#define DEVICE_TYPE_HEAT_PIPE 0x08
#define DEVICE_TYPE_INTEGRATED_REFRIGERATION 0x09
#define DEVICE_TYPE_ACTIVE_COOLING 0x10
#define DEVICE_TYPE_PASSIVE_COOLING 0x11

// Statuses
#define STATUS_OTHER 0x01
#define STATUS_UNKNOWN 0x02
#define STATUS_OK 0x03
#define STATUS_NON_CRITICAL 0x04
#define STATUS_CRITICAL 0x05
#define STATUS_NON_RECOVERABLE 0x06

lazybiosType27Array_t* lazybiosGetType27(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType27Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_COOLING_DEVICE);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_COOLING_DEVICE].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_COOLING_DEVICE].first;
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

		if (type == SMBIOS_TYPE_COOLING_DEVICE) {
			if (index >= count) break;
			lazybiosType27_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READU16(current, temperature_probe_handle, len, TEMPERATURE_PROBE_HANDLE, p);
			if (current->temperature_probe_handle == 0xFFFF) {
				LAZYBIOS_MARK_ABSENT(current, temperature_probe_handle);
			}
			READU8(current, device_type_and_status, len, DEVICE_TYPE_AND_STATUS, p);
			READU8(current, cooling_unit_group, len, COOLING_UNIT_GROUP, p);
			READU32(current, oem_defined, len, OEM_DEFINED, p);
			READU16(current, nominal_speed, len, NOMINAL_SPEED, p);

			if (lazybiosIsVersionPlus(DMIData, 2, 7)) {
				READSTR(current, description, len, DESCRIPTION, p, structure_end);
			}

			current->decoded.device_type = lazybiosType27DeviceTypeStr(current->device_type_and_status);
			current->decoded.status = lazybiosType27StatusStr(current->device_type_and_status);

			index++;
		}
		p = structure_end;
	}
	out->count = index;
	return out;
}

static inline const char* lazybiosType27DeviceTypeStr(uint8_t device_type_and_status) {
	switch (device_type_and_status & DEVICE_TYPE_MASK) {
		case DEVICE_TYPE_OTHER:
			return "Other";
		case DEVICE_TYPE_UNKNOWN:
			return "Unknown";
		case DEVICE_TYPE_FAN:
			return "Fan";
		case DEVICE_TYPE_CENTRIFUGAL_BLOWER:
			return "Centrifugal Blower";
		case DEVICE_TYPE_CHIP_FAN:
			return "Chip Fan";
		case DEVICE_TYPE_CABINET_FAN:
			return "Cabinet Fan";
		case DEVICE_TYPE_POWER_SUPPLY_FAN:
			return "Power Supply Fan";
		case DEVICE_TYPE_HEAT_PIPE:
			return "Heat Pipe";
		case DEVICE_TYPE_INTEGRATED_REFRIGERATION:
			return "Integrated Refrigeration";
		case DEVICE_TYPE_ACTIVE_COOLING:
			return "Active Cooling";
		case DEVICE_TYPE_PASSIVE_COOLING:
			return "Passive Cooling";
		default:
			return "Undefined";
	}
}

static inline const char* lazybiosType27StatusStr(uint8_t device_type_and_status) {
	switch ((device_type_and_status & STATUS_MASK) >> STATUS_SHIFT) {
		case STATUS_OTHER:
			return "Other";
		case STATUS_UNKNOWN:
			return "Unknown";
		case STATUS_OK:
			return "OK";
		case STATUS_NON_CRITICAL:
			return "Non-critical";
		case STATUS_CRITICAL:
			return "Critical";
		case STATUS_NON_RECOVERABLE:
			return "Non-recoverable";
		default:
			return "Undefined";
	}
}

void lazybiosFreeType27(lazybiosType27Array_t* Type27) {
    if (!Type27) return;

    free(Type27->entries);

    free(Type27);
}

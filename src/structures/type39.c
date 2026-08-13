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
 * @file type39.c
 * @brief Implements parsing and decoding for SMBIOS Type 39 System Power Supply.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>

// Fields
#define POWER_UNIT_GROUP 0x04
#define LOCATION 0x05
#define DEVICE_NAME 0x06
#define MANUFACTURER 0x07
#define SERIAL_NUMBER 0x08
#define ASSET_TAG_NUMBER 0x09
#define MODEL_PART_NUMBER 0x0A
#define REVISION_LEVEL 0x0B
#define MAX_POWER_CAPACITY 0x0C
#define POWER_SUPPLY_CHARACTERISTICS 0x0E
#define INPUT_VOLTAGE_PROBE_HANDLE 0x10
#define COOLING_DEVICE_HANDLE 0x12
#define INPUT_CURRENT_PROBE_HANDLE 0x14

// Power Supply Characteristics Masks
#define POWER_SUPPLY_TYPE_MASK 0x3C00
#define POWER_SUPPLY_TYPE_SHIFT 10
#define POWER_SUPPLY_STATUS_MASK 0x0380
#define POWER_SUPPLY_STATUS_SHIFT 7
#define INPUT_VOLTAGE_RANGE_SWITCHING_MASK 0x0078
#define INPUT_VOLTAGE_RANGE_SWITCHING_SHIFT 3
#define POWER_SUPPLY_UNPLUGGED_MASK 0x0004
#define POWER_SUPPLY_PRESENT_MASK 0x0002
#define POWER_SUPPLY_HOT_REPLACEABLE_MASK 0x0001

// Power Supply Types
#define POWER_SUPPLY_TYPE_OTHER 0x01
#define POWER_SUPPLY_TYPE_UNKNOWN 0x02
#define POWER_SUPPLY_TYPE_LINEAR 0x03
#define POWER_SUPPLY_TYPE_SWITCHING 0x04
#define POWER_SUPPLY_TYPE_BATTERY 0x05
#define POWER_SUPPLY_TYPE_UPS 0x06
#define POWER_SUPPLY_TYPE_CONVERTER 0x07
#define POWER_SUPPLY_TYPE_REGULATOR 0x08

// Power Supply Statuses
#define POWER_SUPPLY_STATUS_OTHER 0x01
#define POWER_SUPPLY_STATUS_UNKNOWN 0x02
#define POWER_SUPPLY_STATUS_OK 0x03
#define POWER_SUPPLY_STATUS_NON_CRITICAL 0x04
#define POWER_SUPPLY_STATUS_CRITICAL 0x05

// Input Voltage Range Switching Types
#define INPUT_VOLTAGE_RANGE_SWITCHING_OTHER 0x01
#define INPUT_VOLTAGE_RANGE_SWITCHING_UNKNOWN 0x02
#define INPUT_VOLTAGE_RANGE_SWITCHING_MANUAL 0x03
#define INPUT_VOLTAGE_RANGE_SWITCHING_AUTO_SWITCH 0x04
#define INPUT_VOLTAGE_RANGE_SWITCHING_WIDE_RANGE 0x05
#define INPUT_VOLTAGE_RANGE_SWITCHING_NOT_APPLICABLE 0x06

lazybiosType39_t* lazybiosGetType39(lazybiosType39_t* Type39, size_t* type39_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_SYSTEM_POWER_SUPPLY);
	size_t index = 0;

	Type39 = calloc(count, sizeof(lazybiosType39_t));
	if (!Type39) return NULL;
	if (count == 0) {
		*type39_count = 0;
		return Type39;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_SYSTEM_POWER_SUPPLY) {
			if (index >= count) break;
			lazybiosType39_t* current = &Type39[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READU8(current, power_unit_group, len, POWER_UNIT_GROUP, p);
			READSTR(current, location, len, LOCATION, p, structure_end);
			READSTR(current, device_name, len, DEVICE_NAME, p, structure_end);
			READSTR(current, manufacturer, len, MANUFACTURER, p, structure_end);
			READSTR(current, serial_number, len, SERIAL_NUMBER, p, structure_end);
			READSTR(current, asset_tag_number, len, ASSET_TAG_NUMBER, p, structure_end);
			READSTR(current, model_part_number, len, MODEL_PART_NUMBER, p, structure_end);
			READSTR(current, revision_level, len, REVISION_LEVEL, p, structure_end);
			READU16(current, max_power_capacity, len, MAX_POWER_CAPACITY, p);
			READU16(current, power_supply_characteristics, len, POWER_SUPPLY_CHARACTERISTICS, p);
			READU16(current, input_voltage_probe_handle, len, INPUT_VOLTAGE_PROBE_HANDLE, p);
			READU16(current, cooling_device_handle, len, COOLING_DEVICE_HANDLE, p);
			READU16(current, input_current_probe_handle, len, INPUT_CURRENT_PROBE_HANDLE, p);
			if (current->input_voltage_probe_handle == 0xFFFF) LAZYBIOS_MARK_ABSENT(current, input_voltage_probe_handle);
			if (current->cooling_device_handle == 0xFFFF) LAZYBIOS_MARK_ABSENT(current, cooling_device_handle);
			if (current->input_current_probe_handle == 0xFFFF) LAZYBIOS_MARK_ABSENT(current, input_current_probe_handle);

			index++;
		}
		p = DMINext(p, end);
	}
	*type39_count = index;
	return Type39;
}

const char* lazybiosType39PowerSupplyTypeStr(uint16_t characteristics) {
	switch ((characteristics & POWER_SUPPLY_TYPE_MASK) >> POWER_SUPPLY_TYPE_SHIFT) {
		case POWER_SUPPLY_TYPE_OTHER:
			return "Other";
		case POWER_SUPPLY_TYPE_UNKNOWN:
			return "Unknown";
		case POWER_SUPPLY_TYPE_LINEAR:
			return "Linear";
		case POWER_SUPPLY_TYPE_SWITCHING:
			return "Switching";
		case POWER_SUPPLY_TYPE_BATTERY:
			return "Battery";
		case POWER_SUPPLY_TYPE_UPS:
			return "UPS";
		case POWER_SUPPLY_TYPE_CONVERTER:
			return "Converter";
		case POWER_SUPPLY_TYPE_REGULATOR:
			return "Regulator";
		default:
			return "Reserved";
	}
}

const char* lazybiosType39StatusStr(uint16_t characteristics) {
	switch ((characteristics & POWER_SUPPLY_STATUS_MASK) >> POWER_SUPPLY_STATUS_SHIFT) {
		case POWER_SUPPLY_STATUS_OTHER:
			return "Other";
		case POWER_SUPPLY_STATUS_UNKNOWN:
			return "Unknown";
		case POWER_SUPPLY_STATUS_OK:
			return "OK";
		case POWER_SUPPLY_STATUS_NON_CRITICAL:
			return "Non-critical";
		case POWER_SUPPLY_STATUS_CRITICAL:
			return "Critical";
		default:
			return "Reserved";
	}
}

const char* lazybiosType39InputVoltageRangeSwitchingStr(uint16_t characteristics) {
	switch ((characteristics & INPUT_VOLTAGE_RANGE_SWITCHING_MASK) >> INPUT_VOLTAGE_RANGE_SWITCHING_SHIFT) {
		case INPUT_VOLTAGE_RANGE_SWITCHING_OTHER:
			return "Other";
		case INPUT_VOLTAGE_RANGE_SWITCHING_UNKNOWN:
			return "Unknown";
		case INPUT_VOLTAGE_RANGE_SWITCHING_MANUAL:
			return "Manual";
		case INPUT_VOLTAGE_RANGE_SWITCHING_AUTO_SWITCH:
			return "Auto-switch";
		case INPUT_VOLTAGE_RANGE_SWITCHING_WIDE_RANGE:
			return "Wide Range";
		case INPUT_VOLTAGE_RANGE_SWITCHING_NOT_APPLICABLE:
			return "Not Applicable";
		default:
			return "Reserved";
	}
}

void lazybiosType39CharacteristicsFlagsStr(uint16_t characteristics, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;

	snprintf(buf, buf_len, "Unplugged: %s, Present: %s, Hot-replaceable: %s",
			 (characteristics & POWER_SUPPLY_UNPLUGGED_MASK) ? "Yes" : "No",
			 (characteristics & POWER_SUPPLY_PRESENT_MASK) ? "Yes" : "No",
			 (characteristics & POWER_SUPPLY_HOT_REPLACEABLE_MASK) ? "Yes" : "No");
}

void lazybiosFreeType39(lazybiosType39_t* Type39, size_t type39_count) {
    (void)type39_count;
    if (!Type39) return;

    free(Type39);
}

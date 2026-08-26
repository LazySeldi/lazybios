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
 * @file type3.c
 * @brief Implements parsing and decoding for SMBIOS Type 3 Chassis Information.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* File-local decoders; their output is stored in each record's `decoded`. */
static size_t lazybiosType3ContainedElementTypeStr(uint8_t contained_elements, char* buf, size_t buf_len);
static size_t lazybiosType3TypeStr(uint8_t type, char* buf, size_t buf_len);

/* File-local decoders; their results are stored in each record's `decoded`. */
static inline const char* lazybiosType3StateStr(uint8_t state);
static inline const char* lazybiosType3SecurityStatusStr(uint8_t security_status);

// Fields
#define MANUFACTURER 0x04
#define TYPE 0x05
#define VERSION 0x06
#define SERIAL_NUMBER 0x07
#define ASSET_TAG_NUMBER 0x08
#define BOOT_UP_STATE 0x09
#define POWER_SUPPLY_STATE 0x0A
#define THERMAL_STATE 0x0B
#define SECURITY_STATUS 0x0C
#define OEM_DEFINED 0x0D
#define HEIGHT 0x11
#define NUMBER_OF_POWER_CORDS 0x12
#define CONTAINED_ELEMENT_COUNT 0x13
#define CONTAINED_ELEMENT_RECORD_LENGTH 0x14
#define CONTAINED_ELEMENTS 0x15
#define SKU_NUMBER(n, m) (0x15 + (n * m))
#define RACK_TYPE(n, m) (0x16 + (n * m))
#define RACK_HEIGHT(n, m) (0x17 + (n * m))


// Chassis Type
#define CHASSIS_TYPE_OTHER 0x01
#define CHASSIS_TYPE_UNKNOWN 0x02
#define CHASSIS_TYPE_DESKTOP 0x03
#define CHASSIS_TYPE_LOW_PROFILE_DESKTOP 0x04
#define CHASSIS_TYPE_PIZZA_BOX 0x05
#define CHASSIS_TYPE_MINI_TOWER 0x06
#define CHASSIS_TYPE_TOWER 0x07
#define CHASSIS_TYPE_PORTABLE 0x08
#define CHASSIS_TYPE_LAPTOP 0x09
#define CHASSIS_TYPE_NOTEBOOK 0x0A
#define CHASSIS_TYPE_HAND_HELD 0x0B
#define CHASSIS_TYPE_DOCKING_STATION 0x0C
#define CHASSIS_TYPE_ALL_IN_ONE 0x0D
#define CHASSIS_TYPE_SUB_NOTEBOOK 0x0E
#define CHASSIS_TYPE_SPACE_SAVING 0x0F
#define CHASSIS_TYPE_LUNCH_BOX 0x10
#define CHASSIS_TYPE_MAIN_SERVER_CHASSIS 0x11
#define CHASSIS_TYPE_EXPANSION_CHASSIS 0x12
#define CHASSIS_TYPE_SUBCHASSIS 0x13
#define CHASSIS_TYPE_BUS_EXPANSION_CHASSIS 0x14
#define CHASSIS_TYPE_PERIPHERAL_CHASSIS 0x15
#define CHASSIS_TYPE_RAID_CHASSIS 0x16
#define CHASSIS_TYPE_RACK_MOUNT_CHASSIS 0x17
#define CHASSIS_TYPE_SEALED_CASE_PC 0x18
#define CHASSIS_TYPE_MULTI_SYSTEM_CHASSIS 0x19
#define CHASSIS_TYPE_COMPACT_PCI 0x1A
#define CHASSIS_TYPE_ADVANCED_TCA 0x1B
#define CHASSIS_TYPE_BLADE 0x1C
#define CHASSIS_TYPE_BLADE_ENCLOSURE 0x1D
#define CHASSIS_TYPE_TABLET 0x1E
#define CHASSIS_TYPE_CONVERTIBLE 0x1F
#define CHASSIS_TYPE_DETACHABLE 0x20
#define CHASSIS_TYPE_IOT_GATEWAY 0x21
#define CHASSIS_TYPE_EMBEDDED_PC 0x22
#define CHASSIS_TYPE_MINI_PC 0x23
#define CHASSIS_TYPE_STICK_PC 0x24

// Chassis State
#define CHASSIS_STATE_OTHER 0x01
#define CHASSIS_STATE_UNKNOWN 0x02
#define CHASSIS_STATE_SAFE 0x03
#define CHASSIS_STATE_WARNING 0x04
#define CHASSIS_STATE_CRITICAL 0x05
#define CHASSIS_STATE_NON_RECOVERABLE 0x06

// Chassis Status
#define CHASSIS_SECURITY_STATUS_OTHER 0x01
#define CHASSIS_SECURITY_STATUS_UNKNOWN 0x02
#define CHASSIS_SECURITY_STATUS_NONE 0x03
#define CHASSIS_SECURITY_STATUS_EXT_INTERFACE_LOCKED_OUT 0x04
#define CHASSIS_SECURITY_STATUS_EXT_INTERFACE_ENABLED 0x05

lazybiosType3Array_t* lazybiosGetType3(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType3Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_CHASSIS);
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

		if (type == SMBIOS_TYPE_CHASSIS) {
			if (index >= count) break;
			lazybiosType3_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, manufacturer, len, MANUFACTURER, p, structure_end);

			READU8(current, type, len, TYPE, p);

			READSTR(current, version, len, VERSION, p, structure_end);

			READSTR(current, serial_number, len, SERIAL_NUMBER, p, structure_end);

			READSTR(current, asset_tag, len, ASSET_TAG_NUMBER, p, structure_end);

			if (lazybiosIsVersionPlus(DMIData, 2, 1)) {
				READU8(current, boot_up_state, len, BOOT_UP_STATE, p);
				READU8(current, power_supply_state, len, POWER_SUPPLY_STATE, p);
				READU8(current, thermal_state, len, THERMAL_STATE, p);
				READU8(current, security_status, len, SECURITY_STATUS, p);
			} else {
				current->boot_up_state = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, boot_up_state);
				current->power_supply_state = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, power_supply_state);
				current->thermal_state = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, thermal_state);
				current->security_status = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, security_status);
			}

			if (lazybiosIsVersionPlus(DMIData, 2, 3)) {
				READU32(current, oem_defined, len, OEM_DEFINED, p);

				READU8(current, height, len, HEIGHT, p);
				if (current->height == 0 || current->height == 0xFF) LAZYBIOS_MARK_ABSENT(current, height);

				READU8(current, number_of_power_cords, len, NUMBER_OF_POWER_CORDS, p);
				if (current->number_of_power_cords == 0) LAZYBIOS_MARK_ABSENT(current, number_of_power_cords);

				READU8(current, contained_element_count, len, CONTAINED_ELEMENT_COUNT, p);
				READU8(current, contained_element_record_length, len, CONTAINED_ELEMENT_RECORD_LENGTH, p);

				if (LAZYBIOS_FIELD_STATUS(current, contained_element_count) == LAZYBIOS_FIELD_PRESENT &&
					LAZYBIOS_FIELD_STATUS(current, contained_element_record_length) == LAZYBIOS_FIELD_PRESENT) {
					const size_t array_bytes = (size_t)current->contained_element_count *
						current->contained_element_record_length;

					const int contained_layout_valid =
						(current->contained_element_count == 0 && current->contained_element_record_length == 0) ||
						(current->contained_element_count > 0 && current->contained_element_record_length >= 3 &&
						 (size_t)len >= CONTAINED_ELEMENTS + array_bytes);

					if (current->contained_element_count == 0) {
						LAZYBIOS_MARK_ABSENT(current, contained_elements);
					} else if (contained_layout_valid) {
						current->contained_elements = malloc(array_bytes);
						if (current->contained_elements) {
							memcpy(current->contained_elements, p + CONTAINED_ELEMENTS, array_bytes);
							LAZYBIOS_MARK_PRESENT(current, contained_elements);
						}
						if (current->contained_elements) {
							current->decoded.contained_elements =
								calloc(current->contained_element_count, sizeof(char*));
							if (current->decoded.contained_elements) {
								char elembuf[LAZYBIOS_DECODER_BUF_SIZE];
								for (size_t e = 0; e < current->contained_element_count; e++) {
									const uint8_t* rec = current->contained_elements +
										(e * current->contained_element_record_length);
									lazybiosType3ContainedElementTypeStr(rec[0], elembuf, sizeof(elembuf));
									current->decoded.contained_elements[e] = lazybiosDup(elembuf);
								}
							}
						}
					} else {
						LAZYBIOS_MARK_ABSENT(current, contained_elements);
					}

					if (contained_layout_valid && lazybiosIsVersionPlus(DMIData, 2, 7)) {
						READSTR(current, sku_number, len,
							SKU_NUMBER(current->contained_element_count, current->contained_element_record_length), p, structure_end);
					} else {
						current->sku_number = NULL;
						LAZYBIOS_MARK_UNREACHABLE(current, sku_number);
					}

					if (contained_layout_valid && lazybiosIsVersionPlus(DMIData, 3, 9)) {
						READU8(current, rack_type, len,
							RACK_TYPE(current->contained_element_count, current->contained_element_record_length), p);
						READU8(current, rack_height, len,
							RACK_HEIGHT(current->contained_element_count, current->contained_element_record_length), p);
						if (current->rack_type == 0) {
							LAZYBIOS_MARK_ABSENT(current, rack_type);
							LAZYBIOS_MARK_ABSENT(current, rack_height);
						}
					} else {
						current->rack_type = 0;
						LAZYBIOS_MARK_UNREACHABLE(current, rack_type);
						current->rack_height = 0;
						LAZYBIOS_MARK_UNREACHABLE(current, rack_height);
					}
				}
			} else {
				current->oem_defined = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, oem_defined);
				current->height = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, height);
				current->number_of_power_cords = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, number_of_power_cords);
				current->contained_element_count = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, contained_element_count);
				current->contained_element_record_length = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, contained_element_record_length);
				current->contained_elements = NULL;
				LAZYBIOS_MARK_UNREACHABLE(current, contained_elements);
				current->sku_number = NULL;
				LAZYBIOS_MARK_UNREACHABLE(current, sku_number);
				current->rack_type = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, rack_type);
				current->rack_height = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, rack_height);
			}

			current->decoded.boot_up_state = lazybiosType3StateStr(current->boot_up_state);
			current->decoded.power_supply_state = lazybiosType3StateStr(current->power_supply_state);
			current->decoded.thermal_state = lazybiosType3StateStr(current->thermal_state);
			current->decoded.security_status = lazybiosType3SecurityStatusStr(current->security_status);

			char decbuf[LAZYBIOS_DECODER_BUF_SIZE];
			if (LAZYBIOS_FIELD_STATUS(current, type) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType3TypeStr(current->type, decbuf, sizeof(decbuf));
				current->decoded.type = lazybiosDup(decbuf);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}


// Chassis Type
static size_t lazybiosType3TypeStr(uint8_t type, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	size_t len = 0;
	buf[0] = '\0';

	// Bit 7 is the Chassis lock
	if (type & (1 << 7)) {
		lazybiosDecoderAppend(buf, buf_len, &len, "Chassis lock present, ");
	}

	// Bits 6:0 = chassis type
	uint8_t chassis_type = type & 0x7F;
	switch (chassis_type) {
		case CHASSIS_TYPE_OTHER:
			lazybiosDecoderAppend(buf, buf_len, &len, "Other");
			break;
		case CHASSIS_TYPE_UNKNOWN:
			lazybiosDecoderAppend(buf, buf_len, &len, "Unknown");
			break;
		case CHASSIS_TYPE_DESKTOP:
			lazybiosDecoderAppend(buf, buf_len, &len, "Desktop");
			break;
		case CHASSIS_TYPE_LOW_PROFILE_DESKTOP:
			lazybiosDecoderAppend(buf, buf_len, &len, "Low Profile Desktop");
			break;
		case CHASSIS_TYPE_PIZZA_BOX:
			lazybiosDecoderAppend(buf, buf_len, &len, "Pizza Box");
			break;
		case CHASSIS_TYPE_MINI_TOWER:
			lazybiosDecoderAppend(buf, buf_len, &len, "Mini Tower");
			break;
		case CHASSIS_TYPE_TOWER:
			lazybiosDecoderAppend(buf, buf_len, &len, "Tower");
			break;
		case CHASSIS_TYPE_PORTABLE:
			lazybiosDecoderAppend(buf, buf_len, &len, "Portable");
			break;
		case CHASSIS_TYPE_LAPTOP:
			lazybiosDecoderAppend(buf, buf_len, &len, "Laptop");
			break;
		case CHASSIS_TYPE_NOTEBOOK:
			lazybiosDecoderAppend(buf, buf_len, &len, "Notebook");
			break;
		case CHASSIS_TYPE_HAND_HELD:
			lazybiosDecoderAppend(buf, buf_len, &len, "Hand Held");
			break;
		case CHASSIS_TYPE_DOCKING_STATION:
			lazybiosDecoderAppend(buf, buf_len, &len, "Docking Station");
			break;
		case CHASSIS_TYPE_ALL_IN_ONE:
			lazybiosDecoderAppend(buf, buf_len, &len, "All in One");
			break;
		case CHASSIS_TYPE_SUB_NOTEBOOK:
			lazybiosDecoderAppend(buf, buf_len, &len, "Sub Notebook");
			break;
		case CHASSIS_TYPE_SPACE_SAVING:
			lazybiosDecoderAppend(buf, buf_len, &len, "Space-saving");
			break;
		case CHASSIS_TYPE_LUNCH_BOX:
			lazybiosDecoderAppend(buf, buf_len, &len, "Lunch Box");
			break;
		case CHASSIS_TYPE_MAIN_SERVER_CHASSIS:
			lazybiosDecoderAppend(buf, buf_len, &len, "Main Server Chassis");
			break;
		case CHASSIS_TYPE_EXPANSION_CHASSIS:
			lazybiosDecoderAppend(buf, buf_len, &len, "Expansion Chassis");
			break;
		case CHASSIS_TYPE_SUBCHASSIS:
			lazybiosDecoderAppend(buf, buf_len, &len, "SubChassis");
			break;
		case CHASSIS_TYPE_BUS_EXPANSION_CHASSIS:
			lazybiosDecoderAppend(buf, buf_len, &len, "Bus Expansion Chassis");
			break;
		case CHASSIS_TYPE_PERIPHERAL_CHASSIS:
			lazybiosDecoderAppend(buf, buf_len, &len, "Peripheral Chassis");
			break;
		case CHASSIS_TYPE_RAID_CHASSIS:
			lazybiosDecoderAppend(buf, buf_len, &len, "RAID Chassis");
			break;
		case CHASSIS_TYPE_RACK_MOUNT_CHASSIS:
			lazybiosDecoderAppend(buf, buf_len, &len, "Rack Mount Chassis");
			break;
		case CHASSIS_TYPE_SEALED_CASE_PC:
			lazybiosDecoderAppend(buf, buf_len, &len, "Sealed-case PC");
			break;
		case CHASSIS_TYPE_MULTI_SYSTEM_CHASSIS:
			lazybiosDecoderAppend(buf, buf_len, &len, "Multi-system chassis");
			break;
		case CHASSIS_TYPE_COMPACT_PCI:
			lazybiosDecoderAppend(buf, buf_len, &len, "Compact PCI");
			break;
		case CHASSIS_TYPE_ADVANCED_TCA:
			lazybiosDecoderAppend(buf, buf_len, &len, "Advanced TCA");
			break;
		case CHASSIS_TYPE_BLADE:
			lazybiosDecoderAppend(buf, buf_len, &len, "Blade");
			break;
		case CHASSIS_TYPE_BLADE_ENCLOSURE:
			lazybiosDecoderAppend(buf, buf_len, &len, "Blade Enclosure");
			break;
		case CHASSIS_TYPE_TABLET:
			lazybiosDecoderAppend(buf, buf_len, &len, "Tablet");
			break;
		case CHASSIS_TYPE_CONVERTIBLE:
			lazybiosDecoderAppend(buf, buf_len, &len, "Convertible");
			break;
		case CHASSIS_TYPE_DETACHABLE:
			lazybiosDecoderAppend(buf, buf_len, &len, "Detachable");
			break;
		case CHASSIS_TYPE_IOT_GATEWAY:
			lazybiosDecoderAppend(buf, buf_len, &len, "IoT Gateway");
			break;
		case CHASSIS_TYPE_EMBEDDED_PC:
			lazybiosDecoderAppend(buf, buf_len, &len, "Embedded PC");
			break;
		case CHASSIS_TYPE_MINI_PC:
			lazybiosDecoderAppend(buf, buf_len, &len, "Mini PC");
			break;
		case CHASSIS_TYPE_STICK_PC:
			lazybiosDecoderAppend(buf, buf_len, &len, "Stick PC");
			break;
		default:
			lazybiosDecoderAppend(buf, buf_len, &len, "Unknown Chassis Type");
			break;
	}

	if (len >= 2 && buf[len - 2] == ',') buf[len - 2] = '\0'; // remove trailing ", " ONLY if present
	if (len == 0) {
		snprintf(buf, buf_len, "None");
	}
	return buf ? strlen(buf) : 0;
}

// Chassis State
static inline const char* lazybiosType3StateStr(uint8_t state) {
	switch (state) {
		case CHASSIS_STATE_OTHER:
			return "Other";
		case CHASSIS_STATE_UNKNOWN:
			return "Unknown";
		case CHASSIS_STATE_SAFE:
			return "Safe";
		case CHASSIS_STATE_WARNING:
			return "Warning";
		case CHASSIS_STATE_CRITICAL:
			return "Critical";
		case CHASSIS_STATE_NON_RECOVERABLE:
			return "Non-recoverable";
		default:
			return "Unknown Chassis State";
	}
}

// Chassis Status
static inline const char* lazybiosType3SecurityStatusStr(uint8_t security_status) {
	switch (security_status) {
		case CHASSIS_SECURITY_STATUS_OTHER:
			return "Other";
		case CHASSIS_SECURITY_STATUS_UNKNOWN:
			return "Unknown";
		case CHASSIS_SECURITY_STATUS_NONE:
			return "None";
		case CHASSIS_SECURITY_STATUS_EXT_INTERFACE_LOCKED_OUT:
			return "External interface locked out";
		case CHASSIS_SECURITY_STATUS_EXT_INTERFACE_ENABLED:
			return "External interface enabled";
		default:
			return "Unknown chassis security state";
	}
}

// Chassis Contained Elements
static size_t lazybiosType3ContainedElementTypeStr(uint8_t contained_elements, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	buf[0] = '\0';

	if (contained_elements & 0x80) { // MSB = 1 → SMBIOS structure type
		uint8_t struct_type = contained_elements & 0x7F;
		snprintf(buf, buf_len, "SMBIOS Structure Type %u", struct_type);
	} else { // MSB = 0 → board type
		uint8_t board_type = contained_elements & 0x7F;
		const char* str = lazybiosType2BoardTypeStr(board_type);
		snprintf(buf, buf_len, "%s", str);
	}
	return buf ? strlen(buf) : 0;
}

void lazybiosFreeType3(lazybiosType3Array_t* Type3) {
	if (!Type3) return;

	for (size_t i = 0; i < Type3->count; i++) {
		free(Type3->entries[i].decoded.type);
		if (Type3->entries[i].decoded.contained_elements) {
			for (size_t e = 0; e < Type3->entries[i].contained_element_count; e++)
				free(Type3->entries[i].decoded.contained_elements[e]);
			free(Type3->entries[i].decoded.contained_elements);
		}
	}
	for (size_t i = 0; i < Type3->count; i++) free(Type3->entries[i].contained_elements);

    free(Type3->entries);

    free(Type3);
}

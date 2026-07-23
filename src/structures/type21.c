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
 * @file type21.c
 * @brief Implements parsing and decoding for SMBIOS Type 21 Built-in Pointing Device.
 * @author LazySeldi
 */

//
// Type 21 ( Built-in Pointing Device )
//

#include "lazybios_internal.h"
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define POINTING_DEVICE_TYPE 0x04
#define INTERFACE 0x05
#define NUMBER_OF_BUTTONS 0x06

// Pointing Device Types
#define POINTING_DEVICE_OTHER 0x01
#define POINTING_DEVICE_UNKNOWN 0x02
#define POINTING_DEVICE_MOUSE 0x03
#define POINTING_DEVICE_TRACK_BALL 0x04
#define POINTING_DEVICE_TRACK_POINT 0x05
#define POINTING_DEVICE_GLIDE_POINT 0x06
#define POINTING_DEVICE_TOUCH_PAD 0x07
#define POINTING_DEVICE_TOUCH_SCREEN 0x08
#define POINTING_DEVICE_OPTICAL_SENSOR 0x09

// Interfaces
#define INTERFACE_OTHER 0x01
#define INTERFACE_UNKNOWN 0x02
#define INTERFACE_SERIAL 0x03
#define INTERFACE_PS2 0x04
#define INTERFACE_INFRARED 0x05
#define INTERFACE_HP_HIL 0x06
#define INTERFACE_BUS_MOUSE 0x07
#define INTERFACE_ADB 0x08
#define INTERFACE_BUS_MOUSE_DB9 0xA0
#define INTERFACE_BUS_MOUSE_MICRO_DIN 0xA1
#define INTERFACE_USB 0xA2
#define INTERFACE_I2C 0xA3
#define INTERFACE_SPI 0xA4
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 21 Built-in Pointing Device structures.
 *
 * @param Type21 Existing Type 21 array pointer value; it is not dereferenced or released.
 * @param type21_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 21 array, or NULL on failure.
 */
lazybiosType21_t* lazybiosGetType21(lazybiosType21_t* Type21, size_t* type21_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_BUILT_IN_POINTING_DEVICE);
	size_t index = 0;

	Type21 = calloc(count, sizeof(lazybiosType21_t));
	if (!Type21) return NULL;
	if (count == 0) {
		*type21_count = 0;
		return Type21;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_BUILT_IN_POINTING_DEVICE) {
			if (index >= count) break;
			lazybiosType21_t* current = &Type21[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);

			READU8(current, pointing_device_type, len, POINTING_DEVICE_TYPE, p);
			READU8(current, interface, len, INTERFACE, p);
			READU8(current, number_of_buttons, len, NUMBER_OF_BUTTONS, p);

			index++;
		}
		p = DMINext(p, end);
	}
	*type21_count = index;
	return Type21;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes an SMBIOS built-in pointing-device type.
 *
 * @param pointing_device_type Raw pointing-device type value.
 * @return Static string describing the pointing-device type.
 */
const char* lazybiosType21PointingDeviceTypeStr(uint8_t pointing_device_type) {
	switch (pointing_device_type) {
		case POINTING_DEVICE_OTHER:
			return "Other";
		case POINTING_DEVICE_UNKNOWN:
			return "Unknown";
		case POINTING_DEVICE_MOUSE:
			return "Mouse";
		case POINTING_DEVICE_TRACK_BALL:
			return "Track Ball";
		case POINTING_DEVICE_TRACK_POINT:
			return "Track Point";
		case POINTING_DEVICE_GLIDE_POINT:
			return "Glide Point";
		case POINTING_DEVICE_TOUCH_PAD:
			return "Touch Pad";
		case POINTING_DEVICE_TOUCH_SCREEN:
			return "Touch Screen";
		case POINTING_DEVICE_OPTICAL_SENSOR:
			return "Optical Sensor";
		default:
			return "Undefined";
	}
}

/**
 * @brief Decodes an SMBIOS built-in pointing-device interface.
 *
 * @param interface Raw pointing-device interface value.
 * @return Static string describing the interface.
 */
const char* lazybiosType21InterfaceStr(uint8_t interface) {
	switch (interface) {
		case INTERFACE_OTHER:
			return "Other";
		case INTERFACE_UNKNOWN:
			return "Unknown";
		case INTERFACE_SERIAL:
			return "Serial";
		case INTERFACE_PS2:
			return "PS/2";
		case INTERFACE_INFRARED:
			return "Infrared";
		case INTERFACE_HP_HIL:
			return "HP-HIL";
		case INTERFACE_BUS_MOUSE:
			return "Bus Mouse";
		case INTERFACE_ADB:
			return "ADB (Apple Desktop Bus)";
		case INTERFACE_BUS_MOUSE_DB9:
			return "Bus Mouse DB-9";
		case INTERFACE_BUS_MOUSE_MICRO_DIN:
			return "Bus Mouse Micro-DIN";
		case INTERFACE_USB:
			return "USB";
		case INTERFACE_I2C:
			return "I2C";
		case INTERFACE_SPI:
			return "SPI";
		default:
			return "Undefined";
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 21 structures.
 *
 * @param Type21 Type 21 array to release.
 * @param type21_count Number of elements in Type21.
 */
void lazybiosFreeType21(lazybiosType21_t* Type21, size_t type21_count) {
	(void)type21_count;
	free(Type21);
}

/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type21.c
 * @brief Implements parsing and decoding for SMBIOS Type 21 Built-in Pointing Device.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>

/* File-local decoders; their results are stored in each record's `decoded`. */
static inline const char* lazybiosType21InterfaceStr(uint8_t interface);
static inline const char* lazybiosType21PointingDeviceTypeStr(uint8_t pointing_device_type);

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

lazybiosType21Array_t* lazybiosGetType21(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType21Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_BUILT_IN_POINTING_DEVICE);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_BUILT_IN_POINTING_DEVICE].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_BUILT_IN_POINTING_DEVICE].first;
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

		if (type == SMBIOS_TYPE_BUILT_IN_POINTING_DEVICE) {
			if (index >= count) break;
			lazybiosType21_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READU8(current, pointing_device_type, len, POINTING_DEVICE_TYPE, p);
			READU8(current, interface, len, INTERFACE, p);
			READU8(current, number_of_buttons, len, NUMBER_OF_BUTTONS, p);

			current->decoded.interface = lazybiosType21InterfaceStr(current->interface);
			current->decoded.pointing_device_type = lazybiosType21PointingDeviceTypeStr(current->pointing_device_type);

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

static inline const char* lazybiosType21PointingDeviceTypeStr(uint8_t pointing_device_type) {
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

static inline const char* lazybiosType21InterfaceStr(uint8_t interface) {
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

void lazybiosFreeType21(lazybiosType21Array_t* Type21) {
    if (!Type21) return;

    free(Type21->entries);

    free(Type21);
}

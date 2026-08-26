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
 * @file type24.c
 * @brief Implements parsing and decoding for SMBIOS Type 24 Hardware Security.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>

/* File-local decoders; their results are stored in each record's `decoded`. */
static inline const char* lazybiosType24AdministratorPasswordStatusStr(uint8_t hardware_security_settings);
static inline const char* lazybiosType24FrontPanelResetStatusStr(uint8_t hardware_security_settings);
static inline const char* lazybiosType24KeyboardPasswordStatusStr(uint8_t hardware_security_settings);
static inline const char* lazybiosType24PowerOnPasswordStatusStr(uint8_t hardware_security_settings);

// Fields
#define HARDWARE_SECURITY_SETTINGS 0x04

// Status Masks and Shifts
#define POWER_ON_PASSWORD_STATUS_MASK 0xC0
#define POWER_ON_PASSWORD_STATUS_SHIFT 6
#define KEYBOARD_PASSWORD_STATUS_MASK 0x30
#define KEYBOARD_PASSWORD_STATUS_SHIFT 4
#define ADMINISTRATOR_PASSWORD_STATUS_MASK 0x0C
#define ADMINISTRATOR_PASSWORD_STATUS_SHIFT 2
#define FRONT_PANEL_RESET_STATUS_MASK 0x03

// Status Values
#define SECURITY_STATUS_DISABLED 0x00
#define SECURITY_STATUS_ENABLED 0x01
#define SECURITY_STATUS_NOT_IMPLEMENTED 0x02
#define SECURITY_STATUS_UNKNOWN 0x03

lazybiosType24Array_t* lazybiosGetType24(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType24Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_HARDWARE_SECURITY);
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

		if (type == SMBIOS_TYPE_HARDWARE_SECURITY) {
			if (index >= count) break;
			lazybiosType24_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READU8(current, hardware_security_settings, len, HARDWARE_SECURITY_SETTINGS, p);

			current->decoded.administrator_password_status = lazybiosType24AdministratorPasswordStatusStr(current->hardware_security_settings);
			current->decoded.front_panel_reset_status = lazybiosType24FrontPanelResetStatusStr(current->hardware_security_settings);
			current->decoded.keyboard_password_status = lazybiosType24KeyboardPasswordStatusStr(current->hardware_security_settings);
			current->decoded.power_on_password_status = lazybiosType24PowerOnPasswordStatusStr(current->hardware_security_settings);

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

static inline const char* lazybiosType24PowerOnPasswordStatusStr(uint8_t hardware_security_settings) {
	switch ((hardware_security_settings & POWER_ON_PASSWORD_STATUS_MASK) >> POWER_ON_PASSWORD_STATUS_SHIFT) {
		case SECURITY_STATUS_DISABLED:
			return "Disabled";
		case SECURITY_STATUS_ENABLED:
			return "Enabled";
		case SECURITY_STATUS_NOT_IMPLEMENTED:
			return "Not Implemented";
		case SECURITY_STATUS_UNKNOWN:
			return "Unknown";
		default:
			return "Undefined";
	}
}

static inline const char* lazybiosType24KeyboardPasswordStatusStr(uint8_t hardware_security_settings) {
	switch ((hardware_security_settings & KEYBOARD_PASSWORD_STATUS_MASK) >> KEYBOARD_PASSWORD_STATUS_SHIFT) {
		case SECURITY_STATUS_DISABLED:
			return "Disabled";
		case SECURITY_STATUS_ENABLED:
			return "Enabled";
		case SECURITY_STATUS_NOT_IMPLEMENTED:
			return "Not Implemented";
		case SECURITY_STATUS_UNKNOWN:
			return "Unknown";
		default:
			return "Undefined";
	}
}

static inline const char* lazybiosType24AdministratorPasswordStatusStr(uint8_t hardware_security_settings) {
	switch ((hardware_security_settings & ADMINISTRATOR_PASSWORD_STATUS_MASK) >>
		ADMINISTRATOR_PASSWORD_STATUS_SHIFT) {
		case SECURITY_STATUS_DISABLED:
			return "Disabled";
		case SECURITY_STATUS_ENABLED:
			return "Enabled";
		case SECURITY_STATUS_NOT_IMPLEMENTED:
			return "Not Implemented";
		case SECURITY_STATUS_UNKNOWN:
			return "Unknown";
		default:
			return "Undefined";
	}
}

static inline const char* lazybiosType24FrontPanelResetStatusStr(uint8_t hardware_security_settings) {
	switch (hardware_security_settings & FRONT_PANEL_RESET_STATUS_MASK) {
		case SECURITY_STATUS_DISABLED:
			return "Disabled";
		case SECURITY_STATUS_ENABLED:
			return "Enabled";
		case SECURITY_STATUS_NOT_IMPLEMENTED:
			return "Not Implemented";
		case SECURITY_STATUS_UNKNOWN:
			return "Unknown";
		default:
			return "Undefined";
	}
}

void lazybiosFreeType24(lazybiosType24Array_t* Type24) {
    if (!Type24) return;

    free(Type24->entries);

    free(Type24);
}

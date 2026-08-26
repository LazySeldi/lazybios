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
 * @file type23.c
 * @brief Implements parsing and decoding for SMBIOS Type 23 System Reset.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* File-local decoders; their output is stored in each record's `decoded`. */
static const char* lazybiosType23BootOptionStr(uint8_t capabilities);
static const char* lazybiosType23BootOptionOnLimitStr(uint8_t capabilities);
static size_t lazybiosType23CapabilitiesStr(uint8_t capabilities, char* buf, size_t buf_len);

// Fields
#define CAPABILITIES 0x04
#define RESET_COUNT 0x05
#define RESET_LIMIT 0x07
#define TIMER_INTERVAL 0x09
#define TIMEOUT 0x0B

// Capability Masks
#define RESET_STATUS_MASK 0x01
#define BOOT_OPTION_MASK 0x06
#define BOOT_OPTION_SHIFT 1
#define BOOT_OPTION_ON_LIMIT_MASK 0x18
#define BOOT_OPTION_ON_LIMIT_SHIFT 3
#define WATCHDOG_TIMER_MASK 0x20

// Boot Options
#define BOOT_OPTION_RESERVED 0x00
#define BOOT_OPTION_OPERATING_SYSTEM 0x01
#define BOOT_OPTION_SYSTEM_UTILITIES 0x02
#define BOOT_OPTION_DO_NOT_REBOOT 0x03

lazybiosType23Array_t* lazybiosGetType23(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType23Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_SYSTEM_RESET);
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

		if (type == SMBIOS_TYPE_SYSTEM_RESET) {
			if (index >= count) break;
			lazybiosType23_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READU8(current, capabilities, len, CAPABILITIES, p);
			READU16(current, reset_count, len, RESET_COUNT, p);
			READU16(current, reset_limit, len, RESET_LIMIT, p);
			READU16(current, timer_interval, len, TIMER_INTERVAL, p);
			READU16(current, timeout, len, TIMEOUT, p);

			char decbuf[LAZYBIOS_DECODER_BUF_SIZE];
			if (LAZYBIOS_FIELD_STATUS(current, capabilities) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType23CapabilitiesStr(current->capabilities, decbuf, sizeof(decbuf));
				current->decoded.capabilities = lazybiosDup(decbuf);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

static const char* lazybiosType23BootOptionStr(uint8_t capabilities) {
	switch ((capabilities & BOOT_OPTION_MASK) >> BOOT_OPTION_SHIFT) {
		case BOOT_OPTION_RESERVED:
			return "Reserved";
		case BOOT_OPTION_OPERATING_SYSTEM:
			return "Operating System";
		case BOOT_OPTION_SYSTEM_UTILITIES:
			return "System Utilities";
		case BOOT_OPTION_DO_NOT_REBOOT:
			return "Do Not Reboot";
		default:
			return "Undefined";
	}
}

static const char* lazybiosType23BootOptionOnLimitStr(uint8_t capabilities) {
	switch ((capabilities & BOOT_OPTION_ON_LIMIT_MASK) >> BOOT_OPTION_ON_LIMIT_SHIFT) {
		case BOOT_OPTION_RESERVED:
			return "Reserved";
		case BOOT_OPTION_OPERATING_SYSTEM:
			return "Operating System";
		case BOOT_OPTION_SYSTEM_UTILITIES:
			return "System Utilities";
		case BOOT_OPTION_DO_NOT_REBOOT:
			return "Do Not Reboot";
		default:
			return "Undefined";
	}
}

static size_t lazybiosType23CapabilitiesStr(uint8_t capabilities, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	snprintf(buf, buf_len, "%s, %s, Boot Option: %s, Boot Option on Limit: %s",
		(capabilities & RESET_STATUS_MASK) ? "Reset Enabled" : "Reset Disabled",
		(capabilities & WATCHDOG_TIMER_MASK) ? "Watchdog Timer Present" : "Watchdog Timer Not Present",
		lazybiosType23BootOptionStr(capabilities),
		lazybiosType23BootOptionOnLimitStr(capabilities));
	return buf ? strlen(buf) : 0;
}

void lazybiosFreeType23(lazybiosType23Array_t* Type23) {
    if (!Type23) return;

	for (size_t i = 0; i < Type23->count; i++) {
		free(Type23->entries[i].decoded.capabilities);
	}

    free(Type23->entries);

    free(Type23);
}

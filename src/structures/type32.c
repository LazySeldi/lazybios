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
 * @file type32.c
 * @brief Implements parsing and decoding for SMBIOS Type 32 System Boot Information.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>
#include <string.h>

// Fields
#define RESERVED 0x04
#define RESERVED_SIZE 6
#define BOOT_STATUS 0x0A
#define ADDITIONAL_DATA 0x0B

// Boot Status Values
#define BOOT_STATUS_NO_ERRORS 0x00
#define BOOT_STATUS_NO_BOOTABLE_MEDIA 0x01
#define BOOT_STATUS_NORMAL_OS_FAILED 0x02
#define BOOT_STATUS_FIRMWARE_HARDWARE_FAILURE 0x03
#define BOOT_STATUS_OS_HARDWARE_FAILURE 0x04
#define BOOT_STATUS_USER_REQUESTED 0x05
#define BOOT_STATUS_SECURITY_VIOLATION 0x06
#define BOOT_STATUS_PREVIOUSLY_REQUESTED_IMAGE 0x07
#define BOOT_STATUS_WATCHDOG_EXPIRED 0x08

lazybiosType32_t* lazybiosGetType32(lazybiosType32_t* Type32, size_t* type32_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_SYSTEM_BOOT_INFORMATION);
	size_t index = 0;

	Type32 = calloc(count, sizeof(lazybiosType32_t));
	if (!Type32) return NULL;
	if (count == 0) {
		*type32_count = 0;
		return Type32;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_SYSTEM_BOOT_INFORMATION) {
			if (index >= count) break;
			lazybiosType32_t* current = &Type32[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);

			if ((size_t)len >= RESERVED + RESERVED_SIZE) {
				memcpy(current->reserved, p + RESERVED, RESERVED_SIZE);
				LAZYBIOS_MARK_PRESENT(current, reserved);
			}

			READU8(current, boot_status, len, BOOT_STATUS, p);

			if (LAZYBIOS_FIELD_STATUS(current, boot_status) == LAZYBIOS_FIELD_PRESENT) {
				current->additional_data_size = len - ADDITIONAL_DATA;
				LAZYBIOS_MARK_PRESENT(current, additional_data_size);

				if (current->additional_data_size > 0) {
					current->additional_data = malloc(current->additional_data_size);
					if (!current->additional_data) {
						lazybiosFreeType32(Type32, index + 1);
						return NULL;
					}
					memcpy(current->additional_data, p + ADDITIONAL_DATA, current->additional_data_size);
				}
				LAZYBIOS_MARK_PRESENT(current, additional_data);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type32_count = index;
	return Type32;
}


// Boot Status
const char* lazybiosType32BootStatusStr(uint8_t boot_status) {
	switch (boot_status) {
		case BOOT_STATUS_NO_ERRORS:
			return "No Errors Detected";
		case BOOT_STATUS_NO_BOOTABLE_MEDIA:
			return "No Bootable Media";
		case BOOT_STATUS_NORMAL_OS_FAILED:
			return "Normal Operating System Failed to Load";
		case BOOT_STATUS_FIRMWARE_HARDWARE_FAILURE:
			return "Firmware-detected Hardware Failure";
		case BOOT_STATUS_OS_HARDWARE_FAILURE:
			return "Operating System-detected Hardware Failure";
		case BOOT_STATUS_USER_REQUESTED:
			return "User-requested Boot";
		case BOOT_STATUS_SECURITY_VIOLATION:
			return "System Security Violation";
		case BOOT_STATUS_PREVIOUSLY_REQUESTED_IMAGE:
			return "Previously Requested Image";
		case BOOT_STATUS_WATCHDOG_EXPIRED:
			return "System Watchdog Timer Expired";
		default:
			if (boot_status <= 127) return "Reserved";
			if (boot_status <= 191) return "Vendor/OEM-specific";
			return "Product-specific";
	}
}

void lazybiosFreeType32(lazybiosType32_t* Type32, size_t type32_count) {
    if (!Type32) return;

	for (size_t i = 0; i < type32_count; i++) free(Type32[i].additional_data);

    free(Type32);
}

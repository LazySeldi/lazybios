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
 * @file dell_type177.c
 * @brief Implements parsing for DELL OEM SMBIOS Type 177 Information.
 * @author LazySeldi
 */

#include "lazybios_internal.h"
#include "lazybios/structures/oem/dell/dell_type177.h"
#include <stdlib.h>

#define BIOS_FLAGS 0x04
#define ACPI_WMI_SUPPORTED_BIT 1

lazybiosOemDellType177Array_t* lazybiosGetOemDellType177(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosOemDellType177Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;
	const size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_OEM_DELL_TYPE177);
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

		if (type == SMBIOS_OEM_DELL_TYPE177) {
			lazybiosOemDellType177_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;
			const uint8_t* structure_end = DMINext(p, end);
		    (void)structure_end;

		    // The 64-bit flags word at 0x04 is kept raw; the bit of interest is decoded.
		    if (len >= 0x0C) {
		        memcpy(&current->bios_flags, p + BIOS_FLAGS, sizeof(current->bios_flags));
		        LAZYBIOS_MARK_PRESENT(current, bios_flags);
		    } else {
		        current->bios_flags = 0;
		        LAZYBIOS_MARK_ABSENT(current, bios_flags);
		    }

		    /*
		     * "No" is a real answer, not a missing field, so both outcomes are
		     * present whenever the flags word itself was readable.
		     */
		    current->decoded.acpi_wmi_supported =
		        (current->bios_flags & (1ULL << ACPI_WMI_SUPPORTED_BIT)) ? "Yes" : "No";
		    if (LAZYBIOS_FIELD_STATUS(current, bios_flags) == LAZYBIOS_FIELD_PRESENT) {
		        LAZYBIOS_MARK_PRESENT(current, acpi_wmi_supported);
		    } else {
		        LAZYBIOS_MARK_ABSENT(current, acpi_wmi_supported);
		    }

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

void lazybiosFreeOemDellType177(lazybiosOemDellType177Array_t* DellType177) {
	    if (!DellType177) return;

	free(DellType177->entries);

	free(DellType177);
}

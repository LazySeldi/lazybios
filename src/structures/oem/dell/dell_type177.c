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

#define ACPI_WMI_SUPPORTED 0x04

lazybiosOemDellType177_t* lazybiosGetOemDellType177(lazybiosOemDellType177_t* DELLType177, size_t* delltype177_count, lazybiosDMI_t* DMIData) {
	if (delltype177_count) *delltype177_count = 0;
	if (!delltype177_count || !DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;
	const size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_OEM_DELL_TYPE177);
	size_t index = 0;

	DELLType177 = calloc(count, sizeof(*DELLType177));
	if (!DELLType177) return NULL;

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_OEM_DELL_TYPE177) {
			lazybiosOemDellType177_t* current = &DELLType177[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);
		    (void)structure_end;

		    // Read the 64-bit flags from offset 0x04
		    uint64_t flags = 0;
		    if (len >= 0x0C) {
		        memcpy(&flags, p + ACPI_WMI_SUPPORTED, sizeof(flags));
		    }

		    current->acpi_wmi_supported = (flags & (1ULL << 1)) ? "Yes" : "No";
		    if (flags & (1ULL << 1)) {
		        current->acpi_wmi_supported = "Yes";
		        LAZYBIOS_MARK_PRESENT(current, acpi_wmi_supported);
		    } else {
		        current->acpi_wmi_supported = "No";
		        LAZYBIOS_MARK_ABSENT(current, acpi_wmi_supported);
		    }

			index++;
		}
		p = DMINext(p, end);
	}
	*delltype177_count = index;
	return DELLType177;
}

void lazybiosFreeOemDellType177(lazybiosOemDellType177_t* DELLType177, size_t delltype177_count) {
	(void)delltype177_count;
    if (!DELLType177) return;

	free(DELLType177);
}

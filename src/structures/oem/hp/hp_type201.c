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
 * @file hp_type201.c
 * @brief Implements parsing for HP OEM SMBIOS Type 201 Information.
 * @author LazySeldi
 *
 * @note Experimental and untested.
 */
#include "lazybios_internal.h"
#include "lazybios/structures/oem/hp/hp_type201.h"
#include <stdio.h>
#include <stdlib.h>

// Fields
#define RACK_NAME 0x04
#define HP201_OFFSET_ENCLOSURE_NAME     0x05

lazybiosOemHpType201_t* lazybiosGetOemHpType201(lazybiosOemHpType201_t* HPType201, size_t* hp_type201_count, lazybiosDMI_t* DMIData) {
	if (hp_type201_count) *hp_type201_count = 0;
	if (!hp_type201_count || !DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;
	const size_t count = lazybiosCountStructsByType(DMIData, 201);
	size_t index = 0;

	HPType201 = calloc(count, sizeof(*HPType201));
	if (!HPType201) return NULL;

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == 201) {
			lazybiosOemHpType201_t* current = &HPType201[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, rack_name, len, RACK_NAME, p, structure_end);

			index++;
		}
		p = DMINext(p, end);
	}
	*hp_type201_count = index;
	return HPType201;
}



void lazybiosFreeOemHpType201(lazybiosOemHpType201_t* HPType201, size_t hp_type201_count) {
	(void)hp_type201_count;
    if (!HPType201) return;

	free(HPType201);
}

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
 * @file hp_type204.c
 * @brief Implements parsing for HP OEM SMBIOS Type 204 Information.
 * @author LazySeldi
 *
 * @note Experimental and untested.
 */
#include "lazybios_internal.h"
#include "lazybios/structures/oem/hp/hp_type204.h"
#include <stdio.h>
#include <stdlib.h>

// Fields
#define RACK_NAME 0x04
#define ENCLOSURE_NAME 0x05
#define ENCLOSURE_MODEL 0x06
#define ENCLOSURE_BAYS 0x07
#define ENCLOSURE_SERIAL 0x08
#define SERVER_BAY 0x08
#define BAYS_FILLED 0x09

lazybiosOemHpType204_t* lazybiosGetOemHpType204(lazybiosOemHpType204_t* HPType204, size_t* hptype204_count, lazybiosDMI_t* DMIData) {
	if (hptype204_count) *hptype204_count = 0;
	if (!hptype204_count || !DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;
	const size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_OEM_HP_TYPE204);
	size_t index = 0;

	HPType204 = calloc(count, sizeof(*HPType204));
	if (!HPType204) return NULL;

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_OEM_HP_TYPE204) {
			lazybiosOemHpType204_t* current = &HPType204[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, rack_name, len, RACK_NAME, p, structure_end);
		    READSTR(current, enclosure_name, len, ENCLOSURE_NAME, p, structure_end);
		    READSTR(current, enclosure_model, len, ENCLOSURE_MODEL, p, structure_end);
		    READU8(current, enclosure_bays, len, ENCLOSURE_BAYS, p);
		    READSTR(current, enclosure_serial, len, ENCLOSURE_SERIAL, p, structure_end);
		    READSTR(current, server_bay, len, SERVER_BAY, p, structure_end);
		    READU8(current, bays_filled, len, BAYS_FILLED, p);

			index++;
		}
		p = DMINext(p, end);
	}
	*hptype204_count = index;
	return HPType204;
}

void lazybiosFreeOemHpType204(lazybiosOemHpType204_t* HPType204, size_t hptype204_count) {
	(void)hptype204_count;
    if (!HPType204) return;

	free(HPType204);
}

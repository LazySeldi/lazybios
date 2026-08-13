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
 * @file type19.c
 * @brief Implements parsing for SMBIOS Type 19 Memory Array Mapped Address.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>

// Fields
#define STARTING_ADDRESS 0x04
#define ENDING_ADDRESS 0x08
#define MEMORY_ARRAY_HANDLE 0x0C
#define PARTITION_WIDTH 0x0E
#define EXTENDED_STARTING_ADDRESS 0x0F
#define EXTENDED_ENDING_ADDRESS 0x17

// Address Selection
#define USE_EXTENDED_ADDRESS 0xFFFFFFFFU

lazybiosType19_t* lazybiosGetType19(lazybiosType19_t* Type19, size_t* type19_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS);
	size_t index = 0;

	Type19 = calloc(count, sizeof(lazybiosType19_t));
	if (!Type19) return NULL;
	if (count == 0) {
		*type19_count = 0;
		return Type19;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS) {
			if (index >= count) break;
			lazybiosType19_t* current = &Type19[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);

			READU32(current, starting_address, len, STARTING_ADDRESS, p);
			READU32(current, ending_address, len, ENDING_ADDRESS, p);
			READU16(current, memory_array_handle, len, MEMORY_ARRAY_HANDLE, p);
			if (current->memory_array_handle == 0xFFFF) LAZYBIOS_MARK_ABSENT(current, memory_array_handle);
			READU8(current, partition_width, len, PARTITION_WIDTH, p);

			if (lazybiosIsVersionPlus(DMIData, 2, 7)) {
				READU64(current, extended_starting_address, len, EXTENDED_STARTING_ADDRESS, p);
				READU64(current, extended_ending_address, len, EXTENDED_ENDING_ADDRESS, p);
			} else {
				current->extended_starting_address = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, extended_starting_address);
				current->extended_ending_address = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, extended_ending_address);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type19_count = index;
	return Type19;
}

uint64_t lazybiosType19StartingAddressBytes(uint32_t starting_address, uint64_t extended_starting_address) {
	if (starting_address == USE_EXTENDED_ADDRESS) return extended_starting_address;
	return (uint64_t)starting_address * 1024;
}

uint64_t lazybiosType19EndingAddressBytes(uint32_t ending_address, uint64_t extended_ending_address) {
	if (ending_address == USE_EXTENDED_ADDRESS) return extended_ending_address;
	return (uint64_t)ending_address * 1024 + 1023;
}

void lazybiosFreeType19(lazybiosType19_t* Type19, size_t type19_count) {
    (void)type19_count;
    if (!Type19) return;

    free(Type19);
}

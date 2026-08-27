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
 * @file type14.c
 * @brief Implements parsing for SMBIOS Type 14 Group Associations.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>
#include <string.h>

// Fields
#define GROUP_NAME 0x04
#define ITEMS 0x05
#define ITEM_SIZE 3

lazybiosType14Array_t* lazybiosGetType14(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType14Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_GROUP_ASSOCIATIONS);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_GROUP_ASSOCIATIONS].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_GROUP_ASSOCIATIONS].first;
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
		const uint8_t* structure_end = DMINext(p, end);

		if (type == SMBIOS_TYPE_GROUP_ASSOCIATIONS) {
			if (index >= count) break;
			lazybiosType14_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READSTR(current, group_name, len, GROUP_NAME, p, structure_end);

			if (len >= ITEMS && (len - ITEMS) % ITEM_SIZE == 0) {
				current->item_count = (len - ITEMS) / ITEM_SIZE;
				LAZYBIOS_MARK_PRESENT(current, item_count);

				if (current->item_count > 0) {
					current->items = calloc(current->item_count, sizeof(lazybiosType14Item_t));
					if (!current->items) {
						out->count = index + 1;
						lazybiosFreeType14(out);
						return NULL;
					}

					for (size_t i = 0; i < current->item_count; i++) {
						const size_t item_offset = ITEMS + (i * ITEM_SIZE);
						current->items[i].item_type = p[item_offset];
						memcpy(&current->items[i].item_handle, p + item_offset + 1, sizeof(uint16_t));
						LAZYBIOS_MARK_PRESENT(&current->items[i], item_type);
						LAZYBIOS_MARK_PRESENT(&current->items[i], item_handle);
						if (current->items[i].item_handle == 0xFFFF) {
							LAZYBIOS_MARK_ABSENT(&current->items[i], item_handle);
						}
					}
				}
				LAZYBIOS_MARK_PRESENT(current, items);
			} else {
				current->item_count = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, item_count);
				current->items = NULL;
				LAZYBIOS_MARK_UNREACHABLE(current, items);
				LAZYBIOS_MARK_ABSENT(current, item_count);
				LAZYBIOS_MARK_ABSENT(current, items);
			}

			index++;
		}
		p = structure_end;
	}
	out->count = index;
	return out;
}

void lazybiosFreeType14(lazybiosType14Array_t* Type14) {
    if (!Type14) return;

    for (size_t i = 0; i < Type14->count; i++) free(Type14->entries[i].items);

    free(Type14->entries);

    free(Type14);
}

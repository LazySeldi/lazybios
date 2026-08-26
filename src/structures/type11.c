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
 * @file type11.c
 * @brief Implements parsing for SMBIOS Type 11 OEM Strings.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>

// Fields
#define COUNT 0x04

lazybiosType11Array_t* lazybiosGetType11(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType11Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_OEM_STRINGS);
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

		if (type == SMBIOS_TYPE_OEM_STRINGS) {
			if (index >= count) break;
			lazybiosType11_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;
			const uint8_t* structure_end = DMINext(p, end);

			READU8(current, string_count, len, COUNT, p);

			if (LAZYBIOS_FIELD_STATUS(current, string_count) == LAZYBIOS_FIELD_PRESENT && current->string_count > 0) {
				current->strings = calloc(current->string_count, sizeof(*current->strings));
				if (current->strings) {
					LAZYBIOS_MARK_PRESENT(current, strings);
					for (size_t i = 0; i < current->string_count; i++) {
						current->strings[i] = DMIString(p, len, (uint8_t)(i + 1), structure_end);
						if (!current->strings[i] || current->strings[i][0] == '\0') {
							current->strings[i] = NULL;
							LAZYBIOS_MARK_ABSENT(current, strings);
						}
					}
				} else {
					out->count = index + 1;
					lazybiosFreeType11(out);
					return NULL;
				}
			} else if (LAZYBIOS_FIELD_STATUS(current, string_count) == LAZYBIOS_FIELD_PRESENT) {
				LAZYBIOS_MARK_PRESENT(current, strings);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

void lazybiosFreeType11(lazybiosType11Array_t* Type11) {
    if (!Type11) return;

    for (size_t i = 0; i < Type11->count; i++) {
        free(Type11->entries[i].strings);
    }

    free(Type11->entries);

    free(Type11);
}

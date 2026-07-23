/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This file is part of lazybios.
 *
 * lazybios is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * lazybios is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with lazybios. If not, see <https://www.gnu.org/licenses/>.
 */
/**
 * @file type11.c
 * @brief Implements parsing for SMBIOS Type 11 OEM Strings.
 * @author LazySeldi
 */

//
// Type 11 ( OEM Strings )
//

#include "lazybios_internal.h"
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define COUNT 0x04
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 11 OEM Strings structures.
 *
 * @param Type11 Existing Type 11 array pointer value; it is not dereferenced or released.
 * @param type11_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 11 array, or NULL on failure.
 */
lazybiosType11_t* lazybiosGetType11(lazybiosType11_t* Type11, size_t* type11_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_OEM_STRINGS);
	size_t index = 0;

	Type11 = calloc(count, sizeof(lazybiosType11_t));
	if (!Type11) return NULL;
	if (count == 0) {
		*type11_count = 0;
		return Type11;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_OEM_STRINGS) {
			if (index >= count) break;
			lazybiosType11_t* current = &Type11[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READU8(current, string_count, len, COUNT, p);

			if (LAZYBIOS_FIELD_STATUS(current, string_count) == LAZYBIOS_FIELD_PRESENT && current->string_count > 0) {
				current->strings = calloc(current->string_count, sizeof(char*));
				if (current->strings) {
					LAZYBIOS_MARK_PRESENT(current, strings);
					for (size_t i = 0; i < current->string_count; i++) {
						current->strings[i] = DMIString(p, len, (uint8_t)(i + 1), structure_end);
						if (!current->strings[i] || current->strings[i][0] == '\0') {
							free(current->strings[i]);
							current->strings[i] = NULL;
							LAZYBIOS_MARK_ABSENT(current, strings);
						}
					}
				} else {
					lazybiosFreeType11(Type11, count);
					return NULL;
				}
			} else if (LAZYBIOS_FIELD_STATUS(current, string_count) == LAZYBIOS_FIELD_PRESENT) {
				LAZYBIOS_MARK_PRESENT(current, strings);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type11_count = index;
	return Type11;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 11 structures.
 *
 * @param Type11 Type 11 array to release.
 * @param type11_count Number of elements in Type11.
 */
void lazybiosFreeType11(lazybiosType11_t* Type11, size_t type11_count) {
	if (!Type11) return;

	for (size_t i = 0; i < type11_count; i++) {
		for (size_t j = 0; j < Type11[i].string_count; j++) {
			free(Type11[i].strings[j]);
		}
		free(Type11[i].strings);
	}

	free(Type11);
}

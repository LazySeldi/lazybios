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
 * @file type13.c
 * @brief Implements parsing and decoding for SMBIOS Type 13 Firmware Language Information.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>

/* File-local decoders; their results are stored in each record's `decoded`. */
static inline const char* lazybiosType13LanguageFormatStr(uint8_t flags);

// Fields
#define INSTALLABLE_LANGUAGES 0x04
#define FLAGS 0x05
#define CURRENT_LANGUAGE 0x15

// Flag Masks
#define LANGUAGE_FORMAT_MASK 0x01

lazybiosType13Array_t* lazybiosGetType13(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType13Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_FIRMWARE_LANGUAGE_INFORMATION);
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

		if (type == SMBIOS_TYPE_FIRMWARE_LANGUAGE_INFORMATION) {
			if (index >= count) break;
			lazybiosType13_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;
			const uint8_t* structure_end = DMINext(p, end);

			READU8(current, installable_languages, len, INSTALLABLE_LANGUAGES, p);
			if (lazybiosIsVersionPlus(DMIData, 2, 1)) {
				READU8(current, flags, len, FLAGS, p);
			} else {
				current->flags = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, flags);
			}
			READSTR(current, current_language, len, CURRENT_LANGUAGE, p, structure_end);
			if (LAZYBIOS_FIELD_STATUS(current, current_language) == LAZYBIOS_FIELD_PRESENT &&
				LAZYBIOS_FIELD_STATUS(current, installable_languages) == LAZYBIOS_FIELD_PRESENT &&
				p[CURRENT_LANGUAGE] > current->installable_languages) {
				current->current_language = NULL;
				LAZYBIOS_MARK_UNREACHABLE(current, current_language);
				LAZYBIOS_MARK_ABSENT(current, current_language);
			}

			if (LAZYBIOS_FIELD_STATUS(current, installable_languages) == LAZYBIOS_FIELD_PRESENT &&
				current->installable_languages > 0) {
				current->languages = calloc(current->installable_languages, sizeof(*current->languages));
				if (current->languages) {
					LAZYBIOS_MARK_PRESENT(current, languages);
					for (size_t i = 0; i < current->installable_languages; i++) {
						current->languages[i] = DMIString(p, len, (uint8_t)(i + 1), structure_end);
						if (!current->languages[i] || current->languages[i][0] == '\0') {
							current->languages[i] = NULL;
							LAZYBIOS_MARK_ABSENT(current, languages);
						}
					}
				} else {
					out->count = index + 1;
					lazybiosFreeType13(out);
					return NULL;
				}
			} else if (LAZYBIOS_FIELD_STATUS(current, installable_languages) == LAZYBIOS_FIELD_PRESENT) {
				LAZYBIOS_MARK_PRESENT(current, languages);
			}

			current->decoded.flags = lazybiosType13LanguageFormatStr(current->flags);

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

static inline const char* lazybiosType13LanguageFormatStr(uint8_t flags) {
	return (flags & LANGUAGE_FORMAT_MASK) ? "Abbreviated" : "Long";
}

void lazybiosFreeType13(lazybiosType13Array_t* Type13) {
    if (!Type13) return;

    for (size_t i = 0; i < Type13->count; i++) free(Type13->entries[i].languages);
    
    free(Type13->entries);
    
    free(Type13);
}

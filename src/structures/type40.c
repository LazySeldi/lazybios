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
 * @file type40.c
 * @brief Implements parsing for SMBIOS Type 40 Additional Information.
 * @author LazySeldi
 */

//
// Type 40 ( Additional Information )
//

#include "lazybios_internal.h"
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define ADDITIONAL_INFORMATION_ENTRY_COUNT 0x04
#define ADDITIONAL_INFORMATION_ENTRIES 0x05

// Entry Fields
#define ENTRY_LENGTH 0x00
#define ENTRY_REFERENCED_HANDLE 0x01
#define ENTRY_REFERENCED_OFFSET 0x03
#define ENTRY_STRING 0x04
#define ENTRY_VALUE 0x05
#define MINIMUM_ENTRY_LENGTH 0x06
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 40 Additional Information structures.
 *
 * @param Type40 Existing Type 40 array pointer value; it is not dereferenced or released.
 * @param type40_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 40 array, or NULL on failure.
 */
lazybiosType40_t* lazybiosGetType40(lazybiosType40_t* Type40, size_t* type40_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_ADDITIONAL_INFORMATION);
	size_t index = 0;

	Type40 = calloc(count, sizeof(lazybiosType40_t));
	if (!Type40) return NULL;
	if (count == 0) {
		*type40_count = 0;
		return Type40;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_ADDITIONAL_INFORMATION) {
			if (index >= count) break;
			lazybiosType40_t* current = &Type40[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READU8(current, additional_information_entry_count, len, ADDITIONAL_INFORMATION_ENTRY_COUNT, p);

			if (current->field_status.additional_information_entry_count == LAZYBIOS_FIELD_PRESENT) {
				size_t entry_offset = ADDITIONAL_INFORMATION_ENTRIES;
				int entries_valid = 1;

				for (size_t i = 0; i < current->additional_information_entry_count; i++) {
					if (entry_offset >= len) {
						entries_valid = 0;
						break;
					}

					uint8_t entry_length = p[entry_offset + ENTRY_LENGTH];
					if (entry_length < MINIMUM_ENTRY_LENGTH || (size_t)entry_length > (size_t)len - entry_offset) {
						entries_valid = 0;
						break;
					}
					entry_offset += entry_length;
				}

				if (entries_valid) {
					if (current->additional_information_entry_count > 0) {
						current->additional_information_entries = calloc(
							current->additional_information_entry_count, sizeof(lazybiosType40Entry_t));
						if (!current->additional_information_entries) {
							lazybiosFreeType40(Type40, count);
							return NULL;
						}

						entry_offset = ADDITIONAL_INFORMATION_ENTRIES;
						for (size_t i = 0; i < current->additional_information_entry_count; i++) {
							lazybiosType40Entry_t* entry = &current->additional_information_entries[i];
							entry->entry_length = p[entry_offset + ENTRY_LENGTH];
							memcpy(&entry->referenced_handle, p + entry_offset + ENTRY_REFERENCED_HANDLE,
								   sizeof(uint16_t));
							entry->referenced_offset = p[entry_offset + ENTRY_REFERENCED_OFFSET];
							uint8_t string_number = p[entry_offset + ENTRY_STRING];
							entry->string = DMIString(p, len, string_number, structure_end);
							entry->value_length = entry->entry_length - ENTRY_VALUE;
							if (entry->value_length > 0) {
								entry->value = malloc(entry->value_length);
								if (!entry->value) {
									lazybiosFreeType40(Type40, count);
									return NULL;
								}
								memcpy(entry->value, p + entry_offset + ENTRY_VALUE, entry->value_length);
							}

							LAZYBIOS_MARK_PRESENT(entry, entry_length);
							LAZYBIOS_MARK_PRESENT(entry, referenced_handle);
							if (entry->referenced_handle == 0xFFFF) {
								LAZYBIOS_MARK_ABSENT(entry, referenced_handle);
							}
							LAZYBIOS_MARK_PRESENT(entry, referenced_offset);
							if (entry->string) {
								LAZYBIOS_MARK_PRESENT(entry, string);
							} else {
								LAZYBIOS_MARK_ABSENT(entry, string);
							}
							LAZYBIOS_MARK_PRESENT(entry, value);
							entry_offset += entry->entry_length;
						}
					}
					LAZYBIOS_MARK_PRESENT(current, additional_information_entries);
				} else {
					LAZYBIOS_MARK_ABSENT(current, additional_information_entries);
				}
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type40_count = index;
	return Type40;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 40 structures.
 *
 * @param Type40 Type 40 array to release.
 * @param type40_count Number of elements in Type40.
 */
void lazybiosFreeType40(lazybiosType40_t* Type40, size_t type40_count) {
	if (!Type40) return;

	for (size_t i = 0; i < type40_count; i++) {
		if (Type40[i].additional_information_entries) {
			for (size_t j = 0; j < Type40[i].additional_information_entry_count; j++) {
				free(Type40[i].additional_information_entries[j].string);
				free(Type40[i].additional_information_entries[j].value);
			}
		}
		free(Type40[i].additional_information_entries);
	}
	free(Type40);
}

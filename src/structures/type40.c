/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type40.c
 * @brief Implements parsing for SMBIOS Type 40 Additional Information.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>
#include <string.h>

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

lazybiosType40Array_t* lazybiosGetType40(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType40Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_ADDITIONAL_INFORMATION);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_ADDITIONAL_INFORMATION].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_ADDITIONAL_INFORMATION].first;
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

		if (type == SMBIOS_TYPE_ADDITIONAL_INFORMATION) {
			if (index >= count) break;
			lazybiosType40_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

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
							out->count = index + 1;
							lazybiosFreeType40(out);
							return NULL;
						}

						entry_offset = ADDITIONAL_INFORMATION_ENTRIES;
						for (size_t i = 0; i < current->additional_information_entry_count; i++) {
							lazybiosType40Entry_t* entry = &current->additional_information_entries[i];
							entry->entry_length = p[entry_offset + ENTRY_LENGTH];
							entry->referenced_handle = (uint16_t)lazybiosReadLE(
								p + entry_offset + ENTRY_REFERENCED_HANDLE, sizeof(uint16_t));
							entry->referenced_offset = p[entry_offset + ENTRY_REFERENCED_OFFSET];
							uint8_t string_number = p[entry_offset + ENTRY_STRING];
							entry->string = DMIString(p, len, string_number, structure_end);
							entry->value_length = entry->entry_length - ENTRY_VALUE;
							if (entry->value_length > 0) {
								entry->value = malloc(entry->value_length);
								if (!entry->value) {
									out->count = index + 1;
									lazybiosFreeType40(out);
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
		p = structure_end;
	}
	out->count = index;
	return out;
}

void lazybiosFreeType40(lazybiosType40Array_t* Type40) {
	if (!Type40) return;

	for (size_t i = 0; i < Type40->count; i++) {
		if (Type40->entries[i].additional_information_entries) {
			for (size_t j = 0; j < Type40->entries[i].additional_information_entry_count; j++) {
				free(Type40->entries[i].additional_information_entries[j].value);
			}
		}
		free(Type40->entries[i].additional_information_entries);
	}
	free(Type40->entries);
	free(Type40);
}

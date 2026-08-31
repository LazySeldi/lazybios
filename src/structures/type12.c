/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type12.c
 * @brief Implements parsing for SMBIOS Type 12 System Configuration Options.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>

// Fields
#define COUNT 0x04

lazybiosType12Array_t* lazybiosGetType12(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType12Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_SYSTEM_CONFIGURATION_OPTIONS);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_SYSTEM_CONFIGURATION_OPTIONS].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_SYSTEM_CONFIGURATION_OPTIONS].first;
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

		if (type == SMBIOS_TYPE_SYSTEM_CONFIGURATION_OPTIONS) {
			if (index >= count) break;
			lazybiosType12_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READU8(current, option_count, len, COUNT, p);

			if (LAZYBIOS_FIELD_STATUS(current, option_count) == LAZYBIOS_FIELD_PRESENT && current->option_count > 0) {
				current->options = calloc(current->option_count, sizeof(*current->options));
				if (current->options) {
					LAZYBIOS_MARK_PRESENT(current, options);
					for (size_t i = 0; i < current->option_count; i++) {
						current->options[i] = DMIString(p, len, (uint8_t)(i + 1), structure_end);
						if (!current->options[i] || current->options[i][0] == '\0') {
							current->options[i] = NULL;
							LAZYBIOS_MARK_ABSENT(current, options);
						}
					}
				} else {
					out->count = index + 1;
					lazybiosFreeType12(out);
					return NULL;
				}
			} else if (LAZYBIOS_FIELD_STATUS(current, option_count) == LAZYBIOS_FIELD_PRESENT) {
				LAZYBIOS_MARK_PRESENT(current, options);
			}

			index++;
		}
		p = structure_end;
	}
	out->count = index;
	return out;
}

void lazybiosFreeType12(lazybiosType12Array_t* Type12) {
    if (!Type12) return;

    for (size_t i = 0; i < Type12->count; i++) free(Type12->entries[i].options);

    free(Type12->entries);

    free(Type12);
}

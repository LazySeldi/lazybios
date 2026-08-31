/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type36.c
 * @brief Implements parsing for SMBIOS Type 36 Management Device Threshold Data.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>

// Fields
#define LOWER_THRESHOLD_NON_CRITICAL 0x04
#define UPPER_THRESHOLD_NON_CRITICAL 0x06
#define LOWER_THRESHOLD_CRITICAL 0x08
#define UPPER_THRESHOLD_CRITICAL 0x0A
#define LOWER_THRESHOLD_NON_RECOVERABLE 0x0C
#define UPPER_THRESHOLD_NON_RECOVERABLE 0x0E

lazybiosType36Array_t* lazybiosGetType36(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType36Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_MANAGEMENT_DEVICE_THRESHOLD_DATA);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_MANAGEMENT_DEVICE_THRESHOLD_DATA].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_MANAGEMENT_DEVICE_THRESHOLD_DATA].first;
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

		if (type == SMBIOS_TYPE_MANAGEMENT_DEVICE_THRESHOLD_DATA) {
			if (index >= count) break;
			lazybiosType36_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READU16(current, lower_threshold_non_critical, len, LOWER_THRESHOLD_NON_CRITICAL, p);
			READU16(current, upper_threshold_non_critical, len, UPPER_THRESHOLD_NON_CRITICAL, p);
			READU16(current, lower_threshold_critical, len, LOWER_THRESHOLD_CRITICAL, p);
			READU16(current, upper_threshold_critical, len, UPPER_THRESHOLD_CRITICAL, p);
			READU16(current, lower_threshold_non_recoverable, len, LOWER_THRESHOLD_NON_RECOVERABLE, p);
			READU16(current, upper_threshold_non_recoverable, len, UPPER_THRESHOLD_NON_RECOVERABLE, p);

			if (current->lower_threshold_non_critical == 0x8000) LAZYBIOS_MARK_ABSENT(current, lower_threshold_non_critical);
			if (current->upper_threshold_non_critical == 0x8000) LAZYBIOS_MARK_ABSENT(current, upper_threshold_non_critical);
			if (current->lower_threshold_critical == 0x8000) LAZYBIOS_MARK_ABSENT(current, lower_threshold_critical);
			if (current->upper_threshold_critical == 0x8000) LAZYBIOS_MARK_ABSENT(current, upper_threshold_critical);
			if (current->lower_threshold_non_recoverable == 0x8000) LAZYBIOS_MARK_ABSENT(current, lower_threshold_non_recoverable);
			if (current->upper_threshold_non_recoverable == 0x8000) LAZYBIOS_MARK_ABSENT(current, upper_threshold_non_recoverable);

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

void lazybiosFreeType36(lazybiosType36Array_t* Type36) {
    if (!Type36) return;

    free(Type36->entries);

    free(Type36);
}

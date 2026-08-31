/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type35.c
 * @brief Implements parsing for SMBIOS Type 35 Management Device Component.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>

// Fields
#define DESCRIPTION 0x04
#define MANAGEMENT_DEVICE_HANDLE 0x05
#define COMPONENT_HANDLE 0x07
#define THRESHOLD_HANDLE 0x09

lazybiosType35Array_t* lazybiosGetType35(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType35Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_MANAGEMENT_DEVICE_COMPONENT);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_MANAGEMENT_DEVICE_COMPONENT].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_MANAGEMENT_DEVICE_COMPONENT].first;
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

		if (type == SMBIOS_TYPE_MANAGEMENT_DEVICE_COMPONENT) {
			if (index >= count) break;
			lazybiosType35_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READSTR(current, description, len, DESCRIPTION, p, structure_end);
			READU16(current, management_device_handle, len, MANAGEMENT_DEVICE_HANDLE, p);
			READU16(current, component_handle, len, COMPONENT_HANDLE, p);
			READU16(current, threshold_handle, len, THRESHOLD_HANDLE, p);
			if (current->management_device_handle == 0xFFFF) LAZYBIOS_MARK_ABSENT(current, management_device_handle);
			if (current->component_handle == 0xFFFF) LAZYBIOS_MARK_ABSENT(current, component_handle);
			if (current->threshold_handle == 0xFFFF) LAZYBIOS_MARK_ABSENT(current, threshold_handle);

			index++;
		}
		p = structure_end;
	}
	out->count = index;
	return out;
}

void lazybiosFreeType35(lazybiosType35Array_t* Type35) {
    if (!Type35) return;

    free(Type35->entries);

    free(Type35);
}

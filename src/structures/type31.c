/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type31.c
 * @brief Implements parsing for SMBIOS Type 31 Boot Integrity Services Entry Point.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>

// Fields
#define CHECKSUM 0x04
#define RESERVED_1 0x05
#define RESERVED_2 0x06
#define BIS_ENTRY_POINT_16 0x08
#define BIS_ENTRY_POINT_32 0x0C
#define RESERVED_3 0x10
#define RESERVED_4 0x18

#define TYPE31_MINIMUM_LENGTH 0x1C

lazybiosType31Array_t* lazybiosGetType31(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType31Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_BOOT_INTEGRITY_SERVICES_ENTRY_POINT);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_BOOT_INTEGRITY_SERVICES_ENTRY_POINT].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_BOOT_INTEGRITY_SERVICES_ENTRY_POINT].first;
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

		if (type == SMBIOS_TYPE_BOOT_INTEGRITY_SERVICES_ENTRY_POINT) {
			if (index >= count) break;
			lazybiosType31_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READU8(current, checksum, len, CHECKSUM, p);
			READU8(current, reserved_1, len, RESERVED_1, p);
			READU16(current, reserved_2, len, RESERVED_2, p);
			READU32(current, bis_entry_point_16, len, BIS_ENTRY_POINT_16, p);
			READU32(current, bis_entry_point_32, len, BIS_ENTRY_POINT_32, p);
			READU64(current, reserved_3, len, RESERVED_3, p);
			READU32(current, reserved_4, len, RESERVED_4, p);

			if (len >= TYPE31_MINIMUM_LENGTH) {
				uint8_t sum = 0;
				for (size_t i = 0; i < len; i++) {
					sum = (uint8_t)(sum + p[i]);
				}
				current->checksum_valid = (uint8_t)(sum == 0);
				LAZYBIOS_MARK_PRESENT(current, checksum_valid);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

void lazybiosFreeType31(lazybiosType31Array_t* Type31) {
    if (!Type31) return;

    free(Type31->entries);

    free(Type31);
}

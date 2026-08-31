/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type20.c
 * @brief Implements parsing for SMBIOS Type 20 Memory Device Mapped Address.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>

/* File-local decoders; their results are stored in each record's `decoded`. */
static inline uint64_t lazybiosType20EndingAddressBytes(uint32_t ending_address, uint64_t extended_ending_address);
static inline uint64_t lazybiosType20StartingAddressBytes(uint32_t starting_address, uint64_t extended_starting_address);

// Fields
#define STARTING_ADDRESS 0x04
#define ENDING_ADDRESS 0x08
#define MEMORY_DEVICE_HANDLE 0x0C
#define MEMORY_ARRAY_MAPPED_ADDRESS_HANDLE 0x0E
#define PARTITION_ROW_POSITION 0x10
#define INTERLEAVE_POSITION 0x11
#define INTERLEAVED_DATA_DEPTH 0x12
#define EXTENDED_STARTING_ADDRESS 0x13
#define EXTENDED_ENDING_ADDRESS 0x1B

// Address Selection
#define USE_EXTENDED_ADDRESS 0xFFFFFFFFU

lazybiosType20Array_t* lazybiosGetType20(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType20Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS].first;
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

		if (type == SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS) {
			if (index >= count) break;
			lazybiosType20_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READU32(current, starting_address, len, STARTING_ADDRESS, p);
			READU32(current, ending_address, len, ENDING_ADDRESS, p);
			READU16(current, memory_device_handle, len, MEMORY_DEVICE_HANDLE, p);
			READU16(current, memory_array_mapped_address_handle, len, MEMORY_ARRAY_MAPPED_ADDRESS_HANDLE, p);
			if (current->memory_device_handle == 0xFFFF) LAZYBIOS_MARK_ABSENT(current, memory_device_handle);
			if (current->memory_array_mapped_address_handle == 0xFFFF) {
				LAZYBIOS_MARK_ABSENT(current, memory_array_mapped_address_handle);
			}
			READU8(current, partition_row_position, len, PARTITION_ROW_POSITION, p);
			READU8(current, interleave_position, len, INTERLEAVE_POSITION, p);
			READU8(current, interleaved_data_depth, len, INTERLEAVED_DATA_DEPTH, p);
			/* 0xFF means unknown for each of the three position fields. */
			if (current->partition_row_position == 0xFF) LAZYBIOS_MARK_ABSENT(current, partition_row_position);
			if (current->interleave_position == 0xFF) LAZYBIOS_MARK_ABSENT(current, interleave_position);
			if (current->interleaved_data_depth == 0xFF) LAZYBIOS_MARK_ABSENT(current, interleaved_data_depth);

			if (lazybiosIsVersionPlus(DMIData, 2, 7)) {
				READU64(current, extended_starting_address, len, EXTENDED_STARTING_ADDRESS, p);
				/* Only meaningful when the 32-bit field defers to it. */
				if (current->starting_address != USE_EXTENDED_ADDRESS)
					LAZYBIOS_MARK_ABSENT(current, extended_starting_address);
				READU64(current, extended_ending_address, len, EXTENDED_ENDING_ADDRESS, p);
				/* Only meaningful when the 32-bit field defers to it. */
				if (current->ending_address != USE_EXTENDED_ADDRESS)
					LAZYBIOS_MARK_ABSENT(current, extended_ending_address);
			} else {
				current->extended_starting_address = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, extended_starting_address);
				current->extended_ending_address = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, extended_ending_address);
			}

			current->decoded.ending_address = lazybiosType20EndingAddressBytes(current->ending_address, current->extended_ending_address);
			current->decoded.starting_address = lazybiosType20StartingAddressBytes(current->starting_address, current->extended_starting_address);

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

static inline uint64_t lazybiosType20StartingAddressBytes(uint32_t starting_address, uint64_t extended_starting_address) {
	if (starting_address == USE_EXTENDED_ADDRESS) return extended_starting_address;
	return (uint64_t)starting_address * 1024;
}

static inline uint64_t lazybiosType20EndingAddressBytes(uint32_t ending_address, uint64_t extended_ending_address) {
	if (ending_address == USE_EXTENDED_ADDRESS) return extended_ending_address;
	return (uint64_t)ending_address * 1024 + 1023;
}

void lazybiosFreeType20(lazybiosType20Array_t* Type20) {
    if (!Type20) return;

    free(Type20->entries);

    free(Type20);
}

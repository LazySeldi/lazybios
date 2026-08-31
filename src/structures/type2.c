/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type2.c
 * @brief Implements parsing and decoding for SMBIOS Type 2 Baseboard Information.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* File-local decoders; their output is stored in each record's `decoded`. */
static size_t lazybiosType2FeatureflagsStr(uint8_t feature_flags, char* buf, size_t buf_len);

/* File-local decoders; their results are stored in each record's `decoded`. */
static const char* lazybiosType2BoardTypeStr(uint8_t board_type);

// Fields
#define MANUFACTURER 0x04
#define PRODUCT 0x05
#define VERSION 0x06
#define SERIAL_NUMBER 0x07
#define ASSET_TAG 0x08
#define FEATURE_FLAGS 0x09
#define LOCATION_IN_CHASSIS 0x0A
#define CHASSIS_HANDLE 0x0B
#define BOARD_TYPE 0x0D
#define NUMBER_OF_CONTAINED_OBJECT_HANDLES 0x0E
#define CONTAINED_OBJECT_HANDLES 0x0F


// Board Type
#define BOARD_TYPE_UNKNOWN 0x01
#define BOARD_TYPE_OTHER 0x02
#define BOARD_TYPE_SERVER_BLADE 0x03
#define BOARD_TYPE_CONNECTIVITY_SWITCH 0x04
#define BOARD_TYPE_SYSTEM_MANAGEMENT_MODULE 0x05
#define BOARD_TYPE_PROCESSOR_MODULE 0x06
#define BOARD_TYPE_IO_MODULE 0x07
#define BOARD_TYPE_MEMORY_MODULE 0x08
#define BOARD_TYPE_DAUGHTER_BOARD 0x09
#define BOARD_TYPE_MOTHERBOARD 0x0A
#define BOARD_TYPE_PROCESSOR_MEMORY_MODULE 0x0B
#define BOARD_TYPE_PROCESSOR_IO_MODULE 0x0C
#define BOARD_TYPE_INTERCONNECT_BOARD 0x0D

lazybiosType2Array_t* lazybiosGetType2(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType2Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_BASEBOARD);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_BASEBOARD].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_BASEBOARD].first;
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

		if (type == SMBIOS_TYPE_BASEBOARD) {
			if (index >= count) break;
			lazybiosType2_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;
			
			READSTR(current, manufacturer, len, MANUFACTURER, p, structure_end);

			READSTR(current, product, len, PRODUCT, p, structure_end);

			READSTR(current, version, len, VERSION, p, structure_end);

			READSTR(current, serial_number, len, SERIAL_NUMBER, p, structure_end);

			READSTR(current, asset_tag, len, ASSET_TAG, p, structure_end);

			READU8(current, feature_flags, len, FEATURE_FLAGS, p);

			READSTR(current, location_in_chassis, len, LOCATION_IN_CHASSIS, p, structure_end);

			READU16(current, chassis_handle, len, CHASSIS_HANDLE, p);
			if (current->chassis_handle == 0xFFFF) LAZYBIOS_MARK_ABSENT(current, chassis_handle);

			READU8(current, board_type, len, BOARD_TYPE, p);

			READU8(current, number_of_contained_object_handles, len, NUMBER_OF_CONTAINED_OBJECT_HANDLES, p);

			if (LAZYBIOS_FIELD_STATUS(current, number_of_contained_object_handles) == LAZYBIOS_FIELD_PRESENT &&
				current->number_of_contained_object_handles > 0) {
				const size_t array_bytes = current->number_of_contained_object_handles * sizeof(uint16_t);

				if (len >= CONTAINED_OBJECT_HANDLES + array_bytes) {
					current->contained_object_handles = malloc(array_bytes);
					if (current->contained_object_handles) {
						for (size_t i = 0; i < current->number_of_contained_object_handles; i++) {
							current->contained_object_handles[i] = (uint16_t)lazybiosReadLE(
								p + CONTAINED_OBJECT_HANDLES + (i * sizeof(uint16_t)), sizeof(uint16_t));
						}
						LAZYBIOS_MARK_PRESENT(current, contained_object_handles);
					}
				} else {
					current->contained_object_handles = NULL;
					LAZYBIOS_MARK_UNREACHABLE(current, contained_object_handles);
					LAZYBIOS_MARK_ABSENT(current, contained_object_handles);
				}
			} else if (LAZYBIOS_FIELD_STATUS(current, number_of_contained_object_handles) == LAZYBIOS_FIELD_PRESENT) {
				LAZYBIOS_MARK_ABSENT(current, contained_object_handles);
			}

			current->decoded.board_type = lazybiosType2BoardTypeStr(current->board_type);

			char decbuf[LAZYBIOS_DECODER_BUF_SIZE];
			if (LAZYBIOS_FIELD_STATUS(current, feature_flags) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType2FeatureflagsStr(current->feature_flags, decbuf, sizeof(decbuf));
				current->decoded.feature_flags = lazybiosDup(decbuf);
			}

			index++;
		}
		p = structure_end;
	}
	out->count = index;
	return out;
}

// Feature Flags
static size_t lazybiosType2FeatureflagsStr(uint8_t feature_flags, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	size_t len = 0;
	buf[0] = '\0';

	if (feature_flags & (1 << 0)) lazybiosDecoderAppend(buf, buf_len, &len, "Hosting board, ");
	if (feature_flags & (1 << 1)) lazybiosDecoderAppend(buf, buf_len, &len, "Requires daughter board, ");
	if (feature_flags & (1 << 2)) lazybiosDecoderAppend(buf, buf_len, &len, "Removable, ");
	if (feature_flags & (1 << 3)) lazybiosDecoderAppend(buf, buf_len, &len, "Replaceable, ");
	if (feature_flags & (1 << 4)) lazybiosDecoderAppend(buf, buf_len, &len, "Hot swappable, ");

	if (len == 0) {
		snprintf(buf, buf_len, "None");
	} else if (len >= 2) {
		buf[len - 2] = '\0';
	}
	return buf ? strlen(buf) : 0;
}

// Board Type
static const char* lazybiosType2BoardTypeStr(uint8_t board_type) {
	switch (board_type) {
		case BOARD_TYPE_UNKNOWN:
			return "Unknown";
		case BOARD_TYPE_OTHER:
			return "Other";
		case BOARD_TYPE_SERVER_BLADE:
			return "Server Blade";
		case BOARD_TYPE_CONNECTIVITY_SWITCH:
			return "Connectivity Switch";
		case BOARD_TYPE_SYSTEM_MANAGEMENT_MODULE:
			return "System Management Module";
		case BOARD_TYPE_PROCESSOR_MODULE:
			return "Processor Module";
		case BOARD_TYPE_IO_MODULE:
			return "I/O Module";
		case BOARD_TYPE_MEMORY_MODULE:
			return "Memory Module";
		case BOARD_TYPE_DAUGHTER_BOARD:
			return "Daughter board";
		case BOARD_TYPE_MOTHERBOARD:
			return "Motherboard (includes processor, memory, and I/O)";
		case BOARD_TYPE_PROCESSOR_MEMORY_MODULE:
			return "Processor/Memory Module";
		case BOARD_TYPE_PROCESSOR_IO_MODULE:
			return "Processor/IO Module";
		case BOARD_TYPE_INTERCONNECT_BOARD:
			return "Interconnect board";
		default:
			return "Unknown Board Type!";
	}
}

void lazybiosFreeType2(lazybiosType2Array_t* Type2) {
	if (!Type2) return;

	for (size_t i = 0; i < Type2->count; i++) {
		free(Type2->entries[i].decoded.feature_flags);
	}
	for (size_t i = 0; i < Type2->count; i++) free(Type2->entries[i].contained_object_handles);

    free(Type2->entries);

    free(Type2);
}

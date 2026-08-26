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
 * @file type16.c
 * @brief Implements parsing and decoding for SMBIOS Type 16 Physical Memory Array Information.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>

/* File-local decoders; their results are stored in each record's `decoded`. */
static inline const char* lazybiosType16LocationStr(uint8_t location);
static inline uint64_t lazybiosType16MaximumCapacityBytes(uint32_t maximum_capacity, uint64_t extended_maximum_capacity);
static inline const char* lazybiosType16MemoryErrorCorrectionStr(uint8_t memory_error_correction);
static inline const char* lazybiosType16UseStr(uint8_t use);

// Fields
#define LOCATION 0x04
#define USE 0x05
#define MEMORY_ERROR_CORRECTION 0x06
#define MAXIMUM_CAPACITY 0x07
#define MEMORY_ERROR_INFORMATION_HANDLE 0x0B
#define NUMBER_OF_MEMORY_DEVICES 0x0D
#define EXTENDED_MAXIMUM_CAPACITY 0x0F

// Maximum Capacity
#define USE_EXTENDED_MAXIMUM_CAPACITY 0x80000000U

// Locations
#define LOCATION_OTHER 0x01
#define LOCATION_UNKNOWN 0x02
#define LOCATION_SYSTEM_BOARD 0x03
#define LOCATION_ISA_ADD_ON_CARD 0x04
#define LOCATION_EISA_ADD_ON_CARD 0x05
#define LOCATION_PCI_ADD_ON_CARD 0x06
#define LOCATION_MCA_ADD_ON_CARD 0x07
#define LOCATION_PCMCIA_ADD_ON_CARD 0x08
#define LOCATION_PROPRIETARY_ADD_ON_CARD 0x09
#define LOCATION_NUBUS 0x0A
#define LOCATION_PC98_C20_ADD_ON_CARD 0xA0
#define LOCATION_PC98_C24_ADD_ON_CARD 0xA1
#define LOCATION_PC98_E_ADD_ON_CARD 0xA2
#define LOCATION_PC98_LOCAL_BUS_ADD_ON_CARD 0xA3
#define LOCATION_CXL_ADD_ON_CARD 0xA4

// Uses
#define USE_OTHER 0x01
#define USE_UNKNOWN 0x02
#define USE_SYSTEM_MEMORY 0x03
#define USE_VIDEO_MEMORY 0x04
#define USE_FLASH_MEMORY 0x05
#define USE_NON_VOLATILE_RAM 0x06
#define USE_CACHE_MEMORY 0x07

// Error Correction Types
#define ERROR_CORRECTION_OTHER 0x01
#define ERROR_CORRECTION_UNKNOWN 0x02
#define ERROR_CORRECTION_NONE 0x03
#define ERROR_CORRECTION_PARITY 0x04
#define ERROR_CORRECTION_SINGLE_BIT_ECC 0x05
#define ERROR_CORRECTION_MULTI_BIT_ECC 0x06
#define ERROR_CORRECTION_CRC 0x07

lazybiosType16Array_t* lazybiosGetType16(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType16Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY);
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

		if (type == SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY) {
			if (index >= count) break;
			lazybiosType16_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READU8(current, location, len, LOCATION, p);
			READU8(current, use, len, USE, p);
			READU8(current, memory_error_correction, len, MEMORY_ERROR_CORRECTION, p);
			READU32(current, maximum_capacity, len, MAXIMUM_CAPACITY, p);
			READU16(current, memory_error_information_handle, len, MEMORY_ERROR_INFORMATION_HANDLE, p);
			if (current->memory_error_information_handle == 0xFFFE ||
				current->memory_error_information_handle == 0xFFFF) {
				LAZYBIOS_MARK_ABSENT(current, memory_error_information_handle);
			}
			READU16(current, number_of_memory_devices, len, NUMBER_OF_MEMORY_DEVICES, p);

			if (lazybiosIsVersionPlus(DMIData, 2, 7)) {
				READU64(current, extended_maximum_capacity, len, EXTENDED_MAXIMUM_CAPACITY, p);
			} else {
				current->extended_maximum_capacity = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, extended_maximum_capacity);
			}

			current->decoded.location = lazybiosType16LocationStr(current->location);
			current->decoded.maximum_capacity = lazybiosType16MaximumCapacityBytes(current->maximum_capacity, current->extended_maximum_capacity);
			current->decoded.memory_error_correction = lazybiosType16MemoryErrorCorrectionStr(current->memory_error_correction);
			current->decoded.use = lazybiosType16UseStr(current->use);

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

static inline const char* lazybiosType16LocationStr(uint8_t location) {
	switch (location) {
		case LOCATION_OTHER:
			return "Other";
		case LOCATION_UNKNOWN:
			return "Unknown";
		case LOCATION_SYSTEM_BOARD:
			return "System Board or Motherboard";
		case LOCATION_ISA_ADD_ON_CARD:
			return "ISA Add-on Card";
		case LOCATION_EISA_ADD_ON_CARD:
			return "EISA Add-on Card";
		case LOCATION_PCI_ADD_ON_CARD:
			return "PCI Add-on Card";
		case LOCATION_MCA_ADD_ON_CARD:
			return "MCA Add-on Card";
		case LOCATION_PCMCIA_ADD_ON_CARD:
			return "PCMCIA Add-on Card";
		case LOCATION_PROPRIETARY_ADD_ON_CARD:
			return "Proprietary Add-on Card";
		case LOCATION_NUBUS:
			return "NuBus";
		case LOCATION_PC98_C20_ADD_ON_CARD:
			return "PC-98/C20 Add-on Card";
		case LOCATION_PC98_C24_ADD_ON_CARD:
			return "PC-98/C24 Add-on Card";
		case LOCATION_PC98_E_ADD_ON_CARD:
			return "PC-98/E Add-on Card";
		case LOCATION_PC98_LOCAL_BUS_ADD_ON_CARD:
			return "PC-98/Local Bus Add-on Card";
		case LOCATION_CXL_ADD_ON_CARD:
			return "CXL Add-on Card";
		default:
			return "Undefined";
	}
}

static inline const char* lazybiosType16UseStr(uint8_t use) {
	switch (use) {
		case USE_OTHER:
			return "Other";
		case USE_UNKNOWN:
			return "Unknown";
		case USE_SYSTEM_MEMORY:
			return "System Memory";
		case USE_VIDEO_MEMORY:
			return "Video Memory";
		case USE_FLASH_MEMORY:
			return "Flash Memory";
		case USE_NON_VOLATILE_RAM:
			return "Non-volatile RAM";
		case USE_CACHE_MEMORY:
			return "Cache Memory";
		default:
			return "Undefined";
	}
}

static inline const char* lazybiosType16MemoryErrorCorrectionStr(uint8_t memory_error_correction) {
	switch (memory_error_correction) {
		case ERROR_CORRECTION_OTHER:
			return "Other";
		case ERROR_CORRECTION_UNKNOWN:
			return "Unknown";
		case ERROR_CORRECTION_NONE:
			return "None";
		case ERROR_CORRECTION_PARITY:
			return "Parity";
		case ERROR_CORRECTION_SINGLE_BIT_ECC:
			return "Single-bit ECC";
		case ERROR_CORRECTION_MULTI_BIT_ECC:
			return "Multi-bit ECC";
		case ERROR_CORRECTION_CRC:
			return "CRC";
		default:
			return "Undefined";
	}
}

static inline uint64_t lazybiosType16MaximumCapacityBytes(uint32_t maximum_capacity, uint64_t extended_maximum_capacity) {
	if (maximum_capacity == USE_EXTENDED_MAXIMUM_CAPACITY) return extended_maximum_capacity;
	return (uint64_t)maximum_capacity * 1024;
}

void lazybiosFreeType16(lazybiosType16Array_t* Type16) {
    if (!Type16) return;

    free(Type16->entries);

    free(Type16);
}

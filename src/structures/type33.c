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
 * @file type33.c
 * @brief Implements parsing and decoding for SMBIOS Type 33 64-Bit Memory Error Information.
 * @author LazySeldi
 */

//
// Type 33 ( 64-Bit Memory Error Information )
//

#include "lazybios_internal.h"
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define ERROR_TYPE 0x04
#define ERROR_GRANULARITY 0x05
#define ERROR_OPERATION 0x06
#define VENDOR_SYNDROME 0x07
#define MEMORY_ARRAY_ERROR_ADDRESS 0x0B
#define DEVICE_ERROR_ADDRESS 0x13
#define ERROR_RESOLUTION 0x1B

// Error Types
#define ERROR_TYPE_OTHER 0x01
#define ERROR_TYPE_UNKNOWN 0x02
#define ERROR_TYPE_OK 0x03
#define ERROR_TYPE_BAD_READ 0x04
#define ERROR_TYPE_PARITY_ERROR 0x05
#define ERROR_TYPE_SINGLE_BIT_ERROR 0x06
#define ERROR_TYPE_DOUBLE_BIT_ERROR 0x07
#define ERROR_TYPE_MULTI_BIT_ERROR 0x08
#define ERROR_TYPE_NIBBLE_ERROR 0x09
#define ERROR_TYPE_CHECKSUM_ERROR 0x0A
#define ERROR_TYPE_CRC_ERROR 0x0B
#define ERROR_TYPE_CORRECTED_SINGLE_BIT_ERROR 0x0C
#define ERROR_TYPE_CORRECTED_ERROR 0x0D
#define ERROR_TYPE_UNCORRECTABLE_ERROR 0x0E

// Error Granularities
#define ERROR_GRANULARITY_OTHER 0x01
#define ERROR_GRANULARITY_UNKNOWN 0x02
#define ERROR_GRANULARITY_DEVICE_LEVEL 0x03
#define ERROR_GRANULARITY_MEMORY_PARTITION_LEVEL 0x04

// Error Operations
#define ERROR_OPERATION_OTHER 0x01
#define ERROR_OPERATION_UNKNOWN 0x02
#define ERROR_OPERATION_READ 0x03
#define ERROR_OPERATION_WRITE 0x04
#define ERROR_OPERATION_PARTIAL_WRITE 0x05
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 33 64-Bit Memory Error Information structures.
 *
 * @param Type33 Existing Type 33 array pointer value; it is not dereferenced or released.
 * @param type33_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 33 array, or NULL on failure.
 */
lazybiosType33_t* lazybiosGetType33(lazybiosType33_t* Type33, size_t* type33_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_64BIT_MEMORY_ERROR_INFORMATION);
	size_t index = 0;

	Type33 = calloc(count, sizeof(lazybiosType33_t));
	if (!Type33) return NULL;
	if (count == 0) {
		*type33_count = 0;
		return Type33;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_64BIT_MEMORY_ERROR_INFORMATION) {
			if (index >= count) break;
			lazybiosType33_t* current = &Type33[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);

			READU8(current, error_type, len, ERROR_TYPE, p);
			READU8(current, error_granularity, len, ERROR_GRANULARITY, p);
			READU8(current, error_operation, len, ERROR_OPERATION, p);
			READU32(current, vendor_syndrome, len, VENDOR_SYNDROME, p);
			READU64(current, memory_array_error_address, len, MEMORY_ARRAY_ERROR_ADDRESS, p);
			READU64(current, device_error_address, len, DEVICE_ERROR_ADDRESS, p);
			READU32(current, error_resolution, len, ERROR_RESOLUTION, p);

			index++;
		}
		p = DMINext(p, end);
	}
	*type33_count = index;
	return Type33;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes an SMBIOS 64-bit memory-error type.
 *
 * @param error_type Raw SMBIOS memory-error type value.
 * @return Static string describing the error type.
 */
const char* lazybiosType33ErrorTypeStr(uint8_t error_type) {
	switch (error_type) {
		case ERROR_TYPE_OTHER:
			return "Other";
		case ERROR_TYPE_UNKNOWN:
			return "Unknown";
		case ERROR_TYPE_OK:
			return "OK";
		case ERROR_TYPE_BAD_READ:
			return "Bad Read";
		case ERROR_TYPE_PARITY_ERROR:
			return "Parity Error";
		case ERROR_TYPE_SINGLE_BIT_ERROR:
			return "Single-bit Error";
		case ERROR_TYPE_DOUBLE_BIT_ERROR:
			return "Double-bit Error";
		case ERROR_TYPE_MULTI_BIT_ERROR:
			return "Multi-bit Error";
		case ERROR_TYPE_NIBBLE_ERROR:
			return "Nibble Error";
		case ERROR_TYPE_CHECKSUM_ERROR:
			return "Checksum Error";
		case ERROR_TYPE_CRC_ERROR:
			return "CRC Error";
		case ERROR_TYPE_CORRECTED_SINGLE_BIT_ERROR:
			return "Corrected Single-bit Error";
		case ERROR_TYPE_CORRECTED_ERROR:
			return "Corrected Error";
		case ERROR_TYPE_UNCORRECTABLE_ERROR:
			return "Uncorrectable Error";
		default:
			return "Undefined";
	}
}

/**
 * @brief Decodes SMBIOS 64-bit memory-error granularity.
 *
 * @param error_granularity Raw SMBIOS error-granularity value.
 * @return Static string describing the granularity.
 */
const char* lazybiosType33ErrorGranularityStr(uint8_t error_granularity) {
	switch (error_granularity) {
		case ERROR_GRANULARITY_OTHER:
			return "Other";
		case ERROR_GRANULARITY_UNKNOWN:
			return "Unknown";
		case ERROR_GRANULARITY_DEVICE_LEVEL:
			return "Device Level";
		case ERROR_GRANULARITY_MEMORY_PARTITION_LEVEL:
			return "Memory Partition Level";
		default:
			return "Undefined";
	}
}

/**
 * @brief Decodes the operation that caused an SMBIOS 64-bit memory error.
 *
 * @param error_operation Raw SMBIOS error-operation value.
 * @return Static string describing the operation.
 */
const char* lazybiosType33ErrorOperationStr(uint8_t error_operation) {
	switch (error_operation) {
		case ERROR_OPERATION_OTHER:
			return "Other";
		case ERROR_OPERATION_UNKNOWN:
			return "Unknown";
		case ERROR_OPERATION_READ:
			return "Read";
		case ERROR_OPERATION_WRITE:
			return "Write";
		case ERROR_OPERATION_PARTIAL_WRITE:
			return "Partial Write";
		default:
			return "Undefined";
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 33 structures.
 *
 * @param Type33 Type 33 array to release.
 * @param type33_count Number of elements in Type33.
 */
void lazybiosFreeType33(lazybiosType33_t* Type33, size_t type33_count) {
	(void)type33_count;
	free(Type33);
}

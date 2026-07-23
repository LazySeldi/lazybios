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
 * @file type7.c
 * @brief Implements parsing and decoding for SMBIOS Type 7 Cache Information.
 * @author LazySeldi
 */

//
// Type 7 ( Cache Information )
//

#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define SOCKET_DESIGNATION 0x04
#define CACHE_CONFIGURATION 0x05
#define MAXIMUM_CACHE_SIZE 0x07
#define INSTALLED_SIZE 0x09
#define SUPPORTED_SRAM_TYPE 0x0B
#define CURRENT_SRAM_TYPE 0x0D
#define CACHE_SPEED 0x0F
#define ERROR_CORRECTION_TYPE 0x10
#define SYSTEM_CACHE_TYPE 0x11
#define ASSOCIATIVITY 0x12
#define MAXIMUM_CACHE_SIZE_2 0x13
#define INSTALLED_CACHE_SIZE_2 0x17

// Decoders

// Error Correction Type
#define ECC_OTHER 0x01
#define ECC_UNKNOWN 0x02
#define ECC_NONE 0x03
#define PARITY 0x04
#define SINGLE_BIT_ECC 0x05
#define MULTI_BIT_ECC 0x06

// System Cache Type
#define SC_OTHER 0x01
#define SC_UNKNOWN 0x02
#define INSTRUCTION 0x03
#define DATA 0x04
#define UNIFIED 0x05

// Associativity
#define ASS_OTHER 0x01
#define ASS_UNKNOWN 0x02
#define DIRECT_MAPPED 0x03
#define _2_WAY_SET_ASSOCIATIVE 0x04 // Doesn't allow macros starting with a number
#define _4_WAY_SET_ASSOCIATIVE 0x05
#define FULLY_ASSOCIATIVE 0x06
#define _8_WAY_SET_ASSOCIATIVE 0x07
#define _16_WAY_SET_ASSOCIATIVE 0x08
#define _12_WAY_SET_ASSOCIATIVE 0x09
#define _24_WAY_SET_ASSOCIATIVE 0x0A
#define _32_WAY_SET_ASSOCIATIVE 0x0B
#define _48_WAY_SET_ASSOCIATIVE 0x0C
#define _64_WAY_SET_ASSOCIATIVE 0x0D
#define _20_WAY_SET_ASSOCIATIVE 0x0E
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 7 Cache Information structures.
 *
 * @param Type7 Existing Type 7 array pointer value; it is not dereferenced or released.
 * @param type7_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 7 array, or NULL on failure.
 */
lazybiosType7_t* lazybiosGetType7(lazybiosType7_t* Type7, size_t* type7_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_CACHES);
	size_t index = 0;

	Type7 = calloc(count, sizeof(lazybiosType7_t));
	if (!Type7) return NULL;
	if (count == 0) {
		*type7_count = 0;
		return Type7;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_CACHES) {
			if (index >= count) break;
			lazybiosType7_t* current = &Type7[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, socket_designation, len, SOCKET_DESIGNATION, p, structure_end);
			READU16(current, cache_configuration, len, CACHE_CONFIGURATION, p);
			READU16(current, maximum_cache_size, len, MAXIMUM_CACHE_SIZE, p);
			READU16(current, installed_size, len, INSTALLED_SIZE, p);
			READU16(current, supported_sram_type, len, SUPPORTED_SRAM_TYPE, p);
			READU16(current, current_sram_type, len, CURRENT_SRAM_TYPE, p);
			if (lazybiosIsVersionPlus(DMIData, 2, 1)) {
				READU8(current, cache_speed, len, CACHE_SPEED, p);
				READU8(current, error_correction_type, len, ERROR_CORRECTION_TYPE, p);
				READU8(current, system_cache_type, len, SYSTEM_CACHE_TYPE, p);
				READU8(current, associativity, len, ASSOCIATIVITY, p);
			}
			if (lazybiosIsVersionPlus(DMIData, 3, 1)) {
				READU32(current, maximum_cache_size_2, len, MAXIMUM_CACHE_SIZE_2, p);
				READU32(current, installed_cache_size_2, len, INSTALLED_CACHE_SIZE_2, p);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type7_count = index;
	return Type7;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders

// Cache Size -- This is for both Maximum Cache Size and Installed Size
// Returns the cache size in KB based on the 16-bit  value
/**
 * @brief Converts a 16-bit SMBIOS cache size field to kibibytes.
 *
 * @param raw_size Raw 16-bit maximum or installed cache size value.
 * @return Decoded cache size in KiB.
 */
uint64_t lazybiosType7CacheU16(uint16_t raw_size) {
	uint8_t granularity = (raw_size >> 15) & 0x01;
	uint16_t size_value = raw_size & 0x7FFF; // Bits 14:0

	if (granularity == 1) {
		return (uint64_t)size_value * 64; // 64K granularity
	} else {
		return (uint64_t)size_value;      // 1K granularity
	}
}

// SRAM Type
/**
 * @brief Decodes SMBIOS SRAM type flags into a readable string.
 *
 * @param sram_type Raw supported or current SRAM type bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType7SRAMTypeStr(uint16_t sram_type, char* buf, size_t buf_len) {
	size_t len = 0;
	buf[0] = '\0';

	if (sram_type & (1 << 0)) len += snprintf(buf + len, buf_len - len, "Other, ");
	if (sram_type & (1 << 1)) len += snprintf(buf + len, buf_len - len, "Unknown, ");
	if (sram_type & (1 << 2)) len += snprintf(buf + len, buf_len - len, "Non-Burst, ");
	if (sram_type & (1 << 3)) len += snprintf(buf + len, buf_len - len, "Burst, ");
	if (sram_type & (1 << 4)) len += snprintf(buf + len, buf_len - len, "Pipeline Burst, ");
	if (sram_type & (1 << 5)) len += snprintf(buf + len, buf_len - len, "Synchronous, ");
	if (sram_type & (1 << 6)) len += snprintf(buf + len, buf_len - len, "Asynchronous, ");

	if (len == 0) {
		snprintf(buf, buf_len, "None");
	} else if (len >= 2) {
		buf[len - 2] = '\0';
	}
}

// Error Correction Type
/**
 * @brief Decodes an SMBIOS cache error-correction type.
 *
 * @param ecc_type Raw SMBIOS error-correction type value.
 * @return Static string describing the error-correction type.
 */
const char* lazybiosType7ErrorCorrectionTypeStr(uint8_t ecc_type) {
	switch (ecc_type) {
		case ECC_OTHER:
			return "Other";
		case ECC_UNKNOWN:
			return "Unknown";
		case ECC_NONE:
			return "None";
		case PARITY:
			return "Parity";
		case SINGLE_BIT_ECC:
			return "Single-bit ECC";
		case MULTI_BIT_ECC:
			return "Multi-bit ECC";
		default:
			return "Unknown Error Correction Type";
	}
}

// System Cache Type
/**
 * @brief Decodes an SMBIOS system cache type.
 *
 * @param cache_type Raw SMBIOS system cache type value.
 * @return Static string describing the cache type.
 */
const char* lazybiosType7SystemCacheTypeStr(uint8_t cache_type) {
	switch (cache_type) {
		case SC_OTHER:
			return "Other";
		case SC_UNKNOWN:
			return "Unknown";
		case INSTRUCTION:
			return "Instruction";
		case DATA:
			return "Data";
		case UNIFIED:
			return "Unified";
		default:
			return "Unknown System Cache Type";
	}
}

// Associativity
/**
 * @brief Decodes an SMBIOS cache associativity value.
 *
 * @param associativity Raw SMBIOS cache associativity value.
 * @return Static string describing the associativity.
 */
const char* lazybiosType7AssociativityStr(uint8_t associativity) {
	switch (associativity) {
		case ASS_OTHER:
			return "Other";
		case ASS_UNKNOWN:
			return "Unknown";
		case DIRECT_MAPPED:
			return "Direct Mapped";
		case _2_WAY_SET_ASSOCIATIVE:
			return "2-way Set-Associative";
		case _4_WAY_SET_ASSOCIATIVE:
			return "4-way Set-Associative";
		case FULLY_ASSOCIATIVE:
			return "Fully Associative";
		case _8_WAY_SET_ASSOCIATIVE:
			return "8-way Set-Associative";
		case _16_WAY_SET_ASSOCIATIVE:
			return "16-way Set-Associative";
		case _12_WAY_SET_ASSOCIATIVE:
			return "12-way Set-Associative";
		case _24_WAY_SET_ASSOCIATIVE:
			return "24-way Set-Associative";
		case _32_WAY_SET_ASSOCIATIVE:
			return "32-way Set-Associative";
		case _48_WAY_SET_ASSOCIATIVE:
			return "48-way Set-Associative";
		case _64_WAY_SET_ASSOCIATIVE:
			return "64-way Set-Associative";
		case _20_WAY_SET_ASSOCIATIVE:
			return "20-way Set-Associative";
		default:
			return "Unknown Associativity";
	}
}

// Cache Configuration
/**
 * @brief Decodes an SMBIOS cache configuration bit field.
 *
 * @param config Raw SMBIOS cache configuration value.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType7CacheConfigurationStr(uint16_t config, char* buf, size_t buf_len) {
	size_t len = 0;
	buf[0] = '\0';

	// Bits 2:0 Cache Level (Value 0 = L1, 1 = L2, 2 = L3, etc.)
	uint8_t level = config & 0x07;
	if (level <= 7) {
		len += snprintf(buf + len, buf_len - len, "L%d, ", level + 1);
	}

	// Bit 3 Cache Socketed
	if ((config >> 3) & 0x01) {
		len += snprintf(buf + len, buf_len - len, "Socketed, ");
	} else {
		len += snprintf(buf + len, buf_len - len, "Not Socketed, ");
	}

	// Bits 6:5 Location
	uint8_t location = (config >> 5) & 0x03;
	switch (location) {
		case 0x00: len += snprintf(buf + len, buf_len - len, "Internal, "); break;
		case 0x01: len += snprintf(buf + len, buf_len - len, "External, "); break;
		case 0x02: len += snprintf(buf + len, buf_len - len, "Reserved, "); break;
		case 0x03: len += snprintf(buf + len, buf_len - len, "Unknown, "); break;
	}

	// Bit 7 Enabled/Disabled
	if ((config >> 7) & 0x01) {
		len += snprintf(buf + len, buf_len - len, "Enabled, ");
	} else {
		len += snprintf(buf + len, buf_len - len, "Disabled, ");
	}

	// Bits 9:8 Operational Mode
	uint8_t op_mode = (config >> 8) & 0x03;
	switch (op_mode) {
		case 0x00: len += snprintf(buf + len, buf_len - len, "Write Through, "); break;
		case 0x01: len += snprintf(buf + len, buf_len - len, "Write Back, "); break;
		case 0x02: len += snprintf(buf + len, buf_len - len, "Varies with Memory Address, "); break;
		case 0x03: len += snprintf(buf + len, buf_len - len, "Unknown, "); break;
	}

	if (len >= 2) {
		buf[len - 2] = '\0';
	}
}

// Cache Size 2 -- Used for Maximum Cache Size 2, and Installed Cache Size 2
/**
 * @brief Converts a 32-bit SMBIOS cache size field to kibibytes.
 *
 * @param raw_size Raw 32-bit maximum or installed cache size value.
 * @return Decoded cache size in KiB.
 */
uint64_t lazybiosType7CacheU32(uint32_t raw_size) {
	// If the field is 0, no cache is installed
	if (raw_size == 0) {
		return 0;
	}

	uint8_t granularity = (raw_size >> 31) & 0x01;
	uint32_t size_value = raw_size & 0x7FFFFFFF; // Bits 30:0

	if (granularity == 1) {
		return (uint64_t)size_value * 64; // 64K granularity
	} else {
		return (uint64_t)size_value;      // 1K granularity
	}
}

// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 7 structures.
 *
 * @param Type7 Type 7 array to release.
 * @param type7_count Number of elements in Type7.
 */
void lazybiosFreeType7(lazybiosType7_t* Type7, size_t type7_count) {
	if (!Type7) return;

	for (size_t i = 0; i < type7_count; i++) {
		free(Type7[i].socket_designation);
	}

	free(Type7);
}

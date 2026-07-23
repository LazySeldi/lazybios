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
 * @file type5.c
 * @brief Implements parsing and decoding for obsolete SMBIOS Type 5 Memory Controller Information.
 * @author LazySeldi
 */

//
// Type 5 ( Memory Controller Information, Obsolete )
//

#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define ERROR_DETECTING_METHOD 0x04
#define ERROR_CORRECTING_CAPABILITY 0x05
#define SUPPORTED_INTERLEAVE 0x06
#define CURRENT_INTERLEAVE 0x07
#define MAXIMUM_MEMORY_MODULE_SIZE 0x08
#define SUPPORTED_SPEEDS 0x09
#define SUPPORTED_MEMORY_TYPES 0x0B
#define MEMORY_MODULE_VOLTAGE 0x0D
#define NUMBER_OF_ASSOCIATED_MEMORY_SLOTS 0x0E
#define MEMORY_MODULE_CONFIGURATION_HANDLES 0x0F
#define ENABLED_ERROR_CORRECTING_CAPABILITIES(slot_count) (0x0F + ((slot_count) * 2))

// Error Detecting Methods
#define ERROR_DETECTING_OTHER 0x01
#define ERROR_DETECTING_UNKNOWN 0x02
#define ERROR_DETECTING_NONE 0x03
#define ERROR_DETECTING_8BIT_PARITY 0x04
#define ERROR_DETECTING_32BIT_ECC 0x05
#define ERROR_DETECTING_64BIT_ECC 0x06
#define ERROR_DETECTING_128BIT_ECC 0x07
#define ERROR_DETECTING_CRC 0x08

// Interleave Values
#define INTERLEAVE_OTHER 0x01
#define INTERLEAVE_UNKNOWN 0x02
#define INTERLEAVE_ONE_WAY 0x03
#define INTERLEAVE_TWO_WAY 0x04
#define INTERLEAVE_FOUR_WAY 0x05
#define INTERLEAVE_EIGHT_WAY 0x06
#define INTERLEAVE_SIXTEEN_WAY 0x07
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all obsolete SMBIOS Type 5 Memory Controller Information structures.
 *
 * @param Type5 Existing Type 5 array pointer value; it is not dereferenced or released.
 * @param type5_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 5 array, or NULL on failure.
 */
lazybiosType5_t* lazybiosGetType5(lazybiosType5_t* Type5, size_t* type5_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_MEMORY_CONTROLLER);
	size_t index = 0;

	Type5 = calloc(count, sizeof(lazybiosType5_t));
	if (!Type5) return NULL;
	if (count == 0) {
		*type5_count = 0;
		return Type5;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_MEMORY_CONTROLLER) {
			if (index >= count) break;
			lazybiosType5_t* current = &Type5[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);

			READU8(current, error_detecting_method, len, ERROR_DETECTING_METHOD, p);
			READU8(current, error_correcting_capability, len, ERROR_CORRECTING_CAPABILITY, p);
			READU8(current, supported_interleave, len, SUPPORTED_INTERLEAVE, p);
			READU8(current, current_interleave, len, CURRENT_INTERLEAVE, p);
			READU8(current, maximum_memory_module_size, len, MAXIMUM_MEMORY_MODULE_SIZE, p);
			READU16(current, supported_speeds, len, SUPPORTED_SPEEDS, p);
			READU16(current, supported_memory_types, len, SUPPORTED_MEMORY_TYPES, p);
			READU8(current, memory_module_voltage, len, MEMORY_MODULE_VOLTAGE, p);
			READU8(current, number_of_associated_memory_slots, len, NUMBER_OF_ASSOCIATED_MEMORY_SLOTS, p);

			if (LAZYBIOS_FIELD_STATUS(current, number_of_associated_memory_slots) == LAZYBIOS_FIELD_PRESENT) {
				size_t handle_bytes = (size_t)current->number_of_associated_memory_slots * sizeof(uint16_t);
				if ((size_t)len >= MEMORY_MODULE_CONFIGURATION_HANDLES + handle_bytes) {
					if (current->number_of_associated_memory_slots > 0) {
						current->memory_module_configuration_handles =
							calloc(current->number_of_associated_memory_slots, sizeof(uint16_t));
						if (!current->memory_module_configuration_handles) {
							lazybiosFreeType5(Type5, count);
							return NULL;
						}
						for (size_t i = 0; i < current->number_of_associated_memory_slots; i++) {
							memcpy(&current->memory_module_configuration_handles[i],
								p + MEMORY_MODULE_CONFIGURATION_HANDLES + (i * sizeof(uint16_t)), sizeof(uint16_t));
						}
					}
					LAZYBIOS_MARK_PRESENT(current, memory_module_configuration_handles);
				} else {
					LAZYBIOS_MARK_ABSENT(current, memory_module_configuration_handles);
				}
			}

			if (lazybiosIsVersionPlus(DMIData, 2, 1) &&
				LAZYBIOS_FIELD_STATUS(current, number_of_associated_memory_slots) == LAZYBIOS_FIELD_PRESENT) {
				READU8(current, enabled_error_correcting_capabilities, len,
					ENABLED_ERROR_CORRECTING_CAPABILITIES(current->number_of_associated_memory_slots), p);
			} else {
				current->enabled_error_correcting_capabilities = 0;
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type5_count = index;
	return Type5;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes a Type 5 memory-error detecting method.
 *
 * @param error_detecting_method Raw error-detecting method value.
 * @return Static string describing the method.
 */
const char* lazybiosType5ErrorDetectingMethodStr(uint8_t error_detecting_method) {
	switch (error_detecting_method) {
		case ERROR_DETECTING_OTHER: return "Other";
		case ERROR_DETECTING_UNKNOWN: return "Unknown";
		case ERROR_DETECTING_NONE: return "None";
		case ERROR_DETECTING_8BIT_PARITY: return "8-bit Parity";
		case ERROR_DETECTING_32BIT_ECC: return "32-bit ECC";
		case ERROR_DETECTING_64BIT_ECC: return "64-bit ECC";
		case ERROR_DETECTING_128BIT_ECC: return "128-bit ECC";
		case ERROR_DETECTING_CRC: return "CRC";
		default: return "Undefined";
	}
}

/**
 * @brief Decodes a Type 5 error-correcting capability bit field.
 *
 * @param capability Raw supported or enabled error-correcting capability byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType5ErrorCorrectingCapabilityStr(uint8_t capability, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;
	size_t len = 0;
	buf[0] = '\0';
	if ((capability & (1U << 0)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "Other, ");
	if ((capability & (1U << 1)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "Unknown, ");
	if ((capability & (1U << 2)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "None, ");
	if ((capability & (1U << 3)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "Single-bit Error Correcting, ");
	if ((capability & (1U << 4)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "Double-bit Error Correcting, ");
	if ((capability & (1U << 5)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "Error Scrubbing, ");
	if (len == 0) snprintf(buf, buf_len, "None");
	else if (len >= 2 && len < buf_len) buf[len - 2] = '\0';
	else buf[buf_len - 1] = '\0';
}

/**
 * @brief Decodes a Type 5 memory-interleave value.
 *
 * @param interleave Raw supported or current interleave value.
 * @return Static string describing the interleave configuration.
 */
const char* lazybiosType5InterleaveStr(uint8_t interleave) {
	switch (interleave) {
		case INTERLEAVE_OTHER: return "Other";
		case INTERLEAVE_UNKNOWN: return "Unknown";
		case INTERLEAVE_ONE_WAY: return "One-way Interleave";
		case INTERLEAVE_TWO_WAY: return "Two-way Interleave";
		case INTERLEAVE_FOUR_WAY: return "Four-way Interleave";
		case INTERLEAVE_EIGHT_WAY: return "Eight-way Interleave";
		case INTERLEAVE_SIXTEEN_WAY: return "Sixteen-way Interleave";
		default: return "Undefined";
	}
}

/**
 * @brief Decodes the Type 5 supported-memory-speed bit field.
 *
 * @param supported_speeds Raw supported-memory-speed bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType5SupportedSpeedsStr(uint16_t supported_speeds, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;
	size_t len = 0;
	buf[0] = '\0';
	if ((supported_speeds & (1U << 0)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "Other, ");
	if ((supported_speeds & (1U << 1)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "Unknown, ");
	if ((supported_speeds & (1U << 2)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "70 ns, ");
	if ((supported_speeds & (1U << 3)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "60 ns, ");
	if ((supported_speeds & (1U << 4)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "50 ns, ");
	if (len == 0) snprintf(buf, buf_len, "None");
	else if (len >= 2 && len < buf_len) buf[len - 2] = '\0';
	else buf[buf_len - 1] = '\0';
}

/**
 * @brief Decodes the Type 5 supported-memory-type bit field.
 *
 * @param supported_memory_types Raw supported-memory-type bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType5SupportedMemoryTypesStr(uint16_t supported_memory_types, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;
	size_t len = 0;
	buf[0] = '\0';
	const char* names[] = {"Other", "Unknown", "Standard", "Fast Page Mode", "EDO", "Parity",
		"ECC", "SIMM", "DIMM", "Burst EDO", "SDRAM"};
	for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
		if ((supported_memory_types & (1U << i)) && len < buf_len) {
			len += snprintf(buf + len, buf_len - len, "%s, ", names[i]);
		}
	}
	if (len == 0) snprintf(buf, buf_len, "None");
	else if (len >= 2 && len < buf_len) buf[len - 2] = '\0';
	else buf[buf_len - 1] = '\0';
}

/**
 * @brief Decodes the Type 5 memory-module voltage bit field.
 *
 * @param memory_module_voltage Raw memory-module voltage bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType5MemoryModuleVoltageStr(uint8_t memory_module_voltage, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;
	size_t len = 0;
	buf[0] = '\0';
	if ((memory_module_voltage & (1U << 0)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "5 V, ");
	if ((memory_module_voltage & (1U << 1)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "3.3 V, ");
	if ((memory_module_voltage & (1U << 2)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "2.9 V, ");
	if (len == 0) snprintf(buf, buf_len, "None");
	else if (len >= 2 && len < buf_len) buf[len - 2] = '\0';
	else buf[buf_len - 1] = '\0';
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 5 structures.
 *
 * @param Type5 Type 5 array to release.
 * @param type5_count Number of elements in Type5.
 */
void lazybiosFreeType5(lazybiosType5_t* Type5, size_t type5_count) {
	if (!Type5) return;
	for (size_t i = 0; i < type5_count; i++) free(Type5[i].memory_module_configuration_handles);
	free(Type5);
}

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
 * @file type6.c
 * @brief Implements parsing and decoding for obsolete SMBIOS Type 6 Memory Module Information.
 * @author LazySeldi
 */

//
// Type 6 ( Memory Module Information, Obsolete )
//

#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define SOCKET_DESIGNATION 0x04
#define BANK_CONNECTIONS 0x05
#define CURRENT_SPEED 0x06
#define CURRENT_MEMORY_TYPE 0x07
#define INSTALLED_SIZE 0x09
#define ENABLED_SIZE 0x0A
#define ERROR_STATUS 0x0B

// Size Values
#define SIZE_VALUE_MASK 0x7F
#define SIZE_DOUBLE_BANK_MASK 0x80
#define SIZE_NOT_DETERMINABLE 0x7D
#define SIZE_INSTALLED_NOT_ENABLED 0x7E
#define SIZE_NOT_INSTALLED 0x7F
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all obsolete SMBIOS Type 6 Memory Module Information structures.
 *
 * @param Type6 Existing Type 6 array pointer value; it is not dereferenced or released.
 * @param type6_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 6 array, or NULL on failure.
 */
lazybiosType6_t* lazybiosGetType6(lazybiosType6_t* Type6, size_t* type6_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;
	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;
	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_MEMORY_MODULE);
	size_t index = 0;
	Type6 = calloc(count, sizeof(lazybiosType6_t));
	if (!Type6) return NULL;
	if (count == 0) {
		*type6_count = 0;
		return Type6;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];
		if (type == SMBIOS_TYPE_MEMORY_MODULE) {
			lazybiosType6_t* current = &Type6[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);
			READSTR(current, socket_designation, len, SOCKET_DESIGNATION, p, structure_end);
			READU8(current, bank_connections, len, BANK_CONNECTIONS, p);
			READU8(current, current_speed, len, CURRENT_SPEED, p);
			READU16(current, current_memory_type, len, CURRENT_MEMORY_TYPE, p);
			READU8(current, installed_size, len, INSTALLED_SIZE, p);
			READU8(current, enabled_size, len, ENABLED_SIZE, p);
			READU8(current, error_status, len, ERROR_STATUS, p);
			index++;
		}
		p = DMINext(p, end);
	}
	*type6_count = index;
	return Type6;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Formats the bank connections encoded by a Type 6 structure.
 *
 * @param bank_connections Raw bank-connections byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType6BankConnectionsStr(uint8_t bank_connections, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;
	uint8_t first = (uint8_t)(bank_connections >> 4);
	uint8_t second = (uint8_t)(bank_connections & 0x0F);
	if (first == 0x0F && second == 0x0F) snprintf(buf, buf_len, "None");
	else if (first == 0x0F) snprintf(buf, buf_len, "%hhu", second);
	else if (second == 0x0F) snprintf(buf, buf_len, "%hhu", first);
	else snprintf(buf, buf_len, "%hhu %hhu", first, second);
}

/**
 * @brief Decodes the Type 6 current-memory-type bit field.
 *
 * @param current_memory_type Raw current-memory-type bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType6CurrentMemoryTypeStr(uint16_t current_memory_type, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;
	size_t len = 0;
	buf[0] = '\0';
	const char* names[] = {"Other", "Unknown", "Standard", "Fast Page Mode", "EDO", "Parity",
		"ECC", "SIMM", "DIMM", "Burst EDO", "SDRAM"};
	for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
		if ((current_memory_type & (1U << i)) && len < buf_len) {
			len += snprintf(buf + len, buf_len - len, "%s, ", names[i]);
		}
	}
	if (len == 0) snprintf(buf, buf_len, "None");
	else if (len >= 2 && len < buf_len) buf[len - 2] = '\0';
	else buf[buf_len - 1] = '\0';
}

/**
 * @brief Formats an obsolete Type 6 installed-size value.
 *
 * @param installed_size Raw installed-size byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType6InstalledSizeStr(uint8_t installed_size, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;
	uint8_t size = installed_size & SIZE_VALUE_MASK;
	if (size == SIZE_NOT_DETERMINABLE) snprintf(buf, buf_len, "Not Determinable");
	else if (size == SIZE_INSTALLED_NOT_ENABLED) snprintf(buf, buf_len, "Module Installed, No Memory Enabled");
	else if (size == SIZE_NOT_INSTALLED) snprintf(buf, buf_len, "Not Installed");
	else if (size < 64) snprintf(buf, buf_len, "%llu MiB (%s-bank Connection)",
		(unsigned long long)(1ULL << size), (installed_size & SIZE_DOUBLE_BANK_MASK) ? "Double" : "Single");
	else snprintf(buf, buf_len, "2^%hhu MiB (%s-bank Connection)", size,
		(installed_size & SIZE_DOUBLE_BANK_MASK) ? "Double" : "Single");
}

/**
 * @brief Formats an obsolete Type 6 enabled-size value.
 *
 * @param enabled_size Raw enabled-size byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType6EnabledSizeStr(uint8_t enabled_size, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;
	uint8_t size = enabled_size & SIZE_VALUE_MASK;
	if (size == SIZE_NOT_DETERMINABLE) snprintf(buf, buf_len, "Undefined");
	else if (size == SIZE_INSTALLED_NOT_ENABLED) snprintf(buf, buf_len, "Module Installed, No Memory Enabled");
	else if (size == SIZE_NOT_INSTALLED) snprintf(buf, buf_len, "Not Installed");
	else if (size < 64) snprintf(buf, buf_len, "%llu MiB (%s-bank Connection)",
		(unsigned long long)(1ULL << size), (enabled_size & SIZE_DOUBLE_BANK_MASK) ? "Double" : "Single");
	else snprintf(buf, buf_len, "2^%hhu MiB (%s-bank Connection)", size,
		(enabled_size & SIZE_DOUBLE_BANK_MASK) ? "Double" : "Single");
}

/**
 * @brief Decodes the Type 6 memory-module error-status bit field.
 *
 * @param error_status Raw error-status byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType6ErrorStatusStr(uint8_t error_status, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;
	size_t len = 0;
	buf[0] = '\0';
	if ((error_status & (1U << 2)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "See Event Log, ");
	if ((error_status & (1U << 1)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "Correctable Errors, ");
	if ((error_status & (1U << 0)) && len < buf_len) len += snprintf(buf + len, buf_len - len, "Uncorrectable Errors, ");
	if (len == 0) snprintf(buf, buf_len, "OK");
	else if (len >= 2 && len < buf_len) buf[len - 2] = '\0';
	else buf[buf_len - 1] = '\0';
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 6 structures.
 *
 * @param Type6 Type 6 array to release.
 * @param type6_count Number of elements in Type6.
 */
void lazybiosFreeType6(lazybiosType6_t* Type6, size_t type6_count) {
	if (!Type6) return;
	for (size_t i = 0; i < type6_count; i++) free(Type6[i].socket_designation);
	free(Type6);
}

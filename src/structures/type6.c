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
 * @file type6.c
 * @brief Implements parsing and decoding for obsolete SMBIOS Type 6 Memory Module Information.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* File-local decoders; their output is stored in each record's `decoded`. */
static size_t lazybiosType6BankConnectionsStr(uint8_t bank_connections, char* buf, size_t buf_len);
static size_t lazybiosType6CurrentMemoryTypeStr(uint16_t current_memory_type, char* buf, size_t buf_len);
static size_t lazybiosType6InstalledSizeStr(uint8_t installed_size, char* buf, size_t buf_len);
static size_t lazybiosType6EnabledSizeStr(uint8_t enabled_size, char* buf, size_t buf_len);
static size_t lazybiosType6ErrorStatusStr(uint8_t error_status, char* buf, size_t buf_len);

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
/* --- */

lazybiosType6Array_t* lazybiosGetType6(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType6Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;
	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;
	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_MEMORY_MODULE);
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
		if (type == SMBIOS_TYPE_MEMORY_MODULE) {
			lazybiosType6_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;
			const uint8_t* structure_end = DMINext(p, end);
			READSTR(current, socket_designation, len, SOCKET_DESIGNATION, p, structure_end);
			READU8(current, bank_connections, len, BANK_CONNECTIONS, p);
			READU8(current, current_speed, len, CURRENT_SPEED, p);
			READU16(current, current_memory_type, len, CURRENT_MEMORY_TYPE, p);
			READU8(current, installed_size, len, INSTALLED_SIZE, p);
			READU8(current, enabled_size, len, ENABLED_SIZE, p);
			READU8(current, error_status, len, ERROR_STATUS, p);
			char decbuf[LAZYBIOS_DECODER_BUF_SIZE];
			if (LAZYBIOS_FIELD_STATUS(current, bank_connections) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType6BankConnectionsStr(current->bank_connections, decbuf, sizeof(decbuf));
				current->decoded.bank_connections = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, current_memory_type) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType6CurrentMemoryTypeStr(current->current_memory_type, decbuf, sizeof(decbuf));
				current->decoded.current_memory_type = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, installed_size) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType6InstalledSizeStr(current->installed_size, decbuf, sizeof(decbuf));
				current->decoded.installed_size = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, enabled_size) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType6EnabledSizeStr(current->enabled_size, decbuf, sizeof(decbuf));
				current->decoded.enabled_size = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, error_status) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType6ErrorStatusStr(current->error_status, decbuf, sizeof(decbuf));
				current->decoded.error_status = lazybiosDup(decbuf);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

/* --- */
static size_t lazybiosType6BankConnectionsStr(uint8_t bank_connections, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	uint8_t first = (uint8_t)(bank_connections >> 4);
	uint8_t second = (uint8_t)(bank_connections & 0x0F);
	if (first == 0x0F && second == 0x0F) snprintf(buf, buf_len, "None");
	else if (first == 0x0F) snprintf(buf, buf_len, "%hhu", second);
	else if (second == 0x0F) snprintf(buf, buf_len, "%hhu", first);
	else snprintf(buf, buf_len, "%hhu %hhu", first, second);
	return buf ? strlen(buf) : 0;
}

static size_t lazybiosType6CurrentMemoryTypeStr(uint16_t current_memory_type, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	size_t len = 0;
	buf[0] = '\0';
	const char* names[] = {"Other", "Unknown", "Standard", "Fast Page Mode", "EDO", "Parity",
		"ECC", "SIMM", "DIMM", "Burst EDO", "SDRAM"};
	for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
		if (current_memory_type & (1U << i)) {
			lazybiosDecoderAppend(buf, buf_len, &len, "%s, ", names[i]);
		}
	}
	if (len == 0) snprintf(buf, buf_len, "None");
	else if (len >= 2 && len < buf_len) buf[len - 2] = '\0';
	else buf[buf_len - 1] = '\0';
	return buf ? strlen(buf) : 0;
}

static size_t lazybiosType6InstalledSizeStr(uint8_t installed_size, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	uint8_t size = installed_size & SIZE_VALUE_MASK;
	if (size == SIZE_NOT_DETERMINABLE) snprintf(buf, buf_len, "Not Determinable");
	else if (size == SIZE_INSTALLED_NOT_ENABLED) snprintf(buf, buf_len, "Module Installed, No Memory Enabled");
	else if (size == SIZE_NOT_INSTALLED) snprintf(buf, buf_len, "Not Installed");
	else if (size < 64) snprintf(buf, buf_len, "%llu MiB (%s-bank Connection)",
		(unsigned long long)(1ULL << size), (installed_size & SIZE_DOUBLE_BANK_MASK) ? "Double" : "Single");
	else snprintf(buf, buf_len, "2^%hhu MiB (%s-bank Connection)", size,
		(installed_size & SIZE_DOUBLE_BANK_MASK) ? "Double" : "Single");
	return buf ? strlen(buf) : 0;
}

static size_t lazybiosType6EnabledSizeStr(uint8_t enabled_size, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	uint8_t size = enabled_size & SIZE_VALUE_MASK;
	if (size == SIZE_NOT_DETERMINABLE) snprintf(buf, buf_len, "Undefined");
	else if (size == SIZE_INSTALLED_NOT_ENABLED) snprintf(buf, buf_len, "Module Installed, No Memory Enabled");
	else if (size == SIZE_NOT_INSTALLED) snprintf(buf, buf_len, "Not Installed");
	else if (size < 64) snprintf(buf, buf_len, "%llu MiB (%s-bank Connection)",
		(unsigned long long)(1ULL << size), (enabled_size & SIZE_DOUBLE_BANK_MASK) ? "Double" : "Single");
	else snprintf(buf, buf_len, "2^%hhu MiB (%s-bank Connection)", size,
		(enabled_size & SIZE_DOUBLE_BANK_MASK) ? "Double" : "Single");
	return buf ? strlen(buf) : 0;
}

static size_t lazybiosType6ErrorStatusStr(uint8_t error_status, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	size_t len = 0;
	buf[0] = '\0';
	if (error_status & (1U << 2)) lazybiosDecoderAppend(buf, buf_len, &len, "See Event Log, ");
	if (error_status & (1U << 1)) lazybiosDecoderAppend(buf, buf_len, &len, "Correctable Errors, ");
	if (error_status & (1U << 0)) lazybiosDecoderAppend(buf, buf_len, &len, "Uncorrectable Errors, ");
	if (len == 0) snprintf(buf, buf_len, "OK");
	else if (len >= 2 && len < buf_len) buf[len - 2] = '\0';
	else buf[buf_len - 1] = '\0';
	return buf ? strlen(buf) : 0;
}

/* --- */
void lazybiosFreeType6(lazybiosType6Array_t* Type6) {
    if (!Type6) return;

	for (size_t i = 0; i < Type6->count; i++) {
		free(Type6->entries[i].decoded.bank_connections);
		free(Type6->entries[i].decoded.current_memory_type);
		free(Type6->entries[i].decoded.installed_size);
		free(Type6->entries[i].decoded.enabled_size);
		free(Type6->entries[i].decoded.error_status);
	}

    free(Type6->entries);

    free(Type6);
}

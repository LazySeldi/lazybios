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
 * @file type38.c
 * @brief Implements parsing and decoding for SMBIOS Type 38 IPMI Device Information.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* File-local decoders; their output is stored in each record's `decoded`. */
static size_t lazybiosType38SpecificationRevisionStr(uint8_t revision, char* buf, size_t buf_len);
static size_t lazybiosType38InterruptInfoStr(uint8_t interrupt_info, char* buf, size_t buf_len);

/* File-local decoders; their results are stored in each record's `decoded`. */
static inline const char* lazybiosType38BaseAddressTypeStr(uint64_t base_address);
static inline uint64_t lazybiosType38BaseAddressValue(uint64_t base_address, uint8_t modifier);
static inline const char* lazybiosType38InterfaceTypeStr(uint8_t interface_type);
static inline const char* lazybiosType38RegisterSpacingStr(uint8_t modifier);

// Fields
#define INTERFACE_TYPE 0x04
#define IPMI_SPECIFICATION_REVISION 0x05
#define I2C_TARGET_ADDRESS 0x06
#define NV_STORAGE_DEVICE_ADDRESS 0x07
#define BASE_ADDRESS 0x08
#define BASE_ADDRESS_MODIFIER_INTERRUPT_INFO 0x10
#define INTERRUPT_NUMBER 0x11

// Interface Types
#define INTERFACE_TYPE_UNKNOWN 0x00
#define INTERFACE_TYPE_KCS 0x01
#define INTERFACE_TYPE_SMIC 0x02
#define INTERFACE_TYPE_BT 0x03
#define INTERFACE_TYPE_SSIF 0x04

lazybiosType38Array_t* lazybiosGetType38(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType38Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_IPMI_DEVICE_INFORMATION);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_IPMI_DEVICE_INFORMATION].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_IPMI_DEVICE_INFORMATION].first;
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

		if (type == SMBIOS_TYPE_IPMI_DEVICE_INFORMATION) {
			if (index >= count) break;
			lazybiosType38_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READU8(current, interface_type, len, INTERFACE_TYPE, p);
			READU8(current, ipmi_specification_revision, len, IPMI_SPECIFICATION_REVISION, p);
			READU8(current, i2c_target_address, len, I2C_TARGET_ADDRESS, p);
			READU8(current, nv_storage_device_address, len, NV_STORAGE_DEVICE_ADDRESS, p);
			if (current->nv_storage_device_address == 0xFF) {
				LAZYBIOS_MARK_ABSENT(current, nv_storage_device_address);
			}
			READU64(current, base_address, len, BASE_ADDRESS, p);
			READU8(current, base_address_modifier_interrupt_info, len, BASE_ADDRESS_MODIFIER_INTERRUPT_INFO, p);
			READU8(current, interrupt_number, len, INTERRUPT_NUMBER, p);

			current->decoded.base_address_type = lazybiosType38BaseAddressTypeStr(current->base_address);
			current->decoded.register_spacing = lazybiosType38RegisterSpacingStr(current->base_address_modifier_interrupt_info);
			current->decoded.interface_type = lazybiosType38InterfaceTypeStr(current->interface_type);
			current->decoded.base_address = lazybiosType38BaseAddressValue(current->base_address, current->base_address_modifier_interrupt_info);

			char decbuf[LAZYBIOS_DECODER_BUF_SIZE];
			if (LAZYBIOS_FIELD_STATUS(current, ipmi_specification_revision) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType38SpecificationRevisionStr(current->ipmi_specification_revision, decbuf, sizeof(decbuf));
				current->decoded.ipmi_specification_revision = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, base_address_modifier_interrupt_info) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType38InterruptInfoStr(current->base_address_modifier_interrupt_info, decbuf, sizeof(decbuf));
				current->decoded.base_address_modifier_interrupt_info = lazybiosDup(decbuf);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

static inline const char* lazybiosType38InterfaceTypeStr(uint8_t interface_type) {
	switch (interface_type) {
		case INTERFACE_TYPE_UNKNOWN:
			return "Unknown";
		case INTERFACE_TYPE_KCS:
			return "KCS: Keyboard Controller Style";
		case INTERFACE_TYPE_SMIC:
			return "SMIC: Server Management Interface Chip";
		case INTERFACE_TYPE_BT:
			return "BT: Block Transfer";
		case INTERFACE_TYPE_SSIF:
			return "SSIF: SMBus System Interface";
		default:
			return "Reserved";
	}
}

static size_t lazybiosType38SpecificationRevisionStr(uint8_t revision, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;

	uint8_t major = revision >> 4;
	uint8_t minor = revision & 0x0F;
	if (major <= 9 && minor <= 9) {
		snprintf(buf, buf_len, "%hhu.%hhu", major, minor);
	} else {
		snprintf(buf, buf_len, "Invalid BCD (0x%02X)", revision);
	}
	return buf ? strlen(buf) : 0;
}

static inline const char* lazybiosType38BaseAddressTypeStr(uint64_t base_address) {
	return (base_address & 1ULL) ? "I/O" : "Memory-mapped";
}

static inline uint64_t lazybiosType38BaseAddressValue(uint64_t base_address, uint8_t modifier) {
	return (base_address & ~1ULL) | ((uint64_t)(modifier >> 4) & 1ULL);
}

static inline const char* lazybiosType38RegisterSpacingStr(uint8_t modifier) {
	switch ((modifier >> 6) & 0x03) {
		case 0:
			return "Successive Byte Boundaries";
		case 1:
			return "32-bit Boundaries";
		case 2:
			return "16-byte Boundaries";
		default:
			return "Reserved";
	}
}

static size_t lazybiosType38InterruptInfoStr(uint8_t interrupt_info, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;

	if (!(interrupt_info & (1U << 3))) {
		snprintf(buf, buf_len, "Not Specified");
		return 0;
	}

	const char* polarity = (interrupt_info & (1U << 1)) ? "Active High" : "Active Low";
	const char* trigger_mode = (interrupt_info & 1U) ? "Level-triggered" : "Edge-triggered";
	snprintf(buf, buf_len, "%s, %s", polarity, trigger_mode);
	return buf ? strlen(buf) : 0;
}

void lazybiosFreeType38(lazybiosType38Array_t* Type38) {
    if (!Type38) return;

	for (size_t i = 0; i < Type38->count; i++) {
		free(Type38->entries[i].decoded.ipmi_specification_revision);
		free(Type38->entries[i].decoded.base_address_modifier_interrupt_info);
	}

    free(Type38->entries);

    free(Type38);
}

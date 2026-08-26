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
 * @file type0.c
 * @brief Implements parsing and decoding for SMBIOS Type 0 BIOS Information.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* File-local decoders; their output is stored in each record's `decoded`. */
static size_t lazybiosType0CharacteristicsStr(uint64_t characteristics, char* buf, size_t buf_len);
static size_t lazybiosType0CharacteristicsExtByte1Str(uint8_t char_ext_byte_1, char* buf, size_t buf_len);
static size_t lazybiosType0CharacteristicsExtByte2Str(uint8_t char_ext_byte_2, char* buf, size_t buf_len);

/* File-local decoders; their results are stored in each record's `decoded`. */
static inline uint16_t lazybiosType0ExtendedROMSizeU16(uint16_t raw, char unit[5]);

#define VENDOR 0x04
#define FIRMWARE_VERSION 0x05
#define BIOS_STARTING_SEGMENT 0x06
#define FIRMWARE_RELEASE_DATE 0x08
#define FIRMWARE_ROM_SIZE 0x09
#define FIRMWARE_CHARACTERISTICS 0x0A
#define FIRMWARE_CHARACTERISTICS_EXTENSION_BYTES 0x12
#define PLATFORM_FIRMWARE_MAJOR_RELEASE 0x14
#define PLATFORM_FIRMWARE_MINOR_RELEASE 0x15
#define EMBEDDED_CONTROLLER_FIRMWARE_MAJOR_RELEASE 0x16
#define EMBEDDED_CONTROLLER_FIRMWARE_MINOR_RELEASE 0x17
#define EXTENDED_FIRMWARE_ROM_SIZE 0x18

lazybiosType0Array_t* lazybiosGetType0(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType0Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;
	const size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_BIOS);
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

		if (type == SMBIOS_TYPE_BIOS) {
			lazybiosType0_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, vendor, len, VENDOR, p, structure_end);
			READSTR(current, version, len, FIRMWARE_VERSION, p, structure_end);
			READSTR(current, release_date, len, FIRMWARE_RELEASE_DATE, p, structure_end);

			READU16(current, bios_starting_segment, len, BIOS_STARTING_SEGMENT, p);
			if (current->bios_starting_segment == 0) LAZYBIOS_MARK_ABSENT(current, bios_starting_segment);

			if (len > FIRMWARE_ROM_SIZE && p[FIRMWARE_ROM_SIZE] == 0xFF) {
				if (lazybiosIsVersionPlus(DMIData, 3, 1) && len >= EXTENDED_FIRMWARE_ROM_SIZE + sizeof(uint16_t)) {
					memcpy(&current->extended_rom_size, p + EXTENDED_FIRMWARE_ROM_SIZE, sizeof(uint16_t));
					lazybiosType0ExtendedROMSizeU16(current->extended_rom_size, current->unit);
					LAZYBIOS_MARK_PRESENT(current, extended_rom_size);
					LAZYBIOS_MARK_PRESENT(current, unit);
				} else {
					current->extended_rom_size = 0;
					LAZYBIOS_MARK_UNREACHABLE(current, extended_rom_size);
					if (lazybiosIsVersionPlus(DMIData, 3, 1)) LAZYBIOS_MARK_ABSENT(current, extended_rom_size);
				}

				current->rom_size = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, rom_size);
				current->field_status.rom_size = LAZYBIOS_FIELD_ABSENT;
			} else if (len > FIRMWARE_ROM_SIZE) {
				current->rom_size = (uint32_t)(p[FIRMWARE_ROM_SIZE] + 1) * 64;
				LAZYBIOS_MARK_PRESENT(current, rom_size);
				current->extended_rom_size = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, extended_rom_size);
			} else {
				current->rom_size = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, rom_size);
				current->extended_rom_size = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, extended_rom_size);
				LAZYBIOS_MARK_ABSENT(current, rom_size);
			}

			READU64(current, characteristics, len, FIRMWARE_CHARACTERISTICS, p);

			if (len > FIRMWARE_CHARACTERISTICS_EXTENSION_BYTES) {
				current->firmware_char_ext_bytes_count =
					(size_t)(len - FIRMWARE_CHARACTERISTICS_EXTENSION_BYTES) > 2
						? 2
						: (size_t)(len - FIRMWARE_CHARACTERISTICS_EXTENSION_BYTES);
				LAZYBIOS_MARK_PRESENT(current, firmware_char_ext_bytes_count);
				current->firmware_char_ext_bytes = malloc(current->firmware_char_ext_bytes_count);
				if (current->firmware_char_ext_bytes) {
					memcpy(current->firmware_char_ext_bytes, p + FIRMWARE_CHARACTERISTICS_EXTENSION_BYTES,
						current->firmware_char_ext_bytes_count);
					LAZYBIOS_MARK_PRESENT(current, firmware_char_ext_bytes);
				}
			}

			if (lazybiosIsVersionPlus(DMIData, 2, 4)) {
				READU8(current, platform_major_release, len, PLATFORM_FIRMWARE_MAJOR_RELEASE, p);
				READU8(current, platform_minor_release, len, PLATFORM_FIRMWARE_MINOR_RELEASE, p);
				READU8(current, ec_major_release, len, EMBEDDED_CONTROLLER_FIRMWARE_MAJOR_RELEASE, p);
				READU8(current, ec_minor_release, len, EMBEDDED_CONTROLLER_FIRMWARE_MINOR_RELEASE, p);
				if (current->platform_major_release == 0xFF && current->platform_minor_release == 0xFF) {
					LAZYBIOS_MARK_ABSENT(current, platform_major_release);
					LAZYBIOS_MARK_ABSENT(current, platform_minor_release);
				}
				if (current->ec_major_release == 0xFF) LAZYBIOS_MARK_ABSENT(current, ec_major_release);
				if (current->ec_minor_release == 0xFF) LAZYBIOS_MARK_ABSENT(current, ec_minor_release);
			} else {
				current->platform_major_release = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, platform_major_release);
				current->platform_minor_release = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, platform_minor_release);
				current->ec_major_release = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, ec_major_release);
				current->ec_minor_release = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, ec_minor_release);
			}

			current->decoded.extended_rom_size = lazybiosType0ExtendedROMSizeU16(
				current->extended_rom_size, current->decoded.extended_rom_size_unit);

			char decbuf[LAZYBIOS_DECODER_BUF_SIZE];
			if (LAZYBIOS_FIELD_STATUS(current, characteristics) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType0CharacteristicsStr(current->characteristics, decbuf, sizeof(decbuf));
				current->decoded.characteristics = lazybiosDup(decbuf);
			}
			/* The extension bytes are a counted array; index only what exists. */
			if (LAZYBIOS_FIELD_STATUS(current, firmware_char_ext_bytes) == LAZYBIOS_FIELD_PRESENT &&
					current->firmware_char_ext_bytes && current->firmware_char_ext_bytes_count >= 1) {
				lazybiosType0CharacteristicsExtByte1Str(current->firmware_char_ext_bytes[0], decbuf, sizeof(decbuf));
				current->decoded.characteristics_ext_byte1 = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, firmware_char_ext_bytes) == LAZYBIOS_FIELD_PRESENT &&
					current->firmware_char_ext_bytes && current->firmware_char_ext_bytes_count >= 2) {
				lazybiosType0CharacteristicsExtByte2Str(current->firmware_char_ext_bytes[1], decbuf, sizeof(decbuf));
				current->decoded.characteristics_ext_byte2 = lazybiosDup(decbuf);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

// Firmware Characteristics
static size_t lazybiosType0CharacteristicsStr(uint64_t characteristics, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	size_t len = 0;
	buf[0] = '\0';

	// Bits 0–1 Reserved
	if (characteristics & (1ull << 2)) lazybiosDecoderAppend(buf, buf_len, &len, "Unknown, ");
	if (characteristics & (1ull << 3)) lazybiosDecoderAppend(buf, buf_len, &len, "Firmware Characteristics Unsupported, ");
	if (characteristics & (1ull << 4)) lazybiosDecoderAppend(buf, buf_len, &len, "ISA Supported, ");
	if (characteristics & (1ull << 5)) lazybiosDecoderAppend(buf, buf_len, &len, "MCA Supported, ");
	if (characteristics & (1ull << 6)) lazybiosDecoderAppend(buf, buf_len, &len, "EISA Supported, ");
	if (characteristics & (1ull << 7)) lazybiosDecoderAppend(buf, buf_len, &len, "PCI Supported, ");
	if (characteristics & (1ull << 8)) lazybiosDecoderAppend(buf, buf_len, &len, "PCMCIA Supported, ");
	if (characteristics & (1ull << 9)) lazybiosDecoderAppend(buf, buf_len, &len, "Plug and Play Supported, ");
	if (characteristics & (1ull << 10)) lazybiosDecoderAppend(buf, buf_len, &len, "APM Supported, ");
	if (characteristics & (1ull << 11)) lazybiosDecoderAppend(buf, buf_len, &len, "Firmware Upgradeable (Flash), ");
	if (characteristics & (1ull << 12)) lazybiosDecoderAppend(buf, buf_len, &len, "Firmware Shadowing Allowed, ");
	if (characteristics & (1ull << 13)) lazybiosDecoderAppend(buf, buf_len, &len, "VL-VESA Supported, ");
	if (characteristics & (1ull << 14)) lazybiosDecoderAppend(buf, buf_len, &len, "ESCD Support Available, ");
	if (characteristics & (1ull << 15)) lazybiosDecoderAppend(buf, buf_len, &len, "Boot from CD Supported, ");
	if (characteristics & (1ull << 16)) lazybiosDecoderAppend(buf, buf_len, &len, "Selectable Boot Supported, ");
	if (characteristics & (1ull << 17)) lazybiosDecoderAppend(buf, buf_len, &len, "Firmware ROM Socketed, ");
	if (characteristics & (1ull << 18)) lazybiosDecoderAppend(buf, buf_len, &len, "Boot from PCMCIA Supported, ");
	if (characteristics & (1ull << 19)) lazybiosDecoderAppend(buf, buf_len, &len, "EDD Specification Supported, ");
	if (characteristics & (1ull << 20)) lazybiosDecoderAppend(buf, buf_len, &len, "Int13 NEC 9800 1.2MB Supported, ");
	if (characteristics & (1ull << 21)) lazybiosDecoderAppend(buf, buf_len, &len, "Int13 Toshiba 1.2MB Supported, ");
	if (characteristics & (1ull << 22)) lazybiosDecoderAppend(buf, buf_len, &len, "Int13 5.25/360KB Supported, ");
	if (characteristics & (1ull << 23)) lazybiosDecoderAppend(buf, buf_len, &len, "Int13 5.25/1.2MB Supported, ");
	if (characteristics & (1ull << 24)) lazybiosDecoderAppend(buf, buf_len, &len, "Int13 3.5/720KB Supported, ");
	if (characteristics & (1ull << 25)) lazybiosDecoderAppend(buf, buf_len, &len, "Int13 3.5/2.88MB Supported, ");
	if (characteristics & (1ull << 26)) lazybiosDecoderAppend(buf, buf_len, &len, "Int5 Print Screen Supported, ");
	if (characteristics & (1ull << 27)) lazybiosDecoderAppend(buf, buf_len, &len, "Int9 Keyboard Services Supported, ");
	if (characteristics & (1ull << 28)) lazybiosDecoderAppend(buf, buf_len, &len, "Int14 Serial Services Supported, ");
	if (characteristics & (1ull << 29)) lazybiosDecoderAppend(buf, buf_len, &len, "Int17 Printer Services Supported, ");
	if (characteristics & (1ull << 30)) lazybiosDecoderAppend(buf, buf_len, &len, "Int10 CGA/Mono Video Services Supported, ");
	if (characteristics & (1ull << 31)) lazybiosDecoderAppend(buf, buf_len, &len, "NEC PC-98 Supported, ");

	// Bits 32–63 are reserved, so we will skip them

	if (len == 0) {
		snprintf(buf, buf_len, "None");
	} else if (len >= 2 && len < buf_len) {
		buf[len - 2] = '\0';
	}
	return buf ? strlen(buf) : 0;
}

// Firmware Characteristics Extension Byte 1
static size_t lazybiosType0CharacteristicsExtByte1Str(uint8_t char_ext_byte_1, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	size_t len = 0;
	buf[0] = '\0';

	if (char_ext_byte_1 & (1 << 0)) lazybiosDecoderAppend(buf, buf_len, &len, "ACPI supported, ");
	if (char_ext_byte_1 & (1 << 1)) lazybiosDecoderAppend(buf, buf_len, &len, "USB Legacy supported, ");
	if (char_ext_byte_1 & (1 << 2)) lazybiosDecoderAppend(buf, buf_len, &len, "AGP supported, ");
	if (char_ext_byte_1 & (1 << 3)) lazybiosDecoderAppend(buf, buf_len, &len, "I2O boot supported, ");
	if (char_ext_byte_1 & (1 << 4)) lazybiosDecoderAppend(buf, buf_len, &len, "LS-120 SuperDisk boot supported, ");
	if (char_ext_byte_1 & (1 << 5)) lazybiosDecoderAppend(buf, buf_len, &len, "ATAPI ZIP drive boot supported, ");
	if (char_ext_byte_1 & (1 << 6)) lazybiosDecoderAppend(buf, buf_len, &len, "1394 boot supported, ");
	if (char_ext_byte_1 & (1 << 7)) lazybiosDecoderAppend(buf, buf_len, &len, "Smart battery supported, ");

	if (len == 0) {
		snprintf(buf, buf_len, "None");
	} else if (len >= 2) {
		buf[len - 2] = '\0';
	}
	return buf ? strlen(buf) : 0;
}

// Firmware Characteristics Extension Byte 2
static size_t lazybiosType0CharacteristicsExtByte2Str(uint8_t char_ext_byte_2, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	size_t len = 0;
	buf[0] = '\0';

	if (char_ext_byte_2 & (1 << 0)) lazybiosDecoderAppend(buf, buf_len, &len, "BIOS Boot Specification supported, ");
	if (char_ext_byte_2 & (1 << 1)) lazybiosDecoderAppend(buf, buf_len, &len, "Function key-initiated network service boot supported, ");
	if (char_ext_byte_2 & (1 << 2)) lazybiosDecoderAppend(buf, buf_len, &len, "Targeted content distribution supported, ");
	if (char_ext_byte_2 & (1 << 3)) lazybiosDecoderAppend(buf, buf_len, &len, "UEFI Specification supported, ");
	if (char_ext_byte_2 & (1 << 4)) lazybiosDecoderAppend(buf, buf_len, &len, "Describes a virtual machine, ");
	if (char_ext_byte_2 & (1 << 5)) lazybiosDecoderAppend(buf, buf_len, &len, "Manufacturing mode supported, ");
	if (char_ext_byte_2 & (1 << 6)) lazybiosDecoderAppend(buf, buf_len, &len, "Manufacturing mode enabled, ");

	if (len == 0) {
		snprintf(buf, buf_len, "None");
	} else if (len >= 2) {
		buf[len - 2] = '\0';
	}
	return buf ? strlen(buf) : 0;
}

// Firmware Extended ROM Size
static inline uint16_t lazybiosType0ExtendedROMSizeU16(uint16_t raw, char unit[5]) {
	uint16_t unit_bits = (raw >> 14) & 0x03;
	uint16_t size_bits = raw & 0x3FFF;

	switch (unit_bits) {
		case 0x0:
			strcpy(unit, "MiB");
			break;
		case 0x1:
			strcpy(unit, "GiB");
			break;
		default:
			strcpy(unit, "RES");
			break;
	}

	return size_bits;
}


void lazybiosFreeType0(lazybiosType0Array_t* Type0) {
    if (!Type0) return;

	for (size_t i = 0; i < Type0->count; i++) {
		free(Type0->entries[i].decoded.characteristics);
		free(Type0->entries[i].decoded.characteristics_ext_byte1);
		free(Type0->entries[i].decoded.characteristics_ext_byte2);
	}

	for (size_t i = 0; i < Type0->count; i++) free(Type0->entries[i].firmware_char_ext_bytes);

    free(Type0->entries);

    free(Type0);
}

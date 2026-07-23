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
 * @file type0.c
 * @brief Implements parsing and decoding for SMBIOS Type 0 BIOS Information.
 * @author LazySeldi
 */

//
// Type 0 ( Platform Firmware Information )
//

#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses the first SMBIOS Type 0 BIOS Information structure.
 *
 * @param Type0 Existing Type 0 pointer value; it is not dereferenced or released.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 0 structure, or NULL on failure or absence.
 */
lazybiosType0_t* lazybiosGetType0(lazybiosType0_t* Type0, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	while (p + SMBIOS_HEADER_SIZE <= end) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_BIOS) {
			Type0 = calloc(1, sizeof(*Type0));
			if (!Type0) return NULL;
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(Type0, vendor, len, VENDOR, p, structure_end);
			READSTR(Type0, version, len, FIRMWARE_VERSION, p, structure_end);
			READSTR(Type0, release_date, len, FIRMWARE_RELEASE_DATE, p, structure_end);

			READU16(Type0, bios_starting_segment, len, BIOS_STARTING_SEGMENT, p);
			if (Type0->bios_starting_segment == 0) LAZYBIOS_MARK_ABSENT(Type0, bios_starting_segment);

			if (len > FIRMWARE_ROM_SIZE && p[FIRMWARE_ROM_SIZE] == 0xFF) {
				if (lazybiosIsVersionPlus(DMIData, 3, 1) && len >= EXTENDED_FIRMWARE_ROM_SIZE + sizeof(uint16_t)) {
					memcpy(&Type0->extended_rom_size, p + EXTENDED_FIRMWARE_ROM_SIZE, sizeof(uint16_t));
					lazybiosType0ExtendedROMSizeU16(Type0->extended_rom_size, Type0->unit);
					LAZYBIOS_MARK_PRESENT(Type0, extended_rom_size);
					LAZYBIOS_MARK_PRESENT(Type0, unit);
				} else {
					Type0->extended_rom_size = 0;
					if (lazybiosIsVersionPlus(DMIData, 3, 1)) LAZYBIOS_MARK_ABSENT(Type0, extended_rom_size);
				}

				Type0->rom_size = 0;
				Type0->field_status.rom_size = LAZYBIOS_FIELD_ABSENT;
			} else if (len > FIRMWARE_ROM_SIZE) {
				Type0->rom_size = (uint32_t)(p[FIRMWARE_ROM_SIZE] + 1) * 64;
				LAZYBIOS_MARK_PRESENT(Type0, rom_size);
				Type0->extended_rom_size = 0;
			} else {
				Type0->rom_size = 0;
				Type0->extended_rom_size = 0;
				LAZYBIOS_MARK_ABSENT(Type0, rom_size);
			}

			READU64(Type0, characteristics, len, FIRMWARE_CHARACTERISTICS, p);

			if (len > FIRMWARE_CHARACTERISTICS_EXTENSION_BYTES) {
				Type0->firmware_char_ext_bytes_count =
					(size_t)(len - FIRMWARE_CHARACTERISTICS_EXTENSION_BYTES) > 2
						? 2
						: (size_t)(len - FIRMWARE_CHARACTERISTICS_EXTENSION_BYTES);
				LAZYBIOS_MARK_PRESENT(Type0, firmware_char_ext_bytes_count);
				Type0->firmware_char_ext_bytes = malloc(Type0->firmware_char_ext_bytes_count);

				if (Type0->firmware_char_ext_bytes) {
					memcpy(Type0->firmware_char_ext_bytes, p + FIRMWARE_CHARACTERISTICS_EXTENSION_BYTES, Type0->firmware_char_ext_bytes_count);
					LAZYBIOS_MARK_PRESENT(Type0, firmware_char_ext_bytes);
				}
			}

			if (lazybiosIsVersionPlus(DMIData, 2, 4)) {
				READU8(Type0, platform_major_release, len, PLATFORM_FIRMWARE_MAJOR_RELEASE, p);
				READU8(Type0, platform_minor_release, len, PLATFORM_FIRMWARE_MINOR_RELEASE, p);
				READU8(Type0, ec_major_release, len, EMBEDDED_CONTROLLER_FIRMWARE_MAJOR_RELEASE, p);
				READU8(Type0, ec_minor_release, len, EMBEDDED_CONTROLLER_FIRMWARE_MINOR_RELEASE, p);
				if (Type0->platform_major_release == 0xFF && Type0->platform_minor_release == 0xFF) {
					LAZYBIOS_MARK_ABSENT(Type0, platform_major_release);
					LAZYBIOS_MARK_ABSENT(Type0, platform_minor_release);
				}
				if (Type0->ec_major_release == 0xFF) LAZYBIOS_MARK_ABSENT(Type0, ec_major_release);
				if (Type0->ec_minor_release == 0xFF) LAZYBIOS_MARK_ABSENT(Type0, ec_minor_release);
			} else {
				Type0->platform_major_release = 0;
				Type0->platform_minor_release = 0;
				Type0->ec_major_release = 0;
				Type0->ec_minor_release = 0;
			}

			return Type0;
		}
		p = DMINext(p, end);
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders

// Firmware Characteristics
/**
 * @brief Decodes BIOS characteristics into a readable string.
 *
 * @param characteristics SMBIOS BIOS characteristics bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType0CharacteristicsStr(uint64_t characteristics, char* buf, size_t buf_len) {
	size_t len = 0;
	buf[0] = '\0';

	// Bits 0–1 Reserved
	if (len < buf_len && (characteristics & (1ull << 2))) len += snprintf(buf + len, buf_len - len, "Unknown, ");
	if (len < buf_len && (characteristics & (1ull << 3))) len += snprintf(buf + len, buf_len - len, "Firmware Characteristics Unsupported, ");
	if (len < buf_len && (characteristics & (1ull << 4))) len += snprintf(buf + len, buf_len - len, "ISA Supported, ");
	if (len < buf_len && (characteristics & (1ull << 5))) len += snprintf(buf + len, buf_len - len, "MCA Supported, ");
	if (len < buf_len && (characteristics & (1ull << 6))) len += snprintf(buf + len, buf_len - len, "EISA Supported, ");
	if (len < buf_len && (characteristics & (1ull << 7))) len += snprintf(buf + len, buf_len - len, "PCI Supported, ");
	if (len < buf_len && (characteristics & (1ull << 8))) len += snprintf(buf + len, buf_len - len, "PCMCIA Supported, ");
	if (len < buf_len && (characteristics & (1ull << 9))) len += snprintf(buf + len, buf_len - len, "Plug and Play Supported, ");
	if (len < buf_len && (characteristics & (1ull << 10))) len += snprintf(buf + len, buf_len - len, "APM Supported, ");
	if (len < buf_len && (characteristics & (1ull << 11))) len += snprintf(buf + len, buf_len - len, "Firmware Upgradeable (Flash), ");
	if (len < buf_len && (characteristics & (1ull << 12))) len += snprintf(buf + len, buf_len - len, "Firmware Shadowing Allowed, ");
	if (len < buf_len && (characteristics & (1ull << 13))) len += snprintf(buf + len, buf_len - len, "VL-VESA Supported, ");
	if (len < buf_len && (characteristics & (1ull << 14))) len += snprintf(buf + len, buf_len - len, "ESCD Support Available, ");
	if (len < buf_len && (characteristics & (1ull << 15))) len += snprintf(buf + len, buf_len - len, "Boot from CD Supported, ");
	if (len < buf_len && (characteristics & (1ull << 16))) len += snprintf(buf + len, buf_len - len, "Selectable Boot Supported, ");
	if (len < buf_len && (characteristics & (1ull << 17))) len += snprintf(buf + len, buf_len - len, "Firmware ROM Socketed, ");
	if (len < buf_len && (characteristics & (1ull << 18))) len += snprintf(buf + len, buf_len - len, "Boot from PCMCIA Supported, ");
	if (len < buf_len && (characteristics & (1ull << 19))) len += snprintf(buf + len, buf_len - len, "EDD Specification Supported, ");
	if (len < buf_len && (characteristics & (1ull << 20))) len += snprintf(buf + len, buf_len - len, "Int13 NEC 9800 1.2MB Supported, ");
	if (len < buf_len && (characteristics & (1ull << 21))) len += snprintf(buf + len, buf_len - len, "Int13 Toshiba 1.2MB Supported, ");
	if (len < buf_len && (characteristics & (1ull << 22))) len += snprintf(buf + len, buf_len - len, "Int13 5.25/360KB Supported, ");
	if (len < buf_len && (characteristics & (1ull << 23))) len += snprintf(buf + len, buf_len - len, "Int13 5.25/1.2MB Supported, ");
	if (len < buf_len && (characteristics & (1ull << 24))) len += snprintf(buf + len, buf_len - len, "Int13 3.5/720KB Supported, ");
	if (len < buf_len && (characteristics & (1ull << 25))) len += snprintf(buf + len, buf_len - len, "Int13 3.5/2.88MB Supported, ");
	if (len < buf_len && (characteristics & (1ull << 26))) len += snprintf(buf + len, buf_len - len, "Int5 Print Screen Supported, ");
	if (len < buf_len && (characteristics & (1ull << 27))) len += snprintf(buf + len, buf_len - len, "Int9 Keyboard Services Supported, ");
	if (len < buf_len && (characteristics & (1ull << 28))) len += snprintf(buf + len, buf_len - len, "Int14 Serial Services Supported, ");
	if (len < buf_len && (characteristics & (1ull << 29))) len += snprintf(buf + len, buf_len - len, "Int17 Printer Services Supported, ");
	if (len < buf_len && (characteristics & (1ull << 30))) len += snprintf(buf + len, buf_len - len, "Int10 CGA/Mono Video Services Supported, ");
	if (len < buf_len && (characteristics & (1ull << 31))) len += snprintf(buf + len, buf_len - len, "NEC PC-98 Supported, ");

	// Bits 32–63 are reserved, so we will skip them

	if (len == 0) {
		snprintf(buf, buf_len, "None");
	} else if (len >= 2 && len < buf_len) {
		buf[len - 2] = '\0';
	}
}

// Firmware Characteristics Extension Byte 1
/**
 * @brief Decodes BIOS characteristics extension byte 1.
 *
 * @param char_ext_byte_1 SMBIOS characteristics extension byte 1.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType0CharacteristicsExtByte1Str(uint8_t char_ext_byte_1, char* buf, size_t buf_len) {
	size_t len = 0;
	buf[0] = '\0';

	if (char_ext_byte_1 & (1 << 0)) len += snprintf(buf + len, buf_len - len, "ACPI supported, ");
	if (char_ext_byte_1 & (1 << 1)) len += snprintf(buf + len, buf_len - len, "USB Legacy supported, ");
	if (char_ext_byte_1 & (1 << 2)) len += snprintf(buf + len, buf_len - len, "AGP supported, ");
	if (char_ext_byte_1 & (1 << 3)) len += snprintf(buf + len, buf_len - len, "I2O boot supported, ");
	if (char_ext_byte_1 & (1 << 4)) len += snprintf(buf + len, buf_len - len, "LS-120 SuperDisk boot supported, ");
	if (char_ext_byte_1 & (1 << 5)) len += snprintf(buf + len, buf_len - len, "ATAPI ZIP drive boot supported, ");
	if (char_ext_byte_1 & (1 << 6)) len += snprintf(buf + len, buf_len - len, "1394 boot supported, ");
	if (char_ext_byte_1 & (1 << 7)) len += snprintf(buf + len, buf_len - len, "Smart battery supported, ");

	if (len == 0) {
		snprintf(buf, buf_len, "None");
	} else if (len >= 2) {
		buf[len - 2] = '\0';
	}
}

// Firmware Characteristics Extension Byte 2
/**
 * @brief Decodes BIOS characteristics extension byte 2.
 *
 * @param char_ext_byte_2 SMBIOS characteristics extension byte 2.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType0CharacteristicsExtByte2Str(uint8_t char_ext_byte_2, char* buf, size_t buf_len) {
	size_t len = 0;
	buf[0] = '\0';

	if (char_ext_byte_2 & (1 << 0)) len += snprintf(buf + len, buf_len - len, "BIOS Boot Specification supported, ");
	if (char_ext_byte_2 & (1 << 1)) len += snprintf(buf + len, buf_len - len, "Function key-initiated network service boot supported, ");
	if (char_ext_byte_2 & (1 << 2)) len += snprintf(buf + len, buf_len - len, "Targeted content distribution supported, ");
	if (char_ext_byte_2 & (1 << 3)) len += snprintf(buf + len, buf_len - len, "UEFI Specification supported, ");
	if (char_ext_byte_2 & (1 << 4)) len += snprintf(buf + len, buf_len - len, "Describes a virtual machine, ");
	if (char_ext_byte_2 & (1 << 5)) len += snprintf(buf + len, buf_len - len, "Manufacturing mode supported, ");
	if (char_ext_byte_2 & (1 << 6)) len += snprintf(buf + len, buf_len - len, "Manufacturing mode enabled, ");

	if (len == 0) {
		snprintf(buf, buf_len, "None");
	} else if (len >= 2) {
		buf[len - 2] = '\0';
	}
}

// Firmware Extended ROM Size
/**
 * @brief Extracts the size and unit from an extended BIOS ROM size field.
 *
 * @param raw Raw SMBIOS extended ROM size value.
 * @param unit Output array that receives "MiB", "GiB", or "RES".
 * @return Size portion of the extended ROM size field.
 */
uint16_t lazybiosType0ExtendedROMSizeU16(uint16_t raw, char unit[5]) {
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

// Free Function
/**
 * @brief Releases a parsed SMBIOS Type 0 structure.
 *
 * @param Type0 Type 0 structure to release.
 */
void lazybiosFreeType0(lazybiosType0_t* Type0) {
	if (!Type0) return;

	free(Type0->vendor);
	free(Type0->version);
	free(Type0->release_date);
	free(Type0->firmware_char_ext_bytes);
	free(Type0);
}

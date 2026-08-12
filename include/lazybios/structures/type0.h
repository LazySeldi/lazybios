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
 * @file type0.h
 * @brief Public API for SMBIOS Type 0 BIOS Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE0_H
#define LAZYBIOS_TYPE0_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 0 fields. */
typedef struct {
	lazybiosFieldStatus_t vendor;
	lazybiosFieldStatus_t version;
	lazybiosFieldStatus_t release_date;
	lazybiosFieldStatus_t bios_starting_segment;
	lazybiosFieldStatus_t rom_size;
	lazybiosFieldStatus_t characteristics;
	lazybiosFieldStatus_t firmware_char_ext_bytes_count;
	lazybiosFieldStatus_t firmware_char_ext_bytes;
	lazybiosFieldStatus_t platform_major_release;
	lazybiosFieldStatus_t platform_minor_release;
	lazybiosFieldStatus_t ec_major_release;
	lazybiosFieldStatus_t ec_minor_release;
	lazybiosFieldStatus_t extended_rom_size;
	lazybiosFieldStatus_t unit;
} lazybiosType0FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 0 BIOS Information.
 * @ingroup api_type0
 */
typedef struct {
	char* vendor;
	char* version;
	char* release_date;
	uint16_t bios_starting_segment;
	uint32_t rom_size;
	uint64_t characteristics;
	size_t firmware_char_ext_bytes_count;
	uint8_t* firmware_char_ext_bytes;
	uint8_t platform_major_release;
	uint8_t platform_minor_release;
	uint8_t ec_major_release;
	uint8_t ec_minor_release;
	uint16_t extended_rom_size;
	char unit[5];
	lazybiosType0FieldStatus_t field_status;
} lazybiosType0_t;

/** @addtogroup api_type0
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 0 BIOS Information structures.
 * @param Type0 Existing Type 0 array pointer value; it is not dereferenced or released.
 * @param type0_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 0 array, or NULL on failure.
 */
lazybiosType0_t* lazybiosGetType0(lazybiosType0_t* Type0, size_t* type0_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes BIOS characteristics into a readable string.
 * @param characteristics SMBIOS BIOS characteristics bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType0CharacteristicsStr(uint64_t characteristics, char* buf, size_t buf_len);

/**
 * @brief Decodes BIOS characteristics extension byte 1.
 * @param char_ext_byte_1 SMBIOS characteristics extension byte 1.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType0CharacteristicsExtByte1Str(uint8_t char_ext_byte_1, char* buf, size_t buf_len);

/**
 * @brief Decodes BIOS characteristics extension byte 2.
 * @param char_ext_byte_2 SMBIOS characteristics extension byte 2.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType0CharacteristicsExtByte2Str(uint8_t char_ext_byte_2, char* buf, size_t buf_len);

/**
 * @brief Extracts the size and unit from an extended BIOS ROM size field.
 * @param raw Raw SMBIOS extended ROM size value.
 * @param unit Output array that receives "MiB", "GiB", or "RES".
 * @return Size portion of the extended ROM size field.
 */
uint16_t lazybiosType0ExtendedROMSizeU16(uint16_t raw, char unit[5]);

/**
 * @brief Releases an array of parsed SMBIOS Type 0 structures.
 * @param Type0 Type 0 array to release.
 * @param type0_count Number of elements in Type0.
 */
void lazybiosFreeType0(lazybiosType0_t* Type0, size_t type0_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
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
 * @brief Decoded forms of the Type 0 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	uint16_t extended_rom_size;
	char extended_rom_size_unit[5];
	char* characteristics;
	char* characteristics_ext_byte1;
	char* characteristics_ext_byte2;
} lazybiosType0Decoded_t;

/**
 * @brief Parsed SMBIOS Type 0 BIOS Information.
 * @ingroup api_type0
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* vendor;
	const char* version;
	const char* release_date;
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
	lazybiosType0Decoded_t decoded;
	lazybiosType0FieldStatus_t field_status;
} lazybiosType0_t;

/**
 * @brief A parsed set of SMBIOS Type 0 structures.
 * @ingroup api_type0
 */
typedef struct {
	lazybiosType0_t* entries;
	size_t count;
} lazybiosType0Array_t;

/** @addtogroup api_type0
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 0 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 0 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType0Array_t* lazybiosGetType0(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 0 structures.
 * @param Type0 Set to release; may be NULL.
 */
void lazybiosFreeType0(lazybiosType0Array_t* Type0);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

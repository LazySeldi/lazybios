/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type31.h
 * @brief Public API for SMBIOS Type 31 Boot Integrity Services Entry Point.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE31_H
#define LAZYBIOS_TYPE31_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 31 fields. */
typedef struct {
	lazybiosFieldStatus_t checksum;
	lazybiosFieldStatus_t reserved_1;
	lazybiosFieldStatus_t reserved_2;
	lazybiosFieldStatus_t bis_entry_point_16;
	lazybiosFieldStatus_t bis_entry_point_32;
	lazybiosFieldStatus_t reserved_3;
	lazybiosFieldStatus_t reserved_4;
	lazybiosFieldStatus_t checksum_valid;
} lazybiosType31FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 31 Boot Integrity Services Entry Point information.
 * @ingroup api_type31
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t checksum;
	uint8_t reserved_1;
	uint16_t reserved_2;
	uint32_t bis_entry_point_16;
	uint32_t bis_entry_point_32;
	uint64_t reserved_3;
	uint32_t reserved_4;
	uint8_t checksum_valid;
	lazybiosType31FieldStatus_t field_status;
} lazybiosType31_t;

/**
 * @brief A parsed set of SMBIOS Type 31 structures.
 * @ingroup api_type31
 */
typedef struct {
	lazybiosType31_t* entries;
	size_t count;
} lazybiosType31Array_t;

/** @addtogroup api_type31
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 31 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 31 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType31Array_t* lazybiosGetType31(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 31 structures.
 * @param Type31 Set to release; may be NULL.
 */
void lazybiosFreeType31(lazybiosType31Array_t* Type31);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

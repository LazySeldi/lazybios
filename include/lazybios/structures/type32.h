/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type32.h
 * @brief Public API for SMBIOS Type 32 System Boot Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE32_H
#define LAZYBIOS_TYPE32_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 32 fields. */
typedef struct {
	lazybiosFieldStatus_t reserved;
	lazybiosFieldStatus_t boot_status;
	lazybiosFieldStatus_t additional_data_size;
	lazybiosFieldStatus_t additional_data;
} lazybiosType32FieldStatus_t;

/**
 * @brief Decoded forms of the Type 32 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* boot_status;
} lazybiosType32Decoded_t;

/**
 * @brief Parsed SMBIOS Type 32 System Boot Information.
 * @ingroup api_type32
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t reserved[6];
	uint8_t boot_status;
	size_t additional_data_size;
	uint8_t* additional_data;
	lazybiosType32Decoded_t decoded;
	lazybiosType32FieldStatus_t field_status;
} lazybiosType32_t;

/**
 * @brief A parsed set of SMBIOS Type 32 structures.
 * @ingroup api_type32
 */
typedef struct {
	lazybiosType32_t* entries;
	size_t count;
} lazybiosType32Array_t;

/** @addtogroup api_type32
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 32 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 32 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType32Array_t* lazybiosGetType32(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 32 structures.
 * @param Type32 Set to release; may be NULL.
 */
void lazybiosFreeType32(lazybiosType32Array_t* Type32);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

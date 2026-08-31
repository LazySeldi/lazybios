/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type44.h
 * @brief Public API for SMBIOS Type 44 Processor Additional Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE44_H
#define LAZYBIOS_TYPE44_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 44 fields. */
typedef struct {
	lazybiosFieldStatus_t referenced_handle;
	lazybiosFieldStatus_t block_length;
	lazybiosFieldStatus_t processor_type;
	lazybiosFieldStatus_t processor_specific_data;
} lazybiosType44FieldStatus_t;

/**
 * @brief Decoded forms of the Type 44 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* processor_type;
} lazybiosType44Decoded_t;

/**
 * @brief Parsed SMBIOS Type 44 Processor Additional Information.
 * @ingroup api_type44
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint16_t referenced_handle;
	uint8_t block_length;
	uint8_t processor_type;
	uint8_t* processor_specific_data;
	lazybiosType44Decoded_t decoded;
	lazybiosType44FieldStatus_t field_status;
} lazybiosType44_t;

/**
 * @brief A parsed set of SMBIOS Type 44 structures.
 * @ingroup api_type44
 */
typedef struct {
	lazybiosType44_t* entries;
	size_t count;
} lazybiosType44Array_t;

/** @addtogroup api_type44
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 44 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 44 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType44Array_t* lazybiosGetType44(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 44 structures.
 * @param Type44 Set to release; may be NULL.
 */
void lazybiosFreeType44(lazybiosType44Array_t* Type44);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

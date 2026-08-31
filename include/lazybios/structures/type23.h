/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type23.h
 * @brief Public API for SMBIOS Type 23 System Reset.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE23_H
#define LAZYBIOS_TYPE23_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 23 fields. */
typedef struct {
	lazybiosFieldStatus_t capabilities;
	lazybiosFieldStatus_t reset_count;
	lazybiosFieldStatus_t reset_limit;
	lazybiosFieldStatus_t timer_interval;
	lazybiosFieldStatus_t timeout;
} lazybiosType23FieldStatus_t;

/**
 * @brief Decoded forms of the Type 23 encoded fields.
 */
typedef struct {
	char* capabilities;
} lazybiosType23Decoded_t;

/**
 * @brief Parsed SMBIOS Type 23 System Reset information.
 * @ingroup api_type23
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t capabilities;
	uint16_t reset_count;
	uint16_t reset_limit;
	uint16_t timer_interval;
	uint16_t timeout;
	lazybiosType23Decoded_t decoded;
	lazybiosType23FieldStatus_t field_status;
} lazybiosType23_t;

/**
 * @brief A parsed set of SMBIOS Type 23 structures.
 * @ingroup api_type23
 */
typedef struct {
	lazybiosType23_t* entries;
	size_t count;
} lazybiosType23Array_t;

/** @addtogroup api_type23
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 23 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 23 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType23Array_t* lazybiosGetType23(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 23 structures.
 * @param Type23 Set to release; may be NULL.
 */
void lazybiosFreeType23(lazybiosType23Array_t* Type23);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

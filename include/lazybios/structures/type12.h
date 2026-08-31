/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type12.h
 * @brief Public API for SMBIOS Type 12 System Configuration Options.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE12_H
#define LAZYBIOS_TYPE12_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 12 fields. */
typedef struct {
	lazybiosFieldStatus_t option_count;
	lazybiosFieldStatus_t options;
} lazybiosType12FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 12 System Configuration Options Information.
 * @ingroup api_type12
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t option_count;
	const char** options;
	lazybiosType12FieldStatus_t field_status;
} lazybiosType12_t;

/**
 * @brief A parsed set of SMBIOS Type 12 structures.
 * @ingroup api_type12
 */
typedef struct {
	lazybiosType12_t* entries;
	size_t count;
} lazybiosType12Array_t;

/** @addtogroup api_type12
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 12 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 12 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType12Array_t* lazybiosGetType12(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 12 structures.
 * @param Type12 Set to release; may be NULL.
 */
void lazybiosFreeType12(lazybiosType12Array_t* Type12);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

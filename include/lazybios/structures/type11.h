/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type11.h
 * @brief Public API for SMBIOS Type 11 OEM Strings.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE11_H
#define LAZYBIOS_TYPE11_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 11 fields. */
typedef struct {
	lazybiosFieldStatus_t string_count;
	lazybiosFieldStatus_t strings;
} lazybiosType11FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 11 OEM Strings Information.
 * @ingroup api_type11
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t string_count;
	const char** strings;
	lazybiosType11FieldStatus_t field_status;
} lazybiosType11_t;

/**
 * @brief A parsed set of SMBIOS Type 11 structures.
 * @ingroup api_type11
 */
typedef struct {
	lazybiosType11_t* entries;
	size_t count;
} lazybiosType11Array_t;

/** @addtogroup api_type11
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 11 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 11 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType11Array_t* lazybiosGetType11(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 11 structures.
 * @param Type11 Set to release; may be NULL.
 */
void lazybiosFreeType11(lazybiosType11Array_t* Type11);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

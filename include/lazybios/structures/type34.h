/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type34.h
 * @brief Public API for SMBIOS Type 34 Management Device.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE34_H
#define LAZYBIOS_TYPE34_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 34 fields. */
typedef struct {
	lazybiosFieldStatus_t description;
	lazybiosFieldStatus_t device_type;
	lazybiosFieldStatus_t address;
	lazybiosFieldStatus_t address_type;
} lazybiosType34FieldStatus_t;

/**
 * @brief Decoded forms of the Type 34 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* address_type;
	const char* device_type;
} lazybiosType34Decoded_t;

/**
 * @brief Parsed SMBIOS Type 34 Management Device information.
 * @ingroup api_type34
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* description;
	uint8_t device_type;
	uint32_t address;
	uint8_t address_type;
	lazybiosType34Decoded_t decoded;
	lazybiosType34FieldStatus_t field_status;
} lazybiosType34_t;

/**
 * @brief A parsed set of SMBIOS Type 34 structures.
 * @ingroup api_type34
 */
typedef struct {
	lazybiosType34_t* entries;
	size_t count;
} lazybiosType34Array_t;

/** @addtogroup api_type34
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 34 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 34 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType34Array_t* lazybiosGetType34(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 34 structures.
 * @param Type34 Set to release; may be NULL.
 */
void lazybiosFreeType34(lazybiosType34Array_t* Type34);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

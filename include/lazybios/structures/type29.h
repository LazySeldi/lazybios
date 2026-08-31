/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type29.h
 * @brief Public API for SMBIOS Type 29 Electrical Current Probe.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE29_H
#define LAZYBIOS_TYPE29_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 29 fields. */
typedef struct {
	lazybiosFieldStatus_t description;
	lazybiosFieldStatus_t location_and_status;
	lazybiosFieldStatus_t maximum_value;
	lazybiosFieldStatus_t minimum_value;
	lazybiosFieldStatus_t resolution;
	lazybiosFieldStatus_t tolerance;
	lazybiosFieldStatus_t accuracy;
	lazybiosFieldStatus_t oem_defined;
	lazybiosFieldStatus_t nominal_value;
} lazybiosType29FieldStatus_t;

/**
 * @brief Decoded forms of the Type 29 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* location;
	const char* status;
} lazybiosType29Decoded_t;

/**
 * @brief Parsed SMBIOS Type 29 Electrical Current Probe information.
 * @ingroup api_type29
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* description;
	uint8_t location_and_status;
	uint16_t maximum_value;
	uint16_t minimum_value;
	uint16_t resolution;
	uint16_t tolerance;
	uint16_t accuracy;
	uint32_t oem_defined;
	uint16_t nominal_value;
	lazybiosType29Decoded_t decoded;
	lazybiosType29FieldStatus_t field_status;
} lazybiosType29_t;

/**
 * @brief A parsed set of SMBIOS Type 29 structures.
 * @ingroup api_type29
 */
typedef struct {
	lazybiosType29_t* entries;
	size_t count;
} lazybiosType29Array_t;

/** @addtogroup api_type29
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 29 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 29 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType29Array_t* lazybiosGetType29(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 29 structures.
 * @param Type29 Set to release; may be NULL.
 */
void lazybiosFreeType29(lazybiosType29Array_t* Type29);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

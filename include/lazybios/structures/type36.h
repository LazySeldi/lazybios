/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type36.h
 * @brief Public API for SMBIOS Type 36 Management Device Threshold Data.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE36_H
#define LAZYBIOS_TYPE36_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 36 fields. */
typedef struct {
	lazybiosFieldStatus_t lower_threshold_non_critical;
	lazybiosFieldStatus_t upper_threshold_non_critical;
	lazybiosFieldStatus_t lower_threshold_critical;
	lazybiosFieldStatus_t upper_threshold_critical;
	lazybiosFieldStatus_t lower_threshold_non_recoverable;
	lazybiosFieldStatus_t upper_threshold_non_recoverable;
} lazybiosType36FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 36 Management Device Threshold Data.
 * @ingroup api_type36
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint16_t lower_threshold_non_critical;
	uint16_t upper_threshold_non_critical;
	uint16_t lower_threshold_critical;
	uint16_t upper_threshold_critical;
	uint16_t lower_threshold_non_recoverable;
	uint16_t upper_threshold_non_recoverable;
	lazybiosType36FieldStatus_t field_status;
} lazybiosType36_t;

/**
 * @brief A parsed set of SMBIOS Type 36 structures.
 * @ingroup api_type36
 */
typedef struct {
	lazybiosType36_t* entries;
	size_t count;
} lazybiosType36Array_t;

/** @addtogroup api_type36
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 36 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 36 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType36Array_t* lazybiosGetType36(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 36 structures.
 * @param Type36 Set to release; may be NULL.
 */
void lazybiosFreeType36(lazybiosType36Array_t* Type36);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type21.h
 * @brief Public API for SMBIOS Type 21 Built-in Pointing Device.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE21_H
#define LAZYBIOS_TYPE21_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 21 fields. */
typedef struct {
	lazybiosFieldStatus_t pointing_device_type;
	lazybiosFieldStatus_t interface;
	lazybiosFieldStatus_t number_of_buttons;
} lazybiosType21FieldStatus_t;

/**
 * @brief Decoded forms of the Type 21 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* interface;
	const char* pointing_device_type;
} lazybiosType21Decoded_t;

/**
 * @brief Parsed SMBIOS Type 21 Built-in Pointing Device.
 * @ingroup api_type21
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t pointing_device_type;
	uint8_t interface;
	uint8_t number_of_buttons;
	lazybiosType21Decoded_t decoded;
	lazybiosType21FieldStatus_t field_status;
} lazybiosType21_t;

/**
 * @brief A parsed set of SMBIOS Type 21 structures.
 * @ingroup api_type21
 */
typedef struct {
	lazybiosType21_t* entries;
	size_t count;
} lazybiosType21Array_t;

/** @addtogroup api_type21
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 21 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 21 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType21Array_t* lazybiosGetType21(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 21 structures.
 * @param Type21 Set to release; may be NULL.
 */
void lazybiosFreeType21(lazybiosType21Array_t* Type21);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

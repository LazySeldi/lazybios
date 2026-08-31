/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type24.h
 * @brief Public API for SMBIOS Type 24 Hardware Security.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE24_H
#define LAZYBIOS_TYPE24_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 24 fields. */
typedef struct {
	lazybiosFieldStatus_t hardware_security_settings;
} lazybiosType24FieldStatus_t;

/**
 * @brief Decoded forms of the Type 24 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* administrator_password_status;
	const char* front_panel_reset_status;
	const char* keyboard_password_status;
	const char* power_on_password_status;
} lazybiosType24Decoded_t;

/**
 * @brief Parsed SMBIOS Type 24 Hardware Security information.
 * @ingroup api_type24
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t hardware_security_settings;
	lazybiosType24Decoded_t decoded;
	lazybiosType24FieldStatus_t field_status;
} lazybiosType24_t;

/**
 * @brief A parsed set of SMBIOS Type 24 structures.
 * @ingroup api_type24
 */
typedef struct {
	lazybiosType24_t* entries;
	size_t count;
} lazybiosType24Array_t;

/** @addtogroup api_type24
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 24 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 24 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType24Array_t* lazybiosGetType24(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 24 structures.
 * @param Type24 Set to release; may be NULL.
 */
void lazybiosFreeType24(lazybiosType24Array_t* Type24);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

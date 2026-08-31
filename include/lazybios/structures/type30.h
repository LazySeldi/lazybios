/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type30.h
 * @brief Public API for SMBIOS Type 30 Out-of-Band Remote Access.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE30_H
#define LAZYBIOS_TYPE30_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 30 fields. */
typedef struct {
	lazybiosFieldStatus_t manufacturer_name;
	lazybiosFieldStatus_t connections;
} lazybiosType30FieldStatus_t;

/**
 * @brief Decoded forms of the Type 30 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* inbound_connection;
	const char* outbound_connection;
} lazybiosType30Decoded_t;

/**
 * @brief Parsed SMBIOS Type 30 Out-of-Band Remote Access information.
 * @ingroup api_type30
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* manufacturer_name;
	uint8_t connections;
	lazybiosType30Decoded_t decoded;
	lazybiosType30FieldStatus_t field_status;
} lazybiosType30_t;

/**
 * @brief A parsed set of SMBIOS Type 30 structures.
 * @ingroup api_type30
 */
typedef struct {
	lazybiosType30_t* entries;
	size_t count;
} lazybiosType30Array_t;

/** @addtogroup api_type30
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 30 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 30 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType30Array_t* lazybiosGetType30(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 30 structures.
 * @param Type30 Set to release; may be NULL.
 */
void lazybiosFreeType30(lazybiosType30Array_t* Type30);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

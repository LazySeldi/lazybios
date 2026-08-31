/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type14.h
 * @brief Public API for SMBIOS Type 14 Group Associations.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE14_H
#define LAZYBIOS_TYPE14_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for an SMBIOS Type 14 group item. */
typedef struct {
	lazybiosFieldStatus_t item_type;
	lazybiosFieldStatus_t item_handle;
} lazybiosType14ItemFieldStatus_t;

/**
 * @brief Parsed member of an SMBIOS Type 14 group association.
 * @ingroup api_type14
 */
typedef struct {
	uint8_t item_type;
	uint16_t item_handle;
	lazybiosType14ItemFieldStatus_t field_status;
} lazybiosType14Item_t;

/** @brief Availability metadata for SMBIOS Type 14 fields. */
typedef struct {
	lazybiosFieldStatus_t group_name;
	lazybiosFieldStatus_t item_count;
	lazybiosFieldStatus_t items;
} lazybiosType14FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 14 Group Associations Information.
 * @ingroup api_type14
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* group_name;
	size_t item_count;
	lazybiosType14Item_t* items;
	lazybiosType14FieldStatus_t field_status;
} lazybiosType14_t;

/**
 * @brief A parsed set of SMBIOS Type 14 structures.
 * @ingroup api_type14
 */
typedef struct {
	lazybiosType14_t* entries;
	size_t count;
} lazybiosType14Array_t;

/** @addtogroup api_type14
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 14 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 14 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType14Array_t* lazybiosGetType14(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 14 structures.
 * @param Type14 Set to release; may be NULL.
 */
void lazybiosFreeType14(lazybiosType14Array_t* Type14);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

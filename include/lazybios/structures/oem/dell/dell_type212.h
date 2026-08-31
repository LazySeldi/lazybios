/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file dell_type212.h
 * @brief Public API for Dell Indexed I/O Access (Dell OEM SMBIOS Type 212).
 * @author LazySeldi
 */

#ifndef LAZYBIOS_OEM_DELL_TYPE212_H
#define LAZYBIOS_OEM_DELL_TYPE212_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif



/** @brief Availability metadata for DELL OEM SMBIOS Type 212 fields. */
typedef struct {
    lazybiosFieldStatus_t index_port;
    lazybiosFieldStatus_t data_port;
    lazybiosFieldStatus_t checksum_type;
    lazybiosFieldStatus_t start_index;
    lazybiosFieldStatus_t end_index;
    lazybiosFieldStatus_t value_index;
    lazybiosFieldStatus_t tokens;
} lazybiosOemDellType212FieldStatus_t;

/**
 * @brief Decoded forms of the Dell Type 212 encoded fields.
 */
typedef struct {
    const char* checksum_type;
} lazybiosOemDellType212Decoded_t;

typedef struct {
    uint16_t token_id;
    uint8_t  location;
    uint8_t  and_mask;
    uint8_t  or_mask;
} lazybiosOemDellType212Token_t;

/**
 * @brief Parsed Dell Indexed I/O Access information.
 * @ingroup api_dell_type212
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
    uint16_t index_port;
    uint16_t data_port;
    uint8_t checksum_type;
    uint8_t start_index;
    uint8_t end_index;
    uint8_t value_index;
    lazybiosOemDellType212Token_t* tokens;
    size_t token_count;
    lazybiosOemDellType212Decoded_t decoded;
    lazybiosOemDellType212FieldStatus_t field_status;
} lazybiosOemDellType212_t;

/**
 * @brief A parsed set of Dell OEM Type 212 structures.
 * @ingroup api_dell_type212
 */
typedef struct {
	lazybiosOemDellType212_t* entries;
	size_t count;
} lazybiosOemDellType212Array_t;

/** @addtogroup api_dell_type212
 * @{
 */

/**
 * @brief Parses all Dell OEM SMBIOS Type 212 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no such structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosOemDellType212Array_t* lazybiosGetOemDellType212(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of Dell OEM Type 212 structures.
 * @param DellType212 Set to release; may be NULL.
 */
void lazybiosFreeOemDellType212(lazybiosOemDellType212Array_t* DellType212);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

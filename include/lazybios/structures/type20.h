/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type20.h
 * @brief Public API for SMBIOS Type 20 Memory Device Mapped Address.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE20_H
#define LAZYBIOS_TYPE20_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 20 fields. */
typedef struct {
	lazybiosFieldStatus_t starting_address;
	lazybiosFieldStatus_t ending_address;
	lazybiosFieldStatus_t memory_device_handle;
	lazybiosFieldStatus_t memory_array_mapped_address_handle;
	lazybiosFieldStatus_t partition_row_position;
	lazybiosFieldStatus_t interleave_position;
	lazybiosFieldStatus_t interleaved_data_depth;
	lazybiosFieldStatus_t extended_starting_address;
	lazybiosFieldStatus_t extended_ending_address;
} lazybiosType20FieldStatus_t;

/**
 * @brief Decoded forms of the Type 20 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	uint64_t ending_address;
	uint64_t starting_address;
} lazybiosType20Decoded_t;

/**
 * @brief Parsed SMBIOS Type 20 Memory Device Mapped Address.
 * @ingroup api_type20
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint32_t starting_address;
	uint32_t ending_address;
	uint16_t memory_device_handle;
	uint16_t memory_array_mapped_address_handle;
	uint8_t partition_row_position;
	uint8_t interleave_position;
	uint8_t interleaved_data_depth;
	uint64_t extended_starting_address;
	uint64_t extended_ending_address;
	lazybiosType20Decoded_t decoded;
	lazybiosType20FieldStatus_t field_status;
} lazybiosType20_t;

/**
 * @brief A parsed set of SMBIOS Type 20 structures.
 * @ingroup api_type20
 */
typedef struct {
	lazybiosType20_t* entries;
	size_t count;
} lazybiosType20Array_t;

/** @addtogroup api_type20
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 20 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 20 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType20Array_t* lazybiosGetType20(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 20 structures.
 * @param Type20 Set to release; may be NULL.
 */
void lazybiosFreeType20(lazybiosType20Array_t* Type20);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

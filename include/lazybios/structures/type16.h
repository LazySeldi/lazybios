/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type16.h
 * @brief Public API for SMBIOS Type 16 Physical Memory Array Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE16_H
#define LAZYBIOS_TYPE16_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 16 fields. */
typedef struct {
	lazybiosFieldStatus_t location;
	lazybiosFieldStatus_t use;
	lazybiosFieldStatus_t memory_error_correction;
	lazybiosFieldStatus_t maximum_capacity;
	lazybiosFieldStatus_t memory_error_information_handle;
	lazybiosFieldStatus_t number_of_memory_devices;
	lazybiosFieldStatus_t extended_maximum_capacity;
} lazybiosType16FieldStatus_t;

/**
 * @brief Decoded forms of the Type 16 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* location;
	uint64_t maximum_capacity;
	const char* memory_error_correction;
	const char* use;
} lazybiosType16Decoded_t;

/**
 * @brief Parsed SMBIOS Type 16 Physical Memory Array Information.
 * @ingroup api_type16
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t location;
	uint8_t use;
	uint8_t memory_error_correction;
	uint32_t maximum_capacity;
	uint16_t memory_error_information_handle;
	uint16_t number_of_memory_devices;
	uint64_t extended_maximum_capacity;
	lazybiosType16Decoded_t decoded;
	lazybiosType16FieldStatus_t field_status;
} lazybiosType16_t;

/**
 * @brief A parsed set of SMBIOS Type 16 structures.
 * @ingroup api_type16
 */
typedef struct {
	lazybiosType16_t* entries;
	size_t count;
} lazybiosType16Array_t;

/** @addtogroup api_type16
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 16 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 16 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType16Array_t* lazybiosGetType16(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 16 structures.
 * @param Type16 Set to release; may be NULL.
 */
void lazybiosFreeType16(lazybiosType16Array_t* Type16);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

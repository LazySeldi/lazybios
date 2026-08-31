/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type37.h
 * @brief Public API for SMBIOS Type 37 Memory Channel.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE37_H
#define LAZYBIOS_TYPE37_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for a Type 37 memory-device entry. */
typedef struct {
	lazybiosFieldStatus_t load;
	lazybiosFieldStatus_t handle;
} lazybiosType37MemoryDeviceFieldStatus_t;

/**
 * @brief Parsed memory-device association in an SMBIOS Type 37 channel.
 * @ingroup api_type37
 */
typedef struct {
	uint8_t load;
	uint16_t handle;
	lazybiosType37MemoryDeviceFieldStatus_t field_status;
} lazybiosType37MemoryDevice_t;

/** @brief Availability metadata for SMBIOS Type 37 fields. */
typedef struct {
	lazybiosFieldStatus_t channel_type;
	lazybiosFieldStatus_t maximum_channel_load;
	lazybiosFieldStatus_t memory_device_count;
	lazybiosFieldStatus_t memory_devices;
} lazybiosType37FieldStatus_t;

/**
 * @brief Decoded forms of the Type 37 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* channel_type;
} lazybiosType37Decoded_t;

/**
 * @brief Parsed SMBIOS Type 37 Memory Channel information.
 * @ingroup api_type37
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t channel_type;
	uint8_t maximum_channel_load;
	uint8_t memory_device_count;
	lazybiosType37MemoryDevice_t* memory_devices;
	lazybiosType37Decoded_t decoded;
	lazybiosType37FieldStatus_t field_status;
} lazybiosType37_t;

/**
 * @brief A parsed set of SMBIOS Type 37 structures.
 * @ingroup api_type37
 */
typedef struct {
	lazybiosType37_t* entries;
	size_t count;
} lazybiosType37Array_t;

/** @addtogroup api_type37
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 37 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 37 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType37Array_t* lazybiosGetType37(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 37 structures.
 * @param Type37 Set to release; may be NULL.
 */
void lazybiosFreeType37(lazybiosType37Array_t* Type37);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

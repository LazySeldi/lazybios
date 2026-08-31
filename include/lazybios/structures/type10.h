/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type10.h
 * @brief Public API for obsolete SMBIOS Type 10 On Board Devices Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE10_H
#define LAZYBIOS_TYPE10_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for an obsolete SMBIOS Type 10 device entry. */
typedef struct {
	lazybiosFieldStatus_t device_type_and_status;
	lazybiosFieldStatus_t description;
} lazybiosType10DeviceFieldStatus_t;

/**
 * @brief Decoded forms of the encoded fields in one record.
 */
typedef struct {
	const char* device_type;
	const char* device_status;
} lazybiosType10DeviceDecoded_t;

/**
 * @brief Parsed device entry from an obsolete SMBIOS Type 10 structure.
 * @ingroup api_type10
 */
typedef struct {
	uint8_t device_type_and_status;
	const char* description;
	lazybiosType10DeviceFieldStatus_t field_status;
	lazybiosType10DeviceDecoded_t decoded;
} lazybiosType10Device_t;

/** @brief Availability metadata for SMBIOS Type 10 fields. */
typedef struct {
	lazybiosFieldStatus_t device_count;
	lazybiosFieldStatus_t devices;
} lazybiosType10FieldStatus_t;


/**
 * @brief Parsed obsolete SMBIOS Type 10 On Board Devices Information.
 * @ingroup api_type10
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	size_t device_count;
	lazybiosType10Device_t* devices;
	lazybiosType10FieldStatus_t field_status;
} lazybiosType10_t;

/**
 * @brief A parsed set of SMBIOS Type 10 structures.
 * @ingroup api_type10
 */
typedef struct {
	lazybiosType10_t* entries;
	size_t count;
} lazybiosType10Array_t;

/** @addtogroup api_type10
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 10 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 10 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType10Array_t* lazybiosGetType10(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 10 structures.
 * @param Type10 Set to release; may be NULL.
 */
void lazybiosFreeType10(lazybiosType10Array_t* Type10);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

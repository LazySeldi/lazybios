/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type22.h
 * @brief Public API for SMBIOS Type 22 Portable Battery.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE22_H
#define LAZYBIOS_TYPE22_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 22 fields. */
typedef struct {
	lazybiosFieldStatus_t location;
	lazybiosFieldStatus_t manufacturer;
	lazybiosFieldStatus_t manufacture_date;
	lazybiosFieldStatus_t serial_number;
	lazybiosFieldStatus_t device_name;
	lazybiosFieldStatus_t device_chemistry;
	lazybiosFieldStatus_t design_capacity;
	lazybiosFieldStatus_t design_voltage;
	lazybiosFieldStatus_t sbds_version_number;
	lazybiosFieldStatus_t maximum_error;
	lazybiosFieldStatus_t sbds_serial_number;
	lazybiosFieldStatus_t sbds_manufacture_date;
	lazybiosFieldStatus_t sbds_device_chemistry;
	lazybiosFieldStatus_t design_capacity_multiplier;
	lazybiosFieldStatus_t oem_specific;
} lazybiosType22FieldStatus_t;

/**
 * @brief Decoded forms of the Type 22 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	uint32_t design_capacity;
	const char* device_chemistry;
	char* sbds_manufacture_date;
} lazybiosType22Decoded_t;

/**
 * @brief Parsed SMBIOS Type 22 Portable Battery.
 * @ingroup api_type22
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	const char* location;
	const char* manufacturer;
	const char* manufacture_date;
	const char* serial_number;
	const char* device_name;
	uint8_t device_chemistry;
	uint16_t design_capacity;
	uint16_t design_voltage;
	const char* sbds_version_number;
	uint8_t maximum_error;
	uint16_t sbds_serial_number;
	uint16_t sbds_manufacture_date;
	const char* sbds_device_chemistry;
	uint8_t design_capacity_multiplier;
	uint32_t oem_specific;
	lazybiosType22Decoded_t decoded;
	lazybiosType22FieldStatus_t field_status;
} lazybiosType22_t;

/**
 * @brief A parsed set of SMBIOS Type 22 structures.
 * @ingroup api_type22
 */
typedef struct {
	lazybiosType22_t* entries;
	size_t count;
} lazybiosType22Array_t;

/** @addtogroup api_type22
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 22 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 22 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType22Array_t* lazybiosGetType22(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 22 structures.
 * @param Type22 Set to release; may be NULL.
 */
void lazybiosFreeType22(lazybiosType22Array_t* Type22);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

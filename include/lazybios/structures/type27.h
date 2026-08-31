/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type27.h
 * @brief Public API for SMBIOS Type 27 Cooling Device.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE27_H
#define LAZYBIOS_TYPE27_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 27 fields. */
typedef struct {
	lazybiosFieldStatus_t temperature_probe_handle;
	lazybiosFieldStatus_t device_type_and_status;
	lazybiosFieldStatus_t cooling_unit_group;
	lazybiosFieldStatus_t oem_defined;
	lazybiosFieldStatus_t nominal_speed;
	lazybiosFieldStatus_t description;
} lazybiosType27FieldStatus_t;

/**
 * @brief Decoded forms of the Type 27 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* device_type;
	const char* status;
} lazybiosType27Decoded_t;

/**
 * @brief Parsed SMBIOS Type 27 Cooling Device information.
 * @ingroup api_type27
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint16_t temperature_probe_handle;
	uint8_t device_type_and_status;
	uint8_t cooling_unit_group;
	uint32_t oem_defined;
	uint16_t nominal_speed;
	const char* description;
	lazybiosType27Decoded_t decoded;
	lazybiosType27FieldStatus_t field_status;
} lazybiosType27_t;

/**
 * @brief A parsed set of SMBIOS Type 27 structures.
 * @ingroup api_type27
 */
typedef struct {
	lazybiosType27_t* entries;
	size_t count;
} lazybiosType27Array_t;

/** @addtogroup api_type27
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 27 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 27 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType27Array_t* lazybiosGetType27(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 27 structures.
 * @param Type27 Set to release; may be NULL.
 */
void lazybiosFreeType27(lazybiosType27Array_t* Type27);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

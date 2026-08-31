/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type43.h
 * @brief Public API for SMBIOS Type 43 TPM Device.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE43_H
#define LAZYBIOS_TYPE43_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 43 fields. */
typedef struct {
	lazybiosFieldStatus_t vendor_id;
	lazybiosFieldStatus_t major_spec_version;
	lazybiosFieldStatus_t minor_spec_version;
	lazybiosFieldStatus_t firmware_version_1;
	lazybiosFieldStatus_t firmware_version_2;
	lazybiosFieldStatus_t description;
	lazybiosFieldStatus_t characteristics;
	lazybiosFieldStatus_t oem_defined;
} lazybiosType43FieldStatus_t;

/**
 * @brief Decoded forms of the Type 43 encoded fields.
 */
typedef struct {
	char* major_spec_version;
	char* characteristics;
} lazybiosType43Decoded_t;

/**
 * @brief Parsed SMBIOS Type 43 TPM Device information.
 * @ingroup api_type43
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	char vendor_id[5];
	uint8_t major_spec_version;
	uint8_t minor_spec_version;
	uint32_t firmware_version_1;
	uint32_t firmware_version_2;
	const char* description;
	uint64_t characteristics;
	uint32_t oem_defined;
	lazybiosType43Decoded_t decoded;
	lazybiosType43FieldStatus_t field_status;
} lazybiosType43_t;

/**
 * @brief A parsed set of SMBIOS Type 43 structures.
 * @ingroup api_type43
 */
typedef struct {
	lazybiosType43_t* entries;
	size_t count;
} lazybiosType43Array_t;

/** @addtogroup api_type43
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 43 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 43 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType43Array_t* lazybiosGetType43(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 43 structures.
 * @param Type43 Set to release; may be NULL.
 */
void lazybiosFreeType43(lazybiosType43Array_t* Type43);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

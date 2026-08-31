/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type25.h
 * @brief Public API for SMBIOS Type 25 System Power Controls.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE25_H
#define LAZYBIOS_TYPE25_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 25 fields. */
typedef struct {
	lazybiosFieldStatus_t next_scheduled_power_on_month;
	lazybiosFieldStatus_t next_scheduled_power_on_day;
	lazybiosFieldStatus_t next_scheduled_power_on_hour;
	lazybiosFieldStatus_t next_scheduled_power_on_minute;
	lazybiosFieldStatus_t next_scheduled_power_on_second;
} lazybiosType25FieldStatus_t;

/**
 * @brief Decoded forms of the Type 25 encoded fields.
 */
typedef struct {
	char* next_scheduled_power_on;
} lazybiosType25Decoded_t;

/**
 * @brief Parsed SMBIOS Type 25 System Power Controls information.
 * @ingroup api_type25
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t next_scheduled_power_on_month;
	uint8_t next_scheduled_power_on_day;
	uint8_t next_scheduled_power_on_hour;
	uint8_t next_scheduled_power_on_minute;
	uint8_t next_scheduled_power_on_second;
	lazybiosType25Decoded_t decoded;
	lazybiosType25FieldStatus_t field_status;
} lazybiosType25_t;

/**
 * @brief A parsed set of SMBIOS Type 25 structures.
 * @ingroup api_type25
 */
typedef struct {
	lazybiosType25_t* entries;
	size_t count;
} lazybiosType25Array_t;

/** @addtogroup api_type25
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 25 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 25 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType25Array_t* lazybiosGetType25(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 25 structures.
 * @param Type25 Set to release; may be NULL.
 */
void lazybiosFreeType25(lazybiosType25Array_t* Type25);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

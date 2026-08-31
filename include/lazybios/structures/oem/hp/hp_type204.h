/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file hp_type204.h
 * @brief Public API for HPE ProLiant System/Rack Locator (HP OEM SMBIOS Type 204).
 * @author LazySeldi
 *
 * @note Experimental: This HP OEM type is newly added and has not been tested
 * with real hardware dumps.
 * @warning Untested.
 */

#ifndef LAZYBIOS_OEM_HP_TYPE204_H
#define LAZYBIOS_OEM_HP_TYPE204_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for HP OEM SMBIOS Type 204 fields. */
typedef struct {
    lazybiosFieldStatus_t rack_name;
    lazybiosFieldStatus_t enclosure_name;
    lazybiosFieldStatus_t enclosure_model;
    lazybiosFieldStatus_t enclosure_serial;
    lazybiosFieldStatus_t enclosure_bays;
    lazybiosFieldStatus_t server_bay;
    lazybiosFieldStatus_t bays_filled;
} lazybiosOemHpType204FieldStatus_t;

/**
 * @brief Parsed HPE ProLiant System/Rack Locator information.
 * @ingroup api_hp_type204
 *
 * @note Experimental and untested.
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
    const char* rack_name;
    const char* enclosure_name;
    const char* enclosure_model;
    const char* enclosure_serial;
    uint8_t enclosure_bays;
    const char* server_bay;
    uint8_t bays_filled;
	lazybiosOemHpType204FieldStatus_t field_status;
} lazybiosOemHpType204_t;

/**
 * @brief A parsed set of Hp OEM Type 204 structures.
 * @ingroup api_hp_type204
 */
typedef struct {
	lazybiosOemHpType204_t* entries;
	size_t count;
} lazybiosOemHpType204Array_t;

/** @addtogroup api_hp_type204
 * @{
 */

/**
 * @brief Parses all Hp OEM SMBIOS Type 204 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no such structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosOemHpType204Array_t* lazybiosGetOemHpType204(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of Hp OEM Type 204 structures.
 * @param HpType204 Set to release; may be NULL.
 */
void lazybiosFreeOemHpType204(lazybiosOemHpType204Array_t* HpType204);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

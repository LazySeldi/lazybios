/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This file is part of lazybios.
 *
 * lazybios is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * lazybios is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lazybios. If not, see <https://www.gnu.org/licenses/>.
 */
/**
 * @file dell_type177.h
 * @brief Public API for Dell BIOS Flags (Dell OEM SMBIOS Type 177).
 * @author LazySeldi
 */

#ifndef LAZYBIOS_OEM_DELL_TYPE177_H
#define LAZYBIOS_OEM_DELL_TYPE177_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for DELL OEM SMBIOS Type 177 fields. */
typedef struct {
    lazybiosFieldStatus_t bios_flags;
    lazybiosFieldStatus_t acpi_wmi_supported;
} lazybiosOemDellType177FieldStatus_t;

/**
 * @brief Decoded forms of the Dell Type 177 encoded fields.
 */
typedef struct {
    const char* acpi_wmi_supported;
} lazybiosOemDellType177Decoded_t;

/**
 * @brief Parsed Dell BIOS Flags information.
 * @ingroup api_dell_type177
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint64_t bios_flags;
	lazybiosOemDellType177Decoded_t decoded;
	lazybiosOemDellType177FieldStatus_t field_status;
} lazybiosOemDellType177_t;

/**
 * @brief A parsed set of Dell OEM Type 177 structures.
 * @ingroup api_dell_type177
 */
typedef struct {
	lazybiosOemDellType177_t* entries;
	size_t count;
} lazybiosOemDellType177Array_t;

/** @addtogroup api_dell_type177
 * @{
 */

/**
 * @brief Parses all Dell OEM SMBIOS Type 177 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no such structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosOemDellType177Array_t* lazybiosGetOemDellType177(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of Dell OEM Type 177 structures.
 * @param DellType177 Set to release; may be NULL.
 */
void lazybiosFreeOemDellType177(lazybiosOemDellType177Array_t* DellType177);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

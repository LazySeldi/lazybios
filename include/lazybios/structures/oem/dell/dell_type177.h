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
 * @brief Public API for DELL OEM SMBIOS Type 177 Information.
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
typedef struct {3
    lazybiosFieldStatus_t acpi_wmi_supported;
} lazybiosOemDellType177FieldStatus_t;

/**
 * @brief Parsed DELL OEM SMBIOS Type 177 Information.
 * @ingroup api_dell_type177
 */
typedef struct {

    const char* acpi_wmi_supported; //  Yes | No
	lazybiosOemDellType177FieldStatus_t field_status;
} lazybiosOemDellType177_t;

/** @addtogroup api_dell_type177
 * @{
 */

/**
 * @brief Parses all DELL OEM SMBIOS Type 177 Information structures.
 * @param DELLType177 Existing DELL Type 177 array pointer value.
 * @param delltype177_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated DELL Type 177 array, or NULL on failure.
 */
lazybiosOemDellType177_t* lazybiosGetOemDellType177(lazybiosOemDellType177_t* DELLType177, size_t* delltype177_count, lazybiosDMI_t* DMIData);

/**
 * @brief Releases an array of parsed DELL OEM SMBIOS Type 177 structures.
 * @param DELLType177 DELL Type 177 array to release.
 * @param delltype177_count Number of elements in DELLType177.
 */
void lazybiosFreeOemDellType177(lazybiosOemDellType177_t* DELLType177, size_t delltype177_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

/*
SPDX-License-Identifier: LGPL-2.1-or-later
This file is part of lazybios.
lazybios is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.
lazybios is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License
along with lazybios. If not, see <https://www.gnu.org/licenses/>.
*/
/*
@file lazybios_json.h
@brief cJSON serialization API for parsed SMBIOS structures.
@author LazySeldi
*/

#ifndef LAZYBIOS_JSON_H
#define LAZYBIOS_JSON_H

#include "lazybios.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
@brief Sentinel string written for fields with ::LAZYBIOS_FIELD_UNREACHABLE status.
Distinguishes fields that the firmware structure is physically too short to
contain (newer spec revision required) from fields that are simply absent
(::LAZYBIOS_FIELD_ABSENT), which are written as JSON null.
*/
#define LAZYBIOS_JSON_UNREACHABLE "N/A"

/**
@brief Serialises lazybios library metadata and the parsed SMBIOS entry
       point into a cJSON object and attaches it to @p root under the key
       "smbios_info".
@param DMIData   Raw DMI container holding the parsed entry point.
@param backend   Backend that was used to obtain the data; controls whether
                 the table address field is included.
@param root      cJSON object to attach the resulting object to.
*/
void lazybiosExtJSONAddSMBIOSInfo(const lazybiosDMI_t* DMIData,
                                  lazybiosBackend_t backend, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 0 (BIOS Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type0".
@param type0     Pointer to the first element of a parsed Type 0 array.
@param count     Number of elements in @p type0.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType0(const lazybiosType0_t* type0, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 1 (System Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type1".
@param type1     Pointer to the first element of a parsed Type 1 array.
@param count     Number of elements in @p type1.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType1(const lazybiosType1_t* type1, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 2 (Baseboard Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type2".
@param type2     Pointer to the first element of a parsed Type 2 array.
@param count     Number of elements in @p type2.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType2(const lazybiosType2_t* type2, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 3 (Chassis Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type3".
@param type3     Pointer to the first element of a parsed Type 3 array.
@param count     Number of elements in @p type3.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType3(const lazybiosType3_t* type3, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 4 (Processor Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type4".
@param type4     Pointer to the first element of a parsed Type 4 array.
@param count     Number of elements in @p type4.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType4(const lazybiosType4_t* type4, size_t count, cJSON* root);

#ifdef __cplusplus
}
#endif

#endif
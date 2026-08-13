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
/*
 * @file lazybios_json.h
 * @brief cJSON serialization API for parsed SMBIOS structures.
 * @ingroup api_extensions
 * @author LazySeldi
 */

#ifndef LAZYBIOS_JSON_H
#define LAZYBIOS_JSON_H

#include "lazybios/lazybios.h"
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
@brief Note on the per-type lazybiosExtJSONAddTypeN() functions below:
       when the corresponding structure array is NULL or @p count is 0
       (i.e. no structures of that type were found on the system), the
       "TypeN" key is still attached to @p root, but its value is a
       plain JSON string describing the failure (e.g. "Failed to get
       Memory Controller information") instead of an array of objects.
       Callers should check the JSON type of "TypeN" (array vs string)
       to distinguish the two cases.
*/

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

/**
@brief Serialises an array of parsed SMBIOS Type 5 (Memory Controller
       Information, obsolete) structures into a cJSON array and attaches
       it to @p root under the key "Type5".
@param type5     Pointer to the first element of a parsed Type 5 array.
@param count     Number of elements in @p type5.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType5(const lazybiosType5_t* type5, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 6 (Memory Module
       Information, obsolete) structures into a cJSON array and attaches
       it to @p root under the key "Type6".
@param type6     Pointer to the first element of a parsed Type 6 array.
@param count     Number of elements in @p type6.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType6(const lazybiosType6_t* type6, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 7 (Cache Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type7".
@param type7     Pointer to the first element of a parsed Type 7 array.
@param count     Number of elements in @p type7.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType7(const lazybiosType7_t* type7, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 8 (Port Connector
       Information) structures into a cJSON array and attaches it to
       @p root under the key "Type8".
@param type8     Pointer to the first element of a parsed Type 8 array.
@param count     Number of elements in @p type8.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType8(const lazybiosType8_t* type8, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 9 (System Slots
       Information) structures into a cJSON array and attaches it to
       @p root under the key "Type9".
@param type9     Pointer to the first element of a parsed Type 9 array.
@param count     Number of elements in @p type9.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType9(const lazybiosType9_t* type9, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 10 (On Board Devices
       Information, obsolete) structures into a cJSON array and attaches
       it to @p root under the key "Type10".
@param type10    Pointer to the first element of a parsed Type 10 array.
@param count     Number of elements in @p type10.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType10(const lazybiosType10_t* type10, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 11 (OEM Strings)
       structures into a cJSON array and attaches it to @p root under
       the key "Type11".
@param type11    Pointer to the first element of a parsed Type 11 array.
@param count     Number of elements in @p type11.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType11(const lazybiosType11_t* type11, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 12 (System Configuration
       Options) structures into a cJSON array and attaches it to @p root
       under the key "Type12".
@param type12    Pointer to the first element of a parsed Type 12 array.
@param count     Number of elements in @p type12.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType12(const lazybiosType12_t* type12, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 13 (Firmware Language
       Information) structures into a cJSON array and attaches it to
       @p root under the key "Type13".
@param type13    Pointer to the first element of a parsed Type 13 array.
@param count     Number of elements in @p type13.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType13(const lazybiosType13_t* type13, size_t count, cJSON* root);
 
/**
@brief Serialises an array of parsed SMBIOS Type 14 (Group Associations)
       structures into a cJSON array and attaches it to @p root under
       the key "Type14".
@param type14    Pointer to the first element of a parsed Type 14 array.
@param count     Number of elements in @p type14.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType14(const lazybiosType14_t* type14, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 15 (System Event Log)
       structures into a cJSON array and attaches it to @p root under
       the key "Type15".
@param type15    Pointer to the first element of a parsed Type 15 array.
@param count     Number of elements in @p type15.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType15(const lazybiosType15_t* type15, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 16 (Physical Memory Array)
       structures into a cJSON array and attaches it to @p root under
       the key "Type16".
@param type16    Pointer to the first element of a parsed Type 16 array.
@param count     Number of elements in @p type16.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType16(const lazybiosType16_t* type16, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 17 (Memory Device)
       structures into a cJSON array and attaches it to @p root under
       the key "Type17".
@param type17    Pointer to the first element of a parsed Type 17 array.
@param count     Number of elements in @p type17.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType17(const lazybiosType17_t* type17, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 18 (32-Bit Memory Error Info)
       structures into a cJSON array and attaches it to @p root under
       the key "Type18".
@param type18    Pointer to the first element of a parsed Type 18 array.
@param count     Number of elements in @p type18.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType18(const lazybiosType18_t* type18, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 19 (Memory Array Mapped Address)
       structures into a cJSON array and attaches it to @p root under
       the key "Type19".
@param type19    Pointer to the first element of a parsed Type 19 array.
@param count     Number of elements in @p type19.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType19(const lazybiosType19_t* type19, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 20 (Memory Device Mapped Address)
       structures into a cJSON array and attaches it to @p root under
       the key "Type20".
@param type20    Pointer to the first element of a parsed Type 20 array.
@param count     Number of elements in @p type20.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType20(const lazybiosType20_t* type20, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 21 (Built-in Pointing Device)
       structures into a cJSON array and attaches it to @p root under
       the key "Type21".
@param type21    Pointer to the first element of a parsed Type 21 array.
@param count     Number of elements in @p type21.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType21(const lazybiosType21_t* type21, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 22 (Portable Battery)
       structures into a cJSON array and attaches it to @p root under
       the key "Type22".
@param type22    Pointer to the first element of a parsed Type 22 array.
@param count     Number of elements in @p type22.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType22(const lazybiosType22_t* type22, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 23 (System Reset)
       structures into a cJSON array and attaches it to @p root under
       the key "Type23".
@param type23    Pointer to the first element of a parsed Type 23 array.
@param count     Number of elements in @p type23.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType23(const lazybiosType23_t* type23, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 24 (Hardware Security)
       structures into a cJSON array and attaches it to @p root under
       the key "Type24".
@param type24    Pointer to the first element of a parsed Type 24 array.
@param count     Number of elements in @p type24.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType24(const lazybiosType24_t* type24, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 25 (System Power Controls)
       structures into a cJSON array and attaches it to @p root under
       the key "Type25".
@param type25    Pointer to the first element of a parsed Type 25 array.
@param count     Number of elements in @p type25.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType25(const lazybiosType25_t* type25, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 26 (Voltage Probe)
       structures into a cJSON array and attaches it to @p root under
       the key "Type26".
@param type26    Pointer to the first element of a parsed Type 26 array.
@param count     Number of elements in @p type26.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType26(const lazybiosType26_t* type26, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 27 (Cooling Device)
       structures into a cJSON array and attaches it to @p root under
       the key "Type27".
@param type27    Pointer to the first element of a parsed Type 27 array.
@param count     Number of elements in @p type27.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType27(const lazybiosType27_t* type27, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 28 (Temperature Probe)
       structures into a cJSON array and attaches it to @p root under
       the key "Type28".
@param type28    Pointer to the first element of a parsed Type 28 array.
@param count     Number of elements in @p type28.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType28(const lazybiosType28_t* type28, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 29 (Electrical Current Probe)
       structures into a cJSON array and attaches it to @p root under
       the key "Type29".
@param type29    Pointer to the first element of a parsed Type 29 array.
@param count     Number of elements in @p type29.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType29(const lazybiosType29_t* type29, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 30 (Out-of-Band Remote Access)
       structures into a cJSON array and attaches it to @p root under
       the key "Type30".
@param type30    Pointer to the first element of a parsed Type 30 array.
@param count     Number of elements in @p type30.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType30(const lazybiosType30_t* type30, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 31 (Boot Integrity Services)
       structures into a cJSON array and attaches it to @p root under
       the key "Type31".
@param type31    Pointer to the first element of a parsed Type 31 array.
@param count     Number of elements in @p type31.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType31(const lazybiosType31_t* type31, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 32 (System Boot)
       structures into a cJSON array and attaches it to @p root under
       the key "Type32".
@param type32    Pointer to the first element of a parsed Type 32 array.
@param count     Number of elements in @p type32.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType32(const lazybiosType32_t* type32, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 33 (64-Bit Memory Error Info)
       structures into a cJSON array and attaches it to @p root under
       the key "Type33".
@param type33    Pointer to the first element of a parsed Type 33 array.
@param count     Number of elements in @p type33.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType33(const lazybiosType33_t* type33, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 34 (Management Device)
       structures into a cJSON array and attaches it to @p root under
       the key "Type34".
@param type34    Pointer to the first element of a parsed Type 34 array.
@param count     Number of elements in @p type34.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType34(const lazybiosType34_t* type34, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 35 (Management Device Component)
       structures into a cJSON array and attaches it to @p root under
       the key "Type35".
@param type35    Pointer to the first element of a parsed Type 35 array.
@param count     Number of elements in @p type35.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType35(const lazybiosType35_t* type35, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 36 (Management Device Threshold Data)
       structures into a cJSON array and attaches it to @p root under
       the key "Type36".
@param type36    Pointer to the first element of a parsed Type 36 array.
@param count     Number of elements in @p type36.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType36(const lazybiosType36_t* type36, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 37 (Memory Channel)
       structures into a cJSON array and attaches it to @p root under
       the key "Type37".
@param type37    Pointer to the first element of a parsed Type 37 array.
@param count     Number of elements in @p type37.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType37(const lazybiosType37_t* type37, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 38 (IPMI Device)
       structures into a cJSON array and attaches it to @p root under
       the key "Type38".
@param type38    Pointer to the first element of a parsed Type 38 array.
@param count     Number of elements in @p type38.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType38(const lazybiosType38_t* type38, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 39 (System Power Supply)
       structures into a cJSON array and attaches it to @p root under
       the key "Type39".
@param type39    Pointer to the first element of a parsed Type 39 array.
@param count     Number of elements in @p type39.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType39(const lazybiosType39_t* type39, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 40 (Additional Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type40".
@param type40    Pointer to the first element of a parsed Type 40 array.
@param count     Number of elements in @p type40.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType40(const lazybiosType40_t* type40, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 41 (Onboard Devices Extended Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type41".
@param type41    Pointer to the first element of a parsed Type 41 array.
@param count     Number of elements in @p type41.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType41(const lazybiosType41_t* type41, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 42 (Management Controller Host Interface)
       structures into a cJSON array and attaches it to @p root under
       the key "Type42".
@param type42    Pointer to the first element of a parsed Type 42 array.
@param count     Number of elements in @p type42.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType42(const lazybiosType42_t* type42, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 43 (TPM Device)
       structures into a cJSON array and attaches it to @p root under
       the key "Type43".
@param type43    Pointer to the first element of a parsed Type 43 array.
@param count     Number of elements in @p type43.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType43(const lazybiosType43_t* type43, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 44 (Processor Additional Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type44".
@param type44    Pointer to the first element of a parsed Type 44 array.
@param count     Number of elements in @p type44.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType44(const lazybiosType44_t* type44, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 45 (Firmware Inventory Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type45".
@param type45    Pointer to the first element of a parsed Type 45 array.
@param count     Number of elements in @p type45.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType45(const lazybiosType45_t* type45, size_t count, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 46 (String Property)
       structures into a cJSON array and attaches it to @p root under
       the key "Type46".
@param type46    Pointer to the first element of a parsed Type 46 array.
@param count     Number of elements in @p type46.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType46(const lazybiosType46_t* type46, size_t count, cJSON* root);

#ifdef __cplusplus
}
#endif

#endif
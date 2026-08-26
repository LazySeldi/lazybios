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

/** @addtogroup api_extensions
 * @{
 */

/**
@brief Output contract shared by the per-type lazybiosExtJSONAddTypeN() functions.

       Each attaches its result to @p root under the matching "TypeN" key:
       an array of record objects, an empty array when the table contains no
       structure of that type, or JSON null when the serialiser was handed a
       NULL set. That mirrors the getters, where a non-NULL result with a zero
       count means "none present" and NULL means the call failed.

       Within a record, the top-level members are the raw encodings the
       firmware reported, and their human-readable forms live in a nested
       "decoded" object. Every record also carries "handle" and "length".
       Fields that are absent or unreachable are written as null.
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
@param type0 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType0(const lazybiosType0Array_t* type0, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 1 (System Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type1".
@param type1 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType1(const lazybiosType1Array_t* type1, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 2 (Baseboard Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type2".
@param type2 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType2(const lazybiosType2Array_t* type2, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 3 (Chassis Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type3".
@param type3 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType3(const lazybiosType3Array_t* type3, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 4 (Processor Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type4".
@param type4 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType4(const lazybiosType4Array_t* type4, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 5 (Memory Controller
       Information, obsolete) structures into a cJSON array and attaches
       it to @p root under the key "Type5".
@param type5 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType5(const lazybiosType5Array_t* type5, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 6 (Memory Module
       Information, obsolete) structures into a cJSON array and attaches
       it to @p root under the key "Type6".
@param type6 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType6(const lazybiosType6Array_t* type6, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 7 (Cache Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type7".
@param type7 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType7(const lazybiosType7Array_t* type7, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 8 (Port Connector
       Information) structures into a cJSON array and attaches it to
       @p root under the key "Type8".
@param type8 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType8(const lazybiosType8Array_t* type8, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 9 (System Slots
       Information) structures into a cJSON array and attaches it to
       @p root under the key "Type9".
@param type9 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType9(const lazybiosType9Array_t* type9, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 10 (On Board Devices
       Information, obsolete) structures into a cJSON array and attaches
       it to @p root under the key "Type10".
@param type10 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType10(const lazybiosType10Array_t* type10, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 11 (OEM Strings)
       structures into a cJSON array and attaches it to @p root under
       the key "Type11".
@param type11 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType11(const lazybiosType11Array_t* type11, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 12 (System Configuration
       Options) structures into a cJSON array and attaches it to @p root
       under the key "Type12".
@param type12 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType12(const lazybiosType12Array_t* type12, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 13 (Firmware Language
       Information) structures into a cJSON array and attaches it to
       @p root under the key "Type13".
@param type13 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType13(const lazybiosType13Array_t* type13, cJSON* root);
 
/**
@brief Serialises an array of parsed SMBIOS Type 14 (Group Associations)
       structures into a cJSON array and attaches it to @p root under
       the key "Type14".
@param type14 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType14(const lazybiosType14Array_t* type14, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 15 (System Event Log)
       structures into a cJSON array and attaches it to @p root under
       the key "Type15".
@param type15 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType15(const lazybiosType15Array_t* type15, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 16 (Physical Memory Array)
       structures into a cJSON array and attaches it to @p root under
       the key "Type16".
@param type16 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType16(const lazybiosType16Array_t* type16, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 17 (Memory Device)
       structures into a cJSON array and attaches it to @p root under
       the key "Type17".
@param type17 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType17(const lazybiosType17Array_t* type17, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 18 (32-Bit Memory Error Info)
       structures into a cJSON array and attaches it to @p root under
       the key "Type18".
@param type18 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType18(const lazybiosType18Array_t* type18, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 19 (Memory Array Mapped Address)
       structures into a cJSON array and attaches it to @p root under
       the key "Type19".
@param type19 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType19(const lazybiosType19Array_t* type19, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 20 (Memory Device Mapped Address)
       structures into a cJSON array and attaches it to @p root under
       the key "Type20".
@param type20 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType20(const lazybiosType20Array_t* type20, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 21 (Built-in Pointing Device)
       structures into a cJSON array and attaches it to @p root under
       the key "Type21".
@param type21 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType21(const lazybiosType21Array_t* type21, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 22 (Portable Battery)
       structures into a cJSON array and attaches it to @p root under
       the key "Type22".
@param type22 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType22(const lazybiosType22Array_t* type22, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 23 (System Reset)
       structures into a cJSON array and attaches it to @p root under
       the key "Type23".
@param type23 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType23(const lazybiosType23Array_t* type23, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 24 (Hardware Security)
       structures into a cJSON array and attaches it to @p root under
       the key "Type24".
@param type24 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType24(const lazybiosType24Array_t* type24, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 25 (System Power Controls)
       structures into a cJSON array and attaches it to @p root under
       the key "Type25".
@param type25 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType25(const lazybiosType25Array_t* type25, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 26 (Voltage Probe)
       structures into a cJSON array and attaches it to @p root under
       the key "Type26".
@param type26 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType26(const lazybiosType26Array_t* type26, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 27 (Cooling Device)
       structures into a cJSON array and attaches it to @p root under
       the key "Type27".
@param type27 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType27(const lazybiosType27Array_t* type27, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 28 (Temperature Probe)
       structures into a cJSON array and attaches it to @p root under
       the key "Type28".
@param type28 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType28(const lazybiosType28Array_t* type28, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 29 (Electrical Current Probe)
       structures into a cJSON array and attaches it to @p root under
       the key "Type29".
@param type29 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType29(const lazybiosType29Array_t* type29, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 30 (Out-of-Band Remote Access)
       structures into a cJSON array and attaches it to @p root under
       the key "Type30".
@param type30 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType30(const lazybiosType30Array_t* type30, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 31 (Boot Integrity Services)
       structures into a cJSON array and attaches it to @p root under
       the key "Type31".
@param type31 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType31(const lazybiosType31Array_t* type31, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 32 (System Boot)
       structures into a cJSON array and attaches it to @p root under
       the key "Type32".
@param type32 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType32(const lazybiosType32Array_t* type32, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 33 (64-Bit Memory Error Info)
       structures into a cJSON array and attaches it to @p root under
       the key "Type33".
@param type33 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType33(const lazybiosType33Array_t* type33, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 34 (Management Device)
       structures into a cJSON array and attaches it to @p root under
       the key "Type34".
@param type34 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType34(const lazybiosType34Array_t* type34, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 35 (Management Device Component)
       structures into a cJSON array and attaches it to @p root under
       the key "Type35".
@param type35 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType35(const lazybiosType35Array_t* type35, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 36 (Management Device Threshold Data)
       structures into a cJSON array and attaches it to @p root under
       the key "Type36".
@param type36 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType36(const lazybiosType36Array_t* type36, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 37 (Memory Channel)
       structures into a cJSON array and attaches it to @p root under
       the key "Type37".
@param type37 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType37(const lazybiosType37Array_t* type37, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 38 (IPMI Device)
       structures into a cJSON array and attaches it to @p root under
       the key "Type38".
@param type38 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType38(const lazybiosType38Array_t* type38, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 39 (System Power Supply)
       structures into a cJSON array and attaches it to @p root under
       the key "Type39".
@param type39 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType39(const lazybiosType39Array_t* type39, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 40 (Additional Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type40".
@param type40 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType40(const lazybiosType40Array_t* type40, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 41 (Onboard Devices Extended Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type41".
@param type41 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType41(const lazybiosType41Array_t* type41, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 42 (Management Controller Host Interface)
       structures into a cJSON array and attaches it to @p root under
       the key "Type42".
@param type42 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType42(const lazybiosType42Array_t* type42, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 43 (TPM Device)
       structures into a cJSON array and attaches it to @p root under
       the key "Type43".
@param type43 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType43(const lazybiosType43Array_t* type43, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 44 (Processor Additional Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type44".
@param type44 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType44(const lazybiosType44Array_t* type44, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 45 (Firmware Inventory Information)
       structures into a cJSON array and attaches it to @p root under
       the key "Type45".
@param type45 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType45(const lazybiosType45Array_t* type45, cJSON* root);

/**
@brief Serialises an array of parsed SMBIOS Type 46 (String Property)
       structures into a cJSON array and attaches it to @p root under
       the key "Type46".
@param type46 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddType46(const lazybiosType46Array_t* type46, cJSON* root);

/**
@brief OEM serialisers nest their output as oem.&lt;vendor&gt;.TypeN, mirroring
       ctx->oem->dell->TypeN, because an OEM structure number is only
       meaningful together with its vendor.
*/

/**
@brief Serialises an array of parsed Dell OEM Type 177 (Dell BIOS Flags)
       structures into a cJSON array and attaches it to @p root under
       the key "DellType177".
@param dell177 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddOemDellType177(const lazybiosOemDellType177Array_t* dell177, cJSON* root);

/**
@brief Serialises an array of parsed Dell OEM Type 212 (Dell Indexed I/O
       Access) structures into a cJSON array and attaches it to @p root
       under the key "DellType212".

       Port addresses, indices, and token fields are written as hexadecimal
       strings. Each element carries a "token_count" and a "tokens" array of
       {token_id, location, and_mask, or_mask} objects.
@param dell212 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddOemDellType212(const lazybiosOemDellType212Array_t* dell212, cJSON* root);

/**
@brief Serialises an array of parsed Dell OEM Type 218 (Dell Token Interface)
       structures into a cJSON array and attaches it to @p root under the
       key "DellType218".

       The command port, command code, class bitmap, and token fields are
       written as hexadecimal strings. Each element carries a "token_count"
       and a "tokens" array of {token_id, location, value} objects.
@param dell218 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddOemDellType218(const lazybiosOemDellType218Array_t* dell218, cJSON* root);

/**
@brief Serialises an array of parsed HP OEM Type 204 (HPE ProLiant
       System/Rack Locator) structures into a cJSON array and attaches it
       to @p root under the key "HpType204".
@param hp204 Parsed set to serialise.
@param root      cJSON object to attach the resulting array to.
*/
void lazybiosExtJSONAddOemHpType204(const lazybiosOemHpType204Array_t* hp204, cJSON* root);

/**
@brief Parses every implemented structure type and serialises the lot into @p root.

       Equivalent to calling lazybiosParseAll() followed by every
       lazybiosExtJSONAdd*() function, including "smbios_info". Types already
       parsed in @p ctx are reused rather than parsed again.
@param ctx  Context holding loaded SMBIOS data.
@param root cJSON object to attach the results to.
@return 0 on success, or -1 if @p ctx holds no usable table or @p root is NULL.
*/
LAZYBIOS_WARN_UNUSED int lazybiosParseJSONAll(lazybiosCTX_t* ctx, cJSON* root);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

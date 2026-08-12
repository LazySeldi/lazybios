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
@file lazybios_json.c
@brief cJSON serialization for parsed SMBIOS structures.
@author LazySeldi
*/

#include "lazybios/json/lazybios_json.h"
#include "lazybios/json/cJSON.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

static inline void cjson_add_str(cJSON* obj, const char* key, const char* value) {
    cJSON_AddStringToObject(obj, key, value ? value : "");
}

static inline void cjson_field_str(cJSON* obj, const char* key,
                                   lazybiosFieldStatus_t status, const char* value) {
    if (status == LAZYBIOS_FIELD_PRESENT) {
        cJSON_AddStringToObject(obj, key, value ? value : "");
    } else if (status == LAZYBIOS_FIELD_UNREACHABLE) {
        cJSON_AddStringToObject(obj, key, LAZYBIOS_JSON_UNREACHABLE);
    } else {
        cJSON_AddNullToObject(obj, key);
    }
}

static inline void cjson_field_num(cJSON* obj, const char* key,
                                   lazybiosFieldStatus_t status, double value) {
    if (status == LAZYBIOS_FIELD_PRESENT) {
        cJSON_AddNumberToObject(obj, key, value);
    } else if (status == LAZYBIOS_FIELD_UNREACHABLE) {
        cJSON_AddStringToObject(obj, key, LAZYBIOS_JSON_UNREACHABLE);
    } else {
        cJSON_AddNullToObject(obj, key);
    }
}

void lazybiosExtJSONAddSMBIOSInfo(const lazybiosDMI_t* DMIData,
                                  lazybiosBackend_t backend, cJSON* root) {
    if (!DMIData || !root) {
        return;
    }

    cJSON* obj = cJSON_CreateObject();
    if (!obj) {
        return;
    }

    cJSON_AddStringToObject(obj, "lazybios_version", lazybiosVersion);

    if (DMIData->entry_tag == SMBIOS_VER_3X) {
        const lazybiosSMBIOS3Entry* v3 = DMIData->entry_union.v3;

        char anchor[6];
        snprintf(anchor, sizeof(anchor), "%c%c%c%c%c",
                 v3->anchor[0], v3->anchor[1], v3->anchor[2],
                 v3->anchor[3], v3->anchor[4]);

        char addr[20];
        snprintf(addr, sizeof(addr), "0x%lX", (unsigned long)v3->structure_table_address);

        cJSON_AddStringToObject(obj, "entry_point_version",      "3.x");
        cJSON_AddStringToObject(obj, "anchor",                   anchor);
        cJSON_AddNumberToObject(obj, "entry_point_length",       v3->entry_point_length);
        cJSON_AddNumberToObject(obj, "major_version",            v3->major_version);
        cJSON_AddNumberToObject(obj, "minor_version",            v3->minor_version);
        cJSON_AddNumberToObject(obj, "docrev",                   v3->docrev);
        cJSON_AddNumberToObject(obj, "entry_point_revision",     v3->entry_point_revision);
        cJSON_AddNumberToObject(obj, "structure_table_max_size", v3->structure_table_max_size);

        if (backend == LAZYBIOS_BACKEND_WINDOWS) {
            cJSON_AddNullToObject(obj, "structure_table_address");
        } else {
            cJSON_AddStringToObject(obj, "structure_table_address", addr);
        }

        cJSON_AddBoolToObject(obj, "is_64bit", cJSON_True);

    } else if (DMIData->entry_tag == SMBIOS_VER_2X) {
        const lazybiosSMBIOS2Entry* v2 = DMIData->entry_union.v2;

        char anchor[5];
        snprintf(anchor, sizeof(anchor), "%c%c%c%c",
                 v2->anchor[0], v2->anchor[1], v2->anchor[2], v2->anchor[3]);

        char inter_anchor[6];
        snprintf(inter_anchor, sizeof(inter_anchor), "%c%c%c%c%c",
                 v2->intermediate_anchor[0], v2->intermediate_anchor[1],
                 v2->intermediate_anchor[2], v2->intermediate_anchor[3],
                 v2->intermediate_anchor[4]);

        char bcd[8];
        snprintf(bcd, sizeof(bcd), "%u.%u",
                 (v2->bcd_revision >> 4) & 0x0F,
                  v2->bcd_revision       & 0x0F);

        char addr[20];
        snprintf(addr, sizeof(addr), "0x%lX", (unsigned long)v2->structure_table_address);

        cJSON_AddStringToObject(obj, "entry_point_version",    "2.x");
        cJSON_AddStringToObject(obj, "anchor",                 anchor);
        cJSON_AddNumberToObject(obj, "entry_point_length",     v2->entry_point_length);
        cJSON_AddNumberToObject(obj, "major_version",          v2->major_version);
        cJSON_AddNumberToObject(obj, "minor_version",          v2->minor_version);
        cJSON_AddNumberToObject(obj, "maximum_structure_size", v2->maximum_structure_size);
        cJSON_AddNumberToObject(obj, "entry_point_revision",   v2->entry_point_revision);
        cJSON_AddStringToObject(obj, "intermediate_anchor",    inter_anchor);
        cJSON_AddNumberToObject(obj, "structure_table_length", v2->structure_table_length);
        cJSON_AddNumberToObject(obj, "structure_count",        v2->structure_count);
        cJSON_AddStringToObject(obj, "bcd_revision",           bcd);

        if (backend == LAZYBIOS_BACKEND_WINDOWS) {
            cJSON_AddNullToObject(obj, "structure_table_address");
        } else {
            cJSON_AddStringToObject(obj, "structure_table_address", addr);
        }

        cJSON_AddBoolToObject(obj, "is_64bit", cJSON_False);

    } else {
        cJSON_AddStringToObject(obj, "entry_point_version",     "unknown");
        cJSON_AddNullToObject(obj,   "structure_table_address");
        cJSON_AddNullToObject(obj,   "is_64bit");
    }

    cJSON_AddItemToObject(root, "smbios_info", obj);
}

void lazybiosExtJSONAddType0(const lazybiosType0_t* type0, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type0 || !count) {
        cJSON_AddStringToObject(root, "Type0", "Failed to get BIOS information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType0_t* t = &type0[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_str(obj, "vendor",       t->vendor);
        cjson_add_str(obj, "version",      t->version);
        cjson_add_str(obj, "release_date", t->release_date);

        if (LAZYBIOS_FIELD_STATUS(t, bios_starting_segment) == LAZYBIOS_FIELD_PRESENT) {
            char buf[16];
            snprintf(buf, sizeof(buf), "0x%04hX", t->bios_starting_segment);
            cJSON_AddStringToObject(obj, "bios_starting_segment", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, bios_starting_segment) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "bios_starting_segment", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "bios_starting_segment");
        }

        if (LAZYBIOS_FIELD_STATUS(t, characteristics) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE * 2];
            lazybiosType0CharacteristicsStr(t->characteristics, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "characteristics", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, characteristics) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "characteristics", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "characteristics");
        }

        if (t->firmware_char_ext_bytes && t->firmware_char_ext_bytes_count >= 1) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType0CharacteristicsExtByte1Str(t->firmware_char_ext_bytes[0], buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "characteristics_ext_byte1", buf);
        } else {
            cjson_field_str(obj, "characteristics_ext_byte1",
                            LAZYBIOS_FIELD_STATUS(t, firmware_char_ext_bytes), NULL);
        }

        if (t->firmware_char_ext_bytes && t->firmware_char_ext_bytes_count >= 2) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType0CharacteristicsExtByte2Str(t->firmware_char_ext_bytes[1], buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "characteristics_ext_byte2", buf);
        } else {
            cjson_field_str(obj, "characteristics_ext_byte2",
                            LAZYBIOS_FIELD_STATUS(t, firmware_char_ext_bytes), NULL);
        }

        cjson_field_num(obj, "platform_major_release",
                        LAZYBIOS_FIELD_STATUS(t, platform_major_release),
                        t->platform_major_release);
        cjson_field_num(obj, "platform_minor_release",
                        LAZYBIOS_FIELD_STATUS(t, platform_minor_release),
                        t->platform_minor_release);
        cjson_field_num(obj, "ec_major_release",
                        LAZYBIOS_FIELD_STATUS(t, ec_major_release),
                        t->ec_major_release);
        cjson_field_num(obj, "ec_minor_release",
                        LAZYBIOS_FIELD_STATUS(t, ec_minor_release),
                        t->ec_minor_release);

        if (LAZYBIOS_FIELD_STATUS(t, extended_rom_size) == LAZYBIOS_FIELD_PRESENT) {
            char unit[5];
            char buf[64];
            uint16_t size = lazybiosType0ExtendedROMSizeU16(t->extended_rom_size, unit);
            snprintf(buf, sizeof(buf), "%hu %s", size, unit);
            cJSON_AddStringToObject(obj, "rom_size", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, rom_size) == LAZYBIOS_FIELD_PRESENT) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%u KB", t->rom_size);
            cJSON_AddStringToObject(obj, "rom_size", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, extended_rom_size) == LAZYBIOS_FIELD_UNREACHABLE &&
                   LAZYBIOS_FIELD_STATUS(t, rom_size) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "rom_size", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "rom_size");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type0", arr);
}

void lazybiosExtJSONAddType1(const lazybiosType1_t* type1, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type1 || !count) {
        cJSON_AddStringToObject(root, "Type1", "Failed to get System information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType1_t* t = &type1[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_str(obj, "manufacturer",  t->manufacturer);
        cjson_add_str(obj, "product_name",  t->product_name);
        cjson_add_str(obj, "version",       t->version);
        cjson_add_str(obj, "serial_number", t->serial_number);

        if (LAZYBIOS_FIELD_STATUS(t, uuid) == LAZYBIOS_FIELD_PRESENT) {
            char buf[37];
            snprintf(buf, sizeof(buf),
                     "%02X%02X%02X%02X-%02X%02X-%02X%02X-"
                     "%02X%02X-%02X%02X%02X%02X%02X%02X",
                     t->uuid[3], t->uuid[2], t->uuid[1], t->uuid[0],
                     t->uuid[5], t->uuid[4],
                     t->uuid[7], t->uuid[6],
                     t->uuid[8], t->uuid[9],
                     t->uuid[10], t->uuid[11], t->uuid[12],
                     t->uuid[13], t->uuid[14], t->uuid[15]);
            cJSON_AddStringToObject(obj, "uuid", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, uuid) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "uuid", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "uuid");
        }

        if (LAZYBIOS_FIELD_STATUS(t, wake_up_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "wake_up_type",
                                    lazybiosType1WakeupTypeStr(t->wake_up_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, wake_up_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "wake_up_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "wake_up_type");
        }

        cjson_field_str(obj, "sku_number", LAZYBIOS_FIELD_STATUS(t, sku_number), t->sku_number);
        cjson_field_str(obj, "family",     LAZYBIOS_FIELD_STATUS(t, family),     t->family);

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type1", arr);
}

void lazybiosExtJSONAddType2(const lazybiosType2_t* type2, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type2 || !count) {
        cJSON_AddStringToObject(root, "Type2", "Failed to get Baseboard information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType2_t* t = &type2[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_field_str(obj, "manufacturer",  LAZYBIOS_FIELD_STATUS(t, manufacturer),  t->manufacturer);
        cjson_field_str(obj, "product",       LAZYBIOS_FIELD_STATUS(t, product),       t->product);
        cjson_field_str(obj, "version",       LAZYBIOS_FIELD_STATUS(t, version),       t->version);
        cjson_field_str(obj, "serial_number", LAZYBIOS_FIELD_STATUS(t, serial_number), t->serial_number);
        cjson_field_str(obj, "asset_tag",     LAZYBIOS_FIELD_STATUS(t, asset_tag),     t->asset_tag);

        if (LAZYBIOS_FIELD_STATUS(t, feature_flags) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType2FeatureflagsStr(t->feature_flags, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "feature_flags", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, feature_flags) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "feature_flags", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "feature_flags");
        }

        cjson_field_str(obj, "location_in_chassis",
                        LAZYBIOS_FIELD_STATUS(t, location_in_chassis), t->location_in_chassis);

        if (LAZYBIOS_FIELD_STATUS(t, chassis_handle) == LAZYBIOS_FIELD_PRESENT) {
            char buf[10];
            snprintf(buf, sizeof(buf), "0x%04hX", t->chassis_handle);
            cJSON_AddStringToObject(obj, "chassis_handle", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, chassis_handle) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "chassis_handle", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "chassis_handle");
        }

        if (LAZYBIOS_FIELD_STATUS(t, board_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "board_type", lazybiosType2BoardTypeStr(t->board_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, board_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "board_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "board_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, contained_object_handles) == LAZYBIOS_FIELD_PRESENT &&
                t->contained_object_handles && t->number_of_contained_object_handles > 0) {
            cJSON* handles = cJSON_CreateArray();
            if (handles) {
                for (uint8_t h = 0; h < t->number_of_contained_object_handles; h++) {
                    cJSON_AddItemToArray(handles,
                        cJSON_CreateNumber(t->contained_object_handles[h]));
                }
                cJSON_AddItemToObject(obj, "contained_object_handles", handles);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, contained_object_handles) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "contained_object_handles", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "contained_object_handles");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type2", arr);
}

void lazybiosExtJSONAddType3(const lazybiosType3_t* type3, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type3 || !count) {
        cJSON_AddStringToObject(root, "Type3", "Failed to get Chassis information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType3_t* t = &type3[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_field_str(obj, "manufacturer", LAZYBIOS_FIELD_STATUS(t, manufacturer), t->manufacturer);

        if (LAZYBIOS_FIELD_STATUS(t, type) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType3TypeStr(t->type, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "type", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "type");
        }

        cjson_field_str(obj, "version",       LAZYBIOS_FIELD_STATUS(t, version),       t->version);
        cjson_field_str(obj, "serial_number", LAZYBIOS_FIELD_STATUS(t, serial_number), t->serial_number);
        cjson_field_str(obj, "asset_tag",     LAZYBIOS_FIELD_STATUS(t, asset_tag),     t->asset_tag);

        if (LAZYBIOS_FIELD_STATUS(t, boot_up_state) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "boot_up_state", lazybiosType3StateStr(t->boot_up_state));
        } else if (LAZYBIOS_FIELD_STATUS(t, boot_up_state) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "boot_up_state", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "boot_up_state");
        }

        if (LAZYBIOS_FIELD_STATUS(t, power_supply_state) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "power_supply_state",
                                    lazybiosType3StateStr(t->power_supply_state));
        } else if (LAZYBIOS_FIELD_STATUS(t, power_supply_state) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "power_supply_state", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "power_supply_state");
        }

        if (LAZYBIOS_FIELD_STATUS(t, thermal_state) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "thermal_state", lazybiosType3StateStr(t->thermal_state));
        } else if (LAZYBIOS_FIELD_STATUS(t, thermal_state) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "thermal_state", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "thermal_state");
        }

        if (LAZYBIOS_FIELD_STATUS(t, security_status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "security_status",
                                    lazybiosType3SecurityStatusStr(t->security_status));
        } else if (LAZYBIOS_FIELD_STATUS(t, security_status) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "security_status", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "security_status");
        }

        if (LAZYBIOS_FIELD_STATUS(t, oem_defined) == LAZYBIOS_FIELD_PRESENT) {
            char buf[12];
            snprintf(buf, sizeof(buf), "0x%08X", t->oem_defined);
            cJSON_AddStringToObject(obj, "oem_defined", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, oem_defined) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "oem_defined", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "oem_defined");
        }

        cjson_field_num(obj, "height", LAZYBIOS_FIELD_STATUS(t, height), t->height);
        cjson_field_num(obj, "number_of_power_cords",
                        LAZYBIOS_FIELD_STATUS(t, number_of_power_cords),
                        t->number_of_power_cords);

        if (LAZYBIOS_FIELD_STATUS(t, contained_elements) == LAZYBIOS_FIELD_PRESENT &&
                t->contained_elements && t->contained_element_count > 0 &&
                t->contained_element_record_length > 0) {
            cJSON* elements = cJSON_CreateArray();
            if (elements) {
                for (uint8_t e = 0; e < t->contained_element_count; e++) {
                    uint8_t rec_len    = t->contained_element_record_length;
                    const uint8_t* rec = &t->contained_elements[e * rec_len];
                    cJSON* elem = cJSON_CreateObject();
                    if (!elem) {
                        continue;
                    }
                    char type_buf[LAZYBIOS_DECODER_BUF_SIZE];
                    lazybiosType3ContainedElementTypeStr(rec[0], type_buf, sizeof(type_buf));
                    cJSON_AddStringToObject(elem, "type", type_buf);
                    if (rec_len >= 2) {
                        cJSON_AddNumberToObject(elem, "minimum", rec[1]);
                    }
                    if (rec_len >= 3) {
                        cJSON_AddNumberToObject(elem, "maximum", rec[2]);
                    }
                    cJSON_AddItemToArray(elements, elem);
                }
                cJSON_AddItemToObject(obj, "contained_elements", elements);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, contained_elements) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "contained_elements", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "contained_elements");
        }

        cjson_field_str(obj, "sku_number", LAZYBIOS_FIELD_STATUS(t, sku_number), t->sku_number);
        cjson_field_num(obj, "rack_type",   LAZYBIOS_FIELD_STATUS(t, rack_type),   t->rack_type);
        cjson_field_num(obj, "rack_height", LAZYBIOS_FIELD_STATUS(t, rack_height), t->rack_height);

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type3", arr);
}

void lazybiosExtJSONAddType4(const lazybiosType4_t* type4, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type4 || !count) {
        cJSON_AddStringToObject(root, "Type4", "Failed to get Processor information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType4_t* t = &type4[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_field_str(obj, "socket_designation",
                        LAZYBIOS_FIELD_STATUS(t, socket_designation), t->socket_designation);

        if (LAZYBIOS_FIELD_STATUS(t, processor_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "processor_type",
                                    lazybiosType4TypeStr(t->processor_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, processor_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "processor_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "processor_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, processor_family_2) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "processor_family",
                                    lazybiosType4ProcessorFamilyStr(t->processor_family_2));
        } else if (LAZYBIOS_FIELD_STATUS(t, processor_family) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "processor_family",
                                    lazybiosType4ProcessorFamilyStr(t->processor_family));
        } else if (LAZYBIOS_FIELD_STATUS(t, processor_family_2) == LAZYBIOS_FIELD_UNREACHABLE ||
                   LAZYBIOS_FIELD_STATUS(t, processor_family) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "processor_family", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "processor_family");
        }

        cjson_field_str(obj, "processor_manufacturer",
                        LAZYBIOS_FIELD_STATUS(t, processor_manufacturer),
                        t->processor_manufacturer);

        if (LAZYBIOS_FIELD_STATUS(t, processor_id) == LAZYBIOS_FIELD_PRESENT) {
            char buf[20];
            snprintf(buf, sizeof(buf), "0x%016llX", (unsigned long long)t->processor_id);
            cJSON_AddStringToObject(obj, "processor_id", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, processor_id) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "processor_id", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "processor_id");
        }

        cjson_field_str(obj, "processor_version",
                        LAZYBIOS_FIELD_STATUS(t, processor_version), t->processor_version);

        if (LAZYBIOS_FIELD_STATUS(t, voltage) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType4VoltageStr(t->voltage, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "voltage", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, voltage) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "voltage", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "voltage");
        }

        cjson_field_num(obj, "external_clock_mhz",
                        LAZYBIOS_FIELD_STATUS(t, external_clock), t->external_clock);
        cjson_field_num(obj, "max_speed_mhz",
                        LAZYBIOS_FIELD_STATUS(t, max_speed), t->max_speed);
        cjson_field_num(obj, "current_speed_mhz",
                        LAZYBIOS_FIELD_STATUS(t, current_speed), t->current_speed);

        if (LAZYBIOS_FIELD_STATUS(t, status) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType4StatusStr(t->status, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "status", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, status) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "status", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "status");
        }

        if (LAZYBIOS_FIELD_STATUS(t, processor_upgrade) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "processor_upgrade",
                                    lazybiosType4SocketTypeStr(t->processor_upgrade));
        } else if (LAZYBIOS_FIELD_STATUS(t, processor_upgrade) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "processor_upgrade", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "processor_upgrade");
        }

        if (LAZYBIOS_FIELD_STATUS(t, l1_cache_handle) == LAZYBIOS_FIELD_PRESENT) {
            if (t->l1_cache_handle == 0xFFFF) {
                cJSON_AddNullToObject(obj, "l1_cache_handle");
            } else {
                char buf[10];
                snprintf(buf, sizeof(buf), "0x%04hX", t->l1_cache_handle);
                cJSON_AddStringToObject(obj, "l1_cache_handle", buf);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, l1_cache_handle) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "l1_cache_handle", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "l1_cache_handle");
        }

        if (LAZYBIOS_FIELD_STATUS(t, l2_cache_handle) == LAZYBIOS_FIELD_PRESENT) {
            if (t->l2_cache_handle == 0xFFFF) {
                cJSON_AddNullToObject(obj, "l2_cache_handle");
            } else {
                char buf[10];
                snprintf(buf, sizeof(buf), "0x%04hX", t->l2_cache_handle);
                cJSON_AddStringToObject(obj, "l2_cache_handle", buf);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, l2_cache_handle) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "l2_cache_handle", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "l2_cache_handle");
        }

        if (LAZYBIOS_FIELD_STATUS(t, l3_cache_handle) == LAZYBIOS_FIELD_PRESENT) {
            if (t->l3_cache_handle == 0xFFFF) {
                cJSON_AddNullToObject(obj, "l3_cache_handle");
            } else {
                char buf[10];
                snprintf(buf, sizeof(buf), "0x%04hX", t->l3_cache_handle);
                cJSON_AddStringToObject(obj, "l3_cache_handle", buf);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, l3_cache_handle) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "l3_cache_handle", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "l3_cache_handle");
        }

        cjson_field_str(obj, "serial_number", LAZYBIOS_FIELD_STATUS(t, serial_number), t->serial_number);
        cjson_field_str(obj, "asset_tag",     LAZYBIOS_FIELD_STATUS(t, asset_tag),     t->asset_tag);
        cjson_field_str(obj, "part_number",   LAZYBIOS_FIELD_STATUS(t, part_number),   t->part_number);

        if (LAZYBIOS_FIELD_STATUS(t, core_count_2) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "core_count", t->core_count_2);
        } else if (LAZYBIOS_FIELD_STATUS(t, core_count) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "core_count", t->core_count);
        } else if (LAZYBIOS_FIELD_STATUS(t, core_count_2) == LAZYBIOS_FIELD_UNREACHABLE ||
                   LAZYBIOS_FIELD_STATUS(t, core_count) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "core_count", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "core_count");
        }

        if (LAZYBIOS_FIELD_STATUS(t, core_enabled_2) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "core_enabled", t->core_enabled_2);
        } else if (LAZYBIOS_FIELD_STATUS(t, core_enabled) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "core_enabled", t->core_enabled);
        } else if (LAZYBIOS_FIELD_STATUS(t, core_enabled_2) == LAZYBIOS_FIELD_UNREACHABLE ||
                   LAZYBIOS_FIELD_STATUS(t, core_enabled) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "core_enabled", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "core_enabled");
        }

        if (LAZYBIOS_FIELD_STATUS(t, thread_count_2) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "thread_count", t->thread_count_2);
        } else if (LAZYBIOS_FIELD_STATUS(t, thread_count) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "thread_count", t->thread_count);
        } else if (LAZYBIOS_FIELD_STATUS(t, thread_count_2) == LAZYBIOS_FIELD_UNREACHABLE ||
                   LAZYBIOS_FIELD_STATUS(t, thread_count) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "thread_count", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "thread_count");
        }

        cjson_field_num(obj, "thread_enabled",
                        LAZYBIOS_FIELD_STATUS(t, thread_enabled), t->thread_enabled);

        if (LAZYBIOS_FIELD_STATUS(t, processor_characteristics) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType4CharacteristicsStr(t->processor_characteristics, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "processor_characteristics", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, processor_characteristics) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "processor_characteristics", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "processor_characteristics");
        }

        cjson_field_str(obj, "socket_type", LAZYBIOS_FIELD_STATUS(t, socket_type), t->socket_type);

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type4", arr);
}

void lazybiosExtJSONAddType5(const lazybiosType5_t* type5, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type5 || !count) {
        cJSON_AddStringToObject(root, "Type5", "Failed to get Memory Controller information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType5_t* t = &type5[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        if (LAZYBIOS_FIELD_STATUS(t, error_detecting_method) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "error_detecting_method",
                                    lazybiosType5ErrorDetectingMethodStr(t->error_detecting_method));
        } else if (LAZYBIOS_FIELD_STATUS(t, error_detecting_method) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "error_detecting_method", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "error_detecting_method");
        }

        if (LAZYBIOS_FIELD_STATUS(t, error_correcting_capability) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType5ErrorCorrectingCapabilityStr(t->error_correcting_capability, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "error_correcting_capability", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, error_correcting_capability) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "error_correcting_capability", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "error_correcting_capability");
        }

        if (LAZYBIOS_FIELD_STATUS(t, supported_interleave) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "supported_interleave",
                                    lazybiosType5InterleaveStr(t->supported_interleave));
        } else if (LAZYBIOS_FIELD_STATUS(t, supported_interleave) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "supported_interleave", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "supported_interleave");
        }

        if (LAZYBIOS_FIELD_STATUS(t, current_interleave) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "current_interleave",
                                    lazybiosType5InterleaveStr(t->current_interleave));
        } else if (LAZYBIOS_FIELD_STATUS(t, current_interleave) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "current_interleave", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "current_interleave");
        }

        if (LAZYBIOS_FIELD_STATUS(t, maximum_memory_module_size) == LAZYBIOS_FIELD_PRESENT) {
            char buf[32];
            if (t->maximum_memory_module_size < 64) {
                snprintf(buf, sizeof(buf), "%llu MiB",
                    (unsigned long long)(1ULL << t->maximum_memory_module_size));
            } else {
                snprintf(buf, sizeof(buf), "2^%hhu MiB", t->maximum_memory_module_size);
            }
            cJSON_AddStringToObject(obj, "maximum_memory_module_size", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, maximum_memory_module_size) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "maximum_memory_module_size", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "maximum_memory_module_size");
        }

        if (LAZYBIOS_FIELD_STATUS(t, supported_speeds) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType5SupportedSpeedsStr(t->supported_speeds, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "supported_speeds", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, supported_speeds) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "supported_speeds", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "supported_speeds");
        }

        if (LAZYBIOS_FIELD_STATUS(t, supported_memory_types) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType5SupportedMemoryTypesStr(t->supported_memory_types, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "supported_memory_types", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, supported_memory_types) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "supported_memory_types", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "supported_memory_types");
        }

        if (LAZYBIOS_FIELD_STATUS(t, memory_module_voltage) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType5MemoryModuleVoltageStr(t->memory_module_voltage, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "memory_module_voltage", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, memory_module_voltage) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "memory_module_voltage", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "memory_module_voltage");
        }

        cjson_field_num(obj, "number_of_associated_memory_slots",
                        LAZYBIOS_FIELD_STATUS(t, number_of_associated_memory_slots),
                        t->number_of_associated_memory_slots);

        if (LAZYBIOS_FIELD_STATUS(t, memory_module_configuration_handles) == LAZYBIOS_FIELD_PRESENT &&
                t->memory_module_configuration_handles && t->number_of_associated_memory_slots > 0) {
            cJSON* handles = cJSON_CreateArray();
            if (handles) {
                for (size_t j = 0; j < t->number_of_associated_memory_slots; j++) {
                    cJSON_AddItemToArray(handles,
                        cJSON_CreateNumber(t->memory_module_configuration_handles[j]));
                }
                cJSON_AddItemToObject(obj, "memory_module_configuration_handles", handles);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, memory_module_configuration_handles) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "memory_module_configuration_handles", LAZYBIOS_JSON_UNREACHABLE);
        } else if (LAZYBIOS_FIELD_STATUS(t, memory_module_configuration_handles) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "memory_module_configuration_handles", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "memory_module_configuration_handles");
        }

        if (LAZYBIOS_FIELD_STATUS(t, enabled_error_correcting_capabilities) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType5ErrorCorrectingCapabilityStr(t->enabled_error_correcting_capabilities, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "enabled_error_correcting_capabilities", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, enabled_error_correcting_capabilities) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "enabled_error_correcting_capabilities", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "enabled_error_correcting_capabilities");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type5", arr);
}

void lazybiosExtJSONAddType6(const lazybiosType6_t* type6, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type6 || !count) {
        cJSON_AddStringToObject(root, "Type6", "Failed to get Memory Module information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType6_t* t = &type6[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_field_str(obj, "socket_designation",
                        LAZYBIOS_FIELD_STATUS(t, socket_designation), t->socket_designation);

        if (LAZYBIOS_FIELD_STATUS(t, bank_connections) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType6BankConnectionsStr(t->bank_connections, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "bank_connections", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, bank_connections) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "bank_connections", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "bank_connections");
        }

        if (LAZYBIOS_FIELD_STATUS(t, current_speed) == LAZYBIOS_FIELD_PRESENT) {
            if (t->current_speed == 0) {
                cJSON_AddStringToObject(obj, "current_speed", "Unknown");
            } else {
                char buf[24];
                snprintf(buf, sizeof(buf), "%hhu ns", t->current_speed);
                cJSON_AddStringToObject(obj, "current_speed", buf);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, current_speed) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "current_speed", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "current_speed");
        }

        if (LAZYBIOS_FIELD_STATUS(t, current_memory_type) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType6CurrentMemoryTypeStr(t->current_memory_type, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "current_memory_type", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, current_memory_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "current_memory_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "current_memory_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, installed_size) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType6InstalledSizeStr(t->installed_size, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "installed_size", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, installed_size) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "installed_size", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "installed_size");
        }

        if (LAZYBIOS_FIELD_STATUS(t, enabled_size) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType6EnabledSizeStr(t->enabled_size, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "enabled_size", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, enabled_size) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "enabled_size", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "enabled_size");
        }

        if (LAZYBIOS_FIELD_STATUS(t, error_status) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType6ErrorStatusStr(t->error_status, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "error_status", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, error_status) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "error_status", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "error_status");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type6", arr);
}

void lazybiosExtJSONAddType7(const lazybiosType7_t* type7, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type7 || !count) {
        cJSON_AddStringToObject(root, "Type7", "Failed to get Cache information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType7_t* t = &type7[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_field_str(obj, "socket_designation",
                        LAZYBIOS_FIELD_STATUS(t, socket_designation), t->socket_designation);

        if (LAZYBIOS_FIELD_STATUS(t, cache_configuration) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType7CacheConfigurationStr(t->cache_configuration, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "cache_configuration", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, cache_configuration) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "cache_configuration", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "cache_configuration");
        }

        if (LAZYBIOS_FIELD_STATUS(t, maximum_cache_size_2) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t size = lazybiosType7CacheU32(t->maximum_cache_size_2);
            cJSON_AddNumberToObject(obj, "maximum_cache_size_kb", (double)size);
        } else if (LAZYBIOS_FIELD_STATUS(t, maximum_cache_size) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t size = lazybiosType7CacheU16(t->maximum_cache_size);
            cJSON_AddNumberToObject(obj, "maximum_cache_size_kb", (double)size);
        } else if (LAZYBIOS_FIELD_STATUS(t, maximum_cache_size_2) == LAZYBIOS_FIELD_UNREACHABLE &&
                   LAZYBIOS_FIELD_STATUS(t, maximum_cache_size) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "maximum_cache_size_kb", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "maximum_cache_size_kb");
        }

        if (LAZYBIOS_FIELD_STATUS(t, installed_cache_size_2) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t size = lazybiosType7CacheU32(t->installed_cache_size_2);
            cJSON_AddNumberToObject(obj, "installed_cache_size_kb", (double)size);
        } else if (LAZYBIOS_FIELD_STATUS(t, installed_size) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t size = lazybiosType7CacheU16(t->installed_size);
            cJSON_AddNumberToObject(obj, "installed_cache_size_kb", (double)size);
        } else if (LAZYBIOS_FIELD_STATUS(t, installed_cache_size_2) == LAZYBIOS_FIELD_UNREACHABLE &&
                   LAZYBIOS_FIELD_STATUS(t, installed_size) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "installed_cache_size_kb", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "installed_cache_size_kb");
        }

        if (LAZYBIOS_FIELD_STATUS(t, supported_sram_type) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType7SRAMTypeStr(t->supported_sram_type, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "supported_sram_type", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, supported_sram_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "supported_sram_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "supported_sram_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, current_sram_type) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType7SRAMTypeStr(t->current_sram_type, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "current_sram_type", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, current_sram_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "current_sram_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "current_sram_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, cache_speed) == LAZYBIOS_FIELD_PRESENT) {
            if (t->cache_speed == 0) {
                cJSON_AddStringToObject(obj, "cache_speed", "Unknown");
            } else {
                char buf[24];
                snprintf(buf, sizeof(buf), "%hhu ns", t->cache_speed);
                cJSON_AddStringToObject(obj, "cache_speed", buf);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, cache_speed) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "cache_speed", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "cache_speed");
        }

        if (LAZYBIOS_FIELD_STATUS(t, error_correction_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "error_correction_type",
                                    lazybiosType7ErrorCorrectionTypeStr(t->error_correction_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, error_correction_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "error_correction_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "error_correction_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, system_cache_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "system_cache_type",
                                    lazybiosType7SystemCacheTypeStr(t->system_cache_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, system_cache_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "system_cache_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "system_cache_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, associativity) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "associativity",
                                    lazybiosType7AssociativityStr(t->associativity));
        } else if (LAZYBIOS_FIELD_STATUS(t, associativity) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "associativity", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "associativity");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type7", arr);
}

void lazybiosExtJSONAddType8(const lazybiosType8_t* type8, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type8 || !count) {
        cJSON_AddStringToObject(root, "Type8", "Failed to get Port Connector information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType8_t* t = &type8[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_field_str(obj, "internal_reference_designator",
                        LAZYBIOS_FIELD_STATUS(t, internal_reference_designator),
                        t->internal_reference_designator);

        if (LAZYBIOS_FIELD_STATUS(t, internal_connector_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "internal_connector_type",
                                    lazybiosType8ConnectorTypeStr(t->internal_connector_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, internal_connector_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "internal_connector_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "internal_connector_type");
        }

        cjson_field_str(obj, "external_reference_designator",
                        LAZYBIOS_FIELD_STATUS(t, external_reference_designator),
                        t->external_reference_designator);

        if (LAZYBIOS_FIELD_STATUS(t, external_connector_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "external_connector_type",
                                    lazybiosType8ConnectorTypeStr(t->external_connector_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, external_connector_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "external_connector_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "external_connector_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, port_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "port_type",
                                    lazybiosType8PortTypeStr(t->port_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, port_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "port_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "port_type");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type8", arr);
}

void lazybiosExtJSONAddType9(const lazybiosType9_t* type9, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type9 || !count) {
        cJSON_AddStringToObject(root, "Type9", "Failed to get System Slots information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType9_t* t = &type9[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_field_str(obj, "slot_designation",
                        LAZYBIOS_FIELD_STATUS(t, slot_designation), t->slot_designation);

        if (LAZYBIOS_FIELD_STATUS(t, slot_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "slot_type",
                                    lazybiosType9SlotTypeStr(t->slot_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, slot_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "slot_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "slot_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, slot_data_bus_width) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "slot_data_bus_width",
                                    lazybiosType9SlotWidthStr(t->slot_data_bus_width));
        } else if (LAZYBIOS_FIELD_STATUS(t, slot_data_bus_width) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "slot_data_bus_width", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "slot_data_bus_width");
        }

        if (LAZYBIOS_FIELD_STATUS(t, current_usage) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "current_usage",
                                    lazybiosType9CurrentUsageStr(t->current_usage));
        } else if (LAZYBIOS_FIELD_STATUS(t, current_usage) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "current_usage", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "current_usage");
        }

        if (LAZYBIOS_FIELD_STATUS(t, slot_length) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "slot_length",
                                    lazybiosType9SlotLengthStr(t->slot_length));
        } else if (LAZYBIOS_FIELD_STATUS(t, slot_length) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "slot_length", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "slot_length");
        }

        cjson_field_num(obj, "slot_id", LAZYBIOS_FIELD_STATUS(t, slot_id), t->slot_id);

        if (LAZYBIOS_FIELD_STATUS(t, slot_characteristics_1) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType9Characteristics1Str(t->slot_characteristics_1, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "slot_characteristics_1", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, slot_characteristics_1) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "slot_characteristics_1", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "slot_characteristics_1");
        }

        if (LAZYBIOS_FIELD_STATUS(t, slot_characteristics_2) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType9Characteristics2Str(t->slot_characteristics_2, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "slot_characteristics_2", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, slot_characteristics_2) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "slot_characteristics_2", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "slot_characteristics_2");
        }

        cjson_field_num(obj, "segment_group_number",
                        LAZYBIOS_FIELD_STATUS(t, segment_group_number), t->segment_group_number);
        cjson_field_num(obj, "bus_number",
                        LAZYBIOS_FIELD_STATUS(t, bus_number), t->bus_number);

        if (LAZYBIOS_FIELD_STATUS(t, device_function_number) == LAZYBIOS_FIELD_PRESENT) {
            char buf[16];
            lazybiosType9DeviceFunctionStr(t->device_function_number, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "device_function", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, device_function_number) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "device_function", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "device_function");
        }

        cjson_field_num(obj, "data_bus_width",
                        LAZYBIOS_FIELD_STATUS(t, data_bus_width), t->data_bus_width);

        if (LAZYBIOS_FIELD_STATUS(t, peer_groups) == LAZYBIOS_FIELD_PRESENT) {
            if (t->peer_groups && t->peer_grouping_count > 0) {
                cJSON* groups = cJSON_CreateArray();
                for (uint8_t j = 0; j < t->peer_grouping_count; j++) {
                    cJSON* g = cJSON_CreateObject();
                    cjson_field_num(g, "segment_group_number",
                                    LAZYBIOS_FIELD_STATUS(&t->peer_groups[j], segment_group_number),
                                    t->peer_groups[j].segment_group_number);
                    cjson_field_num(g, "bus_number",
                                    LAZYBIOS_FIELD_STATUS(&t->peer_groups[j], bus_number),
                                    t->peer_groups[j].bus_number);
                    if (LAZYBIOS_FIELD_STATUS(&t->peer_groups[j], device_function_number) == LAZYBIOS_FIELD_PRESENT) {
                        char buf[16];
                        lazybiosType9DeviceFunctionStr(t->peer_groups[j].device_function_number, buf, sizeof(buf));
                        cJSON_AddStringToObject(g, "device_function", buf);
                    } else {
                        cjson_field_str(g, "device_function",
                                        LAZYBIOS_FIELD_STATUS(&t->peer_groups[j], device_function_number), NULL);
                    }
                    cjson_field_num(g, "data_bus_width",
                                    LAZYBIOS_FIELD_STATUS(&t->peer_groups[j], data_bus_width),
                                    t->peer_groups[j].data_bus_width);
                    cJSON_AddItemToArray(groups, g);
                }
                cJSON_AddItemToObject(obj, "peer_groups", groups);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, peer_groups) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "peer_groups", LAZYBIOS_JSON_UNREACHABLE);
        } else if (LAZYBIOS_FIELD_STATUS(t, peer_groups) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "peer_groups", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "peer_groups");
        }

        if (LAZYBIOS_FIELD_STATUS(t, slot_physical_width) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "slot_physical_width",
                                    lazybiosType9SlotWidthStr(t->slot_physical_width));
        } else if (LAZYBIOS_FIELD_STATUS(t, slot_physical_width) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "slot_physical_width", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "slot_physical_width");
        }

        if (LAZYBIOS_FIELD_STATUS(t, slot_pitch) == LAZYBIOS_FIELD_PRESENT) {
            if (t->slot_pitch == 0) {
                cJSON_AddStringToObject(obj, "slot_pitch", "Unknown");
            } else {
                char buf[24];
                snprintf(buf, sizeof(buf), "%u.%02u mm", t->slot_pitch / 100, t->slot_pitch % 100);
                cJSON_AddStringToObject(obj, "slot_pitch", buf);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, slot_pitch) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "slot_pitch", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "slot_pitch");
        }

        if (LAZYBIOS_FIELD_STATUS(t, slot_height) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "slot_height",
                                    lazybiosType9SlotHeightStr(t->slot_height));
        } else if (LAZYBIOS_FIELD_STATUS(t, slot_height) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "slot_height", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "slot_height");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type9", arr);
}
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

void lazybiosExtJSONAddType10(const lazybiosType10_t* type10, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type10 || !count) {
        cJSON_AddStringToObject(root, "Type10", "Failed to get On Board Devices information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType10_t* t = &type10[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        if (LAZYBIOS_FIELD_STATUS(t, devices) == LAZYBIOS_FIELD_PRESENT &&
            t->devices && t->device_count > 0) {
            cJSON* devices = cJSON_CreateArray();
            if (devices) {
                for (size_t j = 0; j < t->device_count; j++) {
                    cJSON* dev = cJSON_CreateObject();
                    if (!dev) {
                        continue;
                    }

                    if (LAZYBIOS_FIELD_STATUS(&t->devices[j], device_type_and_status) == LAZYBIOS_FIELD_PRESENT) {
                        cJSON_AddStringToObject(dev, "type", lazybiosType10DeviceTypeStr(t->devices[j].device_type_and_status));
                        cJSON_AddStringToObject(dev, "status", lazybiosType10DeviceStatusStr(t->devices[j].device_type_and_status));
                    } else if (LAZYBIOS_FIELD_STATUS(&t->devices[j], device_type_and_status) == LAZYBIOS_FIELD_UNREACHABLE) {
                        cJSON_AddStringToObject(dev, "type", LAZYBIOS_JSON_UNREACHABLE);
                        cJSON_AddStringToObject(dev, "status", LAZYBIOS_JSON_UNREACHABLE);
                    } else {
                        cJSON_AddNullToObject(dev, "type");
                        cJSON_AddNullToObject(dev, "status");
                    }

                    cjson_add_str(dev, "description", t->devices[j].description);
                    cJSON_AddItemToArray(devices, dev);
                }
                cJSON_AddItemToObject(obj, "devices", devices);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, devices) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "devices", LAZYBIOS_JSON_UNREACHABLE);
        } else if (LAZYBIOS_FIELD_STATUS(t, devices) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "devices", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "devices");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type10", arr);
}

void lazybiosExtJSONAddType11(const lazybiosType11_t* type11, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type11 || !count) {
        cJSON_AddStringToObject(root, "Type11", "Failed to get OEM Strings information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType11_t* t = &type11[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_field_num(obj, "string_count", LAZYBIOS_FIELD_STATUS(t, string_count), t->string_count);

        if (LAZYBIOS_FIELD_STATUS(t, strings) == LAZYBIOS_FIELD_PRESENT &&
            t->strings && t->string_count > 0) {
            cJSON* strings = cJSON_CreateArray();
            if (strings) {
                for (size_t j = 0; j < t->string_count; j++) {
                    cJSON_AddItemToArray(strings, cJSON_CreateString(t->strings[j] ? t->strings[j] : ""));
                }
                cJSON_AddItemToObject(obj, "strings", strings);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, strings) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "strings", LAZYBIOS_JSON_UNREACHABLE);
        } else if (LAZYBIOS_FIELD_STATUS(t, strings) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "strings", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "strings");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type11", arr);
}

void lazybiosExtJSONAddType12(const lazybiosType12_t* type12, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type12 || !count) {
        cJSON_AddStringToObject(root, "Type12", "Failed to get System Configuration Options information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType12_t* t = &type12[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_field_num(obj, "option_count", LAZYBIOS_FIELD_STATUS(t, option_count), t->option_count);

        if (LAZYBIOS_FIELD_STATUS(t, options) == LAZYBIOS_FIELD_PRESENT &&
            t->options && t->option_count > 0) {
            cJSON* options = cJSON_CreateArray();
            if (options) {
                for (size_t j = 0; j < t->option_count; j++) {
                    cJSON_AddItemToArray(options, cJSON_CreateString(t->options[j] ? t->options[j] : ""));
                }
                cJSON_AddItemToObject(obj, "options", options);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, options) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "options", LAZYBIOS_JSON_UNREACHABLE);
        } else if (LAZYBIOS_FIELD_STATUS(t, options) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "options", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "options");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type12", arr);
}

void lazybiosExtJSONAddType13(const lazybiosType13_t* type13, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type13 || !count) {
        cJSON_AddStringToObject(root, "Type13", "Failed to get Firmware Language information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType13_t* t = &type13[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        if (LAZYBIOS_FIELD_STATUS(t, flags) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "language_description_format", lazybiosType13LanguageFormatStr(t->flags));
        } else if (LAZYBIOS_FIELD_STATUS(t, flags) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "language_description_format", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "language_description_format");
        }

        cjson_field_num(obj, "installable_languages", LAZYBIOS_FIELD_STATUS(t, installable_languages), t->installable_languages);

        if (LAZYBIOS_FIELD_STATUS(t, languages) == LAZYBIOS_FIELD_PRESENT &&
            t->languages && t->installable_languages > 0) {
            cJSON* languages = cJSON_CreateArray();
            if (languages) {
                for (size_t j = 0; j < t->installable_languages; j++) {
                    cJSON_AddItemToArray(languages, cJSON_CreateString(t->languages[j] ? t->languages[j] : ""));
                }
                cJSON_AddItemToObject(obj, "languages", languages);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, languages) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "languages", LAZYBIOS_JSON_UNREACHABLE);
        } else if (LAZYBIOS_FIELD_STATUS(t, languages) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "languages", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "languages");
        }

        cjson_add_str(obj, "current_language", t->current_language);

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type13", arr);
}

void lazybiosExtJSONAddType14(const lazybiosType14_t* type14, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type14 || !count) {
        cJSON_AddStringToObject(root, "Type14", "Failed to get Group Associations information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType14_t* t = &type14[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_str(obj, "group_name", t->group_name);
        cjson_field_num(obj, "item_count", LAZYBIOS_FIELD_STATUS(t, item_count), t->item_count);

        if (LAZYBIOS_FIELD_STATUS(t, items) == LAZYBIOS_FIELD_PRESENT &&
            t->items && t->item_count > 0) {
            cJSON* items = cJSON_CreateArray();
            if (items) {
                for (size_t j = 0; j < t->item_count; j++) {
                    cJSON* item = cJSON_CreateObject();
                    if (!item) {
                        continue;
                    }

                    cjson_field_num(item, "item_type", LAZYBIOS_FIELD_STATUS(&t->items[j], item_type), t->items[j].item_type);
                    cjson_field_num(item, "item_handle", LAZYBIOS_FIELD_STATUS(&t->items[j], item_handle), t->items[j].item_handle);

                    cJSON_AddItemToArray(items, item);
                }
                cJSON_AddItemToObject(obj, "items", items);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, items) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "items", LAZYBIOS_JSON_UNREACHABLE);
        } else if (LAZYBIOS_FIELD_STATUS(t, items) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "items", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "items");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type14", arr);
}

void lazybiosExtJSONAddType15(const lazybiosType15_t* type15, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type15 || !count) {
        cJSON_AddStringToObject(root, "Type15", "Failed to get System Event Log information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType15_t* t = &type15[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_field_num(obj, "log_area_length", LAZYBIOS_FIELD_STATUS(t, log_area_length), t->log_area_length);
        cjson_field_num(obj, "log_header_start_offset", LAZYBIOS_FIELD_STATUS(t, log_header_start_offset), t->log_header_start_offset);
        cjson_field_num(obj, "log_data_start_offset", LAZYBIOS_FIELD_STATUS(t, log_data_start_offset), t->log_data_start_offset);

        if (LAZYBIOS_FIELD_STATUS(t, access_method) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "access_method", lazybiosType15AccessMethodStr(t->access_method));
        } else if (LAZYBIOS_FIELD_STATUS(t, access_method) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "access_method", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "access_method");
        }

        if (LAZYBIOS_FIELD_STATUS(t, log_status) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType15LogStatusStr(t->log_status, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "log_status", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, log_status) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "log_status", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "log_status");
        }

        if (LAZYBIOS_FIELD_STATUS(t, log_change_token) == LAZYBIOS_FIELD_PRESENT) {
            if (t->log_change_token == 0) {
                cJSON_AddStringToObject(obj, "log_change_token", "Not Implemented");
            } else {
                cJSON_AddNumberToObject(obj, "log_change_token", t->log_change_token);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, log_change_token) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "log_change_token", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "log_change_token");
        }

        if (LAZYBIOS_FIELD_STATUS(t, access_method_address) == LAZYBIOS_FIELD_PRESENT) {
            cJSON* addr_obj = cJSON_CreateObject();
            if (addr_obj) {
                if (LAZYBIOS_FIELD_STATUS(t, access_method) == LAZYBIOS_FIELD_PRESENT && t->access_method <= 0x02) {
                    cJSON_AddNumberToObject(addr_obj, "index_address", lazybiosType15IndexAddress(t->access_method_address));
                    cJSON_AddNumberToObject(addr_obj, "data_address", lazybiosType15DataAddress(t->access_method_address));
                } else if (LAZYBIOS_FIELD_STATUS(t, access_method) == LAZYBIOS_FIELD_PRESENT && t->access_method == 0x03) {
                    cJSON_AddNumberToObject(addr_obj, "physical_address", t->access_method_address);
                } else if (LAZYBIOS_FIELD_STATUS(t, access_method) == LAZYBIOS_FIELD_PRESENT && t->access_method == 0x04) {
                    cJSON_AddNumberToObject(addr_obj, "gpnv_handle", lazybiosType15GPNVHandle(t->access_method_address));
                } else {
                    cJSON_AddNumberToObject(addr_obj, "address", t->access_method_address);
                }
                cJSON_AddItemToObject(obj, "access_method_address", addr_obj);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, access_method_address) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "access_method_address", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "access_method_address");
        }

        if (LAZYBIOS_FIELD_STATUS(t, log_header_format) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "log_header_format", lazybiosType15LogHeaderFormatStr(t->log_header_format));
        } else if (LAZYBIOS_FIELD_STATUS(t, log_header_format) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "log_header_format", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "log_header_format");
        }

        cjson_field_num(obj, "supported_log_type_descriptor_count", LAZYBIOS_FIELD_STATUS(t, number_of_supported_log_type_descriptors), t->number_of_supported_log_type_descriptors);
        cjson_field_num(obj, "log_type_descriptor_length", LAZYBIOS_FIELD_STATUS(t, length_of_each_log_type_descriptor), t->length_of_each_log_type_descriptor);

        if (LAZYBIOS_FIELD_STATUS(t, supported_log_type_descriptors) == LAZYBIOS_FIELD_PRESENT &&
            t->supported_log_type_descriptors && t->number_of_supported_log_type_descriptors > 0) {
            cJSON* descriptors = cJSON_CreateArray();
            if (descriptors) {
                for (size_t j = 0; j < t->number_of_supported_log_type_descriptors; j++) {
                    cJSON* desc = cJSON_CreateObject();
                    if (!desc) {
                        continue;
                    }

                    if (LAZYBIOS_FIELD_STATUS(&t->supported_log_type_descriptors[j], log_type) == LAZYBIOS_FIELD_PRESENT) {
                        cJSON_AddStringToObject(desc, "log_type", lazybiosType15LogTypeStr(t->supported_log_type_descriptors[j].log_type));
                    } else if (LAZYBIOS_FIELD_STATUS(&t->supported_log_type_descriptors[j], log_type) == LAZYBIOS_FIELD_UNREACHABLE) {
                        cJSON_AddStringToObject(desc, "log_type", LAZYBIOS_JSON_UNREACHABLE);
                    } else {
                        cJSON_AddNullToObject(desc, "log_type");
                    }

                    if (LAZYBIOS_FIELD_STATUS(&t->supported_log_type_descriptors[j], variable_data_format_type) == LAZYBIOS_FIELD_PRESENT) {
                        cJSON_AddStringToObject(desc, "variable_data_format", lazybiosType15VariableDataFormatTypeStr(t->supported_log_type_descriptors[j].variable_data_format_type));
                    } else if (LAZYBIOS_FIELD_STATUS(&t->supported_log_type_descriptors[j], variable_data_format_type) == LAZYBIOS_FIELD_UNREACHABLE) {
                        cJSON_AddStringToObject(desc, "variable_data_format", LAZYBIOS_JSON_UNREACHABLE);
                    } else {
                        cJSON_AddNullToObject(desc, "variable_data_format");
                    }

                    cJSON_AddItemToArray(descriptors, desc);
                }
                cJSON_AddItemToObject(obj, "supported_log_type_descriptors", descriptors);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, supported_log_type_descriptors) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "supported_log_type_descriptors", LAZYBIOS_JSON_UNREACHABLE);
        } else if (LAZYBIOS_FIELD_STATUS(t, supported_log_type_descriptors) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "supported_log_type_descriptors", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "supported_log_type_descriptors");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type15", arr);
}

void lazybiosExtJSONAddType16(const lazybiosType16_t* type16, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type16 || !count) {
        cJSON_AddStringToObject(root, "Type16", "Failed to get Physical Memory Array information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType16_t* t = &type16[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        if (LAZYBIOS_FIELD_STATUS(t, location) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "location", lazybiosType16LocationStr(t->location));
        } else if (LAZYBIOS_FIELD_STATUS(t, location) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "location", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "location");
        }

        if (LAZYBIOS_FIELD_STATUS(t, use) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "use", lazybiosType16UseStr(t->use));
        } else if (LAZYBIOS_FIELD_STATUS(t, use) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "use", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "use");
        }

        if (LAZYBIOS_FIELD_STATUS(t, memory_error_correction) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "memory_error_correction", lazybiosType16MemoryErrorCorrectionStr(t->memory_error_correction));
        } else if (LAZYBIOS_FIELD_STATUS(t, memory_error_correction) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "memory_error_correction", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "memory_error_correction");
        }

        if (LAZYBIOS_FIELD_STATUS(t, maximum_capacity) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t capacity = lazybiosType16MaximumCapacityBytes(t->maximum_capacity, t->extended_maximum_capacity);
            cJSON_AddNumberToObject(obj, "maximum_capacity_bytes", (double)capacity);
        } else if (LAZYBIOS_FIELD_STATUS(t, maximum_capacity) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "maximum_capacity_bytes", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "maximum_capacity_bytes");
        }

        if (LAZYBIOS_FIELD_STATUS(t, memory_error_information_handle) == LAZYBIOS_FIELD_PRESENT) {
            if (t->memory_error_information_handle == 0xFFFE) {
                cJSON_AddStringToObject(obj, "memory_error_information_handle", "Not Provided");
            } else if (t->memory_error_information_handle == 0xFFFF) {
                cJSON_AddStringToObject(obj, "memory_error_information_handle", "No Error Detected");
            } else {
                cJSON_AddNumberToObject(obj, "memory_error_information_handle", t->memory_error_information_handle);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, memory_error_information_handle) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "memory_error_information_handle", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "memory_error_information_handle");
        }

        cjson_field_num(obj, "number_of_memory_devices", LAZYBIOS_FIELD_STATUS(t, number_of_memory_devices), t->number_of_memory_devices);

        if (LAZYBIOS_FIELD_STATUS(t, extended_maximum_capacity) == LAZYBIOS_FIELD_PRESENT) {
            if (t->maximum_capacity != 0x80000000U) {
                cJSON_AddStringToObject(obj, "extended_maximum_capacity", "Not Used");
            } else {
                cJSON_AddNumberToObject(obj, "extended_maximum_capacity", (double)t->extended_maximum_capacity);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, extended_maximum_capacity) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "extended_maximum_capacity", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "extended_maximum_capacity");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type16", arr);
}

void lazybiosExtJSONAddType17(const lazybiosType17_t* type17, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type17 || !count) {
        cJSON_AddStringToObject(root, "Type17", "Failed to get Memory Device information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType17_t* t = &type17[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_field_num(obj, "physical_memory_array_handle", LAZYBIOS_FIELD_STATUS(t, physical_memory_array_handle), t->physical_memory_array_handle);
        cjson_field_num(obj, "memory_error_information_handle", LAZYBIOS_FIELD_STATUS(t, memory_error_information_handle), t->memory_error_information_handle);

        if (LAZYBIOS_FIELD_STATUS(t, total_width) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "total_width_bits", t->total_width == 0xFFFF ? 0 : t->total_width);
        } else if (LAZYBIOS_FIELD_STATUS(t, total_width) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "total_width_bits", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "total_width_bits");
        }

        if (LAZYBIOS_FIELD_STATUS(t, data_width) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "data_width_bits", t->data_width == 0xFFFF ? 0 : t->data_width);
        } else if (LAZYBIOS_FIELD_STATUS(t, data_width) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "data_width_bits", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "data_width_bits");
        }

        if (LAZYBIOS_FIELD_STATUS(t, size) == LAZYBIOS_FIELD_PRESENT) {
            if (t->size == 0) cJSON_AddStringToObject(obj, "size", "No Module Installed");
            else if (t->size == 0xFFFF) cJSON_AddStringToObject(obj, "size", "Unknown");
            else {
                char size_buf[64];
                snprintf(size_buf, sizeof(size_buf), "%u %s", (t->size & 0x7FFF), (t->size & 0x8000) ? "KB" : "MB");
                cJSON_AddStringToObject(obj, "size", size_buf);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, size) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "size", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "size");
        }

        if (LAZYBIOS_FIELD_STATUS(t, form_factor) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "form_factor", lazybiosType17FormFactorStr(t->form_factor));
        } else if (LAZYBIOS_FIELD_STATUS(t, form_factor) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "form_factor", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "form_factor");
        }

        cjson_field_num(obj, "device_set", LAZYBIOS_FIELD_STATUS(t, device_set), t->device_set);
        cjson_add_str(obj, "device_locator", t->device_locator);
        cjson_add_str(obj, "bank_locator", t->bank_locator);

        if (LAZYBIOS_FIELD_STATUS(t, memory_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "memory_type", lazybiosType17TypeStr(t->memory_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, memory_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "memory_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "memory_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, type_detail) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType17TypeDetailStr(t->type_detail, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "type_detail", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, type_detail) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "type_detail", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "type_detail");
        }

        if (LAZYBIOS_FIELD_STATUS(t, speed) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "speed_mts", t->speed == 0xFFFF ? 0 : t->speed);
        } else if (LAZYBIOS_FIELD_STATUS(t, speed) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "speed_mts", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "speed_mts");
        }

        cjson_add_str(obj, "manufacturer", t->manufacturer);
        cjson_add_str(obj, "serial_number", t->serial_number);
        cjson_add_str(obj, "asset_tag", t->asset_tag);
        cjson_add_str(obj, "part_number", t->part_number);

        if (LAZYBIOS_FIELD_STATUS(t, attributes) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "ranks", t->attributes & 0x7F);
        } else if (LAZYBIOS_FIELD_STATUS(t, attributes) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "ranks", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "ranks");
        }

        if (LAZYBIOS_FIELD_STATUS(t, extended_size) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType17ExtendedSizeStr(t->extended_size, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "extended_size", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, extended_size) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "extended_size", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "extended_size");
        }

        if (LAZYBIOS_FIELD_STATUS(t, configured_memory_speed) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "configured_memory_speed_mts", t->configured_memory_speed == 0xFFFF ? 0 : t->configured_memory_speed);
        } else if (LAZYBIOS_FIELD_STATUS(t, configured_memory_speed) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "configured_memory_speed_mts", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "configured_memory_speed_mts");
        }

        cjson_field_num(obj, "minimum_voltage_mv", LAZYBIOS_FIELD_STATUS(t, minimum_voltage), t->minimum_voltage);
        cjson_field_num(obj, "maximum_voltage_mv", LAZYBIOS_FIELD_STATUS(t, maximum_voltage), t->maximum_voltage);
        cjson_field_num(obj, "configured_voltage_mv", LAZYBIOS_FIELD_STATUS(t, configured_voltage), t->configured_voltage);

        if (LAZYBIOS_FIELD_STATUS(t, memory_technology) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "memory_technology", lazybiosType17MemoryTechnologyStr(t->memory_technology));
        } else if (LAZYBIOS_FIELD_STATUS(t, memory_technology) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "memory_technology", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "memory_technology");
        }

        if (LAZYBIOS_FIELD_STATUS(t, memory_operating_mode_capability) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType17OperatingModeCapabilityStr(t->memory_operating_mode_capability, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "memory_operating_mode_capability", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, memory_operating_mode_capability) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "memory_operating_mode_capability", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "memory_operating_mode_capability");
        }

        cjson_add_str(obj, "firmware_version", t->firmware_version);

        if (LAZYBIOS_FIELD_STATUS(t, module_manufacturer_id) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType17ModuleManufacturerIDStr(t->module_manufacturer_id, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "module_manufacturer_id", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, module_manufacturer_id) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "module_manufacturer_id", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "module_manufacturer_id");
        }

        if (LAZYBIOS_FIELD_STATUS(t, module_product_id) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType17ModuleManufacturerIDStr(t->module_product_id, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "module_product_id", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, module_product_id) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "module_product_id", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "module_product_id");
        }

        if (LAZYBIOS_FIELD_STATUS(t, memory_subsystem_controller_manufacturer_id) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType17ModuleManufacturerIDStr(t->memory_subsystem_controller_manufacturer_id, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "memory_subsystem_controller_manufacturer_id", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, memory_subsystem_controller_manufacturer_id) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "memory_subsystem_controller_manufacturer_id", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "memory_subsystem_controller_manufacturer_id");
        }

        if (LAZYBIOS_FIELD_STATUS(t, memory_subsystem_controller_product_id) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType17ModuleManufacturerIDStr(t->memory_subsystem_controller_product_id, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "memory_subsystem_controller_product_id", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, memory_subsystem_controller_product_id) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "memory_subsystem_controller_product_id", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "memory_subsystem_controller_product_id");
        }

        if (LAZYBIOS_FIELD_STATUS(t, non_volatile_size) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType17NonVolatileSizeStr(t->non_volatile_size, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "non_volatile_size", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, non_volatile_size) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "non_volatile_size", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "non_volatile_size");
        }

        if (LAZYBIOS_FIELD_STATUS(t, volatile_size) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType17VolatileSizeStr(t->volatile_size, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "volatile_size", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, volatile_size) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "volatile_size", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "volatile_size");
        }

        if (LAZYBIOS_FIELD_STATUS(t, cache_size) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType17CacheSizeStr(t->cache_size, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "cache_size", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, cache_size) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "cache_size", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "cache_size");
        }

        cjson_field_num(obj, "logical_size_bytes", LAZYBIOS_FIELD_STATUS(t, logical_size), (double)t->logical_size);

        if (LAZYBIOS_FIELD_STATUS(t, extended_speed) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType17ExtendedSpeedStr(t->extended_speed, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "extended_speed", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, extended_speed) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "extended_speed", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "extended_speed");
        }

        if (LAZYBIOS_FIELD_STATUS(t, extended_configured_memory_speed) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType17ExtendedSpeedStr(t->extended_configured_memory_speed, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "extended_configured_memory_speed", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, extended_configured_memory_speed) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "extended_configured_memory_speed", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "extended_configured_memory_speed");
        }

        if (LAZYBIOS_FIELD_STATUS(t, pmic0_manufacturer_id) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType17PMIC0ManufacturerIDStr(t->pmic0_manufacturer_id, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "pmic0_manufacturer_id", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, pmic0_manufacturer_id) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "pmic0_manufacturer_id", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "pmic0_manufacturer_id");
        }

        if (LAZYBIOS_FIELD_STATUS(t, pmic0_revision_number) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType17PMIC0RevisionStr(t->pmic0_revision_number, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "pmic0_revision_number", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, pmic0_revision_number) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "pmic0_revision_number", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "pmic0_revision_number");
        }

        if (LAZYBIOS_FIELD_STATUS(t, rcd_manufacturer_id) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType17RCDManufacturerIDStr(t->rcd_manufacturer_id, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "rcd_manufacturer_id", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, rcd_manufacturer_id) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "rcd_manufacturer_id", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "rcd_manufacturer_id");
        }

        if (LAZYBIOS_FIELD_STATUS(t, rcd_revision_number) == LAZYBIOS_FIELD_PRESENT) {
            char buf[LAZYBIOS_DECODER_BUF_SIZE];
            lazybiosType17RCDRevisionStr(t->rcd_revision_number, buf, sizeof(buf));
            cJSON_AddStringToObject(obj, "rcd_revision_number", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, rcd_revision_number) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "rcd_revision_number", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "rcd_revision_number");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type17", arr);
}

void lazybiosExtJSONAddType18(const lazybiosType18_t* type18, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type18 || !count) {
        cJSON_AddStringToObject(root, "Type18", "Failed to get 32-Bit Memory Error information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType18_t* t = &type18[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        if (LAZYBIOS_FIELD_STATUS(t, error_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "error_type", lazybiosType18ErrorTypeStr(t->error_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, error_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "error_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "error_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, error_granularity) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "error_granularity", lazybiosType18ErrorGranularityStr(t->error_granularity));
        } else if (LAZYBIOS_FIELD_STATUS(t, error_granularity) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "error_granularity", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "error_granularity");
        }

        if (LAZYBIOS_FIELD_STATUS(t, error_operation) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "error_operation", lazybiosType18ErrorOperationStr(t->error_operation));
        } else if (LAZYBIOS_FIELD_STATUS(t, error_operation) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "error_operation", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "error_operation");
        }

        if (LAZYBIOS_FIELD_STATUS(t, vendor_syndrome) == LAZYBIOS_FIELD_PRESENT) {
            if (t->vendor_syndrome == 0) {
                cJSON_AddStringToObject(obj, "vendor_syndrome", "Unknown");
            } else {
                cJSON_AddNumberToObject(obj, "vendor_syndrome", t->vendor_syndrome);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, vendor_syndrome) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "vendor_syndrome", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "vendor_syndrome");
        }

        if (LAZYBIOS_FIELD_STATUS(t, memory_array_error_address) == LAZYBIOS_FIELD_PRESENT) {
            if (t->memory_array_error_address == 0x80000000U) {
                cJSON_AddStringToObject(obj, "memory_array_error_address", "Unknown");
            } else {
                cJSON_AddNumberToObject(obj, "memory_array_error_address", t->memory_array_error_address);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, memory_array_error_address) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "memory_array_error_address", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "memory_array_error_address");
        }

        if (LAZYBIOS_FIELD_STATUS(t, device_error_address) == LAZYBIOS_FIELD_PRESENT) {
            if (t->device_error_address == 0x80000000U) {
                cJSON_AddStringToObject(obj, "device_error_address", "Unknown");
            } else {
                cJSON_AddNumberToObject(obj, "device_error_address", t->device_error_address);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, device_error_address) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "device_error_address", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "device_error_address");
        }

        if (LAZYBIOS_FIELD_STATUS(t, error_resolution) == LAZYBIOS_FIELD_PRESENT) {
            if (t->error_resolution == 0x80000000U) {
                cJSON_AddStringToObject(obj, "error_resolution", "Unknown");
            } else {
                cJSON_AddNumberToObject(obj, "error_resolution_bytes", t->error_resolution);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, error_resolution) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "error_resolution_bytes", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "error_resolution_bytes");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type18", arr);
}

void lazybiosExtJSONAddType19(const lazybiosType19_t* type19, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type19 || !count) {
        cJSON_AddStringToObject(root, "Type19", "Failed to get Memory Array Mapped Address information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType19_t* t = &type19[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        if (LAZYBIOS_FIELD_STATUS(t, starting_address) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t addr = lazybiosType19StartingAddressBytes(t->starting_address, t->extended_starting_address);
            cJSON_AddNumberToObject(obj, "starting_address_bytes", (double)addr);
        } else if (LAZYBIOS_FIELD_STATUS(t, starting_address) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "starting_address_bytes", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "starting_address_bytes");
        }

        if (LAZYBIOS_FIELD_STATUS(t, ending_address) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t addr = lazybiosType19EndingAddressBytes(t->ending_address, t->extended_ending_address);
            cJSON_AddNumberToObject(obj, "ending_address_bytes", (double)addr);
        } else if (LAZYBIOS_FIELD_STATUS(t, ending_address) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "ending_address_bytes", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "ending_address_bytes");
        }

        cjson_field_num(obj, "memory_array_handle", LAZYBIOS_FIELD_STATUS(t, memory_array_handle), t->memory_array_handle);
        cjson_field_num(obj, "partition_width", LAZYBIOS_FIELD_STATUS(t, partition_width), t->partition_width);

        if (LAZYBIOS_FIELD_STATUS(t, extended_starting_address) == LAZYBIOS_FIELD_PRESENT) {
            if (t->starting_address != 0xFFFFFFFFU) {
                cJSON_AddStringToObject(obj, "extended_starting_address", "Not Used");
            } else {
                cJSON_AddNumberToObject(obj, "extended_starting_address", (double)t->extended_starting_address);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, extended_starting_address) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "extended_starting_address", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "extended_starting_address");
        }

        if (LAZYBIOS_FIELD_STATUS(t, extended_ending_address) == LAZYBIOS_FIELD_PRESENT) {
            if (t->ending_address != 0xFFFFFFFFU) {
                cJSON_AddStringToObject(obj, "extended_ending_address", "Not Used");
            } else {
                cJSON_AddNumberToObject(obj, "extended_ending_address", (double)t->extended_ending_address);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, extended_ending_address) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "extended_ending_address", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "extended_ending_address");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type19", arr);
}

void lazybiosExtJSONAddType20(const lazybiosType20_t* type20, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type20 || !count) {
        cJSON_AddStringToObject(root, "Type20", "Failed to get Memory Device Mapped Address information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType20_t* t = &type20[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        if (LAZYBIOS_FIELD_STATUS(t, starting_address) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t addr = lazybiosType20StartingAddressBytes(t->starting_address, t->extended_starting_address);
            cJSON_AddNumberToObject(obj, "starting_address_bytes", (double)addr);
        } else if (LAZYBIOS_FIELD_STATUS(t, starting_address) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "starting_address_bytes", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "starting_address_bytes");
        }

        if (LAZYBIOS_FIELD_STATUS(t, ending_address) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t addr = lazybiosType20EndingAddressBytes(t->ending_address, t->extended_ending_address);
            cJSON_AddNumberToObject(obj, "ending_address_bytes", (double)addr);
        } else if (LAZYBIOS_FIELD_STATUS(t, ending_address) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "ending_address_bytes", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "ending_address_bytes");
        }

        cjson_field_num(obj, "memory_device_handle", LAZYBIOS_FIELD_STATUS(t, memory_device_handle), t->memory_device_handle);
        cjson_field_num(obj, "memory_array_mapped_address_handle", LAZYBIOS_FIELD_STATUS(t, memory_array_mapped_address_handle), t->memory_array_mapped_address_handle);

        if (LAZYBIOS_FIELD_STATUS(t, partition_row_position) == LAZYBIOS_FIELD_PRESENT) {
            if (t->partition_row_position == 0xFF) cJSON_AddStringToObject(obj, "partition_row_position", "Unknown");
            else if (t->partition_row_position == 0) cJSON_AddStringToObject(obj, "partition_row_position", "Reserved");
            else cJSON_AddNumberToObject(obj, "partition_row_position", t->partition_row_position);
        } else if (LAZYBIOS_FIELD_STATUS(t, partition_row_position) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "partition_row_position", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "partition_row_position");
        }

        if (LAZYBIOS_FIELD_STATUS(t, interleave_position) == LAZYBIOS_FIELD_PRESENT) {
            if (t->interleave_position == 0xFF) cJSON_AddStringToObject(obj, "interleave_position", "Unknown");
            else if (t->interleave_position == 0) cJSON_AddStringToObject(obj, "interleave_position", "Non-interleaved");
            else cJSON_AddNumberToObject(obj, "interleave_position", t->interleave_position);
        } else if (LAZYBIOS_FIELD_STATUS(t, interleave_position) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "interleave_position", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "interleave_position");
        }

        if (LAZYBIOS_FIELD_STATUS(t, interleaved_data_depth) == LAZYBIOS_FIELD_PRESENT) {
            if (t->interleaved_data_depth == 0xFF) cJSON_AddStringToObject(obj, "interleaved_data_depth", "Unknown");
            else if (t->interleaved_data_depth == 0) cJSON_AddStringToObject(obj, "interleaved_data_depth", "Not Interleaved");
            else cJSON_AddNumberToObject(obj, "interleaved_data_depth_rows", t->interleaved_data_depth);
        } else if (LAZYBIOS_FIELD_STATUS(t, interleaved_data_depth) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "interleaved_data_depth_rows", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "interleaved_data_depth_rows");
        }

        if (LAZYBIOS_FIELD_STATUS(t, extended_starting_address) == LAZYBIOS_FIELD_PRESENT) {
            if (t->starting_address != 0xFFFFFFFFU) {
                cJSON_AddStringToObject(obj, "extended_starting_address", "Not Used");
            } else {
                cJSON_AddNumberToObject(obj, "extended_starting_address", (double)t->extended_starting_address);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, extended_starting_address) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "extended_starting_address", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "extended_starting_address");
        }

        if (LAZYBIOS_FIELD_STATUS(t, extended_ending_address) == LAZYBIOS_FIELD_PRESENT) {
            if (t->ending_address != 0xFFFFFFFFU) {
                cJSON_AddStringToObject(obj, "extended_ending_address", "Not Used");
            } else {
                cJSON_AddNumberToObject(obj, "extended_ending_address", (double)t->extended_ending_address);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, extended_ending_address) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "extended_ending_address", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "extended_ending_address");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type20", arr);
}

void lazybiosExtJSONAddType21(const lazybiosType21_t* type21, size_t count, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type21 || !count) {
        cJSON_AddStringToObject(root, "Type21", "Failed to get Built-in Pointing Device information");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const lazybiosType21_t* t = &type21[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        if (LAZYBIOS_FIELD_STATUS(t, pointing_device_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "pointing_device_type", lazybiosType21PointingDeviceTypeStr(t->pointing_device_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, pointing_device_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "pointing_device_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "pointing_device_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, interface) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "interface", lazybiosType21InterfaceStr(t->interface));
        } else if (LAZYBIOS_FIELD_STATUS(t, interface) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "interface", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "interface");
        }

        cjson_field_num(obj, "number_of_buttons", LAZYBIOS_FIELD_STATUS(t, number_of_buttons), t->number_of_buttons);

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type21", arr);
}

void lazybiosExtJSONAddType22(const lazybiosType22_t* type22, size_t count, cJSON* root) {
    if (!root) return;
    if (!type22 || !count) {
        cJSON_AddStringToObject(root, "Type22", "Failed to get Portable Battery information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType22_t* t = &type22[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_str(obj, "location", LAZYBIOS_FIELD_STATUS(t, location), t->location);
        cjson_field_str(obj, "manufacturer", LAZYBIOS_FIELD_STATUS(t, manufacturer), t->manufacturer);
        cjson_field_str(obj, "manufacture_date", LAZYBIOS_FIELD_STATUS(t, manufacture_date), t->manufacture_date);
        cjson_field_str(obj, "serial_number", LAZYBIOS_FIELD_STATUS(t, serial_number), t->serial_number);
        cjson_field_str(obj, "device_name", LAZYBIOS_FIELD_STATUS(t, device_name), t->device_name);
        if (LAZYBIOS_FIELD_STATUS(t, device_chemistry) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "device_chemistry", lazybiosType22DeviceChemistryStr(t->device_chemistry));
        } else if (LAZYBIOS_FIELD_STATUS(t, device_chemistry) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "device_chemistry", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "device_chemistry");
        }
        cjson_field_num(obj, "design_capacity", LAZYBIOS_FIELD_STATUS(t, design_capacity), t->design_capacity);
        cjson_field_num(obj, "design_voltage", LAZYBIOS_FIELD_STATUS(t, design_voltage), t->design_voltage);
        cjson_field_str(obj, "sbds_version_number", LAZYBIOS_FIELD_STATUS(t, sbds_version_number), t->sbds_version_number);
        cjson_field_num(obj, "maximum_error_percent", LAZYBIOS_FIELD_STATUS(t, maximum_error), t->maximum_error);
        cjson_field_num(obj, "sbds_serial_number", LAZYBIOS_FIELD_STATUS(t, sbds_serial_number), t->sbds_serial_number);
        cjson_field_num(obj, "sbds_manufacture_date", LAZYBIOS_FIELD_STATUS(t, sbds_manufacture_date), t->sbds_manufacture_date);
        cjson_field_str(obj, "sbds_device_chemistry", LAZYBIOS_FIELD_STATUS(t, sbds_device_chemistry), t->sbds_device_chemistry);
        cjson_field_num(obj, "design_capacity_multiplier", LAZYBIOS_FIELD_STATUS(t, design_capacity_multiplier), t->design_capacity_multiplier);
        cjson_field_num(obj, "oem_specific", LAZYBIOS_FIELD_STATUS(t, oem_specific), t->oem_specific);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type22", arr);
}

void lazybiosExtJSONAddType23(const lazybiosType23_t* type23, size_t count, cJSON* root) {
    if (!root) return;
    if (!type23 || !count) {
        cJSON_AddStringToObject(root, "Type23", "Failed to get System Reset information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType23_t* t = &type23[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_num(obj, "capabilities", LAZYBIOS_FIELD_STATUS(t, capabilities), t->capabilities);
        cjson_field_num(obj, "reset_count", LAZYBIOS_FIELD_STATUS(t, reset_count), t->reset_count);
        cjson_field_num(obj, "reset_limit", LAZYBIOS_FIELD_STATUS(t, reset_limit), t->reset_limit);
        cjson_field_num(obj, "timer_interval", LAZYBIOS_FIELD_STATUS(t, timer_interval), t->timer_interval);
        cjson_field_num(obj, "timeout", LAZYBIOS_FIELD_STATUS(t, timeout), t->timeout);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type23", arr);
}

void lazybiosExtJSONAddType24(const lazybiosType24_t* type24, size_t count, cJSON* root) {
    if (!root) return;
    if (!type24 || !count) {
        cJSON_AddStringToObject(root, "Type24", "Failed to get Hardware Security information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType24_t* t = &type24[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_num(obj, "hardware_security_settings", LAZYBIOS_FIELD_STATUS(t, hardware_security_settings), t->hardware_security_settings);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type24", arr);
}

void lazybiosExtJSONAddType25(const lazybiosType25_t* type25, size_t count, cJSON* root) {
    if (!root) return;
    if (!type25 || !count) {
        cJSON_AddStringToObject(root, "Type25", "Failed to get System Power Controls information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType25_t* t = &type25[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_num(obj, "next_scheduled_power_on_month", LAZYBIOS_FIELD_STATUS(t, next_scheduled_power_on_month), t->next_scheduled_power_on_month);
        cjson_field_num(obj, "next_scheduled_power_on_day_of_month", LAZYBIOS_FIELD_STATUS(t, next_scheduled_power_on_day), t->next_scheduled_power_on_day);
        cjson_field_num(obj, "next_scheduled_power_on_hour", LAZYBIOS_FIELD_STATUS(t, next_scheduled_power_on_hour), t->next_scheduled_power_on_hour);
        cjson_field_num(obj, "next_scheduled_power_on_minute", LAZYBIOS_FIELD_STATUS(t, next_scheduled_power_on_minute), t->next_scheduled_power_on_minute);
        cjson_field_num(obj, "next_scheduled_power_on_second", LAZYBIOS_FIELD_STATUS(t, next_scheduled_power_on_second), t->next_scheduled_power_on_second);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type25", arr);
}

void lazybiosExtJSONAddType26(const lazybiosType26_t* type26, size_t count, cJSON* root) {
    if (!root) return;
    if (!type26 || !count) {
        cJSON_AddStringToObject(root, "Type26", "Failed to get Voltage Probe information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType26_t* t = &type26[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_str(obj, "description", LAZYBIOS_FIELD_STATUS(t, description), t->description);
        if (LAZYBIOS_FIELD_STATUS(t, location_and_status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "location", lazybiosType26LocationStr(t->location_and_status));
            cJSON_AddStringToObject(obj, "status", lazybiosType26StatusStr(t->location_and_status));
        } else if (LAZYBIOS_FIELD_STATUS(t, location_and_status) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "location", LAZYBIOS_JSON_UNREACHABLE);
            cJSON_AddStringToObject(obj, "status", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "location");
            cJSON_AddNullToObject(obj, "status");
        }
        cjson_field_num(obj, "maximum_value", LAZYBIOS_FIELD_STATUS(t, maximum_value), t->maximum_value);
        cjson_field_num(obj, "minimum_value", LAZYBIOS_FIELD_STATUS(t, minimum_value), t->minimum_value);
        cjson_field_num(obj, "resolution", LAZYBIOS_FIELD_STATUS(t, resolution), t->resolution);
        cjson_field_num(obj, "tolerance", LAZYBIOS_FIELD_STATUS(t, tolerance), t->tolerance);
        cjson_field_num(obj, "accuracy", LAZYBIOS_FIELD_STATUS(t, accuracy), t->accuracy);
        cjson_field_num(obj, "oem_defined", LAZYBIOS_FIELD_STATUS(t, oem_defined), t->oem_defined);
        cjson_field_num(obj, "nominal_value", LAZYBIOS_FIELD_STATUS(t, nominal_value), t->nominal_value);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type26", arr);
}

void lazybiosExtJSONAddType27(const lazybiosType27_t* type27, size_t count, cJSON* root) {
    if (!root) return;
    if (!type27 || !count) {
        cJSON_AddStringToObject(root, "Type27", "Failed to get Cooling Device information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType27_t* t = &type27[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_num(obj, "temperature_probe_handle", LAZYBIOS_FIELD_STATUS(t, temperature_probe_handle), t->temperature_probe_handle);
        if (LAZYBIOS_FIELD_STATUS(t, device_type_and_status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "device_type", lazybiosType27DeviceTypeStr(t->device_type_and_status));
            cJSON_AddStringToObject(obj, "status", lazybiosType27StatusStr(t->device_type_and_status));
        } else if (LAZYBIOS_FIELD_STATUS(t, device_type_and_status) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "device_type", LAZYBIOS_JSON_UNREACHABLE);
            cJSON_AddStringToObject(obj, "status", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "device_type");
            cJSON_AddNullToObject(obj, "status");
        }
        cjson_field_num(obj, "cooling_unit_group", LAZYBIOS_FIELD_STATUS(t, cooling_unit_group), t->cooling_unit_group);
        cjson_field_num(obj, "oem_defined", LAZYBIOS_FIELD_STATUS(t, oem_defined), t->oem_defined);
        cjson_field_num(obj, "nominal_speed", LAZYBIOS_FIELD_STATUS(t, nominal_speed), t->nominal_speed);
        cjson_field_str(obj, "description", LAZYBIOS_FIELD_STATUS(t, description), t->description);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type27", arr);
}

void lazybiosExtJSONAddType28(const lazybiosType28_t* type28, size_t count, cJSON* root) {
    if (!root) return;
    if (!type28 || !count) {
        cJSON_AddStringToObject(root, "Type28", "Failed to get Temperature Probe information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType28_t* t = &type28[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_str(obj, "description", LAZYBIOS_FIELD_STATUS(t, description), t->description);
        if (LAZYBIOS_FIELD_STATUS(t, location_and_status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "location", lazybiosType28LocationStr(t->location_and_status));
            cJSON_AddStringToObject(obj, "status", lazybiosType28StatusStr(t->location_and_status));
        } else if (LAZYBIOS_FIELD_STATUS(t, location_and_status) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "location", LAZYBIOS_JSON_UNREACHABLE);
            cJSON_AddStringToObject(obj, "status", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "location");
            cJSON_AddNullToObject(obj, "status");
        }
        cjson_field_num(obj, "maximum_value", LAZYBIOS_FIELD_STATUS(t, maximum_value), t->maximum_value);
        cjson_field_num(obj, "minimum_value", LAZYBIOS_FIELD_STATUS(t, minimum_value), t->minimum_value);
        cjson_field_num(obj, "resolution", LAZYBIOS_FIELD_STATUS(t, resolution), t->resolution);
        cjson_field_num(obj, "tolerance", LAZYBIOS_FIELD_STATUS(t, tolerance), t->tolerance);
        cjson_field_num(obj, "accuracy", LAZYBIOS_FIELD_STATUS(t, accuracy), t->accuracy);
        cjson_field_num(obj, "oem_defined", LAZYBIOS_FIELD_STATUS(t, oem_defined), t->oem_defined);
        cjson_field_num(obj, "nominal_value", LAZYBIOS_FIELD_STATUS(t, nominal_value), t->nominal_value);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type28", arr);
}

void lazybiosExtJSONAddType29(const lazybiosType29_t* type29, size_t count, cJSON* root) {
    if (!root) return;
    if (!type29 || !count) {
        cJSON_AddStringToObject(root, "Type29", "Failed to get Electrical Current Probe information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType29_t* t = &type29[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_str(obj, "description", LAZYBIOS_FIELD_STATUS(t, description), t->description);
        if (LAZYBIOS_FIELD_STATUS(t, location_and_status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "location", lazybiosType29LocationStr(t->location_and_status));
            cJSON_AddStringToObject(obj, "status", lazybiosType29StatusStr(t->location_and_status));
        } else if (LAZYBIOS_FIELD_STATUS(t, location_and_status) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "location", LAZYBIOS_JSON_UNREACHABLE);
            cJSON_AddStringToObject(obj, "status", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "location");
            cJSON_AddNullToObject(obj, "status");
        }
        cjson_field_num(obj, "maximum_value", LAZYBIOS_FIELD_STATUS(t, maximum_value), t->maximum_value);
        cjson_field_num(obj, "minimum_value", LAZYBIOS_FIELD_STATUS(t, minimum_value), t->minimum_value);
        cjson_field_num(obj, "resolution", LAZYBIOS_FIELD_STATUS(t, resolution), t->resolution);
        cjson_field_num(obj, "tolerance", LAZYBIOS_FIELD_STATUS(t, tolerance), t->tolerance);
        cjson_field_num(obj, "accuracy", LAZYBIOS_FIELD_STATUS(t, accuracy), t->accuracy);
        cjson_field_num(obj, "oem_defined", LAZYBIOS_FIELD_STATUS(t, oem_defined), t->oem_defined);
        cjson_field_num(obj, "nominal_value", LAZYBIOS_FIELD_STATUS(t, nominal_value), t->nominal_value);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type29", arr);
}

void lazybiosExtJSONAddType30(const lazybiosType30_t* type30, size_t count, cJSON* root) {
    if (!root) return;
    if (!type30 || !count) {
        cJSON_AddStringToObject(root, "Type30", "Failed to get Out-of-Band Remote Access information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType30_t* t = &type30[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_str(obj, "manufacturer_name", LAZYBIOS_FIELD_STATUS(t, manufacturer_name), t->manufacturer_name);
        cjson_field_num(obj, "connections", LAZYBIOS_FIELD_STATUS(t, connections), t->connections);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type30", arr);
}

void lazybiosExtJSONAddType31(const lazybiosType31_t* type31, size_t count, cJSON* root) {
    if (!root) return;
    if (!type31 || !count) {
        cJSON_AddStringToObject(root, "Type31", "Failed to get Boot Integrity Services information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType31_t* t = &type31[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_num(obj, "checksum", LAZYBIOS_FIELD_STATUS(t, checksum), t->checksum);
        cjson_field_num(obj, "reserved1", LAZYBIOS_FIELD_STATUS(t, reserved_1), t->reserved_1);
        cjson_field_num(obj, "reserved2", LAZYBIOS_FIELD_STATUS(t, reserved_2), t->reserved_2);
        cjson_field_num(obj, "bis_entry_point_16", LAZYBIOS_FIELD_STATUS(t, bis_entry_point_16), t->bis_entry_point_16);
        cjson_field_num(obj, "bis_entry_point_32", LAZYBIOS_FIELD_STATUS(t, bis_entry_point_32), t->bis_entry_point_32);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type31", arr);
}

void lazybiosExtJSONAddType32(const lazybiosType32_t* type32, size_t count, cJSON* root) {
    if (!root) return;
    if (!type32 || !count) {
        cJSON_AddStringToObject(root, "Type32", "Failed to get System Boot information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType32_t* t = &type32[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        if (LAZYBIOS_FIELD_STATUS(t, reserved) == LAZYBIOS_FIELD_PRESENT) {
            cJSON* res_arr = cJSON_CreateArray();
            for (int j = 0; j < 6; j++) cJSON_AddItemToArray(res_arr, cJSON_CreateNumber(t->reserved[j]));
            cJSON_AddItemToObject(obj, "reserved", res_arr);
        }
        if (LAZYBIOS_FIELD_STATUS(t, boot_status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "boot_status", lazybiosType32BootStatusStr(t->boot_status));
        } else if (LAZYBIOS_FIELD_STATUS(t, boot_status) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "boot_status", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "boot_status");
        }
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type32", arr);
}

void lazybiosExtJSONAddType33(const lazybiosType33_t* type33, size_t count, cJSON* root) {
    if (!root) return;
    if (!type33 || !count) {
        cJSON_AddStringToObject(root, "Type33", "Failed to get 64-Bit Memory Error information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType33_t* t = &type33[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        if (LAZYBIOS_FIELD_STATUS(t, error_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "error_type", lazybiosType33ErrorTypeStr(t->error_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, error_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "error_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "error_type");
        }
        if (LAZYBIOS_FIELD_STATUS(t, error_granularity) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "error_granularity", lazybiosType33ErrorGranularityStr(t->error_granularity));
        } else if (LAZYBIOS_FIELD_STATUS(t, error_granularity) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "error_granularity", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "error_granularity");
        }
        if (LAZYBIOS_FIELD_STATUS(t, error_operation) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "error_operation", lazybiosType33ErrorOperationStr(t->error_operation));
        } else if (LAZYBIOS_FIELD_STATUS(t, error_operation) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "error_operation", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "error_operation");
        }
        if (LAZYBIOS_FIELD_STATUS(t, vendor_syndrome) == LAZYBIOS_FIELD_PRESENT) {
            if (t->vendor_syndrome == 0) cJSON_AddStringToObject(obj, "vendor_syndrome", "Unknown");
            else cJSON_AddNumberToObject(obj, "vendor_syndrome", t->vendor_syndrome);
        } else if (LAZYBIOS_FIELD_STATUS(t, vendor_syndrome) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "vendor_syndrome", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "vendor_syndrome");
        }
        if (LAZYBIOS_FIELD_STATUS(t, memory_array_error_address) == LAZYBIOS_FIELD_PRESENT) {
            if (t->memory_array_error_address == 0x8000000000000000ULL) cJSON_AddStringToObject(obj, "memory_array_error_address", "Unknown");
            else cJSON_AddNumberToObject(obj, "memory_array_error_address", (double)t->memory_array_error_address);
        } else if (LAZYBIOS_FIELD_STATUS(t, memory_array_error_address) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "memory_array_error_address", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "memory_array_error_address");
        }
        if (LAZYBIOS_FIELD_STATUS(t, device_error_address) == LAZYBIOS_FIELD_PRESENT) {
            if (t->device_error_address == 0x8000000000000000ULL) cJSON_AddStringToObject(obj, "device_error_address", "Unknown");
            else cJSON_AddNumberToObject(obj, "device_error_address", (double)t->device_error_address);
        } else if (LAZYBIOS_FIELD_STATUS(t, device_error_address) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "device_error_address", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "device_error_address");
        }
        if (LAZYBIOS_FIELD_STATUS(t, error_resolution) == LAZYBIOS_FIELD_PRESENT) {
            if (t->error_resolution == 0x80000000U) cJSON_AddStringToObject(obj, "error_resolution", "Unknown");
            else cJSON_AddNumberToObject(obj, "error_resolution_bytes", t->error_resolution);
        } else if (LAZYBIOS_FIELD_STATUS(t, error_resolution) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "error_resolution_bytes", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "error_resolution_bytes");
        }
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type33", arr);
}

void lazybiosExtJSONAddType34(const lazybiosType34_t* type34, size_t count, cJSON* root) {
    if (!root) return;
    if (!type34 || !count) {
        cJSON_AddStringToObject(root, "Type34", "Failed to get Management Device information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType34_t* t = &type34[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_str(obj, "description", LAZYBIOS_FIELD_STATUS(t, description), t->description);
        if (LAZYBIOS_FIELD_STATUS(t, device_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "device_type", lazybiosType34DeviceTypeStr(t->device_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, device_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "device_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "device_type");
        }
        cjson_field_num(obj, "address", LAZYBIOS_FIELD_STATUS(t, address), t->address);
        if (LAZYBIOS_FIELD_STATUS(t, address_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "address_type", lazybiosType34AddressTypeStr(t->address_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, address_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "address_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "address_type");
        }
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type34", arr);
}

void lazybiosExtJSONAddType35(const lazybiosType35_t* type35, size_t count, cJSON* root) {
    if (!root) return;
    if (!type35 || !count) {
        cJSON_AddStringToObject(root, "Type35", "Failed to get Management Device Component information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType35_t* t = &type35[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_str(obj, "description", LAZYBIOS_FIELD_STATUS(t, description), t->description);
        cjson_field_num(obj, "management_device_handle", LAZYBIOS_FIELD_STATUS(t, management_device_handle), t->management_device_handle);
        cjson_field_num(obj, "component_handle", LAZYBIOS_FIELD_STATUS(t, component_handle), t->component_handle);
        cjson_field_num(obj, "threshold_handle", LAZYBIOS_FIELD_STATUS(t, threshold_handle), t->threshold_handle);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type35", arr);
}

void lazybiosExtJSONAddType36(const lazybiosType36_t* type36, size_t count, cJSON* root) {
    if (!root) return;
    if (!type36 || !count) {
        cJSON_AddStringToObject(root, "Type36", "Failed to get Management Device Threshold Data information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType36_t* t = &type36[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_num(obj, "lower_threshold_non_critical", LAZYBIOS_FIELD_STATUS(t, lower_threshold_non_critical), t->lower_threshold_non_critical);
        cjson_field_num(obj, "upper_threshold_non_critical", LAZYBIOS_FIELD_STATUS(t, upper_threshold_non_critical), t->upper_threshold_non_critical);
        cjson_field_num(obj, "lower_threshold_critical", LAZYBIOS_FIELD_STATUS(t, lower_threshold_critical), t->lower_threshold_critical);
        cjson_field_num(obj, "upper_threshold_critical", LAZYBIOS_FIELD_STATUS(t, upper_threshold_critical), t->upper_threshold_critical);
        cjson_field_num(obj, "lower_threshold_non_recoverable", LAZYBIOS_FIELD_STATUS(t, lower_threshold_non_recoverable), t->lower_threshold_non_recoverable);
        cjson_field_num(obj, "upper_threshold_non_recoverable", LAZYBIOS_FIELD_STATUS(t, upper_threshold_non_recoverable), t->upper_threshold_non_recoverable);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type36", arr);
}

void lazybiosExtJSONAddType37(const lazybiosType37_t* type37, size_t count, cJSON* root) {
    if (!root) return;
    if (!type37 || !count) {
        cJSON_AddStringToObject(root, "Type37", "Failed to get Memory Channel information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType37_t* t = &type37[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        if (LAZYBIOS_FIELD_STATUS(t, channel_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "channel_type", lazybiosType37ChannelTypeStr(t->channel_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, channel_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "channel_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "channel_type");
        }
        cjson_field_num(obj, "maximum_channel_load", LAZYBIOS_FIELD_STATUS(t, maximum_channel_load), t->maximum_channel_load);
        cjson_field_num(obj, "memory_device_count", LAZYBIOS_FIELD_STATUS(t, memory_device_count), t->memory_device_count);
        if (LAZYBIOS_FIELD_STATUS(t, memory_devices) == LAZYBIOS_FIELD_PRESENT && t->memory_devices) {
            cJSON* load_arr = cJSON_CreateArray();
            for (uint8_t j = 0; j < t->memory_device_count; j++) {
                cJSON* load_obj = cJSON_CreateObject();
                cJSON_AddNumberToObject(load_obj, "load", t->memory_devices[j].load);
                cJSON_AddNumberToObject(load_obj, "handle", t->memory_devices[j].handle);
                cJSON_AddItemToArray(load_arr, load_obj);
            }
            cJSON_AddItemToObject(obj, "memory_devices", load_arr);
        }
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type37", arr);
}

void lazybiosExtJSONAddType38(const lazybiosType38_t* type38, size_t count, cJSON* root) {
    if (!root) return;
    if (!type38 || !count) {
        cJSON_AddStringToObject(root, "Type38", "Failed to get IPMI Device information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType38_t* t = &type38[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        if (LAZYBIOS_FIELD_STATUS(t, interface_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "interface_type", lazybiosType38InterfaceTypeStr(t->interface_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, interface_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "interface_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "interface_type");
        }
        cjson_field_num(obj, "ipmi_specification_revision", LAZYBIOS_FIELD_STATUS(t, ipmi_specification_revision), t->ipmi_specification_revision);
        cjson_field_num(obj, "i2c_target_address", LAZYBIOS_FIELD_STATUS(t, i2c_target_address), t->i2c_target_address);
        cjson_field_num(obj, "nv_storage_device_address", LAZYBIOS_FIELD_STATUS(t, nv_storage_device_address), t->nv_storage_device_address);
        cjson_field_num(obj, "base_address", LAZYBIOS_FIELD_STATUS(t, base_address), (double)t->base_address);
        cjson_field_num(obj, "base_address_modifier_interrupt_info", LAZYBIOS_FIELD_STATUS(t, base_address_modifier_interrupt_info), t->base_address_modifier_interrupt_info);
        cjson_field_num(obj, "interrupt_number", LAZYBIOS_FIELD_STATUS(t, interrupt_number), t->interrupt_number);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type38", arr);
}

void lazybiosExtJSONAddType39(const lazybiosType39_t* type39, size_t count, cJSON* root) {
    if (!root) return;
    if (!type39 || !count) {
        cJSON_AddStringToObject(root, "Type39", "Failed to get System Power Supply information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType39_t* t = &type39[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_num(obj, "power_unit_group", LAZYBIOS_FIELD_STATUS(t, power_unit_group), t->power_unit_group);
        cjson_field_str(obj, "location", LAZYBIOS_FIELD_STATUS(t, location), t->location);
        cjson_field_str(obj, "device_name", LAZYBIOS_FIELD_STATUS(t, device_name), t->device_name);
        cjson_field_str(obj, "manufacturer", LAZYBIOS_FIELD_STATUS(t, manufacturer), t->manufacturer);
        cjson_field_str(obj, "serial_number", LAZYBIOS_FIELD_STATUS(t, serial_number), t->serial_number);
        cjson_field_str(obj, "asset_tag_number", LAZYBIOS_FIELD_STATUS(t, asset_tag_number), t->asset_tag_number);
        cjson_field_str(obj, "model_part_number", LAZYBIOS_FIELD_STATUS(t, model_part_number), t->model_part_number);
        cjson_field_str(obj, "revision_level", LAZYBIOS_FIELD_STATUS(t, revision_level), t->revision_level);
        cjson_field_num(obj, "max_power_capacity", LAZYBIOS_FIELD_STATUS(t, max_power_capacity), t->max_power_capacity);
        cjson_field_num(obj, "power_supply_characteristics", LAZYBIOS_FIELD_STATUS(t, power_supply_characteristics), t->power_supply_characteristics);
        cjson_field_num(obj, "input_voltage_probe_handle", LAZYBIOS_FIELD_STATUS(t, input_voltage_probe_handle), t->input_voltage_probe_handle);
        cjson_field_num(obj, "cooling_device_handle", LAZYBIOS_FIELD_STATUS(t, cooling_device_handle), t->cooling_device_handle);
        cjson_field_num(obj, "input_current_probe_handle", LAZYBIOS_FIELD_STATUS(t, input_current_probe_handle), t->input_current_probe_handle);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type39", arr);
}

void lazybiosExtJSONAddType40(const lazybiosType40_t* type40, size_t count, cJSON* root) {
    if (!root) return;
    if (!type40 || !count) {
        cJSON_AddStringToObject(root, "Type40", "Failed to get Additional Information information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType40_t* t = &type40[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_num(obj, "additional_information_entry_count", LAZYBIOS_FIELD_STATUS(t, additional_information_entry_count), t->additional_information_entry_count);
        if (LAZYBIOS_FIELD_STATUS(t, additional_information_entries) == LAZYBIOS_FIELD_PRESENT && t->additional_information_entries) {
            cJSON* entries_arr = cJSON_CreateArray();
            for (uint8_t j = 0; j < t->additional_information_entry_count; j++) {
                cJSON* entry_obj = cJSON_CreateObject();
                cJSON_AddNumberToObject(entry_obj, "entry_length", t->additional_information_entries[j].entry_length);
                cJSON_AddNumberToObject(entry_obj, "referenced_handle", t->additional_information_entries[j].referenced_handle);
                cJSON_AddNumberToObject(entry_obj, "referenced_offset", t->additional_information_entries[j].referenced_offset);
                cjson_field_str(entry_obj, "string", LAZYBIOS_FIELD_PRESENT, t->additional_information_entries[j].string);
                if (t->additional_information_entries[j].value && t->additional_information_entries[j].value_length > 0) {
                    cJSON* val_arr = cJSON_CreateArray();
                    for (size_t k = 0; k < t->additional_information_entries[j].value_length; k++) {
                        cJSON_AddItemToArray(val_arr, cJSON_CreateNumber(t->additional_information_entries[j].value[k]));
                    }
                    cJSON_AddItemToObject(entry_obj, "value", val_arr);
                }
                cJSON_AddItemToArray(entries_arr, entry_obj);
            }
            cJSON_AddItemToObject(obj, "additional_information_entries", entries_arr);
        }
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type40", arr);
}

void lazybiosExtJSONAddType41(const lazybiosType41_t* type41, size_t count, cJSON* root) {
    if (!root) return;
    if (!type41 || !count) {
        cJSON_AddStringToObject(root, "Type41", "Failed to get Onboard Devices Extended Information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType41_t* t = &type41[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_str(obj, "reference_designation", LAZYBIOS_FIELD_STATUS(t, reference_designation), t->reference_designation);
        if (LAZYBIOS_FIELD_STATUS(t, device_type_and_status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "device_type", lazybiosType41DeviceTypeStr(t->device_type_and_status));
            cJSON_AddStringToObject(obj, "status", lazybiosType41DeviceStatusStr(t->device_type_and_status));
        } else if (LAZYBIOS_FIELD_STATUS(t, device_type_and_status) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "device_type", LAZYBIOS_JSON_UNREACHABLE);
            cJSON_AddStringToObject(obj, "status", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "device_type");
            cJSON_AddNullToObject(obj, "status");
        }
        cjson_field_num(obj, "device_type_instance", LAZYBIOS_FIELD_STATUS(t, device_type_instance), t->device_type_instance);
        cjson_field_num(obj, "segment_group_number", LAZYBIOS_FIELD_STATUS(t, segment_group_number), t->segment_group_number);
        cjson_field_num(obj, "bus_number", LAZYBIOS_FIELD_STATUS(t, bus_number), t->bus_number);
        cjson_field_num(obj, "device_function_number", LAZYBIOS_FIELD_STATUS(t, device_function_number), t->device_function_number);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type41", arr);
}

void lazybiosExtJSONAddType42(const lazybiosType42_t* type42, size_t count, cJSON* root) {
    if (!root) return;
    if (!type42 || !count) {
        cJSON_AddStringToObject(root, "Type42", "Failed to get Management Controller Host Interface information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType42_t* t = &type42[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        if (LAZYBIOS_FIELD_STATUS(t, interface_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "interface_type", lazybiosType42InterfaceTypeStr(t->interface_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, interface_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "interface_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "interface_type");
        }
        if (LAZYBIOS_FIELD_STATUS(t, interface_type_specific_data) == LAZYBIOS_FIELD_PRESENT && t->interface_type_specific_data) {
            cJSON* data_arr = cJSON_CreateArray();
            for (size_t j = 0; j < t->interface_type_specific_data_size; j++) {
                cJSON_AddItemToArray(data_arr, cJSON_CreateNumber(t->interface_type_specific_data[j]));
            }
            cJSON_AddItemToObject(obj, "interface_type_specific_data", data_arr);
        }
        if (LAZYBIOS_FIELD_STATUS(t, protocol_records) == LAZYBIOS_FIELD_PRESENT && t->protocol_records) {
            cJSON* records_arr = cJSON_CreateArray();
            for (uint8_t j = 0; j < t->number_of_protocol_records; j++) {
                cJSON* rec_obj = cJSON_CreateObject();
                cJSON_AddStringToObject(rec_obj, "protocol_type", lazybiosType42ProtocolTypeStr(t->protocol_records[j].protocol_type));
                if (t->protocol_records[j].protocol_type_specific_data && t->protocol_records[j].protocol_type_specific_data_length > 0) {
                    cJSON* pdata_arr = cJSON_CreateArray();
                    for (uint8_t k = 0; k < t->protocol_records[j].protocol_type_specific_data_length; k++) {
                        cJSON_AddItemToArray(pdata_arr, cJSON_CreateNumber(t->protocol_records[j].protocol_type_specific_data[k]));
                    }
                    cJSON_AddItemToObject(rec_obj, "protocol_type_specific_data", pdata_arr);
                }
                cJSON_AddItemToArray(records_arr, rec_obj);
            }
            cJSON_AddItemToObject(obj, "protocol_records", records_arr);
        }
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type42", arr);
}

void lazybiosExtJSONAddType43(const lazybiosType43_t* type43, size_t count, cJSON* root) {
    if (!root) return;
    if (!type43 || !count) {
        cJSON_AddStringToObject(root, "Type43", "Failed to get TPM Device information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType43_t* t = &type43[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_str(obj, "vendor_id", LAZYBIOS_FIELD_STATUS(t, vendor_id), (char*)t->vendor_id);
        cjson_field_num(obj, "major_spec_version", LAZYBIOS_FIELD_STATUS(t, major_spec_version), t->major_spec_version);
        cjson_field_num(obj, "minor_spec_version", LAZYBIOS_FIELD_STATUS(t, minor_spec_version), t->minor_spec_version);
        cjson_field_num(obj, "firmware_version_1", LAZYBIOS_FIELD_STATUS(t, firmware_version_1), t->firmware_version_1);
        cjson_field_num(obj, "firmware_version_2", LAZYBIOS_FIELD_STATUS(t, firmware_version_2), t->firmware_version_2);
        cjson_field_str(obj, "description", LAZYBIOS_FIELD_STATUS(t, description), t->description);
        cjson_field_num(obj, "characteristics", LAZYBIOS_FIELD_STATUS(t, characteristics), (double)t->characteristics);
        cjson_field_num(obj, "oem_defined", LAZYBIOS_FIELD_STATUS(t, oem_defined), t->oem_defined);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type43", arr);
}

void lazybiosExtJSONAddType44(const lazybiosType44_t* type44, size_t count, cJSON* root) {
    if (!root) return;
    if (!type44 || !count) {
        cJSON_AddStringToObject(root, "Type44", "Failed to get Processor Additional Information information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType44_t* t = &type44[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_num(obj, "referenced_handle", LAZYBIOS_FIELD_STATUS(t, referenced_handle), t->referenced_handle);
        if (LAZYBIOS_FIELD_STATUS(t, processor_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "processor_type", lazybiosType44ProcessorTypeStr(t->processor_type));
        } else if (LAZYBIOS_FIELD_STATUS(t, processor_type) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "processor_type", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "processor_type");
        }
        if (LAZYBIOS_FIELD_STATUS(t, processor_specific_data) == LAZYBIOS_FIELD_PRESENT && t->processor_specific_data) {
            cJSON* data_arr = cJSON_CreateArray();
            for (uint8_t j = 0; j < t->block_length; j++) {
                cJSON_AddItemToArray(data_arr, cJSON_CreateNumber(t->processor_specific_data[j]));
            }
            cJSON_AddItemToObject(obj, "processor_specific_data", data_arr);
        }
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type44", arr);
}

void lazybiosExtJSONAddType45(const lazybiosType45_t* type45, size_t count, cJSON* root) {
    if (!root) return;
    if (!type45 || !count) {
        cJSON_AddStringToObject(root, "Type45", "Failed to get Firmware Inventory Information information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType45_t* t = &type45[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_field_str(obj, "firmware_component_name", LAZYBIOS_FIELD_STATUS(t, firmware_component_name), t->firmware_component_name);
        cjson_field_str(obj, "firmware_version", LAZYBIOS_FIELD_STATUS(t, firmware_version), t->firmware_version);
        if (LAZYBIOS_FIELD_STATUS(t, version_format) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "version_format", lazybiosType45VersionFormatStr(t->version_format));
        } else if (LAZYBIOS_FIELD_STATUS(t, version_format) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "version_format", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "version_format");
        }
        cjson_field_str(obj, "firmware_id", LAZYBIOS_FIELD_STATUS(t, firmware_id), t->firmware_id);
        if (LAZYBIOS_FIELD_STATUS(t, firmware_id_format) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "firmware_id_format", lazybiosType45FirmwareIDFormatStr(t->firmware_id_format));
        } else if (LAZYBIOS_FIELD_STATUS(t, firmware_id_format) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "firmware_id_format", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "firmware_id_format");
        }
        cjson_field_str(obj, "release_date", LAZYBIOS_FIELD_STATUS(t, release_date), t->release_date);
        cjson_field_str(obj, "manufacturer", LAZYBIOS_FIELD_STATUS(t, manufacturer), t->manufacturer);
        cjson_field_str(obj, "lowest_supported_firmware_version", LAZYBIOS_FIELD_STATUS(t, lowest_supported_firmware_version), t->lowest_supported_firmware_version);
        cjson_field_num(obj, "image_size", LAZYBIOS_FIELD_STATUS(t, image_size), (double)t->image_size);
        cjson_field_num(obj, "characteristics", LAZYBIOS_FIELD_STATUS(t, characteristics), t->characteristics);
        if (LAZYBIOS_FIELD_STATUS(t, state) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "state", lazybiosType45StateStr(t->state));
        } else if (LAZYBIOS_FIELD_STATUS(t, state) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "state", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "state");
        }
        if (LAZYBIOS_FIELD_STATUS(t, associated_component_handles) == LAZYBIOS_FIELD_PRESENT && t->associated_component_handles) {
            cJSON* handles_arr = cJSON_CreateArray();
            for (uint8_t j = 0; j < t->number_of_associated_components; j++) {
                cJSON_AddItemToArray(handles_arr, cJSON_CreateNumber(t->associated_component_handles[j]));
            }
            cJSON_AddItemToObject(obj, "associated_component_handles", handles_arr);
        }
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type45", arr);
}

void lazybiosExtJSONAddType46(const lazybiosType46_t* type46, size_t count, cJSON* root) {
    if (!root) return;
    if (!type46 || !count) {
        cJSON_AddStringToObject(root, "Type46", "Failed to get String Property information");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < count; i++) {
        const lazybiosType46_t* t = &type46[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        if (LAZYBIOS_FIELD_STATUS(t, string_property_id) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(obj, "string_property_id", lazybiosType46StringPropertyIDStr(t->string_property_id));
        } else if (LAZYBIOS_FIELD_STATUS(t, string_property_id) == LAZYBIOS_FIELD_UNREACHABLE) {
            cJSON_AddStringToObject(obj, "string_property_id", LAZYBIOS_JSON_UNREACHABLE);
        } else {
            cJSON_AddNullToObject(obj, "string_property_id");
        }
        cjson_field_str(obj, "string_property_value", LAZYBIOS_FIELD_STATUS(t, string_property_value), t->string_property_value);
        cjson_field_num(obj, "parent_handle", LAZYBIOS_FIELD_STATUS(t, parent_handle), t->parent_handle);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type46", arr);
}
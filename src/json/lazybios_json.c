/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
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
    } else {
        cJSON_AddNullToObject(obj, key);
    }
}

static inline void cjson_field_num(cJSON* obj, const char* key,
                                   lazybiosFieldStatus_t status, double value) {
    if (status == LAZYBIOS_FIELD_PRESENT) {
        cJSON_AddNumberToObject(obj, key, value);
    } else {
        cJSON_AddNullToObject(obj, key);
    }
}

static inline void cjson_add_hex(cJSON* obj, const char* key,
                                 unsigned long long value, int digits) {
    char buf[32];
    snprintf(buf, sizeof(buf), "0x%0*llX", digits, value);
    cJSON_AddStringToObject(obj, key, buf);
}

static inline void cjson_field_hex(cJSON* obj, const char* key,
                                   lazybiosFieldStatus_t status,
                                   unsigned long long value, int digits) {
    if (status == LAZYBIOS_FIELD_PRESENT) {
        cjson_add_hex(obj, key, value, digits);
    } else {
        cJSON_AddNullToObject(obj, key);
    }
}

/*
 * OEM structure numbers are only meaningful together with their vendor, so the
 * serialised form nests them the same way the context does: oem.<vendor>.TypeN.
 */
/*
 * Decoded forms live in a nested "decoded" object so the top-level members stay
 * the raw encodings the firmware reported.
 */
static cJSON* record_decoded(cJSON* obj) {
    cJSON* d = cJSON_GetObjectItemCaseSensitive(obj, "decoded");
    if (!d) {
        d = cJSON_CreateObject();
        if (!d) return NULL;
        cJSON_AddItemToObject(obj, "decoded", d);
    }
    return d;
}

static cJSON* oem_vendor_object(cJSON* root, const char* vendor) {
    cJSON* oem = cJSON_GetObjectItemCaseSensitive(root, "oem");
    if (!oem) {
        oem = cJSON_CreateObject();
        if (!oem) return NULL;
        cJSON_AddItemToObject(root, "oem", oem);
    }
    cJSON* v = cJSON_GetObjectItemCaseSensitive(oem, vendor);
    if (!v) {
        v = cJSON_CreateObject();
        if (!v) return NULL;
        cJSON_AddItemToObject(oem, vendor, v);
    }
    return v;
}

void lazybiosExtJSONAddSMBIOSInfo(const lazybiosDMI_t* DMIData,
                                  lazybiosBackend_t backend, cJSON* root) {
    if (!DMIData || !root) {
        return;
    }

    cJSON_AddStringToObject(root, "lazybios_version", lazybiosVersion);

    cJSON* obj = cJSON_CreateObject();
    if (!obj) {
        return;
    }


    if (DMIData->entry_tag == SMBIOS_VER_3X) {
        const lazybiosSMBIOS3Entry* v3 = DMIData->entry_union.v3;

        char anchor[6];
        snprintf(anchor, sizeof(anchor), "%c%c%c%c%c",
                 v3->anchor[0], v3->anchor[1], v3->anchor[2],
                 v3->anchor[3], v3->anchor[4]);

        char addr[20];
        snprintf(addr, sizeof(addr), "0x%lX", (unsigned long)v3->structure_table_address);

        char epver[16];
        snprintf(epver, sizeof(epver), "%u.%u.%u",
                 v3->major_version, v3->minor_version, v3->docrev);
        cJSON_AddStringToObject(obj, "entry_point_version",      epver);
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

        char epver[16];
        snprintf(epver, sizeof(epver), "%u.%u", v2->major_version, v2->minor_version);
        cJSON_AddStringToObject(obj, "entry_point_version",    epver);
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

void lazybiosExtJSONAddType0(const lazybiosType0Array_t* type0, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type0) {
        cJSON_AddNullToObject(root, "Type0");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type0->count; i++) {
        const lazybiosType0_t* t = &type0->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "extended_rom_size", LAZYBIOS_FIELD_STATUS(t, extended_rom_size), t->extended_rom_size);

        cjson_add_str(obj, "vendor",       t->vendor);
        cjson_add_str(obj, "version",      t->version);
        cjson_add_str(obj, "release_date", t->release_date);

        if (LAZYBIOS_FIELD_STATUS(t, bios_starting_segment) == LAZYBIOS_FIELD_PRESENT) {
            char buf[16];
            snprintf(buf, sizeof(buf), "0x%04hX", t->bios_starting_segment);
            cJSON_AddStringToObject(obj, "bios_starting_segment", buf);
                } else {
            cJSON_AddNullToObject(obj, "bios_starting_segment");
        }

        if (LAZYBIOS_FIELD_STATUS(t, characteristics) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "characteristics", t->decoded.characteristics);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "characteristics");
        }

        if (t->firmware_char_ext_bytes && t->firmware_char_ext_bytes_count >= 1) {
            cJSON_AddStringToObject(record_decoded(obj), "characteristics_ext_byte1", t->decoded.characteristics_ext_byte1);
        } else {
            cjson_field_str(obj, "characteristics_ext_byte1",
                            LAZYBIOS_FIELD_STATUS(t, firmware_char_ext_bytes), NULL);
        }

        if (t->firmware_char_ext_bytes && t->firmware_char_ext_bytes_count >= 2) {
            cJSON_AddStringToObject(record_decoded(obj), "characteristics_ext_byte2", t->decoded.characteristics_ext_byte2);
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
            char buf[64];
            snprintf(buf, sizeof(buf), "%hu %s",
                     t->decoded.extended_rom_size, t->decoded.extended_rom_size_unit);
            cJSON_AddStringToObject(obj, "rom_size", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, rom_size) == LAZYBIOS_FIELD_PRESENT) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%u KB", t->rom_size);
            cJSON_AddStringToObject(obj, "rom_size", buf);
        } else if (LAZYBIOS_FIELD_STATUS(t, extended_rom_size) == LAZYBIOS_FIELD_UNREACHABLE &&
                   LAZYBIOS_FIELD_STATUS(t, rom_size) == LAZYBIOS_FIELD_UNREACHABLE) {
        } else {
            cJSON_AddNullToObject(obj, "rom_size");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type0", arr);
}

void lazybiosExtJSONAddType1(const lazybiosType1Array_t* type1, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type1) {
        cJSON_AddNullToObject(root, "Type1");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type1->count; i++) {
        const lazybiosType1_t* t = &type1->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "wake_up_type", LAZYBIOS_FIELD_STATUS(t, wake_up_type), t->wake_up_type);

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
                } else {
            cJSON_AddNullToObject(obj, "uuid");
        }

        if (LAZYBIOS_FIELD_STATUS(t, wake_up_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "wake_up_type",
                                    t->decoded.wake_up_type);
                } else {
            cJSON_AddNullToObject(obj, "wake_up_type");
        }

        cjson_field_str(obj, "sku_number", LAZYBIOS_FIELD_STATUS(t, sku_number), t->sku_number);
        cjson_field_str(obj, "family",     LAZYBIOS_FIELD_STATUS(t, family),     t->family);

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type1", arr);
}

void lazybiosExtJSONAddType2(const lazybiosType2Array_t* type2, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type2) {
        cJSON_AddNullToObject(root, "Type2");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type2->count; i++) {
        const lazybiosType2_t* t = &type2->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "board_type", LAZYBIOS_FIELD_STATUS(t, board_type), t->board_type);

        cjson_field_str(obj, "manufacturer",  LAZYBIOS_FIELD_STATUS(t, manufacturer),  t->manufacturer);
        cjson_field_str(obj, "product",       LAZYBIOS_FIELD_STATUS(t, product),       t->product);
        cjson_field_str(obj, "version",       LAZYBIOS_FIELD_STATUS(t, version),       t->version);
        cjson_field_str(obj, "serial_number", LAZYBIOS_FIELD_STATUS(t, serial_number), t->serial_number);
        cjson_field_str(obj, "asset_tag",     LAZYBIOS_FIELD_STATUS(t, asset_tag),     t->asset_tag);

        if (LAZYBIOS_FIELD_STATUS(t, feature_flags) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "feature_flags", t->decoded.feature_flags);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "feature_flags");
        }

        cjson_field_str(obj, "location_in_chassis",
                        LAZYBIOS_FIELD_STATUS(t, location_in_chassis), t->location_in_chassis);

        if (LAZYBIOS_FIELD_STATUS(t, chassis_handle) == LAZYBIOS_FIELD_PRESENT) {
            char buf[10];
            snprintf(buf, sizeof(buf), "0x%04hX", t->chassis_handle);
            cJSON_AddStringToObject(obj, "chassis_handle", buf);
                } else {
            cJSON_AddNullToObject(obj, "chassis_handle");
        }

        if (LAZYBIOS_FIELD_STATUS(t, board_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "board_type", t->decoded.board_type);
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
                } else {
            cJSON_AddNullToObject(obj, "contained_object_handles");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type2", arr);
}

void lazybiosExtJSONAddType3(const lazybiosType3Array_t* type3, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type3) {
        cJSON_AddNullToObject(root, "Type3");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type3->count; i++) {
        const lazybiosType3_t* t = &type3->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "boot_up_state", LAZYBIOS_FIELD_STATUS(t, boot_up_state), t->boot_up_state);
        cjson_field_num(obj, "power_supply_state", LAZYBIOS_FIELD_STATUS(t, power_supply_state), t->power_supply_state);
        cjson_field_num(obj, "thermal_state", LAZYBIOS_FIELD_STATUS(t, thermal_state), t->thermal_state);
        cjson_field_num(obj, "security_status", LAZYBIOS_FIELD_STATUS(t, security_status), t->security_status);

        cjson_field_str(obj, "manufacturer", LAZYBIOS_FIELD_STATUS(t, manufacturer), t->manufacturer);

        if (LAZYBIOS_FIELD_STATUS(t, type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "type", t->decoded.type);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "type");
        }

        cjson_field_str(obj, "version",       LAZYBIOS_FIELD_STATUS(t, version),       t->version);
        cjson_field_str(obj, "serial_number", LAZYBIOS_FIELD_STATUS(t, serial_number), t->serial_number);
        cjson_field_str(obj, "asset_tag",     LAZYBIOS_FIELD_STATUS(t, asset_tag),     t->asset_tag);

        if (LAZYBIOS_FIELD_STATUS(t, boot_up_state) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "boot_up_state", t->decoded.boot_up_state);
                } else {
            cJSON_AddNullToObject(obj, "boot_up_state");
        }

        if (LAZYBIOS_FIELD_STATUS(t, power_supply_state) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "power_supply_state",
                                    t->decoded.power_supply_state);
                } else {
            cJSON_AddNullToObject(obj, "power_supply_state");
        }

        if (LAZYBIOS_FIELD_STATUS(t, thermal_state) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "thermal_state", t->decoded.thermal_state);
                } else {
            cJSON_AddNullToObject(obj, "thermal_state");
        }

        if (LAZYBIOS_FIELD_STATUS(t, security_status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "security_status",
                                    t->decoded.security_status);
                } else {
            cJSON_AddNullToObject(obj, "security_status");
        }

        if (LAZYBIOS_FIELD_STATUS(t, oem_defined) == LAZYBIOS_FIELD_PRESENT) {
            char buf[12];
            snprintf(buf, sizeof(buf), "0x%08X", t->oem_defined);
            cJSON_AddStringToObject(obj, "oem_defined", buf);
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
                    cJSON_AddStringToObject(elem, "type",
                        t->decoded.contained_elements ? t->decoded.contained_elements[e] : "Unknown");
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

void lazybiosExtJSONAddType4(const lazybiosType4Array_t* type4, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type4) {
        cJSON_AddNullToObject(root, "Type4");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type4->count; i++) {
        const lazybiosType4_t* t = &type4->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "processor_family", LAZYBIOS_FIELD_STATUS(t, processor_family), t->processor_family);
        cjson_field_num(obj, "processor_family_2", LAZYBIOS_FIELD_STATUS(t, processor_family_2), t->processor_family_2);
        cjson_field_num(obj, "processor_type", LAZYBIOS_FIELD_STATUS(t, processor_type), t->processor_type);
        cjson_field_num(obj, "processor_upgrade", LAZYBIOS_FIELD_STATUS(t, processor_upgrade), t->processor_upgrade);

        cjson_field_str(obj, "socket_designation",
                        LAZYBIOS_FIELD_STATUS(t, socket_designation), t->socket_designation);

        if (LAZYBIOS_FIELD_STATUS(t, processor_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "processor_type",
                                    t->decoded.processor_type);
                } else {
            cJSON_AddNullToObject(obj, "processor_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, processor_family_2) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "processor_family_2",
                                    t->decoded.processor_family_2);
        } else if (LAZYBIOS_FIELD_STATUS(t, processor_family) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "processor_family",
                                    t->decoded.processor_family);
        } else if (LAZYBIOS_FIELD_STATUS(t, processor_family_2) == LAZYBIOS_FIELD_UNREACHABLE ||
                   LAZYBIOS_FIELD_STATUS(t, processor_family) == LAZYBIOS_FIELD_UNREACHABLE) {
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
                } else {
            cJSON_AddNullToObject(obj, "processor_id");
        }

        cjson_field_str(obj, "processor_version",
                        LAZYBIOS_FIELD_STATUS(t, processor_version), t->processor_version);

        if (LAZYBIOS_FIELD_STATUS(t, voltage) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "voltage", t->decoded.voltage);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "voltage");
        }

        cjson_field_num(obj, "external_clock_mhz",
                        LAZYBIOS_FIELD_STATUS(t, external_clock), t->external_clock);
        cjson_field_num(obj, "max_speed_mhz",
                        LAZYBIOS_FIELD_STATUS(t, max_speed), t->max_speed);
        cjson_field_num(obj, "current_speed_mhz",
                        LAZYBIOS_FIELD_STATUS(t, current_speed), t->current_speed);

        if (LAZYBIOS_FIELD_STATUS(t, status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "status", t->decoded.status);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "status");
        }

        if (LAZYBIOS_FIELD_STATUS(t, processor_upgrade) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "processor_upgrade",
                                    t->decoded.processor_upgrade);
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
        } else {
            cJSON_AddNullToObject(obj, "core_count");
        }

        if (LAZYBIOS_FIELD_STATUS(t, core_enabled_2) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "core_enabled", t->core_enabled_2);
        } else if (LAZYBIOS_FIELD_STATUS(t, core_enabled) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "core_enabled", t->core_enabled);
        } else if (LAZYBIOS_FIELD_STATUS(t, core_enabled_2) == LAZYBIOS_FIELD_UNREACHABLE ||
                   LAZYBIOS_FIELD_STATUS(t, core_enabled) == LAZYBIOS_FIELD_UNREACHABLE) {
        } else {
            cJSON_AddNullToObject(obj, "core_enabled");
        }

        if (LAZYBIOS_FIELD_STATUS(t, thread_count_2) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "thread_count", t->thread_count_2);
        } else if (LAZYBIOS_FIELD_STATUS(t, thread_count) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "thread_count", t->thread_count);
        } else if (LAZYBIOS_FIELD_STATUS(t, thread_count_2) == LAZYBIOS_FIELD_UNREACHABLE ||
                   LAZYBIOS_FIELD_STATUS(t, thread_count) == LAZYBIOS_FIELD_UNREACHABLE) {
        } else {
            cJSON_AddNullToObject(obj, "thread_count");
        }

        cjson_field_num(obj, "thread_enabled",
                        LAZYBIOS_FIELD_STATUS(t, thread_enabled), t->thread_enabled);

        if (LAZYBIOS_FIELD_STATUS(t, processor_characteristics) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "processor_characteristics", t->decoded.processor_characteristics);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "processor_characteristics");
        }

        cjson_field_str(obj, "socket_type", LAZYBIOS_FIELD_STATUS(t, socket_type), t->socket_type);

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type4", arr);
}

void lazybiosExtJSONAddType5(const lazybiosType5Array_t* type5, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type5) {
        cJSON_AddNullToObject(root, "Type5");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type5->count; i++) {
        const lazybiosType5_t* t = &type5->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "current_interleave", LAZYBIOS_FIELD_STATUS(t, current_interleave), t->current_interleave);
        cjson_field_num(obj, "error_detecting_method", LAZYBIOS_FIELD_STATUS(t, error_detecting_method), t->error_detecting_method);
        cjson_field_num(obj, "supported_interleave", LAZYBIOS_FIELD_STATUS(t, supported_interleave), t->supported_interleave);

        if (LAZYBIOS_FIELD_STATUS(t, error_detecting_method) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "error_detecting_method",
                                    t->decoded.error_detecting_method);
                } else {
            cJSON_AddNullToObject(obj, "error_detecting_method");
        }

        if (LAZYBIOS_FIELD_STATUS(t, error_correcting_capability) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "error_correcting_capability", t->decoded.error_correcting_capability);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "error_correcting_capability");
        }

        if (LAZYBIOS_FIELD_STATUS(t, supported_interleave) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "supported_interleave",
                                    t->decoded.supported_interleave);
                } else {
            cJSON_AddNullToObject(obj, "supported_interleave");
        }

        if (LAZYBIOS_FIELD_STATUS(t, current_interleave) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "current_interleave",
                                    t->decoded.current_interleave);
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
                } else {
            cJSON_AddNullToObject(obj, "maximum_memory_module_size");
        }

        if (LAZYBIOS_FIELD_STATUS(t, supported_speeds) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "supported_speeds", t->decoded.supported_speeds);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "supported_speeds");
        }

        if (LAZYBIOS_FIELD_STATUS(t, supported_memory_types) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "supported_memory_types", t->decoded.supported_memory_types);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "supported_memory_types");
        }

        if (LAZYBIOS_FIELD_STATUS(t, memory_module_voltage) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "memory_module_voltage", t->decoded.memory_module_voltage);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "memory_module_voltage");
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
        } else if (LAZYBIOS_FIELD_STATUS(t, memory_module_configuration_handles) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "memory_module_configuration_handles", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "memory_module_configuration_handles");
        }

        if (LAZYBIOS_FIELD_STATUS(t, enabled_error_correcting_capabilities) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "error_correcting_capability", t->decoded.error_correcting_capability);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "enabled_error_correcting_capabilities");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type5", arr);
}

void lazybiosExtJSONAddType6(const lazybiosType6Array_t* type6, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type6) {
        cJSON_AddNullToObject(root, "Type6");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type6->count; i++) {
        const lazybiosType6_t* t = &type6->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);

        cjson_field_str(obj, "socket_designation",
                        LAZYBIOS_FIELD_STATUS(t, socket_designation), t->socket_designation);

        if (LAZYBIOS_FIELD_STATUS(t, bank_connections) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "bank_connections", t->decoded.bank_connections);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "bank_connections");
        }

        if (LAZYBIOS_FIELD_STATUS(t, current_speed) == LAZYBIOS_FIELD_PRESENT) {
            if (t->current_speed == 0) {
                cJSON_AddStringToObject(obj, "current_speed", "Unknown");
            } else {
                char buf[24];
                snprintf(buf, sizeof(buf), "%hhu ns", t->current_speed);
                cJSON_AddStringToObject(obj, "current_speed", buf);
            }
                } else {
            cJSON_AddNullToObject(obj, "current_speed");
        }

        if (LAZYBIOS_FIELD_STATUS(t, current_memory_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "current_memory_type", t->decoded.current_memory_type);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "current_memory_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, installed_size) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "installed_size", t->decoded.installed_size);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "installed_size");
        }

        if (LAZYBIOS_FIELD_STATUS(t, enabled_size) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "enabled_size", t->decoded.enabled_size);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "enabled_size");
        }

        if (LAZYBIOS_FIELD_STATUS(t, error_status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "error_status", t->decoded.error_status);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "error_status");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type6", arr);
}

void lazybiosExtJSONAddType7(const lazybiosType7Array_t* type7, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type7) {
        cJSON_AddNullToObject(root, "Type7");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type7->count; i++) {
        const lazybiosType7_t* t = &type7->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "associativity", LAZYBIOS_FIELD_STATUS(t, associativity), t->associativity);
        cjson_field_num(obj, "error_correction_type", LAZYBIOS_FIELD_STATUS(t, error_correction_type), t->error_correction_type);
        cjson_field_num(obj, "installed_cache_size_2", LAZYBIOS_FIELD_STATUS(t, installed_cache_size_2), t->installed_cache_size_2);
        cjson_field_num(obj, "installed_size", LAZYBIOS_FIELD_STATUS(t, installed_size), t->installed_size);
        cjson_field_num(obj, "maximum_cache_size", LAZYBIOS_FIELD_STATUS(t, maximum_cache_size), t->maximum_cache_size);
        cjson_field_num(obj, "maximum_cache_size_2", LAZYBIOS_FIELD_STATUS(t, maximum_cache_size_2), t->maximum_cache_size_2);
        cjson_field_num(obj, "system_cache_type", LAZYBIOS_FIELD_STATUS(t, system_cache_type), t->system_cache_type);

        cjson_field_str(obj, "socket_designation",
                        LAZYBIOS_FIELD_STATUS(t, socket_designation), t->socket_designation);

        if (LAZYBIOS_FIELD_STATUS(t, cache_configuration) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "cache_configuration", t->decoded.cache_configuration);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "cache_configuration");
        }

        if (LAZYBIOS_FIELD_STATUS(t, maximum_cache_size_2) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t size = t->decoded.maximum_cache_size_2;
            cJSON_AddNumberToObject(obj, "maximum_cache_size_kb", (double)size);
        } else if (LAZYBIOS_FIELD_STATUS(t, maximum_cache_size) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t size = t->decoded.maximum_cache_size;
            cJSON_AddNumberToObject(obj, "maximum_cache_size_kb", (double)size);
        } else if (LAZYBIOS_FIELD_STATUS(t, maximum_cache_size_2) == LAZYBIOS_FIELD_UNREACHABLE &&
                   LAZYBIOS_FIELD_STATUS(t, maximum_cache_size) == LAZYBIOS_FIELD_UNREACHABLE) {
        } else {
            cJSON_AddNullToObject(obj, "maximum_cache_size_kb");
        }

        if (LAZYBIOS_FIELD_STATUS(t, installed_cache_size_2) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t size = t->decoded.installed_cache_size_2;
            cJSON_AddNumberToObject(obj, "installed_cache_size_kb", (double)size);
        } else if (LAZYBIOS_FIELD_STATUS(t, installed_size) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t size = t->decoded.installed_size;
            cJSON_AddNumberToObject(obj, "installed_cache_size_kb", (double)size);
        } else if (LAZYBIOS_FIELD_STATUS(t, installed_cache_size_2) == LAZYBIOS_FIELD_UNREACHABLE &&
                   LAZYBIOS_FIELD_STATUS(t, installed_size) == LAZYBIOS_FIELD_UNREACHABLE) {
        } else {
            cJSON_AddNullToObject(obj, "installed_cache_size_kb");
        }

        if (LAZYBIOS_FIELD_STATUS(t, supported_sram_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "supported_sram_type", t->decoded.supported_sram_type);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "supported_sram_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, current_sram_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "supported_sram_type", t->decoded.supported_sram_type);
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
                } else {
            cJSON_AddNullToObject(obj, "cache_speed");
        }

        if (LAZYBIOS_FIELD_STATUS(t, error_correction_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "error_correction_type",
                                    t->decoded.error_correction_type);
                } else {
            cJSON_AddNullToObject(obj, "error_correction_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, system_cache_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "system_cache_type",
                                    t->decoded.system_cache_type);
                } else {
            cJSON_AddNullToObject(obj, "system_cache_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, associativity) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "associativity",
                                    t->decoded.associativity);
                } else {
            cJSON_AddNullToObject(obj, "associativity");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type7", arr);
}

void lazybiosExtJSONAddType8(const lazybiosType8Array_t* type8, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type8) {
        cJSON_AddNullToObject(root, "Type8");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type8->count; i++) {
        const lazybiosType8_t* t = &type8->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "external_connector_type", LAZYBIOS_FIELD_STATUS(t, external_connector_type), t->external_connector_type);
        cjson_field_num(obj, "internal_connector_type", LAZYBIOS_FIELD_STATUS(t, internal_connector_type), t->internal_connector_type);
        cjson_field_num(obj, "port_type", LAZYBIOS_FIELD_STATUS(t, port_type), t->port_type);

        cjson_field_str(obj, "internal_reference_designator",
                        LAZYBIOS_FIELD_STATUS(t, internal_reference_designator),
                        t->internal_reference_designator);

        if (LAZYBIOS_FIELD_STATUS(t, internal_connector_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "internal_connector_type",
                                    t->decoded.internal_connector_type);
                } else {
            cJSON_AddNullToObject(obj, "internal_connector_type");
        }

        cjson_field_str(obj, "external_reference_designator",
                        LAZYBIOS_FIELD_STATUS(t, external_reference_designator),
                        t->external_reference_designator);

        if (LAZYBIOS_FIELD_STATUS(t, external_connector_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "external_connector_type",
                                    t->decoded.external_connector_type);
                } else {
            cJSON_AddNullToObject(obj, "external_connector_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, port_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "port_type",
                                    t->decoded.port_type);
                } else {
            cJSON_AddNullToObject(obj, "port_type");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type8", arr);
}

void lazybiosExtJSONAddType9(const lazybiosType9Array_t* type9, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type9) {
        cJSON_AddNullToObject(root, "Type9");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type9->count; i++) {
        const lazybiosType9_t* t = &type9->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "current_usage", LAZYBIOS_FIELD_STATUS(t, current_usage), t->current_usage);
        cjson_field_num(obj, "data_bus_width", LAZYBIOS_FIELD_STATUS(t, data_bus_width), t->data_bus_width);
        cjson_field_num(obj, "slot_data_bus_width", LAZYBIOS_FIELD_STATUS(t, slot_data_bus_width), t->slot_data_bus_width);
        cjson_field_num(obj, "slot_height", LAZYBIOS_FIELD_STATUS(t, slot_height), t->slot_height);
        cjson_field_num(obj, "slot_length", LAZYBIOS_FIELD_STATUS(t, slot_length), t->slot_length);
        cjson_field_num(obj, "slot_physical_width", LAZYBIOS_FIELD_STATUS(t, slot_physical_width), t->slot_physical_width);
        cjson_field_num(obj, "slot_type", LAZYBIOS_FIELD_STATUS(t, slot_type), t->slot_type);

        cjson_field_str(obj, "slot_designation",
                        LAZYBIOS_FIELD_STATUS(t, slot_designation), t->slot_designation);

        if (LAZYBIOS_FIELD_STATUS(t, slot_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "slot_type",
                                    t->decoded.slot_type);
                } else {
            cJSON_AddNullToObject(obj, "slot_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, slot_data_bus_width) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "slot_data_bus_width",
                                    t->decoded.slot_data_bus_width);
                } else {
            cJSON_AddNullToObject(obj, "slot_data_bus_width");
        }

        if (LAZYBIOS_FIELD_STATUS(t, current_usage) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "current_usage",
                                    t->decoded.current_usage);
                } else {
            cJSON_AddNullToObject(obj, "current_usage");
        }

        if (LAZYBIOS_FIELD_STATUS(t, slot_length) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "slot_length",
                                    t->decoded.slot_length);
                } else {
            cJSON_AddNullToObject(obj, "slot_length");
        }

        cjson_field_num(obj, "slot_id", LAZYBIOS_FIELD_STATUS(t, slot_id), t->slot_id);

        if (LAZYBIOS_FIELD_STATUS(t, slot_characteristics_1) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "slot_characteristics_1", t->decoded.slot_characteristics_1);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "slot_characteristics_1");
        }

        if (LAZYBIOS_FIELD_STATUS(t, slot_characteristics_2) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "slot_characteristics_2", t->decoded.slot_characteristics_2);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "slot_characteristics_2");
        }

        cjson_field_num(obj, "segment_group_number",
                        LAZYBIOS_FIELD_STATUS(t, segment_group_number), t->segment_group_number);
        cjson_field_num(obj, "bus_number",
                        LAZYBIOS_FIELD_STATUS(t, bus_number), t->bus_number);

        if (LAZYBIOS_FIELD_STATUS(t, device_function_number) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "device_function_number", t->decoded.device_function_number);
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
                        cJSON_AddStringToObject(g, "device_function", t->decoded.device_function_number);
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
        } else if (LAZYBIOS_FIELD_STATUS(t, peer_groups) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "peer_groups", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "peer_groups");
        }

        if (LAZYBIOS_FIELD_STATUS(t, slot_physical_width) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "slot_physical_width",
                                    t->decoded.slot_physical_width);
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
                } else {
            cJSON_AddNullToObject(obj, "slot_pitch");
        }

        if (LAZYBIOS_FIELD_STATUS(t, slot_height) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "slot_height",
                                    t->decoded.slot_height);
                } else {
            cJSON_AddNullToObject(obj, "slot_height");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type9", arr);
}

void lazybiosExtJSONAddType10(const lazybiosType10Array_t* type10, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type10) {
        cJSON_AddNullToObject(root, "Type10");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type10->count; i++) {
        const lazybiosType10_t* t = &type10->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);

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
                        cJSON_AddStringToObject(dev, "type", t->devices[j].decoded.device_type);
                        cJSON_AddStringToObject(dev, "status", t->devices[j].decoded.device_status);
                    } else if (LAZYBIOS_FIELD_STATUS(&t->devices[j], device_type_and_status) == LAZYBIOS_FIELD_UNREACHABLE) {
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
        } else if (LAZYBIOS_FIELD_STATUS(t, devices) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "devices", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "devices");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type10", arr);
}

void lazybiosExtJSONAddType11(const lazybiosType11Array_t* type11, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type11) {
        cJSON_AddNullToObject(root, "Type11");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type11->count; i++) {
        const lazybiosType11_t* t = &type11->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);

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
        } else if (LAZYBIOS_FIELD_STATUS(t, strings) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "strings", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "strings");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type11", arr);
}

void lazybiosExtJSONAddType12(const lazybiosType12Array_t* type12, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type12) {
        cJSON_AddNullToObject(root, "Type12");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type12->count; i++) {
        const lazybiosType12_t* t = &type12->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);

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
        } else if (LAZYBIOS_FIELD_STATUS(t, options) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "options", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "options");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type12", arr);
}

void lazybiosExtJSONAddType13(const lazybiosType13Array_t* type13, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type13) {
        cJSON_AddNullToObject(root, "Type13");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type13->count; i++) {
        const lazybiosType13_t* t = &type13->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "flags", LAZYBIOS_FIELD_STATUS(t, flags), t->flags);

        if (LAZYBIOS_FIELD_STATUS(t, flags) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "flags", t->decoded.flags);
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

void lazybiosExtJSONAddType14(const lazybiosType14Array_t* type14, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type14) {
        cJSON_AddNullToObject(root, "Type14");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type14->count; i++) {
        const lazybiosType14_t* t = &type14->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);

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
        } else if (LAZYBIOS_FIELD_STATUS(t, items) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "items", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "items");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type14", arr);
}

void lazybiosExtJSONAddType15(const lazybiosType15Array_t* type15, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type15) {
        cJSON_AddNullToObject(root, "Type15");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type15->count; i++) {
        const lazybiosType15_t* t = &type15->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "access_method", LAZYBIOS_FIELD_STATUS(t, access_method), t->access_method);
        cjson_field_num(obj, "log_header_format", LAZYBIOS_FIELD_STATUS(t, log_header_format), t->log_header_format);

        cjson_field_num(obj, "log_area_length", LAZYBIOS_FIELD_STATUS(t, log_area_length), t->log_area_length);
        cjson_field_num(obj, "log_header_start_offset", LAZYBIOS_FIELD_STATUS(t, log_header_start_offset), t->log_header_start_offset);
        cjson_field_num(obj, "log_data_start_offset", LAZYBIOS_FIELD_STATUS(t, log_data_start_offset), t->log_data_start_offset);

        if (LAZYBIOS_FIELD_STATUS(t, access_method) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "access_method", t->decoded.access_method);
                } else {
            cJSON_AddNullToObject(obj, "access_method");
        }

        if (LAZYBIOS_FIELD_STATUS(t, log_status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "log_status", t->decoded.log_status);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "log_status");
        }

        cjson_field_num(obj, "log_change_token", LAZYBIOS_FIELD_STATUS(t, log_change_token), t->log_change_token);

        if (LAZYBIOS_FIELD_STATUS(t, access_method_address) == LAZYBIOS_FIELD_PRESENT) {
            cJSON* addr_obj = cJSON_CreateObject();
            if (addr_obj) {
                if (LAZYBIOS_FIELD_STATUS(t, access_method) == LAZYBIOS_FIELD_PRESENT && t->access_method <= 0x02) {
                    cJSON_AddNumberToObject(record_decoded(addr_obj), "index_address", t->decoded.index_address);
                    cJSON_AddNumberToObject(record_decoded(addr_obj), "data_address", t->decoded.data_address);
                } else if (LAZYBIOS_FIELD_STATUS(t, access_method) == LAZYBIOS_FIELD_PRESENT && t->access_method == 0x03) {
                    cJSON_AddNumberToObject(addr_obj, "physical_address", t->access_method_address);
                } else if (LAZYBIOS_FIELD_STATUS(t, access_method) == LAZYBIOS_FIELD_PRESENT && t->access_method == 0x04) {
                    cJSON_AddNumberToObject(record_decoded(addr_obj), "gpnv_handle", t->decoded.gpnv_handle);
                } else {
                    cJSON_AddNumberToObject(addr_obj, "address", t->access_method_address);
                }
                cJSON_AddItemToObject(obj, "access_method_address", addr_obj);
            }
                } else {
            cJSON_AddNullToObject(obj, "access_method_address");
        }

        if (LAZYBIOS_FIELD_STATUS(t, log_header_format) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "log_header_format", t->decoded.log_header_format);
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
                        cJSON_AddStringToObject(desc, "log_type", t->supported_log_type_descriptors[j].decoded.log_type);
                                        } else {
                        cJSON_AddNullToObject(desc, "log_type");
                    }

                    if (LAZYBIOS_FIELD_STATUS(&t->supported_log_type_descriptors[j], variable_data_format_type) == LAZYBIOS_FIELD_PRESENT) {
                        cJSON_AddStringToObject(desc, "variable_data_format", t->supported_log_type_descriptors[j].decoded.variable_data_format_type);
                                        } else {
                        cJSON_AddNullToObject(desc, "variable_data_format");
                    }

                    cJSON_AddItemToArray(descriptors, desc);
                }
                cJSON_AddItemToObject(obj, "supported_log_type_descriptors", descriptors);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, supported_log_type_descriptors) == LAZYBIOS_FIELD_UNREACHABLE) {
        } else if (LAZYBIOS_FIELD_STATUS(t, supported_log_type_descriptors) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "supported_log_type_descriptors", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "supported_log_type_descriptors");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type15", arr);
}

void lazybiosExtJSONAddType16(const lazybiosType16Array_t* type16, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type16) {
        cJSON_AddNullToObject(root, "Type16");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type16->count; i++) {
        const lazybiosType16_t* t = &type16->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "location", LAZYBIOS_FIELD_STATUS(t, location), t->location);
        cjson_field_num(obj, "maximum_capacity", LAZYBIOS_FIELD_STATUS(t, maximum_capacity), t->maximum_capacity);
        cjson_field_num(obj, "memory_error_correction", LAZYBIOS_FIELD_STATUS(t, memory_error_correction), t->memory_error_correction);
        cjson_field_num(obj, "use", LAZYBIOS_FIELD_STATUS(t, use), t->use);

        if (LAZYBIOS_FIELD_STATUS(t, location) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "location", t->decoded.location);
                } else {
            cJSON_AddNullToObject(obj, "location");
        }

        if (LAZYBIOS_FIELD_STATUS(t, use) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "use", t->decoded.use);
                } else {
            cJSON_AddNullToObject(obj, "use");
        }

        if (LAZYBIOS_FIELD_STATUS(t, memory_error_correction) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "memory_error_correction", t->decoded.memory_error_correction);
                } else {
            cJSON_AddNullToObject(obj, "memory_error_correction");
        }

        if (LAZYBIOS_FIELD_STATUS(t, maximum_capacity) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t capacity = t->decoded.maximum_capacity;
            cJSON_AddNumberToObject(obj, "maximum_capacity_bytes", (double)capacity);
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
                } else {
            cJSON_AddNullToObject(obj, "memory_error_information_handle");
        }

        cjson_field_num(obj, "number_of_memory_devices", LAZYBIOS_FIELD_STATUS(t, number_of_memory_devices), t->number_of_memory_devices);

        cjson_field_num(obj, "extended_maximum_capacity", LAZYBIOS_FIELD_STATUS(t, extended_maximum_capacity), t->extended_maximum_capacity);

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type16", arr);
}

void lazybiosExtJSONAddType17(const lazybiosType17Array_t* type17, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type17) {
        cJSON_AddNullToObject(root, "Type17");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type17->count; i++) {
        const lazybiosType17_t* t = &type17->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "form_factor", LAZYBIOS_FIELD_STATUS(t, form_factor), t->form_factor);
        cjson_field_num(obj, "memory_technology", LAZYBIOS_FIELD_STATUS(t, memory_technology), t->memory_technology);
        cjson_field_num(obj, "memory_type", LAZYBIOS_FIELD_STATUS(t, memory_type), t->memory_type);

        cjson_field_num(obj, "physical_memory_array_handle", LAZYBIOS_FIELD_STATUS(t, physical_memory_array_handle), t->physical_memory_array_handle);
        cjson_field_num(obj, "memory_error_information_handle", LAZYBIOS_FIELD_STATUS(t, memory_error_information_handle), t->memory_error_information_handle);

        if (LAZYBIOS_FIELD_STATUS(t, total_width) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "total_width_bits", t->total_width == 0xFFFF ? 0 : t->total_width);
                } else {
            cJSON_AddNullToObject(obj, "total_width_bits");
        }

        if (LAZYBIOS_FIELD_STATUS(t, data_width) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "data_width_bits", t->data_width == 0xFFFF ? 0 : t->data_width);
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
                } else {
            cJSON_AddNullToObject(obj, "size");
        }

        if (LAZYBIOS_FIELD_STATUS(t, form_factor) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "form_factor", t->decoded.form_factor);
                } else {
            cJSON_AddNullToObject(obj, "form_factor");
        }

        cjson_field_num(obj, "device_set", LAZYBIOS_FIELD_STATUS(t, device_set), t->device_set);
        cjson_add_str(obj, "device_locator", t->device_locator);
        cjson_add_str(obj, "bank_locator", t->bank_locator);

        if (LAZYBIOS_FIELD_STATUS(t, memory_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "memory_type", t->decoded.memory_type);
                } else {
            cJSON_AddNullToObject(obj, "memory_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, type_detail) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "type_detail", t->decoded.type_detail);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "type_detail");
        }

        if (LAZYBIOS_FIELD_STATUS(t, speed) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "speed_mts", t->speed == 0xFFFF ? 0 : t->speed);
                } else {
            cJSON_AddNullToObject(obj, "speed_mts");
        }

        cjson_add_str(obj, "manufacturer", t->manufacturer);
        cjson_add_str(obj, "serial_number", t->serial_number);
        cjson_add_str(obj, "asset_tag", t->asset_tag);
        cjson_add_str(obj, "part_number", t->part_number);

        if (LAZYBIOS_FIELD_STATUS(t, attributes) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "ranks", t->attributes & 0x7F);
                } else {
            cJSON_AddNullToObject(obj, "ranks");
        }

        if (LAZYBIOS_FIELD_STATUS(t, extended_size) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "extended_size", t->decoded.extended_size);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "extended_size");
        }

        if (LAZYBIOS_FIELD_STATUS(t, configured_memory_speed) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddNumberToObject(obj, "configured_memory_speed_mts", t->configured_memory_speed == 0xFFFF ? 0 : t->configured_memory_speed);
                } else {
            cJSON_AddNullToObject(obj, "configured_memory_speed_mts");
        }

        cjson_field_num(obj, "minimum_voltage_mv", LAZYBIOS_FIELD_STATUS(t, minimum_voltage), t->minimum_voltage);
        cjson_field_num(obj, "maximum_voltage_mv", LAZYBIOS_FIELD_STATUS(t, maximum_voltage), t->maximum_voltage);
        cjson_field_num(obj, "configured_voltage_mv", LAZYBIOS_FIELD_STATUS(t, configured_voltage), t->configured_voltage);

        if (LAZYBIOS_FIELD_STATUS(t, memory_technology) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "memory_technology", t->decoded.memory_technology);
                } else {
            cJSON_AddNullToObject(obj, "memory_technology");
        }

        if (LAZYBIOS_FIELD_STATUS(t, memory_operating_mode_capability) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "memory_operating_mode_capability", t->decoded.memory_operating_mode_capability);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "memory_operating_mode_capability");
        }

        cjson_add_str(obj, "firmware_version", t->firmware_version);

        if (LAZYBIOS_FIELD_STATUS(t, module_manufacturer_id) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "module_manufacturer_id", t->decoded.module_manufacturer_id);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "module_manufacturer_id");
        }

        if (LAZYBIOS_FIELD_STATUS(t, module_product_id) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "module_manufacturer_id", t->decoded.module_manufacturer_id);
                } else {
            cJSON_AddNullToObject(obj, "module_product_id");
        }

        if (LAZYBIOS_FIELD_STATUS(t, memory_subsystem_controller_manufacturer_id) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "module_manufacturer_id", t->decoded.module_manufacturer_id);
                } else {
            cJSON_AddNullToObject(obj, "memory_subsystem_controller_manufacturer_id");
        }

        if (LAZYBIOS_FIELD_STATUS(t, memory_subsystem_controller_product_id) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "module_manufacturer_id", t->decoded.module_manufacturer_id);
                } else {
            cJSON_AddNullToObject(obj, "memory_subsystem_controller_product_id");
        }

        if (LAZYBIOS_FIELD_STATUS(t, non_volatile_size) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "non_volatile_size", t->decoded.non_volatile_size);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "non_volatile_size");
        }

        if (LAZYBIOS_FIELD_STATUS(t, volatile_size) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "volatile_size", t->decoded.volatile_size);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "volatile_size");
        }

        if (LAZYBIOS_FIELD_STATUS(t, cache_size) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "cache_size", t->decoded.cache_size);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "cache_size");
        }

        cjson_field_num(obj, "logical_size_bytes", LAZYBIOS_FIELD_STATUS(t, logical_size), (double)t->logical_size);

        if (LAZYBIOS_FIELD_STATUS(t, extended_speed) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "extended_speed", t->decoded.extended_speed);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "extended_speed");
        }

        if (LAZYBIOS_FIELD_STATUS(t, extended_configured_memory_speed) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "extended_speed", t->decoded.extended_speed);
                } else {
            cJSON_AddNullToObject(obj, "extended_configured_memory_speed");
        }

        if (LAZYBIOS_FIELD_STATUS(t, pmic0_manufacturer_id) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "pmic0_manufacturer_id", t->decoded.pmic0_manufacturer_id);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "pmic0_manufacturer_id");
        }

        if (LAZYBIOS_FIELD_STATUS(t, pmic0_revision_number) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "pmic0_revision_number", t->decoded.pmic0_revision_number);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "pmic0_revision_number");
        }

        if (LAZYBIOS_FIELD_STATUS(t, rcd_manufacturer_id) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "rcd_manufacturer_id", t->decoded.rcd_manufacturer_id);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "rcd_manufacturer_id");
        }

        if (LAZYBIOS_FIELD_STATUS(t, rcd_revision_number) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "rcd_revision_number", t->decoded.rcd_revision_number);
                } else {
            cJSON_AddNullToObject(record_decoded(obj), "rcd_revision_number");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type17", arr);
}

void lazybiosExtJSONAddType18(const lazybiosType18Array_t* type18, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type18) {
        cJSON_AddNullToObject(root, "Type18");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type18->count; i++) {
        const lazybiosType18_t* t = &type18->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "error_granularity", LAZYBIOS_FIELD_STATUS(t, error_granularity), t->error_granularity);
        cjson_field_num(obj, "error_operation", LAZYBIOS_FIELD_STATUS(t, error_operation), t->error_operation);
        cjson_field_num(obj, "error_type", LAZYBIOS_FIELD_STATUS(t, error_type), t->error_type);

        if (LAZYBIOS_FIELD_STATUS(t, error_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "error_type", t->decoded.error_type);
                } else {
            cJSON_AddNullToObject(obj, "error_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, error_granularity) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "error_granularity", t->decoded.error_granularity);
                } else {
            cJSON_AddNullToObject(obj, "error_granularity");
        }

        if (LAZYBIOS_FIELD_STATUS(t, error_operation) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "error_operation", t->decoded.error_operation);
                } else {
            cJSON_AddNullToObject(obj, "error_operation");
        }

        if (LAZYBIOS_FIELD_STATUS(t, vendor_syndrome) == LAZYBIOS_FIELD_PRESENT) {
            if (t->vendor_syndrome == 0) {
                cJSON_AddStringToObject(obj, "vendor_syndrome", "Unknown");
            } else {
                cJSON_AddNumberToObject(obj, "vendor_syndrome", t->vendor_syndrome);
            }
                } else {
            cJSON_AddNullToObject(obj, "vendor_syndrome");
        }

        if (LAZYBIOS_FIELD_STATUS(t, memory_array_error_address) == LAZYBIOS_FIELD_PRESENT) {
            if (t->memory_array_error_address == 0x80000000U) {
                cJSON_AddStringToObject(obj, "memory_array_error_address", "Unknown");
            } else {
                cJSON_AddNumberToObject(obj, "memory_array_error_address", t->memory_array_error_address);
            }
                } else {
            cJSON_AddNullToObject(obj, "memory_array_error_address");
        }

        if (LAZYBIOS_FIELD_STATUS(t, device_error_address) == LAZYBIOS_FIELD_PRESENT) {
            if (t->device_error_address == 0x80000000U) {
                cJSON_AddStringToObject(obj, "device_error_address", "Unknown");
            } else {
                cJSON_AddNumberToObject(obj, "device_error_address", t->device_error_address);
            }
                } else {
            cJSON_AddNullToObject(obj, "device_error_address");
        }

        if (LAZYBIOS_FIELD_STATUS(t, error_resolution) == LAZYBIOS_FIELD_PRESENT) {
            if (t->error_resolution == 0x80000000U) {
                cJSON_AddStringToObject(obj, "error_resolution", "Unknown");
            } else {
                cJSON_AddNumberToObject(obj, "error_resolution_bytes", t->error_resolution);
            }
                } else {
            cJSON_AddNullToObject(obj, "error_resolution_bytes");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type18", arr);
}

void lazybiosExtJSONAddType19(const lazybiosType19Array_t* type19, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type19) {
        cJSON_AddNullToObject(root, "Type19");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type19->count; i++) {
        const lazybiosType19_t* t = &type19->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "ending_address", LAZYBIOS_FIELD_STATUS(t, ending_address), t->ending_address);
        cjson_field_num(obj, "starting_address", LAZYBIOS_FIELD_STATUS(t, starting_address), t->starting_address);

        if (LAZYBIOS_FIELD_STATUS(t, starting_address) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t addr = t->decoded.starting_address;
            cJSON_AddNumberToObject(obj, "starting_address_bytes", (double)addr);
                } else {
            cJSON_AddNullToObject(obj, "starting_address_bytes");
        }

        if (LAZYBIOS_FIELD_STATUS(t, ending_address) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t addr = t->decoded.ending_address;
            cJSON_AddNumberToObject(obj, "ending_address_bytes", (double)addr);
                } else {
            cJSON_AddNullToObject(obj, "ending_address_bytes");
        }

        cjson_field_num(obj, "memory_array_handle", LAZYBIOS_FIELD_STATUS(t, memory_array_handle), t->memory_array_handle);
        cjson_field_num(obj, "partition_width", LAZYBIOS_FIELD_STATUS(t, partition_width), t->partition_width);

        cjson_field_num(obj, "extended_starting_address", LAZYBIOS_FIELD_STATUS(t, extended_starting_address), (double)t->extended_starting_address);

        cjson_field_num(obj, "extended_ending_address", LAZYBIOS_FIELD_STATUS(t, extended_ending_address), (double)t->extended_ending_address);

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type19", arr);
}

void lazybiosExtJSONAddType20(const lazybiosType20Array_t* type20, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type20) {
        cJSON_AddNullToObject(root, "Type20");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type20->count; i++) {
        const lazybiosType20_t* t = &type20->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "ending_address", LAZYBIOS_FIELD_STATUS(t, ending_address), t->ending_address);
        cjson_field_num(obj, "starting_address", LAZYBIOS_FIELD_STATUS(t, starting_address), t->starting_address);

        if (LAZYBIOS_FIELD_STATUS(t, starting_address) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t addr = t->decoded.starting_address;
            cJSON_AddNumberToObject(obj, "starting_address_bytes", (double)addr);
                } else {
            cJSON_AddNullToObject(obj, "starting_address_bytes");
        }

        if (LAZYBIOS_FIELD_STATUS(t, ending_address) == LAZYBIOS_FIELD_PRESENT) {
            uint64_t addr = t->decoded.ending_address;
            cJSON_AddNumberToObject(obj, "ending_address_bytes", (double)addr);
                } else {
            cJSON_AddNullToObject(obj, "ending_address_bytes");
        }

        cjson_field_num(obj, "memory_device_handle", LAZYBIOS_FIELD_STATUS(t, memory_device_handle), t->memory_device_handle);
        cjson_field_num(obj, "memory_array_mapped_address_handle", LAZYBIOS_FIELD_STATUS(t, memory_array_mapped_address_handle), t->memory_array_mapped_address_handle);

        cjson_field_num(obj, "partition_row_position", LAZYBIOS_FIELD_STATUS(t, partition_row_position), (double)t->partition_row_position);

        cjson_field_num(obj, "interleave_position", LAZYBIOS_FIELD_STATUS(t, interleave_position), (double)t->interleave_position);

        cjson_field_num(obj, "interleaved_data_depth", LAZYBIOS_FIELD_STATUS(t, interleaved_data_depth), (double)t->interleaved_data_depth);

        cjson_field_num(obj, "extended_starting_address", LAZYBIOS_FIELD_STATUS(t, extended_starting_address), (double)t->extended_starting_address);

        cjson_field_num(obj, "extended_ending_address", LAZYBIOS_FIELD_STATUS(t, extended_ending_address), (double)t->extended_ending_address);

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type20", arr);
}

void lazybiosExtJSONAddType21(const lazybiosType21Array_t* type21, cJSON* root) {
    if (!root) {
        return;
    }

    if (!type21) {
        cJSON_AddNullToObject(root, "Type21");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < type21->count; i++) {
        const lazybiosType21_t* t = &type21->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "interface", LAZYBIOS_FIELD_STATUS(t, interface), t->interface);
        cjson_field_num(obj, "pointing_device_type", LAZYBIOS_FIELD_STATUS(t, pointing_device_type), t->pointing_device_type);

        if (LAZYBIOS_FIELD_STATUS(t, pointing_device_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "pointing_device_type", t->decoded.pointing_device_type);
                } else {
            cJSON_AddNullToObject(obj, "pointing_device_type");
        }

        if (LAZYBIOS_FIELD_STATUS(t, interface) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "interface", t->decoded.interface);
                } else {
            cJSON_AddNullToObject(obj, "interface");
        }

        cjson_field_num(obj, "number_of_buttons", LAZYBIOS_FIELD_STATUS(t, number_of_buttons), t->number_of_buttons);

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(root, "Type21", arr);
}

void lazybiosExtJSONAddType22(const lazybiosType22Array_t* type22, cJSON* root) {
    if (!root) return;
    if (!type22) {
        cJSON_AddNullToObject(root, "Type22");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type22->count; i++) {
        const lazybiosType22_t* t = &type22->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "design_capacity", LAZYBIOS_FIELD_STATUS(t, design_capacity), t->design_capacity);
        cjson_field_num(obj, "device_chemistry", LAZYBIOS_FIELD_STATUS(t, device_chemistry), t->device_chemistry);
        cjson_field_str(obj, "location", LAZYBIOS_FIELD_STATUS(t, location), t->location);
        cjson_field_str(obj, "manufacturer", LAZYBIOS_FIELD_STATUS(t, manufacturer), t->manufacturer);
        cjson_field_str(obj, "manufacture_date", LAZYBIOS_FIELD_STATUS(t, manufacture_date), t->manufacture_date);
        cjson_field_str(obj, "serial_number", LAZYBIOS_FIELD_STATUS(t, serial_number), t->serial_number);
        cjson_field_str(obj, "device_name", LAZYBIOS_FIELD_STATUS(t, device_name), t->device_name);
        if (LAZYBIOS_FIELD_STATUS(t, device_chemistry) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "device_chemistry", t->decoded.device_chemistry);
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

void lazybiosExtJSONAddType23(const lazybiosType23Array_t* type23, cJSON* root) {
    if (!root) return;
    if (!type23) {
        cJSON_AddNullToObject(root, "Type23");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type23->count; i++) {
        const lazybiosType23_t* t = &type23->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "capabilities", LAZYBIOS_FIELD_STATUS(t, capabilities), t->capabilities);
        cjson_field_num(obj, "reset_count", LAZYBIOS_FIELD_STATUS(t, reset_count), t->reset_count);
        cjson_field_num(obj, "reset_limit", LAZYBIOS_FIELD_STATUS(t, reset_limit), t->reset_limit);
        cjson_field_num(obj, "timer_interval", LAZYBIOS_FIELD_STATUS(t, timer_interval), t->timer_interval);
        cjson_field_num(obj, "timeout", LAZYBIOS_FIELD_STATUS(t, timeout), t->timeout);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type23", arr);
}

void lazybiosExtJSONAddType24(const lazybiosType24Array_t* type24, cJSON* root) {
    if (!root) return;
    if (!type24) {
        cJSON_AddNullToObject(root, "Type24");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type24->count; i++) {
        const lazybiosType24_t* t = &type24->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "hardware_security_settings", LAZYBIOS_FIELD_STATUS(t, hardware_security_settings), t->hardware_security_settings);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type24", arr);
}

void lazybiosExtJSONAddType25(const lazybiosType25Array_t* type25, cJSON* root) {
    if (!root) return;
    if (!type25) {
        cJSON_AddNullToObject(root, "Type25");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type25->count; i++) {
        const lazybiosType25_t* t = &type25->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "next_scheduled_power_on_month", LAZYBIOS_FIELD_STATUS(t, next_scheduled_power_on_month), t->next_scheduled_power_on_month);
        cjson_field_num(obj, "next_scheduled_power_on_day_of_month", LAZYBIOS_FIELD_STATUS(t, next_scheduled_power_on_day), t->next_scheduled_power_on_day);
        cjson_field_num(obj, "next_scheduled_power_on_hour", LAZYBIOS_FIELD_STATUS(t, next_scheduled_power_on_hour), t->next_scheduled_power_on_hour);
        cjson_field_num(obj, "next_scheduled_power_on_minute", LAZYBIOS_FIELD_STATUS(t, next_scheduled_power_on_minute), t->next_scheduled_power_on_minute);
        cjson_field_num(obj, "next_scheduled_power_on_second", LAZYBIOS_FIELD_STATUS(t, next_scheduled_power_on_second), t->next_scheduled_power_on_second);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type25", arr);
}

void lazybiosExtJSONAddType26(const lazybiosType26Array_t* type26, cJSON* root) {
    if (!root) return;
    if (!type26) {
        cJSON_AddNullToObject(root, "Type26");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type26->count; i++) {
        const lazybiosType26_t* t = &type26->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_str(obj, "description", LAZYBIOS_FIELD_STATUS(t, description), t->description);
        if (LAZYBIOS_FIELD_STATUS(t, location_and_status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "location", t->decoded.location);
            cJSON_AddStringToObject(record_decoded(obj), "status", t->decoded.status);
        } else if (LAZYBIOS_FIELD_STATUS(t, location_and_status) == LAZYBIOS_FIELD_UNREACHABLE) {
        } else {
            cJSON_AddNullToObject(obj, "location");
            cJSON_AddNullToObject(record_decoded(obj), "status");
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

void lazybiosExtJSONAddType27(const lazybiosType27Array_t* type27, cJSON* root) {
    if (!root) return;
    if (!type27) {
        cJSON_AddNullToObject(root, "Type27");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type27->count; i++) {
        const lazybiosType27_t* t = &type27->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "temperature_probe_handle", LAZYBIOS_FIELD_STATUS(t, temperature_probe_handle), t->temperature_probe_handle);
        if (LAZYBIOS_FIELD_STATUS(t, device_type_and_status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "device_type", t->decoded.device_type);
            cJSON_AddStringToObject(record_decoded(obj), "status", t->decoded.status);
        } else if (LAZYBIOS_FIELD_STATUS(t, device_type_and_status) == LAZYBIOS_FIELD_UNREACHABLE) {
        } else {
            cJSON_AddNullToObject(obj, "device_type");
            cJSON_AddNullToObject(record_decoded(obj), "status");
        }
        cjson_field_num(obj, "cooling_unit_group", LAZYBIOS_FIELD_STATUS(t, cooling_unit_group), t->cooling_unit_group);
        cjson_field_num(obj, "oem_defined", LAZYBIOS_FIELD_STATUS(t, oem_defined), t->oem_defined);
        cjson_field_num(obj, "nominal_speed", LAZYBIOS_FIELD_STATUS(t, nominal_speed), t->nominal_speed);
        cjson_field_str(obj, "description", LAZYBIOS_FIELD_STATUS(t, description), t->description);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type27", arr);
}

void lazybiosExtJSONAddType28(const lazybiosType28Array_t* type28, cJSON* root) {
    if (!root) return;
    if (!type28) {
        cJSON_AddNullToObject(root, "Type28");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type28->count; i++) {
        const lazybiosType28_t* t = &type28->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_str(obj, "description", LAZYBIOS_FIELD_STATUS(t, description), t->description);
        if (LAZYBIOS_FIELD_STATUS(t, location_and_status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "location", t->decoded.location);
            cJSON_AddStringToObject(record_decoded(obj), "status", t->decoded.status);
        } else if (LAZYBIOS_FIELD_STATUS(t, location_and_status) == LAZYBIOS_FIELD_UNREACHABLE) {
        } else {
            cJSON_AddNullToObject(obj, "location");
            cJSON_AddNullToObject(record_decoded(obj), "status");
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

void lazybiosExtJSONAddType29(const lazybiosType29Array_t* type29, cJSON* root) {
    if (!root) return;
    if (!type29) {
        cJSON_AddNullToObject(root, "Type29");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type29->count; i++) {
        const lazybiosType29_t* t = &type29->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_str(obj, "description", LAZYBIOS_FIELD_STATUS(t, description), t->description);
        if (LAZYBIOS_FIELD_STATUS(t, location_and_status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "location", t->decoded.location);
            cJSON_AddStringToObject(record_decoded(obj), "status", t->decoded.status);
        } else if (LAZYBIOS_FIELD_STATUS(t, location_and_status) == LAZYBIOS_FIELD_UNREACHABLE) {
        } else {
            cJSON_AddNullToObject(obj, "location");
            cJSON_AddNullToObject(record_decoded(obj), "status");
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

void lazybiosExtJSONAddType30(const lazybiosType30Array_t* type30, cJSON* root) {
    if (!root) return;
    if (!type30) {
        cJSON_AddNullToObject(root, "Type30");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type30->count; i++) {
        const lazybiosType30_t* t = &type30->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_str(obj, "manufacturer_name", LAZYBIOS_FIELD_STATUS(t, manufacturer_name), t->manufacturer_name);
        cjson_field_num(obj, "connections", LAZYBIOS_FIELD_STATUS(t, connections), t->connections);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type30", arr);
}

void lazybiosExtJSONAddType31(const lazybiosType31Array_t* type31, cJSON* root) {
    if (!root) return;
    if (!type31) {
        cJSON_AddNullToObject(root, "Type31");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type31->count; i++) {
        const lazybiosType31_t* t = &type31->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "checksum", LAZYBIOS_FIELD_STATUS(t, checksum), t->checksum);
        cjson_field_num(obj, "reserved1", LAZYBIOS_FIELD_STATUS(t, reserved_1), t->reserved_1);
        cjson_field_num(obj, "reserved2", LAZYBIOS_FIELD_STATUS(t, reserved_2), t->reserved_2);
        cjson_field_num(obj, "bis_entry_point_16", LAZYBIOS_FIELD_STATUS(t, bis_entry_point_16), t->bis_entry_point_16);
        cjson_field_num(obj, "bis_entry_point_32", LAZYBIOS_FIELD_STATUS(t, bis_entry_point_32), t->bis_entry_point_32);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type31", arr);
}

void lazybiosExtJSONAddType32(const lazybiosType32Array_t* type32, cJSON* root) {
    if (!root) return;
    if (!type32) {
        cJSON_AddNullToObject(root, "Type32");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type32->count; i++) {
        const lazybiosType32_t* t = &type32->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "boot_status", LAZYBIOS_FIELD_STATUS(t, boot_status), t->boot_status);
        if (LAZYBIOS_FIELD_STATUS(t, reserved) == LAZYBIOS_FIELD_PRESENT) {
            cJSON* res_arr = cJSON_CreateArray();
            for (int j = 0; j < 6; j++) cJSON_AddItemToArray(res_arr, cJSON_CreateNumber(t->reserved[j]));
            cJSON_AddItemToObject(obj, "reserved", res_arr);
        }
        if (LAZYBIOS_FIELD_STATUS(t, boot_status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "boot_status", t->decoded.boot_status);
                } else {
            cJSON_AddNullToObject(obj, "boot_status");
        }
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type32", arr);
}

void lazybiosExtJSONAddType33(const lazybiosType33Array_t* type33, cJSON* root) {
    if (!root) return;
    if (!type33) {
        cJSON_AddNullToObject(root, "Type33");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type33->count; i++) {
        const lazybiosType33_t* t = &type33->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "error_granularity", LAZYBIOS_FIELD_STATUS(t, error_granularity), t->error_granularity);
        cjson_field_num(obj, "error_operation", LAZYBIOS_FIELD_STATUS(t, error_operation), t->error_operation);
        cjson_field_num(obj, "error_type", LAZYBIOS_FIELD_STATUS(t, error_type), t->error_type);
        if (LAZYBIOS_FIELD_STATUS(t, error_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "error_type", t->decoded.error_type);
                } else {
            cJSON_AddNullToObject(obj, "error_type");
        }
        if (LAZYBIOS_FIELD_STATUS(t, error_granularity) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "error_granularity", t->decoded.error_granularity);
                } else {
            cJSON_AddNullToObject(obj, "error_granularity");
        }
        if (LAZYBIOS_FIELD_STATUS(t, error_operation) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "error_operation", t->decoded.error_operation);
                } else {
            cJSON_AddNullToObject(obj, "error_operation");
        }
        if (LAZYBIOS_FIELD_STATUS(t, vendor_syndrome) == LAZYBIOS_FIELD_PRESENT) {
            if (t->vendor_syndrome == 0) cJSON_AddStringToObject(obj, "vendor_syndrome", "Unknown");
            else cJSON_AddNumberToObject(obj, "vendor_syndrome", t->vendor_syndrome);
                } else {
            cJSON_AddNullToObject(obj, "vendor_syndrome");
        }
        if (LAZYBIOS_FIELD_STATUS(t, memory_array_error_address) == LAZYBIOS_FIELD_PRESENT) {
            if (t->memory_array_error_address == 0x8000000000000000ULL) cJSON_AddStringToObject(obj, "memory_array_error_address", "Unknown");
            else cJSON_AddNumberToObject(obj, "memory_array_error_address", (double)t->memory_array_error_address);
                } else {
            cJSON_AddNullToObject(obj, "memory_array_error_address");
        }
        if (LAZYBIOS_FIELD_STATUS(t, device_error_address) == LAZYBIOS_FIELD_PRESENT) {
            if (t->device_error_address == 0x8000000000000000ULL) cJSON_AddStringToObject(obj, "device_error_address", "Unknown");
            else cJSON_AddNumberToObject(obj, "device_error_address", (double)t->device_error_address);
                } else {
            cJSON_AddNullToObject(obj, "device_error_address");
        }
        if (LAZYBIOS_FIELD_STATUS(t, error_resolution) == LAZYBIOS_FIELD_PRESENT) {
            if (t->error_resolution == 0x80000000U) cJSON_AddStringToObject(obj, "error_resolution", "Unknown");
            else cJSON_AddNumberToObject(obj, "error_resolution_bytes", t->error_resolution);
                } else {
            cJSON_AddNullToObject(obj, "error_resolution_bytes");
        }
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type33", arr);
}

void lazybiosExtJSONAddType34(const lazybiosType34Array_t* type34, cJSON* root) {
    if (!root) return;
    if (!type34) {
        cJSON_AddNullToObject(root, "Type34");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type34->count; i++) {
        const lazybiosType34_t* t = &type34->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "address_type", LAZYBIOS_FIELD_STATUS(t, address_type), t->address_type);
        cjson_field_num(obj, "device_type", LAZYBIOS_FIELD_STATUS(t, device_type), t->device_type);
        cjson_field_str(obj, "description", LAZYBIOS_FIELD_STATUS(t, description), t->description);
        if (LAZYBIOS_FIELD_STATUS(t, device_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "device_type", t->decoded.device_type);
                } else {
            cJSON_AddNullToObject(obj, "device_type");
        }
        cjson_field_num(obj, "address", LAZYBIOS_FIELD_STATUS(t, address), t->address);
        if (LAZYBIOS_FIELD_STATUS(t, address_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "address_type", t->decoded.address_type);
                } else {
            cJSON_AddNullToObject(obj, "address_type");
        }
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type34", arr);
}

void lazybiosExtJSONAddType35(const lazybiosType35Array_t* type35, cJSON* root) {
    if (!root) return;
    if (!type35) {
        cJSON_AddNullToObject(root, "Type35");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type35->count; i++) {
        const lazybiosType35_t* t = &type35->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_str(obj, "description", LAZYBIOS_FIELD_STATUS(t, description), t->description);
        cjson_field_num(obj, "management_device_handle", LAZYBIOS_FIELD_STATUS(t, management_device_handle), t->management_device_handle);
        cjson_field_num(obj, "component_handle", LAZYBIOS_FIELD_STATUS(t, component_handle), t->component_handle);
        cjson_field_num(obj, "threshold_handle", LAZYBIOS_FIELD_STATUS(t, threshold_handle), t->threshold_handle);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type35", arr);
}

void lazybiosExtJSONAddType36(const lazybiosType36Array_t* type36, cJSON* root) {
    if (!root) return;
    if (!type36) {
        cJSON_AddNullToObject(root, "Type36");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type36->count; i++) {
        const lazybiosType36_t* t = &type36->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
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

void lazybiosExtJSONAddType37(const lazybiosType37Array_t* type37, cJSON* root) {
    if (!root) return;
    if (!type37) {
        cJSON_AddNullToObject(root, "Type37");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type37->count; i++) {
        const lazybiosType37_t* t = &type37->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "channel_type", LAZYBIOS_FIELD_STATUS(t, channel_type), t->channel_type);
        if (LAZYBIOS_FIELD_STATUS(t, channel_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "channel_type", t->decoded.channel_type);
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

void lazybiosExtJSONAddType38(const lazybiosType38Array_t* type38, cJSON* root) {
    if (!root) return;
    if (!type38) {
        cJSON_AddNullToObject(root, "Type38");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type38->count; i++) {
        const lazybiosType38_t* t = &type38->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "interface_type", LAZYBIOS_FIELD_STATUS(t, interface_type), t->interface_type);
        cjson_field_num(obj, "base_address", LAZYBIOS_FIELD_STATUS(t, base_address), t->base_address);
        if (LAZYBIOS_FIELD_STATUS(t, interface_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "interface_type", t->decoded.interface_type);
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

void lazybiosExtJSONAddType39(const lazybiosType39Array_t* type39, cJSON* root) {
    if (!root) return;
    if (!type39) {
        cJSON_AddNullToObject(root, "Type39");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type39->count; i++) {
        const lazybiosType39_t* t = &type39->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
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

void lazybiosExtJSONAddType40(const lazybiosType40Array_t* type40, cJSON* root) {
    if (!root) return;
    if (!type40) {
        cJSON_AddNullToObject(root, "Type40");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type40->count; i++) {
        const lazybiosType40_t* t = &type40->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
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

void lazybiosExtJSONAddType41(const lazybiosType41Array_t* type41, cJSON* root) {
    if (!root) return;
    if (!type41) {
        cJSON_AddNullToObject(root, "Type41");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type41->count; i++) {
        const lazybiosType41_t* t = &type41->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_str(obj, "reference_designation", LAZYBIOS_FIELD_STATUS(t, reference_designation), t->reference_designation);
        if (LAZYBIOS_FIELD_STATUS(t, device_type_and_status) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "device_type", t->decoded.device_type);
            cJSON_AddStringToObject(record_decoded(obj), "device_status", t->decoded.device_status);
        } else if (LAZYBIOS_FIELD_STATUS(t, device_type_and_status) == LAZYBIOS_FIELD_UNREACHABLE) {
        } else {
            cJSON_AddNullToObject(obj, "device_type");
            cJSON_AddNullToObject(record_decoded(obj), "status");
        }
        cjson_field_num(obj, "device_type_instance", LAZYBIOS_FIELD_STATUS(t, device_type_instance), t->device_type_instance);
        cjson_field_num(obj, "segment_group_number", LAZYBIOS_FIELD_STATUS(t, segment_group_number), t->segment_group_number);
        cjson_field_num(obj, "bus_number", LAZYBIOS_FIELD_STATUS(t, bus_number), t->bus_number);
        cjson_field_num(obj, "device_function_number", LAZYBIOS_FIELD_STATUS(t, device_function_number), t->device_function_number);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type41", arr);
}

void lazybiosExtJSONAddType42(const lazybiosType42Array_t* type42, cJSON* root) {
    if (!root) return;
    if (!type42) {
        cJSON_AddNullToObject(root, "Type42");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type42->count; i++) {
        const lazybiosType42_t* t = &type42->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "interface_type", LAZYBIOS_FIELD_STATUS(t, interface_type), t->interface_type);
        if (LAZYBIOS_FIELD_STATUS(t, interface_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "interface_type", t->decoded.interface_type);
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
                cJSON_AddStringToObject(rec_obj, "protocol_type", t->protocol_records[j].decoded.protocol_type);
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

void lazybiosExtJSONAddType43(const lazybiosType43Array_t* type43, cJSON* root) {
    if (!root) return;
    if (!type43) {
        cJSON_AddNullToObject(root, "Type43");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type43->count; i++) {
        const lazybiosType43_t* t = &type43->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
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

void lazybiosExtJSONAddType44(const lazybiosType44Array_t* type44, cJSON* root) {
    if (!root) return;
    if (!type44) {
        cJSON_AddNullToObject(root, "Type44");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type44->count; i++) {
        const lazybiosType44_t* t = &type44->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "processor_type", LAZYBIOS_FIELD_STATUS(t, processor_type), t->processor_type);
        cjson_field_num(obj, "referenced_handle", LAZYBIOS_FIELD_STATUS(t, referenced_handle), t->referenced_handle);
        if (LAZYBIOS_FIELD_STATUS(t, processor_type) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "processor_type", t->decoded.processor_type);
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

void lazybiosExtJSONAddType45(const lazybiosType45Array_t* type45, cJSON* root) {
    if (!root) return;
    if (!type45) {
        cJSON_AddNullToObject(root, "Type45");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type45->count; i++) {
        const lazybiosType45_t* t = &type45->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "firmware_id_format", LAZYBIOS_FIELD_STATUS(t, firmware_id_format), t->firmware_id_format);
        cjson_field_num(obj, "state", LAZYBIOS_FIELD_STATUS(t, state), t->state);
        cjson_field_num(obj, "version_format", LAZYBIOS_FIELD_STATUS(t, version_format), t->version_format);
        cjson_field_str(obj, "firmware_component_name", LAZYBIOS_FIELD_STATUS(t, firmware_component_name), t->firmware_component_name);
        cjson_field_str(obj, "firmware_version", LAZYBIOS_FIELD_STATUS(t, firmware_version), t->firmware_version);
        if (LAZYBIOS_FIELD_STATUS(t, version_format) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "version_format", t->decoded.version_format);
                } else {
            cJSON_AddNullToObject(obj, "version_format");
        }
        cjson_field_str(obj, "firmware_id", LAZYBIOS_FIELD_STATUS(t, firmware_id), t->firmware_id);
        if (LAZYBIOS_FIELD_STATUS(t, firmware_id_format) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "firmware_id_format", t->decoded.firmware_id_format);
                } else {
            cJSON_AddNullToObject(obj, "firmware_id_format");
        }
        cjson_field_str(obj, "release_date", LAZYBIOS_FIELD_STATUS(t, release_date), t->release_date);
        cjson_field_str(obj, "manufacturer", LAZYBIOS_FIELD_STATUS(t, manufacturer), t->manufacturer);
        cjson_field_str(obj, "lowest_supported_firmware_version", LAZYBIOS_FIELD_STATUS(t, lowest_supported_firmware_version), t->lowest_supported_firmware_version);
        cjson_field_num(obj, "image_size", LAZYBIOS_FIELD_STATUS(t, image_size), (double)t->image_size);
        cjson_field_num(obj, "characteristics", LAZYBIOS_FIELD_STATUS(t, characteristics), t->characteristics);
        if (LAZYBIOS_FIELD_STATUS(t, state) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "state", t->decoded.state);
                } else {
            cJSON_AddNullToObject(obj, "state");
        }
        if (LAZYBIOS_FIELD_STATUS(t, associated_component_handles) == LAZYBIOS_FIELD_PRESENT && t->associated_component_handles) {
            cJSON* handles_arr = cJSON_CreateArray();
            for (uint8_t j = 0; j < t->number_of_associated_components; j++) {
                cJSON_AddItemToArray(handles_arr,
                    cJSON_CreateNumber(t->associated_component_handles[j]));
            }
            cJSON_AddItemToObject(obj, "associated_component_handles", handles_arr);
        }
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type45", arr);
}

void lazybiosExtJSONAddType46(const lazybiosType46Array_t* type46, cJSON* root) {
    if (!root) return;
    if (!type46) {
        cJSON_AddNullToObject(root, "Type46");
        return;
    }
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return;
    for (size_t i = 0; i < type46->count; i++) {
        const lazybiosType46_t* t = &type46->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) continue;
        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);
        cjson_field_num(obj, "string_property_id", LAZYBIOS_FIELD_STATUS(t, string_property_id), t->string_property_id);
        if (LAZYBIOS_FIELD_STATUS(t, string_property_id) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddStringToObject(record_decoded(obj), "string_property_id", t->decoded.string_property_id);
                } else {
            cJSON_AddNullToObject(obj, "string_property_id");
        }
        cjson_field_str(obj, "string_property_value", LAZYBIOS_FIELD_STATUS(t, string_property_value), t->string_property_value);
        cjson_field_num(obj, "parent_handle", LAZYBIOS_FIELD_STATUS(t, parent_handle), t->parent_handle);
        cJSON_AddItemToArray(arr, obj);
    }
    cJSON_AddItemToObject(root, "Type46", arr);
}

void lazybiosExtJSONAddOemDellType177(const lazybiosOemDellType177Array_t* dell177, cJSON* root) {
    if (!root) {
        return;
    }

    if (!dell177) {
        cJSON_AddNullToObject(oem_vendor_object(root, "dell"), "Type177");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < dell177->count; i++) {
        const lazybiosOemDellType177_t* t = &dell177->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);

        cjson_field_hex(obj, "bios_flags", LAZYBIOS_FIELD_STATUS(t, bios_flags), t->bios_flags, 16);
        cjson_field_str(record_decoded(obj), "acpi_wmi_supported", LAZYBIOS_FIELD_STATUS(t, acpi_wmi_supported), t->decoded.acpi_wmi_supported);

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(oem_vendor_object(root, "dell"), "Type177", arr);
}

void lazybiosExtJSONAddOemDellType212(const lazybiosOemDellType212Array_t* dell212, cJSON* root) {
    if (!root) {
        return;
    }

    if (!dell212) {
        cJSON_AddNullToObject(oem_vendor_object(root, "dell"), "Type212");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < dell212->count; i++) {
        const lazybiosOemDellType212_t* t = &dell212->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);

        cjson_field_hex(obj, "index_port", LAZYBIOS_FIELD_STATUS(t, index_port), t->index_port, 4);
        cjson_field_hex(obj, "data_port", LAZYBIOS_FIELD_STATUS(t, data_port), t->data_port, 4);
        cjson_field_num(obj, "checksum_type", LAZYBIOS_FIELD_STATUS(t, checksum_type), t->checksum_type);
        cjson_field_str(record_decoded(obj), "checksum_type", LAZYBIOS_FIELD_STATUS(t, checksum_type), t->decoded.checksum_type);
        cjson_field_hex(obj, "start_index", LAZYBIOS_FIELD_STATUS(t, start_index), t->start_index, 2);
        cjson_field_hex(obj, "end_index", LAZYBIOS_FIELD_STATUS(t, end_index), t->end_index, 2);
        cjson_field_hex(obj, "value_index", LAZYBIOS_FIELD_STATUS(t, value_index), t->value_index, 2);
        cJSON_AddNumberToObject(obj, "token_count", (double)t->token_count);

        if (LAZYBIOS_FIELD_STATUS(t, tokens) == LAZYBIOS_FIELD_PRESENT &&
            t->tokens && t->token_count > 0) {
            cJSON* tokens = cJSON_CreateArray();
            if (tokens) {
                for (size_t j = 0; j < t->token_count; j++) {
                    cJSON* token = cJSON_CreateObject();
                    if (!token) {
                        continue;
                    }

                    cjson_add_hex(token, "token_id", t->tokens[j].token_id, 4);
                    cjson_add_hex(token, "location", t->tokens[j].location, 2);
                    cjson_add_hex(token, "and_mask", t->tokens[j].and_mask, 2);
                    cjson_add_hex(token, "or_mask", t->tokens[j].or_mask, 2);

                    cJSON_AddItemToArray(tokens, token);
                }
                cJSON_AddItemToObject(obj, "tokens", tokens);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, tokens) == LAZYBIOS_FIELD_UNREACHABLE) {
        } else if (LAZYBIOS_FIELD_STATUS(t, tokens) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "tokens", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "tokens");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(oem_vendor_object(root, "dell"), "Type212", arr);
}

void lazybiosExtJSONAddOemDellType218(const lazybiosOemDellType218Array_t* dell218, cJSON* root) {
    if (!root) {
        return;
    }

    if (!dell218) {
        cJSON_AddNullToObject(oem_vendor_object(root, "dell"), "Type218");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < dell218->count; i++) {
        const lazybiosOemDellType218_t* t = &dell218->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);

        cjson_field_hex(obj, "command_io_address", LAZYBIOS_FIELD_STATUS(t, command_io_address), t->command_io_address, 4);
        cjson_field_hex(obj, "command_io_code", LAZYBIOS_FIELD_STATUS(t, command_io_code), t->command_io_code, 2);
        cjson_field_hex(obj, "supported_command_classes_bitmap", LAZYBIOS_FIELD_STATUS(t, supported_command_classes_bitmap), t->supported_command_classes_bitmap, 8);
        cJSON_AddNumberToObject(obj, "token_count", (double)t->token_count);

        if (LAZYBIOS_FIELD_STATUS(t, tokens) == LAZYBIOS_FIELD_PRESENT &&
            t->tokens && t->token_count > 0) {
            cJSON* tokens = cJSON_CreateArray();
            if (tokens) {
                for (size_t j = 0; j < t->token_count; j++) {
                    cJSON* token = cJSON_CreateObject();
                    if (!token) {
                        continue;
                    }

                    cjson_add_hex(token, "token_id", t->tokens[j].token_id, 4);
                    cjson_add_hex(token, "location", t->tokens[j].location, 4);
                    cjson_add_hex(token, "value", t->tokens[j].value, 4);

                    cJSON_AddItemToArray(tokens, token);
                }
                cJSON_AddItemToObject(obj, "tokens", tokens);
            }
        } else if (LAZYBIOS_FIELD_STATUS(t, tokens) == LAZYBIOS_FIELD_UNREACHABLE) {
        } else if (LAZYBIOS_FIELD_STATUS(t, tokens) == LAZYBIOS_FIELD_PRESENT) {
            cJSON_AddItemToObject(obj, "tokens", cJSON_CreateArray());
        } else {
            cJSON_AddNullToObject(obj, "tokens");
        }

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(oem_vendor_object(root, "dell"), "Type218", arr);
}

void lazybiosExtJSONAddOemHpType204(const lazybiosOemHpType204Array_t* hp204, cJSON* root) {
    if (!root) {
        return;
    }

    if (!hp204) {
        cJSON_AddNullToObject(oem_vendor_object(root, "hp"), "Type204");
        return;
    }

    cJSON* arr = cJSON_CreateArray();
    if (!arr) {
        return;
    }

    for (size_t i = 0; i < hp204->count; i++) {
        const lazybiosOemHpType204_t* t = &hp204->entries[i];
        cJSON* obj = cJSON_CreateObject();
        if (!obj) {
            continue;
        }

        cjson_add_hex(obj, "handle", t->handle, 4);
        cJSON_AddNumberToObject(obj, "length", t->length);

        cjson_field_str(obj, "rack_name", LAZYBIOS_FIELD_STATUS(t, rack_name), t->rack_name);
        cjson_field_str(obj, "enclosure_name", LAZYBIOS_FIELD_STATUS(t, enclosure_name), t->enclosure_name);
        cjson_field_str(obj, "enclosure_model", LAZYBIOS_FIELD_STATUS(t, enclosure_model), t->enclosure_model);
        cjson_field_str(obj, "enclosure_serial", LAZYBIOS_FIELD_STATUS(t, enclosure_serial), t->enclosure_serial);
        cjson_field_num(obj, "enclosure_bays", LAZYBIOS_FIELD_STATUS(t, enclosure_bays), t->enclosure_bays);
        cjson_field_str(obj, "server_bay", LAZYBIOS_FIELD_STATUS(t, server_bay), t->server_bay);
        cjson_field_num(obj, "bays_filled", LAZYBIOS_FIELD_STATUS(t, bays_filled), t->bays_filled);

        cJSON_AddItemToArray(arr, obj);
    }

    cJSON_AddItemToObject(oem_vendor_object(root, "hp"), "Type204", arr);
}

int lazybiosParseJSONAll(lazybiosCTX_t* ctx, cJSON* root) {
	if (!ctx || !root) return -1;
	if (lazybiosParseAll(ctx) != 0) return -1; // Doesn't lose past pointers since lazybiosParseAll guards each getter

	lazybiosExtJSONAddSMBIOSInfo(ctx->DMIData, ctx->backend, root);
	lazybiosExtJSONAddType0(ctx->Type0, root);
	lazybiosExtJSONAddType1(ctx->Type1, root);
	lazybiosExtJSONAddType2(ctx->Type2, root);
	lazybiosExtJSONAddType3(ctx->Type3, root);
	lazybiosExtJSONAddType4(ctx->Type4, root);
	lazybiosExtJSONAddType5(ctx->Type5, root);
	lazybiosExtJSONAddType6(ctx->Type6, root);
	lazybiosExtJSONAddType7(ctx->Type7, root);
	lazybiosExtJSONAddType8(ctx->Type8, root);
	lazybiosExtJSONAddType9(ctx->Type9, root);
	lazybiosExtJSONAddType10(ctx->Type10, root);
	lazybiosExtJSONAddType11(ctx->Type11, root);
	lazybiosExtJSONAddType12(ctx->Type12, root);
	lazybiosExtJSONAddType13(ctx->Type13, root);
	lazybiosExtJSONAddType14(ctx->Type14, root);
	lazybiosExtJSONAddType15(ctx->Type15, root);
	lazybiosExtJSONAddType16(ctx->Type16, root);
	lazybiosExtJSONAddType17(ctx->Type17, root);
	lazybiosExtJSONAddType18(ctx->Type18, root);
	lazybiosExtJSONAddType19(ctx->Type19, root);
	lazybiosExtJSONAddType20(ctx->Type20, root);
	lazybiosExtJSONAddType21(ctx->Type21, root);
	lazybiosExtJSONAddType22(ctx->Type22, root);
	lazybiosExtJSONAddType23(ctx->Type23, root);
	lazybiosExtJSONAddType24(ctx->Type24, root);
	lazybiosExtJSONAddType25(ctx->Type25, root);
	lazybiosExtJSONAddType26(ctx->Type26, root);
	lazybiosExtJSONAddType27(ctx->Type27, root);
	lazybiosExtJSONAddType28(ctx->Type28, root);
	lazybiosExtJSONAddType29(ctx->Type29, root);
	lazybiosExtJSONAddType30(ctx->Type30, root);
	lazybiosExtJSONAddType31(ctx->Type31, root);
	lazybiosExtJSONAddType32(ctx->Type32, root);
	lazybiosExtJSONAddType33(ctx->Type33, root);
	lazybiosExtJSONAddType34(ctx->Type34, root);
	lazybiosExtJSONAddType35(ctx->Type35, root);
	lazybiosExtJSONAddType36(ctx->Type36, root);
	lazybiosExtJSONAddType37(ctx->Type37, root);
	lazybiosExtJSONAddType38(ctx->Type38, root);
	lazybiosExtJSONAddType39(ctx->Type39, root);
	lazybiosExtJSONAddType40(ctx->Type40, root);
	lazybiosExtJSONAddType41(ctx->Type41, root);
	lazybiosExtJSONAddType42(ctx->Type42, root);
	lazybiosExtJSONAddType43(ctx->Type43, root);
	lazybiosExtJSONAddType44(ctx->Type44, root);
	lazybiosExtJSONAddType45(ctx->Type45, root);
	lazybiosExtJSONAddType46(ctx->Type46, root);

	lazybiosExtJSONAddOemDellType177(ctx->oem->dell->Type177, root);
	lazybiosExtJSONAddOemDellType212(ctx->oem->dell->Type212, root);
	lazybiosExtJSONAddOemDellType218(ctx->oem->dell->Type218, root);
	lazybiosExtJSONAddOemHpType204(ctx->oem->hp->Type204, root);

	return 0;
}

//
// Type 2 ( Baseboard (or Module) Information )
//

#include "lazybios.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define MANUFACTURER                        0x04
#define PRODUCT                             0x05
#define VERSION                             0x06
#define SERIAL_NUMBER                       0x07
#define ASSET_TAG                           0x08
#define FEATURE_FLAGS                       0x09
#define LOCATION_IN_CHASSIS                 0x0A
#define CHASSIS_HANDLE                      0x0B
#define BOARD_TYPE                          0x0D
#define NUMBER_OF_CONTAINED_OBJECT_HANDLES  0x0E
#define CONTAINED_OBJECT_HANDLES            0x0F

// Decoders

// Board Type
#define BOARD_TYPE_UNKNOWN                      0x01
#define BOARD_TYPE_OTHER                        0x02
#define BOARD_TYPE_SERVER_BLADE                 0x03
#define BOARD_TYPE_CONNECTIVITY_SWITCH          0x04
#define BOARD_TYPE_SYSTEM_MANAGEMENT_MODULE     0x05
#define BOARD_TYPE_PROCESSOR_MODULE             0x06
#define BOARD_TYPE_IO_MODULE                    0x07
#define BOARD_TYPE_MEMORY_MODULE                0x08
#define BOARD_TYPE_DAUGHTER_BOARD               0x09
#define BOARD_TYPE_MOTHERBOARD                  0x0A
#define BOARD_TYPE_PROCESSOR_MEMORY_MODULE      0x0B
#define BOARD_TYPE_PROCESSOR_IO_MODULE          0x0C
#define BOARD_TYPE_INTERCONNECT_BOARD           0x0D
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

lazybiosType2_t* lazybiosGetType2(lazybiosType2_t *Type2, size_t *type2_count, lazybiosDMI_t* DMIData) {
    if (!DMIData || !DMIData->dmi_data) return LAZYBIOS_NULL;

    const uint8_t* p = DMIData->dmi_data;
    const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

    size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_BASEBOARD);
    size_t index = 0;
    Type2 = calloc(count, sizeof(lazybiosType2_t));
    if (!Type2) return LAZYBIOS_NULL;
	if (count == 0) {
		*type2_count = 0;
		return Type2;
	}

    while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
        uint8_t type = p[0];
        uint8_t len = p[1];

        if (type == SMBIOS_TYPE_BASEBOARD) {
        	if (index >= count) break;
            lazybiosType2_t *current = &Type2[index];

            if (len > MANUFACTURER) current->manufacturer = DMIString(p, len, p[MANUFACTURER], end);
            if (!current->manufacturer) current->manufacturer = strdup(LAZYBIOS_NOT_FOUND_STR);

            if (len > PRODUCT) current->product = DMIString(p, len, p[PRODUCT], end);
            if (!current->product) current->product = strdup(LAZYBIOS_NOT_FOUND_STR);

            if (len > VERSION) current->version = DMIString(p, len, p[VERSION], end);
            if (!current->version) current->version = strdup(LAZYBIOS_NOT_FOUND_STR);

            if (len > SERIAL_NUMBER) current->serial_number = DMIString(p, len, p[SERIAL_NUMBER], end);
            if (!current->serial_number) current->serial_number = strdup(LAZYBIOS_NOT_FOUND_STR);

            if (len > ASSET_TAG) current->asset_tag = DMIString(p, len, p[ASSET_TAG], end);
            if (!current->asset_tag) current->asset_tag = strdup(LAZYBIOS_NOT_FOUND_STR);

            current->feature_flags = (len > FEATURE_FLAGS) ? p[FEATURE_FLAGS] : LAZYBIOS_NOT_FOUND_U8;

        	if (len > LOCATION_IN_CHASSIS) current->location_in_chassis = DMIString(p, len, p[LOCATION_IN_CHASSIS], end);
        	if (!current->location_in_chassis) current->location_in_chassis = strdup(LAZYBIOS_NOT_FOUND_STR);

            if (len >=CHASSIS_HANDLE + sizeof(uint16_t)) {
                memcpy(&current->chassis_handle, p + CHASSIS_HANDLE, sizeof(uint16_t));
            } else {
                current->chassis_handle = LAZYBIOS_NOT_FOUND_U16;
            }

            current->board_type = (len > BOARD_TYPE) ? p[BOARD_TYPE] : LAZYBIOS_NOT_FOUND_U8;

            current->number_of_contained_object_handles = (len > NUMBER_OF_CONTAINED_OBJECT_HANDLES) ? p[NUMBER_OF_CONTAINED_OBJECT_HANDLES] : LAZYBIOS_NOT_FOUND_U8;

            if (current->number_of_contained_object_handles > 0) {
                const size_t array_bytes = current->number_of_contained_object_handles * sizeof(uint16_t);

                if (len >= CONTAINED_OBJECT_HANDLES + array_bytes) {
                    current->contained_object_handles = malloc(array_bytes);
                    if (current->contained_object_handles) memcpy(current->contained_object_handles, p + CONTAINED_OBJECT_HANDLES, array_bytes);
                } else {
                    current->number_of_contained_object_handles = 0;
                    current->contained_object_handles = LAZYBIOS_NULL;
                }
            }

            index++;
        }
        p = DMINext(p, end);
    }
    *type2_count = index;
    return Type2;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders

// Feature Flags
void lazybiosType2FeatureflagsStr(uint8_t feature_flags, char *buf, size_t buf_len) {
    size_t len = 0;
    buf[0] = '\0';

    if (feature_flags & (1 << 0)) len += snprintf(buf + len, buf_len - len, "Hosting board, ");
    if (feature_flags & (1 << 1)) len += snprintf(buf + len, buf_len - len, "Requires daughter board, ");
    if (feature_flags & (1 << 2)) len += snprintf(buf + len, buf_len - len, "Removable, ");
    if (feature_flags & (1 << 3)) len += snprintf(buf + len, buf_len - len, "Replaceable, ");
    if (feature_flags & (1 << 4)) len += snprintf(buf + len, buf_len - len, "Hot swappable, ");


    if (len == 0) {
        snprintf(buf, buf_len, "None");
    } else if (len >= 2) {
        buf[len - 2] = '\0';
    }
}

// Board Type
const char* lazybiosType2BoardTypeStr(uint8_t board_type) {
    switch(board_type) {
        case BOARD_TYPE_UNKNOWN:                    return "Unknown";
        case BOARD_TYPE_OTHER:                      return "Other";
        case BOARD_TYPE_SERVER_BLADE:               return "Server Blade";
        case BOARD_TYPE_CONNECTIVITY_SWITCH:        return "Connectivity Switch";
        case BOARD_TYPE_SYSTEM_MANAGEMENT_MODULE:   return "System Management Module";
        case BOARD_TYPE_PROCESSOR_MODULE:           return "Processor Module";
        case BOARD_TYPE_IO_MODULE:                  return "I/O Module";
        case BOARD_TYPE_MEMORY_MODULE:              return "Memory Module";
        case BOARD_TYPE_DAUGHTER_BOARD:             return "Daughter board";
        case BOARD_TYPE_MOTHERBOARD:                return "Motherboard (includes processor, memory, and I/O)";
        case BOARD_TYPE_PROCESSOR_MEMORY_MODULE:    return "Processor/Memory Module";
        case BOARD_TYPE_PROCESSOR_IO_MODULE:        return "Processor/IO Module";
        case BOARD_TYPE_INTERCONNECT_BOARD:         return "Interconnect board";
        default:                                    return "Unknown Board Type!";
    }
}



// Free Function
void lazybiosFreeType2(lazybiosType2_t *Type2, size_t type2_count) {
    if (!Type2) return;

    for (size_t i = 0; i < type2_count; i++) {
        free(Type2[i].manufacturer);
        free(Type2[i].product);
        free(Type2[i].version);
        free(Type2[i].serial_number);
        free(Type2[i].asset_tag);
        free(Type2[i].location_in_chassis);
        free(Type2[i].contained_object_handles);
    }

    free(Type2);
}

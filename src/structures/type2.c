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

lazybiosType2_t* lazybiosGetType2(lazybiosCTX_t* ctx) {
    if (!ctx || !ctx->dmi_data) return LAZYBIOS_NULL;

    const uint8_t* p = ctx->dmi_data;
    const uint8_t* end = ctx->dmi_data + ctx->dmi_len;

    while (p + SMBIOS_HEADER_SIZE <= end) {
        uint8_t type = p[0];
        uint8_t len = p[1];

        if (type == SMBIOS_TYPE_BASEBOARD) {
            lazybiosType2_t *Type2 = lb_calloc(1, sizeof(*Type2));
            if (!Type2) return LAZYBIOS_NULL;

            if (len > MANUFACTURER) Type2->manufacturer = DMIString(p, len, p[MANUFACTURER], end);
            if (!Type2->manufacturer) Type2->manufacturer = lb_strdup(LAZYBIOS_NOT_FOUND_STR);

            if (len > PRODUCT) Type2->product = DMIString(p, len, p[PRODUCT], end);
            if (!Type2->product) Type2->product = lb_strdup(LAZYBIOS_NOT_FOUND_STR);

            if (len > VERSION) Type2->version = DMIString(p, len, p[VERSION], end);
            if (!Type2->version) Type2->version = lb_strdup(LAZYBIOS_NOT_FOUND_STR);

            if (len > SERIAL_NUMBER) Type2->serial_number = DMIString(p, len, p[SERIAL_NUMBER], end);
            if (!Type2->serial_number) Type2->serial_number = lb_strdup(LAZYBIOS_NOT_FOUND_STR);

            if (len > ASSET_TAG) Type2->asset_tag = DMIString(p, len, p[ASSET_TAG], end);
            if (!Type2->asset_tag) Type2->asset_tag = lb_strdup(LAZYBIOS_NOT_FOUND_STR);

            Type2->feature_flags = (len > FEATURE_FLAGS) ? p[FEATURE_FLAGS] : LAZYBIOS_NOT_FOUND_U8;

            Type2->location_in_chassis = (len > LOCATION_IN_CHASSIS) ? DMIString(p, len, p[LOCATION_IN_CHASSIS], end) : lb_strdup(LAZYBIOS_NOT_FOUND_STR);

            if (len > CHASSIS_HANDLE + sizeof(uint16_t)) {
                lb_memcpy(&Type2->chassis_handle, p + CHASSIS_HANDLE, sizeof(uint16_t));
            } else {
                Type2->chassis_handle = LAZYBIOS_NOT_FOUND_U16;
            }

            Type2->board_type = (len > BOARD_TYPE) ? p[BOARD_TYPE] : LAZYBIOS_NOT_FOUND_U8;

            Type2->number_of_contained_object_handles = (len > NUMBER_OF_CONTAINED_OBJECT_HANDLES) ? p[NUMBER_OF_CONTAINED_OBJECT_HANDLES] : LAZYBIOS_NOT_FOUND_U8;

            if (Type2->number_of_contained_object_handles > 0) {
                const size_t array_bytes = Type2->number_of_contained_object_handles * sizeof(uint16_t);

                if (len >= CONTAINED_OBJECT_HANDLES + array_bytes) {
                    Type2->contained_object_handles = lb_malloc(array_bytes);
                    if (Type2->contained_object_handles) lb_memcpy(&Type2->contained_object_handles, p + CONTAINED_OBJECT_HANDLES, array_bytes);
                } else {
                    Type2->number_of_contained_object_handles = 0;
                    Type2->contained_object_handles = LAZYBIOS_NULL;
                }
            }

            return Type2;
        }
        p = DMINext(p, end);
    }
    return LAZYBIOS_NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders

// Feature Flags
const char* lazybiosType2FeatureflagsStr(uint8_t feature_flags) {
    _Thread_local static char buf[256];
    size_t len = 0;
    buf[0] = '\0'; // Start with empty string

    if (feature_flags & (1 << 0)) len += lb_snprintf(buf + len, sizeof(buf) - len, "Hosting board, ");
    if (feature_flags & (1 << 1)) len += lb_snprintf(buf + len, sizeof(buf) - len, "Requires daughter board, ");
    if (feature_flags & (1 << 2)) len += lb_snprintf(buf + len, sizeof(buf) - len, "Removable, ");
    if (feature_flags & (1 << 3)) len += lb_snprintf(buf + len, sizeof(buf) - len, "Replaceable, ");
    if (feature_flags & (1 << 4)) len += lb_snprintf(buf + len, sizeof(buf) - len, "Hot swappable, ");

    // Bits 7:5 are reserved (should be 0)
    // We could check: if ((feature_flags & 0xE0) != 0) - reserved bits set

    if (len == 0) return "None";
    if (len >= 2) buf[len - 2] = '\0';


    return buf;
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
        case BOARD_TYPE_MOTHERBOARD:                return "Motherboard (includes processor, memory, and I/O";
        case BOARD_TYPE_PROCESSOR_MEMORY_MODULE:    return "Processor/Memory Module";
        case BOARD_TYPE_PROCESSOR_IO_MODULE:        return "Processor/IO Module";
        case BOARD_TYPE_INTERCONNECT_BOARD:         return "Interconnect board";
        default:                                    return "Unknown Board Type!";
    }
}

// End of Decoders

// Free Function
void lazybiosFreeType2(lazybiosType2_t* Type2) {
    if (!Type2) return;

    lb_free(Type2->manufacturer);
    lb_free(Type2->product);
    lb_free(Type2->version);
    lb_free(Type2->serial_number);
    lb_free(Type2->asset_tag);
    lb_free(Type2->location_in_chassis);
    lb_free(Type2->contained_object_handles);
    lb_free(Type2);
}

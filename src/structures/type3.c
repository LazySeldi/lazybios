//
// Type 3 ( System Enclosure or Chassis )
//

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define MANUFACTURER                      0x04
#define TYPE                              0x05
#define VERSION                           0x06
#define SERIAL_NUMBER                     0x07
#define ASSET_TAG_NUMBER                  0x08
#define BOOT_UP_STATE                     0x09
#define POWER_SUPPLY_STATE                0x0A
#define THERMAL_STATE                     0x0B
#define SECURITY_STATUS                   0x0C
#define OEM_DEFINED                       0x0D
#define HEIGHT                            0x11
#define NUMBER_OF_POWER_CORDS             0x12
#define CONTAINED_ELEMENT_COUNT           0x13
#define CONTAINED_ELEMENT_RECORD_LENGTH   0x14
#define CONTAINED_ELEMENTS                0x15
#define SKU_NUMBER(n, m)                  0x15 + (n * m)
#define RACK_TYPE(n, m)                   0x16 + (n * m)
#define RACK_HEIGHT(n, m)                 0x17 + (n * m)

// Decoders

// Chassis Type
#define CHASSIS_TYPE_OTHER                  0x01
#define CHASSIS_TYPE_UNKNOWN                0x02
#define CHASSIS_TYPE_DESKTOP                0x03
#define CHASSIS_TYPE_LOW_PROFILE_DESKTOP    0x04
#define CHASSIS_TYPE_PIZZA_BOX              0x05
#define CHASSIS_TYPE_MINI_TOWER             0x06
#define CHASSIS_TYPE_TOWER                  0x07
#define CHASSIS_TYPE_PORTABLE               0x08
#define CHASSIS_TYPE_LAPTOP                 0x09
#define CHASSIS_TYPE_NOTEBOOK               0x0A
#define CHASSIS_TYPE_HAND_HELD              0x0B
#define CHASSIS_TYPE_DOCKING_STATION        0x0C
#define CHASSIS_TYPE_ALL_IN_ONE             0x0D
#define CHASSIS_TYPE_SUB_NOTEBOOK           0x0E
#define CHASSIS_TYPE_SPACE_SAVING           0x0F
#define CHASSIS_TYPE_LUNCH_BOX              0x10
#define CHASSIS_TYPE_MAIN_SERVER_CHASSIS    0x11
#define CHASSIS_TYPE_EXPANSION_CHASSIS      0x12
#define CHASSIS_TYPE_SUBCHASSIS             0x13
#define CHASSIS_TYPE_BUS_EXPANSION_CHASSIS  0x14
#define CHASSIS_TYPE_PERIPHERAL_CHASSIS     0x15
#define CHASSIS_TYPE_RAID_CHASSIS           0x16
#define CHASSIS_TYPE_RACK_MOUNT_CHASSIS     0x17
#define CHASSIS_TYPE_SEALED_CASE_PC         0x18
#define CHASSIS_TYPE_MULTI_SYSTEM_CHASSIS   0x19
#define CHASSIS_TYPE_COMPACT_PCI            0x1A
#define CHASSIS_TYPE_ADVANCED_TCA           0x1B
#define CHASSIS_TYPE_BLADE                  0x1C
#define CHASSIS_TYPE_BLADE_ENCLOSURE        0x1D
#define CHASSIS_TYPE_TABLET                 0x1E
#define CHASSIS_TYPE_CONVERTIBLE            0x1F
#define CHASSIS_TYPE_DETACHABLE             0x20
#define CHASSIS_TYPE_IOT_GATEWAY            0x21
#define CHASSIS_TYPE_EMBEDDED_PC            0x22
#define CHASSIS_TYPE_MINI_PC                0x23
#define CHASSIS_TYPE_STICK_PC               0x24

// Chassis State
#define CHASSIS_STATE_OTHER             0x01
#define CHASSIS_STATE_UNKNOWN           0x02
#define CHASSIS_STATE_SAFE              0x03
#define CHASSIS_STATE_WARNING           0x04
#define CHASSIS_STATE_CRITICAL          0x05
#define CHASSIS_STATE_NON_RECOVERABLE   0x06

// Chassis Status
#define CHASSIS_SECURITY_STATUS_OTHER                       0x01
#define CHASSIS_SECURITY_STATUS_UNKNOWN                     0x02
#define CHASSIS_SECURITY_STATUS_NONE                        0x03
#define CHASSIS_SECURITY_STATUS_EXT_INTERFACE_LOCKED_OUT    0x04
#define CHASSIS_SECURITY_STATUS_EXT_INTERFACE_ENABLED       0x05
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lazybios.h"

lazybiosType3_t* lazybiosGetType3(lazybiosCTX_t* ctx) {
      if (!ctx || !ctx->dmi_data) return LAZYBIOS_NULL;

      const uint8_t* p   = ctx->dmi_data;
      const uint8_t* end = ctx->dmi_data + ctx->dmi_len;

      while (p + SMBIOS_HEADER_SIZE <= end) {
            uint8_t type = p[0];
            uint8_t len  = p[1];

            if (type == SMBIOS_TYPE_CHASSIS) {
                lazybiosType3_t *Type3 = lb_calloc(1, sizeof(*Type3));
                if (!Type3) return LAZYBIOS_NULL;

                if (len > MANUFACTURER) Type3->manufacturer = DMIString(p, len, p[MANUFACTURER], end);
                if (!Type3->manufacturer) Type3->manufacturer = lb_strdup(LAZYBIOS_NOT_FOUND_STR);

                Type3->type = (len > TYPE) ? p[TYPE] : LAZYBIOS_NOT_FOUND_U8;

                if (len > VERSION) Type3->version = DMIString(p, len, p[VERSION], end);
                if (!Type3->version) Type3->version = lb_strdup(LAZYBIOS_NOT_FOUND_STR);

                if (len > SERIAL_NUMBER) Type3->serial_number = DMIString(p, len, p[SERIAL_NUMBER], end);
                if (!Type3->serial_number) Type3->serial_number = lb_strdup(LAZYBIOS_NOT_FOUND_STR);

                if (len > ASSET_TAG_NUMBER) Type3->asset_tag = DMIString(p, len, p[ASSET_TAG_NUMBER], end);
                if (!Type3->asset_tag) Type3->asset_tag = lb_strdup(LAZYBIOS_NOT_FOUND_STR);

                if (ISVERPLUS(ctx, 2, 1)) {
                    Type3->boot_up_state = (len > BOOT_UP_STATE) ? p[BOOT_UP_STATE] : LAZYBIOS_NOT_FOUND_U8;
                    Type3->power_supply_state = (len > POWER_SUPPLY_STATE) ? p[POWER_SUPPLY_STATE] : LAZYBIOS_NOT_FOUND_U8;
                    Type3->thermal_state = (len > THERMAL_STATE) ? p[THERMAL_STATE] : LAZYBIOS_NOT_FOUND_U8;
                    Type3->security_status = (len > SECURITY_STATUS) ? p[SECURITY_STATUS] : LAZYBIOS_NOT_FOUND_U8;
                } else {
                    Type3->boot_up_state = LAZYBIOS_NOT_FOUND_U8;
                    Type3->power_supply_state = LAZYBIOS_NOT_FOUND_U8;
                    Type3->thermal_state = LAZYBIOS_NOT_FOUND_U8;
                    Type3->security_status = LAZYBIOS_NOT_FOUND_U8;
                }

                if (ISVERPLUS(ctx, 2, 3)) {
                    if (len > OEM_DEFINED + sizeof(uint32_t)) {
                        lb_memcpy(&Type3->oem_defined, p + OEM_DEFINED, sizeof(uint32_t));
                    } else {
                        Type3->oem_defined = LAZYBIOS_NOT_FOUND_U32;
                    }

                    Type3->height = (len > HEIGHT) ? p[HEIGHT] : LAZYBIOS_NOT_FOUND_U8;

                    Type3->number_of_power_cords = (len > NUMBER_OF_POWER_CORDS) ? p[NUMBER_OF_POWER_CORDS] : LAZYBIOS_NOT_FOUND_U8;

                    Type3->contained_element_count = (len > CONTAINED_ELEMENT_COUNT) ? p[CONTAINED_ELEMENT_COUNT] : LAZYBIOS_NOT_FOUND_U8;
                    Type3->contained_element_record_length = (len > CONTAINED_ELEMENT_RECORD_LENGTH) ? p[CONTAINED_ELEMENT_RECORD_LENGTH] : LAZYBIOS_NOT_FOUND_U8;

                    if ((Type3->contained_element_count > 0 && Type3->contained_element_count != LAZYBIOS_NOT_FOUND_U8) && (Type3->contained_element_record_length > 0 && Type3->contained_element_record_length != LAZYBIOS_NOT_FOUND_U8)) {
                        const size_t array_bytes =(Type3->contained_element_count * Type3->contained_element_record_length) * sizeof(uint8_t);
                        if (len > CONTAINED_ELEMENTS) Type3->contained_elements = lb_malloc(array_bytes);
                        if (Type3->contained_elements) lb_memcpy(Type3->contained_elements, p + CONTAINED_ELEMENTS, array_bytes);

                        if (ISVERPLUS(ctx, 2, 7)) {
                            if (len > SKU_NUMBER(Type3->contained_element_count, Type3->contained_element_record_length)) Type3->sku_number = DMIString(p, len, p[SKU_NUMBER(Type3->contained_element_count, Type3->contained_element_record_length)], end);
                            if (!Type3->sku_number) Type3->sku_number = lb_strdup(LAZYBIOS_NOT_FOUND_STR);
                        } else {
                            Type3->sku_number = lb_strdup(LAZYBIOS_NOT_FOUND_STR);
                        }

                        if (ISVERPLUS(ctx, 3, 9)) {
                            Type3->rack_type = (len > RACK_TYPE(Type3->contained_element_count, Type3->contained_element_record_length)) ? p[RACK_TYPE(Type3->contained_element_count, Type3->contained_element_record_length)] : LAZYBIOS_NOT_FOUND_U8;
                            Type3->rack_height = (len > RACK_HEIGHT(Type3->contained_element_count, Type3->contained_element_record_length)) ? p[RACK_HEIGHT(Type3->contained_element_count, Type3->contained_element_record_length)] :LAZYBIOS_NOT_FOUND_U8;
                        } else {
                            Type3->rack_type = LAZYBIOS_NOT_FOUND_U8;
                            Type3->rack_height = LAZYBIOS_NOT_FOUND_U8;
                        }
                    } else {
                        Type3->contained_elements = LAZYBIOS_NULL;
                        Type3->sku_number = lb_strdup(LAZYBIOS_NOT_FOUND_STR);
                        Type3->rack_type = LAZYBIOS_NOT_FOUND_U8;
                        Type3->rack_height = LAZYBIOS_NOT_FOUND_U8;
                    }
                } else {
                    Type3->oem_defined = LAZYBIOS_NOT_FOUND_U32;
                    Type3->height = LAZYBIOS_NOT_FOUND_U8;
                    Type3->number_of_power_cords = LAZYBIOS_NOT_FOUND_U8;
                    Type3->contained_element_count = LAZYBIOS_NOT_FOUND_U8;
                    Type3->contained_element_record_length = LAZYBIOS_NOT_FOUND_U8;
                    Type3->contained_elements = LAZYBIOS_NULL;
                    Type3->sku_number = lb_strdup(LAZYBIOS_NOT_FOUND_STR);
                    Type3->rack_type = LAZYBIOS_NOT_FOUND_U8;
                    Type3->rack_height = LAZYBIOS_NOT_FOUND_U8;

                }

                return Type3;
            }
          p = DMINext(p, end);
      }
    return LAZYBIOS_NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders

// Chassis Type
const char* lazybiosType3TypeStr(uint8_t type) {
    _Thread_local static char buf[128];
    size_t len = 0;
    buf[0] = '\0';

    // Bit 7 is the Chassis lock
    if (type & (1 << 7)) {
        len += lb_snprintf(buf + len, sizeof(buf) - len, "Chassis lock present, ");
    }

    // Bits 6:0 = chassis type
    uint8_t chassis_type = type & 0x7F;
    switch (chassis_type) {
        case CHASSIS_TYPE_OTHER:                  len += lb_snprintf(buf + len, sizeof(buf) - len, "Other"); break;
        case CHASSIS_TYPE_UNKNOWN:                len += lb_snprintf(buf + len, sizeof(buf) - len, "Unknown"); break;
        case CHASSIS_TYPE_DESKTOP:                len += lb_snprintf(buf + len, sizeof(buf) - len, "Desktop"); break;
        case CHASSIS_TYPE_LOW_PROFILE_DESKTOP:    len += lb_snprintf(buf + len, sizeof(buf) - len, "Low Profile Desktop"); break;
        case CHASSIS_TYPE_PIZZA_BOX:              len += lb_snprintf(buf + len, sizeof(buf) - len, "Pizza Box"); break;
        case CHASSIS_TYPE_MINI_TOWER:             len += lb_snprintf(buf + len, sizeof(buf) - len, "Mini Tower"); break;
        case CHASSIS_TYPE_TOWER:                  len += lb_snprintf(buf + len, sizeof(buf) - len, "Tower"); break;
        case CHASSIS_TYPE_PORTABLE:               len += lb_snprintf(buf + len, sizeof(buf) - len, "Portable"); break;
        case CHASSIS_TYPE_LAPTOP:                 len += lb_snprintf(buf + len, sizeof(buf) - len, "Laptop"); break;
        case CHASSIS_TYPE_NOTEBOOK:               len += lb_snprintf(buf + len, sizeof(buf) - len, "Notebook"); break;
        case CHASSIS_TYPE_HAND_HELD:              len += lb_snprintf(buf + len, sizeof(buf) - len, "Hand Held"); break;
        case CHASSIS_TYPE_DOCKING_STATION:        len += lb_snprintf(buf + len, sizeof(buf) - len, "Docking Station"); break;
        case CHASSIS_TYPE_ALL_IN_ONE:             len += lb_snprintf(buf + len, sizeof(buf) - len, "All in One"); break;
        case CHASSIS_TYPE_SUB_NOTEBOOK:           len += lb_snprintf(buf + len, sizeof(buf) - len, "Sub Notebook"); break;
        case CHASSIS_TYPE_SPACE_SAVING:           len += lb_snprintf(buf + len, sizeof(buf) - len, "Space-saving"); break;
        case CHASSIS_TYPE_LUNCH_BOX:              len += lb_snprintf(buf + len, sizeof(buf) - len, "Lunch Box"); break;
        case CHASSIS_TYPE_MAIN_SERVER_CHASSIS:    len += lb_snprintf(buf + len, sizeof(buf) - len, "Main Server Chassis"); break;
        case CHASSIS_TYPE_EXPANSION_CHASSIS:      len += lb_snprintf(buf + len, sizeof(buf) - len, "Expansion Chassis"); break;
        case CHASSIS_TYPE_SUBCHASSIS:             len += lb_snprintf(buf + len, sizeof(buf) - len, "SubChassis"); break;
        case CHASSIS_TYPE_BUS_EXPANSION_CHASSIS:  len += lb_snprintf(buf + len, sizeof(buf) - len, "Bus Expansion Chassis"); break;
        case CHASSIS_TYPE_PERIPHERAL_CHASSIS:     len += lb_snprintf(buf + len, sizeof(buf) - len, "Peripheral Chassis"); break;
        case CHASSIS_TYPE_RAID_CHASSIS:           len += lb_snprintf(buf + len, sizeof(buf) - len, "RAID Chassis"); break;
        case CHASSIS_TYPE_RACK_MOUNT_CHASSIS:     len += lb_snprintf(buf + len, sizeof(buf) - len, "Rack Mount Chassis"); break;
        case CHASSIS_TYPE_SEALED_CASE_PC:         len += lb_snprintf(buf + len, sizeof(buf) - len, "Sealed-case PC"); break;
        case CHASSIS_TYPE_MULTI_SYSTEM_CHASSIS:   len += lb_snprintf(buf + len, sizeof(buf) - len, "Multi-system chassis"); break;
        case CHASSIS_TYPE_COMPACT_PCI:            len += lb_snprintf(buf + len, sizeof(buf) - len, "Compact PCI"); break;
        case CHASSIS_TYPE_ADVANCED_TCA:           len += lb_snprintf(buf + len, sizeof(buf) - len, "Advanced TCA"); break;
        case CHASSIS_TYPE_BLADE:                  len += lb_snprintf(buf + len, sizeof(buf) - len, "Blade"); break;
        case CHASSIS_TYPE_BLADE_ENCLOSURE:        len += lb_snprintf(buf + len, sizeof(buf) - len, "Blade Enclosure"); break;
        case CHASSIS_TYPE_TABLET:                 len += lb_snprintf(buf + len, sizeof(buf) - len, "Tablet"); break;
        case CHASSIS_TYPE_CONVERTIBLE:            len += lb_snprintf(buf + len, sizeof(buf) - len, "Convertible"); break;
        case CHASSIS_TYPE_DETACHABLE:             len += lb_snprintf(buf + len, sizeof(buf) - len, "Detachable"); break;
        case CHASSIS_TYPE_IOT_GATEWAY:            len += lb_snprintf(buf + len, sizeof(buf) - len, "IoT Gateway"); break;
        case CHASSIS_TYPE_EMBEDDED_PC:            len += lb_snprintf(buf + len, sizeof(buf) - len, "Embedded PC"); break;
        case CHASSIS_TYPE_MINI_PC:                len += lb_snprintf(buf + len, sizeof(buf) - len, "Mini PC"); break;
        case CHASSIS_TYPE_STICK_PC:               len += lb_snprintf(buf + len, sizeof(buf) - len, "Stick PC"); break;
        default:                                  len += lb_snprintf(buf + len, sizeof(buf) - len, "Unknown Chassis Type"); break;
    }

    if (len >= 2 && buf[len - 2] == ',') buf[len - 2] = '\0'; // remove trailing ", " ONLY if present
    if (len == 0) return "None";

    return buf;
}

// Chassis State
const char* lazybiosType3StateStr(uint8_t state) {
    switch(state) {
        case CHASSIS_STATE_OTHER:           return "Other";
        case CHASSIS_STATE_UNKNOWN:         return "Unknown";
        case CHASSIS_STATE_SAFE:            return "Safe";
        case CHASSIS_STATE_WARNING:         return "Warning";
        case CHASSIS_STATE_CRITICAL:        return "Critical";
        case CHASSIS_STATE_NON_RECOVERABLE: return "Non-recoverable";
        default:                            return "Unknown Chassis State";
    }
}

// Chassis Status
const char* lazybiosType3SecurityStatusStr(uint8_t security_status) {
    switch(security_status) {
        case CHASSIS_SECURITY_STATUS_OTHER:                     return "Other";
        case CHASSIS_SECURITY_STATUS_UNKNOWN:                   return "Unknown";
        case CHASSIS_SECURITY_STATUS_NONE:                      return "None";
        case CHASSIS_SECURITY_STATUS_EXT_INTERFACE_LOCKED_OUT:  return "External interface locked out";
        case CHASSIS_SECURITY_STATUS_EXT_INTERFACE_ENABLED:     return "External interface enabled";
        default:                                                return "Unknown chassis security state";
    }
}

// Chassis Contained Elements
const char* lazybiosType3ContainedElementTypeStr(uint8_t contained_elements) {
    _Thread_local static char buf[128];
    buf[0] = '\0';

    if (contained_elements & 0x80) { // MSB = 1 → SMBIOS structure type
        uint8_t struct_type = contained_elements & 0x7F;
        lb_snprintf(buf, sizeof(buf), "SMBIOS Structure Type %u", struct_type);
    } else { // MSB = 0 → board type
        uint8_t board_type = contained_elements & 0x7F;
        lb_snprintf(buf, sizeof(buf), "%s", lazybiosType2BoardTypeStr(board_type));
    }

    return buf;
}

// End of Decoders

// Free Function
void lazybiosFreeType3(lazybiosType3_t* Type3) {
    if (!Type3) return;
    lb_free(Type3->manufacturer);
    lb_free(Type3->version);
    lb_free(Type3->serial_number);
    lb_free(Type3->asset_tag);
    lb_free(Type3->contained_elements);
    lb_free(Type3->sku_number);
    lb_free(Type3);
}

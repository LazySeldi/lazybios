//
// SMBIOS types 1, 2, 3
//
#include <stdio.h>
#include "lazybios.h"
#include <string.h>
#include <stdlib.h>

// Type 1 ( System Information )
system_info_t* lazybiosGetSystemInfo(lazybios_ctx_t* ctx) {
    if (!ctx || !ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybiosGetStructMinLength(ctx, SMBIOS_TYPE_SYSTEM);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_SYSTEM && length >= min_length) {
            const char *mfr = DMIString(p, length, p[SYS_MANUFACTURER_OFFSET]);
            const char *prod = DMIString(p, length, p[SYS_PRODUCT_OFFSET]);
            const char *ver = DMIString(p, length, p[SYS_VERSION_OFFSET]);
            const char *ser = DMIString(p, length, p[SYS_SERIAL_OFFSET]);

            ctx->system_info.manufacturer = strdup(mfr ? mfr : "Unknown");
            ctx->system_info.product_name = strdup(prod ? prod : "Unknown");
            ctx->system_info.version = strdup(ver ? ver : "Unknown");
            ctx->system_info.serial_number = strdup(ser ? ser : "Unknown");

            if (length >= 0x18) {
                const uint8_t *uuid = p + SYS_UUID_OFFSET;
                ctx->system_info.uuid = malloc(37);
                if (ctx->system_info.uuid) {
                    snprintf(ctx->system_info.uuid, 37,
                        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                        uuid[0], uuid[1], uuid[2], uuid[3],
                        uuid[4], uuid[5], uuid[6], uuid[7],
                        uuid[8], uuid[9], uuid[10], uuid[11],
                        uuid[12], uuid[13], uuid[14], uuid[15]);
                }
            } else {
                ctx->system_info.uuid = strdup("Not Available");
            }

            return &ctx->system_info;
        }
        p = DMINext(p, end);
    }
    return NULL;
}


// Type 2 ( Baseboard (or Motherboard) Information )
baseboard_info_t* lazybiosGetBaseboardInfo(lazybios_ctx_t* ctx) {
    if (!ctx || !ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybiosGetStructMinLength(ctx, SMBIOS_TYPE_BASEBOARD);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_BASEBOARD && length >= min_length) {
            ctx->baseboard_info.manufacturer = strdup(DMIString(p, length, p[BASEBOARD_MANUFACTURER_OFFSET]));
            ctx->baseboard_info.product = strdup(DMIString(p, length, p[BASEBOARD_PRODUCT_OFFSET]));
            ctx->baseboard_info.version = strdup(DMIString(p, length, p[BASEBOARD_VERSION_OFFSET]));
            ctx->baseboard_info.serial_number = strdup(DMIString(p, length, p[BASEBOARD_SERIAL_OFFSET]));
            ctx->baseboard_info.asset_tag = strdup(DMIString(p, length, p[BASEBOARD_ASSET_TAG_OFFSET]));
            return &ctx->baseboard_info;
        }
        p = DMINext(p, end);
    }
    return NULL;
}


// Type 3 ( System Enclosure/Chassis )
chassis_info_t* lazybiosGetChassisInfo(lazybios_ctx_t* ctx) {
    if (!ctx || !ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybiosGetStructMinLength(ctx, SMBIOS_TYPE_CHASSIS);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_CHASSIS && length >= min_length) {
            if (length > CHASSIS_ASSET_TAG_OFFSET) {
                ctx->chassis_info.asset_tag = strdup(DMIString(p, length, p[CHASSIS_ASSET_TAG_OFFSET]));
            } else {
                ctx->chassis_info.asset_tag = strdup("Not Specified");
            }

            if (length > CHASSIS_SKU_OFFSET) {
                ctx->chassis_info.sku = strdup(DMIString(p, length, p[CHASSIS_SKU_OFFSET]));
            } else {
                ctx->chassis_info.sku = strdup("Not Specified");
            }

            if (length >= 0x05) {
                ctx->chassis_info.type = p[0x05];
                ctx->chassis_info.state = p[0x06];
            }

            return &ctx->chassis_info;
        }
        p = DMINext(p, end);
    }
    return NULL;
}

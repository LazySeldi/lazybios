//
// SMBIOS Types 8, 9, 10
//
#include "lazybios.h"
#include <stdlib.h>
#include <string.h>

// Type 8 ( Port Connector Information )
port_connector_info_t* lazybiosGetPortConnectorInfo(lazybios_ctx_t* ctx, size_t* count) {
    if (!ctx) {
        *count = 0;
        return NULL;
    }

    if (ctx->port_connector_ptr) {
        *count = ctx->port_connector_count;
        return ctx->port_connector_ptr;
    }

    if (!ctx->dmi_data) {
        *count = 0;
        return NULL;
    }

    ctx->port_connector_count = lazybiosCountStructsByType(ctx, SMBIOS_TYPE_PORT_CONNECTOR);
    if (ctx->port_connector_count == 0) {
        *count = 0;
        return NULL;
    }

    ctx->port_connector_ptr = calloc(ctx->port_connector_count, sizeof(port_connector_info_t));
    if (!ctx->port_connector_ptr) {
        *count = 0;
        return NULL;
    }

    size_t index = 0;
    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybiosGetStructMinLength(ctx, SMBIOS_TYPE_PORT_CONNECTOR);

    while (p + SMBIOS_HEADER_SIZE < end && index < ctx->port_connector_count) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_PORT_CONNECTOR && length >= min_length) {
            port_connector_info_t *current = &ctx->port_connector_ptr[index];

            current->port_type = p[PORT_OFFSET_PORT_TYPE];
            current->handle = *(uint16_t*)(p + PORT_OFFSET_HANDLE);
            current->external_connector_type = p[PORT_OFFSET_EXTERNAL_CONNECTOR_TYPE];
            current->internal_connector_type = p[PORT_OFFSET_INTERNAL_CONNECTOR_TYPE];
            current->external_ref_designator = strdup(DMIString(p, length, p[PORT_OFFSET_EXTERNAL_REF_DESIGNATOR]));
            current->internal_ref_designator = strdup(DMIString(p, length, p[PORT_OFFSET_INTERNAL_REF_DESIGNATOR]));

            index++;
        }
        p = DMINext(p, end);
    }

    *count = ctx->port_connector_count;
    return ctx->port_connector_ptr;
}

// Type 10 ( Onboard Devices Information )
onboard_devices_t* lazybiosGetOnboardDeviceInfo(lazybios_ctx_t* ctx, size_t* count) {
    if (!ctx) {
        *count = 0;
        return NULL;
    }

    if (ctx->onboard_devices_ptr) {
        *count = ctx->onboard_devices_count;
        return ctx->onboard_devices_ptr;
    }

    if (!ctx->dmi_data) {
        *count = 0;
        return NULL;
    }

    ctx->onboard_devices_count = lazybiosCountStructsByType(ctx, SMBIOS_TYPE_ONBOARD_DEVICES);
    if (ctx->onboard_devices_count == 0) {
        *count = 0;
        return NULL;
    }

    ctx->onboard_devices_ptr = calloc(ctx->onboard_devices_count, sizeof(onboard_devices_t));
    if (!ctx->onboard_devices_ptr) {
        *count = 0;
        return NULL;
    }

    size_t index = 0;
    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybiosGetStructMinLength(ctx, SMBIOS_TYPE_ONBOARD_DEVICES);

    while (p + SMBIOS_HEADER_SIZE < end && index < ctx->onboard_devices_count) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_ONBOARD_DEVICES && length >= min_length) {
            size_t dev_count = ONBOARD_DEV_COUNT(length);

            for (size_t i = 0; i < dev_count && index < ctx->onboard_devices_count; i++) {
                uint8_t raw = p[ONBOARD_DEV_TYPE_OFFSET(i + 1)];
                uint8_t str_index = p[ONBOARD_DEV_STR_OFFSET(i + 1)];

                bool enabled = raw & 0x80;
                uint8_t dev_type = raw & 0x7F;

                ctx->onboard_devices_ptr[index].enabled = enabled;
                ctx->onboard_devices_ptr[index].type = dev_type;
                ctx->onboard_devices_ptr[index].description_string = strdup(DMIString(p, length, str_index));

                index++;
            }
        }
        p = DMINext(p, end);
    }

    // Update actual count in case we found fewer devices than structures
    ctx->onboard_devices_count = index;
    *count = ctx->onboard_devices_count;
    return ctx->onboard_devices_ptr;
}

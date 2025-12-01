//
// SMBIOS Entry Info + Type 0
//
#include <string.h>
#include "lazybios.h"

// Entry Information
const smbios_entry_info_t* lazybiosGetEntryInfo(const lazybios_ctx_t* ctx) {
    if (!ctx) return NULL;
    return &ctx->entry_info;
}

// Type 0 ( BIOS Information )
bios_info_t* lazybiosGetBIOSInfo(lazybios_ctx_t* ctx) {
    if (!ctx || !ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybiosGetStructMinLength(ctx, SMBIOS_TYPE_BIOS);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_BIOS && length >= min_length) {
            ctx->bios_info.vendor = strdup(DMIString(p, length, p[BIOS_VENDOR_OFFSET]));
            ctx->bios_info.version = strdup(DMIString(p, length, p[BIOS_VERSION_OFFSET]));
            ctx->bios_info.release_date = strdup(DMIString(p, length, p[BIOS_RELEASE_DATE_OFFSET]));

            uint8_t rom_size = p[BIOS_ROM_SIZE_OFFSET];
            if (rom_size == 0xFF) {
                ctx->bios_info.rom_size_kb = 0; // Size not specified
            } else {
                ctx->bios_info.rom_size_kb = (rom_size + 1) * BIOS_ROM_SIZE_MULTIPLIER;
            }

            return &ctx->bios_info;
        }
        p = DMINext(p, end);
    }
    return NULL;
}

//
// SMBIOS Types 11, 12, 13, 14, 15
//
#include "lazybios.h"
#include <string.h>
#include <stdlib.h>

// Type 11 ( OEM Strings )
OEMStrings_t* lazybiosGetOEMStringInfo(lazybios_ctx_t* ctx, size_t* count)
{
    if (!ctx) {
        *count = 0;
        return NULL;
    }

    if (ctx->OEMStrings_ptr) {
        *count = ctx->OEMStrings_count;
        return ctx->OEMStrings_ptr;
    }

    if (!ctx->dmi_data) {
        *count = 0;
        return NULL;
    }

    const uint8_t *p      = ctx->dmi_data;
    const uint8_t *end    = ctx->dmi_data + ctx->dmi_len;
    size_t min_len        = lazybiosGetStructMinLength(ctx, SMBIOS_TYPE_OEM_STRINGS);
    uint8_t str_cnt       = 0;
    while (p + SMBIOS_HEADER_SIZE < end) {
        if (p[0] == SMBIOS_TYPE_END) break;
        if (p[0] == SMBIOS_TYPE_OEM_STRINGS && p[1] >= min_len) {
            str_cnt = p[OEM_STRINGS_COUNT_OFFSET];
            break;
        }
        p = DMINext(p, end);
    }
    if (str_cnt == 0) { // not found
        *count = 0;
        return NULL;
    }

    // allocate one struct + room for str_cnt pointers
    ctx->OEMStrings_count = 1;
    ctx->OEMStrings_ptr = calloc(1, sizeof(OEMStrings_t) + str_cnt * sizeof(char *));
    if (!ctx->OEMStrings_ptr) {
        *count = 0;
        return NULL;
    }

    // we re-walk to the structure and then fill it
    p = ctx->dmi_data;
    while (p + SMBIOS_HEADER_SIZE < end) {
        if (p[0] == SMBIOS_TYPE_END) break;
        if (p[0] == SMBIOS_TYPE_OEM_STRINGS && p[1] >= min_len) {
            OEMStrings_t *cur = ctx->OEMStrings_ptr;
            cur->string_count = str_cnt;

            size_t stroff = OEM_STRINGS_COUNT_OFFSET + 1;
            for (int i = 0; i < cur->string_count; ++i) {
                const char *src = (const char *)(p + stroff);
                cur->strings[i] = strdup(src);
                if (!cur->strings[i]) {
                    *count = 0;
                    lazybiosCleanup(ctx);
                    return NULL;
                }
                stroff += strlen(src) + 1;
            }
            break;
        }
        p = DMINext(p, end);
    }

    *count = ctx->OEMStrings_count;
    return ctx->OEMStrings_ptr;
}

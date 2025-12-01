//
// SMBIOS Types 4, 7
//
#include "lazybios.h"
#include <string.h>
#include <stdlib.h>

// Type 4 ( Processor Information )
processor_info_t* lazybiosGetProcInfo(lazybios_ctx_t* ctx) {
    if (!ctx || !ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybiosGetStructMinLength(ctx, SMBIOS_TYPE_PROCESSOR);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_PROCESSOR && length >= min_length) {
            ctx->processor_info.socket_designation = strdup(DMIString(p, length, p[PROC_SOCKET_OFFSET]));
            ctx->processor_info.version = strdup(DMIString(p, length, p[PROC_VERSION_OFFSET]));
            ctx->processor_info.max_speed_mhz = *(uint16_t*)(p + PROC_MAX_SPEED_OFFSET);
            ctx->processor_info.L1_cache_handle = *(uint16_t*)(p + PROC_L1_CACHE_HANDLE_OFFSET);
            ctx->processor_info.L2_cache_handle = *(uint16_t*)(p + PROC_L2_CACHE_HANDLE_OFFSET);
            ctx->processor_info.L3_cache_handle = *(uint16_t*)(p + PROC_L3_CACHE_HANDLE_OFFSET);
            ctx->processor_info.proc_upgrade = *(uint16_t*)(p + PROC_UPGRADE_OFFSET);

            // Current speed (added)
            if (length > PROC_CURRENT_SPEED_OFFSET + 1) {
                ctx->processor_info.current_speed_mhz = *(uint16_t*)(p + PROC_CURRENT_SPEED_OFFSET);
            } else {
                ctx->processor_info.current_speed_mhz = 0;
            }

            ctx->processor_info.external_clock_mhz = *(uint16_t*)(p + PROC_EXTERNAL_CLOCK_OFFSET);
            ctx->processor_info.voltage = p[PROC_VOLTAGE_OFFSET];
            ctx->processor_info.processor_type = p[PROC_TYPE_OFFSET];
            ctx->processor_info.processor_family = p[PROC_FAMILY_OFFSET];

            // Status
            if (length > PROC_STATUS_OFFSET) {
                ctx->processor_info.status = p[PROC_STATUS_OFFSET];
            } else {
                ctx->processor_info.status = 0;
            }

            // Core count + Extended(If available)
            uint8_t core_count_byte = p[PROC_CORE_COUNT_OFFSET];
            if (core_count_byte == 0xFF && length > PROC_CORE_COUNT2_OFFSET + 1) {
                ctx->processor_info.core_count = *(uint16_t*)(p + PROC_CORE_COUNT2_OFFSET);
            } else {
                ctx->processor_info.core_count = core_count_byte;
            }

            // Cores enabled + Extended
            uint8_t core_enabled_byte = p[PROC_CORE_ENABLED_OFFSET];
            if (core_enabled_byte == 0xFF && length > PROC_CORE_ENABLED2_OFFSET + 1) {
                ctx->processor_info.core_enabled = *(uint16_t*)(p + PROC_CORE_ENABLED2_OFFSET);
            } else {
                ctx->processor_info.core_enabled = core_enabled_byte;
            }

            // Thread count with extended support
            uint8_t thread_count_byte = p[PROC_THREAD_COUNT_OFFSET];
            if (thread_count_byte == 0xFF && length > PROC_THREAD_COUNT2_OFFSET + 1) {
                ctx->processor_info.thread_count = *(uint16_t*)(p + PROC_THREAD_COUNT2_OFFSET);
            } else {
                ctx->processor_info.thread_count = thread_count_byte;
            }

            // Extended processor family
            if (ctx->processor_info.processor_family == 0xFE && length > PROC_FAMILY2_OFFSET + 1) {
                ctx->processor_info.processor_family2 = *(uint16_t*)(p + PROC_FAMILY2_OFFSET);
            } else {
                ctx->processor_info.processor_family2 = ctx->processor_info.processor_family;
            }

            // Serial, asset tag, part number
            if (length > PROC_SERIAL_OFFSET_2_5) {
                ctx->processor_info.serial_number = strdup(DMIString(p, length, p[PROC_SERIAL_OFFSET_2_5]));
            } else {
                ctx->processor_info.serial_number = strdup("Not Specified");
            }

            if (length > PROC_ASSET_TAG_OFFSET_2_5) {
                ctx->processor_info.asset_tag = strdup(DMIString(p, length, p[PROC_ASSET_TAG_OFFSET_2_5]));
            } else {
                ctx->processor_info.asset_tag = strdup("Not Specified");
            }

            if (length > PROC_PART_NUMBER_OFFSET_2_5) {
                ctx->processor_info.part_number = strdup(DMIString(p, length, p[PROC_PART_NUMBER_OFFSET_2_5]));
            } else {
                ctx->processor_info.part_number = strdup("Not Specified");
            }

            if (length > PROC_CHARACTERISTICS_OFFSET + 1) {
                ctx->processor_info.characteristics = *(uint16_t*)(p + PROC_CHARACTERISTICS_OFFSET);
            } else {
                ctx->processor_info.characteristics = 0;
            }

            return &ctx->processor_info;
        }
        p = DMINext(p, end);
    }
    return NULL;
}

// Type 7 ( Cache Information )
cache_info_t* lazybiosGetCacheInfo(lazybios_ctx_t* ctx, size_t* count) {
    if (!ctx) {
        *count = 0;
        return NULL;
    }

    if (ctx->caches_ptr) {
        *count = ctx->caches_count;
        return ctx->caches_ptr;
    }

    if (!ctx->dmi_data) {
        *count = 0;
        return NULL;
    }

    ctx->caches_count = lazybiosCountStructsByType(ctx, SMBIOS_TYPE_CACHES);
    if (ctx->caches_count == 0) {
        *count = 0;
        return NULL;
    }

    ctx->caches_ptr = calloc(ctx->caches_count, sizeof(cache_info_t));
    if (!ctx->caches_ptr) {
        *count = 0;
        return NULL;
    }

    size_t index = 0;
    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybiosGetStructMinLength(ctx, SMBIOS_TYPE_CACHES);
    bool is_31_plus = (ctx->entry_info.major > 3) || (ctx->entry_info.major == 3 && ctx->entry_info.minor >= 1);

    while (p + SMBIOS_HEADER_SIZE < end && index < ctx->caches_count) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_CACHES && length >= min_length) {
            cache_info_t *current = &ctx->caches_ptr[index];

            const char *sock = DMIString(p, length, p[CACHE_SOCKET_DESIGNATION]);
            current->socket_designation = strdup(sock ? sock : "Unknown");

            uint16_t config = *(uint16_t*)(p + CACHE_CONFIGURATION);
            current->level = config & 0x07;

            uint16_t max_size = *(uint16_t*)(p + CACHE_MAXIMUM_SIZE);
            uint16_t installed_size = *(uint16_t*)(p + CACHE_INSTALLED_SIZE);

            if (is_31_plus && length >= CACHE_MIN_LENGTH_3_1) {
                if (max_size == 0xFFFF || (max_size & 0x8000)) {
                    current->size_kb = *(uint32_t*)(p + CACHE_MAXIMUM_SIZE_2);
                } else {
                    uint16_t granularity = (max_size & 0x8000) ? 64 : 1;
                    current->size_kb = (max_size & 0x7FFF) * granularity;
                }
            } else {
                uint16_t granularity = (installed_size & 0x8000) ? 64 : 1;
                current->size_kb = (installed_size & 0x7FFF) * granularity;
            }

            if (length > CACHE_ERROR_CORRECTION_TYPE) {
                current->error_correction_type = p[CACHE_ERROR_CORRECTION_TYPE];
            }
            if (length > CACHE_SYSTEM_CACHE_TYPE) {
                current->system_cache_type = p[CACHE_SYSTEM_CACHE_TYPE];
            }
            if (length > CACHE_ASSOCIATIVITY) {
                current->associativity = p[CACHE_ASSOCIATIVITY];
            }

            index++;
        }
        p = DMINext(p, end);
    }

    *count = ctx->caches_count;
    return ctx->caches_ptr;
}

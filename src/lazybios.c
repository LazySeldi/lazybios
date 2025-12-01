//
// lazybios.c - Core library functions
//
#include "../include/lazybios.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Logging system for lazybios

// Quiet mode - disable ALL logging
#ifdef LAZYBIOS_QUIET
# define lb_log(...)      ((void)0)
# define lb_dbg(...)      ((void)0)

// Debug mode - enable verbose debugging
#elif defined(LAZYBIOS_DEBUG)
# define lb_log(fmt, ...) fprintf(stderr, "[lazybios] " fmt "\n"
, ##__VA_ARGS__)
# define lb_dbg(fmt, ...) fprintf(stderr, "[lazybios-dbg] " fmt "\n", ##__VA_ARGS__)

// Normal mode - standard logging only
#else
# define lb_log(fmt, ...) fprintf(stderr, "[lazybios] " fmt "\n", ##__VA_ARGS__)
# define lb_dbg(...)      ((void)0)
#endif


// ===== Context Management =====
lazybios_ctx_t* lazybiosCTXNew(void) {
    lazybios_ctx_t* ctx = calloc(1, sizeof(lazybios_ctx_t));
    if (!ctx) {
        lb_log("Failed to allocate context");
        return NULL;
    }
    return ctx;
}

// ===== Library Initialization =====
int lazybiosInit(lazybios_ctx_t* ctx) {
    if (!ctx) {
        lb_log("Null context provided");
        return -1;
    }

    FILE *entry = fopen(SMBIOS_ENTRY, "rb");
    if (!entry) {
        lb_log("failed to open %s: %s", SMBIOS_ENTRY, strerror(errno));
        return -1;
    }

    FILE *dmi = fopen(DMI_TABLE, "rb");
    if (!dmi) {
        lb_log("Failed to open %s: %s", DMI_TABLE, strerror(errno));
        fclose(entry);
        return -1;
    }

    uint8_t entry_buf[64];
    size_t n = fread(entry_buf, 1, sizeof(entry_buf), entry);
    fclose(entry);

    if (n < 20) {
        lb_log("Invalid entry point size (%zu bytes)", n);
        fclose(dmi);
        return -1;
    }

    if (lazybiosParseEntry(ctx, entry_buf) != 0) {
        lb_log("No valid SMBIOS anchor\n");
        return -1;
    }

    fseek(dmi, 0, SEEK_END);
    ctx->dmi_len = ftell(dmi);
    rewind(dmi);

    if (ctx->dmi_len == 0) {
        lb_log("Empty DMI table");
        lb_dbg("DMI length: %zu", ctx->dmi_len);
        fclose(dmi);
        return -1;
    }

    ctx->dmi_data = malloc(ctx->dmi_len);
    if (!ctx->dmi_data) {
        lb_log("Failed to allocate %zu bytes for DMI buffer", ctx->dmi_len);
        lb_dbg("malloc(%zu) returned NULL (errno=%d: (%s)", ctx->dmi_len, errno, strerror(errno));
        fclose(dmi);
        return -1;
    }

    size_t got = fread(ctx->dmi_data, 1, ctx->dmi_len, dmi);
    if (got != ctx->dmi_len) {
        lb_log("Failed to read DMI table");
        lb_dbg("short read: got %zu / %zu bytes, errno=%d (%s)", got, ctx->dmi_len, errno, strerror(errno));
        free(ctx->dmi_data);
        ctx->dmi_data = NULL;
        fclose(dmi);
        return -1;
    }

    fclose(dmi);
    return 0;
}

// ===== Version-Aware Helpers =====
size_t lazybiosGetStructMinLength(const lazybios_ctx_t* ctx, uint8_t type) {
    if (!ctx) {
        lb_log("Context not loaded!");
        return -1;
    }

    bool is_64bit = ctx->entry_info.is_64bit;

    switch(type) {
        case SMBIOS_TYPE_BIOS:
            return is_64bit ? BIOS_MIN_LENGTH_3_0 : BIOS_MIN_LENGTH_2_0;

        case SMBIOS_TYPE_SYSTEM:
            return is_64bit ? SYSTEM_MIN_LENGTH_3_0 : SYSTEM_MIN_LENGTH_2_0;

        case SMBIOS_TYPE_BASEBOARD:
            return BASEBOARD_MIN_LENGTH;

        case SMBIOS_TYPE_CHASSIS:
            return is_64bit ? CHASSIS_MIN_LENGTH_3_0 : CHASSIS_MIN_LENGTH_2_0;

        case SMBIOS_TYPE_PROCESSOR:
            return (ctx->entry_info.major > 2 || (ctx->entry_info.major == 2 && ctx->entry_info.minor >= 6))
                   ? PROC_MIN_LENGTH_2_6 : PROC_MIN_LENGTH_2_0;

        case SMBIOS_TYPE_CACHES:
            return (ctx->entry_info.major > 2 || (ctx->entry_info.major == 2 && ctx->entry_info.minor >= 1))
                   ? CACHE_MIN_LENGTH_2_1 : CACHE_MIN_LENGTH_2_0;

        case SMBIOS_TYPE_ONBOARD_DEVICES:
            return ONBOARD_DEVICES_MIN_LENGTH;

        case SMBIOS_TYPE_OEM_STRINGS:
            return OEM_STRINGS_MIN_LENGTH;

        case SMBIOS_TYPE_PORT_CONNECTOR:
            return PORT_CONNECTOR_MIN_LENGTH;

        case SMBIOS_TYPE_MEMARRAY:
            return MEMARRAY_MIN_LENGTH;

        case SMBIOS_TYPE_MEMDEVICE:
            return MEMORY_MIN_LENGTH;

        default:
            return 4;
    }
}

// Entry Point Version Parser
int lazybiosParseEntry(lazybios_ctx_t* ctx, const uint8_t* buf) {
    if (!ctx) {
        lb_log("Context not loaded!");
    }
    if (memcmp(buf, SMBIOS3_ANCHOR, 5) == 0) {
        ctx->entry_info.major = buf[ENTRY3_MAJOR_OFFSET];
        ctx->entry_info.minor = buf[ENTRY3_MINOR_OFFSET];
        ctx->entry_info.docrev = buf[ENTRY3_DOCREV_OFFSET];
        ctx->entry_info.table_length = *(uint32_t*)(buf + ENTRY3_TABLE_LENGTH_OFFSET);
        ctx->entry_info.table_address = *(uint64_t*)(buf + ENTRY3_TABLE_ADDR_OFFSET);
        ctx->entry_info.is_64bit = true;
    } else if (memcmp(buf, SMBIOS2_ANCHOR, 4) == 0) {
        ctx->entry_info.major = buf[ENTRY2_MAJOR_OFFSET];
        ctx->entry_info.minor = buf[ENTRY2_MINOR_OFFSET];
        ctx->entry_info.docrev = 0;  // Not available in SMBIOS 2.x
        ctx->entry_info.table_length = *(uint16_t*)(buf + ENTRY2_TABLE_LENGTH_OFFSET);
        ctx->entry_info.table_address = *(uint32_t*)(buf + ENTRY2_TABLE_ADDR_OFFSET);
        ctx->entry_info.is_64bit = false;
    } else {
        lb_log("Couldn't find SMBIOS version!");
        lb_dbg("anchor bytes: %02X %02X %02X %02X %02X  (SMBIOS-2 only uses first 4)", buf[0], buf[1], buf[2], buf[3], buf[4]);
        return -1;
    }
    return 0;
}

// Find all Strings in given "string_index"
const char* DMIString(const uint8_t *formatted, int length, int string_index) {
    if (string_index == 0) return "Not Specified";

    const char *str = (const char *)(formatted + length);

    for (int i = 1; i < string_index; i++) {
        // Skip to the end of current string
        while (*str) str++;
        str++; // Skip the null terminator

        // Check if we've run out of strings
        if (!*str) return "Not Specified";
    }
    return str;
}

// Next structure
const uint8_t* DMINext(const uint8_t *ptr, const uint8_t *end) {
    const uint8_t *p = ptr + ptr[1];
    while (p + 1 < end && (p[0] || p[1])) p++;
    return p + 2;
}

// Generic Structure Parser
size_t lazybiosCountStructsByType(const lazybios_ctx_t* ctx, uint8_t target_type) {
    if (!ctx || !ctx->dmi_data) return 0;

    size_t count = 0;
    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybiosGetStructMinLength(ctx, target_type);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        if (type == SMBIOS_TYPE_END) break;

        if (type == target_type && p[1] >= min_length) {
            count++;
        }
        p = DMINext(p, end);
    }
    return count;
}

// Cleanup
void lazybiosCleanup(lazybios_ctx_t* ctx) {
    if (!ctx) return;

    free(ctx->bios_info.vendor);
    free(ctx->bios_info.version);
    free(ctx->bios_info.release_date);

    free(ctx->system_info.manufacturer);
    free(ctx->system_info.product_name);
    free(ctx->system_info.version);
    free(ctx->system_info.serial_number);
    free(ctx->system_info.uuid);

    free(ctx->baseboard_info.manufacturer);
    free(ctx->baseboard_info.product);
    free(ctx->baseboard_info.version);
    free(ctx->baseboard_info.serial_number);
    free(ctx->baseboard_info.asset_tag);

    free(ctx->chassis_info.asset_tag);
    free(ctx->chassis_info.sku);

    free(ctx->processor_info.socket_designation);
    free(ctx->processor_info.version);
    free(ctx->processor_info.serial_number);
    free(ctx->processor_info.asset_tag);
    free(ctx->processor_info.part_number);

    if (ctx->caches_ptr) {
        for (size_t i = 0; i < ctx->caches_count; i++) {
            free(ctx->caches_ptr[i].socket_designation);
        }
        free(ctx->caches_ptr);
        ctx->caches_ptr = NULL;
    }
    ctx->caches_count = 0;

    if (ctx->port_connector_ptr) {
        for (size_t i = 0; i < ctx->port_connector_count; i++) {
            free(ctx->port_connector_ptr[i].external_ref_designator);
            free(ctx->port_connector_ptr[i].internal_ref_designator);
        }
        free(ctx->port_connector_ptr);
        ctx->port_connector_ptr = NULL;
    }
    ctx->caches_count = 0;

    if (ctx->onboard_devices_ptr) {
        for (size_t i = 0; i < ctx->onboard_devices_count; i++) {
            free(ctx->onboard_devices_ptr[i].description_string);
        }
        free(ctx->onboard_devices_ptr);
        ctx->onboard_devices_ptr = NULL;
    }
    ctx->onboard_devices_count = 0;

    if (ctx->OEMStrings_ptr) {
        OEMStrings_t *os = ctx->OEMStrings_ptr; // Did this just to not do ctx->OEMStrings_ptr->strings[i]), every compiler optimizes this anyways so no big deal;
        for (size_t i = 0; i < os->string_count; ++i)
            free(os->strings[i]);
        free(os);
        ctx->OEMStrings_ptr   = NULL;
        ctx->OEMStrings_count = 0;
    }
    ctx->OEMStrings_count = 0;

    if (ctx->memory_arrays_ptr) {
        free(ctx->memory_arrays_ptr);
        ctx->memory_arrays_ptr = NULL;
    }
    ctx->memory_arrays_count = 0;

    if (ctx->memory_devices_ptr) {
        for (size_t i = 0; i < ctx->memory_devices_count; i++) {
            free(ctx->memory_devices_ptr[i].locator);
            free(ctx->memory_devices_ptr[i].bank_locator);
            free(ctx->memory_devices_ptr[i].manufacturer);
            free(ctx->memory_devices_ptr[i].serial_number);
            free(ctx->memory_devices_ptr[i].part_number);
        }
        free(ctx->memory_devices_ptr);
        ctx->memory_devices_ptr = NULL;
    }
    ctx->memory_devices_count = 0;

    if (ctx->dmi_data) {
        free(ctx->dmi_data);
        ctx->dmi_data = NULL;
    }
    ctx->dmi_len = 0;

    free(ctx);
}

// Small Helper
void lazybiosPrintVer(const lazybios_ctx_t* ctx) {
    if (!ctx) return;
    if (ctx->entry_info.is_64bit) {
        printf("SMBIOS version %d.%d.%d\n",
               ctx->entry_info.major,
               ctx->entry_info.minor,
               ctx->entry_info.docrev);
    } else {
        printf("SMBIOS version %d.%d\n",
               ctx->entry_info.major,
               ctx->entry_info.minor);
    }
}

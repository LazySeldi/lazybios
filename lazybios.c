#include "lazybios.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ===== Context Management =====
lazybios_ctx_t* lazybios_ctx_new(void) {
    lazybios_ctx_t* ctx = calloc(1, sizeof(lazybios_ctx_t));
    return ctx;
}

void lazybios_ctx_free(lazybios_ctx_t* ctx) {
    if (!ctx) return;
    lazybios_cleanup(ctx);
    free(ctx);
}

// ===== Version-Aware Helpers =====
size_t lazybios_get_smbios_structure_min_length(const lazybios_ctx_t* ctx, uint8_t type) {
    if (!ctx) return 4;

    switch(type) {
        case SMBIOS_TYPE_BIOS:
            return (ctx->entry_info.major >= 3) ? BIOS_MIN_LENGTH_3_0 : BIOS_MIN_LENGTH_2_0;
        case SMBIOS_TYPE_SYSTEM:
            return (ctx->entry_info.major >= 3) ? SYSTEM_MIN_LENGTH_3_0 : SYSTEM_MIN_LENGTH_2_0;
        case SMBIOS_TYPE_CHASSIS:
            return (ctx->entry_info.major >= 3) ? CHASSIS_MIN_LENGTH_3_0 : CHASSIS_MIN_LENGTH_2_0;
        case SMBIOS_TYPE_PROCESSOR:
            return (ctx->entry_info.major > 2 || (ctx->entry_info.major == 2 && ctx->entry_info.minor >= 6))
                   ? PROC_MIN_LENGTH_2_6 : PROC_MIN_LENGTH_2_0;
        case SMBIOS_TYPE_MEMDEVICE:
            return (ctx->entry_info.major >= 3) ? MEMORY_MIN_LENGTH_3_0 : MEMORY_MIN_LENGTH_2_0;
        default:
            return 4;
    }
}

bool lazybios_is_smbios_version_at_least(const lazybios_ctx_t* ctx, uint8_t major, uint8_t minor) {
    if (!ctx) return false;
    return (ctx->entry_info.major > major) ||
           (ctx->entry_info.major == major && ctx->entry_info.minor >= minor);
}

// ===== Comprehensive Processor Family String Mapping =====
const char* lazybios_get_processor_family_string(uint8_t family) {
    switch(family) {
        // Intel Families
        case INTEL_8086: return "8086";
        case INTEL_80286: return "80286";
        case INTEL_80386: return "80386";
        case INTEL_80486: return "80486";
        case INTEL_PENTIUM: return "Pentium";
        case INTEL_PENTIUM_PRO: return "Pentium Pro";
        case INTEL_PENTIUM_II: return "Pentium II";
        case INTEL_PENTIUM_III: return "Pentium III";
        case INTEL_PENTIUM_4: return "Pentium 4";
        case INTEL_PENTIUM_M: return "Pentium M";
        case INTEL_CELERON: return "Celeron";
        case INTEL_CORE: return "Core";
        case INTEL_CORE_2: return "Core 2";
        case INTEL_CORE_I3: return "Core i3";
        case INTEL_CORE_I5: return "Core i5";
        case INTEL_CORE_I7: return "Core i7";
        case INTEL_CORE_I9: return "Core i9";
        case INTEL_ATOM: return "Atom";
        case INTEL_XEON: return "Xeon";
        case INTEL_XEON_MP: return "Xeon MP";
        case INTEL_XEON_3XXX: return "Xeon 3xxx Series";
        case INTEL_XEON_5XXX: return "Xeon 5xxx Series";
        case INTEL_XEON_7XXX: return "Xeon 7xxx Series";

        // AMD Families
        case AMD_K5: return "K5";
        case AMD_K6: return "K6";
        case AMD_ATHLON: return "Athlon";
        case AMD_ATHLON_64: return "Athlon 64";
        case AMD_OPTERON: return "Opteron";
        case AMD_SEMPRON: return "Sempron";
        case AMD_TURION: return "Turion";
        case AMD_PHENOM: return "Phenom";
        case AMD_PHENOM_II: return "Phenom II";
        case AMD_ATHLON_II: return "Athlon II";
        case AMD_FX: return "FX Series";
        case AMD_RYZEN_3: return "Ryzen 3";
        case AMD_RYZEN_5: return "Ryzen 5";
        case AMD_RYZEN_7: return "Ryzen 7";
        case AMD_RYZEN_9: return "Ryzen 9";
        case AMD_RYZEN_THREADRIPPER: return "Ryzen Threadripper";
        case AMD_EPYC: return "EPYC";

        // Other Architectures
        case ARM_CORTEX: return "ARM Cortex";
        case ARM_APPLE_M1: return "Apple M1";
        case ARM_APPLE_M2: return "Apple M2";
        case IBM_POWER: return "IBM POWER";
        case IBM_POWERPC: return "PowerPC";

        default: {
            static char unknown_family[32];
            snprintf(unknown_family, sizeof(unknown_family), "Unknown (0x%02X)", family);
            return unknown_family;
        }
    }
}

// ===== Core Parsing Functions =====
static int parse_smbios2_entry(lazybios_ctx_t* ctx, const uint8_t *buf) {
    if (memcmp(buf, SMBIOS2_ANCHOR, 4) != 0) return -1;

    ctx->entry_info.major = buf[ENTRY_MAJOR_OFFSET];
    ctx->entry_info.minor = buf[ENTRY_MINOR_OFFSET];
    ctx->entry_info.table_length = *(uint16_t*)(buf + ENTRY_TABLE_LENGTH_OFFSET);
    ctx->entry_info.table_address = *(uint32_t*)(buf + ENTRY_TABLE_ADDR_OFFSET);
    ctx->entry_info.is_64bit = false;

    return 0;
}

static int parse_smbios3_entry(lazybios_ctx_t* ctx, const uint8_t *buf) {
    if (memcmp(buf, SMBIOS3_ANCHOR, 5) != 0) return -1;

    ctx->entry_info.major = buf[ENTRY_MAJOR_OFFSET];
    ctx->entry_info.minor = buf[ENTRY_MINOR_OFFSET];
    ctx->entry_info.table_length = *(uint32_t*)(buf + ENTRY_TABLE_LENGTH_OFFSET_3);
    ctx->entry_info.table_address = *(uint64_t*)(buf + ENTRY_TABLE_ADDR_OFFSET_3);
    ctx->entry_info.is_64bit = true;

    return 0;
}

static const char* dmi_string(const uint8_t *formatted, int length, int string_index) {
    if (string_index == 0) return "N/A";
    const char *str = (const char *)(formatted + length);
    for (int i = 1; i < string_index; i++) {
        while (*str) str++;
        str++;
        if (!*str) return "N/A";
    }
    return str;
}

static const uint8_t* dmi_next(const uint8_t *ptr, const uint8_t *end) {
    const uint8_t *p = ptr + ptr[1];
    while (p + 1 < end && (p[0] || p[1])) p++;
    return p + 2;
}

// ===== Generic Structure Parser =====
static size_t count_structures_by_type(const lazybios_ctx_t* ctx, uint8_t target_type) {
    if (!ctx || !ctx->dmi_data) return 0;

    size_t count = 0;
    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, target_type);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        if (type == SMBIOS_TYPE_END) break;

        if (type == target_type && p[1] >= min_length) {
            count++;
        }
        p = dmi_next(p, end);
    }
    return count;
}

// ===== Library Initialization =====
int lazybios_init(lazybios_ctx_t* ctx) {
    if (!ctx) return -1;

    FILE *entry = fopen(SMBIOS_ENTRY, "rb");
    FILE *dmi = fopen(DMI_TABLE, "rb");
    if (!entry || !dmi) {
        perror("lazybios: failed to open DMI tables");
        if (entry) fclose(entry);
        if (dmi) fclose(dmi);
        return -1;
    }

    uint8_t entry_buf[64];
    size_t n = fread(entry_buf, 1, sizeof(entry_buf), entry);
    fclose(entry);

    if (n < 20) {
        fprintf(stderr, "lazybios: invalid entry point size\n");
        fclose(dmi);
        return -1;
    }

    if (parse_smbios3_entry(ctx, entry_buf) == 0) {
        printf("lazybios: detected SMBIOS 3.x (%d.%d)\n", ctx->entry_info.major, ctx->entry_info.minor);
    } else if (parse_smbios2_entry(ctx, entry_buf) == 0) {
        printf("lazybios: detected SMBIOS 2.x (%d.%d)\n", ctx->entry_info.major, ctx->entry_info.minor);
    } else {
        fprintf(stderr, "lazybios: unknown SMBIOS anchor\n");
        fclose(dmi);
        return -1;
    }

    fseek(dmi, 0, SEEK_END);
    ctx->dmi_len = ftell(dmi);
    rewind(dmi);

    ctx->dmi_data = malloc(ctx->dmi_len);
    if (!ctx->dmi_data) {
        perror("lazybios: failed to allocate DMI buffer");
        fclose(dmi);
        return -1;
    }

    if (fread(ctx->dmi_data, 1, ctx->dmi_len, dmi) != ctx->dmi_len) {
        fprintf(stderr, "lazybios: failed to read DMI table\n");
        free(ctx->dmi_data);
        ctx->dmi_data = NULL;
        fclose(dmi);
        return -1;
    }

    fclose(dmi);
    printf("lazybios: loaded %zu bytes of DMI data\n", ctx->dmi_len);
    return 0;
}

// ===== Basic Info Getters =====
bios_info_t* lazybios_get_bios_info(lazybios_ctx_t* ctx) {
    if (!ctx) return NULL;
    if (ctx->bios_info_ptr) return ctx->bios_info_ptr;
    if (!ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_BIOS);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_BIOS && length >= min_length) {
            ctx->bios_info_ptr = calloc(1, sizeof(bios_info_t));
            ctx->bios_info_ptr->vendor = strdup(dmi_string(p, length, p[BIOS_VENDOR_OFFSET]));
            ctx->bios_info_ptr->version = strdup(dmi_string(p, length, p[BIOS_VERSION_OFFSET]));
            ctx->bios_info_ptr->release_date = strdup(dmi_string(p, length, p[BIOS_RELEASE_DATE_OFFSET]));
            ctx->bios_info_ptr->rom_size_kb = (p[BIOS_ROM_SIZE_OFFSET] + 1) * BIOS_ROM_SIZE_MULTIPLIER;
            return ctx->bios_info_ptr;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

system_info_t* lazybios_get_system_info(lazybios_ctx_t* ctx) {
    if (!ctx) return NULL;
    if (ctx->system_info_ptr) return ctx->system_info_ptr;
    if (!ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_SYSTEM);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_SYSTEM && length >= min_length) {
            ctx->system_info_ptr = calloc(1, sizeof(system_info_t));
            ctx->system_info_ptr->manufacturer = strdup(dmi_string(p, length, p[SYS_MANUFACTURER_OFFSET]));
            ctx->system_info_ptr->product_name = strdup(dmi_string(p, length, p[SYS_PRODUCT_OFFSET]));
            ctx->system_info_ptr->version = strdup(dmi_string(p, length, p[SYS_VERSION_OFFSET]));
            ctx->system_info_ptr->serial_number = strdup(dmi_string(p, length, p[SYS_SERIAL_OFFSET]));
            return ctx->system_info_ptr;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

chassis_info_t* lazybios_get_chassis_info(lazybios_ctx_t* ctx) {
    if (!ctx) return NULL;
    if (ctx->chassis_info_ptr) return ctx->chassis_info_ptr;
    if (!ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_CHASSIS);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_CHASSIS && length >= min_length) {
            ctx->chassis_info_ptr = calloc(1, sizeof(chassis_info_t));
            ctx->chassis_info_ptr->asset_tag = strdup(dmi_string(p, length, p[CHASSIS_ASSET_TAG_OFFSET]));
            ctx->chassis_info_ptr->sku = strdup(dmi_string(p, length, p[CHASSIS_SKU_OFFSET]));
            return ctx->chassis_info_ptr;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

// ===== Processor Info Getter =====
processor_info_t* lazybios_get_processor_info(lazybios_ctx_t* ctx) {
    if (!ctx) return NULL;
    if (ctx->processor_info_ptr) return ctx->processor_info_ptr;
    if (!ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_PROCESSOR);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_PROCESSOR && length >= min_length) {
            ctx->processor_info_ptr = calloc(1, sizeof(processor_info_t));

            // Parse all fields
            ctx->processor_info_ptr->socket_designation = strdup(dmi_string(p, length, p[PROC_SOCKET_OFFSET]));
            ctx->processor_info_ptr->version = strdup(dmi_string(p, length, p[PROC_VERSION_OFFSET]));
            ctx->processor_info_ptr->max_speed_mhz = *(uint16_t*)(p + PROC_MAX_SPEED_OFFSET);
            ctx->processor_info_ptr->external_clock_mhz = *(uint16_t*)(p + PROC_EXTERNAL_CLOCK_OFFSET);
            ctx->processor_info_ptr->voltage = p[PROC_VOLTAGE_OFFSET];
            ctx->processor_info_ptr->processor_type = p[PROC_TYPE_OFFSET];
            ctx->processor_info_ptr->processor_family = p[PROC_FAMILY_OFFSET];

            // Core information - using correct offsets for all SMBIOS versions
            ctx->processor_info_ptr->core_count = p[PROC_CORE_COUNT_OFFSET];
            ctx->processor_info_ptr->core_enabled = p[PROC_CORE_ENABLED_OFFSET];
            ctx->processor_info_ptr->thread_count = p[PROC_THREAD_COUNT_OFFSET];

            // String fields
            if (length > PROC_SERIAL_OFFSET_2_5) {
                ctx->processor_info_ptr->serial_number = strdup(dmi_string(p, length, p[PROC_SERIAL_OFFSET_2_5]));
            } else {
                ctx->processor_info_ptr->serial_number = strdup("N/A");
            }

            ctx->processor_info_ptr->asset_tag = strdup("N/A");
            ctx->processor_info_ptr->part_number = strdup("N/A");

            // Optional fields based on structure length
            if (length > PROC_CHARACTERISTICS_OFFSET + 1) {
                ctx->processor_info_ptr->characteristics = *(uint16_t*)(p + PROC_CHARACTERISTICS_OFFSET);
            } else {
                ctx->processor_info_ptr->characteristics = 0;
            }

            return ctx->processor_info_ptr;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

// ===== Memory Devices Getter =====
memory_device_t* lazybios_get_memory_devices(lazybios_ctx_t* ctx, size_t* count) {
    if (!ctx) {
        *count = 0;
        return NULL;
    }

    if (ctx->memory_devices_ptr) {
        *count = ctx->memory_devices_count;
        return ctx->memory_devices_ptr;
    }

    if (!ctx->dmi_data) {
        *count = 0;
        return NULL;
    }

    // Count memory devices
    ctx->memory_devices_count = count_structures_by_type(ctx, SMBIOS_TYPE_MEMDEVICE);
    if (ctx->memory_devices_count == 0) {
        *count = 0;
        return NULL;
    }

    // Allocate array
    ctx->memory_devices_ptr = calloc(ctx->memory_devices_count, sizeof(memory_device_t));
    if (!ctx->memory_devices_ptr) {
        *count = 0;
        return NULL;
    }

    // Parse memory devices
    size_t index = 0;
    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_MEMDEVICE);

    while (p + SMBIOS_HEADER_SIZE < end && index < ctx->memory_devices_count) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_MEMDEVICE && length >= min_length) {
            memory_device_t *current = &ctx->memory_devices_ptr[index];

            current->locator = strdup(dmi_string(p, length, p[MEM_DEVICE_LOCATOR_OFFSET]));
            current->bank_locator = strdup(dmi_string(p, length, p[MEM_DEVICE_BANK_LOCATOR_OFFSET]));
            current->manufacturer = strdup(dmi_string(p, length, p[MEM_DEVICE_MANUFACTURER_OFFSET]));
            current->serial_number = strdup(dmi_string(p, length, p[MEM_DEVICE_SERIAL_OFFSET]));
            current->part_number = strdup(dmi_string(p, length, p[MEM_DEVICE_PART_NUMBER_OFFSET]));
            current->size_mb = *(uint16_t*)(p + MEM_DEVICE_SIZE_OFFSET);
            current->speed_mhz = *(uint16_t*)(p + MEM_DEVICE_SPEED_OFFSET);
            current->memory_type = p[MEM_DEVICE_TYPE_OFFSET];
            current->form_factor = p[MEM_DEVICE_FORM_FACTOR_OFFSET];
            current->data_width = p[MEM_DEVICE_WIDTH_OFFSET];

            index++;
        }
        p = dmi_next(p, end);
    }

    *count = ctx->memory_devices_count;
    return ctx->memory_devices_ptr;
}

// ===== Cleanup =====
void lazybios_cleanup(lazybios_ctx_t* ctx) {
    if (!ctx) return;

    // Basic info cleanup
    if (ctx->bios_info_ptr) {
        free(ctx->bios_info_ptr->vendor);
        free(ctx->bios_info_ptr->version);
        free(ctx->bios_info_ptr->release_date);
        free(ctx->bios_info_ptr);
        ctx->bios_info_ptr = NULL;
    }

    if (ctx->system_info_ptr) {
        free(ctx->system_info_ptr->manufacturer);
        free(ctx->system_info_ptr->product_name);
        free(ctx->system_info_ptr->version);
        free(ctx->system_info_ptr->serial_number);
        free(ctx->system_info_ptr);
        ctx->system_info_ptr = NULL;
    }

    if (ctx->chassis_info_ptr) {
        free(ctx->chassis_info_ptr->asset_tag);
        free(ctx->chassis_info_ptr->sku);
        free(ctx->chassis_info_ptr);
        ctx->chassis_info_ptr = NULL;
    }

    // Processor cleanup
    if (ctx->processor_info_ptr) {
        free(ctx->processor_info_ptr->socket_designation);
        free(ctx->processor_info_ptr->version);
        free(ctx->processor_info_ptr->serial_number);
        free(ctx->processor_info_ptr->asset_tag);
        free(ctx->processor_info_ptr->part_number);
        free(ctx->processor_info_ptr);
        ctx->processor_info_ptr = NULL;
    }

    // Memory devices cleanup
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

    // Raw data cleanup
    if (ctx->dmi_data) {
        free(ctx->dmi_data);
        ctx->dmi_data = NULL;
    }
    ctx->dmi_len = 0;
}

// ===== Public API Functions =====
void lazybios_smbios_ver(const lazybios_ctx_t* ctx) {
    if (!ctx) return;
    printf("SMBIOS version %d.%d\n", ctx->entry_info.major, ctx->entry_info.minor);
}

const smbios_entry_info_t* lazybios_get_entry_info(const lazybios_ctx_t* ctx) {
    if (!ctx) return NULL;
    return &ctx->entry_info;
}

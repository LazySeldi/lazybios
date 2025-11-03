#include "lazybios.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// ===== Context Management =====
lazybios_ctx_t* lazybios_ctx_new(void) {
    lazybios_ctx_t* ctx = calloc(1, sizeof(lazybios_ctx_t));
    if (!ctx) {
        fprintf(stderr, "lazybios: failed to allocate context\n");
        return NULL;
    }
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
                   ? CACHE_MIN_LENGTH_2_2  // SMBIOS 2.1+ length (21 bytes)
                   : CACHE_MIN_LENGTH_2_0; // SMBIOS 2.0 length (15 bytes)
        case SMBIOS_TYPE_MEMDEVICE:
            return is_64bit ? MEMORY_MIN_LENGTH_3_0 : MEMORY_MIN_LENGTH_2_0;
        default:
            return 4;
    }
}

bool lazybios_is_smbios_version_at_least(const lazybios_ctx_t* ctx, uint8_t major, uint8_t minor) {
    if (!ctx) return false;
    return (ctx->entry_info.major > major) ||
           (ctx->entry_info.major == major && ctx->entry_info.minor >= minor);
}

// ===== String Mapping Functions =====
const char* lazybios_get_processor_family_string(uint8_t family) {
    switch(family) {
        // Basic/Other
        case 0x01: return "Other";
        case 0x02: return "Unknown";

        // Intel x86 Legacy
        case 0x03: return "Intel 8086";
        case 0x04: return "Intel 80286";
        case 0x05: return "Intel 80386";
        case 0x06: return "Intel 80486";
        case 0x0B: return "Intel Pentium";
        case 0x0C: return "Pentium Pro";
        case 0x0D: return "Pentium II";
        case 0x0E: return "Pentium MMX";
        case 0x0F: return "Intel Celeron";
        case 0x10: return "Pentium II Xeon";
        case 0x11: return "Pentium III";

        // Intel Modern
        case 0x14: return "Intel Celeron M";
        case 0x15: return "Intel Pentium 4 HT";
        case 0x16: return "Intel Processor";
        case 0x28: return "Intel Core Duo";
        case 0x29: return "Intel Core Duo Mobile";
        case 0x2A: return "Intel Core Solo Mobile";
        case 0x2B: return "Intel Atom";
        case 0x2C: return "Intel Core M";
        case 0x2D: return "Intel Core m3";
        case 0x2E: return "Intel Core m5";
        case 0x2F: return "Intel Core m7";

        // Intel Xeon
        case 0xA1: return "Quad-Core Intel Xeon 3200";
        case 0xA2: return "Dual-Core Intel Xeon 3000";
        case 0xA3: return "Quad-Core Intel Xeon 5300";
        case 0xA4: return "Dual-Core Intel Xeon 5100";
        case 0xA5: return "Dual-Core Intel Xeon 5000";
        case 0xA6: return "Dual-Core Intel Xeon LV";
        case 0xA7: return "Dual-Core Intel Xeon ULV";
        case 0xA8: return "Dual-Core Intel Xeon 7100";
        case 0xA9: return "Quad-Core Intel Xeon 5400";
        case 0xAA: return "Quad-Core Intel Xeon";
        case 0xAB: return "Dual-Core Intel Xeon 5200";
        case 0xAC: return "Dual-Core Intel Xeon 7200";
        case 0xAD: return "Quad-Core Intel Xeon 7300";
        case 0xAE: return "Quad-Core Intel Xeon 7400";
        case 0xAF: return "Multi-Core Intel Xeon 7400";
        case 0xB0: return "Pentium III Xeon";
        case 0xB1: return "Pentium III SpeedStep";
        case 0xB2: return "Pentium 4";
        case 0xB3: return "Intel Xeon";
        case 0xB5: return "Intel Xeon MP";
        case 0xB9: return "Intel Pentium M";
        case 0xBA: return "Intel Celeron D";
        case 0xBB: return "Intel Pentium D";
        case 0xBC: return "Intel Pentium Extreme Edition";
        case 0xBD: return "Intel Core Solo";
        case 0xBF: return "Intel Core 2 Duo";
        case 0xC0: return "Intel Core 2 Solo";
        case 0xC1: return "Intel Core 2 Extreme";
        case 0xC2: return "Intel Core 2 Quad";
        case 0xC3: return "Intel Core 2 Extreme Mobile";
        case 0xC4: return "Intel Core 2 Duo Mobile";
        case 0xC5: return "Intel Core 2 Solo Mobile";
        case 0xC6: return "Intel Core i7";
        case 0xC7: return "Dual-Core Intel Celeron";
        case 0xCD: return "Intel Core i5";
        case 0xCE: return "Intel Core i3";
        case 0xCF: return "Intel Core i9";
        case 0xD0: return "Intel Xeon D";
        case 0xD6: return "Multi-Core Intel Xeon";
        case 0xD7: return "Dual-Core Intel Xeon 3xxx";
        case 0xD8: return "Quad-Core Intel Xeon 3xxx";
        case 0xDA: return "Dual-Core Intel Xeon 5xxx";
        case 0xDB: return "Quad-Core Intel Xeon 5xxx";

        // AMD
        case 0x18: return "AMD Duron";
        case 0x19: return "AMD K5";
        case 0x1A: return "AMD K6";
        case 0x1B: return "AMD K6-2";
        case 0x1C: return "AMD K6-3";
        case 0x1D: return "AMD Athlon";
        case 0x1F: return "AMD K6-2+";
        case 0x38: return "AMD Turion II Ultra Dual-Core Mobile M";
        case 0x39: return "AMD Turion II Dual-Core Mobile M";
        case 0x3A: return "AMD Athlon II Dual-Core M";
        case 0x3B: return "AMD Opteron 6100";
        case 0x3C: return "AMD Opteron 4100";
        case 0x3D: return "AMD Opteron 6200";
        case 0x3E: return "AMD Opteron 4200";
        case 0x3F: return "AMD FX";
        case 0x46: return "AMD C-Series";
        case 0x47: return "AMD E-Series";
        case 0x48: return "AMD A-Series";
        case 0x49: return "AMD G-Series";
        case 0x4A: return "AMD Z-Series";
        case 0x4B: return "AMD R-Series";
        case 0x4C: return "AMD Opteron 4300";
        case 0x4D: return "AMD Opteron 6300";
        case 0x4E: return "AMD Opteron 3300";
        case 0x4F: return "AMD FirePro";
        case 0x66: return "AMD Athlon X4 Quad-Core";
        case 0x67: return "AMD Opteron X1000";
        case 0x68: return "AMD Opteron X2000 APU";
        case 0x69: return "AMD Opteron A-Series";
        case 0x6A: return "AMD Opteron X3000 APU";
        case 0x6B: return "AMD Zen";
        case 0x83: return "AMD Athlon 64";
        case 0x84: return "AMD Opteron";
        case 0x85: return "AMD Sempron";
        case 0x86: return "AMD Turion 64 Mobile";
        case 0x87: return "Dual-Core AMD Opteron";
        case 0x88: return "AMD Athlon 64 X2 Dual-Core";
        case 0x89: return "AMD Turion 64 X2 Mobile";
        case 0x8A: return "Quad-Core AMD Opteron";
        case 0x8B: return "Third-Gen AMD Opteron";
        case 0x8C: return "AMD Phenom FX Quad-Core";
        case 0x8D: return "AMD Phenom X4 Quad-Core";
        case 0x8E: return "AMD Phenom X2 Dual-Core";
        case 0x8F: return "AMD Athlon X2 Dual-Core";
        case 0xB6: return "AMD Athlon XP";
        case 0xB7: return "AMD Athlon MP";

        // ARM/Apple
        case 0x12: return "M1 Family";
        case 0x13: return "M2 Family";

        // SPARC
        case 0x50: return "SPARC";
        case 0x51: return "SuperSPARC";
        case 0x52: return "microSPARC II";
        case 0x53: return "microSPARC IIep";
        case 0x54: return "UltraSPARC";
        case 0x55: return "UltraSPARC II";
        case 0x56: return "UltraSPARC Iii";
        case 0x57: return "UltraSPARC III";
        case 0x58: return "UltraSPARC IIIi";

        // VIA (x86-compatible, runs Linux)
        case 0xD2: return "VIA C7-M";
        case 0xD3: return "VIA C7-D";
        case 0xD4: return "VIA C7";
        case 0xD5: return "VIA Eden";
        case 0xD9: return "VIA Nano";

        default: {
            static char unknown_family[32];
            snprintf(unknown_family, sizeof(unknown_family), "Unknown (0x%02X)", family);
            return unknown_family;
        }
    }
}

const char* lazybios_get_cache_type_string(uint8_t cache_type) {
    switch(cache_type) {
    case 1: return "Other";
    case 2: return "Unknown";
    case 3: return "Instruction";
    case 4: return "Data";
    case 5: return "Unified";
    default: {
        static char unknown[32];
        snprintf(unknown, sizeof(unknown), "Unknown (0x%02X)", cache_type);
        return unknown;
    }
    }
}

const char* lazybios_get_cache_ecc_string(uint8_t ecc_type) {
    switch(ecc_type) {
    case 1: return "Other";
    case 2: return "Unknown";
    case 3: return "None";
    case 4: return "Parity";
    case 5: return "Single-bit ECC";
    case 6: return "Multi-bit ECC";
    case 7: return "CRC";
    default: {
        static char unknown[32];
        snprintf(unknown, sizeof(unknown), "Unknown (0x%02X)", ecc_type);
        return unknown;
    }
    }
}

const char* lazybios_get_cache_associativity_string(uint8_t associativity) {
    switch(associativity) {
    case 1: return "Other";
    case 2: return "Unknown";
    case 3: return "Direct Mapped";
    case 4: return "2-way";
    case 5: return "4-way";
    case 6: return "Fully Associative";
    case 7: return "8-way";
    case 8: return "16-way";
    case 9: return "12-way";
    case 10: return "24-way";
    case 11: return "32-way";
    case 12: return "48-way";
    case 13: return "64-way";
    case 14: return "20-way";
    default: {
        static char unknown[32];
        snprintf(unknown, sizeof(unknown), "Unknown (0x%02X)", associativity);
        return unknown;
    }
    }
}

const char* lazybios_get_memory_type_string(uint8_t type) {
    switch(type) {
        case MEMORY_TYPE_OTHER: return "Other";
        case MEMORY_TYPE_UNKNOWN: return "Unknown";
        case MEMORY_TYPE_DRAM: return "DRAM";
        case MEMORY_TYPE_EDRAM: return "EDRAM";
        case MEMORY_TYPE_VRAM: return "VRAM";
        case MEMORY_TYPE_SRAM: return "SRAM";
        case MEMORY_TYPE_RAM: return "RAM";
        case MEMORY_TYPE_ROM: return "ROM";
        case MEMORY_TYPE_FLASH: return "FLASH";
        case MEMORY_TYPE_EEPROM: return "EEPROM";
        case MEMORY_TYPE_FEPROM: return "FEPROM";
        case MEMORY_TYPE_EPROM: return "EPROM";
        case MEMORY_TYPE_CDRAM: return "CDRAM";
        case MEMORY_TYPE_3DRAM: return "3DRAM";
        case MEMORY_TYPE_SDRAM: return "SDRAM";
        case MEMORY_TYPE_SGRAM: return "SGRAM";
        case MEMORY_TYPE_RDRAM: return "RDRAM";
        case MEMORY_TYPE_DDR: return "DDR";
        case MEMORY_TYPE_DDR2: return "DDR2";
        case MEMORY_TYPE_DDR2_FB_DIMM: return "DDR2 FB-DIMM";
        case MEMORY_TYPE_DDR3: return "DDR3";
        case MEMORY_TYPE_FBD2: return "FBD2";
        case MEMORY_TYPE_DDR4: return "DDR4";
        case MEMORY_TYPE_LPDDR: return "LPDDR";
        case MEMORY_TYPE_LPDDR2: return "LPDDR2";
        case MEMORY_TYPE_LPDDR3: return "LPDDR3";
        case MEMORY_TYPE_LPDDR4: return "LPDDR4";
        case MEMORY_TYPE_LPDDR_NV: return "Logical non-volatile device";
        case MEMORY_TYPE_HBM: return "HBM";
        case MEMORY_TYPE_HBM2: return "HBM2";
        case MEMORY_TYPE_DDR5: return "DDR5";
        case MEMORY_TYPE_LPDDR5: return "LPDDR5";
        case MEMORY_TYPE_HBM3: return "HBM3";
        default: {
            static char unknown_type[32];
            snprintf(unknown_type, sizeof(unknown_type), "Unknown (0x%02X)", type);
            return unknown_type;
        }
    }
}

const char* lazybios_get_memory_form_factor_string(uint8_t form_factor) {
    switch(form_factor) {
        case MEMORY_FORM_FACTOR_OTHER: return "Other";
        case MEMORY_FORM_FACTOR_UNKNOWN: return "Unknown";
        case MEMORY_FORM_FACTOR_SIMM: return "SIMM";
        case MEMORY_FORM_FACTOR_SIP: return "SIP";
        case MEMORY_FORM_FACTOR_CHIP: return "Chip";
        case MEMORY_FORM_FACTOR_DIP: return "DIP";
        case MEMORY_FORM_FACTOR_ZIP: return "ZIP";
        case MEMORY_FORM_FACTOR_PROP_CARD: return "Proprietary Card";
        case MEMORY_FORM_FACTOR_DIMM: return "DIMM";
        case MEMORY_FORM_FACTOR_TSOP: return "TSOP";
        case MEMORY_FORM_FACTOR_ROW_CHIPS: return "Row of chips";
        case MEMORY_FORM_FACTOR_RIMM: return "RIMM";
        case MEMORY_FORM_FACTOR_SODIMM: return "SODIMM";
        case MEMORY_FORM_FACTOR_SRIMM: return "SRIMM";
        case MEMORY_FORM_FACTOR_FB_DIMM: return "FB-DIMM";
        case MEMORY_FORM_FACTOR_DIE: return "Die";
        case MEMORY_FORM_FACTOR_CAMM: return "CAMM";
        default: {
            static char unknown_ff[32];
            snprintf(unknown_ff, sizeof(unknown_ff), "Unknown (0x%02X)", form_factor);
            return unknown_ff;
        }
    }
}

// ===== Core Parsing Functions =====
static int parse_smbios2_entry(lazybios_ctx_t* ctx, const uint8_t *buf) {
    if (memcmp(buf, SMBIOS2_ANCHOR, 4) != 0) return -1;

    ctx->entry_info.major = buf[ENTRY2_MAJOR_OFFSET];
    ctx->entry_info.minor = buf[ENTRY2_MINOR_OFFSET];
    ctx->entry_info.docrev = 0;  // Not available in SMBIOS 2.x
    ctx->entry_info.table_length = *(uint16_t*)(buf + ENTRY2_TABLE_LENGTH_OFFSET);
    ctx->entry_info.table_address = *(uint32_t*)(buf + ENTRY2_TABLE_ADDR_OFFSET);
    ctx->entry_info.is_64bit = false;

    return 0;
}

static int parse_smbios3_entry(lazybios_ctx_t* ctx, const uint8_t *buf) {
    if (memcmp(buf, SMBIOS3_ANCHOR, 5) != 0) return -1;

    // FIXED: Use correct offsets for SMBIOS 3.x
    ctx->entry_info.major = buf[ENTRY3_MAJOR_OFFSET];
    ctx->entry_info.minor = buf[ENTRY3_MINOR_OFFSET];
    ctx->entry_info.docrev = buf[ENTRY3_DOCREV_OFFSET];
    ctx->entry_info.table_length = *(uint32_t*)(buf + ENTRY3_TABLE_LENGTH_OFFSET);
    ctx->entry_info.table_address = *(uint64_t*)(buf + ENTRY3_TABLE_ADDR_OFFSET);
    ctx->entry_info.is_64bit = true;

    return 0;
}

static const char* dmi_string(const uint8_t *formatted, int length, int string_index) {
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
    if (!ctx) {
        fprintf(stderr, "lazybios: null context provided\n");
        return -1;
    }

    FILE *entry = fopen(SMBIOS_ENTRY, "rb");
    if (!entry) {
        fprintf(stderr, "lazybios: failed to open %s: %s\n", SMBIOS_ENTRY, strerror(errno));
        return -1;
    }

    FILE *dmi = fopen(DMI_TABLE, "rb");
    if (!dmi) {
        fprintf(stderr, "lazybios: failed to open %s: %s\n", DMI_TABLE, strerror(errno));
        fclose(entry);
        return -1;
    }

    uint8_t entry_buf[64];
    size_t n = fread(entry_buf, 1, sizeof(entry_buf), entry);
    fclose(entry);

    if (n < 20) {
        fprintf(stderr, "lazybios: invalid entry point size (%zu bytes)\n", n);
        fclose(dmi);
        return -1;
    }

    // Try SMBIOS 3.x first, then "fall back" to 2.x
    if (parse_smbios3_entry(ctx, entry_buf) == 0) {
        // Successfully parsed as SMBIOS 3.x
    } else if (parse_smbios2_entry(ctx, entry_buf) == 0) {
        // Successfully parsed as SMBIOS 2.x
    } else {
        fprintf(stderr, "lazybios: unknown SMBIOS anchor\n");
        fclose(dmi);
        return -1;
    }

    fseek(dmi, 0, SEEK_END);
    ctx->dmi_len = ftell(dmi);
    rewind(dmi);

    if (ctx->dmi_len == 0) {
        fprintf(stderr, "lazybios: empty DMI table\n");
        fclose(dmi);
        return -1;
    }

    ctx->dmi_data = malloc(ctx->dmi_len);
    if (!ctx->dmi_data) {
        fprintf(stderr, "lazybios: failed to allocate %zu bytes for DMI buffer\n", ctx->dmi_len);
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
    if (!ctx || !ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_BIOS);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_BIOS && length >= min_length) {
            ctx->bios_info.vendor = strdup(dmi_string(p, length, p[BIOS_VENDOR_OFFSET]));
            ctx->bios_info.version = strdup(dmi_string(p, length, p[BIOS_VERSION_OFFSET]));
            ctx->bios_info.release_date = strdup(dmi_string(p, length, p[BIOS_RELEASE_DATE_OFFSET]));

            uint8_t rom_size = p[BIOS_ROM_SIZE_OFFSET];
            if (rom_size == 0xFF) {
                ctx->bios_info.rom_size_kb = 0; // Size not specified
            } else {
                ctx->bios_info.rom_size_kb = (rom_size + 1) * BIOS_ROM_SIZE_MULTIPLIER;
            }

            return &ctx->bios_info;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

system_info_t* lazybios_get_system_info(lazybios_ctx_t* ctx) {
    if (!ctx || !ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_SYSTEM);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_SYSTEM && length >= min_length) {
            ctx->system_info.manufacturer = strdup(dmi_string(p, length, p[SYS_MANUFACTURER_OFFSET]));
            ctx->system_info.product_name = strdup(dmi_string(p, length, p[SYS_PRODUCT_OFFSET]));
            ctx->system_info.version = strdup(dmi_string(p, length, p[SYS_VERSION_OFFSET]));
            ctx->system_info.serial_number = strdup(dmi_string(p, length, p[SYS_SERIAL_OFFSET]));

            if (length >= 0x19) {
                ctx->system_info.uuid = strdup("UUID Available");
            } else {
                ctx->system_info.uuid = strdup("Not Available");
            }

            return &ctx->system_info;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

baseboard_info_t* lazybios_get_baseboard_info(lazybios_ctx_t* ctx) {
    if (!ctx || !ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_BASEBOARD);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_BASEBOARD && length >= min_length) {
            ctx->baseboard_info.manufacturer = strdup(dmi_string(p, length, p[BASEBOARD_MANUFACTURER_OFFSET]));
            ctx->baseboard_info.product = strdup(dmi_string(p, length, p[BASEBOARD_PRODUCT_OFFSET]));
            ctx->baseboard_info.version = strdup(dmi_string(p, length, p[BASEBOARD_VERSION_OFFSET]));
            ctx->baseboard_info.serial_number = strdup(dmi_string(p, length, p[BASEBOARD_SERIAL_OFFSET]));
            ctx->baseboard_info.asset_tag = strdup(dmi_string(p, length, p[BASEBOARD_ASSET_TAG_OFFSET]));
            return &ctx->baseboard_info;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

chassis_info_t* lazybios_get_chassis_info(lazybios_ctx_t* ctx) {
    if (!ctx || !ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_CHASSIS);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_CHASSIS && length >= min_length) {
            if (length > CHASSIS_ASSET_TAG_OFFSET) {
                ctx->chassis_info.asset_tag = strdup(dmi_string(p, length, p[CHASSIS_ASSET_TAG_OFFSET]));
            } else {
                ctx->chassis_info.asset_tag = strdup("Not Specified");
            }

            if (length > CHASSIS_SKU_OFFSET) {
                ctx->chassis_info.sku = strdup(dmi_string(p, length, p[CHASSIS_SKU_OFFSET]));
            } else {
                ctx->chassis_info.sku = strdup("Not Specified");
            }

            if (length >= 0x05) {
                ctx->chassis_info.type = p[0x05];
                ctx->chassis_info.state = p[0x06];
            }

            return &ctx->chassis_info;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

processor_info_t* lazybios_get_processor_info(lazybios_ctx_t* ctx) {
    if (!ctx || !ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_PROCESSOR);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_PROCESSOR && length >= min_length) {
            ctx->processor_info.socket_designation = strdup(dmi_string(p, length, p[PROC_SOCKET_OFFSET]));
            ctx->processor_info.version = strdup(dmi_string(p, length, p[PROC_VERSION_OFFSET]));
            ctx->processor_info.max_speed_mhz = *(uint16_t*)(p + PROC_MAX_SPEED_OFFSET);
            ctx->processor_info.L1_cache_handle = *(uint16_t*)(p + PROC_L1_CACHE_HANDLE_OFFSET);
            ctx->processor_info.L2_cache_handle = *(uint16_t*)(p + PROC_L2_CACHE_HANDLE_OFFSET);
            ctx->processor_info.L3_cache_handle = *(uint16_t*)(p + PROC_L3_CACHE_HANDLE_OFFSET);

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

            // Status (added)
            if (length > PROC_STATUS_OFFSET) {
                ctx->processor_info.status = p[PROC_STATUS_OFFSET];
            } else {
                ctx->processor_info.status = 0;
            }

            // Core count with extended support
            uint8_t core_count_byte = p[PROC_CORE_COUNT_OFFSET];
            if (core_count_byte == 0xFF && length > PROC_CORE_COUNT2_OFFSET + 1) {
                ctx->processor_info.core_count = *(uint16_t*)(p + PROC_CORE_COUNT2_OFFSET);
            } else {
                ctx->processor_info.core_count = core_count_byte;
            }

            // Core enabled with extended support
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
                ctx->processor_info.serial_number = strdup(dmi_string(p, length, p[PROC_SERIAL_OFFSET_2_5]));
            } else {
                ctx->processor_info.serial_number = strdup("Not Specified");
            }

            if (length > PROC_ASSET_TAG_OFFSET_2_5) {
                ctx->processor_info.asset_tag = strdup(dmi_string(p, length, p[PROC_ASSET_TAG_OFFSET_2_5]));
            } else {
                ctx->processor_info.asset_tag = strdup("Not Specified");
            }

            if (length > PROC_PART_NUMBER_OFFSET_2_5) {
                ctx->processor_info.part_number = strdup(dmi_string(p, length, p[PROC_PART_NUMBER_OFFSET_2_5]));
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
        p = dmi_next(p, end);
    }
    return NULL;
}

// ===== Cache Information Getter =====
cache_info_t* lazybios_get_caches(lazybios_ctx_t* ctx, size_t* count) {
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

    ctx->caches_count = count_structures_by_type(ctx, SMBIOS_TYPE_CACHES);
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
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_CACHES);

    while (p + SMBIOS_HEADER_SIZE < end && index < ctx->caches_count) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_CACHES && length >= min_length) {
            cache_info_t *current = &ctx->caches_ptr[index];

            current->socket_designation = strdup(dmi_string(p, length, p[CACHE_SOCKET_DESIGNATION]));

            // Extract cache level from configuration (bits 2:0)
            uint16_t config = *(uint16_t*)(p + CACHE_CONFIGURATION);
            current->level = config & 0x07;

            // Get cache size (handle both old and new size fields)
            if (length >= 0x15) {
                // SMBIOS 2.1+ - use extended size field
                current->size_kb = *(uint32_t*)(p + CACHE_MAXIMUM_SIZE_2);
            } else {
                // SMBIOS 2.0 - use old size field
                current->size_kb = *(uint16_t*)(p + CACHE_MAXIMUM_SIZE);
            }

            current->error_correction_type = p[CACHE_ERROR_CORRECTION_TYPE];
            current->system_cache_type = p[CACHE_SYSTEM_CACHE_TYPE];
            current->associativity = p[CACHE_ASSOCIATIVITY];

            index++;
        }
        p = dmi_next(p, end);
    }

    *count = ctx->caches_count;
    return ctx->caches_ptr;
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

    ctx->memory_devices_count = count_structures_by_type(ctx, SMBIOS_TYPE_MEMDEVICE);
    if (ctx->memory_devices_count == 0) {
        *count = 0;
        return NULL;
    }

    ctx->memory_devices_ptr = calloc(ctx->memory_devices_count, sizeof(memory_device_t));
    if (!ctx->memory_devices_ptr) {
        *count = 0;
        return NULL;
    }

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

            uint16_t size = *(uint16_t*)(p + MEM_DEVICE_SIZE_OFFSET);

            if (size == 0xFFFF) {
                // Size not known
                current->size_mb = 0;
                current->size_extended = false;
            } else if (size == 0x7FFF) {
                // Extended size - check if we have the extended size field
                if (length >= 0x20) {  // Structure must be long enough for extended size
                    uint32_t ext_size = *(uint32_t*)(p + MEM_DEVICE_EXT_SIZE_OFFSET);
                    current->size_mb = ext_size;  // Extended size is in MB directly
                    current->size_extended = true;
                } else {
                    current->size_mb = 0;
                    current->size_extended = false;
                }
            } else if (size == 0) {
                // Empty slot
                current->size_mb = 0;
                current->size_extended = false;
            } else {
                // Regular size (in MB)
                current->size_mb = size;
                current->size_extended = false;
            }

            current->speed_mhz = *(uint16_t*)(p + MEM_DEVICE_SPEED_OFFSET);
            current->memory_type = p[MEM_DEVICE_TYPE_OFFSET];
            current->form_factor = p[MEM_DEVICE_FORM_FACTOR_OFFSET];

            // Total width and data width
            if (length > MEM_DEVICE_TOTAL_WIDTH_OFFSET + 1) {
                current->total_width = *(uint16_t*)(p + MEM_DEVICE_TOTAL_WIDTH_OFFSET);
            } else {
                current->total_width = 0;
            }

            if (length > MEM_DEVICE_DATA_WIDTH_OFFSET + 1) {
                current->data_width = *(uint16_t*)(p + MEM_DEVICE_DATA_WIDTH_OFFSET);
            } else {
                current->data_width = 0;
            }

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
}

// ===== Public API Functions =====
void lazybios_smbios_ver(const lazybios_ctx_t* ctx) {
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

const smbios_entry_info_t* lazybios_get_entry_info(const lazybios_ctx_t* ctx) {
    if (!ctx) return NULL;
    return &ctx->entry_info;
}

//
// test.c - File for testing ALL SMBIOS types we have implemented currently
//
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "lazybios.h"


void printType0(lazybiosCTX_t* ctx) {
    printf("=== BIOS INFORMATION ===\n");

    if (!ctx->Type0) {
        ctx->Type0 = lazybiosGetType0(ctx);
    }
    /*
     you could also do it this way:
    if (ctx->Type0) {
        lazybiosFreeType0(ctx->Type0);
    }
    ctx->Type0 = lazybiosGetType0(ctx);
    */

    if (ctx->Type0) {
        printf("Vendor: %s\n", ctx->Type0->vendor);
        printf("Version: %s\n", ctx->Type0->version);
        printf("Release Date: %s\n", ctx->Type0->release_date);
        printf("BIOS Starting Segment: 0x%04X\n", ctx->Type0->bios_starting_segment);
        printf("ROM Size: %u KB\n", ctx->Type0->rom_size);
        printf("Firmware Characteristics: %s\n", lazybiosFirmwareCharacteristicsStr(ctx->Type0->characteristics));
        if (ISVERPLUS(ctx, 2, 4)) {
            if (ctx->Type0->firmware_char_ext_bytes_count >= 1) {
                printf("Firmware Characteristics Extension Bytes 1: %s\n", lazybiosFirmwareCharacteristicsExtByte1Str(ctx->Type0->firmware_char_ext_bytes[0]));
            }

            if (ctx->Type0->firmware_char_ext_bytes_count >= 2) {
                printf("Firmware Characteristics Extension Bytes 2: %s\n", lazybiosFirmwareCharacteristicsExtByte2Str(ctx->Type0->firmware_char_ext_bytes[1]));
            }
        }

        if (ISVERPLUS(ctx, 2, 4)) {
            printf("Platform Major Release: %hu\n", ctx->Type0->platform_major_release);
            printf("Platform Minor Release: %hu\n", ctx->Type0->platform_minor_release);
            printf("EC Major Release: %hu\n", ctx->Type0->ec_major_release);
            printf("EC Minor Release: %hu\n", ctx->Type0->ec_minor_release);
        }
        if (ISVERPLUS(ctx, 3, 1)) {
            printf("Extended ROM Size: %hu%s\n", lazybiosFirmwareExtendedROMSizeU16(ctx->Type0->extended_rom_size, ctx->Type0->unit), ctx->Type0->unit);
        }
        printf("\n");
    } else {
        printf("Failed to get BIOS information\n\n");
    }
}

int print_smbios_version_info(lazybiosCTX_t* ctx) {
    if (!ctx) return -1;
    printf("=== SMBIOS VERSION INFORMATION ===\n");
    lazybiosPrintVer(ctx);

    printf("Table Length: %u bytes\n", ctx->entry_info.table_length);
    if (ctx->backend == LAZYBIOS_BACKEND_WINDOWS) {
        printf("Table Address: Not available (Windows API)\n");
    } else {
        printf("Table Address: 0x%lX\n", ctx->entry_info.table_address);
    }
    printf("Is 64-bit: %s\n", ISVERPLUS(ctx, 3, 0) == 1 ? "Yes (SMBIOS 3.x)" : "No (SMBIOS 2.x)");
    if (ISVERPLUS(ctx, 3, 0)) {
        printf("Docrev: %u\n", ctx->entry_info.docrev);
    }
    printf("\n");

    return 0;
}

static inline void print_usage(const char *progname) {
    printf("Usage: %s [options]\n", progname);
    printf("Options:\n");
    printf("  -help           Show this help message\n");
    printf("  <entry file> <dmi file>  Initialize from file instead of live system\n");
}

int main(int argc, char *argv[]) {
    printf("lazybios - System Information\n");
    printf("=============================================\n\n");

    // Create context
    lazybiosCTX_t* ctx = lazybiosCTXNew();
    if (!ctx) {
        fprintf(stderr, "Failed to allocate lazybios context\n");
        return 1;
    }

    if (argc == 2 && strcmp(argv[1], "-help") == 0) {
        print_usage(argv[0]);
        lazybiosCleanup(ctx);
        return 0;
    } else if (argc == 3) {
        if (lazybiosFile(ctx, argv[1], argv[2]) != 0) {
            fprintf(stderr, "Failed to initialize lazybios from file\n");
            lazybiosCleanup(ctx);
            return 1;
        }
    } else if (argc != 1) {
        fprintf(stderr, "Invalid arguments\n");
        print_usage(argv[0]);
        lazybiosCleanup(ctx);
        return 1;
    } else {
        if (lazybiosInit(ctx) != 0) {
            fprintf(stderr, "Failed to initialize lazybios library\n");
            lazybiosCleanup(ctx);
            return 1;
        }
    }

    printf("Library initialized successfully!\n\n");
    printf("lazybios version %s\n\n", LAZYBIOS_VER);

    // Print SMBIOS version info
    print_smbios_version_info(ctx);

    // Lazy init / cache Type0
    if (!ctx->Type0) {
        ctx->Type0 = lazybiosGetType0(ctx);
    }
    printType0(ctx);

    // Cleanup
    if (lazybiosCleanup(ctx) == 0) {
        printf("Library cleanup completed!\n");
    } else {
        fprintf(stderr, "Could not complete library cleanup\n");
        return -1;
    }

    printf("All tests passed successfully!\n");
    return 0;
}
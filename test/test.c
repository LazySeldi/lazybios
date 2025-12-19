//
// test.c - File for testing ALL SMBIOS types we have implemented currently
//
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "lazybios.h"


void printType0(lazybiosCTX_t* ctx) {
    lb_printf("=== BIOS INFORMATION ===\n");

    if (!ctx->Type0) ctx->Type0 = lazybiosGetType0(ctx);

    /*
     you could also do it this way:
    if (ctx->Type0) {
        lazybiosFreeType0(ctx->Type0);
    }
    ctx->Type0 = lazybiosGetType0(ctx);
    */

    if (ctx->Type0) {
        lb_printf("Vendor: %s\n", ctx->Type0->vendor);
        lb_printf("Version: %s\n", ctx->Type0->version);
        lb_printf("Release Date: %s\n", ctx->Type0->release_date);
        lb_printf("BIOS Starting Segment: 0x%04X\n", ctx->Type0->bios_starting_segment);
        lb_printf("Firmware Characteristics: %s\n", lazybiosFirmwareCharacteristicsStr(ctx->Type0->characteristics));
        if (ISVERPLUS(ctx, 2, 4)) {
            if (ctx->Type0->firmware_char_ext_bytes_count >= 1) {
                lb_printf("Firmware Characteristics Extension Bytes 1: %s\n", lazybiosFirmwareCharacteristicsExtByte1Str(ctx->Type0->firmware_char_ext_bytes[0]));
            }

            if (ctx->Type0->firmware_char_ext_bytes_count >= 2) {
                lb_printf("Firmware Characteristics Extension Bytes 2: %s\n", lazybiosFirmwareCharacteristicsExtByte2Str(ctx->Type0->firmware_char_ext_bytes[1]));
            }
        }

        if (ISVERPLUS(ctx, 2, 4)) {
            lb_printf("Platform Major Release: %hu\n", ctx->Type0->platform_major_release);
            lb_printf("Platform Minor Release: %hu\n", ctx->Type0->platform_minor_release);
            lb_printf("EC Major Release: %hu\n", ctx->Type0->ec_major_release);
            lb_printf("EC Minor Release: %hu\n", ctx->Type0->ec_minor_release);
        }
        if (ISVERPLUS(ctx, 3, 1)) {
            lb_printf("Extended ROM Size: %hu%s\n\n", lazybiosFirmwareExtendedROMSizeU16(ctx->Type0->extended_rom_size, ctx->Type0->unit), ctx->Type0->unit);
        } else {
            lb_printf("ROM Size: %u KB\n\n", ctx->Type0->rom_size);
        }
    } else {
        lb_printf("Failed to get BIOS information\n\n");
    }
}

void printType1(lazybiosCTX_t* ctx) {
    lb_printf("=== SYSTEM INFORMATION ===\n");

    if (!ctx->Type1) ctx->Type1 = lazybiosGetType1(ctx);

    if (ctx->Type1) {
        lb_printf("Manufacturer: %s\n", ctx->Type1->manufacturer);
        lb_printf("Product name: %s\n", ctx->Type1->product_name);
        lb_printf("Version: %s\n", ctx->Type1->version);
        lb_printf("Serial number: %s\n", ctx->Type1->serial_number);
        lb_printf("UUID: %02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n", ctx->Type1->uuid[0], ctx->Type1->uuid[1], ctx->Type1->uuid[2], ctx->Type1->uuid[3], ctx->Type1->uuid[4], ctx->Type1->uuid[5], ctx->Type1->uuid[6], ctx->Type1->uuid[7], ctx->Type1->uuid[8], ctx->Type1->uuid[9], ctx->Type1->uuid[10], ctx->Type1->uuid[11], ctx->Type1->uuid[12], ctx->Type1->uuid[13], ctx->Type1->uuid[14], ctx->Type1->uuid[15]);
        lb_printf("Wake up type: %s\n", lazybiosWakeupTypeStr(ctx->Type1->wake_up_type));
        lb_printf("SKU number: %s\n", ctx->Type1->sku_number);
        lb_printf("Family: %s\n\n", ctx->Type1->family);
    } else {
        lb_printf("Failed to get System information");
    }
}

int print_smbios_version_info(lazybiosCTX_t* ctx) {
    if (!ctx) return -1;
    lb_printf("=== SMBIOS VERSION INFORMATION ===\n");
    lazybiosPrintVer(ctx);

    lb_printf("Table Length: %u bytes\n", ctx->entry_info.table_length);
    if (ctx->backend == LAZYBIOS_BACKEND_WINDOWS) {
        lb_printf("Table Address: Not available (Windows API)\n");
    } else {
        lb_printf("Table Address: 0x%lX\n", ctx->entry_info.table_address);
    }
    lb_printf("Is 64-bit: %s\n", ISVERPLUS(ctx, 3, 0) == 1 ? "Yes (SMBIOS 3.x)" : "No (SMBIOS 2.x)");
    if (ISVERPLUS(ctx, 3, 0)) {
        lb_printf("Docrev: %u\n", ctx->entry_info.docrev);
    }
    lb_printf("\n");

    return 0;
}

static inline void print_usage(const char *progname) {
    lb_printf("Usage: %s [options]\n", progname);
    lb_printf("Options:\n");
    lb_printf("  -help                      Show this help message\n");
    lb_printf("  -dump                      Dumps both the raw entry info and raw DMI table into 2 files into the current directory, If OS is Windows it only dumps DMI.bin\n");
    lb_printf("  -sources <entry> <dmi>     Usees the parser on the 2 specified files <entry> and <dmi>\n");
}

int main(int argc, const char *argv[]) {
    lb_printf("lazybios Version: %s\n", LAZYBIOS_VER);
    lb_printf("=============================================\n\n");

    // Create context
    lazybiosCTX_t* ctx = lazybiosCTXNew();
    if (!ctx) {
        lb_fprintf(stderr, "Failed to allocate lazybios context\n");
        return 1;
    }

    if (argc == 2 && lb_strcmp(argv[1], "-help") == 0) {
        print_usage(argv[0]);
        lazybiosCleanup(ctx);
        return 0;
    } else if (argc == 2 && lb_strcmp(argv[1], "-dump") == 0) {
        lazybiosInit(ctx);
        if (ctx->backend == LAZYBIOS_BACKEND_LINUX) {
            lb_FILE* entry = lb_fopen("smbios_entry_point", "wb");
            if (!entry) {
                lb_printf("Failed to open 'smbios_entry_point': %s\n", lb_strerror(errno));
                return -1;
            }

            lb_FILE* dmi = lb_fopen("DMI", "wb");
            if (!dmi) {
                lb_printf("Failed to open 'DMI': %s\n", lb_strerror(errno));
                lb_fclose(entry);
                return -1;
            }

            lb_fwrite(ctx->dmi_data, 1, ctx->dmi_len, dmi);
            lb_fwrite(ctx->entry_data, 1, ctx->entry_len, entry);
            lb_fclose(entry);
            lb_fclose(dmi);
            lb_printf("smbios_entry_point and DMI dumped sucessfully, look at your current directory to locate them!\n");
            lazybiosCleanup(ctx);
            return 0;

        } else if (ctx->backend == LAZYBIOS_BACKEND_WINDOWS) {
            lb_FILE* dmi = lb_fopen("DMI.bin", "wb");
            lb_printf("Windows only gives DMI data and not the actual entry point data, so only DMI.bin is being dumped.\n");

            if (dmi == LAZYBIOS_NULL) {
                lb_printf("Couldn't create/modify the files for dumping: %s\n", lb_strerror(errno));
                lb_fclose(dmi);
                return -1;
            }

            lb_fwrite(ctx->dmi_data, 1, ctx->dmi_len, dmi);
            lb_fclose(dmi);
            lb_printf("DMI.bin created/filled sucessfully, check your current directory to locate them!\n");
            lazybiosCleanup(ctx);
            return 0;
        }
    } else if (argc == 4 && lb_strcmp(argv[1], "-sources") == 0) {
        if (lazybiosFile(ctx, argv[2], argv[3]) != 0) {
            lb_fprintf(stderr, "Failed to initialize lazybios from file\n");
            lazybiosCleanup(ctx);
            return -1;
        }
    } else if (argc != 1) {
        lb_fprintf(stderr, "Invalid arguments\n");
        print_usage(argv[0]);
        lazybiosCleanup(ctx);
        return -1;
    } else {
        if (lazybiosInit(ctx) != 0) {
            lb_fprintf(stderr, "Failed to initialize lazybios library\n");
            lazybiosCleanup(ctx);
            return -1;
        }
    }

    lb_printf("Library initialized successfully!\n\n");

    // Print SMBIOS version info
    print_smbios_version_info(ctx);

    // Lazy init / cache Type0
    if (!ctx->Type0) {
        ctx->Type0 = lazybiosGetType0(ctx);
    }
    printType0(ctx);

    if (!ctx->Type1) {
        ctx->Type1 = lazybiosGetType1(ctx);
    }
    printType1(ctx);



    // Cleanup
    if (lazybiosCleanup(ctx) == 0) {
        lb_printf("Library cleanup completed!\n");
    } else {
        lb_fprintf(stderr, "Could not complete library cleanup\n");
        return -1;
    }

    lb_printf("All tests passed successfully!\n");
    return 0;
}
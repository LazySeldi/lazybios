//
// test.c - File for testing ALL SMBIOS types we have implemented currently
//
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "lazybios.h"

void printType0(lazybiosCTX_t* ctx) {
    lb_printf("=== BIOS INFORMATION ===\n");

    if (!ctx->Type0) ctx->Type0 = lazybiosGetType0(ctx);

    if (ctx->Type0) {
        lb_printf("Vendor: %s\n", ctx->Type0->vendor);
        lb_printf("Version: %s\n", ctx->Type0->version);
        lb_printf("Release Date: %s\n", ctx->Type0->release_date);
        
        if (ctx->Type0->bios_starting_segment == LAZYBIOS_NOT_FOUND_U16) {
            lb_printf("BIOS Starting Segment: Not Present\n");
        } else {
            lb_printf("BIOS Starting Segment: 0x%04X\n", ctx->Type0->bios_starting_segment);
        }
        
        if (ctx->Type0->characteristics == LAZYBIOS_NOT_FOUND_U64) {
            lb_printf("Firmware Characteristics: Not Present\n");
        } else {
            lb_printf("Firmware Characteristics: %s\n", lazybiosType0CharacteristicsStr(ctx->Type0->characteristics));
        }
        
        if (ISVERPLUS(ctx, 2, 4)) {
            if (ctx->Type0->firmware_char_ext_bytes_count >= 1) {
                lb_printf("Firmware Characteristics Extension Bytes 1: %s\n", lazybiosType0CharacteristicsExtByte1Str(ctx->Type0->firmware_char_ext_bytes[0]));
            } else {
                lb_printf("Firmware Characteristics Extension Bytes 1: Not Present\n");
            }

            if (ctx->Type0->firmware_char_ext_bytes_count >= 2) {
                lb_printf("Firmware Characteristics Extension Bytes 2: %s\n", lazybiosType0CharacteristicsExtByte2Str(ctx->Type0->firmware_char_ext_bytes[1]));
            } else {
                lb_printf("Firmware Characteristics Extension Bytes 2: Not Present\n");
            }
            
            if (ctx->Type0->platform_major_release == LAZYBIOS_NOT_FOUND_U8) {
                lb_printf("Platform Major Release: Not Present\n");
            } else {
                lb_printf("Platform Major Release: %hu\n", ctx->Type0->platform_major_release);
            }
            
            if (ctx->Type0->platform_minor_release == LAZYBIOS_NOT_FOUND_U8) {
                lb_printf("Platform Minor Release: Not Present\n");
            } else {
                lb_printf("Platform Minor Release: %hu\n", ctx->Type0->platform_minor_release);
            }
            
            if (ctx->Type0->ec_major_release == LAZYBIOS_NOT_FOUND_U8) {
                lb_printf("EC Major Release: Not Present\n");
            } else {
                lb_printf("EC Major Release: %hu\n", ctx->Type0->ec_major_release);
            }
            
            if (ctx->Type0->ec_minor_release == LAZYBIOS_NOT_FOUND_U8) {
                lb_printf("EC Minor Release: Not Present\n");
            } else {
                lb_printf("EC Minor Release: %hu\n", ctx->Type0->ec_minor_release);
            }
        } else {
            lb_printf("Firmware Characteristics Extension Bytes 1: [SMBIOS 2.4 required]\n");
            lb_printf("Firmware Characteristics Extension Bytes 2: [SMBIOS 2.4 required]\n");
            lb_printf("Platform Major Release: [SMBIOS 2.4 required]\n");
            lb_printf("Platform Minor Release: [SMBIOS 2.4 required]\n");
            lb_printf("EC Major Release: [SMBIOS 2.4 required]\n");
            lb_printf("EC Minor Release: [SMBIOS 2.4 required]\n");
        }

        if (ISVERPLUS(ctx, 3, 1)) {
            if (ctx->Type0->extended_rom_size == LAZYBIOS_NOT_FOUND_U16) {
                lb_printf("Extended ROM Size: Not Present\n\n");
            } else {
                lb_printf("Extended ROM Size: %hu%s\n\n", lazybiosType0ExtendedROMSizeU16(ctx->Type0->extended_rom_size, ctx->Type0->unit), ctx->Type0->unit);
            }
        } else {
            if (ctx->Type0->rom_size == LAZYBIOS_NOT_FOUND_U32) {
                lb_printf("ROM Size: Not Present\n\n");
            } else {
                lb_printf("ROM Size: %u KB\n\n", ctx->Type0->rom_size);
            }
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

        if (ISVERPLUS(ctx, 2, 1)) {
            // UUID is SMBIOS 2.1+
            int uuid_found = 0;
            for (int i = 0; i < 16; i++) {
                if (ctx->Type1->uuid[i] != LAZYBIOS_NOT_FOUND_U8) {
                    uuid_found = 1;
                    break;
                }
            }

            if (uuid_found) {
                lb_printf("UUID: %02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
                         ctx->Type1->uuid[0], ctx->Type1->uuid[1], ctx->Type1->uuid[2], ctx->Type1->uuid[3],
                         ctx->Type1->uuid[4], ctx->Type1->uuid[5], ctx->Type1->uuid[6], ctx->Type1->uuid[7],
                         ctx->Type1->uuid[8], ctx->Type1->uuid[9], ctx->Type1->uuid[10], ctx->Type1->uuid[11],
                         ctx->Type1->uuid[12], ctx->Type1->uuid[13], ctx->Type1->uuid[14], ctx->Type1->uuid[15]);
            } else {
                lb_printf("UUID: Not Present\n");
            }

            if (ctx->Type1->wake_up_type == LAZYBIOS_NOT_FOUND_U8) {
                lb_printf("Wake up type: Not Present\n");
            } else {
                lb_printf("Wake up type: %s\n", lazybiosType1WakeupTypeStr(ctx->Type1->wake_up_type));
            }
        } else {
            lb_printf("UUID: [SMBIOS 2.1 required]\n");
            lb_printf("Wake up type: [SMBIOS 2.1 required]\n");
        }

        if (ISVERPLUS(ctx, 2, 4)) {
            // SKU and Family are SMBIOS 2.4+
            lb_printf("SKU number: %s\n", ctx->Type1->sku_number);
            lb_printf("Family: %s\n\n", ctx->Type1->family);
        } else {
            lb_printf("SKU number: [SMBIOS 2.4 required]\n");
            lb_printf("Family: [SMBIOS 2.4 required]\n\n");
        }
    } else {
        lb_printf("Failed to get System information\n\n");
    }
}

void printType2(lazybiosCTX_t* ctx) {
    lb_printf("=== BASEBOARD INFORMATION ===\n");

    if (!ctx->Type2) ctx->Type2 = lazybiosGetType2(ctx);

    if (ctx->Type2) {
        lb_printf("Manufacturer: %s\n", ctx->Type2->manufacturer);
        lb_printf("Product: %s\n", ctx->Type2->product);
        lb_printf("Version: %s\n", ctx->Type2->version);
        lb_printf("Serial number: %s\n", ctx->Type2->serial_number);
        lb_printf("Asset tag: %s\n", ctx->Type2->asset_tag);

        if (ISVERPLUS(ctx, 2, 1)) {
            if (ctx->Type2->feature_flags == LAZYBIOS_NOT_FOUND_U8) {
                lb_printf("Feature flags: Not Present\n");
            } else {
                lb_printf("Feature flags: %s\n", lazybiosType2FeatureflagsStr(ctx->Type2->feature_flags));
            }

            lb_printf("Location in Chassis: %s\n", ctx->Type2->location_in_chassis);

            if (ctx->Type2->chassis_handle == LAZYBIOS_NOT_FOUND_U16) {
                lb_printf("Chassis Handle: Not Present\n");
            } else {
                lb_printf("Chassis Handle: 0x%lX\n", ctx->Type2->chassis_handle);
            }

            if (ctx->Type2->board_type == LAZYBIOS_NOT_FOUND_U8) {
                lb_printf("Board Type: Not Present\n");
            } else {
                lb_printf("Board Type: %s\n", lazybiosType2BoardTypeStr(ctx->Type2->board_type));
            }

            if (ctx->Type2->number_of_contained_object_handles == LAZYBIOS_NOT_FOUND_U8 ||
                ctx->Type2->number_of_contained_object_handles == 0) {
                // No contained object handles or field not present here
            } else {
                for (int i = 0; i < ctx->Type2->number_of_contained_object_handles; i++) {
                    lb_printf("  Contained Object Handle %d: 0x%04X\n",
                             i, ctx->Type2->contained_object_handles[i]);
                }
            }
        } else {
            lb_printf("Feature flags: [SMBIOS 2.1 required]\n");
            lb_printf("Location in Chassis: [SMBIOS 2.1 required]\n");
            lb_printf("Chassis Handle: [SMBIOS 2.1 required]\n");
            lb_printf("Board Type: [SMBIOS 2.1 required]\n");
            lb_printf("Contained Object Handles: [SMBIOS 2.1 required]");
        }

        lb_printf("\n");
    } else {
        lb_printf("Failed to get Baseboard information\n\n");
    }
}

void printType3(lazybiosCTX_t* ctx) {
    lb_printf("=== CHASSIS INFORMATION ===\n");

    if (!ctx->Type3) ctx->Type3 = lazybiosGetType3(ctx);

    if (ctx->Type3) {
        lb_printf("Manufacturer: %s\n", ctx->Type3->manufacturer);
        if (ctx->Type3->type == LAZYBIOS_NOT_FOUND_U8) {
            lb_printf("Type: Not Present\n");
        } else {
            lb_printf("Type: %s\n", lazybiosType3TypeStr(ctx->Type3->type));
        }
        lb_printf("Version: %s\n", ctx->Type3->version);
        lb_printf("Serial Number: %s\n", ctx->Type3->serial_number);
        lb_printf("Asset Tag: %s\n", ctx->Type3->asset_tag);
        if (ISVERPLUS(ctx, 2, 1)) {
            if (ctx->Type3->boot_up_state == LAZYBIOS_NOT_FOUND_U8) {
                lb_printf("BootUp State: Not Present\n");
            } else {
                lb_printf("BootUp State: %s\n", lazybiosType3StateStr(ctx->Type3->boot_up_state));
            }

            if (ctx->Type3->power_supply_state == LAZYBIOS_NOT_FOUND_U8) {
                lb_printf("Power Supply State: Not Present\n");
            } else {
                lb_printf("Power Supply State: %s\n", lazybiosType3StateStr(ctx->Type3->power_supply_state));
            }

            if (ctx->Type3->thermal_state == LAZYBIOS_NOT_FOUND_U8) {
                lb_printf("Thermal State: Not Present\n");
            } else {
                lb_printf("Thermal State: %s\n", lazybiosType3StateStr(ctx->Type3->thermal_state));
            }

            if (ctx->Type3->security_status == LAZYBIOS_NOT_FOUND_U8) {
                lb_printf("Security Status: Not Present\n");
            } else {
                lb_printf("Security Status: %s\n", lazybiosType3SecurityStatusStr(ctx->Type3->security_status));
            }

        } else {
            lb_printf("BootUp State: [SMBIOS 2.1 required]\n");
            lb_printf("Power Supply State: [SMBIOS 2.1 required]\n");
            lb_printf("Thermal State: [SMBIOS 2.1 required]\n");
            lb_printf("Security Status: [SMBIOS 2.1 required]\n");
        }

        if (ISVERPLUS(ctx, 2, 3)) {
            if (ctx->Type3->oem_defined == LAZYBIOS_NOT_FOUND_U32) {
                lb_printf("OEM Defined: Not Present\n");
            } else {
                lb_printf("OEM-defined DWORD: 0x%08X\n", ctx->Type3->oem_defined);
            }

            if (ctx->Type3->height == LAZYBIOS_NOT_FOUND_U8) {
                lb_printf("Height: Not Present\n");
            } else if (ctx->Type3->height == 0x00) {
                lb_printf("Height: Unspecified\n");
            } else {
                lb_printf("Height: %uU\n", ctx->Type3->height);
            }

            if (ctx->Type3->number_of_power_cords == LAZYBIOS_NOT_FOUND_U8) {
                lb_printf("Number of Power Cords: Not Present\n");
            } else {
                lb_printf("Number of Power Cords: %u\n", ctx->Type3->number_of_power_cords);
            }

            if (ctx->Type3->contained_element_count == LAZYBIOS_NOT_FOUND_U8) {
                lb_printf("Contained Elements: Not Present\n");
            } else if (ctx->Type3->contained_element_count == 0) {
                lb_printf("Contained Elements: None\n");
            } else {
                lb_printf("Contained Elements (%u):\n", ctx->Type3->contained_element_count);

                for (uint8_t i = 0; i < ctx->Type3->contained_element_count; i++) {
                    uint8_t *record = ctx->Type3->contained_elements +
                                      i * ctx->Type3->contained_element_record_length;

                    uint8_t type_byte = record[0];
                    uint8_t min_count = record[1];
                    uint8_t max_count = record[2];

                    const char *type_str = lazybiosType3ContainedElementTypeStr(type_byte);

                    lb_printf("  Element %u: Type = %s, Min = %u, Max = %u\n",
                              i + 1, type_str, min_count, max_count);

                    // Optional: print extra bytes if present
                    for (uint8_t j = 3; j < ctx->Type3->contained_element_record_length; j++) {
                        lb_printf("    Extra byte %u: 0x%02X\n", j, record[j]);
                    }
                }
            } // This field is kinda tricky since it has its own sub fields, I may "fix" and add a propper decoder for this in the future!

            // Contained Element Record Length
            if (ctx->Type3->contained_element_record_length == LAZYBIOS_NOT_FOUND_U8) {
                // Already covered by "Contained Elements: Not Present"
            } else if (ctx->Type3->contained_element_record_length > 0) {
                lb_printf("Contained Element Record Length: %u bytes\n", ctx->Type3->contained_element_record_length);
            }

            if (ISVERPLUS(ctx, 2, 7)) {
                // SKU Number
                if (ctx->Type3->sku_number == LAZYBIOS_NOT_FOUND_STR) {
                    lb_printf("SKU Number: Not Present\n");
                } else {
                    lb_printf("SKU Number: %s\n", ctx->Type3->sku_number);
                }
            } else {
                lb_printf("SKU Number: [SMBIOS 2.7 required]\n");
            }

            if (ISVERPLUS(ctx, 3, 9)) {
                if (ctx->Type3->rack_type == LAZYBIOS_NOT_FOUND_U8) {
                    lb_printf("Rack Type: Not Present\n");
                } else {
                    lb_printf("Rack Type: 0x%02X\n", ctx->Type3->rack_type);
                }

                // Rack Height (extended)
                if (ctx->Type3->rack_height == LAZYBIOS_NOT_FOUND_U8) {
                    lb_printf("Rack Height (extended): Not Present\n");
                } else {
                    lb_printf("Rack Height (extended): %uU\n", ctx->Type3->rack_height);
                }
            } else {
                lb_printf("Rack Type: [SMBIOS 3.9 required]\n");
                lb_printf("Rack Height (extended): [SMBIOS 3.9 required]\n");
            }
        } else {

            lb_printf("OEM Defined: [SMBIOS 2.3 required]\n");
            lb_printf("Height: [SMBIOS 2.3 required]\n");
            lb_printf("Number of Power Cords: [SMBIOS 2.3 required]\n");
            lb_printf("Contained Elements: [SMBIOS 2.3 required]\n");
            lb_printf("SKU Number: [SMBIOS 2.7 required]\n");
            lb_printf("Rack Type: [SMBIOS 3.9 required]\n");
            lb_printf("Rack Height (extended): [SMBIOS 3.9 required]\n");
        }
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
    lb_printf("  -dump <dir_to_dump>        Dumps both the raw entry info and raw DMI table into 2 files into <dir_to_dump> directory, If OS is Windows it only dumps DMI.bin\n If <dir_to_dump> isn't specified it will default to the current directory\n");
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

    if (!ctx->Type2) {
        ctx->Type2 = lazybiosGetType2(ctx);
    }
    printType2(ctx);

    if (!ctx->Type3) {
        ctx->Type3 = lazybiosGetType3(ctx);
    }
    printType3(ctx);

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

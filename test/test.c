//
// test.c - File for testing ALL SMBIOS types we have implemented currently
//
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "lazybios.h"

void printType0(lazybiosCTX_t* ctx) {
    printf("=== BIOS INFORMATION ===\n");

    if (!ctx->Type0) ctx->Type0 = lazybiosGetType0(ctx);

    if (ctx->Type0) {
        printf("Vendor: %s\n", ctx->Type0->vendor);
        printf("Version: %s\n", ctx->Type0->version);
        printf("Release Date: %s\n", ctx->Type0->release_date);
        
        if (ctx->Type0->bios_starting_segment == LAZYBIOS_NOT_FOUND_U16) {
            printf("BIOS Starting Segment: Not Present\n");
        } else {
            printf("BIOS Starting Segment: 0x%04X\n", ctx->Type0->bios_starting_segment);
        }
        
        if (ctx->Type0->characteristics == LAZYBIOS_NOT_FOUND_U64) {
            printf("Firmware Characteristics: Not Present\n");
        } else {
            printf("Firmware Characteristics: %s\n", lazybiosType0CharacteristicsStr(ctx->Type0->characteristics));
        }
        
        if (ISVERPLUS(ctx, 2, 4)) {
            if (ctx->Type0->firmware_char_ext_bytes_count >= 1) {
                printf("Firmware Characteristics Extension Bytes 1: %s\n", lazybiosType0CharacteristicsExtByte1Str(ctx->Type0->firmware_char_ext_bytes[0]));
            } else {
                printf("Firmware Characteristics Extension Bytes 1: Not Present\n");
            }

            if (ctx->Type0->firmware_char_ext_bytes_count >= 2) {
                printf("Firmware Characteristics Extension Bytes 2: %s\n", lazybiosType0CharacteristicsExtByte2Str(ctx->Type0->firmware_char_ext_bytes[1]));
            } else {
                printf("Firmware Characteristics Extension Bytes 2: Not Present\n");
            }
            
            if (ctx->Type0->platform_major_release == LAZYBIOS_NOT_FOUND_U8) {
                printf("Platform Major Release: Not Present\n");
            } else {
                printf("Platform Major Release: %hu\n", ctx->Type0->platform_major_release);
            }
            
            if (ctx->Type0->platform_minor_release == LAZYBIOS_NOT_FOUND_U8) {
                printf("Platform Minor Release: Not Present\n");
            } else {
                printf("Platform Minor Release: %hu\n", ctx->Type0->platform_minor_release);
            }
            
            if (ctx->Type0->ec_major_release == LAZYBIOS_NOT_FOUND_U8) {
                printf("EC Major Release: Not Present\n");
            } else {
                printf("EC Major Release: %hu\n", ctx->Type0->ec_major_release);
            }
            
            if (ctx->Type0->ec_minor_release == LAZYBIOS_NOT_FOUND_U8) {
                printf("EC Minor Release: Not Present\n");
            } else {
                printf("EC Minor Release: %hu\n", ctx->Type0->ec_minor_release);
            }
        } else {
            printf("Firmware Characteristics Extension Bytes 1: [SMBIOS 2.4 required]\n");
            printf("Firmware Characteristics Extension Bytes 2: [SMBIOS 2.4 required]\n");
            printf("Platform Major Release: [SMBIOS 2.4 required]\n");
            printf("Platform Minor Release: [SMBIOS 2.4 required]\n");
            printf("EC Major Release: [SMBIOS 2.4 required]\n");
            printf("EC Minor Release: [SMBIOS 2.4 required]\n");
        }

        if (ISVERPLUS(ctx, 3, 1)) {
            if (ctx->Type0->extended_rom_size == LAZYBIOS_NOT_FOUND_U16) {
                printf("Extended ROM Size: Not Present\n\n");
            } else {
                printf("Extended ROM Size: %hu%s\n\n", lazybiosType0ExtendedROMSizeU16(ctx->Type0->extended_rom_size, ctx->Type0->unit), ctx->Type0->unit);
            }
        } else {
            if (ctx->Type0->rom_size == LAZYBIOS_NOT_FOUND_U32) {
                printf("ROM Size: Not Present\n\n");
            } else {
                printf("ROM Size: %u KB\n\n", ctx->Type0->rom_size);
            }
        }
    } else {
        printf("Failed to get BIOS information\n\n");
    }
}

void printType1(lazybiosCTX_t* ctx) {
    printf("=== SYSTEM INFORMATION ===\n");

    if (!ctx->Type1) ctx->Type1 = lazybiosGetType1(ctx);

    if (ctx->Type1) {
        printf("Manufacturer: %s\n", ctx->Type1->manufacturer);
        printf("Product name: %s\n", ctx->Type1->product_name);
        printf("Version: %s\n", ctx->Type1->version);
        printf("Serial number: %s\n", ctx->Type1->serial_number);

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
                printf("UUID: %02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
                         ctx->Type1->uuid[0], ctx->Type1->uuid[1], ctx->Type1->uuid[2], ctx->Type1->uuid[3],
                         ctx->Type1->uuid[4], ctx->Type1->uuid[5], ctx->Type1->uuid[6], ctx->Type1->uuid[7],
                         ctx->Type1->uuid[8], ctx->Type1->uuid[9], ctx->Type1->uuid[10], ctx->Type1->uuid[11],
                         ctx->Type1->uuid[12], ctx->Type1->uuid[13], ctx->Type1->uuid[14], ctx->Type1->uuid[15]);
            } else {
                printf("UUID: Not Present\n");
            }

            if (ctx->Type1->wake_up_type == LAZYBIOS_NOT_FOUND_U8) {
                printf("Wake up type: Not Present\n");
            } else {
                printf("Wake up type: %s\n", lazybiosType1WakeupTypeStr(ctx->Type1->wake_up_type));
            }
        } else {
            printf("UUID: [SMBIOS 2.1 required]\n");
            printf("Wake up type: [SMBIOS 2.1 required]\n");
        }

        if (ISVERPLUS(ctx, 2, 4)) {
            // SKU and Family are SMBIOS 2.4+
            printf("SKU number: %s\n", ctx->Type1->sku_number);
            printf("Family: %s\n\n", ctx->Type1->family);
        } else {
            printf("SKU number: [SMBIOS 2.4 required]\n");
            printf("Family: [SMBIOS 2.4 required]\n\n");
        }
    } else {
        printf("Failed to get System information\n\n");
    }
}

void printType2(lazybiosCTX_t* ctx) {
    printf("=== BASEBOARD INFORMATION ===\n");

    if (!ctx->Type2) ctx->Type2 = lazybiosGetType2(ctx);

    if (ctx->Type2) {
        printf("Manufacturer: %s\n", ctx->Type2->manufacturer);
        printf("Product: %s\n", ctx->Type2->product);
        printf("Version: %s\n", ctx->Type2->version);
        printf("Serial number: %s\n", ctx->Type2->serial_number);
        printf("Asset tag: %s\n", ctx->Type2->asset_tag);

        if (ISVERPLUS(ctx, 2, 1)) {
            if (ctx->Type2->feature_flags == LAZYBIOS_NOT_FOUND_U8) {
                printf("Feature flags: Not Present\n");
            } else {
                printf("Feature flags: %s\n", lazybiosType2FeatureflagsStr(ctx->Type2->feature_flags));
            }

            printf("Location in Chassis: %s\n", ctx->Type2->location_in_chassis);

            if (ctx->Type2->chassis_handle == LAZYBIOS_NOT_FOUND_U16) {
                printf("Chassis Handle: Not Present\n");
            } else {
                printf("Chassis Handle: 0x%lX\n", ctx->Type2->chassis_handle);
            }

            if (ctx->Type2->board_type == LAZYBIOS_NOT_FOUND_U8) {
                printf("Board Type: Not Present\n");
            } else {
                printf("Board Type: %s\n", lazybiosType2BoardTypeStr(ctx->Type2->board_type));
            }

            if (ctx->Type2->number_of_contained_object_handles == LAZYBIOS_NOT_FOUND_U8 ||
                ctx->Type2->number_of_contained_object_handles == 0) {
                // No contained object handles or field not present here
            } else {
                for (int i = 0; i < ctx->Type2->number_of_contained_object_handles; i++) {
                    printf("  Contained Object Handle %d: 0x%04X\n",
                             i, ctx->Type2->contained_object_handles[i]);
                }
            }
        } else {
            printf("Feature flags: [SMBIOS 2.1 required]\n");
            printf("Location in Chassis: [SMBIOS 2.1 required]\n");
            printf("Chassis Handle: [SMBIOS 2.1 required]\n");
            printf("Board Type: [SMBIOS 2.1 required]\n");
            printf("Contained Object Handles: [SMBIOS 2.1 required]\n\n");
        }
    } else {
        printf("Failed to get Baseboard information\n\n");
    }
}

void printType3(lazybiosCTX_t* ctx) {
    printf("=== CHASSIS INFORMATION ===\n");

    if (!ctx->Type3) ctx->Type3 = lazybiosGetType3(ctx);

    if (ctx->Type3) {
        printf("Manufacturer: %s\n", ctx->Type3->manufacturer);
        if (ctx->Type3->type == LAZYBIOS_NOT_FOUND_U8) {
            printf("Type: Not Present\n");
        } else {
            printf("Type: %s\n", lazybiosType3TypeStr(ctx->Type3->type));
        }
        printf("Version: %s\n", ctx->Type3->version);
        printf("Serial Number: %s\n", ctx->Type3->serial_number);
        printf("Asset Tag: %s\n", ctx->Type3->asset_tag);
        if (ISVERPLUS(ctx, 2, 1)) {
            if (ctx->Type3->boot_up_state == LAZYBIOS_NOT_FOUND_U8) {
                printf("BootUp State: Not Present\n");
            } else {
                printf("BootUp State: %s\n", lazybiosType3StateStr(ctx->Type3->boot_up_state));
            }

            if (ctx->Type3->power_supply_state == LAZYBIOS_NOT_FOUND_U8) {
                printf("Power Supply State: Not Present\n");
            } else {
                printf("Power Supply State: %s\n", lazybiosType3StateStr(ctx->Type3->power_supply_state));
            }

            if (ctx->Type3->thermal_state == LAZYBIOS_NOT_FOUND_U8) {
                printf("Thermal State: Not Present\n");
            } else {
                printf("Thermal State: %s\n", lazybiosType3StateStr(ctx->Type3->thermal_state));
            }

            if (ctx->Type3->security_status == LAZYBIOS_NOT_FOUND_U8) {
                printf("Security Status: Not Present\n");
            } else {
                printf("Security Status: %s\n", lazybiosType3SecurityStatusStr(ctx->Type3->security_status));
            }

        } else {
            printf("BootUp State: [SMBIOS 2.1 required]\n");
            printf("Power Supply State: [SMBIOS 2.1 required]\n");
            printf("Thermal State: [SMBIOS 2.1 required]\n");
            printf("Security Status: [SMBIOS 2.1 required]\n");
        }

        if (ISVERPLUS(ctx, 2, 3)) {
            if (ctx->Type3->oem_defined == LAZYBIOS_NOT_FOUND_U32) {
                printf("OEM Defined: Not Present\n");
            } else {
                printf("OEM-defined DWORD: 0x%08X\n", ctx->Type3->oem_defined);
            }

            if (ctx->Type3->height == LAZYBIOS_NOT_FOUND_U8) {
                printf("Height: Not Present\n");
            } else if (ctx->Type3->height == 0x00) {
                printf("Height: Unspecified\n");
            } else {
                printf("Height: %uU\n", ctx->Type3->height);
            }

            if (ctx->Type3->number_of_power_cords == LAZYBIOS_NOT_FOUND_U8) {
                printf("Number of Power Cords: Not Present\n");
            } else {
                printf("Number of Power Cords: %u\n", ctx->Type3->number_of_power_cords);
            }

            if (ctx->Type3->contained_element_count == LAZYBIOS_NOT_FOUND_U8) {
                printf("Contained Elements: Not Present\n");
            } else if (ctx->Type3->contained_element_count == 0) {
                printf("Contained Elements: None\n");
            } else {
                printf("Contained Elements (%u):\n", ctx->Type3->contained_element_count);

                for (uint8_t i = 0; i < ctx->Type3->contained_element_count; i++) {
                    uint8_t *record = ctx->Type3->contained_elements +
                                      i * ctx->Type3->contained_element_record_length;

                    uint8_t type_byte = record[0];
                    uint8_t min_count = record[1];
                    uint8_t max_count = record[2];

                    const char *type_str = lazybiosType3ContainedElementTypeStr(type_byte);

                    printf("  Element %u: Type = %s, Min = %u, Max = %u\n",
                              i + 1, type_str, min_count, max_count);

                    // Optional: print extra bytes if present
                    for (uint8_t j = 3; j < ctx->Type3->contained_element_record_length; j++) {
                        printf("    Extra byte %u: 0x%02X\n", j, record[j]);
                    }
                }
            } // This field is kinda tricky since it has its own sub fields, I may "fix" and add a propper decoder for this in the future!

            // Contained Element Record Length
            if (ctx->Type3->contained_element_record_length == LAZYBIOS_NOT_FOUND_U8) {
                // Already covered by "Contained Elements: Not Present"
            } else if (ctx->Type3->contained_element_record_length > 0) {
                printf("Contained Element Record Length: %u bytes\n", ctx->Type3->contained_element_record_length);
            }

            if (ISVERPLUS(ctx, 2, 7)) {
                printf("SKU Number: %s\n", ctx->Type3->sku_number);
            } else {
                printf("SKU Number: [SMBIOS 2.7 required]\n");
            }

            if (ISVERPLUS(ctx, 3, 9)) {
                if (ctx->Type3->rack_type == LAZYBIOS_NOT_FOUND_U8) {
                    printf("Rack Type: Not Present\n");
                } else {
                    printf("Rack Type: 0x%02X\n", ctx->Type3->rack_type);
                }

                if (ctx->Type3->rack_height == LAZYBIOS_NOT_FOUND_U8) {
                    printf("Rack Height (extended): Not Present\n");
                } else {
                    printf("Rack Height (extended): %uU\n", ctx->Type3->rack_height);
                }
            } else {
                printf("Rack Type: [SMBIOS 3.9 required]\n");
                printf("Rack Height (extended): [SMBIOS 3.9 required]\n");
            }
        } else {

            printf("OEM Defined: [SMBIOS 2.3 required]\n");
            printf("Height: [SMBIOS 2.3 required]\n");
            printf("Number of Power Cords: [SMBIOS 2.3 required]\n");
            printf("Contained Elements: [SMBIOS 2.3 required]\n");
            printf("SKU Number: [SMBIOS 2.7 required]\n");
            printf("Rack Type: [SMBIOS 3.9 required]\n");
            printf("Rack Height (extended): [SMBIOS 3.9 required]\n\n");
        }
    } else {
        printf("Failed to get Chassis Information!\n\n");
    }
}

void printType4(lazybiosCTX_t* ctx) {
    printf("=== PROCESSOR INFORMATION ===\n");

    if (!ctx->Type4) ctx->Type4 = lazybiosGetType4(ctx);

    if (ctx->Type4) {
        printf("Socket Designation: %s\n", ctx->Type4->socket_designation);
        
        if (ctx->Type4->processor_type == LAZYBIOS_NOT_FOUND_U8) {
            printf("Processor Type: Not Present\n");
        } else {
            printf("Processor Type: %s\n", lazybiosType4TypeStr(ctx->Type4->processor_type));
        }
        
        if (ctx->Type4->processor_family == LAZYBIOS_NOT_FOUND_U8) {
            printf("Processor Family: Not Present\n");
        } else {
            printf("Processor Family: %s\n", lazybiosType4ProcessorFamilyStr(ctx->Type4->processor_family));
        }
        
        printf("Processor Manufacturer: %s\n", ctx->Type4->processor_manufacturer);
        
        if (ctx->Type4->processor_id == LAZYBIOS_NOT_FOUND_U64) {
            printf("Processor ID: Not Present\n");
        } else {
            printf("Processor ID: 0x%016llX\n", ctx->Type4->processor_id);
        }
        
        printf("Processor Version: %s\n", ctx->Type4->processor_version);
        
        if (ctx->Type4->voltage == LAZYBIOS_NOT_FOUND_U8) {
            printf("Voltage: Not Present\n");
        } else {
            printf("Voltage: 0x%02X\n", ctx->Type4->voltage);
        }
        
        if (ctx->Type4->external_clock == LAZYBIOS_NOT_FOUND_U16) {
            printf("External Clock: Not Present\n");
        } else {
            printf("External Clock: %u MHz\n", ctx->Type4->external_clock);
        }
        
        if (ctx->Type4->max_speed == LAZYBIOS_NOT_FOUND_U16) {
            printf("Max Speed: Not Present\n");
        } else {
            printf("Max Speed: %u MHz\n", ctx->Type4->max_speed);
        }
        
        if (ctx->Type4->current_speed == LAZYBIOS_NOT_FOUND_U16) {
            printf("Current Speed: Not Present\n");
        } else {
            printf("Current Speed: %u MHz\n", ctx->Type4->current_speed);
        }
        
        if (ctx->Type4->status == LAZYBIOS_NOT_FOUND_U8) {
            printf("Status: Not Present\n");
        } else {
            printf("Status: %s\n", lazybiosType4StatusStr(ctx->Type4->status));
        }
        
        if (ctx->Type4->processor_upgrade == LAZYBIOS_NOT_FOUND_U8) {
            printf("Processor Upgrade: Not Present\n");
        } else {
            printf("Processor Upgrade: %s\n", lazybiosType4SocketTypeStr(ctx->Type4->processor_upgrade));
        }
        
        if (ISVERPLUS(ctx, 2, 1)) {
            if (ctx->Type4->l1_cache_handle == LAZYBIOS_NOT_FOUND_U16) {
                printf("L1 Cache Handle: Not Present\n");
            } else {
                printf("L1 Cache Handle: 0x%04X\n", ctx->Type4->l1_cache_handle);
            }
            
            if (ctx->Type4->l2_cache_handle == LAZYBIOS_NOT_FOUND_U16) {
                printf("L2 Cache Handle: Not Present\n");
            } else {
                printf("L2 Cache Handle: 0x%04X\n", ctx->Type4->l2_cache_handle);
            }
            
            if (ctx->Type4->l3_cache_handle == LAZYBIOS_NOT_FOUND_U16) {
                printf("L3 Cache Handle: Not Present\n");
            } else {
                printf("L3 Cache Handle: 0x%04X\n", ctx->Type4->l3_cache_handle);
            }
        } else {
            printf("L1 Cache Handle: [SMBIOS 2.1 required]\n");
            printf("L2 Cache Handle: [SMBIOS 2.1 required]\n");
            printf("L3 Cache Handle: [SMBIOS 2.1 required]\n");
        }
        
        if (ISVERPLUS(ctx, 2, 3)) {
            printf("Serial Number: %s\n", ctx->Type4->serial_number);
            printf("Asset Tag: %s\n", ctx->Type4->asset_tag);
            printf("Part Number: %s\n", ctx->Type4->part_number);
        } else {
            printf("Serial Number: [SMBIOS 2.3 required]\n");
            printf("Asset Tag: [SMBIOS 2.3 required]\n");
            printf("Part Number: [SMBIOS 2.3 required]\n");
        }
        
        if (ISVERPLUS(ctx, 2, 5)) {
            uint32_t actual_core_count = LAZYBIOS_NOT_FOUND_U32;
            if (ctx->Type4->core_count == 0xFF && ctx->Type4->core_count_2 != LAZYBIOS_NOT_FOUND_U16) {
                actual_core_count = ctx->Type4->core_count_2;
            } else if (ctx->Type4->core_count != LAZYBIOS_NOT_FOUND_U8 && ctx->Type4->core_count != 0xFF) {
                actual_core_count = ctx->Type4->core_count;
            }
            
            if (actual_core_count == LAZYBIOS_NOT_FOUND_U32) {
                printf("Core Count: Not Present\n");
            } else {
                printf("Core Count: %u\n", actual_core_count);
            }

            uint32_t actual_core_enabled = LAZYBIOS_NOT_FOUND_U32;
            if (ctx->Type4->core_enabled == 0xFF && ctx->Type4->core_enabled_2 != LAZYBIOS_NOT_FOUND_U16) {
                actual_core_enabled = ctx->Type4->core_enabled_2;
            } else if (ctx->Type4->core_enabled != LAZYBIOS_NOT_FOUND_U8 && ctx->Type4->core_enabled != 0xFF) {
                actual_core_enabled = ctx->Type4->core_enabled;
            }
            
            if (actual_core_enabled == LAZYBIOS_NOT_FOUND_U32) {
                printf("Core Enabled: Not Present\n");
            } else {
                printf("Core Enabled: %u\n", actual_core_enabled);
            }
            
            // Thread count logic
            uint32_t actual_thread_count = LAZYBIOS_NOT_FOUND_U32;
            if (ctx->Type4->thread_count == 0xFF && ctx->Type4->thread_count_2 != LAZYBIOS_NOT_FOUND_U16) {
                actual_thread_count = ctx->Type4->thread_count_2;
            } else if (ctx->Type4->thread_count != LAZYBIOS_NOT_FOUND_U8 && ctx->Type4->thread_count != 0xFF) {
                actual_thread_count = ctx->Type4->thread_count;
            }
            
            if (actual_thread_count == LAZYBIOS_NOT_FOUND_U32) {
                printf("Thread Count: Not Present\n");
            } else {
                printf("Thread Count: %u\n", actual_thread_count);
            }
            
            if (ctx->Type4->processor_characteristics == LAZYBIOS_NOT_FOUND_U16) {
                printf("Processor Characteristics: Not Present\n");
            } else {
                printf("Processor Characteristics: %s\n", lazybiosType4CharacteristicsStr(ctx->Type4->processor_characteristics));
            }
            
            if (ctx->Type4->processor_family_2 == LAZYBIOS_NOT_FOUND_U16) {
                printf("Processor Family 2: Not Present\n");
            } else {
                printf("Processor Family 2: %s\n", lazybiosType4ProcessorFamilyStr(ctx->Type4->processor_family_2));
            }
        } else {
            printf("Core Count: [SMBIOS 2.5 required]\n");
            printf("Core Enabled: [SMBIOS 2.5 required]\n");
            printf("Thread Count: [SMBIOS 2.5 required]\n");
            printf("Processor Characteristics: [SMBIOS 2.5 required]\n");
            printf("Processor Family 2: [SMBIOS 2.5 required]\n");
        }
        
        if (ISVERPLUS(ctx, 3, 6)) {
            if (ctx->Type4->thread_enabled == LAZYBIOS_NOT_FOUND_U16) {
                printf("Thread Enabled: Not Present\n");
            } else {
                printf("Thread Enabled: %u\n", ctx->Type4->thread_enabled);
            }
        } else {
            printf("Thread Enabled: [SMBIOS 3.6 required]\n");
        }
        
        if (ISVERPLUS(ctx, 3, 8)) {
            printf("Socket Type: %s\n", ctx->Type4->socket_type);
        } else {
            printf("Socket Type: [SMBIOS 3.8 required]\n\n");
        }
        
        printf("\n");
    } else {
        printf("Failed to get Processor information!\n\n");
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
    printf("  --help                      Show this help message\n");
    printf("  --type <number>             prints the SMBIOS type <number>, so --type 0 means Firmware Information etc.");
    printf("  --dump <dir_to_dump>        Dumps both the raw entry info and raw DMI table into 2 files into <dir_to_dump> directory, If OS is Windows it only dumps DMI.bin\n If <dir_to_dump> isn't specified it will default to the current directory\n");
    printf("  --sources <entry> <dmi>     Usees the parser on the 2 specified files <entry> and <dmi>\n");
    printf("  --single-source <binary>    Uses the parser on the single specified file <binary>, which should hold the entry point first and dmi data merged together\n");
}

int main(int argc, const char *argv[]) {
    printf("lazybios Version: %s\n", LAZYBIOS_VER);
    printf("=============================================\n\n");

    lazybiosCTX_t* ctx = lazybiosCTXNew();
    if (!ctx) {
        fprintf(stderr, "Failed to allocate lazybios context\n");
        return 1;
    }

    int print_all = 1;
    int type_to_print = -1;
    const char *dump_dir = LAZYBIOS_NULL;
    const char *entry_file = LAZYBIOS_NULL;
    const char *dmi_file = LAZYBIOS_NULL;
    const char *single_file = LAZYBIOS_NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            lazybiosCleanup(ctx);
            return 0;
        }
        else if (strcmp(argv[i], "--type") == 0) {
            if (i + 1 < argc) {
                type_to_print = atoi(argv[i + 1]);
                print_all = 0;
                i++;
            } else {
                fprintf(stderr, "Error: --type requires a type number\n");
                print_usage(argv[0]);
                lazybiosCleanup(ctx);
                return -1;
            }
        }
        else if (strcmp(argv[i], "--dump") == 0) {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                dump_dir = argv[i + 1];
                i++;
            } else {
                dump_dir = ".";  // Here we default to the current directory
            }
        }
        else if (strcmp(argv[i], "--sources") == 0) {
            if (i + 2 < argc) {
                entry_file = argv[i + 1];
                dmi_file = argv[i + 2];
                i += 2;
            } else {
                fprintf(stderr, "Error: --sources requires two file arguments\n");
                print_usage(argv[0]);
                lazybiosCleanup(ctx);
                return -1;
            }
        }
        else if (strcmp(argv[i], "--single-source") == 0) {
            if (i + 1 < argc) {
                single_file = argv[i + 1];
                i++;  // Skip the file name
            } else {
                fprintf(stderr, "Error: --single-source requires a file argument\n");
                print_usage(argv[0]);
                lazybiosCleanup(ctx);
                return -1;
            }
        }
        else {
            fprintf(stderr, "Error: Unknown argument '%s'\n", argv[i]);
            print_usage(argv[0]);
            lazybiosCleanup(ctx);
            return -1;
        }
    }

    if (dump_dir) {
        lazybiosInit(ctx);

        char path_entry[1024];
        char path_dmi[1024];

        #if defined(_WIN32) || defined(_WIN64)
                snprintf(path_dmi, sizeof(path_dmi), "%s\\DMI.bin", dump_dir);
                snprintf(path_entry, sizeof(path_entry), "%s\\smbios_entry_point", dump_dir);
        #else
                snprintf(path_dmi, sizeof(path_dmi), "%s/DMI", dump_dir);
                snprintf(path_entry, sizeof(path_entry), "%s/smbios_entry_point", dump_dir);
        #endif

        FILE* entry = NULL;
        FILE* dmi   = NULL;
        if (ctx->backend == LAZYBIOS_BACKEND_LINUX) {
            entry = fopen(path_entry, "wb");
            if (!entry) {
                printf("Failed to open '%s': %s\n", path_entry, strerror(errno));
                return -1;
            }

            dmi = fopen(path_dmi, "wb");
            if (!dmi) {
                printf("Failed to open '%s': %s\n", path_dmi, strerror(errno));
                fclose(entry);
                return -1;
            }

            fwrite(ctx->dmi_data, 1, ctx->dmi_len, dmi);
            fwrite(ctx->entry_data, 1, ctx->entry_len, entry);
            fclose(entry);
            fclose(dmi);
            printf("%s and %s dumped successfully\n", path_entry, path_dmi);

        } else { // This is for Windows
            dmi = fopen(path_dmi, "wb");
            printf("Dumping only DMI data to '%s'\n", path_dmi);

            if (!dmi) {
                printf("Couldn't create/modify the file: %s\n", strerror(errno));
                return -1;
            }

            fwrite(ctx->dmi_data, 1, ctx->dmi_len, dmi);
            fclose(dmi);
            printf("%s created/filled successfully\n", path_dmi);
        }

        lazybiosCleanup(ctx);
        return 0;
    }

    // We initialize from custom files ONLY if specified
    if (entry_file && dmi_file) {
        if (lazybiosFile(ctx, entry_file, dmi_file) != 0) {
            fprintf(stderr, "Failed to initialize lazybios from files\n");
            lazybiosCleanup(ctx);
            return -1;
        }
    }
    else if (single_file) {
        if (lazybiosSingleFile(ctx, single_file) != 0) {
            fprintf(stderr, "Failed to initialize lazybios from single file\n");
            lazybiosCleanup(ctx);
            return -1;
        }
    }
    else {
        if (lazybiosInit(ctx) != 0) {
            fprintf(stderr, "Failed to initialize lazybios library\n");
            lazybiosCleanup(ctx);
            return -1;
        }
    }

    printf("Library initialized successfully!\n\n");

    print_smbios_version_info(ctx);

    if (print_all) {
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

        if (!ctx->Type4) {
            ctx->Type4 = lazybiosGetType4(ctx);
        }
        printType4(ctx);
    }
    else {
        switch (type_to_print) {
            case 0:
                if (!ctx->Type0) {
                    ctx->Type0 = lazybiosGetType0(ctx);
                }
                printType0(ctx);
                break;

            case 1:
                if (!ctx->Type1) {
                    ctx->Type1 = lazybiosGetType1(ctx);
                }
                printType1(ctx);
                break;

            case 2:
                if (!ctx->Type2) {
                    ctx->Type2 = lazybiosGetType2(ctx);
                }
                printType2(ctx);
                break;

            case 3:
                if (!ctx->Type3) {
                    ctx->Type3 = lazybiosGetType3(ctx);
                }
                printType3(ctx);
                break;

            case 4:
                if (!ctx->Type4) {
                    ctx->Type4 = lazybiosGetType4(ctx);
                }
                printType4(ctx);
                break;

            default:
                fprintf(stderr, "Error: Type %d is not implemented or invalid\n", type_to_print);
                lazybiosCleanup(ctx);
                return -1;
        }
    }

    if (lazybiosCleanup(ctx) == 0) {
        printf("Library cleanup completed!\n");
    } else {
        fprintf(stderr, "Could not complete library cleanup\n");
        return -1;
    }

    printf("Operation completed successfully!\n");
    return 0;
}

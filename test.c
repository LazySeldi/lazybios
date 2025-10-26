#include <stdio.h>
#include <stdlib.h>
#include "lazybios.h"

void print_bios_info(void) {
    printf("=== BIOS INFORMATION ===\n");
    bios_info_t *bios = get_bios_info();
    if (bios) {
        printf("Vendor: %s\n", bios->vendor);
        printf("Version: %s\n", bios->version);
        printf("Release Date: %s\n", bios->release_date);
        printf("ROM Size: %u KB\n", bios->rom_size_kb);
        printf("\n");
    } else {
        printf("Failed to get BIOS information\n\n");
    }
}

void print_system_info(void) {
    printf("=== SYSTEM INFORMATION ===\n");
    system_info_t *sys = get_system_info();
    if (sys) {
        printf("Manufacturer: %s\n", sys->manufacturer);
        printf("Product Name: %s\n", sys->product_name);
        printf("Version: %s\n", sys->version);
        printf("Serial Number: %s\n", sys->serial_number);
        printf("\n");
    } else {
        printf("Failed to get system information\n\n");
    }
}

void print_chassis_info(void) {
    printf("=== CHASSIS INFORMATION ===\n");
    chassis_info_t *chassis = get_chassis_info();
    if (chassis) {
        printf("Asset Tag: %s\n", chassis->asset_tag);
        printf("SKU: %s\n", chassis->sku);
        printf("\n");
    } else {
        printf("Failed to get chassis information\n\n");
    }
}

void print_processor_info(void) {
    printf("=== PROCESSOR INFORMATION ===\n");
    processor_info_t *proc = get_processor_info();
    if (proc) {
        printf("Socket: %s\n", proc->socket_designation);
        printf("Version: %s\n", proc->version);
        printf("Family: %s (0x%02X)\n",
               get_processor_family_string(proc->processor_family),
               proc->processor_family);
        printf("Type: %u\n", proc->processor_type);
        printf("Cores: %u physical, %u enabled\n",
               proc->core_count, proc->core_enabled);
        printf("Threads: %u per core (%u total threads)\n",
               proc->thread_count, proc->core_count * proc->thread_count);
        printf("Max Speed: %u MHz\n", proc->max_speed_mhz);
        printf("External Clock: %u MHz\n", proc->external_clock_mhz);
        printf("Voltage: 0x%02X\n", proc->voltage);
        printf("Characteristics: 0x%04X\n", proc->characteristics);
        printf("Serial: %s\n", proc->serial_number);
        printf("Asset Tag: %s\n", proc->asset_tag);
        printf("Part Number: %s\n", proc->part_number);
        printf("\n");
    } else {
        printf("Failed to get processor information\n\n");
    }
}

void print_memory_info(void) {
    printf("=== MEMORY DEVICES ===\n");
    size_t mem_count;
    memory_device_t *memories = get_memory_devices(&mem_count);

    if (memories && mem_count > 0) {
        printf("Found %zu memory devices:\n", mem_count);

        for (size_t i = 0; i < mem_count; i++) {
            printf("Device %zu:\n", i + 1);
            printf("  Locator: %s\n", memories[i].locator);
            printf("  Bank: %s\n", memories[i].bank_locator);
            printf("  Manufacturer: %s\n", memories[i].manufacturer);
            printf("  Serial: %s\n", memories[i].serial_number);
            printf("  Part Number: %s\n", memories[i].part_number);

            if (memories[i].size_mb == 0) {
                printf("  Size: EMPTY SLOT\n");
            } else if (memories[i].size_mb == 0xFFFF) {
                printf("  Size: UNKNOWN\n");
            } else {
                printf("  Size: %u MB\n", memories[i].size_mb);
            }

            printf("  Speed: %u MHz\n", memories[i].speed_mhz);
            printf("  Type: 0x%02X\n", memories[i].memory_type);
            printf("  Form Factor: 0x%02X\n", memories[i].form_factor);
            printf("  Data Width: %u bits\n", memories[i].data_width);
            printf("\n");
        }

        // Calculate total memory
        size_t total_memory_mb = 0;
        size_t populated_slots = 0;

        for (size_t i = 0; i < mem_count; i++) {
            if (memories[i].size_mb > 0 && memories[i].size_mb != 0xFFFF) {
                total_memory_mb += memories[i].size_mb;
                populated_slots++;
            }
        }

        printf("Memory Summary:\n");
        printf("  Total Populated Slots: %zu\n", populated_slots);
        printf("  Total Memory: %zu MB (%.2f GB)\n",
               total_memory_mb, total_memory_mb / 1024.0);
        printf("\n");
    } else {
        printf("No memory devices found or failed to read memory information\n\n");
    }
}

void print_smbios_version_info(void) {
    printf("=== SMBIOS VERSION INFORMATION ===\n");
    smbios_ver();

    const smbios_entry_info_t *entry = get_entry_info();
    if (entry) {
        printf("Table Length: %u bytes\n", entry->table_length);
        printf("Table Address: 0x%lX\n", entry->table_address);
        printf("Is 64-bit: %s\n", entry->is_64bit ? "Yes" : "No");
        printf("\n");
    }
}

void print_system_summary(void) {
    printf("=== SYSTEM SUMMARY ===\n");

    system_info_t *sys = get_system_info();
    processor_info_t *proc = get_processor_info();

    if (sys && proc) {
        printf("%s %s\n", sys->manufacturer, sys->product_name);
        printf("%s Processor\n", get_processor_family_string(proc->processor_family));
        printf("%u cores, %u threads total\n",
               proc->core_count, proc->core_count * proc->thread_count);

        // Memory summary
        size_t mem_count;
        memory_device_t *memories = get_memory_devices(&mem_count);
        size_t total_memory_mb = 0;
        size_t populated_slots = 0;

        if (memories) {
            for (size_t i = 0; i < mem_count; i++) {
                if (memories[i].size_mb > 0 && memories[i].size_mb != 0xFFFF) {
                    total_memory_mb += memories[i].size_mb;
                    populated_slots++;
                }
            }
        }

        printf("%zu GB RAM across %zu slots\n",
               total_memory_mb / 1024, populated_slots);
    }
    printf("\n");
}

int main(void) {
    printf("lazybios - Comprehensive System Information\n");
    printf("=============================================\n\n");

    // Initialize the library
    if (init() != 0) {
        fprintf(stderr, "❌ Failed to initialize lazybios library\n");
        return 1;
    }

    printf("✅ Library initialized successfully!\n\n");

    // Show everything the library can do
    print_smbios_version_info();
    print_system_summary();
    print_bios_info();
    print_system_info();
    print_chassis_info();
    print_processor_info();
    print_memory_info();

    // Cleanup
    cleanup();
    printf("Library cleanup completed!\n");
    printf("All tests passed successfully!\n");

    return 0;
}
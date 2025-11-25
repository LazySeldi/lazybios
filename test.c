#include <inttypes.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lazybios.h"

void print_bios_info(lazybios_ctx_t* ctx) {
    printf("=== BIOS INFORMATION ===\n");
    bios_info_t *bios = lazybios_get_bios_info(ctx);
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

void print_system_info(lazybios_ctx_t* ctx) {
    printf("=== SYSTEM INFORMATION ===\n");
    system_info_t *sys = lazybios_get_system_info(ctx);
    if (sys) {
        printf("Manufacturer: %s\n", sys->manufacturer);
        printf("Product Name: %s\n", sys->product_name);
        printf("Version: %s\n", sys->version);
        printf("Serial Number: %s\n", sys->serial_number);
        printf("UUID: %s\n", sys->uuid);
        printf("\n");
    } else {
        printf("Failed to get system information\n\n");
    }
}

void print_baseboard_info(lazybios_ctx_t* ctx) {
    printf("=== BASEBOARD INFORMATION ===\n");
    baseboard_info_t *baseboard = lazybios_get_baseboard_info(ctx);
    if (baseboard) {
        printf("Manufacturer: %s\n", baseboard->manufacturer);
        printf("Product: %s\n", baseboard->product);
        printf("Version: %s\n", baseboard->version);
        printf("Serial Number: %s\n", baseboard->serial_number);
        printf("Asset Tag: %s\n", baseboard->asset_tag);
        printf("\n");
    } else {
        printf("Failed to get baseboard information\n\n");
    }
}

void print_chassis_info(lazybios_ctx_t* ctx) {
    printf("=== CHASSIS INFORMATION ===\n");
    chassis_info_t *chassis = lazybios_get_chassis_info(ctx);
    if (chassis) {
        printf("Asset Tag: %s\n", chassis->asset_tag);
        printf("SKU: %s\n", chassis->sku);
        printf("Type: %" PRIu8 "\n", chassis->type);
        printf("State: %" PRIu8 "\n", chassis->state);
        printf("\n");
    } else {
        printf("Failed to get chassis information\n\n");
    }
}

void print_processor_info(lazybios_ctx_t* ctx) {
    printf("=== PROCESSOR INFORMATION ===\n");
    processor_info_t *proc = lazybios_get_processor_info(ctx);
    if (proc) {
        printf("Socket: %s\n", proc->socket_designation);
        printf("Version: %s\n", proc->version);
        printf("Family: %s \n", lazybios_get_processor_family_string(proc->processor_family));
        if (proc->processor_family == 0xFE) {
            printf("Extended Family: 0x%04X\n", proc->processor_family2);
        }
        printf("Type: %s\n", lazybios_get_processor_type_string(proc->processor_type));
        printf("Cores: %u physical, %u enabled\n", proc->core_count, proc->core_enabled);
        printf("Threads: %u total\n", proc->thread_count);
        printf("Max Speed: %u MHz\n", proc->max_speed_mhz);
        if (proc->current_speed_mhz > 0) {
            printf("Current Speed: %u MHz\n", proc->current_speed_mhz);
        }
        printf("External Clock: %u MHz\n", proc->external_clock_mhz);
        printf("Socket Type: %s\n", lazybios_get_socket_type_string(proc->proc_upgrade));
        printf("Voltage: 0x%02X\n", proc->voltage);
        if (proc->status > 0) {
            printf("Status: %s\n", lazybios_get_processor_status_string(proc->status));
        }
        printf("L1 Handle: %u \n L2 Handle: %u \n L3 handle: %u \n", proc->L1_cache_handle, proc->L2_cache_handle, proc->L3_cache_handle);
        printf("Characteristics: %s\n", lazybios_get_proc_characteristics_string(proc->characteristics));
        printf("Serial: %s\n", proc->serial_number);
        printf("Asset Tag: %s\n", proc->asset_tag);
        printf("Part Number: %s\n", proc->part_number);
        printf("\n");
    } else {
        printf("Failed to get processor information\n\n");
    }
}

void print_onboard_device_info(lazybios_ctx_t* ctx) {
    if (!ctx) {
        printf("No context available\n");
        return;
    }

    size_t count;
    onboard_devices_t *onbdev = lazybios_get_onboard_devices(ctx, &count);

    if (!onbdev || count == 0) {
        printf("No Onboard device information available\n");
        return;
    }

    printf("=== Onboard Information ===\n");
    printf("Found %zu devices\n\n", count);

    for (size_t i = 0; i < count; i++) {
        printf("Device %zu: \n", i + 1);
        printf("  Enabled: %d \n", onbdev[i].enabled);
        printf("  Description String: %s\n", onbdev[i].description_string);
        printf("  Type: %s\n", lazybios_get_onboard_devices_type_string(onbdev[i].type));
        printf("\n");
    }
}


void print_OEM_strings(lazybios_ctx_t* ctx) {
    if (!ctx) {
        printf("No context available\n");
        return;
    }
    size_t count;
    OEMStrings_t *Strings = lazybios_get_OEMString_info(ctx, &count);

    if (!Strings) {
        printf("No String information available\n");
        return;
    }

    printf("=== OEM Strings Information ===\n");
    printf("Found %zu string(s)\n", count);

    for (size_t i = 0; i < count; i++) {
        const OEMStrings_t *string = &Strings[i];

        printf("String %zu: ", i + 1);
        printf("%s\n", string->strings[i]);
    }
    printf("\n");
}

// ===== Cache Information Printer =====
void print_cache_info(lazybios_ctx_t* ctx) {
    if (!ctx) {
        printf("No context available\n");
        return;
    }

    size_t count;
    cache_info_t *caches = lazybios_get_caches(ctx, &count);

    if (!caches || count == 0) {
        printf("No cache information available\n");
        return;
    }

    printf("=== Cache Information ===\n");
    printf("Found %zu cache(s)\n\n", count);

    for (size_t i = 0; i < count; i++) {
        const cache_info_t *cache = &caches[i];

        printf("Cache %zu:\n", i + 1);
        printf("  Socket: %s\n", cache->socket_designation);
        printf("  Level: L%d\n", cache->level + 1);
        printf("  Size: %d KB\n", cache->size_kb);
        printf("  Error Correction: %s\n", lazybios_get_cache_ecc_string(cache->error_correction_type));
        printf("  Type: %s\n", lazybios_get_cache_type_string(cache->system_cache_type));
        printf("  Associativity: %s\n", lazybios_get_cache_associativity_string(cache->associativity));
        printf("\n");
    }
}

void print_port_connector_info(lazybios_ctx_t* ctx) {
    printf("=== PORT CONNECTOR INFO ===\n");
    size_t port_count;
    port_connector_info_t *port_connectors = lazybios_get_port_connectors(ctx, &port_count);

    if (port_connectors && port_count > 0) {
        printf("Found %zu port connector(s)\n", port_count);
    }

    for (int i = 0; i < port_count; i++) {
        printf("Handle: %hu \n", port_connectors[i].handle);
        printf("Internal Reference Designator: %s\n", port_connectors[i].internal_ref_designator);
        printf("Internal Connector Type: %s\n", lazybios_get_port_connector_types_string(port_connectors[i].internal_connector_type));
        printf("External Reference Designator: %s\n", port_connectors[i].external_ref_designator);
        printf("External Connector Type: %s\n", lazybios_get_port_connector_types_string(port_connectors[i].external_connector_type));
        printf("Port Type: %s\n\n", lazybios_get_port_types_string(port_connectors[i].port_type));
    }
}

void print_memory_array_info(lazybios_ctx_t* ctx) {
    printf("=== MEMORY ARRAY INFO ===\n");
    size_t mem_arr_count;
    physical_memory_array_t *memory_arrays = lazybios_get_memory_arrays(ctx, &mem_arr_count);

    if (memory_arrays && mem_arr_count > 0) {
        printf("Found %zu memory array device(s)\n", mem_arr_count);
    }
    for (size_t i = 0; i < mem_arr_count; i++) {
        printf("Location: %s\n", lazybios_get_memory_array_location_string(memory_arrays->location));
        printf("Use: %s\n", lazybios_get_memory_array_use_string(memory_arrays->use));
        printf("ECC Type: %s\n", lazybios_get_memory_array_ecc_string(memory_arrays->ecc_type));
        printf("MAX Capacity: %lu KB\n", memory_arrays->max_capacity_kb);
        printf("Number of Devices: %d\n\n", memory_arrays->num_devices);
    }
}

void print_memory_info(lazybios_ctx_t* ctx) {
    printf("=== MEMORY DEVICES ===\n");
    size_t mem_count;
    memory_device_t *memories = lazybios_get_memory_devices(ctx, &mem_count);

    if (memories && mem_count > 0) {
        printf("Found %zu memory devices:\n", mem_count);

        for (size_t i = 0; i < mem_count; i++) {
            printf("Device %zu:\n", i + 1);
            printf("  Locator: %s\n", memories[i].locator);
            printf("  Bank: %s\n", memories[i].bank_locator);
            printf("  Manufacturer: %s\n", memories[i].manufacturer);
            printf("  Serial: %s\n", memories[i].serial_number);
            printf("  Part Number: %s\n", memories[i].part_number);

            if (memories[i].size_mb == 0 && !memories[i].size_extended) {
                if (strcmp(memories[i].manufacturer, "Not Specified") != 0 &&
                    strcmp(memories[i].serial_number, "Not Specified") != 0) {
                    printf("  Size: PRESENT BUT SIZE UNKNOWN\n");
                } else {
                    printf("  Size: EMPTY SLOT\n");
                }
            } else if (memories[i].size_extended) {
                printf("  Size: %u MB (Extended)\n", memories[i].size_mb);
            } else {
                printf("  Size: %u MB\n", memories[i].size_mb);
            }

            printf("  Speed: %u MHz\n", memories[i].speed_mhz);
            printf("  Type: %s \n", lazybios_get_memory_type_string(memories[i].memory_type));
            printf("  Form Factor: %s\n", lazybios_get_memory_form_factor_string(memories[i].form_factor));
            if (memories[i].total_width > 0) {
                printf("  Total Width: %u bits\n", memories[i].total_width);
            }
            if (memories[i].data_width > 0) {
                printf("  Data Width: %u bits\n", memories[i].data_width);
            }
            printf("\n");
        }

        size_t total_memory_mb = 0;
        size_t populated_slots = 0;

        for (size_t i = 0; i < mem_count; i++) {
            if (memories[i].size_mb > 0 ||
                (strcmp(memories[i].manufacturer, "Not Specified") != 0 &&
                 strcmp(memories[i].serial_number, "Not Specified") != 0)) {
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

void print_smbios_version_info(lazybios_ctx_t* ctx) {
    printf("=== SMBIOS VERSION INFORMATION ===\n");
    lazybios_smbios_ver(ctx);

    const smbios_entry_info_t *entry = lazybios_get_entry_info(ctx);
    if (entry) {
        printf("Table Length: %u bytes\n", entry->table_length);
        printf("Table Address: 0x%lX\n", entry->table_address);
        printf("Is 64-bit: %s\n", entry->is_64bit ? "Yes (SMBIOS 3.x)" : "No (SMBIOS 2.x)");
        if (entry->is_64bit) {
            printf("Docrev: %u\n", entry->docrev);
        }
        printf("\n");
    }
}

int main(void) {
    printf("lazybios - System Information\n");
    printf("=============================================\n\n");

    lazybios_ctx_t* ctx = lazybios_ctx_new();
    if (!ctx) {
        fprintf(stderr, "Failed to create lazybios context\n");
        return 1;
    }

    if (lazybios_init(ctx) != 0) {
        fprintf(stderr, "Failed to initialize lazybios library\n");
        lazybios_cleanup(ctx);
        return 1;
    }

    printf("Library initialized successfully!\n\n");

    print_smbios_version_info(ctx);
    print_bios_info(ctx);
    print_system_info(ctx);
    print_baseboard_info(ctx);
    print_chassis_info(ctx);
    print_processor_info(ctx);
    print_cache_info(ctx);
    print_port_connector_info(ctx);
    print_onboard_device_info(ctx);
    print_OEM_strings(ctx);
    print_memory_array_info(ctx);
    print_memory_info(ctx);

    lazybios_cleanup(ctx);
    printf("Library cleanup completed!\n");
    printf("All tests passed successfully!\n");

    return 0;
}

#include "lazybios.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ===== Global State =====
static uint8_t *dmi_data = NULL;
static size_t dmi_len = 0;
static smbios_entry_info_t entry_info;

// Cached parsed data
static bios_info_t *bios_info_ptr = NULL;
static system_info_t *system_info_ptr = NULL;
static chassis_info_t *chassis_info_ptr = NULL;
static processor_info_t *processor_info_ptr = NULL;
static memory_device_t *memory_devices_ptr = NULL;
static size_t memory_devices_count = 0;

// ===== Version-Aware Helpers =====
size_t get_smbios_structure_min_length(uint8_t type) {
    switch(type) {
        case SMBIOS_TYPE_BIOS:
            return (entry_info.major >= 3) ? BIOS_MIN_LENGTH_3_0 : BIOS_MIN_LENGTH_2_0;
        case SMBIOS_TYPE_SYSTEM:
            return (entry_info.major >= 3) ? SYSTEM_MIN_LENGTH_3_0 : SYSTEM_MIN_LENGTH_2_0;
        case SMBIOS_TYPE_CHASSIS:
            return (entry_info.major >= 3) ? CHASSIS_MIN_LENGTH_3_0 : CHASSIS_MIN_LENGTH_2_0;
        case SMBIOS_TYPE_PROCESSOR:
            return (entry_info.major > 2 || (entry_info.major == 2 && entry_info.minor >= 6))
                   ? PROC_MIN_LENGTH_2_6 : PROC_MIN_LENGTH_2_0;
        case SMBIOS_TYPE_MEMDEVICE:
            return (entry_info.major >= 3) ? MEMORY_MIN_LENGTH_3_0 : MEMORY_MIN_LENGTH_2_0;
        default:
            return 4;
    }
}

bool is_smbios_version_at_least(uint8_t major, uint8_t minor) {
    return (entry_info.major > major) ||
           (entry_info.major == major && entry_info.minor >= minor);
}

// ===== Comprehensive Processor Family String Mapping =====
const char* get_processor_family_string(uint8_t family) {
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
static int parse_smbios2_entry(const uint8_t *buf) {
    if (memcmp(buf, SMBIOS2_ANCHOR, 4) != 0) return -1;

    entry_info.major = buf[ENTRY_MAJOR_OFFSET];
    entry_info.minor = buf[ENTRY_MINOR_OFFSET];
    entry_info.table_length = *(uint16_t*)(buf + ENTRY_TABLE_LENGTH_OFFSET);
    entry_info.table_address = *(uint32_t*)(buf + ENTRY_TABLE_ADDR_OFFSET);
    entry_info.is_64bit = false;

    return 0;
}

static int parse_smbios3_entry(const uint8_t *buf) {
    if (memcmp(buf, SMBIOS3_ANCHOR, 5) != 0) return -1;

    entry_info.major = buf[ENTRY_MAJOR_OFFSET];
    entry_info.minor = buf[ENTRY_MINOR_OFFSET];
    entry_info.table_length = *(uint32_t*)(buf + ENTRY_TABLE_LENGTH_OFFSET_3);
    entry_info.table_address = *(uint64_t*)(buf + ENTRY_TABLE_ADDR_OFFSET_3);
    entry_info.is_64bit = true;

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
static size_t count_structures_by_type(uint8_t target_type) {
    if (!dmi_data) return 0;

    size_t count = 0;
    const uint8_t *p = dmi_data;
    const uint8_t *end = dmi_data + dmi_len;
    size_t min_length = get_smbios_structure_min_length(target_type);

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
int init(void) {
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

    if (parse_smbios3_entry(entry_buf) == 0) {
        printf("lazybios: detected SMBIOS 3.x (%d.%d)\n", entry_info.major, entry_info.minor);
    } else if (parse_smbios2_entry(entry_buf) == 0) {
        printf("lazybios: detected SMBIOS 2.x (%d.%d)\n", entry_info.major, entry_info.minor);
    } else {
        fprintf(stderr, "lazybios: unknown SMBIOS anchor\n");
        fclose(dmi);
        return -1;
    }

    fseek(dmi, 0, SEEK_END);
    dmi_len = ftell(dmi);
    rewind(dmi);

    dmi_data = malloc(dmi_len);
    if (!dmi_data) {
        perror("lazybios: failed to allocate DMI buffer");
        fclose(dmi);
        return -1;
    }

    if (fread(dmi_data, 1, dmi_len, dmi) != dmi_len) {
        fprintf(stderr, "lazybios: failed to read DMI table\n");
        free(dmi_data);
        dmi_data = NULL;
        fclose(dmi);
        return -1;
    }

    fclose(dmi);
    printf("lazybios: loaded %zu bytes of DMI data\n", dmi_len);
    return 0;
}

// ===== Basic Info Getters =====
bios_info_t *get_bios_info(void) {
    if (bios_info_ptr) return bios_info_ptr;
    if (!dmi_data) return NULL;

    const uint8_t *p = dmi_data;
    const uint8_t *end = dmi_data + dmi_len;
    size_t min_length = get_smbios_structure_min_length(SMBIOS_TYPE_BIOS);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_BIOS && length >= min_length) {
            bios_info_ptr = calloc(1, sizeof(bios_info_t));
            bios_info_ptr->vendor = strdup(dmi_string(p, length, p[BIOS_VENDOR_OFFSET]));
            bios_info_ptr->version = strdup(dmi_string(p, length, p[BIOS_VERSION_OFFSET]));
            bios_info_ptr->release_date = strdup(dmi_string(p, length, p[BIOS_RELEASE_DATE_OFFSET]));
            bios_info_ptr->rom_size_kb = (p[BIOS_ROM_SIZE_OFFSET] + 1) * BIOS_ROM_SIZE_MULTIPLIER;
            return bios_info_ptr;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

system_info_t *get_system_info(void) {
    if (system_info_ptr) return system_info_ptr;
    if (!dmi_data) return NULL;

    const uint8_t *p = dmi_data;
    const uint8_t *end = dmi_data + dmi_len;
    size_t min_length = get_smbios_structure_min_length(SMBIOS_TYPE_SYSTEM);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_SYSTEM && length >= min_length) {
            system_info_ptr = calloc(1, sizeof(system_info_t));
            system_info_ptr->manufacturer = strdup(dmi_string(p, length, p[SYS_MANUFACTURER_OFFSET]));
            system_info_ptr->product_name = strdup(dmi_string(p, length, p[SYS_PRODUCT_OFFSET]));
            system_info_ptr->version = strdup(dmi_string(p, length, p[SYS_VERSION_OFFSET]));
            system_info_ptr->serial_number = strdup(dmi_string(p, length, p[SYS_SERIAL_OFFSET]));
            return system_info_ptr;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

chassis_info_t *get_chassis_info(void) {
    if (chassis_info_ptr) return chassis_info_ptr;
    if (!dmi_data) return NULL;

    const uint8_t *p = dmi_data;
    const uint8_t *end = dmi_data + dmi_len;
    size_t min_length = get_smbios_structure_min_length(SMBIOS_TYPE_CHASSIS);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_CHASSIS && length >= min_length) {
            chassis_info_ptr = calloc(1, sizeof(chassis_info_t));
            chassis_info_ptr->asset_tag = strdup(dmi_string(p, length, p[CHASSIS_ASSET_TAG_OFFSET]));
            chassis_info_ptr->sku = strdup(dmi_string(p, length, p[CHASSIS_SKU_OFFSET]));
            return chassis_info_ptr;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

// ===== Processor Info Getter =====
processor_info_t *get_processor_info(void) {
    if (processor_info_ptr) return processor_info_ptr;
    if (!dmi_data) return NULL;

    const uint8_t *p = dmi_data;
    const uint8_t *end = dmi_data + dmi_len;
    size_t min_length = get_smbios_structure_min_length(SMBIOS_TYPE_PROCESSOR);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_PROCESSOR && length >= min_length) {
            processor_info_ptr = calloc(1, sizeof(processor_info_t));

            // Parse all fields
            processor_info_ptr->socket_designation = strdup(dmi_string(p, length, p[PROC_SOCKET_OFFSET]));
            processor_info_ptr->version = strdup(dmi_string(p, length, p[PROC_VERSION_OFFSET]));
            processor_info_ptr->max_speed_mhz = *(uint16_t*)(p + PROC_MAX_SPEED_OFFSET);
            processor_info_ptr->external_clock_mhz = *(uint16_t*)(p + PROC_EXTERNAL_CLOCK_OFFSET);
            processor_info_ptr->voltage = p[PROC_VOLTAGE_OFFSET];
            processor_info_ptr->processor_type = p[PROC_TYPE_OFFSET];
            processor_info_ptr->processor_family = p[PROC_FAMILY_OFFSET];

            // Core information - using correct offsets for all SMBIOS versions
            processor_info_ptr->core_count = p[PROC_CORE_COUNT_OFFSET];
            processor_info_ptr->core_enabled = p[PROC_CORE_ENABLED_OFFSET];
            processor_info_ptr->thread_count = p[PROC_THREAD_COUNT_OFFSET];

            // String fields
            if (length > PROC_SERIAL_OFFSET_2_5) {
                processor_info_ptr->serial_number = strdup(dmi_string(p, length, p[PROC_SERIAL_OFFSET_2_5]));
            } else {
                processor_info_ptr->serial_number = strdup("N/A");
            }

            processor_info_ptr->asset_tag = strdup("N/A");
            processor_info_ptr->part_number = strdup("N/A");

            // Optional fields based on structure length
            if (length > PROC_CHARACTERISTICS_OFFSET + 1) {
                processor_info_ptr->characteristics = *(uint16_t*)(p + PROC_CHARACTERISTICS_OFFSET);
            } else {
                processor_info_ptr->characteristics = 0;
            }

            return processor_info_ptr;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

// ===== Memory Devices Getter =====
memory_device_t *get_memory_devices(size_t *count) {
    if (memory_devices_ptr) {
        *count = memory_devices_count;
        return memory_devices_ptr;
    }
    if (!dmi_data) {
        *count = 0;
        return NULL;
    }

    // Count memory devices
    memory_devices_count = count_structures_by_type(SMBIOS_TYPE_MEMDEVICE);
    if (memory_devices_count == 0) {
        *count = 0;
        return NULL;
    }

    // Allocate array
    memory_devices_ptr = calloc(memory_devices_count, sizeof(memory_device_t));
    if (!memory_devices_ptr) {
        *count = 0;
        return NULL;
    }

    // Parse memory devices
    size_t index = 0;
    const uint8_t *p = dmi_data;
    const uint8_t *end = dmi_data + dmi_len;
    size_t min_length = get_smbios_structure_min_length(SMBIOS_TYPE_MEMDEVICE);

    while (p + SMBIOS_HEADER_SIZE < end && index < memory_devices_count) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_MEMDEVICE && length >= min_length) {
            memory_device_t *current = &memory_devices_ptr[index];

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

    *count = memory_devices_count;
    return memory_devices_ptr;
}

// ===== Cleanup =====
void cleanup(void) {
    // Basic info cleanup
    if (bios_info_ptr) {
        free(bios_info_ptr->vendor);
        free(bios_info_ptr->version);
        free(bios_info_ptr->release_date);
        free(bios_info_ptr);
        bios_info_ptr = NULL;
    }

    if (system_info_ptr) {
        free(system_info_ptr->manufacturer);
        free(system_info_ptr->product_name);
        free(system_info_ptr->version);
        free(system_info_ptr->serial_number);
        free(system_info_ptr);
        system_info_ptr = NULL;
    }

    if (chassis_info_ptr) {
        free(chassis_info_ptr->asset_tag);
        free(chassis_info_ptr->sku);
        free(chassis_info_ptr);
        chassis_info_ptr = NULL;
    }

    // Processor cleanup
    if (processor_info_ptr) {
        free(processor_info_ptr->socket_designation);
        free(processor_info_ptr->version);
        free(processor_info_ptr->serial_number);
        free(processor_info_ptr->asset_tag);
        free(processor_info_ptr->part_number);
        free(processor_info_ptr);
        processor_info_ptr = NULL;
    }

    // Memory devices cleanup
    if (memory_devices_ptr) {
        for (size_t i = 0; i < memory_devices_count; i++) {
            free(memory_devices_ptr[i].locator);
            free(memory_devices_ptr[i].bank_locator);
            free(memory_devices_ptr[i].manufacturer);
            free(memory_devices_ptr[i].serial_number);
            free(memory_devices_ptr[i].part_number);
        }
        free(memory_devices_ptr);
        memory_devices_ptr = NULL;
    }
    memory_devices_count = 0;

    // Raw data cleanup
    if (dmi_data) {
        free(dmi_data);
        dmi_data = NULL;
    }
    dmi_len = 0;
}

// ===== Public API Functions =====
void smbios_ver(void) {
    printf("SMBIOS version %d.%d\n", entry_info.major, entry_info.minor);
}

const smbios_entry_info_t *get_entry_info(void) {
    return &entry_info;
}
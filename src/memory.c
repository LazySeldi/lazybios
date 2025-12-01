//
// SMBIOS Types 16-20
//
#include "lazybios.h"
#include <string.h>
#include <stdlib.h>


// Type 16 ( Physical Memory Array )
physical_memory_array_t* lazybiosGetPhysicalMemArrayInfo(lazybios_ctx_t* ctx, size_t* count) {
    if (!ctx) {
        *count = 0;
        return NULL;
    }

    if (ctx->memory_arrays_ptr) {
        *count = ctx->memory_arrays_count;
        return ctx->memory_arrays_ptr;
    }

    if (!ctx->dmi_data) {
        *count = 0;
        return NULL;
    }

    ctx->memory_arrays_count = lazybiosCountStructsByType(ctx, SMBIOS_TYPE_MEMARRAY);
    if (ctx->memory_arrays_count == 0) {
        *count = 0;
        return NULL;
    }

    ctx->memory_arrays_ptr = calloc(ctx->memory_arrays_count, sizeof(physical_memory_array_t));
    if (!ctx->memory_arrays_ptr) {
        *count = 0;
        return NULL;
    }

    size_t index = 0;
    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybiosGetStructMinLength(ctx, SMBIOS_TYPE_MEMARRAY);

    while (p + SMBIOS_HEADER_SIZE < end && index < ctx->memory_arrays_count) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_MEMARRAY && length >= min_length) {
            physical_memory_array_t *current = &ctx->memory_arrays_ptr[index];

            current->location = p[MEMARRAY_LOCATION_OFFSET];
            current->use = p[MEMARRAY_USE_OFFSET];
            current->ecc_type = p[MEMARRAY_ECC_OFFSET];

            uint32_t max_cap = *(uint32_t*)(p + MEMARRAY_MAX_CAPACITY_OFFSET);

            if (max_cap == 0x80000000 && length >= 0x17) {
                uint64_t ext_cap = *(uint64_t*)(p + MEMARRAY_EXT_MAX_CAPACITY_OFFSET);
                current->max_capacity_kb = ext_cap;
                current->extended_capacity = true;
            } else {
                current->max_capacity_kb = max_cap;
                current->extended_capacity = false;
            }

            current->num_devices = *(uint16_t*)(p + MEMARRAY_NUM_DEVICES_OFFSET);

            index++;
        }
        p = DMINext(p, end);
    }

    *count = ctx->memory_arrays_count;
    return ctx->memory_arrays_ptr;
}

// Type 17 ( Memory Devices )
memory_device_t* lazybiosGetMemDeviceInfo(lazybios_ctx_t* ctx, size_t* count) {
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

    ctx->memory_devices_count = lazybiosCountStructsByType(ctx, SMBIOS_TYPE_MEMDEVICE);
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
    size_t min_length = lazybiosGetStructMinLength(ctx, SMBIOS_TYPE_MEMDEVICE);

    while (p + SMBIOS_HEADER_SIZE < end && index < ctx->memory_devices_count) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_MEMDEVICE && length >= min_length) {
            memory_device_t *current = &ctx->memory_devices_ptr[index];

            current->locator = strdup(DMIString(p, length, p[MEM_DEVICE_LOCATOR_OFFSET]));
            current->bank_locator = strdup(DMIString(p, length, p[MEM_DEVICE_BANK_LOCATOR_OFFSET]));
            current->manufacturer = strdup(DMIString(p, length, p[MEM_DEVICE_MANUFACTURER_OFFSET]));
            current->serial_number = strdup(DMIString(p, length, p[MEM_DEVICE_SERIAL_OFFSET]));
            current->part_number = strdup(DMIString(p, length, p[MEM_DEVICE_PART_NUMBER_OFFSET]));

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
        p = DMINext(p, end);
    }

    *count = ctx->memory_devices_count;
    return ctx->memory_devices_ptr;
}
/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file qnx.c
 * @brief Implements native SMBIOS loading on QNX Neutrino.
 */
#include "lazybios_internal.h"

#if defined(OS_QNX)

    #include <errno.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/mman.h>
    #include <unistd.h>

    #define SMBIOS_LEGACY_START UINT64_C(0xF0000)
    #define SMBIOS_LEGACY_SIZE 0x10000U

    #if defined(__i386__) || defined(__x86_64__)
        #define QNX_LEGACY_X86 1
    #else
        #define QNX_LEGACY_X86 0
    #endif

    #if QNX_LEGACY_X86
static int map_physical_at(uint64_t address, uint8_t *buffer, size_t length) {
    long page_size_signed;
    size_t page_size;
    size_t page_offset;
    size_t map_size;
    size_t remainder;
    uint64_t aligned;
    void *mapping;

    if (!buffer || length == 0) return -1;

    page_size_signed = sysconf(_SC_PAGESIZE);
    if (page_size_signed <= 0) {
        lb_dbg("Failed to query the QNX page size: %s", strerror(errno));
        return -1;
    }
    page_size = (size_t)page_size_signed;

    aligned = address - (address % page_size);
    page_offset = (size_t)(address - aligned);
    if (length > SIZE_MAX - page_offset) return -1;

    map_size = length + page_offset;
    remainder = map_size % page_size;
    if (remainder != 0) {
        size_t padding = page_size - remainder;

        if (map_size > SIZE_MAX - padding) return -1;
        map_size += padding;
    }

    /*
     * mmap_device_memory() is the interface QNX documents for physical memory,
     * and it wraps mmap() with MAP_PHYS and NOFD, which is why no descriptor is
     * involved here. It requires the PROCMGR_AID_MEM_PHYS ability, so a caller
     * without it fails at this point and reaches the DEV_MEM fallback in
     * lazybiosQNX instead.
     */
    mapping = mmap_device_memory(NULL, map_size, PROT_READ, 0, aligned);
    if (mapping == MAP_FAILED) {
        lb_dbg("Failed to map physical memory at 0x%llx on QNX: %s", (unsigned long long)aligned, strerror(errno));
        return -1;
    }

    memcpy(buffer, (const uint8_t *)mapping + page_offset, length);
    munmap_device_memory(mapping, map_size);
    return 0;
}

static int load_legacy_x86_scan(lazybiosCTX_t *ctx) {
    uint64_t table_address;
    size_t entry_offset;
    size_t entry_len;
    size_t table_len;
    uint8_t *window;
    uint8_t *table;
    int result = -1;

    window = malloc(SMBIOS_LEGACY_SIZE);
    if (!window) {
        lb_dbg("Failed to allocate %u bytes for the QNX legacy SMBIOS window", SMBIOS_LEGACY_SIZE);
        return -1;
    }

    if (map_physical_at(SMBIOS_LEGACY_START, window, SMBIOS_LEGACY_SIZE) != 0 ||
        lazybiosFindSMBIOSEntryPoint(window, SMBIOS_LEGACY_SIZE, &entry_offset, &entry_len) != 0) {
        free(window);
        return -1;
    }

    if (lazybiosGetSMBIOSTableLocation(window + entry_offset, entry_len, &entry_len, &table_address, &table_len) != 0) {
        lb_dbg("The SMBIOS entry point mapped on QNX has an invalid table location");
        free(window);
        return -1;
    }

    table = malloc(table_len);
    if (!table) {
        lb_dbg("Failed to allocate %zu bytes for the QNX SMBIOS table", table_len);
        free(window);
        return -1;
    }

    if (map_physical_at(table_address, table, table_len) == 0)
        result = lazybiosLoadRawBuffers(ctx, window + entry_offset, entry_len, table, table_len);

    free(table);
    free(window);
    return result;
}
    #endif

int lazybiosQNX(lazybiosCTX_t *ctx) {
    static const char *const device_paths[] = {DEV_MEM};

    if (!ctx || !ctx->DMIData) return -1;

    #if QNX_LEGACY_X86
    if (load_legacy_x86_scan(ctx) == 0) return 0;

    lb_dbg("Falling back from mmap_device_memory() to " DEV_MEM);
    #endif

    return lazybiosLoadLegacyPhysicalMemory(ctx, device_paths, sizeof(device_paths) / sizeof(device_paths[0]), "QNX");
}

#endif

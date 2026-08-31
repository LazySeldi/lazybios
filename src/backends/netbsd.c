/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file netbsd.c
 * @brief Implements native SMBIOS loading on NetBSD.
 */
#include "lazybios_internal.h"

#if defined(OS_NETBSD)

    #include <errno.h>
    #include <fcntl.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/param.h>
    #include <sys/sysctl.h>
    #include <sys/types.h>
    #include <unistd.h>

    #define NETBSD_DEV_SMBIOS "/dev/smbios"
    #define NETBSD_SMBIOS_SYSCTL "machdep.smbios"
    #define SMBIOS_ENTRY_READ_SIZE 0x20
    #define SMBIOS_LEGACY_START 0xF0000
    #define SMBIOS_LEGACY_SIZE 0x10000
    #define DEVICE_READ_CHUNK_SIZE (1024U * 1024U)

static int uint64_to_off_t(uint64_t value, off_t *result) {
    off_t converted;

    if (!result) return -1;

    converted = (off_t)value;
    if (converted < 0 || (uint64_t)converted != value) return -1;

    *result = converted;
    return 0;
}

static int netbsd_smbios_address(uint64_t *address) {
    uint64_t value = 0;
    size_t value_len = sizeof(value);

    if (!address) return -1;

    if (sysctlbyname(NETBSD_SMBIOS_SYSCTL, &value, &value_len, NULL, 0) == -1) {
        lb_dbg("Failed to read " NETBSD_SMBIOS_SYSCTL ": %s", strerror(errno));
        return -1;
    }
    if (value_len != sizeof(value) || value == 0) {
        lb_dbg(NETBSD_SMBIOS_SYSCTL " returned an invalid SMBIOS entry-point address");
        return -1;
    }

    *address = value;
    return 0;
}

static int read_device_at(int fd, const char *path, uint64_t address, uint8_t *buffer, size_t length) {
    off_t offset;
    size_t total = 0;

    if (!path || !buffer || length == 0 || uint64_to_off_t(address, &offset) != 0) {
        lb_dbg("Invalid read request for %s at 0x%llx", path ? path : "device", (unsigned long long)address);
        return -1;
    }

    if (lseek(fd, offset, SEEK_SET) != offset) {
        lb_dbg("Failed to seek %s to 0x%llx: %s", path, (unsigned long long)address, strerror(errno));
        return -1;
    }

    while (total < length) {
        size_t remaining = length - total;
        size_t chunk = remaining < DEVICE_READ_CHUNK_SIZE ? remaining : DEVICE_READ_CHUNK_SIZE;
        ssize_t count = read(fd, buffer + total, chunk);

        if (count < 0) {
            if (errno == EINTR) continue;
            lb_dbg("Failed to read %s at 0x%llx: %s", path, (unsigned long long)(address + total), strerror(errno));
            return -1;
        }
        if (count == 0) {
            lb_dbg("Short read from %s at 0x%llx: expected %zu more bytes", path, (unsigned long long)(address + total), remaining);
            return -1;
        }

        total += (size_t)count;
    }

    return 0;
}

static int load_entry_and_table(lazybiosCTX_t *ctx, int fd, const char *path, const uint8_t *entry, size_t available) {
    uint64_t table_address;
    size_t entry_len;
    size_t table_len;
    uint8_t *table;
    int result;

    if (lazybiosGetSMBIOSTableLocation(entry, available, &entry_len, &table_address, &table_len) != 0) {
        lb_dbg("Invalid SMBIOS entry point read from %s", path);
        return -1;
    }

    table = malloc(table_len);
    if (!table) {
        lb_dbg("Failed to allocate %zu bytes for the NetBSD SMBIOS table", table_len);
        return -1;
    }

    if (read_device_at(fd, path, table_address, table, table_len) != 0) {
        free(table);
        return -1;
    }

    result = lazybiosLoadRawBuffers(ctx, entry, entry_len, table, table_len);
    free(table);
    return result;
}

static int load_from_entry_address(lazybiosCTX_t *ctx, const char *path, uint64_t entry_address) {
    uint8_t entry[SMBIOS_ENTRY_READ_SIZE];
    int fd;
    int result;

    fd = open(path, O_RDONLY);
    if (fd == -1) {
        lb_dbg("Failed to open %s: %s", path, strerror(errno));
        return -1;
    }

    if (read_device_at(fd, path, entry_address, entry, sizeof(entry)) != 0) {
        close(fd);
        return -1;
    }

    result = load_entry_and_table(ctx, fd, path, entry, sizeof(entry));
    close(fd);
    return result;
}

    #if defined(__i386__) || defined(__x86_64__)
static int load_legacy_x86_scan(lazybiosCTX_t *ctx) {
    uint8_t *window;
    size_t entry_offset;
    size_t entry_len;
    int fd;
    int result;

    fd = open(DEV_MEM, O_RDONLY);
    if (fd == -1) {
        lb_dbg("Failed to open " DEV_MEM " for the NetBSD legacy SMBIOS scan: %s", strerror(errno));
        return -1;
    }

    window = malloc(SMBIOS_LEGACY_SIZE);
    if (!window) {
        close(fd);
        return -1;
    }

    if (read_device_at(fd, DEV_MEM, SMBIOS_LEGACY_START, window, SMBIOS_LEGACY_SIZE) != 0 ||
        lazybiosFindSMBIOSEntryPoint(window, SMBIOS_LEGACY_SIZE, &entry_offset, &entry_len) != 0) {
        free(window);
        close(fd);
        return -1;
    }

    result = load_entry_and_table(ctx, fd, DEV_MEM, window + entry_offset, entry_len);
    free(window);
    close(fd);
    return result;
}
    #endif

int lazybiosNetBSD(lazybiosCTX_t *ctx) {
    uint64_t entry_address;

    if (!ctx || !ctx->DMIData) return -1;

    if (netbsd_smbios_address(&entry_address) == 0) {
        if (load_from_entry_address(ctx, NETBSD_DEV_SMBIOS, entry_address) == 0) return 0;

        lb_dbg("Falling back from " NETBSD_DEV_SMBIOS " to " DEV_MEM);
        if (load_from_entry_address(ctx, DEV_MEM, entry_address) == 0) return 0;
    }

    #if defined(__i386__) || defined(__x86_64__)
    lb_dbg("Falling back to the NetBSD legacy x86 SMBIOS scan");
    if (load_legacy_x86_scan(ctx) == 0) return 0;
    #endif

    lb_log("Failed to load SMBIOS data on NetBSD; check " NETBSD_SMBIOS_SYSCTL " and read permissions for " NETBSD_DEV_SMBIOS " or " DEV_MEM);
    return -1;
}

#endif

/*
* SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This file is part of lazybios.
 *
 * lazybios is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * lazybios is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lazybios. If not, see <https://www.gnu.org/licenses/>.
 */
/**
 * @file freebsd.c
 * @brief Implements native SMBIOS loading on FreeBSD.
 */
#include "lazybios_internal.h"

#if defined(OS_FREEBSD)

#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <kenv.h>

#define SMBIOS_START 0xF0000
#define SMBIOS_SIZE 0x10000

static inline int uint64_to_off_t(uint64_t value, off_t *out) {
	uint64_t off_t_max = ((uint64_t)1 << (sizeof(off_t) * CHAR_BIT - 1)) - 1;
	if (value > off_t_max) return -1;
	*out = (off_t)value;
	return 0;
}

static inline uint64_t FreeBSDAddressGetter(void) {
    char value[KENV_MVALLEN + 1];
    int result = kenv(KENV_GET, "hint.smbios.0.mem", value, sizeof(value));

    if (result == -1) return 0;

    char *endptr;
    errno = 0;
    unsigned long long addr = strtoull(value, &endptr, 0);

    if (endptr == value || errno == ERANGE) {  // Parsing either failed or the value was out of range
        return 0;
    }

    return addr;
}

static inline int lazybiosDevMem(lazybiosCTX_t *ctx, uint64_t addr) {
	if (!ctx) return -1;

	int fd = open(DEV_MEM, O_RDONLY);
	if (fd == -1) {
		lb_log("Failed to open /dev/mem, consider checking if you have the permissions to access /dev/mem!");
		lb_dbg("Error: %s", strerror(errno));
		return -1;
	}

	off_t base_addr;
    if (addr != 0) { // If we found a valid address, then we use that, if not then we use the old one
        base_addr = addr;
    } else {
        base_addr = SMBIOS_START;
    }

	long page_size = sysconf(_SC_PAGESIZE);
	if (page_size <= 0) {
		lb_log("Failed to read system page size");
		close(fd);
		return -1;
	}

	off_t aligned_addr = (base_addr / page_size) * page_size;
	off_t page_offset = base_addr - aligned_addr;
	size_t page_size_unsigned = (size_t)page_size;
	size_t page_offset_unsigned = (size_t)page_offset;

	// Round map_size up to the nearest page to be safe
	size_t map_size = SMBIOS_SIZE + page_offset_unsigned;
	map_size = (map_size + page_size_unsigned - 1) & ~(page_size_unsigned - 1);

	void *mapped_data = mmap(NULL, map_size, PROT_READ, MAP_SHARED, fd, aligned_addr);
	if (mapped_data == MAP_FAILED) {
		lb_log("Failed to mmap /dev/mem data");
		lb_dbg("Error: %s", strerror(errno));
		close(fd);
		return -1;
	}

	unsigned char *smbios_data = (unsigned char *)mapped_data + page_offset;

	size_t sig_offset = 0;
	size_t entry_len = 0;
	if (lazybiosFindSMBIOSEntryPoint(
			smbios_data, SMBIOS_SIZE, &sig_offset, &entry_len) != 0) {
		lb_log("SMBIOS/DMI signature not found in /dev/mem");
		munmap(mapped_data, map_size);
		close(fd);
		return -1;
	}

	unsigned char *entry = smbios_data + sig_offset;

	ctx->DMIData->entry_len = entry_len;
	ctx->DMIData->entry_data = malloc(entry_len);
	if (!ctx->DMIData->entry_data) {
		lb_log("Failed to allocate entry_data");
		munmap(mapped_data, map_size);
		close(fd);
		return -1;
	}
	memcpy(ctx->DMIData->entry_data, entry, entry_len);

	if (lazybiosParseEntry(ctx, ctx->DMIData->entry_data, ctx->DMIData->entry_len) != 0) {
		lb_log("Failed to parse SMBIOS entry point");
		free(ctx->DMIData->entry_data);
		ctx->DMIData->entry_data = NULL;
		munmap(mapped_data, map_size);
		close(fd);
		return -1;
	}

	uint64_t table_addr = 0;
	size_t table_len = 0;

	if (ctx->DMIData->entry_tag == SMBIOS_VER_3X) {
		table_addr = ctx->DMIData->entry_union.v3->structure_table_address;
		table_len = ctx->DMIData->entry_union.v3->structure_table_max_size;
	} else {
		table_addr = ctx->DMIData->entry_union.v2->structure_table_address;
		table_len = ctx->DMIData->entry_union.v2->structure_table_length;
	}

	munmap(mapped_data, map_size);

	// From this point on, this function was not made all by me
	off_t table_addr_off = 0;
	if (uint64_to_off_t(table_addr, &table_addr_off) != 0) {
		lb_log("SMBIOS table address does not fit in off_t: 0x%016lx", (unsigned long)table_addr);
		free(ctx->DMIData->entry_data);
		ctx->DMIData->entry_data = NULL;
		close(fd);
		return -1;
	}

	off_t table_aligned = (table_addr_off / page_size) * page_size;
	off_t table_offset_page = table_addr_off - table_aligned;
	size_t table_offset_unsigned = (size_t)table_offset_page;

	// We'll calculate map size by rounding up to the nearest page
	if (table_len > SIZE_MAX - table_offset_unsigned) {
		lb_log("SMBIOS table mapping size overflow");
		free(ctx->DMIData->entry_data);
		ctx->DMIData->entry_data = NULL;
		close(fd);
		return -1;
	}
	size_t table_map_size = table_len + table_offset_unsigned;
	table_map_size = (table_map_size + page_size_unsigned - 1) & ~(page_size_unsigned - 1);

	void *mapped_table = mmap(NULL, table_map_size, PROT_READ, MAP_SHARED, fd, table_aligned);
	if (mapped_table == MAP_FAILED) {
		lb_log("Failed to mmap DMI/SMBIOS table at 0x%016lx", (unsigned long)table_addr);
		lb_dbg("Error: %s", strerror(errno));
		free(ctx->DMIData->entry_data);
		ctx->DMIData->entry_data = NULL;
		close(fd);
		return -1;
	}

	unsigned char *table_data = (unsigned char *)mapped_table + table_offset_page;

	// Store table data in context
	ctx->DMIData->dmi_len = table_len;
	ctx->DMIData->dmi_data = malloc(table_len);
	if (!ctx->DMIData->dmi_data) {
		lb_log("Failed to allocate DMI buffer (%zu bytes)", table_len);
		munmap(mapped_table, table_map_size);
		free(ctx->DMIData->entry_data);
		ctx->DMIData->entry_data = NULL;
		close(fd);
		return -1;
	}
	memcpy(ctx->DMIData->dmi_data, table_data, table_len);

	munmap(mapped_table, table_map_size);
	close(fd);

	return 0;
}

int lazybiosFreeBSD(lazybiosCTX_t *ctx) {
    if (!ctx) return -1;

	return lazybiosDevMem(ctx, FreeBSDAddressGetter());
}

#endif

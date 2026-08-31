/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file backend.c
 * @brief Shared validation, conversion, and physical-memory backend helpers.
 */
#if (defined(OS_HAIKU) || defined(OS_BEOS) || defined(OS_GENERIC) || \
	defined(__HAIKU__) || defined(__BEOS__) || defined(__BeOS) || defined(_BEOS)) && \
	!defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif

#include "lazybios_internal.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#if defined(OS_HAIKU) || defined(OS_BEOS) || defined(OS_GENERIC) || \
	defined(OS_QNX) || defined(OS_MINIX)
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#define WINDOWS_RAW_HEADER_SIZE 8
#define WINDOWS_RAW_LENGTH_OFFSET 4

static uint16_t read_u16_le(const uint8_t data[2]) {
	return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

static uint32_t read_u32_le(const uint8_t data[4]) {
	return (uint32_t)data[0] |
		((uint32_t)data[1] << 8) |
		((uint32_t)data[2] << 16) |
		((uint32_t)data[3] << 24);
}

static uint64_t read_u64_le(const uint8_t data[8]) {
	return (uint64_t)data[0] |
		((uint64_t)data[1] << 8) |
		((uint64_t)data[2] << 16) |
		((uint64_t)data[3] << 24) |
		((uint64_t)data[4] << 32) |
		((uint64_t)data[5] << 40) |
		((uint64_t)data[6] << 48) |
		((uint64_t)data[7] << 56);
}

static void write_u16_le(uint8_t data[2], uint16_t value) {
	data[0] = (uint8_t)value;
	data[1] = (uint8_t)(value >> 8);
}

static void write_u32_le(uint8_t data[4], uint32_t value) {
	data[0] = (uint8_t)value;
	data[1] = (uint8_t)(value >> 8);
	data[2] = (uint8_t)(value >> 16);
	data[3] = (uint8_t)(value >> 24);
}

static void set_checksum(uint8_t* data, size_t begin, size_t end, size_t checksum_offset) {
	uint8_t sum = 0;
	data[checksum_offset] = 0;
	for (size_t i = begin; i < end; i++)
		sum = (uint8_t)(sum + data[i]);
	data[checksum_offset] = (uint8_t)(-sum);
}

int lazybiosLoadRawBuffers(lazybiosCTX_t* ctx,
	const uint8_t* entry_data, size_t entry_len,
	const uint8_t* dmi_data, size_t dmi_len) {
	if (!ctx || !ctx->DMIData || !entry_data || entry_len == 0 ||
		!dmi_data || dmi_len == 0)
		return -1;

	/*
	 * Backend loaders operate on a fresh context. Reject reuse rather than
	 * leaking an older table or leaving already-parsed structures stale.
	 */
	if (ctx->DMIData->entry_data || ctx->DMIData->dmi_data)
		return -1;

	uint8_t* entry_copy = malloc(entry_len);
	uint8_t* dmi_copy = malloc(dmi_len);
	if (!entry_copy || !dmi_copy) {
		free(entry_copy);
		free(dmi_copy);
		return -1;
	}
	memcpy(entry_copy, entry_data, entry_len);
	memcpy(dmi_copy, dmi_data, dmi_len);

	lazybiosDMI_t parsed = {0};
	lazybiosCTX_t temporary = {0};
	parsed.entry_data = entry_copy;
	parsed.entry_len = entry_len;
	temporary.DMIData = &parsed;

	if (lazybiosParseEntry(&temporary, entry_copy, entry_len) != 0) {
		free(entry_copy);
		free(dmi_copy);
		return -1;
	}

	parsed.dmi_data = dmi_copy;
	parsed.dmi_len = dmi_len;
	*ctx->DMIData = parsed;
	return 0;
}

int lazybiosLoadWindowsRawSMBIOSData(lazybiosCTX_t* ctx, const uint8_t* raw_data, size_t raw_len) {
	if (!ctx || !raw_data || raw_len < WINDOWS_RAW_HEADER_SIZE)
		return -1;

	const uint8_t major = raw_data[1];
	const uint8_t minor = raw_data[2];
	const uint8_t docrev = raw_data[3];
	const uint32_t table_len_u32 =
		read_u32_le(raw_data + WINDOWS_RAW_LENGTH_OFFSET);

	const size_t table_len = table_len_u32;
	if (table_len == 0 || table_len > raw_len - WINDOWS_RAW_HEADER_SIZE)
		return -1;
	if (major < 3 && table_len > UINT16_MAX)
		return -1;

	uint8_t entry[SMBIOS2_ENTRY_POINT_LENGTH] = {0};
	size_t entry_len;

	if (major >= 3) {
		entry_len = SMBIOS3_ENTRY_POINT_LENGTH;
		memcpy(entry, SMBIOS3_ANCHOR, SMBIOS3_ANCHOR_SIZE);
		entry[SMBIOS3_LENGTH_OFFSET] = SMBIOS3_ENTRY_POINT_LENGTH;
		entry[SMBIOS3_MAJOR_OFFSET] = major;
		entry[SMBIOS3_MINOR_OFFSET] = minor;
		entry[SMBIOS3_DOCREV_OFFSET] = docrev;
		entry[SMBIOS3_REVISION_OFFSET] = 1;

		write_u32_le(entry + SMBIOS3_TABLE_MAX_SIZE_OFFSET, table_len_u32);
		set_checksum(entry, 0, entry_len, SMBIOS3_CHECKSUM_OFFSET);
	} else {
		entry_len = SMBIOS2_ENTRY_POINT_LENGTH;
		memcpy(entry, SMBIOS2_ANCHOR, SMBIOS2_ANCHOR_SIZE);
		entry[SMBIOS2_LENGTH_OFFSET] = SMBIOS2_ENTRY_POINT_LENGTH;
		entry[SMBIOS2_MAJOR_OFFSET] = major;
		entry[SMBIOS2_MINOR_OFFSET] = minor;
		memcpy(entry + SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET,
			SMBIOS2_INTERMEDIATE_ANCHOR, SMBIOS2_INTERMEDIATE_ANCHOR_SIZE);

		const uint16_t legacy_table_len = (uint16_t)table_len;
		write_u16_le(entry + SMBIOS2_TABLE_LENGTH_OFFSET, legacy_table_len);
		entry[SMBIOS2_BCD_REVISION_OFFSET] =
			(uint8_t)((major << 4) | (minor & 0x0F));
		set_checksum(entry, SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET,
			entry_len, SMBIOS2_INTERMEDIATE_CHECKSUM_OFFSET);
		set_checksum(entry, 0, SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET,
			SMBIOS2_CHECKSUM_OFFSET);
	}

	return lazybiosLoadRawBuffers(ctx, entry, entry_len,
		raw_data + WINDOWS_RAW_HEADER_SIZE, table_len);
}

int lazybiosFindSMBIOSEntryPoint(const uint8_t* image, size_t image_len,
	size_t* entry_offset, size_t* entry_len) {
	if (!image || !entry_offset || !entry_len)
		return -1;

	size_t i = 0;
	while (image_len - i >= SMBIOS2_ANCHOR_SIZE) {
		lazybiosEntryInspection inspection;
		if (lazybiosInspectEntryPoint(
				image + i, image_len - i, &inspection) == 0 &&
			inspection.checksum_valid &&
			inspection.intermediate_checksum_valid) {
			*entry_offset = i;
			*entry_len = inspection.length;
			return 0;
		}

		if (image_len - i < SMBIOS2_ANCHOR_SIZE + 16)
			break;
		i += 16;
	}

	return -1;
}

int lazybiosGetSMBIOSTableLocation(const uint8_t* entry_data, size_t available,
	size_t* entry_len, uint64_t* table_address, size_t* table_len) {
	lazybiosEntryInspection inspection;

	if (!entry_data || !entry_len || !table_address || !table_len)
		return -1;
	if (lazybiosInspectEntryPoint(entry_data, available, &inspection) != 0 ||
		!inspection.checksum_valid ||
		!inspection.intermediate_checksum_valid)
		return -1;

	if (inspection.tag == SMBIOS_VER_3X) {
		*table_address =
			read_u64_le(entry_data + SMBIOS3_TABLE_ADDRESS_OFFSET);
		*table_len = (size_t)read_u32_le(
			entry_data + SMBIOS3_TABLE_MAX_SIZE_OFFSET);
	} else if (inspection.tag == SMBIOS_VER_2X) {
		*table_address =
			read_u32_le(entry_data + SMBIOS2_TABLE_ADDRESS_OFFSET);
		*table_len =
			read_u16_le(entry_data + SMBIOS2_TABLE_LENGTH_OFFSET);
	} else {
		return -1;
	}

	if (*table_address == 0 || *table_len == 0)
		return -1;

	*entry_len = inspection.length;
	return 0;
}

int lazybiosGetSingleFileLayout(const uint8_t* entry_data, size_t available,
	size_t file_len, size_t* entry_len, size_t* table_offset,
	size_t* table_len) {
	lazybiosEntryInspection inspection;
	uint64_t advertised_address;
	size_t advertised_len;

	if (!entry_data || !entry_len || !table_offset || !table_len)
		return -1;
	if (lazybiosInspectEntryPoint(entry_data, available, &inspection) != 0 ||
		inspection.length >= file_len)
		return -1;

	if (inspection.tag == SMBIOS_VER_3X) {
		advertised_address =
			read_u64_le(entry_data + SMBIOS3_TABLE_ADDRESS_OFFSET);
		advertised_len = (size_t)read_u32_le(
			entry_data + SMBIOS3_TABLE_MAX_SIZE_OFFSET);
	} else if (inspection.tag == SMBIOS_VER_2X) {
		advertised_address =
			read_u32_le(entry_data + SMBIOS2_TABLE_ADDRESS_OFFSET);
		advertised_len =
			read_u16_le(entry_data + SMBIOS2_TABLE_LENGTH_OFFSET);
	} else {
		return -1;
	}

	*entry_len = inspection.length;
	*table_offset = inspection.length;
	*table_len = file_len - inspection.length;

	if (advertised_address <= SIZE_MAX) {
		const size_t offset = (size_t)advertised_address;
		if (offset >= inspection.length && offset < file_len &&
			advertised_len != 0 && advertised_len <= file_len - offset) {
			*table_offset = offset;
			*table_len = advertised_len;
		}
	}

	return 0;
}

#if defined(OS_HAIKU) || defined(OS_BEOS) || defined(OS_GENERIC) || \
	defined(OS_QNX) || defined(OS_MINIX)

#define LEGACY_PHYSICAL_SMBIOS_START UINT64_C(0xF0000)
#define LEGACY_PHYSICAL_SMBIOS_SIZE 0x10000U

#if defined(__i386__) || defined(__x86_64__) || defined(__amd64__)
#define LAZYBIOS_LEGACY_X86 1
#else
#define LAZYBIOS_LEGACY_X86 0
#endif

#if LAZYBIOS_LEGACY_X86

static int legacy_uint64_to_off_t(uint64_t value, off_t* result) {
	off_t converted;

	if (!result) return -1;

	converted = (off_t)value;
	if (converted < 0 || (uint64_t)converted != value) return -1;

	*result = converted;
	return 0;
}

static size_t legacy_read_chunk_size(size_t remaining) {
	#ifdef SSIZE_MAX
	if (remaining > (size_t)SSIZE_MAX) return (size_t)SSIZE_MAX;
	#else
	if (remaining > (size_t)INT_MAX) return (size_t)INT_MAX;
	#endif
	return remaining;
}

static int legacy_read_full_with_seek(int fd, uint64_t address,
	uint8_t* buffer, size_t length) {
	off_t offset;
	size_t total = 0;

	if (legacy_uint64_to_off_t(address, &offset) != 0) return -1;
	if (lseek(fd, offset, SEEK_SET) == (off_t)-1) return -1;

	while (total < length) {
		size_t chunk = legacy_read_chunk_size(length - total);
		ssize_t count = read(fd, buffer + total, chunk);

		if (count < 0) {
			if (errno == EINTR) continue;
			return -1;
		}
		if (count == 0) return -1;
		total += (size_t)count;
	}

	return 0;
}

#if defined(_POSIX_VERSION) && _POSIX_VERSION >= 200112L
static int legacy_read_full_with_pread(int fd, uint64_t address,
	uint8_t* buffer, size_t length) {
	size_t total = 0;

	while (total < length) {
		off_t offset;
		size_t chunk;
		ssize_t count;

		if (total > UINT64_MAX - address ||
			legacy_uint64_to_off_t(address + total, &offset) != 0)
			return -1;

		chunk = legacy_read_chunk_size(length - total);
		count = pread(fd, buffer + total, chunk, offset);
		if (count < 0) {
			if (errno == EINTR) continue;
			return -1;
		}
		if (count == 0) return -1;
		total += (size_t)count;
	}

	return 0;
}
#endif

static int legacy_read_full_positionally(int fd, const char* path,
	uint64_t address, uint8_t* buffer, size_t length) {
	(void)path;

	#if defined(_POSIX_VERSION) && _POSIX_VERSION >= 200112L
	if (legacy_read_full_with_pread(fd, address, buffer, length) == 0)
		return 0;
	lb_dbg("pread failed for %s at 0x%llx; trying lseek/read: %s",
		path, (unsigned long long)address, strerror(errno));
	#endif

	return legacy_read_full_with_seek(fd, address, buffer, length);
}

static int legacy_round_mapping_size(size_t required, size_t page_size,
	size_t* map_size) {
	size_t remainder;

	if (!map_size || required == 0 || page_size == 0) return -1;

	remainder = required % page_size;
	if (remainder != 0) {
		size_t padding = page_size - remainder;
		if (required > SIZE_MAX - padding) return -1;
		required += padding;
	}

	*map_size = required;
	return 0;
}

static int legacy_read_physical_range(int fd, const char* path,
	uint64_t address, uint8_t* buffer, size_t length) {
	off_t offset;
	long page_size_signed;

	if (!path || !buffer || length == 0 ||
		legacy_uint64_to_off_t(address, &offset) != 0)
		return -1;

	page_size_signed = sysconf(_SC_PAGESIZE);
	if (page_size_signed > 0) {
		size_t page_size = (size_t)page_size_signed;
		off_t aligned_offset =
			offset - (off_t)((uint64_t)offset % page_size);
		size_t page_offset = (size_t)(offset - aligned_offset);
		size_t required;
		size_t map_size;

		if (length <= SIZE_MAX - page_offset) {
			required = length + page_offset;
			if (legacy_round_mapping_size(
					required, page_size, &map_size) == 0) {
				void* mapping = mmap(NULL, map_size, PROT_READ,
					MAP_SHARED, fd, aligned_offset);
				if (mapping != MAP_FAILED) {
					memcpy(buffer,
						(const uint8_t*)mapping + page_offset, length);
					munmap(mapping, map_size);
					return 0;
				}

				lb_dbg("mmap failed for %s at 0x%llx; trying positional reads: %s",
					path, (unsigned long long)address, strerror(errno));
			}
		}
	}

	return legacy_read_full_positionally(
		fd, path, address, buffer, length);
}

static int legacy_load_from_device(lazybiosCTX_t* ctx, const char* path) {
	uint8_t* window = NULL;
	uint8_t* table = NULL;
	uint64_t table_address;
	size_t entry_offset;
	size_t entry_len;
	size_t table_len;
	int fd = -1;
	int result = -1;

	fd = open(path, O_RDONLY);
	if (fd == -1) {
		lb_dbg("Failed to open %s: %s", path, strerror(errno));
		return -1;
	}

	window = malloc(LEGACY_PHYSICAL_SMBIOS_SIZE);
	if (!window) goto cleanup;

	if (legacy_read_physical_range(fd, path,
			LEGACY_PHYSICAL_SMBIOS_START, window,
			LEGACY_PHYSICAL_SMBIOS_SIZE) != 0) {
		lb_dbg("Failed to read the legacy SMBIOS window from %s: %s",
			path, strerror(errno));
		goto cleanup;
	}

	if (lazybiosFindSMBIOSEntryPoint(window,
			LEGACY_PHYSICAL_SMBIOS_SIZE, &entry_offset,
			&entry_len) != 0) {
		lb_dbg("No valid SMBIOS entry point was found through %s", path);
		goto cleanup;
	}

	if (lazybiosGetSMBIOSTableLocation(window + entry_offset, entry_len,
			&entry_len, &table_address, &table_len) != 0) {
		lb_dbg("The SMBIOS entry point read through %s has an invalid table location",
			path);
		goto cleanup;
	}

	table = malloc(table_len);
	if (!table) goto cleanup;

	if (legacy_read_physical_range(
			fd, path, table_address, table, table_len) != 0) {
		lb_dbg("Failed to read the SMBIOS table from %s at 0x%llx: %s",
			path, (unsigned long long)table_address, strerror(errno));
		goto cleanup;
	}

	result = lazybiosLoadRawBuffers(ctx, window + entry_offset, entry_len,
		table, table_len);

cleanup:
	free(table);
	free(window);
	close(fd);
	return result;
}

#endif

int lazybiosLoadLegacyPhysicalMemory(lazybiosCTX_t* ctx,
	const char* const* device_paths, size_t device_count,
	const char* platform_name) {
	(void)platform_name;

	if (!ctx || !ctx->DMIData || !device_paths || device_count == 0)
		return -1;

	#if !LAZYBIOS_LEGACY_X86
	lb_log("%s host loading requires an x86 legacy SMBIOS firmware window",
		platform_name ? platform_name : "Generic");
	return -1;
	#else
	for (size_t i = 0; i < device_count; i++) {
		if (device_paths[i] &&
			legacy_load_from_device(ctx, device_paths[i]) == 0)
			return 0;
	}

	lb_log("%s SMBIOS loading failed; check physical-memory device availability and permissions",
		platform_name ? platform_name : "Generic");
	return -1;
	#endif
}

#endif

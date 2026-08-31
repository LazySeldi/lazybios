/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file fuzz_backend_buffers.c
 * @brief Fuzzes platform-neutral transformations used by OS SMBIOS backends.
 */

#include "fuzz_common.h"
#include "lazybios_internal.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define WINDOWS_RAW_HEADER_SIZE 8

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size);

static void put_u32_le(uint8_t out[4], uint32_t value) {
	out[0] = (uint8_t)value;
	out[1] = (uint8_t)(value >> 8);
	out[2] = (uint8_t)(value >> 16);
	out[3] = (uint8_t)(value >> 24);
}

static void fuzz_arbitrary_windows_buffer(const uint8_t* data, size_t size) {
	lazybiosCTX_t* ctx = lazybiosCTXNew();
	if (!ctx) return;
	if (lazybiosLoadWindowsRawSMBIOSData(ctx, data, size) == 0)
		fuzz_parse_all_types(ctx);
	lazybiosCleanup(ctx);
}

static void fuzz_wrapped_windows_buffer(const uint8_t* data, size_t size) {
	if (size < 3 || size - 3 > UINT32_MAX) return;

	const size_t table_len = size - 3;
	if (table_len > SIZE_MAX - WINDOWS_RAW_HEADER_SIZE) return;
	uint8_t* raw = malloc(WINDOWS_RAW_HEADER_SIZE + table_len);
	if (!raw) return;

	raw[0] = data[0];
	raw[1] = data[1];
	raw[2] = data[2];
	raw[3] = data[0] >> 1;
	put_u32_le(raw + 4, (uint32_t)table_len);
	memcpy(raw + WINDOWS_RAW_HEADER_SIZE, data + 3, table_len);

	lazybiosCTX_t* ctx = lazybiosCTXNew();
	if (ctx) {
		if (lazybiosLoadWindowsRawSMBIOSData(ctx, raw,
				WINDOWS_RAW_HEADER_SIZE + table_len) == 0)
			fuzz_parse_all_types(ctx);
		lazybiosCleanup(ctx);
	}
	free(raw);
}

static void fuzz_raw_backend_buffers(const uint8_t* data, size_t size) {
	if (size < 4) return;

	const uint8_t selector = data[0];
	const uint8_t* table = data + 3;
	const size_t table_len = size - 3;
	size_t entry_len = 0;
	uint8_t* entry = (selector & 1)
		? fuzz_make_entry_3x(data[1], data[2], selector >> 1, table_len, &entry_len)
		: fuzz_make_entry_2x(data[1], data[2], table_len, &entry_len);
	if (!entry) return;

	size_t located_entry_len = 0;
	size_t located_table_len = 0;
	uint64_t table_address = 0;
	fuzz_sink_val((uint64_t)lazybiosGetSMBIOSTableLocation(
		entry, entry_len, &located_entry_len, &table_address,
		&located_table_len));
	fuzz_sink_val(located_entry_len);
	fuzz_sink_val(table_address);
	fuzz_sink_val(located_table_len);

	lazybiosCTX_t* ctx = lazybiosCTXNew();
	if (ctx) {
		if (lazybiosLoadRawBuffers(ctx, entry, entry_len, table, table_len) == 0) {
			fuzz_parse_all_types(ctx);
			/* Reuse must fail without replacing the context's owned data. */
			fuzz_sink_val((uint64_t)lazybiosLoadRawBuffers(
				ctx, entry, entry_len, table, table_len));
		}
		lazybiosCleanup(ctx);
	}
	free(entry);
}

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
	size_t entry_offset = 0;
	size_t entry_len = 0;
	size_t table_len = 0;
	uint64_t table_address = 0;
	fuzz_sink_val((uint64_t)lazybiosFindSMBIOSEntryPoint(
		data, size, &entry_offset, &entry_len));
	fuzz_sink_val(entry_offset);
	fuzz_sink_val(entry_len);
	fuzz_sink_val((uint64_t)lazybiosGetSMBIOSTableLocation(
		data, size, &entry_len, &table_address, &table_len));
	fuzz_sink_val(entry_len);
	fuzz_sink_val(table_address);
	fuzz_sink_val(table_len);

	fuzz_arbitrary_windows_buffer(data, size);
	fuzz_wrapped_windows_buffer(data, size);
	fuzz_raw_backend_buffers(data, size);
	return 0;
}

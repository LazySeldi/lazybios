/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file fuzz_entry_point.c
 * @brief libFuzzer target for SMBIOS entry point validation.
 *
 * The whole input is treated as a raw entry point. The buffer is allocated at
 * exactly the input size, so any read past the declared entry point length is
 * reported by ASan.
 */

#include "fuzz_common.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size);

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
	if (size == 0) return 0;

	lazybiosCTX_t* ctx = lazybiosCTXNew();
	if (!ctx) return 0;

	uint8_t* entry = malloc(size);
	if (!entry) {
		lazybiosCleanup(ctx);
		return 0;
	}
	memcpy(entry, data, size);

	/* The context owns the buffer from here on. */
	ctx->DMIData->entry_data = entry;
	ctx->DMIData->entry_len = size;

	if (lazybiosParseEntry(ctx, ctx->DMIData->entry_data, ctx->DMIData->entry_len) == 0) {
		for (uint8_t major = 0; major < 4; major++)
			for (uint8_t minor = 0; minor < 8; minor++)
				fuzz_sink_val((uint64_t)lazybiosIsVersionPlus(ctx->DMIData, major, minor));
	}

	lazybiosCleanup(ctx);
	return 0;
}

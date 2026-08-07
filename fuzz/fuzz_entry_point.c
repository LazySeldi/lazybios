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

int LLVMFuzzerInitialize(int* argc, char*** argv);
int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size);

int LLVMFuzzerInitialize(int* argc, char*** argv) {
	(void)argc;
	(void)argv;
	/* lazybiosPrintSMVer() writes to stdout; keep the fuzzer output readable. */
	#if defined(_WIN32) || defined(_WIN64)
		const char* null_device = "NUL";
	#else
		const char* null_device = "/dev/null";
	#endif
	if (!freopen(null_device, "w", stdout)) return 0;
	return 0;
}

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
		lazybiosPrintSMVer(ctx);
		for (uint8_t major = 0; major < 4; major++)
			for (uint8_t minor = 0; minor < 8; minor++)
				fuzz_sink_val((uint64_t)lazybiosIsVersionPlus(ctx->DMIData, major, minor));
	}

	lazybiosCleanup(ctx);
	return 0;
}

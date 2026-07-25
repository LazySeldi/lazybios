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
 * @file fuzz_dmi_table.c
 * @brief libFuzzer target for the SMBIOS structure table parsers.
 *
 * The first three input bytes select the SMBIOS version advertised by a
 * synthesized entry point; the rest of the input becomes the raw DMI table.
 * Building the entry point here (instead of taking it from the input) keeps
 * every iteration reaching the structure parsers, and the version bytes still
 * let the fuzzer flip the version-gated code paths.
 *
 * The DMI buffer is allocated at exactly the input size so that ASan traps any
 * read past the end of the table.
 */

#include "fuzz_common.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size);

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
	if (size < 4) return 0;

	const uint8_t selector = data[0];
	const uint8_t major = data[1];
	const uint8_t minor = data[2];
	const uint8_t* table = data + 3;
	const size_t table_len = size - 3;

	lazybiosCTX_t* ctx = lazybiosCTXNew();
	if (!ctx) return 0;

	size_t entry_len = 0;
	uint8_t* entry = (selector & 1)
		? fuzz_make_entry_3x(major, minor, selector >> 1, table_len, &entry_len)
		: fuzz_make_entry_2x(major, minor, table_len, &entry_len);
	if (!entry) {
		lazybiosCleanup(ctx);
		return 0;
	}

	/* The context owns the entry point from here on. */
	ctx->DMIData->entry_data = entry;
	ctx->DMIData->entry_len = entry_len;

	if (lazybiosParseEntry(ctx, ctx->DMIData->entry_data, ctx->DMIData->entry_len) != 0) {
		lazybiosCleanup(ctx);
		return 0;
	}

	ctx->DMIData->dmi_data = malloc(table_len);
	if (!ctx->DMIData->dmi_data) {
		lazybiosCleanup(ctx);
		return 0;
	}
	memcpy(ctx->DMIData->dmi_data, table, table_len);
	ctx->DMIData->dmi_len = table_len;

	fuzz_sink_val((uint64_t)lazybiosIsVersionPlus(ctx->DMIData, major, minor));

	/* The getters cover types 0-46; this reaches the rest of the type space. */
	fuzz_sink_val(lazybiosCountStructsByType(ctx->DMIData, selector));

	fuzz_parse_all_types(ctx);

	lazybiosCleanup(ctx);
	return 0;
}

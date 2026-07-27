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
 * @file fuzz_helpers.c
 * @brief Fuzzes bounded traversal helpers and partial-context cleanup.
 */

#include "fuzz_common.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size);

static void fuzz_unavailable_init(uint8_t selector) {
	lazybiosCTX_t* ctx = lazybiosCTXNew();
	if (!ctx) return;

	const lazybiosBackend_t backends[] = {
		LAZYBIOS_BACKEND_LINUX,
		LAZYBIOS_BACKEND_WINDOWS,
		LAZYBIOS_BACKEND_MACOS,
		LAZYBIOS_BACKEND_OPENBSD,
		LAZYBIOS_BACKEND_FREEBSD,
		LAZYBIOS_BACKEND_NETBSD,
		LAZYBIOS_BACKEND_SUNOS,
		LAZYBIOS_BACKEND_DRAGONFLY,
		LAZYBIOS_BACKEND_UNKNOWN,
		LAZYBIOS_BACKEND_HAIKU,
		LAZYBIOS_BACKEND_BEOS,
		LAZYBIOS_BACKEND_GENERIC
	};
	lazybiosBackend_t selected =
		backends[selector % (sizeof(backends) / sizeof(backends[0]))];
	if (selected == ctx->backend) selected = LAZYBIOS_BACKEND_UNKNOWN;
	ctx->backend = selected;
	fuzz_sink_val((uint64_t)lazybiosInit(ctx));
	lazybiosCleanup(ctx);
}

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
	if (size < 3) return 0;

	uint8_t* bytes = malloc(size);
	if (!bytes) return 0;
	memcpy(bytes, data, size);

	size_t p_offset = (size_t)data[0] % (size + 1);
	size_t end_offset = (size_t)data[1] % (size + 1);
	const uint8_t* p = bytes + p_offset;
	const uint8_t* end = bytes + end_offset;

	const uint8_t* next = DMINext(p, end);
	if (p <= end && (next < p || next > end))
		abort();

	char* string = DMIString(p, data[2], data[0], end);
	fuzz_sink_str(string);
	free(string);

	lazybiosDMI_t dmi = {
		.dmi_data = bytes,
		.dmi_len = size
	};
	for (unsigned int type = 0; type <= UINT8_MAX; type++)
		fuzz_sink_val(lazybiosCountStructsByType(&dmi, (uint8_t)type));

	lazybiosCTX_t* partial = lazybiosCTXNew();
	if (partial) {
		partial->DMIData->entry_data = malloc(size);
		partial->DMIData->dmi_data = malloc(size);
		if (partial->DMIData->entry_data) {
			memcpy(partial->DMIData->entry_data, data, size);
			partial->DMIData->entry_len = size;
			(void)lazybiosParseEntry(partial,
				partial->DMIData->entry_data, partial->DMIData->entry_len);
		}
		if (partial->DMIData->dmi_data) {
			memcpy(partial->DMIData->dmi_data, data, size);
			partial->DMIData->dmi_len = size;
			fuzz_parse_all_types(partial);
		}
		lazybiosCleanup(partial);
	}

	fuzz_unavailable_init(data[2]);
	free(bytes);
	return 0;
}

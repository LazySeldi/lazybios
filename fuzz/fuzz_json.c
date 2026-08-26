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
 * @file fuzz_json.c
 * @brief libFuzzer target for the cJSON serializers.
 *
 * Structures parsed from an arbitrary DMI table are handed to every
 * lazybiosExtJSONAdd*() function, then the whole tree is printed and released.
 * The serializers read the string pointers the parsers left pointing into the
 * table, so a parser that produced an out-of-bounds or non-terminated string
 * surfaces here as an ASan failure inside cJSON rather than going unnoticed.
 *
 * The first three input bytes select the entry point version and the backend;
 * the rest becomes the raw DMI table, allocated at exactly its own length so
 * that any read past the end traps.
 */

#include "fuzz_common.h"

#include "lazybios/json/lazybios_json.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size);

/* Every standard serializer takes the same (set, root) shape. */
#define JSON_ADD_TYPE(N) \
	lazybiosExtJSONAddType##N(ctx->Type##N, root)

static void fuzz_serialize_all(lazybiosCTX_t* ctx, cJSON* root) {
	JSON_ADD_TYPE(0);  JSON_ADD_TYPE(1);  JSON_ADD_TYPE(2);  JSON_ADD_TYPE(3);
	JSON_ADD_TYPE(4);  JSON_ADD_TYPE(5);  JSON_ADD_TYPE(6);  JSON_ADD_TYPE(7);
	JSON_ADD_TYPE(8);  JSON_ADD_TYPE(9);  JSON_ADD_TYPE(10); JSON_ADD_TYPE(11);
	JSON_ADD_TYPE(12); JSON_ADD_TYPE(13); JSON_ADD_TYPE(14); JSON_ADD_TYPE(15);
	JSON_ADD_TYPE(16); JSON_ADD_TYPE(17); JSON_ADD_TYPE(18); JSON_ADD_TYPE(19);
	JSON_ADD_TYPE(20); JSON_ADD_TYPE(21); JSON_ADD_TYPE(22); JSON_ADD_TYPE(23);
	JSON_ADD_TYPE(24); JSON_ADD_TYPE(25); JSON_ADD_TYPE(26); JSON_ADD_TYPE(27);
	JSON_ADD_TYPE(28); JSON_ADD_TYPE(29); JSON_ADD_TYPE(30); JSON_ADD_TYPE(31);
	JSON_ADD_TYPE(32); JSON_ADD_TYPE(33); JSON_ADD_TYPE(34); JSON_ADD_TYPE(35);
	JSON_ADD_TYPE(36); JSON_ADD_TYPE(37); JSON_ADD_TYPE(38); JSON_ADD_TYPE(39);
	JSON_ADD_TYPE(40); JSON_ADD_TYPE(41); JSON_ADD_TYPE(42); JSON_ADD_TYPE(43);
	JSON_ADD_TYPE(44); JSON_ADD_TYPE(45); JSON_ADD_TYPE(46);

	lazybiosExtJSONAddOemDellType177(ctx->oem->dell->Type177, root);
	lazybiosExtJSONAddOemDellType212(ctx->oem->dell->Type212, root);
	lazybiosExtJSONAddOemDellType218(ctx->oem->dell->Type218, root);
	lazybiosExtJSONAddOemHpType204(ctx->oem->hp->Type204, root);
}

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
		? fuzz_make_entry_3x(major, minor, selector >> 2, table_len, &entry_len)
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

	fuzz_parse_all_types(ctx);

	cJSON* root = cJSON_CreateObject();
	if (root) {
		/*
		 * lazybiosExtJSONAddSMBIOSInfo() omits the table address on the
		 * Windows backend, so drive both sides of that branch.
		 */
		lazybiosExtJSONAddSMBIOSInfo(ctx->DMIData,
			(selector & 2) ? LAZYBIOS_BACKEND_WINDOWS : ctx->backend, root);

		fuzz_serialize_all(ctx, root);

		char* printed = cJSON_Print(root);
		fuzz_sink_str(printed);
		free(printed);
		cJSON_Delete(root);
	}

	lazybiosCleanup(ctx);
	return 0;
}

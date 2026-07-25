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
 * @file fuzz_single_file.c
 * @brief libFuzzer target for lazybiosSingleFile() and the parsers behind it.
 *
 * The input is written to a scratch file and loaded back through the public
 * merged-file loader, so the entry point sizing, seeking and short-read paths
 * are exercised together with the structure parsers.
 */

#include "fuzz_common.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char scratch_path[] = "/tmp/lazybios_fuzz_XXXXXX";

int LLVMFuzzerInitialize(int* argc, char*** argv);
int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size);

static void remove_scratch(void) {
	unlink(scratch_path);
}

int LLVMFuzzerInitialize(int* argc, char*** argv) {
	(void)argc;
	(void)argv;

	int fd = mkstemp(scratch_path);
	if (fd < 0) {
		perror("mkstemp");
		abort();
	}
	close(fd);
	atexit(remove_scratch);
	return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
	FILE* f = fopen(scratch_path, "wb");
	if (!f) return 0;
	if (size != 0 && fwrite(data, 1, size, f) != size) {
		fclose(f);
		return 0;
	}
	if (fclose(f) != 0) return 0;

	lazybiosCTX_t* ctx = lazybiosCTXNew();
	if (!ctx) return 0;

	if (lazybiosSingleFile(ctx, scratch_path) == 0)
		fuzz_parse_all_types(ctx);

	lazybiosCleanup(ctx);
	return 0;
}

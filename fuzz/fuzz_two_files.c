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
 * @file fuzz_two_files.c
 * @brief libFuzzer target for the two-file loader and the parsers behind it.
 *
 * This is the shape a Linux host load takes: lazybiosInit() is given the two
 * sysfs files (smbios_entry_point and DMI). The input is split at a length
 * prefix into those two files. Loading through lazybiosInit() also builds the
 * type index, so the index is fuzzed against the same arbitrary tables.
 */

#include "fuzz_common.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char entry_path[] = "/tmp/lazybios_fuzz_entry_XXXXXX";
static char dmi_path[] = "/tmp/lazybios_fuzz_dmi_XXXXXX";

int LLVMFuzzerInitialize(int* argc, char*** argv);
int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size);

static void remove_scratch(void) {
	unlink(entry_path);
	unlink(dmi_path);
}

static int make_scratch(char* path) {
	int fd = mkstemp(path);
	if (fd < 0) return -1;
	close(fd);
	return 0;
}

static int write_scratch(const char* path, const uint8_t* data, size_t size) {
	FILE* f = fopen(path, "wb");
	if (!f) return -1;
	if (size != 0 && fwrite(data, 1, size, f) != size) {
		fclose(f);
		return -1;
	}
	return fclose(f) == 0 ? 0 : -1;
}

int LLVMFuzzerInitialize(int* argc, char*** argv) {
	(void)argc;
	(void)argv;

	if (make_scratch(entry_path) != 0 || make_scratch(dmi_path) != 0) {
		perror("mkstemp");
		abort();
	}
	atexit(remove_scratch);
	return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
	if (size < 1) return 0;

	/* One byte picks how much of the input becomes the entry point file. */
	size_t entry_size = data[0];
	data++;
	size--;
	if (entry_size > size) entry_size = size;

	if (write_scratch(entry_path, data, entry_size) != 0) return 0;
	if (write_scratch(dmi_path, data + entry_size, size - entry_size) != 0) return 0;

	lazybiosCTX_t* ctx = lazybiosCTXNew();
	if (!ctx) return 0;

	if (lazybiosInit(ctx, entry_path, dmi_path) == 0)
		fuzz_parse_all_types(ctx);

	lazybiosCleanup(ctx);
	return 0;
}

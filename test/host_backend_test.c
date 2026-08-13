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
 * @file host_backend_test.c
 * @brief Optional native integration check for lazybiosInit().
 */

#include "lazybios_internal.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_SKIP 77
#define SMBIOS_END_TYPE 127

int main(void) {
	lazybiosCTX_t* ctx = lazybiosCTXNew();
	if (!ctx) return EXIT_FAILURE;

	if (lazybiosInit(ctx) != 0) {
		fprintf(stderr,
			"host SMBIOS data is unavailable or requires additional privileges\n");
		lazybiosCleanup(ctx);
		return TEST_SKIP;
	}

	lazybiosDMI_t* dmi = ctx->DMIData;
	if (!dmi || !dmi->entry_data || !dmi->dmi_data ||
		dmi->entry_len == 0 || dmi->dmi_len == 0 ||
		dmi->entry_tag == SMBIOS_VER_UNKNOWN) {
		lazybiosCleanup(ctx);
		return EXIT_FAILURE;
	}

	const uint8_t* current = dmi->dmi_data;
	const uint8_t* end = dmi->dmi_data + dmi->dmi_len;
	size_t structures = 0;
	while (current <= end && (size_t)(end - current) >= SMBIOS_HEADER_SIZE) {
		structures++;
		if (current[0] == SMBIOS_END_TYPE) break;

		const uint8_t* next = DMINext(current, end);
		if (next <= current || next > end) {
			lazybiosCleanup(ctx);
			return EXIT_FAILURE;
		}
		current = next;
	}

	if (structures == 0) {
		lazybiosCleanup(ctx);
		return EXIT_FAILURE;
	}

	lazybiosCleanup(ctx);
	return EXIT_SUCCESS;
}

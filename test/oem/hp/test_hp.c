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
 * @file test_hp.c
 * @brief HP OEM specific SMBIOS structure tests.
 * @author LazySeldi
 */

#include "lazybios/lazybios.h"
#include "lazybios/structures/oem/hp/hp_type204.h"
#include "../test_oem.h"
#include <stdio.h>

static void printOemHPType204(lazybiosCTX_t* ctx) {
    printf("=== Oem HP Type 204 ===\n");

    if (!ctx->HpType204) ctx->HpType204 = lazybiosGetOemHpType204(ctx->HpType204, &ctx->hptype204_count, ctx->DMIData);

    if (ctx->HpType204 && ctx->hptype204_count > 0) {
        for (size_t i = 0; i < ctx->hptype204_count; i++) {
            lazybiosOemHpType204_t* HpType204 = &ctx->HpType204[i];

            if (ctx->hptype204_count > 1) printf("--- Oem HP Type 204: %zu ---\n", i + 1);

            if (LAZYBIOS_FIELD_STATUS(HpType204, rack_name) == LAZYBIOS_FIELD_PRESENT) {
                printf("Rack Name: %s\n", HpType204->rack_name);
            }
        }
    } else {
        printf("Failed to get Oem Hp Type 204 information\n\n");
    }
}

void printOemHP(lazybiosCTX_t* ctx) {
    printOemHPType204(ctx);
}

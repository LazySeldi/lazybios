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

#include "lazybios.h"
#include "lazybios/oem/hp/hp_type201.h"
#include "../test_oem.h"
#include <stdio.h>

#ifdef LAZYBIOS_OEM_HP
static void printOemHPType201(lazybiosCTX_t* ctx) {
    printf("=== Oem HP Type 201 ===\n");

    if (!ctx->HpType201) ctx->HpType201 = lazybiosGetOemHpType201(ctx->HpType201, &ctx->hptype201_count, ctx->DMIData);

    if (ctx->HpType201 && ctx->hptype201_count > 0) {
        for (size_t i = 0; i < ctx->hptype201_count; i++) {
            lazybiosOemHpType201_t* HpType201 = &ctx->HpType201[i];

            if (ctx->hptype201_count > 1) printf("--- Oem HP Type 201: %zu ---\n", i + 1);

            if (LAZYBIOS_FIELD_STATUS(HpType201, rack_name) == LAZYBIOS_FIELD_PRESENT) {
                printf("Rack Name: %s\n", HpType201->rack_name);
            }
        }
    } else {
        printf("Failed to get Oem Hp Type 201 information\n\n");
    }
}

void printOemHP(lazybiosCTX_t* ctx) {
    printOemHPType201(ctx);
    /* Add other HP types here */
}
#endif

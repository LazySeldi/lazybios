/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
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

static inline void printOemHPType204(lazybiosCTX_t* ctx) {
    printf("\n=== Oem HP Type 204 ===\n");

    if (!ctx->oem->hp->Type204) ctx->oem->hp->Type204 = lazybiosGetOemHpType204(ctx->DMIData);

    if (ctx->oem->hp->Type204 && ctx->oem->hp->Type204->count > 0) {
        for (size_t i = 0; i < ctx->oem->hp->Type204->count; i++) {
            lazybiosOemHpType204_t* HpType204 = &ctx->oem->hp->Type204->entries[i];

            if (ctx->oem->hp->Type204->count > 1) printf("--- Oem HP Type 204: %zu ---\n", i + 1);

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

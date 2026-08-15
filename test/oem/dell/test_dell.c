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
 * @file test_dell.c
 * @brief DELL OEM specific SMBIOS structure tests.
 * @author LazySeldi
 */

#include <stdio.h>
#include "../test_oem.h"
#include "lazybios/lazybios.h"
#include "lazybios/structures/oem/dell/dell_type177.h"

static void printOemDELLType177(lazybiosCTX_t* ctx) {
    printf("=== Oem DELL Type 177 ===\n");

    if (!ctx->DellType177) ctx->DellType177 = lazybiosGetOemDellType177(ctx->DellType177, &ctx->delltype177_count, ctx->DMIData);

    if (ctx->DellType177 && ctx->delltype177_count > 0) {
        for (size_t i = 0; i < ctx->delltype177_count; i++) {
            lazybiosOemDellType177_t* DellType177 = &ctx->DellType177[i];

            if (ctx->delltype177_count > 1) printf("--- Oem DELL Type 177: %zu ---\n", i + 1);

            if (LAZYBIOS_FIELD_STATUS(DellType177, acpi_wmi_supported) == LAZYBIOS_FIELD_PRESENT) {
                printf("ACPI WMI Supported: %s\n", DellType177->acpi_wmi_supported);
            }
        }
    } else {
        printf("Failed to get Oem Dell Type 177 information\n\n");
    }
}

void printOemDell(lazybiosCTX_t* ctx) {
    printOemDELLType177(ctx);
}

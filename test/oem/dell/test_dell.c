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

static inline void printOemDELLType177(lazybiosCTX_t* ctx) {
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

static inline void printOemDELLType212(lazybiosCTX_t* ctx) {
    printf("=== OEM Dell Type 212 ===\n");

    if (!ctx->DellType212) {
        ctx->DellType212 = lazybiosGetOemDellType212(
            ctx->DellType212,
            &ctx->delltype212_count,
            ctx->DMIData
        );
    }

    if (ctx->DellType212 && ctx->delltype212_count > 0) {
        for (size_t i = 0; i < ctx->delltype212_count; i++) {
            lazybiosOemDellType212_t* current = &ctx->DellType212[i];

            if (ctx->delltype212_count > 1) {
                printf("--- Dell Type 212 Instance %zu ---\n", i + 1);
            }

            // Index Port
            if (LAZYBIOS_FIELD_STATUS(current, index_port) == LAZYBIOS_FIELD_PRESENT) {
                printf("\tIndex Port: 0x%04x\n", current->index_port);
            }

            // Data Port
            if (LAZYBIOS_FIELD_STATUS(current, data_port) == LAZYBIOS_FIELD_PRESENT) {
                printf("\tData Port: 0x%04x\n", current->data_port);
            }

            // Checksum Type
            if (LAZYBIOS_FIELD_STATUS(current, checksum_type) == LAZYBIOS_FIELD_PRESENT) {
                printf("\tType: %s\n", current->checksum_type);
            }

            // Start Index
            if (LAZYBIOS_FIELD_STATUS(current, start_index) == LAZYBIOS_FIELD_PRESENT) {
                printf("\tChecked Range Start Index: 0x%02x\n", current->start_index);
            }

            // End Index
            if (LAZYBIOS_FIELD_STATUS(current, end_index) == LAZYBIOS_FIELD_PRESENT) {
                printf("\tChecked Range End Index: 0x%02x\n", current->end_index);
            }

            // Value Index
            if (LAZYBIOS_FIELD_STATUS(current, value_index) == LAZYBIOS_FIELD_PRESENT) {
                printf("\tCheck Value Index: 0x%02x\n", current->value_index);
            }

            // Tokens
            if (LAZYBIOS_FIELD_STATUS(current, tokens) == LAZYBIOS_FIELD_PRESENT &&
                current->tokens && current->token_count > 0) {

                size_t print_count = current->token_count;

                if (print_count > 0) {
                    printf("\tTokens:\n");
                    for (size_t j = 0; j < print_count; j++) {
                        lazybiosOemDellType212Token_t* token = &current->tokens[j];
                        printf("\t\t0x%04x (location 0x%02x, AND mask 0x%02x, OR mask 0x%02x)\n",
                               token->token_id,
                               token->location,
                               token->and_mask,
                               token->or_mask);
                    }
                }
            }
        }
        printf("\n");
    } else {
        printf("No Dell Type 212 structures found.\n\n");
    }
}

void printOemDell(lazybiosCTX_t* ctx) {
    printOemDELLType177(ctx);
    printOemDELLType212(ctx);
}

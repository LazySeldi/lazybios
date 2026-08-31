/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
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
#include "lazybios/structures/oem/dell/dell_type218.h"

static inline void printOemDELLType177(lazybiosCTX_t* ctx) {
    printf("=== Oem DELL Type 177 ===\n");

    if (!ctx->oem->dell->Type177) ctx->oem->dell->Type177 = lazybiosGetOemDellType177(ctx->DMIData);

    if (ctx->oem->dell->Type177 && ctx->oem->dell->Type177->count > 0) {
        for (size_t i = 0; i < ctx->oem->dell->Type177->count; i++) {
            lazybiosOemDellType177_t* DellType177 = &ctx->oem->dell->Type177->entries[i];

            if (ctx->oem->dell->Type177->count > 1) printf("--- Oem DELL Type 177: %zu ---\n", i + 1);

            if (LAZYBIOS_FIELD_STATUS(DellType177, acpi_wmi_supported) == LAZYBIOS_FIELD_PRESENT) {
                printf("ACPI WMI Supported: %s\n", DellType177->decoded.acpi_wmi_supported);
            }
        }
    } else {
        printf("Failed to get Oem Dell Type 177 information\n\n");
    }
}

static inline void printOemDELLType212(lazybiosCTX_t* ctx) {
    printf("=== OEM Dell Type 212 ===\n");

    if (!ctx->oem->dell->Type212) {
        ctx->oem->dell->Type212 = lazybiosGetOemDellType212(ctx->DMIData);
    }

    if (ctx->oem->dell->Type212 && ctx->oem->dell->Type212->count > 0) {
        for (size_t i = 0; i < ctx->oem->dell->Type212->count; i++) {
            lazybiosOemDellType212_t* current = &ctx->oem->dell->Type212->entries[i];

            if (ctx->oem->dell->Type212->count > 1) {
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
                printf("\tType: %s\n", current->decoded.checksum_type);
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

static inline void printOemDELLType218(lazybiosCTX_t* ctx) {
    printf("=== OEM Dell Type 218 ===\n");

    if (!ctx->oem->dell->Type218) {
        ctx->oem->dell->Type218 = lazybiosGetOemDellType218(ctx->DMIData);
    }

    if (ctx->oem->dell->Type218 && ctx->oem->dell->Type218->count > 0) {
        for (size_t i = 0; i < ctx->oem->dell->Type218->count; i++) {
            lazybiosOemDellType218_t* current = &ctx->oem->dell->Type218->entries[i];

            if (ctx->oem->dell->Type218->count > 1) {
                printf("--- Dell Type 218 Instance %zu ---\n", i + 1);
            }

            // Command I/O Address
            if (LAZYBIOS_FIELD_STATUS(current, command_io_address) == LAZYBIOS_FIELD_PRESENT) {
                printf("\tCommand I/O Address: 0x%04x\n", current->command_io_address);
            }

            // Command I/O Code
            if (LAZYBIOS_FIELD_STATUS(current, command_io_code) == LAZYBIOS_FIELD_PRESENT) {
                printf("\tCommand I/O Code: 0x%02x\n", current->command_io_code);
            }

            // Supported Command Classes Bitmap
            if (LAZYBIOS_FIELD_STATUS(current, supported_command_classes_bitmap) == LAZYBIOS_FIELD_PRESENT) {
                printf("\tSupported Command Classes Bitmap: 0x%08x\n", current->supported_command_classes_bitmap);
            }

            // Tokens
            if (LAZYBIOS_FIELD_STATUS(current, tokens) == LAZYBIOS_FIELD_PRESENT &&
                current->tokens && current->token_count > 0) {

                printf("\tTokens:\n");
                for (size_t j = 0; j < current->token_count; j++) {
                    lazybiosOemDellType218Token_t* token = &current->tokens[j];
                    printf("\t\t0x%04x (location 0x%04x, value 0x%04x)\n",
                           token->token_id,
                           token->location,
                           token->value);
                }
            }
        }
        printf("\n");
    } else {
        printf("No Dell Type 218 structures found.\n\n");
    }
}

void printOemDell(lazybiosCTX_t* ctx) {
    printOemDELLType177(ctx);
    printOemDELLType212(ctx);
    printOemDELLType218(ctx);
}

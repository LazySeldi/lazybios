/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file dell_type218.c
 * @brief Implements parsing for DELL OEM SMBIOS Type 218 Information.
 * @author LazySeldi
 */

#include "lazybios_internal.h"
#include "lazybios/structures/oem/dell/dell_type218.h"
#include <stdlib.h>

#define COMMAND_IO_ADDR 0x04
#define COMMAND_IO_CODE 0x06
#define SUPPORTED_COMMAND_CLASSES_BITMAP 0x07
#define TOKENS_START_OFFSET 0x0B
#define TOKEN_SIZE 6
#define TOKEN_TERMINATOR_BYTE0 0xFF // the last token seems to be a terminator
#define TOKEN_TERMINATOR_BYTE1 0xFF
#define TOKEN_TERMINATOR_BYTE2 0x00
#define TOKEN_TERMINATOR_BYTE3 0x00
#define TOKEN_TERMINATOR_BYTE4 0x00

lazybiosOemDellType218Array_t* lazybiosGetOemDellType218(const lazybiosDMI_t* DMIData) {
    if (!DMIData || !DMIData->dmi_data) return NULL;

    lazybiosOemDellType218Array_t* out = calloc(1, sizeof(*out));
    if (!out) return NULL;

    const uint8_t *end = DMIData->dmi_data + DMIData->dmi_len;
    size_t count;
    const uint8_t *p;
    if (DMIData->index_valid != 1) {
        count = lazybiosCountStructsByType(DMIData, SMBIOS_OEM_DELL_TYPE218);
        p = DMIData->dmi_data;
    } else {
        count = DMIData->index[SMBIOS_OEM_DELL_TYPE218].count;
        p = DMIData->dmi_data + DMIData->index[SMBIOS_OEM_DELL_TYPE218].first;
    }
    size_t index = 0;

    if (count == 0) return out;

    out->entries = calloc(count, sizeof(*out->entries));
    if (!out->entries) {
        free(out);
        return NULL;
    }

    while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
        uint8_t type = p[0];
        uint8_t len = p[1];
        if (len < SMBIOS_HEADER_SIZE) break;
        const uint8_t *structure_end = DMINext(p, end);

        if (type == SMBIOS_OEM_DELL_TYPE218) {
            lazybiosOemDellType218_t *current = &out->entries[index];
            LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
            current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
            current->length = len;

            READU16(current, command_io_address, len, COMMAND_IO_ADDR, p);
            READU8(current, command_io_code, len, COMMAND_IO_CODE, p);
            READU32(current, supported_command_classes_bitmap, len, SUPPORTED_COMMAND_CLASSES_BITMAP, p);

            if (len >= TOKENS_START_OFFSET + TOKEN_SIZE) {
                const size_t token_bytes = len - TOKENS_START_OFFSET;
                current->token_count = token_bytes / TOKEN_SIZE;

                if (current->token_count > 0) {
                    current->tokens = calloc(current->token_count, sizeof(lazybiosOemDellType218Token_t));

                    const uint8_t* null_token = p + TOKENS_START_OFFSET + ((current->token_count - 1) * TOKEN_SIZE);

                    // if last token is a terminator, we skip it
                    if (null_token[0] == TOKEN_TERMINATOR_BYTE0 && null_token[1] == TOKEN_TERMINATOR_BYTE1 && null_token[2] == TOKEN_TERMINATOR_BYTE2
                        && null_token[3] == TOKEN_TERMINATOR_BYTE3 && null_token[4] == TOKEN_TERMINATOR_BYTE4) current->token_count -= 1;


                    if (current->tokens) {
                        for (size_t i = 0; i < current->token_count; i++) {
                            const uint8_t *t = p + TOKENS_START_OFFSET + (i * TOKEN_SIZE);
                            current->tokens[i].token_id = (uint16_t)((uint16_t)t[0] | ((uint16_t)t[1] << 8));
                            current->tokens[i].location = (uint16_t)((uint16_t)t[2] | ((uint16_t)t[3] << 8));
                            current->tokens[i].value = (uint16_t)((uint16_t)t[4] | ((uint16_t)t[5] << 8));
                        }
                        LAZYBIOS_MARK_PRESENT(current, tokens);
                    }
                }
            }

            index++;
        }
        p = structure_end;
    }

    out->count = index;
    return out;
}

void lazybiosFreeOemDellType218(lazybiosOemDellType218Array_t* DellType218) {
    if (!DellType218)
        return;

    for (size_t i = 0; i < DellType218->count; i++) {
        free(DellType218->entries[i].tokens);
    }

    free(DellType218->entries);

    free(DellType218);
}

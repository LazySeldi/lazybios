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
 * @file dell_type212.c
 * @brief Implements parsing for DELL OEM SMBIOS Type 212 Information.
 * @author LazySeldi
 */

#include "lazybios_internal.h"
#include "lazybios/structures/oem/dell/dell_type212.h"
#include <stdlib.h>

#define INDEX_PORT 0x04
#define DATA_PORT 0x06
#define CHECKSUM_TYPE_OFFSET 0x08
#define START_INDEX_OFFSET 0x09
#define END_INDEX_OFFSET 0x0A
#define VALUE_INDEX_OFFSET 0x0B
#define TOKENS_START_OFFSET 0x0C
#define TOKEN_SIZE 5
#define TOKEN_TERMINATOR_BYTE0 0xFF // the last token seems to be a terminator
#define TOKEN_TERMINATOR_BYTE1 0xFF
#define TOKEN_TERMINATOR_BYTE2 0x00
#define TOKEN_TERMINATOR_BYTE3 0x00
#define TOKEN_TERMINATOR_BYTE4 0x00


static inline const char* dell_type212_checksum_type_str(uint8_t type) {
    switch (type) {
        case 0: return "Word Checksum";
        case 1: return "Byte Checksum";
        case 2: return "CRC Checksum";
        case 3: return "Negative Word Checksum";
        default: return "Unknown Checksum";
    }
}

lazybiosOemDellType212Array_t* lazybiosGetOemDellType212(const lazybiosDMI_t* DMIData) {
    if (!DMIData || !DMIData->dmi_data) return NULL;

    lazybiosOemDellType212Array_t* out = calloc(1, sizeof(*out));
    if (!out) return NULL;

    const uint8_t *p = DMIData->dmi_data;
    const uint8_t *end = DMIData->dmi_data + DMIData->dmi_len;
    const size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_OEM_DELL_TYPE212);
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

        if (type == SMBIOS_OEM_DELL_TYPE212) {
            lazybiosOemDellType212_t *current = &out->entries[index];
            LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
            current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
            current->length = len;
            const uint8_t *structure_end = DMINext(p, end);
            (void)structure_end;

            // Fixed fields
            READU16(current, index_port, len, INDEX_PORT, p);
            READU16(current, data_port, len, DATA_PORT, p);

            // Checksum type (if structure is long enough)
            if (len > CHECKSUM_TYPE_OFFSET) {
                current->checksum_type = p[CHECKSUM_TYPE_OFFSET];
                current->decoded.checksum_type = dell_type212_checksum_type_str(current->checksum_type);
                LAZYBIOS_MARK_PRESENT(current, checksum_type);
            } else {
                current->checksum_type = 0;
                current->decoded.checksum_type = dell_type212_checksum_type_str(0);
                LAZYBIOS_MARK_ABSENT(current, checksum_type);
            }

            READU8(current, start_index, len, START_INDEX_OFFSET, p);
            READU8(current, end_index, len, END_INDEX_OFFSET, p);
            READU8(current, value_index, len, VALUE_INDEX_OFFSET, p);

            // Tokens (5 bytes each)
            if (len >= TOKENS_START_OFFSET + TOKEN_SIZE) {
                const size_t token_bytes = len - TOKENS_START_OFFSET;
                current->token_count = token_bytes / TOKEN_SIZE;
                if (current->token_count > 0) {
                    const uint8_t* null_token = p + TOKENS_START_OFFSET + ((current->token_count - 1) * TOKEN_SIZE);

                    // if last token is a terminator, we skip it
                    if (null_token[0] == TOKEN_TERMINATOR_BYTE0 && null_token[1] == TOKEN_TERMINATOR_BYTE1 && null_token[2] == TOKEN_TERMINATOR_BYTE2
                        && null_token[3] == TOKEN_TERMINATOR_BYTE3 && null_token[4] == TOKEN_TERMINATOR_BYTE4) current->token_count -= 1;

                    current->tokens = calloc(current->token_count, sizeof(lazybiosOemDellType212Token_t));

                    if (current->tokens) {
                        for (size_t i = 0; i < current->token_count; i++) {
                            const uint8_t *t = p + TOKENS_START_OFFSET + (i * TOKEN_SIZE);
                            current->tokens[i].token_id = (uint16_t)((uint16_t)t[0] | ((uint16_t)t[1] << 8));
                            current->tokens[i].location = t[2];
                            current->tokens[i].and_mask = t[3];
                            current->tokens[i].or_mask = t[4];
                        }
                        LAZYBIOS_MARK_PRESENT(current, tokens);
                    }
                }
            } else {
                current->token_count = 0;
                current->tokens = NULL;
                LAZYBIOS_MARK_ABSENT(current, tokens);
            }

            index++;
        }
        p = DMINext(p, end);
    }

    out->count = index;
    return out;
}

void lazybiosFreeOemDellType212(lazybiosOemDellType212Array_t* DellType212) {
    if (!DellType212)
        return;

    for (size_t i = 0; i < DellType212->count; i++) {
        free(DellType212->entries[i].tokens);
    }

    free(DellType212->entries);

    free(DellType212);
}

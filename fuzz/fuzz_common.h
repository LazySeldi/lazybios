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
 * @file fuzz_common.h
 * @brief Shared helpers for the lazybios libFuzzer targets.
 */

#ifndef LAZYBIOS_FUZZ_COMMON_H
#define LAZYBIOS_FUZZ_COMMON_H

#include "lazybios_internal.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/** @brief Length of an SMBIOS 2.x entry point. */
#define FUZZ_SMBIOS2_ENTRY_LEN 0x1F
/** @brief Length of an SMBIOS 3.x entry point. */
#define FUZZ_SMBIOS3_ENTRY_LEN 0x18

/** @brief Sequential reader over the buffer handed to the fuzz target. */
typedef struct {
	const uint8_t* data;
	size_t size;
	size_t pos;
} fuzz_reader_t;

static inline void fuzz_reader_init(fuzz_reader_t* r, const uint8_t* data, size_t size) {
	r->data = data;
	r->size = size;
	r->pos = 0;
}

/*
 * Reads the next byte, wrapping around once the input is exhausted so that a
 * short input still reaches every call site in a target.
 */
static inline uint8_t fuzz_u8(fuzz_reader_t* r) {
	if (r->size == 0) return 0;
	if (r->pos >= r->size) r->pos = 0;
	return r->data[r->pos++];
}

static inline uint64_t fuzz_u64(fuzz_reader_t* r) {
	uint64_t v = 0;
	for (int i = 0; i < 8; i++)
		v = (v << 8) | fuzz_u8(r);
	return v;
}

/* Keeps decoder results alive so the optimizer cannot drop the calls. */
static volatile uint64_t fuzz_sink_storage;

static inline void fuzz_sink_val(uint64_t v) {
	fuzz_sink_storage ^= v;
}

static inline void fuzz_sink_str(const char* s) {
	fuzz_sink_storage ^= s ? (uint64_t)strlen(s) : 1u;
}

/**
 * @brief Builds a well-formed SMBIOS 2.x entry point for a table of dmi_len bytes.
 * @return Newly allocated 31-byte entry point, or NULL on allocation failure.
 */
static inline uint8_t* fuzz_make_entry_2x(uint8_t major, uint8_t minor, size_t dmi_len, size_t* out_len) {
	uint8_t* e = calloc(1, FUZZ_SMBIOS2_ENTRY_LEN);
	if (!e) return NULL;

	memcpy(e, "_SM_", 4);
	e[0x05] = FUZZ_SMBIOS2_ENTRY_LEN;
	e[0x06] = major;
	e[0x07] = minor;
	e[0x08] = 0xFF; /* maximum structure size */
	e[0x09] = 0xFF;
	memcpy(e + 0x10, "_DMI_", 5);

	uint16_t table_len = (uint16_t)(dmi_len > 0xFFFF ? 0xFFFF : dmi_len);
	e[0x16] = (uint8_t)(table_len & 0xFF);
	e[0x17] = (uint8_t)(table_len >> 8);
	e[0x18] = 0x00;
	e[0x19] = 0x10;
	e[0x1E] = (uint8_t)((major << 4) | (minor & 0x0F));

	uint8_t sum = 0;
	for (int i = 0x10; i < 0x1F; i++) sum = (uint8_t)(sum + e[i]);
	e[0x15] = (uint8_t)(-sum);

	sum = 0;
	for (int i = 0x00; i < 0x10; i++) sum = (uint8_t)(sum + e[i]);
	e[0x04] = (uint8_t)(-sum);

	*out_len = FUZZ_SMBIOS2_ENTRY_LEN;
	return e;
}

/**
 * @brief Builds a well-formed SMBIOS 3.x entry point for a table of dmi_len bytes.
 * @return Newly allocated 24-byte entry point, or NULL on allocation failure.
 */
static inline uint8_t* fuzz_make_entry_3x(uint8_t major, uint8_t minor, uint8_t docrev, size_t dmi_len, size_t* out_len) {
	uint8_t* e = calloc(1, FUZZ_SMBIOS3_ENTRY_LEN);
	if (!e) return NULL;

	memcpy(e, "_SM3_", 5);
	e[0x06] = FUZZ_SMBIOS3_ENTRY_LEN;
	e[0x07] = major;
	e[0x08] = minor;
	e[0x09] = docrev;
	e[0x0A] = 0x01; /* entry point revision */

	uint32_t table_len = (uint32_t)(dmi_len > 0xFFFFFFFFu ? 0xFFFFFFFFu : dmi_len);
	e[0x0C] = (uint8_t)(table_len & 0xFF);
	e[0x0D] = (uint8_t)((table_len >> 8) & 0xFF);
	e[0x0E] = (uint8_t)((table_len >> 16) & 0xFF);
	e[0x0F] = (uint8_t)((table_len >> 24) & 0xFF);
	e[0x10] = 0x00;
	e[0x11] = 0x10;

	uint8_t sum = 0;
	for (int i = 0; i < FUZZ_SMBIOS3_ENTRY_LEN; i++) sum = (uint8_t)(sum + e[i]);
	e[0x05] = (uint8_t)(-sum);

	*out_len = FUZZ_SMBIOS3_ENTRY_LEN;
	return e;
}

/**
 * @brief Parses every implemented structure type out of an already loaded context.
 *
 * Each getter allocates its own result, so the context takes ownership and
 * lazybiosCleanup() releases everything.
 */
static inline void fuzz_parse_all_types(lazybiosCTX_t* ctx) {
	ctx->Type0 = lazybiosGetType0(ctx->DMIData);
	ctx->Type1 = lazybiosGetType1(ctx->DMIData);
	ctx->Type2 = lazybiosGetType2(ctx->DMIData);
	ctx->Type3 = lazybiosGetType3(ctx->DMIData);
	ctx->Type4 = lazybiosGetType4(ctx->DMIData);
	ctx->Type5 = lazybiosGetType5(ctx->DMIData);
	ctx->Type6 = lazybiosGetType6(ctx->DMIData);
	ctx->Type7 = lazybiosGetType7(ctx->DMIData);
	ctx->Type8 = lazybiosGetType8(ctx->DMIData);
	ctx->Type9 = lazybiosGetType9(ctx->DMIData);
	ctx->Type10 = lazybiosGetType10(ctx->DMIData);
	ctx->Type11 = lazybiosGetType11(ctx->DMIData);
	ctx->Type12 = lazybiosGetType12(ctx->DMIData);
	ctx->Type13 = lazybiosGetType13(ctx->DMIData);
	ctx->Type14 = lazybiosGetType14(ctx->DMIData);
	ctx->Type15 = lazybiosGetType15(ctx->DMIData);
	ctx->Type16 = lazybiosGetType16(ctx->DMIData);
	ctx->Type17 = lazybiosGetType17(ctx->DMIData);
	ctx->Type18 = lazybiosGetType18(ctx->DMIData);
	ctx->Type19 = lazybiosGetType19(ctx->DMIData);
	ctx->Type20 = lazybiosGetType20(ctx->DMIData);
	ctx->Type21 = lazybiosGetType21(ctx->DMIData);
	ctx->Type22 = lazybiosGetType22(ctx->DMIData);
	ctx->Type23 = lazybiosGetType23(ctx->DMIData);
	ctx->Type24 = lazybiosGetType24(ctx->DMIData);
	ctx->Type25 = lazybiosGetType25(ctx->DMIData);
	ctx->Type26 = lazybiosGetType26(ctx->DMIData);
	ctx->Type27 = lazybiosGetType27(ctx->DMIData);
	ctx->Type28 = lazybiosGetType28(ctx->DMIData);
	ctx->Type29 = lazybiosGetType29(ctx->DMIData);
	ctx->Type30 = lazybiosGetType30(ctx->DMIData);
	ctx->Type31 = lazybiosGetType31(ctx->DMIData);
	ctx->Type32 = lazybiosGetType32(ctx->DMIData);
	ctx->Type33 = lazybiosGetType33(ctx->DMIData);
	ctx->Type34 = lazybiosGetType34(ctx->DMIData);
	ctx->Type35 = lazybiosGetType35(ctx->DMIData);
	ctx->Type36 = lazybiosGetType36(ctx->DMIData);
	ctx->Type37 = lazybiosGetType37(ctx->DMIData);
	ctx->Type38 = lazybiosGetType38(ctx->DMIData);
	ctx->Type39 = lazybiosGetType39(ctx->DMIData);
	ctx->Type40 = lazybiosGetType40(ctx->DMIData);
	ctx->Type41 = lazybiosGetType41(ctx->DMIData);
	ctx->Type42 = lazybiosGetType42(ctx->DMIData);
	ctx->Type43 = lazybiosGetType43(ctx->DMIData);
	ctx->Type44 = lazybiosGetType44(ctx->DMIData);
	ctx->Type45 = lazybiosGetType45(ctx->DMIData);
	ctx->Type46 = lazybiosGetType46(ctx->DMIData);
	ctx->oem->hp->Type204 = lazybiosGetOemHpType204(ctx->DMIData);
	ctx->oem->dell->Type177 = lazybiosGetOemDellType177(ctx->DMIData);
	ctx->oem->dell->Type212 = lazybiosGetOemDellType212(ctx->DMIData);
	ctx->oem->dell->Type218 = lazybiosGetOemDellType218(ctx->DMIData);
}

#endif

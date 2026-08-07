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

#include "lazybios.h"

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
	ctx->Type0 = lazybiosGetType0(ctx->Type0, &ctx->type0_count, ctx->DMIData);
	ctx->Type1 = lazybiosGetType1(ctx->Type1, &ctx->type1_count, ctx->DMIData);
	ctx->Type2 = lazybiosGetType2(ctx->Type2, &ctx->type2_count, ctx->DMIData);
	ctx->Type3 = lazybiosGetType3(ctx->Type3, &ctx->type3_count, ctx->DMIData);
	ctx->Type4 = lazybiosGetType4(ctx->Type4, &ctx->type4_count, ctx->DMIData);
	ctx->Type5 = lazybiosGetType5(ctx->Type5, &ctx->type5_count, ctx->DMIData);
	ctx->Type6 = lazybiosGetType6(ctx->Type6, &ctx->type6_count, ctx->DMIData);
	ctx->Type7 = lazybiosGetType7(ctx->Type7, &ctx->type7_count, ctx->DMIData);
	ctx->Type8 = lazybiosGetType8(ctx->Type8, &ctx->type8_count, ctx->DMIData);
	ctx->Type9 = lazybiosGetType9(ctx->Type9, &ctx->type9_count, ctx->DMIData);
	ctx->Type10 = lazybiosGetType10(ctx->Type10, &ctx->type10_count, ctx->DMIData);
	ctx->Type11 = lazybiosGetType11(ctx->Type11, &ctx->type11_count, ctx->DMIData);
	ctx->Type12 = lazybiosGetType12(ctx->Type12, &ctx->type12_count, ctx->DMIData);
	ctx->Type13 = lazybiosGetType13(ctx->Type13, &ctx->type13_count, ctx->DMIData);
	ctx->Type14 = lazybiosGetType14(ctx->Type14, &ctx->type14_count, ctx->DMIData);
	ctx->Type15 = lazybiosGetType15(ctx->Type15, &ctx->type15_count, ctx->DMIData);
	ctx->Type16 = lazybiosGetType16(ctx->Type16, &ctx->type16_count, ctx->DMIData);
	ctx->Type17 = lazybiosGetType17(ctx->Type17, &ctx->type17_count, ctx->DMIData);
	ctx->Type18 = lazybiosGetType18(ctx->Type18, &ctx->type18_count, ctx->DMIData);
	ctx->Type19 = lazybiosGetType19(ctx->Type19, &ctx->type19_count, ctx->DMIData);
	ctx->Type20 = lazybiosGetType20(ctx->Type20, &ctx->type20_count, ctx->DMIData);
	ctx->Type21 = lazybiosGetType21(ctx->Type21, &ctx->type21_count, ctx->DMIData);
	ctx->Type22 = lazybiosGetType22(ctx->Type22, &ctx->type22_count, ctx->DMIData);
	ctx->Type23 = lazybiosGetType23(ctx->Type23, &ctx->type23_count, ctx->DMIData);
	ctx->Type24 = lazybiosGetType24(ctx->Type24, &ctx->type24_count, ctx->DMIData);
	ctx->Type25 = lazybiosGetType25(ctx->Type25, &ctx->type25_count, ctx->DMIData);
	ctx->Type26 = lazybiosGetType26(ctx->Type26, &ctx->type26_count, ctx->DMIData);
	ctx->Type27 = lazybiosGetType27(ctx->Type27, &ctx->type27_count, ctx->DMIData);
	ctx->Type28 = lazybiosGetType28(ctx->Type28, &ctx->type28_count, ctx->DMIData);
	ctx->Type29 = lazybiosGetType29(ctx->Type29, &ctx->type29_count, ctx->DMIData);
	ctx->Type30 = lazybiosGetType30(ctx->Type30, &ctx->type30_count, ctx->DMIData);
	ctx->Type31 = lazybiosGetType31(ctx->Type31, &ctx->type31_count, ctx->DMIData);
	ctx->Type32 = lazybiosGetType32(ctx->Type32, &ctx->type32_count, ctx->DMIData);
	ctx->Type33 = lazybiosGetType33(ctx->Type33, &ctx->type33_count, ctx->DMIData);
	ctx->Type34 = lazybiosGetType34(ctx->Type34, &ctx->type34_count, ctx->DMIData);
	ctx->Type35 = lazybiosGetType35(ctx->Type35, &ctx->type35_count, ctx->DMIData);
	ctx->Type36 = lazybiosGetType36(ctx->Type36, &ctx->type36_count, ctx->DMIData);
	ctx->Type37 = lazybiosGetType37(ctx->Type37, &ctx->type37_count, ctx->DMIData);
	ctx->Type38 = lazybiosGetType38(ctx->Type38, &ctx->type38_count, ctx->DMIData);
	ctx->Type39 = lazybiosGetType39(ctx->Type39, &ctx->type39_count, ctx->DMIData);
	ctx->Type40 = lazybiosGetType40(ctx->Type40, &ctx->type40_count, ctx->DMIData);
	ctx->Type41 = lazybiosGetType41(ctx->Type41, &ctx->type41_count, ctx->DMIData);
	ctx->Type42 = lazybiosGetType42(ctx->Type42, &ctx->type42_count, ctx->DMIData);
	ctx->Type43 = lazybiosGetType43(ctx->Type43, &ctx->type43_count, ctx->DMIData);
	ctx->Type44 = lazybiosGetType44(ctx->Type44, &ctx->type44_count, ctx->DMIData);
	ctx->Type45 = lazybiosGetType45(ctx->Type45, &ctx->type45_count, ctx->DMIData);
	ctx->Type46 = lazybiosGetType46(ctx->Type46, &ctx->type46_count, ctx->DMIData);
}

#endif

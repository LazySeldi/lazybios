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
 * @file semantic_test.c
 * @brief Deterministic checks for specification-derived values and API contracts.
 */

#include "lazybios_internal.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
	if (!(condition)) { \
		fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, #condition); \
		return -1; \
	} \
} while (0)

static void set_checksum(uint8_t* data, size_t begin, size_t end,
	size_t checksum_offset) {
	uint8_t sum = 0;
	data[checksum_offset] = 0;
	for (size_t i = begin; i < end; i++)
		sum = (uint8_t)(sum + data[i]);
	data[checksum_offset] = (uint8_t)(-sum);
}

static void make_entry3(uint8_t entry[SMBIOS3_ENTRY_POINT_LENGTH],
	uint8_t major, uint8_t minor, uint8_t docrev) {
	memset(entry, 0, SMBIOS3_ENTRY_POINT_LENGTH);
	memcpy(entry, SMBIOS3_ANCHOR, SMBIOS3_ANCHOR_SIZE);
	entry[SMBIOS3_LENGTH_OFFSET] = SMBIOS3_ENTRY_POINT_LENGTH;
	entry[SMBIOS3_MAJOR_OFFSET] = major;
	entry[SMBIOS3_MINOR_OFFSET] = minor;
	entry[SMBIOS3_DOCREV_OFFSET] = docrev;
	entry[SMBIOS3_REVISION_OFFSET] = 1;
	set_checksum(entry, 0, SMBIOS3_ENTRY_POINT_LENGTH,
		SMBIOS3_CHECKSUM_OFFSET);
}

static void make_entry2(uint8_t entry[SMBIOS2_ENTRY_POINT_LENGTH],
	uint8_t major, uint8_t minor) {
	memset(entry, 0, SMBIOS2_ENTRY_POINT_LENGTH);
	memcpy(entry, SMBIOS2_ANCHOR, SMBIOS2_ANCHOR_SIZE);
	entry[SMBIOS2_LENGTH_OFFSET] = SMBIOS2_ENTRY_POINT_LENGTH;
	entry[SMBIOS2_MAJOR_OFFSET] = major;
	entry[SMBIOS2_MINOR_OFFSET] = minor;
	memcpy(entry + SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET,
		SMBIOS2_INTERMEDIATE_ANCHOR, SMBIOS2_INTERMEDIATE_ANCHOR_SIZE);
	entry[SMBIOS2_BCD_REVISION_OFFSET] =
		(uint8_t)((major << 4) | (minor & 0x0F));
	set_checksum(entry, SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET,
		SMBIOS2_ENTRY_POINT_LENGTH,
		SMBIOS2_INTERMEDIATE_CHECKSUM_OFFSET);
	set_checksum(entry, 0, SMBIOS2_ENTRY_POINT_LENGTH,
		SMBIOS2_CHECKSUM_OFFSET);
}

static int test_entry_points(void) {
	uint8_t entry[SMBIOS3_ENTRY_POINT_LENGTH];
	make_entry3(entry, 3, 9, 0);

	lazybiosDMI_t dmi = {0};
	lazybiosCTX_t ctx = {0};
	ctx.DMIData = &dmi;

	CHECK(lazybiosParseEntry(&ctx, entry, sizeof(entry)) == 0);
	CHECK(dmi.entry_tag == SMBIOS_VER_3X);
	CHECK(dmi.entry_union.v3->major_version == 3);
	CHECK(dmi.entry_union.v3->minor_version == 9);
	CHECK(lazybiosIsVersionPlus(&dmi, 3, 9));
	CHECK(!lazybiosIsVersionPlus(&dmi, 4, 0));

	CHECK(lazybiosParseEntry(NULL, entry, sizeof(entry)) == -1);
	CHECK(lazybiosParseEntry(&ctx, NULL, sizeof(entry)) == -1);
	CHECK(lazybiosParseEntry(&ctx, entry, SMBIOS3_ENTRY_POINT_LENGTH - 1) == -1);
	entry[SMBIOS3_LENGTH_OFFSET] = SMBIOS3_ENTRY_POINT_LENGTH - 1;
	CHECK(lazybiosParseEntry(&ctx, entry, sizeof(entry)) == -1);

	uint8_t entry2[SMBIOS2_ENTRY_POINT_LENGTH] = {0};
	memcpy(entry2, SMBIOS2_ANCHOR, SMBIOS2_ANCHOR_SIZE);
	entry2[SMBIOS2_LENGTH_OFFSET] = SMBIOS2_ENTRY_POINT_LENGTH;
	entry2[SMBIOS2_MAJOR_OFFSET] = 2;
	entry2[SMBIOS2_MINOR_OFFSET] = 8;
	CHECK(lazybiosParseEntry(&ctx, entry2, sizeof(entry2)) == -1);
	memcpy(entry2 + SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET,
		SMBIOS2_INTERMEDIATE_ANCHOR, SMBIOS2_INTERMEDIATE_ANCHOR_SIZE);
	CHECK(lazybiosParseEntry(&ctx, entry2, sizeof(entry2)) == 0);
	CHECK(lazybiosCleanup(NULL) == -1);
	return 0;
}

static int test_traversal_helpers(void) {
	const uint8_t structure[] = {
		11, 4, 0x34, 0x12,
		'o', 'n', 'e', 0,
		't', 'w', 'o', 0,
		0
	};
	const uint8_t* end = structure + sizeof(structure);

	char* first = DMIString(structure, 4, 1, end);
	char* second = DMIString(structure, 4, 2, end);
	CHECK(first && strcmp(first, "one") == 0);
	CHECK(second && strcmp(second, "two") == 0);
	free(first);
	free(second);

	CHECK(DMIString(structure, 4, 0, end) == NULL);
	CHECK(DMIString(structure, 4, 3, end) == NULL);
	CHECK(DMINext(structure, end) == end);
	CHECK(DMINext(NULL, end) == end);
	CHECK(DMINext(end, end) == end);

	const uint8_t unterminated[] = {11, 4, 0, 0, 'x'};
	CHECK(DMIString(unterminated, 4, 1,
		unterminated + sizeof(unterminated)) == NULL);
	CHECK(DMINext(unterminated, unterminated + sizeof(unterminated)) ==
		unterminated + sizeof(unterminated));
	return 0;
}

static int test_type28_signed_temperature(void) {
	uint8_t entry[SMBIOS3_ENTRY_POINT_LENGTH];
	make_entry3(entry, 3, 9, 0);

	/*
	 * SMBIOS 3.9 Type 28:
	 * maximum 123.4 C, minimum -25.0 C, resolution 0.125 C,
	 * tolerance 1.5 C, accuracy 2.50%, and nominal value 22.0 C.
	 */
	uint8_t table[] = {
		28, 0x16, 0x34, 0x12,
		1, 0x67,
		0xD2, 0x04,
		0x06, 0xFF,
		0x7D, 0x00,
		0x0F, 0x00,
		0xFA, 0x00,
		0x44, 0x33, 0x22, 0x11,
		0xDC, 0x00,
		'C', 'P', 'U', ' ', 'p', 'r', 'o', 'b', 'e', 0, 0
	};

	lazybiosDMI_t dmi = {
		.dmi_data = table,
		.dmi_len = sizeof(table)
	};
	lazybiosCTX_t ctx = {.DMIData = &dmi};
	CHECK(lazybiosParseEntry(&ctx, entry, sizeof(entry)) == 0);

	size_t count = 0;
	lazybiosType28_t* probes = lazybiosGetType28(NULL, &count, &dmi);
	CHECK(probes != NULL);
	CHECK(count == 1);
	CHECK(probes[0].description && strcmp(probes[0].description, "CPU probe") == 0);
	CHECK(probes[0].maximum_value == 1234);
	CHECK(probes[0].minimum_value == -250);
	CHECK(probes[0].resolution == 125);
	CHECK(probes[0].tolerance == 15);
	CHECK(probes[0].accuracy == 250);
	CHECK(probes[0].oem_defined == UINT32_C(0x11223344));
	CHECK(probes[0].nominal_value == 220);
	CHECK(LAZYBIOS_FIELD_STATUS(&probes[0], minimum_value) == LAZYBIOS_FIELD_PRESENT);
	CHECK(strcmp(lazybiosType28LocationStr(probes[0].location_and_status), "Motherboard") == 0);
	CHECK(strcmp(lazybiosType28StatusStr(probes[0].location_and_status), "OK") == 0);
	lazybiosFreeType28(probes, count);

	uint8_t truncated[] = {
		28, 8, 0, 0, 0, 0, 0x34, 0x12, 0, 0
	};
	dmi.dmi_data = truncated;
	dmi.dmi_len = sizeof(truncated);
	count = 0;
	probes = lazybiosGetType28(NULL, &count, &dmi);
	CHECK(probes != NULL);
	CHECK(count == 1);
	CHECK(probes[0].maximum_value == 0x1234);
	CHECK(LAZYBIOS_FIELD_STATUS(&probes[0], maximum_value) == LAZYBIOS_FIELD_PRESENT);
	CHECK(LAZYBIOS_FIELD_STATUS(&probes[0], minimum_value) == LAZYBIOS_FIELD_ABSENT);
	lazybiosFreeType28(probes, count);
	return 0;
}

static int test_type0_type1_counts(void) {
	uint8_t entry[SMBIOS3_ENTRY_POINT_LENGTH];
	make_entry3(entry, 3, 9, 0);

	/* Two Type 0 records followed by two Type 1 records. */
	uint8_t table[128] = {0};
	size_t offset = 0;
	for (size_t i = 0; i < 2; i++) {
		table[offset] = SMBIOS_TYPE_BIOS;
		table[offset + 1] = 0x1A;
		table[offset + 0x09] = 1;
		table[offset + 0x0A] = 0;
		offset += 0x1A + 2;
	}
	for (size_t i = 0; i < 2; i++) {
		table[offset] = SMBIOS_TYPE_SYSTEM;
		table[offset + 1] = 0x1B;
		table[offset + 0x18] = 0;
		offset += 0x1B + 2;
	}

	lazybiosDMI_t dmi = {
		.dmi_data = table,
		.dmi_len = offset
	};
	lazybiosCTX_t ctx = {.DMIData = &dmi};
	CHECK(lazybiosParseEntry(&ctx, entry, sizeof(entry)) == 0);

	lazybiosType0_t* type0 = lazybiosGetType0(NULL, &ctx.type0_count, &dmi);
	lazybiosType1_t* type1 = lazybiosGetType1(NULL, &ctx.type1_count, &dmi);
	CHECK(type0 != NULL);
	CHECK(type1 != NULL);
	CHECK(ctx.type0_count == 2);
	CHECK(ctx.type1_count == 2);
	lazybiosFreeType0(type0, ctx.type0_count);
	lazybiosFreeType1(type1, ctx.type1_count);
	return 0;
}

static int test_numeric_decoders(void) {
	CHECK(lazybiosType7CacheU16(0x0001) == 1);
	CHECK(lazybiosType7CacheU16(0x8001) == 64);
	CHECK(lazybiosType7CacheU32(UINT32_C(0x80000002)) == 128);
	CHECK(lazybiosType16MaximumCapacityBytes(2, 99) == 2048);
	CHECK(lazybiosType16MaximumCapacityBytes(UINT32_C(0x80000000), 99) == 99);
	CHECK(lazybiosType19StartingAddressBytes(2, 99) == 2048);
	CHECK(lazybiosType19EndingAddressBytes(2, 99) == 3071);
	CHECK(lazybiosType20StartingAddressBytes(UINT32_C(0xFFFFFFFF), 1234) == 1234);
	CHECK(lazybiosType20EndingAddressBytes(UINT32_C(0xFFFFFFFF), 5678) == 5678);
	CHECK(lazybiosType22DesignCapacityMWh(100, 3) == 300);
	CHECK(lazybiosType38BaseAddressValue(UINT64_C(0x1001), 0x10) == UINT64_C(0x1001));

	lazybiosType25_t controls = {0};
	controls.next_scheduled_power_on_month = 0x12;
	controls.next_scheduled_power_on_day = 0x31;
	controls.next_scheduled_power_on_hour = 0x23;
	controls.next_scheduled_power_on_minute = 0x59;
	controls.next_scheduled_power_on_second = 0x58;
	controls.field_status.next_scheduled_power_on_month = LAZYBIOS_FIELD_PRESENT;
	controls.field_status.next_scheduled_power_on_day = LAZYBIOS_FIELD_PRESENT;
	controls.field_status.next_scheduled_power_on_hour = LAZYBIOS_FIELD_PRESENT;
	controls.field_status.next_scheduled_power_on_minute = LAZYBIOS_FIELD_PRESENT;
	controls.field_status.next_scheduled_power_on_second = LAZYBIOS_FIELD_PRESENT;
	char decoded[32];
	lazybiosType25NextScheduledPowerOnStr(&controls, decoded, sizeof(decoded));
	CHECK(strcmp(decoded, "12-31 23:59:58") == 0);
	return 0;
}

static void put_u16_le(uint8_t out[2], uint16_t value) {
	out[0] = (uint8_t)value;
	out[1] = (uint8_t)(value >> 8);
}

static void put_u32_le(uint8_t out[4], uint32_t value) {
	out[0] = (uint8_t)value;
	out[1] = (uint8_t)(value >> 8);
	out[2] = (uint8_t)(value >> 16);
	out[3] = (uint8_t)(value >> 24);
}

static void put_u64_le(uint8_t out[8], uint64_t value) {
	for (size_t i = 0; i < 8; i++)
		out[i] = (uint8_t)(value >> (i * 8));
}

static int test_backend_transformations(void) {
	uint8_t raw3[] = {
		0, 3, 9, 0,
		0, 0, 0, 0,
		127, 4, 0, 0, 0, 0
	};
	put_u32_le(raw3 + 4, (uint32_t)(sizeof(raw3) - 8));

	lazybiosCTX_t* ctx = lazybiosCTXNew();
	CHECK(ctx != NULL);
	CHECK(lazybiosLoadWindowsRawSMBIOSData(ctx, raw3, sizeof(raw3)) == 0);
	CHECK(ctx->DMIData->entry_tag == SMBIOS_VER_3X);
	CHECK(ctx->DMIData->entry_union.v3->major_version == 3);
	CHECK(ctx->DMIData->dmi_len == sizeof(raw3) - 8);
	raw3[8] = 0;
	CHECK(ctx->DMIData->dmi_data[0] == 127);
	CHECK(lazybiosLoadWindowsRawSMBIOSData(ctx, raw3, sizeof(raw3)) == -1);
	CHECK(lazybiosCleanup(ctx) == 0);

	uint8_t raw2[] = {
		0, 2, 8, 0,
		0, 0, 0, 0,
		127, 4, 0, 0, 0, 0
	};
	put_u32_le(raw2 + 4, (uint32_t)(sizeof(raw2) - 8));
	ctx = lazybiosCTXNew();
	CHECK(ctx != NULL);
	CHECK(lazybiosLoadWindowsRawSMBIOSData(ctx, raw2, sizeof(raw2)) == 0);
	CHECK(ctx->DMIData->entry_tag == SMBIOS_VER_2X);
	CHECK(ctx->DMIData->entry_union.v2->major_version == 2);
	CHECK(lazybiosCleanup(ctx) == 0);

	for (size_t size = 0; size < 8; size++) {
		ctx = lazybiosCTXNew();
		CHECK(ctx != NULL);
		CHECK(lazybiosLoadWindowsRawSMBIOSData(ctx, raw3, size) == -1);
		CHECK(lazybiosCleanup(ctx) == 0);
	}

	uint8_t image[160] = {0};
	uint8_t entry[SMBIOS3_ENTRY_POINT_LENGTH];
	make_entry3(entry, 3, 9, 0);
	size_t table_entry_len = 0;
	size_t table_len = 0;
	uint64_t table_address = 0;
	CHECK(lazybiosGetSMBIOSTableLocation(entry, sizeof(entry),
		&table_entry_len, &table_address, &table_len) == -1);
	put_u32_le(entry + SMBIOS3_TABLE_MAX_SIZE_OFFSET, UINT32_C(0x123456));
	put_u64_le(entry + SMBIOS3_TABLE_ADDRESS_OFFSET,
		UINT64_C(0x1122334455667788));
	set_checksum(entry, 0, sizeof(entry), SMBIOS3_CHECKSUM_OFFSET);
	CHECK(lazybiosGetSMBIOSTableLocation(entry, sizeof(entry),
		&table_entry_len, &table_address, &table_len) == 0);
	CHECK(table_entry_len == SMBIOS3_ENTRY_POINT_LENGTH);
	CHECK(table_address == UINT64_C(0x1122334455667788));
	CHECK(table_len == 0x123456);
	memcpy(image + 16, entry, sizeof(entry));
	size_t offset = 0;
	size_t length = 0;
	CHECK(lazybiosFindSMBIOSEntryPoint(
		image, sizeof(image), &offset, &length) == 0);
	CHECK(offset == 16);
	CHECK(length == SMBIOS3_ENTRY_POINT_LENGTH);

	/*
	 * An aligned anchor is not sufficient: an OpenBSD VM exposed an aligned
	 * "_SM3_" byte sequence whose declared length was 0x65 before the real
	 * SMBIOS 2.x entry point.
	 */
	memset(image, 0, sizeof(image));
	memcpy(image, SMBIOS3_ANCHOR, SMBIOS3_ANCHOR_SIZE);
	image[SMBIOS3_LENGTH_OFFSET] = 0x65;
	image[SMBIOS3_MAJOR_OFFSET] = 3;
	image[SMBIOS3_REVISION_OFFSET] = 1;
	CHECK(lazybiosFindSMBIOSEntryPoint(
		image, sizeof(image), &offset, &length) == -1);

	uint8_t entry2[SMBIOS2_ENTRY_POINT_LENGTH];
	make_entry2(entry2, 2, 8);
	put_u16_le(entry2 + SMBIOS2_TABLE_LENGTH_OFFSET, UINT16_C(0x3456));
	put_u32_le(entry2 + SMBIOS2_TABLE_ADDRESS_OFFSET,
		UINT32_C(0x89ABCDEF));
	set_checksum(entry2, SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET,
		sizeof(entry2), SMBIOS2_INTERMEDIATE_CHECKSUM_OFFSET);
	set_checksum(entry2, 0, sizeof(entry2), SMBIOS2_CHECKSUM_OFFSET);
	CHECK(lazybiosGetSMBIOSTableLocation(entry2, sizeof(entry2),
		&table_entry_len, &table_address, &table_len) == 0);
	CHECK(table_entry_len == SMBIOS2_ENTRY_POINT_LENGTH);
	CHECK(table_address == UINT32_C(0x89ABCDEF));
	CHECK(table_len == 0x3456);
	entry2[SMBIOS2_INTERMEDIATE_CHECKSUM_OFFSET]++;
	CHECK(lazybiosGetSMBIOSTableLocation(entry2, sizeof(entry2),
		&table_entry_len, &table_address, &table_len) == -1);
	entry2[SMBIOS2_INTERMEDIATE_CHECKSUM_OFFSET]--;
	memcpy(image + 112, entry2, sizeof(entry2));
	CHECK(lazybiosFindSMBIOSEntryPoint(
		image, sizeof(image), &offset, &length) == 0);
	CHECK(offset == 112);
	CHECK(length == SMBIOS2_ENTRY_POINT_LENGTH);

	memset(image, 0, sizeof(image));
	memcpy(image + sizeof(image) - SMBIOS3_ANCHOR_SIZE,
		SMBIOS3_ANCHOR, SMBIOS3_ANCHOR_SIZE);
	CHECK(lazybiosFindSMBIOSEntryPoint(
		image, sizeof(image), &offset, &length) == -1);

	memset(image, 0, sizeof(image));
	memcpy(image + 144, SMBIOS3_ANCHOR, SMBIOS3_ANCHOR_SIZE);
	CHECK(lazybiosFindSMBIOSEntryPoint(
		image, sizeof(image), &offset, &length) == -1);
	return 0;
}

static int test_single_file_layouts(void) {
	const uint8_t table[] = {
		127, 4, 0x34, 0x12, 0, 0
	};
	size_t entry_len;
	size_t table_offset;
	size_t table_len;

	uint8_t padded2[32 + sizeof(table)] = {0};
	make_entry2(padded2, 2, 8);
	put_u16_le(padded2 + SMBIOS2_TABLE_LENGTH_OFFSET,
		(uint16_t)sizeof(table));
	put_u32_le(padded2 + SMBIOS2_TABLE_ADDRESS_OFFSET, UINT32_C(0x20));
	set_checksum(padded2, SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET,
		SMBIOS2_ENTRY_POINT_LENGTH,
		SMBIOS2_INTERMEDIATE_CHECKSUM_OFFSET);
	set_checksum(padded2, 0, SMBIOS2_ENTRY_POINT_LENGTH,
		SMBIOS2_CHECKSUM_OFFSET);
	padded2[SMBIOS2_ENTRY_POINT_LENGTH] = 0xA5;
	memcpy(padded2 + 32, table, sizeof(table));

	CHECK(lazybiosGetSingleFileLayout(padded2,
		SMBIOS2_ENTRY_POINT_LENGTH, sizeof(padded2), &entry_len,
		&table_offset, &table_len) == 0);
	CHECK(entry_len == SMBIOS2_ENTRY_POINT_LENGTH);
	CHECK(table_offset == 32);
	CHECK(table_len == sizeof(table));

	lazybiosCTX_t* ctx = lazybiosCTXNew();
	CHECK(ctx != NULL);
	CHECK(lazybiosLoadRawBuffers(ctx, padded2, entry_len,
		padded2 + table_offset, table_len) == 0);
	CHECK(ctx->DMIData->dmi_len == sizeof(table));
	CHECK(ctx->DMIData->dmi_data[0] == SMBIOS_TYPE_END);
	CHECK(lazybiosCleanup(ctx) == 0);

	uint8_t tight2[SMBIOS2_ENTRY_POINT_LENGTH + sizeof(table)] = {0};
	make_entry2(tight2, 2, 8);
	put_u16_le(tight2 + SMBIOS2_TABLE_LENGTH_OFFSET,
		(uint16_t)sizeof(table));
	put_u32_le(tight2 + SMBIOS2_TABLE_ADDRESS_OFFSET,
		UINT32_C(0x000F1230));
	set_checksum(tight2, SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET,
		SMBIOS2_ENTRY_POINT_LENGTH,
		SMBIOS2_INTERMEDIATE_CHECKSUM_OFFSET);
	set_checksum(tight2, 0, SMBIOS2_ENTRY_POINT_LENGTH,
		SMBIOS2_CHECKSUM_OFFSET);
	memcpy(tight2 + SMBIOS2_ENTRY_POINT_LENGTH, table, sizeof(table));

	CHECK(lazybiosGetSingleFileLayout(tight2,
		SMBIOS2_ENTRY_POINT_LENGTH, sizeof(tight2), &entry_len,
		&table_offset, &table_len) == 0);
	CHECK(table_offset == SMBIOS2_ENTRY_POINT_LENGTH);
	CHECK(table_len == sizeof(table));

	uint8_t padded3[32 + sizeof(table)] = {0};
	make_entry3(padded3, 3, 9, 0);
	put_u32_le(padded3 + SMBIOS3_TABLE_MAX_SIZE_OFFSET,
		(uint32_t)sizeof(table));
	put_u64_le(padded3 + SMBIOS3_TABLE_ADDRESS_OFFSET, UINT64_C(0x20));
	set_checksum(padded3, 0, SMBIOS3_ENTRY_POINT_LENGTH,
		SMBIOS3_CHECKSUM_OFFSET);
	memcpy(padded3 + 32, table, sizeof(table));

	CHECK(lazybiosGetSingleFileLayout(padded3,
		SMBIOS3_ENTRY_POINT_LENGTH, sizeof(padded3), &entry_len,
		&table_offset, &table_len) == 0);
	CHECK(entry_len == SMBIOS3_ENTRY_POINT_LENGTH);
	CHECK(table_offset == 32);
	CHECK(table_len == sizeof(table));
	return 0;
}

static int test_backend_enum_values(void) {
	lazybiosCTX_t* ctx;

	CHECK(LAZYBIOS_BACKEND_HAIKU == 8);
	CHECK(LAZYBIOS_BACKEND_BEOS == 9);
	CHECK(LAZYBIOS_BACKEND_GENERIC == 10);
	CHECK(LAZYBIOS_BACKEND_UNKNOWN == 11);

	ctx = lazybiosCTXNew();
	CHECK(ctx != NULL);
	#if defined(OS_LINUX)
	CHECK(ctx->backend == LAZYBIOS_BACKEND_LINUX);
	#elif defined(OS_WINDOWS)
	CHECK(ctx->backend == LAZYBIOS_BACKEND_WINDOWS);
	#elif defined(OS_MACOS)
	CHECK(ctx->backend == LAZYBIOS_BACKEND_MACOS);
	#elif defined(OS_OPENBSD)
	CHECK(ctx->backend == LAZYBIOS_BACKEND_OPENBSD);
	#elif defined(OS_FREEBSD)
	CHECK(ctx->backend == LAZYBIOS_BACKEND_FREEBSD);
	#elif defined(OS_NETBSD)
	CHECK(ctx->backend == LAZYBIOS_BACKEND_NETBSD);
	#elif defined(OS_SUNOS)
	CHECK(ctx->backend == LAZYBIOS_BACKEND_SUNOS);
	#elif defined(OS_DRAGONFLY)
	CHECK(ctx->backend == LAZYBIOS_BACKEND_DRAGONFLY);
	#elif defined(OS_HAIKU)
	CHECK(ctx->backend == LAZYBIOS_BACKEND_HAIKU);
	#elif defined(OS_BEOS)
	CHECK(ctx->backend == LAZYBIOS_BACKEND_BEOS);
	#elif defined(OS_GENERIC)
	CHECK(ctx->backend == LAZYBIOS_BACKEND_GENERIC);
	#else
	CHECK(ctx->backend == LAZYBIOS_BACKEND_UNKNOWN);
	#endif
	CHECK(lazybiosCleanup(ctx) == 0);
	return 0;
}

static int test_null_free_contracts(void) {
	lazybiosFreeType0(NULL, 4);
	lazybiosFreeType1(NULL, 4);
	lazybiosFreeType2(NULL, 4);
	lazybiosFreeType3(NULL, 4);
	lazybiosFreeType4(NULL, 4);
	lazybiosFreeType5(NULL, 4);
	lazybiosFreeType6(NULL, 4);
	lazybiosFreeType7(NULL, 4);
	lazybiosFreeType8(NULL, 4);
	lazybiosFreeType9(NULL, 4);
	lazybiosFreeType10(NULL, 4);
	lazybiosFreeType11(NULL, 4);
	lazybiosFreeType12(NULL, 4);
	lazybiosFreeType13(NULL, 4);
	lazybiosFreeType14(NULL, 4);
	lazybiosFreeType15(NULL, 4);
	lazybiosFreeType16(NULL, 4);
	lazybiosFreeType17(NULL, 4);
	lazybiosFreeType18(NULL, 4);
	lazybiosFreeType19(NULL, 4);
	lazybiosFreeType20(NULL, 4);
	lazybiosFreeType21(NULL, 4);
	lazybiosFreeType22(NULL, 4);
	lazybiosFreeType23(NULL, 4);
	lazybiosFreeType24(NULL, 4);
	lazybiosFreeType25(NULL, 4);
	lazybiosFreeType26(NULL, 4);
	lazybiosFreeType27(NULL, 4);
	lazybiosFreeType28(NULL, 4);
	lazybiosFreeType29(NULL, 4);
	lazybiosFreeType30(NULL, 4);
	lazybiosFreeType31(NULL, 4);
	lazybiosFreeType32(NULL, 4);
	lazybiosFreeType33(NULL, 4);
	lazybiosFreeType34(NULL, 4);
	lazybiosFreeType35(NULL, 4);
	lazybiosFreeType36(NULL, 4);
	lazybiosFreeType37(NULL, 4);
	lazybiosFreeType38(NULL, 4);
	lazybiosFreeType39(NULL, 4);
	lazybiosFreeType40(NULL, 4);
	lazybiosFreeType41(NULL, 4);
	lazybiosFreeType42(NULL, 4);
	lazybiosFreeType43(NULL, 4);
	lazybiosFreeType44(NULL, 4);
	lazybiosFreeType45(NULL, 4);
	lazybiosFreeType46(NULL, 4);
	return 0;
}

int main(void) {
	if (test_entry_points() != 0 ||
		test_traversal_helpers() != 0 ||
		test_type28_signed_temperature() != 0 ||
		test_type0_type1_counts() != 0 ||
		test_numeric_decoders() != 0 ||
		test_backend_transformations() != 0 ||
		test_backend_enum_values() != 0 ||
		test_single_file_layouts() != 0 ||
		test_null_free_contracts() != 0)
		return EXIT_FAILURE;

	puts("semantic tests passed");
	return EXIT_SUCCESS;
}

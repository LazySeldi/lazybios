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
 * @file lazybios.c
 * @brief Implements context management, data loading, and core SMBIOS parsing.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char lazybiosVersion[] = "2.0.0";

int lazybiosSingleFile(lazybiosCTX_t* ctx, const char* bin_path) {
	if (!ctx || !ctx->DMIData || !bin_path) return -1;

	FILE* binf = fopen(bin_path, "rb");
	if (!binf) {
		lb_log("failed to open %s: %s", bin_path, strerror(errno));
		return -1;
	}

	uint8_t header[5];
	size_t n = fread(header, 1, 5, binf);
	if (n != 5) {
		lb_log("Failed to read SMBIOS header");
		fclose(binf);
		return -1;
	}

	size_t entry_size = 0;
	if (header[3] == '3') {
		entry_size = 24; // for SMBIOS 3.x.x the length is 24 bytes
	} else if (header[3] == '_') {
		entry_size = 31; // for SMBIOS 2.x the length is 31 bytes
	} else {
		entry_size = SIZE_MAX; // our fallback
	}

	if (entry_size == SIZE_MAX) {
		lb_log("Couldn't read SMBIOS anchor!");
		lb_dbg("Header: %02X %02X %02X %02X %02X", header[0], header[1], header[2], header[3], header[4]);
		fclose(binf);
		return -1;
	}

	uint8_t entry_buf[31]; // max possible size is SMBIOS 2.x
	memcpy(entry_buf, header, 5); // first 5 bytes already read
	size_t remaining = entry_size - 5;
	if (remaining > 0) {
		size_t got = fread(entry_buf + 5, 1, remaining, binf);
		if (got != remaining) {
			lb_log("Failed to read full SMBIOS entry point");
			fclose(binf);
			return -1;
		}
	}

	if (fseek(binf, 0, SEEK_END) != 0) {
		lb_log("Failed to seek end of file");
		fclose(binf);
		return -1;
	}
	long file_len = ftell(binf);
	if (file_len <= 0) {
		lb_log("Invalid file length %ld", file_len);
		fclose(binf);
		return -1;
	}

	size_t parsed_entry_len;
	size_t table_offset;
	size_t table_len;
	if (lazybiosGetSingleFileLayout(entry_buf, entry_size,
			(size_t)file_len, &parsed_entry_len, &table_offset,
			&table_len) != 0 ||
		table_offset > (size_t)LONG_MAX) {
		lb_log("Invalid SMBIOS single-file layout");
		fclose(binf);
		return -1;
	}

	if (fseek(binf, (long)table_offset, SEEK_SET) != 0) {
		lb_log("Failed to seek to DMI data start");
		fclose(binf);
		return -1;
	}

	uint8_t* table_data = malloc(table_len);
	if (!table_data) {
		lb_log("Failed to allocate DMI buffer (%zu bytes)", table_len);
		fclose(binf);
		return -1;
	}

	size_t got = fread(table_data, 1, table_len, binf);
	fclose(binf);

	if (got != table_len) {
		lb_log("Short read of DMI data (%zu of %zu bytes)", got, table_len);
		free(table_data);
		return -1;
	}

	int result = lazybiosLoadRawBuffers(ctx, entry_buf, parsed_entry_len, table_data, table_len);
	free(table_data);
	return result;
}

int lazybiosFile(lazybiosCTX_t* ctx, const char* entry_path, const char* dmi_path) {
	if (!ctx) return -1;

	FILE* entry = fopen(entry_path, "rb");
	if (!entry) {
		lb_log("Failed to open %s: %s", entry_path, strerror(errno));
		return -1;
	}

	FILE* dmi = fopen(dmi_path, "rb");
	if (!dmi) {
		lb_log("Failed to open %s: %s", dmi_path, strerror(errno));
		fclose(entry);
		return -1;
	}

	uint8_t entry_buf[64];
	size_t n = fread(entry_buf, 1, sizeof(entry_buf), entry);
	ctx->DMIData->entry_len = n;

	if (n < 20) {
		lb_log("Invalid SMBIOS entry point (%zu bytes)", n);
		fclose(entry);
		fclose(dmi);
		return -1;
	}
	ctx->DMIData->entry_data = malloc(ctx->DMIData->entry_len);
	if (!ctx->DMIData->entry_data) {
		lb_log("Failed to allocate memory for entry_data");
		fclose(entry);
		fclose(dmi);
		return -1;
	}
	memcpy(ctx->DMIData->entry_data, entry_buf, ctx->DMIData->entry_len);
	fclose(entry);

	if (fseek(dmi, 0, SEEK_END) != 0) {
		lb_log("Failed to seek in DMI table");
		fclose(dmi);
		return -1;
	}

	long len = ftell(dmi);
	if (len <= 0) {
		lb_log("Invalid or empty DMI table");
		lb_dbg("ftell returned %ld", len);
		fclose(dmi);
		return -1;
	}

	if (lazybiosParseEntry(ctx, ctx->DMIData->entry_data, ctx->DMIData->entry_len) != 0) {
		fclose(dmi);
		return -1;
	}

	ctx->DMIData->dmi_len = (size_t)len;
	rewind(dmi);

	ctx->DMIData->dmi_data = malloc(ctx->DMIData->dmi_len);
	if (!ctx->DMIData->dmi_data) {
		lb_log("Failed to allocate %zu bytes for DMI buffer", ctx->DMIData->dmi_len);
		fclose(dmi);
		return -1;
	}

	size_t got = fread(ctx->DMIData->dmi_data, 1, ctx->DMIData->dmi_len, dmi);
	fclose(dmi);

	if (got != ctx->DMIData->dmi_len) {
		lb_log("Short read in DMI table");
		free(ctx->DMIData->dmi_data);
		ctx->DMIData->dmi_data = NULL;
		return -1;
	}
	return 0;
}

lazybiosCTX_t* lazybiosCTXNew(void) {
	lazybiosCTX_t* ctx = calloc(1, sizeof(*ctx));
	if (!ctx) return NULL;

	ctx->DMIData = calloc(1, sizeof(*ctx->DMIData));
	if (!ctx->DMIData) {
		free(ctx);
		return NULL;
	}

	#if defined(OS_LINUX)
		ctx->backend = LAZYBIOS_BACKEND_LINUX;
	#elif defined(OS_WINDOWS)
		ctx->backend = LAZYBIOS_BACKEND_WINDOWS;
	#elif defined(OS_MACOS)
		ctx->backend = LAZYBIOS_BACKEND_MACOS;
	#elif defined(OS_OPENBSD)
		ctx->backend = LAZYBIOS_BACKEND_OPENBSD;
	#elif defined(OS_FREEBSD)
		ctx->backend = LAZYBIOS_BACKEND_FREEBSD;
	#elif defined(OS_NETBSD)
		ctx->backend = LAZYBIOS_BACKEND_NETBSD;
    #elif defined(OS_SUNOS)
        ctx->backend = LAZYBIOS_BACKEND_SUNOS;
    #elif defined(OS_DRAGONFLY)
        ctx->backend = LAZYBIOS_BACKEND_DRAGONFLY;
    #elif defined(OS_HAIKU)
        ctx->backend = LAZYBIOS_BACKEND_HAIKU;
    #elif defined(OS_BEOS)
        ctx->backend = LAZYBIOS_BACKEND_BEOS;
    #elif defined(OS_REACTOS)
        ctx->backend = LAZYBIOS_BACKEND_REACTOS;
	#elif defined(OS_GENERIC)
		ctx->backend = LAZYBIOS_BACKEND_GENERIC;
	#else
		ctx->backend = LAZYBIOS_BACKEND_UNKNOWN;
	#endif

	return ctx;
}


int lazybiosInit(lazybiosCTX_t* ctx) {
	if (!ctx) return -1;

	switch (ctx->backend) {
		case LAZYBIOS_BACKEND_LINUX:
			#if defined(OS_LINUX)
			return lazybiosLinux(ctx);
			#else
			lb_log("Linux backend is not available in this build!");
			return -1;
			#endif

		case LAZYBIOS_BACKEND_WINDOWS:
			#if defined(OS_WINDOWS)
			return lazybiosWindows(ctx);
			#else
			lb_log("Windows backend is not available in this build");
			return -1;
			#endif

		case LAZYBIOS_BACKEND_MACOS:
			#if defined(OS_MACOS)
			return lazybiosMacOS(ctx);
			#else
			lb_log("MacOS backend is not available in this build");
			return -1;
			#endif

		case LAZYBIOS_BACKEND_OPENBSD:
			#if defined(OS_OPENBSD)
			return lazybiosOpenBSD(ctx);
			#else
			lb_log("OpenBSD backend is not available in this build");
			return -1;
			#endif

		case LAZYBIOS_BACKEND_FREEBSD:
			#if defined(OS_FREEBSD)
			return lazybiosFreeBSD(ctx);
			#else
			lb_log("FreeBSD backend is not available in this build");
			return -1;
			#endif

		case LAZYBIOS_BACKEND_NETBSD:
			#if defined(OS_NETBSD)
			return lazybiosNetBSD(ctx);
			#else
			lb_log("NetBSD backend is not available in this build");
			return -1;
			#endif

	    case LAZYBIOS_BACKEND_SUNOS:
	        #if defined(OS_SUNOS)
	        return lazybiosSunOS(ctx);
	        #else
	        lb_log("SunOS backend is not available in this build");
	        return -1;
	        #endif

	    case LAZYBIOS_BACKEND_DRAGONFLY:
	        #if defined(OS_DRAGONFLY)
	        return lazybiosDragonFly(ctx);
	        #else
	        lb_log("DragonFly backend is not available in this build");
            return -1;
	        #endif

	    case LAZYBIOS_BACKEND_HAIKU:
	        #if defined(OS_HAIKU)
	        return lazybiosHaiku(ctx);
	        #else
	        lb_log("Haiku backend is not available in this build");
            return -1;
	        #endif

	    case LAZYBIOS_BACKEND_BEOS:
	        #if defined(OS_BEOS)
	        return lazybiosBeOS(ctx);
	        #else
	        lb_log("BeOS backend is not available in this build");
	        return -1;
            #endif

	    case LAZYBIOS_BACKEND_REACTOS:
	        #if defined(OS_REACTOS)
	        return lazybiosReactOS(ctx);
	        #else
	        lb_log("ReactOS backend is not available in this build");
	        return -1;
	        #endif
	        
		case LAZYBIOS_BACKEND_UNKNOWN:
			lb_log("No host backend was selected");
			return -1;

		case LAZYBIOS_BACKEND_GENERIC:
			#if defined(OS_GENERIC)
			return lazybiosGeneric(ctx);
			#else
			lb_log("Generic backend is not available in this build");
			return -1;
			#endif

		default:
			lb_log("No backend found for initialization!");
			return -1;
	}
}

const uint8_t* DMINext(const uint8_t* p, const uint8_t* end) {
	if (!p || !end || p > end || (size_t)(end - p) < SMBIOS_HEADER_SIZE) return end;

	uint8_t len = p[1];
	if (len < SMBIOS_HEADER_SIZE || (size_t)(end - p) <= len) return end;

	const uint8_t* next = p + len;

	// Skip string section safely
	while (next + 1 < end) {
		if (next[0] == 0 && next[1] == 0)
			break; // double null found
		next++;
	}

	// Skip double-null terminator
	if (next + 2 <= end) 
		next += 2;
	else
		next = end;

	return next;
}

const char* DMIString(const uint8_t* p, uint8_t length, uint8_t index, const uint8_t* end) {
	if (!p || !end || p > end || index == 0 || length < SMBIOS_HEADER_SIZE ||
		(size_t)(end - p) <= length)
		return NULL;

	// Point to the start of the unformatted string area
	const uint8_t* str = p + length;
	const uint8_t* strings_end = str;
	while (strings_end + 1 < end && (strings_end[0] != 0 || strings_end[1] != 0))
		strings_end++;
	if (strings_end + 1 >= end) return NULL;

	// Iterate until the selected string
	for (uint8_t i = 1; i < index; i++) {
		if (str >= strings_end || *str == 0) return NULL;
		while (str < strings_end && *str != 0)
			str++;
		if (str >= strings_end) return NULL;
		str++; // here we skip the null terminator
	}
	if (str >= strings_end || *str == 0) return NULL;

    return (const char*)str;
}

int lazybiosIsVersionPlus(const lazybiosDMI_t* DMIData, uint8_t required_major, uint8_t required_minor) {
	uint8_t major;
	uint8_t minor;

	if (!DMIData) return 0;

	if (DMIData->entry_tag == SMBIOS_VER_3X && DMIData->entry_union.v3) {
		major = DMIData->entry_union.v3->major_version;
		minor = DMIData->entry_union.v3->minor_version;
	} else if (DMIData->entry_tag == SMBIOS_VER_2X && DMIData->entry_union.v2) {
		major = DMIData->entry_union.v2->major_version;
		minor = DMIData->entry_union.v2->minor_version;
	} else {
		return 0;
	}

	return major > required_major || (major == required_major && minor >= required_minor);
}

static int lazybiosVerifyChecksum(const uint8_t* entry_buf, size_t len) {
	uint8_t sum = 0;
	for (size_t i = 0; i < len; i++)
		sum = (uint8_t)(sum + entry_buf[i]);
	return sum == 0;
}

int lazybiosInspectEntryPoint(const uint8_t* entry_data, size_t available, lazybiosEntryInspection* inspection) {
	if (!inspection) return -1;

	memset(inspection, 0, sizeof(*inspection));
	inspection->tag = SMBIOS_VER_UNKNOWN;

	if (!entry_data || available < SMBIOS2_ANCHOR_SIZE)
		return -1;

	if (available >= SMBIOS3_ANCHOR_SIZE &&
		memcmp(entry_data, SMBIOS3_ANCHOR, SMBIOS3_ANCHOR_SIZE) == 0) {
		inspection->tag = SMBIOS_VER_3X;
		inspection->length = SMBIOS3_ENTRY_POINT_LENGTH;
		inspection->intermediate_checksum_valid = 1;

		if (available < SMBIOS3_ENTRY_POINT_LENGTH)
			return -1;

		inspection->length = entry_data[SMBIOS3_LENGTH_OFFSET];
		if (inspection->length < SMBIOS3_ENTRY_POINT_LENGTH)
			return -1;
		if (available < inspection->length)
			return -1;

		inspection->checksum_valid =
			lazybiosVerifyChecksum(entry_data, inspection->length);
		return 0;
	}

	if (memcmp(entry_data, SMBIOS2_ANCHOR, SMBIOS2_ANCHOR_SIZE) == 0) {
		inspection->tag = SMBIOS_VER_2X;
		inspection->length = SMBIOS2_ENTRY_POINT_LENGTH;

		if (available < SMBIOS2_ENTRY_POINT_LENGTH)
			return -1;

		inspection->length = entry_data[SMBIOS2_LENGTH_OFFSET];
		if (inspection->length < SMBIOS2_ENTRY_POINT_LENGTH)
			return -1;
		if (available < inspection->length)
			return -1;
		if (memcmp(entry_data + SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET,
				SMBIOS2_INTERMEDIATE_ANCHOR,
				SMBIOS2_INTERMEDIATE_ANCHOR_SIZE) != 0)
			return -1;

		inspection->checksum_valid =
			lazybiosVerifyChecksum(entry_data, inspection->length);
		inspection->intermediate_checksum_valid = lazybiosVerifyChecksum(
			entry_data + SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET,
			SMBIOS2_ENTRY_POINT_LENGTH -
				SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET);
		return 0;
	}

	return -1;
}

int lazybiosParseEntry(lazybiosCTX_t* ctx, const uint8_t* entry_buf, size_t buf_len) {
	if (!ctx || !ctx->DMIData) return -1;

	lazybiosEntryInspection inspection;
	if (lazybiosInspectEntryPoint(
			entry_buf, buf_len, &inspection) != 0) {
		lb_log("Invalid SMBIOS entry point");
		return -1;
	}

	ctx->DMIData->entry_tag = inspection.tag;
	if (inspection.tag == SMBIOS_VER_3X) {
		if (!inspection.checksum_valid)
			lb_dbg("Warning: SMBIOS 3.x Entry Point Checksum failed! "
				"(Proceeding anyway)");
		ctx->DMIData->entry_union.v3 = (lazybiosSMBIOS3Entry*)entry_buf;
	} else {
		if (!inspection.checksum_valid)
			lb_dbg("Warning: SMBIOS 2.x Main Checksum failed! "
				"(Proceeding anyway)");
		if (!inspection.intermediate_checksum_valid)
			lb_dbg("Warning: SMBIOS 2.x Intermediate (_DMI_) "
				"Checksum failed! (Proceeding anyway)");
		ctx->DMIData->entry_union.v2 = (lazybiosSMBIOS2Entry*)entry_buf;
	}

	return 0;
}

size_t lazybiosCountStructsByType(const lazybiosDMI_t* DMIData, uint8_t target_type) {
	if (!DMIData || !DMIData->dmi_data) return 0;

	size_t count = 0;
	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	while (p + 4 < end) { // SMBIOS_HEADER_SIZE
		uint8_t type = p[0];
		if (type == SMBIOS_TYPE_END) break;

		if (type == target_type)
			count++;

		p = DMINext(p, end);
	}

	return count;
}

void lazybiosPrintSMVer(const lazybiosCTX_t* ctx) {
	if (!ctx) return;
	if (ctx->DMIData->entry_tag == SMBIOS_VER_3X) {
		printf("SMBIOS version %d.%d.%d\n", ctx->DMIData->entry_union.v3->major_version, ctx->DMIData->entry_union.v3->minor_version, ctx->DMIData->entry_union.v3->docrev);
	} else if (ctx->DMIData->entry_tag == SMBIOS_VER_2X) {
		printf("SMBIOS version %d.%d\n", ctx->DMIData->entry_union.v2->major_version, ctx->DMIData->entry_union.v2->minor_version);
	} else {
		printf("Couldn't find SMBIOS Version!\n");
	}
}

int lazybiosCleanup(lazybiosCTX_t* ctx) {
	if (!ctx) return -1;

	lazybiosFreeType0(ctx->Type0, ctx->type0_count);
	ctx->Type0 = NULL;
	ctx->type0_count = 0;

	lazybiosFreeType1(ctx->Type1, ctx->type1_count);
	ctx->Type1 = NULL;
	ctx->type1_count = 0;

    lazybiosFreeType2(ctx->Type2, ctx->type2_count);
    ctx->Type2 = NULL;
    ctx->type2_count = 0;

    lazybiosFreeType3(ctx->Type3, ctx->type3_count);
    ctx->Type3 = NULL;
    ctx->type3_count = 0;

    lazybiosFreeType4(ctx->Type4, ctx->type4_count);
    ctx->Type4 = NULL;
    ctx->type4_count = 0;

    lazybiosFreeType5(ctx->Type5, ctx->type5_count);
    ctx->Type5 = NULL;
    ctx->type5_count = 0;

    lazybiosFreeType6(ctx->Type6, ctx->type6_count);
    ctx->Type6 = NULL;
    ctx->type6_count = 0;

    lazybiosFreeType7(ctx->Type7, ctx->type7_count);
    ctx->Type7 = NULL;
    ctx->type7_count = 0;

    lazybiosFreeType8(ctx->Type8, ctx->type8_count);
    ctx->Type8 = NULL;
    ctx->type8_count = 0;

    lazybiosFreeType9(ctx->Type9, ctx->type9_count);
    ctx->Type9 = NULL;
    ctx->type9_count = 0;

    lazybiosFreeType10(ctx->Type10, ctx->type10_count);
    ctx->Type10 = NULL;
    ctx->type10_count = 0;

    lazybiosFreeType11(ctx->Type11, ctx->type11_count);
    ctx->Type11 = NULL;
    ctx->type11_count = 0;

    lazybiosFreeType12(ctx->Type12, ctx->type12_count);
    ctx->Type12 = NULL;
    ctx->type12_count = 0;

    lazybiosFreeType13(ctx->Type13, ctx->type13_count);
    ctx->Type13 = NULL;
    ctx->type13_count = 0;

    lazybiosFreeType14(ctx->Type14, ctx->type14_count);
    ctx->Type14 = NULL;
    ctx->type14_count = 0;

    lazybiosFreeType15(ctx->Type15, ctx->type15_count);
    ctx->Type15 = NULL;
    ctx->type15_count = 0;

    lazybiosFreeType16(ctx->Type16, ctx->type16_count);
    ctx->Type16 = NULL;
    ctx->type16_count = 0;

    lazybiosFreeType17(ctx->Type17, ctx->type17_count);
    ctx->Type17 = NULL;
    ctx->type17_count = 0;

    lazybiosFreeType18(ctx->Type18, ctx->type18_count);
    ctx->Type18 = NULL;
    ctx->type18_count = 0;

    lazybiosFreeType19(ctx->Type19, ctx->type19_count);
    ctx->Type19 = NULL;
    ctx->type19_count = 0;

    lazybiosFreeType20(ctx->Type20, ctx->type20_count);
    ctx->Type20 = NULL;
    ctx->type20_count = 0;

    lazybiosFreeType21(ctx->Type21, ctx->type21_count);
    ctx->Type21 = NULL;
    ctx->type21_count = 0;

    lazybiosFreeType22(ctx->Type22, ctx->type22_count);
    ctx->Type22 = NULL;
    ctx->type22_count = 0;

    lazybiosFreeType23(ctx->Type23, ctx->type23_count);
    ctx->Type23 = NULL;
    ctx->type23_count = 0;

    lazybiosFreeType24(ctx->Type24, ctx->type24_count);
    ctx->Type24 = NULL;
    ctx->type24_count = 0;

    lazybiosFreeType25(ctx->Type25, ctx->type25_count);
    ctx->Type25 = NULL;
    ctx->type25_count = 0;

    lazybiosFreeType26(ctx->Type26, ctx->type26_count);
    ctx->Type26 = NULL;
    ctx->type26_count = 0;

    lazybiosFreeType27(ctx->Type27, ctx->type27_count);
    ctx->Type27 = NULL;
    ctx->type27_count = 0;

    lazybiosFreeType28(ctx->Type28, ctx->type28_count);
    ctx->Type28 = NULL;
    ctx->type28_count = 0;

    lazybiosFreeType29(ctx->Type29, ctx->type29_count);
    ctx->Type29 = NULL;
    ctx->type29_count = 0;

    lazybiosFreeType30(ctx->Type30, ctx->type30_count);
    ctx->Type30 = NULL;
    ctx->type30_count = 0;

    lazybiosFreeType31(ctx->Type31, ctx->type31_count);
    ctx->Type31 = NULL;
    ctx->type31_count = 0;

    lazybiosFreeType32(ctx->Type32, ctx->type32_count);
    ctx->Type32 = NULL;
    ctx->type32_count = 0;

    lazybiosFreeType33(ctx->Type33, ctx->type33_count);
    ctx->Type33 = NULL;
    ctx->type33_count = 0;

    lazybiosFreeType34(ctx->Type34, ctx->type34_count);
    ctx->Type34 = NULL;
    ctx->type34_count = 0;

    lazybiosFreeType35(ctx->Type35, ctx->type35_count);
    ctx->Type35 = NULL;
    ctx->type35_count = 0;

    lazybiosFreeType36(ctx->Type36, ctx->type36_count);
    ctx->Type36 = NULL;
    ctx->type36_count = 0;

    lazybiosFreeType37(ctx->Type37, ctx->type37_count);
    ctx->Type37 = NULL;
    ctx->type37_count = 0;

    lazybiosFreeType38(ctx->Type38, ctx->type38_count);
    ctx->Type38 = NULL;
    ctx->type38_count = 0;

    lazybiosFreeType39(ctx->Type39, ctx->type39_count);
    ctx->Type39 = NULL;
    ctx->type39_count = 0;

    lazybiosFreeType40(ctx->Type40, ctx->type40_count);
    ctx->Type40 = NULL;
    ctx->type40_count = 0;

    lazybiosFreeType41(ctx->Type41, ctx->type41_count);
    ctx->Type41 = NULL;
    ctx->type41_count = 0;

    lazybiosFreeType42(ctx->Type42, ctx->type42_count);
    ctx->Type42 = NULL;
    ctx->type42_count = 0;

    lazybiosFreeType43(ctx->Type43, ctx->type43_count);
    ctx->Type43 = NULL;
    ctx->type43_count = 0;

    lazybiosFreeType44(ctx->Type44, ctx->type44_count);
    ctx->Type44 = NULL;
    ctx->type44_count = 0;

    lazybiosFreeType45(ctx->Type45, ctx->type45_count);
    ctx->Type45 = NULL;
    ctx->type45_count = 0;

    lazybiosFreeType46(ctx->Type46, ctx->type46_count);
    ctx->Type46 = NULL;
    ctx->type46_count = 0;

    lazybiosFreeOemHpType204(ctx->HpType204, ctx->hptype204_count);
    ctx->HpType204 = NULL;
    ctx->hptype204_count = 0;
    
    lazybiosFreeOemDellType177(ctx->DellType177, ctx->delltype177_count);
    ctx->DellType177 = NULL;
    ctx->delltype177_count = 0;
    
    lazybiosFreeOemDellType212(ctx->DellType212, ctx->delltype212_count);
    ctx->DellType212 = NULL;
    ctx->delltype212_count = 0;

	free(ctx->DMIData->dmi_data);
	free(ctx->DMIData->entry_data);
	free(ctx->DMIData);
	free(ctx);
	return 0;
}

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

//
// lazybios.c - Core library functions
//
#include "lazybios_internal.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * @brief Loads SMBIOS entry point and DMI data from one merged file.
 *
 * @param ctx Context that receives the loaded data.
 * @param bin_path Path to a file containing the entry point followed by the DMI table.
 * @return 0 on success, or -1 on failure.
 */
int lazybiosSingleFile(lazybiosCTX_t* ctx, const char* bin_path) {
	if (!ctx) return -1;

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

	ctx->DMIData->entry_len = entry_size;
	ctx->DMIData->entry_data = malloc(ctx->DMIData->entry_len);
	if (!ctx->DMIData->entry_data) {
		lb_log("Failed to allocate memory for entry_data");
		fclose(binf);
		return -1;
	}
	memcpy(ctx->DMIData->entry_data, entry_buf, ctx->DMIData->entry_len);

	if (lazybiosParseEntry(ctx, ctx->DMIData->entry_data, ctx->DMIData->entry_len) != 0) {
		fclose(binf);
		return -1;
	}

	if (fseek(binf, 0, SEEK_END) != 0) {
		lb_log("Failed to seek end of file");
		fclose(binf);
		return -1;
	}
	long file_len = ftell(binf);
	if (file_len <= 0 || ctx->DMIData->entry_len > (size_t)LONG_MAX || file_len < (long)ctx->DMIData->entry_len) {
		lb_log("Invalid file length %ld", file_len);
		fclose(binf);
		return -1;
	}

	ctx->DMIData->dmi_len = (size_t)(file_len - ctx->DMIData->entry_len);
	rewind(binf);
	if (fseek(binf, (long)ctx->DMIData->entry_len, SEEK_SET) != 0) {
		lb_log("Failed to seek to DMI data start");
		fclose(binf);
		return -1;
	}

	ctx->DMIData->dmi_data = malloc(ctx->DMIData->dmi_len);
	if (!ctx->DMIData->dmi_data) {
		lb_log("Failed to allocate DMI buffer (%zu bytes)", ctx->DMIData->dmi_len);
		fclose(binf);
		return -1;
	}

	size_t got = fread(ctx->DMIData->dmi_data, 1, ctx->DMIData->dmi_len, binf);
	fclose(binf);

	if (got != ctx->DMIData->dmi_len) {
		lb_log("Short read of DMI data (%zu of %zu bytes)", got, ctx->DMIData->dmi_len);
		free(ctx->DMIData->dmi_data);
		ctx->DMIData->dmi_data = NULL;
		return -1;
	}

	return 0;
}

/**
 * @brief Loads an SMBIOS entry point and DMI table from separate files.
 *
 * @param ctx Context that receives the loaded data.
 * @param entry_path Path to the raw SMBIOS entry point file.
 * @param dmi_path Path to the raw DMI structure table file.
 * @return 0 on success, or -1 on failure.
 */
int lazybiosFile(lazybiosCTX_t* ctx, const char* entry_path, const char* dmi_path) {
	if (!ctx) return -1;

	FILE* entry = fopen(entry_path, "rb");
	if (!entry) {
		lb_log("failed to open %s: %s", entry_path, strerror(errno));
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

// ===== Context Management =====
/**
 * @brief Allocates and initializes a lazybios context.
 *
 * The selected backend is derived from the target platform.
 *
 * @return Newly allocated context, or NULL if allocation fails.
 */
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
	#else
		ctx->backend = LAZYBIOS_BACKEND_UNKNOWN;
	#endif

	return ctx;
}


/**
 * @brief Loads SMBIOS data using the context's selected platform backend.
 *
 * @param ctx Context that receives the raw entry point and DMI table data.
 * @return 0 on success, or -1 on failure.
 */
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

		case LAZYBIOS_BACKEND_UNKNOWN:
			lb_log("Unknown backend %d", ctx->backend);
			return -1;

		default:
			lb_log("No backend found for initialization!");
			return -1;
	}
}

// ===== Core DMI Parsing Helpers =====

// DMINext finds the next structure in the DMI table
/**
 * @brief Locates the next SMBIOS structure in a DMI table.
 *
 * @param p Start of the current SMBIOS structure.
 * @param end One-past-the-end address of the DMI table buffer.
 * @return Pointer to the next structure, or end when no complete structure remains.
 */
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

// Retrieves a string from the DMI table
/**
 * @brief Copies a string from an SMBIOS structure's string-set.
 *
 * @param p Start of the SMBIOS structure.
 * @param length Length of the structure's formatted section.
 * @param index One-based index of the requested string.
 * @param end One-past-the-end address of the DMI table buffer.
 * @return Newly allocated string, or NULL if the string is unavailable or invalid.
 */
char* DMIString(const uint8_t* p, uint8_t length, uint8_t index, const uint8_t* end) {
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

	// Now str points to the target string
	const uint8_t* s = str;

	// Find null terminator safely
	while (s < strings_end && *s != 0)
		s++;

	// Length is safe
	size_t len = (size_t)(s - str);

	char* copy = malloc(len + 1);
	if (!copy) return NULL;

	memcpy(copy, str, len);
	copy[len] = '\0';
	return copy;
}

/**
 * @brief Tests whether the parsed SMBIOS version meets a minimum version.
 *
 * @param DMIData Raw DMI table container with a parsed SMBIOS entry point.
 * @param required_major Required SMBIOS major version.
 * @param required_minor Required SMBIOS minor version.
 * @return Nonzero when the parsed version is equal to or newer than the required version; otherwise zero.
 * @ingroup api_entry
 */
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

	return major > required_major ||
		(major == required_major && minor >= required_minor);
}

static int lazybiosVerifyChecksum(const uint8_t* entry_buf, size_t len) {
	uint8_t sum = 0;
	for (size_t i = 0; i < len; i++)
		sum = (uint8_t)(sum + entry_buf[i]);
	return sum == 0;
}

int lazybiosInspectEntryPoint(
	const uint8_t* entry_data, size_t available,
	lazybiosEntryInspection* inspection) {
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

/**
 * @brief Validates and identifies an SMBIOS entry point.
 *
 * @param ctx Context whose entry tag and tagged union are updated.
 * @param entry_buf Buffer containing an SMBIOS 2.x or 3.x entry point.
 * @param buf_len Length of entry_buf in bytes.
 * @return 0 on success, or -1 if the entry point is invalid.
 */
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

// Counts the number of structures of a given type
/**
 * @brief Counts SMBIOS structures having a specified type identifier.
 *
 * @param DMIData Raw DMI table container to inspect.
 * @param target_type SMBIOS structure type identifier to count.
 * @return Number of matching structures in the table.
 */
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

// Small Helper
/**
 * @brief Prints the parsed SMBIOS version to standard output.
 *
 * @param ctx Context containing a parsed SMBIOS entry point.
 */
void lazybiosPrintVer(const lazybiosCTX_t* ctx) {
	if (!ctx) return;
	if (ctx->DMIData->entry_tag == SMBIOS_VER_3X) {
		printf("SMBIOS version %d.%d.%d\n", ctx->DMIData->entry_union.v3->major_version, ctx->DMIData->entry_union.v3->minor_version, ctx->DMIData->entry_union.v3->docrev);
	} else if (ctx->DMIData->entry_tag == SMBIOS_VER_2X) {
		printf("SMBIOS version %d.%d\n", ctx->DMIData->entry_union.v2->major_version, ctx->DMIData->entry_union.v2->minor_version);
	} else {
		printf("Couldn't find SMBIOS Version!\n");
	}
}

/**
 * @brief Releases a context and all SMBIOS data owned by it.
 *
 * @param ctx Context to release.
 * @return 0 on success, or -1 if ctx is NULL.
 */
int lazybiosCleanup(lazybiosCTX_t* ctx) {
	if (!ctx) return -1;

	lazybiosFreeType0(ctx->Type0);
	ctx->Type0 = NULL;

	lazybiosFreeType1(ctx->Type1);
	ctx->Type1 = NULL;

	lazybiosFreeType2(ctx->Type2, ctx->type2_count);
	ctx->Type2 = NULL;

	lazybiosFreeType3(ctx->Type3, ctx->type3_count);
	ctx->Type3 = NULL;

	lazybiosFreeType4(ctx->Type4, ctx->type4_count);
	ctx->Type4 = NULL;

	lazybiosFreeType5(ctx->Type5, ctx->type5_count);
	ctx->Type5 = NULL;

	lazybiosFreeType6(ctx->Type6, ctx->type6_count);
	ctx->Type6 = NULL;

	lazybiosFreeType7(ctx->Type7, ctx->type7_count);
	ctx->Type7 = NULL;

	lazybiosFreeType8(ctx->Type8, ctx->type8_count);
	ctx->Type8 = NULL;

	lazybiosFreeType9(ctx->Type9, ctx->type9_count);
	ctx->Type9 = NULL;

	lazybiosFreeType10(ctx->Type10, ctx->type10_count);
	ctx->Type10 = NULL;

	lazybiosFreeType11(ctx->Type11, ctx->type11_count);
	ctx->Type11 = NULL;

	lazybiosFreeType12(ctx->Type12, ctx->type12_count);
	ctx->Type12 = NULL;

	lazybiosFreeType13(ctx->Type13, ctx->type13_count);
	ctx->Type13 = NULL;

	lazybiosFreeType14(ctx->Type14, ctx->type14_count);
	ctx->Type14 = NULL;

	lazybiosFreeType15(ctx->Type15, ctx->type15_count);
	ctx->Type15 = NULL;

	lazybiosFreeType16(ctx->Type16, ctx->type16_count);
	ctx->Type16 = NULL;

	lazybiosFreeType17(ctx->Type17, ctx->type17_count);
	ctx->Type17 = NULL;

	lazybiosFreeType18(ctx->Type18, ctx->type18_count);
	ctx->Type18 = NULL;

	lazybiosFreeType19(ctx->Type19, ctx->type19_count);
	ctx->Type19 = NULL;

	lazybiosFreeType20(ctx->Type20, ctx->type20_count);
	ctx->Type20 = NULL;

	lazybiosFreeType21(ctx->Type21, ctx->type21_count);
	ctx->Type21 = NULL;

	lazybiosFreeType22(ctx->Type22, ctx->type22_count);
	ctx->Type22 = NULL;

	lazybiosFreeType23(ctx->Type23, ctx->type23_count);
	ctx->Type23 = NULL;

	lazybiosFreeType24(ctx->Type24, ctx->type24_count);
	ctx->Type24 = NULL;

	lazybiosFreeType25(ctx->Type25, ctx->type25_count);
	ctx->Type25 = NULL;

	lazybiosFreeType26(ctx->Type26, ctx->type26_count);
	ctx->Type26 = NULL;

	lazybiosFreeType27(ctx->Type27, ctx->type27_count);
	ctx->Type27 = NULL;

	lazybiosFreeType28(ctx->Type28, ctx->type28_count);
	ctx->Type28 = NULL;

	lazybiosFreeType29(ctx->Type29, ctx->type29_count);
	ctx->Type29 = NULL;

	lazybiosFreeType30(ctx->Type30, ctx->type30_count);
	ctx->Type30 = NULL;

	lazybiosFreeType31(ctx->Type31, ctx->type31_count);
	ctx->Type31 = NULL;

	lazybiosFreeType32(ctx->Type32, ctx->type32_count);
	ctx->Type32 = NULL;

	lazybiosFreeType33(ctx->Type33, ctx->type33_count);
	ctx->Type33 = NULL;

	lazybiosFreeType34(ctx->Type34, ctx->type34_count);
	ctx->Type34 = NULL;

	lazybiosFreeType35(ctx->Type35, ctx->type35_count);
	ctx->Type35 = NULL;

	lazybiosFreeType36(ctx->Type36, ctx->type36_count);
	ctx->Type36 = NULL;

	lazybiosFreeType37(ctx->Type37, ctx->type37_count);
	ctx->Type37 = NULL;

	lazybiosFreeType38(ctx->Type38, ctx->type38_count);
	ctx->Type38 = NULL;

	lazybiosFreeType39(ctx->Type39, ctx->type39_count);
	ctx->Type39 = NULL;

	lazybiosFreeType40(ctx->Type40, ctx->type40_count);
	ctx->Type40 = NULL;

	lazybiosFreeType41(ctx->Type41, ctx->type41_count);
	ctx->Type41 = NULL;

	lazybiosFreeType42(ctx->Type42, ctx->type42_count);
	ctx->Type42 = NULL;

	lazybiosFreeType43(ctx->Type43, ctx->type43_count);
	ctx->Type43 = NULL;

	lazybiosFreeType44(ctx->Type44, ctx->type44_count);
	ctx->Type44 = NULL;

	lazybiosFreeType45(ctx->Type45, ctx->type45_count);
	ctx->Type45 = NULL;

	lazybiosFreeType46(ctx->Type46, ctx->type46_count);
	ctx->Type46 = NULL;

	free(ctx->DMIData->dmi_data);
	free(ctx->DMIData->entry_data);
	free(ctx->DMIData);
	free(ctx);
	return 0;
}

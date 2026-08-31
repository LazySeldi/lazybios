/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
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

const char lazybiosVersion[] = "3.0.0";

lazybiosCTX_t* lazybiosCTXNew(void) {
	lazybiosCTX_t* ctx = calloc(1, sizeof(*ctx));
	if (!ctx) return NULL;

	ctx->DMIData = calloc(1, sizeof(*ctx->DMIData));
	if (!ctx->DMIData) {
		free(ctx);
		return NULL;
	}

	/* The vendor containers are always present; only their per-type members
	 * are NULL until the matching getter runs. */
	ctx->oem = calloc(1, sizeof(*ctx->oem));
	if (!ctx->oem) {
		free(ctx->DMIData);
		free(ctx);
		return NULL;
	}
	ctx->oem->dell = calloc(1, sizeof(*ctx->oem->dell));
	ctx->oem->hp = calloc(1, sizeof(*ctx->oem->hp));
	if (!ctx->oem->dell || !ctx->oem->hp) {
		free(ctx->oem->dell);
		free(ctx->oem->hp);
		free(ctx->oem);
		free(ctx->DMIData);
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
    #elif defined(OS_QNX)
        ctx->backend = LAZYBIOS_BACKEND_QNX;
    #elif defined(OS_MINIX)
        ctx->backend = LAZYBIOS_BACKEND_MINIX;
	#elif defined(OS_GENERIC)
		ctx->backend = LAZYBIOS_BACKEND_GENERIC;
	#else
		ctx->backend = LAZYBIOS_BACKEND_UNKNOWN;
	#endif

	return ctx;
}

static inline int lazybiosBuildTypeIndex(lazybiosDMI_t* DMIData) {
    memset(DMIData->index, 0, sizeof(DMIData->index));
    DMIData->index_valid = 0;
    if (!DMIData->dmi_data || DMIData->dmi_len > UINT32_MAX) return -1;

    const uint8_t* p = DMIData->dmi_data;
    const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

    while (p + SMBIOS_HEADER_SIZE < end) {
        const uint8_t type = p[0];
        if (type == SMBIOS_TYPE_END) break;
        if (p[1] < SMBIOS_HEADER_SIZE) break;

        if (DMIData->index[type].count == 0)
            DMIData->index[type].first = (uint32_t)(p - DMIData->dmi_data);
        DMIData->index[type].count++;

        p = DMINext(p, end);
    }
    DMIData->index_valid = 1;
    return 0;
}

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
	if (!ctx || !ctx->DMIData || !entry_path || !dmi_path) return -1;

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

static int lazybiosDetectBackend(lazybiosCTX_t* ctx) {
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

	        case LAZYBIOS_BACKEND_QNX:
	            #if defined(OS_QNX)
	            return lazybiosQNX(ctx);
	            #else
	            lb_log("QNX backend is not available in this build");
	            return -1;
	            #endif

	        case LAZYBIOS_BACKEND_MINIX:
	            #if defined(OS_MINIX)
	            return lazybiosMINIX(ctx);
	            #else
	            lb_log("MINIX backend is not available in this build");
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

int lazybiosInit(lazybiosCTX_t* ctx, const char* entry_point, const char* DMI_BIN) {
	if (!ctx) return -1;
    if (entry_point && !DMI_BIN) {
        lb_log("entry point path set, but DMI table path not set");
        lb_dbg("Entry path: %s", entry_point);
        return -1;
    }

    if (!entry_point && DMI_BIN) {
        if (lazybiosSingleFile(ctx, DMI_BIN) != 0) return -1; // lazybiosSingleFile has already said why it broke
    }

    if (entry_point && DMI_BIN) {
        if (lazybiosFile(ctx, entry_point, DMI_BIN) != 0) return -1;

    }

    if (!entry_point && !DMI_BIN) {
        if (lazybiosDetectBackend(ctx) != 0) return -1;
    }


    if (lazybiosBuildTypeIndex(ctx->DMIData) != 0) {
        lb_log("Couldn't build Type Index table, falling back to manual structure walking");
    }

    return 0;
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

	/*
	 * `end` is the next structure's address, which DMINext places two bytes
	 * past the double NUL that terminates this string set. Deriving the
	 * terminator from it costs a bounds check instead of a scan, and callers
	 * that hand over a truncated structure are rejected by the length guard
	 * above before reaching here.
	 */
	if ((size_t)(end - str) < 2) return NULL;
	const uint8_t* strings_end = end - 2;
	if (strings_end[0] != 0 || strings_end[1] != 0) return NULL;

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

        /* A formatted section is at least a header; anything shorter is not a
         * structure, so the table ends here. DMINext applies the same rule. */
        if (p[1] < SMBIOS_HEADER_SIZE) break;

        if (type == target_type)
            count++;

        p = DMINext(p, end);
    }

    return count;
}

int lazybiosParseAll(lazybiosCTX_t* ctx) {
	if (!ctx || !ctx->DMIData || !ctx->DMIData->dmi_data) return -1;

	/*
	 * Types already parsed are left alone, so calling this twice does not
	 * strand the first result set.
	 */
	if (!ctx->Type0) ctx->Type0 = lazybiosGetType0(ctx->DMIData);
	if (!ctx->Type1) ctx->Type1 = lazybiosGetType1(ctx->DMIData);
	if (!ctx->Type2) ctx->Type2 = lazybiosGetType2(ctx->DMIData);
	if (!ctx->Type3) ctx->Type3 = lazybiosGetType3(ctx->DMIData);
	if (!ctx->Type4) ctx->Type4 = lazybiosGetType4(ctx->DMIData);
	if (!ctx->Type5) ctx->Type5 = lazybiosGetType5(ctx->DMIData);
	if (!ctx->Type6) ctx->Type6 = lazybiosGetType6(ctx->DMIData);
	if (!ctx->Type7) ctx->Type7 = lazybiosGetType7(ctx->DMIData);
	if (!ctx->Type8) ctx->Type8 = lazybiosGetType8(ctx->DMIData);
	if (!ctx->Type9) ctx->Type9 = lazybiosGetType9(ctx->DMIData);
	if (!ctx->Type10) ctx->Type10 = lazybiosGetType10(ctx->DMIData);
	if (!ctx->Type11) ctx->Type11 = lazybiosGetType11(ctx->DMIData);
	if (!ctx->Type12) ctx->Type12 = lazybiosGetType12(ctx->DMIData);
	if (!ctx->Type13) ctx->Type13 = lazybiosGetType13(ctx->DMIData);
	if (!ctx->Type14) ctx->Type14 = lazybiosGetType14(ctx->DMIData);
	if (!ctx->Type15) ctx->Type15 = lazybiosGetType15(ctx->DMIData);
	if (!ctx->Type16) ctx->Type16 = lazybiosGetType16(ctx->DMIData);
	if (!ctx->Type17) ctx->Type17 = lazybiosGetType17(ctx->DMIData);
	if (!ctx->Type18) ctx->Type18 = lazybiosGetType18(ctx->DMIData);
	if (!ctx->Type19) ctx->Type19 = lazybiosGetType19(ctx->DMIData);
	if (!ctx->Type20) ctx->Type20 = lazybiosGetType20(ctx->DMIData);
	if (!ctx->Type21) ctx->Type21 = lazybiosGetType21(ctx->DMIData);
	if (!ctx->Type22) ctx->Type22 = lazybiosGetType22(ctx->DMIData);
	if (!ctx->Type23) ctx->Type23 = lazybiosGetType23(ctx->DMIData);
	if (!ctx->Type24) ctx->Type24 = lazybiosGetType24(ctx->DMIData);
	if (!ctx->Type25) ctx->Type25 = lazybiosGetType25(ctx->DMIData);
	if (!ctx->Type26) ctx->Type26 = lazybiosGetType26(ctx->DMIData);
	if (!ctx->Type27) ctx->Type27 = lazybiosGetType27(ctx->DMIData);
	if (!ctx->Type28) ctx->Type28 = lazybiosGetType28(ctx->DMIData);
	if (!ctx->Type29) ctx->Type29 = lazybiosGetType29(ctx->DMIData);
	if (!ctx->Type30) ctx->Type30 = lazybiosGetType30(ctx->DMIData);
	if (!ctx->Type31) ctx->Type31 = lazybiosGetType31(ctx->DMIData);
	if (!ctx->Type32) ctx->Type32 = lazybiosGetType32(ctx->DMIData);
	if (!ctx->Type33) ctx->Type33 = lazybiosGetType33(ctx->DMIData);
	if (!ctx->Type34) ctx->Type34 = lazybiosGetType34(ctx->DMIData);
	if (!ctx->Type35) ctx->Type35 = lazybiosGetType35(ctx->DMIData);
	if (!ctx->Type36) ctx->Type36 = lazybiosGetType36(ctx->DMIData);
	if (!ctx->Type37) ctx->Type37 = lazybiosGetType37(ctx->DMIData);
	if (!ctx->Type38) ctx->Type38 = lazybiosGetType38(ctx->DMIData);
	if (!ctx->Type39) ctx->Type39 = lazybiosGetType39(ctx->DMIData);
	if (!ctx->Type40) ctx->Type40 = lazybiosGetType40(ctx->DMIData);
	if (!ctx->Type41) ctx->Type41 = lazybiosGetType41(ctx->DMIData);
	if (!ctx->Type42) ctx->Type42 = lazybiosGetType42(ctx->DMIData);
	if (!ctx->Type43) ctx->Type43 = lazybiosGetType43(ctx->DMIData);
	if (!ctx->Type44) ctx->Type44 = lazybiosGetType44(ctx->DMIData);
	if (!ctx->Type45) ctx->Type45 = lazybiosGetType45(ctx->DMIData);
	if (!ctx->Type46) ctx->Type46 = lazybiosGetType46(ctx->DMIData);

	if (!ctx->oem->dell->Type177) ctx->oem->dell->Type177 = lazybiosGetOemDellType177(ctx->DMIData);
	if (!ctx->oem->dell->Type212) ctx->oem->dell->Type212 = lazybiosGetOemDellType212(ctx->DMIData);
	if (!ctx->oem->dell->Type218) ctx->oem->dell->Type218 = lazybiosGetOemDellType218(ctx->DMIData);
	if (!ctx->oem->hp->Type204) ctx->oem->hp->Type204 = lazybiosGetOemHpType204(ctx->DMIData);

	return 0;
}

int lazybiosCleanup(lazybiosCTX_t* ctx) {
	if (!ctx) return -1;

	lazybiosFreeType0(ctx->Type0);
	ctx->Type0 = NULL;

	lazybiosFreeType1(ctx->Type1);
	ctx->Type1 = NULL;

    lazybiosFreeType2(ctx->Type2);
    ctx->Type2 = NULL;

    lazybiosFreeType3(ctx->Type3);
    ctx->Type3 = NULL;

    lazybiosFreeType4(ctx->Type4);
    ctx->Type4 = NULL;

    lazybiosFreeType5(ctx->Type5);
    ctx->Type5 = NULL;

    lazybiosFreeType6(ctx->Type6);
    ctx->Type6 = NULL;

    lazybiosFreeType7(ctx->Type7);
    ctx->Type7 = NULL;

    lazybiosFreeType8(ctx->Type8);
    ctx->Type8 = NULL;

    lazybiosFreeType9(ctx->Type9);
    ctx->Type9 = NULL;

    lazybiosFreeType10(ctx->Type10);
    ctx->Type10 = NULL;

    lazybiosFreeType11(ctx->Type11);
    ctx->Type11 = NULL;

    lazybiosFreeType12(ctx->Type12);
    ctx->Type12 = NULL;

    lazybiosFreeType13(ctx->Type13);
    ctx->Type13 = NULL;

    lazybiosFreeType14(ctx->Type14);
    ctx->Type14 = NULL;

    lazybiosFreeType15(ctx->Type15);
    ctx->Type15 = NULL;

    lazybiosFreeType16(ctx->Type16);
    ctx->Type16 = NULL;

    lazybiosFreeType17(ctx->Type17);
    ctx->Type17 = NULL;

    lazybiosFreeType18(ctx->Type18);
    ctx->Type18 = NULL;

    lazybiosFreeType19(ctx->Type19);
    ctx->Type19 = NULL;

    lazybiosFreeType20(ctx->Type20);
    ctx->Type20 = NULL;

    lazybiosFreeType21(ctx->Type21);
    ctx->Type21 = NULL;

    lazybiosFreeType22(ctx->Type22);
    ctx->Type22 = NULL;

    lazybiosFreeType23(ctx->Type23);
    ctx->Type23 = NULL;

    lazybiosFreeType24(ctx->Type24);
    ctx->Type24 = NULL;

    lazybiosFreeType25(ctx->Type25);
    ctx->Type25 = NULL;

    lazybiosFreeType26(ctx->Type26);
    ctx->Type26 = NULL;

    lazybiosFreeType27(ctx->Type27);
    ctx->Type27 = NULL;

    lazybiosFreeType28(ctx->Type28);
    ctx->Type28 = NULL;

    lazybiosFreeType29(ctx->Type29);
    ctx->Type29 = NULL;

    lazybiosFreeType30(ctx->Type30);
    ctx->Type30 = NULL;

    lazybiosFreeType31(ctx->Type31);
    ctx->Type31 = NULL;

    lazybiosFreeType32(ctx->Type32);
    ctx->Type32 = NULL;

    lazybiosFreeType33(ctx->Type33);
    ctx->Type33 = NULL;

    lazybiosFreeType34(ctx->Type34);
    ctx->Type34 = NULL;

    lazybiosFreeType35(ctx->Type35);
    ctx->Type35 = NULL;

    lazybiosFreeType36(ctx->Type36);
    ctx->Type36 = NULL;

    lazybiosFreeType37(ctx->Type37);
    ctx->Type37 = NULL;

    lazybiosFreeType38(ctx->Type38);
    ctx->Type38 = NULL;

    lazybiosFreeType39(ctx->Type39);
    ctx->Type39 = NULL;

    lazybiosFreeType40(ctx->Type40);
    ctx->Type40 = NULL;

    lazybiosFreeType41(ctx->Type41);
    ctx->Type41 = NULL;

    lazybiosFreeType42(ctx->Type42);
    ctx->Type42 = NULL;

    lazybiosFreeType43(ctx->Type43);
    ctx->Type43 = NULL;

    lazybiosFreeType44(ctx->Type44);
    ctx->Type44 = NULL;

    lazybiosFreeType45(ctx->Type45);
    ctx->Type45 = NULL;

    lazybiosFreeType46(ctx->Type46);
    ctx->Type46 = NULL;

	lazybiosFreeOemDellType177(ctx->oem->dell->Type177);
	lazybiosFreeOemDellType212(ctx->oem->dell->Type212);
	lazybiosFreeOemDellType218(ctx->oem->dell->Type218);
	lazybiosFreeOemHpType204(ctx->oem->hp->Type204);
	free(ctx->oem->dell);
	free(ctx->oem->hp);
	free(ctx->oem);
	ctx->oem = NULL;

    memset(ctx->DMIData->index, 0, sizeof(ctx->DMIData->index));
    ctx->DMIData->index_valid = 0;

	free(ctx->DMIData->dmi_data);
	free(ctx->DMIData->entry_data);
	free(ctx->DMIData);
	free(ctx);
	return 0;
}

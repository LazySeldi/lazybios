//
// lazybios.c - Core library functions
//
#include "lazybios.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#	include <windows.h>
#	define OS_WINDOWS 1

#elif defined(__linux__) || defined(linux) || defined(__linux) || defined(__gnu_linux__)
#	define OS_LINUX 1

#elif defined(__APPLE__) && defined(__MACH__)
#	define OS_MACOS 1

#else
#	define OS_UNKNOWN 1
#endif

// Logging system for lazybios

#ifdef __GNUC__
__attribute__((format(printf, 2, 3)))
#endif
static inline void lazybios_log_internal(const char* prefix, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "%s", prefix);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
}

#ifdef LAZYBIOS_QUIET
#	define lb_log(...) ((void)0)
#	define lb_dbg(...) ((void)0)

#elif defined(LAZYBIOS_DEBUG)
#	define lb_log(...) lazybios_log_internal("[lazybios] ", __VA_ARGS__)
#	define lb_dbg(...) lazybios_log_internal("[lazybios-dbg] ", __VA_ARGS__)

#else
#	define lb_log(...) lazybios_log_internal("[lazybios] ", __VA_ARGS__)
#	define lb_dbg(...) ((void)0)
#endif

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

	if (entry_size < 20) {
		lb_log("Invalid SMBIOS entry size %zu", entry_size);
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

	if (lazybiosParseEntry(ctx, entry_buf) != 0) {
		fclose(binf);
		return -1;
	}

	if (fseek(binf, 0, SEEK_END) != 0) {
		lb_log("Failed to seek end of file");
		fclose(binf);
		return -1;
	}
	long file_len = ftell(binf);
	if (file_len <= 0 || file_len < (long)ctx->DMIData->entry_len) {
		lb_log("Invalid file length %ld", file_len);
		fclose(binf);
		return -1;
	}

	ctx->DMIData->dmi_len = (size_t)(file_len - ctx->DMIData->entry_len);
	rewind(binf);
	if (fseek(binf, ctx->DMIData->entry_len, SEEK_SET) != 0) {
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
		ctx->DMIData->dmi_data = LAZYBIOS_NULL;
		return -1;
	}

	return 0;
}

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
		fclose(dmi);
		return -1;
	}
	ctx->DMIData->entry_data = malloc(ctx->DMIData->entry_len);
	if (!ctx->DMIData->entry_data) {
		lb_log("Failed to allocate memory for entry_data");
		fclose(entry);
		return -1;
	}
	memcpy(ctx->DMIData->entry_data, entry_buf, ctx->DMIData->entry_len);
	fclose(entry);

	if (lazybiosParseEntry(ctx, entry_buf) != 0) {
		fclose(dmi);
		return -1;
	}

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
		ctx->DMIData->dmi_data = LAZYBIOS_NULL;
		return -1;
	}
	return 0;
}

// ===== Context Management =====
lazybiosCTX_t* lazybiosCTXNew(void) {
	lazybiosCTX_t* ctx = calloc(1, sizeof(*ctx));
	if (!ctx) return LAZYBIOS_NULL;

	ctx->DMIData = calloc(1, sizeof(*ctx->DMIData));
	if (!ctx->DMIData) {
		free(ctx);
		return LAZYBIOS_NULL;
	}

#if OS_LINUX
	ctx->backend = LAZYBIOS_BACKEND_LINUX;
#elif OS_WINDOWS
	ctx->backend = LAZYBIOS_BACKEND_WINDOWS;
#elif OS_MACOS
	ctx->backend = LAZYBIOS_BACKEND_MACOS;
#else
	ctx->backend = LAZYBIOS_BACKEND_UNKNOWN;
#endif

	return ctx;
}

static inline int lazybiosDevMem(lazybiosCTX_t* ctx) {
	if (!ctx) return -1;

#if OS_LINUX
	lb_log("/dev/mem backend not implemented yet");
	return -1;
#else
	return -1;
#endif
}

static inline int lazybiosWindows(lazybiosCTX_t* ctx) {
	if (!ctx) return -1;

// Most of the Windows backend wasn't made by me I got help from AI for this since I don't know anything about the WindowsAPI
#if OS_WINDOWS
	// RSMB provider ID (little endian for "RSMB")
	const DWORD sig = 0x52534D42;

	// Query required size
	DWORD size = GetSystemFirmwareTable(sig, 0, LAZYBIOS_NULL, 0);
	if (size == 0) {
		lb_log("GetSystemFirmwareTable failed (size=0)");
		return -1;
	}

	uint8_t* buf = malloc(size);
	if (!buf) return -1;

	// Fetch the table
	DWORD got = GetSystemFirmwareTable(sig, 0, buf, size);
	if (got != size) {
		lb_log("SMBIOS read mismatch (%lu != %lu)", (unsigned long)got, (unsigned long)size);
		free(buf);
		return -1;
	}

	// Cast to RawSMBIOSData header used by Windows API
	typedef struct {
		BYTE Used20CallingMethod;
		BYTE SMBIOSMajorVersion;
		BYTE SMBIOSMinorVersion;
		BYTE DmiRevision;
		DWORD Length; // Length of TableData
		BYTE SMBIOSTableData[1]; // Start of DMI structures
	} RawSMBIOSData;

	RawSMBIOSData* raw = (RawSMBIOSData*)buf;

	// Sanity check: table length must fit inside returned buffer
	if (raw->Length == 0 || raw->Length > size) {
		lb_log("Invalid RawSMBIOSData.Length = %u", raw->Length);
		free(buf);
		return -1;
	}

	// Fill ctx->DMIData->entry_info manually
	ctx->DMIData->entry_info.major = raw->SMBIOSMajorVersion;
	ctx->DMIData->entry_info.minor = raw->SMBIOSMinorVersion;
	ctx->DMIData->entry_info.docrev = raw->DmiRevision;
	ctx->DMIData->entry_info.table_length = raw->Length;
	ctx->DMIData->entry_info.table_address = 0; // Windows gives no physical address
	ctx->DMIData->entry_info.is_64bit = (raw->SMBIOSMajorVersion >= 3);

	// Provide ONLY the DMI table payload
	size_t dmi_len = raw->Length;

	uint8_t* table = malloc(dmi_len);
	if (!table) {
		free(buf);
		return -1;
	}

	memcpy(table, raw->SMBIOSTableData, dmi_len);

	// Free the original RSMB buffer — we only keep the DMI payload
	free(buf);

	ctx->DMIData->dmi_data = table;
	ctx->DMIData->entry_data = LAZYBIOS_NULL; // We don't have this in windows
	ctx->DMIData->dmi_len = dmi_len;

	return 0;

#else
	return -1;
#endif
}

static inline int lazybiosMacOS(lazybiosCTX_t* ctx) {
	if (!ctx) return -1;
#ifdef OS_MACOS
	lb_log("MacOS backend not implemented yet");
	return -1;
#else
	return -1;
#endif
}

int lazybiosInit(lazybiosCTX_t* ctx) {
	if (!ctx) return -1;

	switch (ctx->backend) {
		case LAZYBIOS_BACKEND_LINUX: { // Brackets since you can't declare variables inside a case without brackets, you can only do that in C23
			FILE* f = fopen(LINUX_SYSFS_DMI_TABLE, "r");
			if (!f) {
				return lazybiosDevMem(ctx);
			}
			fclose(f);
			return lazybiosFile(ctx, LINUX_SYSFS_SMBIOS_ENTRY, LINUX_SYSFS_DMI_TABLE);
		}

		case LAZYBIOS_BACKEND_WINDOWS:
			return lazybiosWindows(ctx);

		case LAZYBIOS_BACKEND_MACOS:
			return lazybiosMacOS(ctx);

		case LAZYBIOS_BACKEND_UNKNOWN:
			lb_log("Unknown backend %d", ctx->backend);
			return -1;
		default:
			lb_log("Nothing Found!");
			return -1;
	}
}

// ===== Core DMI Parsing Helpers =====

// Finds the next structure in the DMI table
const uint8_t* DMINext(const uint8_t* p, const uint8_t* end) {
	if (p + 4 > end) return end; // header too small

	uint8_t len = p[1];
	if (p + len >= end) return end; // formatted section exceeds buffer

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
char* DMIString(const uint8_t* p, uint8_t length, uint8_t index, const uint8_t* end) {
	if (index == 0)
		return LAZYBIOS_NULL;

	// Point to the start of the unformatted string area
	const uint8_t* str = p + length;

	// Iterate until the selected string
	for (uint8_t i = 1; i < index; i++) {
		while (str < end && *str != 0)
			str++;
		if (str >= end) return LAZYBIOS_NULL; // out of bounds
		str++; // here we skip the null terminator
	}

	// Now str points to the target string
	const uint8_t* s = str;

	// Find null terminator safely
	while (s < end && *s != 0)
		s++;

	if (s >= end)
		return LAZYBIOS_NULL; // unterminated -> BIOS corruption

	// Length is safe
	size_t len = (size_t)(s - str);

	char* copy = malloc(len + 1);
	if (!copy) return LAZYBIOS_NULL;

	memcpy(copy, str, len);
	copy[len] = '\0';
	return copy;
}

// Parses the SMBIOS Entry Point structure
int lazybiosParseEntry(lazybiosCTX_t* ctx, const uint8_t* entry_buf) {
	if (memcmp(entry_buf, SMBIOS3_ANCHOR, 5) == 0) {
		// SMBIOS 3.x Entry Point
		ctx->DMIData->entry_info.major = entry_buf[SMBIOS3_MAJOR_OFFSET];
		ctx->DMIData->entry_info.minor = entry_buf[SMBIOS3_MINOR_OFFSET];
		ctx->DMIData->entry_info.docrev = entry_buf[SMBIOS3_DOCREV_OFFSET];
		memcpy(&ctx->DMIData->entry_info.table_length, entry_buf + SMBIOS3_TABLE_LENGTH, sizeof(uint32_t));
		memcpy(&ctx->DMIData->entry_info.table_address, entry_buf + SMBIOS3_TABLE_ADDRESS, sizeof(uint64_t));
		ctx->DMIData->entry_info.n_structures = LAZYBIOS_NOT_FOUND_U16; // Not available in SMBIOS 3.x
		ctx->DMIData->entry_info.is_64bit = true;
	} else if (memcmp(entry_buf, SMBIOS2_DMI_ANCHOR, 5) == 0 || memcmp(entry_buf, SMBIOS2_ANCHOR, 4) == 0) {
		// SMBIOS 2.x Entry Point
		ctx->DMIData->entry_info.major = entry_buf[SMBIOS2_MAJOR_OFFSET];
		ctx->DMIData->entry_info.minor = entry_buf[SMBIOS2_MINOR_OFFSET];
		ctx->DMIData->entry_info.docrev = LAZYBIOS_NOT_FOUND_U8; // Not available in SMBIOS 2.x
		memcpy(&ctx->DMIData->entry_info.table_length, entry_buf + SMBIOS2_TABLE_LENGTH, sizeof(uint16_t));
		memcpy(&ctx->DMIData->entry_info.table_address, entry_buf + SMBIOS2_TABLE_ADDRESS, sizeof(uint32_t));
		memcpy(&ctx->DMIData->entry_info.n_structures, entry_buf + SMBIOS2_N_STRUCTURES, sizeof(uint16_t));
		ctx->DMIData->entry_info.is_64bit = false;
	} else {
		lb_log("SMBIOS anchor not found");
		lb_dbg("anchor bytes: %02X %02X %02X %02X %02X  (SMBIOS-2 only uses first 4)", entry_buf[0], entry_buf[1], entry_buf[2], entry_buf[3], entry_buf[4]);
		return -1;
	}

	return 0;
}

// Counts the number of structures of a given type
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
void lazybiosPrintVer(const lazybiosCTX_t* ctx) {
	if (!ctx) return;
	if (ISVERPLUS(ctx->DMIData, 3, 0)) {
		printf("SMBIOS version %d.%d.%d\n",
			   ctx->DMIData->entry_info.major,
			   ctx->DMIData->entry_info.minor,
			   ctx->DMIData->entry_info.docrev);
	} else {
		printf("SMBIOS version %d.%d\n",
			   ctx->DMIData->entry_info.major,
			   ctx->DMIData->entry_info.minor);
	}
}

int lazybiosCleanup(lazybiosCTX_t* ctx) {
	if (!ctx) return -1;

	lazybiosFreeType0(ctx->Type0);
	ctx->Type0 = LAZYBIOS_NULL;

	lazybiosFreeType1(ctx->Type1);
	ctx->Type1 = LAZYBIOS_NULL;

	lazybiosFreeType2(ctx->Type2, ctx->type2_count);
	ctx->Type2 = LAZYBIOS_NULL;

	lazybiosFreeType3(ctx->Type3, ctx->type3_count);
	ctx->Type3 = LAZYBIOS_NULL;

	lazybiosFreeType4(ctx->Type4, ctx->type4_count);
	ctx->Type4 = LAZYBIOS_NULL;

	lazybiosFreeType17(ctx->Type17, ctx->type17_count);
	ctx->Type17 = LAZYBIOS_NULL;

	free(ctx->DMIData->dmi_data);
	free(ctx->DMIData->entry_data);
	free(ctx->DMIData);
	free(ctx);
	return 0;
}

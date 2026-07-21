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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>
	#define OS_WINDOWS 1

#elif defined(__linux__) || defined(linux) || defined(__linux) || defined(__gnu_linux__)
	#include <sys/mman.h>
	#include <sys/types.h>
	#define OS_LINUX 1

#elif defined(__APPLE__) && defined(__MACH__)
#	define OS_MACOS 1

#else
	#define OS_UNKNOWN 1
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

#if defined(OS_LINUX)
static int uint64_to_off_t(uint64_t value, off_t* out) {
	uint64_t off_t_max = ((uint64_t)1 << (sizeof(off_t) * CHAR_BIT - 1)) - 1;
	if (value > off_t_max) return -1;
	*out = (off_t)value;
	return 0;
}
#endif

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
	#else
		ctx->backend = LAZYBIOS_BACKEND_UNKNOWN;
	#endif

	return ctx;
}

#if defined(OS_LINUX)
static inline int lazybiosDevMem(lazybiosCTX_t* ctx) {
    if (!ctx) return -1;
        #define SMBIOS_START 0xF0000
        #define SMBIOS_SIZE  0x10000

        int fd = open(DEV_MEM, O_RDONLY);
        if (fd == -1) {
            lb_log("Failed to open /dev/mem");
            lb_dbg("Error: %s", strerror(errno));
            return -1;
        }

        off_t base_addr = SMBIOS_START;
        long page_size = sysconf(_SC_PAGESIZE);
        if (page_size <= 0) {
            lb_log("Failed to read system page size");
            close(fd);
            return -1;
        }

        off_t aligned_addr = (base_addr / page_size) * page_size;
        off_t page_offset = base_addr - aligned_addr;
        size_t page_size_unsigned = (size_t)page_size;
        size_t page_offset_unsigned = (size_t)page_offset;

        // Round map_size up to the nearest page to be safe
        size_t map_size = SMBIOS_SIZE + page_offset_unsigned;
        map_size = (map_size + page_size_unsigned - 1) & ~(page_size_unsigned - 1);

        void *mapped_data = mmap(NULL, map_size, PROT_READ, MAP_SHARED, fd, aligned_addr);
        if (mapped_data == MAP_FAILED) {
            lb_log("Failed to mmap /dev/mem data");
            lb_dbg("Error: %s", strerror(errno));
            close(fd);
            return -1;
        }

        unsigned char *smbios_data = (unsigned char *)mapped_data + page_offset;

        int found = 0;
        size_t sig_offset = 0;
        int smbios = 0; // Small little check for SMBIOS version, 1 means _DMI_(SMBIOS 2.x), 2, Means _SM_(SMBIOS 2.x) and 3 means _SM3_(SMBIOS 3.x)

        for (size_t i = 0; i < SMBIOS_SIZE - 5; i++) {
            // Check "_SM3_"
            if (smbios_data[i] == '_' && smbios_data[i+1] == 'S' &&
                smbios_data[i+2] == 'M' && smbios_data[i+3] == '3' &&
                smbios_data[i+4] == '_') {
                found = 1;
                smbios = 3;
                sig_offset = i;
                break;
            }

            // Check "_SM_"
            if (smbios_data[i] == '_' && smbios_data[i+1] == 'S' &&
                smbios_data[i+2] == 'M' && smbios_data[i+3] == '_' &&
                smbios_data[i+4] != '3') {
                found = 1;
                smbios = 2;
                sig_offset = i;
                break;
            }

            // Check "_DMI_"
            if (smbios_data[i] == '_' && smbios_data[i+1] == 'D' &&
                smbios_data[i+2] == 'M' && smbios_data[i+3] == 'I' &&
                smbios_data[i+4] == '_') {
                found = 1;
                smbios = 1;
                sig_offset = i;
                break;
            }
        }

        if (!found) {
            lb_log("SMBIOS/DMI signature not found in /dev/mem");
            munmap(mapped_data, map_size);
            close(fd);
            return -1;
        }

        unsigned char *entry = smbios_data + sig_offset;

        // Determine entry length based on signature
        size_t entry_len = 0;
        if (smbios == 3) {
            entry_len = entry[0x06]; // Length at offset 0x06
            // We're gonna cap to the actual size so we don't over-read
            if (entry_len > sizeof(lazybiosSMBIOS3Entry)) entry_len = sizeof(lazybiosSMBIOS3Entry);
            if (entry_len < 0x18) entry_len = 0x18;
        } else if (smbios == 1) {
            entry_len = entry[0x05]; // Length at offset 0x05
            if (entry_len > sizeof(lazybiosSMBIOS2Entry)) entry_len = sizeof(lazybiosSMBIOS2Entry);
            if (entry_len < 0x0F) entry_len = 0x0F;
        } else {
            entry_len = entry[0x05]; // Length at offset 0x05
            if (entry_len > sizeof(lazybiosSMBIOS2Entry)) entry_len = sizeof(lazybiosSMBIOS2Entry);
            if (entry_len < 0x1F) entry_len = 0x1F;
        }

        ctx->DMIData->entry_len = entry_len;
        ctx->DMIData->entry_data = malloc(entry_len);
        if (!ctx->DMIData->entry_data) {
            lb_log("Failed to allocate entry_data");
            munmap(mapped_data, map_size);
            close(fd);
            return -1;
        }
        memcpy(ctx->DMIData->entry_data, entry, entry_len);

        if (lazybiosParseEntry(ctx, ctx->DMIData->entry_data, ctx->DMIData->entry_len) != 0) {
            lb_log("Failed to parse SMBIOS entry point");
            free(ctx->DMIData->entry_data);
            ctx->DMIData->entry_data = NULL;
            munmap(mapped_data, map_size);
            close(fd);
            return -1;
        }

        uint64_t table_addr = 0;
        size_t table_len = 0;

        if (ctx->DMIData->entry_tag == SMBIOS_VER_3X) {
            table_addr = ctx->DMIData->entry_union.v3->structure_table_address;
            table_len = ctx->DMIData->entry_union.v3->structure_table_max_size;
        } else {
            table_addr = ctx->DMIData->entry_union.v2->structure_table_address;
            table_len = ctx->DMIData->entry_union.v2->structure_table_length;
        }

        munmap(mapped_data, map_size);

        // From this point on, this function was not made all by me
        off_t table_addr_off = 0;
        if (uint64_to_off_t(table_addr, &table_addr_off) != 0) {
            lb_log("SMBIOS table address does not fit in off_t: 0x%016lx", (unsigned long)table_addr);
            free(ctx->DMIData->entry_data);
            ctx->DMIData->entry_data = NULL;
            close(fd);
            return -1;
        }

        off_t table_aligned = (table_addr_off / page_size) * page_size;
        off_t table_offset_page = table_addr_off - table_aligned;
        size_t table_offset_unsigned = (size_t)table_offset_page;

        // We'll calculate map size by rounding up to the nearest page
        if (table_len > SIZE_MAX - table_offset_unsigned) {
            lb_log("SMBIOS table mapping size overflow");
            free(ctx->DMIData->entry_data);
            ctx->DMIData->entry_data = NULL;
            close(fd);
            return -1;
        }
        size_t table_map_size = table_len + table_offset_unsigned;
        table_map_size = (table_map_size + page_size_unsigned - 1) & ~(page_size_unsigned - 1);

        void *mapped_table = mmap(NULL, table_map_size, PROT_READ, MAP_SHARED, fd, table_aligned);
        if (mapped_table == MAP_FAILED) {
            lb_log("Failed to mmap DMI/SMBIOS table at 0x%016lx", (unsigned long)table_addr);
            lb_dbg("Error: %s", strerror(errno));
            free(ctx->DMIData->entry_data);
            ctx->DMIData->entry_data = NULL;
            close(fd);
            return -1;
        }

        unsigned char *table_data = (unsigned char *)mapped_table + table_offset_page;

        // Store table data in context
        ctx->DMIData->dmi_len = table_len;
        ctx->DMIData->dmi_data = malloc(table_len);
        if (!ctx->DMIData->dmi_data) {
            lb_log("Failed to allocate DMI buffer (%zu bytes)", table_len);
            munmap(mapped_table, table_map_size);
            free(ctx->DMIData->entry_data);
            ctx->DMIData->entry_data = NULL;
            close(fd);
            return -1;
        }
        memcpy(ctx->DMIData->dmi_data, table_data, table_len);

        munmap(mapped_table, table_map_size);
        close(fd);

        return 0;
}
#endif

#if defined(OS_WINDOWS)
static inline int lazybiosWindows(lazybiosCTX_t* ctx) { // Help with the windows backend is appriciated, since I'm not an expert at the windows API, and it was not fully made by me.
    if (!ctx) return -1;

        // RSMB provider ID (little endian for "RSMB")
        const DWORD sig = 0x52534D42;

        // Query required size
        DWORD size = GetSystemFirmwareTable(sig, 0, NULL, 0);
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

        uint8_t smbios_major = raw->SMBIOSMajorVersion;
        uint8_t smbios_minor = raw->SMBIOSMinorVersion;
        uint8_t smbios_docrev = raw->DmiRevision;
        size_t dmi_len = raw->Length;
        uint8_t* table = malloc(dmi_len);
        if (!table) {
            free(buf);
            return -1;
        }
        memcpy(table, raw->SMBIOSTableData, dmi_len);

        // Free the original RSMB buffer
        free(buf);

        // Store DMI table
        ctx->DMIData->dmi_data = table;
        ctx->DMIData->dmi_len = dmi_len;

		// Since windows doesn't give us a table for the entry, we'll make a half-assed one
        if (smbios_major >= 3) {
            // the SMBIOS 3.x entry is 24B
            uint8_t entry[24] = {0};
            entry[0] = '_'; entry[1] = 'S'; entry[2] = 'M'; entry[3] = '3'; entry[4] = '_';

            // Length 0x18 (24B) and the version
            entry[0x06] = 0x18;
            entry[0x07] = smbios_major;
            entry[0x08] = smbios_minor;
            entry[0x09] = smbios_docrev;

            // Entry point revision is 0x01 for SMBIOS 3.0+
            entry[0x0A] = 0x01;

            // Reserved
            entry[0x0B] = 0x00;

            // Table maximum size (4 bytes, little-endian)
            uint32_t table_max_size = (uint32_t)dmi_len;
            entry[0x0C] = table_max_size & 0xFF;
            entry[0x0D] = (table_max_size >> 8) & 0xFF;
            entry[0x0E] = (table_max_size >> 16) & 0xFF;
            entry[0x0F] = (table_max_size >> 24) & 0xFF;

            // Table address (8 bytes, little-endian) - 0 on Windows
            entry[0x10] = 0; entry[0x11] = 0; entry[0x12] = 0; entry[0x13] = 0;
            entry[0x14] = 0; entry[0x15] = 0; entry[0x16] = 0; entry[0x17] = 0;

            // Calculate checksum (sum of all bytes must be 0)
            entry[0x05] = 0; // Zero out checksum first
            uint8_t sum = 0;
            for (int i = 0; i < sizeof(entry); i++) {
                sum += entry[i];
            }
            entry[0x05] = (uint8_t)(-sum); // Two's complement

            // Store the entry point
            ctx->DMIData->entry_len = sizeof(entry);
            ctx->DMIData->entry_data = malloc(sizeof(entry));
            if (!ctx->DMIData->entry_data) {
                free(ctx->DMIData->dmi_data);
                ctx->DMIData->dmi_data = NULL;
                return -1;
            }
            memcpy(ctx->DMIData->entry_data, entry, sizeof(entry));

        } else {
            // SMBIOS 2.x Entry Point: 31B
            uint8_t entry[31] = {0};
            entry[0] = '_'; entry[1] = 'S'; entry[2] = 'M'; entry[3] = '_';

            // Length is 0x1F (31 bytes)
            entry[0x05] = 0x1F;
            entry[0x06] = smbios_major;
            entry[0x07] = smbios_minor;

            // Max structure size (2 bytes, little-endian)
            entry[0x08] = 0x00; entry[0x09] = 0x00; // Not used

            // Entry point revision is 0x00 for SMBIOS 2.x
            entry[0x0A] = 0x00;

            // Formatted area (5 bytes, reserved - all zeros)
            entry[0x0B] = 0; entry[0x0C] = 0; entry[0x0D] = 0; entry[0x0E] = 0; entry[0x0F] = 0;
            entry[0x10] = '_'; entry[0x11] = 'D'; entry[0x12] = 'M'; entry[0x13] = 'I'; entry[0x14] = '_';

            // Table length (2 bytes, little-endian)
            uint16_t table_len = (uint16_t)dmi_len;
            entry[0x16] = table_len & 0xFF;
            entry[0x17] = (table_len >> 8) & 0xFF;

            // Table address (4 bytes, little-endian) - 0 on Windows
            entry[0x18] = 0; entry[0x19] = 0; entry[0x1A] = 0; entry[0x1B] = 0;

            // Structure count (2 bytes, little-endian) - Not available
            entry[0x1C] = 0; entry[0x1D] = 0;

            // BCD revision
            entry[0x1E] = (smbios_major << 4) | smbios_minor;

            // Calculate intermediate checksum (bytes 0x10 through 0x1E)
            entry[0x15] = 0; // Zero out intermediate checksum first
            uint8_t sum2 = 0;
            for (int i = 0x10; i < 0x1F; i++) {
                sum2 += entry[i];
            }
            entry[0x15] = (uint8_t)(-sum2);

            // Calculate main checksum (bytes 0x00 through 0x0F)
            entry[0x04] = 0; // Zero out main checksum first
            uint8_t sum1 = 0;
            for (int i = 0; i < 0x10; i++) {
                sum1 += entry[i];
            }
            entry[0x04] = (uint8_t)(-sum1);

            // Store the entry point
            ctx->DMIData->entry_len = sizeof(entry);
            ctx->DMIData->entry_data = malloc(sizeof(entry));
            if (!ctx->DMIData->entry_data) {
                free(ctx->DMIData->dmi_data);
                ctx->DMIData->dmi_data = NULL;
                return -1;
            }
            memcpy(ctx->DMIData->entry_data, entry, sizeof(entry));
        }

        if (lazybiosParseEntry(ctx, ctx->DMIData->entry_data, ctx->DMIData->entry_len) != 0) {
            free(ctx->DMIData->entry_data);
            ctx->DMIData->entry_data = NULL;
            free(ctx->DMIData->dmi_data);
            ctx->DMIData->dmi_data = NULL;
            ctx->DMIData->dmi_len = 0;
            return -1;
        }

        return 0;
}
#endif

#if defined(OS_MACOS)
static inline int lazybiosMacOS(lazybiosCTX_t* ctx) {
	if (!ctx) return -1;
	lb_log("MacOS backend not implemented yet");
	return -1;
}
#endif

/**
 * @brief Loads SMBIOS data using the context's selected platform backend.
 *
 * @param ctx Context that receives the raw entry point and DMI table data.
 * @return 0 on success, or -1 on failure.
 */
int lazybiosInit(lazybiosCTX_t* ctx) {
	if (!ctx) return -1;

	switch (ctx->backend) {
		case LAZYBIOS_BACKEND_LINUX: { // Brackets since you can't declare variables inside a case without brackets, you can only do that in C23
			#if defined(OS_LINUX)
			FILE* f = fopen(LINUX_SYSFS_DMI_TABLE, "rb");
			if (!f) {
				return lazybiosDevMem(ctx);
			}
			fclose(f);
			return lazybiosFile(ctx, LINUX_SYSFS_SMBIOS_ENTRY, LINUX_SYSFS_DMI_TABLE);
			#else
			lb_log("Linux backend is not available in this build!");
			return -1;
			#endif
		}

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

static inline int lazybiosVerifyChecksum(const uint8_t* entry_buf, size_t len) {
    uint8_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += entry_buf[i];
    }
    return (sum == 0);
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
    if (!entry_buf || buf_len < 6) {
        lb_log("Buffer too small(buf < 6) to contain an SMBIOS entry point");
        return -1;
    }

    if (memcmp(entry_buf, SMBIOS3_ANCHOR, SMBIOS3_ANCHOR_SIZE) == 0) {
        uint8_t spec_length = entry_buf[SMBIOS3_LENGTH_OFFSET]; // Usually 0x18 (24)

        if (buf_len < spec_length) {
            lb_log("SMBIOS 3.x buffer truncated: expected %d bytes, got %zu", spec_length, buf_len);
            return -1;
        }

        ctx->DMIData->entry_tag = SMBIOS_VER_3X;

        if (!lazybiosVerifyChecksum(entry_buf, spec_length)) {
            lb_dbg("Warning: SMBIOS 3.x Entry Point Checksum failed! (Proceeding anyway)");
        }
        ctx->DMIData->entry_union.v3 = (lazybiosSMBIOS3Entry*)entry_buf;

    } else if (memcmp(entry_buf, SMBIOS2_ANCHOR, SMBIOS2_ANCHOR_SIZE) == 0 || memcmp(entry_buf, SMBIOS2_INTERMEDIATE_ANCHOR, SMBIOS2_INTERMEDIATE_ANCHOR_SIZE) == 0) {
        uint8_t spec_length = entry_buf[SMBIOS2_LENGTH_OFFSET]; // Usually 0x1F (31)

        if (buf_len < spec_length) {
            lb_log("SMBIOS 2.x buffer truncated: expected %d bytes, got %zu", spec_length, buf_len);
            return -1;
        }

        ctx->DMIData->entry_tag = SMBIOS_VER_2X;

        if (!lazybiosVerifyChecksum(entry_buf, 0x10)) {
            lb_dbg("Warning: SMBIOS 2.x Main Checksum failed! (Proceeding anyway)");
        }
        if (!lazybiosVerifyChecksum(entry_buf + 0x10, 0x0F)) {
            lb_dbg("Warning: SMBIOS 2.x Intermediate (_DMI_) Checksum failed! (Proceeding anyway)");
        }
        ctx->DMIData->entry_union.v2 = (lazybiosSMBIOS2Entry*)entry_buf;

    } else {
        lb_log("SMBIOS anchor not found");
        lb_dbg("anchor bytes: %02X %02X %02X %02X %02X (SMBIOS 2.x uses only the first 4)", entry_buf[0], entry_buf[1], entry_buf[2], entry_buf[3], entry_buf[4]);
        return -1;
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

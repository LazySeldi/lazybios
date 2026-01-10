//
// lazybios.c - Core library functions
//
#include "lazybios.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #define OS_WINDOWS 1

#elif defined(__linux__) || defined(linux) || defined(__linux) || defined(__gnu_linux__)
    #define OS_LINUX 1

#elif defined(__APPLE__) && defined(__MACH__)
    #define OS_MACOS 1

#else
    #define OS_UNKNOWN 1
#endif

// Logging system for lazybios

// Internal helper function
static inline void lazybios_log_internal(const char *prefix, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "%s", prefix);    // print the prefix
    vfprintf(stderr, fmt, args);      // print user message
    fprintf(stderr, "\n");            // the newline
    va_end(args);
}

// Quiet mode - no logging
#ifdef LAZYBIOS_QUIET
# define lb_log(...)      ((void)0)
# define lb_dbg(...)      ((void)0)

// Debug mode - enable verbose logging
#elif defined(LAZYBIOS_DEBUG)
# define lb_log(...)      lazybios_log_internal("[lazybios] ", __VA_ARGS__)
# define lb_dbg(...)      lazybios_log_internal("[lazybios-dbg] ", __VA_ARGS__)

// Normal mode - standard logging only
#else
# define lb_log(...)      lazybios_log_internal("[lazybios] ", __VA_ARGS__)
# define lb_dbg(...)      ((void)0)
#endif

// So here I choose an inline function and macros because my only other options were either to switch to C23 and use __VA_ARGS__ normally because C11 doesn't support that, or I could Enable GNU extension and be dependent on it.
// I could also require a dummy argument to "fix" it altogether but that's probably not safe

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
        entry_size = SIZE_MAX; // our falack
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

    // --- Step 2: read full entry point ---
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

    // --- Step 3: allocate entry_data ---
    ctx->entry_len = entry_size;
    ctx->entry_data = malloc(ctx->entry_len);
    if (!ctx->entry_data) {
        lb_log("Failed to allocate memory for entry_data");
        fclose(binf);
        return -1;
    }
    memcpy(ctx->entry_data, entry_buf, ctx->entry_len);

    if (lazybiosParseEntry(ctx, entry_buf) != 0) {
        fclose(binf);
        return -1;
    }

    // --- Step 4: read the rest of the file as DMI table ---
    if (fseek(binf, 0, SEEK_END) != 0) {
        lb_log("Failed to seek end of file");
        fclose(binf);
        return -1;
    }
    long file_len = ftell(binf);
    if (file_len <= 0 || file_len < (long)ctx->entry_len) {
        lb_log("Invalid file length %ld", file_len);
        fclose(binf);
        return -1;
    }

    ctx->dmi_len = (size_t)(file_len - ctx->entry_len);
    rewind(binf);
    if (fseek(binf, ctx->entry_len, SEEK_SET) != 0) {
        lb_log("Failed to seek to DMI data start");
        fclose(binf);
        return -1;
    }

    ctx->dmi_data = malloc(ctx->dmi_len);
    if (!ctx->dmi_data) {
        lb_log("Failed to allocate DMI buffer (%zu bytes)", ctx->dmi_len);
        fclose(binf);
        return -1;
    }

    size_t got = fread(ctx->dmi_data, 1, ctx->dmi_len, binf);
    fclose(binf);

    if (got != ctx->dmi_len) {
        lb_log("Short read of DMI data (%zu of %zu bytes)", got, ctx->dmi_len);
        free(ctx->dmi_data);
        ctx->dmi_data = LAZYBIOS_NULL;
        return -1;
    }

    return 0;
}

int lazybiosFile(lazybiosCTX_t* ctx, const char* entry_path, const char* dmi_path) {
    if (!ctx) return -1;

    FILE *entry = fopen(entry_path, "rb");
    if (!entry) {
        lb_log("failed to open %s: %s", entry_path, strerror(errno));
        return -1;
    }

    FILE *dmi = fopen(dmi_path, "rb");
    if (!dmi) {
        lb_log("Failed to open %s: %s", dmi_path, strerror(errno));
        fclose(entry);
        return -1;
    }

    uint8_t entry_buf[64];
    size_t n = fread(entry_buf, 1, sizeof(entry_buf), entry);
    ctx->entry_len = n;
    ctx->entry_data = malloc(ctx->entry_len);
    if (!ctx->entry_data) {
        lb_log("Failed to allocate memory for entry_data");
        fclose(entry);
        return -1;
    }

    memcpy(ctx->entry_data, entry_buf, ctx->entry_len);
    fclose(entry);

    if (n < 20) {
        lb_log("Invalid SMBIOS entry point (%zu bytes)", n);
        fclose(dmi);
        return -1;
    }

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

    ctx->dmi_len = (size_t)len;
    rewind(dmi);

    ctx->dmi_data = malloc(ctx->dmi_len);
    if (!ctx->dmi_data) {
        lb_log("Failed to allocate %zu bytes for DMI buffer", ctx->dmi_len);
        fclose(dmi);
        return -1;
    }

    size_t got = fread(ctx->dmi_data, 1, ctx->dmi_len, dmi);
    fclose(dmi);

    if (got != ctx->dmi_len) {
        lb_log("Short read in DMI table");
        free(ctx->dmi_data);
        ctx->dmi_data = LAZYBIOS_NULL;
        return -1;
    }
    return 0;
}

// ===== Context Management =====
lazybiosCTX_t* lazybiosCTXNew(void) {
    lazybiosCTX_t *ctx = calloc(1, sizeof(*ctx));
    if (!ctx) return LAZYBIOS_NULL;

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

int lazybiosDevMem(lazybiosCTX_t* ctx) {
    if (!ctx) return -1;

    #if OS_LINUX
        lb_log("/dev/mem backend not implemented yet");
        return -1;
    #else
        return -1;
    #endif
}

int lazybiosWindows(lazybiosCTX_t *ctx) {
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

        uint8_t *buf = malloc(size);
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
            DWORD Length;              // Length of TableData
            BYTE SMBIOSTableData[1];   // Start of DMI structures
        } RawSMBIOSData;

        RawSMBIOSData *raw = (RawSMBIOSData *) buf;

        // Sanity check: table length must fit inside returned buffer
        if (raw->Length == 0 || raw->Length > size) {
            lb_log("Invalid RawSMBIOSData.Length = %u", raw->Length);
            free(buf);
            return -1;
        }

        // Fill ctx->entry_info manually
        ctx->entry_info.major     = raw->SMBIOSMajorVersion;
        ctx->entry_info.minor     = raw->SMBIOSMinorVersion;
        ctx->entry_info.docrev    = raw->DmiRevision;
        ctx->entry_info.table_length  = raw->Length;
        ctx->entry_info.table_address = 0;     // Windows gives no physical address
        ctx->entry_info.is_64bit  = (raw->SMBIOSMajorVersion >= 3);

        // Provide ONLY the DMI table payload
        size_t dmi_len = raw->Length;

        uint8_t *table = malloc(dmi_len);
        if (!table) {
            free(buf);
            return -1;
        }

        memcpy(table, raw->SMBIOSTableData, dmi_len);

        // Free the original RSMB buffer — we only keep the DMI payload
        free(buf);

        ctx->dmi_data = table;
        ctx->entry_data = LAZYBIOS_NULL; // We don't have this in windows
        ctx->dmi_len  = dmi_len;

        return 0;

    #else
        return -1;
    #endif
}

int lazybiosMacOS(lazybiosCTX_t* ctx) {
    if (!ctx) return -1;
    #ifdef OS_MACOS
        lb_log("MacOS backend not implemented yet");
        return -1;
    #else
        return -1;
    #endif
}

int lazybiosInit(lazybiosCTX_t *ctx) {
    if (!ctx) return -1;

    switch (ctx->backend) {
        case LAZYBIOS_BACKEND_LINUX: { // Brackets since you can't declare variables inside a case without brackets, you can only do that in C23
            FILE *f = fopen(LINUX_SYSFS_DMI_TABLE, "r");
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
    if (p + len >=end) return end; // formatted section exceeds buffer

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
char* DMIString(const uint8_t *p, uint8_t length, uint8_t index, const uint8_t *end) {
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

    char *copy = malloc(len + 1);
    if (!copy) return LAZYBIOS_NULL;

    memcpy(copy, str, len);
    copy[len] = '\0';
    return copy;
}

// Parses the SMBIOS Entry Point structure
int lazybiosParseEntry(lazybiosCTX_t* ctx, const uint8_t* entry_buf) {
    if (memcmp(entry_buf, SMBIOS3_ANCHOR, 5) == 0) {
        // SMBIOS 3.x Entry Point
        ctx->entry_info.major = entry_buf[SMBIOS3_MAJOR_OFFSET];
        ctx->entry_info.minor = entry_buf[SMBIOS3_MINOR_OFFSET];
        ctx->entry_info.docrev = entry_buf[SMBIOS3_DOCREV_OFFSET];
        ctx->entry_info.table_length = *(uint32_t*)(entry_buf + SMBIOS3_TABLE_LENGTH);
        ctx->entry_info.table_address = *(uint64_t*)(entry_buf + SMBIOS3_TABLE_ADDRESS);
        ctx->entry_info.n_structures = LAZYBIOS_NOT_FOUND_U16; // Not available in SMBIOS 3.x
        ctx->entry_info.is_64bit = true;
    } else if (memcmp(entry_buf, SMBIOS2_DMI_ANCHOR,5 ) == 0 || memcmp(entry_buf, SMBIOS2_ANCHOR, 4) == 0) {
        // SMBIOS 2.x Entry Point
        ctx->entry_info.major = entry_buf[SMBIOS2_MAJOR_OFFSET];
        ctx->entry_info.minor = entry_buf[SMBIOS2_MINOR_OFFSET];
        ctx->entry_info.docrev = LAZYBIOS_NOT_FOUND_U8; // Not available in SMBIOS 2.x
        ctx->entry_info.table_length = *(uint16_t*)(entry_buf + SMBIOS2_TABLE_LENGTH);
        ctx->entry_info.table_address = *(uint32_t*)(entry_buf + SMBIOS2_TABLE_ADDRESS);
        ctx->entry_info.n_structures = *(uint16_t*)(entry_buf + SMBIOS2_N_STRUCTURES); // Number of structures
        ctx->entry_info.is_64bit = false;
    } else {
        lb_log("SMBIOS anchor not found");
        lb_dbg("anchor bytes: %02X %02X %02X %02X %02X  (SMBIOS-2 only uses first 4)", entry_buf[0], entry_buf[1], entry_buf[2], entry_buf[3], entry_buf[4]);
        return -1;
    }

    return 0;
}

// Counts the number of structures of a given type
size_t lazybiosCountStructsByType(const lazybiosCTX_t* ctx, uint8_t target_type, size_t min_length) {
    if (!ctx || !ctx->dmi_data) return 0;

    size_t count = 0;
    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;

    while (p + 4 < end) { // SMBIOS_HEADER_SIZE
        uint8_t type = p[0];
        if (type == SMBIOS_TYPE_END) break;

        if (type == target_type && p[1] >= min_length)
            count++;

        p = DMINext(p, end);
    }

    return count;
}

// Small Helper
void lazybiosPrintVer(const lazybiosCTX_t* ctx) {
    if (!ctx) return;
    if (ISVERPLUS(ctx, 3, 0)) {
        printf("SMBIOS version %d.%d.%d\n",
               ctx->entry_info.major,
               ctx->entry_info.minor,
               ctx->entry_info.docrev);
    } else {
        printf("SMBIOS version %d.%d\n",
               ctx->entry_info.major,
               ctx->entry_info.minor);
    }
}

int lazybiosCleanup(lazybiosCTX_t* ctx) {
    if (!ctx) return -1;

    lazybiosFreeType0(ctx->Type0);
    ctx->Type0 = LAZYBIOS_NULL;

    lazybiosFreeType1(ctx->Type1);
    ctx->Type1 = LAZYBIOS_NULL;

    lazybiosFreeType2(ctx->Type2);
    ctx->Type2 = LAZYBIOS_NULL;

    lazybiosFreeType3(ctx->Type3);
    ctx->Type3 = LAZYBIOS_NULL;

    free(ctx->dmi_data);
    free(ctx->entry_data);
    free(ctx);
    return 0;
}



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
    #define LB_OS_WINDOWS 1

#elif defined(__linux__) || defined(linux) || defined(__linux) || defined(__gnu_linux__)
    #define LB_OS_LINUX 1

#elif defined(__APPLE__) && defined(__MACH__)
    #define LB_OS_MACOS 1

#else
    #define LB_OS_UNKNOWN 1
#endif

// Logging system for lazybios

// Internal helper function
static inline void lazybios_log_internal(const char *prefix, const char *fmt, ...) {
    lb_va_list args;
    lb_va_start(args, fmt);
    lb_fprintf(stderr, "%s", prefix);    // print the prefix
    lb_vfprintf(stderr, fmt, args);      // print user message
    lb_fprintf(stderr, "\n");            // newline
    lb_va_end(args);
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

int lazybiosFile(lazybiosCTX_t* ctx, const char* entry_path, const char* dmi_path) {
    if (!ctx) return -1;

    lb_FILE *entry = lb_fopen(entry_path, "rb");
    if (!entry) {
        lb_log("failed to open %s: %s", entry_path, lb_strerror(errno));
        return -1;
    }

    lb_FILE *dmi = lb_fopen(dmi_path, "rb");
    if (!dmi) {
        lb_log("Failed to open %s: %s", dmi_path, lb_strerror(errno));
        lb_fclose(entry);
        return -1;
    }

    uint8_t entry_buf[64];
    size_t n = lb_fread(entry_buf, 1, sizeof(entry_buf), entry);
    ctx->entry_len = n;
    ctx->entry_data = lb_malloc(ctx->entry_len);
    if (!ctx->entry_data) {
        lb_log("Failed to allocate memory for entry_data");
        lb_fclose(entry);
        return -1;
    }

    lb_memcpy(ctx->entry_data, entry_buf, ctx->entry_len);
    lb_fclose(entry);

    if (n < 20) {
        lb_log("Invalid SMBIOS entry point (%zu bytes)", n);
        lb_fclose(dmi);
        return -1;
    }

    if (lazybiosParseEntry(ctx, entry_buf) != 0) {
        lb_fclose(dmi);
        return -1;
    }

    if (lb_fseek(dmi, 0, SEEK_END) != 0) {
        lb_log("Failed to seek in DMI table");
        lb_fclose(dmi);
        return -1;
    }

    long len = lb_ftell(dmi);
    if (len <= 0) {
        lb_log("Invalid or empty DMI table");
        lb_dbg("lb_ftell returned %ld", len);
        lb_fclose(dmi);
        return -1;
    }

    ctx->dmi_len = (size_t)len;
    lb_rewind(dmi);

    ctx->dmi_data = lb_malloc(ctx->dmi_len);
    if (!ctx->dmi_data) {
        lb_log("Failed to allocate %zu bytes for DMI buffer", ctx->dmi_len);
        lb_fclose(dmi);
        return -1;
    }

    size_t got = lb_fread(ctx->dmi_data, 1, ctx->dmi_len, dmi);
    lb_fclose(dmi);

    if (got != ctx->dmi_len) {
        lb_log("Short read in DMI table");
        lb_free(ctx->dmi_data);
        ctx->dmi_data = LAZYBIOS_NULL;
        return -1;
    }
    return 0;
}

// ===== Context Management =====
lazybiosCTX_t* lazybiosCTXNew(void) {
    lazybiosCTX_t *ctx = lb_calloc(1, sizeof(*ctx));
    if (!ctx) return LAZYBIOS_NULL;

    #if LB_OS_LINUX
        ctx->backend = LAZYBIOS_BACKEND_LINUX;
    #elif LB_OS_WINDOWS
        ctx->backend = LAZYBIOS_BACKEND_WINDOWS;
    #elif LB_OS_MACOS
        ctx->backend = LAZYBIOS_BACKEND_MACOS;
    #else
        ctx->backend = LAZYBIOS_BACKEND_UNKNOWN;
    #endif

    return ctx;
}

int lazybiosDevMem(lazybiosCTX_t* ctx) {
    if (!ctx) return -1;

    #if LB_OS_LINUX
        lb_log("/dev/mem backend not implemented yet");
        return -1;
    #else
        return -1;
    #endif
}

int lazybiosWindows(lazybiosCTX_t *ctx) {
    if (!ctx) return -1;

    // Most of the Windows backend wasn't made by me I got help from AI for this since I don't know anything about the WindowsAPI
    #if LB_OS_WINDOWS
        // RSMB provider ID (little endian for "RSMB")
        const DWORD sig = 0x52534D42;

        // Query required size
        DWORD size = GetSystemFirmwareTable(sig, 0, LAZYBIOS_NULL, 0);
        if (size == 0) {
            lb_log("GetSystemFirmwareTable failed (size=0)");
            return -1;
        }

        uint8_t *buf = lb_malloc(size);
        if (!buf) return -1;

        // Fetch the table
        DWORD got = GetSystemFirmwareTable(sig, 0, buf, size);
        if (got != size) {
            lb_log("SMBIOS read mismatch (%lu != %lu)", (unsigned long)got, (unsigned long)size);
            lb_free(buf);
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
            lb_free(buf);
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

        uint8_t *table = lb_malloc(dmi_len);
        if (!table) {
            lb_free(buf);
            return -1;
        }

        lb_memcpy(table, raw->SMBIOSTableData, dmi_len);

        // Free the original RSMB buffer — we only keep the DMI payload
        lb_free(buf);

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
    #ifdef LB_OS_MACOS
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
            lb_FILE *f = lb_fopen(LINUX_SYSFS_DMI_TABLE, "r");
            if (!f) {
                return lazybiosDevMem(ctx);
            }
            lb_fclose(f);
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
    if (p + len > end) return end; // formatted section exceeds buffer

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

    char *copy = lb_malloc(len + 1);
    if (!copy) return LAZYBIOS_NULL;

    lb_memcpy(copy, str, len);
    copy[len] = '\0';
    return copy;
}

// Parses the SMBIOS Entry Point structure
int lazybiosParseEntry(lazybiosCTX_t* ctx, const uint8_t* entry_buf) {
    if (lb_memcmp(entry_buf, SMBIOS3_ANCHOR, 5) == 0) {
        // SMBIOS 3.x Entry Point
        ctx->entry_info.major = entry_buf[SMBIOS3_MAJOR_OFFSET];
        ctx->entry_info.minor = entry_buf[SMBIOS3_MINOR_OFFSET];
        ctx->entry_info.docrev = entry_buf[SMBIOS3_DOCREV_OFFSET];
        ctx->entry_info.table_length = *(uint32_t*)(entry_buf + SMBIOS3_TABLE_LENGTH);
        ctx->entry_info.table_address = *(uint64_t*)(entry_buf + SMBIOS3_TABLE_ADDRESS);
        ctx->entry_info.n_structures = LAZYBIOS_NOT_FOUND_U16; // Not available in SMBIOS 3.x
        ctx->entry_info.is_64bit = true;
    } else if (lb_memcmp(entry_buf, SMBIOS2_DMI_ANCHOR,5 ) == 0 || lb_memcmp(entry_buf, SMBIOS2_ANCHOR, 4) == 0) {
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
        lb_printf("SMBIOS version %d.%d.%d\n",
               ctx->entry_info.major,
               ctx->entry_info.minor,
               ctx->entry_info.docrev);
    } else {
        lb_printf("SMBIOS version %d.%d\n",
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

    lb_free(ctx->dmi_data);
    lb_free(ctx->entry_data);
    lb_free(ctx);
    return 0;
}



#ifndef LAZYBIOS_H
#define LAZYBIOS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

// lazybios version
#define LAZYBIOS_VER "3.0.0"

// SMBIOS offsets
#define SMBIOS3_ANCHOR              "_SM3_"
#define SMBIOS3_MAJOR_OFFSET        0x07
#define SMBIOS3_MINOR_OFFSET        0x08
#define SMBIOS3_DOCREV_OFFSET       0x09
#define SMBIOS3_TABLE_LENGTH        0x0C
#define SMBIOS3_TABLE_ADDRESS       0x10

#define SMBIOS2_ANCHOR          "_SM_"
#define SMBIOS2_MAJOR_OFFSET    0x06
#define SMBIOS2_MINOR_OFFSET    0x07
#define SMBIOS2_TABLE_LENGTH    0x16
#define SMBIOS2_TABLE_ADDRESS   0x18
#define SMBIOS2_N_STRUCTURES    0x1C

#define SMBIOS_TYPE_BIOS                    0
#define SMBIOS_TYPE_SYSTEM                  1
#define SMBIOS_TYPE_BASEBOARD               2
#define SMBIOS_TYPE_CHASSIS                 3
#define SMBIOS_TYPE_PROCESSOR               4
#define SMBIOS_TYPE_CACHES                  7
#define SMBIOS_TYPE_PORT_CONNECTOR          8
#define SMBIOS_TYPE_ONBOARD_DEVICES         10
#define SMBIOS_TYPE_OEM_STRINGS             11
#define SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY   16
#define SMBIOS_TYPE_MEMORY_DEVICE           17
#define SMBIOS_TYPE_END                     127

// Helper macro to check SMBIOS version with needed version
#define ISVERPLUS(ctx, req_major, req_minor) \
(((ctx)->entry_info.major > (req_major)) || \
((ctx)->entry_info.major == (req_major) && (ctx)->entry_info.minor >= (req_minor))) // Returns 1 if the version is equal or newer and 0 if its older

// Values for not found when the version is smaller than the fields require
#define LAZYBIOS_NOT_FOUND_U8   0xAB
#define LAZYBIOS_NOT_FOUND_U16  0xABCD
#define LAZYBIOS_NOT_FOUND_U32  0xABCDEFAA
#define LAZYBIOS_NOT_FOUND_U64  0xABCDEFABCDEFABCDULL
#define LAZYBIOS_NULL           NULL // Why not, just being consistent here
#define LAZYBIOS_NOT_FOUND_STR  "Not Present"

// ===== General Constants =====
#define SMBIOS_HEADER_SIZE 4
#define SMBIOS_ENTRY       "/sys/firmware/dmi/tables/smbios_entry_point"
#define DMI_TABLE          "/sys/firmware/dmi/tables/DMI"
#define DEV_MEM            "/dev/mem"

// ===== Data Structures =====
typedef struct {
    uint8_t major;
    uint8_t minor;
    uint8_t docrev;  // Only valid for SMBIOS 3.x
    uint32_t table_length;
    uint64_t table_address;
    uint16_t n_structures;
    bool is_64bit;
} smbios_entry_info_t;

// Type 0 Structure
typedef struct {
    char *vendor;
    char *version;
    char *release_date;

    // --- (SMBIOS 2.0+) ---
    uint16_t bios_starting_segment;

    uint32_t rom_size;
    uint64_t characteristics;

    // --- SMBIOS 2.1+  ---
    size_t firmware_char_ext_bytes_count; // Needed to find out how many of firmware characteristics extension bytes are available.
    uint8_t *firmware_char_ext_bytes;

    // --- SMBIOS 2.4+   ---
    uint8_t platform_major_release;
    uint8_t platform_minor_release;
    uint8_t ec_major_release;
    uint8_t ec_minor_release;

    // --- SMBIOS 3.1+  ---
    uint16_t extended_rom_size;
    char unit[5]; // Uint for Extended ROM Size
} lazybiosType0_t;

// End of Type 0


typedef enum {
    LAZYBIOS_BACKEND_LINUX, // Only Sysfs Currently but /dev/mem coming soon
    LAZYBIOS_BACKEND_WINDOWS, // Implemented through Windows API
    LAZYBIOS_BACKEND_MACOS, // Not Implemented Yet
    LAZYBIOS_BACKEND_UNKNOWN,
} lazybiosBackend_t;

typedef struct lazybios_ctx {
    lazybiosBackend_t backend;
    uint8_t *dmi_data;
    size_t dmi_len;
    smbios_entry_info_t entry_info;

    lazybiosType0_t *Type0;
} lazybiosCTX_t;

// ===== Public API =====
lazybiosCTX_t* lazybiosCTXNew(void);

int lazybiosInit(lazybiosCTX_t* ctx);
int lazybiosSysfs(lazybiosCTX_t* ctx);
int lazybiosFile(lazybiosCTX_t* ctx, const char* entry_path, const char* dmi_path);
int lazybiosCleanup(lazybiosCTX_t* ctx);

// Core parsing functions
char* DMIString(const uint8_t *p, uint8_t length, uint8_t index, const uint8_t *end);
const uint8_t* DMINext(const uint8_t *ptr, const uint8_t *end);
size_t lazybiosCountStructsByType(const lazybiosCTX_t* ctx, uint8_t target_type, size_t min_length);
int lazybiosParseEntry(lazybiosCTX_t* ctx, const uint8_t* buf);

// Basic functions
void lazybiosPrintVer(const lazybiosCTX_t* ctx);
const smbios_entry_info_t* lazybiosGetEntryInfo(const lazybiosCTX_t* ctx);

// Type 0 + Helpers

lazybiosType0_t* lazybiosGetType0(lazybiosCTX_t* ctx);
const char* lazybiosFirmwareCharacteristicsStr(uint64_t characteristics);
const char* lazybiosFirmwareCharacteristicsExtByte1Str(uint8_t char_ext_byte_1);
const char* lazybiosFirmwareCharacteristicsExtByte2Str(uint8_t char_ext_byte_2);
uint16_t lazybiosFirmwareExtendedROMSizeU16(uint16_t raw, char unit[5]);
void lazybiosFreeType0(lazybiosType0_t* Type0);

// End of Type 0

#endif

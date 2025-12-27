#ifndef LAZYBIOS_H
#define LAZYBIOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

// I was thinking of making lazybios libc independent in the future, so right now I'm just going to replace libc function names with my own to make future me's life easier
// Who knows when that time will come, but let's just prepare!
#define lb_malloc   malloc
#define lb_calloc   calloc
#define lb_free     free
#define lb_printf   printf
#define lb_fprintf  fprintf
#define lb_snprintf snprintf
#define lb_memcpy   memcpy
#define lb_memcmp   memcmp
#define lb_strcpy   strcpy
#define lb_va_list  va_list
#define lb_va_start va_start
#define lb_va_end   va_end
#define lb_strcmp   strcmp
#define lb_vfprintf vfprintf
#define lb_FILE     FILE
#define lb_fopen    fopen
#define lb_fclose   fclose
#define lb_fread    fread
#define lb_fwrite   fwrite
#define lb_fseek    fseek
#define lb_ftell    ftell
#define lb_rewind   rewind
#define lb_strdup   strdup
#define lb_strerror strerror

// lazybios version
#define LAZYBIOS_VER "3.2.0"

// SMBIOS offsets
#define SMBIOS3_ANCHOR              "_SM3_"
#define SMBIOS3_MAJOR_OFFSET        0x07
#define SMBIOS3_MINOR_OFFSET        0x08
#define SMBIOS3_DOCREV_OFFSET       0x09
#define SMBIOS3_TABLE_LENGTH        0x0C
#define SMBIOS3_TABLE_ADDRESS       0x10

#define SMBIOS2_ANCHOR          "_SM_"
#define SMBIOS2_DMI_ANCHOR      "_DMI_"
#define SMBIOS2_MAJOR_OFFSET    0x06
#define SMBIOS2_MINOR_OFFSET    0x07
#define SMBIOS2_TABLE_LENGTH    0x16
#define SMBIOS2_TABLE_ADDRESS   0x18
#define SMBIOS2_N_STRUCTURES    0x1C

#define SMBIOS_TYPE_BIOS                    0
#define SMBIOS_TYPE_SYSTEM                  1
#define SMBIOS_TYPE_BASEBOARD               2
#define SMBIOS_TYPE_CHASSIS                 3
#define SMBIOS_TYPE_PROCESSOR               41
#define SMBIOS_TYPE_CACHES                  7
#define SMBIOS_TYPE_PORT_CONNECTOR          8
#define SMBIOS_TYPE_ONBOARD_DEVICES         10
#define SMBIOS_TYPE_OEM_STRINGS             11
#define SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY   16
#define SMBIOS_TYPE_MEMORY_DEVICE           17
#define SMBIOS_TYPE_END                     127

// Helper macros
#define ISVERPLUS(ctx, req_major, req_minor) (((ctx)->entry_info.major > (req_major)) || ((ctx)->entry_info.major == (req_major) && (ctx)->entry_info.minor >= (req_minor))) // Returns 1 if the version is equal or newer and 0 if its older
#define GET_STRING(offset) ((len > (offset)) ? DMIString(p, len, p[(offset)], end) : LAZYBIOS_NULL)

// Values for not found
#define LAZYBIOS_NOT_FOUND_U8     0xFF
#define LAZYBIOS_NOT_FOUND_U16    0xFFFF
#define LAZYBIOS_NOT_FOUND_U32    0xFFFFFFFF
#define LAZYBIOS_NOT_FOUND_U64    0xFFFFFFFFFFFFFFFFULL
#define LAZYBIOS_NULL             NULL
#define LAZYBIOS_NOT_FOUND_STR    "Not Present"

// ===== General Constants =====
#define SMBIOS_HEADER_SIZE             4
#define LINUX_SYSFS_SMBIOS_ENTRY       "/sys/firmware/dmi/tables/smbios_entry_point"
#define LINUX_SYSFS_DMI_TABLE          "/sys/firmware/dmi/tables/DMI"
#define DEV_MEM                        "/dev/mem"

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

typedef struct {
    // --- (SMBIOS 2.0+) ---
    char *vendor;
    char *version;
    char *release_date;
    uint16_t bios_starting_segment;
    uint32_t rom_size;
    uint64_t characteristics; // Decoder Available

    // --- SMBIOS 2.1+  ---
    size_t firmware_char_ext_bytes_count; // Needed to find out how many of firmware characteristics extension bytes are available.
    uint8_t *firmware_char_ext_bytes; // 2 Decoders Available

    // --- SMBIOS 2.4+   ---
    uint8_t platform_major_release;
    uint8_t platform_minor_release;
    uint8_t ec_major_release;
    uint8_t ec_minor_release;

    // --- SMBIOS 3.1+  ---
    uint16_t extended_rom_size; // Decoder Available
    char unit[5]; // Uint for Extended ROM Size
} lazybiosType0_t;

typedef struct {
    // --- (SMBIOS 2.0+) ---
    char *manufacturer;
    char *product_name;
    char *version;
    char *serial_number;

    // --- (SMBIOS 2.1+) ---
    uint8_t uuid[16]; // As per DMTF specs this field is 16 BYTEs.
    uint8_t wake_up_type; // The event that caused the system to wake up.

    // --- (SMBIOS 2.4+) ---
    char *sku_number;
    char *family;
} lazybiosType1_t;

typedef struct {
    // In this field I don't really see a "Version" column, I think this type has the same fields from SMBIOS 2.x to 3.x.x , I will still do some other research to make sure its true!
    char *manufacturer;
    char *product;
    char *version;
    char *serial_number;
    char *asset_tag;
    uint8_t feature_flags;
    char *location_in_chassis;
    uint16_t chassis_handle;
    uint8_t board_type;
    uint8_t number_of_contained_object_handles; // n
    uint16_t *contained_object_handles; // DMTF says this is n WORDs, so that means we allocate number_of_contained_object_handles of memory.
} lazybiosType2_t;

typedef struct {
    // --- (SMBIOS 2.0+) ---
    char *manufacturer;
    uint8_t type;
    char *version;
    char *serial_number;
    char *asset_tag;

    // --- (SMBIOS 2.1+) ---
    uint8_t boot_up_state;
    uint8_t power_supply_state;
    uint8_t thermal_state;
    uint8_t security_status;

    // --- (SMBIOS 2.3+) ---
    uint32_t oem_defined;
    uint8_t height;
    uint8_t number_of_power_cords;
    uint8_t contained_element_count; // n
    uint8_t contained_element_record_length; // m
    uint8_t *contained_elements; // n * m BYTEs
      
    // --- (SMBIOS 2.7+) ---
    char *sku_number; // offset is 0x15 + n * m
    
    // --- (SMBIOS 3.9+) ---
    uint8_t rack_type; // offset is 0x16 + n * m
    uint8_t rack_height; // offset is 0x17 + n * m
} lazybiosType3_t;

typedef enum { // I'm looking to implement more OSes but right now and for a long time I'm mostly going to focus on Linux.
    LAZYBIOS_BACKEND_LINUX, // Only Sysfs Currently but /dev/mem coming some day
    LAZYBIOS_BACKEND_WINDOWS, // Using Windows API
    LAZYBIOS_BACKEND_MACOS, // Not Implemented Yet
    LAZYBIOS_BACKEND_UNKNOWN,
} lazybiosBackend_t;

typedef struct {
    lazybiosBackend_t backend;
    uint8_t *dmi_data;
    size_t dmi_len;
    uint8_t *entry_data;
    size_t entry_len;
    smbios_entry_info_t entry_info;

    lazybiosType0_t *Type0;
    lazybiosType1_t *Type1;
    lazybiosType2_t *Type2;
    lazybiosType3_t *Type3;
} lazybiosCTX_t;

// ===== Public API =====
lazybiosCTX_t* lazybiosCTXNew(void);

int lazybiosInit(lazybiosCTX_t* ctx);
// int lazybiosSysfs(lazybiosCTX_t* ctx); - I used to have this but now its deprecated since we can use lazybiosFile directly instead
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
const char* lazybiosType0CharacteristicsStr(uint64_t characteristics);
const char* lazybiosType0CharacteristicsExtByte1Str(uint8_t char_ext_byte_1);
const char* lazybiosType0CharacteristicsExtByte2Str(uint8_t char_ext_byte_2);
uint16_t lazybiosType0ExtendedROMSizeU16(uint16_t raw, char unit[5]);
void lazybiosFreeType0(lazybiosType0_t* Type0);

// End of Type 0


// Type 1 + Helpers

lazybiosType1_t* lazybiosGetType1(lazybiosCTX_t* ctx);
const char* lazybiosType1WakeupTypeStr(uint8_t wake_up_type);
void lazybiosFreeType1(lazybiosType1_t* Type1);

// End of Type 1


// Type 2 + Helpers

lazybiosType2_t* lazybiosGetType2(lazybiosCTX_t* ctx);
const char* lazybiosType2FeatureflagsStr(uint8_t feature_flags);
const char* lazybiosType2BoardTypeStr(uint8_t board_type);
void lazybiosFreeType2(lazybiosType2_t* Type2);

// End of Type 2


// Type 3 + Helpers

lazybiosType3_t* lazybiosGetType3(lazybiosCTX_t* ctx);
const char* lazybiosType3TypeStr(uint8_t type);
const char* lazybiosType3StateStr(uint8_t state);
const char* lazybiosType3SecurityStatusStr(uint8_t security_status);
const char* lazybiosType3ContainedElementTypeStr(uint8_t contained_elements);
void lazybiosFreeType3(lazybiosType3_t* Type3);

// End of Type 3


#ifdef __cplusplus
}
#endif

#endif

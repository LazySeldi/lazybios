/**
 * @file lazybios.h
 * @brief Public umbrella API for reading and decoding SMBIOS/DMI data.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_SHARED_API_H
/** @brief Include guard for definitions shared by all lazybios public headers. */
#define LAZYBIOS_SHARED_API_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup api_constants
 * @{
 */

/** @brief Complete lazybios semantic version string. */
#define LAZYBIOS_VER "0.5.0"
/** @brief Major component of the lazybios version. */
#define LAZYBIOS_MAJOR 0
/** @brief Minor component of the lazybios version. */
#define LAZYBIOS_MINOR 5
/** @brief Patch component of the lazybios version. */
#define LAZYBIOS_PATCH 0
/** @brief Recommended output buffer size for decoder functions. */
#define LAZYBIOS_DECODER_BUF_SIZE 256
/** @brief Size of the formatted header shared by SMBIOS structures. */
#define SMBIOS_HEADER_SIZE 4
/** @brief Linux sysfs path for the SMBIOS entry point. */
#define LINUX_SYSFS_SMBIOS_ENTRY "/sys/firmware/dmi/tables/smbios_entry_point"
/** @brief Linux sysfs path for the DMI structure table. */
#define LINUX_SYSFS_DMI_TABLE "/sys/firmware/dmi/tables/DMI"
/** @brief Linux device path used for physical-memory SMBIOS access. */
#define DEV_MEM "/dev/mem"

/** @} */

/**
 * @brief Describes whether a parsed SMBIOS field was available and valid.
 * @ingroup api_parsing
 */
typedef enum {
	LAZYBIOS_FIELD_ABSENT = 0, /**< The field is not encoded in the formatted structure. */
	LAZYBIOS_FIELD_PRESENT     /**< The field is encoded and available to the caller. */
} lazybiosFieldStatus_t;

/**
 * @brief Returns the status associated with a parsed structure field.
 * @param structure Non-NULL pointer to a parsed SMBIOS structure.
 * @param field Public field name in the parsed structure.
 */
#define LAZYBIOS_FIELD_STATUS(structure, field) ((structure)->field_status.field)

/**
 * @brief Raw SMBIOS 2.x entry point layout.
 * @ingroup api_entry
 */
typedef struct {
	uint8_t anchor[4];
	uint8_t checksum;
	uint8_t entry_point_length;
	uint8_t major_version;
	uint8_t minor_version;
	uint16_t maximum_structure_size;
	uint8_t entry_point_revision;
	uint8_t formatted_area[5];
	uint8_t intermediate_anchor[5];
	uint8_t intermediate_checksum;
	uint16_t structure_table_length;
	uint32_t structure_table_address;
	uint16_t structure_count;
	uint8_t bcd_revision;
} lazybiosSMBIOS2Entry;

/**
 * @brief Raw SMBIOS 3.x entry point layout.
 * @ingroup api_entry
 */
typedef struct {
	uint8_t anchor[5];
	uint8_t checksum;
	uint8_t entry_point_length;
	uint8_t major_version;
	uint8_t minor_version;
	uint8_t docrev;
	uint8_t entry_point_revision;
	uint8_t reserved;
	uint32_t structure_table_max_size;
	uint64_t structure_table_address;
} lazybiosSMBIOS3Entry;

/**
 * @brief Identifies the SMBIOS entry point layout stored in a DMI container.
 * @ingroup api_entry
 */
typedef enum {
	SMBIOS_VER_UNKNOWN = 0, /**< No supported entry point has been parsed. */
	SMBIOS_VER_2X,          /**< The SMBIOS 2.x entry point member is active. */
	SMBIOS_VER_3X           /**< The SMBIOS 3.x entry point member is active. */
} lazybiosSMBIOSVersionTag;

/**
 * @brief Owns raw DMI table data and its parsed SMBIOS entry point.
 * @ingroup api_entry
 */
typedef struct lazybiosDMI {
	uint8_t* dmi_data;
	size_t dmi_len;
	uint8_t* entry_data;
	size_t entry_len;
	lazybiosSMBIOSVersionTag entry_tag;
	union {
		lazybiosSMBIOS2Entry* v2;
		lazybiosSMBIOS3Entry* v3;
	} entry_union;
} lazybiosDMI_t;

/**
 * @brief Tests whether the parsed SMBIOS version meets a minimum version.
 *
 * @param DMIData Raw DMI table container with a parsed SMBIOS entry point.
 * @param required_major Required SMBIOS major version.
 * @param required_minor Required SMBIOS minor version.
 * @return Nonzero when the parsed version is equal to or newer than the required version; otherwise zero.
 * @ingroup api_entry
 */
int lazybiosIsVersionPlus(const lazybiosDMI_t* DMIData, uint8_t required_major, uint8_t required_minor);

/** @brief Forward declaration of the top-level lazybios context. */
typedef struct lazybiosCTX lazybiosCTX_t;

/** @addtogroup api_parsing
 * @{
 */

/**
 * @brief Copies a string from an SMBIOS structure's string-set.
 * @param p Start of the SMBIOS structure.
 * @param length Length of the structure's formatted section.
 * @param index One-based index of the requested string.
 * @param end One-past-the-end address of the DMI table buffer.
 * @return Newly allocated string, or NULL if the string is unavailable or invalid.
 */
char* DMIString(const uint8_t* p, uint8_t length, uint8_t index, const uint8_t* end);

/**
 * @brief Locates the next SMBIOS structure in a DMI table.
 * @param p Start of the current SMBIOS structure.
 * @param end One-past-the-end address of the DMI table buffer.
 * @return Pointer to the next structure, or end when no complete structure remains.
 */
const uint8_t* DMINext(const uint8_t* p, const uint8_t* end);

/**
 * @brief Counts SMBIOS structures having a specified type identifier.
 * @param DMIData Raw DMI table container to inspect.
 * @param target_type SMBIOS structure type identifier to count.
 * @return Number of matching structures in the table.
 */
size_t lazybiosCountStructsByType(const lazybiosDMI_t* DMIData, uint8_t target_type);

/**
 * @brief Validates and identifies an SMBIOS entry point.
 * @param ctx Context whose entry tag and tagged union are updated.
 * @param entry_buf Buffer containing an SMBIOS 2.x or 3.x entry point.
 * @param buf_len Length of entry_buf in bytes.
 * @return 0 on success, or -1 if the entry point is invalid.
 */
int lazybiosParseEntry(lazybiosCTX_t* ctx, const uint8_t* entry_buf, size_t buf_len);

/**
 * @brief Prints the parsed SMBIOS version to standard output.
 * @param ctx Context containing a parsed SMBIOS entry point.
 */
void lazybiosPrintVer(const lazybiosCTX_t* ctx);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

#if !defined(LAZYBIOS_TYPE_HEADER_ONLY) && !defined(LAZYBIOS_UMBRELLA_API_H)
/** @brief Include guard for the complete lazybios public API. */
#define LAZYBIOS_UMBRELLA_API_H

#include "lazybios/type0.h"
#include "lazybios/type1.h"
#include "lazybios/type2.h"
#include "lazybios/type3.h"
#include "lazybios/type4.h"
#include "lazybios/type5.h"
#include "lazybios/type6.h"
#include "lazybios/type7.h"
#include "lazybios/type8.h"
#include "lazybios/type9.h"
#include "lazybios/type10.h"
#include "lazybios/type11.h"
#include "lazybios/type12.h"
#include "lazybios/type13.h"
#include "lazybios/type14.h"
#include "lazybios/type15.h"
#include "lazybios/type16.h"
#include "lazybios/type17.h"
#include "lazybios/type18.h"
#include "lazybios/type19.h"
#include "lazybios/type20.h"
#include "lazybios/type21.h"
#include "lazybios/type22.h"
#include "lazybios/type23.h"
#include "lazybios/type24.h"
#include "lazybios/type25.h"
#include "lazybios/type26.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Selects the platform backend used to obtain SMBIOS data.
 * @ingroup api_context
 */
typedef enum {
	LAZYBIOS_BACKEND_LINUX,
	LAZYBIOS_BACKEND_WINDOWS,
	LAZYBIOS_BACKEND_MACOS,
	LAZYBIOS_BACKEND_UNKNOWN
} lazybiosBackend_t;

/**
 * @brief Top-level lazybios context containing raw and parsed SMBIOS data.
 * @ingroup api_context
 */
struct lazybiosCTX {
	lazybiosBackend_t backend;
	lazybiosDMI_t* DMIData;

	lazybiosType0_t* Type0;

	lazybiosType1_t* Type1;

	lazybiosType2_t* Type2;
	size_t type2_count;

	lazybiosType3_t* Type3;
	size_t type3_count;

	lazybiosType4_t* Type4;
	size_t type4_count;

	lazybiosType5_t* Type5;
	size_t type5_count;

	lazybiosType6_t* Type6;
	size_t type6_count;

	lazybiosType7_t* Type7;
	size_t type7_count;

	lazybiosType8_t* Type8;
	size_t type8_count;

	lazybiosType9_t* Type9;
	size_t type9_count;

	lazybiosType10_t* Type10;
	size_t type10_count;

	lazybiosType11_t* Type11;
	size_t type11_count;

	lazybiosType12_t* Type12;
	size_t type12_count;

	lazybiosType13_t* Type13;
	size_t type13_count;

	lazybiosType14_t* Type14;
	size_t type14_count;

	lazybiosType15_t* Type15;
	size_t type15_count;

	lazybiosType16_t* Type16;
	size_t type16_count;

	lazybiosType17_t* Type17;
	size_t type17_count;

	lazybiosType18_t* Type18;
	size_t type18_count;

	lazybiosType19_t* Type19;
	size_t type19_count;

	lazybiosType20_t* Type20;
	size_t type20_count;

	lazybiosType21_t* Type21;
	size_t type21_count;

	lazybiosType22_t* Type22;
	size_t type22_count;

	lazybiosType23_t* Type23;
	size_t type23_count;

	lazybiosType24_t* Type24;
	size_t type24_count;

	lazybiosType25_t* Type25;
	size_t type25_count;

	lazybiosType26_t* Type26;
	size_t type26_count;
};

/**
 * @brief Allocates and initializes a lazybios context.
 * @return Newly allocated context, or NULL if allocation fails.
 */
lazybiosCTX_t* lazybiosCTXNew(void);

/**
 * @brief Loads SMBIOS data using the context's selected platform backend.
 * @param ctx Context that receives the raw entry point and DMI table data.
 * @return 0 on success, or -1 on failure.
 */
int lazybiosInit(lazybiosCTX_t* ctx);

/**
 * @brief Loads an SMBIOS entry point and DMI table from separate files.
 * @param ctx Context that receives the loaded data.
 * @param entry_path Path to the raw SMBIOS entry point file.
 * @param dmi_path Path to the raw DMI structure table file.
 * @return 0 on success, or -1 on failure.
 */
int lazybiosFile(lazybiosCTX_t* ctx, const char* entry_path, const char* dmi_path);

/**
 * @brief Loads SMBIOS entry point and DMI data from one merged file.
 * @param ctx Context that receives the loaded data.
 * @param bin_path Path to a file containing the entry point followed by the DMI table.
 * @return 0 on success, or -1 on failure.
 */
int lazybiosSingleFile(lazybiosCTX_t* ctx, const char* bin_path);

/**
 * @brief Releases a context and all SMBIOS data owned by it.
 * @param ctx Context to release.
 * @return 0 on success, or -1 if ctx is NULL.
 */
int lazybiosCleanup(lazybiosCTX_t* ctx);

#ifdef __cplusplus
}
#endif

#endif

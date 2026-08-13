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
 * @file lazybios.h
 * @brief Public umbrella API for reading and decoding SMBIOS/DMI data.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_SHARED_API_H
/** @brief Include guard for definitions shared by all lazybios public headers. */
#define LAZYBIOS_SHARED_API_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Forward declaration of the top-level lazybios context. */
typedef struct lazybiosCTX lazybiosCTX_t;

/**
 * @brief Selects the platform backend used to obtain SMBIOS data.
 * @ingroup api_context
 */
typedef enum {
	LAZYBIOS_BACKEND_LINUX,   /**< Linux sysfs or physical-memory backend. */
	LAZYBIOS_BACKEND_WINDOWS, /**< Windows firmware-table API backend. */
	LAZYBIOS_BACKEND_MACOS,   /**< macOS AppleSMBIOS I/O Registry backend. */
	LAZYBIOS_BACKEND_OPENBSD, /**< OpenBSD dmesg-assisted physical-memory backend. */
	LAZYBIOS_BACKEND_FREEBSD, /**< FreeBSD kenv-assisted physical-memory backend. */
	LAZYBIOS_BACKEND_NETBSD,  /**< NetBSD sysctl-assisted SMBIOS device backend. */
	LAZYBIOS_BACKEND_SUNOS,    /**< SunOS (Solaris/illumos) /dev/smbios snapshot with a physical-memory fallback. */
	LAZYBIOS_BACKEND_DRAGONFLY, /**< DragonFly BSD kenv-assisted physical-memory backend. */
	LAZYBIOS_BACKEND_HAIKU,     /**< Haiku legacy physical-memory backend. */
	LAZYBIOS_BACKEND_BEOS,      /**< BeOS legacy physical-memory backend. */
	LAZYBIOS_BACKEND_REACTOS,   /**< ReactOS Win32 firmware-table API backend. */
	LAZYBIOS_BACKEND_GENERIC,   /**< Generic legacy physical-memory backend. */
	LAZYBIOS_BACKEND_UNKNOWN    /**< No usable host backend was selected. */
} lazybiosBackend_t;

/** @addtogroup api_constants
 * @{
 */

/** @brief Complete lazybios semantic version string. */
extern const char lazybiosVersion[];

/** @brief Major component of the lazybios version. */
#define LAZYBIOS_MAJOR 2
/** @brief Minor component of the lazybios version. */
#define LAZYBIOS_MINOR 1
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
/** @brief Device path used for physical-memory SMBIOS access on Linux and BSD. */
#define DEV_MEM "/dev/mem"

/** @} */

/**
 * @brief Describes whether a parsed SMBIOS field was available and valid.
 * @ingroup api_parsing
 */
typedef enum {
    LAZYBIOS_FIELD_ABSENT = 0,   /**< The field exists in this SMBIOS version but is not populated by the vendor. */
    LAZYBIOS_FIELD_PRESENT,      /**< The field is encoded and contains valid data. */
    LAZYBIOS_FIELD_UNREACHABLE   /**< The field was added in a newer SMBIOS spec; structure is too short to contain it. */
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
#ifdef __cplusplus
}
#endif

#ifndef LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/structures/type0.h"
#include "lazybios/structures/type1.h"
#include "lazybios/structures/type2.h"
#include "lazybios/structures/type3.h"
#include "lazybios/structures/type4.h"
#include "lazybios/structures/type5.h"
#include "lazybios/structures/type6.h"
#include "lazybios/structures/type7.h"
#include "lazybios/structures/type8.h"
#include "lazybios/structures/type9.h"
#include "lazybios/structures/type10.h"
#include "lazybios/structures/type11.h"
#include "lazybios/structures/type12.h"
#include "lazybios/structures/type13.h"
#include "lazybios/structures/type14.h"
#include "lazybios/structures/type15.h"
#include "lazybios/structures/type16.h"
#include "lazybios/structures/type17.h"
#include "lazybios/structures/type18.h"
#include "lazybios/structures/type19.h"
#include "lazybios/structures/type20.h"
#include "lazybios/structures/type21.h"
#include "lazybios/structures/type22.h"
#include "lazybios/structures/type23.h"
#include "lazybios/structures/type24.h"
#include "lazybios/structures/type25.h"
#include "lazybios/structures/type26.h"
#include "lazybios/structures/type27.h"
#include "lazybios/structures/type28.h"
#include "lazybios/structures/type29.h"
#include "lazybios/structures/type30.h"
#include "lazybios/structures/type31.h"
#include "lazybios/structures/type32.h"
#include "lazybios/structures/type33.h"
#include "lazybios/structures/type34.h"
#include "lazybios/structures/type35.h"
#include "lazybios/structures/type36.h"
#include "lazybios/structures/type37.h"
#include "lazybios/structures/type38.h"
#include "lazybios/structures/type39.h"
#include "lazybios/structures/type40.h"
#include "lazybios/structures/type41.h"
#include "lazybios/structures/type42.h"
#include "lazybios/structures/type43.h"
#include "lazybios/structures/type44.h"
#include "lazybios/structures/type45.h"
#include "lazybios/structures/type46.h"
#include "lazybios/structures/oem/hp/hp_type201.h"
#include "lazybios/json/lazybios_json.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Top-level lazybios context containing raw and parsed SMBIOS data.
 * @ingroup api_context
 */
struct lazybiosCTX {
	lazybiosBackend_t backend;
	lazybiosDMI_t* DMIData;

	lazybiosType0_t* Type0;
	size_t type0_count;

	lazybiosType1_t* Type1;
	size_t type1_count;

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

	lazybiosType27_t* Type27;
	size_t type27_count;

	lazybiosType28_t* Type28;
	size_t type28_count;

	lazybiosType29_t* Type29;
	size_t type29_count;

	lazybiosType30_t* Type30;
	size_t type30_count;

	lazybiosType31_t* Type31;
	size_t type31_count;

	lazybiosType32_t* Type32;
	size_t type32_count;

	lazybiosType33_t* Type33;
	size_t type33_count;

	lazybiosType34_t* Type34;
	size_t type34_count;

	lazybiosType35_t* Type35;
	size_t type35_count;

	lazybiosType36_t* Type36;
	size_t type36_count;

	lazybiosType37_t* Type37;
	size_t type37_count;

	lazybiosType38_t* Type38;
	size_t type38_count;

	lazybiosType39_t* Type39;
	size_t type39_count;

	lazybiosType40_t* Type40;
	size_t type40_count;

	lazybiosType41_t* Type41;
	size_t type41_count;

	lazybiosType42_t* Type42;
	size_t type42_count;

	lazybiosType43_t* Type43;
	size_t type43_count;

	lazybiosType44_t* Type44;
	size_t type44_count;

	lazybiosType45_t* Type45;
	size_t type45_count;

	lazybiosType46_t* Type46;
	size_t type46_count;


    lazybiosOemHpType201_t* HpType201;
    size_t hptype201_count;
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
 * @param bin_path Path to a file containing the entry point and DMI table,
 * either concatenated or separated by an advertised file-relative offset.
 * @return 0 on success, or -1 on failure.
 */
int lazybiosSingleFile(lazybiosCTX_t* ctx, const char* bin_path);

/**
 * @brief Releases a context and all SMBIOS data owned by it.
 * @param ctx Context to release.
 * @return 0 on success, or -1 if ctx is NULL.
 */
int lazybiosCleanup(lazybiosCTX_t* ctx);

/**
 * @brief Prints SMBIOS version information to stdout.
 * @param ctx Initialized lazybios context.
 */
void lazybiosPrintSMVer(const lazybiosCTX_t* ctx);

#endif

#ifdef __cplusplus
}
#endif

#endif

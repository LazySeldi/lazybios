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
 * @brief Public API for lazybios. Include this and nothing else.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_SHARED_API_H

#include <stdint.h>
#include <stddef.h>

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
/*
 * WINDOWS_EXPORT_ALL_SYMBOLS exports this from the DLL on its own, but a
 * consumer still has to reference it through an import stub, so it needs
 * dllimport here. Marking it dllexport instead makes the consumer emit the
 * bare symbol name and the link fails. CMake defines lazybios_EXPORTS only
 * while compiling the library itself.
 */
#if defined(_WIN32) && !defined(LAZYBIOS_STATIC) && !defined(lazybios_EXPORTS)
__declspec(dllimport) extern const char lazybiosVersion[];
#else
extern const char lazybiosVersion[];
#endif

/** @brief Major component of the lazybios version. */
#define LAZYBIOS_MAJOR 3
/** @brief Minor component of the lazybios version. */
#define LAZYBIOS_MINOR 0
/** @brief Patch component of the lazybios version. */
#define LAZYBIOS_PATCH 0
/** @} */

/**
 * @brief Describes whether a parsed SMBIOS field was available and valid.
 * @ingroup api_parsing
 */
typedef enum {
    LAZYBIOS_FIELD_ABSENT = 0,   /**< The field exists in this SMBIOS version but is not populated by the vendor. */
    LAZYBIOS_FIELD_PRESENT,      /**< The field is encoded and contains valid data. */
    LAZYBIOS_FIELD_UNREACHABLE   /**< The field was added in a newer SMBIOS spec; structure is too short to contain it. */
} lazybiosFieldStatusValue_t;

/**
 * @brief Storage for one field's availability.
 *
 * Holds a ::lazybiosFieldStatusValue_t. C99 cannot give an enum a one-byte
 * underlying type, and status metadata is roughly half the size of a parsed
 * structure, so the value is stored as a byte and compared against the
 * enumerators directly.
 */
typedef uint8_t lazybiosFieldStatus_t;

/**
 * @brief Returns the status associated with a parsed structure field.
 * @param structure Non-NULL pointer to a parsed SMBIOS structure.
 * @param field Public field name in the parsed structure.
 */
#define LAZYBIOS_FIELD_STATUS(structure, field) ((structure)->field_status.field)

/*
 * Compiler feature detection.
 *
 * Clang normally predefines __GNUC__, so the second test looks redundant, but
 * clang-cl emulates the MSVC predefines instead while still accepting GNU
 * attributes; without it that configuration would fall through to the SAL
 * branch below.
 */
#if defined(__GNUC__) || defined(__clang__)
/**
 * @brief Marks a function whose return value must not be discarded.
 *
 * The getters return a newly allocated array and the loaders return a status
 * code, so dropping either result leaks memory or hides a failed load.
 */
#  define LAZYBIOS_WARN_UNUSED __attribute__((warn_unused_result))
#elif defined(_MSC_VER)
#  include <sal.h>
/**
 * @brief Marks a function whose return value must not be discarded.
 *
 * Only used by the /analyze flag in the msvc compiler, because microsoft
 */
#  define LAZYBIOS_WARN_UNUSED _Check_return_
#else
/** @brief Expands to nothing on compilers without a return-value check. */
#  define LAZYBIOS_WARN_UNUSED
#endif

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
 * @brief Cached location and population count for one SMBIOS structure type.
 * @ingroup api_entry
 *
 * One entry per possible type byte. Both members are meaningful only while the
 * containing @ref lazybiosDMI_t has a non-zero `index_valid`, and `first` only
 * when `count` is greater than zero.
 */
typedef struct {
	uint32_t count; /**< Number of structures of this type in the table. */
	uint32_t first; /**< Byte offset of the first one, from the table start. */
} lazybiosTypeIndex_t;

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

	/**
	 * Per-type location cache, built once when the table is loaded. A type
	 * byte is a single octet, so 256 entries cover every possible type.
	 * Without it, each parser had to walk the whole table just to size its
	 * result array, which dominated parse time.
	 */
	lazybiosTypeIndex_t index[256];

	/**
	 * Non-zero once @p index describes @p dmi_data. A zero-initialized or
	 * hand-assembled container leaves this clear, and the parsers fall back to
	 * walking the table, so the cache is an optimization rather than a
	 * precondition. Clear it if @p dmi_data is replaced.
	 */
	int index_valid;

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
#include "lazybios/structures/oem/dell/dell_type177.h"
#include "lazybios/structures/oem/dell/dell_type212.h"
#include "lazybios/structures/oem/dell/dell_type218.h"
#include "lazybios/structures/oem/hp/hp_type204.h"
#include "lazybios/json/lazybios_json.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Dell OEM structures held by a context.
 * @ingroup api_oem_types
 */
typedef struct {
	lazybiosOemDellType177Array_t* Type177;
	lazybiosOemDellType212Array_t* Type212;
	lazybiosOemDellType218Array_t* Type218;
} lazybiosOemDell_t;

/**
 * @brief HP OEM structures held by a context.
 * @ingroup api_oem_types
 */
typedef struct {
	lazybiosOemHpType204Array_t* Type204;
} lazybiosOemHp_t;

/**
 * @brief Vendor-specific structures held by a context.
 * @ingroup api_oem_types
 *
 * The vendor members are allocated by @ref lazybiosCTXNew and are never NULL.
 * Only the per-type members inside them are NULL until parsed.
 */
typedef struct {
	lazybiosOemDell_t* dell;
	lazybiosOemHp_t*   hp;
} lazybiosOem_t;

/**
 * @brief Top-level lazybios context containing raw and parsed SMBIOS data.
 * @ingroup api_context
 */
struct lazybiosCTX {
	lazybiosBackend_t backend;
	lazybiosDMI_t* DMIData;

	lazybiosType0Array_t* Type0;
	lazybiosType1Array_t* Type1;
	lazybiosType2Array_t* Type2;
	lazybiosType3Array_t* Type3;
	lazybiosType4Array_t* Type4;
	lazybiosType5Array_t* Type5;
	lazybiosType6Array_t* Type6;
	lazybiosType7Array_t* Type7;
	lazybiosType8Array_t* Type8;
	lazybiosType9Array_t* Type9;
	lazybiosType10Array_t* Type10;
	lazybiosType11Array_t* Type11;
	lazybiosType12Array_t* Type12;
	lazybiosType13Array_t* Type13;
	lazybiosType14Array_t* Type14;
	lazybiosType15Array_t* Type15;
	lazybiosType16Array_t* Type16;
	lazybiosType17Array_t* Type17;
	lazybiosType18Array_t* Type18;
	lazybiosType19Array_t* Type19;
	lazybiosType20Array_t* Type20;
	lazybiosType21Array_t* Type21;
	lazybiosType22Array_t* Type22;
	lazybiosType23Array_t* Type23;
	lazybiosType24Array_t* Type24;
	lazybiosType25Array_t* Type25;
	lazybiosType26Array_t* Type26;
	lazybiosType27Array_t* Type27;
	lazybiosType28Array_t* Type28;
	lazybiosType29Array_t* Type29;
	lazybiosType30Array_t* Type30;
	lazybiosType31Array_t* Type31;
	lazybiosType32Array_t* Type32;
	lazybiosType33Array_t* Type33;
	lazybiosType34Array_t* Type34;
	lazybiosType35Array_t* Type35;
	lazybiosType36Array_t* Type36;
	lazybiosType37Array_t* Type37;
	lazybiosType38Array_t* Type38;
	lazybiosType39Array_t* Type39;
	lazybiosType40Array_t* Type40;
	lazybiosType41Array_t* Type41;
	lazybiosType42Array_t* Type42;
	lazybiosType43Array_t* Type43;
	lazybiosType44Array_t* Type44;
	lazybiosType45Array_t* Type45;
	lazybiosType46Array_t* Type46;

	lazybiosOem_t* oem;

};

/**
 * @brief Allocates and initializes a lazybios context.
 * @ingroup api_context
 * @return Newly allocated context, or NULL if allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosCTX_t* lazybiosCTXNew(void);

/**
 * @brief Loads SMBIOS data from the host system or from captured table files.
 * @ingroup api_context
 *
 * The two path arguments select the input mode:
 *
 * | @p entry_point | @p DMI_BIN | Source |
 * | :--- | :--- | :--- |
 * | `NULL` | `NULL` | the host system, through the context's backend |
 * | `NULL` | path | one merged file holding the entry point and table |
 * | path | path | separate raw entry-point and DMI-table files |
 *
 * Supplying an entry point without a table is rejected, because the entry
 * point alone describes nothing to parse.
 *
 * On success the context owns copies of both buffers and its type index is
 * built. A failed load must not be followed by a structure getter; release the
 * context with @ref lazybiosCleanup instead.
 *
 * @param ctx Context that receives the raw entry point and DMI table data.
 * @param entry_point Path to a raw SMBIOS entry-point file, or NULL.
 * @param DMI_BIN Path to a raw DMI table or merged dump file, or NULL for the host.
 * @return 0 on success, or -1 on failure.
 */
LAZYBIOS_WARN_UNUSED int lazybiosInit(lazybiosCTX_t* ctx, const char* entry_point, const char* DMI_BIN);

/**
 * @brief Parses every implemented structure type into the context.
 * @ingroup api_parsing
 *
 * Types already present in @p ctx are left untouched, so calling this more than
 * once does not strand an earlier result set. A type the table does not contain
 * still yields a valid empty set; a type whose parse fails is left NULL, so
 * check each member before use as usual.
 *
 * @param ctx Context holding loaded SMBIOS data.
 * @return 0 on success, or -1 if the context holds no usable DMI table.
 */
LAZYBIOS_WARN_UNUSED int lazybiosParseAll(lazybiosCTX_t* ctx);

/**
 * @brief Releases a context and all SMBIOS data owned by it.
 * @ingroup api_context
 * @param ctx Context to release.
 * @return 0 on success, or -1 if ctx is NULL.
 */
int lazybiosCleanup(lazybiosCTX_t* ctx);

#ifdef __cplusplus
}
#endif

#endif

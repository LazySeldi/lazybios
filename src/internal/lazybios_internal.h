/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file lazybios_internal.h
 * @brief Internal constants and parser helpers for lazybios implementation files.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_INTERNAL_H
#define LAZYBIOS_INTERNAL_H

#include "lazybios/lazybios.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Logging system for lazybios

#ifndef LAZYBIOS_QUIET
#if defined(__MINGW32__)
__attribute__((format(gnu_printf, 2, 3)))
#elif defined(__GNUC__)
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
#endif

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

/**
 * @brief Appends formatted text to a decoder output buffer.
 *
 * snprintf returns the length the output *would* have had, so accumulating its
 * return value lets the running length run past the buffer: the next append
 * then writes at buf + len with a wrapped-around buf_len - len. This keeps
 * *len at the number of bytes actually written, never above buf_len - 1, so
 * later appends and the trailing-separator trim stay inside the buffer.
 *
 * @param buf Output buffer, may be NULL only when buf_len is zero.
 * @param buf_len Capacity of buf in bytes.
 * @param len Running output length, updated in place.
 * @param fmt printf-style format for the text to append.
 */
#if defined(__MINGW32__)
__attribute__((format(gnu_printf, 4, 5)))
#elif defined(__GNUC__)
__attribute__((format(printf, 4, 5)))
#endif
static inline void lazybiosDecoderAppend(char* buf, size_t buf_len, size_t* len, const char* fmt, ...) {
	if (!buf || buf_len == 0 || *len + 1 >= buf_len) return;

	size_t remaining = buf_len - *len;

	/*
	 * Nearly every decoder appends a plain literal, so checking for a
	 * conversion first lets the common case copy the bytes straight in and
	 * never enter the format-parsing machinery. strchr/strlen measured faster
	 * than a hand-rolled scan here; the strings are long enough that the
	 * vectorized libc versions win.
	 */
	if (!strchr(fmt, '%')) {
		size_t n = strlen(fmt);
		if (n >= remaining) n = remaining - 1;
		memcpy(buf + *len, fmt, n);
		buf[*len + n] = '\0';
		*len += n;
		return;
	}

	va_list args;
	va_start(args, fmt);
	int written = vsnprintf(buf + *len, remaining, fmt, args);
	va_end(args);

	if (written < 0) return;
	*len += ((size_t)written >= remaining) ? remaining - 1 : (size_t)written;
}

/**
 * @brief Scratch buffer size for the file-local decoders.
 *
 * The longest decoding in the corpus is a Type 0 characteristics list at just
 * over 500 bytes, so this leaves generous headroom; the decoded text is copied
 * to an exact-sized allocation afterwards.
 */
#define LAZYBIOS_DECODER_BUF_SIZE 1024

/**
 * @brief Duplicates a NUL-terminated string with malloc.
 *
 * strdup is POSIX rather than C99, so the parsers use this when storing a
 * decoded string that the record owns.
 *
 * @param src String to copy, may be NULL.
 * @return Newly allocated copy, or NULL on failure or a NULL input.
 */
static inline char* lazybiosDup(const char* src) {
	if (!src) return NULL;
	size_t n = strlen(src) + 1;
	char* out = malloc(n);
	if (out) memcpy(out, src, n);
	return out;
}

/** @brief Size of an SMBIOS structure header (type, length, handle). */
#define SMBIOS_HEADER_SIZE 4

/** @brief Absolute path to the Linux sysfs SMBIOS entry point. */
#define LINUX_SYSFS_SMBIOS_ENTRY "/sys/firmware/dmi/tables/smbios_entry_point"

/** @brief Absolute path to the Linux sysfs DMI table. */
#define LINUX_SYSFS_DMI_TABLE "/sys/firmware/dmi/tables/DMI"

/** @brief Absolute path to the physical memory device. */
#define DEV_MEM "/dev/mem"

#define SMBIOS_TYPE_BIOS 0
#define SMBIOS_TYPE_SYSTEM 1
#define SMBIOS_TYPE_BASEBOARD 2
#define SMBIOS_TYPE_CHASSIS 3
#define SMBIOS_TYPE_PROCESSOR 4
#define SMBIOS_TYPE_MEMORY_CONTROLLER 5
#define SMBIOS_TYPE_MEMORY_MODULE 6
#define SMBIOS_TYPE_CACHES 7
#define SMBIOS_TYPE_PORT_CONNECTOR 8
#define SMBIOS_TYPE_SYSTEM_SLOTS 9
#define SMBIOS_TYPE_ONBOARD_DEVICES 10
#define SMBIOS_TYPE_OEM_STRINGS 11
#define SMBIOS_TYPE_SYSTEM_CONFIGURATION_OPTIONS 12
#define SMBIOS_TYPE_FIRMWARE_LANGUAGE_INFORMATION 13
#define SMBIOS_TYPE_GROUP_ASSOCIATIONS 14
#define SMBIOS_TYPE_SYSTEM_EVENT_LOG 15
#define SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY 16
#define SMBIOS_TYPE_MEMORY_DEVICE 17
#define SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFORMATION 18
#define SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS 19
#define SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS 20
#define SMBIOS_TYPE_BUILT_IN_POINTING_DEVICE 21
#define SMBIOS_TYPE_PORTABLE_BATTERY 22
#define SMBIOS_TYPE_SYSTEM_RESET 23
#define SMBIOS_TYPE_HARDWARE_SECURITY 24
#define SMBIOS_TYPE_SYSTEM_POWER_CONTROLS 25
#define SMBIOS_TYPE_VOLTAGE_PROBE 26
#define SMBIOS_TYPE_COOLING_DEVICE 27
#define SMBIOS_TYPE_TEMPERATURE_PROBE 28
#define SMBIOS_TYPE_ELECTRICAL_CURRENT_PROBE 29
#define SMBIOS_TYPE_OUT_OF_BAND_REMOTE_ACCESS 30
#define SMBIOS_TYPE_BOOT_INTEGRITY_SERVICES_ENTRY_POINT 31
#define SMBIOS_TYPE_SYSTEM_BOOT_INFORMATION 32
#define SMBIOS_TYPE_64BIT_MEMORY_ERROR_INFORMATION 33
#define SMBIOS_TYPE_MANAGEMENT_DEVICE 34
#define SMBIOS_TYPE_MANAGEMENT_DEVICE_COMPONENT 35
#define SMBIOS_TYPE_MANAGEMENT_DEVICE_THRESHOLD_DATA 36
#define SMBIOS_TYPE_MEMORY_CHANNEL 37
#define SMBIOS_TYPE_IPMI_DEVICE_INFORMATION 38
#define SMBIOS_TYPE_SYSTEM_POWER_SUPPLY 39
#define SMBIOS_TYPE_ADDITIONAL_INFORMATION 40
#define SMBIOS_TYPE_ONBOARD_DEVICES_EXTENDED_INFORMATION 41
#define SMBIOS_TYPE_MANAGEMENT_CONTROLLER_HOST_INTERFACE 42
#define SMBIOS_TYPE_TPM_DEVICE 43
#define SMBIOS_TYPE_PROCESSOR_ADDITIONAL_INFORMATION 44
#define SMBIOS_TYPE_FIRMWARE_INVENTORY_INFORMATION 45
#define SMBIOS_TYPE_STRING_PROPERTY 46
#define SMBIOS_TYPE_END 127

// HP
#define SMBIOS_OEM_HP_TYPE204  204


// DELL
#define SMBIOS_OEM_DELL_TYPE177 177
#define SMBIOS_OEM_DELL_TYPE212 212
#define SMBIOS_OEM_DELL_TYPE218 218


#define SMBIOS3_ANCHOR                 "_SM3_"
#define SMBIOS3_ANCHOR_OFFSET          0x00
#define SMBIOS3_CHECKSUM_OFFSET        0x05
#define SMBIOS3_LENGTH_OFFSET          0x06
#define SMBIOS3_MAJOR_OFFSET           0x07
#define SMBIOS3_MINOR_OFFSET           0x08
#define SMBIOS3_DOCREV_OFFSET          0x09
#define SMBIOS3_REVISION_OFFSET        0x0A
#define SMBIOS3_RESERVED_OFFSET        0x0B
#define SMBIOS3_TABLE_MAX_SIZE_OFFSET  0x0C
#define SMBIOS3_TABLE_ADDRESS_OFFSET   0x10
#define SMBIOS3_ENTRY_POINT_LENGTH     0x18
#define SMBIOS3_ANCHOR_SIZE            5

#define SMBIOS2_ANCHOR                       "_SM_"
#define SMBIOS2_INTERMEDIATE_ANCHOR          "_DMI_"
#define SMBIOS2_ANCHOR_OFFSET                0x00
#define SMBIOS2_CHECKSUM_OFFSET              0x04
#define SMBIOS2_LENGTH_OFFSET                0x05
#define SMBIOS2_MAJOR_OFFSET                 0x06
#define SMBIOS2_MINOR_OFFSET                 0x07
#define SMBIOS2_MAX_STRUCTURE_SIZE_OFFSET    0x08
#define SMBIOS2_REVISION_OFFSET              0x0A
#define SMBIOS2_FORMATTED_AREA_OFFSET        0x0B
#define SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET   0x10
#define SMBIOS2_INTERMEDIATE_CHECKSUM_OFFSET 0x15
#define SMBIOS2_TABLE_LENGTH_OFFSET          0x16
#define SMBIOS2_TABLE_ADDRESS_OFFSET         0x18
#define SMBIOS2_STRUCTURE_COUNT_OFFSET       0x1C
#define SMBIOS2_BCD_REVISION_OFFSET          0x1E
#define SMBIOS2_ENTRY_POINT_LENGTH           0x1F
#define SMBIOS2_ENTRY_POINT_LENGTH_V21       0x1E
#define SMBIOS2_ANCHOR_SIZE                  4
#define SMBIOS2_INTERMEDIATE_ANCHOR_SIZE     5
#define SMBIOS2_FORMATTED_AREA_SIZE          5

/** @brief Marks a parsed structure field as present and valid. */
#define LAZYBIOS_MARK_PRESENT(record, field) do { \
    (record)->field_status.field = (lazybiosFieldStatus_t)LAZYBIOS_FIELD_PRESENT; \
} while (0)

/** @brief Marks a parsed structure field as absent. */
#define LAZYBIOS_MARK_ABSENT(record, field) do { \
    (record)->field_status.field = (lazybiosFieldStatus_t)LAZYBIOS_FIELD_ABSENT; \
} while (0)

/** @brief Marks a parsed structure field as unreachable (structure too short for newer spec version). */
#define LAZYBIOS_MARK_UNREACHABLE(record, field) do { \
    (record)->field_status.field = (lazybiosFieldStatus_t)LAZYBIOS_FIELD_UNREACHABLE; \
} while (0)

/** @brief Clamps a structure length against available buffer boundaries. */
#define LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end) do { \
	if ((size_t)((end) - (p)) < (size_t)(len)) { \
		(len) = (uint8_t)((end) - (p)); \
	} \
} while (0)

/**
 * Internal, platform-neutral backend transformations.
 *
 * Keeping firmware byte validation separate from the operating-system calls
 * lets the same code run under unit tests and libFuzzer on every host.
 */
typedef struct {
	lazybiosSMBIOSVersionTag tag;
	size_t length;
	int checksum_valid;
	int intermediate_checksum_valid;
} lazybiosEntryInspection;

/** @brief Inspects and validates an SMBIOS 2.x or 3.x entry point. */
int lazybiosInspectEntryPoint(const uint8_t* entry_data, size_t available, lazybiosEntryInspection* inspection);
/** @brief Copies validated raw entry-point and DMI-table buffers into a context. */
int lazybiosLoadRawBuffers(lazybiosCTX_t* ctx, const uint8_t* entry_data, size_t entry_len, const uint8_t* dmi_data, size_t dmi_len);
/** @brief Loads the Windows raw SMBIOS table format into a context. */
int lazybiosLoadWindowsRawSMBIOSData(lazybiosCTX_t* ctx, const uint8_t* raw_data, size_t raw_len);
/** @brief Finds a valid SMBIOS entry point within a memory image. */
int lazybiosFindSMBIOSEntryPoint(const uint8_t* image, size_t image_len, size_t* entry_offset, size_t* entry_len);
/** @brief Extracts an SMBIOS table address and length from an entry point. */
int lazybiosGetSMBIOSTableLocation(const uint8_t* entry_data, size_t available, size_t* entry_len, uint64_t* table_address, size_t* table_len);
/** @brief Determines the entry-point and table layout in a combined dump file. */
int lazybiosGetSingleFileLayout(const uint8_t* entry_data, size_t available, size_t file_len, size_t* entry_len, size_t* table_offset, size_t* table_len);

/**
 * @brief Copies a string from an SMBIOS structure's string-set.
 * @param p Start of the SMBIOS structure.
 * @param length Length of the structure's formatted section.
 * @param index One-based index of the requested string.
 * @param end Address of the next structure, as returned by DMINext, which is
 * two bytes past this structure's string-set terminator.
 * @return Pointer to a string in the DMI table, or NULL if the string is unavailable or invalid.
 */
const char* DMIString(const uint8_t* p, uint8_t length, uint8_t index, const uint8_t* end);

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
 * @return Number of matching structures in the table, -1 if index is already filled.
 */
size_t lazybiosCountStructsByType(const lazybiosDMI_t* DMIData, uint8_t target_type);
/**
 * @brief Loads an SMBIOS entry point and DMI table from separate files.
 * @param ctx Context that receives the loaded data.
 * @param entry_path Path to the raw SMBIOS entry point file.
 * @param dmi_path Path to the raw DMI structure table file.
 * @return 0 on success, or -1 on failure.
 */
LAZYBIOS_WARN_UNUSED int lazybiosFile(lazybiosCTX_t* ctx, const char* entry_path, const char* dmi_path);

/**
 * @brief Loads SMBIOS entry point and DMI data from one merged file.
 * @param ctx Context that receives the loaded data.
 * @param bin_path Path to a file containing the entry point and DMI table,
 * either concatenated or separated by an advertised file-relative offset.
 * @return 0 on success, or -1 on failure.
 */
LAZYBIOS_WARN_UNUSED int lazybiosSingleFile(lazybiosCTX_t* ctx, const char* bin_path);

/**
 * @brief Validates and identifies an SMBIOS entry point.
 * @param ctx Context whose entry tag and tagged union are updated.
 * @param entry_buf Buffer containing an SMBIOS 2.x or 3.x entry point.
 * @param buf_len Length of entry_buf in bytes.
 * @return 0 on success, or -1 if the entry point is invalid.
 */
int lazybiosParseEntry(lazybiosCTX_t* ctx, const uint8_t* entry_buf, size_t buf_len);


#define READSTR(record, field, len, offset, p, end) do { \
	if ((len) > (offset)) { \
		(record)->field = DMIString((p), (len), (p)[(offset)], (end)); \
		if ((record)->field != NULL) { \
			LAZYBIOS_MARK_PRESENT((record), field); \
		} else { \
			LAZYBIOS_MARK_ABSENT((record), field); \
		} \
	} else { \
		(record)->field = NULL; \
		LAZYBIOS_MARK_ABSENT((record), field); \
	} \
} while (0)

#define READU8(record, field, len, offset, p) do { \
	if ((len) > (offset)) { \
		(record)->field = (p)[(offset)]; \
		LAZYBIOS_MARK_PRESENT((record), field); \
	} else { \
		(record)->field = 0; \
		LAZYBIOS_MARK_ABSENT((record), field); \
	} \
} while (0)

#define READU16(record, field, len, offset, p) do { \
	if ((size_t)(len) >= (size_t)(offset) + sizeof(uint16_t)) { \
		memcpy(&(record)->field, (p) + (offset), sizeof(uint16_t)); \
		LAZYBIOS_MARK_PRESENT((record), field); \
	} else { \
		(record)->field = 0; \
		LAZYBIOS_MARK_ABSENT((record), field); \
	} \
} while (0)

#define READU32(record, field, len, offset, p) do { \
	if ((size_t)(len) >= (size_t)(offset) + sizeof(uint32_t)) { \
		memcpy(&(record)->field, (p) + (offset), sizeof(uint32_t)); \
		LAZYBIOS_MARK_PRESENT((record), field); \
	} else { \
		(record)->field = 0; \
		LAZYBIOS_MARK_ABSENT((record), field); \
	} \
} while (0)

#define READU64(record, field, len, offset, p) do { \
	if ((size_t)(len) >= (size_t)(offset) + sizeof(uint64_t)) { \
		memcpy(&(record)->field, (p) + (offset), sizeof(uint64_t)); \
		LAZYBIOS_MARK_PRESENT((record), field); \
	} else { \
		(record)->field = 0; \
		LAZYBIOS_MARK_ABSENT((record), field); \
	} \
} while (0)

#if defined(OS_LINUX)
/** @brief Loads SMBIOS data through the Linux backend. */
int lazybiosLinux(lazybiosCTX_t* ctx);
#endif

#if defined(OS_WINDOWS)
/** @brief Loads SMBIOS data through the Windows backend. */
int lazybiosWindows(lazybiosCTX_t* ctx);
#endif

#if defined(OS_MACOS)
/** @brief Loads SMBIOS data through the macOS backend. */
int lazybiosMacOS(lazybiosCTX_t* ctx);
#endif

#if defined(OS_OPENBSD)
/** @brief Loads SMBIOS data through the OpenBSD backend. */
int lazybiosOpenBSD(lazybiosCTX_t* ctx);
#endif

#if defined(OS_FREEBSD)
/** @brief Loads SMBIOS data through the FreeBSD backend. */
int lazybiosFreeBSD(lazybiosCTX_t* ctx);
#endif

#if defined(OS_NETBSD)
/** @brief Loads SMBIOS data through the NetBSD backend. */
int lazybiosNetBSD(lazybiosCTX_t* ctx);
#endif

#if defined(OS_SUNOS)
/** @brief Loads SMBIOS data through the SunOS backend. */
int lazybiosSunOS(lazybiosCTX_t* ctx);
#endif

#if defined(OS_DRAGONFLY)
/** @brief Loads SMBIOS data through the DragonFly BSD backend. */
int lazybiosDragonFly(lazybiosCTX_t* ctx);
#endif

#if defined(OS_HAIKU)
/** @brief Loads SMBIOS data through the Haiku backend. */
int lazybiosHaiku(lazybiosCTX_t* ctx);
#endif

#if defined(OS_BEOS)
/** @brief Loads SMBIOS data through the BeOS backend. */
int lazybiosBeOS(lazybiosCTX_t* ctx);
#endif

#if defined(OS_REACTOS)
/** @brief Loads SMBIOS data through the ReactOS backend. */
int lazybiosReactOS(lazybiosCTX_t* ctx);
#endif

#if defined(OS_QNX)
/** @brief Loads SMBIOS data through the QNX Neutrino backend. */
int lazybiosQNX(lazybiosCTX_t* ctx);
#endif

#if defined(OS_MINIX)
/** @brief Loads SMBIOS data through the MINIX 3 backend. */
int lazybiosMINIX(lazybiosCTX_t* ctx);
#endif

#if defined(OS_GENERIC)
/** @brief Loads SMBIOS data through the generic legacy backend. */
int lazybiosGeneric(lazybiosCTX_t* ctx);
#endif

#if defined(OS_HAIKU) || defined(OS_BEOS) || defined(OS_GENERIC) || \
	defined(OS_QNX) || defined(OS_MINIX)
/** @brief Loads an SMBIOS table from a legacy physical-memory device. */
int lazybiosLoadLegacyPhysicalMemory(lazybiosCTX_t* ctx,
	const char* const* device_paths, size_t device_count,
	const char* platform_name);
#endif

#endif

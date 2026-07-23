/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This file is part of lazybios.
 *
 * lazybios is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * lazybios is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with lazybios. If not, see <https://www.gnu.org/licenses/>.
 */
/**
 * @file lazybios_internal.h
 * @brief Internal constants and parser helpers for lazybios implementation files.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_INTERNAL_H
#define LAZYBIOS_INTERNAL_H

#include "lazybios.h"

#include <stdlib.h>
#include <string.h>

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

#define LAZYBIOS_MARK_PRESENT(record, field) \
	((record)->field_status.field = LAZYBIOS_FIELD_PRESENT)

#define LAZYBIOS_MARK_ABSENT(record, field) do { \
	(record)->field_status.field = LAZYBIOS_FIELD_ABSENT; \
} while (0)

#define LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end) do { \
	if ((size_t)((end) - (p)) < (size_t)(len)) { \
		(len) = (uint8_t)((end) - (p)); \
	} \
} while (0)

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

#endif

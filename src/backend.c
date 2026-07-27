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
 * @file backend.c
 * @brief Platform-neutral validation and conversion for backend firmware data.
 */

#include "lazybios_internal.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define WINDOWS_RAW_HEADER_SIZE 8
#define WINDOWS_RAW_LENGTH_OFFSET 4

static uint16_t read_u16_le(const uint8_t data[2]) {
	return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

static uint32_t read_u32_le(const uint8_t data[4]) {
	return (uint32_t)data[0] |
		((uint32_t)data[1] << 8) |
		((uint32_t)data[2] << 16) |
		((uint32_t)data[3] << 24);
}

static uint64_t read_u64_le(const uint8_t data[8]) {
	return (uint64_t)data[0] |
		((uint64_t)data[1] << 8) |
		((uint64_t)data[2] << 16) |
		((uint64_t)data[3] << 24) |
		((uint64_t)data[4] << 32) |
		((uint64_t)data[5] << 40) |
		((uint64_t)data[6] << 48) |
		((uint64_t)data[7] << 56);
}

static void write_u16_le(uint8_t data[2], uint16_t value) {
	data[0] = (uint8_t)value;
	data[1] = (uint8_t)(value >> 8);
}

static void write_u32_le(uint8_t data[4], uint32_t value) {
	data[0] = (uint8_t)value;
	data[1] = (uint8_t)(value >> 8);
	data[2] = (uint8_t)(value >> 16);
	data[3] = (uint8_t)(value >> 24);
}

static void set_checksum(uint8_t* data, size_t begin, size_t end, size_t checksum_offset) {
	uint8_t sum = 0;
	data[checksum_offset] = 0;
	for (size_t i = begin; i < end; i++)
		sum = (uint8_t)(sum + data[i]);
	data[checksum_offset] = (uint8_t)(-sum);
}

int lazybiosLoadRawBuffers(lazybiosCTX_t* ctx,
	const uint8_t* entry_data, size_t entry_len,
	const uint8_t* dmi_data, size_t dmi_len) {
	if (!ctx || !ctx->DMIData || !entry_data || entry_len == 0 ||
		!dmi_data || dmi_len == 0)
		return -1;

	/*
	 * Backend loaders operate on a fresh context. Reject reuse rather than
	 * leaking an older table or leaving already-parsed structures stale.
	 */
	if (ctx->DMIData->entry_data || ctx->DMIData->dmi_data)
		return -1;

	uint8_t* entry_copy = malloc(entry_len);
	uint8_t* dmi_copy = malloc(dmi_len);
	if (!entry_copy || !dmi_copy) {
		free(entry_copy);
		free(dmi_copy);
		return -1;
	}
	memcpy(entry_copy, entry_data, entry_len);
	memcpy(dmi_copy, dmi_data, dmi_len);

	lazybiosDMI_t parsed = {0};
	lazybiosCTX_t temporary = {0};
	parsed.entry_data = entry_copy;
	parsed.entry_len = entry_len;
	temporary.DMIData = &parsed;

	if (lazybiosParseEntry(&temporary, entry_copy, entry_len) != 0) {
		free(entry_copy);
		free(dmi_copy);
		return -1;
	}

	parsed.dmi_data = dmi_copy;
	parsed.dmi_len = dmi_len;
	*ctx->DMIData = parsed;
	return 0;
}

int lazybiosLoadWindowsRawSMBIOSData(lazybiosCTX_t* ctx, const uint8_t* raw_data, size_t raw_len) {
	if (!ctx || !raw_data || raw_len < WINDOWS_RAW_HEADER_SIZE)
		return -1;

	const uint8_t major = raw_data[1];
	const uint8_t minor = raw_data[2];
	const uint8_t docrev = raw_data[3];
	const uint32_t table_len_u32 =
		read_u32_le(raw_data + WINDOWS_RAW_LENGTH_OFFSET);

	const size_t table_len = table_len_u32;
	if (table_len == 0 || table_len > raw_len - WINDOWS_RAW_HEADER_SIZE)
		return -1;
	if (major < 3 && table_len > UINT16_MAX)
		return -1;

	uint8_t entry[SMBIOS2_ENTRY_POINT_LENGTH] = {0};
	size_t entry_len;

	if (major >= 3) {
		entry_len = SMBIOS3_ENTRY_POINT_LENGTH;
		memcpy(entry, SMBIOS3_ANCHOR, SMBIOS3_ANCHOR_SIZE);
		entry[SMBIOS3_LENGTH_OFFSET] = SMBIOS3_ENTRY_POINT_LENGTH;
		entry[SMBIOS3_MAJOR_OFFSET] = major;
		entry[SMBIOS3_MINOR_OFFSET] = minor;
		entry[SMBIOS3_DOCREV_OFFSET] = docrev;
		entry[SMBIOS3_REVISION_OFFSET] = 1;

		write_u32_le(entry + SMBIOS3_TABLE_MAX_SIZE_OFFSET, table_len_u32);
		set_checksum(entry, 0, entry_len, SMBIOS3_CHECKSUM_OFFSET);
	} else {
		entry_len = SMBIOS2_ENTRY_POINT_LENGTH;
		memcpy(entry, SMBIOS2_ANCHOR, SMBIOS2_ANCHOR_SIZE);
		entry[SMBIOS2_LENGTH_OFFSET] = SMBIOS2_ENTRY_POINT_LENGTH;
		entry[SMBIOS2_MAJOR_OFFSET] = major;
		entry[SMBIOS2_MINOR_OFFSET] = minor;
		memcpy(entry + SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET,
			SMBIOS2_INTERMEDIATE_ANCHOR, SMBIOS2_INTERMEDIATE_ANCHOR_SIZE);

		const uint16_t legacy_table_len = (uint16_t)table_len;
		write_u16_le(entry + SMBIOS2_TABLE_LENGTH_OFFSET, legacy_table_len);
		entry[SMBIOS2_BCD_REVISION_OFFSET] =
			(uint8_t)((major << 4) | (minor & 0x0F));
		set_checksum(entry, SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET,
			entry_len, SMBIOS2_INTERMEDIATE_CHECKSUM_OFFSET);
		set_checksum(entry, 0, SMBIOS2_INTERMEDIATE_ANCHOR_OFFSET,
			SMBIOS2_CHECKSUM_OFFSET);
	}

	return lazybiosLoadRawBuffers(ctx, entry, entry_len,
		raw_data + WINDOWS_RAW_HEADER_SIZE, table_len);
}

int lazybiosFindSMBIOSEntryPoint(const uint8_t* image, size_t image_len,
	size_t* entry_offset, size_t* entry_len) {
	if (!image || !entry_offset || !entry_len)
		return -1;

	size_t i = 0;
	while (image_len - i >= SMBIOS2_ANCHOR_SIZE) {
		lazybiosEntryInspection inspection;
		if (lazybiosInspectEntryPoint(
				image + i, image_len - i, &inspection) == 0 &&
			inspection.checksum_valid &&
			inspection.intermediate_checksum_valid) {
			*entry_offset = i;
			*entry_len = inspection.length;
			return 0;
		}

		if (image_len - i < SMBIOS2_ANCHOR_SIZE + 16)
			break;
		i += 16;
	}

	return -1;
}

/**
 * Extracts the physical structure-table location from a validated entry point.
 *
 * This keeps firmware byte decoding independent of platform I/O so native
 * backends can share deterministic tests and fuzz coverage.
 */
int lazybiosGetSMBIOSTableLocation(const uint8_t* entry_data, size_t available,
	size_t* entry_len, uint64_t* table_address, size_t* table_len) {
	lazybiosEntryInspection inspection;

	if (!entry_data || !entry_len || !table_address || !table_len)
		return -1;
	if (lazybiosInspectEntryPoint(entry_data, available, &inspection) != 0 ||
		!inspection.checksum_valid ||
		!inspection.intermediate_checksum_valid)
		return -1;

	if (inspection.tag == SMBIOS_VER_3X) {
		*table_address =
			read_u64_le(entry_data + SMBIOS3_TABLE_ADDRESS_OFFSET);
		*table_len = (size_t)read_u32_le(
			entry_data + SMBIOS3_TABLE_MAX_SIZE_OFFSET);
	} else if (inspection.tag == SMBIOS_VER_2X) {
		*table_address =
			read_u32_le(entry_data + SMBIOS2_TABLE_ADDRESS_OFFSET);
		*table_len =
			read_u16_le(entry_data + SMBIOS2_TABLE_LENGTH_OFFSET);
	} else {
		return -1;
	}

	if (*table_address == 0 || *table_len == 0)
		return -1;

	*entry_len = inspection.length;
	return 0;
}

/**
 * Selects the structure-table range in a file containing an entry point.
 *
 * Most merged dumps place the table immediately after the entry point and
 * retain its physical firmware address. Some operating-system devices instead
 * rewrite that address into a file-relative offset and align the table. Use
 * the embedded address only when both it and the advertised table length fit
 * entirely inside the file; otherwise preserve the tightly concatenated
 * layout.
 */
int lazybiosGetSingleFileLayout(const uint8_t* entry_data, size_t available,
	size_t file_len, size_t* entry_len, size_t* table_offset,
	size_t* table_len) {
	lazybiosEntryInspection inspection;
	uint64_t advertised_address;
	size_t advertised_len;

	if (!entry_data || !entry_len || !table_offset || !table_len)
		return -1;
	if (lazybiosInspectEntryPoint(entry_data, available, &inspection) != 0 ||
		inspection.length >= file_len)
		return -1;

	if (inspection.tag == SMBIOS_VER_3X) {
		advertised_address =
			read_u64_le(entry_data + SMBIOS3_TABLE_ADDRESS_OFFSET);
		advertised_len = (size_t)read_u32_le(
			entry_data + SMBIOS3_TABLE_MAX_SIZE_OFFSET);
	} else if (inspection.tag == SMBIOS_VER_2X) {
		advertised_address =
			read_u32_le(entry_data + SMBIOS2_TABLE_ADDRESS_OFFSET);
		advertised_len =
			read_u16_le(entry_data + SMBIOS2_TABLE_LENGTH_OFFSET);
	} else {
		return -1;
	}

	*entry_len = inspection.length;
	*table_offset = inspection.length;
	*table_len = file_len - inspection.length;

	if (advertised_address <= SIZE_MAX) {
		const size_t offset = (size_t)advertised_address;
		if (offset >= inspection.length && offset < file_len &&
			advertised_len != 0 && advertised_len <= file_len - offset) {
			*table_offset = offset;
			*table_len = advertised_len;
		}
	}

	return 0;
}

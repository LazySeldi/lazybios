/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type43.c
 * @brief Implements parsing and decoding for SMBIOS Type 43 TPM Device.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* File-local decoders; their output is stored in each record's `decoded`. */
static size_t lazybiosType43FirmwareVersionStr(uint8_t major_spec_version, uint32_t firmware_version_1,
									  uint32_t firmware_version_2, char* buf, size_t buf_len);
static size_t lazybiosType43CharacteristicsStr(uint64_t characteristics, char* buf, size_t buf_len);

// Fields
#define VENDOR_ID 0x04
#define VENDOR_ID_LENGTH 0x04
#define MAJOR_SPEC_VERSION 0x08
#define MINOR_SPEC_VERSION 0x09
#define FIRMWARE_VERSION_1 0x0A
#define FIRMWARE_VERSION_2 0x0E
#define DESCRIPTION 0x12
#define CHARACTERISTICS 0x13
#define OEM_DEFINED 0x1B

// Characteristics Masks
#define CHARACTERISTICS_NOT_SUPPORTED_MASK (1ULL << 2)
#define FAMILY_CONFIGURABLE_FIRMWARE_UPDATE_MASK (1ULL << 3)
#define FAMILY_CONFIGURABLE_PLATFORM_SOFTWARE_MASK (1ULL << 4)
#define FAMILY_CONFIGURABLE_OEM_MASK (1ULL << 5)

lazybiosType43Array_t* lazybiosGetType43(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType43Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_TPM_DEVICE);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_TPM_DEVICE].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_TPM_DEVICE].first;
	}
	size_t index = 0;

	if (count == 0) return out;

	out->entries = calloc(count, sizeof(*out->entries));
	if (!out->entries) {
		free(out);
		return NULL;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];
		if (len < SMBIOS_HEADER_SIZE) break;
		const uint8_t* structure_end = DMINext(p, end);

		if (type == SMBIOS_TYPE_TPM_DEVICE) {
			if (index >= count) break;
			lazybiosType43_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			if ((size_t)len >= VENDOR_ID + VENDOR_ID_LENGTH) {
				memcpy(current->vendor_id, p + VENDOR_ID, VENDOR_ID_LENGTH);
				current->vendor_id[VENDOR_ID_LENGTH] = '\0';
				LAZYBIOS_MARK_PRESENT(current, vendor_id);
			} else {
				memset(current->vendor_id, 0, sizeof(current->vendor_id));
				LAZYBIOS_MARK_ABSENT(current, vendor_id);
			}

			READU8(current, major_spec_version, len, MAJOR_SPEC_VERSION, p);
			READU8(current, minor_spec_version, len, MINOR_SPEC_VERSION, p);
			READU32(current, firmware_version_1, len, FIRMWARE_VERSION_1, p);
			READU32(current, firmware_version_2, len, FIRMWARE_VERSION_2, p);
			READSTR(current, description, len, DESCRIPTION, p, structure_end);
			READU64(current, characteristics, len, CHARACTERISTICS, p);
			READU32(current, oem_defined, len, OEM_DEFINED, p);

			char decbuf[LAZYBIOS_DECODER_BUF_SIZE];
			if (LAZYBIOS_FIELD_STATUS(current, major_spec_version) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType43FirmwareVersionStr(current->major_spec_version, current->firmware_version_1,
						current->firmware_version_2, decbuf, sizeof(decbuf));
				current->decoded.major_spec_version = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, characteristics) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType43CharacteristicsStr(current->characteristics, decbuf, sizeof(decbuf));
				current->decoded.characteristics = lazybiosDup(decbuf);
			}

			index++;
		}
		p = structure_end;
	}
	out->count = index;
	return out;
}

static size_t lazybiosType43FirmwareVersionStr(uint8_t major_spec_version, uint32_t firmware_version_1,
									  uint32_t firmware_version_2, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;

	if (major_spec_version == 1) {
		uint8_t revision_major = (uint8_t)((firmware_version_1 >> 16) & 0xFF);
		uint8_t revision_minor = (uint8_t)((firmware_version_1 >> 24) & 0xFF);
		snprintf(buf, buf_len, "%hhu.%hhu", revision_major, revision_minor);
	} else if (major_spec_version == 2) {
		snprintf(buf, buf_len, "%u.%u", firmware_version_1, firmware_version_2);
	} else {
		snprintf(buf, buf_len, "0x%08X / 0x%08X", firmware_version_1, firmware_version_2);
	}
	return buf ? strlen(buf) : 0;
}

static size_t lazybiosType43CharacteristicsStr(uint64_t characteristics, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;

	if (characteristics & CHARACTERISTICS_NOT_SUPPORTED_MASK) {
		snprintf(buf, buf_len, "TPM Device Characteristics Not Supported");
		return 0;
	}

	snprintf(buf, buf_len, "Firmware Update: %s, Platform Software: %s, OEM Mechanism: %s",
			 (characteristics & FAMILY_CONFIGURABLE_FIRMWARE_UPDATE_MASK) ? "Yes" : "No",
			 (characteristics & FAMILY_CONFIGURABLE_PLATFORM_SOFTWARE_MASK) ? "Yes" : "No",
			 (characteristics & FAMILY_CONFIGURABLE_OEM_MASK) ? "Yes" : "No");
	return buf ? strlen(buf) : 0;
}

void lazybiosFreeType43(lazybiosType43Array_t* Type43) {
    if (!Type43) return;

	for (size_t i = 0; i < Type43->count; i++) {
		free(Type43->entries[i].decoded.major_spec_version);
		free(Type43->entries[i].decoded.characteristics);
	}

    free(Type43->entries);

    free(Type43);
}

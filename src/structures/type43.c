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
 * @file type43.c
 * @brief Implements parsing and decoding for SMBIOS Type 43 TPM Device.
 * @author LazySeldi
 */

//
// Type 43 ( TPM Device )
//

#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 43 TPM Device structures.
 *
 * @param Type43 Existing Type 43 array pointer value; it is not dereferenced or released.
 * @param type43_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 43 array, or NULL on failure.
 */
lazybiosType43_t* lazybiosGetType43(lazybiosType43_t* Type43, size_t* type43_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_TPM_DEVICE);
	size_t index = 0;

	Type43 = calloc(count, sizeof(lazybiosType43_t));
	if (!Type43) return NULL;
	if (count == 0) {
		*type43_count = 0;
		return Type43;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_TPM_DEVICE) {
			if (index >= count) break;
			lazybiosType43_t* current = &Type43[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

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

			index++;
		}
		p = DMINext(p, end);
	}
	*type43_count = index;
	return Type43;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Formats a TPM firmware revision according to its major specification version.
 *
 * @param major_spec_version TPM major specification version.
 * @param firmware_version_1 Raw first TPM firmware-version field.
 * @param firmware_version_2 Raw second TPM firmware-version field.
 * @param buf Output buffer that receives the formatted revision.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType43FirmwareVersionStr(uint8_t major_spec_version, uint32_t firmware_version_1,
									  uint32_t firmware_version_2, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;

	if (major_spec_version == 1) {
		uint8_t revision_major = (uint8_t)((firmware_version_1 >> 16) & 0xFF);
		uint8_t revision_minor = (uint8_t)((firmware_version_1 >> 24) & 0xFF);
		snprintf(buf, buf_len, "%hhu.%hhu", revision_major, revision_minor);
	} else if (major_spec_version == 2) {
		snprintf(buf, buf_len, "%u.%u", firmware_version_1, firmware_version_2);
	} else {
		snprintf(buf, buf_len, "0x%08X / 0x%08X", firmware_version_1, firmware_version_2);
	}
}

/**
 * @brief Decodes SMBIOS Type 43 TPM device characteristics.
 *
 * @param characteristics Raw Type 43 characteristics bit field.
 * @param buf Output buffer that receives the decoded characteristics.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType43CharacteristicsStr(uint64_t characteristics, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;

	if (characteristics & CHARACTERISTICS_NOT_SUPPORTED_MASK) {
		snprintf(buf, buf_len, "TPM Device Characteristics Not Supported");
		return;
	}

	snprintf(buf, buf_len, "Firmware Update: %s, Platform Software: %s, OEM Mechanism: %s",
			 (characteristics & FAMILY_CONFIGURABLE_FIRMWARE_UPDATE_MASK) ? "Yes" : "No",
			 (characteristics & FAMILY_CONFIGURABLE_PLATFORM_SOFTWARE_MASK) ? "Yes" : "No",
			 (characteristics & FAMILY_CONFIGURABLE_OEM_MASK) ? "Yes" : "No");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 43 structures.
 *
 * @param Type43 Type 43 array to release.
 * @param type43_count Number of elements in Type43.
 */
void lazybiosFreeType43(lazybiosType43_t* Type43, size_t type43_count) {
	if (!Type43) return;

	for (size_t i = 0; i < type43_count; i++) {
		free(Type43[i].description);
	}
	free(Type43);
}

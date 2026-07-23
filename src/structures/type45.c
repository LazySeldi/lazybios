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
 * @file type45.c
 * @brief Implements parsing and decoding for SMBIOS Type 45 Firmware Inventory Information.
 * @author LazySeldi
 */

//
// Type 45 ( Firmware Inventory Information )
//

#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define FIRMWARE_COMPONENT_NAME 0x04
#define FIRMWARE_VERSION 0x05
#define VERSION_FORMAT 0x06
#define FIRMWARE_ID 0x07
#define FIRMWARE_ID_FORMAT 0x08
#define RELEASE_DATE 0x09
#define MANUFACTURER 0x0A
#define LOWEST_SUPPORTED_FIRMWARE_VERSION 0x0B
#define IMAGE_SIZE 0x0C
#define CHARACTERISTICS 0x14
#define STATE 0x16
#define NUMBER_OF_ASSOCIATED_COMPONENTS 0x17
#define ASSOCIATED_COMPONENT_HANDLES 0x18

// Version Formats
#define VERSION_FORMAT_FREE_FORM 0x00
#define VERSION_FORMAT_MAJOR_MINOR 0x01
#define VERSION_FORMAT_HEX32 0x02
#define VERSION_FORMAT_HEX64 0x03

// Firmware ID Formats
#define FIRMWARE_ID_FORMAT_FREE_FORM 0x00
#define FIRMWARE_ID_FORMAT_UEFI_GUID 0x01

// Characteristics Masks
#define CHARACTERISTICS_UPDATABLE_MASK 0x0001
#define CHARACTERISTICS_WRITE_PROTECTED_MASK 0x0002

// Firmware States
#define STATE_OTHER 0x01
#define STATE_UNKNOWN 0x02
#define STATE_DISABLED 0x03
#define STATE_ENABLED 0x04
#define STATE_ABSENT 0x05
#define STATE_STANDBY_OFFLINE 0x06
#define STATE_STANDBY_SPARE 0x07
#define STATE_UNAVAILABLE_OFFLINE 0x08
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 45 Firmware Inventory Information structures.
 *
 * @param Type45 Existing Type 45 array pointer value; it is not dereferenced or released.
 * @param type45_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 45 array, or NULL on failure.
 */
lazybiosType45_t* lazybiosGetType45(lazybiosType45_t* Type45, size_t* type45_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_FIRMWARE_INVENTORY_INFORMATION);
	size_t index = 0;

	Type45 = calloc(count, sizeof(lazybiosType45_t));
	if (!Type45) return NULL;
	if (count == 0) {
		*type45_count = 0;
		return Type45;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_FIRMWARE_INVENTORY_INFORMATION) {
			if (index >= count) break;
			lazybiosType45_t* current = &Type45[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, firmware_component_name, len, FIRMWARE_COMPONENT_NAME, p, structure_end);
			READSTR(current, firmware_version, len, FIRMWARE_VERSION, p, structure_end);
			READU8(current, version_format, len, VERSION_FORMAT, p);
			READSTR(current, firmware_id, len, FIRMWARE_ID, p, structure_end);
			READU8(current, firmware_id_format, len, FIRMWARE_ID_FORMAT, p);
			READSTR(current, release_date, len, RELEASE_DATE, p, structure_end);
			READSTR(current, manufacturer, len, MANUFACTURER, p, structure_end);
			READSTR(current, lowest_supported_firmware_version, len, LOWEST_SUPPORTED_FIRMWARE_VERSION, p,
					structure_end);
			READU64(current, image_size, len, IMAGE_SIZE, p);
			READU16(current, characteristics, len, CHARACTERISTICS, p);
			READU8(current, state, len, STATE, p);
			READU8(current, number_of_associated_components, len, NUMBER_OF_ASSOCIATED_COMPONENTS, p);

			if (current->field_status.number_of_associated_components == LAZYBIOS_FIELD_PRESENT) {
				const size_t associated_handles_size =
					(size_t)current->number_of_associated_components * sizeof(uint16_t);
				if ((size_t)len >= ASSOCIATED_COMPONENT_HANDLES + associated_handles_size) {
					if (current->number_of_associated_components > 0) {
						current->associated_component_handles = malloc(associated_handles_size);
						if (!current->associated_component_handles) {
							lazybiosFreeType45(Type45, count);
							return NULL;
						}
						memcpy(current->associated_component_handles, p + ASSOCIATED_COMPONENT_HANDLES,
							   associated_handles_size);
					}
					LAZYBIOS_MARK_PRESENT(current, associated_component_handles);
				} else {
					LAZYBIOS_MARK_ABSENT(current, associated_component_handles);
				}
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type45_count = index;
	return Type45;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes the format of Type 45 firmware-version strings.
 *
 * @param version_format Raw firmware-version format value.
 * @return Static string describing the version format.
 */
const char* lazybiosType45VersionFormatStr(uint8_t version_format) {
	switch (version_format) {
		case VERSION_FORMAT_FREE_FORM:
			return "Free-form String";
		case VERSION_FORMAT_MAJOR_MINOR:
			return "Major.Minor Decimal String";
		case VERSION_FORMAT_HEX32:
			return "32-bit Hexadecimal String";
		case VERSION_FORMAT_HEX64:
			return "64-bit Hexadecimal String";
		default:
			if (version_format <= 0x7F) return "Available for Future Assignment";
			return "Firmware Vendor/OEM-specific";
	}
}

/**
 * @brief Decodes the format of a Type 45 firmware ID string.
 *
 * @param firmware_id_format Raw firmware-ID format value.
 * @return Static string describing the firmware-ID format.
 */
const char* lazybiosType45FirmwareIDFormatStr(uint8_t firmware_id_format) {
	switch (firmware_id_format) {
		case FIRMWARE_ID_FORMAT_FREE_FORM:
			return "Free-form String";
		case FIRMWARE_ID_FORMAT_UEFI_GUID:
			return "UEFI GUID String";
		default:
			if (firmware_id_format <= 0x7F) return "Available for Future Assignment";
			return "Firmware Vendor/OEM-specific";
	}
}

/**
 * @brief Formats the Type 45 updatable and write-protected characteristic flags.
 *
 * @param characteristics Raw Type 45 characteristics word.
 * @param buf Output buffer that receives the decoded flags.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType45CharacteristicsStr(uint16_t characteristics, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;

	snprintf(buf, buf_len, "Updatable: %s, Write-protected: %s",
			 (characteristics & CHARACTERISTICS_UPDATABLE_MASK) ? "Yes" : "No",
			 (characteristics & CHARACTERISTICS_WRITE_PROTECTED_MASK) ? "Yes" : "No");
}

/**
 * @brief Decodes the operational state of a Type 45 firmware component.
 *
 * @param state Raw firmware inventory state value.
 * @return Static string describing the firmware state.
 */
const char* lazybiosType45StateStr(uint8_t state) {
	switch (state) {
		case STATE_OTHER:
			return "Other";
		case STATE_UNKNOWN:
			return "Unknown";
		case STATE_DISABLED:
			return "Disabled";
		case STATE_ENABLED:
			return "Enabled";
		case STATE_ABSENT:
			return "Absent";
		case STATE_STANDBY_OFFLINE:
			return "Standby Offline";
		case STATE_STANDBY_SPARE:
			return "Standby Spare";
		case STATE_UNAVAILABLE_OFFLINE:
			return "Unavailable Offline";
		default:
			return "Undefined";
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 45 structures.
 *
 * @param Type45 Type 45 array to release.
 * @param type45_count Number of elements in Type45.
 */
void lazybiosFreeType45(lazybiosType45_t* Type45, size_t type45_count) {
	if (!Type45) return;

	for (size_t i = 0; i < type45_count; i++) {
		free(Type45[i].firmware_component_name);
		free(Type45[i].firmware_version);
		free(Type45[i].firmware_id);
		free(Type45[i].release_date);
		free(Type45[i].manufacturer);
		free(Type45[i].lowest_supported_firmware_version);
		free(Type45[i].associated_component_handles);
	}
	free(Type45);
}

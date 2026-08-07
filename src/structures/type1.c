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
 * @file type1.c
 * @brief Implements parsing and decoding for SMBIOS Type 1 System Information.
 * @author LazySeldi
 */

//
// Type 1 ( System Information )
//

#include "lazybios_internal.h"
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define MANUFACTURER 0x04
#define PRODUCT_NAME 0x05
#define VERSION 0x06
#define SERIAL_NUMBER 0x07
#define UUID 0x08
#define WAKE_UP_TYPE 0x18
#define SKU_NUMBER 0x19
#define FAMILY 0x1A

// Decoders

// Wake Up Type
#define WAKEUP_TYPE_RESERVED 0x00
#define WAKEUP_TYPE_OTHER 0x01
#define WAKEUP_TYPE_UNKNOWN 0x02
#define WAKEUP_TYPE_APM_TIMER 0x03
#define WAKEUP_TYPE_MODEM_RING 0x04
#define WAKEUP_TYPE_LAN_REMOTE 0x05
#define WAKEUP_TYPE_POWER_SWITCH 0x06
#define WAKEUP_TYPE_PCI_PME 0x07
#define WAKEUP_TYPE_AC_POWER_RESTORED 0x08
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 1 System Information structures.
 *
 * @param Type1 Existing Type 1 array pointer value; it is not dereferenced or released.
 * @param type1_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 1 array, or NULL on failure.
 */
lazybiosType1_t* lazybiosGetType1(lazybiosType1_t* Type1, size_t* type1_count, lazybiosDMI_t* DMIData) {
	if (type1_count) *type1_count = 0;
	if (!type1_count || !DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;
	const size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_SYSTEM);
	size_t index = 0;

	Type1 = calloc(count, sizeof(*Type1));
	if (!Type1) return NULL;

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_SYSTEM) {
			lazybiosType1_t* current = &Type1[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, manufacturer, len, MANUFACTURER, p, structure_end);
			READSTR(current, product_name, len, PRODUCT_NAME, p, structure_end);
			READSTR(current, version, len, VERSION, p, structure_end);
			READSTR(current, serial_number, len, SERIAL_NUMBER, p, structure_end);

			if (lazybiosIsVersionPlus(DMIData, 2, 1)) {
				if (len >= UUID + sizeof(current->uuid)) {
					const uint8_t* uuid = p + UUID;
					int all_zero = 1;
					int all_ff = 1;
					for (int i = 0; i < 16; i++) current->uuid[i] = uuid[i];
					for (int i = 0; i < 16; i++) {
						if (uuid[i] != 0x00) all_zero = 0;
						if (uuid[i] != 0xFF) all_ff = 0;
					}
					if (all_zero || all_ff) {
						LAZYBIOS_MARK_ABSENT(current, uuid);
					} else {
						LAZYBIOS_MARK_PRESENT(current, uuid);
					}
				} else {
					for (int i = 0; i < 16; i++) current->uuid[i] = 0;
					LAZYBIOS_MARK_ABSENT(current, uuid);
				}
				READU8(current, wake_up_type, len, WAKE_UP_TYPE, p);
			} else {
				for (int i = 0; i < 16; i++) current->uuid[i] = 0;
				current->wake_up_type = 0;
			}

			if (lazybiosIsVersionPlus(DMIData, 2, 4)) {
				READSTR(current, sku_number, len, SKU_NUMBER, p, structure_end);
				READSTR(current, family, len, FAMILY, p, structure_end);
			} else {
				current->sku_number = NULL;
				current->family = NULL;
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type1_count = index;
	return Type1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders

// Wake Up Type
/**
 * @brief Decodes an SMBIOS system wake-up type.
 *
 * @param wake_up_type Raw SMBIOS wake-up type value.
 * @return Static string describing the wake-up type.
 */
const char* lazybiosType1WakeupTypeStr(uint8_t wake_up_type) {
	switch (wake_up_type) {
		case WAKEUP_TYPE_RESERVED:
			return "Reserved";
		case WAKEUP_TYPE_OTHER:
			return "Other";
		case WAKEUP_TYPE_UNKNOWN:
			return "Unknown";
		case WAKEUP_TYPE_APM_TIMER:
			return "APM Timer";
		case WAKEUP_TYPE_MODEM_RING:
			return "Modem Ring";
		case WAKEUP_TYPE_LAN_REMOTE:
			return "LAN Remote";
		case WAKEUP_TYPE_POWER_SWITCH:
			return "Power Switch";
		case WAKEUP_TYPE_PCI_PME:
			return "PCI PME#";
		case WAKEUP_TYPE_AC_POWER_RESTORED:
			return "AC Power Restored";
		default:
			return "Unknown/Reserved";
	}
}

// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 1 structures.
 *
 * @param Type1 Type 1 array to release.
 * @param type1_count Number of elements in Type1.
 */
void lazybiosFreeType1(lazybiosType1_t* Type1, size_t type1_count) {
	if (!Type1) return;

	for (size_t i = 0; i < type1_count; i++) {
		free(Type1[i].manufacturer);
		free(Type1[i].product_name);
		free(Type1[i].version);
		free(Type1[i].serial_number);
		free(Type1[i].sku_number);
		free(Type1[i].family);
	}
	free(Type1);
}

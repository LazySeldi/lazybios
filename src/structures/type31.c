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
 * @file type31.c
 * @brief Implements parsing for SMBIOS Type 31 Boot Integrity Services Entry Point.
 * @author LazySeldi
 */

//
// Type 31 ( Boot Integrity Services Entry Point )
//

#include "lazybios_internal.h"
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define CHECKSUM 0x04
#define RESERVED_1 0x05
#define RESERVED_2 0x06
#define BIS_ENTRY_POINT_16 0x08
#define BIS_ENTRY_POINT_32 0x0C
#define RESERVED_3 0x10
#define RESERVED_4 0x18

#define TYPE31_MINIMUM_LENGTH 0x1C
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 31 Boot Integrity Services Entry Point structures.
 *
 * @param Type31 Existing Type 31 array pointer value; it is not dereferenced or released.
 * @param type31_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 31 array, or NULL on failure.
 */
lazybiosType31_t* lazybiosGetType31(lazybiosType31_t* Type31, size_t* type31_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_BOOT_INTEGRITY_SERVICES_ENTRY_POINT);
	size_t index = 0;

	Type31 = calloc(count, sizeof(lazybiosType31_t));
	if (!Type31) return NULL;
	if (count == 0) {
		*type31_count = 0;
		return Type31;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_BOOT_INTEGRITY_SERVICES_ENTRY_POINT) {
			if (index >= count) break;
			lazybiosType31_t* current = &Type31[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);

			READU8(current, checksum, len, CHECKSUM, p);
			READU8(current, reserved_1, len, RESERVED_1, p);
			READU16(current, reserved_2, len, RESERVED_2, p);
			READU32(current, bis_entry_point_16, len, BIS_ENTRY_POINT_16, p);
			READU32(current, bis_entry_point_32, len, BIS_ENTRY_POINT_32, p);
			READU64(current, reserved_3, len, RESERVED_3, p);
			READU32(current, reserved_4, len, RESERVED_4, p);

			if (len >= TYPE31_MINIMUM_LENGTH) {
				uint8_t sum = 0;
				for (size_t i = 0; i < len; i++) {
					sum = (uint8_t)(sum + p[i]);
				}
				current->checksum_valid = (uint8_t)(sum == 0);
				LAZYBIOS_MARK_PRESENT(current, checksum_valid);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type31_count = index;
	return Type31;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 31 structures.
 *
 * @param Type31 Type 31 array to release.
 * @param type31_count Number of elements in Type31.
 */
void lazybiosFreeType31(lazybiosType31_t* Type31, size_t type31_count) {
	(void)type31_count;
	free(Type31);
}

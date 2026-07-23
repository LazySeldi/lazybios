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
 * @file type34.c
 * @brief Implements parsing and decoding for SMBIOS Type 34 Management Device.
 * @author LazySeldi
 */

//
// Type 34 ( Management Device )
//

#include "lazybios_internal.h"
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define DESCRIPTION 0x04
#define DEVICE_TYPE 0x05
#define ADDRESS 0x06
#define ADDRESS_TYPE 0x0A

// Device Types
#define DEVICE_TYPE_OTHER 0x01
#define DEVICE_TYPE_UNKNOWN 0x02
#define DEVICE_TYPE_LM75 0x03
#define DEVICE_TYPE_LM78 0x04
#define DEVICE_TYPE_LM79 0x05
#define DEVICE_TYPE_LM80 0x06
#define DEVICE_TYPE_LM81 0x07
#define DEVICE_TYPE_ADM9240 0x08
#define DEVICE_TYPE_DS1780 0x09
#define DEVICE_TYPE_MAXIM_1617 0x0A
#define DEVICE_TYPE_GL518SM 0x0B
#define DEVICE_TYPE_W83781D 0x0C
#define DEVICE_TYPE_HT82H791 0x0D

// Address Types
#define ADDRESS_TYPE_OTHER 0x01
#define ADDRESS_TYPE_UNKNOWN 0x02
#define ADDRESS_TYPE_IO_PORT 0x03
#define ADDRESS_TYPE_MEMORY 0x04
#define ADDRESS_TYPE_SM_BUS 0x05
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 34 Management Device structures.
 *
 * @param Type34 Existing Type 34 array pointer value; it is not dereferenced or released.
 * @param type34_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 34 array, or NULL on failure.
 */
lazybiosType34_t* lazybiosGetType34(lazybiosType34_t* Type34, size_t* type34_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_MANAGEMENT_DEVICE);
	size_t index = 0;

	Type34 = calloc(count, sizeof(lazybiosType34_t));
	if (!Type34) return NULL;
	if (count == 0) {
		*type34_count = 0;
		return Type34;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_MANAGEMENT_DEVICE) {
			if (index >= count) break;
			lazybiosType34_t* current = &Type34[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, description, len, DESCRIPTION, p, structure_end);
			READU8(current, device_type, len, DEVICE_TYPE, p);
			READU32(current, address, len, ADDRESS, p);
			READU8(current, address_type, len, ADDRESS_TYPE, p);

			index++;
		}
		p = DMINext(p, end);
	}
	*type34_count = index;
	return Type34;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes an SMBIOS Type 34 management-device type.
 *
 * @param device_type Raw management-device type value.
 * @return Static string describing the management-device type.
 */
const char* lazybiosType34DeviceTypeStr(uint8_t device_type) {
	switch (device_type) {
		case DEVICE_TYPE_OTHER:
			return "Other";
		case DEVICE_TYPE_UNKNOWN:
			return "Unknown";
		case DEVICE_TYPE_LM75:
			return "National Semiconductor LM75";
		case DEVICE_TYPE_LM78:
			return "National Semiconductor LM78";
		case DEVICE_TYPE_LM79:
			return "National Semiconductor LM79";
		case DEVICE_TYPE_LM80:
			return "National Semiconductor LM80";
		case DEVICE_TYPE_LM81:
			return "National Semiconductor LM81";
		case DEVICE_TYPE_ADM9240:
			return "Analog Devices ADM9240";
		case DEVICE_TYPE_DS1780:
			return "Dallas Semiconductor DS1780";
		case DEVICE_TYPE_MAXIM_1617:
			return "Maxim 1617";
		case DEVICE_TYPE_GL518SM:
			return "Genesys GL518SM";
		case DEVICE_TYPE_W83781D:
			return "Winbond W83781D";
		case DEVICE_TYPE_HT82H791:
			return "Holtek HT82H791";
		default:
			return "Undefined";
	}
}

/**
 * @brief Decodes an SMBIOS Type 34 management-device address type.
 *
 * @param address_type Raw management-device address-type value.
 * @return Static string describing the address type.
 */
const char* lazybiosType34AddressTypeStr(uint8_t address_type) {
	switch (address_type) {
		case ADDRESS_TYPE_OTHER:
			return "Other";
		case ADDRESS_TYPE_UNKNOWN:
			return "Unknown";
		case ADDRESS_TYPE_IO_PORT:
			return "I/O Port";
		case ADDRESS_TYPE_MEMORY:
			return "Memory";
		case ADDRESS_TYPE_SM_BUS:
			return "SM Bus";
		default:
			return "Undefined";
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 34 structures.
 *
 * @param Type34 Type 34 array to release.
 * @param type34_count Number of elements in Type34.
 */
void lazybiosFreeType34(lazybiosType34_t* Type34, size_t type34_count) {
	if (!Type34) return;

	for (size_t i = 0; i < type34_count; i++) {
		free(Type34[i].description);
	}
	free(Type34);
}

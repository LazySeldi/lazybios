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
 * @file type38.c
 * @brief Implements parsing and decoding for SMBIOS Type 38 IPMI Device Information.
 * @author LazySeldi
 */

//
// Type 38 ( IPMI Device Information )
//

#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define INTERFACE_TYPE 0x04
#define IPMI_SPECIFICATION_REVISION 0x05
#define I2C_TARGET_ADDRESS 0x06
#define NV_STORAGE_DEVICE_ADDRESS 0x07
#define BASE_ADDRESS 0x08
#define BASE_ADDRESS_MODIFIER_INTERRUPT_INFO 0x10
#define INTERRUPT_NUMBER 0x11

// Interface Types
#define INTERFACE_TYPE_UNKNOWN 0x00
#define INTERFACE_TYPE_KCS 0x01
#define INTERFACE_TYPE_SMIC 0x02
#define INTERFACE_TYPE_BT 0x03
#define INTERFACE_TYPE_SSIF 0x04
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 38 IPMI Device Information structures.
 *
 * @param Type38 Existing Type 38 array pointer value; it is not dereferenced or released.
 * @param type38_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 38 array, or NULL on failure.
 */
lazybiosType38_t* lazybiosGetType38(lazybiosType38_t* Type38, size_t* type38_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_IPMI_DEVICE_INFORMATION);
	size_t index = 0;

	Type38 = calloc(count, sizeof(lazybiosType38_t));
	if (!Type38) return NULL;
	if (count == 0) {
		*type38_count = 0;
		return Type38;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_IPMI_DEVICE_INFORMATION) {
			if (index >= count) break;
			lazybiosType38_t* current = &Type38[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);

			READU8(current, interface_type, len, INTERFACE_TYPE, p);
			READU8(current, ipmi_specification_revision, len, IPMI_SPECIFICATION_REVISION, p);
			READU8(current, i2c_target_address, len, I2C_TARGET_ADDRESS, p);
			READU8(current, nv_storage_device_address, len, NV_STORAGE_DEVICE_ADDRESS, p);
			if (current->nv_storage_device_address == 0xFF) {
				LAZYBIOS_MARK_ABSENT(current, nv_storage_device_address);
			}
			READU64(current, base_address, len, BASE_ADDRESS, p);
			READU8(current, base_address_modifier_interrupt_info, len, BASE_ADDRESS_MODIFIER_INTERRUPT_INFO, p);
			READU8(current, interrupt_number, len, INTERRUPT_NUMBER, p);

			index++;
		}
		p = DMINext(p, end);
	}
	*type38_count = index;
	return Type38;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes an IPMI BMC interface type.
 *
 * @param interface_type Raw Type 38 interface-type value.
 * @return Static string describing the BMC interface type.
 */
const char* lazybiosType38InterfaceTypeStr(uint8_t interface_type) {
	switch (interface_type) {
		case INTERFACE_TYPE_UNKNOWN:
			return "Unknown";
		case INTERFACE_TYPE_KCS:
			return "KCS: Keyboard Controller Style";
		case INTERFACE_TYPE_SMIC:
			return "SMIC: Server Management Interface Chip";
		case INTERFACE_TYPE_BT:
			return "BT: Block Transfer";
		case INTERFACE_TYPE_SSIF:
			return "SSIF: SMBus System Interface";
		default:
			return "Reserved";
	}
}

/**
 * @brief Formats the BCD-encoded IPMI specification revision.
 *
 * @param revision Raw BCD-encoded specification revision.
 * @param buf Output buffer that receives the formatted revision.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType38SpecificationRevisionStr(uint8_t revision, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;

	uint8_t major = revision >> 4;
	uint8_t minor = revision & 0x0F;
	if (major <= 9 && minor <= 9) {
		snprintf(buf, buf_len, "%hhu.%hhu", major, minor);
	} else {
		snprintf(buf, buf_len, "Invalid BCD (0x%02X)", revision);
	}
}

/**
 * @brief Decodes the address space selected by a Type 38 base address.
 *
 * @param base_address Raw Type 38 base-address field.
 * @return Static string describing the selected address space.
 */
const char* lazybiosType38BaseAddressTypeStr(uint64_t base_address) {
	return (base_address & 1ULL) ? "I/O" : "Memory-mapped";
}

/**
 * @brief Reconstructs the effective BMC base address.
 *
 * @param base_address Raw Type 38 base-address field.
 * @param modifier Raw base-address-modifier and interrupt-information byte.
 * @return Base address with the address-space flag removed and address bit zero restored.
 */
uint64_t lazybiosType38BaseAddressValue(uint64_t base_address, uint8_t modifier) {
	return (base_address & ~1ULL) | ((uint64_t)(modifier >> 4) & 1ULL);
}

/**
 * @brief Decodes the IPMI interface register spacing.
 *
 * @param modifier Raw base-address-modifier and interrupt-information byte.
 * @return Static string describing the register spacing.
 */
const char* lazybiosType38RegisterSpacingStr(uint8_t modifier) {
	switch ((modifier >> 6) & 0x03) {
		case 0:
			return "Successive Byte Boundaries";
		case 1:
			return "32-bit Boundaries";
		case 2:
			return "16-byte Boundaries";
		default:
			return "Reserved";
	}
}

/**
 * @brief Decodes Type 38 interrupt availability, polarity, and trigger mode.
 *
 * @param interrupt_info Raw base-address-modifier and interrupt-information byte.
 * @param buf Output buffer that receives the decoded interrupt information.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType38InterruptInfoStr(uint8_t interrupt_info, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;

	if (!(interrupt_info & (1U << 3))) {
		snprintf(buf, buf_len, "Not Specified");
		return;
	}

	const char* polarity = (interrupt_info & (1U << 1)) ? "Active High" : "Active Low";
	const char* trigger_mode = (interrupt_info & 1U) ? "Level-triggered" : "Edge-triggered";
	snprintf(buf, buf_len, "%s, %s", polarity, trigger_mode);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 38 structures.
 *
 * @param Type38 Type 38 array to release.
 * @param type38_count Number of elements in Type38.
 */
void lazybiosFreeType38(lazybiosType38_t* Type38, size_t type38_count) {
	(void)type38_count;
	free(Type38);
}

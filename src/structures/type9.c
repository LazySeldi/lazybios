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
 * @file type9.c
 * @brief Implements parsing and decoding for SMBIOS Type 9 System Slots.
 * @author LazySeldi
 */

//
// Type 9 ( System Slots )
//

#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define SLOT_DESIGNATION 0x04
#define SLOT_TYPE 0x05
#define SLOT_DATA_BUS_WIDTH 0x06
#define CURRENT_USAGE 0x07
#define SLOT_LENGTH 0x08
#define SLOT_ID 0x09
#define SLOT_CHARACTERISTICS_1 0x0B
#define SLOT_CHARACTERISTICS_2 0x0C
#define SEGMENT_GROUP_NUMBER 0x0D
#define BUS_NUMBER 0x0F
#define DEVICE_FUNCTION_NUMBER 0x10
#define DATA_BUS_WIDTH 0x11
#define PEER_GROUPING_COUNT 0x12
#define PEER_GROUPS 0x13
#define PEER_GROUP_SIZE 0x05
#define SLOT_INFORMATION(peer_count) (0x13 + ((peer_count) * PEER_GROUP_SIZE))
#define SLOT_PHYSICAL_WIDTH(peer_count) (0x14 + ((peer_count) * PEER_GROUP_SIZE))
#define SLOT_PITCH(peer_count) (0x15 + ((peer_count) * PEER_GROUP_SIZE))
#define SLOT_HEIGHT(peer_count) (0x17 + ((peer_count) * PEER_GROUP_SIZE))

// Decoders

// Slot Type
#define SLOT_TYPE_OTHER 0x01
#define SLOT_TYPE_UNKNOWN 0x02
#define SLOT_TYPE_ISA 0x03
#define SLOT_TYPE_MCA 0x04
#define SLOT_TYPE_EISA 0x05
#define SLOT_TYPE_PCI 0x06
#define SLOT_TYPE_PC_CARD 0x07
#define SLOT_TYPE_VL_VESA 0x08
#define SLOT_TYPE_PROPRIETARY 0x09
#define SLOT_TYPE_PROCESSOR_CARD 0x0A
#define SLOT_TYPE_PROPRIETARY_MEMORY_CARD 0x0B
#define SLOT_TYPE_IO_RISER_CARD 0x0C
#define SLOT_TYPE_NUBUS 0x0D
#define SLOT_TYPE_PCI_66MHZ 0x0E
#define SLOT_TYPE_AGP 0x0F
#define SLOT_TYPE_AGP_2X 0x10
#define SLOT_TYPE_AGP_4X 0x11
#define SLOT_TYPE_PCI_X 0x12
#define SLOT_TYPE_AGP_8X 0x13
#define SLOT_TYPE_M2_SOCKET_1_DP 0x14
#define SLOT_TYPE_M2_SOCKET_1_SD 0x15
#define SLOT_TYPE_M2_SOCKET_2 0x16
#define SLOT_TYPE_M2_SOCKET_3 0x17
#define SLOT_TYPE_MXM_TYPE_I 0x18
#define SLOT_TYPE_MXM_TYPE_II 0x19
#define SLOT_TYPE_MXM_TYPE_III_STANDARD 0x1A
#define SLOT_TYPE_MXM_TYPE_III_HE 0x1B
#define SLOT_TYPE_MXM_TYPE_IV 0x1C
#define SLOT_TYPE_MXM_3_TYPE_A 0x1D
#define SLOT_TYPE_MXM_3_TYPE_B 0x1E
#define SLOT_TYPE_PCIE_GEN_2_SFF_8639 0x1F
#define SLOT_TYPE_PCIE_GEN_3_SFF_8639 0x20
#define SLOT_TYPE_PCIE_MINI_52_WITH_KEEP_OUTS 0x21
#define SLOT_TYPE_PCIE_MINI_52_WITHOUT_KEEP_OUTS 0x22
#define SLOT_TYPE_PCIE_MINI_76 0x23
#define SLOT_TYPE_PCIE_GEN_4_SFF_8639 0x24
#define SLOT_TYPE_PCIE_GEN_5_SFF_8639 0x25
#define SLOT_TYPE_OCP_NIC_3_SFF 0x26
#define SLOT_TYPE_OCP_NIC_3_LFF 0x27
#define SLOT_TYPE_OCP_NIC_PRE_3 0x28
#define SLOT_TYPE_CXL_FLEXBUS_1 0x30
#define SLOT_TYPE_PC_98_C20 0xA0
#define SLOT_TYPE_PC_98_C24 0xA1
#define SLOT_TYPE_PC_98_E 0xA2
#define SLOT_TYPE_PC_98_LOCAL_BUS 0xA3
#define SLOT_TYPE_PC_98_CARD 0xA4
#define SLOT_TYPE_PCIE 0xA5
#define SLOT_TYPE_PCIE_X1 0xA6
#define SLOT_TYPE_PCIE_X2 0xA7
#define SLOT_TYPE_PCIE_X4 0xA8
#define SLOT_TYPE_PCIE_X8 0xA9
#define SLOT_TYPE_PCIE_X16 0xAA
#define SLOT_TYPE_PCIE_GEN_2 0xAB
#define SLOT_TYPE_PCIE_GEN_2_X1 0xAC
#define SLOT_TYPE_PCIE_GEN_2_X2 0xAD
#define SLOT_TYPE_PCIE_GEN_2_X4 0xAE
#define SLOT_TYPE_PCIE_GEN_2_X8 0xAF
#define SLOT_TYPE_PCIE_GEN_2_X16 0xB0
#define SLOT_TYPE_PCIE_GEN_3 0xB1
#define SLOT_TYPE_PCIE_GEN_3_X1 0xB2
#define SLOT_TYPE_PCIE_GEN_3_X2 0xB3
#define SLOT_TYPE_PCIE_GEN_3_X4 0xB4
#define SLOT_TYPE_PCIE_GEN_3_X8 0xB5
#define SLOT_TYPE_PCIE_GEN_3_X16 0xB6
#define SLOT_TYPE_PCIE_GEN_4 0xB8
#define SLOT_TYPE_PCIE_GEN_4_X1 0xB9
#define SLOT_TYPE_PCIE_GEN_4_X2 0xBA
#define SLOT_TYPE_PCIE_GEN_4_X4 0xBB
#define SLOT_TYPE_PCIE_GEN_4_X8 0xBC
#define SLOT_TYPE_PCIE_GEN_4_X16 0xBD
#define SLOT_TYPE_PCIE_GEN_5 0xBE
#define SLOT_TYPE_PCIE_GEN_5_X1 0xBF
#define SLOT_TYPE_PCIE_GEN_5_X2 0xC0
#define SLOT_TYPE_PCIE_GEN_5_X4 0xC1
#define SLOT_TYPE_PCIE_GEN_5_X8 0xC2
#define SLOT_TYPE_PCIE_GEN_5_X16 0xC3
#define SLOT_TYPE_PCIE_GEN_6_AND_BEYOND 0xC4
#define SLOT_TYPE_EDSFF_E1 0xC5
#define SLOT_TYPE_EDSFF_E3 0xC6

// Slot Data Bus Width and Slot Physical Width
#define SLOT_WIDTH_OTHER 0x01
#define SLOT_WIDTH_UNKNOWN 0x02
#define SLOT_WIDTH_8_BIT 0x03
#define SLOT_WIDTH_16_BIT 0x04
#define SLOT_WIDTH_32_BIT 0x05
#define SLOT_WIDTH_64_BIT 0x06
#define SLOT_WIDTH_128_BIT 0x07
#define SLOT_WIDTH_X1 0x08
#define SLOT_WIDTH_X2 0x09
#define SLOT_WIDTH_X4 0x0A
#define SLOT_WIDTH_X8 0x0B
#define SLOT_WIDTH_X12 0x0C
#define SLOT_WIDTH_X16 0x0D
#define SLOT_WIDTH_X32 0x0E

// Current Usage
#define SLOT_USAGE_OTHER 0x01
#define SLOT_USAGE_UNKNOWN 0x02
#define SLOT_USAGE_AVAILABLE 0x03
#define SLOT_USAGE_IN_USE 0x04
#define SLOT_USAGE_UNAVAILABLE 0x05

// Slot Length
#define SLOT_LENGTH_OTHER 0x01
#define SLOT_LENGTH_UNKNOWN 0x02
#define SLOT_LENGTH_SHORT 0x03
#define SLOT_LENGTH_LONG 0x04
#define SLOT_LENGTH_2_5_DRIVE 0x05
#define SLOT_LENGTH_3_5_DRIVE 0x06

// Slot Height
#define SLOT_HEIGHT_NOT_APPLICABLE 0x00
#define SLOT_HEIGHT_OTHER 0x01
#define SLOT_HEIGHT_UNKNOWN 0x02
#define SLOT_HEIGHT_FULL 0x03
#define SLOT_HEIGHT_LOW_PROFILE 0x04
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 9 System Slots structures.
 *
 * @param Type9 Existing Type 9 array pointer value; it is not dereferenced or released.
 * @param type9_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 9 array, or NULL on failure.
 */
lazybiosType9_t* lazybiosGetType9(lazybiosType9_t* Type9, size_t* type9_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_SYSTEM_SLOTS);
	size_t index = 0;

	Type9 = calloc(count, sizeof(lazybiosType9_t));
	if (!Type9) return NULL;
	if (count == 0) {
		*type9_count = 0;
		return Type9;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_SYSTEM_SLOTS) {
			if (index >= count) break;
			lazybiosType9_t* current = &Type9[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, slot_designation, len, SLOT_DESIGNATION, p, structure_end);
			READU8(current, slot_type, len, SLOT_TYPE, p);
			READU8(current, slot_data_bus_width, len, SLOT_DATA_BUS_WIDTH, p);
			READU8(current, current_usage, len, CURRENT_USAGE, p);
			READU8(current, slot_length, len, SLOT_LENGTH, p);
			READU16(current, slot_id, len, SLOT_ID, p);
			READU8(current, slot_characteristics_1, len, SLOT_CHARACTERISTICS_1, p);

			if (lazybiosIsVersionPlus(DMIData, 2, 1)) {
				READU8(current, slot_characteristics_2, len, SLOT_CHARACTERISTICS_2, p);
			} else {
				current->slot_characteristics_2 = 0;
			}

			if (lazybiosIsVersionPlus(DMIData, 2, 6)) {
				READU16(current, segment_group_number, len, SEGMENT_GROUP_NUMBER, p);
				READU8(current, bus_number, len, BUS_NUMBER, p);
				READU8(current, device_function_number, len, DEVICE_FUNCTION_NUMBER, p);
			} else {
				current->segment_group_number = 0;
				current->bus_number = 0;
				current->device_function_number = 0;
			}

			uint8_t declared_peer_grouping_count = 0;
			int peer_layout_valid = 0;
			if (lazybiosIsVersionPlus(DMIData, 3, 2)) {
				READU8(current, data_bus_width, len, DATA_BUS_WIDTH, p);
				READU8(current, peer_grouping_count, len, PEER_GROUPING_COUNT, p);
				declared_peer_grouping_count = current->peer_grouping_count;

				if (LAZYBIOS_FIELD_STATUS(current, peer_grouping_count) == LAZYBIOS_FIELD_PRESENT) {
					const size_t peer_bytes = (size_t)declared_peer_grouping_count * PEER_GROUP_SIZE;
					peer_layout_valid = len >= PEER_GROUPS + peer_bytes;

					if (peer_layout_valid && declared_peer_grouping_count > 0) {
						current->peer_groups = calloc(declared_peer_grouping_count, sizeof(lazybiosType9PeerGroup_t));
						if (current->peer_groups) {
							for (size_t i = 0; i < declared_peer_grouping_count; i++) {
								const size_t peer_offset = PEER_GROUPS + (i * PEER_GROUP_SIZE);
								memcpy(&current->peer_groups[i].segment_group_number, p + peer_offset, sizeof(uint16_t));
								current->peer_groups[i].bus_number = p[peer_offset + 2];
								current->peer_groups[i].device_function_number = p[peer_offset + 3];
								current->peer_groups[i].data_bus_width = p[peer_offset + 4];
								LAZYBIOS_MARK_PRESENT(&current->peer_groups[i], segment_group_number);
								LAZYBIOS_MARK_PRESENT(&current->peer_groups[i], bus_number);
								LAZYBIOS_MARK_PRESENT(&current->peer_groups[i], device_function_number);
								LAZYBIOS_MARK_PRESENT(&current->peer_groups[i], data_bus_width);
							}
							LAZYBIOS_MARK_PRESENT(current, peer_groups);
						} else {
							lazybiosFreeType9(Type9, count);
							return NULL;
						}
					} else if (!peer_layout_valid) {
						LAZYBIOS_MARK_ABSENT(current, peer_groups);
					} else {
						LAZYBIOS_MARK_PRESENT(current, peer_groups);
					}
				}
			} else {
				current->data_bus_width = 0;
				current->peer_grouping_count = 0;
				current->peer_groups = NULL;
			}

			if (lazybiosIsVersionPlus(DMIData, 3, 4) && peer_layout_valid) {
				READU8(current, slot_information, len, SLOT_INFORMATION(declared_peer_grouping_count), p);
				READU8(current, slot_physical_width, len, SLOT_PHYSICAL_WIDTH(declared_peer_grouping_count), p);
				READU16(current, slot_pitch, len, SLOT_PITCH(declared_peer_grouping_count), p);
			} else {
				current->slot_information = 0;
				current->slot_physical_width = 0;
				current->slot_pitch = 0;
				if (lazybiosIsVersionPlus(DMIData, 3, 4) &&
					LAZYBIOS_FIELD_STATUS(current, peer_grouping_count) == LAZYBIOS_FIELD_PRESENT) {
					LAZYBIOS_MARK_ABSENT(current, slot_information);
					LAZYBIOS_MARK_ABSENT(current, slot_physical_width);
					LAZYBIOS_MARK_ABSENT(current, slot_pitch);
				}
			}

			if (lazybiosIsVersionPlus(DMIData, 3, 5) && peer_layout_valid) {
				READU8(current, slot_height, len, SLOT_HEIGHT(declared_peer_grouping_count), p);
			} else {
				current->slot_height = 0;
				if (lazybiosIsVersionPlus(DMIData, 3, 5) &&
					LAZYBIOS_FIELD_STATUS(current, peer_grouping_count) == LAZYBIOS_FIELD_PRESENT) {
					LAZYBIOS_MARK_ABSENT(current, slot_height);
				}
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type9_count = index;
	return Type9;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders

// Slot Type
/**
 * @brief Decodes an SMBIOS system-slot type.
 *
 * @param slot_type Raw SMBIOS slot type value.
 * @return Static string describing the slot type.
 */
const char* lazybiosType9SlotTypeStr(uint8_t slot_type) {
	switch (slot_type) {
		case SLOT_TYPE_OTHER: return "Other";
		case SLOT_TYPE_UNKNOWN: return "Unknown";
		case SLOT_TYPE_ISA: return "ISA";
		case SLOT_TYPE_MCA: return "MCA";
		case SLOT_TYPE_EISA: return "EISA";
		case SLOT_TYPE_PCI: return "PCI";
		case SLOT_TYPE_PC_CARD: return "PC Card (PCMCIA)";
		case SLOT_TYPE_VL_VESA: return "VL-VESA";
		case SLOT_TYPE_PROPRIETARY: return "Proprietary";
		case SLOT_TYPE_PROCESSOR_CARD: return "Processor Card Slot";
		case SLOT_TYPE_PROPRIETARY_MEMORY_CARD: return "Proprietary Memory Card Slot";
		case SLOT_TYPE_IO_RISER_CARD: return "I/O Riser Card Slot";
		case SLOT_TYPE_NUBUS: return "NuBus";
		case SLOT_TYPE_PCI_66MHZ: return "PCI - 66MHz Capable";
		case SLOT_TYPE_AGP: return "AGP";
		case SLOT_TYPE_AGP_2X: return "AGP 2X";
		case SLOT_TYPE_AGP_4X: return "AGP 4X";
		case SLOT_TYPE_PCI_X: return "PCI-X";
		case SLOT_TYPE_AGP_8X: return "AGP 8X";
		case SLOT_TYPE_M2_SOCKET_1_DP: return "M.2 Socket 1-DP (Mechanical Key A)";
		case SLOT_TYPE_M2_SOCKET_1_SD: return "M.2 Socket 1-SD (Mechanical Key E)";
		case SLOT_TYPE_M2_SOCKET_2: return "M.2 Socket 2 (Mechanical Key B)";
		case SLOT_TYPE_M2_SOCKET_3: return "M.2 Socket 3 (Mechanical Key M)";
		case SLOT_TYPE_MXM_TYPE_I: return "MXM Type I";
		case SLOT_TYPE_MXM_TYPE_II: return "MXM Type II";
		case SLOT_TYPE_MXM_TYPE_III_STANDARD: return "MXM Type III (standard connector)";
		case SLOT_TYPE_MXM_TYPE_III_HE: return "MXM Type III (HE connector)";
		case SLOT_TYPE_MXM_TYPE_IV: return "MXM Type IV";
		case SLOT_TYPE_MXM_3_TYPE_A: return "MXM 3.0 Type A";
		case SLOT_TYPE_MXM_3_TYPE_B: return "MXM 3.0 Type B";
		case SLOT_TYPE_PCIE_GEN_2_SFF_8639: return "PCI Express Gen 2 SFF-8639 (U.2)";
		case SLOT_TYPE_PCIE_GEN_3_SFF_8639: return "PCI Express Gen 3 SFF-8639 (U.2)";
		case SLOT_TYPE_PCIE_MINI_52_WITH_KEEP_OUTS: return "PCI Express Mini 52-pin with bottom-side keep-outs";
		case SLOT_TYPE_PCIE_MINI_52_WITHOUT_KEEP_OUTS: return "PCI Express Mini 52-pin without bottom-side keep-outs";
		case SLOT_TYPE_PCIE_MINI_76: return "PCI Express Mini 76-pin";
		case SLOT_TYPE_PCIE_GEN_4_SFF_8639: return "PCI Express Gen 4 SFF-8639 (U.2)";
		case SLOT_TYPE_PCIE_GEN_5_SFF_8639: return "PCI Express Gen 5 SFF-8639 (U.2)";
		case SLOT_TYPE_OCP_NIC_3_SFF: return "OCP NIC 3.0 Small Form Factor (SFF)";
		case SLOT_TYPE_OCP_NIC_3_LFF: return "OCP NIC 3.0 Large Form Factor (LFF)";
		case SLOT_TYPE_OCP_NIC_PRE_3: return "OCP NIC Prior to 3.0";
		case SLOT_TYPE_CXL_FLEXBUS_1: return "CXL Flexbus 1.0 (deprecated)";
		case SLOT_TYPE_PC_98_C20: return "PC-98/C20";
		case SLOT_TYPE_PC_98_C24: return "PC-98/C24";
		case SLOT_TYPE_PC_98_E: return "PC-98/E";
		case SLOT_TYPE_PC_98_LOCAL_BUS: return "PC-98/Local Bus";
		case SLOT_TYPE_PC_98_CARD: return "PC-98/Card";
		case SLOT_TYPE_PCIE: return "PCI Express";
		case SLOT_TYPE_PCIE_X1: return "PCI Express x1";
		case SLOT_TYPE_PCIE_X2: return "PCI Express x2";
		case SLOT_TYPE_PCIE_X4: return "PCI Express x4";
		case SLOT_TYPE_PCIE_X8: return "PCI Express x8";
		case SLOT_TYPE_PCIE_X16: return "PCI Express x16";
		case SLOT_TYPE_PCIE_GEN_2: return "PCI Express Gen 2";
		case SLOT_TYPE_PCIE_GEN_2_X1: return "PCI Express Gen 2 x1";
		case SLOT_TYPE_PCIE_GEN_2_X2: return "PCI Express Gen 2 x2";
		case SLOT_TYPE_PCIE_GEN_2_X4: return "PCI Express Gen 2 x4";
		case SLOT_TYPE_PCIE_GEN_2_X8: return "PCI Express Gen 2 x8";
		case SLOT_TYPE_PCIE_GEN_2_X16: return "PCI Express Gen 2 x16";
		case SLOT_TYPE_PCIE_GEN_3: return "PCI Express Gen 3";
		case SLOT_TYPE_PCIE_GEN_3_X1: return "PCI Express Gen 3 x1";
		case SLOT_TYPE_PCIE_GEN_3_X2: return "PCI Express Gen 3 x2";
		case SLOT_TYPE_PCIE_GEN_3_X4: return "PCI Express Gen 3 x4";
		case SLOT_TYPE_PCIE_GEN_3_X8: return "PCI Express Gen 3 x8";
		case SLOT_TYPE_PCIE_GEN_3_X16: return "PCI Express Gen 3 x16";
		case SLOT_TYPE_PCIE_GEN_4: return "PCI Express Gen 4";
		case SLOT_TYPE_PCIE_GEN_4_X1: return "PCI Express Gen 4 x1";
		case SLOT_TYPE_PCIE_GEN_4_X2: return "PCI Express Gen 4 x2";
		case SLOT_TYPE_PCIE_GEN_4_X4: return "PCI Express Gen 4 x4";
		case SLOT_TYPE_PCIE_GEN_4_X8: return "PCI Express Gen 4 x8";
		case SLOT_TYPE_PCIE_GEN_4_X16: return "PCI Express Gen 4 x16";
		case SLOT_TYPE_PCIE_GEN_5: return "PCI Express Gen 5";
		case SLOT_TYPE_PCIE_GEN_5_X1: return "PCI Express Gen 5 x1";
		case SLOT_TYPE_PCIE_GEN_5_X2: return "PCI Express Gen 5 x2";
		case SLOT_TYPE_PCIE_GEN_5_X4: return "PCI Express Gen 5 x4";
		case SLOT_TYPE_PCIE_GEN_5_X8: return "PCI Express Gen 5 x8";
		case SLOT_TYPE_PCIE_GEN_5_X16: return "PCI Express Gen 5 x16";
		case SLOT_TYPE_PCIE_GEN_6_AND_BEYOND: return "PCI Express Gen 6 and Beyond";
		case SLOT_TYPE_EDSFF_E1: return "Enterprise and Datacenter 1U E1 Form Factor Slot";
		case SLOT_TYPE_EDSFF_E3: return "Enterprise and Datacenter 3-inch E3 Form Factor Slot";
		default: return "Unknown Slot Type";
	}
}

// Slot Data Bus Width and Slot Physical Width
/**
 * @brief Decodes an SMBIOS system-slot width.
 *
 * @param width Raw slot data-bus, peer data-bus, or physical-width value.
 * @return Static string describing the slot width.
 */
const char* lazybiosType9SlotWidthStr(uint8_t width) {
	switch (width) {
		case SLOT_WIDTH_OTHER: return "Other";
		case SLOT_WIDTH_UNKNOWN: return "Unknown";
		case SLOT_WIDTH_8_BIT: return "8 bit";
		case SLOT_WIDTH_16_BIT: return "16 bit";
		case SLOT_WIDTH_32_BIT: return "32 bit";
		case SLOT_WIDTH_64_BIT: return "64 bit";
		case SLOT_WIDTH_128_BIT: return "128 bit";
		case SLOT_WIDTH_X1: return "1x or x1";
		case SLOT_WIDTH_X2: return "2x or x2";
		case SLOT_WIDTH_X4: return "4x or x4";
		case SLOT_WIDTH_X8: return "8x or x8";
		case SLOT_WIDTH_X12: return "12x or x12";
		case SLOT_WIDTH_X16: return "16x or x16";
		case SLOT_WIDTH_X32: return "32x or x32";
		default: return "Unknown Slot Width";
	}
}

// Current Usage
/**
 * @brief Decodes an SMBIOS system-slot current-usage value.
 *
 * @param current_usage Raw SMBIOS current-usage value.
 * @return Static string describing the current usage.
 */
const char* lazybiosType9CurrentUsageStr(uint8_t current_usage) {
	switch (current_usage) {
		case SLOT_USAGE_OTHER: return "Other";
		case SLOT_USAGE_UNKNOWN: return "Unknown";
		case SLOT_USAGE_AVAILABLE: return "Available";
		case SLOT_USAGE_IN_USE: return "In use";
		case SLOT_USAGE_UNAVAILABLE: return "Unavailable";
		default: return "Unknown Current Usage";
	}
}

// Slot Length
/**
 * @brief Decodes an SMBIOS system-slot length.
 *
 * @param slot_length Raw SMBIOS slot-length value.
 * @return Static string describing the slot length.
 */
const char* lazybiosType9SlotLengthStr(uint8_t slot_length) {
	switch (slot_length) {
		case SLOT_LENGTH_OTHER: return "Other";
		case SLOT_LENGTH_UNKNOWN: return "Unknown";
		case SLOT_LENGTH_SHORT: return "Short Length";
		case SLOT_LENGTH_LONG: return "Long Length";
		case SLOT_LENGTH_2_5_DRIVE: return "2.5\" drive form factor";
		case SLOT_LENGTH_3_5_DRIVE: return "3.5\" drive form factor";
		default: return "Unknown Slot Length";
	}
}

// Slot Characteristics 1
/**
 * @brief Decodes SMBIOS system-slot characteristics byte 1.
 *
 * @param characteristics Raw SMBIOS slot characteristics byte 1.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType9Characteristics1Str(uint8_t characteristics, char* buf, size_t buf_len) {
	size_t len = 0;
	buf[0] = '\0';

	if (characteristics & (1 << 0)) len += snprintf(buf + len, buf_len - len, "Characteristics unknown, ");
	if (characteristics & (1 << 1)) len += snprintf(buf + len, buf_len - len, "Provides 5.0 volts, ");
	if (characteristics & (1 << 2)) len += snprintf(buf + len, buf_len - len, "Provides 3.3 volts, ");
	if (characteristics & (1 << 3)) len += snprintf(buf + len, buf_len - len, "Opening shared with another slot, ");
	if (characteristics & (1 << 4)) len += snprintf(buf + len, buf_len - len, "PC Card-16 supported, ");
	if (characteristics & (1 << 5)) len += snprintf(buf + len, buf_len - len, "CardBus supported, ");
	if (characteristics & (1 << 6)) len += snprintf(buf + len, buf_len - len, "Zoom Video supported, ");
	if (characteristics & (1 << 7)) len += snprintf(buf + len, buf_len - len, "Modem Ring Resume supported, ");

	if (len == 0) {
		snprintf(buf, buf_len, "None");
	} else if (len >= 2) {
		buf[len - 2] = '\0';
	}
}

// Slot Characteristics 2
/**
 * @brief Decodes SMBIOS system-slot characteristics byte 2.
 *
 * @param characteristics Raw SMBIOS slot characteristics byte 2.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType9Characteristics2Str(uint8_t characteristics, char* buf, size_t buf_len) {
	size_t len = 0;
	buf[0] = '\0';

	if (characteristics & (1 << 0)) len += snprintf(buf + len, buf_len - len, "PME# signal supported, ");
	if (characteristics & (1 << 1)) len += snprintf(buf + len, buf_len - len, "Hot-plug devices supported, ");
	if (characteristics & (1 << 2)) len += snprintf(buf + len, buf_len - len, "SMBus signal supported, ");
	if (characteristics & (1 << 3)) len += snprintf(buf + len, buf_len - len, "PCIe bifurcation supported, ");
	if (characteristics & (1 << 4)) len += snprintf(buf + len, buf_len - len, "Async/surprise removal supported, ");
	if (characteristics & (1 << 5)) len += snprintf(buf + len, buf_len - len, "CXL 1.0 capable, ");
	if (characteristics & (1 << 6)) len += snprintf(buf + len, buf_len - len, "CXL 2.0 capable, ");
	if (characteristics & (1 << 7)) len += snprintf(buf + len, buf_len - len, "CXL 3.0 capable, ");

	if (len == 0) {
		snprintf(buf, buf_len, "None");
	} else if (len >= 2) {
		buf[len - 2] = '\0';
	}
}

// Device/Function Number
/**
 * @brief Decodes an SMBIOS PCI device/function number field.
 *
 * @param device_function_number Raw packed device/function number value.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType9DeviceFunctionStr(uint8_t device_function_number, char* buf, size_t buf_len) {
	if (device_function_number == 0xFF) {
		snprintf(buf, buf_len, "Not Applicable");
		return;
	}

	uint8_t device = (device_function_number >> 3) & 0x1F;
	uint8_t function = device_function_number & 0x07;
	snprintf(buf, buf_len, "Device %u, Function %u", device, function);
}

// Slot Height
/**
 * @brief Decodes an SMBIOS system-slot height.
 *
 * @param slot_height Raw SMBIOS slot-height value.
 * @return Static string describing the slot height.
 */
const char* lazybiosType9SlotHeightStr(uint8_t slot_height) {
	switch (slot_height) {
		case SLOT_HEIGHT_NOT_APPLICABLE: return "Not applicable";
		case SLOT_HEIGHT_OTHER: return "Other";
		case SLOT_HEIGHT_UNKNOWN: return "Unknown";
		case SLOT_HEIGHT_FULL: return "Full height";
		case SLOT_HEIGHT_LOW_PROFILE: return "Low-profile";
		default: return "Unknown Slot Height";
	}
}

// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 9 structures.
 *
 * @param Type9 Type 9 array to release.
 * @param type9_count Number of elements in Type9.
 */
void lazybiosFreeType9(lazybiosType9_t* Type9, size_t type9_count) {
	if (!Type9) return;

	for (size_t i = 0; i < type9_count; i++) {
		free(Type9[i].slot_designation);
		free(Type9[i].peer_groups);
	}

	free(Type9);
}

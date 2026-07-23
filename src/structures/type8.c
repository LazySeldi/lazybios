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
 * @file type8.c
 * @brief Implements parsing and decoding for SMBIOS Type 8 Port Connector Information.
 * @author LazySeldi
 */

//
// Type 8 ( Port Connector Information )
//

#include "lazybios_internal.h"
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define INTERNAL_REFERENCE_DESIGNATOR 0x04
#define INTERNAL_CONNECTOR_TYPE 0x05
#define EXTERNAL_REFERENCE_DESIGNATOR 0x06
#define EXTERNAL_CONNECTOR_TYPE 0x07
#define PORT_TYPE 0x08

// Decoders

// Connector Type
#define CONNECTOR_TYPE_NONE 0x00
#define CONNECTOR_TYPE_CENTRONICS 0x01
#define CONNECTOR_TYPE_MINI_CENTRONICS 0x02
#define CONNECTOR_TYPE_PROPRIETARY 0x03
#define CONNECTOR_TYPE_DB_25_PIN_MALE 0x04
#define CONNECTOR_TYPE_DB_25_PIN_FEMALE 0x05
#define CONNECTOR_TYPE_DB_15_PIN_MALE 0x06
#define CONNECTOR_TYPE_DB_15_PIN_FEMALE 0x07
#define CONNECTOR_TYPE_DB_9_PIN_MALE 0x08
#define CONNECTOR_TYPE_DB_9_PIN_FEMALE 0x09
#define CONNECTOR_TYPE_RJ_11 0x0A
#define CONNECTOR_TYPE_RJ_45 0x0B
#define CONNECTOR_TYPE_50_PIN_MINISCSI 0x0C
#define CONNECTOR_TYPE_MINI_DIN 0x0D
#define CONNECTOR_TYPE_MICRO_DIN 0x0E
#define CONNECTOR_TYPE_PS_2 0x0F
#define CONNECTOR_TYPE_INFRARED 0x10
#define CONNECTOR_TYPE_HP_HIL 0x11
#define CONNECTOR_TYPE_ACCESS_BUS_USB 0x12
#define CONNECTOR_TYPE_SSA_SCSI 0x13
#define CONNECTOR_TYPE_CIRCULAR_DIN_8_MALE 0x14
#define CONNECTOR_TYPE_CIRCULAR_DIN_8_FEMALE 0x15
#define CONNECTOR_TYPE_ON_BOARD_IDE 0x16
#define CONNECTOR_TYPE_ON_BOARD_FLOPPY 0x17
#define CONNECTOR_TYPE_9_PIN_DUAL_INLINE 0x18
#define CONNECTOR_TYPE_25_PIN_DUAL_INLINE 0x19
#define CONNECTOR_TYPE_50_PIN_DUAL_INLINE 0x1A
#define CONNECTOR_TYPE_68_PIN_DUAL_INLINE 0x1B
#define CONNECTOR_TYPE_ON_BOARD_SOUND_INPUT_FROM_CD_ROM 0x1C
#define CONNECTOR_TYPE_MINI_CENTRONICS_TYPE_14 0x1D
#define CONNECTOR_TYPE_MINI_CENTRONICS_TYPE_26 0x1E
#define CONNECTOR_TYPE_MINI_JACK 0x1F
#define CONNECTOR_TYPE_BNC 0x20
#define CONNECTOR_TYPE_1394 0x21
#define CONNECTOR_TYPE_SAS_SATA_PLUG_RECEPTACLE 0x22
#define CONNECTOR_TYPE_USB_TYPE_C_RECEPTACLE 0x23
#define CONNECTOR_TYPE_PC_98 0xA0
#define CONNECTOR_TYPE_PC_98_HIRESO 0xA1
#define CONNECTOR_TYPE_PC_H98 0xA2
#define CONNECTOR_TYPE_PC_98_NOTE 0xA3
#define CONNECTOR_TYPE_PC_98_FULL 0xA4
#define CONNECTOR_TYPE_OTHER 0xFF

// Port Type
#define PORT_TYPE_NONE 0x00
#define PORT_TYPE_PARALLEL_XT_AT_COMPATIBLE 0x01
#define PORT_TYPE_PARALLEL_PS_2 0x02
#define PORT_TYPE_PARALLEL_ECP 0x03
#define PORT_TYPE_PARALLEL_EPP 0x04
#define PORT_TYPE_PARALLEL_ECP_EPP 0x05
#define PORT_TYPE_SERIAL_XT_AT_COMPATIBLE 0x06
#define PORT_TYPE_SERIAL_16450_COMPATIBLE 0x07
#define PORT_TYPE_SERIAL_16550_COMPATIBLE 0x08
#define PORT_TYPE_SERIAL_16550A_COMPATIBLE 0x09
#define PORT_TYPE_SCSI 0x0A
#define PORT_TYPE_MIDI 0x0B
#define PORT_TYPE_JOY_STICK 0x0C
#define PORT_TYPE_KEYBOARD 0x0D
#define PORT_TYPE_MOUSE 0x0E
#define PORT_TYPE_SSA_SCSI 0x0F
#define PORT_TYPE_USB 0x10
#define PORT_TYPE_FIREWIRE_IEEE_P1394 0x11
#define PORT_TYPE_PCMCIA_TYPE_I 0x12
#define PORT_TYPE_PCMCIA_TYPE_II 0x13
#define PORT_TYPE_PCMCIA_TYPE_III 0x14
#define PORT_TYPE_CARD_BUS 0x15
#define PORT_TYPE_ACCESS_BUS 0x16
#define PORT_TYPE_SCSI_II 0x17
#define PORT_TYPE_SCSI_WIDE 0x18
#define PORT_TYPE_PC_98 0x19
#define PORT_TYPE_PC_98_HIRESO 0x1A
#define PORT_TYPE_PC_H98 0x1B
#define PORT_TYPE_VIDEO 0x1C
#define PORT_TYPE_AUDIO 0x1D
#define PORT_TYPE_MODEM 0x1E
#define PORT_TYPE_NETWORK 0x1F
#define PORT_TYPE_SATA 0x20
#define PORT_TYPE_SAS 0x21
#define PORT_TYPE_MFDP 0x22
#define PORT_TYPE_THUNDERBOLT 0x23
#define PORT_TYPE_8251_COMPATIBLE 0xA0
#define PORT_TYPE_8251_FIFO_COMPATIBLE 0xA1
#define PORT_TYPE_OTHER 0xFF
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 8 Port Connector Information structures.
 *
 * @param Type8 Existing Type 8 array pointer value; it is not dereferenced or released.
 * @param type8_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 8 array, or NULL on failure.
 */
lazybiosType8_t* lazybiosGetType8(lazybiosType8_t* Type8, size_t* type8_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_PORT_CONNECTOR);
	size_t index = 0;

	Type8 = calloc(count, sizeof(lazybiosType8_t));
	if (!Type8) return NULL;
	if (count == 0) {
		*type8_count = 0;
		return Type8;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_PORT_CONNECTOR) {
			if (index >= count) break;
			lazybiosType8_t* current = &Type8[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, internal_reference_designator, len, INTERNAL_REFERENCE_DESIGNATOR, p, structure_end);
			READU8(current, internal_connector_type, len, INTERNAL_CONNECTOR_TYPE, p);
			READSTR(current, external_reference_designator, len, EXTERNAL_REFERENCE_DESIGNATOR, p, structure_end);
			READU8(current, external_connector_type, len, EXTERNAL_CONNECTOR_TYPE, p);
			READU8(current, port_type, len, PORT_TYPE, p);

			index++;
		}
		p = DMINext(p, end);
	}
	*type8_count = index;
	return Type8;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders

// Connector Type
/**
 * @brief Decodes an SMBIOS port connector type.
 *
 * @param connector_type Raw SMBIOS internal or external connector type value.
 * @return Static string describing the connector type.
 */
const char* lazybiosType8ConnectorTypeStr(uint8_t connector_type) {
	switch (connector_type) {
		case CONNECTOR_TYPE_NONE:
			return "None";
		case CONNECTOR_TYPE_CENTRONICS:
			return "Centronics";
		case CONNECTOR_TYPE_MINI_CENTRONICS:
			return "Mini Centronics";
		case CONNECTOR_TYPE_PROPRIETARY:
			return "Proprietary";
		case CONNECTOR_TYPE_DB_25_PIN_MALE:
			return "DB-25 pin male";
		case CONNECTOR_TYPE_DB_25_PIN_FEMALE:
			return "DB-25 pin female";
		case CONNECTOR_TYPE_DB_15_PIN_MALE:
			return "DB-15 pin male";
		case CONNECTOR_TYPE_DB_15_PIN_FEMALE:
			return "DB-15 pin female";
		case CONNECTOR_TYPE_DB_9_PIN_MALE:
			return "DB-9 pin male";
		case CONNECTOR_TYPE_DB_9_PIN_FEMALE:
			return "DB-9 pin female";
		case CONNECTOR_TYPE_RJ_11:
			return "RJ-11";
		case CONNECTOR_TYPE_RJ_45:
			return "RJ-45";
		case CONNECTOR_TYPE_50_PIN_MINISCSI:
			return "50-pin MiniSCSI";
		case CONNECTOR_TYPE_MINI_DIN:
			return "Mini-DIN";
		case CONNECTOR_TYPE_MICRO_DIN:
			return "Micro-DIN";
		case CONNECTOR_TYPE_PS_2:
			return "PS/2";
		case CONNECTOR_TYPE_INFRARED:
			return "Infrared";
		case CONNECTOR_TYPE_HP_HIL:
			return "HP-HIL";
		case CONNECTOR_TYPE_ACCESS_BUS_USB:
			return "Access Bus (USB)";
		case CONNECTOR_TYPE_SSA_SCSI:
			return "SSA SCSI";
		case CONNECTOR_TYPE_CIRCULAR_DIN_8_MALE:
			return "Circular DIN-8 male";
		case CONNECTOR_TYPE_CIRCULAR_DIN_8_FEMALE:
			return "Circular DIN-8 female";
		case CONNECTOR_TYPE_ON_BOARD_IDE:
			return "On Board IDE";
		case CONNECTOR_TYPE_ON_BOARD_FLOPPY:
			return "On Board Floppy";
		case CONNECTOR_TYPE_9_PIN_DUAL_INLINE:
			return "9-pin Dual Inline (pin 10 cut)";
		case CONNECTOR_TYPE_25_PIN_DUAL_INLINE:
			return "25-pin Dual Inline (pin 26 cut)";
		case CONNECTOR_TYPE_50_PIN_DUAL_INLINE:
			return "50-pin Dual Inline";
		case CONNECTOR_TYPE_68_PIN_DUAL_INLINE:
			return "68-pin Dual Inline";
		case CONNECTOR_TYPE_ON_BOARD_SOUND_INPUT_FROM_CD_ROM:
			return "On Board Sound Input from CD-ROM";
		case CONNECTOR_TYPE_MINI_CENTRONICS_TYPE_14:
			return "Mini-Centronics Type-14";
		case CONNECTOR_TYPE_MINI_CENTRONICS_TYPE_26:
			return "Mini-Centronics Type-26";
		case CONNECTOR_TYPE_MINI_JACK:
			return "Mini-jack (headphones)";
		case CONNECTOR_TYPE_BNC:
			return "BNC";
		case CONNECTOR_TYPE_1394:
			return "1394";
		case CONNECTOR_TYPE_SAS_SATA_PLUG_RECEPTACLE:
			return "SAS/SATA Plug Receptacle";
		case CONNECTOR_TYPE_USB_TYPE_C_RECEPTACLE:
			return "USB Type-C Receptacle";
		case CONNECTOR_TYPE_PC_98:
			return "PC-98";
		case CONNECTOR_TYPE_PC_98_HIRESO:
			return "PC-98Hireso";
		case CONNECTOR_TYPE_PC_H98:
			return "PC-H98";
		case CONNECTOR_TYPE_PC_98_NOTE:
			return "PC-98Note";
		case CONNECTOR_TYPE_PC_98_FULL:
			return "PC-98Full";
		case CONNECTOR_TYPE_OTHER:
			return "Other";
		default:
			return "Unknown Connector Type";
	}
}

// Port Type
/**
 * @brief Decodes an SMBIOS port type.
 *
 * @param port_type Raw SMBIOS port type value.
 * @return Static string describing the port type.
 */
const char* lazybiosType8PortTypeStr(uint8_t port_type) {
	switch (port_type) {
		case PORT_TYPE_NONE:
			return "None";
		case PORT_TYPE_PARALLEL_XT_AT_COMPATIBLE:
			return "Parallel Port XT/AT Compatible";
		case PORT_TYPE_PARALLEL_PS_2:
			return "Parallel Port PS/2";
		case PORT_TYPE_PARALLEL_ECP:
			return "Parallel Port ECP";
		case PORT_TYPE_PARALLEL_EPP:
			return "Parallel Port EPP";
		case PORT_TYPE_PARALLEL_ECP_EPP:
			return "Parallel Port ECP/EPP";
		case PORT_TYPE_SERIAL_XT_AT_COMPATIBLE:
			return "Serial Port XT/AT Compatible";
		case PORT_TYPE_SERIAL_16450_COMPATIBLE:
			return "Serial Port 16450 Compatible";
		case PORT_TYPE_SERIAL_16550_COMPATIBLE:
			return "Serial Port 16550 Compatible";
		case PORT_TYPE_SERIAL_16550A_COMPATIBLE:
			return "Serial Port 16550A Compatible";
		case PORT_TYPE_SCSI:
			return "SCSI Port";
		case PORT_TYPE_MIDI:
			return "MIDI Port";
		case PORT_TYPE_JOY_STICK:
			return "Joy Stick Port";
		case PORT_TYPE_KEYBOARD:
			return "Keyboard Port";
		case PORT_TYPE_MOUSE:
			return "Mouse Port";
		case PORT_TYPE_SSA_SCSI:
			return "SSA SCSI";
		case PORT_TYPE_USB:
			return "USB";
		case PORT_TYPE_FIREWIRE_IEEE_P1394:
			return "FireWire (IEEE P1394)";
		case PORT_TYPE_PCMCIA_TYPE_I:
			return "PCMCIA Type I";
		case PORT_TYPE_PCMCIA_TYPE_II:
			return "PCMCIA Type II";
		case PORT_TYPE_PCMCIA_TYPE_III:
			return "PCMCIA Type III";
		case PORT_TYPE_CARD_BUS:
			return "Card bus";
		case PORT_TYPE_ACCESS_BUS:
			return "Access Bus Port";
		case PORT_TYPE_SCSI_II:
			return "SCSI II";
		case PORT_TYPE_SCSI_WIDE:
			return "SCSI Wide";
		case PORT_TYPE_PC_98:
			return "PC-98";
		case PORT_TYPE_PC_98_HIRESO:
			return "PC-98-Hireso";
		case PORT_TYPE_PC_H98:
			return "PC-H98";
		case PORT_TYPE_VIDEO:
			return "Video Port";
		case PORT_TYPE_AUDIO:
			return "Audio Port";
		case PORT_TYPE_MODEM:
			return "Modem Port";
		case PORT_TYPE_NETWORK:
			return "Network Port";
		case PORT_TYPE_SATA:
			return "SATA";
		case PORT_TYPE_SAS:
			return "SAS";
		case PORT_TYPE_MFDP:
			return "MFDP (Multi-Function Display Port)";
		case PORT_TYPE_THUNDERBOLT:
			return "Thunderbolt";
		case PORT_TYPE_8251_COMPATIBLE:
			return "8251 Compatible";
		case PORT_TYPE_8251_FIFO_COMPATIBLE:
			return "8251 FIFO Compatible";
		case PORT_TYPE_OTHER:
			return "Other";
		default:
			return "Unknown Port Type";
	}
}

// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 8 structures.
 *
 * @param Type8 Type 8 array to release.
 * @param type8_count Number of elements in Type8.
 */
void lazybiosFreeType8(lazybiosType8_t* Type8, size_t type8_count) {
	if (!Type8) return;

	for (size_t i = 0; i < type8_count; i++) {
		free(Type8[i].internal_reference_designator);
		free(Type8[i].external_reference_designator);
	}

	free(Type8);
}

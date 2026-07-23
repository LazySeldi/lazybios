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
 * @file type42.c
 * @brief Implements parsing and decoding for SMBIOS Type 42 Management Controller Host Interface.
 * @author LazySeldi
 */

//
// Type 42 ( Management Controller Host Interface )
//

#include "lazybios_internal.h"
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define INTERFACE_TYPE 0x04
#define INTERFACE_TYPE_SPECIFIC_DATA_LENGTH 0x05
#define INTERFACE_TYPE_SPECIFIC_DATA 0x06
#define MINIMUM_INTERFACE_TYPE_SPECIFIC_DATA_LENGTH 0x04
#define PRE_3_2_OEM_DATA_LENGTH 0x04

// Interface Types
#define INTERFACE_TYPE_MCTP_MAX 0x3F
#define INTERFACE_TYPE_NETWORK_HOST_INTERFACE 0x40
#define INTERFACE_TYPE_OEM 0xF0

// Protocol Types
#define PROTOCOL_TYPE_RESERVED_0 0x00
#define PROTOCOL_TYPE_RESERVED_1 0x01
#define PROTOCOL_TYPE_IPMI 0x02
#define PROTOCOL_TYPE_MCTP 0x03
#define PROTOCOL_TYPE_REDFISH_OVER_IP 0x04
#define PROTOCOL_TYPE_OEM 0xF0
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 42 Management Controller Host Interface structures.
 *
 * @param Type42 Existing Type 42 array pointer value; it is not dereferenced or released.
 * @param type42_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 42 array, or NULL on failure.
 */
lazybiosType42_t* lazybiosGetType42(lazybiosType42_t* Type42, size_t* type42_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_MANAGEMENT_CONTROLLER_HOST_INTERFACE);
	size_t index = 0;

	Type42 = calloc(count, sizeof(lazybiosType42_t));
	if (!Type42) return NULL;
	if (count == 0) {
		*type42_count = 0;
		return Type42;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_MANAGEMENT_CONTROLLER_HOST_INTERFACE) {
			if (index >= count) break;
			lazybiosType42_t* current = &Type42[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);

			READU8(current, interface_type, len, INTERFACE_TYPE, p);

			if (lazybiosIsVersionPlus(DMIData, 3, 2)) {
				READU8(current, interface_type_specific_data_length, len,
					   INTERFACE_TYPE_SPECIFIC_DATA_LENGTH, p);

				if (current->field_status.interface_type_specific_data_length == LAZYBIOS_FIELD_PRESENT) {
					const size_t interface_data_end = INTERFACE_TYPE_SPECIFIC_DATA +
						current->interface_type_specific_data_length;

					if ((size_t)len >= interface_data_end) {
						current->interface_type_specific_data_size = current->interface_type_specific_data_length;
						if (current->interface_type_specific_data_size > 0) {
							current->interface_type_specific_data = malloc(current->interface_type_specific_data_size);
							if (!current->interface_type_specific_data) {
								lazybiosFreeType42(Type42, count);
								return NULL;
							}
							memcpy(current->interface_type_specific_data, p + INTERFACE_TYPE_SPECIFIC_DATA,
								   current->interface_type_specific_data_size);
						}
						LAZYBIOS_MARK_PRESENT(current, interface_type_specific_data);

						if ((size_t)len > interface_data_end) {
							current->number_of_protocol_records = p[interface_data_end];
							LAZYBIOS_MARK_PRESENT(current, number_of_protocol_records);

							size_t protocol_offset = interface_data_end + 1;
							int protocols_valid = current->interface_type_specific_data_length >=
								MINIMUM_INTERFACE_TYPE_SPECIFIC_DATA_LENGTH;

							for (size_t i = 0; protocols_valid && i < current->number_of_protocol_records; i++) {
								if (protocol_offset + 2 > len) {
									protocols_valid = 0;
									break;
								}

								uint8_t protocol_data_length = p[protocol_offset + 1];
								if ((size_t)protocol_data_length > (size_t)len - protocol_offset - 2) {
									protocols_valid = 0;
									break;
								}
								protocol_offset += 2 + protocol_data_length;
							}

							if (protocols_valid) {
								if (current->number_of_protocol_records > 0) {
									current->protocol_records = calloc(current->number_of_protocol_records,
																	   sizeof(lazybiosType42ProtocolRecord_t));
									if (!current->protocol_records) {
										lazybiosFreeType42(Type42, count);
										return NULL;
									}

									protocol_offset = interface_data_end + 1;
									for (size_t i = 0; i < current->number_of_protocol_records; i++) {
										lazybiosType42ProtocolRecord_t* protocol = &current->protocol_records[i];
										protocol->protocol_type = p[protocol_offset];
										protocol->protocol_type_specific_data_length = p[protocol_offset + 1];
										if (protocol->protocol_type_specific_data_length > 0) {
											protocol->protocol_type_specific_data = malloc(
												protocol->protocol_type_specific_data_length);
											if (!protocol->protocol_type_specific_data) {
												lazybiosFreeType42(Type42, count);
												return NULL;
											}
											memcpy(protocol->protocol_type_specific_data, p + protocol_offset + 2,
												   protocol->protocol_type_specific_data_length);
										}
										LAZYBIOS_MARK_PRESENT(protocol, protocol_type);
										LAZYBIOS_MARK_PRESENT(protocol, protocol_type_specific_data_length);
										LAZYBIOS_MARK_PRESENT(protocol, protocol_type_specific_data);
										protocol_offset += 2 + protocol->protocol_type_specific_data_length;
									}
								}
								LAZYBIOS_MARK_PRESENT(current, protocol_records);
							} else {
								LAZYBIOS_MARK_ABSENT(current, protocol_records);
							}
						}
					} else {
						LAZYBIOS_MARK_ABSENT(current, interface_type_specific_data);
					}
				}
			} else if (current->field_status.interface_type == LAZYBIOS_FIELD_PRESENT &&
					   current->interface_type == INTERFACE_TYPE_OEM &&
					   (size_t)len >= INTERFACE_TYPE_SPECIFIC_DATA_LENGTH + PRE_3_2_OEM_DATA_LENGTH) {
				current->interface_type_specific_data_size = PRE_3_2_OEM_DATA_LENGTH;
				current->interface_type_specific_data = malloc(current->interface_type_specific_data_size);
				if (!current->interface_type_specific_data) {
					lazybiosFreeType42(Type42, count);
					return NULL;
				}
				memcpy(current->interface_type_specific_data, p + INTERFACE_TYPE_SPECIFIC_DATA_LENGTH,
					   current->interface_type_specific_data_size);
				LAZYBIOS_MARK_PRESENT(current, interface_type_specific_data);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type42_count = index;
	return Type42;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes an SMBIOS Type 42 management-controller host-interface type.
 *
 * @param interface_type Raw management-controller host-interface type value.
 * @return Static string describing the interface type.
 */
const char* lazybiosType42InterfaceTypeStr(uint8_t interface_type) {
	if (interface_type <= INTERFACE_TYPE_MCTP_MAX) return "MCTP Host Interface";

	switch (interface_type) {
		case INTERFACE_TYPE_NETWORK_HOST_INTERFACE:
			return "Network Host Interface";
		case INTERFACE_TYPE_OEM:
			return "OEM-defined";
		default:
			return "Reserved";
	}
}

/**
 * @brief Decodes an SMBIOS Type 42 management-controller protocol type.
 *
 * @param protocol_type Raw management-controller protocol type value.
 * @return Static string describing the protocol type.
 */
const char* lazybiosType42ProtocolTypeStr(uint8_t protocol_type) {
	switch (protocol_type) {
		case PROTOCOL_TYPE_RESERVED_0:
		case PROTOCOL_TYPE_RESERVED_1:
			return "Reserved";
		case PROTOCOL_TYPE_IPMI:
			return "IPMI: Intelligent Platform Management Interface";
		case PROTOCOL_TYPE_MCTP:
			return "MCTP: Management Component Transport Protocol";
		case PROTOCOL_TYPE_REDFISH_OVER_IP:
			return "Redfish over IP";
		case PROTOCOL_TYPE_OEM:
			return "OEM-defined";
		default:
			return "Reserved";
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 42 structures.
 *
 * @param Type42 Type 42 array to release.
 * @param type42_count Number of elements in Type42.
 */
void lazybiosFreeType42(lazybiosType42_t* Type42, size_t type42_count) {
	if (!Type42) return;

	for (size_t i = 0; i < type42_count; i++) {
		free(Type42[i].interface_type_specific_data);
		if (Type42[i].protocol_records) {
			for (size_t j = 0; j < Type42[i].number_of_protocol_records; j++) {
				free(Type42[i].protocol_records[j].protocol_type_specific_data);
			}
		}
		free(Type42[i].protocol_records);
	}
	free(Type42);
}

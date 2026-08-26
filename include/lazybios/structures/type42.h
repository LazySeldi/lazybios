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
 * @file type42.h
 * @brief Public API for SMBIOS Type 42 Management Controller Host Interface.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE42_H
#define LAZYBIOS_TYPE42_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for an SMBIOS Type 42 protocol record. */
typedef struct {
	lazybiosFieldStatus_t protocol_type;
	lazybiosFieldStatus_t protocol_type_specific_data_length;
	lazybiosFieldStatus_t protocol_type_specific_data;
} lazybiosType42ProtocolRecordFieldStatus_t;

/**
 * @brief Decoded forms of the encoded fields in one record.
 */
typedef struct {
	const char* protocol_type;
} lazybiosType42ProtocolRecordDecoded_t;

/**
 * @brief Parsed protocol record from an SMBIOS Type 42 structure.
 * @ingroup api_type42
 */
typedef struct {
	uint8_t protocol_type;
	uint8_t protocol_type_specific_data_length;
	uint8_t* protocol_type_specific_data;
	lazybiosType42ProtocolRecordFieldStatus_t field_status;
	lazybiosType42ProtocolRecordDecoded_t decoded;
} lazybiosType42ProtocolRecord_t;

/** @brief Availability metadata for SMBIOS Type 42 fields. */
typedef struct {
	lazybiosFieldStatus_t interface_type;
	lazybiosFieldStatus_t interface_type_specific_data_length;
	lazybiosFieldStatus_t interface_type_specific_data;
	lazybiosFieldStatus_t number_of_protocol_records;
	lazybiosFieldStatus_t protocol_records;
} lazybiosType42FieldStatus_t;

/**
 * @brief Decoded forms of the Type 42 encoded fields.
 */
typedef struct {
	const char* interface_type;
} lazybiosType42Decoded_t;

/**
 * @brief Parsed SMBIOS Type 42 Management Controller Host Interface.
 * @ingroup api_type42
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t interface_type;
	uint8_t interface_type_specific_data_length;
	size_t interface_type_specific_data_size;
	uint8_t* interface_type_specific_data;
	uint8_t number_of_protocol_records;
	lazybiosType42ProtocolRecord_t* protocol_records;
	lazybiosType42Decoded_t decoded;
	lazybiosType42FieldStatus_t field_status;
} lazybiosType42_t;

/**
 * @brief A parsed set of SMBIOS Type 42 structures.
 * @ingroup api_type42
 */
typedef struct {
	lazybiosType42_t* entries;
	size_t count;
} lazybiosType42Array_t;

/** @addtogroup api_type42
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 42 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 42 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType42Array_t* lazybiosGetType42(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 42 structures.
 * @param Type42 Set to release; may be NULL.
 */
void lazybiosFreeType42(lazybiosType42Array_t* Type42);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

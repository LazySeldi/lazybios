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
 * @file type38.h
 * @brief Public API for SMBIOS Type 38 IPMI Device Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE38_H
#define LAZYBIOS_TYPE38_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "lazybios/lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 38 fields. */
typedef struct {
	lazybiosFieldStatus_t interface_type;
	lazybiosFieldStatus_t ipmi_specification_revision;
	lazybiosFieldStatus_t i2c_target_address;
	lazybiosFieldStatus_t nv_storage_device_address;
	lazybiosFieldStatus_t base_address;
	lazybiosFieldStatus_t base_address_modifier_interrupt_info;
	lazybiosFieldStatus_t interrupt_number;
} lazybiosType38FieldStatus_t;

/**
 * @brief Decoded forms of the Type 38 encoded fields.
 *
 * Each member holds the decoded form of the raw field it is named for.
 * Consult the matching `field_status` member before using one.
 */
typedef struct {
	const char* base_address_type;
	const char* register_spacing;
	const char* interface_type;
	uint64_t base_address;
	char* ipmi_specification_revision;
	char* base_address_modifier_interrupt_info;
} lazybiosType38Decoded_t;

/**
 * @brief Parsed SMBIOS Type 38 IPMI Device Information.
 * @ingroup api_type38
 */
typedef struct {
	uint16_t handle;
	uint8_t length;
	uint8_t interface_type;
	uint8_t ipmi_specification_revision;
	uint8_t i2c_target_address;
	uint8_t nv_storage_device_address;
	uint64_t base_address;
	uint8_t base_address_modifier_interrupt_info;
	uint8_t interrupt_number;
	lazybiosType38Decoded_t decoded;
	lazybiosType38FieldStatus_t field_status;
} lazybiosType38_t;

/**
 * @brief A parsed set of SMBIOS Type 38 structures.
 * @ingroup api_type38
 */
typedef struct {
	lazybiosType38_t* entries;
	size_t count;
} lazybiosType38Array_t;

/** @addtogroup api_type38
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 38 structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated set, empty when the table holds no Type 38 structure,
 *         or NULL when the arguments are unusable or an allocation fails.
 */
LAZYBIOS_WARN_UNUSED lazybiosType38Array_t* lazybiosGetType38(const lazybiosDMI_t* DMIData);

/**
 * @brief Releases a parsed set of SMBIOS Type 38 structures.
 * @param Type38 Set to release; may be NULL.
 */
void lazybiosFreeType38(lazybiosType38Array_t* Type38);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

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
 * @file type44.c
 * @brief Implements parsing and decoding for SMBIOS Type 44 Processor Additional Information.
 * @author LazySeldi
 */

//
// Type 44 ( Processor Additional Information )
//

#include "lazybios_internal.h"
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define REFERENCED_HANDLE 0x04
#define BLOCK_LENGTH 0x06
#define PROCESSOR_TYPE 0x07
#define PROCESSOR_SPECIFIC_DATA 0x08

// Processor Architecture Types
#define PROCESSOR_TYPE_RESERVED 0x00
#define PROCESSOR_TYPE_IA32 0x01
#define PROCESSOR_TYPE_X64 0x02
#define PROCESSOR_TYPE_ITANIUM 0x03
#define PROCESSOR_TYPE_ARM32 0x04
#define PROCESSOR_TYPE_ARM64 0x05
#define PROCESSOR_TYPE_RISCV32 0x06
#define PROCESSOR_TYPE_RISCV64 0x07
#define PROCESSOR_TYPE_RISCV128 0x08
#define PROCESSOR_TYPE_LOONGARCH32 0x09
#define PROCESSOR_TYPE_LOONGARCH64 0x0A
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 44 Processor Additional Information structures.
 *
 * @param Type44 Existing Type 44 array pointer value; it is not dereferenced or released.
 * @param type44_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 44 array, or NULL on failure.
 */
lazybiosType44_t* lazybiosGetType44(lazybiosType44_t* Type44, size_t* type44_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_PROCESSOR_ADDITIONAL_INFORMATION);
	size_t index = 0;

	Type44 = calloc(count, sizeof(lazybiosType44_t));
	if (!Type44) return NULL;
	if (count == 0) {
		*type44_count = 0;
		return Type44;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_PROCESSOR_ADDITIONAL_INFORMATION) {
			if (index >= count) break;
			lazybiosType44_t* current = &Type44[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);

			READU16(current, referenced_handle, len, REFERENCED_HANDLE, p);
			if (current->referenced_handle == 0xFFFF) LAZYBIOS_MARK_ABSENT(current, referenced_handle);
			READU8(current, block_length, len, BLOCK_LENGTH, p);
			READU8(current, processor_type, len, PROCESSOR_TYPE, p);

			if (current->field_status.block_length == LAZYBIOS_FIELD_PRESENT &&
				current->field_status.processor_type == LAZYBIOS_FIELD_PRESENT &&
				(size_t)len >= (size_t)PROCESSOR_SPECIFIC_DATA + current->block_length) {
				if (current->block_length > 0) {
					current->processor_specific_data = malloc(current->block_length);
					if (!current->processor_specific_data) {
						lazybiosFreeType44(Type44, count);
						return NULL;
					}
					memcpy(current->processor_specific_data, p + PROCESSOR_SPECIFIC_DATA, current->block_length);
				}
				LAZYBIOS_MARK_PRESENT(current, processor_specific_data);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type44_count = index;
	return Type44;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes an SMBIOS Type 44 processor architecture type.
 *
 * @param processor_type Raw processor architecture type value.
 * @return Static string describing the processor architecture.
 */
const char* lazybiosType44ProcessorTypeStr(uint8_t processor_type) {
	switch (processor_type) {
		case PROCESSOR_TYPE_RESERVED:
			return "Reserved";
		case PROCESSOR_TYPE_IA32:
			return "IA32 (x86)";
		case PROCESSOR_TYPE_X64:
			return "x64 (x86-64, Intel64, AMD64, EM64T)";
		case PROCESSOR_TYPE_ITANIUM:
			return "Intel Itanium Architecture";
		case PROCESSOR_TYPE_ARM32:
			return "32-bit ARM (AArch32)";
		case PROCESSOR_TYPE_ARM64:
			return "64-bit ARM (AArch64)";
		case PROCESSOR_TYPE_RISCV32:
			return "32-bit RISC-V (RV32)";
		case PROCESSOR_TYPE_RISCV64:
			return "64-bit RISC-V (RV64)";
		case PROCESSOR_TYPE_RISCV128:
			return "128-bit RISC-V (RV128)";
		case PROCESSOR_TYPE_LOONGARCH32:
			return "32-bit LoongArch (LoongArch32)";
		case PROCESSOR_TYPE_LOONGARCH64:
			return "64-bit LoongArch (LoongArch64)";
		default:
			return "Undefined";
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 44 structures.
 *
 * @param Type44 Type 44 array to release.
 * @param type44_count Number of elements in Type44.
 */
void lazybiosFreeType44(lazybiosType44_t* Type44, size_t type44_count) {
	if (!Type44) return;

	for (size_t i = 0; i < type44_count; i++) {
		free(Type44[i].processor_specific_data);
	}
	free(Type44);
}

/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type44.c
 * @brief Implements parsing and decoding for SMBIOS Type 44 Processor Additional Information.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>
#include <string.h>

/* File-local decoders; their results are stored in each record's `decoded`. */
static inline const char* lazybiosType44ProcessorTypeStr(uint8_t processor_type);

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

lazybiosType44Array_t* lazybiosGetType44(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType44Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_PROCESSOR_ADDITIONAL_INFORMATION);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_PROCESSOR_ADDITIONAL_INFORMATION].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_PROCESSOR_ADDITIONAL_INFORMATION].first;
	}
	size_t index = 0;

	if (count == 0) return out;

	out->entries = calloc(count, sizeof(*out->entries));
	if (!out->entries) {
		free(out);
		return NULL;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];
		if (len < SMBIOS_HEADER_SIZE) break;

		if (type == SMBIOS_TYPE_PROCESSOR_ADDITIONAL_INFORMATION) {
			if (index >= count) break;
			lazybiosType44_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

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
						out->count = index + 1;
						lazybiosFreeType44(out);
						return NULL;
					}
					memcpy(current->processor_specific_data, p + PROCESSOR_SPECIFIC_DATA, current->block_length);
				}
				LAZYBIOS_MARK_PRESENT(current, processor_specific_data);
			}

			current->decoded.processor_type = lazybiosType44ProcessorTypeStr(current->processor_type);

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

static inline const char* lazybiosType44ProcessorTypeStr(uint8_t processor_type) {
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

void lazybiosFreeType44(lazybiosType44Array_t* Type44) {
    if (!Type44) return;

	for (size_t i = 0; i < Type44->count; i++) free(Type44->entries[i].processor_specific_data);

    free(Type44->entries);

    free(Type44);
}

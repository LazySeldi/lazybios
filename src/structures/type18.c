/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type18.c
 * @brief Implements parsing and decoding for SMBIOS Type 18 32-Bit Memory Error Information.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdlib.h>

/* File-local decoders; their results are stored in each record's `decoded`. */
static inline const char* lazybiosType18ErrorGranularityStr(uint8_t error_granularity);
static inline const char* lazybiosType18ErrorOperationStr(uint8_t error_operation);
static inline const char* lazybiosType18ErrorTypeStr(uint8_t error_type);

// Fields
#define ERROR_TYPE 0x04
#define ERROR_GRANULARITY 0x05
#define ERROR_OPERATION 0x06
#define VENDOR_SYNDROME 0x07
#define MEMORY_ARRAY_ERROR_ADDRESS 0x0B
#define DEVICE_ERROR_ADDRESS 0x0F
#define ERROR_RESOLUTION 0x13

// Error Types
#define ERROR_TYPE_OTHER 0x01
#define ERROR_TYPE_UNKNOWN 0x02
#define ERROR_TYPE_OK 0x03
#define ERROR_TYPE_BAD_READ 0x04
#define ERROR_TYPE_PARITY_ERROR 0x05
#define ERROR_TYPE_SINGLE_BIT_ERROR 0x06
#define ERROR_TYPE_DOUBLE_BIT_ERROR 0x07
#define ERROR_TYPE_MULTI_BIT_ERROR 0x08
#define ERROR_TYPE_NIBBLE_ERROR 0x09
#define ERROR_TYPE_CHECKSUM_ERROR 0x0A
#define ERROR_TYPE_CRC_ERROR 0x0B
#define ERROR_TYPE_CORRECTED_SINGLE_BIT_ERROR 0x0C
#define ERROR_TYPE_CORRECTED_ERROR 0x0D
#define ERROR_TYPE_UNCORRECTABLE_ERROR 0x0E

// Error Granularities
#define ERROR_GRANULARITY_OTHER 0x01
#define ERROR_GRANULARITY_UNKNOWN 0x02
#define ERROR_GRANULARITY_DEVICE_LEVEL 0x03
#define ERROR_GRANULARITY_MEMORY_PARTITION_LEVEL 0x04

// Error Operations
#define ERROR_OPERATION_OTHER 0x01
#define ERROR_OPERATION_UNKNOWN 0x02
#define ERROR_OPERATION_READ 0x03
#define ERROR_OPERATION_WRITE 0x04
#define ERROR_OPERATION_PARTIAL_WRITE 0x05

lazybiosType18Array_t* lazybiosGetType18(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType18Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFORMATION);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFORMATION].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFORMATION].first;
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

		if (type == SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFORMATION) {
			if (index >= count) break;
			lazybiosType18_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READU8(current, error_type, len, ERROR_TYPE, p);
			READU8(current, error_granularity, len, ERROR_GRANULARITY, p);
			READU8(current, error_operation, len, ERROR_OPERATION, p);
			READU32(current, vendor_syndrome, len, VENDOR_SYNDROME, p);
			READU32(current, memory_array_error_address, len, MEMORY_ARRAY_ERROR_ADDRESS, p);
			READU32(current, device_error_address, len, DEVICE_ERROR_ADDRESS, p);
			READU32(current, error_resolution, len, ERROR_RESOLUTION, p);

			current->decoded.error_granularity = lazybiosType18ErrorGranularityStr(current->error_granularity);
			current->decoded.error_operation = lazybiosType18ErrorOperationStr(current->error_operation);
			current->decoded.error_type = lazybiosType18ErrorTypeStr(current->error_type);

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

static inline const char* lazybiosType18ErrorTypeStr(uint8_t error_type) {
	switch (error_type) {
		case ERROR_TYPE_OTHER:
			return "Other";
		case ERROR_TYPE_UNKNOWN:
			return "Unknown";
		case ERROR_TYPE_OK:
			return "OK";
		case ERROR_TYPE_BAD_READ:
			return "Bad Read";
		case ERROR_TYPE_PARITY_ERROR:
			return "Parity Error";
		case ERROR_TYPE_SINGLE_BIT_ERROR:
			return "Single-bit Error";
		case ERROR_TYPE_DOUBLE_BIT_ERROR:
			return "Double-bit Error";
		case ERROR_TYPE_MULTI_BIT_ERROR:
			return "Multi-bit Error";
		case ERROR_TYPE_NIBBLE_ERROR:
			return "Nibble Error";
		case ERROR_TYPE_CHECKSUM_ERROR:
			return "Checksum Error";
		case ERROR_TYPE_CRC_ERROR:
			return "CRC Error";
		case ERROR_TYPE_CORRECTED_SINGLE_BIT_ERROR:
			return "Corrected Single-bit Error";
		case ERROR_TYPE_CORRECTED_ERROR:
			return "Corrected Error";
		case ERROR_TYPE_UNCORRECTABLE_ERROR:
			return "Uncorrectable Error";
		default:
			return "Undefined";
	}
}

static inline const char* lazybiosType18ErrorGranularityStr(uint8_t error_granularity) {
	switch (error_granularity) {
		case ERROR_GRANULARITY_OTHER:
			return "Other";
		case ERROR_GRANULARITY_UNKNOWN:
			return "Unknown";
		case ERROR_GRANULARITY_DEVICE_LEVEL:
			return "Device Level";
		case ERROR_GRANULARITY_MEMORY_PARTITION_LEVEL:
			return "Memory Partition Level";
		default:
			return "Undefined";
	}
}

static inline const char* lazybiosType18ErrorOperationStr(uint8_t error_operation) {
	switch (error_operation) {
		case ERROR_OPERATION_OTHER:
			return "Other";
		case ERROR_OPERATION_UNKNOWN:
			return "Unknown";
		case ERROR_OPERATION_READ:
			return "Read";
		case ERROR_OPERATION_WRITE:
			return "Write";
		case ERROR_OPERATION_PARTIAL_WRITE:
			return "Partial Write";
		default:
			return "Undefined";
	}
}

void lazybiosFreeType18(lazybiosType18Array_t* Type18) {
    if (!Type18) return;

    free(Type18->entries);

    free(Type18);
}

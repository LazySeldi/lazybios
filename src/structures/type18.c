/**
 * @file type18.c
 * @brief Implements parsing and decoding for SMBIOS Type 18 32-Bit Memory Error Information.
 * @author LazySeldi
 */

//
// Type 18 ( 32-Bit Memory Error Information )
//

#include "lazybios_internal.h"
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 18 32-Bit Memory Error Information structures.
 *
 * @param Type18 Existing Type 18 array pointer value; it is not dereferenced or released.
 * @param type18_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 18 array, or NULL on failure.
 */
lazybiosType18_t* lazybiosGetType18(lazybiosType18_t* Type18, size_t* type18_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFORMATION);
	size_t index = 0;

	Type18 = calloc(count, sizeof(lazybiosType18_t));
	if (!Type18) return NULL;
	if (count == 0) {
		*type18_count = 0;
		return Type18;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFORMATION) {
			if (index >= count) break;
			lazybiosType18_t* current = &Type18[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);

			READU8(current, error_type, len, ERROR_TYPE, p);
			READU8(current, error_granularity, len, ERROR_GRANULARITY, p);
			READU8(current, error_operation, len, ERROR_OPERATION, p);
			READU32(current, vendor_syndrome, len, VENDOR_SYNDROME, p);
			READU32(current, memory_array_error_address, len, MEMORY_ARRAY_ERROR_ADDRESS, p);
			READU32(current, device_error_address, len, DEVICE_ERROR_ADDRESS, p);
			READU32(current, error_resolution, len, ERROR_RESOLUTION, p);

			index++;
		}
		p = DMINext(p, end);
	}
	*type18_count = index;
	return Type18;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes an SMBIOS memory-error type.
 *
 * @param error_type Raw SMBIOS memory-error type value.
 * @return Static string describing the error type.
 */
const char* lazybiosType18ErrorTypeStr(uint8_t error_type) {
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

/**
 * @brief Decodes SMBIOS memory-error granularity.
 *
 * @param error_granularity Raw SMBIOS error-granularity value.
 * @return Static string describing the granularity.
 */
const char* lazybiosType18ErrorGranularityStr(uint8_t error_granularity) {
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

/**
 * @brief Decodes the operation that caused an SMBIOS memory error.
 *
 * @param error_operation Raw SMBIOS error-operation value.
 * @return Static string describing the operation.
 */
const char* lazybiosType18ErrorOperationStr(uint8_t error_operation) {
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 18 structures.
 *
 * @param Type18 Type 18 array to release.
 * @param type18_count Number of elements in Type18.
 */
void lazybiosFreeType18(lazybiosType18_t* Type18, size_t type18_count) {
	(void)type18_count;
	free(Type18);
}

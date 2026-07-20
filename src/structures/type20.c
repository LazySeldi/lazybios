/**
 * @file type20.c
 * @brief Implements parsing for SMBIOS Type 20 Memory Device Mapped Address.
 * @author LazySeldi
 */

//
// Type 20 ( Memory Device Mapped Address )
//

#include "lazybios_internal.h"
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define STARTING_ADDRESS 0x04
#define ENDING_ADDRESS 0x08
#define MEMORY_DEVICE_HANDLE 0x0C
#define MEMORY_ARRAY_MAPPED_ADDRESS_HANDLE 0x0E
#define PARTITION_ROW_POSITION 0x10
#define INTERLEAVE_POSITION 0x11
#define INTERLEAVED_DATA_DEPTH 0x12
#define EXTENDED_STARTING_ADDRESS 0x13
#define EXTENDED_ENDING_ADDRESS 0x1B

// Address Selection
#define USE_EXTENDED_ADDRESS 0xFFFFFFFFU
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 20 Memory Device Mapped Address structures.
 *
 * @param Type20 Existing Type 20 array pointer value; it is not dereferenced or released.
 * @param type20_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 20 array, or NULL on failure.
 */
lazybiosType20_t* lazybiosGetType20(lazybiosType20_t* Type20, size_t* type20_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS);
	size_t index = 0;

	Type20 = calloc(count, sizeof(lazybiosType20_t));
	if (!Type20) return NULL;
	if (count == 0) {
		*type20_count = 0;
		return Type20;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS) {
			if (index >= count) break;
			lazybiosType20_t* current = &Type20[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);

			READU32(current, starting_address, len, STARTING_ADDRESS, p);
			READU32(current, ending_address, len, ENDING_ADDRESS, p);
			READU16(current, memory_device_handle, len, MEMORY_DEVICE_HANDLE, p);
			READU16(current, memory_array_mapped_address_handle, len, MEMORY_ARRAY_MAPPED_ADDRESS_HANDLE, p);
			READU8(current, partition_row_position, len, PARTITION_ROW_POSITION, p);
			READU8(current, interleave_position, len, INTERLEAVE_POSITION, p);
			READU8(current, interleaved_data_depth, len, INTERLEAVED_DATA_DEPTH, p);

			if (lazybiosIsVersionPlus(DMIData, 2, 7)) {
				READU64(current, extended_starting_address, len, EXTENDED_STARTING_ADDRESS, p);
				READU64(current, extended_ending_address, len, EXTENDED_ENDING_ADDRESS, p);
			} else {
				current->extended_starting_address = 0;
				current->extended_ending_address = 0;
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type20_count = index;
	return Type20;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Converts SMBIOS Type 20 starting-address fields to a byte address.
 *
 * @param starting_address Raw 32-bit starting address in KiB.
 * @param extended_starting_address Raw extended starting address in bytes.
 * @return Effective starting address in bytes.
 */
uint64_t lazybiosType20StartingAddressBytes(uint32_t starting_address, uint64_t extended_starting_address) {
	if (starting_address == USE_EXTENDED_ADDRESS) return extended_starting_address;
	return (uint64_t)starting_address * 1024;
}

/**
 * @brief Converts SMBIOS Type 20 ending-address fields to an inclusive byte address.
 *
 * @param ending_address Raw 32-bit ending address identifying the last KiB.
 * @param extended_ending_address Raw extended inclusive ending address in bytes.
 * @return Effective inclusive ending address in bytes.
 */
uint64_t lazybiosType20EndingAddressBytes(uint32_t ending_address, uint64_t extended_ending_address) {
	if (ending_address == USE_EXTENDED_ADDRESS) return extended_ending_address;
	return (uint64_t)ending_address * 1024 + 1023;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 20 structures.
 *
 * @param Type20 Type 20 array to release.
 * @param type20_count Number of elements in Type20.
 */
void lazybiosFreeType20(lazybiosType20_t* Type20, size_t type20_count) {
	(void)type20_count;
	free(Type20);
}

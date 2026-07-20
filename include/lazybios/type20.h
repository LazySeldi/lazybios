/**
 * @file type20.h
 * @brief Public API for SMBIOS Type 20 Memory Device Mapped Address.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE20_H
#define LAZYBIOS_TYPE20_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 20 fields. */
typedef struct {
	lazybiosFieldStatus_t starting_address;
	lazybiosFieldStatus_t ending_address;
	lazybiosFieldStatus_t memory_device_handle;
	lazybiosFieldStatus_t memory_array_mapped_address_handle;
	lazybiosFieldStatus_t partition_row_position;
	lazybiosFieldStatus_t interleave_position;
	lazybiosFieldStatus_t interleaved_data_depth;
	lazybiosFieldStatus_t extended_starting_address;
	lazybiosFieldStatus_t extended_ending_address;
} lazybiosType20FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 20 Memory Device Mapped Address.
 * @ingroup api_type20
 */
typedef struct {
	uint32_t starting_address;
	uint32_t ending_address;
	uint16_t memory_device_handle;
	uint16_t memory_array_mapped_address_handle;
	uint8_t partition_row_position;
	uint8_t interleave_position;
	uint8_t interleaved_data_depth;
	uint64_t extended_starting_address;
	uint64_t extended_ending_address;
	lazybiosType20FieldStatus_t field_status;
} lazybiosType20_t;

/** @addtogroup api_type20
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 20 Memory Device Mapped Address structures.
 * @param Type20 Existing Type 20 array pointer value; it is not dereferenced or released.
 * @param type20_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 20 array, or NULL on failure.
 */
lazybiosType20_t* lazybiosGetType20(lazybiosType20_t* Type20, size_t* type20_count, lazybiosDMI_t* DMIData);

/**
 * @brief Converts SMBIOS Type 20 starting-address fields to a byte address.
 * @param starting_address Raw 32-bit starting address in KiB.
 * @param extended_starting_address Raw extended starting address in bytes.
 * @return Effective starting address in bytes.
 */
uint64_t lazybiosType20StartingAddressBytes(uint32_t starting_address, uint64_t extended_starting_address);

/**
 * @brief Converts SMBIOS Type 20 ending-address fields to an inclusive byte address.
 * @param ending_address Raw 32-bit ending address identifying the last KiB.
 * @param extended_ending_address Raw extended inclusive ending address in bytes.
 * @return Effective inclusive ending address in bytes.
 */
uint64_t lazybiosType20EndingAddressBytes(uint32_t ending_address, uint64_t extended_ending_address);

/**
 * @brief Releases an array of parsed SMBIOS Type 20 structures.
 * @param Type20 Type 20 array to release.
 * @param type20_count Number of elements in Type20.
 */
void lazybiosFreeType20(lazybiosType20_t* Type20, size_t type20_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

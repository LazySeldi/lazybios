/**
 * @file type19.h
 * @brief Public API for SMBIOS Type 19 Memory Array Mapped Address.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE19_H
#define LAZYBIOS_TYPE19_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 19 fields. */
typedef struct {
	lazybiosFieldStatus_t starting_address;
	lazybiosFieldStatus_t ending_address;
	lazybiosFieldStatus_t memory_array_handle;
	lazybiosFieldStatus_t partition_width;
	lazybiosFieldStatus_t extended_starting_address;
	lazybiosFieldStatus_t extended_ending_address;
} lazybiosType19FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 19 Memory Array Mapped Address.
 * @ingroup api_type19
 */
typedef struct {
	uint32_t starting_address;
	uint32_t ending_address;
	uint16_t memory_array_handle;
	uint8_t partition_width;
	uint64_t extended_starting_address;
	uint64_t extended_ending_address;
	lazybiosType19FieldStatus_t field_status;
} lazybiosType19_t;

/** @addtogroup api_type19
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 19 Memory Array Mapped Address structures.
 * @param Type19 Existing Type 19 array pointer value; it is not dereferenced or released.
 * @param type19_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 19 array, or NULL on failure.
 */
lazybiosType19_t* lazybiosGetType19(lazybiosType19_t* Type19, size_t* type19_count, lazybiosDMI_t* DMIData);

/**
 * @brief Converts SMBIOS Type 19 starting-address fields to a byte address.
 * @param starting_address Raw 32-bit starting address in KiB.
 * @param extended_starting_address Raw extended starting address in bytes.
 * @return Effective starting address in bytes.
 */
uint64_t lazybiosType19StartingAddressBytes(uint32_t starting_address, uint64_t extended_starting_address);

/**
 * @brief Converts SMBIOS Type 19 ending-address fields to an inclusive byte address.
 * @param ending_address Raw 32-bit ending address identifying the last KiB.
 * @param extended_ending_address Raw extended inclusive ending address in bytes.
 * @return Effective inclusive ending address in bytes.
 */
uint64_t lazybiosType19EndingAddressBytes(uint32_t ending_address, uint64_t extended_ending_address);

/**
 * @brief Releases an array of parsed SMBIOS Type 19 structures.
 * @param Type19 Type 19 array to release.
 * @param type19_count Number of elements in Type19.
 */
void lazybiosFreeType19(lazybiosType19_t* Type19, size_t type19_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

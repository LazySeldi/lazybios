/**
 * @file type16.h
 * @brief Public API for SMBIOS Type 16 Physical Memory Array Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE16_H
#define LAZYBIOS_TYPE16_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 16 fields. */
typedef struct {
	lazybiosFieldStatus_t location;
	lazybiosFieldStatus_t use;
	lazybiosFieldStatus_t memory_error_correction;
	lazybiosFieldStatus_t maximum_capacity;
	lazybiosFieldStatus_t memory_error_information_handle;
	lazybiosFieldStatus_t number_of_memory_devices;
	lazybiosFieldStatus_t extended_maximum_capacity;
} lazybiosType16FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 16 Physical Memory Array Information.
 * @ingroup api_type16
 */
typedef struct {
	uint8_t location;
	uint8_t use;
	uint8_t memory_error_correction;
	uint32_t maximum_capacity;
	uint16_t memory_error_information_handle;
	uint16_t number_of_memory_devices;
	uint64_t extended_maximum_capacity;
	lazybiosType16FieldStatus_t field_status;
} lazybiosType16_t;

/** @addtogroup api_type16
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 16 Physical Memory Array structures.
 * @param Type16 Existing Type 16 array pointer value; it is not dereferenced or released.
 * @param type16_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 16 array, or NULL on failure.
 */
lazybiosType16_t* lazybiosGetType16(lazybiosType16_t* Type16, size_t* type16_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an SMBIOS physical memory array location.
 * @param location Raw SMBIOS memory-array location value.
 * @return Static string describing the location.
 */
const char* lazybiosType16LocationStr(uint8_t location);

/**
 * @brief Decodes the function of an SMBIOS physical memory array.
 * @param use Raw SMBIOS memory-array use value.
 * @return Static string describing the array function.
 */
const char* lazybiosType16UseStr(uint8_t use);

/**
 * @brief Decodes an SMBIOS physical memory array error-correction method.
 * @param memory_error_correction Raw SMBIOS error-correction value.
 * @return Static string describing the error-correction method.
 */
const char* lazybiosType16MemoryErrorCorrectionStr(uint8_t memory_error_correction);

/**
 * @brief Converts SMBIOS Type 16 maximum-capacity fields to bytes.
 * @param maximum_capacity Raw 32-bit maximum capacity in KiB.
 * @param extended_maximum_capacity Raw extended maximum capacity in bytes.
 * @return Maximum array capacity in bytes.
 */
uint64_t lazybiosType16MaximumCapacityBytes(uint32_t maximum_capacity, uint64_t extended_maximum_capacity);

/**
 * @brief Releases an array of parsed SMBIOS Type 16 structures.
 * @param Type16 Type 16 array to release.
 * @param type16_count Number of elements in Type16.
 */
void lazybiosFreeType16(lazybiosType16_t* Type16, size_t type16_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

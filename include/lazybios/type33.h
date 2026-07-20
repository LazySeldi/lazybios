/**
 * @file type33.h
 * @brief Public API for SMBIOS Type 33 64-Bit Memory Error Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE33_H
#define LAZYBIOS_TYPE33_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 33 fields. */
typedef struct {
	lazybiosFieldStatus_t error_type;
	lazybiosFieldStatus_t error_granularity;
	lazybiosFieldStatus_t error_operation;
	lazybiosFieldStatus_t vendor_syndrome;
	lazybiosFieldStatus_t memory_array_error_address;
	lazybiosFieldStatus_t device_error_address;
	lazybiosFieldStatus_t error_resolution;
} lazybiosType33FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 33 64-Bit Memory Error Information.
 * @ingroup api_type33
 */
typedef struct {
	uint8_t error_type;
	uint8_t error_granularity;
	uint8_t error_operation;
	uint32_t vendor_syndrome;
	uint64_t memory_array_error_address;
	uint64_t device_error_address;
	uint32_t error_resolution;
	lazybiosType33FieldStatus_t field_status;
} lazybiosType33_t;

/** @addtogroup api_type33
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 33 64-Bit Memory Error Information structures.
 * @param Type33 Existing Type 33 array pointer value; it is not dereferenced or released.
 * @param type33_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 33 array, or NULL on failure.
 */
lazybiosType33_t* lazybiosGetType33(lazybiosType33_t* Type33, size_t* type33_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an SMBIOS 64-bit memory-error type.
 * @param error_type Raw SMBIOS memory-error type value.
 * @return Static string describing the error type.
 */
const char* lazybiosType33ErrorTypeStr(uint8_t error_type);

/**
 * @brief Decodes SMBIOS 64-bit memory-error granularity.
 * @param error_granularity Raw SMBIOS error-granularity value.
 * @return Static string describing the granularity.
 */
const char* lazybiosType33ErrorGranularityStr(uint8_t error_granularity);

/**
 * @brief Decodes the operation that caused an SMBIOS 64-bit memory error.
 * @param error_operation Raw SMBIOS error-operation value.
 * @return Static string describing the operation.
 */
const char* lazybiosType33ErrorOperationStr(uint8_t error_operation);

/**
 * @brief Releases an array of parsed SMBIOS Type 33 structures.
 * @param Type33 Type 33 array to release.
 * @param type33_count Number of elements in Type33.
 */
void lazybiosFreeType33(lazybiosType33_t* Type33, size_t type33_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

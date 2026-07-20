/**
 * @file type18.h
 * @brief Public API for SMBIOS Type 18 32-Bit Memory Error Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE18_H
#define LAZYBIOS_TYPE18_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 18 fields. */
typedef struct {
	lazybiosFieldStatus_t error_type;
	lazybiosFieldStatus_t error_granularity;
	lazybiosFieldStatus_t error_operation;
	lazybiosFieldStatus_t vendor_syndrome;
	lazybiosFieldStatus_t memory_array_error_address;
	lazybiosFieldStatus_t device_error_address;
	lazybiosFieldStatus_t error_resolution;
} lazybiosType18FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 18 32-Bit Memory Error Information.
 * @ingroup api_type18
 */
typedef struct {
	uint8_t error_type;
	uint8_t error_granularity;
	uint8_t error_operation;
	uint32_t vendor_syndrome;
	uint32_t memory_array_error_address;
	uint32_t device_error_address;
	uint32_t error_resolution;
	lazybiosType18FieldStatus_t field_status;
} lazybiosType18_t;

/** @addtogroup api_type18
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 18 32-Bit Memory Error Information structures.
 * @param Type18 Existing Type 18 array pointer value; it is not dereferenced or released.
 * @param type18_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 18 array, or NULL on failure.
 */
lazybiosType18_t* lazybiosGetType18(lazybiosType18_t* Type18, size_t* type18_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an SMBIOS memory-error type.
 * @param error_type Raw SMBIOS memory-error type value.
 * @return Static string describing the error type.
 */
const char* lazybiosType18ErrorTypeStr(uint8_t error_type);

/**
 * @brief Decodes SMBIOS memory-error granularity.
 * @param error_granularity Raw SMBIOS error-granularity value.
 * @return Static string describing the granularity.
 */
const char* lazybiosType18ErrorGranularityStr(uint8_t error_granularity);

/**
 * @brief Decodes the operation that caused an SMBIOS memory error.
 * @param error_operation Raw SMBIOS error-operation value.
 * @return Static string describing the operation.
 */
const char* lazybiosType18ErrorOperationStr(uint8_t error_operation);

/**
 * @brief Releases an array of parsed SMBIOS Type 18 structures.
 * @param Type18 Type 18 array to release.
 * @param type18_count Number of elements in Type18.
 */
void lazybiosFreeType18(lazybiosType18_t* Type18, size_t type18_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

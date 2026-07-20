/**
 * @file type5.h
 * @brief Public API for obsolete SMBIOS Type 5 Memory Controller Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE5_H
#define LAZYBIOS_TYPE5_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 5 fields. */
typedef struct {
	lazybiosFieldStatus_t error_detecting_method;
	lazybiosFieldStatus_t error_correcting_capability;
	lazybiosFieldStatus_t supported_interleave;
	lazybiosFieldStatus_t current_interleave;
	lazybiosFieldStatus_t maximum_memory_module_size;
	lazybiosFieldStatus_t supported_speeds;
	lazybiosFieldStatus_t supported_memory_types;
	lazybiosFieldStatus_t memory_module_voltage;
	lazybiosFieldStatus_t number_of_associated_memory_slots;
	lazybiosFieldStatus_t memory_module_configuration_handles;
	lazybiosFieldStatus_t enabled_error_correcting_capabilities;
} lazybiosType5FieldStatus_t;

/**
 * @brief Parsed obsolete SMBIOS Type 5 Memory Controller Information.
 * @ingroup api_type5
 */
typedef struct {
	uint8_t error_detecting_method;
	uint8_t error_correcting_capability;
	uint8_t supported_interleave;
	uint8_t current_interleave;
	uint8_t maximum_memory_module_size;
	uint16_t supported_speeds;
	uint16_t supported_memory_types;
	uint8_t memory_module_voltage;
	uint8_t number_of_associated_memory_slots;
	uint16_t* memory_module_configuration_handles;
	uint8_t enabled_error_correcting_capabilities;
	lazybiosType5FieldStatus_t field_status;
} lazybiosType5_t;

/** @addtogroup api_type5
 * @{
 */

/**
 * @brief Parses all obsolete SMBIOS Type 5 Memory Controller Information structures.
 * @param Type5 Existing Type 5 array pointer value; it is not dereferenced or released.
 * @param type5_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 5 array, or NULL on failure.
 */
lazybiosType5_t* lazybiosGetType5(lazybiosType5_t* Type5, size_t* type5_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes a Type 5 memory-error detecting method.
 * @param error_detecting_method Raw error-detecting method value.
 * @return Static string describing the method.
 */
const char* lazybiosType5ErrorDetectingMethodStr(uint8_t error_detecting_method);

/**
 * @brief Decodes a Type 5 error-correcting capability bit field.
 * @param capability Raw supported or enabled error-correcting capability byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType5ErrorCorrectingCapabilityStr(uint8_t capability, char* buf, size_t buf_len);

/**
 * @brief Decodes a Type 5 memory-interleave value.
 * @param interleave Raw supported or current interleave value.
 * @return Static string describing the interleave configuration.
 */
const char* lazybiosType5InterleaveStr(uint8_t interleave);

/**
 * @brief Decodes the Type 5 supported-memory-speed bit field.
 * @param supported_speeds Raw supported-memory-speed bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType5SupportedSpeedsStr(uint16_t supported_speeds, char* buf, size_t buf_len);

/**
 * @brief Decodes the Type 5 supported-memory-type bit field.
 * @param supported_memory_types Raw supported-memory-type bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType5SupportedMemoryTypesStr(uint16_t supported_memory_types, char* buf, size_t buf_len);

/**
 * @brief Decodes the Type 5 memory-module voltage bit field.
 * @param memory_module_voltage Raw memory-module voltage bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType5MemoryModuleVoltageStr(uint8_t memory_module_voltage, char* buf, size_t buf_len);

/**
 * @brief Releases an array of parsed SMBIOS Type 5 structures.
 * @param Type5 Type 5 array to release.
 * @param type5_count Number of elements in Type5.
 */
void lazybiosFreeType5(lazybiosType5_t* Type5, size_t type5_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

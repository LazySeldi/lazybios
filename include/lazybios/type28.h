/**
 * @file type28.h
 * @brief Public API for SMBIOS Type 28 Temperature Probe.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE28_H
#define LAZYBIOS_TYPE28_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 28 fields. */
typedef struct {
	lazybiosFieldStatus_t description;
	lazybiosFieldStatus_t location_and_status;
	lazybiosFieldStatus_t maximum_value;
	lazybiosFieldStatus_t minimum_value;
	lazybiosFieldStatus_t resolution;
	lazybiosFieldStatus_t tolerance;
	lazybiosFieldStatus_t accuracy;
	lazybiosFieldStatus_t oem_defined;
	lazybiosFieldStatus_t nominal_value;
} lazybiosType28FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 28 Temperature Probe information.
 * @ingroup api_type28
 */
typedef struct {
	char* description;
	uint8_t location_and_status;
	uint16_t maximum_value;
	uint16_t minimum_value;
	uint16_t resolution;
	uint16_t tolerance;
	uint16_t accuracy;
	uint32_t oem_defined;
	uint16_t nominal_value;
	lazybiosType28FieldStatus_t field_status;
} lazybiosType28_t;

/** @addtogroup api_type28
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 28 Temperature Probe structures.
 * @param Type28 Existing Type 28 array pointer value; it is not dereferenced or released.
 * @param type28_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 28 array, or NULL on failure.
 */
lazybiosType28_t* lazybiosGetType28(lazybiosType28_t* Type28, size_t* type28_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes the physical location from a Type 28 location-and-status field.
 * @param location_and_status Raw Type 28 location-and-status byte.
 * @return Static string describing the temperature-probe location.
 */
const char* lazybiosType28LocationStr(uint8_t location_and_status);

/**
 * @brief Decodes the monitored-temperature status from a Type 28 location-and-status field.
 * @param location_and_status Raw Type 28 location-and-status byte.
 * @return Static string describing the monitored-temperature status.
 */
const char* lazybiosType28StatusStr(uint8_t location_and_status);

/**
 * @brief Releases an array of parsed SMBIOS Type 28 structures.
 * @param Type28 Type 28 array to release.
 * @param type28_count Number of elements in Type28.
 */
void lazybiosFreeType28(lazybiosType28_t* Type28, size_t type28_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

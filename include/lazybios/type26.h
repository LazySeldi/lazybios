/**
 * @file type26.h
 * @brief Public API for SMBIOS Type 26 Voltage Probe.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE26_H
#define LAZYBIOS_TYPE26_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 26 fields. */
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
} lazybiosType26FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 26 Voltage Probe information.
 * @ingroup api_type26
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
	lazybiosType26FieldStatus_t field_status;
} lazybiosType26_t;

/** @addtogroup api_type26
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 26 Voltage Probe structures.
 * @param Type26 Existing Type 26 array pointer value; it is not dereferenced or released.
 * @param type26_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 26 array, or NULL on failure.
 */
lazybiosType26_t* lazybiosGetType26(lazybiosType26_t* Type26, size_t* type26_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes the physical location from a Type 26 location-and-status field.
 * @param location_and_status Raw Type 26 location-and-status byte.
 * @return Static string describing the voltage-probe location.
 */
const char* lazybiosType26LocationStr(uint8_t location_and_status);

/**
 * @brief Decodes the monitored-voltage status from a Type 26 location-and-status field.
 * @param location_and_status Raw Type 26 location-and-status byte.
 * @return Static string describing the monitored-voltage status.
 */
const char* lazybiosType26StatusStr(uint8_t location_and_status);

/**
 * @brief Releases an array of parsed SMBIOS Type 26 structures.
 * @param Type26 Type 26 array to release.
 * @param type26_count Number of elements in Type26.
 */
void lazybiosFreeType26(lazybiosType26_t* Type26, size_t type26_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

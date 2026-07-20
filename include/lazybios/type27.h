/**
 * @file type27.h
 * @brief Public API for SMBIOS Type 27 Cooling Device.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE27_H
#define LAZYBIOS_TYPE27_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 27 fields. */
typedef struct {
	lazybiosFieldStatus_t temperature_probe_handle;
	lazybiosFieldStatus_t device_type_and_status;
	lazybiosFieldStatus_t cooling_unit_group;
	lazybiosFieldStatus_t oem_defined;
	lazybiosFieldStatus_t nominal_speed;
	lazybiosFieldStatus_t description;
} lazybiosType27FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 27 Cooling Device information.
 * @ingroup api_type27
 */
typedef struct {
	uint16_t temperature_probe_handle;
	uint8_t device_type_and_status;
	uint8_t cooling_unit_group;
	uint32_t oem_defined;
	uint16_t nominal_speed;
	char* description;
	lazybiosType27FieldStatus_t field_status;
} lazybiosType27_t;

/** @addtogroup api_type27
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 27 Cooling Device structures.
 * @param Type27 Existing Type 27 array pointer value; it is not dereferenced or released.
 * @param type27_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 27 array, or NULL on failure.
 */
lazybiosType27_t* lazybiosGetType27(lazybiosType27_t* Type27, size_t* type27_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes the cooling-device type from a Type 27 device-type-and-status field.
 * @param device_type_and_status Raw Type 27 device-type-and-status byte.
 * @return Static string describing the cooling-device type.
 */
const char* lazybiosType27DeviceTypeStr(uint8_t device_type_and_status);

/**
 * @brief Decodes the cooling-device status from a Type 27 device-type-and-status field.
 * @param device_type_and_status Raw Type 27 device-type-and-status byte.
 * @return Static string describing the cooling-device status.
 */
const char* lazybiosType27StatusStr(uint8_t device_type_and_status);

/**
 * @brief Releases an array of parsed SMBIOS Type 27 structures.
 * @param Type27 Type 27 array to release.
 * @param type27_count Number of elements in Type27.
 */
void lazybiosFreeType27(lazybiosType27_t* Type27, size_t type27_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

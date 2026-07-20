/**
 * @file type10.h
 * @brief Public API for obsolete SMBIOS Type 10 On Board Devices Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE10_H
#define LAZYBIOS_TYPE10_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for an obsolete SMBIOS Type 10 device entry. */
typedef struct {
	lazybiosFieldStatus_t device_type_and_status;
	lazybiosFieldStatus_t description;
} lazybiosType10DeviceFieldStatus_t;

/**
 * @brief Parsed device entry from an obsolete SMBIOS Type 10 structure.
 * @ingroup api_type10
 */
typedef struct {
	uint8_t device_type_and_status;
	char* description;
	lazybiosType10DeviceFieldStatus_t field_status;
} lazybiosType10Device_t;

/** @brief Availability metadata for SMBIOS Type 10 fields. */
typedef struct {
	lazybiosFieldStatus_t device_count;
	lazybiosFieldStatus_t devices;
} lazybiosType10FieldStatus_t;

/**
 * @brief Parsed obsolete SMBIOS Type 10 On Board Devices Information.
 * @ingroup api_type10
 */
typedef struct {
	size_t device_count;
	lazybiosType10Device_t* devices;
	lazybiosType10FieldStatus_t field_status;
} lazybiosType10_t;

/** @addtogroup api_type10
 * @{
 */

/**
 * @brief Parses all obsolete SMBIOS Type 10 On Board Devices Information structures.
 * @param Type10 Existing Type 10 array pointer value; it is not dereferenced or released.
 * @param type10_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 10 array, or NULL on failure.
 */
lazybiosType10_t* lazybiosGetType10(lazybiosType10_t* Type10, size_t* type10_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an obsolete Type 10 onboard-device type.
 * @param device_type_and_status Raw combined device-type and status byte.
 * @return Static string describing the onboard-device type.
 */
const char* lazybiosType10DeviceTypeStr(uint8_t device_type_and_status);

/**
 * @brief Decodes an obsolete Type 10 onboard-device status.
 * @param device_type_and_status Raw combined device-type and status byte.
 * @return Static string describing whether the device is enabled.
 */
const char* lazybiosType10DeviceStatusStr(uint8_t device_type_and_status);

/**
 * @brief Releases an array of parsed SMBIOS Type 10 structures.
 * @param Type10 Type 10 array to release.
 * @param type10_count Number of elements in Type10.
 */
void lazybiosFreeType10(lazybiosType10_t* Type10, size_t type10_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

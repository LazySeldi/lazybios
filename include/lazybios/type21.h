/**
 * @file type21.h
 * @brief Public API for SMBIOS Type 21 Built-in Pointing Device.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE21_H
#define LAZYBIOS_TYPE21_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 21 fields. */
typedef struct {
	lazybiosFieldStatus_t pointing_device_type;
	lazybiosFieldStatus_t interface;
	lazybiosFieldStatus_t number_of_buttons;
} lazybiosType21FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 21 Built-in Pointing Device.
 * @ingroup api_type21
 */
typedef struct {
	uint8_t pointing_device_type;
	uint8_t interface;
	uint8_t number_of_buttons;
	lazybiosType21FieldStatus_t field_status;
} lazybiosType21_t;

/** @addtogroup api_type21
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 21 Built-in Pointing Device structures.
 * @param Type21 Existing Type 21 array pointer value; it is not dereferenced or released.
 * @param type21_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 21 array, or NULL on failure.
 */
lazybiosType21_t* lazybiosGetType21(lazybiosType21_t* Type21, size_t* type21_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an SMBIOS built-in pointing-device type.
 * @param pointing_device_type Raw pointing-device type value.
 * @return Static string describing the pointing-device type.
 */
const char* lazybiosType21PointingDeviceTypeStr(uint8_t pointing_device_type);

/**
 * @brief Decodes an SMBIOS built-in pointing-device interface.
 * @param interface Raw pointing-device interface value.
 * @return Static string describing the interface.
 */
const char* lazybiosType21InterfaceStr(uint8_t interface);

/**
 * @brief Releases an array of parsed SMBIOS Type 21 structures.
 * @param Type21 Type 21 array to release.
 * @param type21_count Number of elements in Type21.
 */
void lazybiosFreeType21(lazybiosType21_t* Type21, size_t type21_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

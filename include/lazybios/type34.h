/**
 * @file type34.h
 * @brief Public API for SMBIOS Type 34 Management Device.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE34_H
#define LAZYBIOS_TYPE34_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 34 fields. */
typedef struct {
	lazybiosFieldStatus_t description;
	lazybiosFieldStatus_t device_type;
	lazybiosFieldStatus_t address;
	lazybiosFieldStatus_t address_type;
} lazybiosType34FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 34 Management Device information.
 * @ingroup api_type34
 */
typedef struct {
	char* description;
	uint8_t device_type;
	uint32_t address;
	uint8_t address_type;
	lazybiosType34FieldStatus_t field_status;
} lazybiosType34_t;

/** @addtogroup api_type34
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 34 Management Device structures.
 * @param Type34 Existing Type 34 array pointer value; it is not dereferenced or released.
 * @param type34_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 34 array, or NULL on failure.
 */
lazybiosType34_t* lazybiosGetType34(lazybiosType34_t* Type34, size_t* type34_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an SMBIOS Type 34 management-device type.
 * @param device_type Raw management-device type value.
 * @return Static string describing the management-device type.
 */
const char* lazybiosType34DeviceTypeStr(uint8_t device_type);

/**
 * @brief Decodes an SMBIOS Type 34 management-device address type.
 * @param address_type Raw management-device address-type value.
 * @return Static string describing the address type.
 */
const char* lazybiosType34AddressTypeStr(uint8_t address_type);

/**
 * @brief Releases an array of parsed SMBIOS Type 34 structures.
 * @param Type34 Type 34 array to release.
 * @param type34_count Number of elements in Type34.
 */
void lazybiosFreeType34(lazybiosType34_t* Type34, size_t type34_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

/**
 * @file type24.h
 * @brief Public API for SMBIOS Type 24 Hardware Security.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE24_H
#define LAZYBIOS_TYPE24_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 24 fields. */
typedef struct {
	lazybiosFieldStatus_t hardware_security_settings;
} lazybiosType24FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 24 Hardware Security information.
 * @ingroup api_type24
 */
typedef struct {
	uint8_t hardware_security_settings;
	lazybiosType24FieldStatus_t field_status;
} lazybiosType24_t;

/** @addtogroup api_type24
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 24 Hardware Security structures.
 * @param Type24 Existing Type 24 array pointer value; it is not dereferenced or released.
 * @param type24_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 24 array, or NULL on failure.
 */
lazybiosType24_t* lazybiosGetType24(lazybiosType24_t* Type24, size_t* type24_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes the power-on password status from Type 24 settings.
 * @param hardware_security_settings Raw Type 24 hardware-security settings byte.
 * @return Static string describing the power-on password status.
 */
const char* lazybiosType24PowerOnPasswordStatusStr(uint8_t hardware_security_settings);

/**
 * @brief Decodes the keyboard password status from Type 24 settings.
 * @param hardware_security_settings Raw Type 24 hardware-security settings byte.
 * @return Static string describing the keyboard password status.
 */
const char* lazybiosType24KeyboardPasswordStatusStr(uint8_t hardware_security_settings);

/**
 * @brief Decodes the administrator password status from Type 24 settings.
 * @param hardware_security_settings Raw Type 24 hardware-security settings byte.
 * @return Static string describing the administrator password status.
 */
const char* lazybiosType24AdministratorPasswordStatusStr(uint8_t hardware_security_settings);

/**
 * @brief Decodes the front-panel reset status from Type 24 settings.
 * @param hardware_security_settings Raw Type 24 hardware-security settings byte.
 * @return Static string describing the front-panel reset status.
 */
const char* lazybiosType24FrontPanelResetStatusStr(uint8_t hardware_security_settings);

/**
 * @brief Releases an array of parsed SMBIOS Type 24 structures.
 * @param Type24 Type 24 array to release.
 * @param type24_count Number of elements in Type24.
 */
void lazybiosFreeType24(lazybiosType24_t* Type24, size_t type24_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

/**
 * @file type46.h
 * @brief Public API for SMBIOS Type 46 String Property.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE46_H
#define LAZYBIOS_TYPE46_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 46 fields. */
typedef struct {
	lazybiosFieldStatus_t string_property_id;
	lazybiosFieldStatus_t string_property_value;
	lazybiosFieldStatus_t parent_handle;
} lazybiosType46FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 46 String Property.
 * @ingroup api_type46
 */
typedef struct {
	uint16_t string_property_id;
	char* string_property_value;
	uint16_t parent_handle;
	lazybiosType46FieldStatus_t field_status;
} lazybiosType46_t;

/** @addtogroup api_type46
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 46 String Property structures.
 * @param Type46 Existing Type 46 array pointer value; it is not dereferenced or released.
 * @param type46_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 46 array, or NULL on failure.
 */
lazybiosType46_t* lazybiosGetType46(lazybiosType46_t* Type46, size_t* type46_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an SMBIOS Type 46 string-property identifier.
 * @param string_property_id Raw string-property identifier.
 * @return Static string describing the property or its allocation range.
 */
const char* lazybiosType46StringPropertyIDStr(uint16_t string_property_id);

/**
 * @brief Releases an array of parsed SMBIOS Type 46 structures.
 * @param Type46 Type 46 array to release.
 * @param type46_count Number of elements in Type46.
 */
void lazybiosFreeType46(lazybiosType46_t* Type46, size_t type46_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

/**
 * @file type35.h
 * @brief Public API for SMBIOS Type 35 Management Device Component.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE35_H
#define LAZYBIOS_TYPE35_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 35 fields. */
typedef struct {
	lazybiosFieldStatus_t description;
	lazybiosFieldStatus_t management_device_handle;
	lazybiosFieldStatus_t component_handle;
	lazybiosFieldStatus_t threshold_handle;
} lazybiosType35FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 35 Management Device Component information.
 * @ingroup api_type35
 */
typedef struct {
	char* description;
	uint16_t management_device_handle;
	uint16_t component_handle;
	uint16_t threshold_handle;
	lazybiosType35FieldStatus_t field_status;
} lazybiosType35_t;

/** @addtogroup api_type35
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 35 Management Device Component structures.
 * @param Type35 Existing Type 35 array pointer value; it is not dereferenced or released.
 * @param type35_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 35 array, or NULL on failure.
 */
lazybiosType35_t* lazybiosGetType35(lazybiosType35_t* Type35, size_t* type35_count, lazybiosDMI_t* DMIData);

/**
 * @brief Releases an array of parsed SMBIOS Type 35 structures.
 * @param Type35 Type 35 array to release.
 * @param type35_count Number of elements in Type35.
 */
void lazybiosFreeType35(lazybiosType35_t* Type35, size_t type35_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

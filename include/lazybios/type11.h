/**
 * @file type11.h
 * @brief Public API for SMBIOS Type 11 OEM Strings.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE11_H
#define LAZYBIOS_TYPE11_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 11 fields. */
typedef struct {
	lazybiosFieldStatus_t string_count;
	lazybiosFieldStatus_t strings;
} lazybiosType11FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 11 OEM Strings Information.
 * @ingroup api_type11
 */
typedef struct {
	uint8_t string_count;
	char** strings;
	lazybiosType11FieldStatus_t field_status;
} lazybiosType11_t;

/** @addtogroup api_type11
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 11 OEM Strings structures.
 * @param Type11 Existing Type 11 array pointer value; it is not dereferenced or released.
 * @param type11_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 11 array, or NULL on failure.
 */
lazybiosType11_t* lazybiosGetType11(lazybiosType11_t* Type11, size_t* type11_count, lazybiosDMI_t* DMIData);

/**
 * @brief Releases an array of parsed SMBIOS Type 11 structures.
 * @param Type11 Type 11 array to release.
 * @param type11_count Number of elements in Type11.
 */
void lazybiosFreeType11(lazybiosType11_t* Type11, size_t type11_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

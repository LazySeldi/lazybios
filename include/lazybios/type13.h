/**
 * @file type13.h
 * @brief Public API for SMBIOS Type 13 Firmware Language Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE13_H
#define LAZYBIOS_TYPE13_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 13 fields. */
typedef struct {
	lazybiosFieldStatus_t installable_languages;
	lazybiosFieldStatus_t flags;
	lazybiosFieldStatus_t languages;
	lazybiosFieldStatus_t current_language;
} lazybiosType13FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 13 Firmware Language Information.
 * @ingroup api_type13
 */
typedef struct {
	uint8_t installable_languages;
	uint8_t flags;
	char** languages;
	char* current_language;
	lazybiosType13FieldStatus_t field_status;
} lazybiosType13_t;

/** @addtogroup api_type13
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 13 Firmware Language Information structures.
 * @param Type13 Existing Type 13 array pointer value; it is not dereferenced or released.
 * @param type13_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 13 array, or NULL on failure.
 */
lazybiosType13_t* lazybiosGetType13(lazybiosType13_t* Type13, size_t* type13_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes the language-description format selected by the Type 13 flags.
 * @param flags Raw SMBIOS Type 13 flags byte.
 * @return Static string describing the language-description format.
 */
const char* lazybiosType13LanguageFormatStr(uint8_t flags);

/**
 * @brief Releases an array of parsed SMBIOS Type 13 structures.
 * @param Type13 Type 13 array to release.
 * @param type13_count Number of elements in Type13.
 */
void lazybiosFreeType13(lazybiosType13_t* Type13, size_t type13_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

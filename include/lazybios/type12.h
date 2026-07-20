/**
 * @file type12.h
 * @brief Public API for SMBIOS Type 12 System Configuration Options.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE12_H
#define LAZYBIOS_TYPE12_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 12 fields. */
typedef struct {
	lazybiosFieldStatus_t option_count;
	lazybiosFieldStatus_t options;
} lazybiosType12FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 12 System Configuration Options Information.
 * @ingroup api_type12
 */
typedef struct {
	uint8_t option_count;
	char** options;
	lazybiosType12FieldStatus_t field_status;
} lazybiosType12_t;

/** @addtogroup api_type12
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 12 System Configuration Options structures.
 * @param Type12 Existing Type 12 array pointer value; it is not dereferenced or released.
 * @param type12_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 12 array, or NULL on failure.
 */
lazybiosType12_t* lazybiosGetType12(lazybiosType12_t* Type12, size_t* type12_count, lazybiosDMI_t* DMIData);

/**
 * @brief Releases an array of parsed SMBIOS Type 12 structures.
 * @param Type12 Type 12 array to release.
 * @param type12_count Number of elements in Type12.
 */
void lazybiosFreeType12(lazybiosType12_t* Type12, size_t type12_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

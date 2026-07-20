/**
 * @file type14.h
 * @brief Public API for SMBIOS Type 14 Group Associations.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE14_H
#define LAZYBIOS_TYPE14_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for an SMBIOS Type 14 group item. */
typedef struct {
	lazybiosFieldStatus_t item_type;
	lazybiosFieldStatus_t item_handle;
} lazybiosType14ItemFieldStatus_t;

/**
 * @brief Parsed member of an SMBIOS Type 14 group association.
 * @ingroup api_type14
 */
typedef struct {
	uint8_t item_type;
	uint16_t item_handle;
	lazybiosType14ItemFieldStatus_t field_status;
} lazybiosType14Item_t;

/** @brief Availability metadata for SMBIOS Type 14 fields. */
typedef struct {
	lazybiosFieldStatus_t group_name;
	lazybiosFieldStatus_t item_count;
	lazybiosFieldStatus_t items;
} lazybiosType14FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 14 Group Associations Information.
 * @ingroup api_type14
 */
typedef struct {
	char* group_name;
	size_t item_count;
	lazybiosType14Item_t* items;
	lazybiosType14FieldStatus_t field_status;
} lazybiosType14_t;

/** @addtogroup api_type14
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 14 Group Associations structures.
 * @param Type14 Existing Type 14 array pointer value; it is not dereferenced or released.
 * @param type14_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 14 array, or NULL on failure.
 */
lazybiosType14_t* lazybiosGetType14(lazybiosType14_t* Type14, size_t* type14_count, lazybiosDMI_t* DMIData);

/**
 * @brief Releases an array of parsed SMBIOS Type 14 structures.
 * @param Type14 Type 14 array to release.
 * @param type14_count Number of elements in Type14.
 */
void lazybiosFreeType14(lazybiosType14_t* Type14, size_t type14_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

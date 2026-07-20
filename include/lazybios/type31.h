/**
 * @file type31.h
 * @brief Public API for SMBIOS Type 31 Boot Integrity Services Entry Point.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE31_H
#define LAZYBIOS_TYPE31_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 31 fields. */
typedef struct {
	lazybiosFieldStatus_t checksum;
	lazybiosFieldStatus_t reserved_1;
	lazybiosFieldStatus_t reserved_2;
	lazybiosFieldStatus_t bis_entry_point_16;
	lazybiosFieldStatus_t bis_entry_point_32;
	lazybiosFieldStatus_t reserved_3;
	lazybiosFieldStatus_t reserved_4;
	lazybiosFieldStatus_t checksum_valid;
} lazybiosType31FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 31 Boot Integrity Services Entry Point information.
 * @ingroup api_type31
 */
typedef struct {
	uint8_t checksum;
	uint8_t reserved_1;
	uint16_t reserved_2;
	uint32_t bis_entry_point_16;
	uint32_t bis_entry_point_32;
	uint64_t reserved_3;
	uint32_t reserved_4;
	uint8_t checksum_valid;
	lazybiosType31FieldStatus_t field_status;
} lazybiosType31_t;

/** @addtogroup api_type31
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 31 Boot Integrity Services Entry Point structures.
 * @param Type31 Existing Type 31 array pointer value; it is not dereferenced or released.
 * @param type31_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 31 array, or NULL on failure.
 */
lazybiosType31_t* lazybiosGetType31(lazybiosType31_t* Type31, size_t* type31_count, lazybiosDMI_t* DMIData);

/**
 * @brief Releases an array of parsed SMBIOS Type 31 structures.
 * @param Type31 Type 31 array to release.
 * @param type31_count Number of elements in Type31.
 */
void lazybiosFreeType31(lazybiosType31_t* Type31, size_t type31_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

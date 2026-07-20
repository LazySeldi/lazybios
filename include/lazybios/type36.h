/**
 * @file type36.h
 * @brief Public API for SMBIOS Type 36 Management Device Threshold Data.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE36_H
#define LAZYBIOS_TYPE36_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 36 fields. */
typedef struct {
	lazybiosFieldStatus_t lower_threshold_non_critical;
	lazybiosFieldStatus_t upper_threshold_non_critical;
	lazybiosFieldStatus_t lower_threshold_critical;
	lazybiosFieldStatus_t upper_threshold_critical;
	lazybiosFieldStatus_t lower_threshold_non_recoverable;
	lazybiosFieldStatus_t upper_threshold_non_recoverable;
} lazybiosType36FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 36 Management Device Threshold Data.
 * @ingroup api_type36
 */
typedef struct {
	uint16_t lower_threshold_non_critical;
	uint16_t upper_threshold_non_critical;
	uint16_t lower_threshold_critical;
	uint16_t upper_threshold_critical;
	uint16_t lower_threshold_non_recoverable;
	uint16_t upper_threshold_non_recoverable;
	lazybiosType36FieldStatus_t field_status;
} lazybiosType36_t;

/** @addtogroup api_type36
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 36 Management Device Threshold Data structures.
 * @param Type36 Existing Type 36 array pointer value; it is not dereferenced or released.
 * @param type36_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 36 array, or NULL on failure.
 */
lazybiosType36_t* lazybiosGetType36(lazybiosType36_t* Type36, size_t* type36_count, lazybiosDMI_t* DMIData);

/**
 * @brief Releases an array of parsed SMBIOS Type 36 structures.
 * @param Type36 Type 36 array to release.
 * @param type36_count Number of elements in Type36.
 */
void lazybiosFreeType36(lazybiosType36_t* Type36, size_t type36_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

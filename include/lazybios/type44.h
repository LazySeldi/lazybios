/**
 * @file type44.h
 * @brief Public API for SMBIOS Type 44 Processor Additional Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE44_H
#define LAZYBIOS_TYPE44_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 44 fields. */
typedef struct {
	lazybiosFieldStatus_t referenced_handle;
	lazybiosFieldStatus_t block_length;
	lazybiosFieldStatus_t processor_type;
	lazybiosFieldStatus_t processor_specific_data;
} lazybiosType44FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 44 Processor Additional Information.
 * @ingroup api_type44
 */
typedef struct {
	uint16_t referenced_handle;
	uint8_t block_length;
	uint8_t processor_type;
	uint8_t* processor_specific_data;
	lazybiosType44FieldStatus_t field_status;
} lazybiosType44_t;

/** @addtogroup api_type44
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 44 Processor Additional Information structures.
 * @param Type44 Existing Type 44 array pointer value; it is not dereferenced or released.
 * @param type44_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 44 array, or NULL on failure.
 */
lazybiosType44_t* lazybiosGetType44(lazybiosType44_t* Type44, size_t* type44_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an SMBIOS Type 44 processor architecture type.
 * @param processor_type Raw processor architecture type value.
 * @return Static string describing the processor architecture.
 */
const char* lazybiosType44ProcessorTypeStr(uint8_t processor_type);

/**
 * @brief Releases an array of parsed SMBIOS Type 44 structures.
 * @param Type44 Type 44 array to release.
 * @param type44_count Number of elements in Type44.
 */
void lazybiosFreeType44(lazybiosType44_t* Type44, size_t type44_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

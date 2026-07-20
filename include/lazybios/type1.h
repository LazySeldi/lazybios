/**
 * @file type1.h
 * @brief Public API for SMBIOS Type 1 System Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE1_H
#define LAZYBIOS_TYPE1_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 1 fields. */
typedef struct {
	lazybiosFieldStatus_t manufacturer;
	lazybiosFieldStatus_t product_name;
	lazybiosFieldStatus_t version;
	lazybiosFieldStatus_t serial_number;
	lazybiosFieldStatus_t uuid;
	lazybiosFieldStatus_t wake_up_type;
	lazybiosFieldStatus_t sku_number;
	lazybiosFieldStatus_t family;
} lazybiosType1FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 1 System Information.
 * @ingroup api_type1
 */
typedef struct {
	char* manufacturer;
	char* product_name;
	char* version;
	char* serial_number;
	uint8_t uuid[16];
	uint8_t wake_up_type;
	char* sku_number;
	char* family;
	lazybiosType1FieldStatus_t field_status;
} lazybiosType1_t;

/** @addtogroup api_type1
 * @{
 */

/**
 * @brief Parses the first SMBIOS Type 1 System Information structure.
 * @param Type1 Existing Type 1 pointer value; it is not dereferenced or released.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 1 structure, or NULL on failure or absence.
 */
lazybiosType1_t* lazybiosGetType1(lazybiosType1_t* Type1, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an SMBIOS system wake-up type.
 * @param wake_up_type Raw SMBIOS wake-up type value.
 * @return Static string describing the wake-up type.
 */
const char* lazybiosType1WakeupTypeStr(uint8_t wake_up_type);

/**
 * @brief Releases a parsed SMBIOS Type 1 structure.
 * @param Type1 Type 1 structure to release.
 */
void lazybiosFreeType1(lazybiosType1_t* Type1);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

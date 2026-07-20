/**
 * @file type2.h
 * @brief Public API for SMBIOS Type 2 Baseboard Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE2_H
#define LAZYBIOS_TYPE2_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 2 fields. */
typedef struct {
	lazybiosFieldStatus_t manufacturer;
	lazybiosFieldStatus_t product;
	lazybiosFieldStatus_t version;
	lazybiosFieldStatus_t serial_number;
	lazybiosFieldStatus_t asset_tag;
	lazybiosFieldStatus_t feature_flags;
	lazybiosFieldStatus_t location_in_chassis;
	lazybiosFieldStatus_t chassis_handle;
	lazybiosFieldStatus_t board_type;
	lazybiosFieldStatus_t number_of_contained_object_handles;
	lazybiosFieldStatus_t contained_object_handles;
} lazybiosType2FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 2 Baseboard Information.
 * @ingroup api_type2
 */
typedef struct {
	char* manufacturer;
	char* product;
	char* version;
	char* serial_number;
	char* asset_tag;
	uint8_t feature_flags;
	char* location_in_chassis;
	uint16_t chassis_handle;
	uint8_t board_type;
	uint8_t number_of_contained_object_handles;
	uint16_t* contained_object_handles;
	lazybiosType2FieldStatus_t field_status;
} lazybiosType2_t;

/** @addtogroup api_type2
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 2 Baseboard Information structures.
 * @param Type2 Existing Type 2 array pointer value; it is not dereferenced or released.
 * @param type2_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 2 array, or NULL on failure.
 */
lazybiosType2_t* lazybiosGetType2(lazybiosType2_t* Type2, size_t* type2_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes SMBIOS baseboard feature flags into a readable string.
 * @param feature_flags Raw SMBIOS baseboard feature flags.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType2FeatureflagsStr(uint8_t feature_flags, char* buf, size_t buf_len);

/**
 * @brief Decodes an SMBIOS baseboard type.
 * @param board_type Raw SMBIOS baseboard type value.
 * @return Static string describing the baseboard type.
 */
const char* lazybiosType2BoardTypeStr(uint8_t board_type);

/**
 * @brief Releases an array of parsed SMBIOS Type 2 structures.
 * @param Type2 Type 2 array to release.
 * @param type2_count Number of elements in Type2.
 */
void lazybiosFreeType2(lazybiosType2_t* Type2, size_t type2_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

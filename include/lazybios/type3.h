/**
 * @file type3.h
 * @brief Public API for SMBIOS Type 3 Chassis Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE3_H
#define LAZYBIOS_TYPE3_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 3 fields. */
typedef struct {
	lazybiosFieldStatus_t manufacturer;
	lazybiosFieldStatus_t type;
	lazybiosFieldStatus_t version;
	lazybiosFieldStatus_t serial_number;
	lazybiosFieldStatus_t asset_tag;
	lazybiosFieldStatus_t boot_up_state;
	lazybiosFieldStatus_t power_supply_state;
	lazybiosFieldStatus_t thermal_state;
	lazybiosFieldStatus_t security_status;
	lazybiosFieldStatus_t oem_defined;
	lazybiosFieldStatus_t height;
	lazybiosFieldStatus_t number_of_power_cords;
	lazybiosFieldStatus_t contained_element_count;
	lazybiosFieldStatus_t contained_element_record_length;
	lazybiosFieldStatus_t contained_elements;
	lazybiosFieldStatus_t sku_number;
	lazybiosFieldStatus_t rack_type;
	lazybiosFieldStatus_t rack_height;
} lazybiosType3FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 3 System Enclosure or Chassis Information.
 * @ingroup api_type3
 */
typedef struct {
	char* manufacturer;
	uint8_t type;
	char* version;
	char* serial_number;
	char* asset_tag;
	uint8_t boot_up_state;
	uint8_t power_supply_state;
	uint8_t thermal_state;
	uint8_t security_status;
	uint32_t oem_defined;
	uint8_t height;
	uint8_t number_of_power_cords;
	uint8_t contained_element_count;
	uint8_t contained_element_record_length;
	uint8_t* contained_elements;
	char* sku_number;
	uint8_t rack_type;
	uint8_t rack_height;
	lazybiosType3FieldStatus_t field_status;
} lazybiosType3_t;

/** @addtogroup api_type3
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 3 Chassis Information structures.
 * @param Type3 Existing Type 3 array pointer value; it is not dereferenced or released.
 * @param type3_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 3 array, or NULL on failure.
 */
lazybiosType3_t* lazybiosGetType3(lazybiosType3_t* Type3, size_t* type3_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an SMBIOS chassis type and lock indicator.
 * @param type Raw SMBIOS chassis type byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType3TypeStr(uint8_t type, char* buf, size_t buf_len);

/**
 * @brief Decodes an SMBIOS chassis state value.
 * @param state Raw boot-up, power-supply, or thermal state value.
 * @return Static string describing the chassis state.
 */
const char* lazybiosType3StateStr(uint8_t state);

/**
 * @brief Decodes an SMBIOS chassis security status.
 * @param security_status Raw SMBIOS security status value.
 * @return Static string describing the security status.
 */
const char* lazybiosType3SecurityStatusStr(uint8_t security_status);

/**
 * @brief Decodes the type field of a chassis contained-element record.
 * @param contained_elements Raw contained-element type byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType3ContainedElementTypeStr(uint8_t contained_elements, char* buf, size_t buf_len);

/**
 * @brief Releases an array of parsed SMBIOS Type 3 structures.
 * @param Type3 Type 3 array to release.
 * @param type3_count Number of elements in Type3.
 */
void lazybiosFreeType3(lazybiosType3_t* Type3, size_t type3_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

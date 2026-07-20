/**
 * @file type39.h
 * @brief Public API for SMBIOS Type 39 System Power Supply.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE39_H
#define LAZYBIOS_TYPE39_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 39 fields. */
typedef struct {
	lazybiosFieldStatus_t power_unit_group;
	lazybiosFieldStatus_t location;
	lazybiosFieldStatus_t device_name;
	lazybiosFieldStatus_t manufacturer;
	lazybiosFieldStatus_t serial_number;
	lazybiosFieldStatus_t asset_tag_number;
	lazybiosFieldStatus_t model_part_number;
	lazybiosFieldStatus_t revision_level;
	lazybiosFieldStatus_t max_power_capacity;
	lazybiosFieldStatus_t power_supply_characteristics;
	lazybiosFieldStatus_t input_voltage_probe_handle;
	lazybiosFieldStatus_t cooling_device_handle;
	lazybiosFieldStatus_t input_current_probe_handle;
} lazybiosType39FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 39 System Power Supply information.
 * @ingroup api_type39
 */
typedef struct {
	uint8_t power_unit_group;
	char* location;
	char* device_name;
	char* manufacturer;
	char* serial_number;
	char* asset_tag_number;
	char* model_part_number;
	char* revision_level;
	uint16_t max_power_capacity;
	uint16_t power_supply_characteristics;
	uint16_t input_voltage_probe_handle;
	uint16_t cooling_device_handle;
	uint16_t input_current_probe_handle;
	lazybiosType39FieldStatus_t field_status;
} lazybiosType39_t;

/** @addtogroup api_type39
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 39 System Power Supply structures.
 * @param Type39 Existing Type 39 array pointer value; it is not dereferenced or released.
 * @param type39_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 39 array, or NULL on failure.
 */
lazybiosType39_t* lazybiosGetType39(lazybiosType39_t* Type39, size_t* type39_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes the DMTF power-supply type from Type 39 characteristics.
 * @param characteristics Raw Type 39 power-supply characteristics word.
 * @return Static string describing the power-supply type.
 */
const char* lazybiosType39PowerSupplyTypeStr(uint16_t characteristics);

/**
 * @brief Decodes the power-supply status from Type 39 characteristics.
 * @param characteristics Raw Type 39 power-supply characteristics word.
 * @return Static string describing the power-supply status.
 */
const char* lazybiosType39StatusStr(uint16_t characteristics);

/**
 * @brief Decodes input-voltage range switching from Type 39 characteristics.
 * @param characteristics Raw Type 39 power-supply characteristics word.
 * @return Static string describing the input-voltage range switching method.
 */
const char* lazybiosType39InputVoltageRangeSwitchingStr(uint16_t characteristics);

/**
 * @brief Formats the unplugged, present, and hot-replaceable characteristic flags.
 * @param characteristics Raw Type 39 power-supply characteristics word.
 * @param buf Output buffer that receives the decoded flags.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType39CharacteristicsFlagsStr(uint16_t characteristics, char* buf, size_t buf_len);

/**
 * @brief Releases an array of parsed SMBIOS Type 39 structures.
 * @param Type39 Type 39 array to release.
 * @param type39_count Number of elements in Type39.
 */
void lazybiosFreeType39(lazybiosType39_t* Type39, size_t type39_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

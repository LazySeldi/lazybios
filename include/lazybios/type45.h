/**
 * @file type45.h
 * @brief Public API for SMBIOS Type 45 Firmware Inventory Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE45_H
#define LAZYBIOS_TYPE45_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 45 fields. */
typedef struct {
	lazybiosFieldStatus_t firmware_component_name;
	lazybiosFieldStatus_t firmware_version;
	lazybiosFieldStatus_t version_format;
	lazybiosFieldStatus_t firmware_id;
	lazybiosFieldStatus_t firmware_id_format;
	lazybiosFieldStatus_t release_date;
	lazybiosFieldStatus_t manufacturer;
	lazybiosFieldStatus_t lowest_supported_firmware_version;
	lazybiosFieldStatus_t image_size;
	lazybiosFieldStatus_t characteristics;
	lazybiosFieldStatus_t state;
	lazybiosFieldStatus_t number_of_associated_components;
	lazybiosFieldStatus_t associated_component_handles;
} lazybiosType45FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 45 Firmware Inventory Information.
 * @ingroup api_type45
 */
typedef struct {
	char* firmware_component_name;
	char* firmware_version;
	uint8_t version_format;
	char* firmware_id;
	uint8_t firmware_id_format;
	char* release_date;
	char* manufacturer;
	char* lowest_supported_firmware_version;
	uint64_t image_size;
	uint16_t characteristics;
	uint8_t state;
	uint8_t number_of_associated_components;
	uint16_t* associated_component_handles;
	lazybiosType45FieldStatus_t field_status;
} lazybiosType45_t;

/** @addtogroup api_type45
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 45 Firmware Inventory Information structures.
 * @param Type45 Existing Type 45 array pointer value; it is not dereferenced or released.
 * @param type45_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 45 array, or NULL on failure.
 */
lazybiosType45_t* lazybiosGetType45(lazybiosType45_t* Type45, size_t* type45_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes the format of Type 45 firmware-version strings.
 * @param version_format Raw firmware-version format value.
 * @return Static string describing the version format.
 */
const char* lazybiosType45VersionFormatStr(uint8_t version_format);

/**
 * @brief Decodes the format of a Type 45 firmware ID string.
 * @param firmware_id_format Raw firmware-ID format value.
 * @return Static string describing the firmware-ID format.
 */
const char* lazybiosType45FirmwareIDFormatStr(uint8_t firmware_id_format);

/**
 * @brief Formats the Type 45 updatable and write-protected characteristic flags.
 * @param characteristics Raw Type 45 characteristics word.
 * @param buf Output buffer that receives the decoded flags.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType45CharacteristicsStr(uint16_t characteristics, char* buf, size_t buf_len);

/**
 * @brief Decodes the operational state of a Type 45 firmware component.
 * @param state Raw firmware inventory state value.
 * @return Static string describing the firmware state.
 */
const char* lazybiosType45StateStr(uint8_t state);

/**
 * @brief Releases an array of parsed SMBIOS Type 45 structures.
 * @param Type45 Type 45 array to release.
 * @param type45_count Number of elements in Type45.
 */
void lazybiosFreeType45(lazybiosType45_t* Type45, size_t type45_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

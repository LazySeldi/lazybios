/**
 * @file type4.h
 * @brief Public API for SMBIOS Type 4 Processor Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE4_H
#define LAZYBIOS_TYPE4_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 4 fields. */
typedef struct {
	lazybiosFieldStatus_t socket_designation;
	lazybiosFieldStatus_t processor_type;
	lazybiosFieldStatus_t processor_family;
	lazybiosFieldStatus_t processor_manufacturer;
	lazybiosFieldStatus_t processor_id;
	lazybiosFieldStatus_t processor_version;
	lazybiosFieldStatus_t voltage;
	lazybiosFieldStatus_t external_clock;
	lazybiosFieldStatus_t max_speed;
	lazybiosFieldStatus_t current_speed;
	lazybiosFieldStatus_t status;
	lazybiosFieldStatus_t processor_upgrade;
	lazybiosFieldStatus_t l1_cache_handle;
	lazybiosFieldStatus_t l2_cache_handle;
	lazybiosFieldStatus_t l3_cache_handle;
	lazybiosFieldStatus_t serial_number;
	lazybiosFieldStatus_t asset_tag;
	lazybiosFieldStatus_t part_number;
	lazybiosFieldStatus_t core_count;
	lazybiosFieldStatus_t core_enabled;
	lazybiosFieldStatus_t thread_count;
	lazybiosFieldStatus_t processor_characteristics;
	lazybiosFieldStatus_t processor_family_2;
	lazybiosFieldStatus_t core_count_2;
	lazybiosFieldStatus_t core_enabled_2;
	lazybiosFieldStatus_t thread_count_2;
	lazybiosFieldStatus_t thread_enabled;
	lazybiosFieldStatus_t socket_type;
} lazybiosType4FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 4 Processor Information.
 * @ingroup api_type4
 */
typedef struct {
	char* socket_designation;
	uint8_t processor_type;
	uint8_t processor_family;
	char* processor_manufacturer;
	uint64_t processor_id;
	char* processor_version;
	uint8_t voltage;
	uint16_t external_clock;
	uint16_t max_speed;
	uint16_t current_speed;
	uint8_t status;
	uint8_t processor_upgrade;
	uint16_t l1_cache_handle;
	uint16_t l2_cache_handle;
	uint16_t l3_cache_handle;
	char* serial_number;
	char* asset_tag;
	char* part_number;
	uint8_t core_count;
	uint8_t core_enabled;
	uint8_t thread_count;
	uint16_t processor_characteristics;
	uint16_t processor_family_2;
	uint16_t core_count_2;
	uint16_t core_enabled_2;
	uint16_t thread_count_2;
	uint16_t thread_enabled;
	char* socket_type;
	lazybiosType4FieldStatus_t field_status;
} lazybiosType4_t;

/** @addtogroup api_type4
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 4 Processor Information structures.
 * @param Type4 Existing Type 4 array pointer value; it is not dereferenced or released.
 * @param type4_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 4 array, or NULL on failure.
 */
lazybiosType4_t* lazybiosGetType4(lazybiosType4_t* Type4, size_t* type4_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an SMBIOS processor family value.
 * @param family Raw 8-bit or extended 16-bit processor family value.
 * @return Static string describing the processor family.
 */
const char* lazybiosType4ProcessorFamilyStr(uint16_t family);

/**
 * @brief Decodes an SMBIOS processor upgrade or socket type.
 * @param type Raw SMBIOS processor upgrade value.
 * @return Static string describing the socket type.
 */
const char* lazybiosType4SocketTypeStr(uint8_t type);

/**
 * @brief Decodes SMBIOS processor characteristics into a readable string.
 * @param characteristics Raw SMBIOS processor characteristics bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType4CharacteristicsStr(uint16_t characteristics, char* buf, size_t buf_len);

/**
 * @brief Decodes an SMBIOS processor type.
 * @param type Raw SMBIOS processor type value.
 * @return Static string describing the processor type.
 */
const char* lazybiosType4TypeStr(uint8_t type);

/**
 * @brief Decodes an SMBIOS processor status byte.
 * @param status Raw SMBIOS processor status value.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType4StatusStr(uint8_t status, char* buf, size_t buf_len);

/**
 * @brief Decodes an SMBIOS processor voltage byte.
 * @param voltage Raw SMBIOS processor voltage value.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType4VoltageStr(uint8_t voltage, char* buf, size_t buf_len);

/**
 * @brief Releases an array of parsed SMBIOS Type 4 structures.
 * @param Type4 Type 4 array to release.
 * @param type4_count Number of elements in Type4.
 */
void lazybiosFreeType4(lazybiosType4_t* Type4, size_t type4_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

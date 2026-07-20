/**
 * @file type22.h
 * @brief Public API for SMBIOS Type 22 Portable Battery.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE22_H
#define LAZYBIOS_TYPE22_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 22 fields. */
typedef struct {
	lazybiosFieldStatus_t location;
	lazybiosFieldStatus_t manufacturer;
	lazybiosFieldStatus_t manufacture_date;
	lazybiosFieldStatus_t serial_number;
	lazybiosFieldStatus_t device_name;
	lazybiosFieldStatus_t device_chemistry;
	lazybiosFieldStatus_t design_capacity;
	lazybiosFieldStatus_t design_voltage;
	lazybiosFieldStatus_t sbds_version_number;
	lazybiosFieldStatus_t maximum_error;
	lazybiosFieldStatus_t sbds_serial_number;
	lazybiosFieldStatus_t sbds_manufacture_date;
	lazybiosFieldStatus_t sbds_device_chemistry;
	lazybiosFieldStatus_t design_capacity_multiplier;
	lazybiosFieldStatus_t oem_specific;
} lazybiosType22FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 22 Portable Battery.
 * @ingroup api_type22
 */
typedef struct {
	char* location;
	char* manufacturer;
	char* manufacture_date;
	char* serial_number;
	char* device_name;
	uint8_t device_chemistry;
	uint16_t design_capacity;
	uint16_t design_voltage;
	char* sbds_version_number;
	uint8_t maximum_error;
	uint16_t sbds_serial_number;
	uint16_t sbds_manufacture_date;
	char* sbds_device_chemistry;
	uint8_t design_capacity_multiplier;
	uint32_t oem_specific;
	lazybiosType22FieldStatus_t field_status;
} lazybiosType22_t;

/** @addtogroup api_type22
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 22 Portable Battery structures.
 * @param Type22 Existing Type 22 array pointer value; it is not dereferenced or released.
 * @param type22_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 22 array, or NULL on failure.
 */
lazybiosType22_t* lazybiosGetType22(lazybiosType22_t* Type22, size_t* type22_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an SMBIOS portable-battery chemistry value.
 * @param device_chemistry Raw portable-battery chemistry value.
 * @return Static string describing the chemistry.
 */
const char* lazybiosType22DeviceChemistryStr(uint8_t device_chemistry);

/**
 * @brief Calculates the effective portable-battery design capacity.
 * @param design_capacity Raw design capacity in mWh.
 * @param design_capacity_multiplier SMBIOS 2.2 multiplication factor, or 1 when unavailable.
 * @return Effective design capacity in mWh.
 */
uint32_t lazybiosType22DesignCapacityMWh(uint16_t design_capacity, uint8_t design_capacity_multiplier);

/**
 * @brief Formats an SMBIOS Smart Battery Data Specification manufacture date.
 * @param sbds_manufacture_date Raw packed SBDS manufacture date.
 * @param buf Output buffer that receives the formatted date.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType22SBDSManufactureDateStr(uint16_t sbds_manufacture_date, char* buf, size_t buf_len);

/**
 * @brief Releases an array of parsed SMBIOS Type 22 structures.
 * @param Type22 Type 22 array to release.
 * @param type22_count Number of elements in Type22.
 */
void lazybiosFreeType22(lazybiosType22_t* Type22, size_t type22_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

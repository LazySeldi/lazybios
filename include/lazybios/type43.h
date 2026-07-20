/**
 * @file type43.h
 * @brief Public API for SMBIOS Type 43 TPM Device.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE43_H
#define LAZYBIOS_TYPE43_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 43 fields. */
typedef struct {
	lazybiosFieldStatus_t vendor_id;
	lazybiosFieldStatus_t major_spec_version;
	lazybiosFieldStatus_t minor_spec_version;
	lazybiosFieldStatus_t firmware_version_1;
	lazybiosFieldStatus_t firmware_version_2;
	lazybiosFieldStatus_t description;
	lazybiosFieldStatus_t characteristics;
	lazybiosFieldStatus_t oem_defined;
} lazybiosType43FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 43 TPM Device information.
 * @ingroup api_type43
 */
typedef struct {
	char vendor_id[5];
	uint8_t major_spec_version;
	uint8_t minor_spec_version;
	uint32_t firmware_version_1;
	uint32_t firmware_version_2;
	char* description;
	uint64_t characteristics;
	uint32_t oem_defined;
	lazybiosType43FieldStatus_t field_status;
} lazybiosType43_t;

/** @addtogroup api_type43
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 43 TPM Device structures.
 * @param Type43 Existing Type 43 array pointer value; it is not dereferenced or released.
 * @param type43_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 43 array, or NULL on failure.
 */
lazybiosType43_t* lazybiosGetType43(lazybiosType43_t* Type43, size_t* type43_count, lazybiosDMI_t* DMIData);

/**
 * @brief Formats a TPM firmware revision according to its major specification version.
 * @param major_spec_version TPM major specification version.
 * @param firmware_version_1 Raw first TPM firmware-version field.
 * @param firmware_version_2 Raw second TPM firmware-version field.
 * @param buf Output buffer that receives the formatted revision.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType43FirmwareVersionStr(uint8_t major_spec_version, uint32_t firmware_version_1,
	uint32_t firmware_version_2, char* buf, size_t buf_len);

/**
 * @brief Decodes SMBIOS Type 43 TPM device characteristics.
 * @param characteristics Raw Type 43 characteristics bit field.
 * @param buf Output buffer that receives the decoded characteristics.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType43CharacteristicsStr(uint64_t characteristics, char* buf, size_t buf_len);

/**
 * @brief Releases an array of parsed SMBIOS Type 43 structures.
 * @param Type43 Type 43 array to release.
 * @param type43_count Number of elements in Type43.
 */
void lazybiosFreeType43(lazybiosType43_t* Type43, size_t type43_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

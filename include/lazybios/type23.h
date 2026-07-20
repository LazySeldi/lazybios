/**
 * @file type23.h
 * @brief Public API for SMBIOS Type 23 System Reset.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE23_H
#define LAZYBIOS_TYPE23_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 23 fields. */
typedef struct {
	lazybiosFieldStatus_t capabilities;
	lazybiosFieldStatus_t reset_count;
	lazybiosFieldStatus_t reset_limit;
	lazybiosFieldStatus_t timer_interval;
	lazybiosFieldStatus_t timeout;
} lazybiosType23FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 23 System Reset information.
 * @ingroup api_type23
 */
typedef struct {
	uint8_t capabilities;
	uint16_t reset_count;
	uint16_t reset_limit;
	uint16_t timer_interval;
	uint16_t timeout;
	lazybiosType23FieldStatus_t field_status;
} lazybiosType23_t;

/** @addtogroup api_type23
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 23 System Reset structures.
 * @param Type23 Existing Type 23 array pointer value; it is not dereferenced or released.
 * @param type23_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 23 array, or NULL on failure.
 */
lazybiosType23_t* lazybiosGetType23(lazybiosType23_t* Type23, size_t* type23_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes the watchdog boot option from Type 23 capabilities.
 * @param capabilities Raw Type 23 capabilities bit field.
 * @return Static string describing the boot option.
 */
const char* lazybiosType23BootOptionStr(uint8_t capabilities);

/**
 * @brief Decodes the reset-limit boot option from Type 23 capabilities.
 * @param capabilities Raw Type 23 capabilities bit field.
 * @return Static string describing the boot option used at the reset limit.
 */
const char* lazybiosType23BootOptionOnLimitStr(uint8_t capabilities);

/**
 * @brief Decodes the SMBIOS Type 23 capabilities bit field.
 * @param capabilities Raw Type 23 capabilities byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType23CapabilitiesStr(uint8_t capabilities, char* buf, size_t buf_len);

/**
 * @brief Releases an array of parsed SMBIOS Type 23 structures.
 * @param Type23 Type 23 array to release.
 * @param type23_count Number of elements in Type23.
 */
void lazybiosFreeType23(lazybiosType23_t* Type23, size_t type23_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

/**
 * @file type7.h
 * @brief Public API for SMBIOS Type 7 Cache Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE7_H
#define LAZYBIOS_TYPE7_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 7 fields. */
typedef struct {
	lazybiosFieldStatus_t socket_designation;
	lazybiosFieldStatus_t cache_configuration;
	lazybiosFieldStatus_t maximum_cache_size;
	lazybiosFieldStatus_t installed_size;
	lazybiosFieldStatus_t supported_sram_type;
	lazybiosFieldStatus_t current_sram_type;
	lazybiosFieldStatus_t cache_speed;
	lazybiosFieldStatus_t error_correction_type;
	lazybiosFieldStatus_t system_cache_type;
	lazybiosFieldStatus_t associativity;
	lazybiosFieldStatus_t maximum_cache_size_2;
	lazybiosFieldStatus_t installed_cache_size_2;
} lazybiosType7FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 7 Cache Information.
 * @ingroup api_type7
 */
typedef struct {
	char* socket_designation;
	uint16_t cache_configuration;
	uint16_t maximum_cache_size;
	uint16_t installed_size;
	uint16_t supported_sram_type;
	uint16_t current_sram_type;
	uint8_t cache_speed;
	uint8_t error_correction_type;
	uint8_t system_cache_type;
	uint8_t associativity;
	uint32_t maximum_cache_size_2;
	uint32_t installed_cache_size_2;
	lazybiosType7FieldStatus_t field_status;
} lazybiosType7_t;

/** @addtogroup api_type7
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 7 Cache Information structures.
 * @param Type7 Existing Type 7 array pointer value; it is not dereferenced or released.
 * @param type7_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 7 array, or NULL on failure.
 */
lazybiosType7_t* lazybiosGetType7(lazybiosType7_t* Type7, size_t* type7_count, lazybiosDMI_t* DMIData);

/** @brief Converts a 16-bit SMBIOS cache size field to kibibytes. */
uint64_t lazybiosType7CacheU16(uint16_t raw_size);
/** @brief Decodes SMBIOS SRAM type flags. */
void lazybiosType7SRAMTypeStr(uint16_t sram_type, char* buf, size_t buf_len);
/** @brief Decodes a cache error-correction type. */
const char* lazybiosType7ErrorCorrectionTypeStr(uint8_t ecc_type);
/** @brief Decodes a system cache type. */
const char* lazybiosType7SystemCacheTypeStr(uint8_t cache_type);
/** @brief Decodes cache associativity. */
const char* lazybiosType7AssociativityStr(uint8_t associativity);
/** @brief Decodes cache configuration. */
void lazybiosType7CacheConfigurationStr(uint16_t config, char* buf, size_t buf_len);
/** @brief Converts a 32-bit SMBIOS cache size field to kibibytes. */
uint64_t lazybiosType7CacheU32(uint32_t raw_size);
/** @brief Releases parsed Type 7 structures. */
void lazybiosFreeType7(lazybiosType7_t* Type7, size_t type7_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

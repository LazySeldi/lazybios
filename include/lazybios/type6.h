/**
 * @file type6.h
 * @brief Public API for obsolete SMBIOS Type 6 Memory Module Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE6_H
#define LAZYBIOS_TYPE6_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 6 fields. */
typedef struct {
	lazybiosFieldStatus_t socket_designation;
	lazybiosFieldStatus_t bank_connections;
	lazybiosFieldStatus_t current_speed;
	lazybiosFieldStatus_t current_memory_type;
	lazybiosFieldStatus_t installed_size;
	lazybiosFieldStatus_t enabled_size;
	lazybiosFieldStatus_t error_status;
} lazybiosType6FieldStatus_t;

/**
 * @brief Parsed obsolete SMBIOS Type 6 Memory Module Information.
 * @ingroup api_type6
 */
typedef struct {
	char* socket_designation;
	uint8_t bank_connections;
	uint8_t current_speed;
	uint16_t current_memory_type;
	uint8_t installed_size;
	uint8_t enabled_size;
	uint8_t error_status;
	lazybiosType6FieldStatus_t field_status;
} lazybiosType6_t;

/** @addtogroup api_type6
 * @{
 */

/**
 * @brief Parses all obsolete SMBIOS Type 6 Memory Module Information structures.
 * @param Type6 Existing Type 6 array pointer value; it is not dereferenced or released.
 * @param type6_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 6 array, or NULL on failure.
 */
lazybiosType6_t* lazybiosGetType6(lazybiosType6_t* Type6, size_t* type6_count, lazybiosDMI_t* DMIData);

/**
 * @brief Formats the bank connections encoded by a Type 6 structure.
 * @param bank_connections Raw bank-connections byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType6BankConnectionsStr(uint8_t bank_connections, char* buf, size_t buf_len);

/**
 * @brief Decodes the Type 6 current-memory-type bit field.
 * @param current_memory_type Raw current-memory-type bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType6CurrentMemoryTypeStr(uint16_t current_memory_type, char* buf, size_t buf_len);

/**
 * @brief Formats an obsolete Type 6 installed-size value.
 * @param installed_size Raw installed-size byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType6InstalledSizeStr(uint8_t installed_size, char* buf, size_t buf_len);

/**
 * @brief Formats an obsolete Type 6 enabled-size value.
 * @param enabled_size Raw enabled-size byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType6EnabledSizeStr(uint8_t enabled_size, char* buf, size_t buf_len);

/**
 * @brief Decodes the Type 6 memory-module error-status bit field.
 * @param error_status Raw error-status byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType6ErrorStatusStr(uint8_t error_status, char* buf, size_t buf_len);

/**
 * @brief Releases an array of parsed SMBIOS Type 6 structures.
 * @param Type6 Type 6 array to release.
 * @param type6_count Number of elements in Type6.
 */
void lazybiosFreeType6(lazybiosType6_t* Type6, size_t type6_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

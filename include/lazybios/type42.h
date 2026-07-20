/**
 * @file type42.h
 * @brief Public API for SMBIOS Type 42 Management Controller Host Interface.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE42_H
#define LAZYBIOS_TYPE42_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for an SMBIOS Type 42 protocol record. */
typedef struct {
	lazybiosFieldStatus_t protocol_type;
	lazybiosFieldStatus_t protocol_type_specific_data_length;
	lazybiosFieldStatus_t protocol_type_specific_data;
} lazybiosType42ProtocolRecordFieldStatus_t;

/**
 * @brief Parsed protocol record from an SMBIOS Type 42 structure.
 * @ingroup api_type42
 */
typedef struct {
	uint8_t protocol_type;
	uint8_t protocol_type_specific_data_length;
	uint8_t* protocol_type_specific_data;
	lazybiosType42ProtocolRecordFieldStatus_t field_status;
} lazybiosType42ProtocolRecord_t;

/** @brief Availability metadata for SMBIOS Type 42 fields. */
typedef struct {
	lazybiosFieldStatus_t interface_type;
	lazybiosFieldStatus_t interface_type_specific_data_length;
	lazybiosFieldStatus_t interface_type_specific_data;
	lazybiosFieldStatus_t number_of_protocol_records;
	lazybiosFieldStatus_t protocol_records;
} lazybiosType42FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 42 Management Controller Host Interface.
 * @ingroup api_type42
 */
typedef struct {
	uint8_t interface_type;
	uint8_t interface_type_specific_data_length;
	size_t interface_type_specific_data_size;
	uint8_t* interface_type_specific_data;
	uint8_t number_of_protocol_records;
	lazybiosType42ProtocolRecord_t* protocol_records;
	lazybiosType42FieldStatus_t field_status;
} lazybiosType42_t;

/** @addtogroup api_type42
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 42 Management Controller Host Interface structures.
 * @param Type42 Existing Type 42 array pointer value; it is not dereferenced or released.
 * @param type42_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 42 array, or NULL on failure.
 */
lazybiosType42_t* lazybiosGetType42(lazybiosType42_t* Type42, size_t* type42_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an SMBIOS Type 42 management-controller host-interface type.
 * @param interface_type Raw management-controller host-interface type value.
 * @return Static string describing the interface type.
 */
const char* lazybiosType42InterfaceTypeStr(uint8_t interface_type);

/**
 * @brief Decodes an SMBIOS Type 42 management-controller protocol type.
 * @param protocol_type Raw management-controller protocol type value.
 * @return Static string describing the protocol type.
 */
const char* lazybiosType42ProtocolTypeStr(uint8_t protocol_type);

/**
 * @brief Releases an array of parsed SMBIOS Type 42 structures.
 * @param Type42 Type 42 array to release.
 * @param type42_count Number of elements in Type42.
 */
void lazybiosFreeType42(lazybiosType42_t* Type42, size_t type42_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

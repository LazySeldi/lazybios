/**
 * @file type15.h
 * @brief Public API for SMBIOS Type 15 System Event Log Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE15_H
#define LAZYBIOS_TYPE15_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for an SMBIOS Type 15 event-log descriptor. */
typedef struct {
	lazybiosFieldStatus_t log_type;
	lazybiosFieldStatus_t variable_data_format_type;
} lazybiosType15LogTypeDescriptorFieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 15 supported event-log type descriptor.
 * @ingroup api_type15
 */
typedef struct {
	uint8_t log_type;
	uint8_t variable_data_format_type;
	lazybiosType15LogTypeDescriptorFieldStatus_t field_status;
} lazybiosType15LogTypeDescriptor_t;

/** @brief Availability metadata for SMBIOS Type 15 fields. */
typedef struct {
	lazybiosFieldStatus_t log_area_length;
	lazybiosFieldStatus_t log_header_start_offset;
	lazybiosFieldStatus_t log_data_start_offset;
	lazybiosFieldStatus_t access_method;
	lazybiosFieldStatus_t log_status;
	lazybiosFieldStatus_t log_change_token;
	lazybiosFieldStatus_t access_method_address;
	lazybiosFieldStatus_t log_header_format;
	lazybiosFieldStatus_t number_of_supported_log_type_descriptors;
	lazybiosFieldStatus_t length_of_each_log_type_descriptor;
	lazybiosFieldStatus_t supported_log_type_descriptors;
} lazybiosType15FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 15 System Event Log Information.
 * @ingroup api_type15
 */
typedef struct {
	uint16_t log_area_length;
	uint16_t log_header_start_offset;
	uint16_t log_data_start_offset;
	uint8_t access_method;
	uint8_t log_status;
	uint32_t log_change_token;
	uint32_t access_method_address;
	uint8_t log_header_format;
	uint8_t number_of_supported_log_type_descriptors;
	uint8_t length_of_each_log_type_descriptor;
	lazybiosType15LogTypeDescriptor_t* supported_log_type_descriptors;
	lazybiosType15FieldStatus_t field_status;
} lazybiosType15_t;

/** @addtogroup api_type15
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 15 System Event Log structures.
 * @param Type15 Existing Type 15 array pointer value; it is not dereferenced or released.
 * @param type15_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 15 array, or NULL on failure.
 */
lazybiosType15_t* lazybiosGetType15(lazybiosType15_t* Type15, size_t* type15_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an SMBIOS Type 15 event-log access method.
 * @param access_method Raw event-log access method value.
 * @return Static string describing the access method.
 */
const char* lazybiosType15AccessMethodStr(uint8_t access_method);

/**
 * @brief Decodes the SMBIOS Type 15 event-log status bit field.
 * @param log_status Raw event-log status byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType15LogStatusStr(uint8_t log_status, char* buf, size_t buf_len);

/**
 * @brief Decodes an SMBIOS Type 15 event-log header format.
 * @param log_header_format Raw event-log header format value.
 * @return Static string describing the header format.
 */
const char* lazybiosType15LogHeaderFormatStr(uint8_t log_header_format);

/**
 * @brief Decodes an SMBIOS event-log type.
 * @param log_type Raw event-log type value.
 * @return Static string describing the event type.
 */
const char* lazybiosType15LogTypeStr(uint8_t log_type);

/**
 * @brief Decodes an SMBIOS event-log variable-data format type.
 * @param format_type Raw variable-data format type value.
 * @return Static string describing the variable-data format.
 */
const char* lazybiosType15VariableDataFormatTypeStr(uint8_t format_type);

/**
 * @brief Extracts the indexed-I/O index address from an access-method address.
 * @param access_method_address Raw SMBIOS Type 15 access-method address.
 * @return 16-bit indexed-I/O index address.
 */
uint16_t lazybiosType15IndexAddress(uint32_t access_method_address);

/**
 * @brief Extracts the indexed-I/O data address from an access-method address.
 * @param access_method_address Raw SMBIOS Type 15 access-method address.
 * @return 16-bit indexed-I/O data address.
 */
uint16_t lazybiosType15DataAddress(uint32_t access_method_address);

/**
 * @brief Extracts the GPNV handle from an access-method address.
 * @param access_method_address Raw SMBIOS Type 15 access-method address.
 * @return 16-bit GPNV handle.
 */
uint16_t lazybiosType15GPNVHandle(uint32_t access_method_address);

/**
 * @brief Releases an array of parsed SMBIOS Type 15 structures.
 * @param Type15 Type 15 array to release.
 * @param type15_count Number of elements in Type15.
 */
void lazybiosFreeType15(lazybiosType15_t* Type15, size_t type15_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

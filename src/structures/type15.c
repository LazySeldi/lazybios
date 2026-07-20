/**
 * @file type15.c
 * @brief Implements parsing and decoding for SMBIOS Type 15 System Event Log Information.
 * @author LazySeldi
 */

//
// Type 15 ( System Event Log )
//

#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define LOG_AREA_LENGTH 0x04
#define LOG_HEADER_START_OFFSET 0x06
#define LOG_DATA_START_OFFSET 0x08
#define ACCESS_METHOD 0x0A
#define LOG_STATUS 0x0B
#define LOG_CHANGE_TOKEN 0x0C
#define ACCESS_METHOD_ADDRESS 0x10
#define LOG_HEADER_FORMAT 0x14
#define NUMBER_OF_SUPPORTED_LOG_TYPE_DESCRIPTORS 0x15
#define LENGTH_OF_EACH_LOG_TYPE_DESCRIPTOR 0x16
#define SUPPORTED_LOG_TYPE_DESCRIPTORS 0x17

// Descriptor Fields
#define DESCRIPTOR_LOG_TYPE 0x00
#define DESCRIPTOR_VARIABLE_DATA_FORMAT_TYPE 0x01
#define MINIMUM_DESCRIPTOR_LENGTH 2

// Access Methods
#define ACCESS_METHOD_INDEXED_IO_8BIT 0x00
#define ACCESS_METHOD_INDEXED_IO_2X8BIT 0x01
#define ACCESS_METHOD_INDEXED_IO_16BIT 0x02
#define ACCESS_METHOD_MEMORY_MAPPED 0x03
#define ACCESS_METHOD_GPNV 0x04

// Log Status Masks
#define LOG_STATUS_VALID_MASK 0x01
#define LOG_STATUS_FULL_MASK 0x02

// Log Header Formats
#define LOG_HEADER_FORMAT_NONE 0x00
#define LOG_HEADER_FORMAT_TYPE_1 0x01

// Event Log Types
#define LOG_TYPE_RESERVED 0x00
#define LOG_TYPE_SINGLE_BIT_ECC_MEMORY_ERROR 0x01
#define LOG_TYPE_MULTI_BIT_ECC_MEMORY_ERROR 0x02
#define LOG_TYPE_PARITY_MEMORY_ERROR 0x03
#define LOG_TYPE_BUS_TIMEOUT 0x04
#define LOG_TYPE_IO_CHANNEL_CHECK 0x05
#define LOG_TYPE_SOFTWARE_NMI 0x06
#define LOG_TYPE_POST_MEMORY_RESIZE 0x07
#define LOG_TYPE_POST_ERROR 0x08
#define LOG_TYPE_PCI_PARITY_ERROR 0x09
#define LOG_TYPE_PCI_SYSTEM_ERROR 0x0A
#define LOG_TYPE_CPU_FAILURE 0x0B
#define LOG_TYPE_EISA_FAILSAFE_TIMER_TIMEOUT 0x0C
#define LOG_TYPE_CORRECTABLE_MEMORY_LOG_DISABLED 0x0D
#define LOG_TYPE_LOGGING_DISABLED 0x0E
#define LOG_TYPE_RESERVED_2 0x0F
#define LOG_TYPE_SYSTEM_LIMIT_EXCEEDED 0x10
#define LOG_TYPE_ASYNCHRONOUS_TIMER_EXPIRED 0x11
#define LOG_TYPE_SYSTEM_CONFIGURATION_INFORMATION 0x12
#define LOG_TYPE_HARD_DISK_INFORMATION 0x13
#define LOG_TYPE_SYSTEM_RECONFIGURED 0x14
#define LOG_TYPE_UNCORRECTABLE_CPU_COMPLEX_ERROR 0x15
#define LOG_TYPE_LOG_AREA_RESET 0x16
#define LOG_TYPE_SYSTEM_BOOT 0x17
#define LOG_TYPE_END_OF_LOG 0xFF

// Variable Data Format Types
#define VARIABLE_DATA_FORMAT_NONE 0x00
#define VARIABLE_DATA_FORMAT_HANDLE 0x01
#define VARIABLE_DATA_FORMAT_MULTIPLE_EVENT 0x02
#define VARIABLE_DATA_FORMAT_MULTIPLE_EVENT_HANDLE 0x03
#define VARIABLE_DATA_FORMAT_POST_RESULTS_BITMAP 0x04
#define VARIABLE_DATA_FORMAT_SYSTEM_MANAGEMENT_TYPE 0x05
#define VARIABLE_DATA_FORMAT_MULTIPLE_EVENT_SYSTEM_MANAGEMENT_TYPE 0x06
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 15 System Event Log structures.
 *
 * @param Type15 Existing Type 15 array pointer value; it is not dereferenced or released.
 * @param type15_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 15 array, or NULL on failure.
 */
lazybiosType15_t* lazybiosGetType15(lazybiosType15_t* Type15, size_t* type15_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_SYSTEM_EVENT_LOG);
	size_t index = 0;

	Type15 = calloc(count, sizeof(lazybiosType15_t));
	if (!Type15) return NULL;
	if (count == 0) {
		*type15_count = 0;
		return Type15;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_SYSTEM_EVENT_LOG) {
			if (index >= count) break;
			lazybiosType15_t* current = &Type15[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);

			READU16(current, log_area_length, len, LOG_AREA_LENGTH, p);
			READU16(current, log_header_start_offset, len, LOG_HEADER_START_OFFSET, p);
			READU16(current, log_data_start_offset, len, LOG_DATA_START_OFFSET, p);
			READU8(current, access_method, len, ACCESS_METHOD, p);
			READU8(current, log_status, len, LOG_STATUS, p);
			READU32(current, log_change_token, len, LOG_CHANGE_TOKEN, p);
			READU32(current, access_method_address, len, ACCESS_METHOD_ADDRESS, p);

			if (lazybiosIsVersionPlus(DMIData, 2, 1)) {
				READU8(current, log_header_format, len, LOG_HEADER_FORMAT, p);
				READU8(current, number_of_supported_log_type_descriptors, len,
					NUMBER_OF_SUPPORTED_LOG_TYPE_DESCRIPTORS, p);
				READU8(current, length_of_each_log_type_descriptor, len,
					LENGTH_OF_EACH_LOG_TYPE_DESCRIPTOR, p);

				if (LAZYBIOS_FIELD_STATUS(current, number_of_supported_log_type_descriptors) == LAZYBIOS_FIELD_PRESENT &&
					LAZYBIOS_FIELD_STATUS(current, length_of_each_log_type_descriptor) == LAZYBIOS_FIELD_PRESENT) {
					const size_t descriptor_bytes = (size_t)current->number_of_supported_log_type_descriptors *
						current->length_of_each_log_type_descriptor;
					const int descriptor_layout_valid = current->length_of_each_log_type_descriptor >= MINIMUM_DESCRIPTOR_LENGTH &&
						(size_t)len >= SUPPORTED_LOG_TYPE_DESCRIPTORS + descriptor_bytes;

					if (current->number_of_supported_log_type_descriptors == 0) {
						LAZYBIOS_MARK_PRESENT(current, supported_log_type_descriptors);
					} else if (descriptor_layout_valid) {
						current->supported_log_type_descriptors = calloc(
							current->number_of_supported_log_type_descriptors,
							sizeof(lazybiosType15LogTypeDescriptor_t));
						if (!current->supported_log_type_descriptors) {
							lazybiosFreeType15(Type15, count);
							return NULL;
						}

						for (size_t i = 0; i < current->number_of_supported_log_type_descriptors; i++) {
							const size_t descriptor_offset = SUPPORTED_LOG_TYPE_DESCRIPTORS +
								(i * current->length_of_each_log_type_descriptor);
							lazybiosType15LogTypeDescriptor_t* descriptor =
								&current->supported_log_type_descriptors[i];
							descriptor->log_type = p[descriptor_offset + DESCRIPTOR_LOG_TYPE];
							descriptor->variable_data_format_type =
								p[descriptor_offset + DESCRIPTOR_VARIABLE_DATA_FORMAT_TYPE];
							LAZYBIOS_MARK_PRESENT(descriptor, log_type);
							LAZYBIOS_MARK_PRESENT(descriptor, variable_data_format_type);
						}
						LAZYBIOS_MARK_PRESENT(current, supported_log_type_descriptors);
					} else {
						current->supported_log_type_descriptors = NULL;
						LAZYBIOS_MARK_ABSENT(current, supported_log_type_descriptors);
					}
				}
			} else {
				current->log_header_format = 0;
				current->number_of_supported_log_type_descriptors = 0;
				current->length_of_each_log_type_descriptor = 0;
				current->supported_log_type_descriptors = NULL;
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type15_count = index;
	return Type15;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes an SMBIOS Type 15 event-log access method.
 *
 * @param access_method Raw event-log access method value.
 * @return Static string describing the access method.
 */
const char* lazybiosType15AccessMethodStr(uint8_t access_method) {
	switch (access_method) {
		case ACCESS_METHOD_INDEXED_IO_8BIT:
			return "Indexed I/O: One 8-bit Index Port and One 8-bit Data Port";
		case ACCESS_METHOD_INDEXED_IO_2X8BIT:
			return "Indexed I/O: Two 8-bit Index Ports and One 8-bit Data Port";
		case ACCESS_METHOD_INDEXED_IO_16BIT:
			return "Indexed I/O: One 16-bit Index Port and One 8-bit Data Port";
		case ACCESS_METHOD_MEMORY_MAPPED:
			return "Memory-mapped Physical 32-bit Address";
		case ACCESS_METHOD_GPNV:
			return "General-Purpose Nonvolatile Data Functions";
		default:
			if (access_method <= 0x7F) return "Available for Future Assignment";
			return "Firmware Vendor/OEM-specific";
	}
}

/**
 * @brief Decodes the SMBIOS Type 15 event-log status bit field.
 *
 * @param log_status Raw event-log status byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType15LogStatusStr(uint8_t log_status, char* buf, size_t buf_len) {
	snprintf(buf, buf_len, "%s, %s",
		(log_status & LOG_STATUS_VALID_MASK) ? "Valid" : "Invalid",
		(log_status & LOG_STATUS_FULL_MASK) ? "Full" : "Not Full");
}

/**
 * @brief Decodes an SMBIOS Type 15 event-log header format.
 *
 * @param log_header_format Raw event-log header format value.
 * @return Static string describing the header format.
 */
const char* lazybiosType15LogHeaderFormatStr(uint8_t log_header_format) {
	switch (log_header_format) {
		case LOG_HEADER_FORMAT_NONE:
			return "No Header";
		case LOG_HEADER_FORMAT_TYPE_1:
			return "Type 1 Log Header";
		default:
			if (log_header_format <= 0x7F) return "Available for Future Assignment";
			return "Firmware Vendor/OEM-specific";
	}
}

/**
 * @brief Decodes an SMBIOS event-log type.
 *
 * @param log_type Raw event-log type value.
 * @return Static string describing the event type.
 */
const char* lazybiosType15LogTypeStr(uint8_t log_type) {
	switch (log_type) {
		case LOG_TYPE_RESERVED:
		case LOG_TYPE_RESERVED_2:
			return "Reserved";
		case LOG_TYPE_SINGLE_BIT_ECC_MEMORY_ERROR:
			return "Single-bit ECC Memory Error";
		case LOG_TYPE_MULTI_BIT_ECC_MEMORY_ERROR:
			return "Multi-bit ECC Memory Error";
		case LOG_TYPE_PARITY_MEMORY_ERROR:
			return "Parity Memory Error";
		case LOG_TYPE_BUS_TIMEOUT:
			return "Bus Timeout";
		case LOG_TYPE_IO_CHANNEL_CHECK:
			return "I/O Channel Check";
		case LOG_TYPE_SOFTWARE_NMI:
			return "Software NMI";
		case LOG_TYPE_POST_MEMORY_RESIZE:
			return "POST Memory Resize";
		case LOG_TYPE_POST_ERROR:
			return "POST Error";
		case LOG_TYPE_PCI_PARITY_ERROR:
			return "PCI Parity Error";
		case LOG_TYPE_PCI_SYSTEM_ERROR:
			return "PCI System Error";
		case LOG_TYPE_CPU_FAILURE:
			return "CPU Failure";
		case LOG_TYPE_EISA_FAILSAFE_TIMER_TIMEOUT:
			return "EISA Failsafe Timer Timeout";
		case LOG_TYPE_CORRECTABLE_MEMORY_LOG_DISABLED:
			return "Correctable Memory Log Disabled";
		case LOG_TYPE_LOGGING_DISABLED:
			return "Logging Disabled for a Specific Event Type";
		case LOG_TYPE_SYSTEM_LIMIT_EXCEEDED:
			return "System Limit Exceeded";
		case LOG_TYPE_ASYNCHRONOUS_TIMER_EXPIRED:
			return "Asynchronous Hardware Timer Expired";
		case LOG_TYPE_SYSTEM_CONFIGURATION_INFORMATION:
			return "System Configuration Information";
		case LOG_TYPE_HARD_DISK_INFORMATION:
			return "Hard-disk Information";
		case LOG_TYPE_SYSTEM_RECONFIGURED:
			return "System Reconfigured";
		case LOG_TYPE_UNCORRECTABLE_CPU_COMPLEX_ERROR:
			return "Uncorrectable CPU-complex Error";
		case LOG_TYPE_LOG_AREA_RESET:
			return "Log Area Reset/Cleared";
		case LOG_TYPE_SYSTEM_BOOT:
			return "System Boot";
		case LOG_TYPE_END_OF_LOG:
			return "End of Log";
		default:
			if (log_type <= 0x7F) return "Unused";
			return "System or OEM-specific";
	}
}

/**
 * @brief Decodes an SMBIOS event-log variable-data format type.
 *
 * @param format_type Raw variable-data format type value.
 * @return Static string describing the variable-data format.
 */
const char* lazybiosType15VariableDataFormatTypeStr(uint8_t format_type) {
	switch (format_type) {
		case VARIABLE_DATA_FORMAT_NONE:
			return "None";
		case VARIABLE_DATA_FORMAT_HANDLE:
			return "Handle";
		case VARIABLE_DATA_FORMAT_MULTIPLE_EVENT:
			return "Multiple-Event";
		case VARIABLE_DATA_FORMAT_MULTIPLE_EVENT_HANDLE:
			return "Multiple-Event Handle";
		case VARIABLE_DATA_FORMAT_POST_RESULTS_BITMAP:
			return "POST Results Bitmap";
		case VARIABLE_DATA_FORMAT_SYSTEM_MANAGEMENT_TYPE:
			return "System Management Type";
		case VARIABLE_DATA_FORMAT_MULTIPLE_EVENT_SYSTEM_MANAGEMENT_TYPE:
			return "Multiple-Event System Management Type";
		default:
			if (format_type <= 0x7F) return "Unused";
			return "OEM Assigned";
	}
}

/**
 * @brief Extracts the indexed-I/O index address from an access-method address.
 *
 * @param access_method_address Raw SMBIOS Type 15 access-method address.
 * @return 16-bit indexed-I/O index address.
 */
uint16_t lazybiosType15IndexAddress(uint32_t access_method_address) {
	return (uint16_t)(access_method_address & 0xFFFF);
}

/**
 * @brief Extracts the indexed-I/O data address from an access-method address.
 *
 * @param access_method_address Raw SMBIOS Type 15 access-method address.
 * @return 16-bit indexed-I/O data address.
 */
uint16_t lazybiosType15DataAddress(uint32_t access_method_address) {
	return (uint16_t)(access_method_address >> 16);
}

/**
 * @brief Extracts the GPNV handle from an access-method address.
 *
 * @param access_method_address Raw SMBIOS Type 15 access-method address.
 * @return 16-bit GPNV handle.
 */
uint16_t lazybiosType15GPNVHandle(uint32_t access_method_address) {
	return (uint16_t)(access_method_address & 0xFFFF);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 15 structures.
 *
 * @param Type15 Type 15 array to release.
 * @param type15_count Number of elements in Type15.
 */
void lazybiosFreeType15(lazybiosType15_t* Type15, size_t type15_count) {
	if (!Type15) return;

	for (size_t i = 0; i < type15_count; i++) {
		free(Type15[i].supported_log_type_descriptors);
	}

	free(Type15);
}

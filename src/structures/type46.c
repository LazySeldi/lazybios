/**
 * @file type46.c
 * @brief Implements parsing and decoding for SMBIOS Type 46 String Property.
 * @author LazySeldi
 */

//
// Type 46 ( String Property )
//

#include "lazybios_internal.h"
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define STRING_PROPERTY_ID 0x04
#define STRING_PROPERTY_VALUE 0x06
#define PARENT_HANDLE 0x07

// String Property IDs
#define STRING_PROPERTY_ID_RESERVED 0x0000
#define STRING_PROPERTY_ID_UEFI_DEVICE_PATH 0x0001
#define STRING_PROPERTY_ID_DMTF_MAX 0x7FFF
#define STRING_PROPERTY_ID_FIRMWARE_VENDOR_MAX 0xBFFF
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 46 String Property structures.
 *
 * @param Type46 Existing Type 46 array pointer value; it is not dereferenced or released.
 * @param type46_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 46 array, or NULL on failure.
 */
lazybiosType46_t* lazybiosGetType46(lazybiosType46_t* Type46, size_t* type46_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_STRING_PROPERTY);
	size_t index = 0;

	Type46 = calloc(count, sizeof(lazybiosType46_t));
	if (!Type46) return NULL;
	if (count == 0) {
		*type46_count = 0;
		return Type46;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_STRING_PROPERTY) {
			if (index >= count) break;
			lazybiosType46_t* current = &Type46[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READU16(current, string_property_id, len, STRING_PROPERTY_ID, p);
			READSTR(current, string_property_value, len, STRING_PROPERTY_VALUE, p, structure_end);
			READU16(current, parent_handle, len, PARENT_HANDLE, p);
			if (current->parent_handle == 0xFFFF) LAZYBIOS_MARK_ABSENT(current, parent_handle);

			index++;
		}
		p = DMINext(p, end);
	}
	*type46_count = index;
	return Type46;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes an SMBIOS Type 46 string-property identifier.
 *
 * @param string_property_id Raw string-property identifier.
 * @return Static string describing the property or its allocation range.
 */
const char* lazybiosType46StringPropertyIDStr(uint16_t string_property_id) {
	if (string_property_id == STRING_PROPERTY_ID_RESERVED) return "Reserved";
	if (string_property_id == STRING_PROPERTY_ID_UEFI_DEVICE_PATH) return "UEFI Device Path";
	if (string_property_id <= STRING_PROPERTY_ID_DMTF_MAX) return "Reserved for Future DMTF Use";
	if (string_property_id <= STRING_PROPERTY_ID_FIRMWARE_VENDOR_MAX) return "Firmware Vendor-defined";
	return "OEM-defined";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 46 structures.
 *
 * @param Type46 Type 46 array to release.
 * @param type46_count Number of elements in Type46.
 */
void lazybiosFreeType46(lazybiosType46_t* Type46, size_t type46_count) {
	if (!Type46) return;

	for (size_t i = 0; i < type46_count; i++) {
		free(Type46[i].string_property_value);
	}
	free(Type46);
}

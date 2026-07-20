/**
 * @file type35.c
 * @brief Implements parsing for SMBIOS Type 35 Management Device Component.
 * @author LazySeldi
 */

//
// Type 35 ( Management Device Component )
//

#include "lazybios_internal.h"
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define DESCRIPTION 0x04
#define MANAGEMENT_DEVICE_HANDLE 0x05
#define COMPONENT_HANDLE 0x07
#define THRESHOLD_HANDLE 0x09
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 35 Management Device Component structures.
 *
 * @param Type35 Existing Type 35 array pointer value; it is not dereferenced or released.
 * @param type35_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 35 array, or NULL on failure.
 */
lazybiosType35_t* lazybiosGetType35(lazybiosType35_t* Type35, size_t* type35_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_MANAGEMENT_DEVICE_COMPONENT);
	size_t index = 0;

	Type35 = calloc(count, sizeof(lazybiosType35_t));
	if (!Type35) return NULL;
	if (count == 0) {
		*type35_count = 0;
		return Type35;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_MANAGEMENT_DEVICE_COMPONENT) {
			if (index >= count) break;
			lazybiosType35_t* current = &Type35[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, description, len, DESCRIPTION, p, structure_end);
			READU16(current, management_device_handle, len, MANAGEMENT_DEVICE_HANDLE, p);
			READU16(current, component_handle, len, COMPONENT_HANDLE, p);
			READU16(current, threshold_handle, len, THRESHOLD_HANDLE, p);

			index++;
		}
		p = DMINext(p, end);
	}
	*type35_count = index;
	return Type35;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 35 structures.
 *
 * @param Type35 Type 35 array to release.
 * @param type35_count Number of elements in Type35.
 */
void lazybiosFreeType35(lazybiosType35_t* Type35, size_t type35_count) {
	if (!Type35) return;

	for (size_t i = 0; i < type35_count; i++) {
		free(Type35[i].description);
	}
	free(Type35);
}

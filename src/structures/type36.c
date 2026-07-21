/**
 * @file type36.c
 * @brief Implements parsing for SMBIOS Type 36 Management Device Threshold Data.
 * @author LazySeldi
 */

//
// Type 36 ( Management Device Threshold Data )
//

#include "lazybios_internal.h"
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define LOWER_THRESHOLD_NON_CRITICAL 0x04
#define UPPER_THRESHOLD_NON_CRITICAL 0x06
#define LOWER_THRESHOLD_CRITICAL 0x08
#define UPPER_THRESHOLD_CRITICAL 0x0A
#define LOWER_THRESHOLD_NON_RECOVERABLE 0x0C
#define UPPER_THRESHOLD_NON_RECOVERABLE 0x0E
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 36 Management Device Threshold Data structures.
 *
 * @param Type36 Existing Type 36 array pointer value; it is not dereferenced or released.
 * @param type36_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 36 array, or NULL on failure.
 */
lazybiosType36_t* lazybiosGetType36(lazybiosType36_t* Type36, size_t* type36_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_MANAGEMENT_DEVICE_THRESHOLD_DATA);
	size_t index = 0;

	Type36 = calloc(count, sizeof(lazybiosType36_t));
	if (!Type36) return NULL;
	if (count == 0) {
		*type36_count = 0;
		return Type36;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_MANAGEMENT_DEVICE_THRESHOLD_DATA) {
			if (index >= count) break;
			lazybiosType36_t* current = &Type36[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);

			READU16(current, lower_threshold_non_critical, len, LOWER_THRESHOLD_NON_CRITICAL, p);
			READU16(current, upper_threshold_non_critical, len, UPPER_THRESHOLD_NON_CRITICAL, p);
			READU16(current, lower_threshold_critical, len, LOWER_THRESHOLD_CRITICAL, p);
			READU16(current, upper_threshold_critical, len, UPPER_THRESHOLD_CRITICAL, p);
			READU16(current, lower_threshold_non_recoverable, len, LOWER_THRESHOLD_NON_RECOVERABLE, p);
			READU16(current, upper_threshold_non_recoverable, len, UPPER_THRESHOLD_NON_RECOVERABLE, p);

			if (current->lower_threshold_non_critical == 0x8000) LAZYBIOS_MARK_ABSENT(current, lower_threshold_non_critical);
			if (current->upper_threshold_non_critical == 0x8000) LAZYBIOS_MARK_ABSENT(current, upper_threshold_non_critical);
			if (current->lower_threshold_critical == 0x8000) LAZYBIOS_MARK_ABSENT(current, lower_threshold_critical);
			if (current->upper_threshold_critical == 0x8000) LAZYBIOS_MARK_ABSENT(current, upper_threshold_critical);
			if (current->lower_threshold_non_recoverable == 0x8000) LAZYBIOS_MARK_ABSENT(current, lower_threshold_non_recoverable);
			if (current->upper_threshold_non_recoverable == 0x8000) LAZYBIOS_MARK_ABSENT(current, upper_threshold_non_recoverable);

			index++;
		}
		p = DMINext(p, end);
	}
	*type36_count = index;
	return Type36;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 36 structures.
 *
 * @param Type36 Type 36 array to release.
 * @param type36_count Number of elements in Type36.
 */
void lazybiosFreeType36(lazybiosType36_t* Type36, size_t type36_count) {
	(void)type36_count;
	free(Type36);
}

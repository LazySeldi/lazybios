/**
 * @file type12.c
 * @brief Implements parsing for SMBIOS Type 12 System Configuration Options.
 * @author LazySeldi
 */

//
// Type 12 ( System Configuration Options )
//

#include "lazybios_internal.h"
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define COUNT 0x04
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 12 System Configuration Options structures.
 *
 * @param Type12 Existing Type 12 array pointer value; it is not dereferenced or released.
 * @param type12_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 12 array, or NULL on failure.
 */
lazybiosType12_t* lazybiosGetType12(lazybiosType12_t* Type12, size_t* type12_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_SYSTEM_CONFIGURATION_OPTIONS);
	size_t index = 0;

	Type12 = calloc(count, sizeof(lazybiosType12_t));
	if (!Type12) return NULL;
	if (count == 0) {
		*type12_count = 0;
		return Type12;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_SYSTEM_CONFIGURATION_OPTIONS) {
			if (index >= count) break;
			lazybiosType12_t* current = &Type12[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READU8(current, option_count, len, COUNT, p);

			if (LAZYBIOS_FIELD_STATUS(current, option_count) == LAZYBIOS_FIELD_PRESENT && current->option_count > 0) {
				current->options = calloc(current->option_count, sizeof(char*));
				if (current->options) {
					LAZYBIOS_MARK_PRESENT(current, options);
					for (size_t i = 0; i < current->option_count; i++) {
						current->options[i] = DMIString(p, len, (uint8_t)(i + 1), structure_end);
						if (!current->options[i] || current->options[i][0] == '\0') {
							free(current->options[i]);
							current->options[i] = NULL;
							LAZYBIOS_MARK_ABSENT(current, options);
						}
					}
				} else {
					lazybiosFreeType12(Type12, count);
					return NULL;
				}
			} else if (LAZYBIOS_FIELD_STATUS(current, option_count) == LAZYBIOS_FIELD_PRESENT) {
				LAZYBIOS_MARK_PRESENT(current, options);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type12_count = index;
	return Type12;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 12 structures.
 *
 * @param Type12 Type 12 array to release.
 * @param type12_count Number of elements in Type12.
 */
void lazybiosFreeType12(lazybiosType12_t* Type12, size_t type12_count) {
	if (!Type12) return;

	for (size_t i = 0; i < type12_count; i++) {
		if (Type12[i].options) {
			for (size_t j = 0; j < Type12[i].option_count; j++) {
				free(Type12[i].options[j]);
			}
		}
		free(Type12[i].options);
	}

	free(Type12);
}

/**
 * @file type13.c
 * @brief Implements parsing and decoding for SMBIOS Type 13 Firmware Language Information.
 * @author LazySeldi
 */

//
// Type 13 ( Firmware Language Information )
//

#include "lazybios_internal.h"
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define INSTALLABLE_LANGUAGES 0x04
#define FLAGS 0x05
#define CURRENT_LANGUAGE 0x15

// Flag Masks
#define LANGUAGE_FORMAT_MASK 0x01
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 13 Firmware Language Information structures.
 *
 * @param Type13 Existing Type 13 array pointer value; it is not dereferenced or released.
 * @param type13_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 13 array, or NULL on failure.
 */
lazybiosType13_t* lazybiosGetType13(lazybiosType13_t* Type13, size_t* type13_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_FIRMWARE_LANGUAGE_INFORMATION);
	size_t index = 0;

	Type13 = calloc(count, sizeof(lazybiosType13_t));
	if (!Type13) return NULL;
	if (count == 0) {
		*type13_count = 0;
		return Type13;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_FIRMWARE_LANGUAGE_INFORMATION) {
			if (index >= count) break;
			lazybiosType13_t* current = &Type13[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READU8(current, installable_languages, len, INSTALLABLE_LANGUAGES, p);
			if (lazybiosIsVersionPlus(DMIData, 2, 1)) {
				READU8(current, flags, len, FLAGS, p);
			} else {
				current->flags = 0;
			}
			READSTR(current, current_language, len, CURRENT_LANGUAGE, p, structure_end);
			if (LAZYBIOS_FIELD_STATUS(current, current_language) == LAZYBIOS_FIELD_PRESENT &&
				LAZYBIOS_FIELD_STATUS(current, installable_languages) == LAZYBIOS_FIELD_PRESENT &&
				p[CURRENT_LANGUAGE] > current->installable_languages) {
				free(current->current_language);
				current->current_language = NULL;
				LAZYBIOS_MARK_ABSENT(current, current_language);
			}

			if (LAZYBIOS_FIELD_STATUS(current, installable_languages) == LAZYBIOS_FIELD_PRESENT &&
				current->installable_languages > 0) {
				current->languages = calloc(current->installable_languages, sizeof(char*));
				if (current->languages) {
					LAZYBIOS_MARK_PRESENT(current, languages);
					for (size_t i = 0; i < current->installable_languages; i++) {
						current->languages[i] = DMIString(p, len, (uint8_t)(i + 1), structure_end);
						if (!current->languages[i] || current->languages[i][0] == '\0') {
							free(current->languages[i]);
							current->languages[i] = NULL;
							LAZYBIOS_MARK_ABSENT(current, languages);
						}
					}
				} else {
					lazybiosFreeType13(Type13, count);
					return NULL;
				}
			} else if (LAZYBIOS_FIELD_STATUS(current, installable_languages) == LAZYBIOS_FIELD_PRESENT) {
				LAZYBIOS_MARK_PRESENT(current, languages);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type13_count = index;
	return Type13;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes the language-description format selected by the Type 13 flags.
 *
 * @param flags Raw SMBIOS Type 13 flags byte.
 * @return Static string describing the language-description format.
 */
const char* lazybiosType13LanguageFormatStr(uint8_t flags) {
	return (flags & LANGUAGE_FORMAT_MASK) ? "Abbreviated" : "Long";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 13 structures.
 *
 * @param Type13 Type 13 array to release.
 * @param type13_count Number of elements in Type13.
 */
void lazybiosFreeType13(lazybiosType13_t* Type13, size_t type13_count) {
	if (!Type13) return;

	for (size_t i = 0; i < type13_count; i++) {
		if (Type13[i].languages) {
			for (size_t j = 0; j < Type13[i].installable_languages; j++) {
				free(Type13[i].languages[j]);
			}
		}
		free(Type13[i].languages);
		free(Type13[i].current_language);
	}

	free(Type13);
}

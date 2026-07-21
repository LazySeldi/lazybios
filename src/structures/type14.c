/**
 * @file type14.c
 * @brief Implements parsing for SMBIOS Type 14 Group Associations.
 * @author LazySeldi
 */

//
// Type 14 ( Group Associations )
//

#include "lazybios_internal.h"
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define GROUP_NAME 0x04
#define ITEMS 0x05
#define ITEM_SIZE 3
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 14 Group Associations structures.
 *
 * @param Type14 Existing Type 14 array pointer value; it is not dereferenced or released.
 * @param type14_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 14 array, or NULL on failure.
 */
lazybiosType14_t* lazybiosGetType14(lazybiosType14_t* Type14, size_t* type14_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_GROUP_ASSOCIATIONS);
	size_t index = 0;

	Type14 = calloc(count, sizeof(lazybiosType14_t));
	if (!Type14) return NULL;
	if (count == 0) {
		*type14_count = 0;
		return Type14;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_GROUP_ASSOCIATIONS) {
			if (index >= count) break;
			lazybiosType14_t* current = &Type14[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, group_name, len, GROUP_NAME, p, structure_end);

			if (len >= ITEMS && (len - ITEMS) % ITEM_SIZE == 0) {
				current->item_count = (len - ITEMS) / ITEM_SIZE;
				LAZYBIOS_MARK_PRESENT(current, item_count);

				if (current->item_count > 0) {
					current->items = calloc(current->item_count, sizeof(lazybiosType14Item_t));
					if (!current->items) {
						lazybiosFreeType14(Type14, count);
						return NULL;
					}

					for (size_t i = 0; i < current->item_count; i++) {
						const size_t item_offset = ITEMS + (i * ITEM_SIZE);
						current->items[i].item_type = p[item_offset];
						memcpy(&current->items[i].item_handle, p + item_offset + 1, sizeof(uint16_t));
						LAZYBIOS_MARK_PRESENT(&current->items[i], item_type);
						LAZYBIOS_MARK_PRESENT(&current->items[i], item_handle);
						if (current->items[i].item_handle == 0xFFFF) {
							LAZYBIOS_MARK_ABSENT(&current->items[i], item_handle);
						}
					}
				}
				LAZYBIOS_MARK_PRESENT(current, items);
			} else {
				current->item_count = 0;
				current->items = NULL;
				LAZYBIOS_MARK_ABSENT(current, item_count);
				LAZYBIOS_MARK_ABSENT(current, items);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type14_count = index;
	return Type14;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 14 structures.
 *
 * @param Type14 Type 14 array to release.
 * @param type14_count Number of elements in Type14.
 */
void lazybiosFreeType14(lazybiosType14_t* Type14, size_t type14_count) {
	if (!Type14) return;

	for (size_t i = 0; i < type14_count; i++) {
		free(Type14[i].group_name);
		free(Type14[i].items);
	}

	free(Type14);
}

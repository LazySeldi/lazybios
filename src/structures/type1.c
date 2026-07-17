/**
 * @file type1.c
 * @brief Implements parsing and decoding for SMBIOS Type 1 System Information.
 * @author LazySeldi
 */

//
// Type 1 ( System Information )
//

#include "lazybios.h"
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define MANUFACTURER 0x04
#define PRODUCT_NAME 0x05
#define VERSION 0x06
#define SERIAL_NUMBER 0x07
#define UUID 0x08
#define WAKE_UP_TYPE 0x18
#define SKU_NUMBER 0x19
#define FAMILY 0x1A

// Decoders

// Wake Up Type
#define WAKEUP_TYPE_RESERVED 0x00
#define WAKEUP_TYPE_OTHER 0x01
#define WAKEUP_TYPE_UNKNOWN 0x02
#define WAKEUP_TYPE_APM_TIMER 0x03
#define WAKEUP_TYPE_MODEM_RING 0x04
#define WAKEUP_TYPE_LAN_REMOTE 0x05
#define WAKEUP_TYPE_POWER_SWITCH 0x06
#define WAKEUP_TYPE_PCI_PME 0x07
#define WAKEUP_TYPE_AC_POWER_RESTORED 0x08
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses the first SMBIOS Type 1 System Information structure.
 *
 * @param Type1 Existing Type 1 pointer value; it is not dereferenced or released.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 1 structure, or NULL on failure or absence.
 */
lazybiosType1_t* lazybiosGetType1(lazybiosType1_t* Type1, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return LAZYBIOS_NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	while (p + SMBIOS_HEADER_SIZE <= end) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_SYSTEM) {
			Type1 = calloc(1, sizeof(*Type1));
			if (!Type1) return LAZYBIOS_NULL;

			if (len > MANUFACTURER) Type1->manufacturer = DMIString(p, len, p[MANUFACTURER], end);
			if (!Type1->manufacturer) Type1->manufacturer = strdup(LAZYBIOS_NOT_FOUND_STR);

			if (len > PRODUCT_NAME) Type1->product_name = DMIString(p, len, p[PRODUCT_NAME], end);
			if (!Type1->product_name) Type1->product_name = strdup(LAZYBIOS_NOT_FOUND_STR);

			if (len > VERSION) Type1->version = DMIString(p, len, p[VERSION], end);
			if (!Type1->version) Type1->version = strdup(LAZYBIOS_NOT_FOUND_STR);

			if (len > SERIAL_NUMBER) Type1->serial_number = DMIString(p, len, p[SERIAL_NUMBER], end);
			if (!Type1->serial_number) Type1->serial_number = strdup(LAZYBIOS_NOT_FOUND_STR);

			if (ISVERPLUS(DMIData, 2, 1)) {
				if (len >= UUID + sizeof(Type1->uuid)) {
					const uint8_t* uuid = p + UUID;
					for (int i = 0; i < 16; i++) Type1->uuid[i] = uuid[i];
				} else {
					for (int i = 0; i < 16; i++) Type1->uuid[i] = LAZYBIOS_NOT_FOUND_U8;
				}
				READU8(Type1->wake_up_type, len, WAKE_UP_TYPE, p)
			} else {
				for (int i = 0; i < 16; i++) Type1->uuid[i] = LAZYBIOS_NOT_FOUND_U8;
				Type1->wake_up_type = LAZYBIOS_NOT_FOUND_U8;
			}

			if (ISVERPLUS(DMIData, 2, 4)) {
				Type1->sku_number = (len > SKU_NUMBER) ? DMIString(p, len, p[SKU_NUMBER], end) : strdup(LAZYBIOS_NOT_FOUND_STR);
				if (!Type1->sku_number) Type1->sku_number = strdup(LAZYBIOS_NOT_FOUND_STR);

				Type1->family = (len > FAMILY) ? DMIString(p, len, p[FAMILY], end) : strdup(LAZYBIOS_NOT_FOUND_STR);
				if (!Type1->family) Type1->family = strdup(LAZYBIOS_NOT_FOUND_STR);
			} else {
				Type1->sku_number = strdup(LAZYBIOS_NOT_FOUND_STR);
				Type1->family = strdup(LAZYBIOS_NOT_FOUND_STR);
			}

			return Type1;
		}
		p = DMINext(p, end);
	}
	return LAZYBIOS_NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders

// Wake Up Type
/**
 * @brief Decodes an SMBIOS system wake-up type.
 *
 * @param wake_up_type Raw SMBIOS wake-up type value.
 * @return Static string describing the wake-up type.
 */
const char* lazybiosType1WakeupTypeStr(uint8_t wake_up_type) {
	switch (wake_up_type) {
		case WAKEUP_TYPE_RESERVED:
			return "Reserved";
		case WAKEUP_TYPE_OTHER:
			return "Other";
		case WAKEUP_TYPE_UNKNOWN:
			return "Unknown";
		case WAKEUP_TYPE_APM_TIMER:
			return "APM Timer";
		case WAKEUP_TYPE_MODEM_RING:
			return "Modem Ring";
		case WAKEUP_TYPE_LAN_REMOTE:
			return "LAN Remote";
		case WAKEUP_TYPE_POWER_SWITCH:
			return "Power Switch";
		case WAKEUP_TYPE_PCI_PME:
			return "PCI PME#";
		case WAKEUP_TYPE_AC_POWER_RESTORED:
			return "AC Power Restored";
		default:
			return "Unknown/Reserved";
	}
}

// Free Function
/**
 * @brief Releases a parsed SMBIOS Type 1 structure.
 *
 * @param Type1 Type 1 structure to release.
 */
void lazybiosFreeType1(lazybiosType1_t* Type1) {
	if (!Type1) return;

	free(Type1->manufacturer);
	free(Type1->product_name);
	free(Type1->version);
	free(Type1->serial_number);
	free(Type1->sku_number);
	free(Type1->family);
	free(Type1);
}

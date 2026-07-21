/**
 * @file type1.c
 * @brief Implements parsing and decoding for SMBIOS Type 1 System Information.
 * @author LazySeldi
 */

//
// Type 1 ( System Information )
//

#include "lazybios_internal.h"
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
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	while (p + SMBIOS_HEADER_SIZE <= end) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_SYSTEM) {
			Type1 = calloc(1, sizeof(*Type1));
			if (!Type1) return NULL;
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(Type1, manufacturer, len, MANUFACTURER, p, structure_end);
			READSTR(Type1, product_name, len, PRODUCT_NAME, p, structure_end);
			READSTR(Type1, version, len, VERSION, p, structure_end);
			READSTR(Type1, serial_number, len, SERIAL_NUMBER, p, structure_end);

			if (lazybiosIsVersionPlus(DMIData, 2, 1)) {
				if (len >= UUID + sizeof(Type1->uuid)) {
					const uint8_t* uuid = p + UUID;
					int all_zero = 1;
					int all_ff = 1;
					for (int i = 0; i < 16; i++) Type1->uuid[i] = uuid[i];
					for (int i = 0; i < 16; i++) {
						if (uuid[i] != 0x00) all_zero = 0;
						if (uuid[i] != 0xFF) all_ff = 0;
					}
					if (all_zero || all_ff) {
						LAZYBIOS_MARK_ABSENT(Type1, uuid);
					} else {
						LAZYBIOS_MARK_PRESENT(Type1, uuid);
					}
				} else {
					for (int i = 0; i < 16; i++) Type1->uuid[i] = 0;
					LAZYBIOS_MARK_ABSENT(Type1, uuid);
				}
				READU8(Type1, wake_up_type, len, WAKE_UP_TYPE, p);
			} else {
				for (int i = 0; i < 16; i++) Type1->uuid[i] = 0;
				Type1->wake_up_type = 0;
			}

			if (lazybiosIsVersionPlus(DMIData, 2, 4)) {
				READSTR(Type1, sku_number, len, SKU_NUMBER, p, structure_end);
				READSTR(Type1, family, len, FAMILY, p, structure_end);
			} else {
				Type1->sku_number = NULL;
				Type1->family = NULL;
			}

			return Type1;
		}
		p = DMINext(p, end);
	}
	return NULL;
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

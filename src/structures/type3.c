/**
 * @file type3.c
 * @brief Implements parsing and decoding for SMBIOS Type 3 Chassis Information.
 * @author LazySeldi
 */

//
// Type 3 ( System Enclosure or Chassis )
//

#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define MANUFACTURER 0x04
#define TYPE 0x05
#define VERSION 0x06
#define SERIAL_NUMBER 0x07
#define ASSET_TAG_NUMBER 0x08
#define BOOT_UP_STATE 0x09
#define POWER_SUPPLY_STATE 0x0A
#define THERMAL_STATE 0x0B
#define SECURITY_STATUS 0x0C
#define OEM_DEFINED 0x0D
#define HEIGHT 0x11
#define NUMBER_OF_POWER_CORDS 0x12
#define CONTAINED_ELEMENT_COUNT 0x13
#define CONTAINED_ELEMENT_RECORD_LENGTH 0x14
#define CONTAINED_ELEMENTS 0x15
#define SKU_NUMBER(n, m) 0x15 + (n * m)
#define RACK_TYPE(n, m) 0x16 + (n * m)
#define RACK_HEIGHT(n, m) 0x17 + (n * m)

// Decoders

// Chassis Type
#define CHASSIS_TYPE_OTHER 0x01
#define CHASSIS_TYPE_UNKNOWN 0x02
#define CHASSIS_TYPE_DESKTOP 0x03
#define CHASSIS_TYPE_LOW_PROFILE_DESKTOP 0x04
#define CHASSIS_TYPE_PIZZA_BOX 0x05
#define CHASSIS_TYPE_MINI_TOWER 0x06
#define CHASSIS_TYPE_TOWER 0x07
#define CHASSIS_TYPE_PORTABLE 0x08
#define CHASSIS_TYPE_LAPTOP 0x09
#define CHASSIS_TYPE_NOTEBOOK 0x0A
#define CHASSIS_TYPE_HAND_HELD 0x0B
#define CHASSIS_TYPE_DOCKING_STATION 0x0C
#define CHASSIS_TYPE_ALL_IN_ONE 0x0D
#define CHASSIS_TYPE_SUB_NOTEBOOK 0x0E
#define CHASSIS_TYPE_SPACE_SAVING 0x0F
#define CHASSIS_TYPE_LUNCH_BOX 0x10
#define CHASSIS_TYPE_MAIN_SERVER_CHASSIS 0x11
#define CHASSIS_TYPE_EXPANSION_CHASSIS 0x12
#define CHASSIS_TYPE_SUBCHASSIS 0x13
#define CHASSIS_TYPE_BUS_EXPANSION_CHASSIS 0x14
#define CHASSIS_TYPE_PERIPHERAL_CHASSIS 0x15
#define CHASSIS_TYPE_RAID_CHASSIS 0x16
#define CHASSIS_TYPE_RACK_MOUNT_CHASSIS 0x17
#define CHASSIS_TYPE_SEALED_CASE_PC 0x18
#define CHASSIS_TYPE_MULTI_SYSTEM_CHASSIS 0x19
#define CHASSIS_TYPE_COMPACT_PCI 0x1A
#define CHASSIS_TYPE_ADVANCED_TCA 0x1B
#define CHASSIS_TYPE_BLADE 0x1C
#define CHASSIS_TYPE_BLADE_ENCLOSURE 0x1D
#define CHASSIS_TYPE_TABLET 0x1E
#define CHASSIS_TYPE_CONVERTIBLE 0x1F
#define CHASSIS_TYPE_DETACHABLE 0x20
#define CHASSIS_TYPE_IOT_GATEWAY 0x21
#define CHASSIS_TYPE_EMBEDDED_PC 0x22
#define CHASSIS_TYPE_MINI_PC 0x23
#define CHASSIS_TYPE_STICK_PC 0x24

// Chassis State
#define CHASSIS_STATE_OTHER 0x01
#define CHASSIS_STATE_UNKNOWN 0x02
#define CHASSIS_STATE_SAFE 0x03
#define CHASSIS_STATE_WARNING 0x04
#define CHASSIS_STATE_CRITICAL 0x05
#define CHASSIS_STATE_NON_RECOVERABLE 0x06

// Chassis Status
#define CHASSIS_SECURITY_STATUS_OTHER 0x01
#define CHASSIS_SECURITY_STATUS_UNKNOWN 0x02
#define CHASSIS_SECURITY_STATUS_NONE 0x03
#define CHASSIS_SECURITY_STATUS_EXT_INTERFACE_LOCKED_OUT 0x04
#define CHASSIS_SECURITY_STATUS_EXT_INTERFACE_ENABLED 0x05
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 3 Chassis Information structures.
 *
 * @param Type3 Existing Type 3 array pointer value; it is not dereferenced or released.
 * @param type3_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 3 array, or NULL on failure.
 */
lazybiosType3_t* lazybiosGetType3(lazybiosType3_t* Type3, size_t* type3_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_CHASSIS);
	size_t index = 0;
	Type3 = calloc(count, sizeof(lazybiosType3_t));
	if (!Type3) return NULL;
	if (count == 0) {
		*type3_count = 0;
		return Type3;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_CHASSIS) {
			if (index >= count) break;
			lazybiosType3_t* current = &Type3[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, manufacturer, len, MANUFACTURER, p, structure_end);

			READU8(current, type, len, TYPE, p);

			READSTR(current, version, len, VERSION, p, structure_end);

			READSTR(current, serial_number, len, SERIAL_NUMBER, p, structure_end);

			READSTR(current, asset_tag, len, ASSET_TAG_NUMBER, p, structure_end);

			if (lazybiosIsVersionPlus(DMIData, 2, 1)) {
				READU8(current, boot_up_state, len, BOOT_UP_STATE, p);
				READU8(current, power_supply_state, len, POWER_SUPPLY_STATE, p);
				READU8(current, thermal_state, len, THERMAL_STATE, p);
				READU8(current, security_status, len, SECURITY_STATUS, p);
			} else {
				current->boot_up_state = 0;
				current->power_supply_state = 0;
				current->thermal_state = 0;
				current->security_status = 0;
			}

			if (lazybiosIsVersionPlus(DMIData, 2, 3)) {
				READU32(current, oem_defined, len, OEM_DEFINED, p);

				READU8(current, height, len, HEIGHT, p);

				READU8(current, number_of_power_cords, len, NUMBER_OF_POWER_CORDS, p);

				READU8(current, contained_element_count, len, CONTAINED_ELEMENT_COUNT, p);
				READU8(current, contained_element_record_length, len, CONTAINED_ELEMENT_RECORD_LENGTH, p);

				if (LAZYBIOS_FIELD_STATUS(current, contained_element_count) == LAZYBIOS_FIELD_PRESENT &&
					LAZYBIOS_FIELD_STATUS(current, contained_element_record_length) == LAZYBIOS_FIELD_PRESENT) {
					const size_t array_bytes = (size_t)current->contained_element_count *
						current->contained_element_record_length;

					if (array_bytes == 0) {
						LAZYBIOS_MARK_PRESENT(current, contained_elements);
					} else if ((size_t)len >= CONTAINED_ELEMENTS + array_bytes) {
						current->contained_elements = malloc(array_bytes);
						if (current->contained_elements) {
							memcpy(current->contained_elements, p + CONTAINED_ELEMENTS, array_bytes);
							LAZYBIOS_MARK_PRESENT(current, contained_elements);
						}
					} else {
						LAZYBIOS_MARK_ABSENT(current, contained_elements);
					}

					if (lazybiosIsVersionPlus(DMIData, 2, 7)) {
						READSTR(current, sku_number, len,
							SKU_NUMBER(current->contained_element_count, current->contained_element_record_length), p, structure_end);
					} else {
						current->sku_number = NULL;
					}

					if (lazybiosIsVersionPlus(DMIData, 3, 9)) {
						READU8(current, rack_type, len,
							RACK_TYPE(current->contained_element_count, current->contained_element_record_length), p);
						READU8(current, rack_height, len,
							RACK_HEIGHT(current->contained_element_count, current->contained_element_record_length), p);
					} else {
						current->rack_type = 0;
						current->rack_height = 0;
					}
				}
			} else {
				current->oem_defined = 0;
				current->height = 0;
				current->number_of_power_cords = 0;
				current->contained_element_count = 0;
				current->contained_element_record_length = 0;
				current->contained_elements = NULL;
				current->sku_number = NULL;
				current->rack_type = 0;
				current->rack_height = 0;
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type3_count = index;
	return Type3;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders

// Chassis Type
/**
 * @brief Decodes an SMBIOS chassis type and lock indicator.
 *
 * @param type Raw SMBIOS chassis type byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType3TypeStr(uint8_t type, char* buf, size_t buf_len) {
	size_t len = 0;
	buf[0] = '\0';

	// Bit 7 is the Chassis lock
	if (type & (1 << 7)) {
		len += snprintf(buf + len, buf_len - len, "Chassis lock present, ");
	}

	// Bits 6:0 = chassis type
	uint8_t chassis_type = type & 0x7F;
	switch (chassis_type) {
		case CHASSIS_TYPE_OTHER:
			len += snprintf(buf + len, buf_len - len, "Other");
			break;
		case CHASSIS_TYPE_UNKNOWN:
			len += snprintf(buf + len, buf_len - len, "Unknown");
			break;
		case CHASSIS_TYPE_DESKTOP:
			len += snprintf(buf + len, buf_len - len, "Desktop");
			break;
		case CHASSIS_TYPE_LOW_PROFILE_DESKTOP:
			len += snprintf(buf + len, buf_len - len, "Low Profile Desktop");
			break;
		case CHASSIS_TYPE_PIZZA_BOX:
			len += snprintf(buf + len, buf_len - len, "Pizza Box");
			break;
		case CHASSIS_TYPE_MINI_TOWER:
			len += snprintf(buf + len, buf_len - len, "Mini Tower");
			break;
		case CHASSIS_TYPE_TOWER:
			len += snprintf(buf + len, buf_len - len, "Tower");
			break;
		case CHASSIS_TYPE_PORTABLE:
			len += snprintf(buf + len, buf_len - len, "Portable");
			break;
		case CHASSIS_TYPE_LAPTOP:
			len += snprintf(buf + len, buf_len - len, "Laptop");
			break;
		case CHASSIS_TYPE_NOTEBOOK:
			len += snprintf(buf + len, buf_len - len, "Notebook");
			break;
		case CHASSIS_TYPE_HAND_HELD:
			len += snprintf(buf + len, buf_len - len, "Hand Held");
			break;
		case CHASSIS_TYPE_DOCKING_STATION:
			len += snprintf(buf + len, buf_len - len, "Docking Station");
			break;
		case CHASSIS_TYPE_ALL_IN_ONE:
			len += snprintf(buf + len, buf_len - len, "All in One");
			break;
		case CHASSIS_TYPE_SUB_NOTEBOOK:
			len += snprintf(buf + len, buf_len - len, "Sub Notebook");
			break;
		case CHASSIS_TYPE_SPACE_SAVING:
			len += snprintf(buf + len, buf_len - len, "Space-saving");
			break;
		case CHASSIS_TYPE_LUNCH_BOX:
			len += snprintf(buf + len, buf_len - len, "Lunch Box");
			break;
		case CHASSIS_TYPE_MAIN_SERVER_CHASSIS:
			len += snprintf(buf + len, buf_len - len, "Main Server Chassis");
			break;
		case CHASSIS_TYPE_EXPANSION_CHASSIS:
			len += snprintf(buf + len, buf_len - len, "Expansion Chassis");
			break;
		case CHASSIS_TYPE_SUBCHASSIS:
			len += snprintf(buf + len, buf_len - len, "SubChassis");
			break;
		case CHASSIS_TYPE_BUS_EXPANSION_CHASSIS:
			len += snprintf(buf + len, buf_len - len, "Bus Expansion Chassis");
			break;
		case CHASSIS_TYPE_PERIPHERAL_CHASSIS:
			len += snprintf(buf + len, buf_len - len, "Peripheral Chassis");
			break;
		case CHASSIS_TYPE_RAID_CHASSIS:
			len += snprintf(buf + len, buf_len - len, "RAID Chassis");
			break;
		case CHASSIS_TYPE_RACK_MOUNT_CHASSIS:
			len += snprintf(buf + len, buf_len - len, "Rack Mount Chassis");
			break;
		case CHASSIS_TYPE_SEALED_CASE_PC:
			len += snprintf(buf + len, buf_len - len, "Sealed-case PC");
			break;
		case CHASSIS_TYPE_MULTI_SYSTEM_CHASSIS:
			len += snprintf(buf + len, buf_len - len, "Multi-system chassis");
			break;
		case CHASSIS_TYPE_COMPACT_PCI:
			len += snprintf(buf + len, buf_len - len, "Compact PCI");
			break;
		case CHASSIS_TYPE_ADVANCED_TCA:
			len += snprintf(buf + len, buf_len - len, "Advanced TCA");
			break;
		case CHASSIS_TYPE_BLADE:
			len += snprintf(buf + len, buf_len - len, "Blade");
			break;
		case CHASSIS_TYPE_BLADE_ENCLOSURE:
			len += snprintf(buf + len, buf_len - len, "Blade Enclosure");
			break;
		case CHASSIS_TYPE_TABLET:
			len += snprintf(buf + len, buf_len - len, "Tablet");
			break;
		case CHASSIS_TYPE_CONVERTIBLE:
			len += snprintf(buf + len, buf_len - len, "Convertible");
			break;
		case CHASSIS_TYPE_DETACHABLE:
			len += snprintf(buf + len, buf_len - len, "Detachable");
			break;
		case CHASSIS_TYPE_IOT_GATEWAY:
			len += snprintf(buf + len, buf_len - len, "IoT Gateway");
			break;
		case CHASSIS_TYPE_EMBEDDED_PC:
			len += snprintf(buf + len, buf_len - len, "Embedded PC");
			break;
		case CHASSIS_TYPE_MINI_PC:
			len += snprintf(buf + len, buf_len - len, "Mini PC");
			break;
		case CHASSIS_TYPE_STICK_PC:
			len += snprintf(buf + len, buf_len - len, "Stick PC");
			break;
		default:
			len += snprintf(buf + len, buf_len - len, "Unknown Chassis Type");
			break;
	}

	if (len >= 2 && buf[len - 2] == ',') buf[len - 2] = '\0'; // remove trailing ", " ONLY if present
	if (len == 0) {
		snprintf(buf, buf_len, "None");
	}
}

// Chassis State
/**
 * @brief Decodes an SMBIOS chassis state value.
 *
 * @param state Raw boot-up, power-supply, or thermal state value.
 * @return Static string describing the chassis state.
 */
const char* lazybiosType3StateStr(uint8_t state) {
	switch (state) {
		case CHASSIS_STATE_OTHER:
			return "Other";
		case CHASSIS_STATE_UNKNOWN:
			return "Unknown";
		case CHASSIS_STATE_SAFE:
			return "Safe";
		case CHASSIS_STATE_WARNING:
			return "Warning";
		case CHASSIS_STATE_CRITICAL:
			return "Critical";
		case CHASSIS_STATE_NON_RECOVERABLE:
			return "Non-recoverable";
		default:
			return "Unknown Chassis State";
	}
}

// Chassis Status
/**
 * @brief Decodes an SMBIOS chassis security status.
 *
 * @param security_status Raw SMBIOS security status value.
 * @return Static string describing the security status.
 */
const char* lazybiosType3SecurityStatusStr(uint8_t security_status) {
	switch (security_status) {
		case CHASSIS_SECURITY_STATUS_OTHER:
			return "Other";
		case CHASSIS_SECURITY_STATUS_UNKNOWN:
			return "Unknown";
		case CHASSIS_SECURITY_STATUS_NONE:
			return "None";
		case CHASSIS_SECURITY_STATUS_EXT_INTERFACE_LOCKED_OUT:
			return "External interface locked out";
		case CHASSIS_SECURITY_STATUS_EXT_INTERFACE_ENABLED:
			return "External interface enabled";
		default:
			return "Unknown chassis security state";
	}
}

// Chassis Contained Elements
/**
 * @brief Decodes the type field of a chassis contained-element record.
 *
 * @param contained_elements Raw contained-element type byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType3ContainedElementTypeStr(uint8_t contained_elements, char* buf, size_t buf_len) {
	buf[0] = '\0';

	if (contained_elements & 0x80) { // MSB = 1 → SMBIOS structure type
		uint8_t struct_type = contained_elements & 0x7F;
		snprintf(buf, buf_len, "SMBIOS Structure Type %u", struct_type);
	} else { // MSB = 0 → board type
		uint8_t board_type = contained_elements & 0x7F;
		const char* str = lazybiosType2BoardTypeStr(board_type);
		snprintf(buf, buf_len, "%s", str);
	}
}

// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 3 structures.
 *
 * @param Type3 Type 3 array to release.
 * @param type3_count Number of elements in Type3.
 */
void lazybiosFreeType3(lazybiosType3_t* Type3, size_t type3_count) {
	if (!Type3) return;

	for (size_t i = 0; i < type3_count; i++) {
		free(Type3[i].manufacturer);
		free(Type3[i].version);
		free(Type3[i].serial_number);
		free(Type3[i].asset_tag);
		free(Type3[i].contained_elements);
		free(Type3[i].sku_number);
	}

	free(Type3);
}

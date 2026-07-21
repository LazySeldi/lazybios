/**
 * @file type27.c
 * @brief Implements parsing and decoding for SMBIOS Type 27 Cooling Device.
 * @author LazySeldi
 */

//
// Type 27 ( Cooling Device )
//

#include "lazybios_internal.h"
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define TEMPERATURE_PROBE_HANDLE 0x04
#define DEVICE_TYPE_AND_STATUS 0x06
#define COOLING_UNIT_GROUP 0x07
#define OEM_DEFINED 0x08
#define NOMINAL_SPEED 0x0C
#define DESCRIPTION 0x0E

// Device Type and Status Masks
#define DEVICE_TYPE_MASK 0x1F
#define STATUS_MASK 0xE0
#define STATUS_SHIFT 5

// Device Types
#define DEVICE_TYPE_OTHER 0x01
#define DEVICE_TYPE_UNKNOWN 0x02
#define DEVICE_TYPE_FAN 0x03
#define DEVICE_TYPE_CENTRIFUGAL_BLOWER 0x04
#define DEVICE_TYPE_CHIP_FAN 0x05
#define DEVICE_TYPE_CABINET_FAN 0x06
#define DEVICE_TYPE_POWER_SUPPLY_FAN 0x07
#define DEVICE_TYPE_HEAT_PIPE 0x08
#define DEVICE_TYPE_INTEGRATED_REFRIGERATION 0x09
#define DEVICE_TYPE_ACTIVE_COOLING 0x10
#define DEVICE_TYPE_PASSIVE_COOLING 0x11

// Statuses
#define STATUS_OTHER 0x01
#define STATUS_UNKNOWN 0x02
#define STATUS_OK 0x03
#define STATUS_NON_CRITICAL 0x04
#define STATUS_CRITICAL 0x05
#define STATUS_NON_RECOVERABLE 0x06
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 27 Cooling Device structures.
 *
 * @param Type27 Existing Type 27 array pointer value; it is not dereferenced or released.
 * @param type27_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 27 array, or NULL on failure.
 */
lazybiosType27_t* lazybiosGetType27(lazybiosType27_t* Type27, size_t* type27_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_COOLING_DEVICE);
	size_t index = 0;

	Type27 = calloc(count, sizeof(lazybiosType27_t));
	if (!Type27) return NULL;
	if (count == 0) {
		*type27_count = 0;
		return Type27;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_COOLING_DEVICE) {
			if (index >= count) break;
			lazybiosType27_t* current = &Type27[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READU16(current, temperature_probe_handle, len, TEMPERATURE_PROBE_HANDLE, p);
			if (current->temperature_probe_handle == 0xFFFF) {
				LAZYBIOS_MARK_ABSENT(current, temperature_probe_handle);
			}
			READU8(current, device_type_and_status, len, DEVICE_TYPE_AND_STATUS, p);
			READU8(current, cooling_unit_group, len, COOLING_UNIT_GROUP, p);
			READU32(current, oem_defined, len, OEM_DEFINED, p);
			READU16(current, nominal_speed, len, NOMINAL_SPEED, p);

			if (lazybiosIsVersionPlus(DMIData, 2, 7)) {
				READSTR(current, description, len, DESCRIPTION, p, structure_end);
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type27_count = index;
	return Type27;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes the cooling-device type from a Type 27 device-type-and-status field.
 *
 * @param device_type_and_status Raw Type 27 device-type-and-status byte.
 * @return Static string describing the cooling-device type.
 */
const char* lazybiosType27DeviceTypeStr(uint8_t device_type_and_status) {
	switch (device_type_and_status & DEVICE_TYPE_MASK) {
		case DEVICE_TYPE_OTHER:
			return "Other";
		case DEVICE_TYPE_UNKNOWN:
			return "Unknown";
		case DEVICE_TYPE_FAN:
			return "Fan";
		case DEVICE_TYPE_CENTRIFUGAL_BLOWER:
			return "Centrifugal Blower";
		case DEVICE_TYPE_CHIP_FAN:
			return "Chip Fan";
		case DEVICE_TYPE_CABINET_FAN:
			return "Cabinet Fan";
		case DEVICE_TYPE_POWER_SUPPLY_FAN:
			return "Power Supply Fan";
		case DEVICE_TYPE_HEAT_PIPE:
			return "Heat Pipe";
		case DEVICE_TYPE_INTEGRATED_REFRIGERATION:
			return "Integrated Refrigeration";
		case DEVICE_TYPE_ACTIVE_COOLING:
			return "Active Cooling";
		case DEVICE_TYPE_PASSIVE_COOLING:
			return "Passive Cooling";
		default:
			return "Undefined";
	}
}

/**
 * @brief Decodes the cooling-device status from a Type 27 device-type-and-status field.
 *
 * @param device_type_and_status Raw Type 27 device-type-and-status byte.
 * @return Static string describing the cooling-device status.
 */
const char* lazybiosType27StatusStr(uint8_t device_type_and_status) {
	switch ((device_type_and_status & STATUS_MASK) >> STATUS_SHIFT) {
		case STATUS_OTHER:
			return "Other";
		case STATUS_UNKNOWN:
			return "Unknown";
		case STATUS_OK:
			return "OK";
		case STATUS_NON_CRITICAL:
			return "Non-critical";
		case STATUS_CRITICAL:
			return "Critical";
		case STATUS_NON_RECOVERABLE:
			return "Non-recoverable";
		default:
			return "Undefined";
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 27 structures.
 *
 * @param Type27 Type 27 array to release.
 * @param type27_count Number of elements in Type27.
 */
void lazybiosFreeType27(lazybiosType27_t* Type27, size_t type27_count) {
	if (!Type27) return;

	for (size_t i = 0; i < type27_count; i++) {
		free(Type27[i].description);
	}
	free(Type27);
}

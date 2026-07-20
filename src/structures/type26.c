/**
 * @file type26.c
 * @brief Implements parsing and decoding for SMBIOS Type 26 Voltage Probe.
 * @author LazySeldi
 */

//
// Type 26 ( Voltage Probe )
//

#include "lazybios_internal.h"
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define DESCRIPTION 0x04
#define LOCATION_AND_STATUS 0x05
#define MAXIMUM_VALUE 0x06
#define MINIMUM_VALUE 0x08
#define RESOLUTION 0x0A
#define TOLERANCE 0x0C
#define ACCURACY 0x0E
#define OEM_DEFINED 0x10
#define NOMINAL_VALUE 0x14

// Location and Status Masks
#define LOCATION_MASK 0x1F
#define STATUS_MASK 0xE0
#define STATUS_SHIFT 5

// Locations
#define LOCATION_OTHER 0x01
#define LOCATION_UNKNOWN 0x02
#define LOCATION_PROCESSOR 0x03
#define LOCATION_DISK 0x04
#define LOCATION_PERIPHERAL_BAY 0x05
#define LOCATION_SYSTEM_MANAGEMENT_MODULE 0x06
#define LOCATION_MOTHERBOARD 0x07
#define LOCATION_MEMORY_MODULE 0x08
#define LOCATION_PROCESSOR_MODULE 0x09
#define LOCATION_POWER_UNIT 0x0A
#define LOCATION_ADD_IN_CARD 0x0B

// Statuses
#define STATUS_OTHER 0x01
#define STATUS_UNKNOWN 0x02
#define STATUS_OK 0x03
#define STATUS_NON_CRITICAL 0x04
#define STATUS_CRITICAL 0x05
#define STATUS_NON_RECOVERABLE 0x06
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 26 Voltage Probe structures.
 *
 * @param Type26 Existing Type 26 array pointer value; it is not dereferenced or released.
 * @param type26_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 26 array, or NULL on failure.
 */
lazybiosType26_t* lazybiosGetType26(lazybiosType26_t* Type26, size_t* type26_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_VOLTAGE_PROBE);
	size_t index = 0;

	Type26 = calloc(count, sizeof(lazybiosType26_t));
	if (!Type26) return NULL;
	if (count == 0) {
		*type26_count = 0;
		return Type26;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_VOLTAGE_PROBE) {
			if (index >= count) break;
			lazybiosType26_t* current = &Type26[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, description, len, DESCRIPTION, p, structure_end);
			READU8(current, location_and_status, len, LOCATION_AND_STATUS, p);
			READU16(current, maximum_value, len, MAXIMUM_VALUE, p);
			READU16(current, minimum_value, len, MINIMUM_VALUE, p);
			READU16(current, resolution, len, RESOLUTION, p);
			READU16(current, tolerance, len, TOLERANCE, p);
			READU16(current, accuracy, len, ACCURACY, p);
			READU32(current, oem_defined, len, OEM_DEFINED, p);
			READU16(current, nominal_value, len, NOMINAL_VALUE, p);

			index++;
		}
		p = DMINext(p, end);
	}
	*type26_count = index;
	return Type26;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes the physical location from a Type 26 location-and-status field.
 *
 * @param location_and_status Raw Type 26 location-and-status byte.
 * @return Static string describing the voltage-probe location.
 */
const char* lazybiosType26LocationStr(uint8_t location_and_status) {
	switch (location_and_status & LOCATION_MASK) {
		case LOCATION_OTHER:
			return "Other";
		case LOCATION_UNKNOWN:
			return "Unknown";
		case LOCATION_PROCESSOR:
			return "Processor";
		case LOCATION_DISK:
			return "Disk";
		case LOCATION_PERIPHERAL_BAY:
			return "Peripheral Bay";
		case LOCATION_SYSTEM_MANAGEMENT_MODULE:
			return "System Management Module";
		case LOCATION_MOTHERBOARD:
			return "Motherboard";
		case LOCATION_MEMORY_MODULE:
			return "Memory Module";
		case LOCATION_PROCESSOR_MODULE:
			return "Processor Module";
		case LOCATION_POWER_UNIT:
			return "Power Unit";
		case LOCATION_ADD_IN_CARD:
			return "Add-in Card";
		default:
			return "Undefined";
	}
}

/**
 * @brief Decodes the monitored-voltage status from a Type 26 location-and-status field.
 *
 * @param location_and_status Raw Type 26 location-and-status byte.
 * @return Static string describing the monitored-voltage status.
 */
const char* lazybiosType26StatusStr(uint8_t location_and_status) {
	switch ((location_and_status & STATUS_MASK) >> STATUS_SHIFT) {
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
 * @brief Releases an array of parsed SMBIOS Type 26 structures.
 *
 * @param Type26 Type 26 array to release.
 * @param type26_count Number of elements in Type26.
 */
void lazybiosFreeType26(lazybiosType26_t* Type26, size_t type26_count) {
	if (!Type26) return;

	for (size_t i = 0; i < type26_count; i++) {
		free(Type26[i].description);
	}
	free(Type26);
}

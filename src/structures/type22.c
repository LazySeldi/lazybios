/**
 * @file type22.c
 * @brief Implements parsing and decoding for SMBIOS Type 22 Portable Battery.
 * @author LazySeldi
 */

//
// Type 22 ( Portable Battery )
//

#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define LOCATION 0x04
#define MANUFACTURER 0x05
#define MANUFACTURE_DATE 0x06
#define SERIAL_NUMBER 0x07
#define DEVICE_NAME 0x08
#define DEVICE_CHEMISTRY 0x09
#define DESIGN_CAPACITY 0x0A
#define DESIGN_VOLTAGE 0x0C
#define SBDS_VERSION_NUMBER 0x0E
#define MAXIMUM_ERROR 0x0F
#define SBDS_SERIAL_NUMBER 0x10
#define SBDS_MANUFACTURE_DATE 0x12
#define SBDS_DEVICE_CHEMISTRY 0x14
#define DESIGN_CAPACITY_MULTIPLIER 0x15
#define OEM_SPECIFIC 0x16

// Device Chemistries
#define DEVICE_CHEMISTRY_OTHER 0x01
#define DEVICE_CHEMISTRY_UNKNOWN 0x02
#define DEVICE_CHEMISTRY_LEAD_ACID 0x03
#define DEVICE_CHEMISTRY_NICKEL_CADMIUM 0x04
#define DEVICE_CHEMISTRY_NICKEL_METAL_HYDRIDE 0x05
#define DEVICE_CHEMISTRY_LITHIUM_ION 0x06
#define DEVICE_CHEMISTRY_ZINC_AIR 0x07
#define DEVICE_CHEMISTRY_LITHIUM_POLYMER 0x08
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 22 Portable Battery structures.
 *
 * @param Type22 Existing Type 22 array pointer value; it is not dereferenced or released.
 * @param type22_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 22 array, or NULL on failure.
 */
lazybiosType22_t* lazybiosGetType22(lazybiosType22_t* Type22, size_t* type22_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_PORTABLE_BATTERY);
	size_t index = 0;

	Type22 = calloc(count, sizeof(lazybiosType22_t));
	if (!Type22) return NULL;
	if (count == 0) {
		*type22_count = 0;
		return Type22;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_PORTABLE_BATTERY) {
			if (index >= count) break;
			lazybiosType22_t* current = &Type22[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, location, len, LOCATION, p, structure_end);
			READSTR(current, manufacturer, len, MANUFACTURER, p, structure_end);
			READSTR(current, manufacture_date, len, MANUFACTURE_DATE, p, structure_end);
			READSTR(current, serial_number, len, SERIAL_NUMBER, p, structure_end);
			READSTR(current, device_name, len, DEVICE_NAME, p, structure_end);
			READU8(current, device_chemistry, len, DEVICE_CHEMISTRY, p);
			READU16(current, design_capacity, len, DESIGN_CAPACITY, p);
			READU16(current, design_voltage, len, DESIGN_VOLTAGE, p);
			READSTR(current, sbds_version_number, len, SBDS_VERSION_NUMBER, p, structure_end);
			READU8(current, maximum_error, len, MAXIMUM_ERROR, p);

			if (lazybiosIsVersionPlus(DMIData, 2, 2)) {
				READU16(current, sbds_serial_number, len, SBDS_SERIAL_NUMBER, p);
				READU16(current, sbds_manufacture_date, len, SBDS_MANUFACTURE_DATE, p);
				READSTR(current, sbds_device_chemistry, len, SBDS_DEVICE_CHEMISTRY, p, structure_end);
				READU8(current, design_capacity_multiplier, len, DESIGN_CAPACITY_MULTIPLIER, p);
				READU32(current, oem_specific, len, OEM_SPECIFIC, p);

				if (len > SERIAL_NUMBER && p[SERIAL_NUMBER] != 0) {
					LAZYBIOS_MARK_ABSENT(current, sbds_serial_number);
				}
				if (len > MANUFACTURE_DATE && p[MANUFACTURE_DATE] != 0) {
					LAZYBIOS_MARK_ABSENT(current, sbds_manufacture_date);
				}
				if (current->field_status.device_chemistry == LAZYBIOS_FIELD_PRESENT &&
					current->device_chemistry != 0x02) {
					LAZYBIOS_MARK_ABSENT(current, sbds_device_chemistry);
				}
			} else {
				current->sbds_serial_number = 0;
				current->sbds_manufacture_date = 0;
				current->sbds_device_chemistry = NULL;
				current->design_capacity_multiplier = 0;
				current->oem_specific = 0;
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type22_count = index;
	return Type22;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes an SMBIOS portable-battery chemistry value.
 *
 * @param device_chemistry Raw portable-battery chemistry value.
 * @return Static string describing the chemistry.
 */
const char* lazybiosType22DeviceChemistryStr(uint8_t device_chemistry) {
	switch (device_chemistry) {
		case DEVICE_CHEMISTRY_OTHER:
			return "Other";
		case DEVICE_CHEMISTRY_UNKNOWN:
			return "Unknown";
		case DEVICE_CHEMISTRY_LEAD_ACID:
			return "Lead Acid";
		case DEVICE_CHEMISTRY_NICKEL_CADMIUM:
			return "Nickel Cadmium";
		case DEVICE_CHEMISTRY_NICKEL_METAL_HYDRIDE:
			return "Nickel Metal Hydride";
		case DEVICE_CHEMISTRY_LITHIUM_ION:
			return "Lithium-ion";
		case DEVICE_CHEMISTRY_ZINC_AIR:
			return "Zinc Air";
		case DEVICE_CHEMISTRY_LITHIUM_POLYMER:
			return "Lithium Polymer";
		default:
			return "Undefined";
	}
}

/**
 * @brief Calculates the effective portable-battery design capacity.
 *
 * @param design_capacity Raw design capacity in mWh.
 * @param design_capacity_multiplier SMBIOS 2.2 multiplication factor, or 1 when unavailable.
 * @return Effective design capacity in mWh.
 */
uint32_t lazybiosType22DesignCapacityMWh(uint16_t design_capacity, uint8_t design_capacity_multiplier) {
	return (uint32_t)design_capacity * design_capacity_multiplier;
}

/**
 * @brief Formats an SMBIOS Smart Battery Data Specification manufacture date.
 *
 * @param sbds_manufacture_date Raw packed SBDS manufacture date.
 * @param buf Output buffer that receives the formatted date.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType22SBDSManufactureDateStr(uint16_t sbds_manufacture_date, char* buf, size_t buf_len) {
	uint16_t year = (uint16_t)(1980 + ((sbds_manufacture_date >> 9) & 0x7F));
	uint8_t month = (uint8_t)((sbds_manufacture_date >> 5) & 0x0F);
	uint8_t day = (uint8_t)(sbds_manufacture_date & 0x1F);

	if (month < 1 || month > 12 || day < 1 || day > 31) {
		snprintf(buf, buf_len, "Invalid");
		return;
	}
	snprintf(buf, buf_len, "%04hu-%02hhu-%02hhu", year, month, day);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 22 structures.
 *
 * @param Type22 Type 22 array to release.
 * @param type22_count Number of elements in Type22.
 */
void lazybiosFreeType22(lazybiosType22_t* Type22, size_t type22_count) {
	if (!Type22) return;

	for (size_t i = 0; i < type22_count; i++) {
		free(Type22[i].location);
		free(Type22[i].manufacturer);
		free(Type22[i].manufacture_date);
		free(Type22[i].serial_number);
		free(Type22[i].device_name);
		free(Type22[i].sbds_version_number);
		free(Type22[i].sbds_device_chemistry);
	}
	free(Type22);
}

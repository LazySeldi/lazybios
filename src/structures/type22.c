/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This file is part of lazybios.
 *
 * lazybios is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * lazybios is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lazybios. If not, see <https://www.gnu.org/licenses/>.
 */
/**
 * @file type22.c
 * @brief Implements parsing and decoding for SMBIOS Type 22 Portable Battery.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* File-local decoders; their output is stored in each record's `decoded`. */
static size_t lazybiosType22SBDSManufactureDateStr(uint16_t sbds_manufacture_date, char* buf, size_t buf_len);

/* File-local decoders; their results are stored in each record's `decoded`. */
static inline uint32_t lazybiosType22DesignCapacityMWh(uint16_t design_capacity, uint8_t design_capacity_multiplier);
static inline const char* lazybiosType22DeviceChemistryStr(uint8_t device_chemistry);

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

lazybiosType22Array_t* lazybiosGetType22(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType22Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_PORTABLE_BATTERY);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_PORTABLE_BATTERY].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_PORTABLE_BATTERY].first;
	}
	size_t index = 0;

	if (count == 0) return out;

	out->entries = calloc(count, sizeof(*out->entries));
	if (!out->entries) {
		free(out);
		return NULL;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];
		if (len < SMBIOS_HEADER_SIZE) break;
		const uint8_t* structure_end = DMINext(p, end);

		if (type == SMBIOS_TYPE_PORTABLE_BATTERY) {
			if (index >= count) break;
			lazybiosType22_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

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
				LAZYBIOS_MARK_UNREACHABLE(current, sbds_serial_number);
				current->sbds_manufacture_date = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, sbds_manufacture_date);
				current->sbds_device_chemistry = NULL;
				LAZYBIOS_MARK_UNREACHABLE(current, sbds_device_chemistry);
				current->design_capacity_multiplier = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, design_capacity_multiplier);
				current->oem_specific = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, oem_specific);
			}

			current->decoded.design_capacity = lazybiosType22DesignCapacityMWh(current->design_capacity, current->design_capacity_multiplier);
			current->decoded.device_chemistry = lazybiosType22DeviceChemistryStr(current->device_chemistry);

			char decbuf[LAZYBIOS_DECODER_BUF_SIZE];
			if (LAZYBIOS_FIELD_STATUS(current, sbds_manufacture_date) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType22SBDSManufactureDateStr(current->sbds_manufacture_date, decbuf, sizeof(decbuf));
				current->decoded.sbds_manufacture_date = lazybiosDup(decbuf);
			}

			index++;
		}
		p = structure_end;
	}
	out->count = index;
	return out;
}

static inline const char* lazybiosType22DeviceChemistryStr(uint8_t device_chemistry) {
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

static inline uint32_t lazybiosType22DesignCapacityMWh(uint16_t design_capacity, uint8_t design_capacity_multiplier) {
	return (uint32_t)design_capacity * design_capacity_multiplier;
}

static size_t lazybiosType22SBDSManufactureDateStr(uint16_t sbds_manufacture_date, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	uint16_t year = (uint16_t)(1980 + ((sbds_manufacture_date >> 9) & 0x7F));
	uint8_t month = (uint8_t)((sbds_manufacture_date >> 5) & 0x0F);
	uint8_t day = (uint8_t)(sbds_manufacture_date & 0x1F);

	if (month < 1 || month > 12 || day < 1 || day > 31) {
		snprintf(buf, buf_len, "Invalid");
		return 0;
	}
	snprintf(buf, buf_len, "%04hu-%02hhu-%02hhu", year, month, day);
	return buf ? strlen(buf) : 0;
}

void lazybiosFreeType22(lazybiosType22Array_t* Type22) {
    if (!Type22) return;

	for (size_t i = 0; i < Type22->count; i++) {
		free(Type22->entries[i].decoded.sbds_manufacture_date);
	}

    free(Type22->entries);

    free(Type22);
}

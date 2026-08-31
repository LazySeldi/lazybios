/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file type17.c
 * @brief Implements parsing and decoding for SMBIOS Type 17 Memory Device Information.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* File-local decoders; their output is stored in each record's `decoded`. */
static size_t lazybiosType17TypeDetailStr(uint16_t type_detail, char* buf, size_t buf_len);
static size_t lazybiosType17ExtendedSizeStr(uint32_t extended_size, char* buf, size_t buf_len);
static size_t lazybiosType17OperatingModeCapabilityStr(uint16_t memory_operating_mode_capability, char* buf, size_t buf_len);
static size_t lazybiosType17ModuleManufacturerIDStr(uint16_t id, char* buf, size_t buf_len);
static size_t lazybiosType17NonVolatileSizeStr(uint64_t non_volatile_size, char* buf, size_t buf_len);
static size_t lazybiosType17VolatileSizeStr(uint64_t volatile_size, char* buf, size_t buf_len);
static size_t lazybiosType17CacheSizeStr(uint64_t cache_size, char* buf, size_t buf_len);
static size_t lazybiosType17ExtendedSpeedStr(uint32_t extended_speed, char* buf, size_t buf_len);
static size_t lazybiosType17PMIC0ManufacturerIDStr(uint16_t id, char* buf, size_t buf_len);
static size_t lazybiosType17PMIC0RevisionStr(uint16_t revision, char* buf, size_t buf_len);
static size_t lazybiosType17RCDManufacturerIDStr(uint16_t id, char* buf, size_t buf_len);
static size_t lazybiosType17RCDRevisionStr(uint16_t revision, char* buf, size_t buf_len);

/* File-local decoders; their results are stored in each record's `decoded`. */
static inline const char* lazybiosType17FormFactorStr(uint8_t form_factor);
static inline const char* lazybiosType17MemoryTechnologyStr(uint8_t memory_technology);
static inline const char* lazybiosType17TypeStr(uint8_t memory_type);

// Fields
#define PHYSICAL_MEMORY_ARRAY_HANDLE 0x04
#define MEMORY_ERROR_INFORMATION_HANDLE 0x06
#define TOTAL_WIDTH 0x08
#define DATA_WIDTH 0x0A
#define SIZE 0x0C
#define FORM_FACTOR 0x0E
#define DEVICE_SET 0x0F
#define DEVICE_LOCATOR 0x10
#define BANK_LOCATOR 0x11
#define MEMORY_TYPE 0x12
#define TYPE_DETAIL 0x13
#define SPEED 0x15
#define MANUFACTURER 0x17
#define SERIAL_NUMBER 0x18
#define ASSET_TAG 0x19
#define PART_NUMBER 0x1A
#define ATTRIBUTES 0x1B
#define EXTENDED_SIZE 0x1C
#define CONFIGURED_MEMORY_SPEED 0x20
#define MINIMUM_VOLTAGE 0x22
#define MAXIMUM_VOLTAGE 0x24
#define CONFIGURED_VOLTAGE 0x26
#define MEMORY_TECHNOLOGY 0x28
#define MEMORY_OPERATING_MODE_CAPABILITY 0x29
#define FIRMWARE_VERSION 0x2B
#define MODULE_MANUFACTURER_ID 0x2C
#define MODULE_PRODUCT_ID 0x2E
#define MEMORY_SUBSYSTEM_CONTROLLER_MANUFACTURER_ID 0x30
#define MEMORY_SUBSYSTEM_CONTROLLER_PRODUCT_ID 0x32
#define NON_VOLATILE_SIZE 0x34
#define VOLATILE_SIZE 0x3C
#define CACHE_SIZE 0x44
#define LOGICAL_SIZE 0x4C
#define EXTENDED_SPEED 0x54
#define EXTENDED_CONFIGURED_MEMORY_SPEED 0x58
#define PMIC0_MANUFACTURER_ID 0x5C
#define PMIC0_REVISION_NUMBER 0x5E
#define RCD_MANUFACTURER_ID 0x60
#define RCD_REVISION_NUMBER 0x62


// Form Factor
#define FF_OTHER 0x01
#define FF_UNKNOWN 0x02
#define SIMM 0x03
#define SIP 0x04
#define CHIP 0x05
#define DIP 0x06
#define ZIP 0x07
#define PROPRIETARY_CARD 0x08
#define DIMM 0x09
#define TSOP 0x0A
#define ROW_OF_CHIPS 0x0B
#define RIMM 0x0C
#define SODIMM 0x0D
#define SRIMM 0x0E
#define FB_DIMM 0x0F
#define DIE 0x10
#define CAMM 0x11
#define CUDIMM 0x12
#define CSODIMM 0x13

// Memory Type
#define MT_OTHER 0x01
#define MT_UNKNOWN 0x02
#define MT_DRAM 0x03
#define EDRAM 0x04
#define VRAM 0x05
#define SRAM 0x06
#define RAM 0x07
#define ROM 0x08
#define FLASH 0x09
#define EEPROM 0x0A
#define FEPROM 0x0B
#define EPROM 0x0C
#define CDRAM 0x0D
#define _3DRAM 0x0E // can't start with a number for some reason
#define SDRAM 0x0F
#define SGRAM 0x10
#define RDRAM 0x11
#define DDR 0x12
#define DDR2 0x13
#define DDR2_FB_DIMM 0x14
#define DDR3 0x18
#define FBD2 0x19
#define DDR4 0x1A
#define LPDDR 0x1B
#define LPDDR2 0x1C
#define LPDDR3 0x1D
#define LPDDR4 0x1E
#define LOGICAL_NON_VOLATILE_DEVICE 0x1F
#define HBM 0x20
#define HBM2 0x21
#define DDR5 0x22
#define LPDDR5 0x23
#define HBM3 0x24
#define MRDIMM 0x25

// Memory Technology
#define MTECH_OTHER 0x01
#define MTECH_UNKNOWN 0x02
#define MTECH_DRAM 0x03
#define NVDIMM_N 0x04
#define NVDIMM_F 0x05
#define NVDIMM_P 0x06
#define INTEL_OPTANE_PERSISTENT_MEMORY 0x07
#define MRDIMM_DEPRECATED 0x08

lazybiosType17Array_t* lazybiosGetType17(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType17Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count;
	const uint8_t* p;
	if (DMIData->index_valid != 1) {
	    count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_MEMORY_DEVICE);
	    p = DMIData->dmi_data;
	} else {
	    count = DMIData->index[SMBIOS_TYPE_MEMORY_DEVICE].count;
	    p = DMIData->dmi_data + DMIData->index[SMBIOS_TYPE_MEMORY_DEVICE].first;
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

		if (type == SMBIOS_TYPE_MEMORY_DEVICE) {
			if (index >= count) break;
			lazybiosType17_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			if (lazybiosIsVersionPlus(DMIData, 2, 1)) {
				READU16(current, physical_memory_array_handle, len, PHYSICAL_MEMORY_ARRAY_HANDLE, p);

				READU16(current, memory_error_information_handle, len, MEMORY_ERROR_INFORMATION_HANDLE, p);
				if (current->physical_memory_array_handle == 0xFFFF) {
					LAZYBIOS_MARK_ABSENT(current, physical_memory_array_handle);
				}
				if (current->memory_error_information_handle == 0xFFFE ||
					current->memory_error_information_handle == 0xFFFF) {
					LAZYBIOS_MARK_ABSENT(current, memory_error_information_handle);
				}

				READU16(current, total_width, len, TOTAL_WIDTH, p);

				READU16(current, data_width, len, DATA_WIDTH, p);

				READU16(current, size, len, SIZE, p);

				READU8(current, form_factor, len, FORM_FACTOR, p);
				READU8(current, device_set, len, DEVICE_SET, p);

				READSTR(current, device_locator, len, DEVICE_LOCATOR, p, structure_end);

				READSTR(current, bank_locator, len, BANK_LOCATOR, p, structure_end);

				READU8(current, memory_type, len, MEMORY_TYPE, p);

				READU16(current, type_detail, len, TYPE_DETAIL, p);
			} else {
				current->physical_memory_array_handle = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, physical_memory_array_handle);
				current->memory_error_information_handle = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, memory_error_information_handle);
				current->total_width = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, total_width);
				current->data_width = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, data_width);
				current->size = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, size);
				current->form_factor = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, form_factor);
				current->device_set = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, device_set);
				current->device_locator = NULL;
				LAZYBIOS_MARK_UNREACHABLE(current, device_locator);
				current->bank_locator = NULL;
				LAZYBIOS_MARK_UNREACHABLE(current, bank_locator);
				current->memory_type = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, memory_type);
				current->type_detail = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, type_detail);
			}

			if (lazybiosIsVersionPlus(DMIData, 2, 3)) {
				READU16(current, speed, len, SPEED, p);

				READSTR(current, manufacturer, len, MANUFACTURER, p, structure_end);

				READSTR(current, serial_number, len, SERIAL_NUMBER, p, structure_end);

				READSTR(current, asset_tag, len, ASSET_TAG, p, structure_end);

				READSTR(current, part_number, len, PART_NUMBER, p, structure_end);
			} else {
				current->speed = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, speed);
				current->manufacturer = NULL;
				LAZYBIOS_MARK_UNREACHABLE(current, manufacturer);
				current->serial_number = NULL;
				LAZYBIOS_MARK_UNREACHABLE(current, serial_number);
				current->asset_tag = NULL;
				LAZYBIOS_MARK_UNREACHABLE(current, asset_tag);
				current->part_number = NULL;
				LAZYBIOS_MARK_UNREACHABLE(current, part_number);
			}

			if (lazybiosIsVersionPlus(DMIData, 2, 6)) {
				READU8(current, attributes, len, ATTRIBUTES, p);
			} else {
				current->attributes = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, attributes);
			}

			if (lazybiosIsVersionPlus(DMIData, 2, 7)) {
				READU32(current, extended_size, len, EXTENDED_SIZE, p);

				READU16(current, configured_memory_speed, len, CONFIGURED_MEMORY_SPEED, p);
			} else {
				current->extended_size = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, extended_size);
				current->configured_memory_speed = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, configured_memory_speed);
			}

			if (lazybiosIsVersionPlus(DMIData, 2, 8)) {
				READU16(current, minimum_voltage, len, MINIMUM_VOLTAGE, p);

				READU16(current, maximum_voltage, len, MAXIMUM_VOLTAGE, p);

				READU16(current, configured_voltage, len, CONFIGURED_VOLTAGE, p);
			} else {
				current->minimum_voltage = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, minimum_voltage);
				current->maximum_voltage = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, maximum_voltage);
				current->configured_voltage = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, configured_voltage);
			}

			if (lazybiosIsVersionPlus(DMIData, 3, 2)) {
				READU8(current, memory_technology, len, MEMORY_TECHNOLOGY, p);

				READU16(current, memory_operating_mode_capability, len, MEMORY_OPERATING_MODE_CAPABILITY, p);

				READSTR(current, firmware_version, len, FIRMWARE_VERSION, p, structure_end);

				READU16(current, module_manufacturer_id, len, MODULE_MANUFACTURER_ID, p);

				READU16(current, module_product_id, len, MODULE_PRODUCT_ID, p);

				READU16(current, memory_subsystem_controller_manufacturer_id, len, MEMORY_SUBSYSTEM_CONTROLLER_MANUFACTURER_ID, p);

				READU16(current, memory_subsystem_controller_product_id, len, MEMORY_SUBSYSTEM_CONTROLLER_PRODUCT_ID, p);

				READU64(current, non_volatile_size, len, NON_VOLATILE_SIZE, p);

				READU64(current, volatile_size, len, VOLATILE_SIZE, p);

				READU64(current, cache_size, len, CACHE_SIZE, p);

				READU64(current, logical_size, len, LOGICAL_SIZE, p);
			} else {
				current->memory_technology = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, memory_technology);
				current->memory_operating_mode_capability = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, memory_operating_mode_capability);
				current->firmware_version = NULL;
				LAZYBIOS_MARK_UNREACHABLE(current, firmware_version);
				current->module_manufacturer_id = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, module_manufacturer_id);
				current->module_product_id = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, module_product_id);
				current->memory_subsystem_controller_manufacturer_id = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, memory_subsystem_controller_manufacturer_id);
				current->memory_subsystem_controller_product_id = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, memory_subsystem_controller_product_id);
				current->non_volatile_size = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, non_volatile_size);
				current->volatile_size = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, volatile_size);
				current->cache_size = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, cache_size);
				current->logical_size = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, logical_size);
			}

			if (lazybiosIsVersionPlus(DMIData, 3, 3)) {
				READU32(current, extended_speed, len, EXTENDED_SPEED, p);

				READU32(current, extended_configured_memory_speed, len, EXTENDED_CONFIGURED_MEMORY_SPEED, p);
			} else {
				current->extended_speed = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, extended_speed);
				current->extended_configured_memory_speed = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, extended_configured_memory_speed);
			}

			if (lazybiosIsVersionPlus(DMIData, 3, 7)) {
				READU16(current, pmic0_manufacturer_id, len, PMIC0_MANUFACTURER_ID, p);

				READU16(current, pmic0_revision_number, len, PMIC0_REVISION_NUMBER, p);

				READU16(current, rcd_manufacturer_id, len, RCD_MANUFACTURER_ID, p);

				READU16(current, rcd_revision_number, len, RCD_REVISION_NUMBER, p);
			} else {
				current->pmic0_manufacturer_id = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, pmic0_manufacturer_id);
				current->pmic0_revision_number = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, pmic0_revision_number);
				current->rcd_manufacturer_id = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, rcd_manufacturer_id);
				current->rcd_revision_number = 0;
				LAZYBIOS_MARK_UNREACHABLE(current, rcd_revision_number);
			}

			current->decoded.form_factor = lazybiosType17FormFactorStr(current->form_factor);
			current->decoded.memory_technology = lazybiosType17MemoryTechnologyStr(current->memory_technology);
			current->decoded.memory_type = lazybiosType17TypeStr(current->memory_type);

			char decbuf[LAZYBIOS_DECODER_BUF_SIZE];
			if (LAZYBIOS_FIELD_STATUS(current, type_detail) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType17TypeDetailStr(current->type_detail, decbuf, sizeof(decbuf));
				current->decoded.type_detail = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, extended_size) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType17ExtendedSizeStr(current->extended_size, decbuf, sizeof(decbuf));
				current->decoded.extended_size = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, memory_operating_mode_capability) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType17OperatingModeCapabilityStr(current->memory_operating_mode_capability, decbuf, sizeof(decbuf));
				current->decoded.memory_operating_mode_capability = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, module_manufacturer_id) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType17ModuleManufacturerIDStr(current->module_manufacturer_id, decbuf, sizeof(decbuf));
				current->decoded.module_manufacturer_id = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, non_volatile_size) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType17NonVolatileSizeStr(current->non_volatile_size, decbuf, sizeof(decbuf));
				current->decoded.non_volatile_size = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, volatile_size) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType17VolatileSizeStr(current->volatile_size, decbuf, sizeof(decbuf));
				current->decoded.volatile_size = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, cache_size) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType17CacheSizeStr(current->cache_size, decbuf, sizeof(decbuf));
				current->decoded.cache_size = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, extended_speed) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType17ExtendedSpeedStr(current->extended_speed, decbuf, sizeof(decbuf));
				current->decoded.extended_speed = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, pmic0_manufacturer_id) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType17PMIC0ManufacturerIDStr(current->pmic0_manufacturer_id, decbuf, sizeof(decbuf));
				current->decoded.pmic0_manufacturer_id = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, pmic0_revision_number) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType17PMIC0RevisionStr(current->pmic0_revision_number, decbuf, sizeof(decbuf));
				current->decoded.pmic0_revision_number = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, rcd_manufacturer_id) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType17RCDManufacturerIDStr(current->rcd_manufacturer_id, decbuf, sizeof(decbuf));
				current->decoded.rcd_manufacturer_id = lazybiosDup(decbuf);
			}
			if (LAZYBIOS_FIELD_STATUS(current, rcd_revision_number) == LAZYBIOS_FIELD_PRESENT) {
				lazybiosType17RCDRevisionStr(current->rcd_revision_number, decbuf, sizeof(decbuf));
				current->decoded.rcd_revision_number = lazybiosDup(decbuf);
			}

			index++;
		}
		p = structure_end;
	}
	out->count = index;
	return out;
}

// Form Factor
static inline const char* lazybiosType17FormFactorStr(uint8_t form_factor) {
	switch (form_factor) {
		case FF_OTHER:
			return "Other";
		case FF_UNKNOWN:
			return "Unknown";
		case SIMM:
			return "SIMM";
		case SIP:
			return "SIP";
		case CHIP:
			return "Chip";
		case DIP:
			return "DIP";
		case ZIP:
			return "Zip";
		case PROPRIETARY_CARD:
			return "Proprietary Card";
		case DIMM:
			return "DIMM";
		case TSOP:
			return "TSOP";
		case ROW_OF_CHIPS:
			return "Row of chips";
		case RIMM:
			return "RIMM";
		case SODIMM:
			return "SODIMM";
		case SRIMM:
			return "SRIMM";
		case FB_DIMM:
			return "FB-DIMM";
		case DIE:
			return "Die";
		case CAMM:
			return "CAMM";
		case CUDIMM:
			return "CUDIMM";
		case CSODIMM:
			return "CSODIMM";
		default:
			return "Unknown Form Factor";
	}
}

// Memory Type
static inline const char* lazybiosType17TypeStr(uint8_t memory_type) {
	switch (memory_type) {
		case MT_OTHER:
			return "Other";
		case MT_UNKNOWN:
			return "Unknown";
		case MT_DRAM:
			return "DRAM";
		case EDRAM:
			return "EDRAM";
		case VRAM:
			return "VRAM";
		case SRAM:
			return "SRAM";
		case RAM:
			return "RAM";
		case ROM:
			return "ROM";
		case FLASH:
			return "FLASH";
		case EEPROM:
			return "EEPROM";
		case FEPROM:
			return "FEPROM";
		case EPROM:
			return "EPROM";
		case CDRAM:
			return "CDRAM";
		case _3DRAM:
			return "3DRAM";
		case SDRAM:
			return "SDRAM";
		case SGRAM:
			return "SGRAM";
		case RDRAM:
			return "RDRAM";
		case DDR:
			return "DDR";
		case DDR2:
			return "DDR2";
		case DDR2_FB_DIMM:
			return "DDR2 FB-DIMM";
		case DDR3:
			return "DDR3";
		case FBD2:
			return "FBD2";
		case DDR4:
			return "DDR4";
		case LPDDR:
			return "LPDDR";
		case LPDDR2:
			return "LPDDR2";
		case LPDDR3:
			return "LPDDR3";
		case LPDDR4:
			return "LPDDR4";
		case LOGICAL_NON_VOLATILE_DEVICE:
			return "Logical non-volatile device";
		case HBM:
			return "HBM (High Bandwidth Memory)";
		case HBM2:
			return "HBM2 (High Bandwidth Memory Generation 2)";
		case DDR5:
			return "DDR5";
		case LPDDR5:
			return "LPDDR5";
		case HBM3:
			return "HBM3 (High Bandwidth Memory Generation 3)";
		case MRDIMM:
			return "MRDIMM";
		default:
			return "Unknown";
	}
}

// Type Detail
static size_t lazybiosType17TypeDetailStr(uint16_t type_detail, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	size_t len = 0;
	buf[0] = '\0';

	if (type_detail & (1 << 1)) lazybiosDecoderAppend(buf, buf_len, &len, "Other, ");
	if (type_detail & (1 << 2)) lazybiosDecoderAppend(buf, buf_len, &len, "Unknown, ");
	if (type_detail & (1 << 3)) lazybiosDecoderAppend(buf, buf_len, &len, "Fast-paged, ");
	if (type_detail & (1 << 4)) lazybiosDecoderAppend(buf, buf_len, &len, "Static column, ");
	if (type_detail & (1 << 5)) lazybiosDecoderAppend(buf, buf_len, &len, "Pseudo-static, ");
	if (type_detail & (1 << 6)) lazybiosDecoderAppend(buf, buf_len, &len, "RAMBUS, ");
	if (type_detail & (1 << 7)) lazybiosDecoderAppend(buf, buf_len, &len, "Synchronous, ");
	if (type_detail & (1 << 8)) lazybiosDecoderAppend(buf, buf_len, &len, "CMOS, ");
	if (type_detail & (1 << 9)) lazybiosDecoderAppend(buf, buf_len, &len, "EDO, ");
	if (type_detail & (1 << 10)) lazybiosDecoderAppend(buf, buf_len, &len, "Window DRAM, ");
	if (type_detail & (1 << 11)) lazybiosDecoderAppend(buf, buf_len, &len, "Cache DRAM, ");
	if (type_detail & (1 << 12)) lazybiosDecoderAppend(buf, buf_len, &len, "Non-volatile, ");
	if (type_detail & (1 << 13)) lazybiosDecoderAppend(buf, buf_len, &len, "Registered (Buffered), ");
	if (type_detail & (1 << 14)) lazybiosDecoderAppend(buf, buf_len, &len, "Unbuffered (Unregistered), ");
	if (type_detail & (1 << 15)) lazybiosDecoderAppend(buf, buf_len, &len, "LRDIMM, ");

	// Remove trailing comma and space if any text was appended
	if (len > 2) {
		buf[len - 2] = '\0';
	} else {
		snprintf(buf, buf_len, "None");
	}
	return buf ? strlen(buf) : 0;
}

// Extended Size
static size_t lazybiosType17ExtendedSizeStr(uint32_t extended_size, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	if (extended_size == 0) {
		snprintf(buf, buf_len, "Not used (Size field applies)");
		return 0;
	}

	// Bit 31 is reserved, must be set to 0.
	// Bits 30:0 represent the size of the memory device in megabytes (MiB).
	uint32_t size_mib = extended_size & 0x7FFFFFFF;

	snprintf(buf, buf_len, "%u MiB", size_mib);
	return buf ? strlen(buf) : 0;
}

// Memory Technology
static inline const char* lazybiosType17MemoryTechnologyStr(uint8_t memory_technology) {
	switch (memory_technology) {
		case MTECH_OTHER:
			return "Other";
		case MTECH_UNKNOWN:
			return "Unknown";
		case MTECH_DRAM:
			return "DRAM";
		case NVDIMM_N:
			return "NVDIMM-N";
		case NVDIMM_F:
			return "NVDIMM-F";
		case NVDIMM_P:
			return "NVDIMM-P";
		case INTEL_OPTANE_PERSISTENT_MEMORY:
			return "Intel Optane persistent memory";
		case MRDIMM_DEPRECATED:
			return "MRDIMM (Deprecated). This value has been deprecated from this table and moved to Memory Device - Type, subclause 7.18.2";
		default:
			return "Unknown";
	}
}

// Memory Operating Mode Capability
static size_t lazybiosType17OperatingModeCapabilityStr(uint16_t memory_operating_mode_capability, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	size_t len = 0;
	buf[0] = '\0';

	if (memory_operating_mode_capability & (1 << 1)) lazybiosDecoderAppend(buf, buf_len, &len, "Other, ");
	if (memory_operating_mode_capability & (1 << 2)) lazybiosDecoderAppend(buf, buf_len, &len, "Unknown, ");
	if (memory_operating_mode_capability & (1 << 3)) lazybiosDecoderAppend(buf, buf_len, &len, "Volatile memory, ");
	if (memory_operating_mode_capability & (1 << 4)) lazybiosDecoderAppend(buf, buf_len, &len, "Byte-accessible persistent memory, ");
	if (memory_operating_mode_capability & (1 << 5)) lazybiosDecoderAppend(buf, buf_len, &len, "Block-accessible persistent memory, ");

	if (len > 2) {
		buf[len - 2] = '\0';
	} else {
		snprintf(buf, buf_len, "None");
	}
	return buf ? strlen(buf) : 0;
}

// Module Manufacturers IDs
static size_t lazybiosType17ModuleManufacturerIDStr(uint16_t id, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	if (id == 0x0000) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "0x%04X", id);
	}
	return buf ? strlen(buf) : 0;
}

// Volatile Size
static size_t lazybiosType17VolatileSizeStr(uint64_t volatile_size, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	if (volatile_size == 0xFFFFFFFFFFFFFFFFULL) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "%llu", (unsigned long long)volatile_size);
	}
	return buf ? strlen(buf) : 0;
}

// Non-Volatile Size
static size_t lazybiosType17NonVolatileSizeStr(uint64_t non_volatile_size, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	if (non_volatile_size == 0xFFFFFFFFFFFFFFFFULL) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "%llu", (unsigned long long)non_volatile_size);
	}
	return buf ? strlen(buf) : 0;
}

// Cache Size
static size_t lazybiosType17CacheSizeStr(uint64_t cache_size, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	if (cache_size == 0xFFFFFFFFFFFFFFFFULL) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "%llu", (unsigned long long)cache_size);
	}
	return buf ? strlen(buf) : 0;
}

// Extended Speed
static size_t lazybiosType17ExtendedSpeedStr(uint32_t extended_speed, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	// Bit 31 is reserved, must be set to 0.
	// Bits 30:0 represent the speed in MT/s.
	uint32_t speed_mts = extended_speed & 0x7FFFFFFF;

	if (speed_mts == 0) {
		snprintf(buf, buf_len, "Not used (Speed field applies)");
	} else {
		snprintf(buf, buf_len, "%u", speed_mts); //  MT/s should be used for this field. Versions before 3.0.0 used MHz
	}
	return buf ? strlen(buf) : 0;
}

// PMIC0 Manufacturer ID
static size_t lazybiosType17PMIC0ManufacturerIDStr(uint16_t id, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	if (id == 0x0000) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "0x%04X", id);
	}
	return buf ? strlen(buf) : 0;
}

// PMIC0 Revision
static size_t lazybiosType17PMIC0RevisionStr(uint16_t revision, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	if (revision == 0xFF00) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "0x%04X", revision);
	}
	return buf ? strlen(buf) : 0;
}

// RCD Manufacturer ID
static size_t lazybiosType17RCDManufacturerIDStr(uint16_t id, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	if (id == 0x0000) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "0x%04X", id);
	}
	return buf ? strlen(buf) : 0;
}

// RCD Revision Number
static size_t lazybiosType17RCDRevisionStr(uint16_t revision, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	if (revision == 0xFF00) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "0x%04X", revision);
	}
	return buf ? strlen(buf) : 0;
}

void lazybiosFreeType17(lazybiosType17Array_t* Type17) {
    if (!Type17) return;

	for (size_t i = 0; i < Type17->count; i++) {
		free(Type17->entries[i].decoded.type_detail);
		free(Type17->entries[i].decoded.extended_size);
		free(Type17->entries[i].decoded.memory_operating_mode_capability);
		free(Type17->entries[i].decoded.module_manufacturer_id);
		free(Type17->entries[i].decoded.non_volatile_size);
		free(Type17->entries[i].decoded.volatile_size);
		free(Type17->entries[i].decoded.cache_size);
		free(Type17->entries[i].decoded.extended_speed);
		free(Type17->entries[i].decoded.pmic0_manufacturer_id);
		free(Type17->entries[i].decoded.pmic0_revision_number);
		free(Type17->entries[i].decoded.rcd_manufacturer_id);
		free(Type17->entries[i].decoded.rcd_revision_number);
	}

    free(Type17->entries);

    free(Type17);
}

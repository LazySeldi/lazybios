//
// Type 17 ( Memory Device )
//

#include "lazybios.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
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

// Decoders

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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

lazybiosType17_t* lazybiosGetType17(lazybiosType17_t* Type17, size_t* type17_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return LAZYBIOS_NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_MEMORY_DEVICE);
	size_t index = 0;
	Type17 = calloc(count, sizeof(lazybiosType17_t));
	if (!Type17) return LAZYBIOS_NULL;
	if (count == 0) {
		*type17_count = 0;
		return Type17;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_MEMORY_DEVICE) {
			if (index >= count) break;
			lazybiosType17_t* current = &Type17[index];

			if (ISVERPLUS(DMIData, 2, 1)) {
				if (len >= PHYSICAL_MEMORY_ARRAY_HANDLE + sizeof(uint16_t)) {
					memcpy(&current->physical_memory_array_handle, p + PHYSICAL_MEMORY_ARRAY_HANDLE, sizeof(uint16_t));
				} else {
					current->physical_memory_array_handle = LAZYBIOS_NOT_FOUND_U16;
				}

				if (len >= MEMORY_ERROR_INFORMATION_HANDLE + sizeof(uint16_t)) {
					memcpy(&current->memory_error_information_handle, p + MEMORY_ERROR_INFORMATION_HANDLE, sizeof(uint16_t));
				} else {
					current->memory_error_information_handle = LAZYBIOS_NOT_FOUND_U16;
				}

				if (len >= TOTAL_WIDTH + sizeof(uint16_t)) {
					memcpy(&current->total_width, p + TOTAL_WIDTH, sizeof(uint16_t));
				} else {
					current->total_width = LAZYBIOS_NOT_FOUND_U16;
				}

				if (len >= DATA_WIDTH + sizeof(uint16_t)) {
					memcpy(&current->data_width, p + DATA_WIDTH, sizeof(uint16_t));
				} else {
					current->data_width = LAZYBIOS_NOT_FOUND_U16;
				}

				if (len >= SIZE + sizeof(uint16_t)) {
					memcpy(&current->size, p + SIZE, sizeof(uint16_t));
				} else {
					current->size = LAZYBIOS_NOT_FOUND_U16;
				}

				current->form_factor = (len > FORM_FACTOR) ? p[FORM_FACTOR] : LAZYBIOS_NOT_FOUND_U8;
				current->device_set = (len > DEVICE_SET) ? p[DEVICE_SET] : LAZYBIOS_NOT_FOUND_U8;

				if (len > DEVICE_LOCATOR) current->device_locator = DMIString(p, len, p[DEVICE_LOCATOR], end);
				if (!current->device_locator) current->device_locator = strdup(LAZYBIOS_NOT_FOUND_STR);

				if (len > BANK_LOCATOR) current->bank_locator = DMIString(p, len, p[BANK_LOCATOR], end);
				if (!current->bank_locator) current->bank_locator = strdup(LAZYBIOS_NOT_FOUND_STR);

				current->memory_type = (len > MEMORY_TYPE) ? p[MEMORY_TYPE] : LAZYBIOS_NOT_FOUND_U8;

				if (len >= TYPE_DETAIL + sizeof(uint16_t)) {
					memcpy(&current->type_detail, p + TYPE_DETAIL, sizeof(uint16_t));
				} else {
					current->type_detail = LAZYBIOS_NOT_FOUND_U16;
				}
			} else {
				current->physical_memory_array_handle = LAZYBIOS_NOT_FOUND_U16;
				current->memory_error_information_handle = LAZYBIOS_NOT_FOUND_U16;
				current->total_width = LAZYBIOS_NOT_FOUND_U16;
				current->data_width = LAZYBIOS_NOT_FOUND_U16;
				current->size = LAZYBIOS_NOT_FOUND_U16;
				current->form_factor = LAZYBIOS_NOT_FOUND_U8;
				current->device_set = LAZYBIOS_NOT_FOUND_U8;
				current->device_locator = strdup(LAZYBIOS_NOT_FOUND_STR);
				current->bank_locator = strdup(LAZYBIOS_NOT_FOUND_STR);
				current->memory_type = LAZYBIOS_NOT_FOUND_U8;
				current->type_detail = LAZYBIOS_NOT_FOUND_U16;
			}

			if (ISVERPLUS(DMIData, 2, 3)) {
				if (len >= SPEED + sizeof(uint16_t)) {
					memcpy(&current->speed, p + SPEED, sizeof(uint16_t));
				} else {
					current->speed = LAZYBIOS_NOT_FOUND_U16;
				}

				if (len > MANUFACTURER) current->manufacturer = DMIString(p, len, p[MANUFACTURER], end);
				if (!current->manufacturer) current->manufacturer = strdup(LAZYBIOS_NOT_FOUND_STR);

				if (len > SERIAL_NUMBER) current->serial_number = DMIString(p, len, p[SERIAL_NUMBER], end);
				if (!current->serial_number) current->serial_number = strdup(LAZYBIOS_NOT_FOUND_STR);

				if (len > ASSET_TAG) current->asset_tag = DMIString(p, len, p[ASSET_TAG], end);
				if (!current->asset_tag) current->asset_tag = strdup(LAZYBIOS_NOT_FOUND_STR);

				if (len > PART_NUMBER) current->part_number = DMIString(p, len, p[PART_NUMBER], end);
				if (!current->part_number) current->part_number = strdup(LAZYBIOS_NOT_FOUND_STR);
			} else {
				current->speed = LAZYBIOS_NOT_FOUND_U16;
				current->manufacturer = strdup(LAZYBIOS_NOT_FOUND_STR);
				current->serial_number = strdup(LAZYBIOS_NOT_FOUND_STR);
				current->asset_tag = strdup(LAZYBIOS_NOT_FOUND_STR);
				current->part_number = strdup(LAZYBIOS_NOT_FOUND_STR);
			}

			if (ISVERPLUS(DMIData, 2, 6)) {
				current->attributes = (len > ATTRIBUTES) ? p[ATTRIBUTES] : LAZYBIOS_NOT_FOUND_U8;
			} else {
				current->attributes = LAZYBIOS_NOT_FOUND_U8;
			}

			if (ISVERPLUS(DMIData, 2, 7)) {
				if (len >= EXTENDED_SIZE + sizeof(uint32_t)) {
					memcpy(&current->extended_size, p + EXTENDED_SIZE, sizeof(uint32_t));
				} else {
					current->extended_size = LAZYBIOS_NOT_FOUND_U32;
				}

				if (len >= CONFIGURED_MEMORY_SPEED + sizeof(uint16_t)) {
					memcpy(&current->configured_memory_speed, p + CONFIGURED_MEMORY_SPEED, sizeof(uint16_t));
				} else {
					current->configured_memory_speed = LAZYBIOS_NOT_FOUND_U16;
				}
			} else {
				current->extended_size = LAZYBIOS_NOT_FOUND_U32;
				current->configured_memory_speed = LAZYBIOS_NOT_FOUND_U16;
			}

			if (ISVERPLUS(DMIData, 2, 8)) {
				if (len >= MINIMUM_VOLTAGE + sizeof(uint16_t)) {
					memcpy(&current->minimum_voltage, p + MINIMUM_VOLTAGE, sizeof(uint16_t));
				} else {
					current->minimum_voltage = LAZYBIOS_NOT_FOUND_U16;
				}

				if (len >= MAXIMUM_VOLTAGE + sizeof(uint16_t)) {
					memcpy(&current->maximum_voltage, p + MAXIMUM_VOLTAGE, sizeof(uint16_t));
				} else {
					current->maximum_voltage = LAZYBIOS_NOT_FOUND_U16;
				}

				if (len >= CONFIGURED_VOLTAGE + sizeof(uint16_t)) {
					memcpy(&current->configured_voltage, p + CONFIGURED_VOLTAGE, sizeof(uint16_t));
				} else {
					current->configured_voltage = LAZYBIOS_NOT_FOUND_U16;
				}
			} else {
				current->minimum_voltage = LAZYBIOS_NOT_FOUND_U16;
				current->maximum_voltage = LAZYBIOS_NOT_FOUND_U16;
				current->configured_voltage = LAZYBIOS_NOT_FOUND_U16;
			}

			if (ISVERPLUS(DMIData, 3, 2)) {
				current->memory_technology = (len > MEMORY_TECHNOLOGY) ? p[MEMORY_TECHNOLOGY] : LAZYBIOS_NOT_FOUND_U8;

				if (len >= MEMORY_OPERATING_MODE_CAPABILITY + sizeof(uint16_t)) {
					memcpy(&current->memory_operating_mode_capability, p + MEMORY_OPERATING_MODE_CAPABILITY, sizeof(uint16_t));
				} else {
					current->memory_operating_mode_capability = LAZYBIOS_NOT_FOUND_U16;
				}

				if (len > FIRMWARE_VERSION) current->firmware_version = DMIString(p, len, p[FIRMWARE_VERSION], end);
				if (!current->firmware_version) current->firmware_version = strdup(LAZYBIOS_NOT_FOUND_STR);

				if (len >= MODULE_MANUFACTURER_ID + sizeof(uint16_t)) {
					memcpy(&current->module_manufacturer_id, p + MODULE_MANUFACTURER_ID, sizeof(uint16_t));
				} else {
					current->module_manufacturer_id = LAZYBIOS_NOT_FOUND_U16;
				}

				if (len >= MODULE_PRODUCT_ID + sizeof(uint16_t)) {
					memcpy(&current->module_product_id, p + MODULE_PRODUCT_ID, sizeof(uint16_t));
				} else {
					current->module_product_id = LAZYBIOS_NOT_FOUND_U16;
				}

				if (len >= MEMORY_SUBSYSTEM_CONTROLLER_MANUFACTURER_ID + sizeof(uint16_t)) {
					memcpy(&current->memory_subsystem_controller_manufacturer_id, p + MEMORY_SUBSYSTEM_CONTROLLER_MANUFACTURER_ID, sizeof(uint16_t));
				} else {
					current->memory_subsystem_controller_manufacturer_id = LAZYBIOS_NOT_FOUND_U16;
				}

				if (len >= MEMORY_SUBSYSTEM_CONTROLLER_PRODUCT_ID + sizeof(uint16_t)) {
					memcpy(&current->memory_subsystem_controller_product_id, p + MEMORY_SUBSYSTEM_CONTROLLER_PRODUCT_ID, sizeof(uint16_t));
				} else {
					current->memory_subsystem_controller_product_id = LAZYBIOS_NOT_FOUND_U16;
				}

				if (len >= NON_VOLATILE_SIZE + sizeof(uint64_t)) {
					memcpy(&current->non_volatile_size, p + NON_VOLATILE_SIZE, sizeof(uint64_t));
				} else {
					current->non_volatile_size = LAZYBIOS_NOT_FOUND_U64;
				}

				if (len >= VOLATILE_SIZE + sizeof(uint64_t)) {
					memcpy(&current->volatile_size, p + VOLATILE_SIZE, sizeof(uint64_t));
				} else {
					current->volatile_size = LAZYBIOS_NOT_FOUND_U64;
				}

				if (len >= CACHE_SIZE + sizeof(uint64_t)) {
					memcpy(&current->cache_size, p + CACHE_SIZE, sizeof(uint64_t));
				} else {
					current->cache_size = LAZYBIOS_NOT_FOUND_U64;
				}

				if (len >= LOGICAL_SIZE + sizeof(uint64_t)) {
					memcpy(&current->logical_size, p + LOGICAL_SIZE, sizeof(uint64_t));
				} else {
					current->logical_size = LAZYBIOS_NOT_FOUND_U64;
				}
			} else {
				current->memory_technology = LAZYBIOS_NOT_FOUND_U8;
				current->memory_operating_mode_capability = LAZYBIOS_NOT_FOUND_U16;
				current->firmware_version = strdup(LAZYBIOS_NOT_FOUND_STR);
				current->module_manufacturer_id = LAZYBIOS_NOT_FOUND_U16;
				current->module_product_id = LAZYBIOS_NOT_FOUND_U16;
				current->memory_subsystem_controller_manufacturer_id = LAZYBIOS_NOT_FOUND_U16;
				current->memory_subsystem_controller_product_id = LAZYBIOS_NOT_FOUND_U16;
				current->non_volatile_size = LAZYBIOS_NOT_FOUND_U64;
				current->volatile_size = LAZYBIOS_NOT_FOUND_U64;
				current->cache_size = LAZYBIOS_NOT_FOUND_U64;
				current->logical_size = LAZYBIOS_NOT_FOUND_U64;
			}

			if (ISVERPLUS(DMIData, 3, 3)) {
				if (len >= EXTENDED_SPEED + sizeof(uint32_t)) {
					memcpy(&current->extended_speed, p + EXTENDED_SPEED, sizeof(uint32_t));
				} else {
					current->extended_speed = LAZYBIOS_NOT_FOUND_U32;
				}

				if (len >= EXTENDED_CONFIGURED_MEMORY_SPEED + sizeof(uint32_t)) {
					memcpy(&current->extended_configured_memory_speed, p + EXTENDED_CONFIGURED_MEMORY_SPEED, sizeof(uint32_t));
				} else {
					current->extended_configured_memory_speed = LAZYBIOS_NOT_FOUND_U32;
				}
			} else {
				current->extended_speed = LAZYBIOS_NOT_FOUND_U32;
				current->extended_configured_memory_speed = LAZYBIOS_NOT_FOUND_U32;
			}

			if (ISVERPLUS(DMIData, 3, 7)) {
				if (len >= PMIC0_MANUFACTURER_ID + sizeof(uint16_t)) {
					memcpy(&current->pmic0_manufacturer_id, p + PMIC0_MANUFACTURER_ID, sizeof(uint16_t));
				} else {
					current->pmic0_manufacturer_id = LAZYBIOS_NOT_FOUND_U16;
				}

				if (len >= PMIC0_REVISION_NUMBER + sizeof(uint16_t)) {
					memcpy(&current->pmic0_revision_number, p + PMIC0_REVISION_NUMBER, sizeof(uint16_t));
				} else {
					current->pmic0_revision_number = LAZYBIOS_NOT_FOUND_U16;
				}

				if (len >= RCD_MANUFACTURER_ID + sizeof(uint16_t)) {
					memcpy(&current->rcd_manufacturer_id, p + RCD_MANUFACTURER_ID, sizeof(uint16_t));
				} else {
					current->rcd_manufacturer_id = LAZYBIOS_NOT_FOUND_U16;
				}

				if (len >= RCD_REVISION_NUMBER + sizeof(uint16_t)) {
					memcpy(&current->rcd_revision_number, p + RCD_REVISION_NUMBER, sizeof(uint16_t));
				} else {
					current->rcd_revision_number = LAZYBIOS_NOT_FOUND_U16;
				}
			} else {
				current->pmic0_manufacturer_id = LAZYBIOS_NOT_FOUND_U16;
				current->pmic0_revision_number = LAZYBIOS_NOT_FOUND_U16;
				current->rcd_manufacturer_id = LAZYBIOS_NOT_FOUND_U16;
				current->rcd_revision_number = LAZYBIOS_NOT_FOUND_U16;
			}

			index++;
		}
		p = DMINext(p, end);
	}
	*type17_count = index;
	return Type17;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders

// Form Factor
const char* lazybiosType17FormFactorStr(uint8_t form_factor) {
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
const char* lazybiosType17TypeStr(uint8_t memory_type) {
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
void lazybiosType17TypeDetailStr(uint16_t type_detail, char* buf, size_t buf_len) {
	size_t len = 0;
	buf[0] = '\0';

	if (type_detail & (1 << 1)) len += snprintf(buf + len, buf_len - len, "Other, ");
	if (type_detail & (1 << 2)) len += snprintf(buf + len, buf_len - len, "Unknown, ");
	if (type_detail & (1 << 3)) len += snprintf(buf + len, buf_len - len, "Fast-paged, ");
	if (type_detail & (1 << 4)) len += snprintf(buf + len, buf_len - len, "Static column, ");
	if (type_detail & (1 << 5)) len += snprintf(buf + len, buf_len - len, "Pseudo-static, ");
	if (type_detail & (1 << 6)) len += snprintf(buf + len, buf_len - len, "RAMBUS, ");
	if (type_detail & (1 << 7)) len += snprintf(buf + len, buf_len - len, "Synchronous, ");
	if (type_detail & (1 << 8)) len += snprintf(buf + len, buf_len - len, "CMOS, ");
	if (type_detail & (1 << 9)) len += snprintf(buf + len, buf_len - len, "EDO, ");
	if (type_detail & (1 << 10)) len += snprintf(buf + len, buf_len - len, "Window DRAM, ");
	if (type_detail & (1 << 11)) len += snprintf(buf + len, buf_len - len, "Cache DRAM, ");
	if (type_detail & (1 << 12)) len += snprintf(buf + len, buf_len - len, "Non-volatile, ");
	if (type_detail & (1 << 13)) len += snprintf(buf + len, buf_len - len, "Registered (Buffered), ");
	if (type_detail & (1 << 14)) len += snprintf(buf + len, buf_len - len, "Unbuffered (Unregistered), ");
	if (type_detail & (1 << 15)) len += snprintf(buf + len, buf_len - len, "LRDIMM, ");

	// Remove trailing comma and space if any text was appended
	if (len > 2) {
		buf[len - 2] = '\0';
	} else {
		snprintf(buf, buf_len, "None");
	}
}

// Extended Size
void lazybiosType17ExtendedSizeStr(uint32_t extended_size, char* buf, size_t buf_len) {
	if (extended_size == 0) {
		snprintf(buf, buf_len, "Not used (Size field applies)");
		return;
	}

	// Bit 31 is reserved, must be set to 0.
	// Bits 30:0 represent the size of the memory device in megabytes (MiB).
	uint32_t size_mib = extended_size & 0x7FFFFFFF;

	snprintf(buf, buf_len, "%u MiB", size_mib);
}

// Memory Technology
const char* lazybiosType17MemoryTechnologyStr(uint8_t memory_technology) {
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
void lazybiosType17OperatingModeCapabilityStr(uint16_t memory_operating_mode_capability, char* buf, size_t buf_len) {
	size_t len = 0;
	buf[0] = '\0';

	if (memory_operating_mode_capability & (1 << 1)) len += snprintf(buf + len, buf_len - len, "Other, ");
	if (memory_operating_mode_capability & (1 << 2)) len += snprintf(buf + len, buf_len - len, "Unknown, ");
	if (memory_operating_mode_capability & (1 << 3)) len += snprintf(buf + len, buf_len - len, "Volatile memory, ");
	if (memory_operating_mode_capability & (1 << 4)) len += snprintf(buf + len, buf_len - len, "Byte-accessible persistent memory, ");
	if (memory_operating_mode_capability & (1 << 5)) len += snprintf(buf + len, buf_len - len, "Block-accessible persistent memory, ");

	if (len > 2) {
		buf[len - 2] = '\0';
	} else {
		snprintf(buf, buf_len, "None");
	}
}

// Module Manufacturers IDs
void lazybiosType17ModuleManufacturerIDStr(uint16_t id, char* buf, size_t buf_len) {
	if (id == 0x0000) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "0x%04X", id);
	}
}

// Volatile Size
void lazybiosType17VolatileSizeStr(uint64_t volatile_size, char* buf, size_t buf_len) {
	if (volatile_size == 0xFFFFFFFFFFFFFFFFULL) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "%llu", (unsigned long long)volatile_size);
	}
}

// Non-Volatile Size
void lazybiosType17NonVolatileSizeStr(uint64_t non_volatile_size, char* buf, size_t buf_len) {
	if (non_volatile_size == 0xFFFFFFFFFFFFFFFFULL) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "%llu", (unsigned long long)non_volatile_size);
	}
}

// Cache Size
void lazybiosType17CacheSizeStr(uint64_t cache_size, char* buf, size_t buf_len) {
	if (cache_size == 0xFFFFFFFFFFFFFFFFULL) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "%llu", (unsigned long long)cache_size);
	}
}

// Extended Speed
void lazybiosType17ExtendedSpeedStr(uint32_t extended_speed, char* buf, size_t buf_len) {
	// Bit 31 is reserved, must be set to 0.
	// Bits 30:0 represent the speed in MT/s.
	uint32_t speed_mts = extended_speed & 0x7FFFFFFF;

	if (speed_mts == 0) {
		snprintf(buf, buf_len, "Not used (Speed field applies)");
	} else {
		snprintf(buf, buf_len, "%u", speed_mts); //  MT/s should be used for this field. Versions below 3.0.0 used MHz
	}
}

// PMIC0 Manufacturer ID
void lazybiosType17PMIC0ManufacturerIDStr(uint16_t id, char* buf, size_t buf_len) {
	if (id == 0x0000) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "0x%04X", id);
	}
}

// PMIC0 Revision
void lazybiosType17PMIC0RevisionStr(uint16_t revision, char* buf, size_t buf_len) {
	if (revision == 0xFF00) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "0x%04X", revision);
	}
}

// RCD Manufacturer ID
void lazybiosType17RCDManufacturerIDStr(uint16_t id, char* buf, size_t buf_len) {
	if (id == 0x0000) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "0x%04X", id);
	}
}

// RCD Revision Number
void lazybiosType17RCDRevisionStr(uint16_t revision, char* buf, size_t buf_len) {
	if (revision == 0xFF00) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "0x%04X", revision);
	}
}

// Free Function
void lazybiosFreeType17(lazybiosType17_t* Type17, size_t type17_count) {
	if (!Type17) return;

	for (size_t i = 0; i < type17_count; i++) {
		free(Type17[i].device_locator);
		free(Type17[i].bank_locator);
		free(Type17[i].manufacturer);
		free(Type17[i].serial_number);
		free(Type17[i].asset_tag);
		free(Type17[i].part_number);
		free(Type17[i].firmware_version);
	}
	free(Type17);
}
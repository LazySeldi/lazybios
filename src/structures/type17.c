/**
 * @file type17.c
 * @brief Implements parsing and decoding for SMBIOS Type 17 Memory Device Information.
 * @author LazySeldi
 */

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

/**
 * @brief Parses all SMBIOS Type 17 Memory Device structures.
 *
 * @param Type17 Existing Type 17 array pointer value; it is not dereferenced or released.
 * @param type17_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 17 array, or NULL on failure.
 */
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
				READU16(current->physical_memory_array_handle, len, PHYSICAL_MEMORY_ARRAY_HANDLE, p);

				READU16(current->memory_error_information_handle, len, MEMORY_ERROR_INFORMATION_HANDLE, p);

				READU16(current->total_width, len, TOTAL_WIDTH, p);

				READU16(current->data_width, len, DATA_WIDTH, p);

				READU16(current->size, len, SIZE, p);

				READU8(current->form_factor, len, FORM_FACTOR, p)
				READU8(current->device_set, len, DEVICE_SET, p)

				READSTR(len, DEVICE_LOCATOR, current->device_locator, p, end);

				READSTR(len, BANK_LOCATOR, current->bank_locator, p, end);

				READU8(current->memory_type, len, MEMORY_TYPE, p)

				READU16(current->type_detail, len, TYPE_DETAIL, p);
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
				READU16(current->speed, len, SPEED, p);

				READSTR(len, MANUFACTURER, current->manufacturer, p, end);

				READSTR(len, SERIAL_NUMBER, current->serial_number, p, end);

				READSTR(len, ASSET_TAG, current->asset_tag, p, end);

				READSTR(len, PART_NUMBER, current->part_number, p, end);
			} else {
				current->speed = LAZYBIOS_NOT_FOUND_U16;
				current->manufacturer = strdup(LAZYBIOS_NOT_FOUND_STR);
				current->serial_number = strdup(LAZYBIOS_NOT_FOUND_STR);
				current->asset_tag = strdup(LAZYBIOS_NOT_FOUND_STR);
				current->part_number = strdup(LAZYBIOS_NOT_FOUND_STR);
			}

			if (ISVERPLUS(DMIData, 2, 6)) {
				READU8(current->attributes, len, ATTRIBUTES, p)
			} else {
				current->attributes = LAZYBIOS_NOT_FOUND_U8;
			}

			if (ISVERPLUS(DMIData, 2, 7)) {
				READU32(current->extended_size, len, EXTENDED_SIZE, p)

				READU16(current->configured_memory_speed, len, CONFIGURED_MEMORY_SPEED, p);
			} else {
				current->extended_size = LAZYBIOS_NOT_FOUND_U32;
				current->configured_memory_speed = LAZYBIOS_NOT_FOUND_U16;
			}

			if (ISVERPLUS(DMIData, 2, 8)) {
				READU16(current->minimum_voltage, len, MINIMUM_VOLTAGE, p);

				READU16(current->maximum_voltage, len, MAXIMUM_VOLTAGE, p);

				READU16(current->configured_voltage, len, CONFIGURED_VOLTAGE, p);
			} else {
				current->minimum_voltage = LAZYBIOS_NOT_FOUND_U16;
				current->maximum_voltage = LAZYBIOS_NOT_FOUND_U16;
				current->configured_voltage = LAZYBIOS_NOT_FOUND_U16;
			}

			if (ISVERPLUS(DMIData, 3, 2)) {
				READU8(current->memory_technology, len, MEMORY_TECHNOLOGY, p)

				READU16(current->memory_operating_mode_capability, len, MEMORY_OPERATING_MODE_CAPABILITY, p);

				READSTR(len, FIRMWARE_VERSION, current->firmware_version, p, end);

				READU16(current->module_manufacturer_id, len, MODULE_MANUFACTURER_ID, p);

				READU16(current->module_product_id, len, MODULE_PRODUCT_ID, p);

				READU16(current->memory_subsystem_controller_manufacturer_id, len, MEMORY_SUBSYSTEM_CONTROLLER_MANUFACTURER_ID, p);

				READU16(current->memory_subsystem_controller_product_id, len, MEMORY_SUBSYSTEM_CONTROLLER_PRODUCT_ID, p);

				READU64(current->non_volatile_size, len, NON_VOLATILE_SIZE, p)

				READU64(current->volatile_size, len, VOLATILE_SIZE, p)

				READU64(current->cache_size, len, CACHE_SIZE, p)

				READU64(current->logical_size, len, LOGICAL_SIZE, p)
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
				READU32(current->extended_speed, len, EXTENDED_SPEED, p)

				READU32(current->extended_configured_memory_speed, len, EXTENDED_CONFIGURED_MEMORY_SPEED, p)
			} else {
				current->extended_speed = LAZYBIOS_NOT_FOUND_U32;
				current->extended_configured_memory_speed = LAZYBIOS_NOT_FOUND_U32;
			}

			if (ISVERPLUS(DMIData, 3, 7)) {
				READU16(current->pmic0_manufacturer_id, len, PMIC0_MANUFACTURER_ID, p);

				READU16(current->pmic0_revision_number, len, PMIC0_REVISION_NUMBER, p);

				READU16(current->rcd_manufacturer_id, len, RCD_MANUFACTURER_ID, p);

				READU16(current->rcd_revision_number, len, RCD_REVISION_NUMBER, p);
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
/**
 * @brief Decodes an SMBIOS memory-device form factor.
 *
 * @param form_factor Raw SMBIOS form-factor value.
 * @return Static string describing the form factor.
 */
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
/**
 * @brief Decodes an SMBIOS memory type.
 *
 * @param memory_type Raw SMBIOS memory type value.
 * @return Static string describing the memory type.
 */
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
/**
 * @brief Decodes SMBIOS memory type-detail flags.
 *
 * @param type_detail Raw SMBIOS memory type-detail bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
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
/**
 * @brief Formats an SMBIOS extended memory-device size.
 *
 * @param extended_size Raw SMBIOS extended size value.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
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
/**
 * @brief Decodes an SMBIOS memory technology value.
 *
 * @param memory_technology Raw SMBIOS memory technology value.
 * @return Static string describing the memory technology.
 */
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
/**
 * @brief Decodes memory operating-mode capability flags.
 *
 * @param memory_operating_mode_capability Raw SMBIOS capability bit field.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
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
/**
 * @brief Formats a memory module manufacturer or product identifier.
 *
 * @param id Raw SMBIOS identifier value.
 * @param buf Output buffer that receives the formatted identifier.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17ModuleManufacturerIDStr(uint16_t id, char* buf, size_t buf_len) {
	if (id == 0x0000) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "0x%04X", id);
	}
}

// Volatile Size
/**
 * @brief Formats the volatile capacity of a memory device.
 *
 * @param volatile_size Volatile capacity in bytes.
 * @param buf Output buffer that receives the formatted size.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17VolatileSizeStr(uint64_t volatile_size, char* buf, size_t buf_len) {
	if (volatile_size == 0xFFFFFFFFFFFFFFFFULL) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "%llu", (unsigned long long)volatile_size);
	}
}

// Non-Volatile Size
/**
 * @brief Formats the non-volatile capacity of a memory device.
 *
 * @param non_volatile_size Non-volatile capacity in bytes.
 * @param buf Output buffer that receives the formatted size.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17NonVolatileSizeStr(uint64_t non_volatile_size, char* buf, size_t buf_len) {
	if (non_volatile_size == 0xFFFFFFFFFFFFFFFFULL) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "%llu", (unsigned long long)non_volatile_size);
	}
}

// Cache Size
/**
 * @brief Formats the cache capacity of a memory device.
 *
 * @param cache_size Cache capacity in bytes.
 * @param buf Output buffer that receives the formatted size.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17CacheSizeStr(uint64_t cache_size, char* buf, size_t buf_len) {
	if (cache_size == 0xFFFFFFFFFFFFFFFFULL) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "%llu", (unsigned long long)cache_size);
	}
}

// Extended Speed
/**
 * @brief Formats an SMBIOS extended memory speed.
 *
 * @param extended_speed Raw extended speed value in MT/s.
 * @param buf Output buffer that receives the formatted speed.
 * @param buf_len Capacity of buf in bytes.
 */
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
/**
 * @brief Formats a PMIC0 manufacturer identifier.
 *
 * @param id Raw SMBIOS PMIC0 manufacturer identifier.
 * @param buf Output buffer that receives the formatted identifier.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17PMIC0ManufacturerIDStr(uint16_t id, char* buf, size_t buf_len) {
	if (id == 0x0000) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "0x%04X", id);
	}
}

// PMIC0 Revision
/**
 * @brief Formats a PMIC0 revision number.
 *
 * @param revision Raw SMBIOS PMIC0 revision value.
 * @param buf Output buffer that receives the formatted revision.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17PMIC0RevisionStr(uint16_t revision, char* buf, size_t buf_len) {
	if (revision == 0xFF00) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "0x%04X", revision);
	}
}

// RCD Manufacturer ID
/**
 * @brief Formats a register clock driver manufacturer identifier.
 *
 * @param id Raw SMBIOS RCD manufacturer identifier.
 * @param buf Output buffer that receives the formatted identifier.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17RCDManufacturerIDStr(uint16_t id, char* buf, size_t buf_len) {
	if (id == 0x0000) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "0x%04X", id);
	}
}

// RCD Revision Number
/**
 * @brief Formats a register clock driver revision number.
 *
 * @param revision Raw SMBIOS RCD revision value.
 * @param buf Output buffer that receives the formatted revision.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType17RCDRevisionStr(uint16_t revision, char* buf, size_t buf_len) {
	if (revision == 0xFF00) {
		snprintf(buf, buf_len, "Unknown");
	} else {
		snprintf(buf, buf_len, "0x%04X", revision);
	}
}

// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 17 structures.
 *
 * @param Type17 Type 17 array to release.
 * @param type17_count Number of elements in Type17.
 */
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

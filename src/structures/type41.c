/**
 * @file type41.c
 * @brief Implements parsing and decoding for SMBIOS Type 41 Onboard Devices Extended Information.
 * @author LazySeldi
 */

//
// Type 41 ( Onboard Devices Extended Information )
//

#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define REFERENCE_DESIGNATION 0x04
#define DEVICE_TYPE_AND_STATUS 0x05
#define DEVICE_TYPE_INSTANCE 0x06
#define SEGMENT_GROUP_NUMBER 0x07
#define BUS_NUMBER 0x09
#define DEVICE_FUNCTION_NUMBER 0x0A

// Device Type and Status Masks
#define DEVICE_STATUS_MASK 0x80
#define DEVICE_TYPE_MASK 0x7F

// Device Types
#define DEVICE_TYPE_OTHER 0x01
#define DEVICE_TYPE_UNKNOWN 0x02
#define DEVICE_TYPE_VIDEO 0x03
#define DEVICE_TYPE_SCSI_CONTROLLER 0x04
#define DEVICE_TYPE_ETHERNET 0x05
#define DEVICE_TYPE_TOKEN_RING 0x06
#define DEVICE_TYPE_SOUND 0x07
#define DEVICE_TYPE_PATA_CONTROLLER 0x08
#define DEVICE_TYPE_SATA_CONTROLLER 0x09
#define DEVICE_TYPE_SAS_CONTROLLER 0x0A
#define DEVICE_TYPE_WIRELESS_LAN 0x0B
#define DEVICE_TYPE_BLUETOOTH 0x0C
#define DEVICE_TYPE_WWAN 0x0D
#define DEVICE_TYPE_EMMC 0x0E
#define DEVICE_TYPE_NVME_CONTROLLER 0x0F
#define DEVICE_TYPE_UFS_CONTROLLER 0x10
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 41 Onboard Devices Extended Information structures.
 *
 * @param Type41 Existing Type 41 array pointer value; it is not dereferenced or released.
 * @param type41_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 41 array, or NULL on failure.
 */
lazybiosType41_t* lazybiosGetType41(lazybiosType41_t* Type41, size_t* type41_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_ONBOARD_DEVICES_EXTENDED_INFORMATION);
	size_t index = 0;

	Type41 = calloc(count, sizeof(lazybiosType41_t));
	if (!Type41) return NULL;
	if (count == 0) {
		*type41_count = 0;
		return Type41;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_ONBOARD_DEVICES_EXTENDED_INFORMATION) {
			if (index >= count) break;
			lazybiosType41_t* current = &Type41[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			const uint8_t* structure_end = DMINext(p, end);

			READSTR(current, reference_designation, len, REFERENCE_DESIGNATION, p, structure_end);
			READU8(current, device_type_and_status, len, DEVICE_TYPE_AND_STATUS, p);
			READU8(current, device_type_instance, len, DEVICE_TYPE_INSTANCE, p);
			READU16(current, segment_group_number, len, SEGMENT_GROUP_NUMBER, p);
			READU8(current, bus_number, len, BUS_NUMBER, p);
			READU8(current, device_function_number, len, DEVICE_FUNCTION_NUMBER, p);

			index++;
		}
		p = DMINext(p, end);
	}
	*type41_count = index;
	return Type41;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes the onboard-device type from a combined Type 41 type-and-status byte.
 *
 * @param device_type_and_status Raw Type 41 device-type and status byte.
 * @return Static string describing the onboard-device type.
 */
const char* lazybiosType41DeviceTypeStr(uint8_t device_type_and_status) {
	switch (device_type_and_status & DEVICE_TYPE_MASK) {
		case DEVICE_TYPE_OTHER:
			return "Other";
		case DEVICE_TYPE_UNKNOWN:
			return "Unknown";
		case DEVICE_TYPE_VIDEO:
			return "Video";
		case DEVICE_TYPE_SCSI_CONTROLLER:
			return "SCSI Controller";
		case DEVICE_TYPE_ETHERNET:
			return "Ethernet";
		case DEVICE_TYPE_TOKEN_RING:
			return "Token Ring";
		case DEVICE_TYPE_SOUND:
			return "Sound";
		case DEVICE_TYPE_PATA_CONTROLLER:
			return "PATA Controller";
		case DEVICE_TYPE_SATA_CONTROLLER:
			return "SATA Controller";
		case DEVICE_TYPE_SAS_CONTROLLER:
			return "SAS Controller";
		case DEVICE_TYPE_WIRELESS_LAN:
			return "Wireless LAN";
		case DEVICE_TYPE_BLUETOOTH:
			return "Bluetooth";
		case DEVICE_TYPE_WWAN:
			return "WWAN";
		case DEVICE_TYPE_EMMC:
			return "eMMC (Embedded Multi-Media Controller)";
		case DEVICE_TYPE_NVME_CONTROLLER:
			return "NVMe Controller";
		case DEVICE_TYPE_UFS_CONTROLLER:
			return "UFS Controller";
		default:
			return "Undefined";
	}
}

/**
 * @brief Decodes the enabled status from a combined Type 41 type-and-status byte.
 *
 * @param device_type_and_status Raw Type 41 device-type and status byte.
 * @return Static string describing whether the onboard device is enabled.
 */
const char* lazybiosType41DeviceStatusStr(uint8_t device_type_and_status) {
	return (device_type_and_status & DEVICE_STATUS_MASK) ? "Enabled" : "Disabled";
}

/**
 * @brief Formats an SMBIOS Type 41 packed PCI device/function number.
 *
 * @param device_function_number Raw packed device/function number value.
 * @param buf Output buffer that receives the decoded value.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType41DeviceFunctionStr(uint8_t device_function_number, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;

	if (device_function_number == 0xFF) {
		snprintf(buf, buf_len, "Not Applicable");
		return;
	}

	uint8_t device = (device_function_number >> 3) & 0x1F;
	uint8_t function = device_function_number & 0x07;
	snprintf(buf, buf_len, "Device %hhu, Function %hhu", device, function);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 41 structures.
 *
 * @param Type41 Type 41 array to release.
 * @param type41_count Number of elements in Type41.
 */
void lazybiosFreeType41(lazybiosType41_t* Type41, size_t type41_count) {
	if (!Type41) return;

	for (size_t i = 0; i < type41_count; i++) {
		free(Type41[i].reference_designation);
	}
	free(Type41);
}

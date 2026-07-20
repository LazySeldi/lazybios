/**
 * @file type23.c
 * @brief Implements parsing and decoding for SMBIOS Type 23 System Reset.
 * @author LazySeldi
 */

//
// Type 23 ( System Reset )
//

#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define CAPABILITIES 0x04
#define RESET_COUNT 0x05
#define RESET_LIMIT 0x07
#define TIMER_INTERVAL 0x09
#define TIMEOUT 0x0B

// Capability Masks
#define RESET_STATUS_MASK 0x01
#define BOOT_OPTION_MASK 0x06
#define BOOT_OPTION_SHIFT 1
#define BOOT_OPTION_ON_LIMIT_MASK 0x18
#define BOOT_OPTION_ON_LIMIT_SHIFT 3
#define WATCHDOG_TIMER_MASK 0x20

// Boot Options
#define BOOT_OPTION_RESERVED 0x00
#define BOOT_OPTION_OPERATING_SYSTEM 0x01
#define BOOT_OPTION_SYSTEM_UTILITIES 0x02
#define BOOT_OPTION_DO_NOT_REBOOT 0x03
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 23 System Reset structures.
 *
 * @param Type23 Existing Type 23 array pointer value; it is not dereferenced or released.
 * @param type23_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 23 array, or NULL on failure.
 */
lazybiosType23_t* lazybiosGetType23(lazybiosType23_t* Type23, size_t* type23_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_SYSTEM_RESET);
	size_t index = 0;

	Type23 = calloc(count, sizeof(lazybiosType23_t));
	if (!Type23) return NULL;
	if (count == 0) {
		*type23_count = 0;
		return Type23;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_SYSTEM_RESET) {
			if (index >= count) break;
			lazybiosType23_t* current = &Type23[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);

			READU8(current, capabilities, len, CAPABILITIES, p);
			READU16(current, reset_count, len, RESET_COUNT, p);
			READU16(current, reset_limit, len, RESET_LIMIT, p);
			READU16(current, timer_interval, len, TIMER_INTERVAL, p);
			READU16(current, timeout, len, TIMEOUT, p);

			index++;
		}
		p = DMINext(p, end);
	}
	*type23_count = index;
	return Type23;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Decodes the watchdog boot option from Type 23 capabilities.
 *
 * @param capabilities Raw Type 23 capabilities bit field.
 * @return Static string describing the boot option.
 */
const char* lazybiosType23BootOptionStr(uint8_t capabilities) {
	switch ((capabilities & BOOT_OPTION_MASK) >> BOOT_OPTION_SHIFT) {
		case BOOT_OPTION_RESERVED:
			return "Reserved";
		case BOOT_OPTION_OPERATING_SYSTEM:
			return "Operating System";
		case BOOT_OPTION_SYSTEM_UTILITIES:
			return "System Utilities";
		case BOOT_OPTION_DO_NOT_REBOOT:
			return "Do Not Reboot";
		default:
			return "Undefined";
	}
}

/**
 * @brief Decodes the reset-limit boot option from Type 23 capabilities.
 *
 * @param capabilities Raw Type 23 capabilities bit field.
 * @return Static string describing the boot option used at the reset limit.
 */
const char* lazybiosType23BootOptionOnLimitStr(uint8_t capabilities) {
	switch ((capabilities & BOOT_OPTION_ON_LIMIT_MASK) >> BOOT_OPTION_ON_LIMIT_SHIFT) {
		case BOOT_OPTION_RESERVED:
			return "Reserved";
		case BOOT_OPTION_OPERATING_SYSTEM:
			return "Operating System";
		case BOOT_OPTION_SYSTEM_UTILITIES:
			return "System Utilities";
		case BOOT_OPTION_DO_NOT_REBOOT:
			return "Do Not Reboot";
		default:
			return "Undefined";
	}
}

/**
 * @brief Decodes the SMBIOS Type 23 capabilities bit field.
 *
 * @param capabilities Raw Type 23 capabilities byte.
 * @param buf Output buffer that receives the decoded text.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType23CapabilitiesStr(uint8_t capabilities, char* buf, size_t buf_len) {
	snprintf(buf, buf_len, "%s, %s, Boot Option: %s, Boot Option on Limit: %s",
		(capabilities & RESET_STATUS_MASK) ? "Reset Enabled" : "Reset Disabled",
		(capabilities & WATCHDOG_TIMER_MASK) ? "Watchdog Timer Present" : "Watchdog Timer Not Present",
		lazybiosType23BootOptionStr(capabilities),
		lazybiosType23BootOptionOnLimitStr(capabilities));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 23 structures.
 *
 * @param Type23 Type 23 array to release.
 * @param type23_count Number of elements in Type23.
 */
void lazybiosFreeType23(lazybiosType23_t* Type23, size_t type23_count) {
	(void)type23_count;
	free(Type23);
}

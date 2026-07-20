/**
 * @file type25.c
 * @brief Implements parsing and decoding for SMBIOS Type 25 System Power Controls.
 * @author LazySeldi
 */

//
// Type 25 ( System Power Controls )
//

#include "lazybios_internal.h"
#include <stdio.h>
#include <stdlib.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define NEXT_SCHEDULED_POWER_ON_MONTH 0x04
#define NEXT_SCHEDULED_POWER_ON_DAY 0x05
#define NEXT_SCHEDULED_POWER_ON_HOUR 0x06
#define NEXT_SCHEDULED_POWER_ON_MINUTE 0x07
#define NEXT_SCHEDULED_POWER_ON_SECOND 0x08
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses all SMBIOS Type 25 System Power Controls structures.
 *
 * @param Type25 Existing Type 25 array pointer value; it is not dereferenced or released.
 * @param type25_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 25 array, or NULL on failure.
 */
lazybiosType25_t* lazybiosGetType25(lazybiosType25_t* Type25, size_t* type25_count, lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_SYSTEM_POWER_CONTROLS);
	size_t index = 0;

	Type25 = calloc(count, sizeof(lazybiosType25_t));
	if (!Type25) return NULL;
	if (count == 0) {
		*type25_count = 0;
		return Type25;
	}

	while (p + SMBIOS_HEADER_SIZE <= end && index < count) {
		uint8_t type = p[0];
		uint8_t len = p[1];

		if (type == SMBIOS_TYPE_SYSTEM_POWER_CONTROLS) {
			if (index >= count) break;
			lazybiosType25_t* current = &Type25[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);

			READU8(current, next_scheduled_power_on_month, len, NEXT_SCHEDULED_POWER_ON_MONTH, p);
			READU8(current, next_scheduled_power_on_day, len, NEXT_SCHEDULED_POWER_ON_DAY, p);
			READU8(current, next_scheduled_power_on_hour, len, NEXT_SCHEDULED_POWER_ON_HOUR, p);
			READU8(current, next_scheduled_power_on_minute, len, NEXT_SCHEDULED_POWER_ON_MINUTE, p);
			READU8(current, next_scheduled_power_on_second, len, NEXT_SCHEDULED_POWER_ON_SECOND, p);

			index++;
		}
		p = DMINext(p, end);
	}
	*type25_count = index;
	return Type25;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders
/**
 * @brief Formats the next scheduled power-on date and time.
 *
 * @param Type25 Parsed Type 25 structure containing BCD date and time fields.
 * @param buf Output buffer that receives the formatted schedule.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType25NextScheduledPowerOnStr(const lazybiosType25_t* Type25, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return;
	if (!Type25) {
		snprintf(buf, buf_len, "Not Present");
		return;
	}

	uint8_t fields[5] = {
		Type25->next_scheduled_power_on_month,
		Type25->next_scheduled_power_on_day,
		Type25->next_scheduled_power_on_hour,
		Type25->next_scheduled_power_on_minute,
		Type25->next_scheduled_power_on_second
	};
	lazybiosFieldStatus_t statuses[5] = {
		Type25->field_status.next_scheduled_power_on_month,
		Type25->field_status.next_scheduled_power_on_day,
		Type25->field_status.next_scheduled_power_on_hour,
		Type25->field_status.next_scheduled_power_on_minute,
		Type25->field_status.next_scheduled_power_on_second
	};
	uint8_t minimums[5] = {1, 1, 0, 0, 0};
	uint8_t maximums[5] = {12, 31, 23, 59, 59};
	char values[5][4];

	for (size_t i = 0; i < 5; i++) {
		uint8_t high = (uint8_t)(fields[i] >> 4);
		uint8_t low = (uint8_t)(fields[i] & 0x0F);
		uint8_t value = (uint8_t)(high * 10 + low);

		if (statuses[i] != LAZYBIOS_FIELD_PRESENT || high > 9 || low > 9 ||
			value < minimums[i] || value > maximums[i]) {
			snprintf(values[i], sizeof(values[i]), "*");
		} else {
			snprintf(values[i], sizeof(values[i]), "%02hhu", value);
		}
	}

	snprintf(buf, buf_len, "%s-%s %s:%s:%s",
		values[0], values[1], values[2], values[3], values[4]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free Function
/**
 * @brief Releases an array of parsed SMBIOS Type 25 structures.
 *
 * @param Type25 Type 25 array to release.
 * @param type25_count Number of elements in Type25.
 */
void lazybiosFreeType25(lazybiosType25_t* Type25, size_t type25_count) {
	(void)type25_count;
	free(Type25);
}

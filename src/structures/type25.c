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
 * @file type25.c
 * @brief Implements parsing and decoding for SMBIOS Type 25 System Power Controls.
 * @author LazySeldi
 */
#include "lazybios_internal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* File-local decoders; their output is stored in each record's `decoded`. */
static size_t lazybiosType25NextScheduledPowerOnStr(const lazybiosType25_t* Type25, char* buf, size_t buf_len);

// Fields
#define NEXT_SCHEDULED_POWER_ON_MONTH 0x04
#define NEXT_SCHEDULED_POWER_ON_DAY 0x05
#define NEXT_SCHEDULED_POWER_ON_HOUR 0x06
#define NEXT_SCHEDULED_POWER_ON_MINUTE 0x07
#define NEXT_SCHEDULED_POWER_ON_SECOND 0x08

lazybiosType25Array_t* lazybiosGetType25(const lazybiosDMI_t* DMIData) {
	if (!DMIData || !DMIData->dmi_data) return NULL;

	lazybiosType25Array_t* out = calloc(1, sizeof(*out));
	if (!out) return NULL;

	const uint8_t* p = DMIData->dmi_data;
	const uint8_t* end = DMIData->dmi_data + DMIData->dmi_len;

	size_t count = lazybiosCountStructsByType(DMIData, SMBIOS_TYPE_SYSTEM_POWER_CONTROLS);
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

		if (type == SMBIOS_TYPE_SYSTEM_POWER_CONTROLS) {
			if (index >= count) break;
			lazybiosType25_t* current = &out->entries[index];
			LAZYBIOS_CLAMP_STRUCTURE_LENGTH(len, p, end);
			current->handle = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
			current->length = len;

			READU8(current, next_scheduled_power_on_month, len, NEXT_SCHEDULED_POWER_ON_MONTH, p);
			READU8(current, next_scheduled_power_on_day, len, NEXT_SCHEDULED_POWER_ON_DAY, p);
			READU8(current, next_scheduled_power_on_hour, len, NEXT_SCHEDULED_POWER_ON_HOUR, p);
			READU8(current, next_scheduled_power_on_minute, len, NEXT_SCHEDULED_POWER_ON_MINUTE, p);
			READU8(current, next_scheduled_power_on_second, len, NEXT_SCHEDULED_POWER_ON_SECOND, p);

			char decbuf[LAZYBIOS_DECODER_BUF_SIZE];
			lazybiosType25NextScheduledPowerOnStr(current, decbuf, sizeof(decbuf));
			current->decoded.next_scheduled_power_on = lazybiosDup(decbuf);

			index++;
		}
		p = DMINext(p, end);
	}
	out->count = index;
	return out;
}

static size_t lazybiosType25NextScheduledPowerOnStr(const lazybiosType25_t* Type25, char* buf, size_t buf_len) {
	if (!buf || buf_len == 0) return 0;
	if (!Type25) {
		snprintf(buf, buf_len, "Not Present");
		return 0;
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
	return buf ? strlen(buf) : 0;
}

void lazybiosFreeType25(lazybiosType25Array_t* Type25) {
    if (!Type25) return;

	for (size_t i = 0; i < Type25->count; i++) {
		free(Type25->entries[i].decoded.next_scheduled_power_on);
	}

    free(Type25->entries);

    free(Type25);
}

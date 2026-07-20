/**
 * @file type25.h
 * @brief Public API for SMBIOS Type 25 System Power Controls.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE25_H
#define LAZYBIOS_TYPE25_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 25 fields. */
typedef struct {
	lazybiosFieldStatus_t next_scheduled_power_on_month;
	lazybiosFieldStatus_t next_scheduled_power_on_day;
	lazybiosFieldStatus_t next_scheduled_power_on_hour;
	lazybiosFieldStatus_t next_scheduled_power_on_minute;
	lazybiosFieldStatus_t next_scheduled_power_on_second;
} lazybiosType25FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 25 System Power Controls information.
 * @ingroup api_type25
 */
typedef struct {
	uint8_t next_scheduled_power_on_month;
	uint8_t next_scheduled_power_on_day;
	uint8_t next_scheduled_power_on_hour;
	uint8_t next_scheduled_power_on_minute;
	uint8_t next_scheduled_power_on_second;
	lazybiosType25FieldStatus_t field_status;
} lazybiosType25_t;

/** @addtogroup api_type25
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 25 System Power Controls structures.
 * @param Type25 Existing Type 25 array pointer value; it is not dereferenced or released.
 * @param type25_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 25 array, or NULL on failure.
 */
lazybiosType25_t* lazybiosGetType25(lazybiosType25_t* Type25, size_t* type25_count, lazybiosDMI_t* DMIData);

/**
 * @brief Formats the next scheduled power-on date and time.
 * @param Type25 Parsed Type 25 structure containing BCD date and time fields.
 * @param buf Output buffer that receives the formatted schedule.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType25NextScheduledPowerOnStr(const lazybiosType25_t* Type25, char* buf, size_t buf_len);

/**
 * @brief Releases an array of parsed SMBIOS Type 25 structures.
 * @param Type25 Type 25 array to release.
 * @param type25_count Number of elements in Type25.
 */
void lazybiosFreeType25(lazybiosType25_t* Type25, size_t type25_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

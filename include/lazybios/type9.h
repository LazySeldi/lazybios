/**
 * @file type9.h
 * @brief Public API for SMBIOS Type 9 System Slots.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE9_H
#define LAZYBIOS_TYPE9_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for a Type 9 peer group. */
typedef struct {
	lazybiosFieldStatus_t segment_group_number;
	lazybiosFieldStatus_t bus_number;
	lazybiosFieldStatus_t device_function_number;
	lazybiosFieldStatus_t data_bus_width;
} lazybiosType9PeerGroupFieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 9 peer grouping entry.
 * @ingroup api_type9
 */
typedef struct {
	uint16_t segment_group_number;
	uint8_t bus_number;
	uint8_t device_function_number;
	uint8_t data_bus_width;
	lazybiosType9PeerGroupFieldStatus_t field_status;
} lazybiosType9PeerGroup_t;

/** @brief Availability metadata for SMBIOS Type 9 fields. */
typedef struct {
	lazybiosFieldStatus_t slot_designation;
	lazybiosFieldStatus_t slot_type;
	lazybiosFieldStatus_t slot_data_bus_width;
	lazybiosFieldStatus_t current_usage;
	lazybiosFieldStatus_t slot_length;
	lazybiosFieldStatus_t slot_id;
	lazybiosFieldStatus_t slot_characteristics_1;
	lazybiosFieldStatus_t slot_characteristics_2;
	lazybiosFieldStatus_t segment_group_number;
	lazybiosFieldStatus_t bus_number;
	lazybiosFieldStatus_t device_function_number;
	lazybiosFieldStatus_t data_bus_width;
	lazybiosFieldStatus_t peer_grouping_count;
	lazybiosFieldStatus_t peer_groups;
	lazybiosFieldStatus_t slot_information;
	lazybiosFieldStatus_t slot_physical_width;
	lazybiosFieldStatus_t slot_pitch;
	lazybiosFieldStatus_t slot_height;
} lazybiosType9FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 9 System Slots Information.
 * @ingroup api_type9
 */
typedef struct {
	char* slot_designation;
	uint8_t slot_type;
	uint8_t slot_data_bus_width;
	uint8_t current_usage;
	uint8_t slot_length;
	uint16_t slot_id;
	uint8_t slot_characteristics_1;
	uint8_t slot_characteristics_2;
	uint16_t segment_group_number;
	uint8_t bus_number;
	uint8_t device_function_number;
	uint8_t data_bus_width;
	uint8_t peer_grouping_count;
	lazybiosType9PeerGroup_t* peer_groups;
	uint8_t slot_information;
	uint8_t slot_physical_width;
	uint16_t slot_pitch;
	uint8_t slot_height;
	lazybiosType9FieldStatus_t field_status;
} lazybiosType9_t;

/** @addtogroup api_type9
 * @{
 */

/** @brief Parses all Type 9 structures. */
lazybiosType9_t* lazybiosGetType9(lazybiosType9_t* Type9, size_t* type9_count, lazybiosDMI_t* DMIData);
/** @brief Decodes a system-slot type. */
const char* lazybiosType9SlotTypeStr(uint8_t slot_type);
/** @brief Decodes a system-slot width. */
const char* lazybiosType9SlotWidthStr(uint8_t width);
/** @brief Decodes current slot usage. */
const char* lazybiosType9CurrentUsageStr(uint8_t current_usage);
/** @brief Decodes system-slot length. */
const char* lazybiosType9SlotLengthStr(uint8_t slot_length);
/** @brief Decodes characteristics byte 1. */
void lazybiosType9Characteristics1Str(uint8_t characteristics, char* buf, size_t buf_len);
/** @brief Decodes characteristics byte 2. */
void lazybiosType9Characteristics2Str(uint8_t characteristics, char* buf, size_t buf_len);
/** @brief Decodes a packed PCI device/function field. */
void lazybiosType9DeviceFunctionStr(uint8_t device_function_number, char* buf, size_t buf_len);
/** @brief Decodes system-slot height. */
const char* lazybiosType9SlotHeightStr(uint8_t slot_height);
/** @brief Releases parsed Type 9 structures. */
void lazybiosFreeType9(lazybiosType9_t* Type9, size_t type9_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

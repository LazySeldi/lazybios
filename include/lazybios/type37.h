/**
 * @file type37.h
 * @brief Public API for SMBIOS Type 37 Memory Channel.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE37_H
#define LAZYBIOS_TYPE37_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for a Type 37 memory-device entry. */
typedef struct {
	lazybiosFieldStatus_t load;
	lazybiosFieldStatus_t handle;
} lazybiosType37MemoryDeviceFieldStatus_t;

/**
 * @brief Parsed memory-device association in an SMBIOS Type 37 channel.
 * @ingroup api_type37
 */
typedef struct {
	uint8_t load;
	uint16_t handle;
	lazybiosType37MemoryDeviceFieldStatus_t field_status;
} lazybiosType37MemoryDevice_t;

/** @brief Availability metadata for SMBIOS Type 37 fields. */
typedef struct {
	lazybiosFieldStatus_t channel_type;
	lazybiosFieldStatus_t maximum_channel_load;
	lazybiosFieldStatus_t memory_device_count;
	lazybiosFieldStatus_t memory_devices;
} lazybiosType37FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 37 Memory Channel information.
 * @ingroup api_type37
 */
typedef struct {
	uint8_t channel_type;
	uint8_t maximum_channel_load;
	uint8_t memory_device_count;
	lazybiosType37MemoryDevice_t* memory_devices;
	lazybiosType37FieldStatus_t field_status;
} lazybiosType37_t;

/** @addtogroup api_type37
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 37 Memory Channel structures.
 * @param Type37 Existing Type 37 array pointer value; it is not dereferenced or released.
 * @param type37_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 37 array, or NULL on failure.
 */
lazybiosType37_t* lazybiosGetType37(lazybiosType37_t* Type37, size_t* type37_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an SMBIOS Type 37 memory-channel type.
 * @param channel_type Raw memory-channel type value.
 * @return Static string describing the channel type.
 */
const char* lazybiosType37ChannelTypeStr(uint8_t channel_type);

/**
 * @brief Releases an array of parsed SMBIOS Type 37 structures.
 * @param Type37 Type 37 array to release.
 * @param type37_count Number of elements in Type37.
 */
void lazybiosFreeType37(lazybiosType37_t* Type37, size_t type37_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

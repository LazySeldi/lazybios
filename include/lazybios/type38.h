/**
 * @file type38.h
 * @brief Public API for SMBIOS Type 38 IPMI Device Information.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TYPE38_H
#define LAZYBIOS_TYPE38_H

#ifndef LAZYBIOS_SHARED_API_H
#define LAZYBIOS_TYPE_HEADER_ONLY
#include "../lazybios.h"
#undef LAZYBIOS_TYPE_HEADER_ONLY
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Availability metadata for SMBIOS Type 38 fields. */
typedef struct {
	lazybiosFieldStatus_t interface_type;
	lazybiosFieldStatus_t ipmi_specification_revision;
	lazybiosFieldStatus_t i2c_target_address;
	lazybiosFieldStatus_t nv_storage_device_address;
	lazybiosFieldStatus_t base_address;
	lazybiosFieldStatus_t base_address_modifier_interrupt_info;
	lazybiosFieldStatus_t interrupt_number;
} lazybiosType38FieldStatus_t;

/**
 * @brief Parsed SMBIOS Type 38 IPMI Device Information.
 * @ingroup api_type38
 */
typedef struct {
	uint8_t interface_type;
	uint8_t ipmi_specification_revision;
	uint8_t i2c_target_address;
	uint8_t nv_storage_device_address;
	uint64_t base_address;
	uint8_t base_address_modifier_interrupt_info;
	uint8_t interrupt_number;
	lazybiosType38FieldStatus_t field_status;
} lazybiosType38_t;

/** @addtogroup api_type38
 * @{
 */

/**
 * @brief Parses all SMBIOS Type 38 IPMI Device Information structures.
 * @param Type38 Existing Type 38 array pointer value; it is not dereferenced or released.
 * @param type38_count Output location for the number of parsed structures.
 * @param DMIData Raw DMI table container to parse.
 * @return Newly allocated Type 38 array, or NULL on failure.
 */
lazybiosType38_t* lazybiosGetType38(lazybiosType38_t* Type38, size_t* type38_count, lazybiosDMI_t* DMIData);

/**
 * @brief Decodes an IPMI BMC interface type.
 * @param interface_type Raw Type 38 interface-type value.
 * @return Static string describing the BMC interface type.
 */
const char* lazybiosType38InterfaceTypeStr(uint8_t interface_type);

/**
 * @brief Formats the BCD-encoded IPMI specification revision.
 * @param revision Raw BCD-encoded specification revision.
 * @param buf Output buffer that receives the formatted revision.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType38SpecificationRevisionStr(uint8_t revision, char* buf, size_t buf_len);

/**
 * @brief Decodes the address space selected by a Type 38 base address.
 * @param base_address Raw Type 38 base-address field.
 * @return Static string describing the selected address space.
 */
const char* lazybiosType38BaseAddressTypeStr(uint64_t base_address);

/**
 * @brief Reconstructs the effective BMC base address.
 * @param base_address Raw Type 38 base-address field.
 * @param modifier Raw base-address-modifier and interrupt-information byte.
 * @return Base address with the address-space flag removed and address bit zero restored.
 */
uint64_t lazybiosType38BaseAddressValue(uint64_t base_address, uint8_t modifier);

/**
 * @brief Decodes the IPMI interface register spacing.
 * @param modifier Raw base-address-modifier and interrupt-information byte.
 * @return Static string describing the register spacing.
 */
const char* lazybiosType38RegisterSpacingStr(uint8_t modifier);

/**
 * @brief Decodes Type 38 interrupt availability, polarity, and trigger mode.
 * @param interrupt_info Raw base-address-modifier and interrupt-information byte.
 * @param buf Output buffer that receives the decoded interrupt information.
 * @param buf_len Capacity of buf in bytes.
 */
void lazybiosType38InterruptInfoStr(uint8_t interrupt_info, char* buf, size_t buf_len);

/**
 * @brief Releases an array of parsed SMBIOS Type 38 structures.
 * @param Type38 Type 38 array to release.
 * @param type38_count Number of elements in Type38.
 */
void lazybiosFreeType38(lazybiosType38_t* Type38, size_t type38_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

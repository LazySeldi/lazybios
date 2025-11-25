#ifndef LAZYBIOS_H
#define LAZYBIOS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// ===== SMBIOS Entry Point Constants =====
#define SMBIOS2_ANCHOR     "_SM_"
#define SMBIOS3_ANCHOR     "_SM3_"
#define SMBIOS2_ENTRY_SIZE 31
#define SMBIOS3_ENTRY_SIZE 24

#define ENTRY2_MAJOR_OFFSET        6
#define ENTRY2_MINOR_OFFSET        7
#define ENTRY2_TABLE_LENGTH_OFFSET 0x16
#define ENTRY2_TABLE_ADDR_OFFSET   0x18

#define ENTRY3_MAJOR_OFFSET        7
#define ENTRY3_MINOR_OFFSET        8
#define ENTRY3_DOCREV_OFFSET       9
#define ENTRY3_TABLE_LENGTH_OFFSET 0x0C
#define ENTRY3_TABLE_ADDR_OFFSET   0x10

// ===== SMBIOS Structure Types =====
#define SMBIOS_TYPE_BIOS             0
#define SMBIOS_TYPE_SYSTEM           1
#define SMBIOS_TYPE_BASEBOARD        2
#define SMBIOS_TYPE_CHASSIS          3
#define SMBIOS_TYPE_PROCESSOR        4
#define SMBIOS_TYPE_CACHES           7
#define SMBIOS_TYPE_PORT_CONNECTOR   8
#define SMBIOS_TYPE_ONBOARD_DEVICES  10
#define SMBIOS_TYPE_OEM_STRINGS      11
#define SMBIOS_TYPE_MEMARRAY         16
#define SMBIOS_TYPE_MEMDEVICE        17
#define SMBIOS_TYPE_END              127

// ===== SMBIOS Type 0: BIOS Information =====
#define BIOS_MIN_LENGTH_2_0      0x12
#define BIOS_MIN_LENGTH_3_0      0x18
#define BIOS_VENDOR_OFFSET       0x04
#define BIOS_VERSION_OFFSET      0x05
#define BIOS_RELEASE_DATE_OFFSET 0x08
#define BIOS_ROM_SIZE_OFFSET     0x09
#define BIOS_ROM_SIZE_MULTIPLIER 64

// ===== SMBIOS Type 1: System Information =====
#define SYSTEM_MIN_LENGTH_2_0   0x08
#define SYSTEM_MIN_LENGTH_3_0   0x1B
#define SYS_MANUFACTURER_OFFSET 0x04
#define SYS_PRODUCT_OFFSET      0x05
#define SYS_VERSION_OFFSET      0x06
#define SYS_SERIAL_OFFSET       0x07
#define SYS_UUID_OFFSET         0x08

// ===== SMBIOS Type 2: Baseboard Information =====
#define BASEBOARD_MIN_LENGTH          0x08
#define BASEBOARD_MANUFACTURER_OFFSET 0x04
#define BASEBOARD_PRODUCT_OFFSET      0x05
#define BASEBOARD_VERSION_OFFSET      0x06
#define BASEBOARD_SERIAL_OFFSET       0x07
#define BASEBOARD_ASSET_TAG_OFFSET    0x08

// ===== SMBIOS Type 3: Chassis Information =====
#define CHASSIS_MIN_LENGTH_2_0   0x09
#define CHASSIS_MIN_LENGTH_3_0   0x15
#define CHASSIS_ASSET_TAG_OFFSET 0x08
#define CHASSIS_SKU_OFFSET       0x19

// ===== SMBIOS Type 4: Processor Information =====
#define PROC_MIN_LENGTH_2_0          0x1A
#define PROC_MIN_LENGTH_2_6          0x2A
#define PROC_SOCKET_OFFSET           0x04
#define PROC_TYPE_OFFSET             0x05
#define PROC_FAMILY_OFFSET           0x06
#define PROC_MANUFACTURER_OFFSET     0x07
#define PROC_ID_OFFSET               0x08
#define PROC_VERSION_OFFSET          0x10
#define PROC_VOLTAGE_OFFSET          0x11
#define PROC_EXTERNAL_CLOCK_OFFSET   0x12
#define PROC_MAX_SPEED_OFFSET        0x14
#define PROC_CURRENT_SPEED_OFFSET    0x16
#define PROC_STATUS_OFFSET           0x18
#define PROC_UPGRADE_OFFSET          0x19
#define PROC_L1_CACHE_HANDLE_OFFSET  0x1A
#define PROC_L2_CACHE_HANDLE_OFFSET  0x1C
#define PROC_L3_CACHE_HANDLE_OFFSET  0x1E
#define PROC_SERIAL_OFFSET_2_5       0x20
#define PROC_ASSET_TAG_OFFSET_2_5    0x21
#define PROC_PART_NUMBER_OFFSET_2_5  0x22
#define PROC_CORE_COUNT_OFFSET       0x23
#define PROC_CORE_ENABLED_OFFSET     0x24
#define PROC_THREAD_COUNT_OFFSET     0x25
#define PROC_CHARACTERISTICS_OFFSET  0x26
#define PROC_FAMILY2_OFFSET          0x28
#define PROC_CORE_COUNT2_OFFSET      0x2A
#define PROC_CORE_ENABLED2_OFFSET    0x2C
#define PROC_THREAD_COUNT2_OFFSET    0x2E

// ===== SMBIOS Type 7: Cache Information =====
#define CACHE_MIN_LENGTH_2_0        0x0F
#define CACHE_MIN_LENGTH_2_1        0x13
#define CACHE_MIN_LENGTH_3_1        0x1B
#define CACHE_SOCKET_DESIGNATION    0x04
#define CACHE_CONFIGURATION         0x05
#define CACHE_MAXIMUM_SIZE          0x07
#define CACHE_INSTALLED_SIZE        0x09
#define CACHE_SUPPORTED_SRAM_TYPE   0x0B
#define CACHE_CURRENT_SRAM_TYPE     0x0D
#define CACHE_SPEED                 0x0F
#define CACHE_ERROR_CORRECTION_TYPE 0x10
#define CACHE_SYSTEM_CACHE_TYPE     0x11
#define CACHE_ASSOCIATIVITY         0x12
#define CACHE_MAXIMUM_SIZE_2        0x13
#define CACHE_INSTALLED_SIZE_2      0x17

// ===== SMBIOS Type 8: Port Connectors info =====
#define PORT_CONNECTOR_MIN_LENGTH                 0x09 // Same in ALL SMBIOS versions
#define PORT_OFFSET_HANDLE                        0x02
#define PORT_OFFSET_INTERNAL_REF_DESIGNATOR       0x04
#define PORT_OFFSET_INTERNAL_CONNECTOR_TYPE       0x05
#define PORT_OFFSET_EXTERNAL_REF_DESIGNATOR       0x06
#define PORT_OFFSET_EXTERNAL_CONNECTOR_TYPE       0x07
#define PORT_OFFSET_PORT_TYPE                     0x08

// ===== SMBIOS Type 10: Onboard Devices =====
#define ONBOARD_DEVICES_MIN_LENGTH       0x04
#define ONBOARD_DEV_TYPE_OFFSET(n)       (4 + 2 * ((n) - 1))
#define ONBOARD_DEV_STR_OFFSET(n)        (4 + 2 * ((n) - 1) + 1)
#define ONBOARD_DEV_COUNT(length)        (((length) - 4) / 2)

// ===== SMBIOS Type 11: OEM Strings =====
#define OEM_STRINGS_MIN_LENGTH   0x05
#define OEM_STRINGS_COUNT_OFFSET 0x04

// ===== SMBIOS Type 16: Physical Memory Array =====
#define MEMARRAY_MIN_LENGTH              0x0F
#define MEMARRAY_LOCATION_OFFSET         0x04
#define MEMARRAY_USE_OFFSET              0x05
#define MEMARRAY_ECC_OFFSET              0x06
#define MEMARRAY_MAX_CAPACITY_OFFSET     0x07
#define MEMARRAY_NUM_DEVICES_OFFSET      0x0D
#define MEMARRAY_EXT_MAX_CAPACITY_OFFSET 0x0F

// ===== SMBIOS Type 17: Memory Device =====
#define MEMORY_MIN_LENGTH               0x01
#define MEM_DEVICE_ARRAY_HANDLE_OFFSET  0x04
#define MEM_DEVICE_ERROR_HANDLE_OFFSET  0x06
#define MEM_DEVICE_TOTAL_WIDTH_OFFSET   0x08
#define MEM_DEVICE_DATA_WIDTH_OFFSET    0x0A
#define MEM_DEVICE_SIZE_OFFSET          0x0C
#define MEM_DEVICE_FORM_FACTOR_OFFSET   0x0E
#define MEM_DEVICE_SET_OFFSET           0x0F
#define MEM_DEVICE_LOCATOR_OFFSET       0x10
#define MEM_DEVICE_BANK_LOCATOR_OFFSET  0x11
#define MEM_DEVICE_TYPE_OFFSET          0x12
#define MEM_DEVICE_TYPE_DETAIL_OFFSET   0x13
#define MEM_DEVICE_SPEED_OFFSET         0x15
#define MEM_DEVICE_MANUFACTURER_OFFSET  0x17
#define MEM_DEVICE_SERIAL_OFFSET        0x18
#define MEM_DEVICE_ASSET_TAG_OFFSET     0x19
#define MEM_DEVICE_PART_NUMBER_OFFSET   0x1A
#define MEM_DEVICE_ATTRIBUTES_OFFSET    0x1B
#define MEM_DEVICE_EXT_SIZE_OFFSET      0x1C
#define MEM_DEVICE_CONF_SPEED_OFFSET    0x20
#define MEM_DEVICE_MIN_VOLTAGE_OFFSET   0x22
#define MEM_DEVICE_MAX_VOLTAGE_OFFSET   0x24
#define MEM_DEVICE_CONF_VOLTAGE_OFFSET  0x26

// ==== HELPERS =====

// Processor family codes
#define PROC_FAMILY_OTHER                    0x01
#define PROC_FAMILY_UNKNOWN                  0x02

// Intel legacy
#define PROC_FAMILY_INTEL_8086               0x03
#define PROC_FAMILY_INTEL_80286              0x04
#define PROC_FAMILY_INTEL_80386              0x05
#define PROC_FAMILY_INTEL_80486              0x06
#define PROC_FAMILY_INTEL_PENTIUM            0x0B
#define PROC_FAMILY_INTEL_PENTIUM_PRO        0x0C
#define PROC_FAMILY_INTEL_PENTIUM_II         0x0D
#define PROC_FAMILY_INTEL_PENTIUM_MMX        0x0E
#define PROC_FAMILY_INTEL_CELERON            0x0F
#define PROC_FAMILY_PENTIUM_II_XEON          0x10
#define PROC_FAMILY_PENTIUM_III              0x11

// Intel modern
#define PROC_FAMILY_INTEL_CELERON_M          0x14
#define PROC_FAMILY_INTEL_PENTIUM_4_HT       0x15
#define PROC_FAMILY_INTEL_PROCESSOR          0x16
#define PROC_FAMILY_INTEL_CORE_DUO           0x28
#define PROC_FAMILY_INTEL_CORE_DUO_MOBILE    0x29
#define PROC_FAMILY_INTEL_CORE_SOLO_MOBILE   0x2A
#define PROC_FAMILY_INTEL_ATOM               0x2B
#define PROC_FAMILY_INTEL_CORE_M             0x2C
#define PROC_FAMILY_INTEL_CORE_M3            0x2D
#define PROC_FAMILY_INTEL_CORE_M5            0x2E
#define PROC_FAMILY_INTEL_CORE_M7            0x2F

// Intel Xeon / extended
#define PROC_FAMILY_XEON_3200                0xA1
#define PROC_FAMILY_XEON_3000                0xA2
#define PROC_FAMILY_XEON_5300                0xA3
#define PROC_FAMILY_XEON_5100                0xA4
#define PROC_FAMILY_XEON_5000                0xA5
#define PROC_FAMILY_XEON_LV                  0xA6
#define PROC_FAMILY_XEON_ULV                 0xA7
#define PROC_FAMILY_XEON_7100                0xA8
#define PROC_FAMILY_XEON_5400                0xA9
#define PROC_FAMILY_XEON_GENERIC             0xAA
#define PROC_FAMILY_XEON_5200                0xAB
#define PROC_FAMILY_XEON_7200                0xAC
#define PROC_FAMILY_XEON_7300                0xAD
#define PROC_FAMILY_XEON_7400                0xAE
#define PROC_FAMILY_XEON_MULTI_7400          0xAF
#define PROC_FAMILY_PENTIUM_III_XEON         0xB0
#define PROC_FAMILY_PENTIUM_III_SPEEDSTEP    0xB1
#define PROC_FAMILY_PENTIUM_4                0xB2
#define PROC_FAMILY_INTEL_XEON               0xB3
#define PROC_FAMILY_INTEL_XEON_MP            0xB5
#define PROC_FAMILY_INTEL_PENTIUM_M          0xB9
#define PROC_FAMILY_INTEL_CELERON_D          0xBA
#define PROC_FAMILY_INTEL_PENTIUM_D          0xBB
#define PROC_FAMILY_INTEL_PENTIUM_EXTREME    0xBC
#define PROC_FAMILY_INTEL_CORE_SOLO          0xBD
#define PROC_FAMILY_INTEL_CORE_2_DUO         0xBF
#define PROC_FAMILY_INTEL_CORE_2_SOLO        0xC0
#define PROC_FAMILY_INTEL_CORE_2_EXTREME     0xC1
#define PROC_FAMILY_INTEL_CORE_2_QUAD        0xC2
#define PROC_FAMILY_CORE_2_EXTREME_MOBILE    0xC3
#define PROC_FAMILY_CORE_2_DUO_MOBILE        0xC4
#define PROC_FAMILY_CORE_2_SOLO_MOBILE       0xC5
#define PROC_FAMILY_INTEL_CORE_I7            0xC6
#define PROC_FAMILY_INTEL_DUAL_CELERON       0xC7
#define PROC_FAMILY_INTEL_CORE_I5            0xCD
#define PROC_FAMILY_INTEL_CORE_I3            0xCE
#define PROC_FAMILY_INTEL_CORE_I9            0xCF
#define PROC_FAMILY_INTEL_XEON_D             0xD0
#define PROC_FAMILY_INTEL_XEON_MULTI         0xD6
#define PROC_FAMILY_XEON_DC_3XXX             0xD7
#define PROC_FAMILY_XEON_QC_3XXX             0xD8
#define PROC_FAMILY_XEON_DC_5XXX             0xDA
#define PROC_FAMILY_XEON_QC_5XXX             0xDB

// AMD
#define PROC_FAMILY_AMD_DURON                0x18
#define PROC_FAMILY_AMD_K5                   0x19
#define PROC_FAMILY_AMD_K6                   0x1A
#define PROC_FAMILY_AMD_K6_2                 0x1B
#define PROC_FAMILY_AMD_K6_3                 0x1C
#define PROC_FAMILY_AMD_ATHLON               0x1D
#define PROC_FAMILY_AMD_K6_2_PLUS            0x1F
#define PROC_FAMILY_AMD_TURION_II_ULTRA      0x38
#define PROC_FAMILY_AMD_TURION_II            0x39
#define PROC_FAMILY_AMD_ATHLON_II            0x3A
#define PROC_FAMILY_AMD_OPTERON_6100         0x3B
#define PROC_FAMILY_AMD_OPTERON_4100         0x3C
#define PROC_FAMILY_AMD_OPTERON_6200         0x3D
#define PROC_FAMILY_AMD_OPTERON_4200         0x3E
#define PROC_FAMILY_AMD_FX                   0x3F
#define PROC_FAMILY_AMD_C_SERIES             0x46
#define PROC_FAMILY_AMD_E_SERIES             0x47
#define PROC_FAMILY_AMD_A_SERIES             0x48
#define PROC_FAMILY_AMD_G_SERIES             0x49
#define PROC_FAMILY_AMD_Z_SERIES             0x4A
#define PROC_FAMILY_AMD_R_SERIES             0x4B
#define PROC_FAMILY_AMD_OPTERON_4300         0x4C
#define PROC_FAMILY_AMD_OPTERON_6300         0x4D
#define PROC_FAMILY_AMD_OPTERON_3300         0x4E
#define PROC_FAMILY_AMD_FIREPRO              0x4F
#define PROC_FAMILY_AMD_ATHLON_X4            0x66
#define PROC_FAMILY_AMD_OPTERON_X1000        0x67
#define PROC_FAMILY_AMD_OPTERON_X2000        0x68
#define PROC_FAMILY_AMD_OPTERON_A_SERIES     0x69
#define PROC_FAMILY_AMD_OPTERON_X3000        0x6A
#define PROC_FAMILY_AMD_ZEN                  0x6B
#define PROC_FAMILY_AMD_ATHLON_64            0x83
#define PROC_FAMILY_AMD_OPTERON              0x84
#define PROC_FAMILY_AMD_SEMPRON              0x85
#define PROC_FAMILY_AMD_TURION_64_MOBILE     0x86
#define PROC_FAMILY_AMD_DUAL_OPTERON         0x87
#define PROC_FAMILY_AMD_ATHLON_64_X2         0x88
#define PROC_FAMILY_AMD_TURION_64_X2         0x89
#define PROC_FAMILY_AMD_QUAD_OPTERON         0x8A
#define PROC_FAMILY_AMD_3RD_OPTERON          0x8B
#define PROC_FAMILY_AMD_PHENOM_FX            0x8C
#define PROC_FAMILY_AMD_PHENOM_X4            0x8D
#define PROC_FAMILY_AMD_PHENOM_X2            0x8E
#define PROC_FAMILY_AMD_ATHLON_X2            0x8F
#define PROC_FAMILY_AMD_ATHLON_XP            0xB6
#define PROC_FAMILY_AMD_ATHLON_MP            0xB7

// ARM/Apple
#define PROC_FAMILY_APPLE_M1                 0x12
#define PROC_FAMILY_APPLE_M2                 0x13

// SPARC
#define PROC_FAMILY_SPARC                   0x50
#define PROC_FAMILY_SUPERSPARC              0x51
#define PROC_FAMILY_MICROSPARC_II           0x52
#define PROC_FAMILY_MICROSPARC_IIEP         0x53
#define PROC_FAMILY_ULTRASPARC              0x54
#define PROC_FAMILY_ULTRASPARC_II           0x55
#define PROC_FAMILY_ULTRASPARC_III          0x56
#define PROC_FAMILY_ULTRASPARC_IIIX         0x57
#define PROC_FAMILY_ULTRASPARC_IIIi         0x58

// VIA
#define PROC_FAMILY_VIA_C7_M                0xD2
#define PROC_FAMILY_VIA_C7_D                0xD3
#define PROC_FAMILY_VIA_C7                  0xD4
#define PROC_FAMILY_VIA_EDEN                0xD5
#define PROC_FAMILY_VIA_NANO                0xD9


// Processor Socket Types
#define SOCKET_TYPE_OTHER                0x01
#define SOCKET_TYPE_UNKNOWN              0x02
#define SOCKET_TYPE_DAUGHTER_BOARD       0x03
#define SOCKET_TYPE_ZIF_SOCKET           0x04
#define SOCKET_TYPE_PIGGY_BACK           0x05
#define SOCKET_TYPE_NONE                 0x06
#define SOCKET_TYPE_LIF_SOCKET           0x07
#define SOCKET_TYPE_SLOT_1               0x08
#define SOCKET_TYPE_SLOT_2               0x09
#define SOCKET_TYPE_370_PIN              0x0A
#define SOCKET_TYPE_SLOT_A               0x0B
#define SOCKET_TYPE_SLOT_M               0x0C
#define SOCKET_TYPE_423                  0x0D
#define SOCKET_TYPE_SOCKET_A             0x0E
#define SOCKET_TYPE_478                  0x0F
#define SOCKET_TYPE_754                  0x10
#define SOCKET_TYPE_940                  0x11
#define SOCKET_TYPE_939                  0x12
#define SOCKET_TYPE_MPGA604              0x13
#define SOCKET_TYPE_LGA771               0x14
#define SOCKET_TYPE_LGA775               0x15
#define SOCKET_TYPE_S1                   0x16
#define SOCKET_TYPE_AM2                  0x17
#define SOCKET_TYPE_F_1207               0x18
#define SOCKET_TYPE_LGA1366              0x19
#define SOCKET_TYPE_G34                  0x1A
#define SOCKET_TYPE_AM3                  0x1B
#define SOCKET_TYPE_C32                  0x1C
#define SOCKET_TYPE_LGA1156              0x1D
#define SOCKET_TYPE_LGA1567              0x1E
#define SOCKET_TYPE_PGA988A              0x1F
#define SOCKET_TYPE_BGA1288              0x20
#define SOCKET_TYPE_RPGA988B             0x21
#define SOCKET_TYPE_BGA1023              0x22
#define SOCKET_TYPE_BGA1224              0x23
#define SOCKET_TYPE_LGA1155              0x24
#define SOCKET_TYPE_LGA1356              0x25
#define SOCKET_TYPE_LGA2011              0x26
#define SOCKET_TYPE_FS1                  0x27
#define SOCKET_TYPE_FS2                  0x28
#define SOCKET_TYPE_FM1                  0x29
#define SOCKET_TYPE_FM2                  0x2A
#define SOCKET_TYPE_LGA2011_3            0x2B
#define SOCKET_TYPE_LGA1356_3            0x2C
#define SOCKET_TYPE_LGA1150              0x2D
#define SOCKET_TYPE_BGA1168              0x2E
#define SOCKET_TYPE_BGA1234              0x2F
#define SOCKET_TYPE_BGA1364              0x30
#define SOCKET_TYPE_AM4                  0x31
#define SOCKET_TYPE_LGA1151              0x32
#define SOCKET_TYPE_BGA1356              0x33
#define SOCKET_TYPE_BGA1440              0x34
#define SOCKET_TYPE_BGA1515              0x35
#define SOCKET_TYPE_LGA3647_1            0x36
#define SOCKET_TYPE_SP3                  0x37
#define SOCKET_TYPE_SP3R2                0x38
#define SOCKET_TYPE_LGA2066              0x39
#define SOCKET_TYPE_BGA1392              0x3A
#define SOCKET_TYPE_BGA1510              0x3B
#define SOCKET_TYPE_BGA1528              0x3C
#define SOCKET_TYPE_LGA4189              0x3D
#define SOCKET_TYPE_LGA1200              0x3E
#define SOCKET_TYPE_LGA4677              0x3F
#define SOCKET_TYPE_LGA1700              0x40
#define SOCKET_TYPE_BGA1744              0x41
#define SOCKET_TYPE_BGA1781              0x42
#define SOCKET_TYPE_BGA1211              0x43
#define SOCKET_TYPE_BGA2422              0x44
#define SOCKET_TYPE_LGA1211              0x45
#define SOCKET_TYPE_LGA2422              0x46
#define SOCKET_TYPE_LGA5773              0x47
#define SOCKET_TYPE_BGA5773              0x48
#define SOCKET_TYPE_AM5                  0x49
#define SOCKET_TYPE_SP5                  0x4A
#define SOCKET_TYPE_SP6                  0x4B
#define SOCKET_TYPE_BGA883               0x4C
#define SOCKET_TYPE_BGA1190              0x4D
#define SOCKET_TYPE_BGA4129              0x4E
#define SOCKET_TYPE_LGA4710              0x4F
#define SOCKET_TYPE_LGA7529              0x50
#define SOCKET_TYPE_BGA1964              0x51
#define SOCKET_TYPE_BGA1792              0x52
#define SOCKET_TYPE_BGA2049              0x53
#define SOCKET_TYPE_BGA2551              0x54
#define SOCKET_TYPE_LGA1851              0x55
#define SOCKET_TYPE_BGA2114              0x56
#define SOCKET_TYPE_BGA2833              0x57
#define SOCKET_TYPE_USE_STRING           0xFF


// Processor Type codes
#define PROC_TYPE_OTHER                     0x01
#define PROC_TYPE_UNKNOWN                   0x02
#define PROC_TYPE_CENTRAL_PROCESSOR         0x03
#define PROC_TYPE_MATH_PROCESSOR            0x04
#define PROC_TYPE_DSP_PROCESSOR             0x05
#define PROC_TYPE_VIDEO_PROCESSOR           0x06


// Processor Status (lower 3 bits)
#define PROC_STATUS_MASK                    0x07
#define PROC_STATUS_UNKNOWN                 0x00
#define PROC_STATUS_ENABLED                 0x01
#define PROC_STATUS_DISABLED_BY_USER        0x02
#define PROC_STATUS_DISABLED_BY_BIOS        0x03
#define PROC_STATUS_IDLE                    0x04
#define PROC_STATUS_OTHER                   0x07

// Cache type
#define CACHE_TYPE_OTHER                    0x01
#define CACHE_TYPE_UNKNOWN                  0x02
#define CACHE_TYPE_INSTRUCTION              0x03
#define CACHE_TYPE_DATA                     0x04
#define CACHE_TYPE_UNIFIED                  0x05

// Cache ECC / error correction
#define CACHE_ECC_OTHER                     0x01
#define CACHE_ECC_UNKNOWN                   0x02
#define CACHE_ECC_NONE                      0x03
#define CACHE_ECC_PARITY                    0x04
#define CACHE_ECC_SINGLE_BIT                0x05
#define CACHE_ECC_MULTI_BIT                 0x06
#define CACHE_ECC_CRC                       0x07

// Cache associativity
#define CACHE_ASSOC_OTHER                   0x01
#define CACHE_ASSOC_UNKNOWN                 0x02
#define CACHE_ASSOC_DIRECT_MAPPED           0x03
#define CACHE_ASSOC_2_WAY                   0x04
#define CACHE_ASSOC_4_WAY                   0x05
#define CACHE_ASSOC_FULLY_ASSOC             0x06
#define CACHE_ASSOC_8_WAY                   0x07
#define CACHE_ASSOC_16_WAY                  0x08
#define CACHE_ASSOC_12_WAY                  0x09
#define CACHE_ASSOC_24_WAY                  0x0A
#define CACHE_ASSOC_32_WAY                  0x0B
#define CACHE_ASSOC_48_WAY                  0x0C
#define CACHE_ASSOC_64_WAY                  0x0D
#define CACHE_ASSOC_20_WAY                  0x0E

// Port Connector types
#define CONNECTOR_TYPE_NONE                      0x00
#define CONNECTOR_TYPE_CENTRONICS                0x01
#define CONNECTOR_TYPE_MINI_CENTRONICS           0x02
#define CONNECTOR_TYPE_PROPRIETARY               0x03
#define CONNECTOR_TYPE_DB25_MALE                 0x04
#define CONNECTOR_TYPE_DB25_FEMALE               0x05
#define CONNECTOR_TYPE_DB15_MALE                 0x06
#define CONNECTOR_TYPE_DB15_FEMALE               0x07
#define CONNECTOR_TYPE_DB9_MALE                  0x08
#define CONNECTOR_TYPE_DB9_FEMALE                0x09
#define CONNECTOR_TYPE_RJ11                      0x0A
#define CONNECTOR_TYPE_RJ45                      0x0B
#define CONNECTOR_TYPE_50PIN_MINISCSI            0x0C
#define CONNECTOR_TYPE_MINIDIN                   0x0D
#define CONNECTOR_TYPE_MICRODIN                  0x0E
#define CONNECTOR_TYPE_PS2                       0x0F
#define CONNECTOR_TYPE_INFRARED                  0x10
#define CONNECTOR_TYPE_HP_HIL                    0x11
#define CONNECTOR_TYPE_USB                       0x12
#define CONNECTOR_TYPE_SSA_SCSI                  0x13
#define CONNECTOR_TYPE_CIRCULAR_DIN8_MALE        0x14
#define CONNECTOR_TYPE_CIRCULAR_DIN8_FEMALE      0x15
#define CONNECTOR_TYPE_ONBOARD_IDE               0x16
#define CONNECTOR_TYPE_ONBOARD_FLOPPY            0x17
#define CONNECTOR_TYPE_9PIN_DUAL_INLINE          0x18
#define CONNECTOR_TYPE_25PIN_DUAL_INLINE         0x19
#define CONNECTOR_TYPE_50PIN_DUAL_INLINE         0x1A
#define CONNECTOR_TYPE_68PIN_DUAL_INLINE         0x1B
#define CONNECTOR_TYPE_ONBOARD_SOUND_CDROM       0x1C
#define CONNECTOR_TYPE_MINICENTRONICS_14         0x1D
#define CONNECTOR_TYPE_MINICENTRONICS_26         0x1E
#define CONNECTOR_TYPE_MINI_JACK                 0x1F
#define CONNECTOR_TYPE_BNC                       0x20
#define CONNECTOR_TYPE_1394                      0x21
#define CONNECTOR_TYPE_SAS_SATA                  0x22
#define CONNECTOR_TYPE_USB_C                     0x23
#define CONNECTOR_TYPE_PC98                      0xA0
#define CONNECTOR_TYPE_PC98_HIRESO               0xA1
#define CONNECTOR_TYPE_PC_H98                    0xA2
#define CONNECTOR_TYPE_PC98_NOTE                 0xA3
#define CONNECTOR_TYPE_PC98_FULL                 0xA4
#define CONNECTOR_TYPE_OTHER                     0xFF

// Port types
#define PORT_TYPE_SPEC_NONE                   0x00
#define PORT_TYPE_SPEC_PARALLEL_XT_AT         0x01
#define PORT_TYPE_SPEC_PARALLEL_PS2           0x02
#define PORT_TYPE_SPEC_PARALLEL_ECP           0x03
#define PORT_TYPE_SPEC_PARALLEL_EPP           0x04
#define PORT_TYPE_SPEC_PARALLEL_ECP_EPP       0x05
#define PORT_TYPE_SPEC_SERIAL_XT_AT           0x06
#define PORT_TYPE_SPEC_SERIAL_16450           0x07
#define PORT_TYPE_SPEC_SERIAL_16550           0x08
#define PORT_TYPE_SPEC_SERIAL_16550A          0x09
#define PORT_TYPE_SPEC_SCSI                   0x0A
#define PORT_TYPE_SPEC_MIDI                   0x0B
#define PORT_TYPE_SPEC_JOYSTICK               0x0C
#define PORT_TYPE_SPEC_KEYBOARD               0x0D
#define PORT_TYPE_SPEC_MOUSE                  0x0E
#define PORT_TYPE_SPEC_SSA_SCSI               0x0F
#define PORT_TYPE_SPEC_USB                    0x10
#define PORT_TYPE_SPEC_FIREWIRE               0x11
#define PORT_TYPE_SPEC_PCMCIA_I2              0x12
#define PORT_TYPE_SPEC_PCMCIA_II              0x13
#define PORT_TYPE_SPEC_PCMCIA_III             0x14
#define PORT_TYPE_SPEC_CARDBUS                0x15
#define PORT_TYPE_SPEC_ACCESS_BUS             0x16
#define PORT_TYPE_SPEC_SCSI_II                0x17
#define PORT_TYPE_SPEC_SCSI_WIDE              0x18
#define PORT_TYPE_SPEC_PC98                   0x19
#define PORT_TYPE_SPEC_PC98_HIRESO            0x1A
#define PORT_TYPE_SPEC_PC_H98                 0x1B
#define PORT_TYPE_SPEC_VIDEO                  0x1C
#define PORT_TYPE_SPEC_AUDIO                  0x1D
#define PORT_TYPE_SPEC_MODEM                  0x1E
#define PORT_TYPE_SPEC_NETWORK                0x1F
#define PORT_TYPE_SPEC_SATA                   0x20
#define PORT_TYPE_SPEC_SAS                    0x21
#define PORT_TYPE_SPEC_MFDP                   0x22
#define PORT_TYPE_SPEC_THUNDERBOLT            0x23
#define PORT_TYPE_SPEC_8251_COMPATIBLE        0xA0
#define PORT_TYPE_SPEC_8251_FIFO_COMPATIBLE   0xA1
#define PORT_TYPE_SPEC_OTHER                  0xFF

// On Board Devices Helpers
#define ONBOARD_DEVICE_OTHER            0x01
#define ONBOARD_DEVICE_UNKNOWN          0x02
#define ONBOARD_DEVICE_VIDEO            0x03
#define ONBOARD_DEVICE_SCSI_CTRL        0x04
#define ONBOARD_DEVICE_ETHERNET         0x05
#define ONBOARD_DEVICE_TOKEN_RING       0x06
#define ONBOARD_DEVICE_SOUND            0x07
#define ONBOARD_DEVICE_PATA_CTRL        0x08
#define ONBOARD_DEVICE_SATA_CTRL        0x09
#define ONBOARD_DEVICE_SAS_CTRL         0x0A

// Memory Array helpers
#define MEMORY_SIZE_UNKNOWN    0xFFFF
#define MEMORY_SIZE_EXTENDED   0x7FFF

#define MEMORY_TYPE_OTHER        0x01
#define MEMORY_TYPE_UNKNOWN      0x02
#define MEMORY_TYPE_DRAM         0x03
#define MEMORY_TYPE_EDRAM        0x04
#define MEMORY_TYPE_VRAM         0x05
#define MEMORY_TYPE_SRAM         0x06
#define MEMORY_TYPE_RAM          0x07
#define MEMORY_TYPE_ROM          0x08
#define MEMORY_TYPE_FLASH        0x09
#define MEMORY_TYPE_EEPROM       0x0A
#define MEMORY_TYPE_FEPROM       0x0B
#define MEMORY_TYPE_EPROM        0x0C
#define MEMORY_TYPE_CDRAM        0x0D
#define MEMORY_TYPE_3DRAM        0x0E
#define MEMORY_TYPE_SDRAM        0x0F
#define MEMORY_TYPE_SGRAM        0x10
#define MEMORY_TYPE_RDRAM        0x11
#define MEMORY_TYPE_DDR          0x12
#define MEMORY_TYPE_DDR2         0x13
#define MEMORY_TYPE_DDR2_FB_DIMM 0x14
#define MEMORY_TYPE_DDR3         0x18
#define MEMORY_TYPE_FBD2         0x19
#define MEMORY_TYPE_DDR4         0x1A
#define MEMORY_TYPE_LPDDR        0x1B
#define MEMORY_TYPE_LPDDR2       0x1C
#define MEMORY_TYPE_LPDDR3       0x1D
#define MEMORY_TYPE_LPDDR4       0x1E
#define MEMORY_TYPE_LPDDR_NV     0x1F
#define MEMORY_TYPE_HBM          0x20
#define MEMORY_TYPE_HBM2         0x21
#define MEMORY_TYPE_DDR5         0x22
#define MEMORY_TYPE_LPDDR5       0x23
#define MEMORY_TYPE_HBM3         0x24

#define MEMORY_FORM_FACTOR_OTHER     0x01
#define MEMORY_FORM_FACTOR_UNKNOWN   0x02
#define MEMORY_FORM_FACTOR_SIMM      0x03
#define MEMORY_FORM_FACTOR_SIP       0x04
#define MEMORY_FORM_FACTOR_CHIP      0x05
#define MEMORY_FORM_FACTOR_DIP       0x06
#define MEMORY_FORM_FACTOR_ZIP       0x07
#define MEMORY_FORM_FACTOR_PROP_CARD 0x08
#define MEMORY_FORM_FACTOR_DIMM      0x09
#define MEMORY_FORM_FACTOR_TSOP      0x0A
#define MEMORY_FORM_FACTOR_ROW_CHIPS 0x0B
#define MEMORY_FORM_FACTOR_RIMM      0x0C
#define MEMORY_FORM_FACTOR_SODIMM    0x0D
#define MEMORY_FORM_FACTOR_SRIMM     0x0E
#define MEMORY_FORM_FACTOR_FB_DIMM   0x0F
#define MEMORY_FORM_FACTOR_DIE       0x10
#define MEMORY_FORM_FACTOR_CAMM      0x11

// Memory Array Location
#define MEM_LOC_OTHER                 0x01
#define MEM_LOC_UNKNOWN               0x02
#define MEM_LOC_SYSTEM_BOARD          0x03
#define MEM_LOC_ISA_ADDON             0x04
#define MEM_LOC_EISA_ADDON            0x05
#define MEM_LOC_PCI_ADDON             0x06
#define MEM_LOC_MCA_ADDON             0x07
#define MEM_LOC_PCMCIA_ADDON          0x08
#define MEM_LOC_PROPRIETARY_ADDON     0x09
#define MEM_LOC_NUBUS                 0x0A
#define MEM_LOC_PC98_C20              0xA0
#define MEM_LOC_PC98_C24              0xA1
#define MEM_LOC_PC98_E                0xA2
#define MEM_LOC_PC98_LOCAL            0xA3
#define MEM_LOC_CXL_ADDON             0xA4

// Memory Array Use
#define MEM_USE_OTHER                 0x01
#define MEM_USE_UNKNOWN               0x02
#define MEM_USE_SYSTEM_MEMORY         0x03
#define MEM_USE_VIDEO_MEMORY          0x04
#define MEM_USE_FLASH_MEMORY          0x05
#define MEM_USE_NVRAM                 0x06
#define MEM_USE_CACHE_MEMORY          0x07

// Memory Array ECC
#define MEM_ECC_OTHER                 0x01
#define MEM_ECC_UNKNOWN               0x02
#define MEM_ECC_NONE                  0x03
#define MEM_ECC_PARITY                0x04
#define MEM_ECC_SINGLE_BIT            0x05
#define MEM_ECC_MULTI_BIT             0x06
#define MEM_ECC_CRC                   0x07


// ===== General Constants =====
#define SMBIOS_HEADER_SIZE 4
#define SMBIOS_ENTRY       "/sys/firmware/dmi/tables/smbios_entry_point"
#define DMI_TABLE          "/sys/firmware/dmi/tables/DMI"

// ===== Data Structures =====
typedef struct {
    uint8_t major;
    uint8_t minor;
    uint8_t docrev;  // Only valid for SMBIOS 3.x
    uint32_t table_length;
    uint64_t table_address;
    bool is_64bit;
} smbios_entry_info_t;

typedef struct {
    char *vendor;
    char *version;
    char *release_date;
    uint16_t rom_size_kb;
} bios_info_t;

typedef struct {
    char *manufacturer;
    char *product_name;
    char *version;
    char *serial_number;
    char *uuid;
} system_info_t;

typedef struct {
    char *manufacturer;
    char *product;
    char *version;
    char *serial_number;
    char *asset_tag;
} baseboard_info_t;

typedef struct {
    char *asset_tag;
    char *sku;
    uint8_t type;
    uint8_t state;
} chassis_info_t;

typedef struct {
    char *socket_designation;
    char *version;
    char *serial_number;
    char *asset_tag;
    char *part_number;
    uint16_t core_count;
    uint16_t core_enabled;
    uint16_t thread_count;
    uint16_t max_speed_mhz;
    uint16_t current_speed_mhz;
    uint8_t processor_type;      // Use const char* lazybios_get_processor_type_string(uint8_t type)
    uint8_t processor_family;    // CPU family, needs the lazybios_get_processor_family_string() decoder
    uint16_t processor_family2;  // Extended family for values >= 0xFE
    uint16_t characteristics;    // Use const char* lazybios_get_proc_characteristics_string(uint16_t characteristics)
    uint16_t L1_cache_handle;    // THE HANDLE of the Caches not the actual size of the caches.
    uint16_t L2_cache_handle;
    uint16_t L3_cache_handle;
    uint8_t proc_upgrade;  // Use const char* lazybios_get_socket_type_string(uint8_t type);
    uint8_t voltage;
    uint16_t external_clock_mhz;
    uint8_t status;            // Use the const char* lazybios_get_processor_status_string(uint8_t status)
} processor_info_t;

typedef struct {
    char *description_string;
    uint8_t type;
    bool enabled;
} onboard_devices_t;

typedef struct {
    uint8_t string_count;
    char *strings[];
} OEMStrings_t;

typedef struct {
    char *locator;
    char *bank_locator;
    char *manufacturer;
    char *serial_number;
    char *part_number;
    uint32_t size_mb;
    uint16_t speed_mhz;
    uint8_t memory_type;  // Use with const char* lazybios_get_memory_type_string(uint8_t type)
    uint8_t form_factor;  // Use with const char* lazybios_get_memory_form_factor_string(uint8_t ff)
    uint16_t total_width;
    uint16_t data_width;
    bool size_extended;
} memory_device_t;

typedef struct {
    char *socket_designation;
    uint8_t level;
    uint32_t size_kb;
    uint8_t error_correction_type; //  Use with const char* lazybios_get_cache_ecc_string(uint8_t ecc_type)
    uint8_t system_cache_type; // Use with const char* lazybios_get_cache_type_string(uint8_t cache_type)
    uint8_t associativity;     //  Use with const char* lazybios_get_cache_associativity_string(uint8_t assoc)
} cache_info_t;

typedef struct {
    uint16_t handle;
    char *internal_ref_designator;
    uint8_t  internal_connector_type;
    char *external_ref_designator;
    uint8_t  external_connector_type;
    uint8_t  port_type;
} port_connector_info_t;

typedef struct {
    uint8_t location;  // Use with const char* lazybios_get_memory_array_location_string(uint8_t loc
    uint8_t use;       // Use with const char* lazybios_get_memory_array_use_string(uint8_t use)
    uint8_t ecc_type;  // Use with const char* lazybios_get_memory_array_ecc_string(uint8_t ecc)
    uint64_t max_capacity_kb;
    uint16_t num_devices;
    bool extended_capacity;
} physical_memory_array_t;

typedef struct lazybios_ctx {
    uint8_t *dmi_data;
    size_t dmi_len;
    smbios_entry_info_t entry_info;

    bios_info_t bios_info;
    system_info_t system_info;
    baseboard_info_t baseboard_info;
    chassis_info_t chassis_info;
    processor_info_t processor_info;

    cache_info_t *caches_ptr;
    size_t caches_count;

    port_connector_info_t *port_connector_ptr;
    size_t port_connector_count;

    onboard_devices_t *onboard_devices_ptr;
    size_t onboard_devices_count;

    OEMStrings_t *OEMStrings_ptr;
    size_t OEMStrings_count;

    physical_memory_array_t *memory_arrays_ptr;
    size_t memory_arrays_count;

    memory_device_t *memory_devices_ptr;
    size_t memory_devices_count;
} lazybios_ctx_t;

// ===== Public API =====
lazybios_ctx_t* lazybios_ctx_new(void);
void lazybios_ctx_free(lazybios_ctx_t* ctx);

int lazybios_init(lazybios_ctx_t* ctx);
void lazybios_cleanup(lazybios_ctx_t* ctx);

// Basic functions
void lazybios_smbios_ver(const lazybios_ctx_t* ctx);
const smbios_entry_info_t* lazybios_get_entry_info(const lazybios_ctx_t* ctx);

bios_info_t* lazybios_get_bios_info(lazybios_ctx_t* ctx);
system_info_t* lazybios_get_system_info(lazybios_ctx_t* ctx);
baseboard_info_t* lazybios_get_baseboard_info(lazybios_ctx_t* ctx);
chassis_info_t* lazybios_get_chassis_info(lazybios_ctx_t* ctx);
processor_info_t* lazybios_get_processor_info(lazybios_ctx_t* ctx);

port_connector_info_t* lazybios_get_port_connectors(lazybios_ctx_t* ctx, size_t* count);
cache_info_t* lazybios_get_caches(lazybios_ctx_t* ctx, size_t* count);
onboard_devices_t* lazybios_get_onboard_devices(lazybios_ctx_t* ctx, size_t* count);
OEMStrings_t* lazybios_get_OEMString_info(lazybios_ctx_t* ctx, size_t* count);
physical_memory_array_t* lazybios_get_memory_arrays(lazybios_ctx_t* ctx, size_t* count);
memory_device_t* lazybios_get_memory_devices(lazybios_ctx_t* ctx, size_t* count);

size_t lazybios_get_smbios_structure_min_length(const lazybios_ctx_t* ctx, uint8_t type);
bool lazybios_is_smbios_version_at_least(const lazybios_ctx_t* ctx, uint8_t major, uint8_t minor);

// API Helpers
const char* lazybios_get_processor_family_string(uint8_t family);
const char* lazybios_get_processor_type_string(uint8_t type);
const char* lazybios_get_processor_status_string(uint8_t status);
const char* lazybios_get_proc_characteristics_string(uint16_t characteristics);
const char* lazybios_get_socket_type_string(uint8_t type);

const char* lazybios_get_cache_type_string(uint8_t cache_type);
const char* lazybios_get_cache_ecc_string(uint8_t ecc_type);
const char* lazybios_get_cache_associativity_string(uint8_t associativity);

const char* lazybios_get_onboard_devices_type_string(uint8_t type);

const char* lazybios_get_port_connector_types_string(uint8_t connector_type);
const char* lazybios_get_port_types_string(uint8_t port_type);

const char* lazybios_get_memory_type_string(uint8_t type);
const char* lazybios_get_memory_form_factor_string(uint8_t form_factor);
const char* lazybios_get_memory_array_location_string(uint8_t location);
const char* lazybios_get_memory_array_use_string(uint8_t use);
const char* lazybios_get_memory_array_ecc_string(uint8_t ecc);

#endif

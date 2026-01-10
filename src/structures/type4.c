//
// Type 4 ( Processor Information )
//

#include "lazybios.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
// Fields
#define SOCKET_DESIGNATION              0x04
#define PROCESSOR_TYPE                  0x05
#define PROCESSOR_FAMILY                0x06
#define PROCESSOR_MANUFACTURER          0x07
#define PROCESSOR_ID                    0x08
#define PROCESSOR_VERSION               0x10
#define VOLTAGE                         0x11
#define EXTERNAL_CLOCK                  0x12
#define MAX_SPEED                       0x14
#define CURRENT_SPEED                   0x16
#define STATUS                          0x18
#define PROCESSOR_UPGRADE               0x19
#define L1_CACHE_HANDLE                 0x1A
#define L2_CACHE_HANDLE                 0x1C
#define L3_CACHE_HANDLE                 0x1E
#define SERIAL_NUMBER                   0x20
#define ASSET_TAG                       0x21
#define PART_NUMBER                     0x22
#define CORE_COUNT                      0x23
#define CORE_ENABLED                    0x24
#define THREAD_COUNT                    0x25
#define PROCESSOR_CHARACTERISTICS       0x26
#define PROCESSOR_FAMILY_2              0x28
#define CORE_COUNT_2                    0x2A
#define CORE_ENABLED_2                  0x2C
#define THREAD_COUNT_2                  0x2E
#define THREAD_ENABLED                  0x30
#define SOCKET_TYPE                     0x32

// Decoders

// Processor family
#define PROC_FAMILY_OTHER                           0x01
#define PROC_FAMILY_UNKNOWN                         0x02
#define PROC_FAMILY_8086                            0x03
#define PROC_FAMILY_80286                           0x04
#define PROC_FAMILY_INTEL386                        0x05
#define PROC_FAMILY_INTEL486                        0x06
#define PROC_FAMILY_8087                            0x07
#define PROC_FAMILY_80287                           0x08
#define PROC_FAMILY_80387                           0x09
#define PROC_FAMILY_80487                           0x0A
#define PROC_FAMILY_INTEL_PENTIUM                   0x0B
#define PROC_FAMILY_PENTIUM_PRO                     0x0C
#define PROC_FAMILY_PENTIUM_II                      0x0D
#define PROC_FAMILY_PENTIUM_WITH_MMX                0x0E
#define PROC_FAMILY_INTEL_CELERON                   0x0F
#define PROC_FAMILY_PENTIUM_II_XEON                 0x10
#define PROC_FAMILY_PENTIUM_III                     0x11
#define PROC_FAMILY_M1                              0x12
#define PROC_FAMILY_M2                              0x13
#define PROC_FAMILY_INTEL_CELERON_M                 0x14
#define PROC_FAMILY_INTEL_PENTIUM_4_HT              0x15
#define PROC_FAMILY_INTEL_PROCESSOR                 0x16
#define PROC_FAMILY_AMD_DURON                       0x18
#define PROC_FAMILY_K5                              0x19
#define PROC_FAMILY_K6                              0x1A
#define PROC_FAMILY_K6_2                            0x1B
#define PROC_FAMILY_K6_3                            0x1C
#define PROC_FAMILY_AMD_ATHLON                      0x1D
#define PROC_FAMILY_AMD29000                        0x1E
#define PROC_FAMILY_K6_2_PLUS                       0x1F
#define PROC_FAMILY_POWER_PC                        0x20
#define PROC_FAMILY_POWER_PC_601                    0x21
#define PROC_FAMILY_POWER_PC_603                    0x22
#define PROC_FAMILY_POWER_PC_603_PLUS               0x23
#define PROC_FAMILY_POWER_PC_604                    0x24
#define PROC_FAMILY_POWER_PC_620                    0x25
#define PROC_FAMILY_POWER_PC_X704                   0x26
#define PROC_FAMILY_POWER_PC_750                    0x27
#define PROC_FAMILY_INTEL_CORE_DUO                  0x28
#define PROC_FAMILY_INTEL_CORE_DUO_MOBILE           0x29
#define PROC_FAMILY_INTEL_CORE_SOLO_MOBILE          0x2A
#define PROC_FAMILY_INTEL_ATOM                      0x2B
#define PROC_FAMILY_INTEL_CORE_M                    0x2C
#define PROC_FAMILY_INTEL_CORE_M3                   0x2D
#define PROC_FAMILY_INTEL_CORE_M5                   0x2E
#define PROC_FAMILY_INTEL_CORE_M7                   0x2F
#define PROC_FAMILY_ALPHA                           0x30
#define PROC_FAMILY_ALPHA_21064                     0x31
#define PROC_FAMILY_ALPHA_21066                     0x32
#define PROC_FAMILY_ALPHA_21164                     0x33
#define PROC_FAMILY_ALPHA_21164PC                   0x34
#define PROC_FAMILY_ALPHA_21164A                    0x35
#define PROC_FAMILY_ALPHA_21264                     0x36
#define PROC_FAMILY_ALPHA_21364                     0x37
#define PROC_FAMILY_AMD_TURION_II_ULTRA_DUAL_CORE_MOBILE 0x38
#define PROC_FAMILY_AMD_TURION_II_DUAL_CORE_MOBILE  0x39
#define PROC_FAMILY_AMD_ATHLON_II_DUAL_CORE_M       0x3A
#define PROC_FAMILY_AMD_OPTERON_6100                0x3B
#define PROC_FAMILY_AMD_OPTERON_4100                0x3C
#define PROC_FAMILY_AMD_OPTERON_6200                0x3D
#define PROC_FAMILY_AMD_OPTERON_4200                0x3E
#define PROC_FAMILY_AMD_FX                          0x3F
#define PROC_FAMILY_MIPS                            0x40
#define PROC_FAMILY_MIPS_R4000                      0x41
#define PROC_FAMILY_MIPS_R4200                      0x42
#define PROC_FAMILY_MIPS_R4400                      0x43
#define PROC_FAMILY_MIPS_R4600                      0x44
#define PROC_FAMILY_MIPS_R10000                     0x45
#define PROC_FAMILY_AMD_C_SERIES                    0x46
#define PROC_FAMILY_AMD_E_SERIES                    0x47
#define PROC_FAMILY_AMD_A_SERIES                    0x48
#define PROC_FAMILY_AMD_G_SERIES                    0x49
#define PROC_FAMILY_AMD_Z_SERIES                    0x4A
#define PROC_FAMILY_AMD_R_SERIES                    0x4B
#define PROC_FAMILY_AMD_OPTERON_4300                0x4C
#define PROC_FAMILY_AMD_OPTERON_6300                0x4D
#define PROC_FAMILY_AMD_OPTERON_3300                0x4E
#define PROC_FAMILY_AMD_FIREPRO                     0x4F
#define PROC_FAMILY_SPARC                           0x50
#define PROC_FAMILY_SUPERSPARC                      0x51
#define PROC_FAMILY_MICROSPARC_II                   0x52
#define PROC_FAMILY_MICROSPARC_IIEP                 0x53
#define PROC_FAMILY_ULTRASPARC                      0x54
#define PROC_FAMILY_ULTRASPARC_II                   0x55
#define PROC_FAMILY_ULTRASPARC_III                  0x56
#define PROC_FAMILY_ULTRASPARC_III_2                0x57
#define PROC_FAMILY_ULTRASPARC_IIII                 0x58
#define PROC_FAMILY_68040                           0x60
#define PROC_FAMILY_68XXX                           0x61
#define PROC_FAMILY_68000                           0x62
#define PROC_FAMILY_68010                           0x63
#define PROC_FAMILY_68020                           0x64
#define PROC_FAMILY_68030                           0x65
#define PROC_FAMILY_AMD_ATHLON_X4_QUAD_CORE         0x66
#define PROC_FAMILY_AMD_OPTERON_X1000               0x67
#define PROC_FAMILY_AMD_OPTERON_X2000_APU           0x68
#define PROC_FAMILY_AMD_OPTERON_A_SERIES            0x69
#define PROC_FAMILY_AMD_OPTERON_X3000_APU           0x6A
#define PROC_FAMILY_AMD_ZEN                         0x6B
#define PROC_FAMILY_HOBBIT                          0x70
#define PROC_FAMILY_CRUSOE_TM5000                   0x78
#define PROC_FAMILY_CRUSOE_TM3000                   0x79
#define PROC_FAMILY_EFFICEON_TM8000                 0x7A
#define PROC_FAMILY_WEITEK                          0x80
#define PROC_FAMILY_ITANIUM                         0x82
#define PROC_FAMILY_AMD_ATHLON_64                   0x83
#define PROC_FAMILY_AMD_OPTERON                     0x84
#define PROC_FAMILY_AMD_SEMPRON                     0x85
#define PROC_FAMILY_AMD_TURION_64_MOBILE            0x86
#define PROC_FAMILY_DUAL_CORE_AMD_OPTERON           0x87
#define PROC_FAMILY_AMD_ATHLON_64_X2                0x88
#define PROC_FAMILY_AMD_TURION_64_X2                0x89
#define PROC_FAMILY_QUAD_CORE_AMD_OPTERON           0x8A
#define PROC_FAMILY_THIRD_GEN_AMD_OPTERON           0x8B
#define PROC_FAMILY_AMD_PHENOM_FX_QUAD_CORE         0x8C
#define PROC_FAMILY_AMD_PHENOM_X4_QUAD_CORE         0x8D
#define PROC_FAMILY_AMD_PHENOM_X2_DUAL_CORE         0x8E
#define PROC_FAMILY_AMD_ATHLON_X2_DUAL_CORE         0x8F
#define PROC_FAMILY_PA_RISC                         0x90
#define PROC_FAMILY_PA_RISC_8500                    0x91
#define PROC_FAMILY_PA_RISC_8000                    0x92
#define PROC_FAMILY_PA_RISC_7300LC                  0x93
#define PROC_FAMILY_PA_RISC_7200                    0x94
#define PROC_FAMILY_PA_RISC_7100LC                  0x95
#define PROC_FAMILY_PA_RISC_7100                    0x96
#define PROC_FAMILY_V30                             0xA0
#define PROC_FAMILY_QUAD_CORE_INTEL_XEON_3200       0xA1
#define PROC_FAMILY_DUAL_CORE_INTEL_XEON_3000       0xA2
#define PROC_FAMILY_QUAD_CORE_INTEL_XEON_5300       0xA3
#define PROC_FAMILY_DUAL_CORE_INTEL_XEON_5100       0xA4
#define PROC_FAMILY_DUAL_CORE_INTEL_XEON_5000       0xA5
#define PROC_FAMILY_DUAL_CORE_INTEL_XEON_LV         0xA6
#define PROC_FAMILY_DUAL_CORE_INTEL_XEON_ULV        0xA7
#define PROC_FAMILY_DUAL_CORE_INTEL_XEON_7100       0xA8
#define PROC_FAMILY_QUAD_CORE_INTEL_XEON_5400       0xA9
#define PROC_FAMILY_QUAD_CORE_INTEL_XEON            0xAA
#define PROC_FAMILY_DUAL_CORE_INTEL_XEON_5200       0xAB
#define PROC_FAMILY_DUAL_CORE_INTEL_XEON_7200       0xAC
#define PROC_FAMILY_QUAD_CORE_INTEL_XEON_7300       0xAD
#define PROC_FAMILY_QUAD_CORE_INTEL_XEON_7400       0xAE
#define PROC_FAMILY_MULTI_CORE_INTEL_XEON_7400      0xAF
#define PROC_FAMILY_PENTIUM_III_XEON                0xB0
#define PROC_FAMILY_PENTIUM_III_SPEEDSTEP           0xB1
#define PROC_FAMILY_PENTIUM_4                       0xB2
#define PROC_FAMILY_INTEL_XEON                      0xB3
#define PROC_FAMILY_AS400                           0xB4
#define PROC_FAMILY_INTEL_XEON_MP                   0xB5
#define PROC_FAMILY_AMD_ATHLON_XP                   0xB6
#define PROC_FAMILY_AMD_ATHLON_MP                   0xB7
#define PROC_FAMILY_INTEL_ITANIUM_2                 0xB8
#define PROC_FAMILY_INTEL_PENTIUM_M                 0xB9
#define PROC_FAMILY_INTEL_CELERON_D                 0xBA
#define PROC_FAMILY_INTEL_PENTIUM_D                 0xBB
#define PROC_FAMILY_INTEL_PENTIUM_EXTREME           0xBC
#define PROC_FAMILY_INTEL_CORE_SOLO                 0xBD
#define PROC_FAMILY_INTEL_CORE_2_DUO                0xBF
#define PROC_FAMILY_INTEL_CORE_2_SOLO               0xC0
#define PROC_FAMILY_INTEL_CORE_2_EXTREME            0xC1
#define PROC_FAMILY_INTEL_CORE_2_QUAD               0xC2
#define PROC_FAMILY_INTEL_CORE_2_EXTREME_MOBILE     0xC3
#define PROC_FAMILY_INTEL_CORE_2_DUO_MOBILE         0xC4
#define PROC_FAMILY_INTEL_CORE_2_SOLO_MOBILE        0xC5
#define PROC_FAMILY_INTEL_CORE_I7                   0xC6
#define PROC_FAMILY_DUAL_CORE_INTEL_CELERON         0xC7
#define PROC_FAMILY_IBM390                          0xC8
#define PROC_FAMILY_G4                              0xC9
#define PROC_FAMILY_G5                              0xCA
#define PROC_FAMILY_ESA_390_G6                      0xCB
#define PROC_FAMILY_Z_ARCHITECTURE                  0xCC
#define PROC_FAMILY_INTEL_CORE_I5                   0xCD
#define PROC_FAMILY_INTEL_CORE_I3                   0xCE
#define PROC_FAMILY_INTEL_CORE_I9                   0xCF
#define PROC_FAMILY_INTEL_XEON_D                    0xD0
#define PROC_FAMILY_VIA_C7_M                        0xD2
#define PROC_FAMILY_VIA_C7_D                        0xD3
#define PROC_FAMILY_VIA_C7                          0xD4
#define PROC_FAMILY_VIA_EDEN                        0xD5
#define PROC_FAMILY_MULTI_CORE_INTEL_XEON           0xD6
#define PROC_FAMILY_DUAL_CORE_INTEL_XEON_3XXX       0xD7
#define PROC_FAMILY_QUAD_CORE_INTEL_XEON_3XXX       0xD8
#define PROC_FAMILY_VIA_NANO                        0xD9
#define PROC_FAMILY_DUAL_CORE_INTEL_XEON_5XXX       0xDA
#define PROC_FAMILY_QUAD_CORE_INTEL_XEON_5XXX       0xDB
#define PROC_FAMILY_DUAL_CORE_INTEL_XEON_7XXX       0xDD
#define PROC_FAMILY_QUAD_CORE_INTEL_XEON_7XXX       0xDE
#define PROC_FAMILY_MULTI_CORE_INTEL_XEON_7XXX      0xDF
#define PROC_FAMILY_MULTI_CORE_INTEL_XEON_3400      0xE0
#define PROC_FAMILY_AMD_OPTERON_3000                0xE4
#define PROC_FAMILY_AMD_SEMPRON_II                  0xE5
#define PROC_FAMILY_EMBEDDED_AMD_OPTERON_QUAD_CORE  0xE6
#define PROC_FAMILY_AMD_PHENOM_TRIPLE_CORE          0xE7
#define PROC_FAMILY_AMD_TURION_ULTRA_DUAL_CORE_MOBILE 0xE8
#define PROC_FAMILY_AMD_TURION_DUAL_CORE_MOBILE     0xE9
#define PROC_FAMILY_AMD_ATHLON_DUAL_CORE            0xEA
#define PROC_FAMILY_AMD_SEMPRON_SI                  0xEB
#define PROC_FAMILY_AMD_PHENOM_II                   0xEC
#define PROC_FAMILY_AMD_ATHLON_II                   0xED
#define PROC_FAMILY_SIX_CORE_AMD_OPTERON            0xEE
#define PROC_FAMILY_AMD_SEMPRON_M                   0xEF
#define PROC_FAMILY_I860                            0xFA
#define PROC_FAMILY_I960                            0xFB
#define PROC_FAMILY_INDICATOR_FROM_FAMILY2          0xFE
#define PROC_FAMILY_ARMV7                           0x100
#define PROC_FAMILY_ARMV8                           0x101
#define PROC_FAMILY_ARMV9                           0x102
#define PROC_FAMILY_SH_3                            0x104
#define PROC_FAMILY_SH_4                            0x105
#define PROC_FAMILY_ARM                             0x118
#define PROC_FAMILY_STRONGARM                       0x119
#define PROC_FAMILY_6X86                            0x12C
#define PROC_FAMILY_MEDIAGX                         0x12D
#define PROC_FAMILY_MII                             0x12E
#define PROC_FAMILY_WINCHIP                         0x140
#define PROC_FAMILY_DSP                             0x15E
#define PROC_FAMILY_VIDEO_PROCESSOR                 0x1F4
#define PROC_FAMILY_RISC_V_RV32                     0x200
#define PROC_FAMILY_RISC_V_RV64                     0x201
#define PROC_FAMILY_RISC_V_RV128                    0x202
#define PROC_FAMILY_LOONGARCH                       0x258
#define PROC_FAMILY_LOONGSON_1                      0x259
#define PROC_FAMILY_LOONGSON_2                      0x25A
#define PROC_FAMILY_LOONGSON_3                      0x25B
#define PROC_FAMILY_LOONGSON_2K                     0x25C
#define PROC_FAMILY_LOONGSON_3A                     0x25D
#define PROC_FAMILY_LOONGSON_3B                     0x25E
#define PROC_FAMILY_LOONGSON_3C                     0x25F
#define PROC_FAMILY_LOONGSON_3D                     0x260
#define PROC_FAMILY_LOONGSON_3E                     0x261
#define PROC_FAMILY_DUAL_CORE_LOONGSON_2K_2XXX      0x262
#define PROC_FAMILY_QUAD_CORE_LOONGSON_3A_5XXX      0x26C
#define PROC_FAMILY_MULTI_CORE_LOONGSON_3A_5XXX     0x26D
#define PROC_FAMILY_QUAD_CORE_LOONGSON_3B_5XXX      0x26E
#define PROC_FAMILY_MULTI_CORE_LOONGSON_3B_5XXX     0x26F
#define PROC_FAMILY_MULTI_CORE_LOONGSON_3C_5XXX     0x270
#define PROC_FAMILY_MULTI_CORE_LOONGSON_3D_5XXX     0x271
#define PROC_FAMILY_INTEL_CORE_3                    0x300
#define PROC_FAMILY_INTEL_CORE_5                    0x301
#define PROC_FAMILY_INTEL_CORE_7                    0x302
#define PROC_FAMILY_INTEL_CORE_9                    0x303
#define PROC_FAMILY_INTEL_CORE_ULTRA_3              0x304
#define PROC_FAMILY_INTEL_CORE_ULTRA_5              0x305
#define PROC_FAMILY_INTEL_CORE_ULTRA_7              0x306
#define PROC_FAMILY_INTEL_CORE_ULTRA_9              0x307

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

lazybiosType4_t* lazybiosGetType4(lazybiosCTX_t* ctx) {
    if (!ctx || !ctx->dmi_data) return LAZYBIOS_NULL;

    const uint8_t* p = ctx->dmi_data;
    const uint8_t* end = ctx->dmi_data + ctx->dmi_len;

    while (p + SMBIOS_HEADER_SIZE <= end) {
        uint8_t type = p[0];
        uint8_t len = p[1];

        if (type == SMBIOS_TYPE_PROCESSOR) {
            lazybiosType4_t *Type4 = calloc(1, sizeof(*Type4));
            if (!Type4) return LAZYBIOS_NULL;

            if (len >=SOCKET_DESIGNATION) Type4->socket_designation = DMIString(p, len, p[SOCKET_DESIGNATION], end);
            if (!Type4->socket_designation) Type4->socket_designation = strdup(LAZYBIOS_NOT_FOUND_STR);

            Type4->processor_type = (len >=PROCESSOR_TYPE) ? p[PROCESSOR_TYPE] : LAZYBIOS_NOT_FOUND_U8;
            Type4->processor_family = (len >=PROCESSOR_FAMILY) ? p[PROCESSOR_FAMILY] : LAZYBIOS_NOT_FOUND_U8;

            if (len >=PROCESSOR_MANUFACTURER) Type4->processor_manufacturer = DMIString(p, len, p[PROCESSOR_MANUFACTURER], end);
            if (!Type4->processor_manufacturer) Type4->processor_manufacturer = strdup(LAZYBIOS_NOT_FOUND_STR);

            if (len >=PROCESSOR_ID + sizeof(uint64_t)) {
                memcpy(&Type4->processor_id, p + PROCESSOR_ID, sizeof(uint64_t));
            } else {
                Type4->processor_id = LAZYBIOS_NOT_FOUND_U64;
            }

            if (len >=PROCESSOR_VERSION) Type4->processor_version = DMIString(p, len, p[PROCESSOR_VERSION], end);
            if (!Type4->processor_version) Type4->processor_version = strdup(LAZYBIOS_NOT_FOUND_STR);

            Type4->voltage = (len >=VOLTAGE) ? p[VOLTAGE] : LAZYBIOS_NOT_FOUND_U8;

            if (len >=EXTERNAL_CLOCK + sizeof(uint16_t)) {
                memcpy(&Type4->external_clock, p + EXTERNAL_CLOCK, sizeof(uint16_t));
            } else {
                Type4->external_clock = LAZYBIOS_NOT_FOUND_U16;
            }

            if (len >=MAX_SPEED + sizeof(uint16_t)) {
                memcpy(&Type4->max_speed, p + MAX_SPEED, sizeof(uint16_t));
            } else {
                Type4->max_speed = LAZYBIOS_NOT_FOUND_U16;
            }

            if (len >=CURRENT_SPEED + sizeof(uint16_t)) {
                memcpy(&Type4->current_speed, p + CURRENT_SPEED, sizeof(uint16_t));
            } else {
                Type4->current_speed = LAZYBIOS_NOT_FOUND_U16;
            }

            Type4->status = (len >=STATUS) ? p[STATUS] : LAZYBIOS_NOT_FOUND_U8;
            Type4->processor_upgrade = (len >=PROCESSOR_UPGRADE) ? p[PROCESSOR_UPGRADE] : LAZYBIOS_NOT_FOUND_U8;

            if (ISVERPLUS(ctx, 2, 1)) {
                if (len >=L1_CACHE_HANDLE + sizeof(uint16_t)) {
                    memcpy(&Type4->l1_cache_handle, p + L1_CACHE_HANDLE, sizeof(uint16_t));
                } else {
                    Type4->l1_cache_handle = LAZYBIOS_NOT_FOUND_U16;
                }

                if (len >=L2_CACHE_HANDLE + sizeof(uint16_t)) {
                    memcpy(&Type4->l2_cache_handle, p + L2_CACHE_HANDLE, sizeof(uint16_t));
                } else {
                    Type4->l2_cache_handle = LAZYBIOS_NOT_FOUND_U16;
                }

                if (len >=L3_CACHE_HANDLE + sizeof(uint16_t)) {
                    memcpy(&Type4->l3_cache_handle, p + L3_CACHE_HANDLE, sizeof(uint16_t));
                } else {
                    Type4->l3_cache_handle = LAZYBIOS_NOT_FOUND_U16;
                }
            } else {
                Type4->l1_cache_handle = LAZYBIOS_NOT_FOUND_U16;
                Type4->l2_cache_handle = LAZYBIOS_NOT_FOUND_U16;
                Type4->l3_cache_handle = LAZYBIOS_NOT_FOUND_U16;
            }

            if (ISVERPLUS(ctx, 2, 3)) {
                if (len >=SERIAL_NUMBER) Type4->serial_number = DMIString(p, len, p[SERIAL_NUMBER], end);
                if (!Type4->serial_number) Type4->serial_number = strdup(LAZYBIOS_NOT_FOUND_STR);

                if (len >=ASSET_TAG) Type4->asset_tag = DMIString(p, len, p[ASSET_TAG], end);
                if (!Type4->asset_tag) Type4->asset_tag = strdup(LAZYBIOS_NOT_FOUND_STR);

                if (len >=PART_NUMBER) Type4->part_number = DMIString(p, len, p[PART_NUMBER], end);
                if (!Type4->part_number) Type4->part_number = strdup(LAZYBIOS_NOT_FOUND_STR);
            } else {
                Type4->serial_number = strdup(LAZYBIOS_NOT_FOUND_STR);
                Type4->asset_tag = strdup(LAZYBIOS_NOT_FOUND_STR);
                Type4->part_number = strdup(LAZYBIOS_NOT_FOUND_STR);
            }

            if (ISVERPLUS(ctx, 2, 5)) {
                if (len >=CORE_COUNT) {
                    Type4->core_count = p[CORE_COUNT];
                } else {
                    Type4->core_count = LAZYBIOS_NOT_FOUND_U8;
                }

                if (len >=CORE_ENABLED) {
                    Type4->core_enabled = p[CORE_ENABLED];
                } else {
                    Type4->core_enabled = LAZYBIOS_NOT_FOUND_U8;
                }

                if (len >=THREAD_COUNT) {
                    Type4->thread_count = p[THREAD_COUNT];
                } else {
                    Type4->thread_count = LAZYBIOS_NOT_FOUND_U8;
                }

                if (len >=PROCESSOR_CHARACTERISTICS + sizeof(uint16_t)) {
                    memcpy(&Type4->processor_characteristics, p + PROCESSOR_CHARACTERISTICS, sizeof(uint16_t));
                } else {
                    Type4->processor_characteristics = LAZYBIOS_NOT_FOUND_U16;
                }

                if (len >=PROCESSOR_FAMILY_2 + sizeof(uint16_t)) {
                    memcpy(&Type4->processor_family_2, p + PROCESSOR_FAMILY_2, sizeof(uint16_t));
                } else {
                    Type4->processor_family_2 = LAZYBIOS_NOT_FOUND_U16;
                }
            } else {
                Type4->core_count = LAZYBIOS_NOT_FOUND_U8;
                Type4->core_enabled = LAZYBIOS_NOT_FOUND_U8;
                Type4->thread_count = LAZYBIOS_NOT_FOUND_U8;
                Type4->processor_characteristics = LAZYBIOS_NOT_FOUND_U16;
                Type4->processor_family_2 = LAZYBIOS_NOT_FOUND_U16;
            }

            if (ISVERPLUS(ctx, 3, 0)) {
                if (len >=CORE_COUNT_2 + sizeof(uint16_t)) {
                    memcpy(&Type4->core_count_2, p + CORE_COUNT_2, sizeof(uint16_t));
                } else {
                    Type4->core_count_2 = LAZYBIOS_NOT_FOUND_U16;
                }

                if (len >=CORE_ENABLED_2 + sizeof(uint16_t)) {
                    memcpy(&Type4->core_enabled_2, p + CORE_ENABLED_2, sizeof(uint16_t));
                } else {
                    Type4->core_enabled_2 = LAZYBIOS_NOT_FOUND_U16;
                }

                if (len >=THREAD_COUNT_2 + sizeof(uint16_t)) {
                    memcpy(&Type4->thread_count_2, p + THREAD_COUNT_2, sizeof(uint16_t));
                } else {
                    Type4->thread_count_2 = LAZYBIOS_NOT_FOUND_U16;
                }
            } else {
                Type4->core_count_2 = LAZYBIOS_NOT_FOUND_U16;
                Type4->core_enabled_2 = LAZYBIOS_NOT_FOUND_U16;
                Type4->thread_count_2 = LAZYBIOS_NOT_FOUND_U16;
            }

            if (ISVERPLUS(ctx, 3, 6)) {
                if (len >=THREAD_ENABLED + sizeof(uint16_t)) {
                    memcpy(&Type4->thread_enabled, p + THREAD_ENABLED, sizeof(uint16_t));
                } else {
                    Type4->thread_enabled = LAZYBIOS_NOT_FOUND_U16;
                }
            } else {
                Type4->thread_enabled = LAZYBIOS_NOT_FOUND_U16;
            }

            if (ISVERPLUS(ctx, 3, 8)) {
                if (len >=SOCKET_TYPE) Type4->socket_type = DMIString(p, len, p[SOCKET_TYPE], end);
                if (!Type4->socket_type) Type4->socket_type = strdup(LAZYBIOS_NOT_FOUND_STR);
            } else {
                Type4->socket_type = strdup(LAZYBIOS_NOT_FOUND_STR);
            }

            return Type4;
        }
        p = DMINext(p , end);
    }
    return LAZYBIOS_NULL;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders

// Processor Family
const char* lazybiosType4ProcessorFamilyStr(uint16_t family) { // I do not know if everything here is added as per DMTF docs because I asked AI to extract the fields since I wans't gonna write this whole thing by hand, it will be checked though.
    switch (family) {
        case PROC_FAMILY_OTHER:                                return "Other";
        case PROC_FAMILY_UNKNOWN:                              return "Unknown";
        case PROC_FAMILY_8086:                                 return "8086";
        case PROC_FAMILY_80286:                                return "80286";
        case PROC_FAMILY_INTEL386:                             return "Intel386";
        case PROC_FAMILY_INTEL486:                             return "Intel486";
        case PROC_FAMILY_8087:                                 return "8087";
        case PROC_FAMILY_80287:                                return "80287";
        case PROC_FAMILY_80387:                                return "80387";
        case PROC_FAMILY_80487:                                return "80487";
        case PROC_FAMILY_INTEL_PENTIUM:                        return "Intel Pentium";
        case PROC_FAMILY_PENTIUM_PRO:                          return "Pentium Pro";
        case PROC_FAMILY_PENTIUM_II:                           return "Pentium II";
        case PROC_FAMILY_PENTIUM_WITH_MMX:                     return "Pentium with MMX";
        case PROC_FAMILY_INTEL_CELERON:                        return "Intel Celeron";
        case PROC_FAMILY_PENTIUM_II_XEON:                      return "Pentium II Xeon";
        case PROC_FAMILY_PENTIUM_III:                          return "Pentium III";
        case PROC_FAMILY_M1:                                   return "M1 Family";
        case PROC_FAMILY_M2:                                   return "M2 Family";
        case PROC_FAMILY_INTEL_CELERON_M:                      return "Intel Celeron M";
        case PROC_FAMILY_INTEL_PENTIUM_4_HT:                   return "Intel Pentium 4 HT";
        case PROC_FAMILY_INTEL_PROCESSOR:                      return "Intel Processor";
        case PROC_FAMILY_AMD_DURON:                            return "AMD Duron";
        case PROC_FAMILY_K5:                                   return "K5 Family";
        case PROC_FAMILY_K6:                                   return "K6 Family";
        case PROC_FAMILY_K6_2:                                 return "K6-2";
        case PROC_FAMILY_K6_3:                                 return "K6-3";
        case PROC_FAMILY_AMD_ATHLON:                           return "AMD Athlon";
        case PROC_FAMILY_AMD29000:                             return "AMD29000 Family";
        case PROC_FAMILY_K6_2_PLUS:                            return "K6-2+";
        case PROC_FAMILY_POWER_PC:                             return "Power PC Family";
        case PROC_FAMILY_POWER_PC_601:                         return "Power PC 601";
        case PROC_FAMILY_POWER_PC_603:                         return "Power PC 603";
        case PROC_FAMILY_POWER_PC_603_PLUS:                    return "Power PC 603+";
        case PROC_FAMILY_POWER_PC_604:                         return "Power PC 604";
        case PROC_FAMILY_POWER_PC_620:                         return "Power PC 620";
        case PROC_FAMILY_POWER_PC_X704:                        return "Power PC x704";
        case PROC_FAMILY_POWER_PC_750:                         return "Power PC 750";
        case PROC_FAMILY_INTEL_CORE_DUO:                       return "Intel Core Duo";
        case PROC_FAMILY_INTEL_CORE_DUO_MOBILE:                return "Intel Core Duo mobile";
        case PROC_FAMILY_INTEL_CORE_SOLO_MOBILE:               return "Intel Core Solo mobile";
        case PROC_FAMILY_INTEL_ATOM:                           return "Intel Atom";
        case PROC_FAMILY_INTEL_CORE_M:                         return "Intel Core M";
        case PROC_FAMILY_INTEL_CORE_M3:                        return "Intel Core m3";
        case PROC_FAMILY_INTEL_CORE_M5:                        return "Intel Core m5";
        case PROC_FAMILY_INTEL_CORE_M7:                        return "Intel Core m7";
        case PROC_FAMILY_ALPHA:                                return "Alpha Family";
        case PROC_FAMILY_ALPHA_21064:                          return "Alpha 21064";
        case PROC_FAMILY_ALPHA_21066:                          return "Alpha 21066";
        case PROC_FAMILY_ALPHA_21164:                          return "Alpha 21164";
        case PROC_FAMILY_ALPHA_21164PC:                        return "Alpha 21164PC";
        case PROC_FAMILY_ALPHA_21164A:                         return "Alpha 21164a";
        case PROC_FAMILY_ALPHA_21264:                          return "Alpha 21264";
        case PROC_FAMILY_ALPHA_21364:                          return "Alpha 21364";
        case PROC_FAMILY_AMD_TURION_II_ULTRA_DUAL_CORE_MOBILE: return "AMD Turion II Ultra Dual-Core Mobile M";
        case PROC_FAMILY_AMD_TURION_II_DUAL_CORE_MOBILE:       return "AMD Turion II Dual-Core Mobile M";
        case PROC_FAMILY_AMD_ATHLON_II_DUAL_CORE_M:            return "AMD Athlon II Dual-Core M";
        case PROC_FAMILY_AMD_OPTERON_6100:                     return "AMD Opteron 6100 Series";
        case PROC_FAMILY_AMD_OPTERON_4100:                     return "AMD Opteron 4100 Series";
        case PROC_FAMILY_AMD_OPTERON_6200:                     return "AMD Opteron 6200 Series";
        case PROC_FAMILY_AMD_OPTERON_4200:                     return "AMD Opteron 4200 Series";
        case PROC_FAMILY_AMD_FX:                               return "AMD FX Series";
        case PROC_FAMILY_MIPS:                                 return "MIPS Family";
        case PROC_FAMILY_MIPS_R4000:                           return "MIPS R4000";
        case PROC_FAMILY_MIPS_R4200:                           return "MIPS R4200";
        case PROC_FAMILY_MIPS_R4400:                           return "MIPS R4400";
        case PROC_FAMILY_MIPS_R4600:                           return "MIPS R4600";
        case PROC_FAMILY_MIPS_R10000:                          return "MIPS R10000";
        case PROC_FAMILY_AMD_C_SERIES:                         return "AMD C-Series";
        case PROC_FAMILY_AMD_E_SERIES:                         return "AMD E-Series";
        case PROC_FAMILY_AMD_A_SERIES:                         return "AMD A-Series";
        case PROC_FAMILY_AMD_G_SERIES:                         return "AMD G-Series";
        case PROC_FAMILY_AMD_Z_SERIES:                         return "AMD Z-Series";
        case PROC_FAMILY_AMD_R_SERIES:                         return "AMD R-Series";
        case PROC_FAMILY_AMD_OPTERON_4300:                     return "AMD Opteron 4300 Series";
        case PROC_FAMILY_AMD_OPTERON_6300:                     return "AMD Opteron 6300 Series";
        case PROC_FAMILY_AMD_OPTERON_3300:                     return "AMD Opteron 3300 Series";
        case PROC_FAMILY_AMD_FIREPRO:                          return "AMD FirePro Series";
        case PROC_FAMILY_SPARC:                                return "SPARC Family";
        case PROC_FAMILY_SUPERSPARC:                           return "SuperSPARC";
        case PROC_FAMILY_MICROSPARC_II:                        return "microSPARC II";
        case PROC_FAMILY_MICROSPARC_IIEP:                      return "microSPARC IIep";
        case PROC_FAMILY_ULTRASPARC:                           return "UltraSPARC";
        case PROC_FAMILY_ULTRASPARC_II:                        return "UltraSPARC II";
        case PROC_FAMILY_ULTRASPARC_III:                       return "UltraSPARC III";
        case PROC_FAMILY_ULTRASPARC_III_2:                     return "UltraSPARC III";
        case PROC_FAMILY_ULTRASPARC_IIII:                      return "UltraSPARC IIIi";
        case PROC_FAMILY_68040:                                return "68040 Family";
        case PROC_FAMILY_68XXX:                                return "68xxx";
        case PROC_FAMILY_68000:                                return "68000";
        case PROC_FAMILY_68010:                                return "68010";
        case PROC_FAMILY_68020:                                return "68020";
        case PROC_FAMILY_68030:                                return "68030";
        case PROC_FAMILY_AMD_ATHLON_X4_QUAD_CORE:              return "AMD Athlon X4 Quad-Core";
        case PROC_FAMILY_AMD_OPTERON_X1000:                    return "AMD Opteron X1000 Series";
        case PROC_FAMILY_AMD_OPTERON_X2000_APU:                return "AMD Opteron X2000 Series APU";
        case PROC_FAMILY_AMD_OPTERON_A_SERIES:                 return "AMD Opteron A-Series";
        case PROC_FAMILY_AMD_OPTERON_X3000_APU:                return "AMD Opteron X3000 Series APU";
        case PROC_FAMILY_AMD_ZEN:                              return "AMD Zen";
        case PROC_FAMILY_HOBBIT:                               return "Hobbit Family";
        case PROC_FAMILY_CRUSOE_TM5000:                        return "Crusoe TM5000 Family";
        case PROC_FAMILY_CRUSOE_TM3000:                        return "Crusoe TM3000 Family";
        case PROC_FAMILY_EFFICEON_TM8000:                      return "Efficeon TM8000 Family";
        case PROC_FAMILY_WEITEK:                               return "Weitek";
        case PROC_FAMILY_ITANIUM:                              return "Itanium";
        case PROC_FAMILY_AMD_ATHLON_64:                        return "AMD Athlon 64";
        case PROC_FAMILY_AMD_OPTERON:                          return "AMD Opteron";
        case PROC_FAMILY_AMD_SEMPRON:                          return "AMD Sempron";
        case PROC_FAMILY_AMD_TURION_64_MOBILE:                 return "AMD Turion 64 Mobile";
        case PROC_FAMILY_DUAL_CORE_AMD_OPTERON:                return "Dual-Core AMD Opteron";
        case PROC_FAMILY_AMD_ATHLON_64_X2:                     return "AMD Athlon 64 X2 Dual-Core";
        case PROC_FAMILY_AMD_TURION_64_X2:                     return "AMD Turion 64 X2 Mobile";
        case PROC_FAMILY_QUAD_CORE_AMD_OPTERON:                return "Quad-Core AMD Opteron";
        case PROC_FAMILY_THIRD_GEN_AMD_OPTERON:                return "Third-Generation AMD Opteron";
        case PROC_FAMILY_AMD_PHENOM_FX_QUAD_CORE:              return "AMD Phenom FX Quad-Core";
        case PROC_FAMILY_AMD_PHENOM_X4_QUAD_CORE:              return "AMD Phenom X4 Quad-Core";
        case PROC_FAMILY_AMD_PHENOM_X2_DUAL_CORE:              return "AMD Phenom X2 Dual-Core";
        case PROC_FAMILY_AMD_ATHLON_X2_DUAL_CORE:              return "AMD Athlon X2 Dual-Core";
        case PROC_FAMILY_PA_RISC:                              return "PA-RISC Family";
        case PROC_FAMILY_PA_RISC_8500:                         return "PA-RISC 8500";
        case PROC_FAMILY_PA_RISC_8000:                         return "PA-RISC 8000";
        case PROC_FAMILY_PA_RISC_7300LC:                       return "PA-RISC 7300LC";
        case PROC_FAMILY_PA_RISC_7200:                         return "PA-RISC 7200";
        case PROC_FAMILY_PA_RISC_7100LC:                       return "PA-RISC 7100LC";
        case PROC_FAMILY_PA_RISC_7100:                         return "PA-RISC 7100";
        case PROC_FAMILY_V30:                                  return "V30 Family";
        case PROC_FAMILY_QUAD_CORE_INTEL_XEON_3200:            return "Quad-Core Intel Xeon 3200 Series";
        case PROC_FAMILY_DUAL_CORE_INTEL_XEON_3000:            return "Dual-Core Intel Xeon 3000 Series";
        case PROC_FAMILY_QUAD_CORE_INTEL_XEON_5300:            return "Quad-Core Intel Xeon 5300 Series";
        case PROC_FAMILY_DUAL_CORE_INTEL_XEON_5100:            return "Dual-Core Intel Xeon 5100 Series";
        case PROC_FAMILY_DUAL_CORE_INTEL_XEON_5000:            return "Dual-Core Intel Xeon 5000 Series";
        case PROC_FAMILY_DUAL_CORE_INTEL_XEON_LV:              return "Dual-Core Intel Xeon LV";
        case PROC_FAMILY_DUAL_CORE_INTEL_XEON_ULV:             return "Dual-Core Intel Xeon ULV";
        case PROC_FAMILY_DUAL_CORE_INTEL_XEON_7100:            return "Dual-Core Intel Xeon 7100 Series";
        case PROC_FAMILY_QUAD_CORE_INTEL_XEON_5400:            return "Quad-Core Intel Xeon 5400 Series";
        case PROC_FAMILY_QUAD_CORE_INTEL_XEON:                 return "Quad-Core Intel Xeon";
        case PROC_FAMILY_DUAL_CORE_INTEL_XEON_5200:            return "Dual-Core Intel Xeon 5200 Series";
        case PROC_FAMILY_DUAL_CORE_INTEL_XEON_7200:            return "Dual-Core Intel Xeon 7200 Series";
        case PROC_FAMILY_QUAD_CORE_INTEL_XEON_7300:            return "Quad-Core Intel Xeon 7300 Series";
        case PROC_FAMILY_QUAD_CORE_INTEL_XEON_7400:            return "Quad-Core Intel Xeon 7400 Series";
        case PROC_FAMILY_MULTI_CORE_INTEL_XEON_7400:           return "Multi-Core Intel Xeon 7400 Series";
        case PROC_FAMILY_PENTIUM_III_XEON:                     return "Pentium III Xeon";
        case PROC_FAMILY_PENTIUM_III_SPEEDSTEP:                return "Pentium III with SpeedStep";
        case PROC_FAMILY_PENTIUM_4:                            return "Pentium 4";
        case PROC_FAMILY_INTEL_XEON:                           return "Intel Xeon";
        case PROC_FAMILY_AS400:                                return "AS400 Family";
        case PROC_FAMILY_INTEL_XEON_MP:                        return "Intel Xeon MP";
        case PROC_FAMILY_AMD_ATHLON_XP:                        return "AMD Athlon XP";
        case PROC_FAMILY_AMD_ATHLON_MP:                        return "AMD Athlon MP";
        case PROC_FAMILY_INTEL_ITANIUM_2:                      return "Intel Itanium 2";
        case PROC_FAMILY_INTEL_PENTIUM_M:                      return "Intel Pentium M";
        case PROC_FAMILY_INTEL_CELERON_D:                      return "Intel Celeron D";
        case PROC_FAMILY_INTEL_PENTIUM_D:                      return "Intel Pentium D";
        case PROC_FAMILY_INTEL_PENTIUM_EXTREME:                return "Intel Pentium Extreme Edition";
        case PROC_FAMILY_INTEL_CORE_SOLO:                      return "Intel Core Solo";
        case PROC_FAMILY_INTEL_CORE_2_DUO:                     return "Intel Core 2 Duo";
        case PROC_FAMILY_INTEL_CORE_2_SOLO:                    return "Intel Core 2 Solo";
        case PROC_FAMILY_INTEL_CORE_2_EXTREME:                 return "Intel Core 2 Extreme";
        case PROC_FAMILY_INTEL_CORE_2_QUAD:                    return "Intel Core 2 Quad";
        case PROC_FAMILY_INTEL_CORE_2_EXTREME_MOBILE:          return "Intel Core 2 Extreme mobile";
        case PROC_FAMILY_INTEL_CORE_2_DUO_MOBILE:              return "Intel Core 2 Duo mobile";
        case PROC_FAMILY_INTEL_CORE_2_SOLO_MOBILE:             return "Intel Core 2 Solo mobile";
        case PROC_FAMILY_INTEL_CORE_I7:                        return "Intel Core i7";
        case PROC_FAMILY_DUAL_CORE_INTEL_CELERON:              return "Dual-Core Intel Celeron";
        case PROC_FAMILY_IBM390:                               return "IBM390 Family";
        case PROC_FAMILY_G4:                                   return "G4";
        case PROC_FAMILY_G5:                                   return "G5";
        case PROC_FAMILY_ESA_390_G6:                           return "ESA/390 G6";
        case PROC_FAMILY_Z_ARCHITECTURE:                       return "z/Architecture base";
        case PROC_FAMILY_INTEL_CORE_I5:                        return "Intel Core i5";
        case PROC_FAMILY_INTEL_CORE_I3:                        return "Intel Core i3";
        case PROC_FAMILY_INTEL_CORE_I9:                        return "Intel Core i9";
        case PROC_FAMILY_INTEL_XEON_D:                         return "Intel Xeon D";
        case PROC_FAMILY_VIA_C7_M:                             return "VIA C7-M";
        case PROC_FAMILY_VIA_C7_D:                             return "VIA C7-D";
        case PROC_FAMILY_VIA_C7:                               return "VIA C7";
        case PROC_FAMILY_VIA_EDEN:                             return "VIA Eden";
        case PROC_FAMILY_MULTI_CORE_INTEL_XEON:                return "Multi-Core Intel Xeon";
        case PROC_FAMILY_DUAL_CORE_INTEL_XEON_3XXX:            return "Dual-Core Intel Xeon 3xxx Series";
        case PROC_FAMILY_QUAD_CORE_INTEL_XEON_3XXX:            return "Quad-Core Intel Xeon 3xxx Series";
        case PROC_FAMILY_VIA_NANO:                             return "VIA Nano";
        case PROC_FAMILY_DUAL_CORE_INTEL_XEON_5XXX:            return "Dual-Core Intel Xeon 5xxx Series";
        case PROC_FAMILY_QUAD_CORE_INTEL_XEON_5XXX:            return "Quad-Core Intel Xeon 5xxx Series";
        case PROC_FAMILY_DUAL_CORE_INTEL_XEON_7XXX:            return "Dual-Core Intel Xeon 7xxx Series";
        case PROC_FAMILY_QUAD_CORE_INTEL_XEON_7XXX:            return "Quad-Core Intel Xeon 7xxx Series";
        case PROC_FAMILY_MULTI_CORE_INTEL_XEON_7XXX:           return "Multi-Core Intel Xeon 7xxx Series";
        case PROC_FAMILY_MULTI_CORE_INTEL_XEON_3400:           return "Multi-Core Intel Xeon 3400 Series";
        case PROC_FAMILY_AMD_OPTERON_3000:                     return "AMD Opteron 3000 Series";
        case PROC_FAMILY_AMD_SEMPRON_II:                       return "AMD Sempron II";
        case PROC_FAMILY_EMBEDDED_AMD_OPTERON_QUAD_CORE:       return "Embedded AMD Opteron Quad-Core";
        case PROC_FAMILY_AMD_PHENOM_TRIPLE_CORE:               return "AMD Phenom Triple-Core";
        case PROC_FAMILY_AMD_TURION_ULTRA_DUAL_CORE_MOBILE:    return "AMD Turion Ultra Dual-Core Mobile";
        case PROC_FAMILY_AMD_TURION_DUAL_CORE_MOBILE:          return "AMD Turion Dual-Core Mobile";
        case PROC_FAMILY_AMD_ATHLON_DUAL_CORE:                 return "AMD Athlon Dual-Core";
        case PROC_FAMILY_AMD_SEMPRON_SI:                       return "AMD Sempron SI";
        case PROC_FAMILY_AMD_PHENOM_II:                        return "AMD Phenom II";
        case PROC_FAMILY_AMD_ATHLON_II:                        return "AMD Athlon II";
        case PROC_FAMILY_SIX_CORE_AMD_OPTERON:                 return "Six-Core AMD Opteron";
        case PROC_FAMILY_AMD_SEMPRON_M:                        return "AMD Sempron M";
        case PROC_FAMILY_I860:                                 return "i860";
        case PROC_FAMILY_I960:                                 return "i960";
        case PROC_FAMILY_INDICATOR_FROM_FAMILY2:               return "Use Processor Family 2";
        case PROC_FAMILY_ARMV7:                                return "ARMv7";
        case PROC_FAMILY_ARMV8:                                return "ARMv8";
        case PROC_FAMILY_ARMV9:                                return "ARMv9";
        case PROC_FAMILY_SH_3:                                 return "SH-3";
        case PROC_FAMILY_SH_4:                                 return "SH-4";
        case PROC_FAMILY_ARM:                                  return "ARM";
        case PROC_FAMILY_STRONGARM:                            return "StrongARM";
        case PROC_FAMILY_6X86:                                 return "6x86";
        case PROC_FAMILY_MEDIAGX:                              return "MediaGX";
        case PROC_FAMILY_MII:                                  return "MII";
        case PROC_FAMILY_WINCHIP:                              return "WinChip";
        case PROC_FAMILY_DSP:                                  return "DSP";
        case PROC_FAMILY_VIDEO_PROCESSOR:                      return "Video Processor";
        case PROC_FAMILY_RISC_V_RV32:                          return "RISC-V RV32";
        case PROC_FAMILY_RISC_V_RV64:                          return "RISC-V RV64";
        case PROC_FAMILY_RISC_V_RV128:                         return "RISC-V RV128";
        case PROC_FAMILY_LOONGARCH:                            return "LoongArch";
        case PROC_FAMILY_LOONGSON_1:                           return "Loongson 1";
        case PROC_FAMILY_LOONGSON_2:                           return "Loongson 2";
        case PROC_FAMILY_LOONGSON_3:                           return "Loongson 3";
        case PROC_FAMILY_LOONGSON_2K:                          return "Loongson 2K";
        case PROC_FAMILY_LOONGSON_3A:                          return "Loongson 3A";
        case PROC_FAMILY_LOONGSON_3B:                          return "Loongson 3B";
        case PROC_FAMILY_LOONGSON_3C:                          return "Loongson 3C";
        case PROC_FAMILY_LOONGSON_3D:                          return "Loongson 3D";
        case PROC_FAMILY_LOONGSON_3E:                          return "Loongson 3E";
        case PROC_FAMILY_DUAL_CORE_LOONGSON_2K_2XXX:           return "Dual-Core Loongson 2K 2xxx Series";
        case PROC_FAMILY_QUAD_CORE_LOONGSON_3A_5XXX:           return "Quad-Core Loongson 3A 5xxx Series";
        case PROC_FAMILY_MULTI_CORE_LOONGSON_3A_5XXX:          return "Multi-Core Loongson 3A 5xxx Series";
        case PROC_FAMILY_QUAD_CORE_LOONGSON_3B_5XXX:           return "Quad-Core Loongson 3B 5xxx Series";
        case PROC_FAMILY_MULTI_CORE_LOONGSON_3B_5XXX:          return "Multi-Core Loongson 3B 5xxx Series";
        case PROC_FAMILY_MULTI_CORE_LOONGSON_3C_5XXX:          return "Multi-Core Loongson 3C 5xxx Series";
        case PROC_FAMILY_MULTI_CORE_LOONGSON_3D_5XXX:          return "Multi-Core Loongson 3D 5xxx Series";
        case PROC_FAMILY_INTEL_CORE_3:                         return "Intel Core 3";
        case PROC_FAMILY_INTEL_CORE_5:                         return "Intel Core 5";
        case PROC_FAMILY_INTEL_CORE_7:                         return "Intel Core 7";
        case PROC_FAMILY_INTEL_CORE_9:                         return "Intel Core 9";
        case PROC_FAMILY_INTEL_CORE_ULTRA_3:                   return "Intel Core Ultra 3";
        case PROC_FAMILY_INTEL_CORE_ULTRA_5:                   return "Intel Core Ultra 5";
        case PROC_FAMILY_INTEL_CORE_ULTRA_7:                   return "Intel Core Ultra 7";
        case PROC_FAMILY_INTEL_CORE_ULTRA_9:                   return "Intel Core Ultra 9";
        default:                                               return "Unknown Processor Family";
    }
}

// Processor Type
const char* lazybiosType4SocketTypeStr(uint8_t type) {
    switch(type) {
        case SOCKET_TYPE_OTHER:          return "Other";
        case SOCKET_TYPE_UNKNOWN:        return "Unknown";
        case SOCKET_TYPE_DAUGHTER_BOARD: return "Daughter Board";
        case SOCKET_TYPE_ZIF_SOCKET:     return "ZIF Socket";
        case SOCKET_TYPE_PIGGY_BACK:     return "Replaceable Piggy Back";
        case SOCKET_TYPE_NONE:           return "None";
        case SOCKET_TYPE_LIF_SOCKET:     return "LIF Socket";
        case SOCKET_TYPE_SLOT_1:         return "Slot 1";
        case SOCKET_TYPE_SLOT_2:         return "Slot 2";
        case SOCKET_TYPE_370_PIN:        return "370-pin socket";
        case SOCKET_TYPE_SLOT_A:         return "Slot A";
        case SOCKET_TYPE_SLOT_M:         return "Slot M";
        case SOCKET_TYPE_423:            return "423";
        case SOCKET_TYPE_SOCKET_A:       return "A (462)";
        case SOCKET_TYPE_478:            return "478";
        case SOCKET_TYPE_754:            return "754";
        case SOCKET_TYPE_940:            return "940";
        case SOCKET_TYPE_939:            return "939";
        case SOCKET_TYPE_MPGA604:        return "mPGA604";
        case SOCKET_TYPE_LGA771:         return "LGA771";
        case SOCKET_TYPE_LGA775:         return "LGA775";
        case SOCKET_TYPE_S1:             return "S1";
        case SOCKET_TYPE_AM2:            return "AM2";
        case SOCKET_TYPE_F_1207:         return "F (1207)";
        case SOCKET_TYPE_LGA1366:        return "LGA1366";
        case SOCKET_TYPE_G34:            return "G34";
        case SOCKET_TYPE_AM3:            return "AM3";
        case SOCKET_TYPE_C32:            return "C32";
        case SOCKET_TYPE_LGA1156:        return "LGA1156";
        case SOCKET_TYPE_LGA1567:        return "LGA1567";
        case SOCKET_TYPE_PGA988A:        return "PGA988A";
        case SOCKET_TYPE_BGA1288:        return "BGA1288";
        case SOCKET_TYPE_RPGA988B:       return "rPGA988B";
        case SOCKET_TYPE_BGA1023:        return "BGA1023";
        case SOCKET_TYPE_BGA1224:        return "BGA1224";
        case SOCKET_TYPE_LGA1155:        return "LGA1155";
        case SOCKET_TYPE_LGA1356:        return "LGA1356";
        case SOCKET_TYPE_LGA2011:        return "LGA2011";
        case SOCKET_TYPE_FS1:            return "FS1";
        case SOCKET_TYPE_FS2:            return "FS2";
        case SOCKET_TYPE_FM1:            return "FM1";
        case SOCKET_TYPE_FM2:            return "FM2";
        case SOCKET_TYPE_LGA2011_3:      return "LGA2011-3";
        case SOCKET_TYPE_LGA1356_3:      return "LGA1356-3";
        case SOCKET_TYPE_LGA1150:        return "LGA1150";
        case SOCKET_TYPE_BGA1168:        return "BGA1168";
        case SOCKET_TYPE_BGA1234:        return "BGA1234";
        case SOCKET_TYPE_BGA1364:        return "BGA1364";
        case SOCKET_TYPE_AM4:            return "AM4";
        case SOCKET_TYPE_LGA1151:        return "LGA1151";
        case SOCKET_TYPE_BGA1356:        return "BGA1356";
        case SOCKET_TYPE_BGA1440:        return "BGA1440";
        case SOCKET_TYPE_BGA1515:        return "BGA1515";
        case SOCKET_TYPE_LGA3647_1:      return "LGA3647-1";
        case SOCKET_TYPE_SP3:            return "SP3";
        case SOCKET_TYPE_SP3R2:          return "SP3r2";
        case SOCKET_TYPE_LGA2066:        return "LGA2066";
        case SOCKET_TYPE_BGA1392:        return "BGA1392";
        case SOCKET_TYPE_BGA1510:        return "BGA1510";
        case SOCKET_TYPE_BGA1528:        return "BGA1528";
        case SOCKET_TYPE_LGA4189:        return "LGA4189";
        case SOCKET_TYPE_LGA1200:        return "LGA1200";
        case SOCKET_TYPE_LGA4677:        return "LGA4677";
        case SOCKET_TYPE_LGA1700:        return "LGA1700";
        case SOCKET_TYPE_BGA1744:        return "BGA1744";
        case SOCKET_TYPE_BGA1781:        return "BGA1781";
        case SOCKET_TYPE_BGA1211:        return "BGA1211";
        case SOCKET_TYPE_BGA2422:        return "BGA2422";
        case SOCKET_TYPE_LGA1211:        return "LGA1211";
        case SOCKET_TYPE_LGA2422:        return "LGA2422";
        case SOCKET_TYPE_LGA5773:        return "LGA5773";
        case SOCKET_TYPE_BGA5773:        return "BGA5773";
        case SOCKET_TYPE_AM5:            return "AM5";
        case SOCKET_TYPE_SP5:            return "SP5";
        case SOCKET_TYPE_SP6:            return "SP6";
        case SOCKET_TYPE_BGA883:         return "BGA883";
        case SOCKET_TYPE_BGA1190:        return "BGA1190";
        case SOCKET_TYPE_BGA4129:        return "BGA4129";
        case SOCKET_TYPE_LGA4710:        return "LGA4710";
        case SOCKET_TYPE_LGA7529:        return "LGA7529";
        case SOCKET_TYPE_BGA1964:        return "BGA1964";
        case SOCKET_TYPE_BGA1792:        return "BGA1792";
        case SOCKET_TYPE_BGA2049:        return "BGA2049";
        case SOCKET_TYPE_BGA2551:        return "BGA2551";
        case SOCKET_TYPE_LGA1851:        return "LGA1851";
        case SOCKET_TYPE_BGA2114:        return "BGA2114";
        case SOCKET_TYPE_BGA2833:        return "BGA2833";
        case SOCKET_TYPE_USE_STRING:     return "No Valid Offset available";
        default:                         return "Unknown Prosecutor Socket Type";
    }
}

// Processor Characteristics
const char* lazybiosType4CharacteristicsStr(uint16_t characteristics) {
    _Thread_local static char buf[256];
    size_t len = 0;
    buf[0] = '\0';

    if (characteristics & (1 << 1))  len += snprintf(buf + len, sizeof(buf) - len, "Unknown ");
    if (characteristics & (1 << 2))  len += snprintf(buf + len, sizeof(buf) - len, "64-bit Capable, ");
    if (characteristics & (1 << 3))  len += snprintf(buf + len, sizeof(buf) - len, "Multi-Core, ");
    if (characteristics & (1 << 4))  len += snprintf(buf + len, sizeof(buf) - len, "Hardware Thread, ");
    if (characteristics & (1 << 5))  len += snprintf(buf + len, sizeof(buf) - len, "Execute Protection, ");
    if (characteristics & (1 << 6))  len += snprintf(buf + len, sizeof(buf) - len, "Enhanced Virtualization, ");
    if (characteristics & (1 << 7))  len += snprintf(buf + len, sizeof(buf) - len, "Power/Performance Control, ");
    if (characteristics & (1 << 8))  len += snprintf(buf + len, sizeof(buf) - len, "128-bit Capable, ");
    if (characteristics & (1 << 9))  len += snprintf(buf + len, sizeof(buf) - len, "Arm64 SoC ID. ");

    if (len == 0) return "None";

    if (len >= 2) buf[len - 2] = '\0';

    return buf;
}

// Processor type
const char* lazybiosType4TypeStr(uint8_t type) {
    switch(type) {
        case PROC_TYPE_OTHER:             return "Other";
        case PROC_TYPE_UNKNOWN:           return "Unknown";
        case PROC_TYPE_CENTRAL_PROCESSOR: return "Central Processor";
        case PROC_TYPE_MATH_PROCESSOR:    return "Math Processor";
        case PROC_TYPE_DSP_PROCESSOR:     return "DSP Processor";
        case PROC_TYPE_VIDEO_PROCESSOR:   return "Video Processor";
        default:                          return "Unknown Processor Type";
    }
}

// Processor Status
const char* lazybiosType4StatusStr(uint8_t status) {
    _Thread_local static char buf[64];
    size_t len = 0;
    buf[0] = '\0';

    if (status & (1 << 6)) len += snprintf(buf + len, sizeof(buf) - len, "Socket Populated, ");
    else len += snprintf(buf + len, sizeof(buf) - len, "Socket Unpopulated, ");

    switch (status & 0x07) {
        case 0x0: len += snprintf(buf + len, sizeof(buf) - len, "Unknown"); break;
        case 0x1: len += snprintf(buf + len, sizeof(buf) - len, "CPU Enabled"); break;
        case 0x2: len += snprintf(buf + len, sizeof(buf) - len, "Disabled by User"); break;
        case 0x3: len += snprintf(buf + len, sizeof(buf) - len, "Disabled by Firmware (POST Error)"); break;
        case 0x4: len += snprintf(buf + len, sizeof(buf) - len, "CPU Idle"); break;
        case 0x7: len += snprintf(buf + len, sizeof(buf) - len, "Other"); break;
        default: len += snprintf(buf + len, sizeof(buf) - len, "Reserved"); break;
    }

    return buf;
}

// End of Decoders

// Free Function
void lazybiosFreeType4(lazybiosType4_t* Type4) {
    if (!Type4) return;

    free(Type4->socket_designation);
    free(Type4->processor_manufacturer);
    free(Type4->processor_version);
    free(Type4->serial_number);
    free(Type4->asset_tag);
    free(Type4->part_number);
    free(Type4->socket_type);
    free(Type4);
}
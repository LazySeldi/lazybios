//
// All Decoders
//
#include "lazybios.h"
#include <string.h>

// Type 4 ( Processor Information )

// Processor Family ---------------------------
const char* lazybiosProcFamilyStr(uint8_t family) {
    switch(family) {
        case PROC_FAMILY_OTHER:                    return "Other";
        case PROC_FAMILY_UNKNOWN:                  return "Unknown";

        case PROC_FAMILY_INTEL_8086:               return "Intel 8086";
        case PROC_FAMILY_INTEL_80286:              return "Intel 80286";
        case PROC_FAMILY_INTEL_80386:              return "Intel 80386";
        case PROC_FAMILY_INTEL_80486:              return "Intel 80486";
        case PROC_FAMILY_INTEL_PENTIUM:            return "Intel Pentium";
        case PROC_FAMILY_INTEL_PENTIUM_PRO:        return "Pentium Pro";
        case PROC_FAMILY_INTEL_PENTIUM_II:         return "Pentium II";
        case PROC_FAMILY_INTEL_PENTIUM_MMX:        return "Pentium MMX";
        case PROC_FAMILY_INTEL_CELERON:            return "Intel Celeron";
        case PROC_FAMILY_PENTIUM_II_XEON:          return "Pentium II Xeon";
        case PROC_FAMILY_PENTIUM_III:              return "Pentium III";

        case PROC_FAMILY_INTEL_CELERON_M:          return "Intel Celeron M";
        case PROC_FAMILY_INTEL_PENTIUM_4_HT:       return "Intel Pentium 4 HT";
        case PROC_FAMILY_INTEL_PROCESSOR:          return "Intel Processor";
        case PROC_FAMILY_INTEL_CORE_DUO:           return "Intel Core Duo";
        case PROC_FAMILY_INTEL_CORE_DUO_MOBILE:    return "Intel Core Duo Mobile";
        case PROC_FAMILY_INTEL_CORE_SOLO_MOBILE:   return "Intel Core Solo Mobile";
        case PROC_FAMILY_INTEL_ATOM:               return "Intel Atom";
        case PROC_FAMILY_INTEL_CORE_M:             return "Intel Core M";
        case PROC_FAMILY_INTEL_CORE_M3:            return "Intel Core m3";
        case PROC_FAMILY_INTEL_CORE_M5:            return "Intel Core m5";
        case PROC_FAMILY_INTEL_CORE_M7:            return "Intel Core m7";

        case PROC_FAMILY_XEON_3200:                return "Quad-Core Intel Xeon 3200";
        case PROC_FAMILY_XEON_3000:                return "Dual-Core Intel Xeon 3000";
        case PROC_FAMILY_XEON_5300:                return "Quad-Core Intel Xeon 5300";
        case PROC_FAMILY_XEON_5100:                return "Dual-Core Intel Xeon 5100";
        case PROC_FAMILY_XEON_5000:                return "Dual-Core Intel Xeon 5000";
        case PROC_FAMILY_XEON_LV:                  return "Dual-Core Intel Xeon LV";
        case PROC_FAMILY_XEON_ULV:                 return "Dual-Core Intel Xeon ULV";
        case PROC_FAMILY_XEON_7100:                return "Dual-Core Intel Xeon 7100";
        case PROC_FAMILY_XEON_5400:                return "Quad-Core Intel Xeon 5400";
        case PROC_FAMILY_XEON_GENERIC:             return "Quad-Core Intel Xeon";
        case PROC_FAMILY_XEON_5200:                return "Dual-Core Intel Xeon 5200";
        case PROC_FAMILY_XEON_7200:                return "Dual-Core Intel Xeon 7200";
        case PROC_FAMILY_XEON_7300:                return "Quad-Core Intel Xeon 7300";
        case PROC_FAMILY_XEON_7400:                return "Quad-Core Intel Xeon 7400";
        case PROC_FAMILY_XEON_MULTI_7400:          return "Multi-Core Intel Xeon 7400";
        case PROC_FAMILY_PENTIUM_III_XEON:         return "Pentium III Xeon";
        case PROC_FAMILY_PENTIUM_III_SPEEDSTEP:    return "Pentium III SpeedStep";
        case PROC_FAMILY_PENTIUM_4:                return "Pentium 4";
        case PROC_FAMILY_INTEL_XEON:               return "Intel Xeon";
        case PROC_FAMILY_INTEL_XEON_MP:            return "Intel Xeon MP";
        case PROC_FAMILY_INTEL_PENTIUM_M:          return "Intel Pentium M";
        case PROC_FAMILY_INTEL_CELERON_D:          return "Intel Celeron D";
        case PROC_FAMILY_INTEL_PENTIUM_D:          return "Intel Pentium D";
        case PROC_FAMILY_INTEL_PENTIUM_EXTREME:    return "Intel Pentium Extreme Edition";
        case PROC_FAMILY_INTEL_CORE_SOLO:          return "Intel Core Solo";
        case PROC_FAMILY_INTEL_CORE_2_DUO:         return "Intel Core 2 Duo";
        case PROC_FAMILY_INTEL_CORE_2_SOLO:        return "Intel Core 2 Solo";
        case PROC_FAMILY_INTEL_CORE_2_EXTREME:     return "Intel Core 2 Extreme";
        case PROC_FAMILY_INTEL_CORE_2_QUAD:        return "Intel Core 2 Quad";
        case PROC_FAMILY_CORE_2_EXTREME_MOBILE:    return "Intel Core 2 Extreme Mobile";
        case PROC_FAMILY_CORE_2_DUO_MOBILE:        return "Intel Core 2 Duo Mobile";
        case PROC_FAMILY_CORE_2_SOLO_MOBILE:       return "Intel Core 2 Solo Mobile";
        case PROC_FAMILY_INTEL_CORE_I7:            return "Intel Core i7";
        case PROC_FAMILY_INTEL_DUAL_CELERON:       return "Dual-Core Intel Celeron";
        case PROC_FAMILY_INTEL_CORE_I5:            return "Intel Core i5";
        case PROC_FAMILY_INTEL_CORE_I3:            return "Intel Core i3";
        case PROC_FAMILY_INTEL_CORE_I9:            return "Intel Core i9";
        case PROC_FAMILY_INTEL_XEON_D:             return "Intel Xeon D";
        case PROC_FAMILY_INTEL_XEON_MULTI:         return "Multi-Core Intel Xeon";
        case PROC_FAMILY_XEON_DC_3XXX:             return "Dual-Core Intel Xeon 3xxx";
        case PROC_FAMILY_XEON_QC_3XXX:             return "Quad-Core Intel Xeon 3xxx";
        case PROC_FAMILY_XEON_DC_5XXX:             return "Dual-Core Intel Xeon 5xxx";
        case PROC_FAMILY_XEON_QC_5XXX:             return "Quad-Core Intel Xeon 5xxx";

        case PROC_FAMILY_AMD_DURON:                return "AMD Duron";
        case PROC_FAMILY_AMD_K5:                   return "AMD K5";
        case PROC_FAMILY_AMD_K6:                   return "AMD K6";
        case PROC_FAMILY_AMD_K6_2:                 return "AMD K6-2";
        case PROC_FAMILY_AMD_K6_3:                 return "AMD K6-3";
        case PROC_FAMILY_AMD_ATHLON:               return "AMD Athlon";
        case PROC_FAMILY_AMD_K6_2_PLUS:            return "AMD K6-2+";
        case PROC_FAMILY_AMD_TURION_II_ULTRA:      return "AMD Turion II Ultra Dual-Core Mobile M";
        case PROC_FAMILY_AMD_TURION_II:            return "AMD Turion II Dual-Core Mobile M";
        case PROC_FAMILY_AMD_ATHLON_II:            return "AMD Athlon II Dual-Core M";
        case PROC_FAMILY_AMD_OPTERON_6100:         return "AMD Opteron 6100";
        case PROC_FAMILY_AMD_OPTERON_4100:         return "AMD Opteron 4100";
        case PROC_FAMILY_AMD_OPTERON_6200:         return "AMD Opteron 6200";
        case PROC_FAMILY_AMD_OPTERON_4200:         return "AMD Opteron 4200";
        case PROC_FAMILY_AMD_FX:                   return "AMD FX";
        case PROC_FAMILY_AMD_C_SERIES:             return "AMD C-Series";
        case PROC_FAMILY_AMD_E_SERIES:             return "AMD E-Series";
        case PROC_FAMILY_AMD_A_SERIES:             return "AMD A-Series";
        case PROC_FAMILY_AMD_G_SERIES:             return "AMD G-Series";
        case PROC_FAMILY_AMD_Z_SERIES:             return "AMD Z-Series";
        case PROC_FAMILY_AMD_R_SERIES:             return "AMD R-Series";
        case PROC_FAMILY_AMD_OPTERON_4300:         return "AMD Opteron 4300";
        case PROC_FAMILY_AMD_OPTERON_6300:         return "AMD Opteron 6300";
        case PROC_FAMILY_AMD_OPTERON_3300:         return "AMD Opteron 3300";
        case PROC_FAMILY_AMD_FIREPRO:              return "AMD FirePro";
        case PROC_FAMILY_AMD_ATHLON_X4:            return "AMD Athlon X4 Quad-Core";
        case PROC_FAMILY_AMD_OPTERON_X1000:        return "AMD Opteron X1000";
        case PROC_FAMILY_AMD_OPTERON_X2000:        return "AMD Opteron X2000 APU";
        case PROC_FAMILY_AMD_OPTERON_A_SERIES:     return "AMD Opteron A-Series";
        case PROC_FAMILY_AMD_OPTERON_X3000:        return "AMD Opteron X3000 APU";
        case PROC_FAMILY_AMD_ZEN:                  return "AMD Zen";
        case PROC_FAMILY_AMD_ATHLON_64:            return "AMD Athlon 64";
        case PROC_FAMILY_AMD_OPTERON:              return "AMD Opteron";
        case PROC_FAMILY_AMD_SEMPRON:              return "AMD Sempron";
        case PROC_FAMILY_AMD_TURION_64_MOBILE:     return "AMD Turion 64 Mobile";
        case PROC_FAMILY_AMD_DUAL_OPTERON:         return "Dual-Core AMD Opteron";
        case PROC_FAMILY_AMD_ATHLON_64_X2:         return "AMD Athlon 64 X2 Dual-Core";
        case PROC_FAMILY_AMD_TURION_64_X2:         return "AMD Turion 64 X2 Mobile";
        case PROC_FAMILY_AMD_QUAD_OPTERON:         return "Quad-Core AMD Opteron";
        case PROC_FAMILY_AMD_3RD_OPTERON:          return "Third-Gen AMD Opteron";
        case PROC_FAMILY_AMD_PHENOM_FX:            return "AMD Phenom FX Quad-Core";
        case PROC_FAMILY_AMD_PHENOM_X4:            return "AMD Phenom X4 Quad-Core";
        case PROC_FAMILY_AMD_PHENOM_X2:            return "AMD Phenom X2 Dual-Core";
        case PROC_FAMILY_AMD_ATHLON_X2:            return "AMD Athlon X2 Dual-Core";
        case PROC_FAMILY_AMD_ATHLON_XP:            return "AMD Athlon XP";
        case PROC_FAMILY_AMD_ATHLON_MP:            return "AMD Athlon MP";

        case PROC_FAMILY_APPLE_M1:                 return "M1 Family";
        case PROC_FAMILY_APPLE_M2:                 return "M2 Family";

        case PROC_FAMILY_SPARC:                    return "SPARC";
        case PROC_FAMILY_SUPERSPARC:               return "SuperSPARC";
        case PROC_FAMILY_MICROSPARC_II:            return "microSPARC II";
        case PROC_FAMILY_MICROSPARC_IIEP:          return "microSPARC IIep";
        case PROC_FAMILY_ULTRASPARC:               return "UltraSPARC";
        case PROC_FAMILY_ULTRASPARC_II:            return "UltraSPARC II";
        case PROC_FAMILY_ULTRASPARC_III:           return "UltraSPARC Iii";
        case PROC_FAMILY_ULTRASPARC_IIIX:          return "UltraSPARC III";
        case PROC_FAMILY_ULTRASPARC_IIIi:          return "UltraSPARC IIIi";

        case PROC_FAMILY_VIA_C7_M:                 return "VIA C7-M";
        case PROC_FAMILY_VIA_C7_D:                 return "VIA C7-D";
        case PROC_FAMILY_VIA_C7:                   return "VIA C7";
        case PROC_FAMILY_VIA_EDEN:                 return "VIA Eden";
        case PROC_FAMILY_VIA_NANO:                 return "VIA Nano";

        default:                                   return "Unknown";
    }
}

// Processor Type
const char* lazybiosProcSocketTypeStr(uint8_t type) {
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
        case SOCKET_TYPE_USE_STRING:     return "Use string at 0x32";
        default:                         return "Unknown";
    }
}

// Processor Characteristics
const char* lazybiosProcCharacteristicsStr(uint16_t characteristics) {
    static char buf[256];
    buf[0] = '\0';

    if (characteristics & 0x0002) strcat(buf, "Not 64-bit Support, ");
    if (characteristics & 0x0004) strcat(buf, "64-bit Capable, ");
    if (characteristics & 0x0008) strcat(buf, "Multi-Core, ");
    if (characteristics & 0x0010) strcat(buf, "Hardware Thread, ");
    if (characteristics & 0x0020) strcat(buf, "Execute Protection, ");
    if (characteristics & 0x0040) strcat(buf, "Virtualization, ");
    if (characteristics & 0x0080) strcat(buf, "Power Control, ");

    if (buf[0] == '\0')
        return "None";

    buf[strlen(buf) - 2] = '\0';

    return buf;
}

// Processor type
const char* lazybiosProcTypeStr(uint8_t type) {
    switch(type) {
        case PROC_TYPE_OTHER:             return "Other";
        case PROC_TYPE_UNKNOWN:           return "Unknown";
        case PROC_TYPE_CENTRAL_PROCESSOR: return "Central Processor";
        case PROC_TYPE_MATH_PROCESSOR:    return "Math Processor";
        case PROC_TYPE_DSP_PROCESSOR:     return "DSP Processor";
        case PROC_TYPE_VIDEO_PROCESSOR:   return "Video Processor";
        default:                          return "Unknown";
    }
}

// Processor Status (lower 3 bits)
const char* lazybiosProcStatusStr(uint8_t status) {
    uint8_t cpu_status = status & PROC_STATUS_MASK;
    switch(cpu_status) {
        case PROC_STATUS_UNKNOWN:            return "Unknown";
        case PROC_STATUS_ENABLED:            return "Enabled";
        case PROC_STATUS_DISABLED_BY_USER:   return "Disabled by User";
        case PROC_STATUS_DISABLED_BY_BIOS:   return "Disabled by BIOS";
        case PROC_STATUS_IDLE:               return "Idle";
        case PROC_STATUS_OTHER:              return "Other";
        default:                             return "Reserved";
    }
}

// End of Type 4 -----------------------------


// Type 7 ( Cache Information ) ------------------

// Cache Type
const char* lazybiosCacheTypeStr(uint8_t cache_type) {
    switch(cache_type) {
        case CACHE_TYPE_OTHER:       return "Other";
        case CACHE_TYPE_UNKNOWN:     return "Unknown";
        case CACHE_TYPE_INSTRUCTION: return "Instruction";
        case CACHE_TYPE_DATA:        return "Data";
        case CACHE_TYPE_UNIFIED:     return "Unified";
        default:                     return "Unknown";
    }
}

// Cache ECC
const char* lazybiosCacheECCTypeStr(uint8_t ecc_type) {
    switch(ecc_type) {
        case CACHE_ECC_OTHER:       return "Other";
        case CACHE_ECC_UNKNOWN:     return "Unknown";
        case CACHE_ECC_NONE:        return "None";
        case CACHE_ECC_PARITY:      return "Parity";
        case CACHE_ECC_SINGLE_BIT:  return "Single-bit ECC";
        case CACHE_ECC_MULTI_BIT:   return "Multi-bit ECC";
        case CACHE_ECC_CRC:         return "CRC";
        default:                    return "Unknown";
    }
}

// Cache Associativity
const char* lazybiosCacheAssociativityStr(uint8_t associativity) {
    switch(associativity) {
        case CACHE_ASSOC_OTHER:            return "Other";
        case CACHE_ASSOC_UNKNOWN:          return "Unknown";
        case CACHE_ASSOC_DIRECT_MAPPED:    return "Direct Mapped";
        case CACHE_ASSOC_2_WAY:            return "2-way";
        case CACHE_ASSOC_4_WAY:            return "4-way";
        case CACHE_ASSOC_FULLY_ASSOC:      return "Fully Associative";
        case CACHE_ASSOC_8_WAY:            return "8-way";
        case CACHE_ASSOC_16_WAY:           return "16-way";
        case CACHE_ASSOC_12_WAY:           return "12-way";
        case CACHE_ASSOC_24_WAY:           return "24-way";
        case CACHE_ASSOC_32_WAY:           return "32-way";
        case CACHE_ASSOC_48_WAY:           return "48-way";
        case CACHE_ASSOC_64_WAY:           return "64-way";
        case CACHE_ASSOC_20_WAY:           return "20-way";
        default:                           return "Unknown";
    }
}

// End of Type 7 ----------------------------------


// Type 8 ( Port Connector ) --------------------------

// Port Connector Types
const char* lazybiosPortConnectorTypeStr(uint8_t connector_type) {
    switch(connector_type) {
        case CONNECTOR_TYPE_NONE:                    return "None";
        case CONNECTOR_TYPE_CENTRONICS:              return "Centronics";
        case CONNECTOR_TYPE_MINI_CENTRONICS:         return "Mini Centronics";
        case CONNECTOR_TYPE_PROPRIETARY:             return "Proprietary";
        case CONNECTOR_TYPE_DB25_MALE:               return "DB 25 Male";
        case CONNECTOR_TYPE_DB25_FEMALE:             return "DB 25 Female";
        case CONNECTOR_TYPE_DB15_MALE:               return "DB 15 Male";
        case CONNECTOR_TYPE_DB15_FEMALE:             return "DB 15 Female";
        case CONNECTOR_TYPE_DB9_MALE:                return "DB 9 Male";
        case CONNECTOR_TYPE_DB9_FEMALE:              return "DB 9 Female";
        case CONNECTOR_TYPE_RJ11:                    return "RJ 11";
        case CONNECTOR_TYPE_RJ45:                    return "RJ 45";
        case CONNECTOR_TYPE_50PIN_MINISCSI:          return "50 Pin Mini SCSI";
        case CONNECTOR_TYPE_MINIDIN:                 return "Mini DIN";
        case CONNECTOR_TYPE_MICRODIN:                return "Micro DIN";
        case CONNECTOR_TYPE_PS2:                     return "PS2";
        case CONNECTOR_TYPE_INFRARED:                return "Infrared";
        case CONNECTOR_TYPE_HP_HIL:                  return "HP HIL";
        case CONNECTOR_TYPE_USB:                     return "USB";
        case CONNECTOR_TYPE_SSA_SCSI:                return "SSA SCSI";
        case CONNECTOR_TYPE_CIRCULAR_DIN8_MALE:      return "Circular DIN-8 Male";
        case CONNECTOR_TYPE_CIRCULAR_DIN8_FEMALE:    return "Circular DIN-8 Female";
        case CONNECTOR_TYPE_ONBOARD_IDE:             return "On Board IDE";
        case CONNECTOR_TYPE_ONBOARD_FLOPPY:          return "On Board Floppy";
        case CONNECTOR_TYPE_9PIN_DUAL_INLINE:        return "9-pin Dual Inline (pin 10 cut)";
        case CONNECTOR_TYPE_25PIN_DUAL_INLINE:       return "25-pin Dual Inline (pin 26 cut)";
        case CONNECTOR_TYPE_50PIN_DUAL_INLINE:       return "50-pin Dual Inline";
        case CONNECTOR_TYPE_68PIN_DUAL_INLINE:       return "68-pin Dual Inline";
        case CONNECTOR_TYPE_ONBOARD_SOUND_CDROM:     return "On Board Sound Input from CD-ROM";
        case CONNECTOR_TYPE_MINICENTRONICS_14:       return "Mini-Centronics Type-14";
        case CONNECTOR_TYPE_MINICENTRONICS_26:       return "Mini-Centronics Type-26";
        case CONNECTOR_TYPE_MINI_JACK:               return "Mini-jack (headphones)";
        case CONNECTOR_TYPE_BNC:                     return "BNC";
        case CONNECTOR_TYPE_1394:                    return "1394";
        case CONNECTOR_TYPE_SAS_SATA:                return "SAS/SATA Plug Receptacle";
        case CONNECTOR_TYPE_USB_C:                   return "USB Type-C Receptacle";
        case CONNECTOR_TYPE_PC98:                    return "PC-98";
        case CONNECTOR_TYPE_PC98_HIRESO:             return "PC-98Hireso";
        case CONNECTOR_TYPE_PC_H98:                  return "PC-H98";
        case CONNECTOR_TYPE_PC98_NOTE:               return "PC-98Note";
        case CONNECTOR_TYPE_PC98_FULL:               return "PC-98Full";
        case CONNECTOR_TYPE_OTHER:                   return "Other";
        default:                                     return "Unknown";
    }
}

// Port Types
const char* lazybiosPortTypeStr(uint8_t port_type) {
    switch(port_type) {
        case PORT_TYPE_SPEC_NONE:                   return "None";
        case PORT_TYPE_SPEC_PARALLEL_XT_AT:         return "Parallel Port XT/AT Compatible";
        case PORT_TYPE_SPEC_PARALLEL_PS2:           return "Parallel Port PS/2";
        case PORT_TYPE_SPEC_PARALLEL_ECP:           return "Parallel Port ECP";
        case PORT_TYPE_SPEC_PARALLEL_EPP:           return "Parallel Port EPP";
        case PORT_TYPE_SPEC_PARALLEL_ECP_EPP:       return "Parallel Port ECP/EPP";
        case PORT_TYPE_SPEC_SERIAL_XT_AT:           return "Serial Port XT/AT Compatible";
        case PORT_TYPE_SPEC_SERIAL_16450:           return "Serial Port 16450 Compatible";
        case PORT_TYPE_SPEC_SERIAL_16550:           return "Serial Port 16550 Compatible";
        case PORT_TYPE_SPEC_SERIAL_16550A:          return "Serial Port 16550A Compatible";
        case PORT_TYPE_SPEC_SCSI:                   return "SCSI Port";
        case PORT_TYPE_SPEC_MIDI:                   return "MIDI Port";
        case PORT_TYPE_SPEC_JOYSTICK:               return "Joy Stick Port";
        case PORT_TYPE_SPEC_KEYBOARD:               return "Keyboard Port";
        case PORT_TYPE_SPEC_MOUSE:                  return "Mouse Port";
        case PORT_TYPE_SPEC_SSA_SCSI:               return "SSA SCSI";
        case PORT_TYPE_SPEC_USB:                    return "USB";
        case PORT_TYPE_SPEC_FIREWIRE:               return "FireWire (IEEE P1394)";
        case PORT_TYPE_SPEC_PCMCIA_I2:              return "PCMCIA Type I2";
        case PORT_TYPE_SPEC_PCMCIA_II:              return "PCMCIA Type II";
        case PORT_TYPE_SPEC_PCMCIA_III:             return "PCMCIA Type III";
        case PORT_TYPE_SPEC_CARDBUS:                return "Card Bus";
        case PORT_TYPE_SPEC_ACCESS_BUS:             return "Access Bus Port";
        case PORT_TYPE_SPEC_SCSI_II:                return "SCSI II";
        case PORT_TYPE_SPEC_SCSI_WIDE:              return "SCSI Wide";
        case PORT_TYPE_SPEC_PC98:                   return "PC-98";
        case PORT_TYPE_SPEC_PC98_HIRESO:            return "PC-98-Hireso";
        case PORT_TYPE_SPEC_PC_H98:                 return "PC-H98";
        case PORT_TYPE_SPEC_VIDEO:                  return "Video Port";
        case PORT_TYPE_SPEC_AUDIO:                  return "Audio Port";
        case PORT_TYPE_SPEC_MODEM:                  return "Modem Port";
        case PORT_TYPE_SPEC_NETWORK:                return "Network Port";
        case PORT_TYPE_SPEC_SATA:                   return "SATA";
        case PORT_TYPE_SPEC_SAS:                    return "SAS";
        case PORT_TYPE_SPEC_MFDP:                   return "MFDP (Multi-Function Display Port)";
        case PORT_TYPE_SPEC_THUNDERBOLT:            return "Thunderbolt";
        case PORT_TYPE_SPEC_8251_COMPATIBLE:        return "8251 Compatible";
        case PORT_TYPE_SPEC_8251_FIFO_COMPATIBLE:   return "8251 FIFO Compatible";
        case PORT_TYPE_SPEC_OTHER:                  return "Other";
        default:                                    return "Unknown";
    }
}

// End of Type 8 --------------------------------------


// Type 10 ( Onboard Devices ) ---------------------

// Onboard Devices Type
const char* lazybiosOnboardDeviceTypeStr(uint8_t type) {
    switch (type) {
        case ONBOARD_DEVICE_OTHER:          return "Other";
        case ONBOARD_DEVICE_UNKNOWN:        return "Unknown";
        case ONBOARD_DEVICE_VIDEO:          return "Video";
        case ONBOARD_DEVICE_SCSI_CTRL:      return "SCSI Controller";
        case ONBOARD_DEVICE_ETHERNET:       return "Ethernet";
        case ONBOARD_DEVICE_TOKEN_RING:     return "Token Ring";
        case ONBOARD_DEVICE_SOUND:          return "Sound";
        case ONBOARD_DEVICE_PATA_CTRL:      return "PATA Controller";
        case ONBOARD_DEVICE_SATA_CTRL:      return "SATA Controller";
        case ONBOARD_DEVICE_SAS_CTRL:       return "SAS Controller";
        default:                            return "Unknown";
    }
}

// End of Type 10 ----------------------------------


// Type 16 ( Physical Memory Array ) -------------------

// Memorry Array Location
const char* lazybiosPhysicalMemArrayLocationStr(uint8_t location) {
    switch(location) {
        case MEM_LOC_OTHER:               return "Other";
        case MEM_LOC_UNKNOWN:             return "Unknown";
        case MEM_LOC_SYSTEM_BOARD:        return "System board or motherboard";
        case MEM_LOC_ISA_ADDON:           return "ISA add-on card";
        case MEM_LOC_EISA_ADDON:          return "EISA add-on card";
        case MEM_LOC_PCI_ADDON:           return "PCI add-on card";
        case MEM_LOC_MCA_ADDON:           return "MCA add-on card";
        case MEM_LOC_PCMCIA_ADDON:        return "PCMCIA add-on card";
        case MEM_LOC_PROPRIETARY_ADDON:   return "Proprietary add-on card";
        case MEM_LOC_NUBUS:               return "NuBus";
        case MEM_LOC_PC98_C20:            return "PC-98/C20 add-on card";
        case MEM_LOC_PC98_C24:            return "PC-98/C24 add-on card";
        case MEM_LOC_PC98_E:              return "PC-98/E add-on card";
        case MEM_LOC_PC98_LOCAL:          return "PC-98/Local bus add-on card";
        case MEM_LOC_CXL_ADDON:           return "CXL add-on card";
        default:                          return "Unknown";
    }
}

// Memory Array Use
const char* lazybiosPhysicalMemArrayUseStr(uint8_t use) {
    switch(use) {
        case MEM_USE_OTHER:          return "Other";
        case MEM_USE_UNKNOWN:        return "Unknown";
        case MEM_USE_SYSTEM_MEMORY:  return "System memory";
        case MEM_USE_VIDEO_MEMORY:   return "Video memory";
        case MEM_USE_FLASH_MEMORY:   return "Flash memory";
        case MEM_USE_NVRAM:          return "Non-volatile RAM";
        case MEM_USE_CACHE_MEMORY:   return "Cache memory";
        default:                     return "Unknown";
    }
}

// Memory Array ECC
const char* lazybiosPhysicalMemArrayECCStr(uint8_t ecc) {
    switch(ecc) {
        case MEM_ECC_OTHER:       return "Other";
        case MEM_ECC_UNKNOWN:     return "Unknown";
        case MEM_ECC_NONE:        return "None";
        case MEM_ECC_PARITY:      return "Parity";
        case MEM_ECC_SINGLE_BIT:  return "Single-bit ECC";
        case MEM_ECC_MULTI_BIT:   return "Multi-bit ECC";
        case MEM_ECC_CRC:         return "CRC";
        default:                  return "Unknown";
    }
}

// End of Type 16 --------------------------------------


// Type 17 ( Memory Device ) -------------------------

// Memory Type
const char* lazybiosMemTypeStr(uint8_t type) {
    switch(type) {
        case MEMORY_TYPE_OTHER:         return "Other";
        case MEMORY_TYPE_UNKNOWN:       return "Unknown";
        case MEMORY_TYPE_DRAM:          return "DRAM";
        case MEMORY_TYPE_EDRAM:         return "EDRAM";
        case MEMORY_TYPE_VRAM:          return "VRAM";
        case MEMORY_TYPE_SRAM:          return "SRAM";
        case MEMORY_TYPE_RAM:           return "RAM";
        case MEMORY_TYPE_ROM:           return "ROM";
        case MEMORY_TYPE_FLASH:         return "FLASH";
        case MEMORY_TYPE_EEPROM:        return "EEPROM";
        case MEMORY_TYPE_FEPROM:        return "FEPROM";
        case MEMORY_TYPE_EPROM:         return "EPROM";
        case MEMORY_TYPE_CDRAM:         return "CDRAM";
        case MEMORY_TYPE_3DRAM:         return "3DRAM";
        case MEMORY_TYPE_SDRAM:         return "SDRAM";
        case MEMORY_TYPE_SGRAM:         return "SGRAM";
        case MEMORY_TYPE_RDRAM:         return "RDRAM";
        case MEMORY_TYPE_DDR:           return "DDR";
        case MEMORY_TYPE_DDR2:          return "DDR2";
        case MEMORY_TYPE_DDR2_FB_DIMM:  return "DDR2 FB-DIMM";
        case MEMORY_TYPE_DDR3:          return "DDR3";
        case MEMORY_TYPE_FBD2:          return "FBD2";
        case MEMORY_TYPE_DDR4:          return "DDR4";
        case MEMORY_TYPE_LPDDR:         return "LPDDR";
        case MEMORY_TYPE_LPDDR2:        return "LPDDR2";
        case MEMORY_TYPE_LPDDR3:        return "LPDDR3";
        case MEMORY_TYPE_LPDDR4:        return "LPDDR4";
        case MEMORY_TYPE_LPDDR_NV:      return "Logical non-volatile device";
        case MEMORY_TYPE_HBM:           return "HBM";
        case MEMORY_TYPE_HBM2:          return "HBM2";
        case MEMORY_TYPE_DDR5:          return "DDR5";
        case MEMORY_TYPE_LPDDR5:        return "LPDDR5";
        case MEMORY_TYPE_HBM3:          return "HBM3";
        default:                        return "Unknown";
    }
}

// Memory Form Factor
const char* lazybiosMemFormFactorStr(uint8_t form_factor) {
    switch(form_factor) {
        case MEMORY_FORM_FACTOR_OTHER:      return "Other";
        case MEMORY_FORM_FACTOR_UNKNOWN:    return "Unknown";
        case MEMORY_FORM_FACTOR_SIMM:       return "SIMM";
        case MEMORY_FORM_FACTOR_SIP:        return "SIP";
        case MEMORY_FORM_FACTOR_CHIP:       return "Chip";
        case MEMORY_FORM_FACTOR_DIP:        return "DIP";
        case MEMORY_FORM_FACTOR_ZIP:        return "ZIP";
        case MEMORY_FORM_FACTOR_PROP_CARD:  return "Proprietary Card";
        case MEMORY_FORM_FACTOR_DIMM:       return "DIMM";
        case MEMORY_FORM_FACTOR_TSOP:       return "TSOP";
        case MEMORY_FORM_FACTOR_ROW_CHIPS:  return "Row of chips";
        case MEMORY_FORM_FACTOR_RIMM:       return "RIMM";
        case MEMORY_FORM_FACTOR_SODIMM:     return "SODIMM";
        case MEMORY_FORM_FACTOR_SRIMM:      return "SRIMM";
        case MEMORY_FORM_FACTOR_FB_DIMM:    return "FB-DIMM";
        case MEMORY_FORM_FACTOR_DIE:        return "Die";
        case MEMORY_FORM_FACTOR_CAMM:       return "CAMM";
        default:                            return "Unknown";
    }
}

// End of Type 17 ------------------------------------

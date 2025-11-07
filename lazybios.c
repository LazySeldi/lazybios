#include "lazybios.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// ===== Context Management =====
lazybios_ctx_t* lazybios_ctx_new(void) {
    lazybios_ctx_t* ctx = calloc(1, sizeof(lazybios_ctx_t));
    if (!ctx) {
        fprintf(stderr, "lazybios: failed to allocate context\n");
        return NULL;
    }
    return ctx;
}

void lazybios_ctx_free(lazybios_ctx_t* ctx) {
    if (!ctx) return;
    lazybios_cleanup(ctx);
    free(ctx);
}

// ===== Version-Aware Helpers =====
size_t lazybios_get_smbios_structure_min_length(const lazybios_ctx_t* ctx, uint8_t type) {
    if (!ctx) return 4;

    bool is_64bit = ctx->entry_info.is_64bit;

    switch(type) {
        case SMBIOS_TYPE_BIOS:
            return is_64bit ? BIOS_MIN_LENGTH_3_0 : BIOS_MIN_LENGTH_2_0;

        case SMBIOS_TYPE_SYSTEM:
            return is_64bit ? SYSTEM_MIN_LENGTH_3_0 : SYSTEM_MIN_LENGTH_2_0;

        case SMBIOS_TYPE_BASEBOARD:
            return BASEBOARD_MIN_LENGTH;

        case SMBIOS_TYPE_CHASSIS:
            return is_64bit ? CHASSIS_MIN_LENGTH_3_0 : CHASSIS_MIN_LENGTH_2_0;

        case SMBIOS_TYPE_PROCESSOR:
            return (ctx->entry_info.major > 2 || (ctx->entry_info.major == 2 && ctx->entry_info.minor >= 6))
                   ? PROC_MIN_LENGTH_2_6 : PROC_MIN_LENGTH_2_0;

        case SMBIOS_TYPE_CACHES:
            return (ctx->entry_info.major > 2 || (ctx->entry_info.major == 2 && ctx->entry_info.minor >= 1))
                   ? CACHE_MIN_LENGTH_2_1 : CACHE_MIN_LENGTH_2_0;

        case SMBIOS_TYPE_PORT_CONNECTOR:
            return PORT_CONNECTOR_MIN_LENGTH;

        case SMBIOS_TYPE_MEMARRAY:
            return MEMARRAY_MIN_LENGTH;

        case SMBIOS_TYPE_MEMDEVICE:
            return is_64bit ? MEMORY_MIN_LENGTH_3_0 : MEMORY_MIN_LENGTH_2_0;

        default:
            return 4;
    }
}

bool lazybios_is_smbios_version_at_least(const lazybios_ctx_t* ctx, uint8_t major, uint8_t minor) {
    if (!ctx) return false;
    return (ctx->entry_info.major > major) ||
           (ctx->entry_info.major == major && ctx->entry_info.minor >= minor);
}

// ===== String Mapping Functions =====
const char* lazybios_get_processor_family_string(uint8_t family) {
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

        default:
            return "Unknown";
    }
}

// Processor type
const char* lazybios_get_processor_type_string(uint8_t type) {
    switch(type) {
        case PROC_TYPE_OTHER:            return "Other";
        case PROC_TYPE_UNKNOWN:          return "Unknown";
        case PROC_TYPE_CENTRAL_PROCESSOR:return "Central Processor";
        case PROC_TYPE_MATH_PROCESSOR:   return "Math Processor";
        case PROC_TYPE_DSP_PROCESSOR:    return "DSP Processor";
        case PROC_TYPE_VIDEO_PROCESSOR:  return "Video Processor";
        default: return "Unknown";
    }
}

// Processor status (lower 3 bits)
const char* lazybios_get_processor_status_string(uint8_t status) {
    uint8_t cpu_status = status & PROC_STATUS_MASK;
    switch(cpu_status) {
        case PROC_STATUS_UNKNOWN:            return "Unknown";
        case PROC_STATUS_ENABLED:            return "Enabled";
        case PROC_STATUS_DISABLED_BY_USER:   return "Disabled by User";
        case PROC_STATUS_DISABLED_BY_BIOS:   return "Disabled by BIOS";
        case PROC_STATUS_IDLE:               return "Idle";
        case PROC_STATUS_OTHER:              return "Other";
        default: return "Reserved";
    }
}

// Cache type
const char* lazybios_get_cache_type_string(uint8_t cache_type) {
    switch(cache_type) {
        case CACHE_TYPE_OTHER:       return "Other";
        case CACHE_TYPE_UNKNOWN:     return "Unknown";
        case CACHE_TYPE_INSTRUCTION: return "Instruction";
        case CACHE_TYPE_DATA:        return "Data";
        case CACHE_TYPE_UNIFIED:     return "Unified";
        default: return "Unknown";
    }
}

// Cache ECC
const char* lazybios_get_cache_ecc_string(uint8_t ecc_type) {
    switch(ecc_type) {
        case CACHE_ECC_OTHER:       return "Other";
        case CACHE_ECC_UNKNOWN:     return "Unknown";
        case CACHE_ECC_NONE:        return "None";
        case CACHE_ECC_PARITY:      return "Parity";
        case CACHE_ECC_SINGLE_BIT:  return "Single-bit ECC";
        case CACHE_ECC_MULTI_BIT:   return "Multi-bit ECC";
        case CACHE_ECC_CRC:         return "CRC";
        default: return "Unknown";
    }
}

// Cache associativity
const char* lazybios_get_cache_associativity_string(uint8_t associativity) {
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
        default: return "Unknown";
    }
}

const char* lazybios_get_port_connector_types_string(uint8_t connector_type) {
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

const char* lazybios_get_port_types_string(uint8_t port_type) {
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

const char* lazybios_get_memory_type_string(uint8_t type) {
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

const char* lazybios_get_memory_form_factor_string(uint8_t form_factor) {
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

const char* lazybios_get_memory_array_location_string(uint8_t location) {
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

const char* lazybios_get_memory_array_use_string(uint8_t use) {
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

const char* lazybios_get_memory_array_ecc_string(uint8_t ecc) {
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
// ===== Core Parsing Functions =====
static int parse_smbios2_entry(lazybios_ctx_t* ctx, const uint8_t *buf) {
    if (memcmp(buf, SMBIOS2_ANCHOR, 4) != 0) return -1;

    ctx->entry_info.major = buf[ENTRY2_MAJOR_OFFSET];
    ctx->entry_info.minor = buf[ENTRY2_MINOR_OFFSET];
    ctx->entry_info.docrev = 0;  // Not available in SMBIOS 2.x
    ctx->entry_info.table_length = *(uint16_t*)(buf + ENTRY2_TABLE_LENGTH_OFFSET);
    ctx->entry_info.table_address = *(uint32_t*)(buf + ENTRY2_TABLE_ADDR_OFFSET);
    ctx->entry_info.is_64bit = false;

    return 0;
}

static int parse_smbios3_entry(lazybios_ctx_t* ctx, const uint8_t *buf) {
    if (memcmp(buf, SMBIOS3_ANCHOR, 5) != 0) return -1;

    // FIXED: Use correct offsets for SMBIOS 3.x
    ctx->entry_info.major = buf[ENTRY3_MAJOR_OFFSET];
    ctx->entry_info.minor = buf[ENTRY3_MINOR_OFFSET];
    ctx->entry_info.docrev = buf[ENTRY3_DOCREV_OFFSET];
    ctx->entry_info.table_length = *(uint32_t*)(buf + ENTRY3_TABLE_LENGTH_OFFSET);
    ctx->entry_info.table_address = *(uint64_t*)(buf + ENTRY3_TABLE_ADDR_OFFSET);
    ctx->entry_info.is_64bit = true;

    return 0;
}

static const char* dmi_string(const uint8_t *formatted, int length, int string_index) {
    if (string_index == 0) return "Not Specified";

    const char *str = (const char *)(formatted + length);

    for (int i = 1; i < string_index; i++) {
        // Skip to the end of current string
        while (*str) str++;
        str++; // Skip the null terminator

        // Check if we've run out of strings
        if (!*str) return "Not Specified";
    }
    return str;
}

static const uint8_t* dmi_next(const uint8_t *ptr, const uint8_t *end) {
    const uint8_t *p = ptr + ptr[1];
    while (p + 1 < end && (p[0] || p[1])) p++;
    return p + 2;
}

// ===== Generic Structure Parser =====
static size_t count_structures_by_type(const lazybios_ctx_t* ctx, uint8_t target_type) {
    if (!ctx || !ctx->dmi_data) return 0;

    size_t count = 0;
    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, target_type);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        if (type == SMBIOS_TYPE_END) break;

        if (type == target_type && p[1] >= min_length) {
            count++;
        }
        p = dmi_next(p, end);
    }
    return count;
}

// ===== Library Initialization =====
int lazybios_init(lazybios_ctx_t* ctx) {
    if (!ctx) {
        fprintf(stderr, "lazybios: null context provided\n");
        return -1;
    }

    FILE *entry = fopen(SMBIOS_ENTRY, "rb");
    if (!entry) {
        fprintf(stderr, "lazybios: failed to open %s: %s\n", SMBIOS_ENTRY, strerror(errno));
        return -1;
    }

    FILE *dmi = fopen(DMI_TABLE, "rb");
    if (!dmi) {
        fprintf(stderr, "lazybios: failed to open %s: %s\n", DMI_TABLE, strerror(errno));
        fclose(entry);
        return -1;
    }

    uint8_t entry_buf[64];
    size_t n = fread(entry_buf, 1, sizeof(entry_buf), entry);
    fclose(entry);

    if (n < 20) {
        fprintf(stderr, "lazybios: invalid entry point size (%zu bytes)\n", n);
        fclose(dmi);
        return -1;
    }

    // Try SMBIOS 3.x first, then "fall back" to 2.x
    if (parse_smbios3_entry(ctx, entry_buf) == 0) {
        // Successfully parsed as SMBIOS 3.x
    } else if (parse_smbios2_entry(ctx, entry_buf) == 0) {
        // Successfully parsed as SMBIOS 2.x
    } else {
        fprintf(stderr, "lazybios: unknown SMBIOS anchor\n");
        fclose(dmi);
        return -1;
    }

    fseek(dmi, 0, SEEK_END);
    ctx->dmi_len = ftell(dmi);
    rewind(dmi);

    if (ctx->dmi_len == 0) {
        fprintf(stderr, "lazybios: empty DMI table\n");
        fclose(dmi);
        return -1;
    }

    ctx->dmi_data = malloc(ctx->dmi_len);
    if (!ctx->dmi_data) {
        fprintf(stderr, "lazybios: failed to allocate %zu bytes for DMI buffer\n", ctx->dmi_len);
        fclose(dmi);
        return -1;
    }

    if (fread(ctx->dmi_data, 1, ctx->dmi_len, dmi) != ctx->dmi_len) {
        fprintf(stderr, "lazybios: failed to read DMI table\n");
        free(ctx->dmi_data);
        ctx->dmi_data = NULL;
        fclose(dmi);
        return -1;
    }

    fclose(dmi);
    printf("lazybios: loaded %zu bytes of DMI data\n", ctx->dmi_len);
    return 0;
}

// ===== Basic Info Getters =====
bios_info_t* lazybios_get_bios_info(lazybios_ctx_t* ctx) {
    if (!ctx || !ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_BIOS);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_BIOS && length >= min_length) {
            ctx->bios_info.vendor = strdup(dmi_string(p, length, p[BIOS_VENDOR_OFFSET]));
            ctx->bios_info.version = strdup(dmi_string(p, length, p[BIOS_VERSION_OFFSET]));
            ctx->bios_info.release_date = strdup(dmi_string(p, length, p[BIOS_RELEASE_DATE_OFFSET]));

            uint8_t rom_size = p[BIOS_ROM_SIZE_OFFSET];
            if (rom_size == 0xFF) {
                ctx->bios_info.rom_size_kb = 0; // Size not specified
            } else {
                ctx->bios_info.rom_size_kb = (rom_size + 1) * BIOS_ROM_SIZE_MULTIPLIER;
            }

            return &ctx->bios_info;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

system_info_t* lazybios_get_system_info(lazybios_ctx_t* ctx) {
    if (!ctx || !ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_SYSTEM);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_SYSTEM && length >= min_length) {
            const char *mfr = dmi_string(p, length, p[SYS_MANUFACTURER_OFFSET]);
            const char *prod = dmi_string(p, length, p[SYS_PRODUCT_OFFSET]);
            const char *ver = dmi_string(p, length, p[SYS_VERSION_OFFSET]);
            const char *ser = dmi_string(p, length, p[SYS_SERIAL_OFFSET]);

            ctx->system_info.manufacturer = strdup(mfr ? mfr : "Unknown");
            ctx->system_info.product_name = strdup(prod ? prod : "Unknown");
            ctx->system_info.version = strdup(ver ? ver : "Unknown");
            ctx->system_info.serial_number = strdup(ser ? ser : "Unknown");

            if (length >= 0x18) {
                const uint8_t *uuid = p + SYS_UUID_OFFSET;
                ctx->system_info.uuid = malloc(37);
                if (ctx->system_info.uuid) {
                    snprintf(ctx->system_info.uuid, 37,
                        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                        uuid[0], uuid[1], uuid[2], uuid[3],
                        uuid[4], uuid[5], uuid[6], uuid[7],
                        uuid[8], uuid[9], uuid[10], uuid[11],
                        uuid[12], uuid[13], uuid[14], uuid[15]);
                }
            } else {
                ctx->system_info.uuid = strdup("Not Available");
            }

            return &ctx->system_info;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

baseboard_info_t* lazybios_get_baseboard_info(lazybios_ctx_t* ctx) {
    if (!ctx || !ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_BASEBOARD);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_BASEBOARD && length >= min_length) {
            ctx->baseboard_info.manufacturer = strdup(dmi_string(p, length, p[BASEBOARD_MANUFACTURER_OFFSET]));
            ctx->baseboard_info.product = strdup(dmi_string(p, length, p[BASEBOARD_PRODUCT_OFFSET]));
            ctx->baseboard_info.version = strdup(dmi_string(p, length, p[BASEBOARD_VERSION_OFFSET]));
            ctx->baseboard_info.serial_number = strdup(dmi_string(p, length, p[BASEBOARD_SERIAL_OFFSET]));
            ctx->baseboard_info.asset_tag = strdup(dmi_string(p, length, p[BASEBOARD_ASSET_TAG_OFFSET]));
            return &ctx->baseboard_info;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

chassis_info_t* lazybios_get_chassis_info(lazybios_ctx_t* ctx) {
    if (!ctx || !ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_CHASSIS);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;
        if (type == SMBIOS_TYPE_CHASSIS && length >= min_length) {
            if (length > CHASSIS_ASSET_TAG_OFFSET) {
                ctx->chassis_info.asset_tag = strdup(dmi_string(p, length, p[CHASSIS_ASSET_TAG_OFFSET]));
            } else {
                ctx->chassis_info.asset_tag = strdup("Not Specified");
            }

            if (length > CHASSIS_SKU_OFFSET) {
                ctx->chassis_info.sku = strdup(dmi_string(p, length, p[CHASSIS_SKU_OFFSET]));
            } else {
                ctx->chassis_info.sku = strdup("Not Specified");
            }

            if (length >= 0x05) {
                ctx->chassis_info.type = p[0x05];
                ctx->chassis_info.state = p[0x06];
            }

            return &ctx->chassis_info;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

processor_info_t* lazybios_get_processor_info(lazybios_ctx_t* ctx) {
    if (!ctx || !ctx->dmi_data) return NULL;

    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_PROCESSOR);

    while (p + SMBIOS_HEADER_SIZE < end) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_PROCESSOR && length >= min_length) {
            ctx->processor_info.socket_designation = strdup(dmi_string(p, length, p[PROC_SOCKET_OFFSET]));
            ctx->processor_info.version = strdup(dmi_string(p, length, p[PROC_VERSION_OFFSET]));
            ctx->processor_info.max_speed_mhz = *(uint16_t*)(p + PROC_MAX_SPEED_OFFSET);
            ctx->processor_info.L1_cache_handle = *(uint16_t*)(p + PROC_L1_CACHE_HANDLE_OFFSET);
            ctx->processor_info.L2_cache_handle = *(uint16_t*)(p + PROC_L2_CACHE_HANDLE_OFFSET);
            ctx->processor_info.L3_cache_handle = *(uint16_t*)(p + PROC_L3_CACHE_HANDLE_OFFSET);

            // Current speed (added)
            if (length > PROC_CURRENT_SPEED_OFFSET + 1) {
                ctx->processor_info.current_speed_mhz = *(uint16_t*)(p + PROC_CURRENT_SPEED_OFFSET);
            } else {
                ctx->processor_info.current_speed_mhz = 0;
            }

            ctx->processor_info.external_clock_mhz = *(uint16_t*)(p + PROC_EXTERNAL_CLOCK_OFFSET);
            ctx->processor_info.voltage = p[PROC_VOLTAGE_OFFSET];
            ctx->processor_info.processor_type = p[PROC_TYPE_OFFSET];
            ctx->processor_info.processor_family = p[PROC_FAMILY_OFFSET];

            // Status
            if (length > PROC_STATUS_OFFSET) {
                ctx->processor_info.status = p[PROC_STATUS_OFFSET];
            } else {
                ctx->processor_info.status = 0;
            }

            // Core count + Extended(If available)
            uint8_t core_count_byte = p[PROC_CORE_COUNT_OFFSET];
            if (core_count_byte == 0xFF && length > PROC_CORE_COUNT2_OFFSET + 1) {
                ctx->processor_info.core_count = *(uint16_t*)(p + PROC_CORE_COUNT2_OFFSET);
            } else {
                ctx->processor_info.core_count = core_count_byte;
            }

            // Cores enabled + Extended
            uint8_t core_enabled_byte = p[PROC_CORE_ENABLED_OFFSET];
            if (core_enabled_byte == 0xFF && length > PROC_CORE_ENABLED2_OFFSET + 1) {
                ctx->processor_info.core_enabled = *(uint16_t*)(p + PROC_CORE_ENABLED2_OFFSET);
            } else {
                ctx->processor_info.core_enabled = core_enabled_byte;
            }

            // Thread count with extended support
            uint8_t thread_count_byte = p[PROC_THREAD_COUNT_OFFSET];
            if (thread_count_byte == 0xFF && length > PROC_THREAD_COUNT2_OFFSET + 1) {
                ctx->processor_info.thread_count = *(uint16_t*)(p + PROC_THREAD_COUNT2_OFFSET);
            } else {
                ctx->processor_info.thread_count = thread_count_byte;
            }

            // Extended processor family
            if (ctx->processor_info.processor_family == 0xFE && length > PROC_FAMILY2_OFFSET + 1) {
                ctx->processor_info.processor_family2 = *(uint16_t*)(p + PROC_FAMILY2_OFFSET);
            } else {
                ctx->processor_info.processor_family2 = ctx->processor_info.processor_family;
            }

            // Serial, asset tag, part number
            if (length > PROC_SERIAL_OFFSET_2_5) {
                ctx->processor_info.serial_number = strdup(dmi_string(p, length, p[PROC_SERIAL_OFFSET_2_5]));
            } else {
                ctx->processor_info.serial_number = strdup("Not Specified");
            }

            if (length > PROC_ASSET_TAG_OFFSET_2_5) {
                ctx->processor_info.asset_tag = strdup(dmi_string(p, length, p[PROC_ASSET_TAG_OFFSET_2_5]));
            } else {
                ctx->processor_info.asset_tag = strdup("Not Specified");
            }

            if (length > PROC_PART_NUMBER_OFFSET_2_5) {
                ctx->processor_info.part_number = strdup(dmi_string(p, length, p[PROC_PART_NUMBER_OFFSET_2_5]));
            } else {
                ctx->processor_info.part_number = strdup("Not Specified");
            }

            if (length > PROC_CHARACTERISTICS_OFFSET + 1) {
                ctx->processor_info.characteristics = *(uint16_t*)(p + PROC_CHARACTERISTICS_OFFSET);
            } else {
                ctx->processor_info.characteristics = 0;
            }

            return &ctx->processor_info;
        }
        p = dmi_next(p, end);
    }
    return NULL;
}

// ===== Cache Information Getter =====
cache_info_t* lazybios_get_caches(lazybios_ctx_t* ctx, size_t* count) {
    if (!ctx) {
        *count = 0;
        return NULL;
    }

    if (ctx->caches_ptr) {
        *count = ctx->caches_count;
        return ctx->caches_ptr;
    }

    if (!ctx->dmi_data) {
        *count = 0;
        return NULL;
    }

    ctx->caches_count = count_structures_by_type(ctx, SMBIOS_TYPE_CACHES);
    if (ctx->caches_count == 0) {
        *count = 0;
        return NULL;
    }

    ctx->caches_ptr = calloc(ctx->caches_count, sizeof(cache_info_t));
    if (!ctx->caches_ptr) {
        *count = 0;
        return NULL;
    }

    size_t index = 0;
    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_CACHES);
    bool is_31_plus = lazybios_is_smbios_version_at_least(ctx, 3, 1);

    while (p + SMBIOS_HEADER_SIZE < end && index < ctx->caches_count) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_CACHES && length >= min_length) {
            cache_info_t *current = &ctx->caches_ptr[index];

            const char *sock = dmi_string(p, length, p[CACHE_SOCKET_DESIGNATION]);
            current->socket_designation = strdup(sock ? sock : "Unknown");

            uint16_t config = *(uint16_t*)(p + CACHE_CONFIGURATION);
            current->level = config & 0x07;

            uint16_t max_size = *(uint16_t*)(p + CACHE_MAXIMUM_SIZE);
            uint16_t installed_size = *(uint16_t*)(p + CACHE_INSTALLED_SIZE);

            if (is_31_plus && length >= CACHE_MIN_LENGTH_3_1) {
                if (max_size == 0xFFFF || (max_size & 0x8000)) {
                    current->size_kb = *(uint32_t*)(p + CACHE_MAXIMUM_SIZE_2);
                } else {
                    uint16_t granularity = (max_size & 0x8000) ? 64 : 1;
                    current->size_kb = (max_size & 0x7FFF) * granularity;
                }
            } else {
                uint16_t granularity = (installed_size & 0x8000) ? 64 : 1;
                current->size_kb = (installed_size & 0x7FFF) * granularity;
            }

            if (length > CACHE_ERROR_CORRECTION_TYPE) {
                current->error_correction_type = p[CACHE_ERROR_CORRECTION_TYPE];
            }
            if (length > CACHE_SYSTEM_CACHE_TYPE) {
                current->system_cache_type = p[CACHE_SYSTEM_CACHE_TYPE];
            }
            if (length > CACHE_ASSOCIATIVITY) {
                current->associativity = p[CACHE_ASSOCIATIVITY];
            }

            index++;
        }
        p = dmi_next(p, end);
    }

    *count = ctx->caches_count;
    return ctx->caches_ptr;
}

// ===== Port Connector Getter =====
port_connector_info_t* lazybios_get_port_connectors(lazybios_ctx_t* ctx, size_t* count) {
    if (!ctx) {
        *count = 0;
        return NULL;
    }

    if (ctx->port_connector_ptr) {
        *count = ctx->port_connector_count;
        return ctx->port_connector_ptr;
    }

    if (!ctx->dmi_data) {
        *count = 0;
        return NULL;
    }

    ctx->port_connector_count = count_structures_by_type(ctx, SMBIOS_TYPE_PORT_CONNECTOR);
    if (ctx->port_connector_count == 0) {
        *count = 0;
        return NULL;
    }

    ctx->port_connector_ptr = calloc(ctx->port_connector_count, sizeof(port_connector_info_t));
    if (!ctx->port_connector_ptr) {
        *count = 0;
        return NULL;
    }

    size_t index = 0;
    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_PORT_CONNECTOR);

    while (p + SMBIOS_HEADER_SIZE < end && index < ctx->port_connector_count) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_PORT_CONNECTOR && length >= min_length) {
            port_connector_info_t *current = &ctx->port_connector_ptr[index];

            current->port_type = p[PORT_OFFSET_PORT_TYPE];
            current->handle = *(uint16_t*)(p + PORT_OFFSET_HANDLE);
            current->external_connector_type = p[PORT_OFFSET_EXTERNAL_CONNECTOR_TYPE];
            current->internal_connector_type = p[PORT_OFFSET_INTERNAL_CONNECTOR_TYPE];
            current->external_ref_designator = strdup(dmi_string(p, length, p[PORT_OFFSET_EXTERNAL_REF_DESIGNATOR]));
            current->internal_ref_designator = strdup(dmi_string(p, length, p[PORT_OFFSET_INTERNAL_REF_DESIGNATOR]));

            index++;
        }
        p = dmi_next(p, end);
    }

    *count = ctx->port_connector_count;
    return ctx->port_connector_ptr;
}

// ===== Memory Devices Getter =====
memory_device_t* lazybios_get_memory_devices(lazybios_ctx_t* ctx, size_t* count) {
    if (!ctx) {
        *count = 0;
        return NULL;
    }

    if (ctx->memory_devices_ptr) {
        *count = ctx->memory_devices_count;
        return ctx->memory_devices_ptr;
    }

    if (!ctx->dmi_data) {
        *count = 0;
        return NULL;
    }

    ctx->memory_devices_count = count_structures_by_type(ctx, SMBIOS_TYPE_MEMDEVICE);
    if (ctx->memory_devices_count == 0) {
        *count = 0;
        return NULL;
    }

    ctx->memory_devices_ptr = calloc(ctx->memory_devices_count, sizeof(memory_device_t));
    if (!ctx->memory_devices_ptr) {
        *count = 0;
        return NULL;
    }

    size_t index = 0;
    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_MEMDEVICE);

    while (p + SMBIOS_HEADER_SIZE < end && index < ctx->memory_devices_count) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_MEMDEVICE && length >= min_length) {
            memory_device_t *current = &ctx->memory_devices_ptr[index];

            current->locator = strdup(dmi_string(p, length, p[MEM_DEVICE_LOCATOR_OFFSET]));
            current->bank_locator = strdup(dmi_string(p, length, p[MEM_DEVICE_BANK_LOCATOR_OFFSET]));
            current->manufacturer = strdup(dmi_string(p, length, p[MEM_DEVICE_MANUFACTURER_OFFSET]));
            current->serial_number = strdup(dmi_string(p, length, p[MEM_DEVICE_SERIAL_OFFSET]));
            current->part_number = strdup(dmi_string(p, length, p[MEM_DEVICE_PART_NUMBER_OFFSET]));

            uint16_t size = *(uint16_t*)(p + MEM_DEVICE_SIZE_OFFSET);

            if (size == 0xFFFF) {
                // Size not known
                current->size_mb = 0;
                current->size_extended = false;
            } else if (size == 0x7FFF) {
                // Extended size - check if we have the extended size field
                if (length >= 0x20) {  // Structure must be long enough for extended size
                    uint32_t ext_size = *(uint32_t*)(p + MEM_DEVICE_EXT_SIZE_OFFSET);
                    current->size_mb = ext_size;  // Extended size is in MB directly
                    current->size_extended = true;
                } else {
                    current->size_mb = 0;
                    current->size_extended = false;
                }
            } else if (size == 0) {
                // Empty slot
                current->size_mb = 0;
                current->size_extended = false;
            } else {
                // Regular size (in MB)
                current->size_mb = size;
                current->size_extended = false;
            }

            current->speed_mhz = *(uint16_t*)(p + MEM_DEVICE_SPEED_OFFSET);
            current->memory_type = p[MEM_DEVICE_TYPE_OFFSET];
            current->form_factor = p[MEM_DEVICE_FORM_FACTOR_OFFSET];

            // Total width and data width
            if (length > MEM_DEVICE_TOTAL_WIDTH_OFFSET + 1) {
                current->total_width = *(uint16_t*)(p + MEM_DEVICE_TOTAL_WIDTH_OFFSET);
            } else {
                current->total_width = 0;
            }

            if (length > MEM_DEVICE_DATA_WIDTH_OFFSET + 1) {
                current->data_width = *(uint16_t*)(p + MEM_DEVICE_DATA_WIDTH_OFFSET);
            } else {
                current->data_width = 0;
            }

            index++;
        }
        p = dmi_next(p, end);
    }

    *count = ctx->memory_devices_count;
    return ctx->memory_devices_ptr;
}

physical_memory_array_t* lazybios_get_memory_arrays(lazybios_ctx_t* ctx, size_t* count) {
    if (!ctx) {
        *count = 0;
        return NULL;
    }

    if (ctx->memory_arrays_ptr) {
        *count = ctx->memory_arrays_count;
        return ctx->memory_arrays_ptr;
    }

    if (!ctx->dmi_data) {
        *count = 0;
        return NULL;
    }

    ctx->memory_arrays_count = count_structures_by_type(ctx, SMBIOS_TYPE_MEMARRAY);
    if (ctx->memory_arrays_count == 0) {
        *count = 0;
        return NULL;
    }

    ctx->memory_arrays_ptr = calloc(ctx->memory_arrays_count, sizeof(physical_memory_array_t));
    if (!ctx->memory_arrays_ptr) {
        *count = 0;
        return NULL;
    }

    size_t index = 0;
    const uint8_t *p = ctx->dmi_data;
    const uint8_t *end = ctx->dmi_data + ctx->dmi_len;
    size_t min_length = lazybios_get_smbios_structure_min_length(ctx, SMBIOS_TYPE_MEMARRAY);

    while (p + SMBIOS_HEADER_SIZE < end && index < ctx->memory_arrays_count) {
        uint8_t type = p[0];
        uint8_t length = p[1];

        if (type == SMBIOS_TYPE_END) break;

        if (type == SMBIOS_TYPE_MEMARRAY && length >= min_length) {
            physical_memory_array_t *current = &ctx->memory_arrays_ptr[index];

            current->location = p[MEMARRAY_LOCATION_OFFSET];
            current->use = p[MEMARRAY_USE_OFFSET];
            current->ecc_type = p[MEMARRAY_ECC_OFFSET];

            uint32_t max_cap = *(uint32_t*)(p + MEMARRAY_MAX_CAPACITY_OFFSET);

            if (max_cap == 0x80000000 && length >= 0x17) {
                uint64_t ext_cap = *(uint64_t*)(p + MEMARRAY_EXT_MAX_CAPACITY_OFFSET);
                current->max_capacity_kb = ext_cap;
                current->extended_capacity = true;
            } else {
                current->max_capacity_kb = max_cap;
                current->extended_capacity = false;
            }

            current->num_devices = *(uint16_t*)(p + MEMARRAY_NUM_DEVICES_OFFSET);

            index++;
        }
        p = dmi_next(p, end);
    }

    *count = ctx->memory_arrays_count;
    return ctx->memory_arrays_ptr;
}

// ===== Cleanup =====
void lazybios_cleanup(lazybios_ctx_t* ctx) {
    if (!ctx) return;

    free(ctx->bios_info.vendor);
    free(ctx->bios_info.version);
    free(ctx->bios_info.release_date);

    free(ctx->system_info.manufacturer);
    free(ctx->system_info.product_name);
    free(ctx->system_info.version);
    free(ctx->system_info.serial_number);
    free(ctx->system_info.uuid);

    free(ctx->baseboard_info.manufacturer);
    free(ctx->baseboard_info.product);
    free(ctx->baseboard_info.version);
    free(ctx->baseboard_info.serial_number);
    free(ctx->baseboard_info.asset_tag);

    free(ctx->chassis_info.asset_tag);
    free(ctx->chassis_info.sku);

    free(ctx->processor_info.socket_designation);
    free(ctx->processor_info.version);
    free(ctx->processor_info.serial_number);
    free(ctx->processor_info.asset_tag);
    free(ctx->processor_info.part_number);

    if (ctx->caches_ptr) {
        for (size_t i = 0; i < ctx->caches_count; i++) {
            free(ctx->caches_ptr[i].socket_designation);
        }
        free(ctx->caches_ptr);
        ctx->caches_ptr = NULL;
    }
    ctx->caches_count = 0;

    if (ctx->port_connector_ptr) {
        for (size_t i = 0; i < ctx->port_connector_count; i++) {
            free(ctx->port_connector_ptr[i].external_ref_designator);
            free(ctx->port_connector_ptr[i].internal_ref_designator);
        }
        free(ctx->port_connector_ptr);
        ctx->port_connector_ptr = NULL;
    }
    ctx->caches_count = 0;

    if (ctx->memory_arrays_ptr) {
        free(ctx->memory_arrays_ptr);
        ctx->memory_arrays_ptr = NULL;
    }
    ctx->memory_arrays_count = 0;

    if (ctx->memory_devices_ptr) {
        for (size_t i = 0; i < ctx->memory_devices_count; i++) {
            free(ctx->memory_devices_ptr[i].locator);
            free(ctx->memory_devices_ptr[i].bank_locator);
            free(ctx->memory_devices_ptr[i].manufacturer);
            free(ctx->memory_devices_ptr[i].serial_number);
            free(ctx->memory_devices_ptr[i].part_number);
        }
        free(ctx->memory_devices_ptr);
        ctx->memory_devices_ptr = NULL;
    }
    ctx->memory_devices_count = 0;

    if (ctx->dmi_data) {
        free(ctx->dmi_data);
        ctx->dmi_data = NULL;
    }
    ctx->dmi_len = 0;
}

// ===== Public API Functions =====
void lazybios_smbios_ver(const lazybios_ctx_t* ctx) {
    if (!ctx) return;
    if (ctx->entry_info.is_64bit) {
        printf("SMBIOS version %d.%d.%d\n",
               ctx->entry_info.major,
               ctx->entry_info.minor,
               ctx->entry_info.docrev);
    } else {
        printf("SMBIOS version %d.%d\n",
               ctx->entry_info.major,
               ctx->entry_info.minor);
    }
}

const smbios_entry_info_t* lazybios_get_entry_info(const lazybios_ctx_t* ctx) {
    if (!ctx) return NULL;
    return &ctx->entry_info;
}

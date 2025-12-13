//
// Type 0 ( Platform Firmware Information )
//

// Defines for Readability //////////////////////////////////////////////////////////////////////////////////////////////////////
#define VENDOR                                      0x04
#define FIRMWARE_VERSION                            0x05
#define BIOS_STARTING_SEGMENT                       0x06
#define FIRMWARE_RELEASE_DATE                       0x08
#define FIRMWARE_ROM_SIZE                           0x09
#define FIRMWARE_CHARACTERISTICS                    0x0A
#define FIRMWARE_CHARACTERISTICS_EXTENSION_BYTES    0x12
#define PLATFORM_FIRMWARE_MAJOR_RELEASE             0x14
#define PLATFORM_FIRMWARE_MINOR_RELEASE             0x15
#define EMBEDDED_CONTROLLER_FIRMWARE_MAJOR_RELEASE  0x16
#define EMBEDDED_CONTROLLER_FIRMWARE_MINOR_RELEASE  0x17
#define EXTENDED_FIRMWARE_ROM_SIZE                  0x18
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lazybios.h"

lazybiosType0_t* lazybiosGetType0(lazybiosCTX_t* ctx) {
    if (!ctx || !ctx->dmi_data) return LAZYBIOS_NULL;

    const uint8_t* p   = ctx->dmi_data;
    const uint8_t* end = ctx->dmi_data + ctx->dmi_len;

    while (p + SMBIOS_HEADER_SIZE <= end) {
        uint8_t type = p[0];
        uint8_t len  = p[1];

        if (type == 0) {
            lazybiosType0_t* Type0 = calloc(1, sizeof(*Type0));
            if (!Type0) return LAZYBIOS_NULL;

            Type0->vendor = DMIString(p, len, p[VENDOR], end);
            if (!Type0->vendor) Type0->vendor = strdup(LAZYBIOS_NOT_FOUND_STR);

            Type0->version = DMIString(p, len, p[FIRMWARE_VERSION], end);
            if (!Type0->version) Type0->version = strdup(LAZYBIOS_NOT_FOUND_STR);

            Type0->release_date = DMIString(p, len, p[FIRMWARE_RELEASE_DATE], end);
            if (!Type0->release_date) Type0->release_date = strdup(LAZYBIOS_NOT_FOUND_STR);

            if (len >= BIOS_STARTING_SEGMENT + sizeof(uint16_t)) { memcpy(&Type0->bios_starting_segment,p + BIOS_STARTING_SEGMENT, sizeof(uint16_t)); } else { Type0->bios_starting_segment = LAZYBIOS_NOT_FOUND_U16; }

            if (len > FIRMWARE_ROM_SIZE && p[FIRMWARE_ROM_SIZE] == 0xFF) {
                if (ISVERPLUS(ctx, 3, 1) && len >= EXTENDED_FIRMWARE_ROM_SIZE + sizeof(uint16_t)) {
                    memcpy(&Type0->extended_rom_size, p + EXTENDED_FIRMWARE_ROM_SIZE,sizeof(uint16_t));
                } else {
                    Type0->extended_rom_size = LAZYBIOS_NOT_FOUND_U16;
                }

                Type0->rom_size = LAZYBIOS_NOT_FOUND_U32;
            } else if (len > FIRMWARE_ROM_SIZE) {
                Type0->rom_size = (uint32_t)(p[FIRMWARE_ROM_SIZE] + 1) * 64;
                Type0->extended_rom_size = LAZYBIOS_NOT_FOUND_U16;
            } else {
                Type0->rom_size = LAZYBIOS_NOT_FOUND_U32;
                Type0->extended_rom_size = LAZYBIOS_NOT_FOUND_U16;
            }

            if (len >= FIRMWARE_CHARACTERISTICS + sizeof(uint64_t)) {
                memcpy(&Type0->characteristics,p + FIRMWARE_CHARACTERISTICS,sizeof(uint64_t));
            } else {
                Type0->characteristics = LAZYBIOS_NOT_FOUND_U64;
            }

            if (len > FIRMWARE_CHARACTERISTICS_EXTENSION_BYTES) {
                Type0->firmware_char_ext_bytes_count = len - FIRMWARE_CHARACTERISTICS_EXTENSION_BYTES;
                Type0->firmware_char_ext_bytes = malloc(Type0->firmware_char_ext_bytes_count);

                if (Type0->firmware_char_ext_bytes) {
                    memcpy(Type0->firmware_char_ext_bytes, p + FIRMWARE_CHARACTERISTICS_EXTENSION_BYTES, Type0->firmware_char_ext_bytes_count);
                }
            }

            if (ISVERPLUS(ctx, 2, 4)) {
                Type0->platform_major_release = (len > PLATFORM_FIRMWARE_MAJOR_RELEASE) ? p[PLATFORM_FIRMWARE_MAJOR_RELEASE] : LAZYBIOS_NOT_FOUND_U8;
                Type0->platform_minor_release = (len > PLATFORM_FIRMWARE_MINOR_RELEASE) ? p[PLATFORM_FIRMWARE_MINOR_RELEASE] : LAZYBIOS_NOT_FOUND_U8;
                Type0->ec_major_release = (len > EMBEDDED_CONTROLLER_FIRMWARE_MAJOR_RELEASE) ? p[EMBEDDED_CONTROLLER_FIRMWARE_MAJOR_RELEASE] : LAZYBIOS_NOT_FOUND_U8;
                Type0->ec_minor_release = (len > EMBEDDED_CONTROLLER_FIRMWARE_MINOR_RELEASE) ? p[EMBEDDED_CONTROLLER_FIRMWARE_MINOR_RELEASE] : LAZYBIOS_NOT_FOUND_U8;
            } else {
                Type0->platform_major_release = LAZYBIOS_NOT_FOUND_U8;
                Type0->platform_minor_release = LAZYBIOS_NOT_FOUND_U8;
                Type0->ec_major_release = LAZYBIOS_NOT_FOUND_U8;
                Type0->ec_minor_release = LAZYBIOS_NOT_FOUND_U8;
            }
            return Type0;
        }
        p = DMINext(p, end);
    }
    return LAZYBIOS_NULL;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decoders

// Firmware Characteristics
const char* lazybiosFirmwareCharacteristicsStr(uint64_t characteristics) {
    _Thread_local static char buf[1024];
    size_t len = 0;
    buf[0] = '\0';

    // Bits 0–1 Reserved
    if (characteristics & (1ull << 2))  len += snprintf(buf + len, sizeof(buf) - len, "Unknown, ");
    if (characteristics & (1ull << 3))  len += snprintf(buf + len, sizeof(buf) - len, "Firmware Characteristics Unsupported, ");
    if (characteristics & (1ull << 4))  len += snprintf(buf + len, sizeof(buf) - len, "ISA Supported, ");
    if (characteristics & (1ull << 5))  len += snprintf(buf + len, sizeof(buf) - len, "MCA Supported, ");
    if (characteristics & (1ull << 6))  len += snprintf(buf + len, sizeof(buf) - len, "EISA Supported, ");
    if (characteristics & (1ull << 7))  len += snprintf(buf + len, sizeof(buf) - len, "PCI Supported, ");
    if (characteristics & (1ull << 8))  len += snprintf(buf + len, sizeof(buf) - len, "PCMCIA Supported, ");
    if (characteristics & (1ull << 9))  len += snprintf(buf + len, sizeof(buf) - len, "Plug and Play Supported, ");
    if (characteristics & (1ull << 10)) len += snprintf(buf + len, sizeof(buf) - len, "APM Supported, ");
    if (characteristics & (1ull << 11)) len += snprintf(buf + len, sizeof(buf) - len, "Firmware Upgradeable (Flash), ");
    if (characteristics & (1ull << 12)) len += snprintf(buf + len, sizeof(buf) - len, "Firmware Shadowing Allowed, ");
    if (characteristics & (1ull << 13)) len += snprintf(buf + len, sizeof(buf) - len, "VL-VESA Supported, ");
    if (characteristics & (1ull << 14)) len += snprintf(buf + len, sizeof(buf) - len, "ESCD Support Available, ");
    if (characteristics & (1ull << 15)) len += snprintf(buf + len, sizeof(buf) - len, "Boot from CD Supported, ");
    if (characteristics & (1ull << 16)) len += snprintf(buf + len, sizeof(buf) - len, "Selectable Boot Supported, ");
    if (characteristics & (1ull << 17)) len += snprintf(buf + len, sizeof(buf) - len, "Firmware ROM Socketed, ");
    if (characteristics & (1ull << 18)) len += snprintf(buf + len, sizeof(buf) - len, "Boot from PCMCIA Supported, ");
    if (characteristics & (1ull << 19)) len += snprintf(buf + len, sizeof(buf) - len, "EDD Specification Supported, ");
    if (characteristics & (1ull << 20)) len += snprintf(buf + len, sizeof(buf) - len, "Int13 NEC 9800 1.2MB Supported, ");
    if (characteristics & (1ull << 21)) len += snprintf(buf + len, sizeof(buf) - len, "Int13 Toshiba 1.2MB Supported, ");
    if (characteristics & (1ull << 22)) len += snprintf(buf + len, sizeof(buf) - len, "Int13 5.25/360KB Supported, ");
    if (characteristics & (1ull << 23)) len += snprintf(buf + len, sizeof(buf) - len, "Int13 5.25/1.2MB Supported, ");
    if (characteristics & (1ull << 24)) len += snprintf(buf + len, sizeof(buf) - len, "Int13 3.5/720KB Supported, ");
    if (characteristics & (1ull << 25)) len += snprintf(buf + len, sizeof(buf) - len, "Int13 3.5/2.88MB Supported, ");
    if (characteristics & (1ull << 26)) len += snprintf(buf + len, sizeof(buf) - len, "Int5 Print Screen Supported, ");
    if (characteristics & (1ull << 27)) len += snprintf(buf + len, sizeof(buf) - len, "Int9 Keyboard Services Supported, ");
    if (characteristics & (1ull << 28)) len += snprintf(buf + len, sizeof(buf) - len, "Int14 Serial Services Supported, ");
    if (characteristics & (1ull << 29)) len += snprintf(buf + len, sizeof(buf) - len, "Int17 Printer Services Supported, ");
    if (characteristics & (1ull << 30)) len += snprintf(buf + len, sizeof(buf) - len, "Int10 CGA/Mono Video Services Supported, ");
    if (characteristics & (1ull << 31)) len += snprintf(buf + len, sizeof(buf) - len, "NEC PC-98 Supported, ");

    // Bits 32–63 are reserved, so we will skip them

    if (len == 0) return "None";

    if (len >= 2) buf[len - 2] = '\0';

    return buf;
}

// Firmware Characteristics Extension Byte 1
const char* lazybiosFirmwareCharacteristicsExtByte1Str(uint8_t char_ext_byte_1) {
    _Thread_local static char buf[256];
    size_t len = 0;
    buf[0] = '\0';

    if (char_ext_byte_1 & (1 << 0)) len += snprintf(buf + len, sizeof(buf) - len, "ACPI supported, ");
    if (char_ext_byte_1 & (1 << 1)) len += snprintf(buf + len, sizeof(buf) - len, "USB Legacy supported, ");
    if (char_ext_byte_1 & (1 << 2)) len += snprintf(buf + len, sizeof(buf) - len, "AGP supported, ");
    if (char_ext_byte_1 & (1 << 3)) len += snprintf(buf + len, sizeof(buf) - len, "I2O boot supported, ");
    if (char_ext_byte_1 & (1 << 4)) len += snprintf(buf + len, sizeof(buf) - len, "LS-120 SuperDisk boot supported, ");
    if (char_ext_byte_1 & (1 << 5)) len += snprintf(buf + len, sizeof(buf) - len, "ATAPI ZIP drive boot supported, ");
    if (char_ext_byte_1 & (1 << 6)) len += snprintf(buf + len, sizeof(buf) - len, "1394 boot supported, ");
    if (char_ext_byte_1 & (1 << 7)) len += snprintf(buf + len, sizeof(buf) - len, "Smart battery supported, ");

    if (len == 0) return "None";
    if (len >= 2) buf[len - 2] = '\0';

    return buf;
}

// Firmware Characteristics Extension Byte 2
const char* lazybiosFirmwareCharacteristicsExtByte2Str(uint8_t char_ext_byte_2) {
    _Thread_local static char buf[512];
    size_t len = 0;
    buf[0] = '\0';

    if (char_ext_byte_2 & (1 << 0)) len += snprintf(buf + len, sizeof(buf) - len, "BIOS Boot Specification supported, ");
    if (char_ext_byte_2 & (1 << 1)) len += snprintf(buf + len, sizeof(buf) - len, "Function key-initiated network service boot supported, ");
    if (char_ext_byte_2 & (1 << 2)) len += snprintf(buf + len, sizeof(buf) - len, "Targeted content distribution supported, ");
    if (char_ext_byte_2 & (1 << 3)) len += snprintf(buf + len, sizeof(buf) - len, "UEFI Specification supported, ");
    if (char_ext_byte_2 & (1 << 4)) len += snprintf(buf + len, sizeof(buf) - len, "Describes a virtual machine, ");
    if (char_ext_byte_2 & (1 << 5)) len += snprintf(buf + len, sizeof(buf) - len, "Manufacturing mode supported, ");
    if (char_ext_byte_2 & (1 << 6)) len += snprintf(buf + len, sizeof(buf) - len, "Manufacturing mode enabled, ");

    if (len == 0) return "None";
    if (len >= 2) buf[len - 2] = '\0';

    return buf;
}

// Firmware Extended ROM Size
uint16_t lazybiosFirmwareExtendedROMSizeU16(uint16_t raw, char unit[5]) {
    uint16_t unit_bits = (raw >> 14) & 0x03;
    uint16_t size_bits = raw & 0x3FFF;

    switch (unit_bits) {
        case 0x0:
            strcpy(unit, "MiB");
            break;
        case 0x1:
            strcpy(unit, "GiB");
            break;
        default:
            strcpy(unit, "RES");
            break;
    }

    return size_bits;
}

// End of Decoders
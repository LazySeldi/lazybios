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
 * @file fuzz_decoders.c
 * @brief libFuzzer target for the human-readable decoder helpers.
 *
 * Every decoder is called with values taken from the input. The decoders that
 * format into a caller-supplied buffer get a heap buffer sized from the input
 * as well, so ASan reports a write of even a single byte past the capacity the
 * caller declared.
 */

#include "fuzz_common.h"

#include <stdint.h>
#include <stdlib.h>

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size);

/* Decoders that map a value to a static string. */
static void fuzz_string_decoders(fuzz_reader_t r) {
	fuzz_sink_str(lazybiosType1WakeupTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType2BoardTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType3StateStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType3SecurityStatusStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType4ProcessorFamilyStr((uint16_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType4SocketTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType4TypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType5ErrorDetectingMethodStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType5InterleaveStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType7ErrorCorrectionTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType7SystemCacheTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType7AssociativityStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType8ConnectorTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType8PortTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType9SlotTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType9SlotWidthStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType9CurrentUsageStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType9SlotLengthStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType9SlotHeightStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType10DeviceTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType10DeviceStatusStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType13LanguageFormatStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType15AccessMethodStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType15LogHeaderFormatStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType15LogTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType15VariableDataFormatTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType16LocationStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType16UseStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType16MemoryErrorCorrectionStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType17FormFactorStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType17TypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType17MemoryTechnologyStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType18ErrorTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType18ErrorGranularityStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType18ErrorOperationStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType21PointingDeviceTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType21InterfaceStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType22DeviceChemistryStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType23BootOptionStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType23BootOptionOnLimitStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType24PowerOnPasswordStatusStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType24KeyboardPasswordStatusStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType24AdministratorPasswordStatusStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType24FrontPanelResetStatusStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType26LocationStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType26StatusStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType27DeviceTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType27StatusStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType28LocationStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType28StatusStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType29LocationStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType29StatusStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType30InboundConnectionStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType30OutboundConnectionStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType32BootStatusStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType33ErrorTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType33ErrorGranularityStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType33ErrorOperationStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType34DeviceTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType34AddressTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType37ChannelTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType38InterfaceTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType38BaseAddressTypeStr((uint64_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType38RegisterSpacingStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType39PowerSupplyTypeStr((uint16_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType39StatusStr((uint16_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType39InputVoltageRangeSwitchingStr((uint16_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType41DeviceTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType41DeviceStatusStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType42InterfaceTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType42ProtocolTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType44ProcessorTypeStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType45VersionFormatStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType45FirmwareIDFormatStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType45StateStr((uint8_t)fuzz_u64(&r)));
	fuzz_sink_str(lazybiosType46StringPropertyIDStr((uint16_t)fuzz_u64(&r)));
}

/* Decoders that convert an encoded value. */
static void fuzz_value_decoders(fuzz_reader_t r) {
	fuzz_sink_val(lazybiosType7CacheU16((uint16_t)fuzz_u64(&r)));
	fuzz_sink_val(lazybiosType7CacheU32((uint32_t)fuzz_u64(&r)));
	fuzz_sink_val(lazybiosType15IndexAddress((uint32_t)fuzz_u64(&r)));
	fuzz_sink_val(lazybiosType15DataAddress((uint32_t)fuzz_u64(&r)));
	fuzz_sink_val(lazybiosType15GPNVHandle((uint32_t)fuzz_u64(&r)));
	fuzz_sink_val(lazybiosType16MaximumCapacityBytes((uint32_t)fuzz_u64(&r), (uint64_t)fuzz_u64(&r)));
	fuzz_sink_val(lazybiosType19StartingAddressBytes((uint32_t)fuzz_u64(&r), (uint64_t)fuzz_u64(&r)));
	fuzz_sink_val(lazybiosType19EndingAddressBytes((uint32_t)fuzz_u64(&r), (uint64_t)fuzz_u64(&r)));
	fuzz_sink_val(lazybiosType20StartingAddressBytes((uint32_t)fuzz_u64(&r), (uint64_t)fuzz_u64(&r)));
	fuzz_sink_val(lazybiosType20EndingAddressBytes((uint32_t)fuzz_u64(&r), (uint64_t)fuzz_u64(&r)));
	fuzz_sink_val(lazybiosType22DesignCapacityMWh((uint16_t)fuzz_u64(&r), (uint8_t)fuzz_u64(&r)));
	fuzz_sink_val(lazybiosType38BaseAddressValue((uint64_t)fuzz_u64(&r), (uint8_t)fuzz_u64(&r)));
}

/* Decoders that format into a caller-supplied buffer. */
static void fuzz_buffer_decoders(fuzz_reader_t r, char* buf, size_t buf_len) {
	char unit[5];

	lazybiosType0CharacteristicsStr((uint64_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType0CharacteristicsExtByte1Str((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType0CharacteristicsExtByte2Str((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType2FeatureflagsStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType3TypeStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType3ContainedElementTypeStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType4CharacteristicsStr((uint16_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType4StatusStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType4VoltageStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType5ErrorCorrectingCapabilityStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType5SupportedSpeedsStr((uint16_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType5SupportedMemoryTypesStr((uint16_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType5MemoryModuleVoltageStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType6BankConnectionsStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType6CurrentMemoryTypeStr((uint16_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType6InstalledSizeStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType6EnabledSizeStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType6ErrorStatusStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType7SRAMTypeStr((uint16_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType7CacheConfigurationStr((uint16_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType9Characteristics1Str((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType9Characteristics2Str((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType9DeviceFunctionStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType15LogStatusStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType17TypeDetailStr((uint16_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType17ExtendedSizeStr((uint32_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType17OperatingModeCapabilityStr((uint16_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType17ModuleManufacturerIDStr((uint16_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType17VolatileSizeStr((uint64_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType17NonVolatileSizeStr((uint64_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType17CacheSizeStr((uint64_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType17ExtendedSpeedStr((uint32_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType17PMIC0ManufacturerIDStr((uint16_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType17PMIC0RevisionStr((uint16_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType17RCDManufacturerIDStr((uint16_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType17RCDRevisionStr((uint16_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType22SBDSManufactureDateStr((uint16_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType23CapabilitiesStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType38SpecificationRevisionStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType38InterruptInfoStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType39CharacteristicsFlagsStr((uint16_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType41DeviceFunctionStr((uint8_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType43FirmwareVersionStr((uint8_t)fuzz_u64(&r), (uint32_t)fuzz_u64(&r), (uint32_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType43CharacteristicsStr((uint64_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType45CharacteristicsStr((uint16_t)fuzz_u64(&r), buf, buf_len);
	lazybiosType0ExtendedROMSizeU16((uint16_t)fuzz_u64(&r), unit);
	fuzz_sink_str(unit);

	lazybiosType25_t type25;
	type25.next_scheduled_power_on_month = fuzz_u8(&r);
	type25.next_scheduled_power_on_day = fuzz_u8(&r);
	type25.next_scheduled_power_on_hour = fuzz_u8(&r);
	type25.next_scheduled_power_on_minute = fuzz_u8(&r);
	type25.next_scheduled_power_on_second = fuzz_u8(&r);
	type25.field_status.next_scheduled_power_on_month = (fuzz_u8(&r) & 1) ? LAZYBIOS_FIELD_PRESENT : LAZYBIOS_FIELD_ABSENT;
	type25.field_status.next_scheduled_power_on_day = (fuzz_u8(&r) & 1) ? LAZYBIOS_FIELD_PRESENT : LAZYBIOS_FIELD_ABSENT;
	type25.field_status.next_scheduled_power_on_hour = (fuzz_u8(&r) & 1) ? LAZYBIOS_FIELD_PRESENT : LAZYBIOS_FIELD_ABSENT;
	type25.field_status.next_scheduled_power_on_minute = (fuzz_u8(&r) & 1) ? LAZYBIOS_FIELD_PRESENT : LAZYBIOS_FIELD_ABSENT;
	type25.field_status.next_scheduled_power_on_second = (fuzz_u8(&r) & 1) ? LAZYBIOS_FIELD_PRESENT : LAZYBIOS_FIELD_ABSENT;
	lazybiosType25NextScheduledPowerOnStr(&type25, buf, buf_len);
}

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
	if (size < 2) return 0;

	/*
	 * The reported capacity always matches the real allocation, so anything
	 * ASan flags here is a decoder writing outside the buffer it was given.
	 */
	size_t buf_len = (size_t)data[0] % (LAZYBIOS_DECODER_BUF_SIZE * 2 + 1);
	char* buf = malloc(buf_len);
	if (buf_len != 0 && !buf) return 0;

	fuzz_reader_t r;
	fuzz_reader_init(&r, data + 1, size - 1);

	fuzz_string_decoders(r);
	fuzz_value_decoders(r);
	fuzz_buffer_decoders(r, buf, buf_len);

	free(buf);
	return 0;
}

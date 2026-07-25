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
 * @file macos.c
 * @brief Implements native SMBIOS loading on macOS.
 */

#include "lazybios_internal.h"

#if defined(OS_MACOS)

#include <AvailabilityMacros.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOTypes.h>

int lazybiosMacOS(lazybiosCTX_t *ctx) {
	if (!ctx)
		return -1;

	// For Macs older than 12.0 we use Master instead of Main
	#if MAC_OS_X_VERSION_MIN_REQUIRED >= 120000
	io_service_t service = IOServiceGetMatchingService(kIOMainPortDefault, IOServiceMatching("AppleSMBIOS"));
	#else
	io_service_t service = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("AppleSMBIOS"));
	#endif

	if (service == IO_OBJECT_NULL) {
		lb_log("AppleSMBIOS not found\n");
		return -1;
	}

	CFTypeRef eps_property = IORegistryEntryCreateCFProperty(service, CFSTR("SMBIOS-EPS"), kCFAllocatorDefault, 0);
	if (!eps_property || CFGetTypeID(eps_property) != CFDataGetTypeID()) {
		lb_log("Entry point structure not found\n");
		if (eps_property)
			CFRelease(eps_property);
		IOObjectRelease(service);
		return -1;
	}
	CFDataRef eps = (CFDataRef)eps_property;

	CFTypeRef dmi_property = IORegistryEntryCreateCFProperty(service, CFSTR("SMBIOS"), kCFAllocatorDefault, 0);
	if (!dmi_property || CFGetTypeID(dmi_property) != CFDataGetTypeID()) {
		lb_log("DMI Table not found\n");
		if (dmi_property)
			CFRelease(dmi_property);
		IOObjectRelease(service);
		CFRelease(eps);
		return -1;
	}
	CFDataRef dmi = (CFDataRef)dmi_property;

	CFIndex entry_length = CFDataGetLength(eps);
	CFIndex dmi_length = CFDataGetLength(dmi);
	int result = -1;
	if (entry_length > 0 && dmi_length > 0) {
		result = lazybiosLoadRawBuffers(ctx, CFDataGetBytePtr(eps), (size_t)entry_length, CFDataGetBytePtr(dmi),
										(size_t)dmi_length);
	}

	CFRelease(eps);
	CFRelease(dmi);
	IOObjectRelease(service);
	return result;
}

#endif

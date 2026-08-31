/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file windows.c
 * @brief Implements native SMBIOS loading on Windows.
 */
#include "lazybios_internal.h"

#if defined(OS_WINDOWS)

	#include <stdlib.h>
	#include <windows.h>

int lazybiosWindows(lazybiosCTX_t *ctx) { // Help with the windows backend is appriciated, since I'm not an expert at
										  // the windows API, and it was not fully made by me.
	if (!ctx)
		return -1;

	// RSMB provider ID (little endian for "RSMB")
	const DWORD sig = 0x52534D42;

	DWORD size = GetSystemFirmwareTable(sig, 0, NULL, 0);
	if (size == 0) {
		lb_log("GetSystemFirmwareTable failed (size=0)");
		return -1;
	}

	uint8_t *buf = malloc(size);
	if (!buf)
		return -1;

	DWORD got = GetSystemFirmwareTable(sig, 0, buf, size);
	if (got != size) {
		lb_log("SMBIOS read mismatch (%lu != %lu)", (unsigned long)got, (unsigned long)size);
		free(buf);
		return -1;
	}

	int result = lazybiosLoadWindowsRawSMBIOSData(ctx, buf, size);
	free(buf);
	return result;
}

#endif

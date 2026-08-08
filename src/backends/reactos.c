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
 * @file reactos.c
 * @brief Implements native SMBIOS loading on ReactOS.
 */

#include "lazybios_internal.h"

#if defined(OS_REACTOS)

    #include <stdlib.h>
    #include <windows.h>

int lazybiosReactOS(lazybiosCTX_t *ctx) { // This is how it's supposed to look like, similar to windows's backend, but I have no idea if it works. I can't find any way to compile something for something as old as reactOS
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

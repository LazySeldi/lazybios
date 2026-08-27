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
 * @file minix.c
 * @brief Implements native SMBIOS loading on MINIX 3.
 */
#include "lazybios_internal.h"

#if defined(OS_MINIX)

int lazybiosMINIX(lazybiosCTX_t *ctx) {
    static const char *const device_paths[] = {DEV_MEM};

    return lazybiosLoadLegacyPhysicalMemory(ctx, device_paths, sizeof(device_paths) / sizeof(device_paths[0]), "MINIX");
}

#endif

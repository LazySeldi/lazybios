/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file haiku.c
 * @brief Implements native SMBIOS loading on Haiku.
 */
#include "lazybios_internal.h"

#if defined(OS_HAIKU)

int lazybiosHaiku(lazybiosCTX_t *ctx) {
    static const char *const device_paths[] = {"/dev/misc/mem", DEV_MEM};

    return lazybiosLoadLegacyPhysicalMemory(ctx, device_paths, sizeof(device_paths) / sizeof(device_paths[0]), "Haiku");
}

#endif

/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file beos.c
 * @brief Implements native SMBIOS loading on BeOS.
 */
#include "lazybios_internal.h"

#if defined(OS_BEOS)

int lazybiosBeOS(lazybiosCTX_t *ctx) {
    static const char *const device_paths[] = {"/dev/misc/mem", DEV_MEM};

    return lazybiosLoadLegacyPhysicalMemory(ctx, device_paths, sizeof(device_paths) / sizeof(device_paths[0]), "BeOS");
}

#endif

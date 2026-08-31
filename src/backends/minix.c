/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
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

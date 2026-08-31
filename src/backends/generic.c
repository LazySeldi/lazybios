/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file generic.c
 * @brief Implements legacy SMBIOS loading on compatible unknown systems.
 */
#include "lazybios_internal.h"

#if defined(OS_GENERIC)

int lazybiosGeneric(lazybiosCTX_t *ctx) {
    static const char *const device_paths[] = {DEV_MEM, "/dev/misc/mem"};

    return lazybiosLoadLegacyPhysicalMemory(ctx, device_paths, sizeof(device_paths) / sizeof(device_paths[0]), "Generic");
}

#endif

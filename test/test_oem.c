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
 * @file test_oem.c
 * @brief Main entry point and dispatcher for OEM SMBIOS structure tests.
 * @author LazySeldi
 */

#include "oem/test_oem.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lazybios/lazybios.h"

static void printOemStructures(lazybiosCTX_t* ctx) {
    printOemHP(ctx);
    printOemDell(ctx);
}

#ifndef TEST_OEM_NO_MAIN
int main(int argc, char* argv[]) {
    const char* entry_file = NULL;
    const char* dmi_file = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--sources") == 0) {
            if (i + 2 <= argc) {
                entry_file = argv[i + 1];
                dmi_file = argv[i + 2];
            }
        }
    }

    lazybiosCTX_t* ctx = lazybiosCTXNew();
    if (!ctx) {
        fprintf(stderr, "Failed to create lazybios context\n");
        return -1;
    }

    if (entry_file && dmi_file) {
        if (lazybiosInit(ctx, entry_file, dmi_file) != 0) {
            fprintf(stderr, "Failed to load source files\n");
            lazybiosCleanup(ctx);
            return -1;
        }
    } else if (lazybiosInit(ctx, NULL, NULL) != 0) {
        fprintf(stderr, "Failed to initialize lazybios context\n");
        lazybiosCleanup(ctx);
        return -1;
    }



    printOemStructures(ctx);

    lazybiosCleanup(ctx);
    return 0;
}
#endif

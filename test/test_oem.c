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

#include "lazybios.h"
#include "oem/test_oem.h"
#include <stdio.h>

void printOemStructures(lazybiosCTX_t* ctx) {
#ifdef LAZYBIOS_OEM_HP
    printOemHP(ctx);
#endif

}

#ifndef TEST_OEM_NO_MAIN
int main() {

    lazybiosCTX_t* ctx = lazybiosCTXNew();
    if (!ctx) {
        fprintf(stderr, "Failed to create lazybios context\n");
        return -1;
    }

    if (lazybiosInit(ctx) != 0) {
        fprintf(stderr, "Failed to initialize lazybios context\n");
        lazybiosCleanup(ctx);
        return -1;
    }

    printOemStructures(ctx);

    lazybiosCleanup(ctx);
    return 0;
}
#endif

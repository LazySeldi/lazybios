/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
/**
 * @file test_oem.h
 * @brief Common header for OEM SMBIOS structure tests.
 * @author LazySeldi
 */

#ifndef LAZYBIOS_TEST_OEM_H
#define LAZYBIOS_TEST_OEM_H

#include "lazybios/lazybios.h"

void printOemDell(lazybiosCTX_t* ctx);
void printOemHP(lazybiosCTX_t* ctx);

#endif

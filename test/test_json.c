/*
 * SPDX-License-Identifier: MIT
 *
 * This file is part of lazybios.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lazybios/lazybios.h"
#include "lazybios/json/lazybios_json.h"

static inline void printType(int type, lazybiosCTX_t* ctx, cJSON* root) {
    switch (type) {
        case 0:
            ctx->Type0 = lazybiosGetType0(ctx->DMIData);
            lazybiosExtJSONAddType0(ctx->Type0, root);
            break;
        case 1:
            ctx->Type1 = lazybiosGetType1(ctx->DMIData);
            lazybiosExtJSONAddType1(ctx->Type1, root);
            break;
        case 2:
            ctx->Type2 = lazybiosGetType2(ctx->DMIData);
            lazybiosExtJSONAddType2(ctx->Type2, root);
            break;
        case 3:
            ctx->Type3 = lazybiosGetType3(ctx->DMIData);
            lazybiosExtJSONAddType3(ctx->Type3, root);
            break;
        case 4:
            ctx->Type4 = lazybiosGetType4(ctx->DMIData);
            lazybiosExtJSONAddType4(ctx->Type4, root);
            break;
        case 5:
            ctx->Type5 = lazybiosGetType5(ctx->DMIData);
            lazybiosExtJSONAddType5(ctx->Type5, root);
            break;
        case 6:
            ctx->Type6 = lazybiosGetType6(ctx->DMIData);
            lazybiosExtJSONAddType6(ctx->Type6, root);
            break;
        case 7:
            ctx->Type7 = lazybiosGetType7(ctx->DMIData);
            lazybiosExtJSONAddType7(ctx->Type7, root);
            break;
        case 8:
            ctx->Type8 = lazybiosGetType8(ctx->DMIData);
            lazybiosExtJSONAddType8(ctx->Type8, root);
            break;
        case 9:
            ctx->Type9 = lazybiosGetType9(ctx->DMIData);
            lazybiosExtJSONAddType9(ctx->Type9, root);
            break;
        case 10:
            ctx->Type10 = lazybiosGetType10(ctx->DMIData);
            lazybiosExtJSONAddType10(ctx->Type10, root);
            break;
        case 11:
            ctx->Type11 = lazybiosGetType11(ctx->DMIData);
            lazybiosExtJSONAddType11(ctx->Type11, root);
            break;
        case 12:
            ctx->Type12 = lazybiosGetType12(ctx->DMIData);
            lazybiosExtJSONAddType12(ctx->Type12, root);
            break;
        case 13:
            ctx->Type13 = lazybiosGetType13(ctx->DMIData);
            lazybiosExtJSONAddType13(ctx->Type13, root);
            break;
        case 14:
            ctx->Type14 = lazybiosGetType14(ctx->DMIData);
            lazybiosExtJSONAddType14(ctx->Type14, root);
            break;
        case 15:
            ctx->Type15 = lazybiosGetType15(ctx->DMIData);
            lazybiosExtJSONAddType15(ctx->Type15, root);
            break;
        case 16:
            ctx->Type16 = lazybiosGetType16(ctx->DMIData);
            lazybiosExtJSONAddType16(ctx->Type16, root);
            break;
        case 17:
            ctx->Type17 = lazybiosGetType17(ctx->DMIData);
            lazybiosExtJSONAddType17(ctx->Type17, root);
            break;
        case 18:
            ctx->Type18 = lazybiosGetType18(ctx->DMIData);
            lazybiosExtJSONAddType18(ctx->Type18, root);
            break;
        case 19:
            ctx->Type19 = lazybiosGetType19(ctx->DMIData);
            lazybiosExtJSONAddType19(ctx->Type19, root);
            break;
        case 20:
            ctx->Type20 = lazybiosGetType20(ctx->DMIData);
            lazybiosExtJSONAddType20(ctx->Type20, root);
            break;
        case 21:
            ctx->Type21 = lazybiosGetType21(ctx->DMIData);
            lazybiosExtJSONAddType21(ctx->Type21, root);
            break;
        case 22:
            ctx->Type22 = lazybiosGetType22(ctx->DMIData);
            lazybiosExtJSONAddType22(ctx->Type22, root);
            break;
        case 23:
            ctx->Type23 = lazybiosGetType23(ctx->DMIData);
            lazybiosExtJSONAddType23(ctx->Type23, root);
            break;
        case 24:
            ctx->Type24 = lazybiosGetType24(ctx->DMIData);
            lazybiosExtJSONAddType24(ctx->Type24, root);
            break;
        case 25:
            ctx->Type25 = lazybiosGetType25(ctx->DMIData);
            lazybiosExtJSONAddType25(ctx->Type25, root);
            break;
        case 26:
            ctx->Type26 = lazybiosGetType26(ctx->DMIData);
            lazybiosExtJSONAddType26(ctx->Type26, root);
            break;
        case 27:
            ctx->Type27 = lazybiosGetType27(ctx->DMIData);
            lazybiosExtJSONAddType27(ctx->Type27, root);
            break;
        case 28:
            ctx->Type28 = lazybiosGetType28(ctx->DMIData);
            lazybiosExtJSONAddType28(ctx->Type28, root);
            break;
        case 29:
            ctx->Type29 = lazybiosGetType29(ctx->DMIData);
            lazybiosExtJSONAddType29(ctx->Type29, root);
            break;
        case 30:
            ctx->Type30 = lazybiosGetType30(ctx->DMIData);
            lazybiosExtJSONAddType30(ctx->Type30, root);
            break;
        case 31:
            ctx->Type31 = lazybiosGetType31(ctx->DMIData);
            lazybiosExtJSONAddType31(ctx->Type31, root);
            break;
        case 32:
            ctx->Type32 = lazybiosGetType32(ctx->DMIData);
            lazybiosExtJSONAddType32(ctx->Type32, root);
            break;
        case 33:
            ctx->Type33 = lazybiosGetType33(ctx->DMIData);
            lazybiosExtJSONAddType33(ctx->Type33, root);
            break;
        case 34:
            ctx->Type34 = lazybiosGetType34(ctx->DMIData);
            lazybiosExtJSONAddType34(ctx->Type34, root);
            break;
        case 35:
            ctx->Type35 = lazybiosGetType35(ctx->DMIData);
            lazybiosExtJSONAddType35(ctx->Type35, root);
            break;
        case 36:
            ctx->Type36 = lazybiosGetType36(ctx->DMIData);
            lazybiosExtJSONAddType36(ctx->Type36, root);
            break;
        case 37:
            ctx->Type37 = lazybiosGetType37(ctx->DMIData);
            lazybiosExtJSONAddType37(ctx->Type37, root);
            break;
        case 38:
            ctx->Type38 = lazybiosGetType38(ctx->DMIData);
            lazybiosExtJSONAddType38(ctx->Type38, root);
            break;
        case 39:
            ctx->Type39 = lazybiosGetType39(ctx->DMIData);
            lazybiosExtJSONAddType39(ctx->Type39, root);
            break;
        case 40:
            ctx->Type40 = lazybiosGetType40(ctx->DMIData);
            lazybiosExtJSONAddType40(ctx->Type40, root);
            break;
        case 41:
            ctx->Type41 = lazybiosGetType41(ctx->DMIData);
            lazybiosExtJSONAddType41(ctx->Type41, root);
            break;
        case 42:
            ctx->Type42 = lazybiosGetType42(ctx->DMIData);
            lazybiosExtJSONAddType42(ctx->Type42, root);
            break;
        case 43:
            ctx->Type43 = lazybiosGetType43(ctx->DMIData);
            lazybiosExtJSONAddType43(ctx->Type43, root);
            break;
        case 44:
            ctx->Type44 = lazybiosGetType44(ctx->DMIData);
            lazybiosExtJSONAddType44(ctx->Type44, root);
            break;
        case 45:
            ctx->Type45 = lazybiosGetType45(ctx->DMIData);
            lazybiosExtJSONAddType45(ctx->Type45, root);
            break;
        case 46:
            ctx->Type46 = lazybiosGetType46(ctx->DMIData);
            lazybiosExtJSONAddType46(ctx->Type46, root);
            break;
        case 177:
            ctx->oem->dell->Type177 = lazybiosGetOemDellType177(ctx->DMIData);
            lazybiosExtJSONAddOemDellType177(ctx->oem->dell->Type177, root);
            break;
        case 204:
            ctx->oem->hp->Type204 = lazybiosGetOemHpType204(ctx->DMIData);
            lazybiosExtJSONAddOemHpType204(ctx->oem->hp->Type204, root);
            break;
        case 212:
            ctx->oem->dell->Type212 = lazybiosGetOemDellType212(ctx->DMIData);
            lazybiosExtJSONAddOemDellType212(ctx->oem->dell->Type212, root);
            break;
        case 218:
            ctx->oem->dell->Type218 = lazybiosGetOemDellType218(ctx->DMIData);
            lazybiosExtJSONAddOemDellType218(ctx->oem->dell->Type218, root);
            break;
        default:
            printf("Invalid Type!");
            break;
    }
}

static inline void printAll(lazybiosCTX_t* ctx, cJSON* root) {
    ctx->Type0 = lazybiosGetType0(ctx->DMIData);
    ctx->Type1 = lazybiosGetType1(ctx->DMIData);
    ctx->Type2 = lazybiosGetType2(ctx->DMIData);
    ctx->Type3 = lazybiosGetType3(ctx->DMIData);
    ctx->Type4 = lazybiosGetType4(ctx->DMIData);
    ctx->Type5 = lazybiosGetType5(ctx->DMIData);
    ctx->Type6 = lazybiosGetType6(ctx->DMIData);
    ctx->Type7 = lazybiosGetType7(ctx->DMIData);
    ctx->Type8 = lazybiosGetType8(ctx->DMIData);
    ctx->Type9 = lazybiosGetType9(ctx->DMIData);
    ctx->Type10 = lazybiosGetType10(ctx->DMIData);
    ctx->Type11 = lazybiosGetType11(ctx->DMIData);
    ctx->Type12 = lazybiosGetType12(ctx->DMIData);
    ctx->Type13 = lazybiosGetType13(ctx->DMIData);
    ctx->Type14 = lazybiosGetType14(ctx->DMIData);
    ctx->Type15 = lazybiosGetType15(ctx->DMIData);
    ctx->Type16 = lazybiosGetType16(ctx->DMIData);
    ctx->Type17 = lazybiosGetType17(ctx->DMIData);
    ctx->Type18 = lazybiosGetType18(ctx->DMIData);
    ctx->Type19 = lazybiosGetType19(ctx->DMIData);
    ctx->Type20 = lazybiosGetType20(ctx->DMIData);
    ctx->Type21 = lazybiosGetType21(ctx->DMIData);
    ctx->Type22 = lazybiosGetType22(ctx->DMIData);
    ctx->Type23 = lazybiosGetType23(ctx->DMIData);
    ctx->Type24 = lazybiosGetType24(ctx->DMIData);
    ctx->Type25 = lazybiosGetType25(ctx->DMIData);
    ctx->Type26 = lazybiosGetType26(ctx->DMIData);
    ctx->Type27 = lazybiosGetType27(ctx->DMIData);
    ctx->Type28 = lazybiosGetType28(ctx->DMIData);
    ctx->Type29 = lazybiosGetType29(ctx->DMIData);
    ctx->Type30 = lazybiosGetType30(ctx->DMIData);
    ctx->Type31 = lazybiosGetType31(ctx->DMIData);
    ctx->Type32 = lazybiosGetType32(ctx->DMIData);
    ctx->Type33 = lazybiosGetType33(ctx->DMIData);
    ctx->Type34 = lazybiosGetType34(ctx->DMIData);
    ctx->Type35 = lazybiosGetType35(ctx->DMIData);
    ctx->Type36 = lazybiosGetType36(ctx->DMIData);
    ctx->Type37 = lazybiosGetType37(ctx->DMIData);
    ctx->Type38 = lazybiosGetType38(ctx->DMIData);
    ctx->Type39 = lazybiosGetType39(ctx->DMIData);
    ctx->Type40 = lazybiosGetType40(ctx->DMIData);
    ctx->Type41 = lazybiosGetType41(ctx->DMIData);
    ctx->Type42 = lazybiosGetType42(ctx->DMIData);
    ctx->Type43 = lazybiosGetType43(ctx->DMIData);
    ctx->Type44 = lazybiosGetType44(ctx->DMIData);
    ctx->Type45 = lazybiosGetType45(ctx->DMIData);
    ctx->Type46 = lazybiosGetType46(ctx->DMIData);

    ctx->oem->dell->Type177 = lazybiosGetOemDellType177(ctx->DMIData);
    ctx->oem->dell->Type212 = lazybiosGetOemDellType212(ctx->DMIData);
    ctx->oem->dell->Type218 = lazybiosGetOemDellType218(ctx->DMIData);
    ctx->oem->hp->Type204 = lazybiosGetOemHpType204(ctx->DMIData);

    lazybiosExtJSONAddType0(ctx->Type0, root);
    lazybiosExtJSONAddType1(ctx->Type1, root);
    lazybiosExtJSONAddType2(ctx->Type2, root);
    lazybiosExtJSONAddType3(ctx->Type3, root);
    lazybiosExtJSONAddType4(ctx->Type4, root);
    lazybiosExtJSONAddType5(ctx->Type5, root);
    lazybiosExtJSONAddType6(ctx->Type6, root);
    lazybiosExtJSONAddType7(ctx->Type7, root);
    lazybiosExtJSONAddType8(ctx->Type8, root);
    lazybiosExtJSONAddType9(ctx->Type9, root);
    lazybiosExtJSONAddType10(ctx->Type10, root);
    lazybiosExtJSONAddType11(ctx->Type11, root);
    lazybiosExtJSONAddType12(ctx->Type12, root);
    lazybiosExtJSONAddType13(ctx->Type13, root);
    lazybiosExtJSONAddType14(ctx->Type14, root);
    lazybiosExtJSONAddType15(ctx->Type15, root);
    lazybiosExtJSONAddType16(ctx->Type16, root);
    lazybiosExtJSONAddType17(ctx->Type17, root);
    lazybiosExtJSONAddType18(ctx->Type18, root);
    lazybiosExtJSONAddType19(ctx->Type19, root);
    lazybiosExtJSONAddType20(ctx->Type20, root);
    lazybiosExtJSONAddType21(ctx->Type21, root);
    lazybiosExtJSONAddType22(ctx->Type22, root);
    lazybiosExtJSONAddType23(ctx->Type23, root);
    lazybiosExtJSONAddType24(ctx->Type24, root);
    lazybiosExtJSONAddType25(ctx->Type25, root);
    lazybiosExtJSONAddType26(ctx->Type26, root);
    lazybiosExtJSONAddType27(ctx->Type27, root);
    lazybiosExtJSONAddType28(ctx->Type28, root);
    lazybiosExtJSONAddType29(ctx->Type29, root);
    lazybiosExtJSONAddType30(ctx->Type30, root);
    lazybiosExtJSONAddType31(ctx->Type31, root);
    lazybiosExtJSONAddType32(ctx->Type32, root);
    lazybiosExtJSONAddType33(ctx->Type33, root);
    lazybiosExtJSONAddType34(ctx->Type34, root);
    lazybiosExtJSONAddType35(ctx->Type35, root);
    lazybiosExtJSONAddType36(ctx->Type36, root);
    lazybiosExtJSONAddType37(ctx->Type37, root);
    lazybiosExtJSONAddType38(ctx->Type38, root);
    lazybiosExtJSONAddType39(ctx->Type39, root);
    lazybiosExtJSONAddType40(ctx->Type40, root);
    lazybiosExtJSONAddType41(ctx->Type41, root);
    lazybiosExtJSONAddType42(ctx->Type42, root);
    lazybiosExtJSONAddType43(ctx->Type43, root);
    lazybiosExtJSONAddType44(ctx->Type44, root);
    lazybiosExtJSONAddType45(ctx->Type45, root);
    lazybiosExtJSONAddType46(ctx->Type46, root);

    lazybiosExtJSONAddOemDellType177(ctx->oem->dell->Type177, root);
    lazybiosExtJSONAddOemDellType212(ctx->oem->dell->Type212, root);
    lazybiosExtJSONAddOemDellType218(ctx->oem->dell->Type218, root);
    lazybiosExtJSONAddOemHpType204(ctx->oem->hp->Type204, root);
}

int main(int argc, char* argv[]) {

    int type = 3500;
    const char* entry_file = NULL;
    const char* dmi_file = NULL;
    for (int i = 0; i < argc; i++) { // Very minimal, not like test.c
        if (strcmp(argv[i], "--type") == 0 || strcmp(argv[i], "-t") == 0) {
            type = (int)strtol(argv[i + 1], NULL, 0);
        } else if (strcmp(argv[i], "--sources") == 0) {
            entry_file = argv[i + 1];
            dmi_file = argv[i + 2];
        } else if (strcmp(argv[i], "--all") == 0 && entry_file == NULL) {

        }
    }

    lazybiosCTX_t *ctx = lazybiosCTXNew();
    if (!ctx) {
        fprintf(stderr, "Failed to create lazybios context\n");
        return -1;
    }
    cJSON *root = cJSON_CreateObject();
    const int from_files = (entry_file != NULL && dmi_file != NULL);
    if (lazybiosInit(ctx, from_files ? entry_file : NULL,
                          from_files ? dmi_file   : NULL) != 0) {
        fprintf(stderr, "Failed to load SMBIOS data\n");
        cJSON_Delete(root);
        lazybiosCleanup(ctx);
        return -1;
    }
    lazybiosExtJSONAddSMBIOSInfo(ctx->DMIData, ctx->backend, root);
    if (type == 3500) {
        printAll(ctx, root);
    } else {
        // printType's default case reports unknown numbers, and OEM types
        // (177, 204, 212, 218) sit above the standard 0-46 range.
        printType(type, ctx, root);
    }

    char *json_string = cJSON_Print(root);
    printf("%s\n", json_string);

    cJSON_Delete(root);
    lazybiosCleanup(ctx);

    return 0;
}
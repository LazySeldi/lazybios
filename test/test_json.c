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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lazybios/lazybios.h"
#include "lazybios/json/lazybios_json.h"

static inline void printType(int type, lazybiosCTX_t* ctx, cJSON* root) {
    switch (type) {
        case 0:
            ctx->Type0 = lazybiosGetType0(ctx->Type0, &ctx->type0_count, ctx->DMIData);
            lazybiosExtJSONAddType0(ctx->Type0, ctx->type0_count, root);
            break;
        case 1:
            ctx->Type1 = lazybiosGetType1(ctx->Type1, &ctx->type1_count, ctx->DMIData);
            lazybiosExtJSONAddType1(ctx->Type1, ctx->type1_count, root);
            break;
        case 2:
            ctx->Type2 = lazybiosGetType2(ctx->Type2, &ctx->type2_count, ctx->DMIData);
            lazybiosExtJSONAddType2(ctx->Type2, ctx->type2_count, root);
            break;
        case 3:
            ctx->Type3 = lazybiosGetType3(ctx->Type3, &ctx->type3_count, ctx->DMIData);
            lazybiosExtJSONAddType3(ctx->Type3, ctx->type3_count, root);
            break;
        case 4:
            ctx->Type4 = lazybiosGetType4(ctx->Type4, &ctx->type4_count, ctx->DMIData);
            lazybiosExtJSONAddType4(ctx->Type4, ctx->type4_count, root);
            break;
        case 5:
            ctx->Type5 = lazybiosGetType5(ctx->Type5, &ctx->type5_count, ctx->DMIData);
            lazybiosExtJSONAddType5(ctx->Type5, ctx->type5_count, root);
            break;
        case 6:
            ctx->Type6 = lazybiosGetType6(ctx->Type6, &ctx->type6_count, ctx->DMIData);
            lazybiosExtJSONAddType6(ctx->Type6, ctx->type6_count, root);
            break;
        case 7:
            ctx->Type7 = lazybiosGetType7(ctx->Type7, &ctx->type7_count, ctx->DMIData);
            lazybiosExtJSONAddType7(ctx->Type7, ctx->type7_count, root);
            break;
        case 8:
            ctx->Type8 = lazybiosGetType8(ctx->Type8, &ctx->type8_count, ctx->DMIData);
            lazybiosExtJSONAddType8(ctx->Type8, ctx->type8_count, root);
            break;
        case 9:
            ctx->Type9 = lazybiosGetType9(ctx->Type9, &ctx->type9_count, ctx->DMIData);
            lazybiosExtJSONAddType9(ctx->Type9, ctx->type9_count, root);
            break;
        case 10:
            ctx->Type10 = lazybiosGetType10(ctx->Type10, &ctx->type10_count, ctx->DMIData);
            lazybiosExtJSONAddType10(ctx->Type10, ctx->type10_count, root);
            break;
        case 11:
            ctx->Type11 = lazybiosGetType11(ctx->Type11, &ctx->type11_count, ctx->DMIData);
            lazybiosExtJSONAddType11(ctx->Type11, ctx->type11_count, root);
            break;
        case 12:
            ctx->Type12 = lazybiosGetType12(ctx->Type12, &ctx->type12_count, ctx->DMIData);
            lazybiosExtJSONAddType12(ctx->Type12, ctx->type12_count, root);
            break;
        case 13:
            ctx->Type13 = lazybiosGetType13(ctx->Type13, &ctx->type13_count, ctx->DMIData);
            lazybiosExtJSONAddType13(ctx->Type13, ctx->type13_count, root);
            break;
        case 14:
            ctx->Type14 = lazybiosGetType14(ctx->Type14, &ctx->type14_count, ctx->DMIData);
            lazybiosExtJSONAddType14(ctx->Type14, ctx->type14_count, root);
            break;
        case 15:
            ctx->Type15 = lazybiosGetType15(ctx->Type15, &ctx->type15_count, ctx->DMIData);
            lazybiosExtJSONAddType15(ctx->Type15, ctx->type15_count, root);
            break;
        case 16:
            ctx->Type16 = lazybiosGetType16(ctx->Type16, &ctx->type16_count, ctx->DMIData);
            lazybiosExtJSONAddType16(ctx->Type16, ctx->type16_count, root);
            break;
        case 17:
            ctx->Type17 = lazybiosGetType17(ctx->Type17, &ctx->type17_count, ctx->DMIData);
            lazybiosExtJSONAddType17(ctx->Type17, ctx->type17_count, root);
            break;
        case 18:
            ctx->Type18 = lazybiosGetType18(ctx->Type18, &ctx->type18_count, ctx->DMIData);
            lazybiosExtJSONAddType18(ctx->Type18, ctx->type18_count, root);
            break;
        case 19:
            ctx->Type19 = lazybiosGetType19(ctx->Type19, &ctx->type19_count, ctx->DMIData);
            lazybiosExtJSONAddType19(ctx->Type19, ctx->type19_count, root);
            break;
        case 20:
            ctx->Type20 = lazybiosGetType20(ctx->Type20, &ctx->type20_count, ctx->DMIData);
            lazybiosExtJSONAddType20(ctx->Type20, ctx->type20_count, root);
            break;
        case 21:
            ctx->Type21 = lazybiosGetType21(ctx->Type21, &ctx->type21_count, ctx->DMIData);
            lazybiosExtJSONAddType21(ctx->Type21, ctx->type21_count, root);
            break;
        case 22:
            ctx->Type22 = lazybiosGetType22(ctx->Type22, &ctx->type22_count, ctx->DMIData);
            lazybiosExtJSONAddType22(ctx->Type22, ctx->type22_count, root);
            break;
        case 23:
            ctx->Type23 = lazybiosGetType23(ctx->Type23, &ctx->type23_count, ctx->DMIData);
            lazybiosExtJSONAddType23(ctx->Type23, ctx->type23_count, root);
            break;
        case 24:
            ctx->Type24 = lazybiosGetType24(ctx->Type24, &ctx->type24_count, ctx->DMIData);
            lazybiosExtJSONAddType24(ctx->Type24, ctx->type24_count, root);
            break;
        case 25:
            ctx->Type25 = lazybiosGetType25(ctx->Type25, &ctx->type25_count, ctx->DMIData);
            lazybiosExtJSONAddType25(ctx->Type25, ctx->type25_count, root);
            break;
        case 26:
            ctx->Type26 = lazybiosGetType26(ctx->Type26, &ctx->type26_count, ctx->DMIData);
            lazybiosExtJSONAddType26(ctx->Type26, ctx->type26_count, root);
            break;
        case 27:
            ctx->Type27 = lazybiosGetType27(ctx->Type27, &ctx->type27_count, ctx->DMIData);
            lazybiosExtJSONAddType27(ctx->Type27, ctx->type27_count, root);
            break;
        case 28:
            ctx->Type28 = lazybiosGetType28(ctx->Type28, &ctx->type28_count, ctx->DMIData);
            lazybiosExtJSONAddType28(ctx->Type28, ctx->type28_count, root);
            break;
        case 29:
            ctx->Type29 = lazybiosGetType29(ctx->Type29, &ctx->type29_count, ctx->DMIData);
            lazybiosExtJSONAddType29(ctx->Type29, ctx->type29_count, root);
            break;
        case 30:
            ctx->Type30 = lazybiosGetType30(ctx->Type30, &ctx->type30_count, ctx->DMIData);
            lazybiosExtJSONAddType30(ctx->Type30, ctx->type30_count, root);
            break;
        case 31:
            ctx->Type31 = lazybiosGetType31(ctx->Type31, &ctx->type31_count, ctx->DMIData);
            lazybiosExtJSONAddType31(ctx->Type31, ctx->type31_count, root);
            break;
        case 32:
            ctx->Type32 = lazybiosGetType32(ctx->Type32, &ctx->type32_count, ctx->DMIData);
            lazybiosExtJSONAddType32(ctx->Type32, ctx->type32_count, root);
            break;
        case 33:
            ctx->Type33 = lazybiosGetType33(ctx->Type33, &ctx->type33_count, ctx->DMIData);
            lazybiosExtJSONAddType33(ctx->Type33, ctx->type33_count, root);
            break;
        case 34:
            ctx->Type34 = lazybiosGetType34(ctx->Type34, &ctx->type34_count, ctx->DMIData);
            lazybiosExtJSONAddType34(ctx->Type34, ctx->type34_count, root);
            break;
        case 35:
            ctx->Type35 = lazybiosGetType35(ctx->Type35, &ctx->type35_count, ctx->DMIData);
            lazybiosExtJSONAddType35(ctx->Type35, ctx->type35_count, root);
            break;
        case 36:
            ctx->Type36 = lazybiosGetType36(ctx->Type36, &ctx->type36_count, ctx->DMIData);
            lazybiosExtJSONAddType36(ctx->Type36, ctx->type36_count, root);
            break;
        case 37:
            ctx->Type37 = lazybiosGetType37(ctx->Type37, &ctx->type37_count, ctx->DMIData);
            lazybiosExtJSONAddType37(ctx->Type37, ctx->type37_count, root);
            break;
        case 38:
            ctx->Type38 = lazybiosGetType38(ctx->Type38, &ctx->type38_count, ctx->DMIData);
            lazybiosExtJSONAddType38(ctx->Type38, ctx->type38_count, root);
            break;
        case 39:
            ctx->Type39 = lazybiosGetType39(ctx->Type39, &ctx->type39_count, ctx->DMIData);
            lazybiosExtJSONAddType39(ctx->Type39, ctx->type39_count, root);
            break;
        case 40:
            ctx->Type40 = lazybiosGetType40(ctx->Type40, &ctx->type40_count, ctx->DMIData);
            lazybiosExtJSONAddType40(ctx->Type40, ctx->type40_count, root);
            break;
        case 41:
            ctx->Type41 = lazybiosGetType41(ctx->Type41, &ctx->type41_count, ctx->DMIData);
            lazybiosExtJSONAddType41(ctx->Type41, ctx->type41_count, root);
            break;
        case 42:
            ctx->Type42 = lazybiosGetType42(ctx->Type42, &ctx->type42_count, ctx->DMIData);
            lazybiosExtJSONAddType42(ctx->Type42, ctx->type42_count, root);
            break;
        case 43:
            ctx->Type43 = lazybiosGetType43(ctx->Type43, &ctx->type43_count, ctx->DMIData);
            lazybiosExtJSONAddType43(ctx->Type43, ctx->type43_count, root);
            break;
        case 44:
            ctx->Type44 = lazybiosGetType44(ctx->Type44, &ctx->type44_count, ctx->DMIData);
            lazybiosExtJSONAddType44(ctx->Type44, ctx->type44_count, root);
            break;
        case 45:
            ctx->Type45 = lazybiosGetType45(ctx->Type45, &ctx->type45_count, ctx->DMIData);
            lazybiosExtJSONAddType45(ctx->Type45, ctx->type45_count, root);
            break;
        case 46:
            ctx->Type46 = lazybiosGetType46(ctx->Type46, &ctx->type46_count, ctx->DMIData);
            lazybiosExtJSONAddType46(ctx->Type46, ctx->type46_count, root);
            break;
        default:
            printf("Invalid Type!");
            break;
    }
}

static inline void printAll(lazybiosCTX_t* ctx, cJSON* root) {
    ctx->Type0 = lazybiosGetType0(ctx->Type0, &ctx->type0_count, ctx->DMIData);
    ctx->Type1 = lazybiosGetType1(ctx->Type1, &ctx->type1_count, ctx->DMIData);
    ctx->Type2 = lazybiosGetType2(ctx->Type2, &ctx->type2_count, ctx->DMIData);
    ctx->Type3 = lazybiosGetType3(ctx->Type3, &ctx->type3_count, ctx->DMIData);
    ctx->Type4 = lazybiosGetType4(ctx->Type4, &ctx->type4_count, ctx->DMIData);
    ctx->Type5 = lazybiosGetType5(ctx->Type5, &ctx->type5_count, ctx->DMIData);
    ctx->Type6 = lazybiosGetType6(ctx->Type6, &ctx->type6_count, ctx->DMIData);
    ctx->Type7 = lazybiosGetType7(ctx->Type7, &ctx->type7_count, ctx->DMIData);
    ctx->Type8 = lazybiosGetType8(ctx->Type8, &ctx->type8_count, ctx->DMIData);
    ctx->Type9 = lazybiosGetType9(ctx->Type9, &ctx->type9_count, ctx->DMIData);
    ctx->Type10 = lazybiosGetType10(ctx->Type10, &ctx->type10_count, ctx->DMIData);
    ctx->Type11 = lazybiosGetType11(ctx->Type11, &ctx->type11_count, ctx->DMIData);
    ctx->Type12 = lazybiosGetType12(ctx->Type12, &ctx->type12_count, ctx->DMIData);
    ctx->Type13 = lazybiosGetType13(ctx->Type13, &ctx->type13_count, ctx->DMIData);
    ctx->Type14 = lazybiosGetType14(ctx->Type14, &ctx->type14_count, ctx->DMIData);
    ctx->Type15 = lazybiosGetType15(ctx->Type15, &ctx->type15_count, ctx->DMIData);
    ctx->Type16 = lazybiosGetType16(ctx->Type16, &ctx->type16_count, ctx->DMIData);
    ctx->Type17 = lazybiosGetType17(ctx->Type17, &ctx->type17_count, ctx->DMIData);
    ctx->Type18 = lazybiosGetType18(ctx->Type18, &ctx->type18_count, ctx->DMIData);
    ctx->Type19 = lazybiosGetType19(ctx->Type19, &ctx->type19_count, ctx->DMIData);
    ctx->Type20 = lazybiosGetType20(ctx->Type20, &ctx->type20_count, ctx->DMIData);
    ctx->Type21 = lazybiosGetType21(ctx->Type21, &ctx->type21_count, ctx->DMIData);
    ctx->Type22 = lazybiosGetType22(ctx->Type22, &ctx->type22_count, ctx->DMIData);
    ctx->Type23 = lazybiosGetType23(ctx->Type23, &ctx->type23_count, ctx->DMIData);
    ctx->Type24 = lazybiosGetType24(ctx->Type24, &ctx->type24_count, ctx->DMIData);
    ctx->Type25 = lazybiosGetType25(ctx->Type25, &ctx->type25_count, ctx->DMIData);
    ctx->Type26 = lazybiosGetType26(ctx->Type26, &ctx->type26_count, ctx->DMIData);
    ctx->Type27 = lazybiosGetType27(ctx->Type27, &ctx->type27_count, ctx->DMIData);
    ctx->Type28 = lazybiosGetType28(ctx->Type28, &ctx->type28_count, ctx->DMIData);
    ctx->Type29 = lazybiosGetType29(ctx->Type29, &ctx->type29_count, ctx->DMIData);
    ctx->Type30 = lazybiosGetType30(ctx->Type30, &ctx->type30_count, ctx->DMIData);
    ctx->Type31 = lazybiosGetType31(ctx->Type31, &ctx->type31_count, ctx->DMIData);
    ctx->Type32 = lazybiosGetType32(ctx->Type32, &ctx->type32_count, ctx->DMIData);
    ctx->Type33 = lazybiosGetType33(ctx->Type33, &ctx->type33_count, ctx->DMIData);
    ctx->Type34 = lazybiosGetType34(ctx->Type34, &ctx->type34_count, ctx->DMIData);
    ctx->Type35 = lazybiosGetType35(ctx->Type35, &ctx->type35_count, ctx->DMIData);
    ctx->Type36 = lazybiosGetType36(ctx->Type36, &ctx->type36_count, ctx->DMIData);
    ctx->Type37 = lazybiosGetType37(ctx->Type37, &ctx->type37_count, ctx->DMIData);
    ctx->Type38 = lazybiosGetType38(ctx->Type38, &ctx->type38_count, ctx->DMIData);
    ctx->Type39 = lazybiosGetType39(ctx->Type39, &ctx->type39_count, ctx->DMIData);
    ctx->Type40 = lazybiosGetType40(ctx->Type40, &ctx->type40_count, ctx->DMIData);
    ctx->Type41 = lazybiosGetType41(ctx->Type41, &ctx->type41_count, ctx->DMIData);
    ctx->Type42 = lazybiosGetType42(ctx->Type42, &ctx->type42_count, ctx->DMIData);
    ctx->Type43 = lazybiosGetType43(ctx->Type43, &ctx->type43_count, ctx->DMIData);
    ctx->Type44 = lazybiosGetType44(ctx->Type44, &ctx->type44_count, ctx->DMIData);
    ctx->Type45 = lazybiosGetType45(ctx->Type45, &ctx->type45_count, ctx->DMIData);
    ctx->Type46 = lazybiosGetType46(ctx->Type46, &ctx->type46_count, ctx->DMIData);

    lazybiosExtJSONAddType0(ctx->Type0, ctx->type0_count, root);
    lazybiosExtJSONAddType1(ctx->Type1, ctx->type1_count, root);
    lazybiosExtJSONAddType2(ctx->Type2, ctx->type2_count, root);
    lazybiosExtJSONAddType3(ctx->Type3, ctx->type3_count, root);
    lazybiosExtJSONAddType4(ctx->Type4, ctx->type4_count, root);
    lazybiosExtJSONAddType5(ctx->Type5, ctx->type5_count, root);
    lazybiosExtJSONAddType6(ctx->Type6, ctx->type6_count, root);
    lazybiosExtJSONAddType7(ctx->Type7, ctx->type7_count, root);
    lazybiosExtJSONAddType8(ctx->Type8, ctx->type8_count, root);
    lazybiosExtJSONAddType9(ctx->Type9, ctx->type9_count, root);
    lazybiosExtJSONAddType10(ctx->Type10, ctx->type10_count, root);
    lazybiosExtJSONAddType11(ctx->Type11, ctx->type11_count, root);
    lazybiosExtJSONAddType12(ctx->Type12, ctx->type12_count, root);
    lazybiosExtJSONAddType13(ctx->Type13, ctx->type13_count, root);
    lazybiosExtJSONAddType14(ctx->Type14, ctx->type14_count, root);
    lazybiosExtJSONAddType15(ctx->Type15, ctx->type15_count, root);
    lazybiosExtJSONAddType16(ctx->Type16, ctx->type16_count, root);
    lazybiosExtJSONAddType17(ctx->Type17, ctx->type17_count, root);
    lazybiosExtJSONAddType18(ctx->Type18, ctx->type18_count, root);
    lazybiosExtJSONAddType19(ctx->Type19, ctx->type19_count, root);
    lazybiosExtJSONAddType20(ctx->Type20, ctx->type20_count, root);
    lazybiosExtJSONAddType21(ctx->Type21, ctx->type21_count, root);
    lazybiosExtJSONAddType22(ctx->Type22, ctx->type22_count, root);
    lazybiosExtJSONAddType23(ctx->Type23, ctx->type23_count, root);
    lazybiosExtJSONAddType24(ctx->Type24, ctx->type24_count, root);
    lazybiosExtJSONAddType25(ctx->Type25, ctx->type25_count, root);
    lazybiosExtJSONAddType26(ctx->Type26, ctx->type26_count, root);
    lazybiosExtJSONAddType27(ctx->Type27, ctx->type27_count, root);
    lazybiosExtJSONAddType28(ctx->Type28, ctx->type28_count, root);
    lazybiosExtJSONAddType29(ctx->Type29, ctx->type29_count, root);
    lazybiosExtJSONAddType30(ctx->Type30, ctx->type30_count, root);
    lazybiosExtJSONAddType31(ctx->Type31, ctx->type31_count, root);
    lazybiosExtJSONAddType32(ctx->Type32, ctx->type32_count, root);
    lazybiosExtJSONAddType33(ctx->Type33, ctx->type33_count, root);
    lazybiosExtJSONAddType34(ctx->Type34, ctx->type34_count, root);
    lazybiosExtJSONAddType35(ctx->Type35, ctx->type35_count, root);
    lazybiosExtJSONAddType36(ctx->Type36, ctx->type36_count, root);
    lazybiosExtJSONAddType37(ctx->Type37, ctx->type37_count, root);
    lazybiosExtJSONAddType38(ctx->Type38, ctx->type38_count, root);
    lazybiosExtJSONAddType39(ctx->Type39, ctx->type39_count, root);
    lazybiosExtJSONAddType40(ctx->Type40, ctx->type40_count, root);
    lazybiosExtJSONAddType41(ctx->Type41, ctx->type41_count, root);
    lazybiosExtJSONAddType42(ctx->Type42, ctx->type42_count, root);
    lazybiosExtJSONAddType43(ctx->Type43, ctx->type43_count, root);
    lazybiosExtJSONAddType44(ctx->Type44, ctx->type44_count, root);
    lazybiosExtJSONAddType45(ctx->Type45, ctx->type45_count, root);
    lazybiosExtJSONAddType46(ctx->Type46, ctx->type46_count, root);
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
    cJSON *root = cJSON_CreateObject();
    if (lazybiosFile(ctx, entry_file, dmi_file) != 0) {
        lazybiosInit(ctx);
    }
    lazybiosExtJSONAddSMBIOSInfo(ctx->DMIData, ctx->backend, root);
    if (type == 3500) {
        printAll(ctx, root);
    } else if (type <= 46) {
        printType(type, ctx, root);
    }

    char *json_string = cJSON_Print(root);
    printf("%s\n", json_string);

    cJSON_Delete(root);
    lazybiosCleanup(ctx);

    return 0;
}
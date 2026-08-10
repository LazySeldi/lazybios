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
#include "lazybios.h"
#include "lazybios_json.h"

int main () {
    lazybiosCTX_t *ctx = lazybiosCTXNew();
    lazybiosInit(ctx);
    cJSON *root = cJSON_CreateObject();

    ctx->Type0 = lazybiosGetType0(ctx->Type0, &ctx->type0_count, ctx->DMIData);
    ctx->Type1 = lazybiosGetType1(ctx->Type1, &ctx->type1_count, ctx->DMIData);
    ctx->Type2 = lazybiosGetType2(ctx->Type2, &ctx->type2_count, ctx->DMIData);
    ctx->Type3 = lazybiosGetType3(ctx->Type3, &ctx->type3_count, ctx->DMIData);
    ctx->Type4 = lazybiosGetType4(ctx->Type4, &ctx->type4_count, ctx->DMIData);
    ctx->Type5 = lazybiosGetType5(ctx->Type5, &ctx->type5_count, ctx->DMIData);

    lazybiosExtJSONAddSMBIOSInfo(ctx->DMIData, ctx->backend, root);
    lazybiosExtJSONAddType0(ctx->Type0, ctx->type0_count, root);
    lazybiosExtJSONAddType1(ctx->Type1, ctx->type1_count, root);
    lazybiosExtJSONAddType2(ctx->Type2, ctx->type2_count, root);
    lazybiosExtJSONAddType3(ctx->Type3, ctx->type3_count, root);
    lazybiosExtJSONAddType4(ctx->Type4, ctx->type4_count, root);
    lazybiosExtJSONAddType5(ctx->Type5, ctx->type5_count, root);

    char *json_string = cJSON_Print(root);
    printf("%s\n", json_string);

    cJSON_Delete(root);
    lazybiosCleanup(ctx);

    return 0;
}
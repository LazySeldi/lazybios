#include <stdio.h>
#include "lazybios.h"
#include "lazybios_json.h"

int main () {
    lazybiosCTX_t *ctx = lazybiosCTXNew();
    lazybiosInit(ctx);
    cJSON *root = cJSON_CreateObject();

    ctx->Type0 = lazybiosGetType0(ctx->Type0 ,&ctx->type0_count ,ctx->DMIData);
    ctx->Type1 = lazybiosGetType1(ctx->Type1 ,&ctx->type1_count ,ctx->DMIData);
    ctx->Type2 = lazybiosGetType2(ctx->Type2 ,&ctx->type2_count ,ctx->DMIData);
    ctx->Type3 = lazybiosGetType3(ctx->Type3 ,&ctx->type3_count ,ctx->DMIData);
    ctx->Type4 = lazybiosGetType4(ctx->Type4 ,&ctx->type4_count ,ctx->DMIData);

    lazybiosExtJSONAddSMBIOSInfo(ctx->DMIData ,ctx->backend ,root);
    lazybiosExtJSONAddType0(ctx->Type0 ,ctx->type0_count ,root);
    lazybiosExtJSONAddType1(ctx->Type1 ,ctx->type1_count ,root);
    lazybiosExtJSONAddType2(ctx->Type2 ,ctx->type2_count ,root);
    lazybiosExtJSONAddType3(ctx->Type3 ,ctx->type3_count ,root);
    lazybiosExtJSONAddType4(ctx->Type4 ,ctx->type4_count ,root);


    char *json_string = cJSON_Print(root);
    printf("%s\n" ,json_string);

    cJSON_Delete(root);
    lazybiosCleanup(ctx);

    return 0;
}
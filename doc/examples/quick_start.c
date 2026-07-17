#include "lazybios.h"

//! [host-system]
int read_host_bios(void) {
	lazybiosCTX_t* ctx = lazybiosCTXNew();
	if (!ctx) return -1;

	if (lazybiosInit(ctx) != 0) {
		lazybiosCleanup(ctx);
		return -1;
	}

	ctx->Type0 = lazybiosGetType0(ctx->Type0, ctx->DMIData);
	if (!ctx->Type0) {
		lazybiosCleanup(ctx);
		return -1;
	}

	lazybiosCleanup(ctx);
	return 0;
}
//! [host-system]

//! [separate-files]
int read_dump_files(const char* entry_path, const char* dmi_path) {
	lazybiosCTX_t* ctx = lazybiosCTXNew();
	if (!ctx) return -1;

	if (lazybiosFile(ctx, entry_path, dmi_path) != 0) {
		lazybiosCleanup(ctx);
		return -1;
	}

	ctx->Type1 = lazybiosGetType1(ctx->Type1, ctx->DMIData);
	int result = ctx->Type1 ? 0 : -1;
	lazybiosCleanup(ctx);
	return result;
}
//! [separate-files]

//! [merged-file]
int read_merged_dump(const char* binary_path) {
	lazybiosCTX_t* ctx = lazybiosCTXNew();
	if (!ctx) return -1;

	int result = lazybiosSingleFile(ctx, binary_path);
	lazybiosCleanup(ctx);
	return result;
}
//! [merged-file]

//! [processor-array]
int inspect_processors(lazybiosCTX_t* ctx) {
	ctx->Type4 = lazybiosGetType4(ctx->Type4, &ctx->type4_count, ctx->DMIData);
	if (!ctx->Type4) return -1;

	for (size_t i = 0; i < ctx->type4_count; ++i) {
		const char* family = lazybiosType4ProcessorFamilyStr(
			ctx->Type4[i].processor_family);
		(void)family;
	}

	return 0;
}
//! [processor-array]

//! [explicit-cleanup]
int parse_type2_without_context_ownership(lazybiosDMI_t* dmi_data) {
	size_t count = 0;
	lazybiosType2_t* boards = lazybiosGetType2(NULL, &count, dmi_data);
	if (!boards) return -1;

	/* Use boards[0] through boards[count - 1]. */
	lazybiosFreeType2(boards, count);
	return 0;
}
//! [explicit-cleanup]

//! [type0-decode]
void decode_type0(const lazybiosType0_t* bios, char* output, size_t output_size) {
	lazybiosType0CharacteristicsStr(bios->characteristics, output, output_size);
}
//! [type0-decode]

//! [type1-wakeup]
const char* decode_type1_wakeup(const lazybiosType1_t* system) {
	if (system->wake_up_type == LAZYBIOS_NOT_FOUND_U8) return LAZYBIOS_NOT_FOUND_STR;
	return lazybiosType1WakeupTypeStr(system->wake_up_type);
}
//! [type1-wakeup]

//! [type2-flags]
void decode_type2_flags(const lazybiosType2_t* board, char* output, size_t output_size) {
	lazybiosType2FeatureflagsStr(board->feature_flags, output, output_size);
}
//! [type2-flags]

//! [type3-description]
void decode_type3(const lazybiosType3_t* chassis, char* output, size_t output_size) {
	lazybiosType3TypeStr(chassis->type, output, output_size);
}
//! [type3-description]

//! [type4-counts]
uint16_t processor_core_count(const lazybiosType4_t* processor) {
	if (processor->core_count == 0xFF) return processor->core_count_2;
	return processor->core_count;
}
//! [type4-counts]

//! [type7-capacity]
uint64_t installed_cache_kib(const lazybiosType7_t* cache) {
	if (cache->installed_size == 0xFFFF) {
		return lazybiosType7CacheU32(cache->installed_cache_size_2);
	}
	return lazybiosType7CacheU16(cache->installed_size);
}
//! [type7-capacity]

//! [type17-size]
uint32_t memory_device_size_mib(const lazybiosType17_t* memory) {
	if (memory->size == 0x7FFF) return memory->extended_size & 0x7FFFFFFF;
	if (memory->size == 0 || memory->size == 0xFFFF || (memory->size & 0x8000)) return 0;
	return memory->size;
}
//! [type17-size]

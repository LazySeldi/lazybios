#include "lazybios/lazybios.h"

//! [host-system]
int read_host_bios(void) {
	lazybiosCTX_t* ctx = lazybiosCTXNew();
	if (!ctx) return -1;

	if (lazybiosInit(ctx, NULL, NULL) != 0) {
		lazybiosCleanup(ctx);
		return -1;
	}

	ctx->Type0 = lazybiosGetType0(ctx->DMIData);
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

	if (lazybiosInit(ctx, entry_path, dmi_path) != 0) {
		lazybiosCleanup(ctx);
		return -1;
	}

	ctx->Type1 = lazybiosGetType1(ctx->DMIData);
	int result = ctx->Type1 ? 0 : -1;
	lazybiosCleanup(ctx);
	return result;
}
//! [separate-files]

//! [merged-file]
int read_merged_dump(const char* binary_path) {
	lazybiosCTX_t* ctx = lazybiosCTXNew();
	if (!ctx) return -1;

	int result = lazybiosInit(ctx, NULL, binary_path);
	lazybiosCleanup(ctx);
	return result;
}
//! [merged-file]

//! [parse-all]
int parse_everything(lazybiosCTX_t* ctx) {
	if (lazybiosParseAll(ctx) != 0) return -1;

	/* Every implemented type is now populated, or NULL if that parse failed. */
	if (ctx->Type17) {
		for (size_t i = 0; i < ctx->Type17->count; ++i) {
			const char* locator = ctx->Type17->entries[i].device_locator;
			(void)locator;
		}
	}

	return 0;
}
//! [parse-all]

//! [processor-array]
int inspect_processors(lazybiosCTX_t* ctx) {
	ctx->Type4 = lazybiosGetType4(ctx->DMIData);
	if (!ctx->Type4) return -1;

	for (size_t i = 0; i < ctx->Type4->count; ++i) {
		const char* family = ctx->Type4->entries[i].decoded.processor_family;
		(void)family;
	}

	return 0;
}
//! [processor-array]

//! [explicit-cleanup]
int parse_type2_without_context_ownership(lazybiosDMI_t* dmi_data) {
	lazybiosType2Array_t* boards = lazybiosGetType2(dmi_data);
	if (!boards) return -1;

	/* Use boards->entries[0] through boards->entries[boards->count - 1]. */
	lazybiosFreeType2(boards);
	return 0;
}
//! [explicit-cleanup]

//! [type0-decode]
const char* decode_type0(const lazybiosType0_t* bios) {
	if (LAZYBIOS_FIELD_STATUS(bios, characteristics) != LAZYBIOS_FIELD_PRESENT) return "Not Present";
	return bios->decoded.characteristics;
}
//! [type0-decode]

//! [type1-wakeup]
const char* decode_type1_wakeup(const lazybiosType1_t* system) {
	if (LAZYBIOS_FIELD_STATUS(system, wake_up_type) != LAZYBIOS_FIELD_PRESENT) return "Not Present";
	return system->decoded.wake_up_type;
}
//! [type1-wakeup]

//! [type2-flags]
const char* decode_type2_flags(const lazybiosType2_t* board) {
	if (LAZYBIOS_FIELD_STATUS(board, feature_flags) != LAZYBIOS_FIELD_PRESENT) return "Not Present";
	return board->decoded.feature_flags;
}
//! [type2-flags]

//! [type3-description]
const char* decode_type3(const lazybiosType3_t* chassis) {
	if (LAZYBIOS_FIELD_STATUS(chassis, type) != LAZYBIOS_FIELD_PRESENT) return "Not Present";
	return chassis->decoded.type;
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
		return cache->decoded.installed_cache_size_2;
	}
	return cache->decoded.installed_size;
}
//! [type7-capacity]

//! [type17-size]
uint32_t memory_device_size_mib(const lazybiosType17_t* memory) {
	if (memory->size == 0x7FFF) return memory->extended_size & 0x7FFFFFFF;
	if (memory->size == 0 || memory->size == 0xFFFF || (memory->size & 0x8000)) return 0;
	return memory->size;
}
//! [type17-size]

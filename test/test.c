//
// test.c - File for testing ALL SMBIOS types we have implemented currently
//
#include "lazybios.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void printType0(lazybiosCTX_t* ctx) {
	printf("=== BIOS INFORMATION ===\n");

	if (!ctx->Type0) ctx->Type0 = lazybiosGetType0(ctx->Type0, ctx->DMIData);

	if (ctx->Type0) {
		printf("Vendor: %s\n", ctx->Type0->vendor);
		printf("Version: %s\n", ctx->Type0->version);
		printf("Release Date: %s\n", ctx->Type0->release_date);

		if (ctx->Type0->bios_starting_segment == LAZYBIOS_NOT_FOUND_U16) {
			printf("BIOS Starting Segment: Not Present\n");
		} else {
			printf("BIOS Starting Segment: 0x%04hX\n", ctx->Type0->bios_starting_segment);
		}

		if (ctx->Type0->characteristics == LAZYBIOS_NOT_FOUND_U64) {
			printf("Firmware Characteristics: Not Present\n");
		} else {
			char buf[(LAZYBIOS_DECODER_BUF_SIZE * 2)]; // Needs a bigger size
			lazybiosType0CharacteristicsStr(ctx->Type0->characteristics, buf, sizeof(buf));
			printf("Firmware Characteristics: %s\n", buf);
		}

		if (ISVERPLUS(ctx->DMIData, 2, 4)) {
			if (ctx->Type0->firmware_char_ext_bytes_count >= 1) {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];
				lazybiosType0CharacteristicsExtByte1Str(ctx->Type0->firmware_char_ext_bytes[0], buf, sizeof(buf));
				printf("Firmware Characteristics Extension Bytes 1: %s\n", buf);
			} else {
				printf("Firmware Characteristics Extension Bytes 1: Not Present\n");
			}

			if (ctx->Type0->firmware_char_ext_bytes_count >= 2) {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];
				lazybiosType0CharacteristicsExtByte2Str(ctx->Type0->firmware_char_ext_bytes[1], buf, sizeof(buf));
				printf("Firmware Characteristics Extension Bytes 2: %s\n", buf);
			} else {
				printf("Firmware Characteristics Extension Bytes 2: Not Present\n");
			}

			if (ctx->Type0->platform_major_release == LAZYBIOS_NOT_FOUND_U8) {
				printf("Platform Major Release: Not Present\n");
			} else {
				printf("Platform Major Release: %hhu\n", ctx->Type0->platform_major_release);
			}

			if (ctx->Type0->platform_minor_release == LAZYBIOS_NOT_FOUND_U8) {
				printf("Platform Minor Release: Not Present\n");
			} else {
				printf("Platform Minor Release: %hhu\n", ctx->Type0->platform_minor_release);
			}

			if (ctx->Type0->ec_major_release == LAZYBIOS_NOT_FOUND_U8) {
				printf("EC Major Release: Not Present\n");
			} else {
				printf("EC Major Release: %hhu\n", ctx->Type0->ec_major_release);
			}

			if (ctx->Type0->ec_minor_release == LAZYBIOS_NOT_FOUND_U8) {
				printf("EC Minor Release: Not Present\n");
			} else {
				printf("EC Minor Release: %hhu\n", ctx->Type0->ec_minor_release);
			}
		} else {
			printf("Firmware Characteristics Extension Bytes 1: [SMBIOS 2.4 required]\n");
			printf("Firmware Characteristics Extension Bytes 2: [SMBIOS 2.4 required]\n");
			printf("Platform Major Release: [SMBIOS 2.4 required]\n");
			printf("Platform Minor Release: [SMBIOS 2.4 required]\n");
			printf("EC Major Release: [SMBIOS 2.4 required]\n");
			printf("EC Minor Release: [SMBIOS 2.4 required]\n");
		}

		if (ISVERPLUS(ctx->DMIData, 3, 1)) {
			if (ctx->Type0->extended_rom_size == LAZYBIOS_NOT_FOUND_U16) {
				printf("Extended ROM Size: Not Present\n\n");
			} else {
				printf("Extended ROM Size: %hu%s\n\n", lazybiosType0ExtendedROMSizeU16(ctx->Type0->extended_rom_size, ctx->Type0->unit), ctx->Type0->unit);
			}
		} else {
			if (ctx->Type0->rom_size == LAZYBIOS_NOT_FOUND_U32) {
				printf("ROM Size: Not Present\n\n");
			} else {
				printf("ROM Size: %u KB\n\n", ctx->Type0->rom_size);
			}
		}
	} else {
		printf("Failed to get BIOS information\n\n");
	}
}

static void printType1(lazybiosCTX_t* ctx) {
	printf("=== SYSTEM INFORMATION ===\n");

	if (!ctx->Type1) ctx->Type1 = lazybiosGetType1(ctx->Type1, ctx->DMIData);

	if (ctx->Type1) {
		printf("Manufacturer: %s\n", ctx->Type1->manufacturer);
		printf("Product name: %s\n", ctx->Type1->product_name);
		printf("Version: %s\n", ctx->Type1->version);
		printf("Serial number: %s\n", ctx->Type1->serial_number);

		if (ISVERPLUS(ctx->DMIData, 2, 1)) {
			// UUID is SMBIOS 2.1+
			int uuid_found = 0;
			for (int i = 0; i < 16; i++) {
				if (ctx->Type1->uuid[i] != LAZYBIOS_NOT_FOUND_U8) {
					uuid_found = 1;
					break;
				}
			}

			if (uuid_found) {
				printf("UUID: %02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
					   ctx->Type1->uuid[3], ctx->Type1->uuid[2], ctx->Type1->uuid[1], ctx->Type1->uuid[0],
					   ctx->Type1->uuid[5], ctx->Type1->uuid[4],
					   ctx->Type1->uuid[7], ctx->Type1->uuid[6],
					   ctx->Type1->uuid[8], ctx->Type1->uuid[9], ctx->Type1->uuid[10], ctx->Type1->uuid[11],
					   ctx->Type1->uuid[12], ctx->Type1->uuid[13], ctx->Type1->uuid[14], ctx->Type1->uuid[15]);
			} else {
				printf("UUID: Not Present\n");
			}

			if (ctx->Type1->wake_up_type == LAZYBIOS_NOT_FOUND_U8) {
				printf("Wake up type: Not Present\n");
			} else {
				printf("Wake up type: %s\n", lazybiosType1WakeupTypeStr(ctx->Type1->wake_up_type));
			}
		} else {
			printf("UUID: [SMBIOS 2.1 required]\n");
			printf("Wake up type: [SMBIOS 2.1 required]\n");
		}

		if (ISVERPLUS(ctx->DMIData, 2, 4)) {
			// SKU and Family are SMBIOS 2.4+
			printf("SKU number: %s\n", ctx->Type1->sku_number);
			printf("Family: %s\n\n", ctx->Type1->family);
		} else {
			printf("SKU number: [SMBIOS 2.4 required]\n");
			printf("Family: [SMBIOS 2.4 required]\n\n");
		}
	} else {
		printf("Failed to get System information\n\n");
	}
}

static void printType2(lazybiosCTX_t* ctx) {
	printf("=== BASEBOARD INFORMATION ===\n");

	if (!ctx->Type2) ctx->Type2 = lazybiosGetType2(ctx->Type2, &ctx->type2_count, ctx->DMIData);

	if (ctx->Type2 && ctx->type2_count > 0) {
		for (size_t i = 0; i < ctx->type2_count; i++) {
			lazybiosType2_t* type2 = &ctx->Type2[i];

			if (ctx->type2_count > 1) {
				printf("--- Baseboard %zu ---\n", i + 1);
			}

			printf("Manufacturer: %s\n", type2->manufacturer);
			printf("Product: %s\n", type2->product);
			printf("Version: %s\n", type2->version);
			printf("Serial number: %s\n", type2->serial_number);
			printf("Asset tag: %s\n", type2->asset_tag);

			if (ISVERPLUS(ctx->DMIData, 2, 1)) {
				if (type2->feature_flags == LAZYBIOS_NOT_FOUND_U8) {
					printf("Feature flags: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType2FeatureflagsStr(type2->feature_flags, buf, sizeof(buf));
					printf("Feature flags: %s\n", buf);
				}

				printf("Location in Chassis: %s\n", type2->location_in_chassis);

				if (type2->chassis_handle == LAZYBIOS_NOT_FOUND_U16) {
					printf("Chassis Handle: Not Present\n");
				} else {
					printf("Chassis Handle: 0x%04hX\n", type2->chassis_handle);
				}

				if (type2->board_type == LAZYBIOS_NOT_FOUND_U8) {
					printf("Board Type: Not Present\n");
				} else {
					printf("Board Type: %s\n", lazybiosType2BoardTypeStr(type2->board_type));
				}

				if (type2->number_of_contained_object_handles == LAZYBIOS_NOT_FOUND_U8 ||
					type2->number_of_contained_object_handles == 0) {
					// No contained object handles or field not present here
				} else {
					for (int j = 0; j < type2->number_of_contained_object_handles; j++) {
						printf("  Contained Object Handle %d: 0x%04hX\n",
							   j, type2->contained_object_handles[j]);
					}
				}
			} else {
				printf("Feature flags: [SMBIOS 2.1 required]\n");
				printf("Location in Chassis: [SMBIOS 2.1 required]\n");
				printf("Chassis Handle: [SMBIOS 2.1 required]\n");
				printf("Board Type: [SMBIOS 2.1 required]\n");
				printf("Contained Object Handles: [SMBIOS 2.1 required]\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Baseboard information\n\n");
	}
}

static void printType3(lazybiosCTX_t* ctx) {
	printf("=== CHASSIS INFORMATION ===\n");

	if (!ctx->Type3) ctx->Type3 = lazybiosGetType3(ctx->Type3, &ctx->type3_count, ctx->DMIData);

	if (ctx->Type3 && ctx->type3_count > 0) {
		for (size_t i = 0; i < ctx->type3_count; i++) {
			lazybiosType3_t* type3 = &ctx->Type3[i];

			if (ctx->type3_count > 1) {
				printf("--- Chassis %zu ---\n", i + 1);
			}

			printf("Manufacturer: %s\n", type3->manufacturer);
			if (type3->type == LAZYBIOS_NOT_FOUND_U8) {
				printf("Type: Not Present\n");
			} else {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];
				lazybiosType3TypeStr(type3->type, buf, sizeof(buf));
				printf("Type: %s\n", buf);
			}
			printf("Version: %s\n", type3->version);
			printf("Serial Number: %s\n", type3->serial_number);
			printf("Asset Tag: %s\n", type3->asset_tag);

			if (ISVERPLUS(ctx->DMIData, 2, 1)) {
				if (type3->boot_up_state == LAZYBIOS_NOT_FOUND_U8) {
					printf("BootUp State: Not Present\n");
				} else {
					printf("BootUp State: %s\n", lazybiosType3StateStr(type3->boot_up_state));
				}

				if (type3->power_supply_state == LAZYBIOS_NOT_FOUND_U8) {
					printf("Power Supply State: Not Present\n");
				} else {
					printf("Power Supply State: %s\n", lazybiosType3StateStr(type3->power_supply_state));
				}

				if (type3->thermal_state == LAZYBIOS_NOT_FOUND_U8) {
					printf("Thermal State: Not Present\n");
				} else {
					printf("Thermal State: %s\n", lazybiosType3StateStr(type3->thermal_state));
				}

				if (type3->security_status == LAZYBIOS_NOT_FOUND_U8) {
					printf("Security Status: Not Present\n");
				} else {
					printf("Security Status: %s\n", lazybiosType3SecurityStatusStr(type3->security_status));
				}
			} else {
				printf("BootUp State: [SMBIOS 2.1 required]\n");
				printf("Power Supply State: [SMBIOS 2.1 required]\n");
				printf("Thermal State: [SMBIOS 2.1 required]\n");
				printf("Security Status: [SMBIOS 2.1 required]\n");
			}

			if (ISVERPLUS(ctx->DMIData, 2, 3)) {
				if (type3->oem_defined == LAZYBIOS_NOT_FOUND_U32) {
					printf("OEM Defined: Not Present\n");
				} else {
					printf("OEM-defined DWORD: 0x%08X\n", type3->oem_defined);
				}

				if (type3->height == LAZYBIOS_NOT_FOUND_U8) {
					printf("Height: Not Present\n");
				} else if (type3->height == 0x00) {
					printf("Height: Unspecified\n");
				} else {
					printf("Height: %hhuU\n", type3->height);
				}

				if (type3->number_of_power_cords == LAZYBIOS_NOT_FOUND_U8) {
					printf("Number of Power Cords: Not Present\n");
				} else {
					printf("Number of Power Cords: %hhu\n", type3->number_of_power_cords);
				}

				if (type3->contained_element_count == LAZYBIOS_NOT_FOUND_U8) {
					printf("Contained Elements: Not Present\n");
				} else if (type3->contained_element_count == 0) {
					printf("Contained Elements: None\n");
				} else if (!type3->contained_elements) {
					printf("Contained Elements: Not Present\n");
				} else {
					printf("Contained Elements (%hhu):\n", type3->contained_element_count);

					for (uint8_t element_index = 0; element_index < type3->contained_element_count; element_index++) {
						uint8_t* record = type3->contained_elements +
							element_index * type3->contained_element_record_length;

						uint8_t type_byte = record[0];
						uint8_t min_count = record[1];
						uint8_t max_count = record[2];

						char type_str[LAZYBIOS_DECODER_BUF_SIZE];
						lazybiosType3ContainedElementTypeStr(type_byte, type_str, sizeof(type_str));

						printf("  Element %hhu: Type = %s, Min = %hhu, Max = %hhu\n",
							   element_index + 1, type_str, min_count, max_count);

						for (uint8_t j = 3; j < type3->contained_element_record_length; j++) {
							printf("    Extra byte %hhu: 0x%02hhX\n", j, record[j]);
						}
					}
				}

				if (type3->contained_element_record_length == LAZYBIOS_NOT_FOUND_U8) {
				} else if (type3->contained_element_record_length > 0) {
					printf("Contained Element Record Length: %hhu bytes\n", type3->contained_element_record_length);
				}

				if (ISVERPLUS(ctx->DMIData, 2, 7)) {
					printf("SKU Number: %s\n", type3->sku_number);
				} else {
					printf("SKU Number: [SMBIOS 2.7 required]\n");
				}

				if (ISVERPLUS(ctx->DMIData, 3, 9)) {
					if (type3->rack_type == LAZYBIOS_NOT_FOUND_U8) {
						printf("Rack Type: Not Present\n");
					} else {
						printf("Rack Type: 0x%02hhX\n", type3->rack_type);
					}

					if (type3->rack_height == LAZYBIOS_NOT_FOUND_U8) {
						printf("Rack Height (extended): Not Present\n");
					} else {
						printf("Rack Height (extended): %hhuU\n", type3->rack_height);
					}
				} else {
					printf("Rack Type: [SMBIOS 3.9 required]\n");
					printf("Rack Height (extended): [SMBIOS 3.9 required]\n");
				}
			} else {
				printf("OEM Defined: [SMBIOS 2.3 required]\n");
				printf("Height: [SMBIOS 2.3 required]\n");
				printf("Number of Power Cords: [SMBIOS 2.3 required]\n");
				printf("Contained Elements: [SMBIOS 2.3 required]\n");
				printf("SKU Number: [SMBIOS 2.7 required]\n");
				printf("Rack Type: [SMBIOS 3.9 required]\n");
				printf("Rack Height (extended): [SMBIOS 3.9 required]\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Chassis Information!\n\n");
	}
}

static void printType4(lazybiosCTX_t* ctx) {
	printf("=== PROCESSOR INFORMATION ===\n");

	if (!ctx->Type4) ctx->Type4 = lazybiosGetType4(ctx->Type4, &ctx->type4_count, ctx->DMIData);

	if (ctx->Type4 && ctx->type4_count > 0) {
		for (size_t i = 0; i < ctx->type4_count; i++) {
			lazybiosType4_t* type4 = &ctx->Type4[i];

			if (ctx->type4_count > 1) {
				printf("--- Processor %zu ---\n", i + 1);
			}

			printf("Socket Designation: %s\n", type4->socket_designation);

			if (type4->processor_type == LAZYBIOS_NOT_FOUND_U8) {
				printf("Processor Type: Not Present\n");
			} else {
				printf("Processor Type: %s\n", lazybiosType4TypeStr(type4->processor_type));
			}

			if (type4->processor_family == LAZYBIOS_NOT_FOUND_U8) {
				printf("Processor Family: Not Present\n");
			} else {
				printf("Processor Family: %s\n", lazybiosType4ProcessorFamilyStr(type4->processor_family));
			}

			printf("Processor Manufacturer: %s\n", type4->processor_manufacturer);

			if (type4->processor_id == LAZYBIOS_NOT_FOUND_U64) {
				printf("Processor ID: Not Present\n");
			} else {
				printf("Processor ID: %02X %02X %02X %02X %02X %02X %02X %02X\n",
					   (unsigned int)(type4->processor_id & 0xFF),
					   (unsigned int)((type4->processor_id >> 8) & 0xFF),
					   (unsigned int)((type4->processor_id >> 16) & 0xFF),
					   (unsigned int)((type4->processor_id >> 24) & 0xFF),
					   (unsigned int)((type4->processor_id >> 32) & 0xFF),
					   (unsigned int)((type4->processor_id >> 40) & 0xFF),
					   (unsigned int)((type4->processor_id >> 48) & 0xFF),
					   (unsigned int)((type4->processor_id >> 56) & 0xFF));
			}

			printf("Processor Version: %s\n", type4->processor_version);

			if (type4->voltage == LAZYBIOS_NOT_FOUND_U8) {
				printf("Voltage: Not Present\n");
			} else {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];
				lazybiosType4VoltageStr(type4->voltage, buf, sizeof(buf));
				printf("Voltage: %s\n", buf);
			}

			if (type4->external_clock == LAZYBIOS_NOT_FOUND_U16) {
				printf("External Clock: Not Present\n");
			} else {
				printf("External Clock: %u MHz\n", type4->external_clock);
			}

			if (type4->max_speed == LAZYBIOS_NOT_FOUND_U16) {
				printf("Max Speed: Not Present\n");
			} else {
				printf("Max Speed: %u MHz\n", type4->max_speed);
			}

			if (type4->current_speed == LAZYBIOS_NOT_FOUND_U16) {
				printf("Current Speed: Not Present\n");
			} else {
				printf("Current Speed: %u MHz\n", type4->current_speed);
			}

			if (type4->status == LAZYBIOS_NOT_FOUND_U8) {
				printf("Status: Not Present\n");
			} else {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];
				lazybiosType4StatusStr(type4->status, buf, sizeof(buf));
				printf("Status: %s\n", buf);
			}

			if (type4->processor_upgrade == LAZYBIOS_NOT_FOUND_U8) {
				printf("Processor Upgrade: Not Present\n");
			} else {
				printf("Processor Upgrade: %s\n", lazybiosType4SocketTypeStr(type4->processor_upgrade));
			}

			if (ISVERPLUS(ctx->DMIData, 2, 1)) {
				if (type4->l1_cache_handle == LAZYBIOS_NOT_FOUND_U16) {
					printf("L1 Cache Handle: Not Present\n");
				} else {
					printf("L1 Cache Handle: 0x%04hX\n", type4->l1_cache_handle);
				}

				if (type4->l2_cache_handle == LAZYBIOS_NOT_FOUND_U16) {
					printf("L2 Cache Handle: Not Present\n");
				} else {
					printf("L2 Cache Handle: 0x%04hX\n", type4->l2_cache_handle);
				}

				if (type4->l3_cache_handle == LAZYBIOS_NOT_FOUND_U16) {
					printf("L3 Cache Handle: Not Present\n");
				} else {
					printf("L3 Cache Handle: 0x%04hX\n", type4->l3_cache_handle);
				}
			} else {
				printf("L1 Cache Handle: [SMBIOS 2.1 required]\n");
				printf("L2 Cache Handle: [SMBIOS 2.1 required]\n");
				printf("L3 Cache Handle: [SMBIOS 2.1 required]\n");
			}

			if (ISVERPLUS(ctx->DMIData, 2, 3)) {
				printf("Serial Number: %s\n", type4->serial_number);
				printf("Asset Tag: %s\n", type4->asset_tag);
				printf("Part Number: %s\n", type4->part_number);
			} else {
				printf("Serial Number: [SMBIOS 2.3 required]\n");
				printf("Asset Tag: [SMBIOS 2.3 required]\n");
				printf("Part Number: [SMBIOS 2.3 required]\n");
			}

			if (ISVERPLUS(ctx->DMIData, 2, 5)) {
				uint32_t actual_core_count = LAZYBIOS_NOT_FOUND_U32;
				if (type4->core_count == 0xFF && type4->core_count_2 != LAZYBIOS_NOT_FOUND_U16) {
					actual_core_count = type4->core_count_2;
				} else if (type4->core_count != LAZYBIOS_NOT_FOUND_U8 && type4->core_count != 0xFF) {
					actual_core_count = type4->core_count;
				}

				if (actual_core_count == LAZYBIOS_NOT_FOUND_U32) {
					printf("Core Count: Not Present\n");
				} else {
					printf("Core Count: %u\n", actual_core_count);
				}

				uint32_t actual_core_enabled = LAZYBIOS_NOT_FOUND_U32;
				if (type4->core_enabled == 0xFF && type4->core_enabled_2 != LAZYBIOS_NOT_FOUND_U16) {
					actual_core_enabled = type4->core_enabled_2;
				} else if (type4->core_enabled != LAZYBIOS_NOT_FOUND_U8 && type4->core_enabled != 0xFF) {
					actual_core_enabled = type4->core_enabled;
				}

				if (actual_core_enabled == LAZYBIOS_NOT_FOUND_U32) {
					printf("Core Enabled: Not Present\n");
				} else {
					printf("Core Enabled: %u\n", actual_core_enabled);
				}

				uint32_t actual_thread_count = LAZYBIOS_NOT_FOUND_U32;
				if (type4->thread_count == 0xFF && type4->thread_count_2 != LAZYBIOS_NOT_FOUND_U16) {
					actual_thread_count = type4->thread_count_2;
				} else if (type4->thread_count != LAZYBIOS_NOT_FOUND_U8 && type4->thread_count != 0xFF) {
					actual_thread_count = type4->thread_count;
				}

				if (actual_thread_count == LAZYBIOS_NOT_FOUND_U32) {
					printf("Thread Count: Not Present\n");
				} else {
					printf("Thread Count: %u\n", actual_thread_count);
				}

				if (type4->processor_characteristics == LAZYBIOS_NOT_FOUND_U16) {
					printf("Processor Characteristics: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType4CharacteristicsStr(type4->processor_characteristics, buf, sizeof(buf));
					printf("Processor Characteristics: %s\n", buf);
				}

				if (type4->processor_family_2 == LAZYBIOS_NOT_FOUND_U16) {
					printf("Processor Family 2: Not Present\n");
				} else {
					printf("Processor Family 2: %s\n", lazybiosType4ProcessorFamilyStr(type4->processor_family_2));
				}
			} else {
				printf("Core Count: [SMBIOS 2.5 required]\n");
				printf("Core Enabled: [SMBIOS 2.5 required]\n");
				printf("Thread Count: [SMBIOS 2.5 required]\n");
				printf("Processor Characteristics: [SMBIOS 2.5 required]\n");
				printf("Processor Family 2: [SMBIOS 2.5 required]\n");
			}

			if (ISVERPLUS(ctx->DMIData, 3, 6)) {
				if (type4->thread_enabled == LAZYBIOS_NOT_FOUND_U16) {
					printf("Thread Enabled: Not Present\n");
				} else {
					printf("Thread Enabled: %u\n", type4->thread_enabled);
				}
			} else {
				printf("Thread Enabled: [SMBIOS 3.6 required]\n");
			}

			if (ISVERPLUS(ctx->DMIData, 3, 8)) {
				printf("Socket Type: %s\n", type4->socket_type);
			} else {
				printf("Socket Type: [SMBIOS 3.8 required]\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Processor information!\n\n");
	}
}

static void printType17(lazybiosCTX_t* ctx) {
	printf("=== MEMORY DEVICE ===\n");

	if (!ctx->Type17) ctx->Type17 = lazybiosGetType17(ctx->Type17, &ctx->type17_count, ctx->DMIData);

	if (ctx->Type17 && ctx->type17_count > 0) {
		for (size_t i = 0; i < ctx->type17_count; i++) {
			lazybiosType17_t* type17 = &ctx->Type17[i];

			if (ctx->type17_count > 1) {
				printf("--- Memory Device %zu ---\n", i + 1);
			}

			if (ISVERPLUS(ctx->DMIData, 2, 1)) {
				if (type17->physical_memory_array_handle == LAZYBIOS_NOT_FOUND_U16) {
					printf("Physical Memory Array Handle: Not Present\n");
				} else {
					printf("Physical Memory Array Handle: 0x%04hX\n", type17->physical_memory_array_handle);
				}

				if (type17->memory_error_information_handle == LAZYBIOS_NOT_FOUND_U16) {
					printf("Memory Error Information Handle: Not Present\n");
				} else {
					printf("Memory Error Information Handle: 0x%04hX\n", type17->memory_error_information_handle);
				}

				if (type17->total_width == LAZYBIOS_NOT_FOUND_U16) {
					printf("Total Width: Not Present\n");
				} else {
					printf("Total Width: %u bits\n", type17->total_width == 0xFFFF ? 0 : type17->total_width);
				}

				if (type17->data_width == LAZYBIOS_NOT_FOUND_U16) {
					printf("Data Width: Not Present\n");
				} else {
					printf("Data Width: %u bits\n", type17->data_width == 0xFFFF ? 0 : type17->data_width);
				}

				if (type17->size == LAZYBIOS_NOT_FOUND_U16) {
					printf("Size: Not Present\n");
				} else {
					// Size: 0 = no device, 0xFFFF = unknown, Bit 15 = KB, else MB
					if (type17->size == 0) printf("Size: No Module Installed\n");
					else if (type17->size == 0xFFFF) printf("Size: Unknown\n");
					else printf("Size: %u %s\n", (type17->size & 0x7FFF), (type17->size & 0x8000) ? "KB" : "MB");
				}

				if (type17->form_factor == LAZYBIOS_NOT_FOUND_U8) {
					printf("Form Factor: Not Present\n");
				} else {
					printf("Form Factor: %s\n", lazybiosType17FormFactorStr(type17->form_factor));
				}

				if (type17->device_set == LAZYBIOS_NOT_FOUND_U8) {
					printf("Device Set: Not Present\n");
				} else {
					printf("Device Set: %u\n", type17->device_set);
				}

				printf("Device Locator: %s\n", type17->device_locator);
				printf("Bank Locator: %s\n", type17->bank_locator);

				if (type17->memory_type == LAZYBIOS_NOT_FOUND_U8) {
					printf("Memory Type: Not Present\n");
				} else {
					printf("Memory Type: %s\n", lazybiosType17TypeStr(type17->memory_type));
				}

				if (type17->type_detail == LAZYBIOS_NOT_FOUND_U16) {
					printf("Type Detail: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType17TypeDetailStr(type17->type_detail, buf, sizeof(buf));
					printf("Type Detail: %s\n", buf);
				}
			} else {
				printf("[SMBIOS 2.1 required for Memory Device details]\n");
			}

			if (ISVERPLUS(ctx->DMIData, 2, 3)) {
				if (type17->speed == LAZYBIOS_NOT_FOUND_U16) {
					printf("Speed: Not Present\n");
				} else {
					printf("Speed: %u MT/s\n", type17->speed == 0xFFFF ? 0 : type17->speed);
				}

				printf("Manufacturer: %s\n", type17->manufacturer);
				printf("Serial Number: %s\n", type17->serial_number);
				printf("Asset Tag: %s\n", type17->asset_tag);
				printf("Part Number: %s\n", type17->part_number);

			} else {
				printf("Speed: [SMBIOS 2.3 required]\n");
				printf("Manufacturer: [SMBIOS 2.3 required]\n");
				printf("Serial Number: [SMBIOS 2.3 required]\n");
				printf("Asset Tag: [SMBIOS 2.3 required]\n");
				printf("Part Number: [SMBIOS 2.3 required]\n");
			}

			if (ISVERPLUS(ctx->DMIData, 2, 6)) {
				if (type17->attributes == LAZYBIOS_NOT_FOUND_U8) {
					printf("Attributes: Not Present\n");
				} else {
					printf("Attributes: %u rank(s)\n", type17->attributes & 0x7F);
				}
			} else {
				printf("Attributes: [SMBIOS 2.6 required]\n");
			}

			if (ISVERPLUS(ctx->DMIData, 2, 7)) {
				if (type17->extended_size == LAZYBIOS_NOT_FOUND_U32) {
					printf("Extended Size: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType17ExtendedSizeStr(type17->extended_size, buf, sizeof(buf));
					printf("Extended Size: %s\n", buf);
				}

				if (type17->configured_memory_speed == LAZYBIOS_NOT_FOUND_U16) {
					printf("Configured Memory Speed: Not Present\n");
				} else {
					printf("Configured Memory Speed: %u MT/s\n", type17->configured_memory_speed == 0xFFFF ? 0 : type17->configured_memory_speed);
				}
			} else {
				printf("Extended Size: [SMBIOS 2.7 required]\n");
				printf("Configured Memory Speed: [SMBIOS 2.7 required]\n");
			}

			if (ISVERPLUS(ctx->DMIData, 2, 8)) {
				if (type17->minimum_voltage == LAZYBIOS_NOT_FOUND_U16) {
					printf("Minimum Voltage: Not Present\n");
				} else {
					printf("Minimum Voltage: %u mV\n", type17->minimum_voltage);
				}

				if (type17->maximum_voltage == LAZYBIOS_NOT_FOUND_U16) {
					printf("Maximum Voltage: Not Present\n");
				} else {
					printf("Maximum Voltage: %u mV\n", type17->maximum_voltage);
				}

				if (type17->configured_voltage == LAZYBIOS_NOT_FOUND_U16) {
					printf("Configured Voltage: Not Present\n");
				} else {
					printf("Configured Voltage: %u mV\n", type17->configured_voltage);
				}
			} else {
				printf("Minimum Voltage: [SMBIOS 2.8 required]\n");
				printf("Maximum Voltage: [SMBIOS 2.8 required]\n");
				printf("Configured Voltage: [SMBIOS 2.8 required]\n");
			}

			if (ISVERPLUS(ctx->DMIData, 3, 2)) {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];

				if (type17->memory_technology == LAZYBIOS_NOT_FOUND_U8) {
					printf("Memory Technology: Not Present\n");
				} else {
					printf("Memory Technology: %s\n", lazybiosType17MemoryTechnologyStr(type17->memory_technology));
				}

				if (type17->memory_operating_mode_capability == LAZYBIOS_NOT_FOUND_U16) {
					printf("Memory Operating Mode Capability: Not Present\n");
				} else {
					lazybiosType17OperatingModeCapabilityStr(type17->memory_operating_mode_capability, buf, sizeof(buf));
					printf("Memory Operating Mode Capability: %s\n", buf);
				}

				printf("Firmware Version: %s\n", type17->firmware_version);

				if (type17->module_manufacturer_id == LAZYBIOS_NOT_FOUND_U16) {
					printf("Module Manufacturer ID: Not Present\n");
				} else {
					lazybiosType17ModuleManufacturerIDStr(type17->module_manufacturer_id, buf, sizeof(buf));
					printf("Module Manufacturer ID: %s\n", buf);
				}

				if (type17->module_product_id == LAZYBIOS_NOT_FOUND_U16) {
					printf("Module Product ID: Not Present\n");
				} else {
					lazybiosType17ModuleManufacturerIDStr(type17->module_product_id, buf, sizeof(buf));
					printf("Module Product ID: %s\n", buf);
				}

				if (type17->memory_subsystem_controller_manufacturer_id == LAZYBIOS_NOT_FOUND_U16) {
					printf("Memory Subsystem Controller Manufacturer ID: Not Present\n");
				} else {
					lazybiosType17ModuleManufacturerIDStr(type17->memory_subsystem_controller_manufacturer_id, buf, sizeof(buf));
					printf("Memory Subsystem Controller Manufacturer ID: %s\n", buf);
				}

				if (type17->memory_subsystem_controller_product_id == LAZYBIOS_NOT_FOUND_U16) {
					printf("Memory Subsystem Controller Product ID: Not Present\n");
				} else {
					lazybiosType17ModuleManufacturerIDStr(type17->memory_subsystem_controller_product_id, buf, sizeof(buf));
					printf("Memory Subsystem Controller Product ID: %s\n", buf);
				}

				if (type17->non_volatile_size == LAZYBIOS_NOT_FOUND_U64) {
					printf("Non-volatile Size: Not Present\n");
				} else {
					lazybiosType17NonVolatileSizeStr(type17->non_volatile_size, buf, sizeof(buf));
					printf("Non-volatile Size: %s\n", buf);
				}

				if (type17->volatile_size == LAZYBIOS_NOT_FOUND_U64) {
					printf("Volatile Size: Not Present\n");
				} else {
					lazybiosType17VolatileSizeStr(type17->volatile_size, buf, sizeof(buf));
					printf("Volatile Size: %s\n", buf);
				}

				if (type17->cache_size == LAZYBIOS_NOT_FOUND_U64) {
					printf("Cache Size: Not Present\n");
				} else {
					lazybiosType17CacheSizeStr(type17->cache_size, buf, sizeof(buf));
					printf("Cache Size: %s\n", buf);
				}

				if (type17->logical_size == LAZYBIOS_NOT_FOUND_U64) {
					printf("Logical Size: Not Present\n");
				} else {
					printf("Logical Size: %llu bytes\n", (unsigned long long)type17->logical_size);
				}
			} else {
				printf("Memory Technology: [SMBIOS 3.2 required]\n");
				printf("Memory Operating Mode Capability: [SMBIOS 3.2 required]\n");
				printf("Firmware Version: [SMBIOS 3.2 required]\n");
				printf("Module Manufacturer ID: [SMBIOS 3.2 required]\n");
				printf("Module Product ID: [SMBIOS 3.2 required]\n");
				printf("Memory Subsystem Controller Manufacturer ID: [SMBIOS 3.2 required]\n");
				printf("Memory Subsystem Controller Product ID: [SMBIOS 3.2 required]\n");
				printf("Non-volatile Size: [SMBIOS 3.2 required]\n");
				printf("Volatile Size: [SMBIOS 3.2 required]\n");
				printf("Cache Size: [SMBIOS 3.2 required]\n");
				printf("Logical Size: [SMBIOS 3.2 required]\n");
			}

			if (ISVERPLUS(ctx->DMIData, 3, 3)) {
				if (type17->extended_speed == LAZYBIOS_NOT_FOUND_U32) {
					printf("Extended Speed: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType17ExtendedSpeedStr(type17->extended_speed, buf, sizeof(buf));
					printf("Extended Speed: %s\n", buf);
				}

				if (type17->extended_configured_memory_speed == LAZYBIOS_NOT_FOUND_U32) {
					printf("Extended Configured Memory Speed: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType17ExtendedSpeedStr(type17->extended_configured_memory_speed, buf, sizeof(buf));
					printf("Extended Configured Memory Speed: %s\n", buf);
				}
			} else {
				printf("Extended Speed: [SMBIOS 3.3 required]\n");
				printf("Extended Configured Memory Speed: [SMBIOS 3.3 required]\n");
			}

			if (ISVERPLUS(ctx->DMIData, 3, 7)) {
				if (type17->pmic0_manufacturer_id == LAZYBIOS_NOT_FOUND_U16) {
					printf("PMIC0 Manufacturer ID: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType17PMIC0ManufacturerIDStr(type17->pmic0_manufacturer_id, buf, sizeof(buf));
					printf("PMIC0 Manufacturer ID: %s\n", buf);
				}

				if (type17->pmic0_revision_number == LAZYBIOS_NOT_FOUND_U16) {
					printf("PMIC0 Revision Number: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType17PMIC0RevisionStr(type17->pmic0_revision_number, buf, sizeof(buf));
					printf("PMIC0 Revision Number: %s\n", buf);
				}

				if (type17->rcd_manufacturer_id == LAZYBIOS_NOT_FOUND_U16) {
					printf("RCD Manufacturer ID: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType17RCDManufacturerIDStr(type17->rcd_manufacturer_id, buf, sizeof(buf));
					printf("RCD Manufacturer ID: %s\n", buf);
				}

				if (type17->rcd_revision_number == LAZYBIOS_NOT_FOUND_U16) {
					printf("RCD Revision Number: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType17RCDRevisionStr(type17->rcd_revision_number, buf, sizeof(buf));
					printf("RCD Revision Number: %s\n", buf);
				}
			} else {
				printf("PMIC0 Manufacturer ID: [SMBIOS 3.7 required]\n");
				printf("PMIC0 Revision Number: [SMBIOS 3.7 required]\n");
				printf("RCD Manufacturer ID: [SMBIOS 3.7 required]\n");
				printf("RCD Revision Number: [SMBIOS 3.7 required]\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Memory Device information\n\n");
	}
}

int print_smbios_version_info(lazybiosCTX_t* ctx) {
	if (!ctx) return -1;
	printf("=== SMBIOS VERSION INFORMATION ===\n");
	lazybiosPrintVer(ctx);

	printf("Table Length: %u bytes\n", ctx->DMIData->entry_info.table_length);
	if (ctx->backend == LAZYBIOS_BACKEND_WINDOWS) {
		printf("Table Address: Not available (Windows API)\n");
	} else {
		printf("Table Address: 0x%lX\n", ctx->DMIData->entry_info.table_address);
	}
	printf("Is 64-bit: %s\n", ISVERPLUS(ctx->DMIData, 3, 0) == 1 ? "Yes (SMBIOS 3.x)" : "No (SMBIOS 2.x)");
	if (ISVERPLUS(ctx->DMIData, 3, 0)) {
		printf("Docrev: %u\n", ctx->DMIData->entry_info.docrev);
	}
	printf("\n");

	return 0;
}

static inline void print_usage(const char* progname) {
	printf("Usage: %s [options]\n", progname);
	printf("Options:\n");
	printf("  --help                      Show this help message\n");
	printf("  --type <number>             prints the SMBIOS type <number>, so --type 0 means Firmware Information etc.");
	printf("  --dump <dir_to_dump>        Dumps both the raw entry info and raw DMI table into 2 files into <dir_to_dump> directory, If OS is Windows it only dumps DMI.bin\n If <dir_to_dump> isn't specified it will default to the current directory\n");
	printf("  --sources <entry> <dmi>     Usees the parser on the 2 specified files <entry> and <dmi>\n");
	printf("  --single-source <binary>    Uses the parser on the single specified file <binary>, which should hold the entry point first and dmi data merged together\n");
}

int main(int argc, const char* argv[]) {
	printf("lazybios Version: %s\n", LAZYBIOS_VER);
	printf("=============================================\n\n");

	lazybiosCTX_t* ctx = lazybiosCTXNew();
	if (!ctx) {
		fprintf(stderr, "Failed to allocate lazybios context\n");
		return 1;
	}

	int print_all = 1;
	int type_to_print = -1;
	const char* dump_dir = LAZYBIOS_NULL;
	const char* entry_file = LAZYBIOS_NULL;
	const char* dmi_file = LAZYBIOS_NULL;
	const char* single_file = LAZYBIOS_NULL;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--help") == 0) {
			print_usage(argv[0]);
			lazybiosCleanup(ctx);
			return 0;
		} else if (strcmp(argv[i], "--type") == 0) {
			if (i + 1 < argc) {
				type_to_print = atoi(argv[i + 1]);
				print_all = 0;
				i++;
			} else {
				fprintf(stderr, "Error: --type requires a type number\n");
				print_usage(argv[0]);
				lazybiosCleanup(ctx);
				return -1;
			}
		} else if (strcmp(argv[i], "--dump") == 0) {
			if (i + 1 < argc && argv[i + 1][0] != '-') {
				dump_dir = argv[i + 1];
				i++;
			} else {
				dump_dir = "."; // in here we default to the current directory
			}
		} else if (strcmp(argv[i], "--sources") == 0) {
			if (i + 2 < argc) {
				entry_file = argv[i + 1];
				dmi_file = argv[i + 2];
				i += 2;
			} else {
				fprintf(stderr, "Error: --sources requires two file arguments\n");
				print_usage(argv[0]);
				lazybiosCleanup(ctx);
				return -1;
			}
		} else if (strcmp(argv[i], "--single-source") == 0) {
			if (i + 1 < argc) {
				single_file = argv[i + 1];
				i++;
			} else {
				fprintf(stderr, "Error: --single-source requires a file argument\n");
				print_usage(argv[0]);
				lazybiosCleanup(ctx);
				return -1;
			}
		} else {
			fprintf(stderr, "Error: Unknown argument '%s'\n", argv[i]);
			print_usage(argv[0]);
			lazybiosCleanup(ctx);
			return -1;
		}
	}

	if (dump_dir) {
		lazybiosInit(ctx);

		char path_entry[1024];
		char path_dmi[1024];

#if defined(_WIN32) || defined(_WIN64)
		snprintf(path_dmi, sizeof(path_dmi), "%s\\DMI.bin", dump_dir);
		snprintf(path_entry, sizeof(path_entry), "%s\\generated_smbios_entry_point.bin", dump_dir);
#else
		snprintf(path_dmi, sizeof(path_dmi), "%s/DMI", dump_dir);
		snprintf(path_entry, sizeof(path_entry), "%s/smbios_entry_point", dump_dir);
#endif

		FILE* entry = NULL;
		FILE* dmi = NULL;
		if (ctx->backend == LAZYBIOS_BACKEND_LINUX) {
			entry = fopen(path_entry, "wb");
			if (!entry) {
				printf("Failed to open '%s': %s\n", path_entry, strerror(errno));
				return -1;
			}

			dmi = fopen(path_dmi, "wb");
			if (!dmi) {
				printf("Failed to open '%s': %s\n", path_dmi, strerror(errno));
				fclose(entry);
				return -1;
			}

			fwrite(ctx->DMIData->dmi_data, 1, ctx->DMIData->dmi_len, dmi);
			fwrite(ctx->DMIData->entry_data, 1, ctx->DMIData->entry_len, entry);
			fclose(entry);
			fclose(dmi);
			printf("%s and %s dumped successfully\n", path_entry, path_dmi);

		} else if (ctx->backend == LAZYBIOS_BACKEND_WINDOWS) {
			entry = fopen(path_entry, "wb");
			if (!entry) {
				printf("Failed to open '%s': %s\n", path_entry, strerror(errno));
				return -1;
			}

			dmi = fopen(path_dmi, "wb");
			if (!dmi) {
				printf("Failed to open '%s': %s\n", path_dmi, strerror(errno));
				fclose(entry);
				return -1;
			}

			fwrite(ctx->DMIData->dmi_data, 1, ctx->DMIData->dmi_len, dmi);
			fwrite(ctx->DMIData->entry_data, 1, ctx->DMIData->entry_len, entry);
			fclose(entry);
			fclose(dmi);
			printf("%s and %s dumped successfully\n", path_entry, path_dmi);
		} else { // MacOS coming soon? I don't have a mac so I can't test. But I'll try it on VMs.
			printf("Can't dump data. no backend found!");
		}

		lazybiosCleanup(ctx);
		return 0;
	}

	// We initialize from custom files ONLY if specified
	if (entry_file && dmi_file) {
		if (lazybiosFile(ctx, entry_file, dmi_file) != 0) {
			fprintf(stderr, "Failed to initialize lazybios from files\n");
			lazybiosCleanup(ctx);
			return -1;
		}
	} else if (single_file) {
		if (lazybiosSingleFile(ctx, single_file) != 0) {
			fprintf(stderr, "Failed to initialize lazybios from single file\n");
			lazybiosCleanup(ctx);
			return -1;
		}
	} else {
		if (lazybiosInit(ctx) != 0) {
			fprintf(stderr, "Failed to initialize lazybios library\n");
			lazybiosCleanup(ctx);
			return -1;
		}
	}

	printf("Library initialized successfully!\n\n");

	print_smbios_version_info(ctx);

	if (print_all) {
		if (!ctx->Type0) ctx->Type0 = lazybiosGetType0(ctx->Type0, ctx->DMIData);
		printType0(ctx);

		if (!ctx->Type1) ctx->Type1 = lazybiosGetType1(ctx->Type1, ctx->DMIData);
		printType1(ctx);

		if (!ctx->Type2) ctx->Type2 = lazybiosGetType2(ctx->Type2, &ctx->type2_count, ctx->DMIData);
		printType2(ctx);

		if (!ctx->Type3) ctx->Type3 = lazybiosGetType3(ctx->Type3, &ctx->type3_count, ctx->DMIData);
		printType3(ctx);

		if (!ctx->Type4) ctx->Type4 = lazybiosGetType4(ctx->Type4, &ctx->type4_count, ctx->DMIData);
		printType4(ctx);

		if (!ctx->Type17) ctx->Type17 = lazybiosGetType17(ctx->Type17, &ctx->type17_count, ctx->DMIData);
		printType17(ctx);
	} else {
		switch (type_to_print) {
			case 0:
				if (!ctx->Type0) ctx->Type0 = lazybiosGetType0(ctx->Type0, ctx->DMIData);

				printType0(ctx);
				break;

			case 1:
				if (!ctx->Type1) ctx->Type1 = lazybiosGetType1(ctx->Type1, ctx->DMIData);

				printType1(ctx);
				break;

			case 2:
				if (!ctx->Type2) ctx->Type2 = lazybiosGetType2(ctx->Type2, &ctx->type2_count, ctx->DMIData);

				printType2(ctx);
				break;

			case 3:
				if (!ctx->Type3) ctx->Type3 = lazybiosGetType3(ctx->Type3, &ctx->type3_count, ctx->DMIData);

				printType3(ctx);
				break;

			case 4:
				if (!ctx->Type4) ctx->Type4 = lazybiosGetType4(ctx->Type4, &ctx->type4_count, ctx->DMIData);
				printType4(ctx);
				break;

			case 17:
				if (!ctx->Type17) ctx->Type17 = lazybiosGetType17(ctx->Type17, &ctx->type17_count, ctx->DMIData);
				printType17(ctx);
				break;

			default:
				fprintf(stderr, "Error: Type %d is not implemented or invalid\n", type_to_print);
				lazybiosCleanup(ctx);
				return -1;
		}
	}

	if (lazybiosCleanup(ctx) == 0) {
		printf("Library cleanup completed!\n");
	} else {
		fprintf(stderr, "Could not complete library cleanup\n");
		return -1;
	}

	printf("Operation completed successfully!\n");
	return 0;
}

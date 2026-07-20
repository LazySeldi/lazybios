/**
 * @file test.c
 * @brief Provides the command-line test and inspection program for lazybios.
 * @author LazySeldi
 */

//
// test.c - File for testing ALL SMBIOS types we have implemented currently
//
#include "lazybios.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int compact_output = 0;

static void printType0(lazybiosCTX_t* ctx) {
	printf("=== BIOS INFORMATION ===\n");

	if (!ctx->Type0) ctx->Type0 = lazybiosGetType0(ctx->Type0, ctx->DMIData);

	if (ctx->Type0) {
		printf("Vendor: %s\n", ctx->Type0->vendor ? ctx->Type0->vendor : "Not Present");
		printf("Version: %s\n", ctx->Type0->version ? ctx->Type0->version : "Not Present");
		printf("Release Date: %s\n", ctx->Type0->release_date ? ctx->Type0->release_date : "Not Present");

		if (LAZYBIOS_FIELD_STATUS(ctx->Type0, bios_starting_segment) != LAZYBIOS_FIELD_PRESENT) {
			printf("BIOS Starting Segment: Not Present\n");
		} else {
			printf("BIOS Starting Segment: 0x%04hX\n", ctx->Type0->bios_starting_segment);
		}

		if (LAZYBIOS_FIELD_STATUS(ctx->Type0, characteristics) != LAZYBIOS_FIELD_PRESENT) {
			printf("Firmware Characteristics: Not Present\n");
		} else {
			char buf[(LAZYBIOS_DECODER_BUF_SIZE * 2)]; // Needs a bigger size
			lazybiosType0CharacteristicsStr(ctx->Type0->characteristics, buf, sizeof(buf));
			printf("Firmware Characteristics: %s\n", buf);
		}

		if (lazybiosIsVersionPlus(ctx->DMIData, 2, 4)) {
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

			if (LAZYBIOS_FIELD_STATUS(ctx->Type0, platform_major_release) != LAZYBIOS_FIELD_PRESENT) {
				printf("Platform Major Release: Not Present\n");
			} else {
				printf("Platform Major Release: %hhu\n", ctx->Type0->platform_major_release);
			}

			if (LAZYBIOS_FIELD_STATUS(ctx->Type0, platform_minor_release) != LAZYBIOS_FIELD_PRESENT) {
				printf("Platform Minor Release: Not Present\n");
			} else {
				printf("Platform Minor Release: %hhu\n", ctx->Type0->platform_minor_release);
			}

			if (LAZYBIOS_FIELD_STATUS(ctx->Type0, ec_major_release) != LAZYBIOS_FIELD_PRESENT) {
				printf("EC Major Release: Not Present\n");
			} else {
				printf("EC Major Release: %hhu\n", ctx->Type0->ec_major_release);
			}

			if (LAZYBIOS_FIELD_STATUS(ctx->Type0, ec_minor_release) != LAZYBIOS_FIELD_PRESENT) {
				printf("EC Minor Release: Not Present\n");
			} else {
				printf("EC Minor Release: %hhu\n", ctx->Type0->ec_minor_release);
			}
		} else {
			if (!compact_output) printf("Firmware Characteristics Extension Bytes 1: [SMBIOS 2.4 required]\n");
			if (!compact_output) printf("Firmware Characteristics Extension Bytes 2: [SMBIOS 2.4 required]\n");
			if (!compact_output) printf("Platform Major Release: [SMBIOS 2.4 required]\n");
			if (!compact_output) printf("Platform Minor Release: [SMBIOS 2.4 required]\n");
			if (!compact_output) printf("EC Major Release: [SMBIOS 2.4 required]\n");
			if (!compact_output) printf("EC Minor Release: [SMBIOS 2.4 required]\n");
		}

		if (lazybiosIsVersionPlus(ctx->DMIData, 3, 1)) {
			if (LAZYBIOS_FIELD_STATUS(ctx->Type0, extended_rom_size) != LAZYBIOS_FIELD_PRESENT) {
				printf("Extended ROM Size: Not Present\n\n");
			} else {
				printf("Extended ROM Size: %hu%s\n\n", lazybiosType0ExtendedROMSizeU16(ctx->Type0->extended_rom_size, ctx->Type0->unit), ctx->Type0->unit);
			}
		} else {
			if (LAZYBIOS_FIELD_STATUS(ctx->Type0, rom_size) != LAZYBIOS_FIELD_PRESENT) {
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
		printf("Manufacturer: %s\n", ctx->Type1->manufacturer ? ctx->Type1->manufacturer : "Not Present");
		printf("Product name: %s\n", ctx->Type1->product_name ? ctx->Type1->product_name : "Not Present");
		printf("Version: %s\n", ctx->Type1->version ? ctx->Type1->version : "Not Present");
		printf("Serial number: %s\n", ctx->Type1->serial_number ? ctx->Type1->serial_number : "Not Present");

		if (lazybiosIsVersionPlus(ctx->DMIData, 2, 1)) {
			// UUID is SMBIOS 2.1+
			if (LAZYBIOS_FIELD_STATUS(ctx->Type1, uuid) == LAZYBIOS_FIELD_PRESENT) {
				printf("UUID: %02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
					   ctx->Type1->uuid[3], ctx->Type1->uuid[2], ctx->Type1->uuid[1], ctx->Type1->uuid[0],
					   ctx->Type1->uuid[5], ctx->Type1->uuid[4],
					   ctx->Type1->uuid[7], ctx->Type1->uuid[6],
					   ctx->Type1->uuid[8], ctx->Type1->uuid[9], ctx->Type1->uuid[10], ctx->Type1->uuid[11],
					   ctx->Type1->uuid[12], ctx->Type1->uuid[13], ctx->Type1->uuid[14], ctx->Type1->uuid[15]);
			} else {
				printf("UUID: Not Present\n");
			}

			if (LAZYBIOS_FIELD_STATUS(ctx->Type1, wake_up_type) != LAZYBIOS_FIELD_PRESENT) {
				printf("Wake up type: Not Present\n");
			} else {
				printf("Wake up type: %s\n", lazybiosType1WakeupTypeStr(ctx->Type1->wake_up_type));
			}
		} else {
			if (!compact_output) printf("UUID: [SMBIOS 2.1 required]\n");
			if (!compact_output) printf("Wake up type: [SMBIOS 2.1 required]\n");
		}

		if (lazybiosIsVersionPlus(ctx->DMIData, 2, 4)) {
			// SKU and Family are SMBIOS 2.4+
			printf("SKU number: %s\n", ctx->Type1->sku_number ? ctx->Type1->sku_number : "Not Present");
			printf("Family: %s\n\n", ctx->Type1->family ? ctx->Type1->family : "Not Present");
		} else {
			if (!compact_output) printf("SKU number: [SMBIOS 2.4 required]\n");
			if (!compact_output) printf("Family: [SMBIOS 2.4 required]\n\n");
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

			printf("Manufacturer: %s\n", type2->manufacturer ? type2->manufacturer : "Not Present");
			printf("Product: %s\n", type2->product ? type2->product : "Not Present");
			printf("Version: %s\n", type2->version ? type2->version : "Not Present");
			printf("Serial number: %s\n", type2->serial_number ? type2->serial_number : "Not Present");
			printf("Asset tag: %s\n", type2->asset_tag ? type2->asset_tag : "Not Present");

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 1)) {
				if (LAZYBIOS_FIELD_STATUS(type2, feature_flags) != LAZYBIOS_FIELD_PRESENT) {
					printf("Feature flags: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType2FeatureflagsStr(type2->feature_flags, buf, sizeof(buf));
					printf("Feature flags: %s\n", buf);
				}

				printf("Location in Chassis: %s\n", type2->location_in_chassis ? type2->location_in_chassis : "Not Present");

				if (LAZYBIOS_FIELD_STATUS(type2, chassis_handle) != LAZYBIOS_FIELD_PRESENT) {
					printf("Chassis Handle: Not Present\n");
				} else {
					printf("Chassis Handle: 0x%04hX\n", type2->chassis_handle);
				}

				if (LAZYBIOS_FIELD_STATUS(type2, board_type) != LAZYBIOS_FIELD_PRESENT) {
					printf("Board Type: Not Present\n");
				} else {
					printf("Board Type: %s\n", lazybiosType2BoardTypeStr(type2->board_type));
				}

				if (LAZYBIOS_FIELD_STATUS(type2, number_of_contained_object_handles) != LAZYBIOS_FIELD_PRESENT ||
					type2->number_of_contained_object_handles == 0) {
					// No contained object handles or field not present here
				} else {
					for (int j = 0; j < type2->number_of_contained_object_handles; j++) {
						printf("  Contained Object Handle %d: 0x%04hX\n",
							   j, type2->contained_object_handles[j]);
					}
				}
			} else {
				if (!compact_output) printf("Feature flags: [SMBIOS 2.1 required]\n");
				if (!compact_output) printf("Location in Chassis: [SMBIOS 2.1 required]\n");
				if (!compact_output) printf("Chassis Handle: [SMBIOS 2.1 required]\n");
				if (!compact_output) printf("Board Type: [SMBIOS 2.1 required]\n");
				if (!compact_output) printf("Contained Object Handles: [SMBIOS 2.1 required]\n");
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

			printf("Manufacturer: %s\n", type3->manufacturer ? type3->manufacturer : "Not Present");
			if (LAZYBIOS_FIELD_STATUS(type3, type) != LAZYBIOS_FIELD_PRESENT) {
				printf("Type: Not Present\n");
			} else {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];
				lazybiosType3TypeStr(type3->type, buf, sizeof(buf));
				printf("Type: %s\n", buf);
			}
			printf("Version: %s\n", type3->version ? type3->version : "Not Present");
			printf("Serial Number: %s\n", type3->serial_number ? type3->serial_number : "Not Present");
			printf("Asset Tag: %s\n", type3->asset_tag ? type3->asset_tag : "Not Present");

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 1)) {
				if (LAZYBIOS_FIELD_STATUS(type3, boot_up_state) != LAZYBIOS_FIELD_PRESENT) {
					printf("BootUp State: Not Present\n");
				} else {
					printf("BootUp State: %s\n", lazybiosType3StateStr(type3->boot_up_state));
				}

				if (LAZYBIOS_FIELD_STATUS(type3, power_supply_state) != LAZYBIOS_FIELD_PRESENT) {
					printf("Power Supply State: Not Present\n");
				} else {
					printf("Power Supply State: %s\n", lazybiosType3StateStr(type3->power_supply_state));
				}

				if (LAZYBIOS_FIELD_STATUS(type3, thermal_state) != LAZYBIOS_FIELD_PRESENT) {
					printf("Thermal State: Not Present\n");
				} else {
					printf("Thermal State: %s\n", lazybiosType3StateStr(type3->thermal_state));
				}

				if (LAZYBIOS_FIELD_STATUS(type3, security_status) != LAZYBIOS_FIELD_PRESENT) {
					printf("Security Status: Not Present\n");
				} else {
					printf("Security Status: %s\n", lazybiosType3SecurityStatusStr(type3->security_status));
				}
			} else {
				if (!compact_output) printf("BootUp State: [SMBIOS 2.1 required]\n");
				if (!compact_output) printf("Power Supply State: [SMBIOS 2.1 required]\n");
				if (!compact_output) printf("Thermal State: [SMBIOS 2.1 required]\n");
				if (!compact_output) printf("Security Status: [SMBIOS 2.1 required]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 3)) {
				if (LAZYBIOS_FIELD_STATUS(type3, oem_defined) != LAZYBIOS_FIELD_PRESENT) {
					printf("OEM Defined: Not Present\n");
				} else {
					printf("OEM-defined DWORD: 0x%08X\n", type3->oem_defined);
				}

				if (LAZYBIOS_FIELD_STATUS(type3, height) != LAZYBIOS_FIELD_PRESENT) {
					printf("Height: Not Present\n");
				} else if (type3->height == 0x00) {
					printf("Height: Unspecified\n");
				} else {
					printf("Height: %hhuU\n", type3->height);
				}

				if (LAZYBIOS_FIELD_STATUS(type3, number_of_power_cords) != LAZYBIOS_FIELD_PRESENT) {
					printf("Number of Power Cords: Not Present\n");
				} else {
					printf("Number of Power Cords: %hhu\n", type3->number_of_power_cords);
				}

				if (LAZYBIOS_FIELD_STATUS(type3, contained_element_count) != LAZYBIOS_FIELD_PRESENT) {
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

				if (LAZYBIOS_FIELD_STATUS(type3, contained_element_record_length) != LAZYBIOS_FIELD_PRESENT) {
				} else if (type3->contained_element_record_length > 0) {
					printf("Contained Element Record Length: %hhu bytes\n", type3->contained_element_record_length);
				}

				if (lazybiosIsVersionPlus(ctx->DMIData, 2, 7)) {
					printf("SKU Number: %s\n", type3->sku_number ? type3->sku_number : "Not Present");
				} else {
					if (!compact_output) printf("SKU Number: [SMBIOS 2.7 required]\n");
				}

				if (lazybiosIsVersionPlus(ctx->DMIData, 3, 9)) {
					if (LAZYBIOS_FIELD_STATUS(type3, rack_type) != LAZYBIOS_FIELD_PRESENT) {
						printf("Rack Type: Not Present\n");
					} else {
						printf("Rack Type: 0x%02hhX\n", type3->rack_type);
					}

					if (LAZYBIOS_FIELD_STATUS(type3, rack_height) != LAZYBIOS_FIELD_PRESENT) {
						printf("Rack Height (extended): Not Present\n");
					} else {
						printf("Rack Height (extended): %hhuU\n", type3->rack_height);
					}
				} else {
					if (!compact_output) printf("Rack Type: [SMBIOS 3.9 required]\n");
					if (!compact_output) printf("Rack Height (extended): [SMBIOS 3.9 required]\n");
				}
			} else {
				if (!compact_output) printf("OEM Defined: [SMBIOS 2.3 required]\n");
				if (!compact_output) printf("Height: [SMBIOS 2.3 required]\n");
				if (!compact_output) printf("Number of Power Cords: [SMBIOS 2.3 required]\n");
				if (!compact_output) printf("Contained Elements: [SMBIOS 2.3 required]\n");
				if (!compact_output) printf("SKU Number: [SMBIOS 2.7 required]\n");
				if (!compact_output) printf("Rack Type: [SMBIOS 3.9 required]\n");
				if (!compact_output) printf("Rack Height (extended): [SMBIOS 3.9 required]\n");
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

			printf("Socket Designation: %s\n", type4->socket_designation ? type4->socket_designation : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type4, processor_type) != LAZYBIOS_FIELD_PRESENT) {
				printf("Processor Type: Not Present\n");
			} else {
				printf("Processor Type: %s\n", lazybiosType4TypeStr(type4->processor_type));
			}

			if (LAZYBIOS_FIELD_STATUS(type4, processor_family) != LAZYBIOS_FIELD_PRESENT) {
				printf("Processor Family: Not Present\n");
			} else {
				printf("Processor Family: %s\n", lazybiosType4ProcessorFamilyStr(type4->processor_family));
			}

			printf("Processor Manufacturer: %s\n", type4->processor_manufacturer ? type4->processor_manufacturer : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type4, processor_id) != LAZYBIOS_FIELD_PRESENT) {
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

			printf("Processor Version: %s\n", type4->processor_version ? type4->processor_version : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type4, voltage) != LAZYBIOS_FIELD_PRESENT) {
				printf("Voltage: Not Present\n");
			} else {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];
				lazybiosType4VoltageStr(type4->voltage, buf, sizeof(buf));
				printf("Voltage: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type4, external_clock) != LAZYBIOS_FIELD_PRESENT) {
				printf("External Clock: Not Present\n");
			} else {
				printf("External Clock: %u MHz\n", type4->external_clock);
			}

			if (LAZYBIOS_FIELD_STATUS(type4, max_speed) != LAZYBIOS_FIELD_PRESENT) {
				printf("Max Speed: Not Present\n");
			} else {
				printf("Max Speed: %u MHz\n", type4->max_speed);
			}

			if (LAZYBIOS_FIELD_STATUS(type4, current_speed) != LAZYBIOS_FIELD_PRESENT) {
				printf("Current Speed: Not Present\n");
			} else {
				printf("Current Speed: %u MHz\n", type4->current_speed);
			}

			if (LAZYBIOS_FIELD_STATUS(type4, status) != LAZYBIOS_FIELD_PRESENT) {
				printf("Status: Not Present\n");
			} else {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];
				lazybiosType4StatusStr(type4->status, buf, sizeof(buf));
				printf("Status: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type4, processor_upgrade) != LAZYBIOS_FIELD_PRESENT) {
				printf("Processor Upgrade: Not Present\n");
			} else {
				printf("Processor Upgrade: %s\n", lazybiosType4SocketTypeStr(type4->processor_upgrade));
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 1)) {
				if (LAZYBIOS_FIELD_STATUS(type4, l1_cache_handle) != LAZYBIOS_FIELD_PRESENT) {
					printf("L1 Cache Handle: Not Present\n");
				} else {
					printf("L1 Cache Handle: 0x%04hX\n", type4->l1_cache_handle);
				}

				if (LAZYBIOS_FIELD_STATUS(type4, l2_cache_handle) != LAZYBIOS_FIELD_PRESENT) {
					printf("L2 Cache Handle: Not Present\n");
				} else {
					printf("L2 Cache Handle: 0x%04hX\n", type4->l2_cache_handle);
				}

				if (LAZYBIOS_FIELD_STATUS(type4, l3_cache_handle) != LAZYBIOS_FIELD_PRESENT) {
					printf("L3 Cache Handle: Not Present\n");
				} else {
					printf("L3 Cache Handle: 0x%04hX\n", type4->l3_cache_handle);
				}
			} else {
				if (!compact_output) printf("L1 Cache Handle: [SMBIOS 2.1 required]\n");
				if (!compact_output) printf("L2 Cache Handle: [SMBIOS 2.1 required]\n");
				if (!compact_output) printf("L3 Cache Handle: [SMBIOS 2.1 required]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 3)) {
				printf("Serial Number: %s\n", type4->serial_number ? type4->serial_number : "Not Present");
				printf("Asset Tag: %s\n", type4->asset_tag ? type4->asset_tag : "Not Present");
				printf("Part Number: %s\n", type4->part_number ? type4->part_number : "Not Present");
			} else {
				if (!compact_output) printf("Serial Number: [SMBIOS 2.3 required]\n");
				if (!compact_output) printf("Asset Tag: [SMBIOS 2.3 required]\n");
				if (!compact_output) printf("Part Number: [SMBIOS 2.3 required]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 5)) {
				uint32_t actual_core_count = 0;
				int has_core_count = 0;
				if (type4->core_count == 0xFF && (LAZYBIOS_FIELD_STATUS(type4, core_count_2) == LAZYBIOS_FIELD_PRESENT)) {
					actual_core_count = type4->core_count_2;
					has_core_count = 1;
				} else if (LAZYBIOS_FIELD_STATUS(type4, core_count) == LAZYBIOS_FIELD_PRESENT && type4->core_count != 0xFF) {
					actual_core_count = type4->core_count;
					has_core_count = 1;
				}

				if (!has_core_count) {
					printf("Core Count: Not Present\n");
				} else {
					printf("Core Count: %u\n", actual_core_count);
				}

				uint32_t actual_core_enabled = 0;
				int has_core_enabled = 0;
				if (type4->core_enabled == 0xFF && (LAZYBIOS_FIELD_STATUS(type4, core_enabled_2) == LAZYBIOS_FIELD_PRESENT)) {
					actual_core_enabled = type4->core_enabled_2;
					has_core_enabled = 1;
				} else if (LAZYBIOS_FIELD_STATUS(type4, core_enabled) == LAZYBIOS_FIELD_PRESENT && type4->core_enabled != 0xFF) {
					actual_core_enabled = type4->core_enabled;
					has_core_enabled = 1;
				}

				if (!has_core_enabled) {
					printf("Core Enabled: Not Present\n");
				} else {
					printf("Core Enabled: %u\n", actual_core_enabled);
				}

				uint32_t actual_thread_count = 0;
				int has_thread_count = 0;
				if (type4->thread_count == 0xFF && (LAZYBIOS_FIELD_STATUS(type4, thread_count_2) == LAZYBIOS_FIELD_PRESENT)) {
					actual_thread_count = type4->thread_count_2;
					has_thread_count = 1;
				} else if (LAZYBIOS_FIELD_STATUS(type4, thread_count) == LAZYBIOS_FIELD_PRESENT && type4->thread_count != 0xFF) {
					actual_thread_count = type4->thread_count;
					has_thread_count = 1;
				}

				if (!has_thread_count) {
					printf("Thread Count: Not Present\n");
				} else {
					printf("Thread Count: %u\n", actual_thread_count);
				}

				if (LAZYBIOS_FIELD_STATUS(type4, processor_characteristics) != LAZYBIOS_FIELD_PRESENT) {
					printf("Processor Characteristics: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType4CharacteristicsStr(type4->processor_characteristics, buf, sizeof(buf));
					printf("Processor Characteristics: %s\n", buf);
				}

				if (LAZYBIOS_FIELD_STATUS(type4, processor_family_2) != LAZYBIOS_FIELD_PRESENT) {
					printf("Processor Family 2: Not Present\n");
				} else {
					printf("Processor Family 2: %s\n", lazybiosType4ProcessorFamilyStr(type4->processor_family_2));
				}
			} else {
				if (!compact_output) printf("Core Count: [SMBIOS 2.5 required]\n");
				if (!compact_output) printf("Core Enabled: [SMBIOS 2.5 required]\n");
				if (!compact_output) printf("Thread Count: [SMBIOS 2.5 required]\n");
				if (!compact_output) printf("Processor Characteristics: [SMBIOS 2.5 required]\n");
				if (!compact_output) printf("Processor Family 2: [SMBIOS 2.5 required]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 3, 6)) {
				if (LAZYBIOS_FIELD_STATUS(type4, thread_enabled) != LAZYBIOS_FIELD_PRESENT) {
					printf("Thread Enabled: Not Present\n");
				} else {
					printf("Thread Enabled: %u\n", type4->thread_enabled);
				}
			} else {
				if (!compact_output) printf("Thread Enabled: [SMBIOS 3.6 required]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 3, 8)) {
				printf("Socket Type: %s\n", type4->socket_type ? type4->socket_type : "Not Present");
			} else {
				if (!compact_output) printf("Socket Type: [SMBIOS 3.8 required]\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Processor information!\n\n");
	}
}

static void printType5(lazybiosCTX_t* ctx) {
	printf("=== MEMORY CONTROLLER INFORMATION (OBSOLETE) ===\n");

	if (!ctx->Type5) ctx->Type5 = lazybiosGetType5(ctx->Type5, &ctx->type5_count, ctx->DMIData);

	if (ctx->Type5 && ctx->type5_count > 0) {
		for (size_t i = 0; i < ctx->type5_count; i++) {
			lazybiosType5_t* type5 = &ctx->Type5[i];
			char buf[LAZYBIOS_DECODER_BUF_SIZE];

			if (ctx->type5_count > 1) printf("--- Memory Controller %zu ---\n", i + 1);

			if (LAZYBIOS_FIELD_STATUS(type5, error_detecting_method) != LAZYBIOS_FIELD_PRESENT) {
				printf("Error Detecting Method: Not Present\n");
			} else {
				printf("Error Detecting Method: %s\n",
					lazybiosType5ErrorDetectingMethodStr(type5->error_detecting_method));
			}

			if (LAZYBIOS_FIELD_STATUS(type5, error_correcting_capability) != LAZYBIOS_FIELD_PRESENT) {
				printf("Error Correcting Capability: Not Present\n");
			} else {
				lazybiosType5ErrorCorrectingCapabilityStr(type5->error_correcting_capability, buf, sizeof(buf));
				printf("Error Correcting Capability: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type5, supported_interleave) != LAZYBIOS_FIELD_PRESENT) {
				printf("Supported Interleave: Not Present\n");
			} else {
				printf("Supported Interleave: %s\n", lazybiosType5InterleaveStr(type5->supported_interleave));
			}

			if (LAZYBIOS_FIELD_STATUS(type5, current_interleave) != LAZYBIOS_FIELD_PRESENT) {
				printf("Current Interleave: Not Present\n");
			} else {
				printf("Current Interleave: %s\n", lazybiosType5InterleaveStr(type5->current_interleave));
			}

			if (LAZYBIOS_FIELD_STATUS(type5, maximum_memory_module_size) != LAZYBIOS_FIELD_PRESENT) {
				printf("Maximum Memory Module Size: Not Present\n");
			} else if (type5->maximum_memory_module_size < 64) {
				printf("Maximum Memory Module Size: %llu MiB\n",
					(unsigned long long)(1ULL << type5->maximum_memory_module_size));
			} else {
				printf("Maximum Memory Module Size: 2^%hhu MiB\n", type5->maximum_memory_module_size);
			}

			if (LAZYBIOS_FIELD_STATUS(type5, supported_speeds) != LAZYBIOS_FIELD_PRESENT) {
				printf("Supported Speeds: Not Present\n");
			} else {
				lazybiosType5SupportedSpeedsStr(type5->supported_speeds, buf, sizeof(buf));
				printf("Supported Speeds: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type5, supported_memory_types) != LAZYBIOS_FIELD_PRESENT) {
				printf("Supported Memory Types: Not Present\n");
			} else {
				lazybiosType5SupportedMemoryTypesStr(type5->supported_memory_types, buf, sizeof(buf));
				printf("Supported Memory Types: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type5, memory_module_voltage) != LAZYBIOS_FIELD_PRESENT) {
				printf("Memory Module Voltage: Not Present\n");
			} else {
				lazybiosType5MemoryModuleVoltageStr(type5->memory_module_voltage, buf, sizeof(buf));
				printf("Memory Module Voltage: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type5, number_of_associated_memory_slots) != LAZYBIOS_FIELD_PRESENT) {
				printf("Number of Associated Memory Slots: Not Present\n");
			} else {
				printf("Number of Associated Memory Slots: %hhu\n", type5->number_of_associated_memory_slots);
			}

			if (LAZYBIOS_FIELD_STATUS(type5, memory_module_configuration_handles) != LAZYBIOS_FIELD_PRESENT) {
				printf("Memory Module Configuration Handles: Not Present\n");
			} else if (type5->number_of_associated_memory_slots == 0) {
				printf("Memory Module Configuration Handles: None\n");
			} else {
				for (size_t j = 0; j < type5->number_of_associated_memory_slots; j++) {
					printf("Memory Module Configuration Handle %zu: 0x%04hX\n", j + 1,
						type5->memory_module_configuration_handles[j]);
				}
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 1)) {
				if (LAZYBIOS_FIELD_STATUS(type5, enabled_error_correcting_capabilities) != LAZYBIOS_FIELD_PRESENT) {
					printf("Enabled Error Correcting Capabilities: Not Present\n");
				} else {
					lazybiosType5ErrorCorrectingCapabilityStr(
						type5->enabled_error_correcting_capabilities, buf, sizeof(buf));
					printf("Enabled Error Correcting Capabilities: %s\n", buf);
				}
			} else if (!compact_output) {
				printf("Enabled Error Correcting Capabilities: [SMBIOS 2.1 required]\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Memory Controller information\n\n");
	}
}

static void printType6(lazybiosCTX_t* ctx) {
	printf("=== MEMORY MODULE INFORMATION (OBSOLETE) ===\n");

	if (!ctx->Type6) ctx->Type6 = lazybiosGetType6(ctx->Type6, &ctx->type6_count, ctx->DMIData);

	if (ctx->Type6 && ctx->type6_count > 0) {
		for (size_t i = 0; i < ctx->type6_count; i++) {
			lazybiosType6_t* type6 = &ctx->Type6[i];
			char buf[LAZYBIOS_DECODER_BUF_SIZE];

			if (ctx->type6_count > 1) printf("--- Memory Module %zu ---\n", i + 1);
			printf("Socket Designation: %s\n", type6->socket_designation ? type6->socket_designation : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type6, bank_connections) != LAZYBIOS_FIELD_PRESENT) {
				printf("Bank Connections: Not Present\n");
			} else {
				lazybiosType6BankConnectionsStr(type6->bank_connections, buf, sizeof(buf));
				printf("Bank Connections: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type6, current_speed) != LAZYBIOS_FIELD_PRESENT) {
				printf("Current Speed: Not Present\n");
			} else if (type6->current_speed == 0) {
				printf("Current Speed: Unknown\n");
			} else {
				printf("Current Speed: %hhu ns\n", type6->current_speed);
			}

			if (LAZYBIOS_FIELD_STATUS(type6, current_memory_type) != LAZYBIOS_FIELD_PRESENT) {
				printf("Current Memory Type: Not Present\n");
			} else {
				lazybiosType6CurrentMemoryTypeStr(type6->current_memory_type, buf, sizeof(buf));
				printf("Current Memory Type: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type6, installed_size) != LAZYBIOS_FIELD_PRESENT) {
				printf("Installed Size: Not Present\n");
			} else {
				lazybiosType6InstalledSizeStr(type6->installed_size, buf, sizeof(buf));
				printf("Installed Size: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type6, enabled_size) != LAZYBIOS_FIELD_PRESENT) {
				printf("Enabled Size: Not Present\n");
			} else {
				lazybiosType6EnabledSizeStr(type6->enabled_size, buf, sizeof(buf));
				printf("Enabled Size: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type6, error_status) != LAZYBIOS_FIELD_PRESENT) {
				printf("Error Status: Not Present\n");
			} else {
				lazybiosType6ErrorStatusStr(type6->error_status, buf, sizeof(buf));
				printf("Error Status: %s\n", buf);
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Memory Module information\n\n");
	}
}

static void printType7(lazybiosCTX_t* ctx) {
	printf("=== CACHE INFORMATION ===\n");

	if (!ctx->Type7) ctx->Type7 = lazybiosGetType7(ctx->Type7, &ctx->type7_count, ctx->DMIData);

	if (ctx->Type7 && ctx->type7_count > 0) {
		for (size_t i = 0; i < ctx->type7_count; i++) {
			lazybiosType7_t* type7 = &ctx->Type7[i];

			if (ctx->type7_count > 1) {
				printf("--- Cache %zu ---\n", i + 1);
			}

			printf("Socket Designation: %s\n", type7->socket_designation ? type7->socket_designation : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type7, cache_configuration) != LAZYBIOS_FIELD_PRESENT) {
				printf("Cache Configuration: Not Present\n");
			} else {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];
				lazybiosType7CacheConfigurationStr(type7->cache_configuration, buf, sizeof(buf));
				printf("Cache Configuration: %s\n", buf);
			}

			if (type7->maximum_cache_size == 0xFFFF) {
				if (LAZYBIOS_FIELD_STATUS(type7, maximum_cache_size_2) != LAZYBIOS_FIELD_PRESENT) {
					printf("Maximum Cache Size: Not Present\n");
				} else {
					uint64_t size_kb = lazybiosType7CacheU32(type7->maximum_cache_size_2);
					if (size_kb > 1024) {
						printf("Maximum Cache Size: %.2f MB\n", (double)size_kb / 1024.0);
					} else {
						printf("Maximum Cache Size: %llu KB\n", (unsigned long long)size_kb);
					}
				}
			} else {
				uint64_t size_kb = lazybiosType7CacheU16(type7->maximum_cache_size);
				if (size_kb > 1024) {
					printf("Maximum Cache Size: %.2f MB\n", (double)size_kb / 1024.0);
				} else {
					printf("Maximum Cache Size: %llu KB\n", (unsigned long long)size_kb);
				}
			}

			if (type7->installed_size == 0xFFFF) {
				if (LAZYBIOS_FIELD_STATUS(type7, installed_cache_size_2) != LAZYBIOS_FIELD_PRESENT) {
					printf("Installed Size: Not Present\n");
				} else {
					uint64_t size_kb = lazybiosType7CacheU32(type7->installed_cache_size_2);
					if (size_kb > 1024) {
						printf("Installed Size: %.2f MB\n", (double)size_kb / 1024.0);
					} else {
						printf("Installed Size: %llu KB\n", (unsigned long long)size_kb);
					}
				}
			} else {
				uint64_t size_kb = lazybiosType7CacheU16(type7->installed_size);
				if (size_kb > 1024) {
					printf("Installed Size: %.2f MB\n", (double)size_kb / 1024.0);
				} else {
					printf("Installed Size: %llu KB\n", (unsigned long long)size_kb);
				}
			}

			if (LAZYBIOS_FIELD_STATUS(type7, supported_sram_type) != LAZYBIOS_FIELD_PRESENT) {
				printf("Supported SRAM Type: Not Present\n");
			} else {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];
				lazybiosType7SRAMTypeStr(type7->supported_sram_type, buf, sizeof(buf));
				printf("Supported SRAM Type: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type7, current_sram_type) != LAZYBIOS_FIELD_PRESENT) {
				printf("Current SRAM Type: Not Present\n");
			} else {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];
				lazybiosType7SRAMTypeStr(type7->current_sram_type, buf, sizeof(buf));
				printf("Current SRAM Type: %s\n", buf);
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 1)) {
				if (LAZYBIOS_FIELD_STATUS(type7, cache_speed) != LAZYBIOS_FIELD_PRESENT) {
					printf("Cache Speed: Not Present\n");
				} else if (type7->cache_speed == 0) {
					printf("Cache Speed: Unknown\n");
				} else {
					printf("Cache Speed: %hhu ns\n", type7->cache_speed);
				}

				if (LAZYBIOS_FIELD_STATUS(type7, error_correction_type) != LAZYBIOS_FIELD_PRESENT) {
					printf("Error Correction Type: Not Present\n");
				} else {
					printf("Error Correction Type: %s\n", lazybiosType7ErrorCorrectionTypeStr(type7->error_correction_type));
				}

				if (LAZYBIOS_FIELD_STATUS(type7, system_cache_type) != LAZYBIOS_FIELD_PRESENT) {
					printf("System Cache Type: Not Present\n");
				} else {
					printf("System Cache Type: %s\n", lazybiosType7SystemCacheTypeStr(type7->system_cache_type));
				}

				if (LAZYBIOS_FIELD_STATUS(type7, associativity) != LAZYBIOS_FIELD_PRESENT) {
					printf("Associativity: Not Present\n");
				} else {
					printf("Associativity: %s\n", lazybiosType7AssociativityStr(type7->associativity));
				}
			} else {
				if (!compact_output) printf("Cache Speed: [SMBIOS 2.1 required]\n");
				if (!compact_output) printf("Error Correction Type: [SMBIOS 2.1 required]\n");
				if (!compact_output) printf("System Cache Type: [SMBIOS 2.1 required]\n");
				if (!compact_output) printf("Associativity: [SMBIOS 2.1 required]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 3, 1)) {
				if (LAZYBIOS_FIELD_STATUS(type7, maximum_cache_size_2) != LAZYBIOS_FIELD_PRESENT) {
					printf("Maximum Cache Size 2: Not Present\n");
				} else {
					uint64_t size_kb = lazybiosType7CacheU32(type7->maximum_cache_size_2);
					if (size_kb > 1024) {
						printf("Maximum Cache Size 2: %.2f MB\n", (double)size_kb / 1024.0);
					} else {
						printf("Maximum Cache Size 2: %llu KB\n", (unsigned long long)size_kb);
					}
				}

				if (LAZYBIOS_FIELD_STATUS(type7, installed_cache_size_2) != LAZYBIOS_FIELD_PRESENT) {
					printf("Installed Cache Size 2: Not Present\n");
				} else {
					uint64_t size_kb = lazybiosType7CacheU32(type7->installed_cache_size_2);
					if (size_kb > 1024) {
						printf("Installed Cache Size 2: %.2f MB\n", (double)size_kb / 1024.0);
					} else {
						printf("Installed Cache Size 2: %llu KB\n", (unsigned long long)size_kb);
					}
				}
			} else {
				if (!compact_output) printf("Maximum Cache Size 2: [SMBIOS 3.1 required]\n");
				if (!compact_output) printf("Installed Cache Size 2: [SMBIOS 3.1 required]\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Cache information\n\n");
	}
}

static void printType8(lazybiosCTX_t* ctx) {
	printf("=== PORT CONNECTOR INFORMATION ===\n");

	if (!ctx->Type8) ctx->Type8 = lazybiosGetType8(ctx->Type8, &ctx->type8_count, ctx->DMIData);

	if (ctx->Type8 && ctx->type8_count > 0) {
		for (size_t i = 0; i < ctx->type8_count; i++) {
			lazybiosType8_t* type8 = &ctx->Type8[i];

			if (ctx->type8_count > 1) {
				printf("--- Port Connector %zu ---\n", i + 1);
			}

			printf("Internal Reference Designator: %s\n", type8->internal_reference_designator ? type8->internal_reference_designator : "Not Present");
			printf("Internal Connector Type: %s\n", lazybiosType8ConnectorTypeStr(type8->internal_connector_type));
			printf("External Reference Designator: %s\n", type8->external_reference_designator ? type8->external_reference_designator : "Not Present");
			printf("External Connector Type: %s\n", lazybiosType8ConnectorTypeStr(type8->external_connector_type));
			printf("Port Type: %s\n", lazybiosType8PortTypeStr(type8->port_type));

			printf("\n");
		}
	} else {
		printf("Failed to get Port Connector information\n\n");
	}
}

static void printType9(lazybiosCTX_t* ctx) {
	printf("=== SYSTEM SLOTS ===\n");

	if (!ctx->Type9) ctx->Type9 = lazybiosGetType9(ctx->Type9, &ctx->type9_count, ctx->DMIData);

	if (ctx->Type9 && ctx->type9_count > 0) {
		for (size_t i = 0; i < ctx->type9_count; i++) {
			lazybiosType9_t* type9 = &ctx->Type9[i];

			if (ctx->type9_count > 1) {
				printf("--- System Slot %zu ---\n", i + 1);
			}

			printf("Slot Designation: %s\n", type9->slot_designation ? type9->slot_designation : "Not Present");
			if (LAZYBIOS_FIELD_STATUS(type9, slot_type) != LAZYBIOS_FIELD_PRESENT) {
				printf("Slot Type: Not Present\n");
			} else {
				printf("Slot Type: %s\n", lazybiosType9SlotTypeStr(type9->slot_type));
			}

			if (LAZYBIOS_FIELD_STATUS(type9, slot_data_bus_width) != LAZYBIOS_FIELD_PRESENT) {
				printf("Slot Data Bus Width: Not Present\n");
			} else {
				printf("Slot Data Bus Width: %s\n", lazybiosType9SlotWidthStr(type9->slot_data_bus_width));
			}

			if (LAZYBIOS_FIELD_STATUS(type9, current_usage) != LAZYBIOS_FIELD_PRESENT) {
				printf("Current Usage: Not Present\n");
			} else {
				printf("Current Usage: %s\n", lazybiosType9CurrentUsageStr(type9->current_usage));
			}

			if (LAZYBIOS_FIELD_STATUS(type9, slot_length) != LAZYBIOS_FIELD_PRESENT) {
				printf("Slot Length: Not Present\n");
			} else {
				printf("Slot Length: %s\n", lazybiosType9SlotLengthStr(type9->slot_length));
			}

			if (LAZYBIOS_FIELD_STATUS(type9, slot_id) != LAZYBIOS_FIELD_PRESENT) {
				printf("Slot ID: Not Present\n");
			} else {
				printf("Slot ID: 0x%04hX\n", type9->slot_id);
			}

			char buf[LAZYBIOS_DECODER_BUF_SIZE];
			lazybiosType9Characteristics1Str(type9->slot_characteristics_1, buf, sizeof(buf));
			printf("Slot Characteristics 1: %s\n", buf);

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 1)) {
				lazybiosType9Characteristics2Str(type9->slot_characteristics_2, buf, sizeof(buf));
				printf("Slot Characteristics 2: %s\n", buf);
			} else {
				if (!compact_output) printf("Slot Characteristics 2: [SMBIOS 2.1 required]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 6)) {
				if (LAZYBIOS_FIELD_STATUS(type9, segment_group_number) != LAZYBIOS_FIELD_PRESENT) {
					printf("Segment Group Number: Not Applicable\n");
				} else {
					printf("Segment Group Number: %hu\n", type9->segment_group_number);
				}

				if (LAZYBIOS_FIELD_STATUS(type9, bus_number) != LAZYBIOS_FIELD_PRESENT) {
					printf("Bus Number: Not Applicable\n");
				} else {
					printf("Bus Number: %hhu\n", type9->bus_number);
				}

				lazybiosType9DeviceFunctionStr(type9->device_function_number, buf, sizeof(buf));
				printf("Device/Function Number: %s\n", buf);
			} else {
				if (!compact_output) printf("Segment Group Number: [SMBIOS 2.6 required]\n");
				if (!compact_output) printf("Bus Number: [SMBIOS 2.6 required]\n");
				if (!compact_output) printf("Device/Function Number: [SMBIOS 2.6 required]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 3, 2)) {
				if (LAZYBIOS_FIELD_STATUS(type9, data_bus_width) != LAZYBIOS_FIELD_PRESENT) {
					printf("Data Bus Width: Not Present\n");
				} else {
					printf("Data Bus Width: %s\n", lazybiosType9SlotWidthStr(type9->data_bus_width));
				}

				if (LAZYBIOS_FIELD_STATUS(type9, peer_grouping_count) != LAZYBIOS_FIELD_PRESENT) {
					printf("Peer Grouping Count: Not Present\n");
				} else {
					printf("Peer Grouping Count: %hhu\n", type9->peer_grouping_count);
					for (size_t j = 0; j < type9->peer_grouping_count && type9->peer_groups; j++) {
						lazybiosType9PeerGroup_t* peer = &type9->peer_groups[j];
						printf("  Peer Group %zu:\n", j + 1);
						if (LAZYBIOS_FIELD_STATUS(peer, segment_group_number) != LAZYBIOS_FIELD_PRESENT) {
							printf("    Segment Group Number: Not Applicable\n");
						} else {
							printf("    Segment Group Number: %hu\n", peer->segment_group_number);
						}
						if (LAZYBIOS_FIELD_STATUS(peer, bus_number) != LAZYBIOS_FIELD_PRESENT) {
							printf("    Bus Number: Not Applicable\n");
						} else {
							printf("    Bus Number: %hhu\n", peer->bus_number);
						}
						lazybiosType9DeviceFunctionStr(peer->device_function_number, buf, sizeof(buf));
						printf("    Device/Function Number: %s\n", buf);
						printf("    Data Bus Width: %s\n", lazybiosType9SlotWidthStr(peer->data_bus_width));
					}
				}
			} else {
				if (!compact_output) printf("Data Bus Width: [SMBIOS 3.2 required]\n");
				if (!compact_output) printf("Peer Groups: [SMBIOS 3.2 required]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 3, 4)) {
				if (LAZYBIOS_FIELD_STATUS(type9, slot_information) != LAZYBIOS_FIELD_PRESENT) {
					printf("Slot Information: Not Present\n");
				} else if (type9->slot_type == 0xC4) {
					printf("Slot Information: PCI Express Generation %hhu\n", type9->slot_information);
				} else {
					printf("Slot Information: 0x%02hhX\n", type9->slot_information);
				}

				if (LAZYBIOS_FIELD_STATUS(type9, slot_physical_width) != LAZYBIOS_FIELD_PRESENT) {
					printf("Slot Physical Width: Not Present\n");
				} else {
					printf("Slot Physical Width: %s\n", lazybiosType9SlotWidthStr(type9->slot_physical_width));
				}

				if (LAZYBIOS_FIELD_STATUS(type9, slot_pitch) != LAZYBIOS_FIELD_PRESENT) {
					printf("Slot Pitch: Not Present\n");
				} else if (type9->slot_pitch == 0) {
					printf("Slot Pitch: Unknown\n");
				} else {
					printf("Slot Pitch: %.2f mm\n", (double)type9->slot_pitch / 100.0);
				}
			} else {
				if (!compact_output) printf("Slot Information: [SMBIOS 3.4 required]\n");
				if (!compact_output) printf("Slot Physical Width: [SMBIOS 3.4 required]\n");
				if (!compact_output) printf("Slot Pitch: [SMBIOS 3.4 required]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 3, 5)) {
				if (LAZYBIOS_FIELD_STATUS(type9, slot_height) != LAZYBIOS_FIELD_PRESENT) {
					printf("Slot Height: Not Present\n");
				} else {
					printf("Slot Height: %s\n", lazybiosType9SlotHeightStr(type9->slot_height));
				}
			} else {
				if (!compact_output) printf("Slot Height: [SMBIOS 3.5 required]\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get System Slot information\n\n");
	}
}

static void printType10(lazybiosCTX_t* ctx) {
	printf("=== ON BOARD DEVICES INFORMATION (OBSOLETE) ===\n");

	if (!ctx->Type10) ctx->Type10 = lazybiosGetType10(ctx->Type10, &ctx->type10_count, ctx->DMIData);

	if (ctx->Type10 && ctx->type10_count > 0) {
		for (size_t i = 0; i < ctx->type10_count; i++) {
			lazybiosType10_t* type10 = &ctx->Type10[i];

			if (ctx->type10_count > 1) printf("--- On Board Devices Structure %zu ---\n", i + 1);

			if (LAZYBIOS_FIELD_STATUS(type10, devices) != LAZYBIOS_FIELD_PRESENT) {
				printf("Devices: Not Present\n\n");
				continue;
			}
			if (type10->device_count == 0) {
				printf("Devices: None\n\n");
				continue;
			}

			for (size_t j = 0; j < type10->device_count; j++) {
				lazybiosType10Device_t* device = &type10->devices[j];
				if (type10->device_count > 1) printf("Device %zu:\n", j + 1);
				if (LAZYBIOS_FIELD_STATUS(device, device_type_and_status) != LAZYBIOS_FIELD_PRESENT) {
					printf("Type: Not Present\n");
					printf("Status: Not Present\n");
				} else {
					printf("Type: %s\n", lazybiosType10DeviceTypeStr(device->device_type_and_status));
					printf("Status: %s\n", lazybiosType10DeviceStatusStr(device->device_type_and_status));
				}
				printf("Description: %s\n", device->description ? device->description : "Not Present");
			}
			printf("\n");
		}
	} else {
		printf("Failed to get On Board Devices information\n\n");
	}
}

static void printType11(lazybiosCTX_t* ctx) {
	printf("=== OEM STRINGS ===\n");

	if (!ctx->Type11) ctx->Type11 = lazybiosGetType11(ctx->Type11, &ctx->type11_count, ctx->DMIData);

	if (ctx->Type11 && ctx->type11_count > 0) {
		for (size_t i = 0; i < ctx->type11_count; i++) {
			lazybiosType11_t* type11 = &ctx->Type11[i];

			if (ctx->type11_count > 1) {
				printf("--- OEM Strings Structure %zu ---\n", i + 1);
			}

			printf("String Count: %hhu\n", type11->string_count);
			for (size_t j = 0; j < type11->string_count; j++) {
				printf("OEM String %zu: %s\n", j + 1, type11->strings[j] ? type11->strings[j] : "Not Present");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get OEM Strings information\n\n");
	}
}

static void printType12(lazybiosCTX_t* ctx) {
	printf("=== SYSTEM CONFIGURATION OPTIONS ===\n");

	if (!ctx->Type12) ctx->Type12 = lazybiosGetType12(ctx->Type12, &ctx->type12_count, ctx->DMIData);

	if (ctx->Type12 && ctx->type12_count > 0) {
		for (size_t i = 0; i < ctx->type12_count; i++) {
			lazybiosType12_t* type12 = &ctx->Type12[i];

			if (ctx->type12_count > 1) {
				printf("--- System Configuration Options Structure %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type12, option_count) != LAZYBIOS_FIELD_PRESENT) {
				printf("Option Count: Not Present\n");
			} else {
				printf("Option Count: %hhu\n", type12->option_count);
			}

			if (LAZYBIOS_FIELD_STATUS(type12, options) != LAZYBIOS_FIELD_PRESENT) {
				printf("Configuration Options: Not Present\n");
			} else {
				for (size_t j = 0; j < type12->option_count; j++) {
					printf("Configuration Option %zu: %s\n", j + 1,
						type12->options[j] ? type12->options[j] : "Not Present");
				}
			}

			printf("\n");
		}
	} else {
		printf("Failed to get System Configuration Options information\n\n");
	}
}

static void printType13(lazybiosCTX_t* ctx) {
	printf("=== FIRMWARE LANGUAGE INFORMATION ===\n");

	if (!ctx->Type13) ctx->Type13 = lazybiosGetType13(ctx->Type13, &ctx->type13_count, ctx->DMIData);

	if (ctx->Type13 && ctx->type13_count > 0) {
		for (size_t i = 0; i < ctx->type13_count; i++) {
			lazybiosType13_t* type13 = &ctx->Type13[i];

			if (ctx->type13_count > 1) {
				printf("--- Firmware Language Structure %zu ---\n", i + 1);
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 1)) {
				if (LAZYBIOS_FIELD_STATUS(type13, flags) != LAZYBIOS_FIELD_PRESENT) {
					printf("Language Description Format: Not Present\n");
				} else {
					printf("Language Description Format: %s\n", lazybiosType13LanguageFormatStr(type13->flags));
				}
			} else {
				if (!compact_output) printf("Language Description Format: [SMBIOS 2.1 required]\n");
			}

			if (LAZYBIOS_FIELD_STATUS(type13, installable_languages) != LAZYBIOS_FIELD_PRESENT) {
				printf("Installable Languages: Not Present\n");
			} else {
				printf("Installable Languages: %hhu\n", type13->installable_languages);
			}

			if (LAZYBIOS_FIELD_STATUS(type13, languages) != LAZYBIOS_FIELD_PRESENT) {
				printf("Languages: Not Present\n");
			} else {
				for (size_t j = 0; j < type13->installable_languages; j++) {
					printf("Language %zu: %s\n", j + 1,
						type13->languages[j] ? type13->languages[j] : "Not Present");
				}
			}

			printf("Currently Installed Language: %s\n",
				type13->current_language ? type13->current_language : "Not Present");
			printf("\n");
		}
	} else {
		printf("Failed to get Firmware Language information\n\n");
	}
}

static void printType14(lazybiosCTX_t* ctx) {
	printf("=== GROUP ASSOCIATIONS ===\n");

	if (!ctx->Type14) ctx->Type14 = lazybiosGetType14(ctx->Type14, &ctx->type14_count, ctx->DMIData);

	if (ctx->Type14 && ctx->type14_count > 0) {
		for (size_t i = 0; i < ctx->type14_count; i++) {
			lazybiosType14_t* type14 = &ctx->Type14[i];

			if (ctx->type14_count > 1) {
				printf("--- Group Association %zu ---\n", i + 1);
			}

			printf("Group Name: %s\n", type14->group_name ? type14->group_name : "Not Present");
			if (LAZYBIOS_FIELD_STATUS(type14, item_count) != LAZYBIOS_FIELD_PRESENT) {
				printf("Item Count: Not Present\n");
			} else {
				printf("Item Count: %zu\n", type14->item_count);
			}

			if (LAZYBIOS_FIELD_STATUS(type14, items) != LAZYBIOS_FIELD_PRESENT) {
				printf("Items: Not Present\n");
			} else {
				for (size_t j = 0; j < type14->item_count; j++) {
					lazybiosType14Item_t* item = &type14->items[j];
					printf("Item %zu Type: %hhu\n", j + 1, item->item_type);
					printf("Item %zu Handle: 0x%04hX\n", j + 1, item->item_handle);
				}
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Group Associations information\n\n");
	}
}

static void printType15(lazybiosCTX_t* ctx) {
	printf("=== SYSTEM EVENT LOG ===\n");

	if (!ctx->Type15) ctx->Type15 = lazybiosGetType15(ctx->Type15, &ctx->type15_count, ctx->DMIData);

	if (ctx->Type15 && ctx->type15_count > 0) {
		for (size_t i = 0; i < ctx->type15_count; i++) {
			lazybiosType15_t* type15 = &ctx->Type15[i];
			char buf[LAZYBIOS_DECODER_BUF_SIZE];

			if (ctx->type15_count > 1) {
				printf("--- System Event Log %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type15, log_area_length) != LAZYBIOS_FIELD_PRESENT) {
				printf("Log Area Length: Not Present\n");
			} else {
				printf("Log Area Length: %hu bytes\n", type15->log_area_length);
			}

			if (LAZYBIOS_FIELD_STATUS(type15, log_header_start_offset) != LAZYBIOS_FIELD_PRESENT) {
				printf("Log Header Start Offset: Not Present\n");
			} else {
				printf("Log Header Start Offset: 0x%04hX\n", type15->log_header_start_offset);
			}

			if (LAZYBIOS_FIELD_STATUS(type15, log_data_start_offset) != LAZYBIOS_FIELD_PRESENT) {
				printf("Log Data Start Offset: Not Present\n");
			} else {
				printf("Log Data Start Offset: 0x%04hX\n", type15->log_data_start_offset);
			}

			if (LAZYBIOS_FIELD_STATUS(type15, access_method) != LAZYBIOS_FIELD_PRESENT) {
				printf("Access Method: Not Present\n");
			} else {
				printf("Access Method: %s\n", lazybiosType15AccessMethodStr(type15->access_method));
			}

			if (LAZYBIOS_FIELD_STATUS(type15, log_status) != LAZYBIOS_FIELD_PRESENT) {
				printf("Log Status: Not Present\n");
			} else {
				lazybiosType15LogStatusStr(type15->log_status, buf, sizeof(buf));
				printf("Log Status: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type15, log_change_token) != LAZYBIOS_FIELD_PRESENT) {
				printf("Log Change Token: Not Present\n");
			} else if (type15->log_change_token == 0) {
				printf("Log Change Token: Not Implemented\n");
			} else {
				printf("Log Change Token: 0x%08X\n", type15->log_change_token);
			}

			if (LAZYBIOS_FIELD_STATUS(type15, access_method_address) != LAZYBIOS_FIELD_PRESENT) {
				printf("Access Method Address: Not Present\n");
			} else if (LAZYBIOS_FIELD_STATUS(type15, access_method) == LAZYBIOS_FIELD_PRESENT &&
				type15->access_method <= 0x02) {
				printf("Index Address: 0x%04hX\n", lazybiosType15IndexAddress(type15->access_method_address));
				printf("Data Address: 0x%04hX\n", lazybiosType15DataAddress(type15->access_method_address));
			} else if (LAZYBIOS_FIELD_STATUS(type15, access_method) == LAZYBIOS_FIELD_PRESENT &&
				type15->access_method == 0x03) {
				printf("Physical Address: 0x%08X\n", type15->access_method_address);
			} else if (LAZYBIOS_FIELD_STATUS(type15, access_method) == LAZYBIOS_FIELD_PRESENT &&
				type15->access_method == 0x04) {
				printf("GPNV Handle: 0x%04hX\n", lazybiosType15GPNVHandle(type15->access_method_address));
			} else {
				printf("Access Method Address: 0x%08X\n", type15->access_method_address);
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 1)) {
				if (LAZYBIOS_FIELD_STATUS(type15, log_header_format) != LAZYBIOS_FIELD_PRESENT) {
					printf("Log Header Format: Not Present\n");
				} else {
					printf("Log Header Format: %s\n", lazybiosType15LogHeaderFormatStr(type15->log_header_format));
				}

				if (LAZYBIOS_FIELD_STATUS(type15, number_of_supported_log_type_descriptors) != LAZYBIOS_FIELD_PRESENT) {
					printf("Supported Log Type Descriptor Count: Not Present\n");
				} else {
					printf("Supported Log Type Descriptor Count: %hhu\n",
						type15->number_of_supported_log_type_descriptors);
				}

				if (LAZYBIOS_FIELD_STATUS(type15, length_of_each_log_type_descriptor) != LAZYBIOS_FIELD_PRESENT) {
					printf("Log Type Descriptor Length: Not Present\n");
				} else {
					printf("Log Type Descriptor Length: %hhu bytes\n",
						type15->length_of_each_log_type_descriptor);
				}

				if (LAZYBIOS_FIELD_STATUS(type15, supported_log_type_descriptors) != LAZYBIOS_FIELD_PRESENT) {
					printf("Supported Log Type Descriptors: Not Present\n");
				} else {
					for (size_t j = 0; j < type15->number_of_supported_log_type_descriptors; j++) {
						lazybiosType15LogTypeDescriptor_t* descriptor =
							&type15->supported_log_type_descriptors[j];
						printf("Descriptor %zu Log Type: %s\n", j + 1,
							lazybiosType15LogTypeStr(descriptor->log_type));
						printf("Descriptor %zu Variable Data Format: %s\n", j + 1,
							lazybiosType15VariableDataFormatTypeStr(descriptor->variable_data_format_type));
					}
				}
			} else if (!compact_output) {
				printf("Log Header Format: [SMBIOS 2.1 required]\n");
				printf("Supported Log Type Descriptors: [SMBIOS 2.1 required]\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get System Event Log information\n\n");
	}
}

static void printType16(lazybiosCTX_t* ctx) {
	printf("=== PHYSICAL MEMORY ARRAY ===\n");

	if (!ctx->Type16) ctx->Type16 = lazybiosGetType16(ctx->Type16, &ctx->type16_count, ctx->DMIData);

	if (ctx->Type16 && ctx->type16_count > 0) {
		for (size_t i = 0; i < ctx->type16_count; i++) {
			lazybiosType16_t* type16 = &ctx->Type16[i];

			if (ctx->type16_count > 1) {
				printf("--- Physical Memory Array %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type16, location) != LAZYBIOS_FIELD_PRESENT) {
				printf("Location: Not Present\n");
			} else {
				printf("Location: %s\n", lazybiosType16LocationStr(type16->location));
			}

			if (LAZYBIOS_FIELD_STATUS(type16, use) != LAZYBIOS_FIELD_PRESENT) {
				printf("Use: Not Present\n");
			} else {
				printf("Use: %s\n", lazybiosType16UseStr(type16->use));
			}

			if (LAZYBIOS_FIELD_STATUS(type16, memory_error_correction) != LAZYBIOS_FIELD_PRESENT) {
				printf("Memory Error Correction: Not Present\n");
			} else {
				printf("Memory Error Correction: %s\n",
					lazybiosType16MemoryErrorCorrectionStr(type16->memory_error_correction));
			}

			if (LAZYBIOS_FIELD_STATUS(type16, maximum_capacity) != LAZYBIOS_FIELD_PRESENT) {
				printf("Maximum Capacity: Not Present\n");
			} else if (type16->maximum_capacity == 0x80000000U &&
				LAZYBIOS_FIELD_STATUS(type16, extended_maximum_capacity) != LAZYBIOS_FIELD_PRESENT) {
				printf("Maximum Capacity: Not Present\n");
			} else {
				uint64_t capacity = lazybiosType16MaximumCapacityBytes(
					type16->maximum_capacity, type16->extended_maximum_capacity);
				if (capacity % (1024ULL * 1024 * 1024 * 1024) == 0) {
					printf("Maximum Capacity: %llu TiB\n",
						(unsigned long long)(capacity / (1024ULL * 1024 * 1024 * 1024)));
				} else if (capacity % (1024ULL * 1024 * 1024) == 0) {
					printf("Maximum Capacity: %llu GiB\n",
						(unsigned long long)(capacity / (1024ULL * 1024 * 1024)));
				} else if (capacity % (1024ULL * 1024) == 0) {
					printf("Maximum Capacity: %llu MiB\n",
						(unsigned long long)(capacity / (1024ULL * 1024)));
				} else {
					printf("Maximum Capacity: %llu bytes\n", (unsigned long long)capacity);
				}
			}

			if (LAZYBIOS_FIELD_STATUS(type16, memory_error_information_handle) != LAZYBIOS_FIELD_PRESENT) {
				printf("Memory Error Information Handle: Not Present\n");
			} else if (type16->memory_error_information_handle == 0xFFFE) {
				printf("Memory Error Information Handle: Not Provided\n");
			} else if (type16->memory_error_information_handle == 0xFFFF) {
				printf("Memory Error Information Handle: No Error Detected\n");
			} else {
				printf("Memory Error Information Handle: 0x%04hX\n", type16->memory_error_information_handle);
			}

			if (LAZYBIOS_FIELD_STATUS(type16, number_of_memory_devices) != LAZYBIOS_FIELD_PRESENT) {
				printf("Number of Memory Devices: Not Present\n");
			} else {
				printf("Number of Memory Devices: %hu\n", type16->number_of_memory_devices);
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 7)) {
				if (LAZYBIOS_FIELD_STATUS(type16, extended_maximum_capacity) != LAZYBIOS_FIELD_PRESENT) {
					printf("Extended Maximum Capacity: Not Present\n");
				} else if (type16->maximum_capacity != 0x80000000U) {
					printf("Extended Maximum Capacity: Not Used\n");
				} else {
					printf("Extended Maximum Capacity: %llu bytes\n",
						(unsigned long long)type16->extended_maximum_capacity);
				}
			} else if (!compact_output) {
				printf("Extended Maximum Capacity: [SMBIOS 2.7 required]\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Physical Memory Array information\n\n");
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

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 1)) {
				if (LAZYBIOS_FIELD_STATUS(type17, physical_memory_array_handle) != LAZYBIOS_FIELD_PRESENT) {
					printf("Physical Memory Array Handle: Not Present\n");
				} else {
					printf("Physical Memory Array Handle: 0x%04hX\n", type17->physical_memory_array_handle);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, memory_error_information_handle) != LAZYBIOS_FIELD_PRESENT) {
					printf("Memory Error Information Handle: Not Present\n");
				} else {
					printf("Memory Error Information Handle: 0x%04hX\n", type17->memory_error_information_handle);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, total_width) != LAZYBIOS_FIELD_PRESENT) {
					printf("Total Width: Not Present\n");
				} else {
					printf("Total Width: %u bits\n", type17->total_width == 0xFFFF ? 0 : type17->total_width);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, data_width) != LAZYBIOS_FIELD_PRESENT) {
					printf("Data Width: Not Present\n");
				} else {
					printf("Data Width: %u bits\n", type17->data_width == 0xFFFF ? 0 : type17->data_width);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, size) != LAZYBIOS_FIELD_PRESENT) {
					printf("Size: Not Present\n");
				} else {
					// Size: 0 = no device, 0xFFFF = unknown, Bit 15 = KB, else MB
					if (type17->size == 0) printf("Size: No Module Installed\n");
					else if (type17->size == 0xFFFF) printf("Size: Unknown\n");
					else printf("Size: %u %s\n", (type17->size & 0x7FFF), (type17->size & 0x8000) ? "KB" : "MB");
				}

				if (LAZYBIOS_FIELD_STATUS(type17, form_factor) != LAZYBIOS_FIELD_PRESENT) {
					printf("Form Factor: Not Present\n");
				} else {
					printf("Form Factor: %s\n", lazybiosType17FormFactorStr(type17->form_factor));
				}

				if (LAZYBIOS_FIELD_STATUS(type17, device_set) != LAZYBIOS_FIELD_PRESENT) {
					printf("Device Set: Not Present\n");
				} else {
					printf("Device Set: %u\n", type17->device_set);
				}

				printf("Device Locator: %s\n", type17->device_locator ? type17->device_locator : "Not Present");
				printf("Bank Locator: %s\n", type17->bank_locator ? type17->bank_locator : "Not Present");

				if (LAZYBIOS_FIELD_STATUS(type17, memory_type) != LAZYBIOS_FIELD_PRESENT) {
					printf("Memory Type: Not Present\n");
				} else {
					printf("Memory Type: %s\n", lazybiosType17TypeStr(type17->memory_type));
				}

				if (LAZYBIOS_FIELD_STATUS(type17, type_detail) != LAZYBIOS_FIELD_PRESENT) {
					printf("Type Detail: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType17TypeDetailStr(type17->type_detail, buf, sizeof(buf));
					printf("Type Detail: %s\n", buf);
				}
			} else {
				if (!compact_output) printf("[SMBIOS 2.1 required for Memory Device details]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 3)) {
				if (LAZYBIOS_FIELD_STATUS(type17, speed) != LAZYBIOS_FIELD_PRESENT) {
					printf("Speed: Not Present\n");
				} else {
					printf("Speed: %u MT/s\n", type17->speed == 0xFFFF ? 0 : type17->speed);
				}

				printf("Manufacturer: %s\n", type17->manufacturer ? type17->manufacturer : "Not Present");
				printf("Serial Number: %s\n", type17->serial_number ? type17->serial_number : "Not Present");
				printf("Asset Tag: %s\n", type17->asset_tag ? type17->asset_tag : "Not Present");
				printf("Part Number: %s\n", type17->part_number ? type17->part_number : "Not Present");

			} else {
				if (!compact_output) printf("Speed: [SMBIOS 2.3 required]\n");
				if (!compact_output) printf("Manufacturer: [SMBIOS 2.3 required]\n");
				if (!compact_output) printf("Serial Number: [SMBIOS 2.3 required]\n");
				if (!compact_output) printf("Asset Tag: [SMBIOS 2.3 required]\n");
				if (!compact_output) printf("Part Number: [SMBIOS 2.3 required]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 6)) {
				if (LAZYBIOS_FIELD_STATUS(type17, attributes) != LAZYBIOS_FIELD_PRESENT) {
					printf("Attributes: Not Present\n");
				} else {
					printf("Attributes: %u rank(s)\n", type17->attributes & 0x7F);
				}
			} else {
				if (!compact_output) printf("Attributes: [SMBIOS 2.6 required]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 7)) {
				if (LAZYBIOS_FIELD_STATUS(type17, extended_size) != LAZYBIOS_FIELD_PRESENT) {
					printf("Extended Size: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType17ExtendedSizeStr(type17->extended_size, buf, sizeof(buf));
					printf("Extended Size: %s\n", buf);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, configured_memory_speed) != LAZYBIOS_FIELD_PRESENT) {
					printf("Configured Memory Speed: Not Present\n");
				} else {
					printf("Configured Memory Speed: %u MT/s\n", type17->configured_memory_speed == 0xFFFF ? 0 : type17->configured_memory_speed);
				}
			} else {
				if (!compact_output) printf("Extended Size: [SMBIOS 2.7 required]\n");
				if (!compact_output) printf("Configured Memory Speed: [SMBIOS 2.7 required]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 8)) {
				if (LAZYBIOS_FIELD_STATUS(type17, minimum_voltage) != LAZYBIOS_FIELD_PRESENT) {
					printf("Minimum Voltage: Not Present\n");
				} else {
					printf("Minimum Voltage: %u mV\n", type17->minimum_voltage);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, maximum_voltage) != LAZYBIOS_FIELD_PRESENT) {
					printf("Maximum Voltage: Not Present\n");
				} else {
					printf("Maximum Voltage: %u mV\n", type17->maximum_voltage);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, configured_voltage) != LAZYBIOS_FIELD_PRESENT) {
					printf("Configured Voltage: Not Present\n");
				} else {
					printf("Configured Voltage: %u mV\n", type17->configured_voltage);
				}
			} else {
				if (!compact_output) printf("Minimum Voltage: [SMBIOS 2.8 required]\n");
				if (!compact_output) printf("Maximum Voltage: [SMBIOS 2.8 required]\n");
				if (!compact_output) printf("Configured Voltage: [SMBIOS 2.8 required]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 3, 2)) {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];

				if (LAZYBIOS_FIELD_STATUS(type17, memory_technology) != LAZYBIOS_FIELD_PRESENT) {
					printf("Memory Technology: Not Present\n");
				} else {
					printf("Memory Technology: %s\n", lazybiosType17MemoryTechnologyStr(type17->memory_technology));
				}

				if (LAZYBIOS_FIELD_STATUS(type17, memory_operating_mode_capability) != LAZYBIOS_FIELD_PRESENT) {
					printf("Memory Operating Mode Capability: Not Present\n");
				} else {
					lazybiosType17OperatingModeCapabilityStr(type17->memory_operating_mode_capability, buf, sizeof(buf));
					printf("Memory Operating Mode Capability: %s\n", buf);
				}

				printf("Firmware Version: %s\n", type17->firmware_version ? type17->firmware_version : "Not Present");

				if (LAZYBIOS_FIELD_STATUS(type17, module_manufacturer_id) != LAZYBIOS_FIELD_PRESENT) {
					printf("Module Manufacturer ID: Not Present\n");
				} else {
					lazybiosType17ModuleManufacturerIDStr(type17->module_manufacturer_id, buf, sizeof(buf));
					printf("Module Manufacturer ID: %s\n", buf);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, module_product_id) != LAZYBIOS_FIELD_PRESENT) {
					printf("Module Product ID: Not Present\n");
				} else {
					lazybiosType17ModuleManufacturerIDStr(type17->module_product_id, buf, sizeof(buf));
					printf("Module Product ID: %s\n", buf);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, memory_subsystem_controller_manufacturer_id) != LAZYBIOS_FIELD_PRESENT) {
					printf("Memory Subsystem Controller Manufacturer ID: Not Present\n");
				} else {
					lazybiosType17ModuleManufacturerIDStr(type17->memory_subsystem_controller_manufacturer_id, buf, sizeof(buf));
					printf("Memory Subsystem Controller Manufacturer ID: %s\n", buf);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, memory_subsystem_controller_product_id) != LAZYBIOS_FIELD_PRESENT) {
					printf("Memory Subsystem Controller Product ID: Not Present\n");
				} else {
					lazybiosType17ModuleManufacturerIDStr(type17->memory_subsystem_controller_product_id, buf, sizeof(buf));
					printf("Memory Subsystem Controller Product ID: %s\n", buf);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, non_volatile_size) != LAZYBIOS_FIELD_PRESENT) {
					printf("Non-volatile Size: Not Present\n");
				} else {
					lazybiosType17NonVolatileSizeStr(type17->non_volatile_size, buf, sizeof(buf));
					printf("Non-volatile Size: %s\n", buf);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, volatile_size) != LAZYBIOS_FIELD_PRESENT) {
					printf("Volatile Size: Not Present\n");
				} else {
					lazybiosType17VolatileSizeStr(type17->volatile_size, buf, sizeof(buf));
					printf("Volatile Size: %s\n", buf);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, cache_size) != LAZYBIOS_FIELD_PRESENT) {
					printf("Cache Size: Not Present\n");
				} else {
					lazybiosType17CacheSizeStr(type17->cache_size, buf, sizeof(buf));
					printf("Cache Size: %s\n", buf);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, logical_size) != LAZYBIOS_FIELD_PRESENT) {
					printf("Logical Size: Not Present\n");
				} else {
					printf("Logical Size: %llu bytes\n", (unsigned long long)type17->logical_size);
				}
			} else {
				if (!compact_output) printf("Memory Technology: [SMBIOS 3.2 required]\n");
				if (!compact_output) printf("Memory Operating Mode Capability: [SMBIOS 3.2 required]\n");
				if (!compact_output) printf("Firmware Version: [SMBIOS 3.2 required]\n");
				if (!compact_output) printf("Module Manufacturer ID: [SMBIOS 3.2 required]\n");
				if (!compact_output) printf("Module Product ID: [SMBIOS 3.2 required]\n");
				if (!compact_output) printf("Memory Subsystem Controller Manufacturer ID: [SMBIOS 3.2 required]\n");
				if (!compact_output) printf("Memory Subsystem Controller Product ID: [SMBIOS 3.2 required]\n");
				if (!compact_output) printf("Non-volatile Size: [SMBIOS 3.2 required]\n");
				if (!compact_output) printf("Volatile Size: [SMBIOS 3.2 required]\n");
				if (!compact_output) printf("Cache Size: [SMBIOS 3.2 required]\n");
				if (!compact_output) printf("Logical Size: [SMBIOS 3.2 required]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 3, 3)) {
				if (LAZYBIOS_FIELD_STATUS(type17, extended_speed) != LAZYBIOS_FIELD_PRESENT) {
					printf("Extended Speed: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType17ExtendedSpeedStr(type17->extended_speed, buf, sizeof(buf));
					printf("Extended Speed: %s\n", buf);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, extended_configured_memory_speed) != LAZYBIOS_FIELD_PRESENT) {
					printf("Extended Configured Memory Speed: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType17ExtendedSpeedStr(type17->extended_configured_memory_speed, buf, sizeof(buf));
					printf("Extended Configured Memory Speed: %s\n", buf);
				}
			} else {
				if (!compact_output) printf("Extended Speed: [SMBIOS 3.3 required]\n");
				if (!compact_output) printf("Extended Configured Memory Speed: [SMBIOS 3.3 required]\n");
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 3, 7)) {
				if (LAZYBIOS_FIELD_STATUS(type17, pmic0_manufacturer_id) != LAZYBIOS_FIELD_PRESENT) {
					printf("PMIC0 Manufacturer ID: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType17PMIC0ManufacturerIDStr(type17->pmic0_manufacturer_id, buf, sizeof(buf));
					printf("PMIC0 Manufacturer ID: %s\n", buf);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, pmic0_revision_number) != LAZYBIOS_FIELD_PRESENT) {
					printf("PMIC0 Revision Number: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType17PMIC0RevisionStr(type17->pmic0_revision_number, buf, sizeof(buf));
					printf("PMIC0 Revision Number: %s\n", buf);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, rcd_manufacturer_id) != LAZYBIOS_FIELD_PRESENT) {
					printf("RCD Manufacturer ID: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType17RCDManufacturerIDStr(type17->rcd_manufacturer_id, buf, sizeof(buf));
					printf("RCD Manufacturer ID: %s\n", buf);
				}

				if (LAZYBIOS_FIELD_STATUS(type17, rcd_revision_number) != LAZYBIOS_FIELD_PRESENT) {
					printf("RCD Revision Number: Not Present\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType17RCDRevisionStr(type17->rcd_revision_number, buf, sizeof(buf));
					printf("RCD Revision Number: %s\n", buf);
				}
			} else {
				if (!compact_output) printf("PMIC0 Manufacturer ID: [SMBIOS 3.7 required]\n");
				if (!compact_output) printf("PMIC0 Revision Number: [SMBIOS 3.7 required]\n");
				if (!compact_output) printf("RCD Manufacturer ID: [SMBIOS 3.7 required]\n");
				if (!compact_output) printf("RCD Revision Number: [SMBIOS 3.7 required]\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Memory Device information\n\n");
	}
}

static void printType18(lazybiosCTX_t* ctx) {
	printf("=== 32-BIT MEMORY ERROR INFORMATION ===\n");

	if (!ctx->Type18) ctx->Type18 = lazybiosGetType18(ctx->Type18, &ctx->type18_count, ctx->DMIData);

	if (ctx->Type18 && ctx->type18_count > 0) {
		for (size_t i = 0; i < ctx->type18_count; i++) {
			lazybiosType18_t* type18 = &ctx->Type18[i];

			if (ctx->type18_count > 1) {
				printf("--- Memory Error Information %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type18, error_type) != LAZYBIOS_FIELD_PRESENT) {
				printf("Error Type: Not Present\n");
			} else {
				printf("Error Type: %s\n", lazybiosType18ErrorTypeStr(type18->error_type));
			}

			if (LAZYBIOS_FIELD_STATUS(type18, error_granularity) != LAZYBIOS_FIELD_PRESENT) {
				printf("Error Granularity: Not Present\n");
			} else {
				printf("Error Granularity: %s\n",
					lazybiosType18ErrorGranularityStr(type18->error_granularity));
			}

			if (LAZYBIOS_FIELD_STATUS(type18, error_operation) != LAZYBIOS_FIELD_PRESENT) {
				printf("Error Operation: Not Present\n");
			} else {
				printf("Error Operation: %s\n", lazybiosType18ErrorOperationStr(type18->error_operation));
			}

			if (LAZYBIOS_FIELD_STATUS(type18, vendor_syndrome) != LAZYBIOS_FIELD_PRESENT) {
				printf("Vendor Syndrome: Not Present\n");
			} else if (type18->vendor_syndrome == 0) {
				printf("Vendor Syndrome: Unknown\n");
			} else {
				printf("Vendor Syndrome: 0x%08X\n", type18->vendor_syndrome);
			}

			if (LAZYBIOS_FIELD_STATUS(type18, memory_array_error_address) != LAZYBIOS_FIELD_PRESENT) {
				printf("Memory Array Error Address: Not Present\n");
			} else if (type18->memory_array_error_address == 0x80000000U) {
				printf("Memory Array Error Address: Unknown\n");
			} else {
				printf("Memory Array Error Address: 0x%08X\n", type18->memory_array_error_address);
			}

			if (LAZYBIOS_FIELD_STATUS(type18, device_error_address) != LAZYBIOS_FIELD_PRESENT) {
				printf("Device Error Address: Not Present\n");
			} else if (type18->device_error_address == 0x80000000U) {
				printf("Device Error Address: Unknown\n");
			} else {
				printf("Device Error Address: 0x%08X\n", type18->device_error_address);
			}

			if (LAZYBIOS_FIELD_STATUS(type18, error_resolution) != LAZYBIOS_FIELD_PRESENT) {
				printf("Error Resolution: Not Present\n");
			} else if (type18->error_resolution == 0x80000000U) {
				printf("Error Resolution: Unknown\n");
			} else {
				printf("Error Resolution: %u bytes\n", type18->error_resolution);
			}

			printf("\n");
		}
	} else {
		printf("Failed to get 32-Bit Memory Error information\n\n");
	}
}

static void printType19(lazybiosCTX_t* ctx) {
	printf("=== MEMORY ARRAY MAPPED ADDRESS ===\n");

	if (!ctx->Type19) ctx->Type19 = lazybiosGetType19(ctx->Type19, &ctx->type19_count, ctx->DMIData);

	if (ctx->Type19 && ctx->type19_count > 0) {
		for (size_t i = 0; i < ctx->type19_count; i++) {
			lazybiosType19_t* type19 = &ctx->Type19[i];

			if (ctx->type19_count > 1) {
				printf("--- Memory Array Mapped Address %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type19, starting_address) != LAZYBIOS_FIELD_PRESENT) {
				printf("Starting Address: Not Present\n");
			} else if (type19->starting_address == 0xFFFFFFFFU &&
				LAZYBIOS_FIELD_STATUS(type19, extended_starting_address) != LAZYBIOS_FIELD_PRESENT) {
				printf("Starting Address: Not Present\n");
			} else {
				printf("Starting Address: 0x%016llX\n",
					(unsigned long long)lazybiosType19StartingAddressBytes(
						type19->starting_address, type19->extended_starting_address));
			}

			if (LAZYBIOS_FIELD_STATUS(type19, ending_address) != LAZYBIOS_FIELD_PRESENT) {
				printf("Ending Address: Not Present\n");
			} else if (type19->ending_address == 0xFFFFFFFFU &&
				LAZYBIOS_FIELD_STATUS(type19, extended_ending_address) != LAZYBIOS_FIELD_PRESENT) {
				printf("Ending Address: Not Present\n");
			} else {
				printf("Ending Address: 0x%016llX\n",
					(unsigned long long)lazybiosType19EndingAddressBytes(
						type19->ending_address, type19->extended_ending_address));
			}

			if (LAZYBIOS_FIELD_STATUS(type19, memory_array_handle) != LAZYBIOS_FIELD_PRESENT) {
				printf("Memory Array Handle: Not Present\n");
			} else {
				printf("Memory Array Handle: 0x%04hX\n", type19->memory_array_handle);
			}

			if (LAZYBIOS_FIELD_STATUS(type19, partition_width) != LAZYBIOS_FIELD_PRESENT) {
				printf("Partition Width: Not Present\n");
			} else {
				printf("Partition Width: %hhu\n", type19->partition_width);
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 7)) {
				if (LAZYBIOS_FIELD_STATUS(type19, extended_starting_address) != LAZYBIOS_FIELD_PRESENT) {
					printf("Extended Starting Address: Not Present\n");
				} else if (type19->starting_address != 0xFFFFFFFFU) {
					printf("Extended Starting Address: Not Used\n");
				} else {
					printf("Extended Starting Address: 0x%016llX\n",
						(unsigned long long)type19->extended_starting_address);
				}

				if (LAZYBIOS_FIELD_STATUS(type19, extended_ending_address) != LAZYBIOS_FIELD_PRESENT) {
					printf("Extended Ending Address: Not Present\n");
				} else if (type19->ending_address != 0xFFFFFFFFU) {
					printf("Extended Ending Address: Not Used\n");
				} else {
					printf("Extended Ending Address: 0x%016llX\n",
						(unsigned long long)type19->extended_ending_address);
				}
			} else if (!compact_output) {
				printf("Extended Starting Address: [SMBIOS 2.7 required]\n");
				printf("Extended Ending Address: [SMBIOS 2.7 required]\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Memory Array Mapped Address information\n\n");
	}
}

static void printType20(lazybiosCTX_t* ctx) {
	printf("=== MEMORY DEVICE MAPPED ADDRESS ===\n");

	if (!ctx->Type20) ctx->Type20 = lazybiosGetType20(ctx->Type20, &ctx->type20_count, ctx->DMIData);

	if (ctx->Type20 && ctx->type20_count > 0) {
		for (size_t i = 0; i < ctx->type20_count; i++) {
			lazybiosType20_t* type20 = &ctx->Type20[i];

			if (ctx->type20_count > 1) {
				printf("--- Memory Device Mapped Address %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type20, starting_address) != LAZYBIOS_FIELD_PRESENT) {
				printf("Starting Address: Not Present\n");
			} else if (type20->starting_address == 0xFFFFFFFFU &&
				LAZYBIOS_FIELD_STATUS(type20, extended_starting_address) != LAZYBIOS_FIELD_PRESENT) {
				printf("Starting Address: Not Present\n");
			} else {
				printf("Starting Address: 0x%016llX\n",
					(unsigned long long)lazybiosType20StartingAddressBytes(
						type20->starting_address, type20->extended_starting_address));
			}

			if (LAZYBIOS_FIELD_STATUS(type20, ending_address) != LAZYBIOS_FIELD_PRESENT) {
				printf("Ending Address: Not Present\n");
			} else if (type20->ending_address == 0xFFFFFFFFU &&
				LAZYBIOS_FIELD_STATUS(type20, extended_ending_address) != LAZYBIOS_FIELD_PRESENT) {
				printf("Ending Address: Not Present\n");
			} else {
				printf("Ending Address: 0x%016llX\n",
					(unsigned long long)lazybiosType20EndingAddressBytes(
						type20->ending_address, type20->extended_ending_address));
			}

			if (LAZYBIOS_FIELD_STATUS(type20, memory_device_handle) != LAZYBIOS_FIELD_PRESENT) {
				printf("Memory Device Handle: Not Present\n");
			} else {
				printf("Memory Device Handle: 0x%04hX\n", type20->memory_device_handle);
			}

			if (LAZYBIOS_FIELD_STATUS(type20, memory_array_mapped_address_handle) != LAZYBIOS_FIELD_PRESENT) {
				printf("Memory Array Mapped Address Handle: Not Present\n");
			} else {
				printf("Memory Array Mapped Address Handle: 0x%04hX\n",
					type20->memory_array_mapped_address_handle);
			}

			if (LAZYBIOS_FIELD_STATUS(type20, partition_row_position) != LAZYBIOS_FIELD_PRESENT) {
				printf("Partition Row Position: Not Present\n");
			} else if (type20->partition_row_position == 0xFF) {
				printf("Partition Row Position: Unknown\n");
			} else if (type20->partition_row_position == 0) {
				printf("Partition Row Position: Reserved\n");
			} else {
				printf("Partition Row Position: %hhu\n", type20->partition_row_position);
			}

			if (LAZYBIOS_FIELD_STATUS(type20, interleave_position) != LAZYBIOS_FIELD_PRESENT) {
				printf("Interleave Position: Not Present\n");
			} else if (type20->interleave_position == 0xFF) {
				printf("Interleave Position: Unknown\n");
			} else if (type20->interleave_position == 0) {
				printf("Interleave Position: Non-interleaved\n");
			} else {
				printf("Interleave Position: %hhu\n", type20->interleave_position);
			}

			if (LAZYBIOS_FIELD_STATUS(type20, interleaved_data_depth) != LAZYBIOS_FIELD_PRESENT) {
				printf("Interleaved Data Depth: Not Present\n");
			} else if (type20->interleaved_data_depth == 0xFF) {
				printf("Interleaved Data Depth: Unknown\n");
			} else if (type20->interleaved_data_depth == 0) {
				printf("Interleaved Data Depth: Not Interleaved\n");
			} else {
				printf("Interleaved Data Depth: %hhu rows\n", type20->interleaved_data_depth);
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 7)) {
				if (LAZYBIOS_FIELD_STATUS(type20, extended_starting_address) != LAZYBIOS_FIELD_PRESENT) {
					printf("Extended Starting Address: Not Present\n");
				} else if (type20->starting_address != 0xFFFFFFFFU) {
					printf("Extended Starting Address: Not Used\n");
				} else {
					printf("Extended Starting Address: 0x%016llX\n",
						(unsigned long long)type20->extended_starting_address);
				}

				if (LAZYBIOS_FIELD_STATUS(type20, extended_ending_address) != LAZYBIOS_FIELD_PRESENT) {
					printf("Extended Ending Address: Not Present\n");
				} else if (type20->ending_address != 0xFFFFFFFFU) {
					printf("Extended Ending Address: Not Used\n");
				} else {
					printf("Extended Ending Address: 0x%016llX\n",
						(unsigned long long)type20->extended_ending_address);
				}
			} else if (!compact_output) {
				printf("Extended Starting Address: [SMBIOS 2.7 required]\n");
				printf("Extended Ending Address: [SMBIOS 2.7 required]\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Memory Device Mapped Address information\n\n");
	}
}

static void printType21(lazybiosCTX_t* ctx) {
	printf("=== BUILT-IN POINTING DEVICE ===\n");

	if (!ctx->Type21) ctx->Type21 = lazybiosGetType21(ctx->Type21, &ctx->type21_count, ctx->DMIData);

	if (ctx->Type21 && ctx->type21_count > 0) {
		for (size_t i = 0; i < ctx->type21_count; i++) {
			lazybiosType21_t* type21 = &ctx->Type21[i];

			if (ctx->type21_count > 1) {
				printf("--- Built-in Pointing Device %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type21, pointing_device_type) != LAZYBIOS_FIELD_PRESENT) {
				printf("Type: Not Present\n");
			} else {
				printf("Type: %s\n", lazybiosType21PointingDeviceTypeStr(type21->pointing_device_type));
			}

			if (LAZYBIOS_FIELD_STATUS(type21, interface) != LAZYBIOS_FIELD_PRESENT) {
				printf("Interface: Not Present\n");
			} else {
				printf("Interface: %s\n", lazybiosType21InterfaceStr(type21->interface));
			}

			if (LAZYBIOS_FIELD_STATUS(type21, number_of_buttons) != LAZYBIOS_FIELD_PRESENT) {
				printf("Number of Buttons: Not Present\n");
			} else {
				printf("Number of Buttons: %hhu\n", type21->number_of_buttons);
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Built-in Pointing Device information\n\n");
	}
}

static void printType22(lazybiosCTX_t* ctx) {
	printf("=== PORTABLE BATTERY ===\n");

	if (!ctx->Type22) ctx->Type22 = lazybiosGetType22(ctx->Type22, &ctx->type22_count, ctx->DMIData);

	if (ctx->Type22 && ctx->type22_count > 0) {
		for (size_t i = 0; i < ctx->type22_count; i++) {
			lazybiosType22_t* type22 = &ctx->Type22[i];

			if (ctx->type22_count > 1) {
				printf("--- Portable Battery %zu ---\n", i + 1);
			}

			printf("Location: %s\n", type22->location ? type22->location : "Not Present");
			printf("Manufacturer: %s\n", type22->manufacturer ? type22->manufacturer : "Not Present");
			printf("Manufacture Date: %s\n", type22->manufacture_date ? type22->manufacture_date : "Not Present");
			printf("Serial Number: %s\n", type22->serial_number ? type22->serial_number : "Not Present");
			printf("Device Name: %s\n", type22->device_name ? type22->device_name : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type22, device_chemistry) != LAZYBIOS_FIELD_PRESENT) {
				printf("Device Chemistry: Not Present\n");
			} else {
				printf("Device Chemistry: %s\n",
					lazybiosType22DeviceChemistryStr(type22->device_chemistry));
			}

			if (LAZYBIOS_FIELD_STATUS(type22, design_capacity) != LAZYBIOS_FIELD_PRESENT) {
				printf("Design Capacity: Not Present\n");
			} else if (type22->design_capacity == 0) {
				printf("Design Capacity: Unknown\n");
			} else if (lazybiosIsVersionPlus(ctx->DMIData, 2, 2) &&
				LAZYBIOS_FIELD_STATUS(type22, design_capacity_multiplier) != LAZYBIOS_FIELD_PRESENT) {
				printf("Design Capacity: %hu mWh (raw; multiplier not present)\n", type22->design_capacity);
			} else {
				uint8_t multiplier = lazybiosIsVersionPlus(ctx->DMIData, 2, 2) ?
					type22->design_capacity_multiplier : 1;
				printf("Design Capacity: %u mWh\n",
					lazybiosType22DesignCapacityMWh(type22->design_capacity, multiplier));
			}

			if (LAZYBIOS_FIELD_STATUS(type22, design_voltage) != LAZYBIOS_FIELD_PRESENT) {
				printf("Design Voltage: Not Present\n");
			} else if (type22->design_voltage == 0) {
				printf("Design Voltage: Unknown\n");
			} else {
				printf("Design Voltage: %hu mV\n", type22->design_voltage);
			}

			printf("SBDS Version Number: %s\n",
				type22->sbds_version_number ? type22->sbds_version_number : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type22, maximum_error) != LAZYBIOS_FIELD_PRESENT) {
				printf("Maximum Error in Battery Data: Not Present\n");
			} else if (type22->maximum_error == 0xFF) {
				printf("Maximum Error in Battery Data: Unknown\n");
			} else {
				printf("Maximum Error in Battery Data: %hhu%%\n", type22->maximum_error);
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 2)) {
				if (LAZYBIOS_FIELD_STATUS(type22, sbds_serial_number) != LAZYBIOS_FIELD_PRESENT) {
					printf("SBDS Serial Number: Not Present\n");
				} else if (type22->serial_number) {
					printf("SBDS Serial Number: Not Used\n");
				} else {
					printf("SBDS Serial Number: 0x%04hX\n", type22->sbds_serial_number);
				}

				if (LAZYBIOS_FIELD_STATUS(type22, sbds_manufacture_date) != LAZYBIOS_FIELD_PRESENT) {
					printf("SBDS Manufacture Date: Not Present\n");
				} else if (type22->manufacture_date) {
					printf("SBDS Manufacture Date: Not Used\n");
				} else {
					char buf[LAZYBIOS_DECODER_BUF_SIZE];
					lazybiosType22SBDSManufactureDateStr(type22->sbds_manufacture_date, buf, sizeof(buf));
					printf("SBDS Manufacture Date: %s\n", buf);
				}

				if (LAZYBIOS_FIELD_STATUS(type22, sbds_device_chemistry) != LAZYBIOS_FIELD_PRESENT) {
					printf("SBDS Device Chemistry: Not Present\n");
				} else if (type22->device_chemistry != 0x02) {
					printf("SBDS Device Chemistry: Not Used\n");
				} else {
					printf("SBDS Device Chemistry: %s\n",
						type22->sbds_device_chemistry ? type22->sbds_device_chemistry : "Not Specified");
				}

				if (LAZYBIOS_FIELD_STATUS(type22, design_capacity_multiplier) != LAZYBIOS_FIELD_PRESENT) {
					printf("Design Capacity Multiplier: Not Present\n");
				} else {
					printf("Design Capacity Multiplier: %hhu\n", type22->design_capacity_multiplier);
				}

				if (LAZYBIOS_FIELD_STATUS(type22, oem_specific) != LAZYBIOS_FIELD_PRESENT) {
					printf("OEM-specific Information: Not Present\n");
				} else {
					printf("OEM-specific Information: 0x%08X\n", type22->oem_specific);
				}
			} else if (!compact_output) {
				printf("SBDS Serial Number: [SMBIOS 2.2 required]\n");
				printf("SBDS Manufacture Date: [SMBIOS 2.2 required]\n");
				printf("SBDS Device Chemistry: [SMBIOS 2.2 required]\n");
				printf("Design Capacity Multiplier: [SMBIOS 2.2 required]\n");
				printf("OEM-specific Information: [SMBIOS 2.2 required]\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Portable Battery information\n\n");
	}
}

static void printType23(lazybiosCTX_t* ctx) {
	printf("=== SYSTEM RESET ===\n");

	if (!ctx->Type23) ctx->Type23 = lazybiosGetType23(ctx->Type23, &ctx->type23_count, ctx->DMIData);

	if (ctx->Type23 && ctx->type23_count > 0) {
		for (size_t i = 0; i < ctx->type23_count; i++) {
			lazybiosType23_t* type23 = &ctx->Type23[i];

			if (ctx->type23_count > 1) {
				printf("--- System Reset %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type23, capabilities) != LAZYBIOS_FIELD_PRESENT) {
				printf("Capabilities: Not Present\n");
			} else {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];
				lazybiosType23CapabilitiesStr(type23->capabilities, buf, sizeof(buf));
				printf("Capabilities: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type23, reset_count) != LAZYBIOS_FIELD_PRESENT) {
				printf("Reset Count: Not Present\n");
			} else if (type23->reset_count == 0xFFFF) {
				printf("Reset Count: Unknown\n");
			} else {
				printf("Reset Count: %hu\n", type23->reset_count);
			}

			if (LAZYBIOS_FIELD_STATUS(type23, reset_limit) != LAZYBIOS_FIELD_PRESENT) {
				printf("Reset Limit: Not Present\n");
			} else if (type23->reset_limit == 0xFFFF) {
				printf("Reset Limit: Unknown\n");
			} else {
				printf("Reset Limit: %hu\n", type23->reset_limit);
			}

			if (LAZYBIOS_FIELD_STATUS(type23, timer_interval) != LAZYBIOS_FIELD_PRESENT) {
				printf("Timer Interval: Not Present\n");
			} else if (type23->timer_interval == 0xFFFF) {
				printf("Timer Interval: Unknown\n");
			} else {
				printf("Timer Interval: %hu minutes\n", type23->timer_interval);
			}

			if (LAZYBIOS_FIELD_STATUS(type23, timeout) != LAZYBIOS_FIELD_PRESENT) {
				printf("Timeout: Not Present\n");
			} else if (type23->timeout == 0xFFFF) {
				printf("Timeout: Unknown\n");
			} else {
				printf("Timeout: %hu minutes\n", type23->timeout);
			}

			printf("\n");
		}
	} else {
		printf("Failed to get System Reset information\n\n");
	}
}

static void printType24(lazybiosCTX_t* ctx) {
	printf("=== HARDWARE SECURITY ===\n");

	if (!ctx->Type24) ctx->Type24 = lazybiosGetType24(ctx->Type24, &ctx->type24_count, ctx->DMIData);

	if (ctx->Type24 && ctx->type24_count > 0) {
		for (size_t i = 0; i < ctx->type24_count; i++) {
			lazybiosType24_t* type24 = &ctx->Type24[i];

			if (ctx->type24_count > 1) {
				printf("--- Hardware Security %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type24, hardware_security_settings) != LAZYBIOS_FIELD_PRESENT) {
				printf("Power-on Password Status: Not Present\n");
				printf("Keyboard Password Status: Not Present\n");
				printf("Administrator Password Status: Not Present\n");
				printf("Front Panel Reset Status: Not Present\n");
			} else {
				printf("Power-on Password Status: %s\n",
					lazybiosType24PowerOnPasswordStatusStr(type24->hardware_security_settings));
				printf("Keyboard Password Status: %s\n",
					lazybiosType24KeyboardPasswordStatusStr(type24->hardware_security_settings));
				printf("Administrator Password Status: %s\n",
					lazybiosType24AdministratorPasswordStatusStr(type24->hardware_security_settings));
				printf("Front Panel Reset Status: %s\n",
					lazybiosType24FrontPanelResetStatusStr(type24->hardware_security_settings));
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Hardware Security information\n\n");
	}
}

static void printType25(lazybiosCTX_t* ctx) {
	printf("=== SYSTEM POWER CONTROLS ===\n");

	if (!ctx->Type25) ctx->Type25 = lazybiosGetType25(ctx->Type25, &ctx->type25_count, ctx->DMIData);

	if (ctx->Type25 && ctx->type25_count > 0) {
		for (size_t i = 0; i < ctx->type25_count; i++) {
			lazybiosType25_t* type25 = &ctx->Type25[i];
			char buf[LAZYBIOS_DECODER_BUF_SIZE];

			if (ctx->type25_count > 1) {
				printf("--- System Power Controls %zu ---\n", i + 1);
			}

			lazybiosType25NextScheduledPowerOnStr(type25, buf, sizeof(buf));
			printf("Next Scheduled Power-on: %s\n\n", buf);
		}
	} else {
		printf("Failed to get System Power Controls information\n\n");
	}
}

static void printType26(lazybiosCTX_t* ctx) {
	printf("=== VOLTAGE PROBE ===\n");

	if (!ctx->Type26) ctx->Type26 = lazybiosGetType26(ctx->Type26, &ctx->type26_count, ctx->DMIData);

	if (ctx->Type26 && ctx->type26_count > 0) {
		for (size_t i = 0; i < ctx->type26_count; i++) {
			lazybiosType26_t* type26 = &ctx->Type26[i];

			if (ctx->type26_count > 1) {
				printf("--- Voltage Probe %zu ---\n", i + 1);
			}

			printf("Description: %s\n", type26->description ? type26->description : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type26, location_and_status) != LAZYBIOS_FIELD_PRESENT) {
				printf("Location: Not Present\n");
				printf("Status: Not Present\n");
			} else {
				printf("Location: %s\n", lazybiosType26LocationStr(type26->location_and_status));
				printf("Status: %s\n", lazybiosType26StatusStr(type26->location_and_status));
			}

			if (LAZYBIOS_FIELD_STATUS(type26, maximum_value) != LAZYBIOS_FIELD_PRESENT) {
				printf("Maximum Value: Not Present\n");
			} else if (type26->maximum_value == 0x8000) {
				printf("Maximum Value: Unknown\n");
			} else {
				printf("Maximum Value: %hu mV\n", type26->maximum_value);
			}

			if (LAZYBIOS_FIELD_STATUS(type26, minimum_value) != LAZYBIOS_FIELD_PRESENT) {
				printf("Minimum Value: Not Present\n");
			} else if (type26->minimum_value == 0x8000) {
				printf("Minimum Value: Unknown\n");
			} else {
				printf("Minimum Value: %hu mV\n", type26->minimum_value);
			}

			if (LAZYBIOS_FIELD_STATUS(type26, resolution) != LAZYBIOS_FIELD_PRESENT) {
				printf("Resolution: Not Present\n");
			} else if (type26->resolution == 0x8000) {
				printf("Resolution: Unknown\n");
			} else {
				printf("Resolution: %hu.%hu mV\n", type26->resolution / 10, type26->resolution % 10);
			}

			if (LAZYBIOS_FIELD_STATUS(type26, tolerance) != LAZYBIOS_FIELD_PRESENT) {
				printf("Tolerance: Not Present\n");
			} else if (type26->tolerance == 0x8000) {
				printf("Tolerance: Unknown\n");
			} else {
				printf("Tolerance: +/- %hu mV\n", type26->tolerance);
			}

			if (LAZYBIOS_FIELD_STATUS(type26, accuracy) != LAZYBIOS_FIELD_PRESENT) {
				printf("Accuracy: Not Present\n");
			} else if (type26->accuracy == 0x8000) {
				printf("Accuracy: Unknown\n");
			} else {
				printf("Accuracy: +/- %hu.%02hu%%\n", type26->accuracy / 100, type26->accuracy % 100);
			}

			if (LAZYBIOS_FIELD_STATUS(type26, oem_defined) != LAZYBIOS_FIELD_PRESENT) {
				printf("OEM-defined Information: Not Present\n");
			} else {
				printf("OEM-defined Information: 0x%08X\n", type26->oem_defined);
			}

			if (LAZYBIOS_FIELD_STATUS(type26, nominal_value) != LAZYBIOS_FIELD_PRESENT) {
				printf("Nominal Value: Not Present\n");
			} else if (type26->nominal_value == 0x8000) {
				printf("Nominal Value: Unknown\n");
			} else {
				printf("Nominal Value: %hu mV\n", type26->nominal_value);
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Voltage Probe information\n\n");
	}
}

static void printType27(lazybiosCTX_t* ctx) {
	printf("=== COOLING DEVICE ===\n");

	if (!ctx->Type27) ctx->Type27 = lazybiosGetType27(ctx->Type27, &ctx->type27_count, ctx->DMIData);

	if (ctx->Type27 && ctx->type27_count > 0) {
		for (size_t i = 0; i < ctx->type27_count; i++) {
			lazybiosType27_t* type27 = &ctx->Type27[i];

			if (ctx->type27_count > 1) {
				printf("--- Cooling Device %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type27, temperature_probe_handle) != LAZYBIOS_FIELD_PRESENT) {
				printf("Temperature Probe Handle: Not Present\n");
			} else {
				printf("Temperature Probe Handle: 0x%04hX\n", type27->temperature_probe_handle);
			}

			if (LAZYBIOS_FIELD_STATUS(type27, device_type_and_status) != LAZYBIOS_FIELD_PRESENT) {
				printf("Device Type: Not Present\n");
				printf("Status: Not Present\n");
			} else {
				printf("Device Type: %s\n", lazybiosType27DeviceTypeStr(type27->device_type_and_status));
				printf("Status: %s\n", lazybiosType27StatusStr(type27->device_type_and_status));
			}

			if (LAZYBIOS_FIELD_STATUS(type27, cooling_unit_group) != LAZYBIOS_FIELD_PRESENT) {
				printf("Cooling Unit Group: Not Present\n");
			} else if (type27->cooling_unit_group == 0) {
				printf("Cooling Unit Group: None\n");
			} else {
				printf("Cooling Unit Group: %hhu\n", type27->cooling_unit_group);
			}

			if (LAZYBIOS_FIELD_STATUS(type27, oem_defined) != LAZYBIOS_FIELD_PRESENT) {
				printf("OEM-defined Information: Not Present\n");
			} else {
				printf("OEM-defined Information: 0x%08X\n", type27->oem_defined);
			}

			if (LAZYBIOS_FIELD_STATUS(type27, nominal_speed) != LAZYBIOS_FIELD_PRESENT) {
				printf("Nominal Speed: Not Present\n");
			} else if (type27->nominal_speed == 0x8000) {
				printf("Nominal Speed: Unknown or Non-rotating\n");
			} else {
				printf("Nominal Speed: %hu rpm\n", type27->nominal_speed);
			}

			if (lazybiosIsVersionPlus(ctx->DMIData, 2, 7)) {
				printf("Description: %s\n", type27->description ? type27->description : "Not Present");
			} else if (!compact_output) {
				printf("Description: [SMBIOS 2.7 required]\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Cooling Device information\n\n");
	}
}

static void printType28(lazybiosCTX_t* ctx) {
	printf("=== TEMPERATURE PROBE ===\n");

	if (!ctx->Type28) ctx->Type28 = lazybiosGetType28(ctx->Type28, &ctx->type28_count, ctx->DMIData);

	if (ctx->Type28 && ctx->type28_count > 0) {
		for (size_t i = 0; i < ctx->type28_count; i++) {
			lazybiosType28_t* type28 = &ctx->Type28[i];

			if (ctx->type28_count > 1) {
				printf("--- Temperature Probe %zu ---\n", i + 1);
			}

			printf("Description: %s\n", type28->description ? type28->description : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type28, location_and_status) != LAZYBIOS_FIELD_PRESENT) {
				printf("Location: Not Present\n");
				printf("Status: Not Present\n");
			} else {
				printf("Location: %s\n", lazybiosType28LocationStr(type28->location_and_status));
				printf("Status: %s\n", lazybiosType28StatusStr(type28->location_and_status));
			}

			if (LAZYBIOS_FIELD_STATUS(type28, maximum_value) != LAZYBIOS_FIELD_PRESENT) {
				printf("Maximum Value: Not Present\n");
			} else if (type28->maximum_value == 0x8000) {
				printf("Maximum Value: Unknown\n");
			} else {
				printf("Maximum Value: %hu.%hu degrees C\n", type28->maximum_value / 10, type28->maximum_value % 10);
			}

			if (LAZYBIOS_FIELD_STATUS(type28, minimum_value) != LAZYBIOS_FIELD_PRESENT) {
				printf("Minimum Value: Not Present\n");
			} else if (type28->minimum_value == 0x8000) {
				printf("Minimum Value: Unknown\n");
			} else {
				printf("Minimum Value: %hu.%hu degrees C\n", type28->minimum_value / 10, type28->minimum_value % 10);
			}

			if (LAZYBIOS_FIELD_STATUS(type28, resolution) != LAZYBIOS_FIELD_PRESENT) {
				printf("Resolution: Not Present\n");
			} else if (type28->resolution == 0x8000) {
				printf("Resolution: Unknown\n");
			} else {
				printf("Resolution: %hu.%03hu degrees C\n", type28->resolution / 1000, type28->resolution % 1000);
			}

			if (LAZYBIOS_FIELD_STATUS(type28, tolerance) != LAZYBIOS_FIELD_PRESENT) {
				printf("Tolerance: Not Present\n");
			} else if (type28->tolerance == 0x8000) {
				printf("Tolerance: Unknown\n");
			} else {
				printf("Tolerance: +/- %hu.%hu degrees C\n", type28->tolerance / 10, type28->tolerance % 10);
			}

			if (LAZYBIOS_FIELD_STATUS(type28, accuracy) != LAZYBIOS_FIELD_PRESENT) {
				printf("Accuracy: Not Present\n");
			} else if (type28->accuracy == 0x8000) {
				printf("Accuracy: Unknown\n");
			} else {
				printf("Accuracy: +/- %hu.%02hu%%\n", type28->accuracy / 100, type28->accuracy % 100);
			}

			if (LAZYBIOS_FIELD_STATUS(type28, oem_defined) != LAZYBIOS_FIELD_PRESENT) {
				printf("OEM-defined Information: Not Present\n");
			} else {
				printf("OEM-defined Information: 0x%08X\n", type28->oem_defined);
			}

			if (LAZYBIOS_FIELD_STATUS(type28, nominal_value) != LAZYBIOS_FIELD_PRESENT) {
				printf("Nominal Value: Not Present\n");
			} else if (type28->nominal_value == 0x8000) {
				printf("Nominal Value: Unknown\n");
			} else {
				printf("Nominal Value: %hu.%hu degrees C\n", type28->nominal_value / 10, type28->nominal_value % 10);
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Temperature Probe information\n\n");
	}
}

static void printType29(lazybiosCTX_t* ctx) {
	printf("=== ELECTRICAL CURRENT PROBE ===\n");

	if (!ctx->Type29) ctx->Type29 = lazybiosGetType29(ctx->Type29, &ctx->type29_count, ctx->DMIData);

	if (ctx->Type29 && ctx->type29_count > 0) {
		for (size_t i = 0; i < ctx->type29_count; i++) {
			lazybiosType29_t* type29 = &ctx->Type29[i];

			if (ctx->type29_count > 1) {
				printf("--- Electrical Current Probe %zu ---\n", i + 1);
			}

			printf("Description: %s\n", type29->description ? type29->description : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type29, location_and_status) != LAZYBIOS_FIELD_PRESENT) {
				printf("Location: Not Present\n");
				printf("Status: Not Present\n");
			} else {
				printf("Location: %s\n", lazybiosType29LocationStr(type29->location_and_status));
				printf("Status: %s\n", lazybiosType29StatusStr(type29->location_and_status));
			}

			if (LAZYBIOS_FIELD_STATUS(type29, maximum_value) != LAZYBIOS_FIELD_PRESENT) {
				printf("Maximum Value: Not Present\n");
			} else if (type29->maximum_value == 0x8000) {
				printf("Maximum Value: Unknown\n");
			} else {
				printf("Maximum Value: %hu mA\n", type29->maximum_value);
			}

			if (LAZYBIOS_FIELD_STATUS(type29, minimum_value) != LAZYBIOS_FIELD_PRESENT) {
				printf("Minimum Value: Not Present\n");
			} else if (type29->minimum_value == 0x8000) {
				printf("Minimum Value: Unknown\n");
			} else {
				printf("Minimum Value: %hu mA\n", type29->minimum_value);
			}

			if (LAZYBIOS_FIELD_STATUS(type29, resolution) != LAZYBIOS_FIELD_PRESENT) {
				printf("Resolution: Not Present\n");
			} else if (type29->resolution == 0x8000) {
				printf("Resolution: Unknown\n");
			} else {
				printf("Resolution: %hu.%hu mA\n", type29->resolution / 10, type29->resolution % 10);
			}

			if (LAZYBIOS_FIELD_STATUS(type29, tolerance) != LAZYBIOS_FIELD_PRESENT) {
				printf("Tolerance: Not Present\n");
			} else if (type29->tolerance == 0x8000) {
				printf("Tolerance: Unknown\n");
			} else {
				printf("Tolerance: +/- %hu mA\n", type29->tolerance);
			}

			if (LAZYBIOS_FIELD_STATUS(type29, accuracy) != LAZYBIOS_FIELD_PRESENT) {
				printf("Accuracy: Not Present\n");
			} else if (type29->accuracy == 0x8000) {
				printf("Accuracy: Unknown\n");
			} else {
				printf("Accuracy: +/- %hu.%02hu%%\n", type29->accuracy / 100, type29->accuracy % 100);
			}

			if (LAZYBIOS_FIELD_STATUS(type29, oem_defined) != LAZYBIOS_FIELD_PRESENT) {
				printf("OEM-defined Information: Not Present\n");
			} else {
				printf("OEM-defined Information: 0x%08X\n", type29->oem_defined);
			}

			if (LAZYBIOS_FIELD_STATUS(type29, nominal_value) != LAZYBIOS_FIELD_PRESENT) {
				printf("Nominal Value: Not Present\n");
			} else if (type29->nominal_value == 0x8000) {
				printf("Nominal Value: Unknown\n");
			} else {
				printf("Nominal Value: %hu mA\n", type29->nominal_value);
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Electrical Current Probe information\n\n");
	}
}

static void printType30(lazybiosCTX_t* ctx) {
	printf("=== OUT-OF-BAND REMOTE ACCESS ===\n");

	if (!ctx->Type30) ctx->Type30 = lazybiosGetType30(ctx->Type30, &ctx->type30_count, ctx->DMIData);

	if (ctx->Type30 && ctx->type30_count > 0) {
		for (size_t i = 0; i < ctx->type30_count; i++) {
			lazybiosType30_t* type30 = &ctx->Type30[i];

			if (ctx->type30_count > 1) {
				printf("--- Out-of-Band Remote Access %zu ---\n", i + 1);
			}

			printf("Manufacturer Name: %s\n",
				type30->manufacturer_name ? type30->manufacturer_name : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type30, connections) != LAZYBIOS_FIELD_PRESENT) {
				printf("Inbound Connection: Not Present\n");
				printf("Outbound Connection: Not Present\n");
			} else {
				printf("Inbound Connection: %s\n", lazybiosType30InboundConnectionStr(type30->connections));
				printf("Outbound Connection: %s\n", lazybiosType30OutboundConnectionStr(type30->connections));
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Out-of-Band Remote Access information\n\n");
	}
}

static void printType31(lazybiosCTX_t* ctx) {
	printf("=== BOOT INTEGRITY SERVICES ENTRY POINT ===\n");

	if (!ctx->Type31) ctx->Type31 = lazybiosGetType31(ctx->Type31, &ctx->type31_count, ctx->DMIData);

	if (ctx->Type31 && ctx->type31_count > 0) {
		for (size_t i = 0; i < ctx->type31_count; i++) {
			lazybiosType31_t* type31 = &ctx->Type31[i];

			if (ctx->type31_count > 1) {
				printf("--- Boot Integrity Services Entry Point %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type31, checksum) != LAZYBIOS_FIELD_PRESENT) {
				printf("Checksum: Not Present\n");
			} else {
				printf("Checksum: 0x%02X\n", type31->checksum);
			}

			if (LAZYBIOS_FIELD_STATUS(type31, checksum_valid) != LAZYBIOS_FIELD_PRESENT) {
				printf("Structure Checksum: Not Available\n");
			} else {
				printf("Structure Checksum: %s\n", type31->checksum_valid ? "Valid" : "Invalid");
			}

			if (LAZYBIOS_FIELD_STATUS(type31, bis_entry_point_16) != LAZYBIOS_FIELD_PRESENT) {
				printf("16-bit Entry Point: Not Present\n");
			} else {
				printf("16-bit Entry Point: 0x%08X\n", type31->bis_entry_point_16);
			}

			if (LAZYBIOS_FIELD_STATUS(type31, bis_entry_point_32) != LAZYBIOS_FIELD_PRESENT) {
				printf("32-bit Entry Point: Not Present\n");
			} else {
				printf("32-bit Entry Point: 0x%08X\n", type31->bis_entry_point_32);
			}

			if (LAZYBIOS_FIELD_STATUS(type31, reserved_1) != LAZYBIOS_FIELD_PRESENT ||
				LAZYBIOS_FIELD_STATUS(type31, reserved_2) != LAZYBIOS_FIELD_PRESENT ||
				LAZYBIOS_FIELD_STATUS(type31, reserved_3) != LAZYBIOS_FIELD_PRESENT ||
				LAZYBIOS_FIELD_STATUS(type31, reserved_4) != LAZYBIOS_FIELD_PRESENT) {
				printf("Reserved Fields: Not Present\n");
			} else {
				printf("Reserved Fields: 0x%02X, 0x%04hX, 0x%016llX, 0x%08X\n",
					type31->reserved_1,
					type31->reserved_2,
					(unsigned long long)type31->reserved_3,
					type31->reserved_4);
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Boot Integrity Services Entry Point information\n\n");
	}
}

static void printType32(lazybiosCTX_t* ctx) {
	printf("=== SYSTEM BOOT INFORMATION ===\n");

	if (!ctx->Type32) ctx->Type32 = lazybiosGetType32(ctx->Type32, &ctx->type32_count, ctx->DMIData);

	if (ctx->Type32 && ctx->type32_count > 0) {
		for (size_t i = 0; i < ctx->type32_count; i++) {
			lazybiosType32_t* type32 = &ctx->Type32[i];

			if (ctx->type32_count > 1) {
				printf("--- System Boot Information %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type32, reserved) != LAZYBIOS_FIELD_PRESENT) {
				printf("Reserved Bytes: Not Present\n");
			} else {
				printf("Reserved Bytes:");
				for (size_t j = 0; j < sizeof(type32->reserved); j++) {
					printf(" %02X", type32->reserved[j]);
				}
				printf("\n");
			}

			if (LAZYBIOS_FIELD_STATUS(type32, boot_status) != LAZYBIOS_FIELD_PRESENT) {
				printf("Boot Status: Not Present\n");
			} else {
				printf("Boot Status: %s (%hhu)\n",
					lazybiosType32BootStatusStr(type32->boot_status), type32->boot_status);
			}

			if (LAZYBIOS_FIELD_STATUS(type32, additional_data) != LAZYBIOS_FIELD_PRESENT) {
				printf("Additional Data: Not Present\n");
			} else if (type32->additional_data_size == 0) {
				printf("Additional Data: None\n");
			} else {
				printf("Additional Data:");
				for (size_t j = 0; j < type32->additional_data_size; j++) {
					printf(" %02X", type32->additional_data[j]);
				}
				printf("\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get System Boot Information\n\n");
	}
}

static void printType33(lazybiosCTX_t* ctx) {
	printf("=== 64-BIT MEMORY ERROR INFORMATION ===\n");

	if (!ctx->Type33) ctx->Type33 = lazybiosGetType33(ctx->Type33, &ctx->type33_count, ctx->DMIData);

	if (ctx->Type33 && ctx->type33_count > 0) {
		for (size_t i = 0; i < ctx->type33_count; i++) {
			lazybiosType33_t* type33 = &ctx->Type33[i];

			if (ctx->type33_count > 1) {
				printf("--- 64-Bit Memory Error Information %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type33, error_type) != LAZYBIOS_FIELD_PRESENT) {
				printf("Error Type: Not Present\n");
			} else {
				printf("Error Type: %s\n", lazybiosType33ErrorTypeStr(type33->error_type));
			}

			if (LAZYBIOS_FIELD_STATUS(type33, error_granularity) != LAZYBIOS_FIELD_PRESENT) {
				printf("Error Granularity: Not Present\n");
			} else {
				printf("Error Granularity: %s\n",
					lazybiosType33ErrorGranularityStr(type33->error_granularity));
			}

			if (LAZYBIOS_FIELD_STATUS(type33, error_operation) != LAZYBIOS_FIELD_PRESENT) {
				printf("Error Operation: Not Present\n");
			} else {
				printf("Error Operation: %s\n", lazybiosType33ErrorOperationStr(type33->error_operation));
			}

			if (LAZYBIOS_FIELD_STATUS(type33, vendor_syndrome) != LAZYBIOS_FIELD_PRESENT) {
				printf("Vendor Syndrome: Not Present\n");
			} else if (type33->vendor_syndrome == 0) {
				printf("Vendor Syndrome: Unknown\n");
			} else {
				printf("Vendor Syndrome: 0x%08X\n", type33->vendor_syndrome);
			}

			if (LAZYBIOS_FIELD_STATUS(type33, memory_array_error_address) != LAZYBIOS_FIELD_PRESENT) {
				printf("Memory Array Error Address: Not Present\n");
			} else if (type33->memory_array_error_address == 0x8000000000000000ULL) {
				printf("Memory Array Error Address: Unknown\n");
			} else {
				printf("Memory Array Error Address: 0x%016llX\n",
					(unsigned long long)type33->memory_array_error_address);
			}

			if (LAZYBIOS_FIELD_STATUS(type33, device_error_address) != LAZYBIOS_FIELD_PRESENT) {
				printf("Device Error Address: Not Present\n");
			} else if (type33->device_error_address == 0x8000000000000000ULL) {
				printf("Device Error Address: Unknown\n");
			} else {
				printf("Device Error Address: 0x%016llX\n",
					(unsigned long long)type33->device_error_address);
			}

			if (LAZYBIOS_FIELD_STATUS(type33, error_resolution) != LAZYBIOS_FIELD_PRESENT) {
				printf("Error Resolution: Not Present\n");
			} else if (type33->error_resolution == 0x80000000U) {
				printf("Error Resolution: Unknown\n");
			} else {
				printf("Error Resolution: %u bytes\n", type33->error_resolution);
			}

			printf("\n");
		}
	} else {
		printf("Failed to get 64-Bit Memory Error information\n\n");
	}
}

static void printType34(lazybiosCTX_t* ctx) {
	printf("=== MANAGEMENT DEVICE ===\n");

	if (!ctx->Type34) ctx->Type34 = lazybiosGetType34(ctx->Type34, &ctx->type34_count, ctx->DMIData);

	if (ctx->Type34 && ctx->type34_count > 0) {
		for (size_t i = 0; i < ctx->type34_count; i++) {
			lazybiosType34_t* type34 = &ctx->Type34[i];

			if (ctx->type34_count > 1) {
				printf("--- Management Device %zu ---\n", i + 1);
			}

			printf("Description: %s\n", type34->description ? type34->description : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type34, device_type) != LAZYBIOS_FIELD_PRESENT) {
				printf("Device Type: Not Present\n");
			} else {
				printf("Device Type: %s\n", lazybiosType34DeviceTypeStr(type34->device_type));
			}

			if (LAZYBIOS_FIELD_STATUS(type34, address) != LAZYBIOS_FIELD_PRESENT) {
				printf("Address: Not Present\n");
			} else {
				printf("Address: 0x%08X\n", type34->address);
			}

			if (LAZYBIOS_FIELD_STATUS(type34, address_type) != LAZYBIOS_FIELD_PRESENT) {
				printf("Address Type: Not Present\n");
			} else {
				printf("Address Type: %s\n", lazybiosType34AddressTypeStr(type34->address_type));
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Management Device information\n\n");
	}
}

static void printType35(lazybiosCTX_t* ctx) {
	printf("=== MANAGEMENT DEVICE COMPONENT ===\n");

	if (!ctx->Type35) ctx->Type35 = lazybiosGetType35(ctx->Type35, &ctx->type35_count, ctx->DMIData);

	if (ctx->Type35 && ctx->type35_count > 0) {
		for (size_t i = 0; i < ctx->type35_count; i++) {
			lazybiosType35_t* type35 = &ctx->Type35[i];

			if (ctx->type35_count > 1) {
				printf("--- Management Device Component %zu ---\n", i + 1);
			}

			printf("Description: %s\n", type35->description ? type35->description : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type35, management_device_handle) != LAZYBIOS_FIELD_PRESENT) {
				printf("Management Device Handle: Not Present\n");
			} else {
				printf("Management Device Handle: 0x%04hX\n", type35->management_device_handle);
			}

			if (LAZYBIOS_FIELD_STATUS(type35, component_handle) != LAZYBIOS_FIELD_PRESENT) {
				printf("Component Handle: Not Present\n");
			} else {
				printf("Component Handle: 0x%04hX\n", type35->component_handle);
			}

			if (LAZYBIOS_FIELD_STATUS(type35, threshold_handle) != LAZYBIOS_FIELD_PRESENT) {
				printf("Threshold Handle: Not Present\n");
			} else {
				printf("Threshold Handle: 0x%04hX\n", type35->threshold_handle);
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Management Device Component information\n\n");
	}
}

static void printType36(lazybiosCTX_t* ctx) {
	printf("=== MANAGEMENT DEVICE THRESHOLD DATA ===\n");

	if (!ctx->Type36) ctx->Type36 = lazybiosGetType36(ctx->Type36, &ctx->type36_count, ctx->DMIData);

	if (ctx->Type36 && ctx->type36_count > 0) {
		for (size_t i = 0; i < ctx->type36_count; i++) {
			lazybiosType36_t* type36 = &ctx->Type36[i];

			if (ctx->type36_count > 1) {
				printf("--- Management Device Threshold Data %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type36, lower_threshold_non_critical) != LAZYBIOS_FIELD_PRESENT) {
				printf("Lower Non-critical Threshold: Not Present\n");
			} else if (type36->lower_threshold_non_critical == 0x8000) {
				printf("Lower Non-critical Threshold: Unavailable\n");
			} else {
				printf("Lower Non-critical Threshold: %hu\n", type36->lower_threshold_non_critical);
			}

			if (LAZYBIOS_FIELD_STATUS(type36, upper_threshold_non_critical) != LAZYBIOS_FIELD_PRESENT) {
				printf("Upper Non-critical Threshold: Not Present\n");
			} else if (type36->upper_threshold_non_critical == 0x8000) {
				printf("Upper Non-critical Threshold: Unavailable\n");
			} else {
				printf("Upper Non-critical Threshold: %hu\n", type36->upper_threshold_non_critical);
			}

			if (LAZYBIOS_FIELD_STATUS(type36, lower_threshold_critical) != LAZYBIOS_FIELD_PRESENT) {
				printf("Lower Critical Threshold: Not Present\n");
			} else if (type36->lower_threshold_critical == 0x8000) {
				printf("Lower Critical Threshold: Unavailable\n");
			} else {
				printf("Lower Critical Threshold: %hu\n", type36->lower_threshold_critical);
			}

			if (LAZYBIOS_FIELD_STATUS(type36, upper_threshold_critical) != LAZYBIOS_FIELD_PRESENT) {
				printf("Upper Critical Threshold: Not Present\n");
			} else if (type36->upper_threshold_critical == 0x8000) {
				printf("Upper Critical Threshold: Unavailable\n");
			} else {
				printf("Upper Critical Threshold: %hu\n", type36->upper_threshold_critical);
			}

			if (LAZYBIOS_FIELD_STATUS(type36, lower_threshold_non_recoverable) != LAZYBIOS_FIELD_PRESENT) {
				printf("Lower Non-recoverable Threshold: Not Present\n");
			} else if (type36->lower_threshold_non_recoverable == 0x8000) {
				printf("Lower Non-recoverable Threshold: Unavailable\n");
			} else {
				printf("Lower Non-recoverable Threshold: %hu\n", type36->lower_threshold_non_recoverable);
			}

			if (LAZYBIOS_FIELD_STATUS(type36, upper_threshold_non_recoverable) != LAZYBIOS_FIELD_PRESENT) {
				printf("Upper Non-recoverable Threshold: Not Present\n");
			} else if (type36->upper_threshold_non_recoverable == 0x8000) {
				printf("Upper Non-recoverable Threshold: Unavailable\n");
			} else {
				printf("Upper Non-recoverable Threshold: %hu\n", type36->upper_threshold_non_recoverable);
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Management Device Threshold Data information\n\n");
	}
}

static void printType37(lazybiosCTX_t* ctx) {
	printf("=== MEMORY CHANNEL ===\n");

	if (!ctx->Type37) ctx->Type37 = lazybiosGetType37(ctx->Type37, &ctx->type37_count, ctx->DMIData);

	if (ctx->Type37 && ctx->type37_count > 0) {
		for (size_t i = 0; i < ctx->type37_count; i++) {
			lazybiosType37_t* type37 = &ctx->Type37[i];

			if (ctx->type37_count > 1) {
				printf("--- Memory Channel %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type37, channel_type) != LAZYBIOS_FIELD_PRESENT) {
				printf("Channel Type: Not Present\n");
			} else {
				printf("Channel Type: %s\n", lazybiosType37ChannelTypeStr(type37->channel_type));
			}

			if (LAZYBIOS_FIELD_STATUS(type37, maximum_channel_load) != LAZYBIOS_FIELD_PRESENT) {
				printf("Maximum Channel Load: Not Present\n");
			} else {
				printf("Maximum Channel Load: %hhu\n", type37->maximum_channel_load);
			}

			if (LAZYBIOS_FIELD_STATUS(type37, memory_device_count) != LAZYBIOS_FIELD_PRESENT) {
				printf("Memory Device Count: Not Present\n");
			} else {
				printf("Memory Device Count: %hhu\n", type37->memory_device_count);
			}

			if (LAZYBIOS_FIELD_STATUS(type37, memory_devices) != LAZYBIOS_FIELD_PRESENT) {
				printf("Memory Devices: Not Present\n");
			} else {
				for (size_t j = 0; j < type37->memory_device_count; j++) {
					lazybiosType37MemoryDevice_t* device = &type37->memory_devices[j];
					printf("Memory Device %zu Load: %hhu\n", j + 1, device->load);
					printf("Memory Device %zu Handle: 0x%04hX\n", j + 1, device->handle);
				}
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Memory Channel information\n\n");
	}
}

static void printType38(lazybiosCTX_t* ctx) {
	printf("=== IPMI DEVICE INFORMATION ===\n");

	if (!ctx->Type38) ctx->Type38 = lazybiosGetType38(ctx->Type38, &ctx->type38_count, ctx->DMIData);

	if (ctx->Type38 && ctx->type38_count > 0) {
		for (size_t i = 0; i < ctx->type38_count; i++) {
			lazybiosType38_t* type38 = &ctx->Type38[i];
			char buf[LAZYBIOS_DECODER_BUF_SIZE];

			if (ctx->type38_count > 1) {
				printf("--- IPMI Device Information %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type38, interface_type) != LAZYBIOS_FIELD_PRESENT) {
				printf("Interface Type: Not Present\n");
			} else {
				printf("Interface Type: %s\n", lazybiosType38InterfaceTypeStr(type38->interface_type));
			}

			if (LAZYBIOS_FIELD_STATUS(type38, ipmi_specification_revision) != LAZYBIOS_FIELD_PRESENT) {
				printf("IPMI Specification Revision: Not Present\n");
			} else {
				lazybiosType38SpecificationRevisionStr(type38->ipmi_specification_revision, buf, sizeof(buf));
				printf("IPMI Specification Revision: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type38, i2c_target_address) != LAZYBIOS_FIELD_PRESENT) {
				printf("I2C Target Address: Not Present\n");
			} else {
				printf("I2C Target Address: 0x%02hhX\n", type38->i2c_target_address);
			}

			if (LAZYBIOS_FIELD_STATUS(type38, nv_storage_device_address) != LAZYBIOS_FIELD_PRESENT) {
				printf("NV Storage Device Address: Not Present\n");
			} else if (type38->nv_storage_device_address == 0xFF) {
				printf("NV Storage Device Address: No Storage Device\n");
			} else {
				printf("NV Storage Device Address: 0x%02hhX\n", type38->nv_storage_device_address);
			}

			if (LAZYBIOS_FIELD_STATUS(type38, base_address) != LAZYBIOS_FIELD_PRESENT) {
				printf("Base Address: Not Present\n");
				printf("Base Address Type: Not Present\n");
			} else {
				printf("Base Address Type: %s\n", lazybiosType38BaseAddressTypeStr(type38->base_address));
				if (LAZYBIOS_FIELD_STATUS(type38, base_address_modifier_interrupt_info) == LAZYBIOS_FIELD_PRESENT) {
					printf("Base Address: 0x%016llX\n", (unsigned long long)lazybiosType38BaseAddressValue(
						type38->base_address, type38->base_address_modifier_interrupt_info));
				} else {
					printf("Base Address: 0x%016llX (raw)\n", (unsigned long long)type38->base_address);
				}
			}

			if (LAZYBIOS_FIELD_STATUS(type38, base_address_modifier_interrupt_info) != LAZYBIOS_FIELD_PRESENT) {
				printf("Register Spacing: Not Present\n");
				printf("Interrupt Information: Not Present\n");
			} else {
				printf("Register Spacing: %s\n",
					lazybiosType38RegisterSpacingStr(type38->base_address_modifier_interrupt_info));
				lazybiosType38InterruptInfoStr(type38->base_address_modifier_interrupt_info, buf, sizeof(buf));
				printf("Interrupt Information: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type38, interrupt_number) != LAZYBIOS_FIELD_PRESENT) {
				printf("Interrupt Number: Not Present\n");
			} else if (type38->interrupt_number == 0) {
				printf("Interrupt Number: Unspecified/Unsupported\n");
			} else {
				printf("Interrupt Number: %hhu\n", type38->interrupt_number);
			}

			printf("\n");
		}
	} else {
		printf("Failed to get IPMI Device information\n\n");
	}
}

static void printType39(lazybiosCTX_t* ctx) {
	printf("=== SYSTEM POWER SUPPLY ===\n");

	if (!ctx->Type39) ctx->Type39 = lazybiosGetType39(ctx->Type39, &ctx->type39_count, ctx->DMIData);

	if (ctx->Type39 && ctx->type39_count > 0) {
		for (size_t i = 0; i < ctx->type39_count; i++) {
			lazybiosType39_t* type39 = &ctx->Type39[i];

			if (ctx->type39_count > 1) {
				printf("--- System Power Supply %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type39, power_unit_group) != LAZYBIOS_FIELD_PRESENT) {
				printf("Power Unit Group: Not Present\n");
			} else if (type39->power_unit_group == 0) {
				printf("Power Unit Group: 0 (Not Redundant)\n");
			} else {
				printf("Power Unit Group: %hhu\n", type39->power_unit_group);
			}

			printf("Location: %s\n", type39->location ? type39->location : "Not Present");
			printf("Device Name: %s\n", type39->device_name ? type39->device_name : "Not Present");
			printf("Manufacturer: %s\n", type39->manufacturer ? type39->manufacturer : "Not Present");
			printf("Serial Number: %s\n", type39->serial_number ? type39->serial_number : "Not Present");
			printf("Asset Tag Number: %s\n", type39->asset_tag_number ? type39->asset_tag_number : "Not Present");
			printf("Model Part Number: %s\n", type39->model_part_number ? type39->model_part_number : "Not Present");
			printf("Revision Level: %s\n", type39->revision_level ? type39->revision_level : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type39, max_power_capacity) != LAZYBIOS_FIELD_PRESENT) {
				printf("Maximum Power Capacity: Not Present\n");
			} else if (type39->max_power_capacity == 0x8000) {
				printf("Maximum Power Capacity: Unknown\n");
			} else {
				printf("Maximum Power Capacity: %hu W\n", type39->max_power_capacity);
			}

			if (LAZYBIOS_FIELD_STATUS(type39, power_supply_characteristics) != LAZYBIOS_FIELD_PRESENT) {
				printf("Power Supply Type: Not Present\n");
				printf("Power Supply Status: Not Present\n");
				printf("Input Voltage Range Switching: Not Present\n");
				printf("Power Supply Flags: Not Present\n");
			} else {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];
				printf("Power Supply Type: %s\n",
					lazybiosType39PowerSupplyTypeStr(type39->power_supply_characteristics));
				printf("Power Supply Status: %s\n", lazybiosType39StatusStr(type39->power_supply_characteristics));
				printf("Input Voltage Range Switching: %s\n",
					lazybiosType39InputVoltageRangeSwitchingStr(type39->power_supply_characteristics));
				lazybiosType39CharacteristicsFlagsStr(type39->power_supply_characteristics, buf, sizeof(buf));
				printf("Power Supply Flags: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type39, input_voltage_probe_handle) != LAZYBIOS_FIELD_PRESENT) {
				printf("Input Voltage Probe Handle: Not Present\n");
			} else {
				printf("Input Voltage Probe Handle: 0x%04hX\n", type39->input_voltage_probe_handle);
			}

			if (LAZYBIOS_FIELD_STATUS(type39, cooling_device_handle) != LAZYBIOS_FIELD_PRESENT) {
				printf("Cooling Device Handle: Not Present\n");
			} else {
				printf("Cooling Device Handle: 0x%04hX\n", type39->cooling_device_handle);
			}

			if (LAZYBIOS_FIELD_STATUS(type39, input_current_probe_handle) != LAZYBIOS_FIELD_PRESENT) {
				printf("Input Current Probe Handle: Not Present\n");
			} else {
				printf("Input Current Probe Handle: 0x%04hX\n", type39->input_current_probe_handle);
			}

			printf("\n");
		}
	} else {
		printf("Failed to get System Power Supply information\n\n");
	}
}

static void printType40(lazybiosCTX_t* ctx) {
	printf("=== ADDITIONAL INFORMATION ===\n");

	if (!ctx->Type40) ctx->Type40 = lazybiosGetType40(ctx->Type40, &ctx->type40_count, ctx->DMIData);

	if (ctx->Type40 && ctx->type40_count > 0) {
		for (size_t i = 0; i < ctx->type40_count; i++) {
			lazybiosType40_t* type40 = &ctx->Type40[i];

			if (ctx->type40_count > 1) {
				printf("--- Additional Information %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type40, additional_information_entry_count) != LAZYBIOS_FIELD_PRESENT) {
				printf("Additional Information Entry Count: Not Present\n");
			} else {
				printf("Additional Information Entry Count: %hhu\n",
					type40->additional_information_entry_count);
			}

			if (LAZYBIOS_FIELD_STATUS(type40, additional_information_entries) != LAZYBIOS_FIELD_PRESENT) {
				printf("Additional Information Entries: Not Present\n");
			} else {
				for (size_t j = 0; j < type40->additional_information_entry_count; j++) {
					lazybiosType40Entry_t* entry = &type40->additional_information_entries[j];
					printf("--- Entry %zu ---\n", j + 1);
					printf("Entry Length: %hhu bytes\n", entry->entry_length);
					printf("Referenced Handle: 0x%04hX\n", entry->referenced_handle);
					printf("Referenced Offset: 0x%02hhX\n", entry->referenced_offset);
					if (entry->field_status.string != LAZYBIOS_FIELD_PRESENT) {
						printf("String: Not Present\n");
					} else {
						printf("String: %s\n", entry->string ? entry->string : "Not Specified");
					}
					printf("Value:");
					for (size_t k = 0; k < entry->value_length; k++) {
						printf(" %02hhX", entry->value[k]);
					}
					printf("\n");
				}
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Additional Information\n\n");
	}
}

static void printType41(lazybiosCTX_t* ctx) {
	printf("=== ONBOARD DEVICES EXTENDED INFORMATION ===\n");

	if (!ctx->Type41) ctx->Type41 = lazybiosGetType41(ctx->Type41, &ctx->type41_count, ctx->DMIData);

	if (ctx->Type41 && ctx->type41_count > 0) {
		for (size_t i = 0; i < ctx->type41_count; i++) {
			lazybiosType41_t* type41 = &ctx->Type41[i];

			if (ctx->type41_count > 1) {
				printf("--- Onboard Device %zu ---\n", i + 1);
			}

			printf("Reference Designation: %s\n",
				type41->reference_designation ? type41->reference_designation : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type41, device_type_and_status) != LAZYBIOS_FIELD_PRESENT) {
				printf("Device Type: Not Present\n");
				printf("Device Status: Not Present\n");
			} else {
				printf("Device Type: %s\n", lazybiosType41DeviceTypeStr(type41->device_type_and_status));
				printf("Device Status: %s\n", lazybiosType41DeviceStatusStr(type41->device_type_and_status));
			}

			if (LAZYBIOS_FIELD_STATUS(type41, device_type_instance) != LAZYBIOS_FIELD_PRESENT) {
				printf("Device Type Instance: Not Present\n");
			} else {
				printf("Device Type Instance: %hhu\n", type41->device_type_instance);
			}

			if (LAZYBIOS_FIELD_STATUS(type41, segment_group_number) != LAZYBIOS_FIELD_PRESENT) {
				printf("Segment Group Number: Not Present\n");
			} else if (type41->segment_group_number == 0xFFFF) {
				printf("Segment Group Number: Not Applicable\n");
			} else {
				printf("Segment Group Number: %hu\n", type41->segment_group_number);
			}

			if (LAZYBIOS_FIELD_STATUS(type41, bus_number) != LAZYBIOS_FIELD_PRESENT) {
				printf("Bus Number: Not Present\n");
			} else if (type41->bus_number == 0xFF) {
				printf("Bus Number: Not Applicable\n");
			} else {
				printf("Bus Number: %hhu\n", type41->bus_number);
			}

			if (LAZYBIOS_FIELD_STATUS(type41, device_function_number) != LAZYBIOS_FIELD_PRESENT) {
				printf("Device/Function Number: Not Present\n");
			} else {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];
				lazybiosType41DeviceFunctionStr(type41->device_function_number, buf, sizeof(buf));
				printf("Device/Function Number: %s\n", buf);
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Onboard Devices Extended information\n\n");
	}
}

static void printType42(lazybiosCTX_t* ctx) {
	printf("=== MANAGEMENT CONTROLLER HOST INTERFACE ===\n");

	if (!ctx->Type42) ctx->Type42 = lazybiosGetType42(ctx->Type42, &ctx->type42_count, ctx->DMIData);

	if (ctx->Type42 && ctx->type42_count > 0) {
		for (size_t i = 0; i < ctx->type42_count; i++) {
			lazybiosType42_t* type42 = &ctx->Type42[i];

			if (ctx->type42_count > 1) {
				printf("--- Management Controller Host Interface %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type42, interface_type) != LAZYBIOS_FIELD_PRESENT) {
				printf("Interface Type: Not Present\n");
			} else {
				printf("Interface Type: %s (0x%02hhX)\n",
					lazybiosType42InterfaceTypeStr(type42->interface_type), type42->interface_type);
			}

			if (LAZYBIOS_FIELD_STATUS(type42, interface_type_specific_data) != LAZYBIOS_FIELD_PRESENT) {
				printf("Interface Type Specific Data: Not Present\n");
			} else {
				printf("Interface Type Specific Data (%zu bytes):", type42->interface_type_specific_data_size);
				for (size_t j = 0; j < type42->interface_type_specific_data_size; j++) {
					printf(" %02hhX", type42->interface_type_specific_data[j]);
				}
				printf("\n");
			}

			if (LAZYBIOS_FIELD_STATUS(type42, number_of_protocol_records) != LAZYBIOS_FIELD_PRESENT) {
				printf("Protocol Record Count: Not Present\n");
			} else {
				printf("Protocol Record Count: %hhu\n", type42->number_of_protocol_records);
			}

			if (LAZYBIOS_FIELD_STATUS(type42, protocol_records) != LAZYBIOS_FIELD_PRESENT) {
				printf("Protocol Records: Not Present\n");
			} else {
				for (size_t j = 0; j < type42->number_of_protocol_records; j++) {
					lazybiosType42ProtocolRecord_t* protocol = &type42->protocol_records[j];
					printf("--- Protocol Record %zu ---\n", j + 1);
					printf("Protocol Type: %s (0x%02hhX)\n",
						lazybiosType42ProtocolTypeStr(protocol->protocol_type), protocol->protocol_type);
					printf("Protocol Type Specific Data (%hhu bytes):",
						protocol->protocol_type_specific_data_length);
					for (size_t k = 0; k < protocol->protocol_type_specific_data_length; k++) {
						printf(" %02hhX", protocol->protocol_type_specific_data[k]);
					}
					printf("\n");
				}
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Management Controller Host Interface information\n\n");
	}
}

static void printType43(lazybiosCTX_t* ctx) {
	printf("=== TPM DEVICE ===\n");

	if (!ctx->Type43) ctx->Type43 = lazybiosGetType43(ctx->Type43, &ctx->type43_count, ctx->DMIData);

	if (ctx->Type43 && ctx->type43_count > 0) {
		for (size_t i = 0; i < ctx->type43_count; i++) {
			lazybiosType43_t* type43 = &ctx->Type43[i];
			char buf[LAZYBIOS_DECODER_BUF_SIZE];

			if (ctx->type43_count > 1) {
				printf("--- TPM Device %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type43, vendor_id) != LAZYBIOS_FIELD_PRESENT) {
				printf("Vendor ID: Not Present\n");
			} else {
				printf("Vendor ID: %s\n", type43->vendor_id);
			}

			if (LAZYBIOS_FIELD_STATUS(type43, major_spec_version) != LAZYBIOS_FIELD_PRESENT ||
				LAZYBIOS_FIELD_STATUS(type43, minor_spec_version) != LAZYBIOS_FIELD_PRESENT) {
				printf("TPM Specification Version: Not Present\n");
			} else {
				printf("TPM Specification Version: %hhu.%hhu\n",
					type43->major_spec_version, type43->minor_spec_version);
			}

			if (LAZYBIOS_FIELD_STATUS(type43, firmware_version_1) != LAZYBIOS_FIELD_PRESENT ||
				LAZYBIOS_FIELD_STATUS(type43, firmware_version_2) != LAZYBIOS_FIELD_PRESENT ||
				LAZYBIOS_FIELD_STATUS(type43, major_spec_version) != LAZYBIOS_FIELD_PRESENT) {
				printf("Firmware Revision: Not Present\n");
			} else {
				lazybiosType43FirmwareVersionStr(type43->major_spec_version, type43->firmware_version_1,
					type43->firmware_version_2, buf, sizeof(buf));
				printf("Firmware Revision: %s\n", buf);
			}

			printf("Description: %s\n", type43->description ? type43->description : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type43, characteristics) != LAZYBIOS_FIELD_PRESENT) {
				printf("Characteristics: Not Present\n");
			} else {
				lazybiosType43CharacteristicsStr(type43->characteristics, buf, sizeof(buf));
				printf("Characteristics: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type43, oem_defined) != LAZYBIOS_FIELD_PRESENT) {
				printf("OEM-defined: Not Present\n");
			} else {
				printf("OEM-defined: 0x%08X\n", type43->oem_defined);
			}

			printf("\n");
		}
	} else {
		printf("Failed to get TPM Device information\n\n");
	}
}

static void printType44(lazybiosCTX_t* ctx) {
	printf("=== PROCESSOR ADDITIONAL INFORMATION ===\n");

	if (!ctx->Type44) ctx->Type44 = lazybiosGetType44(ctx->Type44, &ctx->type44_count, ctx->DMIData);

	if (ctx->Type44 && ctx->type44_count > 0) {
		for (size_t i = 0; i < ctx->type44_count; i++) {
			lazybiosType44_t* type44 = &ctx->Type44[i];

			if (ctx->type44_count > 1) {
				printf("--- Processor Additional Information %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type44, referenced_handle) != LAZYBIOS_FIELD_PRESENT) {
				printf("Referenced Processor Handle: Not Present\n");
			} else {
				printf("Referenced Processor Handle: 0x%04hX\n", type44->referenced_handle);
			}

			if (LAZYBIOS_FIELD_STATUS(type44, block_length) != LAZYBIOS_FIELD_PRESENT) {
				printf("Processor-specific Data Length: Not Present\n");
			} else {
				printf("Processor-specific Data Length: %hhu bytes\n", type44->block_length);
			}

			if (LAZYBIOS_FIELD_STATUS(type44, processor_type) != LAZYBIOS_FIELD_PRESENT) {
				printf("Processor Type: Not Present\n");
			} else {
				printf("Processor Type: %s\n", lazybiosType44ProcessorTypeStr(type44->processor_type));
			}

			if (LAZYBIOS_FIELD_STATUS(type44, processor_specific_data) != LAZYBIOS_FIELD_PRESENT) {
				printf("Processor-specific Data: Not Present\n");
			} else {
				printf("Processor-specific Data:");
				for (size_t j = 0; j < type44->block_length; j++) {
					printf(" %02hhX", type44->processor_specific_data[j]);
				}
				printf("\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Processor Additional Information\n\n");
	}
}

static void printType45(lazybiosCTX_t* ctx) {
	printf("=== FIRMWARE INVENTORY INFORMATION ===\n");

	if (!ctx->Type45) ctx->Type45 = lazybiosGetType45(ctx->Type45, &ctx->type45_count, ctx->DMIData);

	if (ctx->Type45 && ctx->type45_count > 0) {
		for (size_t i = 0; i < ctx->type45_count; i++) {
			lazybiosType45_t* type45 = &ctx->Type45[i];

			if (ctx->type45_count > 1) {
				printf("--- Firmware Component %zu ---\n", i + 1);
			}

			printf("Firmware Component Name: %s\n",
				type45->firmware_component_name ? type45->firmware_component_name : "Not Present");
			printf("Firmware Version: %s\n", type45->firmware_version ? type45->firmware_version : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type45, version_format) != LAZYBIOS_FIELD_PRESENT) {
				printf("Version Format: Not Present\n");
			} else {
				printf("Version Format: %s\n", lazybiosType45VersionFormatStr(type45->version_format));
			}

			printf("Firmware ID: %s\n", type45->firmware_id ? type45->firmware_id : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type45, firmware_id_format) != LAZYBIOS_FIELD_PRESENT) {
				printf("Firmware ID Format: Not Present\n");
			} else {
				printf("Firmware ID Format: %s\n",
					lazybiosType45FirmwareIDFormatStr(type45->firmware_id_format));
			}

			printf("Release Date: %s\n", type45->release_date ? type45->release_date : "Not Present");
			printf("Manufacturer: %s\n", type45->manufacturer ? type45->manufacturer : "Not Present");
			printf("Lowest Supported Firmware Version: %s\n",
				type45->lowest_supported_firmware_version ? type45->lowest_supported_firmware_version : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type45, image_size) != LAZYBIOS_FIELD_PRESENT) {
				printf("Image Size: Not Present\n");
			} else if (type45->image_size == UINT64_MAX) {
				printf("Image Size: Unknown\n");
			} else {
				printf("Image Size: %llu bytes\n", (unsigned long long)type45->image_size);
			}

			if (LAZYBIOS_FIELD_STATUS(type45, characteristics) != LAZYBIOS_FIELD_PRESENT) {
				printf("Characteristics: Not Present\n");
			} else {
				char buf[LAZYBIOS_DECODER_BUF_SIZE];
				lazybiosType45CharacteristicsStr(type45->characteristics, buf, sizeof(buf));
				printf("Characteristics: %s\n", buf);
			}

			if (LAZYBIOS_FIELD_STATUS(type45, state) != LAZYBIOS_FIELD_PRESENT) {
				printf("State: Not Present\n");
			} else {
				printf("State: %s\n", lazybiosType45StateStr(type45->state));
			}

			if (LAZYBIOS_FIELD_STATUS(type45, number_of_associated_components) != LAZYBIOS_FIELD_PRESENT) {
				printf("Number of Associated Components: Not Present\n");
			} else {
				printf("Number of Associated Components: %hhu\n", type45->number_of_associated_components);
			}

			if (LAZYBIOS_FIELD_STATUS(type45, associated_component_handles) != LAZYBIOS_FIELD_PRESENT) {
				printf("Associated Component Handles: Not Present\n");
			} else if (type45->number_of_associated_components == 0) {
				printf("Associated Component Handles: None\n");
			} else {
				printf("Associated Component Handles:");
				for (size_t j = 0; j < type45->number_of_associated_components; j++) {
					printf(" 0x%04hX", type45->associated_component_handles[j]);
				}
				printf("\n");
			}

			printf("\n");
		}
	} else {
		printf("Failed to get Firmware Inventory information\n\n");
	}
}

static void printType46(lazybiosCTX_t* ctx) {
	printf("=== STRING PROPERTY ===\n");

	if (!ctx->Type46) ctx->Type46 = lazybiosGetType46(ctx->Type46, &ctx->type46_count, ctx->DMIData);

	if (ctx->Type46 && ctx->type46_count > 0) {
		for (size_t i = 0; i < ctx->type46_count; i++) {
			lazybiosType46_t* type46 = &ctx->Type46[i];

			if (ctx->type46_count > 1) {
				printf("--- String Property %zu ---\n", i + 1);
			}

			if (LAZYBIOS_FIELD_STATUS(type46, string_property_id) != LAZYBIOS_FIELD_PRESENT) {
				printf("String Property ID: Not Present\n");
			} else {
				printf("String Property ID: %hu (%s)\n", type46->string_property_id,
					lazybiosType46StringPropertyIDStr(type46->string_property_id));
			}

			printf("String Property Value: %s\n",
				type46->string_property_value ? type46->string_property_value : "Not Present");

			if (LAZYBIOS_FIELD_STATUS(type46, parent_handle) != LAZYBIOS_FIELD_PRESENT) {
				printf("Parent Handle: Not Present\n");
			} else {
				printf("Parent Handle: 0x%04hX\n", type46->parent_handle);
			}

			printf("\n");
		}
	} else {
		printf("Failed to get String Property information\n\n");
	}
}

/**
 * @brief Prints detailed SMBIOS entry point version information.
 *
 * @param ctx Initialized lazybios context containing parsed entry point data.
 * @return 0 on success, or -1 if ctx is NULL.
 */
int print_smbios_version_info(lazybiosCTX_t* ctx) {
    if (!ctx) return -1;
    printf("=== SMBIOS VERSION INFORMATION ===\n");
    lazybiosPrintVer(ctx);

    if (ctx->DMIData->entry_tag == SMBIOS_VER_3X) {
        lazybiosSMBIOS3Entry* v3 = ctx->DMIData->entry_union.v3;

        printf("Entry Point Version: 3.x\n");
        printf("Anchor: %c%c%c%c%c\n", v3->anchor[0], v3->anchor[1], v3->anchor[2], v3->anchor[3], v3->anchor[4]);
        printf("Entry Point Length: %u bytes\n", v3->entry_point_length);
        printf("Major Version: %u\n", v3->major_version);
        printf("Minor Version: %u\n", v3->minor_version);
        printf("Docrev: %u\n", v3->docrev);
        printf("Entry Point Revision: %u\n", v3->entry_point_revision);
        printf("Table Max Size: %u bytes\n", v3->structure_table_max_size);

        if (ctx->backend == LAZYBIOS_BACKEND_WINDOWS) {
            printf("Table Address: Not available (Windows API)\n");
        } else {
            printf("Table Address: 0x%lX\n", (unsigned long)v3->structure_table_address);
        }
        printf("Is 64-bit: Yes\n");

    } else if (ctx->DMIData->entry_tag == SMBIOS_VER_2X) {
        lazybiosSMBIOS2Entry* v2 = ctx->DMIData->entry_union.v2;

        printf("Entry Point Version: 2.x\n");
        printf("Anchor: %c%c%c%c\n", v2->anchor[0], v2->anchor[1], v2->anchor[2], v2->anchor[3]);
        printf("Entry Point Length: %u bytes\n", v2->entry_point_length);
        printf("Major Version: %u\n", v2->major_version);
        printf("Minor Version: %u\n", v2->minor_version);
        printf("Maximum Structure Size: %u bytes\n", v2->maximum_structure_size);
        printf("Entry Point Revision: %u\n", v2->entry_point_revision);
        printf("Intermediate Anchor: %c%c%c%c%c\n", v2->intermediate_anchor[0], v2->intermediate_anchor[1], v2->intermediate_anchor[2], v2->intermediate_anchor[3], v2->intermediate_anchor[4]);
        printf("Structure Table Length: %u bytes\n", v2->structure_table_length);
        printf("Number of Structures: %u\n", v2->structure_count);
        printf("BCD Revision: %u.%u\n", (v2->bcd_revision >> 4) & 0x0F, v2->bcd_revision & 0x0F);

        if (ctx->backend == LAZYBIOS_BACKEND_WINDOWS) {
            printf("Table Address: Not available (Windows API)\n");
        } else {
            printf("Table Address: 0x%lX\n", (unsigned long)v2->structure_table_address);
        }
        printf("Is 64-bit: No\n");

    } else {
        printf("Entry Point Version: Unknown\n");
        printf("Table Length: Not Present\n");
        printf("Table Address: Not Present\n");
        printf("Is 64-bit: Not Present\n");
    }
    printf("\n");

    return 0;
}

static inline void print_usage(const char* progname) {
	printf("Usage: %s [options]\n", progname);
	printf("Options:\n");
	printf("  --help                      Show this help message\n");
	printf("  --type <number>             prints the SMBIOS type <number>, so --type 0 means Firmware Information etc.\n");
	printf("  --compact                   Hide fields that require a newer SMBIOS version\n");
	printf("  --dump <dir_to_dump>        Dumps both the raw entry info and raw DMI table into 2 files into <dir_to_dump> directory, If OS is Windows it only dumps DMI.bin\n If <dir_to_dump> isn't specified it will default to the current directory\n");
	printf("  --sources <entry> <dmi>     Usees the parser on the 2 specified files <entry> and <dmi>\n");
	printf("  --single-source <binary>    Uses the parser on the single specified file <binary>, which should hold the entry point first and dmi data merged together\n");
}

/**
 * @brief Runs the lazybios command-line test and inspection program.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return Process exit status, with zero indicating success.
 */
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
	const char* dump_dir = NULL;
	const char* entry_file = NULL;
	const char* dmi_file = NULL;
	const char* single_file = NULL;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--help") == 0) {
			print_usage(argv[0]);
			lazybiosCleanup(ctx);
			return 0;
		} else if (strcmp(argv[i], "--compact") == 0) {
			compact_output = 1;
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

		if (!ctx->Type5) ctx->Type5 = lazybiosGetType5(ctx->Type5, &ctx->type5_count, ctx->DMIData);
		printType5(ctx);

		if (!ctx->Type6) ctx->Type6 = lazybiosGetType6(ctx->Type6, &ctx->type6_count, ctx->DMIData);
		printType6(ctx);

		if (!ctx->Type7) ctx->Type7 = lazybiosGetType7(ctx->Type7, &ctx->type7_count, ctx->DMIData);
		printType7(ctx);

		if (!ctx->Type8) ctx->Type8 = lazybiosGetType8(ctx->Type8, &ctx->type8_count, ctx->DMIData);
		printType8(ctx);

		if (!ctx->Type9) ctx->Type9 = lazybiosGetType9(ctx->Type9, &ctx->type9_count, ctx->DMIData);
		printType9(ctx);

		if (!ctx->Type10) ctx->Type10 = lazybiosGetType10(ctx->Type10, &ctx->type10_count, ctx->DMIData);
		printType10(ctx);

		if (!ctx->Type11) ctx->Type11 = lazybiosGetType11(ctx->Type11, &ctx->type11_count, ctx->DMIData);
		printType11(ctx);

		if (!ctx->Type12) ctx->Type12 = lazybiosGetType12(ctx->Type12, &ctx->type12_count, ctx->DMIData);
		printType12(ctx);

		if (!ctx->Type13) ctx->Type13 = lazybiosGetType13(ctx->Type13, &ctx->type13_count, ctx->DMIData);
		printType13(ctx);

		if (!ctx->Type14) ctx->Type14 = lazybiosGetType14(ctx->Type14, &ctx->type14_count, ctx->DMIData);
		printType14(ctx);

		if (!ctx->Type15) ctx->Type15 = lazybiosGetType15(ctx->Type15, &ctx->type15_count, ctx->DMIData);
		printType15(ctx);

		if (!ctx->Type16) ctx->Type16 = lazybiosGetType16(ctx->Type16, &ctx->type16_count, ctx->DMIData);
		printType16(ctx);

		if (!ctx->Type17) ctx->Type17 = lazybiosGetType17(ctx->Type17, &ctx->type17_count, ctx->DMIData);
		printType17(ctx);

		if (!ctx->Type18) ctx->Type18 = lazybiosGetType18(ctx->Type18, &ctx->type18_count, ctx->DMIData);
		printType18(ctx);

		if (!ctx->Type19) ctx->Type19 = lazybiosGetType19(ctx->Type19, &ctx->type19_count, ctx->DMIData);
		printType19(ctx);

		if (!ctx->Type20) ctx->Type20 = lazybiosGetType20(ctx->Type20, &ctx->type20_count, ctx->DMIData);
		printType20(ctx);

		if (!ctx->Type21) ctx->Type21 = lazybiosGetType21(ctx->Type21, &ctx->type21_count, ctx->DMIData);
		printType21(ctx);

		if (!ctx->Type22) ctx->Type22 = lazybiosGetType22(ctx->Type22, &ctx->type22_count, ctx->DMIData);
		printType22(ctx);

		if (!ctx->Type23) ctx->Type23 = lazybiosGetType23(ctx->Type23, &ctx->type23_count, ctx->DMIData);
		printType23(ctx);

		if (!ctx->Type24) ctx->Type24 = lazybiosGetType24(ctx->Type24, &ctx->type24_count, ctx->DMIData);
		printType24(ctx);

		if (!ctx->Type25) ctx->Type25 = lazybiosGetType25(ctx->Type25, &ctx->type25_count, ctx->DMIData);
		printType25(ctx);

		if (!ctx->Type26) ctx->Type26 = lazybiosGetType26(ctx->Type26, &ctx->type26_count, ctx->DMIData);
		printType26(ctx);

		if (!ctx->Type27) ctx->Type27 = lazybiosGetType27(ctx->Type27, &ctx->type27_count, ctx->DMIData);
		printType27(ctx);

		if (!ctx->Type28) ctx->Type28 = lazybiosGetType28(ctx->Type28, &ctx->type28_count, ctx->DMIData);
		printType28(ctx);

		if (!ctx->Type29) ctx->Type29 = lazybiosGetType29(ctx->Type29, &ctx->type29_count, ctx->DMIData);
		printType29(ctx);

		if (!ctx->Type30) ctx->Type30 = lazybiosGetType30(ctx->Type30, &ctx->type30_count, ctx->DMIData);
		printType30(ctx);

		if (!ctx->Type31) ctx->Type31 = lazybiosGetType31(ctx->Type31, &ctx->type31_count, ctx->DMIData);
		printType31(ctx);

		if (!ctx->Type32) ctx->Type32 = lazybiosGetType32(ctx->Type32, &ctx->type32_count, ctx->DMIData);
		printType32(ctx);

		if (!ctx->Type33) ctx->Type33 = lazybiosGetType33(ctx->Type33, &ctx->type33_count, ctx->DMIData);
		printType33(ctx);

		if (!ctx->Type34) ctx->Type34 = lazybiosGetType34(ctx->Type34, &ctx->type34_count, ctx->DMIData);
		printType34(ctx);

		if (!ctx->Type35) ctx->Type35 = lazybiosGetType35(ctx->Type35, &ctx->type35_count, ctx->DMIData);
		printType35(ctx);

		if (!ctx->Type36) ctx->Type36 = lazybiosGetType36(ctx->Type36, &ctx->type36_count, ctx->DMIData);
		printType36(ctx);

		if (!ctx->Type37) ctx->Type37 = lazybiosGetType37(ctx->Type37, &ctx->type37_count, ctx->DMIData);
		printType37(ctx);

		if (!ctx->Type38) ctx->Type38 = lazybiosGetType38(ctx->Type38, &ctx->type38_count, ctx->DMIData);
		printType38(ctx);

		if (!ctx->Type39) ctx->Type39 = lazybiosGetType39(ctx->Type39, &ctx->type39_count, ctx->DMIData);
		printType39(ctx);

		if (!ctx->Type40) ctx->Type40 = lazybiosGetType40(ctx->Type40, &ctx->type40_count, ctx->DMIData);
		printType40(ctx);

		if (!ctx->Type41) ctx->Type41 = lazybiosGetType41(ctx->Type41, &ctx->type41_count, ctx->DMIData);
		printType41(ctx);

		if (!ctx->Type42) ctx->Type42 = lazybiosGetType42(ctx->Type42, &ctx->type42_count, ctx->DMIData);
		printType42(ctx);

		if (!ctx->Type43) ctx->Type43 = lazybiosGetType43(ctx->Type43, &ctx->type43_count, ctx->DMIData);
		printType43(ctx);

		if (!ctx->Type44) ctx->Type44 = lazybiosGetType44(ctx->Type44, &ctx->type44_count, ctx->DMIData);
		printType44(ctx);

		if (!ctx->Type45) ctx->Type45 = lazybiosGetType45(ctx->Type45, &ctx->type45_count, ctx->DMIData);
		printType45(ctx);

		if (!ctx->Type46) ctx->Type46 = lazybiosGetType46(ctx->Type46, &ctx->type46_count, ctx->DMIData);
		printType46(ctx);
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

			case 5:
				if (!ctx->Type5) ctx->Type5 = lazybiosGetType5(ctx->Type5, &ctx->type5_count, ctx->DMIData);
				printType5(ctx);
				break;

			case 6:
				if (!ctx->Type6) ctx->Type6 = lazybiosGetType6(ctx->Type6, &ctx->type6_count, ctx->DMIData);
				printType6(ctx);
				break;

			case 7:
				if (!ctx->Type7) ctx->Type7 = lazybiosGetType7(ctx->Type7, &ctx->type7_count, ctx->DMIData);
				printType7(ctx);
				break;

			case 8:
				if (!ctx->Type8) ctx->Type8 = lazybiosGetType8(ctx->Type8, &ctx->type8_count, ctx->DMIData);
				printType8(ctx);
				break;

			case 9:
				if (!ctx->Type9) ctx->Type9 = lazybiosGetType9(ctx->Type9, &ctx->type9_count, ctx->DMIData);
				printType9(ctx);
				break;

			case 10:
				if (!ctx->Type10) ctx->Type10 = lazybiosGetType10(ctx->Type10, &ctx->type10_count, ctx->DMIData);
				printType10(ctx);
				break;

			case 11:
				if (!ctx->Type11) ctx->Type11 = lazybiosGetType11(ctx->Type11, &ctx->type11_count, ctx->DMIData);
				printType11(ctx);
				break;

			case 12:
				if (!ctx->Type12) ctx->Type12 = lazybiosGetType12(ctx->Type12, &ctx->type12_count, ctx->DMIData);
				printType12(ctx);
				break;

			case 13:
				if (!ctx->Type13) ctx->Type13 = lazybiosGetType13(ctx->Type13, &ctx->type13_count, ctx->DMIData);
				printType13(ctx);
				break;

			case 14:
				if (!ctx->Type14) ctx->Type14 = lazybiosGetType14(ctx->Type14, &ctx->type14_count, ctx->DMIData);
				printType14(ctx);
				break;

			case 15:
				if (!ctx->Type15) ctx->Type15 = lazybiosGetType15(ctx->Type15, &ctx->type15_count, ctx->DMIData);
				printType15(ctx);
				break;

			case 16:
				if (!ctx->Type16) ctx->Type16 = lazybiosGetType16(ctx->Type16, &ctx->type16_count, ctx->DMIData);
				printType16(ctx);
				break;

			case 17:
				if (!ctx->Type17) ctx->Type17 = lazybiosGetType17(ctx->Type17, &ctx->type17_count, ctx->DMIData);
				printType17(ctx);
				break;

			case 18:
				if (!ctx->Type18) ctx->Type18 = lazybiosGetType18(ctx->Type18, &ctx->type18_count, ctx->DMIData);
				printType18(ctx);
				break;

			case 19:
				if (!ctx->Type19) ctx->Type19 = lazybiosGetType19(ctx->Type19, &ctx->type19_count, ctx->DMIData);
				printType19(ctx);
				break;

			case 20:
				if (!ctx->Type20) ctx->Type20 = lazybiosGetType20(ctx->Type20, &ctx->type20_count, ctx->DMIData);
				printType20(ctx);
				break;

			case 21:
				if (!ctx->Type21) ctx->Type21 = lazybiosGetType21(ctx->Type21, &ctx->type21_count, ctx->DMIData);
				printType21(ctx);
				break;

			case 22:
				if (!ctx->Type22) ctx->Type22 = lazybiosGetType22(ctx->Type22, &ctx->type22_count, ctx->DMIData);
				printType22(ctx);
				break;

			case 23:
				if (!ctx->Type23) ctx->Type23 = lazybiosGetType23(ctx->Type23, &ctx->type23_count, ctx->DMIData);
				printType23(ctx);
				break;

			case 24:
				if (!ctx->Type24) ctx->Type24 = lazybiosGetType24(ctx->Type24, &ctx->type24_count, ctx->DMIData);
				printType24(ctx);
				break;

			case 25:
				if (!ctx->Type25) ctx->Type25 = lazybiosGetType25(ctx->Type25, &ctx->type25_count, ctx->DMIData);
				printType25(ctx);
				break;

			case 26:
				if (!ctx->Type26) ctx->Type26 = lazybiosGetType26(ctx->Type26, &ctx->type26_count, ctx->DMIData);
				printType26(ctx);
				break;

			case 27:
				if (!ctx->Type27) ctx->Type27 = lazybiosGetType27(ctx->Type27, &ctx->type27_count, ctx->DMIData);
				printType27(ctx);
				break;

			case 28:
				if (!ctx->Type28) ctx->Type28 = lazybiosGetType28(ctx->Type28, &ctx->type28_count, ctx->DMIData);
				printType28(ctx);
				break;

			case 29:
				if (!ctx->Type29) ctx->Type29 = lazybiosGetType29(ctx->Type29, &ctx->type29_count, ctx->DMIData);
				printType29(ctx);
				break;

			case 30:
				if (!ctx->Type30) ctx->Type30 = lazybiosGetType30(ctx->Type30, &ctx->type30_count, ctx->DMIData);
				printType30(ctx);
				break;

			case 31:
				if (!ctx->Type31) ctx->Type31 = lazybiosGetType31(ctx->Type31, &ctx->type31_count, ctx->DMIData);
				printType31(ctx);
				break;

			case 32:
				if (!ctx->Type32) ctx->Type32 = lazybiosGetType32(ctx->Type32, &ctx->type32_count, ctx->DMIData);
				printType32(ctx);
				break;

			case 33:
				if (!ctx->Type33) ctx->Type33 = lazybiosGetType33(ctx->Type33, &ctx->type33_count, ctx->DMIData);
				printType33(ctx);
				break;

			case 34:
				if (!ctx->Type34) ctx->Type34 = lazybiosGetType34(ctx->Type34, &ctx->type34_count, ctx->DMIData);
				printType34(ctx);
				break;

			case 35:
				if (!ctx->Type35) ctx->Type35 = lazybiosGetType35(ctx->Type35, &ctx->type35_count, ctx->DMIData);
				printType35(ctx);
				break;

			case 36:
				if (!ctx->Type36) ctx->Type36 = lazybiosGetType36(ctx->Type36, &ctx->type36_count, ctx->DMIData);
				printType36(ctx);
				break;

			case 37:
				if (!ctx->Type37) ctx->Type37 = lazybiosGetType37(ctx->Type37, &ctx->type37_count, ctx->DMIData);
				printType37(ctx);
				break;

			case 38:
				if (!ctx->Type38) ctx->Type38 = lazybiosGetType38(ctx->Type38, &ctx->type38_count, ctx->DMIData);
				printType38(ctx);
				break;

			case 39:
				if (!ctx->Type39) ctx->Type39 = lazybiosGetType39(ctx->Type39, &ctx->type39_count, ctx->DMIData);
				printType39(ctx);
				break;

			case 40:
				if (!ctx->Type40) ctx->Type40 = lazybiosGetType40(ctx->Type40, &ctx->type40_count, ctx->DMIData);
				printType40(ctx);
				break;

			case 41:
				if (!ctx->Type41) ctx->Type41 = lazybiosGetType41(ctx->Type41, &ctx->type41_count, ctx->DMIData);
				printType41(ctx);
				break;

			case 42:
				if (!ctx->Type42) ctx->Type42 = lazybiosGetType42(ctx->Type42, &ctx->type42_count, ctx->DMIData);
				printType42(ctx);
				break;

			case 43:
				if (!ctx->Type43) ctx->Type43 = lazybiosGetType43(ctx->Type43, &ctx->type43_count, ctx->DMIData);
				printType43(ctx);
				break;

			case 44:
				if (!ctx->Type44) ctx->Type44 = lazybiosGetType44(ctx->Type44, &ctx->type44_count, ctx->DMIData);
				printType44(ctx);
				break;

			case 45:
				if (!ctx->Type45) ctx->Type45 = lazybiosGetType45(ctx->Type45, &ctx->type45_count, ctx->DMIData);
				printType45(ctx);
				break;

			case 46:
				if (!ctx->Type46) ctx->Type46 = lazybiosGetType46(ctx->Type46, &ctx->type46_count, ctx->DMIData);
				printType46(ctx);
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

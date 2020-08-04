#include <gtest/gtest.h>

#include "../include/EEPROMByteSentinelWearLeveling.h"

#include "tests_utils.h"

TEST(tests_byte, read_fresh)
{
	initEEPROMClear(6);

	currentOffset = &offset0;
	EEPROMByteSentinelWearLeveling<TRecord, 6, &offset0> eeprom;

	TRecord rec;
	ASSERT_FALSE(eeprom.load(rec));
}

TEST(tests_byte, insert_into_fresh)
{
	initEEPROMClear(6);

	currentOffset = &offset0;
	EEPROMByteSentinelWearLeveling<TRecord, 6, &offset0> eeprom;

	eeprom.save(TRecord{ .val=1 });
	ASSERT_EEPROM_VALUE(eeprom, { 0, 1, 255, 255, 255, 255 }, 1);
}

TEST(tests_byte, insert_into_used)
{
	initEEPROMWithContent({ 0, 1, 255, 255, 255, 255 });

	currentOffset = &offset0;
	EEPROMByteSentinelWearLeveling<TRecord, 6, &offset0> eeprom;

	eeprom.save(TRecord{ .val=2 });
	ASSERT_EEPROM_VALUE(eeprom, { 0, 1, 0, 2, 255, 255 }, 2);

	eeprom.save(TRecord{ .val=3 });
	ASSERT_EEPROM_VALUE(eeprom, { 0, 1, 0, 2, 0, 3 }, 3);
}

TEST(tests_byte, insert_into_full)
{
	initEEPROMWithContent({ 0, 1, 0, 2, 0, 3 });

	currentOffset = &offset0;
	EEPROMByteSentinelWearLeveling<TRecord, 6, &offset0> eeprom;

	eeprom.save(TRecord{ .val=4 });
	ASSERT_EEPROM_VALUE(eeprom,{ 0xfe, 4, 0, 2, 0, 3 }, 4);

	eeprom.save(TRecord{ .val=5 });
	ASSERT_EEPROM_VALUE(eeprom,{ 0xfe, 4, 0xfe, 5, 0, 3 }, 5);
}

TEST(tests_byte, with_offset)
{
	initEEPROMWithContent({ 123, 255, 255, 255, 255, 255, 255 });

	currentOffset = &offset1;
	EEPROMByteSentinelWearLeveling<TRecord, 6, &offset1> eeprom;

	eeprom.save(TRecord{ .val=1 });
	ASSERT_EEPROM_VALUE(eeprom, { 123, 0, 1, 255, 255, 255, 255 }, 1);

	eeprom.save(TRecord{ .val=2 });
	ASSERT_EEPROM_VALUE(eeprom, { 123, 0, 1, 0, 2, 255, 255 }, 2);

	eeprom.save(TRecord{ .val=3 });
	ASSERT_EEPROM_VALUE(eeprom, { 123, 0, 1, 0, 2, 0, 3 }, 3);

	eeprom.save(TRecord{ .val=4 });
	ASSERT_EEPROM_VALUE(eeprom, { 123, 0xfe, 4, 0, 2, 0, 3 }, 4);
}
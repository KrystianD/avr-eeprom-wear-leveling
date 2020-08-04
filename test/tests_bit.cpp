#include <gtest/gtest.h>

#include "../include/EEPROMBitSentinelWearLeveling.h"

#include "tests_utils.h"

TEST(tests_bit, read_fresh)
{
	initEEPROMClear(3);

	currentOffset = &offset0;
	EEPROMBitSentinelWearLeveling<TRecord, 3, &offset0> eeprom;

	TRecord rec;
	ASSERT_FALSE(eeprom.load(rec));
}

TEST(tests_bit, insert_into_fresh)
{
	initEEPROMClear(3);

	currentOffset = &offset0;
	EEPROMBitSentinelWearLeveling<TRecord, 3, &offset0> eeprom;

	eeprom.save(TRecord{ .val=1 });
	ASSERT_EEPROM_VALUE(eeprom, { SENTINEL(false, 1), 255, 255 }, 1);
}

TEST(tests_bit, insert_into_used)
{
	initEEPROMWithContent({ SENTINEL(false, 1), 255, 255 });

	currentOffset = &offset0;
	EEPROMBitSentinelWearLeveling<TRecord, 3, &offset0> eeprom;

	eeprom.save(TRecord{ .val=2 });
	ASSERT_EEPROM_VALUE(eeprom, { SENTINEL(false, 1), SENTINEL(false, 2), 255 }, 2);

	eeprom.save(TRecord{ .val=3 });
	ASSERT_EEPROM_VALUE(eeprom, { SENTINEL(false, 1), SENTINEL(false, 2), SENTINEL(false, 3) }, 3);
}

TEST(tests_bit, insert_into_full)
{
	ASSERT_EEPROM({ SENTINEL(false, 1), SENTINEL(false, 2), SENTINEL(false, 3) });

	currentOffset = &offset0;
	EEPROMBitSentinelWearLeveling<TRecord, 3, &offset0> eeprom;

	TRecord rec;

	eeprom.save(TRecord{ .val=4 });
	ASSERT_EEPROM_VALUE(eeprom, { SENTINEL(true, 4), SENTINEL(false, 2), SENTINEL(false, 3) }, 4);

	eeprom.save(TRecord{ .val=5 });
	ASSERT_EEPROM_VALUE(eeprom, { SENTINEL(true, 4), SENTINEL(true, 5), SENTINEL(false, 3) }, 5);
}

TEST(tests_bit, with_offset)
{
	initEEPROMWithContent({ 123, 255, 255, 255 });

	currentOffset = &offset1;
	EEPROMBitSentinelWearLeveling<TRecord, 3, &offset1> eeprom;

	eeprom.save(TRecord{ .val=1 });
	ASSERT_EEPROM_VALUE(eeprom, { 123, SENTINEL(false, 1), 255, 255 }, 1);

	eeprom.save(TRecord{ .val=2 });
	ASSERT_EEPROM_VALUE(eeprom, { 123, SENTINEL(false, 1), SENTINEL(false, 2), 255 }, 2);

	eeprom.save(TRecord{ .val=3 });
	ASSERT_EEPROM_VALUE(eeprom, { 123, SENTINEL(false, 1), SENTINEL(false, 2), SENTINEL(false, 3) }, 3);

	eeprom.save(TRecord{ .val=4 });
	ASSERT_EEPROM_VALUE(eeprom, { 123, SENTINEL(true, 4), SENTINEL(false, 2), SENTINEL(false, 3) }, 4);
}
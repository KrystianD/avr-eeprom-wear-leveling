#pragma once

#include <cstdint>

#include <vector>

#include <gtest/gtest.h>

struct TRecord
{
	uint8_t val;
};

extern std::vector<uint8_t> eepromData;
extern uint8_t offset0;
extern uint8_t offset1;

extern uint8_t* currentOffset;

#define ASSERT_EQ_ARRAY(a1, a2, len) \
  { for (int i = 0; i < len; i++) { ASSERT_EQ((uint8_t)a2[i], (uint8_t)a1[i]); } }

static void initEEPROMClear(int size)
{
	eepromData.resize(size);
	for (int i = 0; i < size; i++)
		eepromData[i] = 0xff;
}

static void initEEPROMWithContent(const std::vector<uint8_t>& data)
{
	eepromData = data;
}

static void ASSERT_EEPROM(const std::vector<uint8_t>& data)
{
	ASSERT_EQ_ARRAY(data.data(), eepromData.data(), eepromData.size());
}

template<typename T>
static void ASSERT_EEPROM_VALUE(T& eeprom, const std::vector<uint8_t>& data, int value)
{
	TRecord rec;
	ASSERT_EEPROM(data);
	ASSERT_TRUE(eeprom.load(rec));
	ASSERT_EQ(rec.val, value);
}

static uint8_t SENTINEL(bool sentinel, uint8_t value)
{
	return (sentinel ? 0x80 : 0x00) | value;
}
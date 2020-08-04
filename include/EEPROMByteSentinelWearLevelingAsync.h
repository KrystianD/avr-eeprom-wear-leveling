#pragma once

#include <stdint.h>
#include <string.h>

#include <avr/eeprom.h>

extern void eeprom_update_block_async_reverse(const void* buffer, uint16_t address, uint16_t length);

template<class TRecord, int EEPROMSize, uint8_t* EEPROMStart, typename IndexType=int16_t>
class EEPROMByteSentinelWearLevelingAsync
{
	static const int SentinelSize = 1;
	static const int InternalRecordSize = sizeof(TRecord) + SentinelSize;

	uint8_t buffer[InternalRecordSize];

	static_assert(sizeof(TRecord) > 0, "TRecord size must be > 0");
	static_assert(EEPROMSize > 0, "EEPROM size must be > 0");
	static_assert((EEPROMSize % InternalRecordSize) == 0, "EEPROM size must be multiple of TRecord size plus 1");

public:
	bool load(TRecord& record)
	{
		IndexType lastIdx = findLastIdx();

		if (lastIdx == -1)
			return false;

		eeprom_read_block(&record, (uint8_t*)(EEPROMStart + lastIdx + SentinelSize), sizeof(TRecord));

		return true;
	}

	void save(const TRecord& record)
	{
		uint8_t sentinel = readSentinel();
		IndexType nextIdx = findNextEmptyIdx();

		if (nextIdx == 0)
			sentinel = sentinel ? 0 : 0xfe;

		const uint8_t* recordBytes = (uint8_t*)&record;

		buffer[0] = sentinel;
		memcpy(buffer + SentinelSize, recordBytes, sizeof(TRecord));

		eeprom_update_block_async_reverse(buffer, (uint16_t)EEPROMStart + nextIdx, InternalRecordSize);
	}

private:
	uint8_t readSentinel()
	{
		return eeprom_read_byte((uint8_t*)EEPROMStart);
	}

	IndexType findLastIdx()
	{
		uint8_t sentinel = readSentinel();
		IndexType lastIdx = -1;
		bool hasAny = false;

		for (IndexType i = 0; i < EEPROMSize; i += InternalRecordSize) {
			uint8_t curSentinel = eeprom_read_byte((uint8_t*)(EEPROMStart + i));
			if (curSentinel != 0xff)
				hasAny = true;
			if (curSentinel != sentinel)
				break;
			lastIdx = i;
		}

		return hasAny ? lastIdx : -1;
	}

	IndexType findNextEmptyIdx()
	{
		IndexType lastIdx = findLastIdx();

		if (lastIdx == -1)
			return 0;

		IndexType nextIdx = lastIdx + InternalRecordSize;

		if (nextIdx >= EEPROMSize)
			nextIdx = 0;

		return nextIdx;
	}
};

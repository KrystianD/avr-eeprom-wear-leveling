#pragma once

#include <stdint.h>
#include <string.h>

#include <avr/eeprom.h>

template<class TRecord, int EEPROMSize, uint8_t* EEPROMStart, typename IndexType=int16_t>
class EEPROMByteSentinelWearLeveling
{
	static const int SentinelSize = 1;
	static const int InternalRecordSize = sizeof(TRecord) + SentinelSize;

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

		// save all record bytes
		eeprom_update_block(recordBytes, (uint8_t*)(EEPROMStart + nextIdx + SentinelSize), sizeof(TRecord));
		// commit transaction by saving the sentinel byte
		eeprom_update_byte((uint8_t*)(EEPROMStart + nextIdx), sentinel);
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

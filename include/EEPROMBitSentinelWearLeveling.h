#pragma once

#include <cstdint>

#include <avr/eeprom.h>

template<class TRecord, int EEPROMSize, uint8_t* EEPROMStart, typename IndexType=int16_t>
class EEPROMBitSentinelWearLeveling
{
	static const int InternalRecordSize = sizeof(TRecord);

	static_assert(sizeof(TRecord) > 0, "TRecord size must be > 0");
	static_assert(EEPROMSize > 0, "EEPROM size must be > 0");
	static_assert((EEPROMSize % InternalRecordSize) == 0, "EEPROM size must be multiple of TRecord size");

public:
	bool load(TRecord& record)
	{
		IndexType lastIdx = findLastIdx();

		if (lastIdx == -1)
			return false;

		uint8_t* recordBytes = (uint8_t*)&record;

		eeprom_read_block(recordBytes, (uint8_t*)(EEPROMStart + lastIdx), sizeof(TRecord));
		recordBytes[0] &= ~0x80; // remove the sentinel from the first byte

		return true;
	}

	void save(const TRecord& record)
	{
		uint8_t sentinel = readSentinel();
		IndexType nextIdx = findNextEmptyIdx();

		if (nextIdx == 0)
			sentinel ^= 1;

		const uint8_t* recordBytes = (uint8_t*)&record;

		uint8_t firstByte = recordBytes[0]; // copy first byte not to modify the original record
		firstByte = (firstByte & ~0x80) | (sentinel ? 0x80 : 0x00); // apply current sentinel to the first byte

		// save all bytes except the first one (to prevent corruption if write is interrupted)
		eeprom_update_block(recordBytes + 1, (uint8_t*)(EEPROMStart + nextIdx + 1), sizeof(TRecord) - 1);
		// commit transaction by saving the first byte with the sentinel
		eeprom_update_byte((uint8_t*)(EEPROMStart + nextIdx), firstByte);
	}

private:
	uint8_t readSentinel()
	{
		return !!(eeprom_read_byte((uint8_t*)EEPROMStart) & 0x80);
	}

	IndexType findLastIdx()
	{
		uint8_t sentinel = readSentinel();
		IndexType lastIdx = 0;

		for (IndexType i = 0; i < EEPROMSize; i += InternalRecordSize) {
			if (!!(eeprom_read_byte((uint8_t*)(EEPROMStart + i)) & 0x80) != sentinel)
				break;
			lastIdx = i;
		}

		// last entry was found to be the most recent one, check if EEPROM is just empty
		if (lastIdx == EEPROMSize - InternalRecordSize) {
			uint8_t mask = 0xff;
			for (IndexType i = 0; i < EEPROMSize; i++)
				mask &= eeprom_read_byte((uint8_t*)(EEPROMStart + i));
			if (mask == 0xff)
				return -1;
		}

		return lastIdx;
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

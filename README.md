avr-eeprom-wear-leveling
=====

## Overview

Header-only C++ implementation of [Danny's](https://sites.google.com/site/dannychouinard/Home/atmel-avr-stuff/eeprom-longevity) algorithm of EEPROM wear leveling for AVR microcontrollers.

## Specification

* support for multiple separate EEPROM wear-leveling blocks,
* can coexist with other values stored in EEPROM (`EEMEM`),
* synchronous (not interrupt-driven) or asynchronous modes,
* protected against write corruption.

## Classes

Repository implements 2 different approaches to wear-leveling and one async version:

* EEPROMBitSentinelWearLeveling
* EEPROMByteSentinelWearLeveling
* EEPROMByteSentinelWearLevelingAsync *(requires src/eeprom_async.cpp)*

### EEPROMBitSentinelWearLeveling

Sentinel bit included in MSB of the first byte.

**Requirements:**
* MSB of the first byte of data record can't be used. For example:
```c++
struct Record {
	uint16_t val1;
	uint16_t val2;
};
```

Most significant bit of `val1` (`0x8000`) should not be used as it will be cleared. `val2` can use all bits (note the following requirement),

* record can't have all bytes equal to `0xff` as it would not allow detecting fresh EEPROM.

**Pros:**
* most space-efficient way,

**Cons:**
* one bit lost for sentinel,
* any record has to have at least 1 bit set to 0.

### EEPROMByteSentinelWearLeveling

Separate byte for sentinel.

**Pros:**
* No restrictions on Record's fields' values,

**Cons:**
* Full byte lost for the sentinel.

### EEPROMByteSentinelWearLevelingAsync

Similar to `EEPROMByteSentinelWearLeveling`, except that `save()` method executes asynchronously (using interrupts).

## API

All three classes exposes the same API:

```c++
void EEPROMByteSentinelWearLeveling::save(TRecord& record);
```
`record` - record to be saved to EEPROM,

```c++
bool EEPROMByteSentinelWearLeveling::load(TRecord& record);
```
`record` - record loaded from EEPROM, \
**return value** - true, if record was loaded successfully, false otherwise.

## Example

```c++
#include <avr/eeprom.h>
#include <EEPROMByteSentinelWearLeveling.h>

#pragma pack(1)
struct Record {
	uint16_t val1;
	uint16_t val2;
};
#pragma pack()

// EEMEM uint8_t eepromBlock[sizeof(Record) * 20]; // for bit version
EEMEM uint8_t eepromBlock[(sizeof(Record) + 1) * 20];

// EEPROMBitSentinelWearLeveling<Record, sizeof(eepromBlock), eepromBlock> eeprom;
EEPROMByteSentinelWearLeveling<Record, sizeof(eepromBlock), eepromBlock> eeprom;
// EEPROMByteSentinelWearLevelingAsync<Record, sizeof(eepromBlock), eepromBlock> eeprom;

void main() {
  Record record;
  
  if (eeprom.load(record)) {
    // record loaded
    record.val1++;
  }
  else {
    // no record loaded, EEPROM is clean
    record.val1 = 1;
    record.val2 = 0;
  }
  
  eeprom.save(record);
}
```

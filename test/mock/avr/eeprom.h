#pragma once

#include <cstdint>

#include <vector>

#include "../../tests_utils.h"

static uint8_t eeprom_read_byte(uint8_t* __p)
{
	return eepromData[(size_t)(__p - currentOffset + *currentOffset)];
}

static void eeprom_read_block(void* __dst, const void* __src, size_t __n)
{
	memcpy(__dst, &eepromData[(size_t)((uint8_t*)__src - currentOffset + *currentOffset)], __n);
}

static void eeprom_update_byte(uint8_t* __p, uint8_t __value)
{
	eepromData[(size_t)(__p - currentOffset + *currentOffset)] = __value;
}

static void eeprom_update_block(const void* __src, void* __dst, size_t __n)
{
	memcpy(&eepromData[(size_t)((uint8_t*)__dst - currentOffset + *currentOffset)], __src, __n);
}
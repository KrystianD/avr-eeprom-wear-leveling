#include <avr/interrupt.h>

static const uint8_t* eepromWriteBuffer;
static uint16_t eepromWriteRemaining;

SIGNAL(EE_RDY_vect)
{
	eepromWriteRemaining--;

	if (eepromWriteRemaining > 0) {
		eepromWriteBuffer--;

		EEAR--;
		EEDR = *eepromWriteBuffer;

		EECR = _BV(EEMWE);
		EECR = _BV(EEWE) | _BV(EERIE);
	}
	else {
		EECR = 0;
	}
}

void eeprom_update_block_async_reverse(const void* buffer, uint16_t address, uint16_t length)
{
	while (EECR & _BV(EEWE));

	uint8_t* data = (uint8_t*)buffer;
	eepromWriteBuffer = data + length - 1;
	eepromWriteRemaining = length;

	EEAR = address + length - 1;
	EEDR = *eepromWriteBuffer;

	cli();
	EECR = _BV(EEMWE);
	EECR = _BV(EEWE);
	if (eepromWriteRemaining > 1)
		EECR |= _BV(EERIE);
	sei();
}

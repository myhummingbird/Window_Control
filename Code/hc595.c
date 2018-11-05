#include <avr/io.h>
#include <stdint.h>

#include "global.h"

#define HC595_SERIN_HIGH()	HC595_SERIN_PORT |=  (1 << HC595_SERIN_BIT)
#define HC595_SERIN_LOW()	HC595_SERIN_PORT &= ~(1 << HC595_SERIN_BIT)

#define HC595_CLK_HIGH()	HC595_CLK_PORT |=  (1 << HC595_CLK_BIT)
#define HC595_CLK_LOW()		HC595_CLK_PORT &= ~(1 << HC595_CLK_BIT)

#define HC595_LATCH_HIGH()	HC595_LATCH_PORT |=  (1 << HC595_LATCH_BIT)
#define HC595_LATCH_LOW()	HC595_LATCH_PORT &= ~(1 << HC595_LATCH_BIT)


void hc595_init(void)
{
	HC595_SERIN_DDR |= (1 << HC595_SERIN_BIT);
	HC595_CLK_DDR	|= (1 << HC595_CLK_BIT	);
	HC595_LATCH_DDR |= (1 << HC595_LATCH_BIT);
	
	HC595_SERIN_LOW();
	HC595_CLK_LOW();
	HC595_LATCH_LOW();
}

void hc595_write(uint8_t *byte, uint8_t size)
{
	int8_t s = 0;
	int8_t b = 0;

	HC595_LATCH_LOW();

	for (s = (size - 1); s >= 0; s--)
	{
		for (b = 7; b >= 0; b--)
		{
			if (byte[s] & (1 << b))
				HC595_SERIN_HIGH();
			else
				HC595_SERIN_LOW();

			HC595_CLK_HIGH();
			// delay
			HC595_CLK_LOW();
		}

	}

	HC595_LATCH_HIGH();
}

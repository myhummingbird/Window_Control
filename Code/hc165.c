#include <avr/io.h>
#include <stdint.h>
#include <string.h>

#include "global.h"

#define HC165_CLK_HIGH()	HC165_CLK_PORT |=  (1 << HC165_CLK_BIT)
#define HC165_CLK_LOW()		HC165_CLK_PORT &= ~(1 << HC165_CLK_BIT)

#define HC165_LATCH_HIGH()	HC165_LATCH_PORT |=  (1 << HC165_LATCH_BIT)
#define HC165_LATCH_LOW()	HC165_LATCH_PORT &= ~(1 << HC165_LATCH_BIT)


void hc165_init(void)
{
	HC165_SEROUT_PORT	|=  (1 << HC165_SEROUT_BIT);
	HC165_SEROUT_DDR	&= ~(1 << HC165_SEROUT_BIT);
	HC165_CLK_DDR		|=  (1 << HC165_CLK_BIT	);
	HC165_LATCH_DDR		|=  (1 << HC165_LATCH_BIT);
	
	HC165_CLK_LOW();
	HC165_LATCH_HIGH();
}

void hc165_read(uint8_t *byte, uint8_t size)
{
	int8_t s = 0;
	int8_t b = 0;

	HC165_LATCH_LOW();
	// delay
	HC165_LATCH_HIGH();

	memset(byte, 0, size);

	for (s = 0; s < size; s++)
	{
		for (b = 7; b >= 0; b--)
		{
			byte[s] <<= 1;
			byte[s] |= ((HC165_SEROUT_PIN >> HC165_SEROUT_BIT) & 1);
			
			HC165_CLK_HIGH();
			// delay
			HC165_CLK_LOW();
			
		}

	}
}

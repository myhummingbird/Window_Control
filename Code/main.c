#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#include <stdint.h>

#include "global.h"

#include "adc.h"
#include "hc165.h"
#include "hc595.h"

void get_window(uint8_t n, uint8_t *an, uint8_t *in)
{
	uint8_t i;
	
	for (i = 0; i < n; i++)
			an[i] = adc_start_conversion(i);
		
	hc165_read (in , n);
}

int main(void)
{
	int i;
	
	uint8_t adc   [WINDOW_NO] = {0};
	uint8_t input [WINDOW_NO] = {0};
	uint8_t output[WINDOW_NO] = {0};
	
	DDRC = 0xFF;
	
	adc_init();
	hc165_init();
	hc595_init();
	
	output[0] = 1;
	
	hc595_write(output, WINDOW_NO);
	
	while (1)
	{
		//PORTC = adc_start_conversion(0);
		
		//get_window(WINDOW_NO, adc, input);
		
		//hc595_write(adc, WINDOW_NO);
		
		_delay_ms(500);
		hc595_write(output, WINDOW_NO);
		output[i] *= 2;
		if (output[i] == 0)
		{
			i++;
			if (i >=  WINDOW_NO)
				i = 0;
			output[i] = 1;
		}
		
	}

	return 0;
}

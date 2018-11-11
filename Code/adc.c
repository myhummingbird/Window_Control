#include <avr/io.h>
#include <stdint.h>

void adc_init(void)
{
	// select reference voltage
	// AVCC with external capacitor at AREF pin, Left adjust result
	ADMUX |= (0 << REFS1) | (1 << REFS0) | (1 << ADLAR);
	
	// set prescaller , enable ADC and ADC Interrupt enable
	ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

void adc_start(uint8_t channel)
{
	// select ADC channel
	ADMUX = (ADMUX & 0xF0) | channel;
	
	//Start conversion
	ADCSRA |= (1 << ADSC);
}

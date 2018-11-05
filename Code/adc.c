#include <avr/io.h>
#include <stdint.h>

void adc_init(void)
{
	// select reference voltage
	// AVCC with external capacitor at AREF pin, Left adjust result
	ADMUX |= (0 << REFS1) | (1 << REFS0) | (1 << ADLAR);
	
	// set prescaller and enable ADC
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (0 << ADPS1) | (1 << ADPS0);
}

uint8_t adc_start_conversion(uint8_t channel)
{
	// select ADC channel
	ADMUX = (ADMUX & 0xF0) | channel;
	
	//Start conversion
	ADCSRA |= (1 << ADSC);
	
	// wait until conversion complete ADSC=0 -> Complete
    while (ADCSRA & (1 << ADSC));
	
	// Get ADC the Result
	return ADCH;
	// ADCL
	// ADCW
}

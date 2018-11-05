#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#include <stdint.h>

#define BAUD 9600

#include <util/setbaud.h>

void uart_init(void) {
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;

#if USE_2X
    UCSRA |= _BV(U2X);
#else
    UCSRA &= ~(_BV(U2X));
#endif

    UCSRC = _BV(UCSZ1) | _BV(UCSZ0); /* 8-bit data */
    UCSRB = _BV(RXEN) | _BV(TXEN);   /* Enable RX and TX */
}

void uart_putchar(char c, FILE *stream) {
    loop_until_bit_is_set(UCSRA, UDRE); /* Wait until data register empty. */
    UDR = c;
}

char uart_getchar(FILE *stream) {
    loop_until_bit_is_set(UCSRA, RXC); /* Wait until data exists. */
    return UDR;
}

FILE uart_output = FDEV_SETUP_STREAM((void*)uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, (void*)uart_getchar, _FDEV_SETUP_READ);

FILE uart_io = FDEV_SETUP_STREAM((void*)uart_putchar, (void*)uart_getchar, _FDEV_SETUP_RW);

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
	uint8_t adc   [WINDOW_NO] = {0};
	uint8_t input [WINDOW_NO] = {0};
	uint8_t output[WINDOW_NO] = {0};
	
	uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;
	
	//DDRC = 0xFF;
	
	adc_init();
	hc165_init();
	hc595_init();
	
	while (1)
	{
		//PORTC = adc_start_conversion(0);
		
		get_window(WINDOW_NO, adc, input);
		hc595_write(adc, WINDOW_NO);
		
		printf ("\r0x%02x", adc[0]);
		_delay_ms(100);
	}

	return 0;
}

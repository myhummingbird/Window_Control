#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include <util/delay.h>

#include <stdint.h>

#define BAUD 9600

#include <util/setbaud.h>

void uart_init(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(RXEN0)  | _BV(TXEN0);   /* Enable RX and TX */
}

void uart_putchar(char c, FILE *stream) {
    loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
    UDR0 = c;
}

char uart_getchar(FILE *stream) {
    loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
    return UDR0;
}

FILE uart_output = FDEV_SETUP_STREAM((void*)uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, (void*)uart_getchar, _FDEV_SETUP_READ);

FILE uart_io = FDEV_SETUP_STREAM((void*)uart_putchar, (void*)uart_getchar, _FDEV_SETUP_RW);

#include "global.h"

#include "adc.h"
#include "hc165.h"
#include "hc595.h"

void init(void)
{
	uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;
	
	//adc_init();
	hc165_init();
	hc595_init();
	
	// init global alarm as output
	GLOBAL_ALARM_DDR |= (1 << GLOBAL_ALARM_BIT);
	GLOBAL_ALARM_INACTIVE();
	
	// init global bas as input
	GLOBAL_BAS_PORT |=  (1 << GLOBAL_BAS_BIT);	// pull-up
	GLOBAL_BAS_DDR  &= ~(1 << GLOBAL_BAS_BIT);
}

void get_window(uint8_t n, uint8_t *an, uint8_t *in)
{
	uint8_t i;
	
//	for (i = 0; i < n; i++)
//		an[i] = adc_start_conversion(i);
		
	hc165_read (in , n);
}

typedef union {
	uint8_t byte;
	struct {
		uint8_t config	: 2;
		uint8_t 		: 2;
		uint8_t up_down	: 2;
		uint8_t bas		: 1;
		uint8_t 		: 1;
	};
} input_t;

#define CONF_2CH	0
#define CONF_LCH	1
#define CONF_RCH	2
#define CONF_NONE	3

#define SW_STOP 0
#define SW_DOWN 1
#define SW_UP	2

typedef union {
	uint8_t byte;
	struct {
		uint8_t 		: 1;
		uint8_t run		: 1;
		uint8_t dir		: 1;
		uint8_t 		: 1;
		uint8_t alarm	: 1;
		uint8_t 		: 3;
	};
} output_t;

#define RUN_INACTIVE	0
#define RUN_ACTIVE		1

#define DIR_RESET	0
#define DIR_UP		0
#define DIR_DOWN	1

#define ALARM_INACTIVE	0
#define ALARM_ACTIVE	1

#define CURRENT_1CH		0x60	// 96
#define CURRENT_2CH		0xC0	// 192

uint32_t ms_tick = 0;
uint32_t  s_tick = 0;
bool tick_flag = false;

uint16_t preload_timer;

// ms_tick
ISR(TIMER1_OVF_vect)
{
	TCNT1 = preload_timer;
	
	ms_tick++;
	if (ms_tick == 1000)
	{
		ms_tick = 0;
		s_tick++;
	}
	
	if ((ms_tick % 100) == 0)
		tick_flag = true;
}

void timer_init(void)
{
	TCCR1A = 0; // initialize timer1
	TCCR1B = 0; //mode 0
	TCCR1B |= (1 << CS10); // no prescaler
	TCNT1 = preload_timer;
	TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt
}

int main(void)
{
	uint8_t adc      [WINDOW_NO] = {0x80, 0x80, 0x80, 0x80, 0x80};
	
	uint8_t input_n  [WINDOW_NO] = {0xff, 0xff, 0xff, 0xff, 0xff};
	uint8_t input_n_1[WINDOW_NO] = {0xff, 0xff, 0xff, 0xff, 0xff};
	bool    input_dif[WINDOW_NO] = {false,false,false,false,false};
	
	uint8_t output   [WINDOW_NO] = {0x00, 0x00, 0x00, 0x00, 0x00};
	
	input_t  *in;
	output_t *out;
	uint8_t current_limit;
	
	uint8_t index;
	
	preload_timer = 65535 - (F_CPU / 1000) + 1; // preload timer count for 1ms
	
	//wdt_reset();
    //wdt_disable();
	
	init();
	
	// all off when init
	hc595_write (output, WINDOW_NO);
	
	timer_init();
	sei();
	
	while (1)
	{
		// loop 10 Hz
		if (tick_flag == true)
		{
			tick_flag = false;
			
			get_window(WINDOW_NO, adc, input_n);
			memset (output, 0, WINDOW_NO);
			
			for (index = 0; index < WINDOW_NO; index++)
			{
				in  = (input_t* )(input_n + index);
				out = (output_t*)(output  + index);
/*				
				if (in->config == CONF_2CH) // all on
				{
					current_limit = CURRENT_2CH;
				}
				else // CONF_LCH or CONF_RCH
				{
					current_limit = CURRENT_1CH;
				}
				
				// stop when current reach to limit
				if (adc[index] >= current_limit)
				{
					out->dir = DIR_RESET;
					out->run = RUN_INACTIVE;
				}
*/				
				// debounce algorithm
/*				if (input_n_1[index] != input_n[index])
				{
					// first for change event set_flag
					// if has event again before debounce re-set_flag
					input_dif[index] = true;
					input_n_1[index] = input_n[index];
				}
				else
				{
					// if still current change, event has pass debounce
					if (input_dif[index] == true)
					{
						input_dif[index] = false;
						
						// check config switch
						if (in->config == CONF_NONE) // all off
						{
							// disable this window channel
							out->byte = 0;
						}
						else
						{
							// check control
							if (in->up_down == SW_UP)
							{
								out->dir = DIR_UP;
								out->run = RUN_ACTIVE;
							}
							else if (in->up_down == SW_DOWN)
							{
								out->dir = DIR_DOWN;
								out->run = RUN_ACTIVE;
							}
							else if (in->up_down == SW_STOP)
							{
								out->dir = DIR_RESET;
								out->run = RUN_INACTIVE;
							}
							else {}
						}
					}
				}*/
				
				if (in->up_down == SW_UP)
				{
					out->dir = DIR_UP;
					out->run = RUN_ACTIVE;
				}
				else if (in->up_down == SW_DOWN)
				{
					out->dir = DIR_DOWN;
					out->run = RUN_ACTIVE;
				}
				else if (in->up_down == SW_STOP)
				{
					out->dir = DIR_RESET;
					out->run = RUN_INACTIVE;
				}
			}
			
			hc595_write (output, WINDOW_NO);
			//printf ("\r%lu", ms_tick);
			printf ("\r0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", input_n[0], input_n[1], input_n[2], input_n[3], input_n[4]);
		}
		
		//printf ("\r0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", adc[0], adc[1], adc[2], adc[3], adc[4]);
	}

	return 0;
}

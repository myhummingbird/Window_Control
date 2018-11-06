#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdint.h>

#include <avr/io.h>

// define for HC595 interface
#define HC595_SERIN_DDR		DDRD
#define HC595_SERIN_PORT	PORTD
#define HC595_SERIN_BIT		PIN2

#define HC595_CLK_DDR		DDRD
#define HC595_CLK_PORT		PORTD
#define HC595_CLK_BIT		PIN3

#define HC595_LATCH_DDR		DDRD
#define HC595_LATCH_PORT	PORTD
#define HC595_LATCH_BIT		PIN4

// define for HC165 interface
#define HC165_SEROUT_DDR	DDRD
#define HC165_SEROUT_PORT	PIND
#define HC165_SEROUT_BIT	PIN7

#define HC165_CLK_DDR		DDRD
#define HC165_CLK_PORT		PORTD
#define HC165_CLK_BIT		PIN6

#define HC165_LATCH_DDR		DDRD
#define HC165_LATCH_PORT	PORTD
#define HC165_LATCH_BIT		PIN5

// User Application define
#define WINDOW_NO	5

#define GLOBAL_ALARM_DDR	DDRB
#define GLOBAL_ALARM_PORT	PORTB
#define GLOBAL_ALARM_BIT	PIN1

#define GLOBAL_ALARM_ACTIVE()	GLOBAL_ALARM_PORT |=  (1 << GLOBAL_ALARM_BIT)
#define GLOBAL_ALARM_INACTIVE()	GLOBAL_ALARM_PORT &= ~(1 << GLOBAL_ALARM_BIT)

#define GLOBAL_BAS_DDR		DDRB
#define GLOBAL_BAS_PORT		PORTB
#define GLOBAL_BAS_PIN		PINB
#define GLOBAL_BAS_BIT		PIN0

#define GLOBAL_BAS_READ()	((GLOBAL_BAS_PIN >> GLOBAL_BAS_BIT) & 1)

// User - Window Input
// User - Window Output

#endif // __GLOBAL_H__

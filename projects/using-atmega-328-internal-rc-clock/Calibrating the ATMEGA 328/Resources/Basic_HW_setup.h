
#define T0_delay_10ms 5,178
#define T1_delay_100ms 5,0xFCF2

/*****************************************************************************/
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>

/*****************************************************************************/
#define SW_reset; {wdt_enable(WDTO_30MS);while(1);}

/*****************************************************************************/
#define setup_HW;\
setup_watchdog;\
USART_init(0,16);\
nop_delay(10);\
ADMUX |= (1 << REFS0);\
Timer_T0_10mS_delay_x_m(1);\
Unused_I_O;

/*****************************************************************************/
#define setup_watchdog;\
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;

#define wdr()  __asm__ __volatile__("wdr")

/*****************************************************************************/
#define Unused_I_O;\
MCUCR &= (~(1 << PUD));\
DDRB &= (~((1 << PB2)|(1 << PB7)));\
DDRC &= (~((1 << PC0)|(1 << PC1)|(1 << PC2)));\
DDRD &= (~((1 << PD3)|(1 << PD4)|(1 << PD5)|(1 << PD6)));\
PORTB |= ((1 << PB2)|(1 << PB7));\
PORTC |= ((1 << PC0)|(1 << PC1)|(1 << PC2));\
PORTD |= ((1 << PD3)|(1 << PD4)|(1 << PD5)|(1 << PD6));
//Unused IO varies from design to design. Here unused ports are set to weak pull up.

/*****************************************************************************/
#define Atmega_328_OSC_CAL;\
eeprom_write_byte((uint8_t*)0x3FD, OSCCAL);\
if ((eeprom_read_byte((uint8_t*)0x3FE) > 0x0F)\
&&  (eeprom_read_byte((uint8_t*)0x3FE) < 0xF0) && (eeprom_read_byte((uint8_t*)0x3FE)\
== eeprom_read_byte((uint8_t*)0x3FF))) {OSCCAL = eeprom_read_byte((uint8_t*)0x3FE);\
cal_factor=1;} else {cal_factor=0;}


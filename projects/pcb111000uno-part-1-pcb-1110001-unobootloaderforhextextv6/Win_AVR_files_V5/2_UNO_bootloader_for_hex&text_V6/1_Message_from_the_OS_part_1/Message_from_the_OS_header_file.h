


void send_byte_with_Ack(char);
void send_byte_with_Nack(char);
char receive_byte_with_Ack(void);
char receive_byte_with_Nack(void);
void Char_to_PC(char);


#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>


#define T1_delay_50ms 5,0xFE78
#define T1_delay_100ms 5,0xFCF2
#define T1_delay_250ms 5,0xF85F
#define T1_delay_500ms 5,0xF0BE

#define T0_delay_33ms 5,0



/*****************************************************************************/
#define SW_reset {wdt_enable(WDTO_30MS);while(1);}



/*****************************************************************************/
#define setup_HW \
setup_watchdog;\
Unused_I_O;\
set_up_I2C;\
ADMUX |= (1 << REFS0);\
Timer_T0_sub(T0_delay_33ms);\
USART_init(0,16);



/*****************************************************************************/
#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;

#define wdr()  __asm__ __volatile__("wdr")



/*****************************************************************************/
#define set_up_I2C \
TWAR = 0x02;



/*****************************************************************************/
#define Unused_I_O \
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC  = 0xFF;\
PORTD = 0xFF;



/*****************************************************************************/
#define waiting_for_I2C_master \
TWCR = (1 << TWEA) | (1 << TWEN);\
while (!(TWCR & (1 << TWINT)));\
TWDR;

#define clear_I2C_interrupt \
TWCR = (1 << TWINT);



/*****************************************************************************/
#define set_up_switched_inputs \
MCUCR &= (~(1 << PUD));\
DDRD &= (~((1 << PD2)|(1 << PD7)));\
PORTD |= ((1 << PD2) | (1 << PD7));\
DDRB &= (~(1 << PB2));\
PORTB |= (1 << PB2);


#define switch_1_down	((PIND & 0x04)^0x04)
#define switch_1_up		(PIND & 0x04)
#define switch_2_down	((PIND & 0x80)^0x80)
#define switch_2_up		(PIND & 0x80)


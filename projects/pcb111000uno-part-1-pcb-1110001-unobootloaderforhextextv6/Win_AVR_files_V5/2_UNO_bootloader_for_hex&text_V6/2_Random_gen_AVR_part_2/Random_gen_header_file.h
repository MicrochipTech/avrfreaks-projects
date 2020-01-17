
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>





void I2C_Tx_2_integers(unsigned int, unsigned int);
void Timer_T1_sub(char, unsigned int);
unsigned int PRN_16bit_GEN(unsigned int );
void USART_init (unsigned char, unsigned char);
void I2C_Tx_LED_dimmer(void);



#define T1_delay_50ms 5,0xFE78
#define T1_delay_100ms 5,0xFCF2
#define T1_delay_250ms 5,0xF85F
#define T1_delay_500ms 5,0xF0BE


/*****************************************************************************/



/*****************************************************************************/
#define SW_reset {wdt_enable(WDTO_30MS);while(1);}

/*****************************************************************************/
#define setup_HW \
setup_watchdog;\
Unused_I_O;\
set_up_switched_inputs;\
set_up_I2C;\
ADMUX |= (1 << REFS0);\
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

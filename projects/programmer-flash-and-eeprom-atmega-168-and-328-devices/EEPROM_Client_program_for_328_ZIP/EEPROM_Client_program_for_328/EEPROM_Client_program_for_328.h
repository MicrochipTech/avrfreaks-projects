
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>


#include "Resources\Basic_IO_and_Timer_subs.c"
#include "Resources\ASKII_subroutines.c"


void Text_from_EEPROM(int);
char Char_from_EEPROM(int);
long L_num_from_EEPROM(int);
int I_num_from_EEPROM(int);


#define message_1 0x5
#define message_2 0x5D
#define message_3 0x81
#define message_4 0xE0
#define message_5 0x13A
#define message_6 0x181
#define message_7 0x1EF

#define offset 0

#define I_array	0x229+offset
#define L_array	0X231+offset
#define Ch_array 0x241+offset
#define I_data 0x245+offset
#define L_data 0x2B5+offset
#define T_data 0x2F5+offset
#define large_nos 0x325+offset


#define T1_delay_1sec 5,0xE17B



/************************************************************************************************************************************/
#define setup_168_HW;\
setup_watchdog;\
cal_Atmega168;\
ADMUX |= (1 << REFS0);\
USART_init(0,16);\
Set_LED_ports;\
LEDs_off;\
Unused_I_O;\
Config_Xtal_port;



/************************************************************************************************************************************/
#define setup_328_HW;\
setup_watchdog;\
cal_Atmega328;\
ADMUX |= (1 << REFS0);\
USART_init(0,16);\
Set_LED_ports;\
LEDs_off;\
Unused_I_O;\
Config_Xtal_port;



/************************************************************************************************************************************/
#define wdr()  __asm__ __volatile__("wdr")
#define Version "Atmega_programmer_V1.2\r\n" 



#define setup_watchdog;\
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;



/************************************************************************************************************************************/
#define cal_Atmega168;\
if ((eeprom_read_byte((uint8_t*)0x1F7) > 0x0F)\
&&  (eeprom_read_byte((uint8_t*)0x1F7) < 0xF0) && (eeprom_read_byte((uint8_t*)0x1F7)\
== eeprom_read_byte((uint8_t*)0x1F8))) {OSCCAL = eeprom_read_byte((uint8_t*)0x1F7);cal_factor=1;}\
else cal_factor=0;




/************************************************************************************************************************************/
#define cal_Atmega328;\
if ((eeprom_read_byte((uint8_t*)0x3FE) > 0x0F)\
&&  (eeprom_read_byte((uint8_t*)0x3FE) < 0xF0) && (eeprom_read_byte((uint8_t*)0x3FE)\
== eeprom_read_byte((uint8_t*)0x3FF))) {OSCCAL = eeprom_read_byte((uint8_t*)0x3FE); cal_factor=1;}\
else cal_factor=0;


/************************************************************************************************************************************/
#define Set_LED_ports;	DDRB = (1 << DDB0) | (1 << DDB1);
#define LEDs_off; PORTB &= (~((1 << PB0)|(1 << PB1)));



/************************************************************************************************************************************/
#define Unused_I_O;\
MCUCR &= (~(1 << PUD));\
\
DDRB &= (~(1 << PB2));\
PORTB |= ((1 << PB2));\
\
DDRC &= (~((1 << PC0)|(1 << PC1)|(1 << PC2)|(1 << PC4)|(1 << PC5)));\
PORTC |= ((1 << PC0)|(1 << PC1)|(1 << PC2)|(1 << PC4)|(1 << PC5));\
\
DDRD &= (~((1 << PD2)|(1 << PD3)|(1 << PD4)|(1 << PD5)|(1 << PD6)|(1 << PD7)));\
\
PORTD |= ((1 << PD2)|(1 << PD3)|(1 << PD4)|(1 << PD5)|(1 << PD6)|(1 << PD7));



#define Config_Xtal_port;\
ASSR = (1 << AS2);	

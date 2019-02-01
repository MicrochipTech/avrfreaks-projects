
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>

void numeric_entry(void);
void segment_illumination(void);
void intensity_control(void);
void pcb_test(void);

int letter, digit_num;
volatile int disp_ptr;
volatile char mode; 
volatile int buf_ptr;
volatile char T0_interupt_cnt;
char display_buf[8]; 


/********************************Device setup*******************************************/
#define setup_HW;\
reset_digits;\
reset_segments;\
ADMUX |= (1 << REFS0);\
setup_watchdog;\
Atmega_328_OSC_CAL;\
set_digit_drivers;\
USART_init(0,16);


#define setup_watchdog \
if (MCUSR & (1 << WDRF))watch_dog_reset = 1;\
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;

#define wdr()  __asm__ __volatile__("wdr")
#define SW_reset {wdt_enable(WDTO_30MS);while(1);}



/*****************Check for previous user calibration of the 8MHz oscillator*****************/
#define Atmega_328_OSC_CAL;\
eeprom_write_byte((uint8_t*)0x3FD, OSCCAL);\
if ((eeprom_read_byte((uint8_t*)0x3FE) > 0x0F)\
&&  (eeprom_read_byte((uint8_t*)0x3FE) < 0xF0) && (eeprom_read_byte((uint8_t*)0x3FE)\
== eeprom_read_byte((uint8_t*)0x3FF))) {OSCCAL = eeprom_read_byte((uint8_t*)0x3FE);\
cal_factor=1;} else {cal_factor=0;}


/******************Specify which pins are used to drive the individual digits********************/
#define	digit_3		PORTB &= (~(1 << PB0));
#define	digit_2		PORTB &= (~(1 << PB2));
#define	digit_1		PORTB &= (~(1 << PB3));
#define	digit_0		PORTB &= (~(1 << PB4));

#define	digit_7		PORTB &= (~(1 << PB5));
#define	digit_6		PORTC &= (~(1 << PC0));
#define	digit_5		PORTC &= (~(1 << PC1));
#define	digit_4		PORTC &= (~(1 << PC2));
#define blank		{reset_segments;}	



/**********************Specify which pins are used to drive the individual segments********************/
#define	seg_a 	(1 << PB1)
#define	seg_b 	(1 << PD2)
#define	seg_c 	(1 << PD3)
#define	seg_d 	(1 << PD4)
#define	seg_e 	(1 << PD5)
#define	seg_f 	(1 << PD6)
#define	seg_g 	(1 << PD7)


/***********************************Set driver pins to output******************************************/
#define set_digit_drivers;\
DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3) | (1 << DDB4) | (1 << DDB5);\
DDRC |= (1 << DDC0) | (1 << DDC1) | (1 << DDC2);\
DDRD |= (1 << DDD2) | (1 << DDD3) | (1 << DDD4) | (1 << DDD5) | (1 << DDD6) | (1 << DDD7);


/*************************Set digit and segment drivers high to temporarily blank display*****************************/
#define reset_digits; PORTB |= ((1 << PB0) | (1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5));\
PORTC |= ((1 << PC0) | (1 << PC1) | (1 << PC2));

#define reset_segments; PORTB |= (seg_a );\
PORTD |= (seg_b | seg_c | seg_d | seg_e | seg_f | seg_g);


/**********************Permanently Blank display**********************************/
#define clear_display_buffer; for(int m = 0; m<=7; m++)display_buf[m] = 0;



/**********************Define digits in terms of their segments******************************************/
#define ONE		PORTD &= (~(seg_e | seg_f));
#define one 	PORTD &= (~(seg_b | seg_c));
#define two 	PORTB &= (~(seg_a)); PORTD &= (~(seg_b | seg_d | seg_e | seg_g));
#define three 	PORTB &= (~(seg_a)); PORTD &= (~(seg_b | seg_c | seg_d | seg_g));
#define four 	PORTD &= (~(seg_b | seg_c | seg_f | seg_g));
#define five 	PORTB &= (~(seg_a)); PORTD &= (~(seg_c | seg_d | seg_f | seg_g));
#define six 	PORTD &= (~(seg_c | seg_d | seg_e | seg_f | seg_g));
#define seven 	PORTB &= (~(seg_a)); PORTD &= (~(seg_b | seg_c ));
#define eight 	PORTB &= (~(seg_a));  PORTD &= (~(seg_b | seg_c | seg_d | seg_e | seg_f | seg_g));
#define nine	PORTB &= (~(seg_a)); PORTD &= (~(seg_b | seg_c | seg_f | seg_g));
#define zero	PORTB &= (~(seg_a)); PORTD &= (~(seg_b | seg_c | seg_d | seg_e | seg_f ));
#define decimalP	PORTB &= (~(seg_a));	PORTD &= (~(seg_b | seg_e | seg_f | seg_g ));


/*************************Define timer delays***********************************************************************/
#define T0_delay_2ms 3,0
#define T0_delay_500us 3,192
#define T0_delay_125us 3,240

#define T1_delay_100ms 5,0xFCF2
#define T1_delay_500ms 5, 0xF0C3
#define T1_delay_10ms 3, 0xF63C




		




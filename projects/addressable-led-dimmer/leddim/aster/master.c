/*
 * pwmlcd.c
 *
 * Created: 5/6/2011 12:16:25 PM
 *  Author: aaron_r
 */ 

//Aaron Rackoff
//Spectrum Lighting
//994 Jefferson st
//Fall River, MA 02721
//Last Revised: 30 April 2011
//Program Description:
// Inputs : Serial DMX stream
// Output : Four independently controllable PWM channels
// Output :

// Pin descriptions for ATmega328p PDIP-28
// MAIN PIN FUNCTIONS:                       (ISP programmer use)
// **************************************************************
// Pin  1: 10k resistor to ground            (connect to ISP RST)
// Pin  2: RX, buffered DMX input
// Pin  3:
// Pin  4:
// Pin  5: LED PWM output (OCR2B): blue
// Pin  6:
// Pin  7: VCC  (5V)                         (connect to ISP VCC)
// Pin  8: GND                               (connect to ISP GND)
// Pin  9: 
// Pin 10: RCLK (74hc595 pin 12)
// Pin 11: LED PWM output (OCR0B): red
// Pin 12: LED PWM output (OCR0A): white
// Pin 13: SERI (74hc595 pin 14)
// Pin 14: LCD READ/WRITE line
// Pin 15: CLRR (74hc595 pin 11)
// Pin 16: SCLK (74hc595 pin 10)
// Pin 17: LED PWM output (OCR2A): green     (used for MOSI line)
// Pin 18:                                   (used for MISO line)
// Pin 19:                                   (used for SCLK line)
// Pin 20: AVCC (5V) **optional
// Pin 21: AREF (5V) **optional
// Pin 22: GND       **optional
// Pin 23: RED-knob
// Pin 24: GRN-knob
// Pin 25: BLU-knob
// Pin 26: WHT-knob
// Pin 27: 
// Pin 28: 

#ifndef F_CPU
#define F_CPU 8000000UL
#endif


#include "lcd.h"

#include <stdlib.h>
#include <avr/io.h>

#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h> 

//UTILITY INTS
int i,j,k;
//END UTILITY INTS

//ADC DEFINITIONS 
static uint8_t adcval  = 0x00;
static uint8_t mode    = 0x00;
//END ADC DEFINITIONS

//PWM DEFINITIONS
static uint8_t red     = 0x00;
static uint8_t grn     = 0x00;
static uint8_t blu     = 0x00;
static uint8_t wht     = 0x00;
static uint8_t adr     = 0x00;
static uint8_t cto     = 0x00;
static uint8_t dim     = 0x00;
//END PWM DEFINITIONS

int adc_reads(int colour){
	ADMUX    = colour;
	ADMUX   |= (1<<REFS0);  //use AVcc
	ADMUX   |= (1<<ADLAR);  //8 bit resolution
	ADCSRA  |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);//prescale
	ADCSRA  |= (1<<ADEN);  //AD ENable
	ADCSRA  |= (1<<ADSC);
	_delay_ms(1);
	adcval = ADCH;
	return adcval;
};


void upd_mode1(void){
	lcd_gotoxy( 0,0);//move cursor to line 1
	lcd_puts( "Rd: Gr: Bl: Wh:");
	
    char buffer[7];
	lcd_gotoxy( 0,1);//move cursor to line 2
	itoa( red , buffer, 10);/* convert integer into string */
	if     (red< 10){lcd_puts("  ");lcd_puts(buffer);}
	else if(red<100){lcd_putc(' ');lcd_puts(buffer);}
	else             {lcd_puts(buffer);};/* put converted string to display */	
		
	lcd_gotoxy( 4,1);//move cursor to line 2
	itoa( grn , buffer, 10);/* convert integer into string */
	if     (grn< 10){lcd_puts("  ");lcd_puts(buffer);}
	else if(grn<100){lcd_putc(' ');lcd_puts(buffer);}
	else             {lcd_puts(buffer);};/* put converted string to display */	
		
	lcd_gotoxy( 8,1);//move cursor to line 2
	itoa( blu , buffer, 10);/* convert integer into string */
	if     (blu< 10){lcd_puts("  ");lcd_puts(buffer);}
	else if(blu<100){lcd_putc(' ');lcd_puts(buffer);}
	else             {lcd_puts(buffer);};/* put converted string to display */	
	
	lcd_gotoxy(12,1);//move cursor to line 2
	itoa( wht , buffer, 10);/* convert integer into string */
	if     (wht< 10){lcd_puts("  ");lcd_puts(buffer);}
	else if(wht<100){lcd_putc(' ');lcd_puts(buffer);}
	else             {lcd_puts(buffer);};/* put converted string to display */	
	_delay_ms(20);
};//end upd_mode1()

void upd_mode0(void){
	char buffer[7];
	lcd_clrscr();
	lcd_gotoxy( 0,0);//move cursor to line 1
	lcd_puts("Fixture: ");
	lcd_gotoxy(0,1);
	itoa( adr , buffer, 10);/* convert integer into string */
	if     (adr< 10){lcd_puts("  ");lcd_puts(buffer);}
	else if(adr<100){lcd_putc(' ');lcd_puts(buffer);}
	else             {lcd_puts(buffer);};
	_delay_ms(1000);
};//end upd_mode0();

uint8_t lonib(int data, int color){
	uint8_t output = (color<<4)+0xC0;
	output |= (data>>4);
	return output;
}//end lonib();
uint8_t hinib(int data, int color){
	uint8_t output = (color<<4)+0x80;
	output |= (data&0x0f);
	return output;
}//end hinib();

void dat_write(int a, int b, int c, int d, int address){
	uint8_t buff[9];
	address &= 0x3f;//6 bits, just to be sure!
	buff[0x00]=address;
	buff[0x01]=lonib(a,0);
	buff[0x02]=address;
	buff[0x03]=hinib(a,0);
	buff[0x04]=address;
	buff[0x05]=lonib(b,1);
	buff[0x06]=address;
	buff[0x07]=hinib(b,1);
	buff[0x08]=address;
	buff[0x09]=lonib(c,2);
	buff[0x0a]=address;
	buff[0x0b]=hinib(c,2);
	buff[0x0c]=address;
	buff[0x0d]=lonib(d,3);
	buff[0x0e]=address;
	buff[0x0f]=hinib(d,3);
	for(k=0;k<16;k++){
		while((UCSR0A & (1<<UDRE0))==0);
		UDR0 = buff[k];
	};//end for
};//end dat_write()

int main(void){
/* initialize display, cursor off */
	_delay_ms(1000);
    lcd_init(LCD_DISP_ON);
	_delay_ms(1000);
	lcd_clrscr();
	lcd_gotoxy( 0,0);//move cursor to line 1
	lcd_puts("PORT_0x00");
	lcd_gotoxy( 0,1);//move cursor to line 2
	lcd_puts("hello world");
	_delay_ms(1000);
	lcd_clrscr();
	UBRR0H = 0;
	UBRR0L = 0x01;
	/* Enable receiver and interrupt*/
	UCSR0B = (1<<TXEN0);
	UCSR0C = (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);
	DDRD   |= 0x02;;
	while(1){
		for(j=0;j<5;j++){
			if     (j==0){
				cto = adc_reads(j);
				if((cto>>6) == adr){adr = (cto>>6);}
				else{adr=(cto>>6);upd_mode0();};
			}
			else if(j==1){red = adc_reads(j);}
			else if(j==2){grn = adc_reads(j);}
			else if(j==3){blu = adc_reads(j);}
			else if(j==4){wht = adc_reads(j);}
		};//end for
		dat_write(red,grn,blu,wht,adr);
		upd_mode1();
	};//end while
};//end main



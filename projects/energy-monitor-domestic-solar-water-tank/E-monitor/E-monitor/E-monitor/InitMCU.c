#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#define F_CPU 16000000UL 
#include<avr/interrupt.h>
void Init (void);



void Init (void)
{
	
	// Device Name = ATmega32 16PU

	//
	// PORTA variable initializations for PB Soft keys
	//
	// PORTA0 for 1: __PA0 as Input with pullups - Reserved for pin PA0
	// PORTA1 for 1: __PA1 as Input with pullups - Reserved for pin PA1
	// PORTA2 for 1: __PA2 as Input with pullups - Reserved for pin PA2
	// PORTA3 for 1: __PA3 as Input with pullups - Reserved for pin PA3
	// PORTA4 for 1: __PA4 as Input with pullups - Reserved for pin PA4
	// PORTA5 for 1: __PA5 as Input with pullups - Key
	// PORTA6 for 1: __PA6 as Input with pullups - key
	// PORTA7 for 1: __PA7 as Input with pullups - key
	PORTA = 0xFF;	// pullups


	//
	// PORTB  LEDs1-3 status indicator TWI for debugging

	DDRB = 0xFF;
	PORTB = 0;

	//
	// PORTC IC2 Bus Display
	//
	// PORTC0 for 1: __SCL as Input - Reserved for pin SCL
	// PORTC1 for 1: __SDA as Input - Reserved for pin SDA
	PORTC = 3;						// pullups

	//
	// PORTD variable initializations STICK controller
	//
	// PORTD1 for 1: __TXD as Output - Reserved for pin TXD
	// PORTD5 for 1: __PD5 as Output - close file
	// PORTD6 for 1: __PD6 as Output - LED 1 sec
	// PORTD7 for 1: __PD7 as Input - 1 wire bus
	
	DDRD = 0b01100010;	//98
	PORTD = 98;

	//
	// TIMER_COUNTER_1 timebase for RTC
	//
	// Clock Select = clk / 8 (From prescaler) 2Mhz
	// Output Compare Register A = 50000	
	// Output Compare Match Interrupt Enable A = Enabled
	// Output Compare Register A Time = 25 msec  40Hz
	TIMSK = (1<<OCIE1A);
	TCCR1B = (1<<CS11)|(1<<WGM12);	// CTC mode OCR1A
	OCR1A = 0xC350;
	//
	// TWI
	//
	// SCL Clock Frequency = 9.9khz
	// Prescaler = 4
	// Bitrate = 100
	// F = 16Mhz/16+(2*100)*4
	TWBR = 100;
	TWSR = 1;		// prescaler value = 4
	TWCR = 0x04;	// (1<<TWEN)
	//
	// USART0
	//
	// Transmitter Enable = Enabled
	// Baud Rate Register = 51
	//     (9600 baud, 0,2 %error)
	UCSRC = 0b10000110;		// 8bit,1STbit
	
	UBRRL = 103;		// RSEL,baud rate .2% error
	UCSRB = 0x08;	// (1<<TXEN0)
	sei();
}




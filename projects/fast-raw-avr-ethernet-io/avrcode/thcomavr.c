
// Test for high-speed ethernet communication
// One-byte packet is used to control a PWM output on the AVR. The packet
// is returned to the PC.
// (c) Rogier Schouten, GPL V2


#include <avr/io.h>
#include <avr/interrupt.h>
#ifndef F_CPU
#define F_CPU 12500000
#endif
#include <util/delay.h>
#include "enc28j60.h"

// LED settings
#define LED_PIN PB1

// Ethernet settings
static uint8_t mymac[6] = {0x52,0x4f,0x47,0x49,0x45,0x52};
#define BUFFER_SIZE 1

// Ethernet runtime
static uint8_t recvbuf[BUFFER_SIZE];
static uint8_t recvlen;
static uint8_t sendbuf[BUFFER_SIZE];

// PWM settings
#define PWM_PIN_0 PD0

// PWM runtime
static uint8_t pwmState = 0;
static uint8_t pwmSetpoint0 = 0; // setpoint 0/195th ... 256/195th ms

ISR(TIMER0_COMPA_vect)
{
	switch (pwmState)
	{
	case 0:
		{  // 50Hz period is over, output HIGH
			PORTD |= (1 << PWM_PIN_0);
			TCCR0B = (1 << CS02); // clock division by 256
			OCR0A = 49;	// 49*256 clock ticks = 1ms
			pwmState = 1;
			break;
		}
	case 1:
		{
			// First millisecond is over, count toward setpoint
			TCCR0B = ((1 << CS01) | (1 << CS00)); // clock division by 64
			OCR0A = pwmSetpoint0; // setpoint * 1/195th ms
			pwmState = 2;
			break;
		}
	default:
		{	// Setpoint time is over, output LOW
			PORTD &= ~(1 << PWM_PIN_0);
			TCCR0B = ((1 << CS02) | (1 << CS00)); // clock division by 1024
			OCR0A = 231 - (pwmSetpoint0 >> 4); // 19ms - setpoint
			pwmState = 0;
			break;
		}
	}
}


int main(void)
{
	// Set the clock prescaler to divide-by-1
	CLKPR=(1<<CLKPCE);
	CLKPR=0;
	
	_delay_ms(20);
	
	// initialize enc28j60
	enc28j60Init(mymac);
	_delay_ms(50);
		
	// initialize I/O ports
	DDRB |= _BV(LED_PIN);
	PORTB |= _BV(LED_PIN);
	DDRD |= (1 << PWM_PIN_0);
	PORTD &= ~(1 << PWM_PIN_0);

	// initialize timer-0
	TCCR0A = 1 << WGM01;    // "Clear Timer on Compare match" mode
	TCCR0B = (1 << CS02); 	// clock division by 1024
	OCR0A = 220; 				// 18ms
	TIMSK0 = 1 << OCIE0A; 	// enable output compare A interrupt
	TCNT0 = 0;

	uint8_t ledon = 0;

	sei();
	while(1)	
	{
		// get the next new packet:
		recvlen = enc28j60PollPacket(BUFFER_SIZE, recvbuf);
		if (recvlen != 0)
		{
			// toggle LED to acknowledge reception of packet
			if (ledon)
			{
				PORTB|= (1<<PB1);
				ledon=0;
			}
			else
			{
				PORTB &= ~(1<<PB1);
				ledon=1;
			}
			// set received setpoint
			pwmSetpoint0 = recvbuf[0];
			
			// return the last value to the sender
			sendbuf[0] = pwmSetpoint0;			
			enc28j60TransmitPacket(BUFFER_SIZE, sendbuf);
		}
	}
	return (0);
}



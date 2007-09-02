/** 
 * Converter from UART (38400 baud) to Midi (31250 baud) (bidirectional)
 * Uses ATmega8 at 8 MHz, compile with WinAVR_2007_xxxxx or compatible.
 *
 * Author: 2007-08-31: Lutz Lisseck (lutz.lisseck at gmx.de)
 *
 * Shows the usage of a full-duplex interrupt-driven software-UART 
 * written in C using Timer1
 *
 * Shows the usage of a felxible timing-system using Timer0 and a
 * 32-bit Softwaretimer.
 *
 * Shows how to implement a simple ringbuffer (for the data-direction from
 * UART to Midi, 38400 baud -> 31250 baud)
 */

#include <avr/io.h> 
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>
#include "hardware.h"
#include "lutzmacro.h"
#include "suart.h"
#include "timer.h"

#define MIDI_BAUDRATEVAL 31250
#define MIDI_BAUDRATE (int)(((F_CPU / (MIDI_BAUDRATEVAL * 16.0))+.5) - 1)

/* Initialize Hardware-UART */
void MIDI_UART_Init( unsigned int baudrate )
{
	/* Set the baud rate */
	MIDI_UBRRH = (unsigned char) (baudrate>>8);                  
	MIDI_UBRRL = (unsigned char) baudrate;
	
	/* Enable UART receiver and transmitter */
	MIDI_UCSRB = ( ( 1 << MIDI_RXEN ) | ( 1 << MIDI_TXEN ) ); 
	MIDI_UCSRA = _BV(MIDI_TXC);
	
	/* Set frame format: 8 data 1stop */
	MIDI_UCSRC = (1<<MIDI_URSEL)|(1<<MIDI_UCSZ1)|(1<<MIDI_UCSZ0);        
}


/* Read and write functions */
unsigned char MIDI_UART_Receive( void )
{
	/* Wait for incomming data */
	while ( !(MIDI_UCSRA & (1<<MIDI_RXC)) ) 	
		;			                
	/* Return the data */
	return MIDI_UDR;
}

void MIDI_UART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !(MIDI_UCSRA & (1<<MIDI_UDRE)) )
		; 			                
	/* Start transmittion */
	MIDI_UDR = data; 			        
}

#define MIDI_UART_ReceiveBufferFull (MIDI_UCSRA & (1<<MIDI_RXC)) 
#define MIDI_UART_TransmitBufferFree (MIDI_UCSRA & (1<<MIDI_UDRE))

// ringbuffer
#define RINGBUF_SIZE        512
struct s_ringbuf {
    unsigned short len;
    unsigned short head;
    unsigned short bot;
    unsigned char  buf[RINGBUF_SIZE];
} rbuf1;

int main(void) {
	u8 c;
    
    // timer-handles
    unsigned long thandles[3] = {0UL,0UL,0UL};

	// init
	DDRB = DDRB_INIT;
	DDRC = DDRC_INIT;
	DDRD = DDRD_INIT;
	PORTB = PORTB_INIT;
	PORTC = PORTC_INIT;
	PORTD = PORTD_INIT;
    
	SUART_init();
    timer_init();
	MIDI_UART_Init(MIDI_BAUDRATE);
    sei();					
	
	SUART_puts("Reset!\n\r" );
    
    rbuf1.len = 0;
    rbuf1.head = 0;
    rbuf1.bot = 0;
	
	for(;;) {
		
        // PC to MIDI
        // Overflow is possible if PC sends data to fast
        // because PC-UART-speed is higher than MIDI-UART-speed
        // so put data in ringbuf first
		if( SUART_rxdone() ) {
			c = SUART_getchar();
            
            // space left in ringbuf?
            if(rbuf1.len < RINGBUF_SIZE) {
                rbuf1.buf[rbuf1.head] = c;
                rbuf1.head++; if(rbuf1.head>=RINGBUF_SIZE) rbuf1.head = 0;
                rbuf1.len++;
            } else {
                // fatal, we lost data due to buffer overflow!
                LED_PORT &= ~(1<<LED3); thandles[2] = timer_getvalue();
            }
            
            // Lit LED1 when ring-buffer is about to be used
            if(!MIDI_UART_TransmitBufferFree) {
                LED_PORT &= ~(1<<LED1); thandles[0] = timer_getvalue();
            }
            
            // Lit LED2 if ring-buffer is more than half-full
            if(rbuf1.len > (RINGBUF_SIZE / 2)) {
                LED_PORT &= ~(1<<LED2); thandles[1] = timer_getvalue();
            }
		}

        // if UART is free empty ringbuffer
        if(MIDI_UART_TransmitBufferFree && (rbuf1.len > 0)) {
            MIDI_UART_Transmit(rbuf1.buf[rbuf1.bot]);
            rbuf1.bot++; if(rbuf1.bot>=RINGBUF_SIZE) rbuf1.bot = 0;
            rbuf1.len--;
        }
		
        // MIDI to PC
        // No overflow should be possible here
        if(MIDI_UART_ReceiveBufferFull) {
           c = MIDI_UART_Receive();
           SUART_putchar(c);
        }
        
        // Turn off LEDs after 1 second
        if(thandles[0] && (timer_elapsed_time(thandles[0]) > TIMER_MS(1000) )) {
            thandles[0] = 0;
            LED_PORT |= (1<<LED1);
        }
        if(thandles[1] && (timer_elapsed_time(thandles[1]) > TIMER_MS(1000) )) {
            thandles[1] = 0;
            LED_PORT |= (1<<LED2);
        }        
        if(thandles[2] && (timer_elapsed_time(thandles[2]) > TIMER_MS(1000) )) {
            thandles[2] = 0;
            LED_PORT |= (1<<LED3);
        }     
    //    if(timer_autodelay(&thandles[1], TIMER_MS(100))) LED_PORT ^= (1<<LED2);
        
	}
}


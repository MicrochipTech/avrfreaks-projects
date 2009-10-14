//-------------------------------------------------------------------------------------
//
//
//	Program for simple radio beacon in AVR 90S2313. ATtiny2313 could be used with 
//	minor changes, but this was what I had...
// 
//	
//-------------------------------------------------------------------------------------
//
//
//	BEERWARE LICENSE text.
//	=====================
//
// 	This program was written by per-arne asp, se4a/sm4inv.							
//
// 	You may do what you want with this software as long as you keep this license text.
//
//	If we meet some day in the future and you think this software is (or was) useful 
//  you can buy me a beer (or two, or thr.. :-).
//
//                                         Sept/Oct, 2009, Torsby, Sweden
//                                         per-arne@asp.st
// 
//
//-------------------------------------------------------------------------------------
//
//
// version history:
// ---------------
//
// ver.	when	who		why
// ----	----	---		---
//
// 1.00	090911	asp		1:st try!
//		091004	asp		Added int0 isr and soft_reset().
//		091014	asp		Tested OK at 150 char/min (~30WPM).
//
//
//-------------------------------------------------------------------------------------
//
//
//  Resources:
//  ----------
//
//	Program memory:	2k bytes,  appr. 51% left
//	RAM memory:		128 bytes, about 39% left
//	EEPROM memory:	128 bytes, all reserved for beacon text message.
//
//	
//-------------------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h> 
#include <avr/eeprom.h>
#include <ctype.h>
#include "cw.h"
#include "bit.h"
#include "delay_x.h"

//-------------------------------------------------------------------------------------
//
// 	max beacon message length, 
// 	DON'T CHANGE THIS!
//
#define MAX_MSG_LEN 128			//max 128 chars in 2313 EEPROM
//
//-------------------------------------------------------------------------------------
//
// 	text (in EEPROM) to send, MAX_MSG_LEN (=128) chars including '\0' 
//	(terminating zero) at end 
// 	CHANGE AS NEEDED, THEN REBUILD S/W AND REPROGRAM EEPROM! 
//
char EEMEM msg[ (int)MAX_MSG_LEN ] = 
	"vvv vvv vvv de se4a/beacon se4a/beacon = qth jp60ld jp60ld = email per-arne@asp.st per-arne@asp.st +\0";
//x1 01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567 
//x10          1         2         3         4         5         6         7         8         9         0         1         2      2 
//x100                                                                                                   1         1         1      1
//
//-------------------------------------------------------------------------------------
//
// 	Values that have to be computed at compile time
// 	CHANGE AS NEEDED, THEN REBUILD S/W AND REPROGRAM FLASH MEMORY! 
// 
#define CWSPEED 60				//desired cw speed [chars/min], (chars/min ~ WPM * 5)
#define PAUSE 	60				//desired pause after message [s]
//
//
//-------------------------------------------------------------------------------------
//
// 	Values that also have to be computed at compile time
// 	but DON'T CHANGE! The values below are responsible for all timing!
//
#define PERTIME 6000.0/CWSPEED	//dot time = interchar time, [ms], 
//								//1 char/min = 10 dot times/min => dottime = 6 s = 6000 ms
#define DASHTIME 3*PERTIME		//dash time
#define CHARSPACE 2*PERTIME		//should be 3*PERTIME but 1 already done in interchar space
#define WORDSPACE 4*PERTIME		//should be 7*PERTIME but 3 already done in CHARSPACE (+ interchar space)
#define PAUSETIME PAUSE*1000	//pause after msg [ms]
#define ONTIME PAUSETIME-(2*WORDSPACE)-DASHTIME	//key on time during pause (if used)
#define FLASHTIME 100			//Led's flashtime [ms]
//
//
//-------------------------------------------------------------------------------------
//
//	macros for port manupulation.
// 	DON'T CHANGE unless you changed the h/w!
//
// 	tx keyer, active LOW
#define KEYUP PORTD |=BIT6		
#define KEYDWN PORTD &=~BIT6	
//
// 	key LED, active HI
#define LEDOFF PORTD &=~BIT5	
#define LEDON PORTD |=BIT5		
//
// 	pause LED, active HI
#define PAUSEOFF PORTD &=~BIT4	
#define PAUSEON PORTD |=BIT4	
//
// 	buzzer, active HI
#define BUZZOFF PORTD &=~BIT3	
#define BUZZON PORTD |=BIT3		
//
//
//-------------------------------------------------------------------------------------
//
// 	s/w reset macro by invokinging WDT (watchdog timer)
// 	(from avr-libc Reference Manual, FAQ ).
//	DON'T CHANGE THIS!
//
#define soft_reset() 				\
	do 								\
	{ 								\
		wdt_enable( WDTO_15MS );	\
		for( ;; ) 					\
		{} 							\
	}while( 0 )				
//
//
//-------------------------------------------------------------------------------------



char txt;					//msg char to convert to cwChar
char ledFlash;				//flag for led flashing when keying off
char cwChr;					//cw char to send
unsigned char chkBit;		//bit value for searching inside cwChar
int i, j, k;				//counters
char *ptr;					//pointer to current char in msg[]


//-------------------------------------------------------------------------------------
//
//	Interrupt Service Routine for int0
//
//	called when "Keying ON" switch is switched OFF
//
ISR(INT0_vect)
{
	asm( "cli" );
	KEYUP;			//turn off keying, buzzer & LEDs 
	BUZZOFF;
	LEDOFF;	
	PAUSEOFF;
	ledFlash = 0;
	while( ( PIND & (char)BIT2 ) != BIT2 )	//if switch is OFF, wait and flash leds until ...
	{
		if( ledFlash )
			PAUSEOFF;
		else
			PAUSEON;
		ledFlash = !ledFlash;
		_delay_ms( (double)FLASHTIME );			//flash Pause LED when keying OFF		
	}			
	soft_reset();							//... switch flipped to "Keying ON" then restart	
}


//-------------------------------------------------------------------------------------
//
// Pause after msg sendt
// If txOn = 0 then silence during pause else tx carrier
//
void pause( char txOn )
{
	if( txOn == 0 )
		_delay_ms( (double)PAUSETIME );
	else
	{		
		_delay_ms( (double)WORDSPACE );

		KEYDWN; BUZZON; LEDON; 
		_delay_ms( (double)ONTIME );
		KEYUP; BUZZOFF; LEDOFF;

		_delay_ms( (double)WORDSPACE );
		_delay_ms( (double)DASHTIME );	 
	}
}


//-------------------------------------------------------------------------------------
//
//	main program
//
int main( void )
{
	asm( "cli" );	//disable interrupts			
	wdt_disable();	//disable wdt	
	 
	DDRB = 0xff;	//all outputs
	PORTB = 0xff;	//all hi
	DDRD = 0x79;	//all outputs, except PD2 (int0) & PD1 (JP1)
	PORTD = 0x46;	//all lo, except PD6 (keying), PD2 (int0) & PD1 (JP1)

	KEYUP;			//turn off keying, buzzer & LEDs 
	BUZZOFF;
	LEDOFF;	
	PAUSEOFF;
		
	GIMSK = BIT6;	//enable int0 in int. mask 
	asm( "sei" );	//enable interrupts

	for( ;; )		//loop for ever (or at least until interrupted ...)
	{
		ptr = &msg[0];	//init msg ptr

		for( i = 0; i < (int)MAX_MSG_LEN; i++ )	//loop thru msg
		{
			chkBit = BIT7;		//init chkBit

			//get message char from eeprom (and convert to upper case)
			txt = toupper( (char)eeprom_read_byte( (uint8_t*)ptr ) );
			
			if( txt > ' ' && txt <= '\x5f' )
				cwChr = chr[ txt - 0x20 ];					
			else if( txt == ' ' )	//word space
			{
				_delay_ms( (double)WORDSPACE );
				ptr++;
				continue;
			}				
			else if( txt == '½' )	//prosign 'end of transmission'
				cwChr = cwSK;
			else if( txt == '§' )	//prosign 'wait'
				cwChr = cwAS;
			else if( txt == '\0' )	//end of msg, after PAUSE delay start all over again 
				break;	
			else 					//illegal char
			{
				ptr++;
				continue;			
			}
							
			//search for startbit
			for( j = 7; j >= 0; j-- )
			{
				if( cwChr & chkBit )
					break;
				chkBit = chkBit >> 1; 
			}
			chkBit = chkBit >> 1;  

			//send cw char
			for( k = j - 1; k >= 0; k-- )
			{
				if( cwChr & chkBit )	
				{	
					//dash				
					KEYDWN; BUZZON; LEDON; 
					_delay_ms( (double)DASHTIME );
					KEYUP; BUZZOFF; LEDOFF; 
				}
				else	
				{	
					//dot				
					KEYDWN; BUZZON; LEDON; 
					_delay_ms( (double)PERTIME );
					KEYUP; BUZZOFF; LEDOFF; 
				}				
				_delay_ms( (double)PERTIME );	//delay 1 period (inter char space)	
				chkBit = chkBit >> 1; 			
			}
			_delay_ms( (double)CHARSPACE );		//char space												
			ptr++;								//inc msg ptr
		}
		PAUSEON;
		pause( (char)( PIND & BIT1 ) );	//if no JP1, transmit carrier during pause else no tx during pause
		PAUSEOFF;
	}
}



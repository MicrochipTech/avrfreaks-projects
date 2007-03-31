//
//
// The schematics 
//	PB0	(5) TX To PC
//	PB1	(6) RXD ( form PC)
//	PB2	(7) Servo 1
//	PB3	(2) Servo 2
//	PB4	(3) Servo 3
//	PB5	(1) Reset ( and debug wire )
//
#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr\pgmspace.h>
#include "ServoControl.h"

#define ServoMax	184 // 2.4 ms / 13.02 US = 184
#define ServoMin	39	// 0.5 ms / 13.02 US = 39

int main()
{
unsigned char RCByte;
unsigned char Device_ID;
unsigned char Device_4ID;

PORTB	= 0x00;
DDRB	= 0x1c; //
// OSCCAL	= 0x5e;					// If your Tiny is a bit off freq. Then you need to tune this
MCUCR	= ( 1<< PUD) | (1 << SE ) | (1 << SM1) ;  	// PULLUP DISABLE, Sleep Enable Power-down
TCCR0B  = ( 1 << CS00) ; 							// clkI/O/(No prescaling)

// The CKDIV8 Fuse determines the initial value of the CLKPS bits. 
CLKPR  = 0x80;
CLKPR  = 0x00; 						// Set devide by 1 Hi Speed 9.6 MC

TCCR0A	= ( 1 << WGM01 ); 			// Timer mode 2 Clear Timer on Compare Match (CTC) mode,
OCR0A	= 124; 						// This is 9600 baud/8 -1
TIMSK0  = ( 1 << OCIE0A ); 			// Enable the Timer interupt.

Servo1=Servo2=Servo3=(ServoMin+ServoMax)/2;
Prescaler8=Prescaler64=Prescaler20ms=1;
sei();								// enable interrupts 

Wating20ms=Device_ID=EepromRead(0);
Device_4ID=(Device_ID<<2);			// Get the Device Commands A,B,C,D
while(Wating20ms);					// Wait until the previus device has send its information

Puts((unsigned char *)PSTR("Servo "));
PutHex(Device_ID);
PutCRNL();

for(;;)
	{
		RCByte=GetChar();
		switch((RCByte-Device_4ID))
			{
			case 'A':
				if(Servo1<ServoMax)Servo1++;
				break;
			case 'a':
				if(Servo1>ServoMin)Servo1--;
				break;
			case 'B':
				if(Servo2<ServoMax)Servo2++;
				break;
			case 'b':
				if(Servo2>ServoMin)Servo2--;
				break;
			case 'C':
				if(Servo3<ServoMax)Servo3++;
				break;
			case 'c':
				if(Servo3>ServoMin)Servo3--;
				break;
			case 'D':
			case 'd':
				//Puts(PSTR("Status "));
				PutHex(Device_ID);
				PutChar(',');
				PutHex(Servo1);
				PutChar(',');
				PutHex(Servo2);
				PutChar(',');
				PutHex(Servo3);
				PutCRNL();
				break;
			}
	}
}

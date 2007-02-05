//
// This is a project to have a ferry simple PcScoop 
// I think you can buld it for les then 3 euro
//
// The schematics 
//	PB0	(5) TX To PC
//	PB1	(6) RXD ( form PC)
//	PB2	(7) ADC1  PEN
//	PB3	(2) ADC3  Y
//	PB4	(3) ADC2  X
//	PB5	(1) Reset ( and debug wire )
//

#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr\pgmspace.h>

#include "XYPlotter.h"

unsigned int ADC_X;
unsigned int ADC_Y;
unsigned char Pen;
unsigned int Last_ADC_X;
unsigned int Last_ADC_Y;
unsigned char Last_Pen;
unsigned int dec_out;
unsigned char Idel_Mode;


void PutDec()
{
unsigned char c;
unsigned char DecStarted=0;
unsigned int Devider=10000;

	while (Devider>1)
	{
		c=dec_out/Devider;
		if(c!=0 || DecStarted)
			{
				PutChar('0'+c);
				DecStarted=1;
			}
			dec_out%=Devider;
			Devider/=10;
	}
	PutChar((unsigned char)'0'+dec_out);
}


void PutSemicolom(void )
{
	Puts(PSTR(";\r\n")); //Semicolom;
}

void PutLastXY(void )
{
		Last_ADC_X = ADC_X;
		Last_ADC_Y = ADC_Y;
		dec_out=Last_ADC_X;
		PutDec();
		PutChar(',');
		dec_out=Last_ADC_Y;
		PutDec();
}

void Wait_For_Char_End(void)
{
	char t;
	for(t=0;t<25;t++)
		UART_delay(); // This is to sync the RS232 25 * 0.5 Bit time = 12.5 Bits 
}

void PutPlotterInitalization(void)
{
	//IN;  			Initialize, start a plotting job
	//IP;			Set the initial point (origin), in this case the default 0,0
	//DI;
	//SP1;			Select pen 1
	//SC0,1024,0,1024;	Set the scale so the page is 1024 to 1024 in both X and Y directions
	Puts(PSTR("IN;IP;DI;SP1;SC"));
	ADC_X=0;
	ADC_Y=1024;
	PutLastXY();
	PutChar(',');
	PutLastXY();
	PutSemicolom();
}

void PutDiffrentXY()
{
	if(PINB & (1 << PB2))
	{
		Pen='U';
	}
	else 
	{
		Pen='D';
	}
	Get_adc(2); // X
	ADC_X=ADCW;
	Get_adc(3); // Y
	ADC_Y=ADCW;
	if ( Last_ADC_X != ADC_X || Last_ADC_Y != ADC_Y || Last_Pen != Pen )
	{
		// The Pen Up of Down Commands are Send
		Last_Pen = Pen;
		PutChar('P');
		PutChar(Pen);

		// The X,Y; Is send acording to the AD value
		PutLastXY();		
		PutSemicolom();
	}
}

int main()
{
PORTB	= 0x00;
DDRB	= 0x01; //
MCUCR	= ( 1<< PUD) | (1 << SE ) | (1 << SM1) ;  	// PULLUP DISABLE, Sleep Enable Power-down
TCCR0B  = ( 1 << CS01) | ( 1 << CS00) ; 			// clkI/O/64 (From prescaler)
TIMSK0  = ( 1 << TOIE0);

// The CKDIV8 Fuse determines the initial value of the CLKPS bits. 
CLKPR  = 0x80;
CLKPR  = 0x00; // Set devide by 1 Hi Speed 9.6 MC
PCMSK  = (1 << PCINT1);
Idel_Mode=1;

Puts(PSTR("XYPlotter By Emile van der Laan www.emsign.nl\r\n")); 

	for(;;)
	{
		if ( GIFR &(1<<PCIF))				// Has it detected a start bit or other bits
		{ 
			Wait_For_Char_End();			// This will prevent that it start and stops in the same character
			if(Idel_Mode)
			{
				PutPlotterInitalization();
				Pen='?';					// As we do noet the Current Pen status
				Last_ADC_X=0xffff;
				Idel_Mode=0;				// set not in idle mode or run mode
			}
			else
			{			
				Idel_Mode=1;				// In idle mode it will only wait
			}
			GIFR = (1<<PCIF); 				// Clear the interupt flag, Not that i used the Interupt only the Flag.
		}
		if(!Idel_Mode)
		{
			PutDiffrentXY();				// This will only print if the XY or Pen is diffrent to reduce the file size
		}
	}

#ifdef NOT_USED
		for(;;)
		{
			c= GetChar();						// Simple Echo test 
			PutChar(c);
		}
#endif
	return 0;
}

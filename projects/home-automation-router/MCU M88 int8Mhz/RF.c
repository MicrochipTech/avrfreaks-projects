#include <util/delay.h>
#include "RF.h"

#define Number_of_Word_Bits		12	//<=16!
#define Number_Words_in_Frame	4

/*
Protocol based on:
Remote Decoder PT2272.pdf
Remote Encoder HX2262.pdf
*/

//Half_Bit < Bit < Word < Frame

	//No '1's are used!
	//Bit Float := 0
	//Bit 0 := 1

//local global
unsigned int Data_out_16 = 0;
//dummy bits, A0	A1	A2	A3	A4	A5	A6	A7	A8	A9	D1	D0

static void Wait_4T ( void )
{
	_delay_us(330);

}

static void RF_Output_High( void )
{
	bit_set(PORTB,BIT(2));
	LEDon;
}

static void RF_Output_Low( void )
{
	bit_clear(PORTB,BIT(2));
	LEDoff;
}

static void Wait_12T ( void )
{
	for (unsigned char i=0;i<3;i++)
		Wait_4T();
}

static inline void Send_Out_Bit_Float ( void )
{
	RF_Output_High();
	Wait_4T();
	RF_Output_Low();
	Wait_12T();
	
	RF_Output_High();
	Wait_12T();	
	RF_Output_Low();
	Wait_4T();
}

static inline void Send_Out_Half_Bit_High ( void )
{
	RF_Output_High();
	Wait_12T();
	RF_Output_Low();
	Wait_4T();
}


static inline void Send_Out_Half_Bit_Low ( void )
{
	RF_Output_High();
	Wait_4T();
	RF_Output_Low();
	Wait_12T();
}


static inline void Send_Out_Bit ( unsigned int outBit )	//Bit arrives in original position!!! must be 16bit ling
{//1,0,not impl->2 (float)

	//No '1's are used!
	//Bit Float := 0
	//Bit 0 := 1
	
	if ( outBit )
		for (unsigned char i=0;i<2;i++)	//Can be decomposed to 2 halfes
			Send_Out_Half_Bit_Low();
	else
			Send_Out_Bit_Float();

}


static inline void Send_Out_Sync ( void )
{
	RF_Output_High();
	Wait_4T();
	RF_Output_Low();
	
	for (unsigned char i=0;i<31;i++)	//Based on protocol!
		Wait_4T();

}


static inline void Send_Out_Word ( void )
{

	//use only if (bit)!
	for (unsigned char i=0;i<Number_of_Word_Bits;i++)
		Send_Out_Bit( bit_get( Data_out_16, BIT(Number_of_Word_Bits-1-i) ) );

	Send_Out_Sync();
}


void Send_Out_Frame ( unsigned int input_16 )
{
	Data_out_16 = input_16;

	for (unsigned char i=0;i<Number_Words_in_Frame;i++)
		Send_Out_Word();

	Data_out_16 = 0;
}

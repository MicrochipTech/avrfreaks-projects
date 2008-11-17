//**********************************************************************//
//FILE: main.c 
//AUTHOR: Adam Kadolph
//DATE:	11-15-2008
//DESCRIPTION: main file to implement external interrupts on the AVR
//**********************************************************************//

#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.h"

void EXT_INT0_init(void);
void HexConvert(unsigned char cnt);

unsigned volatile char flag,cnt0,cnt1;
unsigned char hundr,tens,ones;

unsigned char array0[]={"INT0:"};
unsigned char array1[]={"INT1:"};
unsigned char array2[]={"Total:"};

ISR(INT0_vect)
{
	flag = 0x01;
	cnt0++;
}

ISR(INT1_vect)
{
	flag = 0x02;
	cnt1++;
}

int main(void)
{
	flag = 0x00;	//initialize flag
	cnt0 = 0x00;
	cnt1 = 0x00;

	SREG |= 0x80;	//enable all interrupts

	EXT_INT0_init();
	LCDinit();

	LCDclr();

	while(1)
	{
		if(flag==0x01)
		{
			LCDGotoXY(0,0);
			LCDstring(array0,5);
			
			HexConvert(cnt0);

			LCDsendChar(hundr);
			LCDsendChar(tens);
			LCDsendChar(ones);

			flag = 0x00;
		}
		else if(flag==0x02)
		{
			LCDGotoXY(0,1);
			LCDstring(array1,5);
				
			HexConvert(cnt1);		//display ext int1 key presses

			LCDsendChar(hundr);
			LCDsendChar(tens);
			LCDsendChar(ones);

			flag = 0x00;
		}
		else
		{
			LCDGotoXY(0,3);
			LCDstring(array2,6);
			
			HexConvert((cnt0+cnt1));	//display total key presses

			LCDsendChar(hundr);
			LCDsendChar(tens);
			LCDsendChar(ones);
		}
	}

	return 0;
}

void EXT_INT0_init(void)
{
	MCUCR = (1<<ISC11)|(1<<ISC01);		//set external interrupt to falling edge detection
	GICR = (1<<INT1)|(1<<INT0);			//enable external interrup 0 and 1
}

void HexConvert(unsigned char cnt)
{
	unsigned char dec,high,low;

	high = (cnt>>4);
	low = (cnt&0x0F);	//mask upper bits

	dec = (high*16) + low;	//decimal convert

	hundr = dec/100;	//BCD convert
	ones = dec%10;
	tens = ((dec%100)-ones)/10;

	hundr |= 0x30;	//place in ascii format for display
	tens |= 0x30;	
	ones |= 0x30;
}

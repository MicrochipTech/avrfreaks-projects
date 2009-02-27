//**********************************************************************//
//FILE: main.c 
//AUTHOR: Adam Kadolph
//DATE:	2-7-09
//DESCRIPTION: This project is to control a custom LED 5x6 Display 
//**********************************************************************//

#include <avr/io.h>
#include <avr/interrupt.h>
#include "font.h"

void SYS_HW_init(void);
void TIMER0_init(void);
void TIMER1_init(void);

unsigned char buffer[6],data[3];
unsigned char row[6] = { 0x1F, 0x2F, 0x37, 0x3B, 0x3D, 0x3E };

unsigned volatile char i,j,cnt;

ISR (TIMER0_OVF_vect)
{
	//update a LED output row every 255 us
	PORTB = row[i];
	PORTC = buffer[i];

	if(i == 5)
	{
		i = 0;
	}
	else
	{
		i++;
	}
}

ISR (TIMER1_COMPA_vect)
{
	//update LED output buffer for new data
	for(j=0; j<3; j++)
	{
		buffer[j] = data[j];
	}

	cnt++;
}

int main(void)
{
	i = 0;
	j = 0;
	cnt = 0;

	SYS_HW_init();	//initialize Port Directions and Values
	TIMER0_init();	//setup Timer 0 for output update
	TIMER1_init();	//setup Timer 1 for data buffer update

	SREG |= 0x80;	//turn on all interrupts

	while(1)
	{
		switch (cnt)
		{
			case 0:
				data[0] = font_27[1];
				data[1] = font_27[2];
				data[2] = font_27[3];
				break;
			
			case 25:
				data[0] = font_28[1];
				data[1] = font_28[2];
				data[2] = font_28[3];
				break;

			case 50:
				data[0] = font_29[1];
				data[1] = font_29[2];
				data[2] = font_29[3];
				break;

			case 75:
				data[0] = font_30[1];
				data[1] = font_30[2];
				data[2] = font_30[3];
				break;

			case 100:
				data[0] = font_31[1];
				data[1] = font_31[2];
				data[2] = font_31[3];
				break;

			case 125:
				data[0] = font_32[1];
				data[1] = font_32[2];
				data[2] = font_32[3];
				break;

			case 150:
				data[0] = font_33[1];
				data[1] = font_33[2];
				data[2] = font_33[3];
				break;

			case 175:
				data[0] = font_34[1];
				data[1] = font_34[2];
				data[2] = font_34[3];
				break;

			case 200:
				data[0] = font_35[1];
				data[1] = font_35[2];
				data[2] = font_35[3];
				break;

			case 225:
				data[0] = font_36[1];
				data[1] = font_36[2];
				data[2] = font_36[3];
				break;

			/*default:
				data[0] = 0x1F;	//default value for buffer
				data[1] = 0x1F;
				data[2] = 0x1F;
				break;*/
		}
	}

	return 0;
}

void SYS_HW_init(void)
{
	DDRB = 0x3F;	//all outputs, PB0-5 control Rows 1-6
	DDRC = 0x1F;	//all outputs, PC0-4 control Columns 1-5

	PORTB = 0xFF;	//high logic level will turn Rows off
	PORTC = 0x00;	//low logic level will turn Columns off
}

void TIMER0_init(void)
{
	TIMSK = (1<<TOIE0)|(1<<OCIE1A);	//Timer 0 Overflow and Timer 1 OutComp interrupt enable
	TCCR0 = (1<<CS01);				//Prescaling equal to clk/8
}

void TIMER1_init(void)
{	
	OCR1AL = 0xED;
	OCR1AH = 0x85;		//setup Timer 1 for 500 ms

	TCCR1B = (1<<WGM12)|(1<<CS12);	//Prescaler equal to clk/256
}

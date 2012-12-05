/*
 * BinaryCalculator.c
 *
 * Created: 9/11/2012 5:06:47 PM
 *  Author: Taylor Melton
 */ 

#include <avr/io.h>
#include "hd44780.h"
#include "hd44780_settings.h"
#include <util/delay.h>
#include <avr/interrupt.h>
uint8_t right=0, left=0, count, z;
uint16_t num=0,tempNum, multiplier=1;
char bText[20],text[7];

ISR(PCINT1_vect)
{	
	if(PINC == 0b00000110)//One pin grounded by rotary switch
	{ 
		if(right==0)//steps needed in case of misread
			right=1;//establishing rotation direction
		if(left==1)
			left=0;
	}
	
	if(PINC == 0b00000101)//One pin grounded by rotary switch
	{
		if(left==0)//steps needed in case of misread
			left=1;//establishing rotation direction
		if(right==1)
			right=0;
	}
	
	if(PINC == 0b00000100)//Both pins are grounded by rotary switch
	{
		if(left==1)//turning knob counterclockwise (decrease)
		{
			if(num==0)
				num=65535;//jump to largest number		
			else
			{
				if(multiplier>1)
				{
					num=num-multiplier;//only subtract by the rate chosen
				}
				else
					num--;
			}
			//reset lcd and rewrite everything	
			lcd_clrscr();
			
			lcd_goto(0x00);
			lcd_puts("Hex");
			lcd_goto(0x40);
			lcd_puts("Dec");
			lcd_goto(0x14);
			lcd_puts("Oct");
			lcd_goto(0x54);
			lcd_puts("B");
			
			lcd_goto(0x04);
			sprintf (text, "%X", num);//converting uint16_t to string
			lcd_puts(text);
			
			lcd_goto(0x0B);
			lcd_puts("Rate:");
			sprintf (text, "%u", multiplier);//converting uint16_t to string
			lcd_puts(text);
			
			lcd_goto(0x44);
			sprintf (text, "%u", num);//converting uint16_t to string
			lcd_puts(text);
			
			lcd_goto(0x18);
			sprintf (text, "%o", num);//converting uint16_t to string
			lcd_puts(text);
			
			lcd_goto(0x55);
			//steps needed to covert uint16_t to binary
			tempNum=num;
			bText[19] = '\0';
			count=18;
			for (z=16; z >0; z--)
			{
				if(z==4 | z==8 | z==12)//space every 4 bits
				{
					bText[count]=' ';
					count--;
				}
				
				if(tempNum%2)
					bText[count]='1';
				else
					bText[count]='0';
				
				count--;
				tempNum=tempNum/2;
			}
			lcd_puts(bText);
		}
					
		if(right==1)//turning knob clockwise (increase)
		{
			if(num==65535)
				num=0;			
			else
			{
				if(multiplier>1)
				{
					num=num+multiplier;//only add by the rate chosen
				}
				else
					num++;
			}									
			//reset lcd and rewrite everything	
			lcd_clrscr();
			
			lcd_goto(0x00);
			lcd_puts("Hex");
			lcd_goto(0x40);
			lcd_puts("Dec");
			lcd_goto(0x14);
			lcd_puts("Oct");
			lcd_goto(0x54);
			lcd_puts("B");
				
			lcd_goto(0x04);
			sprintf (text, "%X", num);//converting uint16_t to string
			lcd_puts(text);
			
			lcd_goto(0x0B);
			lcd_puts("Rate:");
			sprintf (text, "%u", multiplier);//converting uint16_t to string
			lcd_puts(text);
			
			lcd_goto(0x44);
			sprintf (text, "%u", num);//converting uint16_t to string
			lcd_puts(text);
			
			lcd_goto(0x18);
			sprintf (text, "%o", num);//converting uint16_t to string
			lcd_puts(text);
			//steps needed to convert uint16_t to binary
			lcd_goto(0x55);
			tempNum=num;
			bText[19] = '\0';
			count=18;
			for (z=16; z >0; z--)
			{
				if(z==4 | z==8 | z==12)//space every 4 bits
				{
					bText[count]=' ';
					count--;
				}
				
				if(tempNum%2)
					bText[count]='1';
				else
					bText[count]='0';
				
				count--;
				tempNum=tempNum/2;
			}
			lcd_puts(bText);
		}			
	}
	
	if(PINC == 0b00000011)//Rotary knob depressed (change rate)
	{
		if(multiplier==1000)
		{
			multiplier=1;//reset multiplier to 1
			lcd_goto(0x11);//clear out 1000
			lcd_puts(" ");
			lcd_goto(0x12);//clear out 1000
			lcd_puts(" ");
			lcd_goto(0x13);//clear out 1000
			lcd_puts(" ");
		}			
		else
			multiplier=multiplier*10;
		//lcd output
		lcd_goto(0x0B);
		lcd_puts("Rate:");
		sprintf (text, "%u", multiplier);//converting uint16_t to string
		lcd_puts(text);
	}		
}

int main(void)
{	
	sei();//enable global interrupts
	PCICR=(1<<PCIE1);//Pin Change Interrupt Control Register/Enable pins[15:8]
	PCMSK1=(1<<PCINT10 | 1<<PCINT9 | 1<<PCINT8);//Pin Change Mask Register 1 (monitoring pins 2, 1, & 0)
	
	PORTC = (1<<PC2|1<<PC1|1<<PC0);//Define pull-ups and set outputs high
	
	lcd_init();//initialize lcd

    lcd_clrscr();//clear screen
	//output to lcd
	lcd_puts("Welcome to the");
	lcd_goto(0x40);
	lcd_puts("Binary Calculator");
	lcd_goto(0x14);
	lcd_puts("Press to change rate");
	_delay_ms(250);
	lcd_clrscr();
	
	lcd_goto(0x00);
	lcd_puts("Hex");
	_delay_ms(30);
	lcd_goto(0x40);
	lcd_puts("Dec");
	_delay_ms(30);
	lcd_goto(0x14);
	lcd_puts("Oct");
	_delay_ms(30);
	lcd_goto(0x54);
	lcd_puts("B");	
	_delay_ms(30);
	lcd_goto(0x0B);
	lcd_puts("Rate:1");
		
    while(1)
    {	
		//processor loops forever waiting on interrupts from rotary switch
    }
}
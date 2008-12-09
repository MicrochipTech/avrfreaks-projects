//**********************************************************************//
//FILE: main.c 
//AUTHOR: Adam Kadolph
//DATE:	12-8-2008
//DESCRIPTION: Main file to implement EEPROM on the AVR throught the i2c bus
//the Microchip 24LC32 EEPROM is used
//**********************************************************************//

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "i2c_eeprom.h"
#include "twi.h"
#include "lcd.h"

unsigned char page0_buffer[8]={'A','d','a','m',' ','i','s',' '};
unsigned char page1_buffer[8]={'a','w','e','s','o','m','e','!'};

unsigned volatile char erase,program;

void EXT_INT0_init(void);

ISR(INT0_vect)
{
	erase = 1;	//set erase mode
}

ISR(INT1_vect)
{
	program = 1;	//set program mode
}

int main(void)
{
	erase = 0;		//initialize interrupt flags
	program = 0;

	i2c_init();		//initialize peripherals
	LCDinit();
	EXT_INT0_init();

	SREG |= 0x80;	//enable all interrupts

	LCDclr();

	while(1)
	{
		int i,temp;

		//erase all memory if set
		if(erase == 1)
		{
			i2c_EEPROM_erase();
			
			_delay_ms(10);
			
			erase = 0;
		}
		
		//Program first two pages in memory if set
		if(program == 1)
		{
			for(i=0; i<8; i++)
			{
				buffer[i] = page0_buffer[i];
			}
	
			i2c_EEPROM_page_write(0x00);	//random EEPROM writes
			_delay_ms(1);
	
			for(i=0; i<8; i++)
			{
				buffer[i] = page1_buffer[i];
			}
	
			i2c_EEPROM_page_write(0x01);
			_delay_ms(1);

			_delay_ms(10);
			
			program = 0;
		}

		//read current memory
		else
		{
			for(i=0; i<0x20; i++)
			{
				temp = i2c_EEPROM_byte_read(i);

				LCDGotoXY(i,0);
	
				LCDsendChar(temp);
			}

			_delay_ms(10);
		}
	}

	return 0;
}

void EXT_INT0_init(void)
{
	MCUCR = (1<<ISC11)|(1<<ISC01);		//set external interrupt to falling edge detection
	GICR = (1<<INT1)|(1<<INT0);			//enable external interrup 0 and 1
}

// Conway's AI-Life for the AVR via an 8x8 LED array
// By computerdork@verizon.net 05/18/08
// redistribute it, hack it, make it your own!

/* Standard Includes */
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 2000000UL
#include <util/delay.h>

// MACROS from abcminiuser's post on bit manipulation in C via avrfreaks.net
#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x)) 


/* Prototypes */
const uint8_t disp_seq[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };

//uint8_t display_ico[8] = { 0x20, 0x10, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00 }; // glider
uint8_t display_ico[8] = { 0x00, 0x00, 0x24, 0x40, 0x44, 0x78, 0x00, 0x00 }; // spaceship


int is_alive_next(uint8_t symbol[], uint8_t step_bit, uint8_t loc_bit)
{
	int8_t i, j;
	uint8_t count = 0;
	for (i=(step_bit-1); i<(step_bit+2); i++)
	{
		for (j=(loc_bit-1); j<(loc_bit+2); j++)
		{
			if ( (i>=0) && (i<8) && (j>=0) && (j<8) )
			{
				if (bit_get(symbol[i],BIT(j)))
				{
					count++;
				}
			}
			
			else if ( (i==-1) && (j>=0) && (j<8) )
			{
				if (bit_get(symbol[7],BIT(j)))
				{
					count++;
				}
			}
			
			else if ( (i==8) && (j>=0) && (j<8) )
			{
				if (bit_get(symbol[0],BIT(j)))
				{
					count++;
				}
			}
			
			else if ( (i>=0) && (i<8) && (j==-1) )
			{
				if (bit_get(symbol[i],BIT(7)))
				{
					count++;
				}
			}
			
			else if ( (i>=0) && (i<8) && (j==8) )
			{
				if (bit_get(symbol[i],BIT(0)))
				{
					count++;
				}
			}
			
			else if ( (i==8) && (j==8) )
			{
				if (bit_get(symbol[0],BIT(0)))
				{
					count++;
				}
			}
			
			else if ( (i==-1) && (j==-1) )
			{
				if (bit_get(symbol[7],BIT(7)))
				{
					count++;
				}
			}
			
			else if ( (i==-1) && (j==8) )
			{
				if (bit_get(symbol[7],BIT(0)))
				{
					count++;
				}
			}
			
			else if ( (i==8) && (j==-1) )
			{
				if (bit_get(symbol[0],BIT(7)))
				{
					count++;
				}
			}
		}
	}
	
	
	if (bit_get(symbol[step_bit],BIT(loc_bit)))
	{
		count--;
	}
	
	if ((count<2) || (count>3))
		return 0;
	else if (count==3)
		return 1;
	else if ( (count==2) && ( bit_get(symbol[step_bit],BIT(loc_bit)) ) )
		return 1;
	else 
		return 0;
	
}


void display_routine(uint8_t symbol[], uint16_t delay)
{
	uint8_t i,j, last_symbol[8];
	for ( i=0; i<8; i++ )
		{
			last_symbol[i] = symbol[i];
			PORTA = last_symbol[i];
			PORTB = i;
			symbol[i] = 0x00;
			_delay_us(delay/2);
		}

//		
	int temp;
	
	for ( i=0; i<8; i++)
		{
			for ( j=0; j<8; j++)
				{
						temp = is_alive_next(last_symbol, i, j);
						if ( temp==1 )
						{
							bit_set(symbol[i],BIT(j));
						}
				}
		}
}

int main(void)
{
	
	DDRA = 0xFF; 			/* Enable output on all of the B pins */
	DDRB = 0x07;			

	while(1)			/* main loop here */
	{			
		
		display_routine(display_ico, 1000);
		_delay_ms(1);
	}
}




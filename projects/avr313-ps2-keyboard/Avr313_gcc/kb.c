#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "kb.h"
#include "serial.h"
#include "gpr.h"
#include "scancodes.h"

#define KB_BUFF_SIZE 64

volatile uint8_t kb_buffer[KB_BUFF_SIZE];
volatile uint8_t *kb_inptr;
volatile uint8_t *kb_outptr;
volatile uint8_t kb_buffcnt;



void init_kb(void)
{
	kb_inptr =  kb_buffer;					  // Initialize buffer
	kb_outptr = kb_buffer;
	kb_buffcnt = 0;

	MCUCR = (1 << ISC01);					  // INT0 interrupt on falling edge
	GICR= (1 << INT0);						  // Enable INT0 interrupt
}




ISR (INT0_vect)
{
	static uint8_t data = 0;				  // Holds the received scan code
	static uint8_t bitcount = 11;			  // 0 = neg.  1 = pos.

	if(bitcount < 11 && bitcount > 2)		  // Bit 3 to 10 is data. Parity bit,
	{										  // start and stop bits are ignored.
		data = (data >> 1);
		if(PIND & (1 << DATAPIN))
			data = data | 0x80;				  // Store a '1'
	}

	if(--bitcount == 0)						  // All bits received
	{
		bitcount = 11;
		decode(data);
	}
}



void decode(uint8_t sc)
{
	static uint8_t is_up = 0, mode = 0;
	static uint8_t shift = 0;
	uint8_t i, ch;

	if (!is_up)								  // Last data received was the up-key identifier
	{
		if(sc == 0xF0)						  // The up-key identifier
		{
			is_up = 1;
		}

		else if(sc == 0x12 || sc == 0x59)	  // Left SHIFT or Right SHIFT
		{
			shift = 1;
		}

		else if(sc == 0x05)					  // F1
		{
			if(mode == 0)
				mode = 1;					  // Enter scan code mode
			if(mode == 2)
				mode = 3;					  // Leave scan code mode
		}

		else
		{
			if(mode == 0 || mode == 3)		  // If ASCII mode
			{
				if(!shift)					  // If shift not pressed,
				{							  // do a table look-up
					for(i = 0; (ch = pgm_read_byte(&unshifted[i][0])) != sc && ch; i++);
					if (ch == sc)
					{
						put_kbbuff(pgm_read_byte(&unshifted[i][1]));
					}
				}							  // If shift pressed
				else
				{
					for(i = 0; (ch = pgm_read_byte(&shifted[i][0])) != sc && ch; i++);
					if (ch == sc)
					{
						put_kbbuff( pgm_read_byte(&shifted[i][1]));
					}
				}
			}								  // Scan code mode
			else
			{
				print_hexbyte(sc);			  // Print scan code
				put_kbbuff(' ');
				put_kbbuff(' ');
			}
		}
	}
	else
	{
		is_up = 0;							  // Two 0xF0 in a row not allowed

		if(sc == 0x12 || sc == 0x59)		  // Left SHIFT or Right SHIFT
		{
			shift = 0;
		}

		else if(sc == 0x05)					  // F1
		{
			if(mode == 1)
				mode = 2;
			if(mode == 3)
				mode = 0;
		}
		// case 0x06 :						// F2
		//   clr();
		//   break;
	}
}


//-------------------------------------------------------------------
// Stuff a decoded byte into the keyboard buffer.
// This routine is currently only called by "decode" which is called 
// from within the ISR so atomic precautions are not needed here.
//-------------------------------------------------------------------
void put_kbbuff(uint8_t c)
{
	// uint8_t tmp = SREG;
	// cli();

	if (kb_buffcnt < KB_BUFF_SIZE)			  // If buffer not full
	{
		// Put character into buffer
		// Increment pointer
		*kb_inptr++ = c;
		kb_buffcnt++;

		// Pointer wrapping
		if (kb_inptr >= kb_buffer + KB_BUFF_SIZE)
			kb_inptr = kb_buffer;
	}

	// SREG = tmp;
}


//-------------------------------------------------------------------
// Get a char from the keyboard buffer.
// Routine does not return until a character is ready!
//-------------------------------------------------------------------

int get_kbchar(void)
{
	int byte;

	// Wait for data
	while(kb_buffcnt == 0);

	uint8_t tmp = SREG;
	cli();

	// Get byte - Increment pointer
	byte = *kb_outptr++;

	// Pointer wrapping
	if (kb_outptr >= kb_buffer + KB_BUFF_SIZE)
		kb_outptr = kb_buffer;

	// Decrement buffer count
	kb_buffcnt--;

	SREG = tmp;

	return byte;
}

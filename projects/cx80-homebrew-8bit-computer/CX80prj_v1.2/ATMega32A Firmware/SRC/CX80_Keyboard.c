/*
 * CX80_Keyboard.c
 *
 * Created: 03/04/2013 11:07:48
 *  Author: Calogiuri Enzo Antonio
 */ 
#include "CX80_Keyboard.h"
#include "CX80_Keycodes.h"

#define RISE			1
#define FALL			0
#define MAX_KEYS		4

#define LAMP_SCROLL		0
#define LAMP_NUM		1
#define LAMP_CAPS		2

static const prog_uchar PROGMEM scan_to_ascii[][5] = 
{
{0x01, F9,	SHIFT_F9,	CTL_F9,		ALT_F9},
{0x03, F5,	SHIFT_F5,	CTL_F5,		ALT_F5},
{0x04, F3,	SHIFT_F3,	CTL_F3,		ALT_F3},
{0x05, F1,	SHIFT_F1,	CTL_F1,		ALT_F1},
{0x06, F2,	SHIFT_F2,	CTL_F2,		ALT_F2},
{0x07, F12,	SHIFT_F12,	CTL_F12,	ALT_F12},
{0x09, F10,	SHIFT_F10,	CTL_F10,	ALT_F10},
{0x0a, F8,	SHIFT_F8,	CTL_F8,		ALT_F8},
{0x0b, F6,	SHIFT_F6,	CTL_F6,		ALT_F6},
{0x0c, F4,	SHIFT_F4,	CTL_F4,		ALT_F4},
{0x0d, TAB,	SHIFT_TAB,	NOT_USED,	NOT_USED},
{0x0e, '~',	'`',		NOT_USED,	NOT_USED},
{0x15, 'q',	'Q',		CTL_Q,		NOT_USED},
{0x16, '1',	'!',		NOT_USED,	NOT_USED},
{0x1a, 'z',	'Z',		CTL_Z,		NOT_USED},
{0x1b, 's',	'S',		CTL_S,		NOT_USED},
{0x1c, 'a',	'A',		CTL_A,		NOT_USED},
{0x1d, 'w',	'W',		CTL_W,		NOT_USED},
{0x1e, '2',	'@',		NOT_USED,	NOT_USED},
{0x1f, WIN,	WIN,		WIN,		WIN},
{0x21, 'c',	'C',		CTL_C,		NOT_USED},
{0x22, 'x',	'X',		CTL_X,		NOT_USED},
{0x23, 'd',	'D',		CTL_D,		NOT_USED},
{0x24, 'e',	'E',		CTL_E,		NOT_USED},
{0x25, '4',	'$',		NOT_USED,	NOT_USED},
{0x26, '3',	'#',		NOT_USED,	NOT_USED},
{0x29, SPACE,	SPACE,		SPACE,		SPACE},
{0x2a, 'v',	'V',		CTL_V,		NOT_USED},
{0x2b, 'f',	'F',		CTL_F,		NOT_USED},
{0x2c, 't',	'T',		CTL_T,		NOT_USED},
{0x2d, 'r',	'R',		CTL_R,		NOT_USED},
{0x2e, '5',	'%',		NOT_USED,	NOT_USED},
{0x2f, MENU,	MENU,		MENU,		MENU},
{0x31, 'n',	'N',		CTL_N,		NOT_USED},
{0x32, 'b',	'B',		CTL_B,		NOT_USED},
{0x33, 'h',	'H',		CTL_H,		NOT_USED},
{0x34, 'g',	'G',		CTL_G,		NOT_USED},
{0x35, 'y',	'Y',		CTL_Y,		NOT_USED},
{0x36, '6',	'^',		NOT_USED,	NOT_USED},
{0x3a, 'm',	'M',		CTL_M,		NOT_USED},
{0x3b, 'j',	'J',		CTL_J,		NOT_USED},
{0x3c, 'u',	'U',		CTL_U,		NOT_USED},
{0x3d, '7',	'&',		NOT_USED,	NOT_USED},
{0x3e, '8',	'*',		NOT_USED,	NOT_USED},
{0x41, ',',	'<',		NOT_USED,	NOT_USED},
{0x42, 'k',	'K',		CTL_K,		NOT_USED},
{0x43, 'i',	'I',		CTL_I,		NOT_USED},
{0x44, 'o',	'O',		CTL_O,		NOT_USED},
{0x45, '0',	')',		NOT_USED,	NOT_USED},
{0x46, '9',	'(',		NOT_USED,	NOT_USED},
{0x49, '.',	'>',		NOT_USED,	NOT_USED},
{0x4a, '/',	'?',		NOT_USED,	NOT_USED},
{0x4b, 'l',	'L',		CTL_L,		NOT_USED},
{0x4c, ';',	':',		NOT_USED,	NOT_USED},
{0x4d, 'p',	'P',		CTL_P,		NOT_USED},
{0x4e, '-',	'_',		NOT_USED,	NOT_USED},
{0x52, 0x27,	'"',		NOT_USED,	NOT_USED},
{0x54, '[',	'{',		NOT_USED,	NOT_USED},
{0x55, '=',	'+',		NOT_USED,	NOT_USED},
{0x58, C_LOCK,	C_LOCK,		C_LOCK,		C_LOCK},
{0x5a, 0x0d,	0x0d,		0x0d,		0x0d},
{0x5b, ']',	'}',		NOT_USED,	NOT_USED},
{0x5d, '\\',	'|',		NOT_USED,	NOT_USED},
{0x66, BS,	SHIFT_BS,	CTL_BS,		NOT_USED},
{0x69, END,	SHIFT_END,	CTL_END,	NOT_USED},
{0x6b, LEFT,	SHIFT_LEFT,	CTL_LEFT,	NOT_USED},
{0x6c, HOME,	SHIFT_HOME,	CTL_HOME,	NOT_USED},
{0x70, INS,	SHIFT_INS,	CTL_INS,	NOT_USED},
{0x71, DEL,	SHIFT_DEL,	CTL_DEL,	NOT_USED},
{0x72, DOWN,	SHIFT_DOWN,	CTL_DOWN,	NOT_USED},
{0x74, RIGHT,	SHIFT_RIGHT,	CTL_RIGHT,	NOT_USED},
{0x75, UP,	SHIFT_UP,	CTL_UP,		NOT_USED},
{0x76, ESC,	SHIFT_ESC,	CTL_ESC,	NOT_USED},
{0x78, F11,	SHIFT_F11,	CTL_F11,	ALT_F11},
{0x7a, PGDN,	SHIFT_PGDN,	CTL_PGDN,	NOT_USED},
{0x7d, PGUP,	SHIFT_PGUP,	CTL_PGUP,	NOT_USED},
{0x83, F7,	SHIFT_F7,	CTL_F7,		ALT_F7}
};

volatile byte _edge, _bitCount, data;
byte Flags = 0, keyCount = 75;
byte KBScancode;
volatile byte keyBuffer[MAX_KEYS];
volatile byte *inPtr, *outPtr, *endPtr;

/*-------------------------------------------------------------------------*/

/*Insert a pressed key into buffer*/
static void putKey(byte key)
{
	*inPtr = key;
	
	if (++inPtr == endPtr)
		inPtr = keyBuffer;
		
	if (inPtr == outPtr)
		if (++outPtr == endPtr)
			outPtr = keyBuffer;
}

/*Return the first key pressed from buffer*/
byte CX80_GetKey(void)
{
	byte key = '\0';
	
	if (outPtr == endPtr)
		outPtr = keyBuffer;
		
	if (outPtr != inPtr)
		key = *outPtr++;
		
	return key;
}

/*Convert the scancode of key pressed into ascii value*/
static byte DecodeScanCode(byte ScanCode, byte Flags)
{
	byte offset;
	byte result = NOT_USED;
	byte min = 0;
	byte max = keyCount - 1;
	byte mid, value;
	
	while (min <= max)
	{
		mid = (min + max) / 2;
		value = pgm_read_byte(&scan_to_ascii[mid][0]);
		
		if ( value == ScanCode )
			break;
			
		if (ScanCode > value)
			min = mid + 1;
		else
			max = mid - 1;
	}
	
	if (min <= max)
	{
		offset = 1;
		
		if (Flags & (SHIFT_FLAG | CLOCK_FLAG))
			offset = 2;
		else
			if (Flags & CTL_FLAG)
				offset = 3;
			else
				if (Flags & ALT_FLAG)
					offset = 4;
					
		result = pgm_read_byte(&scan_to_ascii[mid][offset]);
	}
	
	return result;
}

/*Read value from keyboard and convert it*/
static void KB_decode(byte data)
{
	byte chr;
	
	if ((Flags & UP_FLAG ) == 0)
	{
		if (data == 0xf0)
			Flags |= UP_FLAG;
		else
			if(( data == 0x12 ) || ( data == 0x59))
				Flags |= SHIFT_FLAG;
			else
				if (data == 0x58)
					Flags ^= CLOCK_FLAG;
				else
					if (data == 0x14)
						Flags |= CTL_FLAG;
					else
						if (data == 0x11)
							Flags |= ALT_FLAG;
						else
							if (data == 0xe0)
								return;
							else
								if ((chr = DecodeScanCode(data, Flags)) != 0)
								{
									KBScancode = data;
									
									putKey(chr);
								}
	}
	else
	{
		Flags &= ~UP_FLAG;
		
		if ((data == 0x12 ) || ( data == 0x59))
			Flags &= ~SHIFT_FLAG;
		else
			if (data == 0x14)
				Flags &= ~CTL_FLAG;
			else
				if (data == 0x11)
					Flags &= ~ALT_FLAG;
	}
}

/*Setup the keyboard port*/
void CX80_KeyboardInit(void)
{
	SYSTEM_BUS_PORT_DDR &= 0b11110001;
	SYSTEM_BUS_PORT |= 0x0c;
	
	_edge = FALL;		// Next edge should be falling.
	_bitCount = 11;		// Get 11 bits from the keyboard.	
	
	/*
	GICR |= (0 << INT1) | (1 << INT0) | (0 << INT2);
	MCUCR = (0 << ISC11) | (0 << ISC10) | (1 << ISC01) | (0 << ISC00);
	MCUCSR = (0 << ISC2);
	GIFR = (0 << INTF1) | (1 << INTF0) | (0 << INTF2);
	*/
	
	MCUCR &= ~0x03;		// Setup INT0 for the falling edge.
	MCUCR |= 0x02;
	GICR = 0x40;		// Enable INT0 interrupt
	
	inPtr = outPtr = keyBuffer;
	endPtr = inPtr + (MAX_KEYS);
	
	sei();
}

/*This is the interrupt that control the keyboard*/
ISR(INT0_vect)
{
	if (_edge == FALL)
	{
		if (_bitCount < 11 && _bitCount > 2)
		{
			data = (data >> 1);
			
			if (PIND & 8)
				data |= 0x80;
		}
		
		_edge = RISE;
		
		//MCUCR = (0 << ISC11) | (0 << ISC10) | (1 << ISC01) | (1 << ISC00);
		MCUCR |= 0x03;			// Setup INT0 for rising edge.
	}
	else
	{
		if (--_bitCount == 0)
		{
			KB_decode(data);
			
			_bitCount = 11;
		}
		
		_edge = FALL;
		
		//MCUCR = (0 << ISC11) | (0 << ISC10) | (1 << ISC01) | (0 << ISC00);
		MCUCR &= ~0x03;			// Setup INT0 for the falling edge.
		MCUCR |= 0x02;
	}
}

/*Read a line typed, until user press return*/
byte CX80_GetLine(char *buf, byte bufsize)
{
	char c, receivedChars = 0;
	
	for (;;)
	{
		c = (char)CX80_GetKey();
		
		if (c)
		{
			if (c == '\r' || c == '\n')
				break;
			
			if (c == '\b')
			{
				if (receivedChars > 0)
				{
					buf--;
					receivedChars--;
					
					CX80_Usart_SendChar(c);
					CX80_Usart_SendChar(' ');
					CX80_Usart_SendChar(c);
				}
			}
			else
				if (receivedChars < bufsize - 1)
				{
					*buf++ = c;
					receivedChars++;
					
					CX80_Usart_SendChar(c);
				}
		}
	}
	
	*buf++ = 0;
	CX80_Usart_SendChar('\r');
	
	return receivedChars;
}
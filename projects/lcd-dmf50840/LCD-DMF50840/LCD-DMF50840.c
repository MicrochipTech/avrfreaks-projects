//
// LCD controller for Optrex DMF-50840 display
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation, 
//  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// This is a QVGA monochrome display. 4 data bits.
// A single Atmega324P or Atmega644 does all the work; a 32 KB SRam chip is used to store the pixel data.
//
// This means 80 nibbles per horizontal line, 240 horizontal lines
// X coordinate (hor. position) is stored in 7 address bits. (PB0..PB6)
// Y coordinate  (vert. position) is store in 8 address bits. (PA0..PA7)
// The screen refresh is done in the Timer0 ISR using a software DotClk (PC6).
// HSYNC (PD3) and VSYNC (PD2) are also generated here, as is the ENABLE (PC0) line.
//
// The main program must assume the addressing ports (PB, PA) can change at any time.
// The main program will mainly write to the SRam and must reload the
// addressing ports for every write, and disable interrupts during the write.
// 
// The data port is set to output (write) by default; the refresh interrupt
// will change that temporarily to input (high-Z), so that the LCD doesn't fight with the
// AVR when it needs to read the 4 data bits.
// 
// The controller uses a large serial input buffer. 512 bytes seems plenty for the most demanding input.
// It accepts incoming characters at all times. There are a few short moments
// where interrupts are disabled, mostly to ensure atomic operations on 16 bit pointers
// or when writing to the SRam and of course, when an LCD line is being refreshed.
// The AVR will honor the serial input interrupt as soon as the TIMER0 interrupt is done.
//
// Graphics processing is done in the forever loop.
//
// DMF-50840 LCD specs:
//
// 320 x 1 x 240
// 4-bit parallel data interface.
// It takes 4 cycles per nibble refresh (200 nSec), implying a DotClk of 5 Mhz.
// No overscan is needed. The LCD actually receives data only about 30% of time.
//
// The 80 nibbles of line data is stored in SRam locations with Lower Address bits a0..a6 (Vx)
// The 240 lines are stored in SRam locations with Higher Address bits A7..A14 (Vy).
//
// This means that there are 240 x (128-80) + (256-240) * 128 = 13568 spare nibbles available.
//
// Current Pixel/nibble data are stored in Vpix, Vx and Vy. These are only used for the communication interface.
//
// We'll use the AVCX Serial Interface Protocol for now, extended and changed for B/W bitmaps.
//
// There are multi-byte commands plus the bitmaps, which expect 9,600 bytes (2 nibbles packed in a byte).
//
// Once the parser gets out of sync, it may gobble the next 9600 input bytes as it expects bitmap data.
// Therefore an input timeout is active. The timer countdown value will be reset everytime data is received.
// Once the timer triggers, we should check that the SerialIn buffer is empty (how could it not be) and
// then reset the parser state to idle. Check if there is a little hole when the timer and SerialIn interrupt happen
// almost simultaneously.
//
// Timer1 is running at 100 mSecs. to do LED blinking, inByte time-out, clock update general wait.
//
// Many commands expect the relevant pixel address to be nibble aligned. To enforce this, the nibble address
// is automatically aligned to the closest nibble address before the current pixel address (no change
// if already aligned). The pixel/nibble issue only applies to the X direction.
//
// There is no set background or foreground color. (white on black or black on white)
//
// DMF-50840 AVCX Graphics Protocol Changes
//
//	<SETX>. <SETY>, <SETX,PSET> & <SETY,PSET> now all take a 2 byte argument (pixel address)
//	<DRAW_LINE> command (0xA8) takes 4 two-byte arguments (pixel address): x0 (2), y0 (2) ,x1 (2) ,y1 (2)
//  <DRAW_CIRCLE> command (0xC8) takes 3 two-byte arguments (pixel address): x, y, radius
//	<DEMO> command (0xB8) takes a 1 byte argument (1,2,3 or 4)
//	<PICTURE> command now needs x0,y0 and x1,y1 nibble arguments before data bytes. Todo. sb pixel argument
// 		SetPicture takes x,y coordinates for upper left and lower right corners. Reverse order rows expected
//	<CLOCK_ON>, <CLOCK_OFF> commands.
//	<SETSCREEN> command with color and x,y coordinates (like <PICTURE>)
//
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "./config.h"
#include "./uart.h"
#include "./util.h"

typedef unsigned long u32;
typedef			 int s16;

#define NIBBLEALIGN(px) if (1) {px = (px + 3) & 0xfffc;}	//align on next 4 bit boundary

uint8 RxBuffer[RXSIZE];		//Serial In Receive Buffer
uint8 pwrup_state;
uint8 RefreshLine;
uint8 PDATA;
uint8 BDATA;
uint8 Vx, Vy;
uint  Vpix;
uint8 BMVx, BMVy;
uint  BMVpix;
uint8 ParseState, PrevParseCommand;
volatile uint8 *TailPtr;
volatile uint8 *HeadPtr;
volatile unsigned int RxFree;
uint8 GlobalFault;
uint8 CommandTimeout;
uint  NoActivityTimeout;
uint8 SecsCountdown;
volatile uint8 SecsToAdd;
uint8 TimeSecs, TimeMins, TimeHrs;
uint8 fShowClock, fStartClock, fAutoClock;
unsigned int BMByteCount;

extern prog_uint8_t Font8x8[][8];
extern prog_uint8_t DialData[];
extern prog_uint8_t DialDigits[];
extern prog_uint8_t SecPtrData[];
extern prog_uint8_t MinPtrData[];
extern prog_uint8_t HrPtrData[];
extern prog_uint8_t bitmap[];

#define COLORWHITE	0b00000001
#define COLORBLACK	0b00000000
#define BITSPERPIXEL	1
#define DATABITS		4
//
#define nLines	240
#define lastLine (nLines - 1)
#define nHorPixels 320
#define lastHorPixel (nHorPixels - 1)
#define	nDataPerLine ((nHorPixels * BITSPERPIXEL) / DATABITS)
#define MaxPictureBytes  (nLines * (nHorPixels / 8))
//
// a cluster is the lowest common denominator of
// pixels and memory data (bytes/nibbles)
// 
#define nClustersPerLine (nHorPixels / DATABITS)
#define nLinesPerBar 20

void LCDDemo(uint8 val);
void ProcessCommands(void);
void Serial_In (uint8 data);
void DrawTable(prog_uint8_t *Table, uint8 erase);

extern void InitMem(void);
#if DEBUG
uint8 ReadNibble(uint8 x, uint8 y)   __attribute__ ((noinline));
void WriteNibble(uint8 val, uint8 x, uint8 y)   __attribute__ ((noinline));
#else
uint8 ReadNibble(uint8 x, uint8 y);
void WriteNibble(uint8 val, uint8 x, uint8 y);
#endif
void SetPixel(uint8 color, uint x, uint8 y);
void PutChar(uint8 ch);
void Parse(uint8 inByte);
//
// Enable debug led
//
void InitDebug(void)
{
#if USE_LED
	cbi(LEDO,LED_BIT);			//low
	sbi(LED_DDR,LED_BIT);			//output
#endif
}

const char DiagStr0[] PROGMEM = "DebugL0\n\r";
const char DiagStr1[] PROGMEM = "\n\rDebugL1: 0x";
const char DiagStr2[] PROGMEM = "DebugL2\n\r";
const char DiagStr3[] PROGMEM = "DebugL3\n\r";
const char DiagStr4[] PROGMEM = "DebugL4\n\r";
const char DiagStr5[] PROGMEM = "DebugL5\n\r";
const char DiagStr6[] PROGMEM = "\n\rDebugL6  ";
const char DiagStr7[] PROGMEM = "         ";
const char DiagStr8[] PROGMEM = ".  ";
const char DiagStr9[] PROGMEM = "\n\rDebugL9  ";
const char DiagStr10[] PROGMEM = "\n\rDebugL10  ";

void DebugL0(void)  __attribute__ ((noinline));
void DebugL0(void)
{
	UART_PrintfProgStr_I((prog_char*)DiagStr0);
	FlashLed(5, 1);
}

void DebugL1(void)
{
	uint8 *ptr;
	UART_PrintfProgStr_I((prog_char*)DiagStr1);
	UART_Printfu16((int)TailPtr);
	UART_SendByte(' ');
	FlashLed(10, 1);
	//
	// now print last 32 bytes from input queue
	//
	for (int i = 32; i >= 1; --i) {
		ptr = (uint8 *)(TailPtr - i);
		if (ptr < &RxBuffer[0]) ptr += RXSIZE;
		UART_Printfuint8(*ptr);
		UART_SendByte(' ');
	}
	UART_PrintfEndOfLine();
}

void DebugL2(void)
{
	UART_PrintfProgStr_I((prog_char*)DiagStr2);
	FlashLed(10, 2);
}

void DebugL3(void)
{
	UART_PrintfProgStr_I((prog_char*)DiagStr3);
	FlashLed(10, 3);
}

void DebugL4(void)
{
	UART_PrintfProgStr_I((prog_char*)DiagStr4);
	FlashLed(10, 4);
}

void DebugL5(void)
{
	UART_PrintfProgStr_I((prog_char*)DiagStr5);
	FlashLed(10, 5);
}
//
// Start Memory Test
//
// write to memory. Max 128 nibbles per line
//
void MemTest(void)
{
	uint8 i, j, tmp;
	Vy = 0;
	do {
		Vx = 0;
		i = 120;
		do {
			WriteNibble(Vx & 0x0f, Vx, Vy);		//Vx, Vy
			++Vx;
		} while (--i);
	} while (++Vy);						// until wrap
//
//  read it back.
//
	Vy = 0;
	do {
		Vx = 0;
		i = 120;
		do {
			j = ReadNibble(Vx, Vy);			//Vx, Vy
			tmp = (uint8)(Vx & 0x0f);
			if (j != tmp) {
				DebugL0();
			}
			++Vx;
		} while (--i);
	} while (++Vy);						// until wrap
}
//
// TC0 is used to do the LCD Screen Refresh
//
// Initialize TC0
// Set prescaler to clkio/64. At 20 Mhz, this is 3.2 uSec
// Using a Timer0CountUp value of 21 yields an match every 67.2 uSec.
// resulting in a refresh rate of 62 (62 x 240 = 14,880 Hz -> 67.2 uSec)
//
#if F_CPU==20000000UL
#define Timer0CountUp 21		// increase Refresh Rate for video: use 10
#elif F_CPU==8000000UL
#define Timer0CountUp 11
#else
error: F_CPU undefined
#endif

void InitTC0(void)
{
	TCCR0A = 0;
	TCCR0B = (0<<CS02)|(1<<CS01)|(1<<CS00);
	OCR0A = Timer0CountUp;
}

void EnableTC0(void)
{
	TIFR0 = (1<<OCF0A);		//clear pending match
	TIMSK0 = (1<<OCIE0A);	//Enable TC0.match
}

//
// Timer1 is setup in util.c but calls the application
// specific function "AppTimer1Stuff"
//
void AppTimer1Stuff(void)
{
	if (--SecsCountdown == 0) {
		SecsCountdown = 10;
		++SecsToAdd;
	}
	if (CommandTimeout != 0) {
		if (--CommandTimeout == 0) {
			// Command Time out. Reset State Machine
			ParseState = STANDBY;
		}
	}
	if (NoActivityTimeout != 0) {
		if (--NoActivityTimeout == 0) {
			// No Activity Timeout. Start Clock
			// Ought to save current image, then clrscreen first
			fStartClock = TRUE;
			fAutoClock = TRUE;
		}
	}
}

void InitDebug(void);

void InitCtrl(void)
{
//
// I/O PORTS INITIALIZATION
//
	sbi(SRAM_WE, WE_BIT);		//active low
	sbi(WE_DDR, WE_BIT);		//set as output
#if USE_CE
	sbi(SRAM_CE, CE_BIT);		//active low
	sbi(CE_DDR, CE_BIT);		//set as output
#endif



	SRAMVX = 0;					//set as output
	SRAMVY = 0;
	SRAMVX_DDR = 0x7f;			//pin 7 not used
	SRAMVY_DDR = 0xff;
#if DEFDATAPORTOUT
 	SRAMDATA_DDR = 0b11111100;	//data port defaults to output
#else
  	SRAMDATA_DDR = 0b00001100;	//data port defaults to input
#endif

	cbi(LCD_VSYNC, VSYNC_BIT);	//active High
	cbi(LCD_HSYNC, HSYNC_BIT);
	cbi(LCD_ENABLE, ENABLE_BIT);//disable LCD
	cbi(DOTCLK, DOTCLK_BIT);	//active on falling edge

	sbi(HSYNC_DDR, HSYNC_BIT);	//set all as output
	sbi(VSYNC_DDR, VSYNC_BIT);
	sbi(DOTCLK_DDR, DOTCLK_BIT);
	sbi(ENABLE_DDR, ENABLE_BIT);
#if M-LINE
	sbi(LCDM_DDR, LCDM_BIT);
#endif
//
// Disable unused parts of the AVR to reduce power consumption
//
// unused TWI, ADC, UART1, TIM2, TIM1, SPI
//
// Reserve TIM1 for inByte timeout
//
	PRR0 = _BV(PRTWI) | _BV(PRADC) | _BV(PRTIM2) | _BV(PRSPI);

	InitTC0();					//LCD refresh timer
	InitTC1();					//Timeout/Led
	InitDebug();
//
// VARIABLES INITIALIZATION
//
	RefreshLine = 0;			//LCD Line being displayed
	pwrup_state = 0;
	ParseState = 0;				//GrState = STANDBY
	PrevParseCommand = 0;			//nothing
	PDATA = COLORWHITE;
	BDATA = COLORBLACK;
	Vpix = Vx = Vy = 0;
//
// initialize Serial In buffer pointers
//
	HeadPtr = TailPtr = &RxBuffer[0];
	RxFree = RXSIZE;
	GlobalFault = 0;
	SecsCountdown = 10;
	SecsToAdd = 0;
	CommandTimeout = 0;
	NoActivityTimeout = NOACTIVITY;
	BlinkFastNoInts();
}
//
// Serial In Queue HANDLER
//
// Store 'data' to queue pointed to by HeadPtr
// This code must always run with ints disabled
//
// HeadPtr points to first free position
// TailPtr points to oldest entry
//
void Serial_In (uint8 data)
{
	*HeadPtr++ = data;
	--RxFree;
	if (HeadPtr >= &RxBuffer[RXSIZE]) {
		HeadPtr = RxBuffer;
	}
	if (HeadPtr == TailPtr) {	//overflow test
		if (RxFree != 0) {
			GlobalFault = 1;
		}
		RxFree = 1;
		--HeadPtr;
		if (HeadPtr < RxBuffer) {
			HeadPtr = &RxBuffer[RXSIZE-1];
		}
	}
}

//
// RS232 RECEIVE EVENT HANDLER
//
// Serial Input Buffer pointers
//
ISR(USART0_RX_vect)
{
	uint8 ch;
//
// Get Data from RS232
//
	ch = UDR0;
	Serial_In(ch);
}

//
// WriteNibble:	write a nibble to SRam.
//
// IN:	r24:data	r22 x-address	r20 y-address
//
// SRAMDATA upper 4 bits default setting is output
//
void WriteNibble(uint8 data, uint8 x, uint8 y)
{
	uint8	CpuStatus;

	CpuStatus = SREG;			//save interrupt status
	cli();
#if !DEFDATAPORTOUT
  	SRAMDATA_DDR = 0b11111100;	//data port to output
	asm volatile (
		"nop	\n\t"
	);
#endif
	//
	// only HSYNC and VSYNC relevant
	// in lower 4 bits, which are always low here
	//
	SRAMDATAO = (data << 4);	// | (SRAMDATAI & 0b00001111);
	SRAMVX = x;
	SRAMVY = y;
#if USE_CE
	cbi(SRAM_CE, CE_BIT);
#endif
	cbi(SRAM_WE, WE_BIT);
	sbi(SRAM_WE, WE_BIT);
#if USE_CE
	sbi(SRAM_CE, CE_BIT);
#endif
#if !DEFDATAPORTOUT
  	SRAMDATA_DDR = 0b00001100;	//data port back to input
#endif
	SREG = CpuStatus;			//restore interrupt status
}

uint8 ReadNibble(uint8 x, uint8 y)
{
	uint8 data, CpuStatus;
	CpuStatus = SREG;			//save interrupt status
	cli();
	data = (x & 0x0f);
#if DEFDATAPORTOUT
	SRAMDATA_DDR = 0b00001100;	//set upper 4 bits as input
#endif
	SRAMVX = x;
	SRAMVY = y;
#if USE_CE
	cbi(SRAM_CE, CE_BIT);
#endif
	// 2 nops are necessary when running at 20 Mhz (100 nSecs delay)
	asm volatile (
		"nop	\n\t"
		"nop	\n\t"
	);
	data = (SRAMDATAI >> 4);
#if USE_CE
	sbi(SRAM_CE, CE_BIT);
#endif
#if DEFDATAPORTOUT
	SRAMDATA_DDR = 0b11111100;	//set all relevant bits as output
#endif
	SREG = CpuStatus;			//restore interrupt status
	return data;
}
//
// Write a character at the current pixel location.
// May not wrap Vpix and Vy properly
//
void PutChar(uint8 ch)
{
	uint8	i,FontLine,px;
	prog_uint8_t *pCode;

	NIBBLEALIGN(Vpix);
	if ((Vpix + 8) >= nHorPixels) {			// char won't fit on current line
		Vpix = 0;
		++Vy;								// Line overflow check below
	}
	px = (Vpix >> 2);						// Convert to nibble
	if ((Vy + 8) > nLines) {				// char won't fit on screen
		Vy = 0;
	}
	//
	// write One Line at a time
	for (i = 0; i < 8; i++) {
		pCode = &(Font8x8[ch][i]);
		FontLine = __LPM(pCode);
		WriteNibble(FontLine >> 4, px, Vy+i);
		WriteNibble(FontLine & 0x0f, px+1, Vy+i);
	}
	Vpix += 8;
}
//
// Set Pixel at location x, y to color
// Should use 4 byte table to compute bitmask
//
void SetPixel(uint8 color, uint x, uint8 y)
{
	uint8 v, offset, bitmask, px;
	px = (x >> 2);				//enclosing nibble
	v = ReadNibble(px, y);
	offset = x & 0x03;
	bitmask = 8;
	while (offset > 0) {
		bitmask >>= 1;
		offset--;
	}
	v &= ~bitmask;		//clear pixel
	if (color) {
		v |= bitmask;
	}
	WriteNibble(v, px, y);
}
//
// Bresenham line drawing
//
void DrawLine(uint x0, uint y0, uint x1, uint y1)
{
	uint steep, tmp;
	u32 x, y;
	s16 ystep;
	int deltax, deltay, error;

#if DEBUG
	if ((x0 >= 320) || (x1 >= 320) || (y0 >= 240) || (y1 >= 240)) return;
#endif

	steep = (abs(y1 - y0) > abs (x1 - x0));
	if (steep) {
		tmp = x0; x0 = y0; y0 = tmp;		//  swap(x0, y0)
		tmp = x1; x1 = y1; y1 = tmp;        //  swap(x1, y1)
	}
    if (x0 > x1) {
		tmp = x0; x0 = x1; x1 = tmp;		// swap(x0, x1)
		tmp = y0; y0 = y1; y1 = tmp;		// swap(y0, y1)
	}
	deltax = x1 - x0;
	deltay = abs(y1 - y0);
	error = 0;
	y = y0;
	if (y0 < y1) ystep = 1;
	else ystep = -1;
	for (x = x0; x <= x1; x++) {
		if (steep) SetPixel(PDATA, y, x);
		else SetPixel(PDATA, x, y);
		error += deltay;
		if ((error * 2) >= deltax) {
			y += ystep;
			error -= deltax;
		}
	}
}

//
// Bresenham circle drawing
//
void DrawCircle(unsigned int cx, unsigned int cy ,unsigned int radius)
{
	int x, y, xchange, ychange, radiusError;
	x = radius;
	y = 0;
	xchange = 1 - 2 * radius;
	ychange = 1;
	radiusError = 0;
	while(x >= y) {
	  SetPixel(PDATA, cx+x, cy+y ); 
	  SetPixel(PDATA, cx-x, cy+y ); 
	  SetPixel(PDATA, cx-x, cy-y );
	  SetPixel(PDATA, cx+x, cy-y ); 
	  SetPixel(PDATA, cx+y, cy+x ); 
	  SetPixel(PDATA, cx-y, cy+x ); 
	  SetPixel(PDATA, cx-y, cy-x ); 
	  SetPixel(PDATA, cx+y, cy-x ); 
	  y++;
	  radiusError += ychange;
	  ychange += 2;
	  if ( 2*radiusError + xchange > 0 ) {
	    x--;
		radiusError += xchange;
		xchange += 2;
		}
	}
}

void DrawTable(prog_uint8_t *Table, uint8 erase)
{
	prog_uint8_t *pCode = Table;
	uint8 x0,y0,x1,y1;
	uint8 SavedColor = PDATA;

	if (erase) {
		PDATA = BDATA;
	}

	if (fShowClock) {
		do {
			while ((x0 = __LPM(pCode)) == 0) {		//skip leading 0 bytes
				++pCode;
			}
			++pCode;
			ASSERT (x0 != 0);
			y0 = 240-__LPM(pCode);
			++pCode;
			do {
				x1 = __LPM(pCode);
				++pCode;
				if (0 == x1) break;				//move to next coordinate pair
				if (0xff == x1) {				//end of coordinate pairs
					if (erase) {
						BDATA = PDATA;
						PDATA = SavedColor;
					}
					return;
				}
				y1 = 240-__LPM(pCode);
				++pCode;
				DrawLine(x0,y0, x1,y1);
				x0 = x1;
				y0 = y1;
			} while (1);
		} while (1);
	}
	if (erase) {
		BDATA = PDATA;
		PDATA = SavedColor;
	}
}

void DrawSecsHand(uint8 sec, uint8 erase)
{
	if (sec >= 60)
		return;

	DrawTable(&SecPtrData[sec * 5], erase);
}

void DrawMinsHand(uint8 min, uint8 erase)
{
	if (min >= 60)
		return;

	DrawTable(&MinPtrData[min * 9], erase);
}

void DrawHrsHand(uint8 hr, uint8 erase)
{
	if (hr >= 12)
		return;

	DrawTable(&HrPtrData[hr * 45], erase);
}

void UpdateTime(void)
{
	while (SecsToAdd > 0) {
		--SecsToAdd;
		DrawSecsHand(TimeSecs, TRUE);			//erase current seconds hand
		++TimeSecs;
		if (TimeSecs >= 60) {
			DrawMinsHand(TimeMins, TRUE);		//erase current minutes hand
			TimeSecs = 0;
			++TimeMins;
			if (TimeMins >= 60) {
				DrawHrsHand(TimeHrs, TRUE);		//erase current hours hand
				TimeMins = 0;
				++TimeHrs;
				if (TimeHrs >= 12) {
					TimeHrs = 0;
				}
			}
		}
		DrawTable(DialDigits, FALSE);
		DrawSecsHand(TimeSecs, FALSE);
		DrawMinsHand(TimeMins, FALSE);
		DrawHrsHand(TimeHrs, FALSE);
	}
}

void StartClock(void)
{
	fStartClock = FALSE;
	fShowClock = TRUE;
	DrawTable(DialData, FALSE);
	DrawTable(DialDigits, FALSE);
	DrawSecsHand(TimeSecs, FALSE);
	DrawMinsHand(TimeMins, FALSE);
	DrawHrsHand(TimeHrs, FALSE);
}

void StopClock(void)
{
	DrawTable(DialData, TRUE);
	DrawTable(DialDigits, TRUE);
	DrawSecsHand(TimeSecs, TRUE);
	DrawMinsHand(TimeMins, TRUE);
	DrawHrsHand(TimeHrs, TRUE);
	fShowClock = FALSE;
}

uint8 PrepareNibble(uint8 color)
{
#if DEBUG
	color &= 0x01;
#endif
	return (color) ? 0b00001111 : 0;
}
//
// set screen area to color. Set BDATA, Vx, Vy, Vpix
//
void SetScreenX(uint8 color, uint x0, uint8 y0, uint x1, uint8 y1)
{
	uint8 nibble, x, y;
//
// convert pixel address X0 to nibble boundary
// align to next nibble boundary
//
	NIBBLEALIGN(x0);
	NIBBLEALIGN(x1);
	nibble = PrepareNibble(color);
	for (y = y0; y <= y1; ++y) {		// Top to Bottom
		for (x = (x0 >> 2) ; x <= (x1 >> 2); ++x) {
			WriteNibble(nibble, x, y);
		}
	}
	BDATA = color;						//set color as background color
	Vy = y0;
	Vx = (x0 >> 2);
	Vpix = (Vx << 2);
}

//
// set screen to color. Set BDATA, Vx, Vy, Vpix
//
void SetScreen(uint8 color)
{
	SetScreenX(color, 0, 0, nHorPixels - DATABITS , nLines - 1);
}

#if DEBUG
//
// Draw a line using argument color. Starting at current Vy
// Use WriteNibble()
//
void HorizontalLine(uint8 color)
{
	uint8 x, nibble;
	nibble = PrepareNibble(color);
	for (x = 0; x < nDataPerLine; ++x) {
		WriteNibble(nibble, x, Vy);
	}
}

//
// Draw a line using argument color. Starting at current Vy.
// Use SetPixel
//
void HorizontalPixelLine(uint8 color)
{
	PDATA = color;
	uint px;
	for (px = 0; px < (nHorPixels/16); ++px) {
		SetPixel(color, px, Vy);
	}
}
#endif

//
// Draw a bar using argument color. Starting at current Vy
//
void HorizontalBar(uint8 color)
{
	uint8 i, x, nibble;
	i = nLinesPerBar;			//number of lines
	nibble = PrepareNibble(color);
	do {
		for (x = 0; x < nDataPerLine; ++x) {
			WriteNibble(nibble, x, Vy);
		}
		++Vy;
	} while (--i);
	Vx = 0;
}
//	
// horizontal test bars.
//
void HorizontalTest(void)
{
	uint8 i;
	Vy = 0;
	for (i = 0; i < (nLines/(2 * nLinesPerBar)); ++i) {		//as many as will fit
		HorizontalBar(COLORWHITE);
		HorizontalBar(COLORBLACK);
	}
#if DEBUG
	if (Vy != nLines) {
		DebugL0();
	}
#endif
	Vx = Vy = 0;
}
//
//  compare data with PDATA
//
void VerifyNibble(uint8 data)
{
	// bit 0: D0 .. bit 3: D0
	if ((data & 0x01) != PDATA) DebugL0();
	data >>= 1;
	if ((data & 0x01) != PDATA) DebugL0();
	data >>= 1;
	if ((data & 0x01) != PDATA) DebugL0();
	data >>= 1;
	if ((data & 0x01) != PDATA) DebugL0();
}

void VerifyHorizontalLine(void)
{
	uint8 nibble, i = nClustersPerLine;
	Vx = 0;
	do {
		nibble = ReadNibble(Vx, Vy);
		++Vx;
		VerifyNibble(nibble);		//compare with PDATA
	} while (--i);
}
//
// argument: expected color 
//
void VerifyHorizontalBar(uint8 color)
{
	uint8 i = nLinesPerBar;
	PDATA = color;			//save
	do {
		VerifyHorizontalLine();
		++Vy;
	} while (--i);
}

void VerifyHorizontalTest()
{
	Vy = 0;
	VerifyHorizontalBar(COLORWHITE);
	VerifyHorizontalBar(COLORBLACK);
	VerifyHorizontalBar(COLORWHITE);
	VerifyHorizontalBar(COLORBLACK);
	VerifyHorizontalBar(COLORWHITE);
	VerifyHorizontalBar(COLORBLACK);
	VerifyHorizontalBar(COLORWHITE);
	VerifyHorizontalBar(COLORBLACK);
	VerifyHorizontalBar(COLORWHITE);
	VerifyHorizontalBar(COLORBLACK);
	VerifyHorizontalBar(COLORWHITE);
	VerifyHorizontalBar(COLORBLACK);
	Vx = Vy = 0;				//reset
	Vpix = 0;
}
//
// VerticalTest: draw vertical bars
//
void VerticalTest()
{
	uint8 color, i, nibble, x, y;
	for (y = 0; y < nLines; ++y) {
		x = 0;
		color = COLORBLACK;		//color
		for (i = 0; i < (nDataPerLine >> 1); ++i) {
			if (++color > COLORWHITE) {	//next color
				color = COLORBLACK;
			}
			nibble = PrepareNibble(color);	//set from PDATA
			WriteNibble(nibble, x, y);
			WriteNibble(nibble, x+1, y);
			x += 2;
		}
	}
	Vx = Vy = 0;				//reset
	Vpix = 0;
}

//
// HorVertTest: draw clusters
//
void HorVertTest()
{
	uint8 i, j, color0, color, nibble, x, y;
	color0 = COLORBLACK;				//start color
	for (y = 0; y < nLines;) {
		if (++color0 > COLORWHITE) {	//next color
			color0 = COLORBLACK;
		}
		color = color0;
		for (j = 0; j < 8; ++j) {		//nr of lines per block
			x = 0;
			for (i = 0; i < (nDataPerLine >> 1); ++i) {
				nibble = PrepareNibble(color);	//set  from PDATA
				WriteNibble(nibble, x, y);
				WriteNibble(nibble, x+1, y);
				x += 2;
				if (++color > COLORWHITE) {	//next color
					color = COLORBLACK;
				}
			}
			++y;
		}
	}
	Vx = Vy = 0;			//reset
	Vpix = 0;
}
//
// Draw horizontal bars slowly
//
void SlowHorizontalBars(void)
{
	uint8 i;
	Vy = 0;
	for (i = 0; i < (nLines/(2 * nLinesPerBar)); ++i) {		//as many as will fit
		HorizontalBar(COLORWHITE);
		HorizontalBar(COLORBLACK);
		delayOneSecond();
	}
	Vx = 0;
	Vy = 0;
}

//
// Select a demo. All delays done by sender
//
void LCDDemo(uint8 val)
{
	if (val == 1) {
		HorizontalTest();
		VerifyHorizontalTest();
	} else if (val == 2) {
		VerticalTest();
	} else if (val == 3) {
		HorVertTest();
	} else if (val == 4) {
		SlowHorizontalBars();		//takes a while
	}
}

// for Line Command
uint8 SavedArgs[9];
uint8 SavedIdx;

void Parse(uint8 inByte)
{
	uint8 nAddress;
	//
	// Setting CommandTimeout to 50 gives us 5 second
	// before the ParseState is set to STANDBY
	// Timeout will never occur while inside Parse() - that's the intent
	//
	CommandTimeout = 50;
	if (ParseState == STANDBY) {
		//
		//Decode inByte
		//
  		if ((inByte & SINGLEBYTEMASK) != SINGLEBYTECMD) {
			//
			//MultiByteCommand
			//
			PrevParseCommand = inByte;
			ParseState = inByte & MULTIBYTEMASK;
		}
		else {
			//
			//SingleByteCommand
			//
			if (CLEAR_SCR == inByte) {
				SetScreen(COLORBLACK);		// sets BDATA
				PDATA = COLORWHITE;
			} else if (CLOCK_ON == inByte) {
				StartClock();
			} else if (CLOCK_OFF == inByte) {
				StopClock();
			}
			//
			// done with unparameterized single byte commands.
			// Must be <PSET>, <X++>, <Y++>, <X-->, <Y-->, <X++,PSET>, <Y++,PSET>, <X--,PSET> or, <Y--,PSET>
			//	
			else if ((inByte & PSETINCDECMASK) == PSETINCDECCMD) { //Increment, Decrement, PSET
				if (inByte & 0b00000100) {
					SetPixel(PDATA, Vpix, Vy);
				}
				//
				// Now check for X/Y increment or decrement
				nAddress = (inByte & 0b000000011);//X, Y or nothing
				if (nAddress == 0b00000001) {			//X
					if (inByte & 0b00001000) {	//Decrement
						if (--Vpix == 0xffff) Vpix = lastHorPixel;
					} else {							//Increment
						if (++Vpix == nHorPixels) Vpix = 0;
					}
				} else if (nAddress == 0b00000010) {	//Y
					if (inByte & 0b00001000) {	//Decrement
						if (--Vy == 0xff) Vy = nLines;
					} else {							//Increment
						if (++Vy == nLines) Vy = 0;
					}
				} else {								//else nothing
					// CLEAR_SCR uses this field, value 0b00000011
				}
			} //PSETINCDECCMD
			else { // UNKNOWN or UNIMPLEMENTED
			//ignore since it's single byte
			}
		} // SingleByteCommand
	} //ParseState == STANDBY
	else if (ParseState == PICTURE) {
		//
		// TODO. sb pixel argument for x. If x0 or x1 not aligned
		// on nibble boundary, discard the data.
		// first 4 byte arguments (x0, y0, x1, y1. Nibble addresses
		// then data: (((x1 - x0 + 1) * (y1 - y0 + 1)) / 2) bytes
  		if ((PrevParseCommand & MULTIBYTEMASK) == PICTURE) {
			//First time through.
			SavedIdx = 0;
			PrevParseCommand &= ~MULTIBYTEMASK;
		}
		// 4 bytes argument: X0,Y0,X1,Y1 (nibble addresses)
		SavedArgs[SavedIdx++] = inByte;
		if (SavedIdx >= 4) {	// collected all args
			BMByteCount = ((SavedArgs[2] - SavedArgs[0] + 1) * (SavedArgs[3] - SavedArgs[1] + 1) / 2);
			if (BMByteCount > MaxPictureBytes) {
				DebugL1();
			}
			//
			// TODO: no need for separate BMVx and BMVy as long as
			// Vx and Vy are ONLY used for protocal state values
			//
			if (SavedArgs[0] >= nClustersPerLine) SavedArgs[0] = 0;
			BMVx = SavedArgs[0];
			if (SavedArgs[3] >= nLines) SavedArgs[3] = nLines - 1;
			BMVy = SavedArgs[3];
			ParseState = PICTURE_DATA;
#if DEBUG
			PrevParseCommand = PICTURE;
#endif
		}
	}	// PICTURE
	else if (ParseState == PICTURE_DATA) {
#if DEBUG
		if (PrevParseCommand != PICTURE) {
			// inconsistency
				DebugL1();
		}
#endif
		// 2 nibbles packed per byte, packed Upper nibble, then Lower nibble
		// works as long as "x1 - x0 + 1" is even
		// Data come in bottom of area lines first
		WriteNibble((inByte >> 4), BMVx, BMVy);
		WriteNibble((inByte & 0x0f), BMVx+1, BMVy);
		BMVx += 2;
		if ((BMVx - 1) == SavedArgs[2]) {
			BMVx = SavedArgs[0];
			if (--BMVy == SavedArgs[1] - 1) {
				BMVy = 0;
			}
		}
		if (--BMByteCount == 0) {
			ParseState = STANDBY;
		}

	} //PICTURE_DATA
	else if ((ParseState & MULTIBYTEMASK) == COLOR) {
		PDATA = (PrevParseCommand & 0x07);
		BDATA = PDATA ? 0 : 1;
		// ignore current byte in inByte
		ParseState = STANDBY;
	}
	else if (ParseState == SET) {
  		if ((PrevParseCommand & MULTIBYTEMASK) == SET) {
			//First time through.
			SavedIdx = 0;
			PrevParseCommand &= ~MULTIBYTEMASK;
		}
		// 2 bytes argument: X or Y
		SavedArgs[SavedIdx++] = inByte;
		if (SavedIdx >= 2) {	// collected all args
			if (PrevParseCommand & 0b00000001) Vpix = SavedArgs[0] | (SavedArgs[1] << 8);
			else Vy = (uint8)(SavedArgs[0] | (SavedArgs[1] << 8));
			if (PrevParseCommand & 0b00000100) {
				SetPixel(PDATA, Vpix, Vy);
			}
			ParseState = STANDBY;
		}
	}
	else if (ParseState == PRINT_STRING) {
		//
		// Print String State. Check if NULL
		//
		if (inByte != 0) {
			PutChar(inByte);
		}
		else {
			ParseState = STANDBY;	// End of String
		}
	}
	else if (ParseState == PUT_CHAR) {
		PutChar(inByte);
		ParseState = STANDBY;
	}
	else if (ParseState == DRAW_LINE) {
  		if ((PrevParseCommand & MULTIBYTEMASK) == DRAW_LINE) {
			//First time through.
			SavedIdx = 0;
			PrevParseCommand &= ~MULTIBYTEMASK;
		}
		// 8 bytes argument: X0,Y0,X1,Y1
		// X0, X1, Y0 and Y1 are 2 bytes each (low, high)
		SavedArgs[SavedIdx++] = inByte;
		if (SavedIdx >= 8) {	// collected all args
			DrawLine(SavedArgs[0] | (SavedArgs[1] << 8),
					 SavedArgs[2] | (SavedArgs[3] << 8),
					 SavedArgs[4] | (SavedArgs[5] << 8),
					 SavedArgs[6] | (SavedArgs[7] << 8));
			//
			// Set current coordinate to last pixel drawn
			//
			Vpix = SavedArgs[4] | (SavedArgs[5] << 8);
			Vy = SavedArgs[6] | (SavedArgs[7] << 8);
			ParseState = STANDBY;
		}
	}
	else if (ParseState == DRAW_CIRCLE) {
  		if ((PrevParseCommand & MULTIBYTEMASK) == DRAW_CIRCLE) {
			//First time through.
			SavedIdx = 0;
			PrevParseCommand &= ~MULTIBYTEMASK;
		}
		// 6 bytes argument: X,Y,Radius
		// X, Y and Radius are 2 bytes each (low, high)
		SavedArgs[SavedIdx++] = inByte;
		if (SavedIdx >= 6) {	// collected all args
			DrawCircle(SavedArgs[0] | (SavedArgs[1] << 8),
					   SavedArgs[2] | (SavedArgs[3] << 8),
					   SavedArgs[4] | (SavedArgs[5] << 8));
			//
			// Set current coordinate to center of circle
			//
			Vpix = SavedArgs[0] | (SavedArgs[1] << 8);
			Vy = SavedArgs[2] | (SavedArgs[3] << 8);
			ParseState = STANDBY;
		}
	}
	else if (ParseState == SET_SCREEN) {
  		if ((PrevParseCommand & MULTIBYTEMASK) == SET_SCREEN) {
			//First time through.
			SavedIdx = 0;
			PrevParseCommand &= ~MULTIBYTEMASK;
		}
		// 9 bytes argument: color, X0,Y0,X1,Y1
		// color is 1 byte. X0, X1, Y0 and Y1 are 2 bytes each (low, high)
		SavedArgs[SavedIdx++] = inByte;
		if (SavedIdx >= 9) {	// collected all args
			SetScreenX(	SavedArgs[0],
						SavedArgs[1] | (SavedArgs[2] << 8),				//x0
						(uint8)(SavedArgs[3] | (SavedArgs[4] << 8)),	//y0
						SavedArgs[5] | (SavedArgs[6] << 8),				//x1
						(uint8)(SavedArgs[7] | (SavedArgs[8] << 8)));	//y1
			ParseState = STANDBY;
		}
	}
	else if (ParseState == DEMO) {
		LCDDemo(inByte);		//select Demo
		ParseState = STANDBY;
	}
	else {
		DebugL1();
		ParseState = STANDBY;
	}
#if DEBUG
	if (STANDBY == ParseState) {
		UART_SendByte('.');
	}
#endif
}
//
// Graphic Commands Processing
//
void ProcessCommands()
{
	uint8 inByte;
	while (1) {			// while (HeadPtr != TailPtr)
		cli();
		if (HeadPtr == TailPtr) {
			sei();
			break;		// nothing to do
		}
		inByte = *TailPtr++;
		RxFree++;
		if (TailPtr >= &RxBuffer[RXSIZE]) {
			TailPtr = &RxBuffer[0];
		}
		sei();
		if (fAutoClock) {
			fAutoClock = FALSE;
			StopClock();
		}
		Parse(inByte);
		NoActivityTimeout = NOACTIVITY;		// 1 minute
 	}				 //END while (TailPtr != HeadPtr)
}

const char StartStr1[] PROGMEM = "\r\nStart LCD Controller\r\n";

void Preset(void)
{
	Serial_In(CLEAR_SCR);		//clear Screen
	Serial_In(PRINT_STRING);	//String
  	Serial_In('R');
	Serial_In('e');
	Serial_In('a');
	Serial_In('d');
	Serial_In('y');
	Serial_In(0);
}

int main(void)
{
	unsigned long i = 0;
	MCUSR = 0;
#if DEBUG
	InitMem();
#endif
	InitCtrl();
	UART_Init();
	MemTest();

	fShowClock = FALSE;
	fStartClock = FALSE;
	fAutoClock = FALSE;
	TimeSecs = 0;
	TimeMins = 0;
	TimeHrs = 1;
#if SPLASHSCREEN
//
// should compress image before enabling this feature
//
	prog_uint8_t *pCode = bitmap;
	uint8 j, v;
	int k;
	for (k = nLines - 1; k >= 0; --k) {
		for (j = 0; j < (nDataPerLine); j += 2) {
				v = __LPM(pCode++);
				WriteNibble(v >> 4, j, k);
				WriteNibble(v & 0x0f, j + 1, k);
		}
	}
#else
	Preset();		// text to lines 0..7 through SerialIn()
#endif
//
//	Enable Timer0 & Timer1 in Interrupt Mask
//
	EnableTC0();
	EnableTC1();
	sei();							//start interrupts -> screen refresh, serial in
	UART_PrintfProgStr((prog_char*)StartStr1);

	do {
		//
		// process queued graphic commands from SerialIn
		//
		ProcessCommands();

		if (GlobalFault) {
			DebugL0();
		}
		//
		// Signal a low RX buffer warning. Wait 1 second
		// before issuing another warning
		//
		if ((RxFree < 100) && (Timer1Counter2 == 0)) {
			UART_SendByte('!');
			Timer1Counter2 = 10;
		}

		if (++i == 0x4ffff) {
			FlashLed(1, 5);
			i = 0;
		}

		if (fStartClock) {
			SetScreen(COLORBLACK);		// sets BDATA
			PDATA = COLORWHITE;
			StartClock();
		}
		UpdateTime();

	} while (1);
}



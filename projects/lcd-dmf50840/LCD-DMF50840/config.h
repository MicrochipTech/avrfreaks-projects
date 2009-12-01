//
// config.h
//
// wired as follows:
//	Vy SRam A7..A14: PA0..PA7
//  Vx SRam A0..A6: PB0..PB6
//  PB7: generic input
//  Serial port: PD0..PD1
//	VSYNC: PD2
//  HSYNC: PD3
//	I/O: PD4..PD7 
//	ENABLE: PC0
//	WE: PC1
//	DOTCLK: PC6
//	DebugLED: PC7
//	OE: always LOW
//	CE: always LOW (or PC7)
//
#define	SRAMVX		PORTB
#define SRAMVX_DDR	DDRB
//
// PA0..PA7 connected to A7..A14 (Vy)
//
#define	SRAMVY		PORTA
#define	SRAMVY_R	PINA
#define	SRAMVY_DDR	DDRA
//
// PD4..PD7 connected to D0..D3 (nibble data)
//
#define	SRAMDATAI	PIND
#define	SRAMDATAO	PORTD
#define	SRAMDATA_DDR DDRD
//
// PD3 connected to LCD HSYNC
//
#define	LCD_HSYNC	PORTD
#define	HSYNC_DDR	DDRD
#define HSYNC_BIT	3
//
// PD2 connected to LCD VSYNC
//
#define	LCD_VSYNC	PORTD
#define	VSYNC_DDR	DDRD
#define VSYNC_BIT	2
//
// PC0 connected to LCD Enable
//
#define	LCD_ENABLE	PORTC
#define	ENABLE_DDR	DDRC
#define ENABLE_BIT	0
//
// PC1 connected to SRam WE
//
#define	SRAM_WE		PORTC
#define	WE_DDR		DDRC
#define WE_BIT		1
//
// PC6 connected to software DOTCLOCK
//
#define	DOTCLK		PORTC
#define	DOTCLK_DDR	DDRC
#define DOTCLK_BIT	6
//
// PC7 connected to SRam CE or LED
//
#define	SRAM_CE		PORTC
#define	CE_DDR		DDRC
#define	CE_BIT		7

#define	LEDI		PINC
#define	LEDO		PORTC
#define	LED_DDR		DDRC
#define	LED_BIT		7

#ifndef cbi
	#define cbi(reg,bit)	reg &= ~(_BV(bit))
#endif
#ifndef sbi
	#define sbi(reg,bit)	reg |= (_BV(bit))
#endif

#define VERBOSE 1

#include ".\Commoncfg.h"

typedef unsigned char  uint8;
typedef          char  int8;
typedef unsigned int   uint;
typedef unsigned long  ulong;

#define NOACTIVITY 600
#define RXSIZE 512
//
//
// Serial Protocol Parser
//
#define SINGLEBYTEMASK	0b11000000
#define SINGLEBYTECMD	0b01000000
#define MULTIBYTEMASK	0b11111000
#define PSETINCDECMASK	0b11110000
#define PSETINCDECCMD	0b11000000

#define	STANDBY		0x00
//Single-Byte commands
#define	CLEAR_SCR	0x43	//0b01000011		uses the address field of PSETINCDECCMD
#define	CLOCK_ON	0x7E	//0b01111110
#define	CLOCK_OFF	0x7F	//0b01111111

//Multi-Byte commands
#define	COLOR		0x88
#define	SET			0x90		//SetX, SetY, SetX+Pset, SetY+Pset
#define	PUT_CHAR	0xA0
#define	DRAW_LINE	0xA8
#define	DRAW_CIRCLE	0xC8
#define	PRINT_STRING 0xB0
#define	PICTURE		0xC0
#define	SET_SCREEN	0x98
#define	PICTURE_DATA 0xE0
#define	DEMO		0xB8		// Takes one argument to select demo

#define FALSE	0
#define TRUE	1

#define LCDERROR0	0
#define LCDERROR1	1
#define LCDERROR2	2
#define LCDERROR3	3
#define LCDERROR4	4
#define LCDERROR5	5
#define LCDERROR6	6
#define LCDERROR7	7


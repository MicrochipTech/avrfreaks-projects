//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvPS2Key_out.h
//
// Description:
//	Driver definition for PS2 Keyboard output
//--------------------------------------------------------------------------- 

#ifndef __DRV_PS2KEY_OUT_H_
#define __DRV_PS2KEY_OUT_H_

#include "types.h"
#include "driver.h"

//---------------------------------------------------------------------------
// Scan code definitions
//---------------------------------------------------------------------------
#define CODE_PACK(x,y)	(((USHORT)x) << 8) | (USHORT)y)
//---------------------------------------------------------------------------
#define EXTEND_PREFIX	(0x00E0)
#define BREAK_PREFIX	(0x00F0)
//---------------------------------------------------------------------------
#define PS2_A			(0x001C)
#define PS2_B			(0x0032)
#define PS2_C			(0x0021)
#define PS2_D			(0x0023)
#define PS2_E			(0x0024)
#define PS2_F			(0x002B)
#define PS2_G			(0x0034)
#define PS2_H			(0x0033)
#define PS2_I			(0x0043)
#define PS2_J			(0x003B)
#define PS2_K			(0x0042)
#define PS2_L			(0x004B)
#define PS2_M			(0x003A)
#define PS2_N			(0x0031)
#define PS2_O			(0x0044)
#define PS2_P			(0x004D)
#define PS2_Q			(0x0015)
#define PS2_R			(0x002D)
#define PS2_S			(0x001B)
#define PS2_T			(0x002C)
#define PS2_U			(0x003C)
#define PS2_V			(0x002A)
#define PS2_W			(0x001D)
#define PS2_X			(0x0022)
#define PS2_Y			(0x0035)
#define PS2_Z			(0x001A)
#define PS2_0			(0x0045)
#define PS2_1			(0x0016)
#define PS2_2			(0x001E)
#define PS2_3			(0x0026)
#define PS2_4			(0x0025)
#define PS2_5			(0x002E)
#define PS2_6			(0x0036)
#define PS2_7			(0x003D)
#define PS2_8			(0x003E)
#define PS2_9			(0x0046)
#define PS2_APOSTRAPHE	(0x000E)
#define PS2_MINUS		(0x004E)
#define PS2_EQUALS		(0x0055)
#define PS2_BACKSLACH	(0x005D)
#define PS2_BKSP		(0x0066)
#define PS2_SPACE		(0x0029)
#define PS2_TAB			(0x000D)
#define PS2_CAPS		(0x0058)
#define PS2_LSHIFT		(0x0012)
#define PS2_LCTRL		(0x0014)
#define PS2_LWIN		CODE_PACK(EXTEND_PREFIX, 0x001F)
#define PS2_LALT		(0x0011)
#define PS2_RSHIFT		(0x0059)
#define PS2_RCTRL		CODE_PACK(EXTEND_PREFIX, 0x0014)
#define PS2_RWIN		CODE_PACK(EXTEND_PREFIX, 0x0027)
#define PS2_RALT		CODE_PACK(EXTEND_PREFIX, 0x0011)
#define PS2_ENTER		(0x005A)
#define PS2_ESC			(0x0076)
#define PS2_F1			(0x0005)
#define PS2_F2			(0x0006)
#define PS2_F3			(0x0004)
#define PS2_F4			(0x000c)
#define PS2_F5			(0x0003)
#define PS2_F6			(0x000B)
#define PS2_F7			(0x0083)
#define PS2_F8			(0x000A)
#define PS2_F9			(0x0001)
#define PS2_F10			(0x0009)
#define PS2_F11			(0x0078)
#define PS2_F12			(0x0007)
#define PS2_SCROLL		(0x007E)
#define PS2_LBRACKET	(0x0054)
#define PS2_INSERT		CODE_PACK(EXTEND_PREFIX, 0x0070)
#define PS2_HOME		CODE_PACK(EXTEND_PREFIX, 0x006C)
#define PS2_PGUP		CODE_PACK(EXTEND_PREFIX, 0x007D)
#define PS2_DELETE		CODE_PACK(EXTEND_PREFIX, 0x0071)
#define PS2_END			CODE_PACK(EXTEND_PREFIX, 0x0069)
#define PS2_PGDN		CODE_PACK(EXTEND_PREFIX, 0x007A)
#define PS2_UP			CODE_PACK(EXTEND_PREFIX, 0x0075)
#define PS2_LEFT		CODE_PACK(EXTEND_PREFIX, 0x006B)
#define PS2_DOWN		CODE_PACK(EXTEND_PREFIX, 0x0072)
#define PS2_RIGHT		CODE_PACK(EXTEND_PREFIX, 0x0074)
#define PS2_NUM			(0x0077)
#define PS2_PAD_SLASH	CODE_PACK(EXTEND_PREFIX, 0x004A)
#define PS2_PAD_STAR	(0x007C)
#define PS2_PAD_MINUS	(0x007B)
#define PS2_PAD_PLUS	(0x0079)
#define PS2_PAD_EN		CODE_PACK(EXTEND_PREFIX, 0x005A)
#define PS2_PAD_DOT		(0x0071)
#define PS2_PAD_0		(0x0070)
#define PS2_PAD_1		(0x0069)
#define PS2_PAD_2		(0x0072)
#define PS2_PAD_3		(0x007A)
#define PS2_PAD_4		(0x006B)
#define PS2_PAD_5		(0x0073)
#define PS2_PAD_6		(0x0074)
#define PS2_PAD_7		(0x006C)
#define PS2_PAD_8		(0x0075)
#define PS2_PAD_9		(0x007D)
#define PS2_RBRACKET	(0x005B)
#define PS2_SEMICOLON	(0x004C)
#define PS2_QUOTE		(0x0041)
#define PS2_DOT			(0x0049)
#define PS2_SLASH		(0x004A)

//---------------------------------------------------------------------------
typedef enum
{
	PS2_OUT_BIT_START,
	PS2_OUT_BIT_0,
	PS2_OUT_BIT_1,
	PS2_OUT_BIT_2,
	PS2_OUT_BIT_3,
	PS2_OUT_BIT_4,
	PS2_OUT_BIT_5,
	PS2_OUT_BIT_6,
	PS2_OUT_BIT_7,
	PS2_OUT_BIT_PARITY,
	PS2_OUT_BIT_STOP,
	PS2_OUT_BIT_COUNT
} PS2_OUT_BIT_STATE;

//---------------------------------------------------------------------------
typedef enum
{
	PS2KEY_OUT_EVENT_WRITE = DRIVER_EVENTS,
	PS2KEY_OUT_EVENT_READ
} PS2KEY_OUT_EVENT;

//---------------------------------------------------------------------------
// Command set defines
//---------------------------------------------------------------------------
typedef struct
{
	//-- Must Be First ------------------------------------------------------
	DRIVER_STRUCT stDriver;
	
	//-- Type-specific data -------------------------------------------------
	volatile UCHAR *pucClkDir;
	volatile UCHAR *pucClkPort;
	volatile UCHAR *pucClkIn;
		
	volatile UCHAR *pucDataDir;
	volatile UCHAR *pucDataPort;	
	volatile UCHAR *pucDataIn;
	
	//-- Signal bit variables -----------------------------------------------
	UCHAR ucClkBit;
	UCHAR ucDataBit;
	
	//-- Single buffer used if a scan is interrupted by the host ------------
	UCHAR ucBufChar;
	UCHAR ucBufCode;	// i.e. "break" code
	
} PS2KEY_OUT_STRUCT;
//---------------------------------------------------------------------------
void PS2KeyOut_Init(DRIVER_STRUCT *pstThis_);
void PS2KeyOut_Start(DRIVER_STRUCT *pstThis_);
void PS2KeyOut_Stop(DRIVER_STRUCT *pstThis_);
void PS2KeyOut_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_);

#endif 


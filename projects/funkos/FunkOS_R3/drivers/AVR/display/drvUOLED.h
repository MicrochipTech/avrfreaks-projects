//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvUOLED.h
//
// Description:
//	Display driver for 4D Systems uOLED displays
//--------------------------------------------------------------------------- 

#ifndef __DRVUOLED_H__
#define __DRVUOLED_H__

#include "kernelcfg.h"
#include "types.h"
#include "driver.h"

//---------------------------------------------------------------------------
// Baudrate generator values
//---------------------------------------------------------------------------
#define BAUDRATE 224000
#define UBRRVAL ((SYSTEM_FREQ/16)/BAUDRATE)-1

//---------------------------------------------------------------------------
// Port configuration for reset line - user-configurable
//---------------------------------------------------------------------------
#define UOLED_RESET_PORT		(PORTB)
#define UOLED_RESET_BIT			(0x02)
#define UOLED_RESET_DDR			(DDRB)

//---------------------------------------------------------------------------
// UART defines - user-configurable for different targets
//---------------------------------------------------------------------------
#define UART_SRA				(UCSR0A)
#define UART_SRB				(UCSR0B)
#define UART_SRC				(UCSR0C)
#define UART_BAUDH				(UBRR0H)
#define UART_BAUDL				(UBRR0L)
#define UART_RXEN				(RXEN0)
#define UART_TXEN				(TXEN0)
#define UART_TXCIE				(TXCIE0)
#define UART_RXCIE				(RXCIE0)
#define UART_UDR				(UDR0)
#define UART_UDRE				(UDRE0)

#define UART_RX_ISR				(USART_RX_vect)
#define UART_TX_ISR				(USART_TX_vect)
//---------------------------------------------------------------------------

typedef struct
{
	//-- Must Be First ------------------------------------------------------
	DRIVER_STRUCT stDriver;
	
	//--- No driver-specific data for this ----------------------------------
} UOLED_DRIVER_STRUCT;

//---------------------------------------------------------------------------
typedef enum
{
	CMD_ADD_USER_BITMAP,
	CMD_SET_BG_COLOR,
	CMD_PLACE_TEXT_BUTTON,
	CMD_DRAW_CIRCLE,
	CMD_COPY_PASTE,
	CMD_DISPLAY_USER_BITMAP,
	CMD_ERASE_SCREEN,
	CMD_FONT_SIZE,
	CMD_DRAW_TRIANGLE,
	CMD_DRAW_POLYGON,
	CMD_DISPLAY_IMAGE,
	CMD_DRAW_LINE,
	CMD_OPAQUE_TRANSPARENT_TEXT,
	CMD_PUT_PIXEL,
	CMD_SET_PEN_SIZE,
	CMD_READ_PIXEL,
	CMD_DRAW_RECTANGLE,
	CMD_PLACE_UNFORMATTED_TEXT,
	CMD_PLACE_FORMATTED_TEXT,
	CMD_PLACE_FORMATTED_CHAR,
	CMD_PLACE_UNFORMATTED_CHAR,
	CMD_DEVICE_INFO_REQUEST,
	CMD_SPECIAL_COMMAND
} CMD_UOLED;

//---------------------------------------------------------------------------
#define ID_ADD_USER_BITMAP			(0x41)
#define ID_SET_BG_COLOR				(0x42)
#define ID_PLACE_TEXT_BUTTON		(0x62)
#define ID_DRAW_CIRCLE				(0x43)
#define ID_COPY_PASTE				(0x63)
#define ID_DISPLAY_USER_BITMAP		(0x44)
#define ID_ERASE_SCREEN				(0x45)
#define ID_FONT_SIZE				(0x46)
#define ID_DRAW_TRIANGLE			(0x47)
#define ID_DRAW_POLYGON				(0x67)
#define ID_DISPLAY_IMAGE			(0x49)
#define ID_DRAW_LINE				(0x4C)
#define ID_OPAQUE_TRANSPARENT_TEXT	(0x4F)
#define ID_PUT_PIXEL				(0x50)
#define ID_SET_PEN_SIZE				(0x70)
#define ID_READ_PIXEL				(0x52)
#define ID_DRAW_RECTANGLE			(0x72)
#define ID_PLACE_UNFORMATTED_TEXT	(0x53)
#define ID_PLACE_FORMATTED_TEXT		(0x73)
#define ID_PLACE_FORMATTED_CHAR		(0x54)
#define ID_PLACE_UNFORMATTED_CHAR	(0x74)
#define ID_DEVICE_INFO_REQUEST		(0x56)
#define ID_SPECIAL_COMMAND			(0x59)

//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucCharID;			// 32 sprites, ID = 0-31
	UCHAR *pucData;			// 8x8 sprites
}UOLED_ADD_USER_BITMAP;
//---------------------------------------------------------------------------
typedef struct
{
	USHORT usColor;			// 5:6:5 (R:G:B format)
}UOLED_SET_BG_COLOR;		
//---------------------------------------------------------------------------
typedef struct
{
	BOOL bUp;
	UCHAR ucLeft;
	UCHAR ucTop;
	USHORT usColor;
	UCHAR ucFont;		// 0: 5x7, 1: 8x8, 2:8x12
	UCHAR ucWidth;
	UCHAR ucHeight;
	UCHAR *pucString;   // String pointer
}UOLED_PLACE_TEXT_BUTTON;
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucCenterX;
	UCHAR ucCenterY;
	UCHAR ucRadius;
	USHORT usColor;
}UOLED_DRAW_CIRCLE;	
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucSrcLeft;
	UCHAR ucSrcTop;
	UCHAR ucDstLeft;
	UCHAR ucDstTop;
	UCHAR ucWidth;
	UCHAR ucHeight;
}UOLED_COPY_PASTE;
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucCharID;
	UCHAR ucLeft;
	UCHAR ucTop;
	USHORT usColor;
}UOLED_DISPLAY_USER_BITMAP;
//---------------------------------------------------------------------------
//typedef struct
//{
//	/* NULL STRUCT */
//}UOLED_ERASE_SCREEN;
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucFontSize;
}UOLED_FONT_SIZE;
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucX1;
	UCHAR ucY1;
	UCHAR ucX2;
	UCHAR ucY2;
	UCHAR ucX3;
	UCHAR ucY3;
	USHORT usColor;
}UOLED_DRAW_TRIANGLE;
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucVertices;
	UCHAR aucX[7];
	UCHAR aucY[7];
	USHORT usColor;
}UOLED_DRAW_POLYGON;
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucLeft;
	UCHAR ucTop;
	UCHAR ucWidth;
	UCHAR ucHeight;
	UCHAR ucColorMode; //8 = 8bit, 16 = 16bit
	UCHAR *pucPixels;  // Width x Height pixels
}UOLED_DISPLAY_IMAGE;
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucX1;
	UCHAR ucY1;
	UCHAR ucX2;
	UCHAR ucY2;
	USHORT usColor;
}UOLED_DRAW_LINE;
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucMode;	// 0 = transparent, 1 = opaque
}UOLED_OPAQUE_TRANSPARENT_TEXT;
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucX;
	UCHAR ucY;
	USHORT usColor;
}UOLED_PUT_PIXEL;			
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucSize; //0 = solid, 1 = wireframe
}UOLED_SET_PEN_SIZE;
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucX;
	UCHAR ucY;
	USHORT usPixel;
}UOLED_READ_PIXEL;		
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucLeft;
	UCHAR ucTop;
	UCHAR ucRight;
	UCHAR ucBottom;
	USHORT usColor;
}UOLED_DRAW_RECTANGLE;
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucLeft;
	UCHAR ucTop;
	UCHAR ucFont;
	USHORT usColor;
	UCHAR ucWidth;
	UCHAR ucHeight;
	UCHAR *pucString;
}UOLED_PLACE_UNFORMATTED_TEXT;
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucColumn; // 0-20 for 5x7; 0-15 for 8x8 & 8x12
	UCHAR ucRow;	// 0-15 for 5x7 & 8x8; 0-9 for 8x12
	UCHAR ucFont;
	USHORT usColor;
	UCHAR *pucString;
}UOLED_PLACE_FORMATTED_TEXT;
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucChar;
	UCHAR ucColumn;
	UCHAR ucRow;
	USHORT usColor;
}UOLED_PLACE_FORMATTED_CHAR;
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucChar;
	UCHAR ucLeft;
	UCHAR ucTop;
	USHORT usColor;
	UCHAR ucScaleX;
	UCHAR ucScaleY;
}UOLED_PLACE_UNFORMATTED_CHAR;
//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucOutput;		// 0 for serial, 1 for screen
	UCHAR ucDevID;
	UCHAR ucHW_Rev;
	UCHAR ucFW_Rev;
	UCHAR ucResX;
	UCHAR ucResY;
}UOLED_DEVICE_INFO_REQUEST;

//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucCommand;		// 1 = Display on/off, 2 = OLED Contrast, 3 = OLED Power-up/down
	UCHAR ucData;		    // 0 = off, 1 = on     0-15 			  0 = powerdown 1 = poweruf
}UOLED_SPECIAL_COMMAND;		

//---------------------------------------------------------------------------
void UOLED_Init(DRIVER_STRUCT *pstDriver_);
void UOLED_Start(DRIVER_STRUCT *pstDriver_);
void UOLED_Stop(DRIVER_STRUCT *pstDriver_);
void UOLED_Control(DRIVER_STRUCT *pstDriver_, USHORT usID_, void *pvData_);

#endif

//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvUOLED.c
//
// Description:
//	Display driver for 4D Systems uOLED displays
//--------------------------------------------------------------------------- 

#include "types.h"
#include "kernelcfg.h"
#include "driver.h"
#include "drvUOLED.h"

#include <avr\io.h>
#include <avr\interrupt.h>

//---------------------------------------------------------------------------
static void UOLED_AddUserBitmap(UOLED_ADD_USER_BITMAP *pstData_);
static void UOLED_SetBGColor(UOLED_SET_BG_COLOR *pstData_);
static void UOLED_PlaceTextButton(UOLED_PLACE_TEXT_BUTTON *pstData_);
static void UOLED_DrawCircle(UOLED_DRAW_CIRCLE *pstData_);
static void UOLED_CopyPaste(UOLED_COPY_PASTE *pstData_);
static void UOLED_DisplayUserBitmap(UOLED_DISPLAY_USER_BITMAP *pstData_);
static void UOLED_EraseScreen(void);
static void UOLED_FontSize(UOLED_FONT_SIZE *pstData_);
static void UOLED_DrawTriangle(UOLED_DRAW_TRIANGLE *pstData_);
static void UOLED_DrawPolygon(UOLED_DRAW_POLYGON *pstData_);
static void UOLED_DisplayImage(UOLED_DISPLAY_IMAGE *pstData_);
static void UOLED_DrawLine(UOLED_DRAW_LINE *pstData_);
static void UOLED_OpaqueTransparentText(UOLED_OPAQUE_TRANSPARENT_TEXT *pstData_);
static void UOLED_PutPixel(UOLED_PUT_PIXEL *pstData_);
static void UOLED_DrawRectangle(UOLED_DRAW_RECTANGLE *pstData_);
static void UOLED_SetPenSize(UOLED_SET_PEN_SIZE *pstData_);
static void UOLED_ReadPixel(UOLED_READ_PIXEL *pstData_);
static void UOLED_PlaceUnformattedText(UOLED_PLACE_UNFORMATTED_TEXT *pstData_);
static void UOLED_PlaceFormattedText(UOLED_PLACE_FORMATTED_TEXT *pstData_);
static void UOLED_PlaceFormattedChar(UOLED_PLACE_FORMATTED_CHAR *pstData_);
static void UOLED_PlaceUnformattedChar(UOLED_PLACE_UNFORMATTED_CHAR *pstData_);
static void UOLED_DeviceInfoRequest(UOLED_DEVICE_INFO_REQUEST *pstData_);
static void UOLED_SpecialCommand(UOLED_SPECIAL_COMMAND *pstData_);

//---------------------------------------------------------------------------
static void UOLED_WriteByte(UCHAR ucByte_);
static void UOLED_WriteUSHORT(USHORT usData_);
static UCHAR UOLED_ReadByte(void);

//---------------------------------------------------------------------------
// Acknowledge value needs its own
volatile UCHAR ucAckVal = 0;

//---------------------------------------------------------------------------
void UOLED_Init(DRIVER_STRUCT *pstDriver_)
{
	volatile ULONG ulSpin = 0;
		
	// Set up the hardware UART:
	// Initialize the UART to 256kbaud
	UART_BAUDH = UBRRVAL >> 8;
	UART_BAUDL = UBRRVAL;
	
	// Clear flags
	UART_SRA = 0;

	// Enable receiver and transmitter 
	UART_SRB = (1 << UART_RXEN) | ( 1 << UART_TXEN);
	UART_SRB |= (1 << UART_TXCIE) | (1 << UART_RXCIE);
	// Set frame format: 8 N 1
	UART_SRC = 0x06;
	
	// Reset the board by toggling pin portb1
	UOLED_RESET_DDR |= UOLED_RESET_BIT;
	UOLED_RESET_PORT &= ~UOLED_RESET_BIT;
	for (ulSpin = 0; ulSpin < 150; ulSpin++){ /* Do Nothing */}
	UOLED_RESET_PORT |= UOLED_RESET_BIT;

	// 1 second delay	
	for (ulSpin = 0; ulSpin < 750000; ulSpin++){/* DO NOTHING */}
	
	ucAckVal = 0;

	// Write "U" out to port
	UOLED_WriteByte('U');
	
	// Wait for Ack (0x06)
	UOLED_ReadByte();
}
//---------------------------------------------------------------------------
void UOLED_Start(DRIVER_STRUCT *pstDriver_)
{
	// Write device poweron command
}
//---------------------------------------------------------------------------
void UOLED_Stop(DRIVER_STRUCT *pstDriver_)
{
	// Write device shutdown
}
//---------------------------------------------------------------------------
void UOLED_Control(DRIVER_STRUCT *pstDriver_, USHORT usID_, void *pvData_)
{
	BOOL bWaitForAck = TRUE;
	BOOL bRetry = TRUE;
	
	ucAckVal = 0;

	while (bRetry == TRUE)
	{
		// State machine for handling 
		switch ((CMD_UOLED)usID_)
		{
			case CMD_ADD_USER_BITMAP:
				UOLED_AddUserBitmap((UOLED_ADD_USER_BITMAP*)pvData_);
				break;
			case CMD_SET_BG_COLOR:
				UOLED_SetBGColor((UOLED_SET_BG_COLOR*)pvData_);
				break;
			case CMD_PLACE_TEXT_BUTTON:
				UOLED_PlaceTextButton((UOLED_PLACE_TEXT_BUTTON*)pvData_);
				break;
			case CMD_DRAW_CIRCLE:
				UOLED_DrawCircle((UOLED_DRAW_CIRCLE*)pvData_);
				break;
			case CMD_COPY_PASTE:
				UOLED_CopyPaste((UOLED_COPY_PASTE*)pvData_);
				break;
			case CMD_DISPLAY_USER_BITMAP:
				UOLED_DisplayUserBitmap((UOLED_DISPLAY_USER_BITMAP*)pvData_);
				break;
			case CMD_ERASE_SCREEN:
				UOLED_EraseScreen();
				break;
			case CMD_FONT_SIZE:
				UOLED_FontSize((UOLED_FONT_SIZE*)pvData_);
				break;
			case CMD_DRAW_TRIANGLE:
				UOLED_DrawTriangle((UOLED_DRAW_TRIANGLE*)pvData_);
				break;
			case CMD_DRAW_POLYGON:
				UOLED_DrawPolygon((UOLED_DRAW_POLYGON*)pvData_);
				break;
			case CMD_DISPLAY_IMAGE:
				UOLED_DisplayImage((UOLED_DISPLAY_IMAGE*)pvData_);
				break;
			case CMD_DRAW_LINE:
				UOLED_DrawLine((UOLED_DRAW_LINE*)pvData_);
				break;
			case CMD_OPAQUE_TRANSPARENT_TEXT:
				UOLED_OpaqueTransparentText((UOLED_OPAQUE_TRANSPARENT_TEXT*)pvData_);
				break;
			case CMD_PUT_PIXEL:
				UOLED_PutPixel((UOLED_PUT_PIXEL*)pvData_);
				break;
			case CMD_SET_PEN_SIZE:
				UOLED_SetPenSize((UOLED_SET_PEN_SIZE*)pvData_);
				break;
			case CMD_READ_PIXEL:
				UOLED_ReadPixel((UOLED_READ_PIXEL*)pvData_);
				break;
			case CMD_DRAW_RECTANGLE:
				UOLED_DrawRectangle((UOLED_DRAW_RECTANGLE*)pvData_);
				break;
			case CMD_PLACE_UNFORMATTED_TEXT:
				UOLED_PlaceUnformattedText((UOLED_PLACE_UNFORMATTED_TEXT*)pvData_);
				break;
			case CMD_PLACE_FORMATTED_TEXT:
				UOLED_PlaceFormattedText((UOLED_PLACE_FORMATTED_TEXT*)pvData_);
				break;
			case CMD_PLACE_FORMATTED_CHAR:
				UOLED_PlaceFormattedChar((UOLED_PLACE_FORMATTED_CHAR*)pvData_);
				break;
			case CMD_PLACE_UNFORMATTED_CHAR:
				UOLED_PlaceUnformattedChar((UOLED_PLACE_UNFORMATTED_CHAR*)pvData_);
				break;
			case CMD_DEVICE_INFO_REQUEST:
				UOLED_DeviceInfoRequest((UOLED_DEVICE_INFO_REQUEST*)pvData_);
				bWaitForAck = FALSE;			
				break;			
			case CMD_SPECIAL_COMMAND: 
				UOLED_SpecialCommand((UOLED_SPECIAL_COMMAND*)pvData_);
				break;
			default:
				bWaitForAck = FALSE;
				break;
		}
		
		if (bWaitForAck == TRUE)
		{
			if (UOLED_ReadByte() == 0x06)
			{
				bRetry = FALSE;
			}
			else
			{
				bRetry = TRUE;
			}
		}
	}
}
//---------------------------------------------------------------------------
static void UOLED_AddUserBitmap(UOLED_ADD_USER_BITMAP *pstData_)
{
	UCHAR i;
	UOLED_WriteByte(ID_ADD_USER_BITMAP);
	UOLED_WriteByte(pstData_->ucCharID);
	for (i = 0; i < 8; i++)
	{
		UOLED_WriteByte(pstData_->pucData[i]);
	}
}
//---------------------------------------------------------------------------
static void UOLED_SetBGColor(UOLED_SET_BG_COLOR *pstData_)
{
	UOLED_WriteByte(ID_SET_BG_COLOR);
	UOLED_WriteUSHORT(pstData_->usColor);
}
//---------------------------------------------------------------------------
static void UOLED_PlaceTextButton(UOLED_PLACE_TEXT_BUTTON *pstData_)
{
	UCHAR i = 0;
	
	UOLED_WriteByte(ID_PLACE_TEXT_BUTTON);
	UOLED_WriteByte(pstData_->bUp);
	UOLED_WriteByte(pstData_->ucLeft);
	UOLED_WriteByte(pstData_->ucTop);
	UOLED_WriteUSHORT(pstData_->usColor);
	UOLED_WriteByte(pstData_->ucFont);
	UOLED_WriteByte(pstData_->ucWidth);
	UOLED_WriteByte(pstData_->ucHeight);
	
	while(pstData_->pucString[i] != 0)
	{
		UOLED_WriteByte(pstData_->pucString[i]);   
		i++;
	}
	
	UOLED_WriteByte(0);   
}
//---------------------------------------------------------------------------
static void UOLED_DrawCircle(UOLED_DRAW_CIRCLE *pstData_)
{
	UOLED_WriteByte(ID_DRAW_CIRCLE);
	
	UOLED_WriteByte(pstData_->ucCenterX);
	UOLED_WriteByte(pstData_->ucCenterY);
	UOLED_WriteByte(pstData_->ucRadius);
	UOLED_WriteUSHORT(pstData_->usColor);
}
//---------------------------------------------------------------------------
static void UOLED_CopyPaste(UOLED_COPY_PASTE *pstData_)
{
	UOLED_WriteByte(ID_COPY_PASTE);
	
	UOLED_WriteByte(pstData_->ucSrcLeft);
	UOLED_WriteByte(pstData_->ucSrcTop);
	UOLED_WriteByte(pstData_->ucDstLeft);
	UOLED_WriteByte(pstData_->ucDstTop);
	UOLED_WriteByte(pstData_->ucWidth);
	UOLED_WriteByte(pstData_->ucHeight);
}
//---------------------------------------------------------------------------
static void UOLED_DisplayUserBitmap(UOLED_DISPLAY_USER_BITMAP *pstData_)
{
	UOLED_WriteByte(ID_DISPLAY_USER_BITMAP);
	
	UOLED_WriteByte(pstData_->ucCharID);
	UOLED_WriteByte(pstData_->ucLeft);
	UOLED_WriteByte(pstData_->ucTop);
	UOLED_WriteUSHORT(pstData_->usColor);
}
//---------------------------------------------------------------------------
static void UOLED_EraseScreen(void)
{
	UOLED_WriteByte(ID_ERASE_SCREEN);
}
//---------------------------------------------------------------------------
static void UOLED_FontSize(UOLED_FONT_SIZE *pstData_)
{
	UOLED_WriteByte(ID_FONT_SIZE);
	
	UOLED_WriteByte(pstData_->ucFontSize);
}
//---------------------------------------------------------------------------
static void UOLED_DrawTriangle(UOLED_DRAW_TRIANGLE *pstData_)
{
	UOLED_WriteByte(ID_DRAW_TRIANGLE);
	
	UOLED_WriteByte(pstData_->ucX1);
	UOLED_WriteByte(pstData_->ucY1);
	UOLED_WriteByte(pstData_->ucX2);
	UOLED_WriteByte(pstData_->ucY2);
	UOLED_WriteByte(pstData_->ucX3);
	UOLED_WriteByte(pstData_->ucY3);
	
	UOLED_WriteUSHORT(pstData_->usColor);
}
//---------------------------------------------------------------------------
static void UOLED_DrawPolygon(UOLED_DRAW_POLYGON *pstData_)
{
	UCHAR i;

	UOLED_WriteByte(ID_DRAW_POLYGON);
	
	UOLED_WriteByte(pstData_->ucVertices);
	for (i = 0; i < pstData_->ucVertices; i++)
	{
		UOLED_WriteByte(pstData_->aucX[i]);
		UOLED_WriteByte(pstData_->aucY[i]);
	}
	
	UOLED_WriteUSHORT(pstData_->usColor);
}
//---------------------------------------------------------------------------
static void UOLED_DisplayImage(UOLED_DISPLAY_IMAGE *pstData_)
{
	USHORT i;
	USHORT usNumBytes;
	
	UOLED_WriteByte(ID_DISPLAY_IMAGE);
	
	UOLED_WriteByte(pstData_->ucLeft);
	UOLED_WriteByte(pstData_->ucTop);
	UOLED_WriteByte(pstData_->ucWidth);
	UOLED_WriteByte(pstData_->ucHeight);
	UOLED_WriteByte(pstData_->ucColorMode);
	
	usNumBytes = (USHORT)pstData_->ucWidth * (USHORT)pstData_->ucHeight;

	// If it's two-bytes per pixel, multiply size by 2.
	if (pstData_->ucColorMode == 16)
	{
		usNumBytes *= 2; 
	}

	// Write out the image bytes
	for (i = 0; i < usNumBytes; i++)
	{
		UOLED_WriteByte(pstData_->pucPixels[i]);
	}
}
//---------------------------------------------------------------------------
static void UOLED_DrawLine(UOLED_DRAW_LINE *pstData_)
{
	UOLED_WriteByte(ID_DRAW_LINE);
	
	UOLED_WriteByte(pstData_->ucX1);
	UOLED_WriteByte(pstData_->ucY1);
	UOLED_WriteByte(pstData_->ucX2);
	UOLED_WriteByte(pstData_->ucY2);
	UOLED_WriteUSHORT(pstData_->usColor);
}
//---------------------------------------------------------------------------
static void UOLED_OpaqueTransparentText(UOLED_OPAQUE_TRANSPARENT_TEXT *pstData_)
{
	UOLED_WriteByte(ID_OPAQUE_TRANSPARENT_TEXT);
	
	UOLED_WriteByte(pstData_->ucMode);
}
//---------------------------------------------------------------------------
static void UOLED_PutPixel(UOLED_PUT_PIXEL *pstData_)
{
	UOLED_WriteByte(ID_PUT_PIXEL);
	
	UOLED_WriteByte(pstData_->ucX);
	UOLED_WriteByte(pstData_->ucY);
	UOLED_WriteUSHORT(pstData_->usColor);
}
//---------------------------------------------------------------------------
static void UOLED_SetPenSize(UOLED_SET_PEN_SIZE *pstData_)
{
	UOLED_WriteByte(ID_SET_PEN_SIZE);
	UOLED_WriteByte(pstData_->ucSize);
}
//---------------------------------------------------------------------------
static void UOLED_ReadPixel(UOLED_READ_PIXEL *pstData_)
{
	USHORT usPixel;
	
	UOLED_WriteByte(ID_READ_PIXEL);
	
	UOLED_WriteByte(pstData_->ucX);
	UOLED_WriteByte(pstData_->ucY);
	
	usPixel = UOLED_ReadByte();
	usPixel <<= 8;
	usPixel |= UOLED_ReadByte();	
}
//---------------------------------------------------------------------------
static void UOLED_DrawRectangle(UOLED_DRAW_RECTANGLE *pstData_)
{
	UOLED_WriteByte(ID_DRAW_RECTANGLE);
	
	UOLED_WriteByte(pstData_->ucLeft);
	UOLED_WriteByte(pstData_->ucTop);
	UOLED_WriteByte(pstData_->ucRight);
	UOLED_WriteByte(pstData_->ucBottom);
	UOLED_WriteUSHORT(pstData_->usColor);
}
//---------------------------------------------------------------------------
static void UOLED_PlaceUnformattedText(UOLED_PLACE_UNFORMATTED_TEXT *pstData_)
{
	UCHAR i = 0;
	
	UOLED_WriteByte(ID_PLACE_UNFORMATTED_TEXT);
	
	UOLED_WriteByte(pstData_->ucLeft);
	UOLED_WriteByte(pstData_->ucTop);
	UOLED_WriteByte(pstData_->ucFont);
	
	UOLED_WriteUSHORT(pstData_->usColor);
	
	UOLED_WriteByte(pstData_->ucWidth);
	UOLED_WriteByte(pstData_->ucHeight);
	
	while (pstData_->pucString[i] != 0)
	{
		UOLED_WriteByte(pstData_->pucString[i]);
		i++;
	}
	
	UOLED_WriteByte(0);
}
//---------------------------------------------------------------------------
static void UOLED_PlaceFormattedText(UOLED_PLACE_FORMATTED_TEXT *pstData_)
{
	UCHAR i = 0;
	
	UOLED_WriteByte(ID_PLACE_FORMATTED_TEXT);
	
	UOLED_WriteByte(pstData_->ucColumn);
	UOLED_WriteByte(pstData_->ucRow);
	UOLED_WriteByte(pstData_->ucFont);
	UOLED_WriteUSHORT(pstData_->usColor);
	
	while (pstData_->pucString[i] != 0)
	{
		UOLED_WriteByte(pstData_->pucString[i]);
		i++;
	}
	
	UOLED_WriteByte(0);
}
//---------------------------------------------------------------------------
static void UOLED_PlaceFormattedChar(UOLED_PLACE_FORMATTED_CHAR *pstData_)
{
	UOLED_WriteByte(ID_PLACE_FORMATTED_CHAR);
	
	UOLED_WriteByte(pstData_->ucChar);
	UOLED_WriteByte(pstData_->ucColumn);
	UOLED_WriteByte(pstData_->ucRow);
	UOLED_WriteUSHORT(pstData_->usColor);
}
//---------------------------------------------------------------------------
static void UOLED_PlaceUnformattedChar(UOLED_PLACE_UNFORMATTED_CHAR *pstData_)
{
	UOLED_WriteByte(ID_PLACE_UNFORMATTED_CHAR);
	
	UOLED_WriteByte(pstData_->ucChar);
	UOLED_WriteByte(pstData_->ucLeft);
	UOLED_WriteByte(pstData_->ucTop);
	UOLED_WriteUSHORT(pstData_->usColor);
	UOLED_WriteByte(pstData_->ucScaleX);
	UOLED_WriteByte(pstData_->ucScaleY);
}
//---------------------------------------------------------------------------
static void UOLED_DeviceInfoRequest(UOLED_DEVICE_INFO_REQUEST *pstData_)
{
	UOLED_WriteByte(ID_DEVICE_INFO_REQUEST);
	
	UOLED_WriteByte(pstData_->ucOutput);
	UOLED_WriteByte(pstData_->ucDevID);
	UOLED_WriteByte(pstData_->ucHW_Rev);
	UOLED_WriteByte(pstData_->ucFW_Rev);
	UOLED_WriteByte(pstData_->ucResX);
	UOLED_WriteByte(pstData_->ucResY);
}

//---------------------------------------------------------------------------
static void UOLED_SpecialCommand(UOLED_SPECIAL_COMMAND *pstData_)
{
	UOLED_WriteByte(ID_SPECIAL_COMMAND);
	
	UOLED_WriteByte(pstData_->ucCommand);
	UOLED_WriteByte(pstData_->ucData);
}

//===========================================================================
static void UOLED_WriteUSHORT(USHORT usData_)
{
	UCHAR ucOut;
	// Send MSB first...
	ucOut = (UCHAR)((usData_ & 0xFF00) >> 8);
	UOLED_WriteByte(ucOut);
	// Send LSB next...
	ucOut = (UCHAR)(usData_ & 0x00FF);
	UOLED_WriteByte(ucOut);
}
//---------------------------------------------------------------------------
static void UOLED_WriteByte(UCHAR ucByte_)
{
	volatile UCHAR ucTemp =0;	
	// Wait until the TX buffer is empty
	while (!( UART_SRA & (1<<UART_UDRE) )){/* DO NOTHING */}
	for (ucTemp =0; ucTemp < 2; ucTemp++)
	{
		ucTemp = ucTemp;
	}
	// Send the byte
	UDR0 = ucByte_;	
}
//---------------------------------------------------------------------------
static UCHAR UOLED_ReadByte(void)
{	
	// Wait for data to be received
	while (ucAckVal == 0) {};
	
	return ucAckVal;
}
//---------------------------------------------------------------------------
ISR(UART_RX_ISR)
{
	ucAckVal = UART_UDR;
}
//---------------------------------------------------------------------------
ISR(UART_TX_ISR)
{
	// Stub
}





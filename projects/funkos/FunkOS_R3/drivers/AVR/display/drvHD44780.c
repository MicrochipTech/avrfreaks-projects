//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvHD44780.c
//
// Description:
//	Driver for HD44780-compatible LCD displays.  
//
// Usage:
//	This driver is generic, abstracted such that multiple LCDs 
// can be connected to a single system.  The HW ports and bits are 
// defined for each display through the use of the pvDriverExpansion
// pointer in each driver struct.  Here, the pointer is assumed to be a 
// HD44780_DRIVER_STRUCT which can be specified for each display
// at runtime or design time.  See the .h file for more information about
// the implementation.
//--------------------------------------------------------------------------- 

#include "types.h"
#include "driver.h"
#include "drvHD44780.h"

//---------------------------------------------------------------------------
// Command code lookup table
static const UCHAR aucCmdLookup[] = 
{
	HD44780_CMD_CLEAR, 
	HD44780_CMD_HOME, 
	HD44780_CMD_MODE,
	HD44780_CMD_DISPLAY,
	HD44780_CMD_CURSOR,
	HD44780_CMD_FUNCTION,
	HD44780_CMD_SETCGRAM,
	HD44780_CMD_SETDDRAM
};

//---------------------------------------------------------------------------
static void HD44780_Write(HD44780_DRIVER_STRUCT *pstThis_, UCHAR ucData_, BOOL bData_);
static void HD44780_Read(HD44780_DRIVER_STRUCT *pstThis_, UCHAR *pucData_, BOOL bData_);
static BOOL HD44780_IsBusy(HD44780_DRIVER_STRUCT *pstThis_);
static void HD44780_EToggle(HD44780_DRIVER_STRUCT *pstThis_);

//---------------------------------------------------------------------------
static void HD44780_Delay(USHORT us)
{
	volatile UCHAR i;
	
	// Approximate loop - 1us per loop @ 16MHz (w/loop overhead)
	//!! NOTE - GCC optimizes this out, move to an unoptimized source module if
	// optimizing under AVRGCC
	for (i = 0; i < usMicroSeconds_; i++)
	{
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
	}
}
 
//---------------------------------------------------------------------------
void HD44780_Init(DRIVER_STRUCT *pstThis_)
{
	volatile HD44780_DRIVER_STRUCT *pstLCDConfig = (HD44780_DRIVER_STRUCT*)pstThis_;
	// Setup appropriate driving pins as outputs
	HD44780_DATA_DIR |= (0x0F) << HD44780_DATA_SHIFT;
	HD44780_RS_DIR |= HD44780_RS_BIT;
	HD44780_RS_OUT &= ~HD44780_RS_BIT;
	HD44780_RW_DIR |= HD44780_RW_BIT;
	HD44780_RW_OUT &= ~HD44780_RW_BIT;
	HD44780_ENABLE_DIR |= HD44780_ENABLE_BIT;
	HD44780_ENABLE_OUT &= ~HD44780_ENABLE_BIT;
	
	
	HD44780_DATA_DIR |= (0x0F) << HD44780_DATA_SHIFT;
	HD44780_RS_DIR |= HD44780_RS_BIT;
	HD44780_RS_OUT &= ~HD44780_RS_BIT;
	HD44780_RW_DIR |= HD44780_RW_BIT;
	HD44780_RW_OUT &= ~HD44780_RW_BIT;
	HD44780_ENABLE_DIR |= HD44780_ENABLE_BIT;
	HD44780_ENABLE_OUT &= ~HD44780_ENABLE_BIT;
}
//---------------------------------------------------------------------------
void HD44780_Start(DRIVER_STRUCT *pstThis_)
{
	// proper reset of the HD44780 LCD
	UCHAR ucFlags;
	UCHAR ucTemp;
	volatile HD44780_DRIVER_STRUCT *pstLCDConfig = (HD44780_DRIVER_STRUCT*)pstThis_;
	
	// Time-hungry
    HD44780_Delay(16000);                           /* wait 16ms after power-on     */

	// Default init requires an 8-bit set.
	ucTemp = 0x03;
	HD44780_DATA_OUT &= ~((0x0F) << HD44780_DATA_SHIFT);
	HD44780_DATA_OUT |= (ucTemp << HD44780_DATA_SHIFT);

	HD44780_EToggle(pstLCDConfig);
    HD44780_Delay(4992);         

	HD44780_DATA_OUT &= ~((0x0F) << HD44780_DATA_SHIFT);
	HD44780_DATA_OUT |= (ucTemp<< HD44780_DATA_SHIFT);

    HD44780_EToggle(pstLCDConfig);
    HD44780_Delay(64);           

	HD44780_DATA_OUT &= ~((0x0F) << HD44780_DATA_SHIFT);
	HD44780_DATA_OUT |= (ucTemp<< HD44780_DATA_SHIFT);

    HD44780_EToggle(pstLCDConfig);
    HD44780_Delay(64);           

	if (pstLCDConfig->b4Bit)
	{
		// Set up the interface for 4-bit mode
		ucTemp = 0x02; 
	}
	else
	{
		// Run as 8-bit mode
		ucTemp = 0x03;
	}

	HD44780_DATA_OUT &= ~((0x0F) << HD44780_DATA_SHIFT);
	HD44780_DATA_OUT |= (ucTemp<< HD44780_DATA_SHIFT);

    HD44780_EToggle(pstLCDConfig);
    HD44780_Delay(64);           

	// LCD Initialized - use command interface.
	pstLCDConfig->bInit = TRUE;
	ucFlags = HD44780_FUNCTION_4BIT | HD44780_FUNCTION_1LINE;
	HD44780_Control(pstLCDConfig, HD44780_EVENT_FUNCTION, &ucFlags);

	//Display off
	ucFlags = HD44780_DISPLAY_BIT_OFF;
	HD44780_Control(pstLCDConfig, HD44780_EVENT_DISPLAY, &ucFlags);
	
	//Reset screen
	HD44780_Control(pstLCDConfig, HD44780_EVENT_RESET, NULL);
	
	// Set *final* display parameters
	if (pstLCDConfig->usNumRows == 1)
	{
		ucFlags = HD44780_FUNCTION_1LINE;
	}
	else
	{
		ucFlags = HD44780_FUNCTION_2LINE;
	}
	
	if (pstLCDConfig->b4Bit)
	{
		ucFlags |= HD44780_FUNCTION_4BIT;
	}
	else
	{
		ucFlags |= HD44780_FUNCTION_8BIT;
	}

	HD44780_Control(pstLCDConfig, HD44780_EVENT_FUNCTION, &ucFlags);

	ucFlags = HD44780_DISPLAY_BIT_OFF;
	HD44780_Control(pstLCDConfig, HD44780_EVENT_DISPLAY, &ucFlags);

	// Set the cursor to move  automatically
	ucFlags = HD44780_CURSOR_MOVE | HD44780_CURSOR_RIGHT;
	HD44780_Control(pstLCDConfig, HD44780_EVENT_CURSOR, &ucFlags);
	
	ucFlags = HD44780_MODE_BIT_DECREMENT | HD44780_MODE_BIT_SHIFT;
	HD44780_Control(pstLCDConfig, HD44780_EVENT_MODE, &ucFlags);

	//Display on
	ucFlags = HD44780_DISPLAY_BIT_ON;
	HD44780_Control(pstLCDConfig, HD44780_EVENT_DISPLAY, &ucFlags);

	HD44780_Control(pstLCDConfig, HD44780_EVENT_RESET, NULL);
}
//---------------------------------------------------------------------------
void HD44780_Stop(DRIVER_STRUCT *pstThis_)
{
	// Set drivers as tri-state input
	HD44780_DATA_DIR &= ~((0x0F) << HD44780_DATA_SHIFT);
	HD44780_DATA_OUT &= ~((0x0F) << HD44780_DATA_SHIFT);
	HD44780_RS_DIR &= ~HD44780_RS_BIT;
	HD44780_RS_OUT &= ~HD44780_RS_BIT;
	HD44780_RW_DIR &= ~HD44780_RW_BIT;
	HD44780_RW_OUT &= ~HD44780_RW_BIT;
	HD44780_ENABLE_DIR &= ~HD44780_ENABLE_BIT;
	HD44780_ENABLE_OUT &= ~HD44780_ENABLE_BIT;
}
//---------------------------------------------------------------------------
void HD44780_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_)
{	
	UCHAR ucFlags = 0;
	volatile HD44780_DRIVER_STRUCT *pstLCDConfig = (HD44780_DRIVER_STRUCT*)pstThis_;

	if (pvData_ != NULL)
	{
		ucFlags = *((UCHAR*)pvData_);
	}

	if (pstLCDConfig->bInit)
	{
		while(HD44780_IsBusy(pstLCDConfig)){};
	}

	switch(usID_)
	{
		// Switch command IDs - group together by function
		case HD44780_EVENT_RESET:
		case HD44780_EVENT_HOME:
		case HD44780_EVENT_MODE:
		case HD44780_EVENT_DISPLAY:
		case HD44780_EVENT_CURSOR:
		case HD44780_EVENT_FUNCTION:
		case HD44780_EVENT_SETCG:
		case HD44780_EVENT_SETDD:
		{
			HD44780_Write(pstLCDConfig, aucCmdLookup[usID_-HD44780_EVENT_RESET] | ucFlags, FALSE); 
		}
			break;
		case HD44780_EVENT_WRITE:
		{
			UCHAR *pucData = (UCHAR*)pvData_;
			UCHAR i = 0;

			HD44780_Write(pstLCDConfig, HD44780_CMD_SETDDRAM | (pstLCDConfig->ucRow * 0x40) | pstLCDConfig->ucCol, FALSE);

			i = 0;
			while (pucData[i] != '\0' && i < 255)
			{ 
				while(HD44780_IsBusy(pstLCDConfig)){};
				if (pucData[i] == '\0' || pucData[i] == '\n' || pucData[i] == '\t' || pucData[i] == '\v' || pucData[i] == 13)
				{
					pstLCDConfig->ucRow = (pstLCDConfig->ucRow + 1) % pstLCDConfig->usNumRows;
					pstLCDConfig->ucCol = 0;
					HD44780_Write(pstLCDConfig, HD44780_CMD_SETDDRAM | (pstLCDConfig->ucRow * 0x40) , FALSE);
					i++;
					continue;
				}
				
				HD44780_Write(pstLCDConfig, pucData[i], TRUE); 
				i++;
				pstLCDConfig->ucCol++;

				if (pstLCDConfig->ucCol >= pstLCDConfig->usNumCols)
				{
					pstLCDConfig->ucCol = 0;
					pstLCDConfig->ucRow = (pstLCDConfig->ucRow + 1) % pstLCDConfig->usNumRows;
					while(HD44780_IsBusy(pstLCDConfig)){};					
					HD44780_Write(pstLCDConfig, HD44780_CMD_SETDDRAM | (pstLCDConfig->ucRow * 0x40) , FALSE);
				}
			}
		}
			break;
		case HD44780_EVENT_READ:
		{
		}
			break;
		case HD44780_EVENT_SETROW:
		{
			pstLCDConfig->ucRow = ucFlags;
		}
			break;
		case HD44780_EVENT_SETCOL:
		{
			pstLCDConfig->ucCol = ucFlags;
		}
			break;
		default:
			break;		
	}
}
//---------------------------------------------------------------------------
static void HD44780_Write(HD44780_DRIVER_STRUCT *pstThis_, UCHAR ucData_, BOOL bData_)
{
	// Set data pins as output
	if (pstThis_->b4Bit)
	{
		HD44780_DATA_DIR |= (0x0F) << HD44780_DATA_SHIFT;
	}
	else
	{
		HD44780_DATA_DIR |= 0xFF;
	}

	HD44780_RW_DIR |= HD44780_RW_BIT;
	HD44780_RW_OUT &= ~HD44780_RW_BIT;

	HD44780_RS_DIR |= HD44780_RS_BIT;

	// Write Data
	if (bData_)
	{
		HD44780_RS_OUT |= HD44780_RS_BIT;
	}
	// Write Instruction
	else
	{
		HD44780_RS_OUT &= ~HD44780_RS_BIT;
	}
	
	// Ensure all of the control lines are enabled as outputs
	*HD44780_ENABLE_DIR |= HD44780_ENABLE_BIT;

	// Set Char, Toggle Enable
	if (pstThis_->b4Bit)
	{
		UCHAR ucTemp = HD44780_DATA_OUT;
		ucTemp &= ~(0x0F << HD44780_DATA_SHIFT);
		ucTemp |= ((ucData_ & 0xF0) >> 4) << HD44780_DATA_SHIFT;

		// Set high nibble 
		HD44780_DATA_OUT = ucTemp;
		HD44780_EToggle(pstThis_);
		
		ucTemp = HD44780_DATA_OUT;
		ucTemp &= ~(0x0F << HD44780_DATA_SHIFT);
		ucTemp |= (ucData_ & 0x0F) << HD44780_DATA_SHIFT;

		// Set low nibble
		HD44780_DATA_OUT = ucTemp;
	}
	else
	{
		// Clock in all 8-bits
		HD44780_DATA_OUT = ucData_;
	}

	// Toggle Enable line
	HD44780_EToggle(pstThis_);

	// All lines high
	HD44780_DATA_OUT = (0x0F << HD44780_DATA_SHIFT);

}
//---------------------------------------------------------------------------
static void HD44780_Read(HD44780_DRIVER_STRUCT *pstThis_, UCHAR *pucData_, BOOL bData_)
{
	UCHAR ucDataH = 0;
	UCHAR ucDataL = 0;

    if (bData_)
	{
        HD44780_RS_OUT |= HD44780_RS_BIT;
	}
    else
	{
        HD44780_RS_OUT &= ~HD44780_RS_BIT;
	}
    
	HD44780_RW_OUT |= HD44780_RW_BIT;

    // configure data pins as input 
	if (pstThis_->b4Bit)
	{
    	HD44780_DATA_DIR &= ~(0x0F << HD44780_DATA_SHIFT);
	}
	else
	{
		HD44780_DATA_DIR &= ~0xFF;
	}

	HD44780_ENABLE_OUT |= HD44780_ENABLE_BIT;
  
	HD44780_Delay(40);
    ucDataH = HD44780_PIN;              

	HD44780_ENABLE_OUT &= ~HD44780_ENABLE_BIT;
	
	if (pstThis_->b4Bit)
	{
	    HD44780_Delay(40);

	    HD44780_ENABLE_OUT |= HD44780_ENABLE_BIT;
    
		HD44780_Delay(40);
	    ucDataL = HD44780_PIN;              /* read high nibble first */
	    HD44780_ENABLE_OUT &= ~HD44780_ENABLE_BIT;
		return ( (ucDataH&0xf0) | (ucDataL>>4) );
	}
	return ucDataH;

}
//---------------------------------------------------------------------------
static BOOL HD44780_IsBusy(HD44780_DRIVER_STRUCT *pstThis_)
{
	// Read and see if we're busy
	UCHAR ucData;
	HD44780_Read(pstThis_, &ucData, FALSE);
	if (ucData & 0x80)
	{
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------
static void HD44780_EToggle(HD44780_DRIVER_STRUCT *pstThis_)
{
	// Toggle Enable line
	HD44780_ENABLE_OUT |= HD44780_ENABLE_BIT;
	HD44780_Delay(100);
	HD44780_ENABLE_OUT &= ~HD44780_ENABLE_BIT;
	HD44780_Delay(100);
}
//---------------------------------------------------------------------------

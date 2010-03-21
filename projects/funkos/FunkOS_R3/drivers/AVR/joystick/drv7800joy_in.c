//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drv7800joy_in.c
//
// Description:
//	Input driver for Atari 7800 compatible joysticks.  
//--------------------------------------------------------------------------- 

#include "types.h"
#include "driver.h"
#include "drv7800joy_in.h"

//---------------------------------------------------------------------------
static void Atari7800i_Read(ATARI7800_IN_STRUCT *pstJoy_);

//---------------------------------------------------------------------------
void Atari7800i_Init(DRIVER_STRUCT *pstThis_)
{
	ATARI7800_IN_STRUCT *pstJoy = (ATARI7800_IN_STRUCT*)pstThis_;
	
	// Set the joypad port bits to inputs.
	*(pstJoy->pucPortDirUp)		&= ~pstJoy->ucPortBitUp;			
	*(pstJoy->pucPortDirDown)	&= ~pstJoy->ucPortBitDown;		
	*(pstJoy->pucPortDirLeft)	&= ~pstJoy->ucPortBitLeft;		
	*(pstJoy->pucPortDirRight)	&= ~pstJoy->ucPortBitRight;		
	*(pstJoy->pucPortDirButtonL)&= ~pstJoy->ucPortBitButtonL;	
	*(pstJoy->pucPortDirButtonR)&= ~pstJoy->ucPortBitButtonR;		

	// Set pullups on D-pad lines, but not buttons.
	*(pstJoy->pucPortUp)		|= pstJoy->ucPortBitUp;			
	*(pstJoy->pucPortDown)		|= pstJoy->ucPortBitDown;		
	*(pstJoy->pucPortLeft)		|= pstJoy->ucPortBitLeft;		
	*(pstJoy->pucPortRight)		|= pstJoy->ucPortBitRight;		
	*(pstJoy->pucPortButtonL)	&= ~pstJoy->ucPortBitButtonL;	
	*(pstJoy->pucPortButtonR)	&= ~pstJoy->ucPortBitButtonR;	
	
	// Clear the data in the joystick report struct
	Joystick_Clear(&pstJoy->stReport);
}
//---------------------------------------------------------------------------
void Atari7800i_Start(DRIVER_STRUCT *pstThis_)
{
	// Nothing to do to start the driver...
}
//---------------------------------------------------------------------------
void Atari7800i_Stop(DRIVER_STRUCT *pstThis_)
{
	// Nothing to do to stop the driver...
}
//---------------------------------------------------------------------------
void Atari7800i_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_)
{	
	// Cast the driver to the specified type
	ATARI7800_IN_STRUCT *pstJoy = (ATARI7800_IN_STRUCT*)pstThis_;
	
	// Switch I/O control operation by ID
	switch(usID_)
	{
		// Read the joystick buttons
		case JOY_IN_READ:
		{
			// Read the buttons...
			Atari7800i_Read(pstJoy);
		}
			break;
			
		// Copy the value of the buttons from another type
		case JOY_IN_COPY:
		{
			// Check to make sure the output ptr isn't null first...
			if (pvData_ == NULL)
			{
				break;
			}
			
			// Copy the joystick data out
			Joystick_Copy(pvData_, &pstJoy->stReport);
		}
			break;
		default:
			break;
	}
}

//---------------------------------------------------------------------------
static void Atari7800i_Read(ATARI7800_IN_STRUCT *pstJoy_)
{
	USHORT usDebounceCount;
	USHORT usTimeoutCount;
	BOOL bButtonState;
	unsigned int uiButtonVal;
	ATARI7800_IN_BUTTONS eReadIndex;
	
	// For each switch on the controller
	for (eReadIndex = (ATARI7800_IN_BUTTONS)0; eReadIndex < ATARI7800_IN_BUTTON_COUNT; eReadIndex++)
	{
		// Reset the debounce counter
		usDebounceCount = ATARI7800_IN_INITIAL;
		// Reset the timeout counter
		usTimeoutCount = 0;
		
		// Button debounce loop
		while (	
				// While there isn't a consensus about the state of the button
				(usDebounceCount < ATARI7800_IN_HIGHCOUNT) &&
				(usDebounceCount > ATARI7800_IN_LOWCOUNT) &&
				
				// And the button hasn't timed out
				(usTimeoutCount < ATARI7800_IN_TIMEOUT)
			  )
		{
			// Preset the button state to FALSE - set TRUE on read high.
			bButtonState = FALSE;
			switch(eReadIndex)
			{
				//---------------------------------------------------
				case ATARI7800_IN_BUTTON_UP:
				{
					if (*(pstJoy_->pucPortInUp) & pstJoy_->ucPortBitUp)
					{
						bButtonState = TRUE;
					}
				}
					break;
				
				//---------------------------------------------------
				case ATARI7800_IN_BUTTON_DOWN:
				{
					if (*(pstJoy_->pucPortInDown) & pstJoy_->ucPortBitDown)
					{
						bButtonState = TRUE;
					}
				}
					break;
				
				//---------------------------------------------------
				case ATARI7800_IN_BUTTON_LEFT:
				{
					if (*(pstJoy_->pucPortInLeft) & pstJoy_->ucPortBitLeft)
					{
						bButtonState = TRUE;
					}
				}
					break;
				
				//---------------------------------------------------
				case ATARI7800_IN_BUTTON_RIGHT:
				{
					if (*(pstJoy_->pucPortInRight) & pstJoy_->ucPortBitRight)
					{
						bButtonState = TRUE;
					}
				}
					break;
				
				//---------------------------------------------------
				case ATARI7800_IN_BUTTON_BUTTONL:
				{
					if (*(pstJoy_->pucPortInButtonL) & pstJoy_->ucPortBitButtonL)
					{
						bButtonState = TRUE;
					}
				}
					break;
				
				//---------------------------------------------------
				case ATARI7800_IN_BUTTON_BUTTONR:
				{
					if (*(pstJoy_->pucPortInButtonR) & pstJoy_->ucPortBitButtonR)
					{
						bButtonState = TRUE;
					}
				}
					break;
				
				//---------------------------------------------------
				default:
					break;
			}
			
			// Increment or decrement the debounce count
			if (bButtonState)
			{
				usDebounceCount++;
			}
			else
			{
				usDebounceCount--;
			}
			
			// Increment the debounce stability timeout counter
			usTimeoutCount++;
		}
		
		// Check to see the state of the switch...
		// - Either we hit the high or low markers, or the timeout
		if (usDebounceCount > ATARI7800_IN_INITIAL)
		{
			uiButtonVal = 1;
		}
		else
		{
			uiButtonVal = 0;
		}
		
		// Set or clear the value for the current switch
		switch(eReadIndex)
		{
			//--[UP Switch]--------------------------------------
			case ATARI7800_IN_BUTTON_UP:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_ATARI7800.bUp = uiButtonVal;
			}
				break;
			
			//--[DOWN Switch]------------------------------------			
			case ATARI7800_IN_BUTTON_DOWN:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_ATARI7800.bDown = uiButtonVal;
			}
				break;
			
			//--[LEFT Switch]------------------------------------
			case ATARI7800_IN_BUTTON_LEFT:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_ATARI7800.bLeft = uiButtonVal;
			}
				break;
				
			//--[RIGHT Switch]-----------------------------------
			case ATARI7800_IN_BUTTON_RIGHT:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_ATARI7800.bRight = uiButtonVal;
			}
				break;
			
			//--[BUTTONL Switch]---------------------------------
			case ATARI7800_IN_BUTTON_BUTTONL:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_ATARI7800.bButtonL = uiButtonVal;
			}
				break;
			
			//--[BUTTONR Switch]---------------------------------
			case ATARI7800_IN_BUTTON_BUTTONR:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_ATARI7800.bButtonR = uiButtonVal;
			}
				break;
				
			// Should never get here.
			default:
				break;
		}
	}
}


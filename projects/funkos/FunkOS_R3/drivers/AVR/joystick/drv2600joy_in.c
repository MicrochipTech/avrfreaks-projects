//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drv2600joy_in.c
//
// Description:
//	Input driver for Atari 2600 compatible joysticks.  
//--------------------------------------------------------------------------- 

#include "types.h"
#include "driver.h"
#include "drv2600joy_in.h"
#include "joystick.h"

//---------------------------------------------------------------------------
static void Atari2600i_Read(ATARI2600_IN_STRUCT *pstJoy_);

//---------------------------------------------------------------------------
void Atari2600i_Init(DRIVER_STRUCT *pstThis_)
{
	ATARI2600_IN_STRUCT *pstJoy = (ATARI2600_IN_STRUCT*)pstThis_;
	
	// Set the joypad port bits to inputs.
	ATARI_DIR_UP	&= ~ATARI_BIT_UP;
	ATARI_DIR_DOWN	&= ~ATARI_BIT_DOWN;		
	ATARI_DIR_LEFT	&= ~ATARI_BIT_LEFT;		
	ATARI_DIR_RIGHT	&= ~ATARI_BIT_RIGHT;		
	ATARI_DIR_FIRE	&= ~ATARI_BIT_FIRE;	
	
	// Set pullups on every line
	ATARI_PORT_UP		|= ATARI_BIT_UP;			
	ATARI_PORT_DOWN		|= ATARI_BIT_DOWN;		
	ATARI_PORT_LEFT		|= ATARI_BIT_LEFT;		
	ATARI_PORT_RIGHT	|= ATARI_BIT_RIGHT;		
	ATARI_PORT_FIRE		|= ATARI_BIT_FIRE;	
	
	// Clear the data in the joystick report struct
	Joystick_Clear(&pstJoy->stReport);
}
//---------------------------------------------------------------------------
void Atari2600i_Start(DRIVER_STRUCT *pstThis_)
{
	// Nothing to do to start the driver...
}
//---------------------------------------------------------------------------
void Atari2600i_Stop(DRIVER_STRUCT *pstThis_)
{
	// Nothing to do to stop the driver...
}
//---------------------------------------------------------------------------
void Atari2600i_Control(DRIVER_STRUCT *pstThis_, USHORT usID_, void *pvData_)
{	
	// Cast the driver to the specified type
	ATARI2600_IN_STRUCT *pstJoy = (ATARI2600_IN_STRUCT*)pstThis_;
	
	// Switch I/O control operation by ID
	switch(usID_)
	{
		// Read the joystick buttons
		case JOY_IN_READ:
		{
			// Read the buttons...
			Atari2600i_Read(pstJoy);
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
			Joystick_Copy((pvData_), (&pstJoy->stReport));
		}
			break;
		default:
			break;
	}
}

//---------------------------------------------------------------------------
static void Atari2600i_Read(ATARI2600_IN_STRUCT *pstJoy_)
{
	USHORT usDebounceCount;
	USHORT usTimeoutCount;
	BOOL bButtonState;
	unsigned int uiButtonVal;
	ATARI2600_IN_BUTTONS eReadIndex;
	
	// For each switch on the controller
	for (eReadIndex = (ATARI2600_IN_BUTTONS)0; eReadIndex < ATARI2600_IN_BUTTON_COUNT; eReadIndex++)
	{
		// Reset the debounce counter
		usDebounceCount = ATARI2600_IN_INITIAL;
		// Reset the timeout counter
		usTimeoutCount = 0;
		
		// Button debounce loop
		while (	
				// While there isn't a consensus about the state of the button
				(usDebounceCount < ATARI2600_IN_HIGHCOUNT) &&
				(usDebounceCount > ATARI2600_IN_LOWCOUNT) &&
				
				// And the button hasn't timed out
				(usTimeoutCount < ATARI2600_IN_TIMEOUT)
			  )
		{
			// Preset the button state to FALSE - set TRUE on read high.
			bButtonState = FALSE;
			switch(eReadIndex)
			{
				//---------------------------------------------------
				case ATARI2600_IN_BUTTON_UP:
				{
					if ((ATARI_PIN_UP & ATARI_BIT_UP) == 0)
					{
						bButtonState = TRUE;
					}
				}
					break;
				
				//---------------------------------------------------
				case ATARI2600_IN_BUTTON_DOWN:
				{
					if ((ATARI_PIN_DOWN & ATARI_BIT_DOWN) == 0)
					{
						bButtonState = TRUE;
					}
				}
					break;
				
				//---------------------------------------------------
				case ATARI2600_IN_BUTTON_LEFT:
				{
					if ((ATARI_PIN_LEFT & ATARI_BIT_LEFT) == 0)
					{
						bButtonState = TRUE;
					}
				}
					break;
				
				//---------------------------------------------------
				case ATARI2600_IN_BUTTON_RIGHT:
				{
					if ((ATARI_PIN_RIGHT & ATARI_BIT_RIGHT) == 0)
					{
						bButtonState = TRUE;
					}
				}
					break;
				
				//---------------------------------------------------
				case ATARI2600_IN_BUTTON_BUTTON:
				{
					if ((ATARI_PIN_FIRE & ATARI_BIT_FIRE) == 0)
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
		if (usDebounceCount > ATARI2600_IN_INITIAL)
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
			case ATARI2600_IN_BUTTON_UP:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_ATARI2600.bUp = uiButtonVal;
			}
				break;
			
			//--[DOWN Switch]------------------------------------			
			case ATARI2600_IN_BUTTON_DOWN:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_ATARI2600.bDown = uiButtonVal;
			}
				break;
			
			//--[LEFT Switch]------------------------------------
			case ATARI2600_IN_BUTTON_LEFT:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_ATARI2600.bLeft = uiButtonVal;
			}
				break;
				
			//--[RIGHT Switch]-----------------------------------
			case ATARI2600_IN_BUTTON_RIGHT:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_ATARI2600.bRight = uiButtonVal;
			}
				break;
			
			//--[BUTTON Switch]----------------------------------
			case ATARI2600_IN_BUTTON_BUTTON:
			{
				pstJoy_->stReport.JOY_DIGITAL.JOY_ATARI2600.bButton = uiButtonVal;
			}
				break;
				
			// Should never get here.
			default:
				break;
		}
	}
}


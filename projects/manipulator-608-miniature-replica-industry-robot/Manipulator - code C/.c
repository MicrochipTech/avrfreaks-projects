/*
 * Menu.c
 *
 *  Created on: 16 lis 2013
 *      Author: M.R.O.
 */

/**************************
 * --->>  Includes  <<--- *
 **************************/

#include <avr/io.h>
#include "hd44780.h"
#include <util/delay.h>
#include "Functions.h"
#include "PcComands.h"
#include "Globals_Defs.h"

/***************************
 * --->>  Functions  <<--- *
 ***************************/

/*!
* LedLight - Control of buttons LEDs
*
* Function is used to turn on/off four LEDs mounted in buttons.
* Only four LSB bits are used. Each high level of bit turn on corresponding LED.
* 	0x01 - Yellow led L Button.
* 	0x02 - Yellow led P Button.
* 	0x04 - Green led OK Button.
* 	0x08 - Red led ESC Button.
*/

void LedLight(uint8 Led)
{
	if (Led & 0x01) /*L Button*/
	{
		PORTC_OUTSET = PIN5_bm;
	}
	else { PORTC_OUTCLR = PIN5_bm;	}

	if (Led & 0x02) /*P Button*/
	{
		PORTQ_OUTSET |= PIN1_bm;
	}
	else { PORTQ_OUTCLR = PIN1_bm;	}

	if (Led & 0x04) /*OK Button*/
	{
		PORTQ_OUTSET |= PIN2_bm;
	}
	else { PORTQ_OUTCLR = PIN2_bm;	}

	if (Led & 0x08) /*ESC Button*/
	{
		PORTH_OUTSET |= PIN1_bm;
	}
	else { PORTH_OUTCLR = PIN1_bm;	}
}

/*!
* ButtonState - Debouncing buttons
*
* Function is used to increment or decrement global array Menulvl[3] which causes menu navigation.
* It react on buttons changes mounted in driver housing.
* 	Menulvl[0] range: 1..3,
* 	Menulvl[1] range: 1..7,
* 	Menulvl[2] range: 0..1,
*/

void ButtonState(void)
{
		if (BUTTON_UP) /* Button P */
		{
			if (ResetDelayFlag == TRUE) /* Delay loop */
			{
				RESET_DELAY_VAR
				WaitTime = 200;
			}
			if (DelayFlag == TRUE)
			{
				if (BUTTON_UP) /* If button is still holden */
				{
					if (ButtonWork == MENU) /* If we are navigating in menu, increment menu variable */
					{
						if ( Menulvl[0] == 1 ) /* Main menu */
						{
							Menulvl[1]++;
							if (Menulvl[1] == 8) Menulvl[1]=0;
						}
						else if (Menulvl[0] == 2 ) /* 1st level */
						{
							Menulvl[2]++;
							if (Menulvl[2] == 2) Menulvl[2] = 0;
						}
					}
					else if (ButtonWork == VELOCITY_SETUP) /* Else if we are in speed setup (Menulvl[0] = 2 and Menulvl[1] = 1..6) increment speed variable */
					{
						Pid[Menulvl[1]-1].MaxCv += 50;
						if (Pid[Menulvl[1]-1].MaxCv >= 1000)
						{
							Pid[Menulvl[1]-1].MaxCv = 1000;
						}
					}
					else if (ButtonWork == OVERRIDE) /* Else if we are in internal control do not increment any variable. Button P or L cause movement. Button OK and ESC working normal */
					{
					}
				}
			ResetDelayFlag = TRUE;
			DelayFlag = FALSE;
			AnyButtonChange = TRUE; /* Set flag so text on LCD will be refreshed */
			}
		}

		else if (BUTTON_DOWN) /* Button L */
		{
			if (ResetDelayFlag == TRUE)
			{
				RESET_DELAY_VAR
				WaitTime = 200;
			}
			if (DelayFlag == TRUE)
			{
				if (BUTTON_DOWN)
				{
					if (ButtonWork == MENU)
						{
							if (Menulvl[0] == 1)	/* Main menu */
							{
								Menulvl[1]--;
								if (Menulvl[1] == 255) Menulvl[1]=7;
							}
							if (Menulvl[0] == 2)	/* 1st level */
							{
								Menulvl[2]--;
								if (Menulvl[2] == 255) Menulvl[2] = 1;
							}
						}
						else if (ButtonWork == VELOCITY_SETUP)
						{
							Pid[Menulvl[1]-1].MaxCv -= 50;
							if (Pid[Menulvl[1]-1].MaxCv <= 100)
							{
								Pid[Menulvl[1]-1].MaxCv = 100;
							}
						}
						else if (ButtonWork == OVERRIDE)
						{
						}
					}
				ResetDelayFlag = TRUE;
				DelayFlag = FALSE;
				AnyButtonChange = TRUE;
				}
			}

	if (BUTTON_OK) /* Button OK */
	{
		if (ResetDelayFlag == TRUE)
		{
			RESET_DELAY_VAR
			WaitTime = 200;
		}
		if (DelayFlag == TRUE)
		{
			if (BUTTON_OK)
			{
				Menulvl[0]++;
				if (Menulvl[0] == 4)
				{
					Menulvl[0] = 3;
				}
			}
			ResetDelayFlag = TRUE;
			DelayFlag = FALSE;
			AnyButtonChange = TRUE;
		}
	}

	if (BUTTON_ESC) /* Button ESC */
	{
		if (ResetDelayFlag == TRUE)
		{
			RESET_DELAY_VAR
			WaitTime = 200;
		}
		if (DelayFlag == TRUE)
		{
			if (BUTTON_ESC)
			{
				Menulvl[0]--;
				ControlType = IDLE_CONTROL;
				if (Menulvl[0] == 0)
				{
					Menulvl[0] = 1;
				}
			}
			ResetDelayFlag = TRUE;
			DelayFlag = FALSE;
			AnyButtonChange = TRUE;
		}
	}
}

/*!
* LCDWelcomTxt - Most important function in program
*/

void LCDWelcomeTxt(void)
{
		LcdClear();
		LcdWrite("    Welcome!");
		LcdGoto(2,1);
		LcdWrite(" Menu - OK");
}

/*!
* LCDDisplay - Displaying messages on LCD screen
*
* function is to display current statement of menu on the Lcd Screen, also switches
* between internal/external/idle control.
*/

void LCDDisplay(void)
{
	if (Menulvl[0] == 1)
	{
		switch (Menulvl[1])
		{
		case 0: LcdClear();
				LcdWrite(" Control via PC");
				LedLight(6);
				break;
		case 1: LcdClear();
				LcdWrite("Selected axis: 1");
				LedLight(7);
				break;
		case 2: LcdClear();
				LcdWrite("Selected axis: 2");
				LedLight(7);
				break;
		case 3: LcdClear();
				LcdWrite("Selected axis: 3");
				LedLight(7);
				break;
		case 4: LcdClear();
				LcdWrite("Selected axis: 4");
				LedLight(7);
				break;
		case 5: LcdClear();
				LcdWrite("Selected axis: 5");
				LedLight(7);
				break;
		case 6: LcdClear();
				LcdWrite("Selected axis: 6");
				LedLight(7);
				break;
		case 7: LcdClear();
				LcdWrite("    Gripper");
				LedLight(5);
				break;
		}
	}

	if ((Menulvl[0] == 2) && (Menulvl[1] == 0))
	{
		LcdClear();
		LcdWrite(" Connected to PC");
		LedLight(8);

		ControlType = EXTERNAL_CONTROL;
	}

	if (Menulvl[0] == 1 && Menulvl[1] == 0)
	{
		LcdClear();
		LcdWrite(" Control via PC");
		LedLight(6);
		ControlType = IDLE_CONTROL;
	}

	if (Menulvl[0] == 2 && Menulvl[1] >= 1 && Menulvl[1] <=6)
	{
		switch (Menulvl[2])
		{
			case 0: LcdClear();
					LcdWrite(" Axis movement");
					LedLight(15);
					break;
			case 1: LcdClear();
					LcdWrite("    Velocity");
					LedLight(15);
					break;
		}

	}
	if (Menulvl[0] == 2 && Menulvl[1] == 7)
	{
		switch (Menulvl[2])
		{
			case 0: LcdClear();
					LcdWrite("    Gripper");
					LedLight(15);
					LcdGoto(0,1);
					LcdWrite("    Control");
					LedLight(15);
		}
	}

	if (Menulvl[0] == 3 && Menulvl[1] >= 1 && Menulvl[1] <= 6 && Menulvl[2] == 1)
	{
		ButtonWork = VELOCITY_SETUP;

		LcdClear();
		LcdGoto(0,0);
		LcdWrite("    Velocity");
		LcdGoto(1,0);
		LcdWrite("  Value: ");
		LedLight(11);
		LcdDec(Pid[Menulvl[1]-1].MaxCv);
	}

	if (Menulvl[0] == 2 && Menulvl[1] >= 1 && Menulvl[1] <= 6 && Menulvl[2] == 1)
	{
		ControlType = IDLE_CONTROL;
		ButtonWork = MENU;
	}

	if (Menulvl[0] == 3 && Menulvl[1] >= 1 && Menulvl[1] <= 7 && Menulvl[2] == 0)
	{
		LcdClear();
		LcdWrite(" Ready to move");
		LedLight(11);
		ButtonWork = OVERRIDE;
		ControlType = INTERNAL_CONTROL;
	}

	if (Menulvl[0] == 2 && Menulvl[1] >=1 && Menulvl[1] <=7 && Menulvl[2] == 0)
	{
		ControlType = IDLE_CONTROL;
		ButtonWork = MENU;
	}
}

/*!
* InternalControl - Controlling robots movement via buttons.
*/

void InternalControl(void)
{
	if (Menulvl[1] > 0 && Menulvl[1]<8)
	{
		if (BUTTON_UP)  Onlinecontrol((Menulvl[1]-1),LEFT);
		if (BUTTON_DOWN) Onlinecontrol((Menulvl[1]-1),RIGHT);
		if (!BUTTON_UP && !BUTTON_DOWN) Onlinecontrol((Menulvl[1]-1),STOP);
	}
}


/*
 * Functions.c

 *
 *  Created on: 18 mar 2015
 *      Author: M.R.O.
 */

/**************************
 * --->>  Includes  <<--- *
 **************************/

#include <stdio.h>
#include <avr/io.h>
#include "hd44780.h"
#include "Menu.h"
#include "usart.h"
#include "Adc.h"
#include <util/delay.h>
#include "dma_driver.h"
#include "wdt_driver.h"
#include "Functions.h"
#include "Globals_Defs.h"

/***************************
 * --->>  Functions  <<--- *
 ***************************/

/*!
* ABS - Calculation of absolute value.
*/

int16_t ABS(int16_t value)
{
	if (value < FALSE)
	{
		return -value;
	}
	else
	{
		return value;
	}
}

/*!
* PID -  Calculation of Pid regulator response. In each enter to function, next motor is selected.
*/

void PID(void)
{
	static uint8_t index;

	if (index == GRIPPER) /* 6 engines*/
	{
		index = AXIS_1;
	}

	if (index == AXIS_1)
	{
		Pid[index].ActPoz = A;
	}
	if (index == AXIS_2)
	{
		Pid[index].ActPoz = B;
	}
	if (index == AXIS_3)
	{
		Pid[index].ActPoz = C;
	}
	if (index == AXIS_4)
	{
		Pid[index].ActPoz = D;
	}
	if (index == AXIS_5)
	{
		Pid[index].ActPoz = E;
	}
	if (index == AXIS_6)
	{
		Pid[index].ActPoz = F;
	}

	Pid[index].Error = Pid[index].PozRef - Pid[index].ActPoz;
	Pid[index].AbsError = ABS(Pid[index].Error);

	/********************************************
	 **Setup maximum error of proportional part**
	 ********************************************/

	if (Pid[index].Error > Pid[index].Maxerror)
	{
		Pid[index].Error = 350;

	}
	else if (Pid[index].Error < -(Pid[index].Maxerror))
	{
		Pid[index].Error = -350;
	}

	/********************************************
	 **Setup deadband (optional, initially = 0)**
	 ********************************************/

	if (Pid[index].Error > Pid[index].DeadBand)
		Pid[index].Error = Pid[index].Error - Pid[index].DeadBand;
	else if (Pid[index].Error < Pid[index].DeadBand)
		Pid[index].Error = Pid[index].Error + Pid[index].DeadBand;
	else
		Pid[index].Error = 0;

	Pid[index].Pterm = Pid[index].Error * Pid[index].Kp; /*Calculation of proportional part*/

	if (Pid[index].Ki > 0 && Pid[index].Enable != 0) /*Enable integral calculation if PID on*/
	{
		if (Pid[index].Saturated == 0) /*If Output is on max power, stop calculating integral part*/
		{
			Pid[index].Errori += Pid[index].Error;
		}
		if (Pid[index].MaxErrori != 0)
		{
			if (Pid[index].Errori > Pid[index].MaxErrori)
			{
				Pid[index].Errori = Pid[index].MaxErrori;
			}
			else if (Pid[index].Errori < -Pid[index].MaxErrori)
			{
				Pid[index].Errori = -Pid[index].MaxErrori;
			}
		}
	}
	else
	{
		Pid[index].Errori = 0;
	}
	Pid[index].Iterm = Pid[index].Errori * Pid[index].Ki; /*integral part equation*/

	Pid[index].Errord = Pid[index].Error - Pid[index].LastError; /*Derivative calculation*/
	Pid[index].LastError = Pid[index].Error;

	if (Pid[index].MaxErrord != 0)
	{
		if (Pid[index].Errord > Pid[index].MaxErrord)
		{
			Pid[index].Errord = Pid[index].MaxErrord;
		}
		else if (Pid[index].Errord < -Pid[index].MaxErrord)
		{
			Pid[index].Errord = -Pid[index].MaxErrord;
		}
	}
	Pid[index].Dterm = Pid[index].Errord * Pid[index].Kd;

	Pid[index].Cv = Pid[index].Pterm + Pid[index].Iterm; /*Answer of PID regulator*/
	Pid[index].Cv += Pid[index].Dterm;

	if (Pid[index].Cv > Pid[index].MaxCv) /*Maximum for Answer*/
	{
		Pid[index].Cv = Pid[index].MaxCv;
		Pid[index].Saturated = 1;
	}
	else if (Pid[index].Cv < -Pid[index].MaxCv)
	{
		Pid[index].Cv = -Pid[index].MaxCv;
		Pid[index].Saturated = 1;
	}
	else
	{
		Pid[index].Saturated = 0;
	}

	/*Setting signal for Motor 1*/
	if (Pid[index].Enable == 1)
	{
		if (index == AXIS_1)
		{
			if (Pid[0].Cv >= 0)
			{
				Pid[0].Direction = 1;
				M1_LEFT;
			}
			else
			{
				Pid[0].Direction = 0;
				M1_RIGHT;
			}
			if (Pid[0].AbsError <= Pid[0].DeadBand)
			{
				M1_STOP;
			}
			if (Pid[0].Direction == Pid[0].LastDirection)
			{
				TCE0.CCABUF = ABS(Pid[0].Cv);

			}
			else
			{
				TCE0.CCABUF = 0;
			}
			Pid[0].LastDirection = Pid[0].Direction;
		}

		if (Pid[index].Enable == 1)
		{

			/*Setting signal for Motor 2*/

			if (index == AXIS_2)
			{
				if (Pid[1].Cv >= 0)
				{
					Pid[1].Direction = 1;
					M2_LEFT;
				}
				else
				{
					Pid[1].Direction = 0;
					M2_RIGHT;
				}
				if (Pid[1].AbsError <= Pid[1].DeadBand)
				{
					M2_STOP;

				}
				if (Pid[1].Direction == Pid[1].LastDirection)
				{
					TCE0.CCBBUF = ABS(Pid[1].Cv);
				}
				else
				{
					TCE0.CCBBUF = 0;
				}
				Pid[1].LastDirection = Pid[1].Direction;
			}

			/*Setting signal for Motor 3*/

			if (index == AXIS_3)
			{
				if (Pid[index].Cv >= 0)
				{
					Pid[index].Direction = 1;
					M3_LEFT;
				}
				else
				{
					Pid[index].Direction = 0;
					M3_RIGHT;
				}
				if (Pid[index].AbsError <= Pid[index].DeadBand)
				{
					M3_STOP;
				}
				if (Pid[index].Direction == Pid[index].LastDirection)
				{
					TCE0.CCCBUF = ABS(Pid[index].Cv);
				}
				else
				{
					TCE0.CCCBUF = 0;
				}
				Pid[index].LastDirection = Pid[index].Direction;
			}

			/*Setting signal for Motor 4*/

			if (index == AXIS_4)
			{
				if (Pid[index].Cv >= 0)
				{
					Pid[index].Direction = 1;
					M4_RIGHT;
				}
				else
				{
					Pid[index].Direction = 0;
					M4_LEFT;
				}
				if (Pid[index].AbsError <= Pid[index].DeadBand)
				{
					M4_STOP;
				}
				if (Pid[index].Direction == Pid[index].LastDirection)
				{
					TCE0.CCDBUF = ABS(Pid[index].Cv);
				}
				else
				{
					TCE0.CCDBUF = 0;
				}
				Pid[index].LastDirection = Pid[index].Direction;
			}

			/*Setting signal for Motor 5*/

			if (index == AXIS_5)
			{
				if (Pid[index].Cv >= 0)
				{
					Pid[index].Direction = 1;
					M5_LEFT;
				}
				else
				{
					Pid[index].Direction = 0;
					M5_RIGHT;
				}
				if (Pid[index].AbsError <= Pid[index].DeadBand)
				{
					M5_STOP;
				}
				if (Pid[index].Direction == Pid[index].LastDirection)
				{
					TCF0.CCABUF = ABS(Pid[index].Cv);
				}
				else
				{
					TCF0.CCABUF = 0;
				}
				Pid[index].LastDirection = Pid[index].Direction;
			}

			/*Setting signal for Motor 6*/

			if (index == AXIS_6)
			{
				if (Pid[index].Cv >= 0)
				{
					Pid[index].Direction = 1;
					M6_RIGHT;
				}
				else
				{
					Pid[index].Direction = 0;
					M6_LEFT;
				}
				if (Pid[index].AbsError <= Pid[index].DeadBand)
				{
					M6_STOP;
				}
				if (Pid[index].Direction == Pid[index].LastDirection)
				{
					TCF0.CCBBUF = ABS(Pid[index].Cv);
				}
				else
				{
					TCF0.CCBBUF = 0;
				}
				Pid[index].LastDirection = Pid[index].Direction;
			}
		}
	}
	index++;
}

/*!
* Onlinecontrol - Function is steering motors rotation direction and speed.
* 	index - range : 0 - 6. 0 - Axis 1, 1 - Axis 2 ... 6 - Gripper
* 	button - range 1-3. 1 - Left, 2 - Right, 3 Stop
*/

void Onlinecontrol (uint8_t index,uint8_t button)
{
	if (index == 0)
	{
			if (button == 1)
			{
				Pid[0].PozRef = A + Pid[0].OnlineSpeed;
			}
			else if (button == 2)
			{
				Pid[0].PozRef = A - Pid[0].OnlineSpeed;
			}
			else if (button == 3)
			{
				Pid[0].PozRef = A;
			}
		}
	else if (index == 1)
		{
			if (button == 1)
				{
					Pid[1].PozRef = B + Pid[1].OnlineSpeed;
				}
				else if (button == 2)
				{
					Pid[1].PozRef = B - Pid[1].OnlineSpeed;
				}
				else if (button == 3)
				{
					Pid[1].PozRef = B;
				}
		}
	else if (index == 2)
			{
				if (button == 1)
				{
					Pid[index].PozRef = C + Pid[index].OnlineSpeed;
					Pid[index].PositionBuffor = Pid[index].ActPoz;
				}
				else if (button == 2)
				{
					Pid[index].PozRef = C - Pid[index].OnlineSpeed;
					Pid[index].PositionBuffor = Pid[index].ActPoz;
				}
				else if (button == 3)
				{
					Pid[index].PozRef = Pid[index].PositionBuffor;
				}
			}
	else if (index == 3)
			{
				if (button == 1)
				{
					Pid[index].PozRef = D + Pid[index].OnlineSpeed;
					Pid[index].PositionBuffor = Pid[index].ActPoz;
				}
				else if (button == 2)
				{
					Pid[index].PozRef = D - Pid[index].OnlineSpeed;
					Pid[index].PositionBuffor = Pid[index].ActPoz;
				}
				else if (button == 3)
				{
					Pid[index].PozRef = Pid[index].PositionBuffor;
				}
			}
	else if (index == 4)
			{
				if (button == 1)
				{
					Pid[index].PozRef = E + Pid[index].OnlineSpeed;
					Pid[index].PositionBuffor = Pid[index].ActPoz;
				}
				else if (button == 2)
				{
					Pid[index].PozRef = E - Pid[index].OnlineSpeed;
					Pid[index].PositionBuffor = Pid[index].ActPoz;
				}
				else if (button == 3)
				{
					Pid[index].PozRef = Pid[index].PositionBuffor;
				}
			}
	else if (index == 5)
			{
				if (button == 1)
				{
					Pid[index].PozRef = F + Pid[index].OnlineSpeed;
					Pid[index].PositionBuffor = Pid[index].ActPoz;
				}
				else if (button == 2)
				{
					Pid[index].PozRef = F - Pid[index].OnlineSpeed;
					Pid[index].PositionBuffor = Pid[index].ActPoz;
				}
				else if (button == 3)
				{
					Pid[index].PozRef = Pid[index].PositionBuffor;
				}
			}
	else if (index == 6)
	{
		if (button == 1)
		{
			M7_OPEN;
		}
		else if (button == 2)
		{
			M7_CLOSE;
		}
		else if (button == 3)
		{
			M7_STOP;
		}
	}
}

/*!
* PIDParameters - Function for changing PID gain, maximum answers and deadband.
*/

void PIDParameters(int8_t index, int8_t kp, int8_t ki, int8_t kd,
		int16_t maxerror, int16_t maxerrori, int16_t maxerrord, int16_t maxcv,
		int16_t Deadband)
{
	Pid[index].Kp = kp;
	Pid[index].Ki = ki;
	Pid[index].Kd = kd;
	Pid[index].Maxerror = maxerror;
	Pid[index].MaxErrori = maxerrori;
	Pid[index].MaxErrord = maxerrord;
	Pid[index].MaxCv = maxcv;
	Pid[index].DeadBand = Deadband;
}

/*!
* PIDDefParameters - Initialization function, contain predefined gains, factors for the Pid regulators.
*/

void PIDDefParameters(void)
{
	PIDParameters(0, 55, 3, 180, 600, 100, 700, 300, 0); /*Axis 1*/
	PIDParameters(1, 55, 2, 120, 600, 10, 600, 300, 0);
	PIDParameters(2, 60, 2, 110, 600, 10, 700, 300, 0);
	PIDParameters(3, 60, 10, 110, 600, 200, 700, 300, 0);
	PIDParameters(4, 25, 3, 85, 200, 200, 800, 300, 0);
	PIDParameters(5, 19, 3, 119, 350, 50, 800, 300, 0); /*Axis 6*/
	Pid[0].PozRef = Pid[0].ActPoz = Pid[0].PositionBuffor = A = 5000;
	Pid[1].PozRef = Pid[1].ActPoz = Pid[1].PositionBuffor = B = 5000;
	Pid[2].PozRef = Pid[2].ActPoz = Pid[2].PositionBuffor = C = 5000;
	Pid[3].PozRef = Pid[3].ActPoz = Pid[3].PositionBuffor = D = 5000;
	Pid[4].PozRef = Pid[4].ActPoz = Pid[4].PositionBuffor = E = 5000;
	Pid[5].PozRef = Pid[5].ActPoz = Pid[5].PositionBuffor = F = 5000;
	for (int i = 0; i < 6; i++)
	{
		Pid[i].Enable = 1;
	}
	for (int i = 0; i < 6; i++)
	{
		Pid[i].OnlineSpeed = 120;
	}
}

/*!
* EngineStatus6 - Function for checking if any motor is still running.
*
* 	1 - all engines are in desired position and idle.
* 	0 - some engines are still working.
*/

uint8 EngineStatus6(void)
{
	uint8 Ready1 = 0, Ready2 = 0, Ready3 = 0, Ready4 = 0, Ready5 = 0,
			Ready6 = 0;

	if (Pid[0].AbsError <= Pid[0].DeadBand) /* if actual position error is lower o equal deadband then set flag. By default DeadBad = 0*/
	{
		Ready1 = 1;
	}
	if (Pid[1].AbsError <= Pid[1].DeadBand)
	{
		Ready2 = 1;
	}
	if (Pid[2].AbsError <= Pid[2].DeadBand)
	{
		Ready3 = 1;
	}
	if (Pid[3].AbsError <= Pid[3].DeadBand)
	{
		Ready4 = 1;
	}
	if (Pid[4].AbsError <= Pid[4].DeadBand)
	{
		Ready5 = 1;
	}
	if (Pid[5].AbsError <= Pid[5].DeadBand)
	{
		Ready6 = 1;
	}

	uint8 result = Ready1 + Ready2 + Ready3 + Ready4 + Ready5 + Ready6;

	if (result == VI_ENGINES_IN_POSITION)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*!
* EngineStatus5 - Function for checking if 5 motors are working.
*	Functions is used in calibration sequence because there are
*	only five limit switches mounted in robot.
*
*  	1 - all engines are in desired position and idle.
* 	0 - some engines are still working.
*/

uint8 EngineStatus5(void)
{
	uint8 Ready1 = 0, Ready2 = 0, Ready3 = 0, Ready4 = 0, Ready5 = 0;
	if (Pid[0].AbsError <= Pid[0].DeadBand)
	{
		Ready1 = 1;
	}
	if (Pid[1].AbsError <= Pid[1].DeadBand)
	{
		Ready2 = 1;
	}
	if (Pid[2].AbsError <= Pid[2].DeadBand)
	{
		Ready3 = 1;
	}
	if (Pid[3].AbsError <= Pid[3].DeadBand)
	{
		Ready4 = 1;
	}
	if (Pid[4].AbsError <= Pid[4].DeadBand)
	{
		Ready5 = 1;
	}

	uint8 result = Ready1 + Ready2 + Ready3 + Ready4 + Ready5; /* Flag from 6 engine is excluded because of common limit switch with 5 axis  */
	if (result == V_ENGINES_IN_POSITION)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*!
* Calibration - Function calibrate robot to reference position.
*
* There are 6 limit switches hidden in robot, function drive into them.
*
* 			!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* 			!																	!
* 			! 	It is important to run calibration from specified position		!
* 			! 	detailed in documentation else it may cause damage of robot.	!
* 			!																	!
* 			!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

void Calibration(void)
{
	uint8 i = 1, M1LS = 0, M2LS = 0, M3LS = 0, M4LS = 0, M5LS = 0, suma = 0, Engine5Status = 0;
	uint8 static CalibrationFlag = 0, Engine6Flag = 0, Flag5EnginesReady = 0, Flag6EnginesReady = 0;

	if (CalibrationFlag != TRUE)
	{
		CalibrationFlag = TRUE;	/*Reset CalibrationFlag*/
		for (i=0;i<6;i++)	/*Disable PID regulators*/
		{
			Pid[i].Enable = 0;
		}

		M1_LEFT;	/*Move in desired direction */
		M2_RIGHT;
		M3_RIGHT;
		M4_RIGHT;
		M5_RIGHT;
		TCE0.CCABUF = 312;	/*and with desired speed*/
		TCE0.CCBBUF = 212;
		TCE0.CCCBUF = 212;
		TCE0.CCDBUF = 312;
		TCF0.CCABUF = 312;
		TCF0.CCBBUF = 100;

		for (i=0; i<5; i++)	/*Just in case, set maximum Pid answer*/
		{
			Pid[i].MaxCv = 312;
		}
			Pid[5].MaxCv = 100;
		do	/*Loop will end when all working axis press limit switches*/
		{
			WDT_Reset(); /*Feed dog*/

			if (LIMIT_SWITCH_1 > FALSE)
			{
				M1_STOP;
				M1LS = 1;
				A = 650;
			}
			if (LIMIT_SWITCH_2 > FALSE)
			{
				M2_STOP;
				M2LS = 1;
				B = 500;
			}
			if (LIMIT_SWITCH_3 > FALSE)
			{
				M3_STOP;
				M3LS = 1;
				C = 550;
			}
			if (LIMIT_SWITCH_4 > FALSE)
			{
				M4_STOP;
				M4LS = 1;
				D = 700;
			}
			if (LIMIT_SWITCH_5 > FALSE)
			{
				M5_STOP;
				M5LS = 1;
				E = 450;
			}
			suma = M1LS + M2LS + M3LS + M4LS + M5LS;
		} while (suma != V_ENGINES_IN_POSITION);

		Pid[0].PozRef = Pid[0].ActPoz = Pid[0].PositionBuffor = A; /* Rewrite all position variables in PID regulator*/
		Pid[1].PozRef = Pid[1].ActPoz = Pid[1].PositionBuffor = B;
		Pid[2].PozRef = Pid[2].ActPoz = Pid[2].PositionBuffor = C;
		Pid[3].PozRef = Pid[3].ActPoz = Pid[3].PositionBuffor = D;
		Pid[4].PozRef = Pid[4].ActPoz = Pid[4].PositionBuffor = E;

		Pid[0].PozRef = 500;	/*Set destination*/
		Pid[1].PozRef = 950;
		Pid[2].PozRef = 900;
		Pid[3].PozRef = 500;
		Pid[4].PozRef = 1550;

		for (i=0;i<5;i++)	/* Enable PID regulator */
		{
			Pid[i].Enable = 1;
		}
		Flag5EnginesReady = TRUE; /* set flag to go to next state */
		PID();
	}
	else
	{

	}

	if (Flag5EnginesReady != FALSE)
	{
		Engine5Status = EngineStatus5();
		if (Engine5Status == TRUE) /* If 1-5 engine reach position */
		{
			if (ResetDelayFlag == TRUE)
			{
				RESET_DELAY_VAR
				WaitTime = 100;
			}
			if (DelayFlag == TRUE)
			{
				M6_RIGHT;	/* Start moving axis 6 */
				do	/* loop ends when limit switch is pressed */
				{
					WDT_Reset(); /*Feed dog*/
					if (LIMIT_SWITCH_5 > FALSE)
					{
						M6_STOP;
						F = 3000;
						Engine6Flag = TRUE;
					}
				}while( Engine6Flag != TRUE );
				Pid[5].PozRef = Pid[5].ActPoz = Pid[5].PositionBuffor = F; /* Rewrite all position variables in PID regulator */
				Pid[5].PozRef = 2050; /* Rewrite position */
				Pid[5].Enable = 1;	/* Enable PID */

				Engine6Flag = FALSE;
				ResetDelayFlag = TRUE;
				DelayFlag = FALSE;
				Flag5EnginesReady = FALSE; /* Reset flag */
				Flag6EnginesReady = TRUE;	/* set flag to go to next state */
				PID();
			}
		}
	}
	if (Engine6Status != FALSE && Flag6EnginesReady != FALSE)
	{
		if (ResetDelayFlag == TRUE)
		{
			RESET_DELAY_VAR
			WaitTime = 100;
		}
		if (DelayFlag == TRUE)
		{
			Pid[0].PozRef = Pid[0].ActPoz = Pid[0].PositionBuffor = A = 3500;	/* Rewrite final arms position */
			Pid[1].PozRef = Pid[1].ActPoz = Pid[1].PositionBuffor = B = 1750;
			Pid[2].PozRef = Pid[2].ActPoz = Pid[2].PositionBuffor = C = 5000;
			Pid[3].PozRef = Pid[3].ActPoz = Pid[3].PositionBuffor = D = 3500;
			Pid[4].PozRef = Pid[4].ActPoz = Pid[4].PositionBuffor = E = 3500;
			Pid[5].PozRef = Pid[5].ActPoz = Pid[5].PositionBuffor = F = 2500;

			ResetDelayFlag = TRUE;
			DelayFlag = FALSE;
			Flag6EnginesReady = FALSE; /* Reset flag */
			CalibrationFlag = FALSE;	/* Set flag */
			CalibrationStatus = TRUE; /* Set calibration flag to finish */
			Engine6Status = FALSE;
		}
	}
}

/*!
* FL_Pidparameters - Thesis function
*
* Function for changing parameters of selected PID regulator. It require less data then function PIDParameters.
*/

void FL_Pidparameters(uint8 index, uint8 kp, uint8 ki, uint8 kd)
{
	Pid[index].Kp = kp;
	Pid[index].Ki = ki;
	Pid[index].Kd = kd;
}

/*!
* Vodka - Nothing is done without purpose :).
*
*  Function for filling glass with desired amount of favorite drink.
*  Pour glass until time out or limit switch shortcut. Remember to drink responsible.
*/

void Vodka(void)
{
	if (ResetDelayFlag == TRUE)
	{
		VODKA_ON;	/*Turn on vodka */
		DelayFlag = FALSE;
		RESET_DELAY_VAR;
		WaitTime = VodkaTimeOut;
	}
	if ((DelayFlag == TRUE) || (VODKA_SENSE == 1))
	{
		VODKA_OFF;
		ResetDelayFlag = TRUE;
		DelayFlag = FALSE;
		VodkaStatus = TRUE;
	//	Receive_1 = 0; /* Enable for PcComand 49044*/
	}
}

/*!
* GripperWork - Function for steering gripper
*
*  GripperSpeed - speed of movement (PWM value)
*  GripperForce - demanded closing force 3 - 4095 (ADC value).
* 	1 - idle , 2 - open until limit switch is pressed.
*
* 	Average maximum measured value of ADC is about 3500.
*/

void GripperWork(void)
{
	uint16 MeasuredCurrent = 0;
	uint8 static flag = 0;

	GRIPPER_SPEED = GripperSpeed;

	GripperBusy = TRUE; /*Set flag to prevent executing sequence*/

	if (GripperForce == 2) /*Open gripper*/
	{
		if (LIMIT_SWITCH_6) /*if limit switch is pressed stop gripper*/
		{
			M7_STOP;
			GripperForce = 0;
			ReceiveFlag = 0;
			GripperBusy = FALSE;
			GripperStatus = TRUE;	/* disable for PcComand 49047  */
		//	Receive_1 = 0;	/* enable for PcComand 49047 */
		//	i = 0;	/* enable for PcComand 49047 */
		}
		else
		{
			M7_OPEN;
		}
	}

	if (GripperForce > 2) /* Close gripper with desire force */
	{
		M7_CLOSE;
		MeasuredCurrent = AdcBuf[6];
		if (MeasuredCurrent > GripperForce)
		{
			if (ResetDelayFlag == TRUE)
			{
				RESET_DELAY_VAR
				WaitTime = 100;
			}
			if (DelayFlag == TRUE)
			{
				flag++;	/* If Adc = GripperForce */
				ResetDelayFlag = TRUE;
				DelayFlag = FALSE;
			}
		}
		else
		{
			flag = 0;
		}

		if (flag >= 3)	/* if clutch lasts for 300 ms stop gripper */
		{
			M7_STOP;
			flag = 0;
			ReceiveFlag = 0;
			GripperBusy = FALSE;
			GripperStatus = TRUE;	/* disable for PcComand 49047 */
		//	Receive_1 = 0;	/* enable for PcComand 49047 */
		//	i = 0;	/* enable for PcComand 49047 */
		}
	}
}


/*!
* DmaChangeLength - function for handling UART TX
*
* Function is changing length of data moved by the DMA to UART TX register.
*/

void DmaChangeLength (volatile DMA_CH_t *channel, uint8 length)
{
	if (channel->TRFCNT != length)
	{
		if (DMA_CH_IsPending(channel) == FALSE)
		{
			channel->TRFCNT = length;
		}
		else
		{
			SafeRobot(TRANSMISSION);
			DmaChangeLength(channel, length);
		}
	}
}

/*!
* SendData - function for starting transmission
*
* Function launch DMA TX channel.
*/

void SendData(uint8 length)
{
	static uint8 PreviousLength = 0;

	if (PreviousLength != length)
	{
	DmaChangeLength(&DMA.CH0,length*2);
	}
	DMA_EnableChannel(&DMA.CH0);
	PreviousLength = length;
}

/*!
* Excitation - Thesis function
*
* Main task of this function is to obtain step response of selected axis.
*/

void Excitation(int16 Steps, uint8 Axis, uint8 Time)
{
	static uint8 PozRefSaveFlag = TRUE;
	static uint16 TimeOut;

	if (PozRefSaveFlag != FALSE) /* For first enter to function flag is true */
	{
		Pid[Axis].PozRef += Steps;
		TimeOut = 0;
		PozRefSaveFlag = FALSE;
	}

	if (Axis == AXIS_1)
	{
		TxBuf[0] = A;
		SendData(1);
	}
	else if (Axis == AXIS_2)
	{
		TxBuf[0] = B;
		SendData(1);
	}
	else if (Axis == AXIS_3)
	{
		TxBuf[0] = C;
		SendData(1);
	}
	else if (Axis == AXIS_4)
	{
		TxBuf[0] = D;
		SendData(1);
	}
	else if (Axis == AXIS_5)
	{
		TxBuf[0] = E;
		SendData(1);
	}
	else if (Axis == AXIS_6)
	{
		TxBuf[0] = F;
		SendData(1);
	}
	TimeOut++;

	if (TimeOut > Time) /* When the TimeOut passes */
	{
		PozRefSaveFlag = TRUE;	/*Set flag*/
		ExcitationEnable = 0;	/*Disable excitation*/
		TxBuf[0] = 22;	/*Send information to PC that excitation has ended*/
		SendData(1);
	}
}

/*!
* SafeRobot
*
* Function for prevent robot from damage. ADC measure motors load and if overload is detected robot full stop
* and sign than an error has occupied.
*/

void SafeRobot (uint8 source)
{
	static uint8 Engine = 0;
	static uint16 AdcValue = 0;

	if (source == OVERLOAD)
	{
		AdcValue += AdcBuf[Engine];	/*sum adc value from current selected engine to variable*/

		if (AdcValue > ALARMING_VALUE)	/*if ADC value is higher than a Alarming value, skip changing motor*/
		{
		}
		else
		{
			AdcValue = 0;	/*If ADC value is less than a Alarming value reset variable and select next motor */
			Engine++;
			if (Engine > 6)
			{
				Engine = 0;
			}
		}
		if (AdcValue >= ALARM_THRESHOLD_VALUE) /*If Engine is overloaded for more than a 3 loops sound the alarm by turning
												on buzzer, disabling motors,displaying message on LCD and sending to PC
												error message. In order to exit from a error state, OK button must be press. */
		{
			BUZZER_HORN_ENABLE;
			ERROR_LED_ON;
			TCE0.CCABUF = 0;
			TCE0.CCBBUF = 0;
			TCE0.CCCBUF = 0;
			TCE0.CCDBUF = 0;
			TCF0.CCABUF = 0;
			TCF0.CCBBUF = 0;
			TCF0.CCCBUF = 0;
			LcdClear();
			LcdWrite("OVERLOAED AXIS ");
			LcdDec(Engine+1);
			LedLight(15);
			LcdGoto(0,1);
			LcdWrite("   PRESS - ESC   ");
			LedLight(15);
			for (Engine = 0; Engine<6;Engine++)
			{
				Pid[Engine].Enable = 0;
			}
			AdcValue = 0;
			Engine = 0;
			TxBuf[0] = ERROR_MESSAGE_ADC;
			SendData(1);
			Pid[0].PozRef = Pid[0].ActPoz = Pid[0].PositionBuffor = A; /* Rewrite all position variables in PID regulator*/
			Pid[1].PozRef = Pid[1].ActPoz = Pid[1].PositionBuffor = B;
			Pid[2].PozRef = Pid[2].ActPoz = Pid[2].PositionBuffor = C;
			Pid[3].PozRef = Pid[3].ActPoz = Pid[3].PositionBuffor = D;
			Pid[4].PozRef = Pid[4].ActPoz = Pid[4].PositionBuffor = E;
			Pid[5].PozRef = Pid[5].ActPoz = Pid[5].PositionBuffor = F;

			while (!BUTTON_ESC)
			{
				WDT_Reset();
			};
			BUZZER_HORN_DISABLE;
			ERROR_LED_OFF;
			for (Engine = 0; Engine<6;Engine++)
			{
				Pid[Engine].Enable = 1;
			}
			AnyButtonChange = TRUE;
			Menulvl[0] = 1;
			Menulvl[1] = 0;
		}
	}
	else if (source == TRANSMISSION)
	{
		BUZZER_HORN_ENABLE;
		ERROR_LED_ON;
		TxBuf[0] = ERROR_MESSAGE_TRANSMISSION;
		SendData(1);
		LcdClear();
		LcdWrite("TRANSMISION ERR");
		LedLight(15);
		LcdGoto(0,1);
		LcdWrite("   PRESS - ESC   ");
		for (Engine = 0; Engine<6;Engine++)
		{
			Pid[Engine].Enable = 0;
		}

		while (!BUTTON_ESC)
		{
			WDT_Reset();
		};
		DMA_EnableChannel(&DMA.CH0);
		BUZZER_HORN_DISABLE;
		ERROR_LED_OFF;
		for (Engine = 0; Engine<6;Engine++)
		{
			Pid[Engine].Enable = 1;
		}
		AnyButtonChange = TRUE;
	}
}



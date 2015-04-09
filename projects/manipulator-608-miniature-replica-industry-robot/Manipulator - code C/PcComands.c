/*
 * PcComands.c
 *
 *  Created on: 2 gru 2013
 *      Author: M.R.O.
 */

/**************************
 * --->>  Includes  <<--- *
 **************************/

#include <avr/io.h>
#include <util/delay.h>
#include "Menu.h"
#include "hd44780.h"
#include "usart.h"
#include "AVR1315/eeprom_driver.h"
#include "wdt_driver.h"
#include "Functions.h"
#include "Globals_Defs.h"

/***************************
 * --->>  Functions  <<--- *
 ***************************/

/*!
* ExternalControl - Function contain all executable orders from a external control units.
*
* Full list of orders can be found in documentation. It is possible to control robot from PC and Android device.
*/

void ExternalControl(void)
{
	uint8 static ReceiveCounter = 0, EchoEnable = 0;
	uint8 index = 0, kp = 0, ki = 0, kd = 0;
	uint8 static SkipLoop, NewSequenceFlag = 1, DelaySkipFlag = 0;
	uint16 maxerror = 0, maxerrori = 0, maxerrord = 0, maxcv = 0, Deadband = 0,
			Data = 0;

	if (RxBuf[ReceiveCounter] != 0)
	{
		if (ReceiveFlag == 0)
		{
			Receive_1 = RxBuf[ReceiveCounter];
			RxBuf[ReceiveCounter] = 0;
			if (EchoEnable == TRUE)
			{
				TxBuf[0] = Receive_1;
				SendData(1);
			}
		}
		else
		{
			Receive_2 = RxBuf[ReceiveCounter];
			RxBuf[ReceiveCounter] = 0;
			if (EchoEnable == TRUE)
			{
				TxBuf[0] = Receive_2;
				SendData(1);
			}
		}
	}

	ReceiveCounter++;
	if (ReceiveCounter == RING_BUFFER_LENGTH)
	{
		ReceiveCounter = 0;
	}

	if (Receive_1 == 1) /*Axis 1*/
	{
		Onlinecontrol(0, 1);
		Data = 0;
	}
	if (Receive_1 == 3)
	{
		Onlinecontrol(0, 2);
		Data = 0;
	}
	if (Receive_1 == 2)
	{
		Onlinecontrol(0, 3);
		Receive_1 = 0;
		Data = 0;
	}
	if (Receive_1 == 7001) /*Axis 2*/
	{
		Onlinecontrol(1, 1);
		Data = 0;
	}
	if (Receive_1 == 7002)
	{
		Onlinecontrol(1, 3);
		Receive_1 = 0;
		Data = 0;
	}
	if (Receive_1 == 7003)
	{
		Onlinecontrol(1, 2);
		Data = 0;
	}
	if (Receive_1 == 14001) /*Axis 3*/
	{
		Onlinecontrol(2, 1);
		Data = 0;
	}
	if (Receive_1 == 14002)
	{
		Onlinecontrol(2, 3);
		Receive_1 = 0;
		Data = 0;
	}
	if (Receive_1 == 14003)
	{
		Onlinecontrol(2, 2);
		Data = 0;
	}
	if (Receive_1 == 21001) /*Axis 4*/
	{
		Onlinecontrol(3, 1);
		Data = 0;
	}
	if (Receive_1 == 21002)
	{
		Onlinecontrol(3, 3);
		Receive_1 = 0;
		Data = 0;
	}
	if (Receive_1 == 21003)
	{
		Onlinecontrol(3, 2);
		Data = 0;
	}
	if (Receive_1 == 28001) /*Axis 5*/
	{
		Onlinecontrol(4, 1);
		Data = 0;
	}
	if (Receive_1 == 28002)
	{
		Onlinecontrol(4, 3);
		Receive_1 = 0;
		Data = 0;
	}
	if (Receive_1 == 28003)
	{
		Onlinecontrol(4, 2);
		Data = 0;
	}
	if (Receive_1 == 35001) /*Axis 6*/
	{
		Onlinecontrol(5, 1);
		Data = 0;
	}
	if (Receive_1 == 35002)
	{
		Onlinecontrol(5, 3);
		Receive_1 = 0;
		Data = 0;
	}
	if (Receive_1 == 35003)
	{
		Onlinecontrol(5, 2);
		Data = 0;
	}
	if (LIMIT_SWITCH_6 == 1 && Receive_1 == 48001) /*Griper restrictions*/
	{
		Receive_1 = 0;
		Onlinecontrol(6, 3);
	}
	if (LIMIT_SWITCH_6 == 1 && Receive_1 == 48003)
	{
		Receive_1 = 0;
		Onlinecontrol(6, 3);
	}
	if (Receive_1 == 48001) /*Griper*/
	{
		Onlinecontrol(6, 1);
		Data = 0;
	}
	if (Receive_1 == 48002)
	{
		Onlinecontrol(6, 3);
		Receive_1 = 0;
		Data = 0;
	}
	if (Receive_1 == 48003)
	{
		Onlinecontrol(6, 2);
		Data = 0;
	}
	if (Receive_1 > 600 && Receive_1 < 7000) /*Position setup*/
	{
		Pid[0].PozRef = Receive_1;
		Receive_1 = 0;
	}
	if (Receive_1 > 7003 && Receive_1 < 14000)
	{
		Pid[1].PozRef = Receive_1 - 7000;
		Receive_1 = 0;
	}
	if (Receive_1 > 14003 && Receive_1 < 21000)
	{
		Pid[2].PozRef = Receive_1 - 14000;
		Receive_1 = 0;
	}
	if (Receive_1 > 21003 && Receive_1 < 28000)
	{
		Pid[3].PozRef = Receive_1 - 21000;
		Receive_1 = 0;
	}
	if (Receive_1 > 28003 && Receive_1 < 35000)
	{
		Pid[4].PozRef = Receive_1 - 28000;
		Receive_1 = 0;
	}
	if (Receive_1 > 35003 && Receive_1 < 42000)
	{
		Pid[5].PozRef = Receive_1 - 35000;
		Receive_1 = 0;
	}
	if (Receive_1 > 42001 && Receive_1 < 43000) /*Speed setup*/
	{
		Pid[0].MaxCv = Receive_1 - 42000;
		Receive_1 = 0;
	}
	if (Receive_1 > 43001 && Receive_1 < 44000)
	{
		Pid[1].MaxCv = Receive_1 - 43000;
		Receive_1 = 0;
	}
	if (Receive_1 > 44001 && Receive_1 < 45000)
	{
		Pid[2].MaxCv = Receive_1 - 44000;
		Receive_1 = 0;
	}
	if (Receive_1 > 45001 && Receive_1 < 46000)
	{
		Pid[3].MaxCv = Receive_1 - 45000;
		Receive_1 = 0;
	}
	if (Receive_1 > 46001 && Receive_1 < 47000)
	{
		Pid[4].MaxCv = Receive_1 - 46000;
		Receive_1 = 0;
	}
	if (Receive_1 > 47001 && Receive_1 < 48000)
	{
		Pid[5].MaxCv = Receive_1 - 47000;
		Receive_1 = 0;
	}
	if (Receive_1 > 48003 && Receive_1 < 49000)
	{
		TCF0.CCCBUF = Receive_1 - 48000;
		Receive_1 = 0;
	}
	if (Receive_1 == 49006) /*PID parameters*/
	{
		ReceiveFlag = 1; /*Start receiving data to Receive_2 variable*/

		if (Receive_2 != 0 && i == 0)
		{
			index = Receive_2 - 1;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 1)
		{
			kp = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 2)
		{
			ki = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 3)
		{
			kd = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 4)
		{
			maxerror = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 5)
		{
			maxerrori = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 6)
		{
			maxerrord = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 7)
		{
			maxcv = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 8)
		{
			Deadband = Receive_2 - 1;
			Receive_2 = 0;
			i = 0;
			PIDParameters(index, kp, ki, kd, maxerror, maxerrori, maxerrord,
					maxcv, Deadband);
			ReceiveFlag = 0; /*End receiving data to Recievie_2 variable*/
			Receive_1 = 0;
		}
	}
	if (Receive_1 == 49005) /* Calibration */
	{
		Calibration();
		if (CalibrationStatus !=FALSE)
		{
			Receive_1 = 0;
			CalibrationStatus = 0;
		}
	}
	if (Receive_1 == 49007) /* Send back axis position */
	{
		TxBuf[0] = A;
		SendData(1);
		Receive_1 = 0;
	}
	if (Receive_1 == 49008)
	{
		TxBuf[0] = B;
		SendData(1);
		Receive_1 = 0;
	}
	if (Receive_1 == 49009)
	{
		TxBuf[0] = C;
		SendData(1);
		Receive_1 = 0;
	}
	if (Receive_1 == 49010)
	{
		TxBuf[0] = D;
		SendData(1);
		Receive_1 = 0;
	}
	if (Receive_1 == 49011)
	{
		TxBuf[0] = E;
		SendData(1);
		Receive_1 = 0;
	}
	if (Receive_1 == 49012)
	{
		TxBuf[0] = F;
		SendData(1);
		Receive_1 = 0;
	}
	if (Receive_1 == 49013) /* receive sequences array */
	{
		if (ReceiveFlag == FALSE)
		{
			SequencesQuantity = 0;	/*If new sequence is send reset sequences counter */
		}
		ReceiveFlag = 1; /* Start receiving data to Raceive_2 variable */

		if (Receive_2 == STOP_RECEIVING)
		{
			Receive_1 = 0;
			Receive_2 = 0;
			i = 0;
			ReceiveFlag = 0;
		}
		if (Receive_2 != 0)
		{
			SeqArray[SequencesQuantity][i] = Receive_2;
			i++;

			Receive_2 = 0;
			if (i == 16)
			{
				SequencesQuantity++;
				i = 0;
			}
		}
	}

	if (Receive_1 == 49014) /* "Delete" sequences */
	{
		SequencesQuantity = 0;
		Receive_1 = 0;
	}

	if (Receive_1 == 49015) /*Begin sequence of movement*/
	{
		if (Engine6Status != FALSE)
		{
			if (NewSequenceFlag == TRUE)
			{
				TxBuf[0] = NEW_SEQUENCE_NOTIFICATION;	/*Sending notification to controller that new sequence begun (req. for FL) */
				SendData(1);
				NewSequenceFlag = FALSE;
			}
			Pid[0].PozRef = SeqArray[l][0];
			Pid[0].MaxCv = SeqArray[l][1];
			Pid[1].PozRef = SeqArray[l][2];
			Pid[1].MaxCv = SeqArray[l][3];
			Pid[2].PozRef = SeqArray[l][4];
			Pid[2].MaxCv = SeqArray[l][5];
			Pid[3].PozRef = SeqArray[l][6];
			Pid[3].MaxCv = SeqArray[l][7];
			Pid[4].PozRef = SeqArray[l][8];
			Pid[4].MaxCv = SeqArray[l][9];
			Pid[5].PozRef = SeqArray[l][10];
			Pid[5].MaxCv = SeqArray[l][11];

			if (SeqArray[l][12] > GRIPPER_IDLE)
			{
				if (GripperStatus != TRUE)
					{
						GripperForce = SeqArray[l][12];
						GripperSpeed = SeqArray[l][13];
						GripperWork();
					}
			}
			if (GripperBusy == FALSE)
			{
				if (SeqArray[l][14] >= 2)
				{
					if (VodkaStatus != TRUE)
					{
						Vodka();
					}
				}
				if ((VodkaStatus == TRUE) || (SeqArray[l][14] < 2))
				{
					if (ResetDelayFlag == TRUE)
					{
						RESET_DELAY_VAR
						WaitTime = SeqArray[l][15];
					}
					if (DelayFlag == TRUE)
					{
						l++; /*increment sequence*/
						NewSequenceFlag = TRUE;
						ResetDelayFlag = TRUE;
						DelayFlag = FALSE;
						VodkaStatus = FALSE;
						GripperStatus = FALSE;
					}
				}
			}
		}
		if ((l) == SequencesQuantity)
		{
			Receive_1 = 0;
			l = 0;
		}

	}
	if (Receive_1 >= 49016 && Receive_1 <= 49021) /*Send to PC PiD parameters*/
	{
		TxBuf[0] = Pid[Receive_1 - 49016].Kp;
		TxBuf[1] = Pid[Receive_1 - 49016].Ki;
		TxBuf[2] = Pid[Receive_1 - 49016].Kd;
		TxBuf[3] = Pid[Receive_1 - 49016].Maxerror;
		TxBuf[4] = Pid[Receive_1 - 49016].MaxErrori;
		TxBuf[5] = Pid[Receive_1 - 49016].MaxErrord;
		TxBuf[6] = Pid[Receive_1 - 49016].MaxCv;
		TxBuf[7] = (Pid[Receive_1 - 49016].DeadBand) + 1;
		SendData(8);
		Receive_1 = 0;
	}
	if (Receive_1 == 49022) /*Send to PC actual position and velocity information*/
	{
		TxBuf[0] = A;
		TxBuf[1] = Pid[0].MaxCv;
		TxBuf[2] = B;
		TxBuf[3] = Pid[1].MaxCv;
		TxBuf[4] = C;
		TxBuf[5] = Pid[2].MaxCv;
		TxBuf[6] = D;
		TxBuf[7] = Pid[3].MaxCv;
		TxBuf[8] = E;
		TxBuf[9] = Pid[4].MaxCv;
		TxBuf[10] = F;
		TxBuf[11] = Pid[5].MaxCv;
		TxBuf[12] = 1;
		TxBuf[13] = TCF0.CCCBUF;
		TxBuf[14] = 1;
		TxBuf[15] = 11;
		SendData(16);
		Receive_1 = 0;
	}
	if (Receive_1 == 49023) /*Go back to reference position*/
	{
		Pid[0].PozRef = 3500;
		Pid[1].PozRef = 1750;
		Pid[2].PozRef = 5000;
		Pid[3].PozRef = 3500;
		Pid[4].PozRef = 3500;
		Pid[5].PozRef = 2500;
		Receive_1 = 0;
	}
	if (Receive_1 == 49024) /* Transporting position (on car) */
	{
		Pid[0].PozRef = 4935;
		Pid[0].MaxCv = 600;
		Pid[1].PozRef = 2230;
		Pid[1].MaxCv = 600;
		Pid[2].PozRef = 2880;
		Pid[2].MaxCv = 600;
		Pid[3].PozRef = 3500;
		Pid[3].MaxCv = 600;
		Pid[4].PozRef = 3500;
		Pid[4].MaxCv = 600;
		Pid[5].PozRef = 2500;
		Pid[5].MaxCv = 600;

		if (Engine6Status != FALSE && SkipLoop == CONTINUE_LOOP) /* Skipping one loop to ensure Engine6Status correct value  */
		{
			Pid[1].PozRef = 2875;
			Pid[1].MaxCv = 500;
			Receive_1 = 0;
			SkipLoop = SKIP_LOOP;
		}
		else
		{
			SkipLoop = CONTINUE_LOOP;
		}

	}
	if (Receive_1 == 49025) /* From transport position to 0 */
	{
		Pid[0].PozRef = A = 4935;
		Pid[0].MaxCv = 600;
		Pid[1].PozRef = B = 2875;
		Pid[1].MaxCv = 500;
		Pid[2].PozRef = C = 2880;
		Pid[2].MaxCv = 600;
		Pid[3].PozRef = D = 3500;
		Pid[3].MaxCv = 600;
		Pid[4].PozRef = E = 3500;
		Pid[4].MaxCv = 600;
		Pid[5].PozRef = F = 2500;
		Pid[5].MaxCv = 600;
		Pid[1].PozRef = 2230;
		Pid[1].MaxCv = 600;
		if (ResetDelayFlag == TRUE)
		{
			RESET_DELAY_VAR
			WaitTime = 10;
		}
		if (DelayFlag == TRUE)
		{
			if (Engine6Status == TRUE)
			{
				Pid[0].PozRef = 3500;
				Pid[0].MaxCv = 600;
				Pid[1].PozRef = 1750;
				Pid[1].MaxCv = 500;
				Pid[2].PozRef = 2000;
				Pid[2].MaxCv = 600;
				Pid[3].PozRef = 3500;
				Pid[3].MaxCv = 600;
				Pid[4].PozRef = 3500;
				Pid[4].MaxCv = 600;
				Pid[5].PozRef = 2500;
				Pid[5].MaxCv = 600;
				ResetDelayFlag = TRUE;
				DelayFlag = FALSE;
				Receive_1 = 0;
			}
		}
	}
	if (Receive_1 == 49026) /*Take candy*/
	{
		Pid[0].PozRef = 4930;
		Pid[0].MaxCv = 900;
		if (ResetDelayFlag == TRUE)
		{
			RESET_DELAY_VAR
			WaitTime = 10;
		}
		if (DelayFlag == TRUE)
		{
			if (Engine6Status == TRUE)
			{
				Pid[1].PozRef = 2730;
				Pid[1].MaxCv = 350;
				Pid[2].PozRef = 2402;
				Pid[2].MaxCv = 400;
				Pid[3].PozRef = 3500;
				Pid[3].MaxCv = 600;
				Pid[4].PozRef = 3750;
				Pid[4].MaxCv = 300;
				Pid[5].PozRef = 2500;
				Pid[5].MaxCv = 600;
				ResetDelayFlag = TRUE;
				DelayFlag = FALSE;
				Receive_1 = 0;
			}
		}
	}

	if (Receive_1 == 49027) /*Give candy*/
	{
		Pid[4].PozRef = 3500;
		Pid[4].MaxCv = 200;
		Pid[1].PozRef = 1750;
		Pid[1].MaxCv = 500;
		if (ResetDelayFlag == TRUE && DelaySkipFlag == 1) /*DelaySkipFlag is required because of 2 delays */
		{
			RESET_DELAY_VAR
			WaitTime = 10;
			DelaySkipFlag = 1;
		}
		if (DelayFlag == TRUE && DelaySkipFlag == 1)
		{
			if (Engine6Status == TRUE)
			{
				Pid[0].PozRef = 3500;
				Pid[0].MaxCv = 1000;
				Pid[2].PozRef = 2000;
				Pid[2].MaxCv = 500;
				Pid[3].PozRef = 3500;
				Pid[3].MaxCv = 600;
				Pid[5].PozRef = 2500;
				Pid[5].MaxCv = 300;
				ResetDelayFlag = TRUE;
				DelayFlag = FALSE;
				DelaySkipFlag++;
			}
		}

		if (ResetDelayFlag == TRUE && DelaySkipFlag == 2)
		{
			RESET_DELAY_VAR
			WaitTime = 10;
			DelaySkipFlag = 2;
		}
		if (DelayFlag == TRUE && DelaySkipFlag == 2)
		{
			if (Engine6Status == TRUE)
			{
				Pid[0].PozRef = 3500;
				Pid[0].MaxCv = 600;
				Pid[1].PozRef = 2700;
				Pid[1].MaxCv = 400;
				Pid[2].PozRef = 3400;
				Pid[2].MaxCv = 450;
				Pid[3].PozRef = 3500;
				Pid[3].MaxCv = 600;
				Pid[4].PozRef = 3700;
				Pid[4].MaxCv = 200;
				Pid[5].PozRef = 2500;
				Pid[5].MaxCv = 600;
				Receive_1 = 0;
				ResetDelayFlag = TRUE;
				DelayFlag = FALSE;
				DelaySkipFlag = 0;
			}
		}
	}
	if (Receive_1 == 49028) /*Force axis +*/
	{
		ReceiveFlag = 1; /* Start receiving data to Raceive_2 variable */
		if (Receive_2 != FALSE && i == 0)
		{
			StepsAmount = Receive_2;
			Receive_2 = 0;
			i++;
		}
		if (Receive_2 != FALSE && i == 1)
		{
			SelectedEngine = Receive_2 - 1;
			ExcitationEnable = 1;

			Receive_1 = 0;
			Receive_2 = 0;
			ReceiveFlag = 0;
			i = 0;
		}
	}
	if (Receive_1 == 49029) /*Force axis -*/
	{
		ReceiveFlag = 1;

		if (Receive_2 != FALSE && i == 0)
		{
			StepsAmount = Receive_2;
			StepsAmount = -StepsAmount;
			Receive_2 = 0;
			i++;
		}
		if (Receive_2 != FALSE && i == 1)
		{
			SelectedEngine = Receive_2 - 1;
			ExcitationEnable = 1;

			Receive_2 = 0;
			Receive_1 = 0;
			ReceiveFlag = 0;
			i = 0;
		}
	}
	if (Receive_1 == 49030)
	{
		ReceiveFlag = 1;

		if (Receive_2 != 0 && i == 0)
		{
			index = Receive_2 - 1;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 1)
		{
			kp = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 2)
		{
			ki = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 3)
		{
			kd = Receive_2;
			FL_Pidparameters(index, kp, ki, kd);

			i = 0;
			Receive_2 = 0;
			Receive_1 = 1;
			ReceiveFlag = 0;
		}
	}
	if (Receive_1 == 49032) /* Receive PID parameters from FL */
	{
		ReceiveFlag = 1;

		if (Receive_2 != 0 && i == 0)
		{
			Pid[0].Kp = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 1)
		{
			Pid[0].Ki = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 2)
		{
			Pid[0].Kd = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 3)
		{
			Pid[1].Kp = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 4)
		{
			Pid[1].Ki = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 5)
		{
			Pid[1].Kd = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 6)
		{
			Pid[2].Kp = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 7)
		{
			Pid[2].Ki = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 8)
		{
			Pid[2].Kd = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 9)
		{
			Pid[3].Kp = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 10)
		{
			Pid[3].Ki = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 11)
		{
			Pid[3].Kd = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 12)
		{
			Pid[4].Kp = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 13)
		{
			Pid[4].Ki = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 14)
		{
			Pid[4].Kd = Receive_2;
			Receive_2 = 0;
			Receive_1 = 0;
			i = 0;
			ReceiveFlag = 0;
		}
	}

	if ((Receive_1 >= 49035) && (Receive_1 <= 49040) ) /*Measuring Current*/
	{
		AdcMeasurmentEnableFlag = TRUE;
		AdcSetAxis = Receive_1 - 49035;
		Receive_1 = 0;
	}
	if (Receive_1 == 49041)
	{
		AdcMeasurmentEnableFlag = FALSE;
		Receive_1 = 0;
	}
	if (Receive_1 == 49042)
	{
		VODKA_ON;
		Receive_1 = 0;
	}
	if (Receive_1 == 49043)
	{
		VODKA_OFF;
		Receive_1 = 0;
	}
	if (Receive_1 == 49044)
	{
		Vodka();
	}
	if (Receive_1 == 49045)
	{
		ReceiveFlag = 1;

		if (Receive_2 != FALSE)
		{
			EEPROM_WriteByte(0, 0x08, Receive_2 & 0x00FF);
			EEPROM_WriteByte(0, 0x09, (Receive_2>>8));
			VodkaTimeOut = Receive_2;

			Receive_1 = 0;
			Receive_2 = 0;
			ReceiveFlag = 0;
		}
	}
	if (Receive_1 == 49046)
	{
		TxBuf[0] = VodkaTimeOut;
		SendData(1);
		Receive_1 = 0;
	}

	if (Receive_1 == 49047)
	{
		ReceiveFlag = 1;

		if (Receive_2 != 0 && i == 0)
		{
			GripperForce = Receive_2;
			i++;
			Receive_2 = 0;
		}
		else if (Receive_2 != 0 && i == 1)
		{
			GripperSpeed = Receive_2;
			Receive_2 = 0;
			i++;
		}
		else if (i == 2)
		{
				if (GripperStatus != TRUE)
					{
						GripperWork();
					}
		}
	}

	if (Receive_1 == 49048)
	{
		EchoEnable = TRUE;
	}
	if (Receive_1 == 49049)
	{
		EchoEnable = FALSE;
	}

	if (Receive_1 == 49050)
	{
		for (l = 0; l<SequencesQuantity; l++)
		{
			for(i=0;i<16;i++)
			{
				TxBuf[0] = SeqArray[l][i];
				SendData(1);
			}
		WDT_Reset(); /*Feed dog*/
		}
		Receive_1 = 0;
	}
}

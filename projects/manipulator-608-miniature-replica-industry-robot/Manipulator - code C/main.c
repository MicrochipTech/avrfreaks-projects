/*
 * main.c
 *
 *  Created on: 22 paŸ 2013
 *      Author: M.R.O.
 */

/**************************
 * --->>  Includes  <<--- *
 **************************/

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stddef.h>
#include <avr\pgmspace.h>
#include "Init.h"
#include "Menu.h"
#include "Functions.h"
#include "hd44780.h"
#include "PcComands.h"
#include "Adc.h"
#include "AVR1315/eeprom_driver.h"
#include "dma_driver.h"
#include "wdt_driver.h"
#include "Globals_Defs.h"
#include "usart.h"

/********************************************************************
 *																	*
 * -------------------->>>  MAIN PROGRAM  <<<-----------------------*
 * 																	*
 ********************************************************************/

int main(void)
{
	static uint16 Counter_ExcitationTimeFlag;
	static uint8 Counter_PidFlag;
	static uint16 Counter_AdcTimeFlag;
	static uint16 Counter_SafeFlag;
	static uint8 ExcitationTimeFlag;
	static uint8 PidFlag;
	static uint8 AdcTimeFlag;
	static uint8 SafeFlag;

	PortConfing();
	LcdInit();
	LCDWelcomeTxt();
	LedLight(0x04);

	/* First loop to ensure correct start-up of program. In case of error restart device */

	while (1)
	{

		if (BUTTON_OK)
		{

			/* Peripheral initialization */

			PortConfing();
			LedLight(0x0F);
			setClockTo32MHz();
			UsartInit();
			InterruptConfing();
			AdcConfig();
			PIDDefParameters();
			sei();
			TimerConfing();
			DmaConfig();
			DMA_Enable();
			WDT_EnableAndSetTimeout( WDT_PER_32CLK_gc);

			ControlType = IDLE_CONTROL; /* Set primary control mode */

			VodkaTimeOut = EEPROM_ReadByte(0, 0x08);
			VodkaTimeOut |= EEPROM_ReadByte(0, 0x09)<<8;

			/* MAIN PROGRAM LOOP */

			while (1)
			{
				if (Counter_Value >= 1) /* Time base, 32Mhz/1024 = 31250 Hz */
				{
					Counter_PidFlag++;
					Counter_ExcitationTimeFlag++;
					Counter_AdcTimeFlag++;
					Counter_Flag10ms++;
					Counter_SafeFlag++;
					Counter_Value = 0;
				}

				if (Counter_PidFlag >= 25) /* 32Mhz/1024/25 = 1250 Hz */
				{
					Counter_PidFlag = 0;
					PidFlag = 1;
				}

				if (Counter_ExcitationTimeFlag >= 469) /* 32Mhz/1024/469 ~= 66 Hz */
				{
					Counter_ExcitationTimeFlag = 0;
					ExcitationTimeFlag = 1;
				}

				if (Counter_AdcTimeFlag >= 312) /* 32Mhz/1024/312 = 100Hz */
				{
					Counter_AdcTimeFlag = 0;
					DMA_EnableChannel(&DMA.CH2);
					AdcTimeFlag = 1;
				}

				if (Counter_SafeFlag >= 312) /* 32Mhz/1024/312 = 100Hz */
				{
					Counter_SafeFlag = 0;
					SafeFlag = 1;
				}

				/****************************************************************
				 * 																*
				 * ------------------>SCHEDULED FUNCTIONS<----------------------*
				 * 																*
				 ****************************************************************/

				if (Flag10ms != FALSE) /* Set DelayFlag in desired amount of time */
				{
					if (WaitTime == 0)
					{
						DelayFlag = TRUE;
					}
					WaitTime--;
					Flag10ms = FALSE;
				}

				if (PidFlag != FALSE) /* Flag for Pid calculation, start every 80us */
				{
					PID();
					PidFlag = FALSE;
				}

				if (ExcitationTimeFlag && ExcitationEnable != FALSE) /* Excitation */
				{
					Excitation(StepsAmount, SelectedEngine, 100);
					ExcitationTimeFlag = FALSE;
				}

				if (AdcTimeFlag && AdcMeasurmentEnableFlag != FALSE) /* Adc measurement */
				{
					TxBuf[0] = AdcBuf[AdcSetAxis];
					SendData(1);
					AdcTimeFlag = FALSE;
				}

				if (SafeFlag != FALSE) /* Safe robot function flag */
				{
					SafeRobot(OVERLOAD);	/* Sounding the alarm can take from 30ms to 80ms */
					SafeFlag = FALSE;
				}

				/************************************************************
				 * 															*
				 * --------------->NON-SCHEDULED FUNCTIONS<-----------------*
				 * 														    *
				 ************************************************************/

				Engine6Status = EngineStatus6(); /* Checking if any engine is working 0- operate 1- idle */

				ButtonState(); /* menu navigation */

				if (AnyButtonChange == TRUE) /* AnyButtonChange = true when any button press */
				{
					LCDDisplay();
					AnyButtonChange = FALSE;
				}

				if (ControlType == INTERNAL_CONTROL) /* Robot is controlled via buttons on driver box */
				{
					InternalControl();
				}

				if (ControlType == EXTERNAL_CONTROL) /* Robot is controlled via UART/BT */
				{
					Engine6Status = EngineStatus6();
					ExternalControl();
				}

				WDT_Reset(); /* Feed dog */

			}
		}
	}
}

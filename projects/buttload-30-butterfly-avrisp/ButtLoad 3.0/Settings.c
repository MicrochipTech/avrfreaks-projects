/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#include "Settings.h"

const char    OffText[]                        PROGMEM = "OFF";

const char    USISpeeds[USI_PRESET_SPEEDS][11] PROGMEM = {"1843200 HZ", " 921600 HZ", " 230400 HZ", " 115200 HZ", "  57600 HZ", "  28800 HZ",  "RECOVERY"};
const char    USISpeedVals[]                   PROGMEM = {           0,            0,            1,            1,            2,            3,           3}; // Translate from stored SCK value to nearest AVRStudio SCK value
const char    USISpeedIndex[]                  PROGMEM = {                         1,            2,                          4,            5,           6}; // Translate from recieved AVRStudio SCK value to nearest Buttload SCK value

const char    SPIResetModes[2][6]              PROGMEM = {"LOGIC", "FLOAT"};
const char    StartupModes[3][11]              PROGMEM = {"NORMAL", "PRODUCTION", "AVRISP"};

// ======================================================================================

/*
 NAME:      | SET_SetContrast (static)
 PURPOSE:   | Changes the LCD's contrast to the user's preference
 ARGUMENTS: | None
 RETURNS:   | None
*/
void SET_SetContrast(void)
{
	uint8_t Contrast = (eeprom_read_byte(&EEPROMVars.LCDContrast) & 0x0F); // Ranges from 0-15 so mask retuns 15 (darkest) on blank EEPROM (0xFF)
	char Buffer[6];
	
	JoyStatus = JOY_INVALID;                     // Use an invalid joystick value to force the program to write the
	                                             // name of the default command onto the LCD
	for (;;)
	{
		if (JoyStatus)
		{
			if (JoyStatus & JOY_UP)
			{
				if (Contrast < 15)
				  Contrast++;
			}
			else if (JoyStatus & JOY_DOWN)
			{
				if (Contrast > 1)                // Zero is non-visible, so 1 is the minimum
				  Contrast--;
			}
			else if (JoyStatus & JOY_LEFT)
			{
				eeprom_write_byte(&EEPROMVars.LCDContrast, Contrast);
				return;
			}
					
			Buffer[0] = 'C';
			Buffer[1] = 'T';
			Buffer[2] = ' ';

			MAIN_IntToStr((uint16_t)Contrast, &Buffer[3]);
			LCD_PutStr(Buffer);

			LCD_CONTRAST_LEVEL(Contrast);

			MAIN_WaitForJoyRelease();
		}

		MAIN_MenuSleep();
	}
}

/*
 NAME:      | SET_SetISPSpeed (static)
 PURPOSE:   | Changes the target connection ISP speed to the user's preference
 ARGUMENTS: | None
 RETURNS:   | None
*/
void SET_SetISPSpeed(void)
{
	uint8_t CurrSpeed = eeprom_read_byte(&EEPROMVars.SCKDuration);

	if (CurrSpeed > ARRAY_UPPERBOUND(USISpeeds))
	  CurrSpeed = pgm_read_byte(&USISpeedIndex[0]); // Protection against blank EEPROM

	JoyStatus = JOY_INVALID;                     // Use an invalid joystick value to force the program to write the
	                                             // name of the default command onto the LCD
	for (;;)
	{
		if (JoyStatus)
		{
			if (JoyStatus & JOY_UP)
			{
				(CurrSpeed == 0)? CurrSpeed = ARRAY_UPPERBOUND(USISpeeds) : CurrSpeed--;
			}
			else if (JoyStatus & JOY_DOWN)
			{
				(CurrSpeed == ARRAY_UPPERBOUND(USISpeeds))? CurrSpeed = 0 : CurrSpeed++;
			}
			else if (JoyStatus & JOY_LEFT)
			{
				eeprom_write_byte(&EEPROMVars.SCKDuration, CurrSpeed);
				return;
			}

			LCD_PutStr_f(USISpeeds[CurrSpeed]);

			MAIN_WaitForJoyRelease();
		}

		MAIN_MenuSleep();
	}
}

/*
 NAME:      | SET_SetResetMode (static)
 PURPOSE:   | Changes the target reset line inactive mode (LOGIC or FLOAT) to the user's preference
 ARGUMENTS: | None
 RETURNS:   | None
*/
void SET_SetResetMode(void)
{
	uint8_t CurrMode = (eeprom_read_byte(&EEPROMVars.SPIResetMode) & 0x01);

	JoyStatus = JOY_INVALID;                     // Use an invalid joystick value to force the program to write the
	                                             // name of the default command onto the LCD
	for (;;)
	{
		if (JoyStatus)
		{
			if (JoyStatus & (JOY_UP | JOY_DOWN))
			{
				CurrMode ^= 1;
			}
			else if (JoyStatus & JOY_LEFT)
			{
				eeprom_write_byte(&EEPROMVars.SPIResetMode, CurrMode);
				MAIN_SetTargetResetLine(MAIN_RESET_INACTIVE);
				return;
			}

			LCD_PutStr_f(SPIResetModes[CurrMode]);

			MAIN_WaitForJoyRelease();
		}

		MAIN_MenuSleep();
	}
}

/*
 NAME:      | SET_SetFirmMinorVer (static)
 PURPOSE:   | Changes the returned firmware revision number to the user's preference
 ARGUMENTS: | None
 RETURNS:   | None
*/
void SET_SetFirmMinorVer(void)
{
	uint8_t VerMinor = eeprom_read_byte(&EEPROMVars.FirmVerMinor);
	char    VerBuffer[5];

	if (VerMinor > 19)
	  VerMinor = V2P_SW_VERSION_MINOR_DEFAULT;
	
	VerBuffer[0] = 'V';
	VerBuffer[1] = '2';

	JoyStatus = JOY_INVALID;                     // Use an invalid joystick value to force the program to write the
	                                             // name of the default command onto the LCD
	for (;;)
	{
		if (JoyStatus)
		{
			if (JoyStatus & JOY_UP)
			{
				if (VerMinor < 20)
				  VerMinor++;
			}
			else if (JoyStatus & JOY_DOWN)
			{
				if (VerMinor)
				  VerMinor--;
			}
			else if (JoyStatus & JOY_LEFT)
			{
				eeprom_write_byte(&EEPROMVars.FirmVerMinor, VerMinor);
				return;
			}
			
			MAIN_IntToStr(VerMinor, &VerBuffer[2]);
			VerBuffer[2] = '-';
			LCD_PutStr(VerBuffer);

			MAIN_WaitForJoyRelease();
		}

		MAIN_MenuSleep();
	}	
}

/*
 NAME:      | SET_SetAutoSleepTimeOut (static)
 PURPOSE:   | Changes the length of time of inactivity before entering sleep mode to the user's preference
 ARGUMENTS: | None
 RETURNS:   | None
*/
void SET_SetAutoSleepTimeOut(void)
{
	uint8_t SleepVal = eeprom_read_byte(&EEPROMVars.AutoSleepValIndex);
	char    SleepTxtBuffer[8];

	if (SleepVal > ARRAY_UPPERBOUND(AutoSleepTOValues))
	  SleepVal = ARRAY_UPPERBOUND(AutoSleepTOValues);

	strcpy_P(SleepTxtBuffer, PSTR("    SEC"));
	
	JoyStatus = JOY_INVALID;                     // Use an invalid joystick value to force the program to write the
	                                             // name of the default command onto the LCD
	for (;;)
	{
		if (JoyStatus)
		{
			if (JoyStatus & JOY_UP)
			{
				(SleepVal == 0)? SleepVal = ARRAY_UPPERBOUND(AutoSleepTOValues) : SleepVal--;
			}
			if (JoyStatus & JOY_DOWN)
			{
				(SleepVal == ARRAY_UPPERBOUND(AutoSleepTOValues))? SleepVal = 0 : SleepVal++;
			}
			else if (JoyStatus & JOY_LEFT)
			{
				eeprom_write_byte(&EEPROMVars.AutoSleepValIndex, SleepVal);
				TOUT_SetupSleepTimer();
				return;
			}

			if (!(SleepVal))
			{
				LCD_PutStr_f(OffText);
			}
			else
			{
				MAIN_IntToStr(pgm_read_byte(&AutoSleepTOValues[SleepVal]), SleepTxtBuffer);
				SleepTxtBuffer[3] = ' ';         // Remove the auto-string termination from the buffer that was added by MAIN_IntToStr
				LCD_PutStr(SleepTxtBuffer);
			}

			MAIN_WaitForJoyRelease();
		}

		MAIN_MenuSleep();
	}	
}

/*
 NAME:      | SET_SetToneVol (static)
 PURPOSE:   | Changes the tone volume to the user's preference
 ARGUMENTS: | None
 RETURNS:   | None
*/
void SET_SetToneVol(void)
{
	char VolBuffer[5];

	VolBuffer[0] = 'V';
	
	JoyStatus = JOY_INVALID;                     // Use an invalid joystick value to force the program to write the
	                                             // name of the default command onto the LCD
	for (;;)
	{
		if (JoyStatus)
		{
			uint8_t ToneVolLcl = ToneVol;        // Copy the global to a local variable to save code space

			if (JoyStatus & JOY_UP)
			{
				ToneVolLcl = ((ToneVolLcl == 80)? 0 : (ToneVolLcl + 8));
			}
			else if (JoyStatus & JOY_DOWN)
			{
				ToneVolLcl = ((ToneVolLcl == 0)? 80 : (ToneVolLcl - 8));
			}
			else if (JoyStatus & JOY_LEFT)
			{
				eeprom_write_byte(&EEPROMVars.ToneVolume, ToneVolLcl);
				return;
			}

			ToneVol = ToneVolLcl;                // Copy the local value back into the global

			if (!(ToneVolLcl))
			{
				LCD_PutStr_f(OffText);
			}
			else
			{
				TG_PlayToneSeq(TONEGEN_SEQ_VOLTEST);
				MAIN_IntToStr((ToneVol >> 3), &VolBuffer[1]);
				LCD_PutStr(VolBuffer);				
			}

			MAIN_WaitForJoyRelease();
		}

		MAIN_MenuSleep();
	}	
}

/*
 NAME:      | SET_SetStartupMode (static)
 PURPOSE:   | Changes the startup mode to the user's preference
 ARGUMENTS: | None
 RETURNS:   | None
*/
void SET_SetStartupMode(void)
{
	uint8_t StartupMode = eeprom_read_byte(&EEPROMVars.StartupMode);

	if (StartupMode > ARRAY_UPPERBOUND(StartupModes))
	  StartupMode = 0;

	JoyStatus = JOY_INVALID;                     // Use an invalid joystick value to force the program to write the
	                                             // name of the default command onto the LCD
	for (;;)
	{
		if (JoyStatus)
		{
			if (JoyStatus & JOY_UP)
			{
				(StartupMode == 0)? StartupMode = ARRAY_UPPERBOUND(StartupModes) : StartupMode--;
			}
			else if (JoyStatus & JOY_DOWN)
			{
				(StartupMode == ARRAY_UPPERBOUND(StartupModes))? StartupMode = 0 : StartupMode++;
			}
			else if (JoyStatus & JOY_LEFT)
			{
				eeprom_write_byte(&EEPROMVars.StartupMode, StartupMode);
				return;
			}

			LCD_PutStr_f(StartupModes[StartupMode]);

			MAIN_WaitForJoyRelease();
		}

		MAIN_MenuSleep();
	}	
}

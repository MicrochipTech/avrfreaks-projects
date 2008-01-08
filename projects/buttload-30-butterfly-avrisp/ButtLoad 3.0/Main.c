/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

   Requires AVR-GCC 4.2.2 or above, AVRLibC version 1.5.1 or
 above. Compile in GNU99 standards mode, with optimization -0s.

           Released under the GPL Licence, Version 2.
*/

/*
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.
*/

/*
	FEATURES:
		
		1) Ability to emulate an AVRISP to allow the programming of AVRs with a computer
		   via the SPI interface
		
		2) Ability to store a program (including EEPROM, lockbit and fusebit data) in
		   non-volatile memory and be able to retrieve said program on user request and
		   thus program AVRs via ISP in the field
		
		3) Devices with larger than 16-bit flash addresses (such as the MEGA2560) are supported.



			      Status Colour   | Description
			      ----------------+-------------
			       Green          | Ready
			       Orange         | Busy
			       Red            | Programming
			       Red (Flashing) | Error
*/

/*
	CONNECTIONS:
	
	 * Because the on-board dataflash is connected to the SPI interface, the code uses
	 the USI system in three-wire mode to communicate with the slave AVR instead. This
	 means that the two systems can run at different data rates without switching, and
	 is also nessesary because the slave AVR does not have a /CS pin.
	
		USI Interface
			Pin 1  (SCK) - Slave AVR SCK
			Pin 2  (DI)  - Slave AVR MISO
			Pin 3  (DO)  - Slave AVR MOSI
			Pin 4  (GND) - Slave AVR GND
			
		PORTF (as viewed from the TOP of the board)
			Pin 1  (PF4) - Green (active-high) lead of a Bicolour LED (optional)
			Pin 5  (PF5) - Red (active-high) lead of a Bicolour LED (optional)
			Pin 3  (PF6) - /RESET line of slave AVR
			Pin 9  (PF7) - Recovery mode target clock out
			Pin 10 (GND) - Status LED ground if used (optional)			
			
		USART Interface
			Pin 1        - Recieve relative to Butterfly
			Pin 2        - Transmit relative to Butterfly
			Pin 3        - Ground

	 * Level shifting circuitry must be employed that can translate the 3.3V Butterfly
	 signals to the target AVR's voltage and vice-versa AT SUFFICIENT CURRENT.
*/

/*
	AUTHOR CREDIT:

		FILE                                     |  AUTHOR
		-----------------------------------------+---------------------------------------------
		Analogue.c + Header file                 | By Dean Camera
		Atomic.h                                 | By Dean Camera
		AVRISPCommandBytes.h                     | By Dean Camera
		AVRISPCommandInterpreter.c + Header file | By Dean Camera
		ButtLoadTag.h                            | By Dean Camera
		Dataflash.c + Header file                | By Dean Camera, re-coded from the generic dataflash
		                                         | code by Atmel (ported to GCC by Martin Thomas)
		DataflashCommandBytes.h                  | By Atmel, modified by Martin Thomas
		Delay.h                                  | By Hans-Juergen Heinrichs
		EEPROMVariables.h                        | By Dean Camera
		GlobalMacros.h                           | By Dean Camera
		ISPChipComm.c + Header file              | By Dean Camera
		ISRMacro.h                               | By Dean Camera
		LCD_Driver.c + Header file               | By Dean Camera
		Main.c + Header file                     | By Dean Camera
		OSCCal.c + Header file                   | By Dean Camera, based on sample code by Colin Oflynn
		ProgramManager.c + Header file           | By Dean Camera
		RingBuff.c + Header file                 | By Dean Camera
		SPI.c + Header file                      | By Dean Camera
		Settings.c + Header file                 | By Dean Camera
		StorageManager.c + Header file           | By Dean Camera
		TagManager.c + Header file               | By Dean Camera
		TargetCal.S                              | By Dean Camera
		Timeout.c + Header file                  | By Dean Camera
		ToneGeneration.c + Header file           | By Dean Camera
		USART.c + Header file                    | By Dean Camera
		USI.c + Header file                      | By Dean Camera
		USITransfer.S                            | By Dean Camera with assistance from John Samperi
		V2Protocol.c + Header file               | By Dean Camera
		VirtualAVRMemManager.c + Header file     | By Dean Camera
-------------------------------------------------+-----------------------------------------------------

   Special thanks to Barry, Mike Henning, Nard Awater and Scott Coppersmith of AVRFreaks, for without
   their equipment and wisdom in debugging this monstrocity I'd still be working on it. Also thanks to
   the other members of AVRFreaks for their ideas.

--------------------------------------------------------------------------------------------------------
*/

#define  INC_FROM_MAIN
#include "Main.h"

// PROGRAM TAGS:
BUTTLOADTAG(Title,     "BUTTLOAD AVRISP");
BUTTLOADTAG(Author,    "BY DEAN CAMERA");
BUTTLOADTAG(Version,   VERSION_VSTRING);
BUTTLOADTAG(Copyright, "<C> 2007 - GPL");
BUTTLOADTAG(BTIME,     __TIME__ " " __DATE__);

// PROGMEM CONSTANTS:
const char*   AboutTextPtrs[]                   PROGMEM = {BUTTTAG_Title.TagData, BUTTTAG_Version.TagData, BUTTTAG_Author.TagData, BUTTTAG_Copyright.TagData};

const char    BusyText[]                        PROGMEM = "*BUSY*";

const char    Func_ISPPRGM[]                    PROGMEM = "AVRISP MODE";
const char    Func_STOREPRGM[]                  PROGMEM = "STORE PRGM";
const char    Func_PRGMAVR[]                    PROGMEM = "PROGRAM AVR";
const char    Func_PRGMSTOREINFO[]              PROGMEM = "DATAFLASH STATS";
const char    Func_SETTINGS[]                   PROGMEM = "SETTINGS";
const char    Func_SLEEP[]                      PROGMEM = "SLEEP MODE";

const char*   MainFunctionNames[]               PROGMEM = {Func_ISPPRGM   , Func_STOREPRGM   , Func_PRGMAVR   , Func_PRGMSTOREINFO, Func_SETTINGS      , Func_SLEEP};
const FuncPtr MainFunctionPtrs[]                PROGMEM = {MAIN_AVRISPMode, MAIN_StoreProgram, MAIN_ProgramAVR, MAIN_StorageInfo  , MAIN_ChangeSettings, MAIN_SleepMode};

const char    SFunc_SETCONTRAST[]               PROGMEM = "SET CONTRAST";
const char    SFunc_SETSPISPEED[]               PROGMEM = "SET ISP SPEED";
const char    SFunc_SETRESETMODE[]              PROGMEM = "SET RESET MODE";
const char    SFunc_SETFIRMMINOR[]              PROGMEM = "SET FIRM VERSION";
const char    SFunc_SETAUTOSLEEPTO[]            PROGMEM = "SET SLEEP TIMEOUT";
const char    SFunc_SETTONEVOL[]                PROGMEM = "SET TONE VOLUME";
const char    SFunc_SETSTARTUP[]                PROGMEM = "SET STARTUP MODE";
const char    SFunc_CLEARMEM[]                  PROGMEM = "CLEAR SETTINGS";
const char    SFunc_GOBOOTLOADER[]              PROGMEM = "JUMP TO BOOTLOADER";

const char*   SettingFunctionNames[]            PROGMEM = {SFunc_SETCONTRAST, SFunc_SETSPISPEED, SFunc_SETRESETMODE, SFunc_SETFIRMMINOR , SFunc_SETAUTOSLEEPTO   , SFunc_SETTONEVOL, SFunc_SETSTARTUP  , SFunc_CLEARMEM, SFunc_GOBOOTLOADER};
const FuncPtr SettingFunctionPtrs[]             PROGMEM = {SET_SetContrast  , SET_SetISPSpeed  , SET_SetResetMode  , SET_SetFirmMinorVer, SET_SetAutoSleepTimeOut, SET_SetToneVol  , SET_SetStartupMode, MAIN_ClearMem , MAIN_GoBootloader};

const char    ProgramAVROptions[2][8]           PROGMEM = {"START", "OPTIONS"};
const char    SIFONames[2][15]                  PROGMEM = {"STORAGE SIZES", "VIEW DATA TAGS"};

const uint8_t BitTable[]                        PROGMEM = {(1 << 0), (1 << 1), (1 << 2), (1 << 3), (1 << 4), (1 << 5), (1 << 6), (1 << 7)}; // Lookup table, bitnum to bit mask

// GLOBAL EEPROM VARIABLE STRUCT:
EEPROMVarsType EEPROMVars EEMEM;

// ======================================================================================

/*
 NAME:      | main
 PURPOSE:   | Entry point into program, handles top level menu and driver initialization
 ARGUMENTS: | None
 RETURNS:   | Never
*/
int main(void)
{	
	uint8_t CurrFunc    = 0;
	uint8_t StartupMode = eeprom_read_byte(&EEPROMVars.StartupMode);

	#ifndef DEBUG_JTAGON
		register volatile uint8_t MD asm("r24") = (MCUCR | (1 << JTD)); // Forces compiler to use IN, OR plus two OUTs rather than two lots of IN/AND/OUTs
		MCUCR = MD;                              // Turn on JTAG via code
		MCUCR = MD;
	#endif

	ACSR    = (1 << ACD);                        // Disable the unused Analogue Comparitor to save power
	PRR     = ((1 << PRADC) | (1 << PRSPI) | (1 << PRUSART0)); // Disable subsystems (for now) to save power
	
	DDRF    = ((1 << 4) | (1 << 5) | (1 << 3) | (1 << 7)); // Set status LEDs, VCP and target clock out as outputs, rest as inputs
	DDRB    = ((1 << 0) | (1 << 1) | (1 << 2) | (1 << 5)); // On-board dataflash /CS, ISP MOSI/SCK and beeper as outputs
	PORTB   = ((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3)   // Set SPI pins to high/pullups, and disable dataflash (send /CS high)
	        | JOY_BMASK);                        // \ Turn joystick
	PORTE   = JOY_EMASK;                         // /  pullups on
		  
	PCMSK0  = JOY_EMASK;                         // \.
	PCMSK1  = JOY_BMASK;                         // | Turn on joystick
	EIMSK   = ((1 << PCIE0) | (1 << PCIE1));     // | interrupts
	EIFR    = ((1 << PCIF0) | (1 << PCIF1));     // /

	MAIN_SETSTATUSLED(MAIN_STATLED_ORANGE);
	MAIN_SetTargetResetLine(MAIN_RESET_INACTIVE);

	sei();                                       // Enable interrupts

	LCD_Init();
	LCD_PutStr_f(BusyText);
	
	if ((eeprom_read_word(&EEPROMVars.MagicNumber) != MAGIC_NUM) || (eeprom_read_byte(&EEPROMVars.VersionNumber) != ((VERSION_MAJOR << 4) | VERSION_MINOR)))
	{
		for (uint16_t EAddr = 0; EAddr < sizeof(EEPROMVars); EAddr++) // Clear the EEPROM if first run
		  eeprom_write_byte((uint8_t*)EAddr, 0xFF);
		  
		SPI_SPIInit();
		DF_ENABLEDATAFLASH(TRUE);

		VAMM_EraseAVRMemory(VAMM_ERASE_ERASEDATA);

		DF_ENABLEDATAFLASH(FALSE);
		SPI_SPIOFF();

		eeprom_write_word(&EEPROMVars.MagicNumber, MAGIC_NUM);
		eeprom_write_byte(&EEPROMVars.VersionNumber, ((VERSION_MAJOR << 4) | VERSION_MINOR));
	}
	
	LCD_CONTRAST_LEVEL(eeprom_read_byte(&EEPROMVars.LCDContrast));
	DF_ENABLEDATAFLASH(FALSE);
	OSCCAL_Calibrate();
	TOUT_SetupSleepTimer();
	MAIN_SETSTATUSLED(MAIN_STATLED_GREEN);
	
	TONEGEN_GET_TONE_VOL();
	TG_PlayToneSeq(TONEGEN_SEQ_STARTUP);

	JoyStatus = JOY_INVALID;                     // Use an invalid joystick value to force the program to write the
	                                             // name of the default command onto the LCD

	if (StartupMode == 1)                        // Check if production startup mode
	{
		CurrFunc  = 2;                           // Index for PROGRAM AVR mode	
		LCD_PutStr_f(ProgramAVROptions[0]);      // Show default PROGRAM AVR submenu item
		JoyStatus = JOY_PRESS;                   // Force a joy press to enter the function in the main menu loop
	}
	else if (StartupMode == 2)                   // Check if AVRISP startup mode
	{
		CurrFunc  = 0;                           // Index for AVRISP mode	
		JoyStatus = JOY_PRESS;                   // Force a joy press to enter the function in the main menu loop
	}

	for (;;)
	{
		if (JoyStatus)
		{
			if (JoyStatus & JOY_UP)
			  (CurrFunc == 0)? CurrFunc = ARRAY_UPPERBOUND(MainFunctionPtrs): CurrFunc--;
			else if (JoyStatus & JOY_DOWN)
			  (CurrFunc == ARRAY_UPPERBOUND(MainFunctionPtrs))? CurrFunc = 0 : CurrFunc++;
			else if (JoyStatus & JOY_PRESS)
			  ((FuncPtr)pgm_read_word(&MainFunctionPtrs[CurrFunc]))(); // Run associated function
			else if (JoyStatus & JOY_RIGHT)
			  MAIN_ShowAbout();

			LCD_PutStr_f((char*)pgm_read_word(&MainFunctionNames[CurrFunc]));

			MAIN_WaitForJoyRelease();
		}

		MAIN_MenuSleep();
	}
}

// ======================================================================================

/*
 NAME:      | MAIN_Delay10MS
 PURPOSE:   | Delays for specified blocks of 10 milliseconds
 ARGUMENTS: | Number of blocks of 10ms to delay
 RETURNS:   | None
*/
void MAIN_Delay10MS(uint8_t loops)
{
  /* Prevents the use of floating point libraries. Delaying in groups of
     10ms increases accuracy by reducing the time overhead for each loop
     interation of the while.                                            */

	while (loops--)
	  _delay_ms(10);
}

/*
 NAME:      | MAIN_Delay1MS
 PURPOSE:   | Delays for specified blocks of 1 milliseconds
 ARGUMENTS: | Number of blocks of 1ms to delay
 RETURNS:   | None
*/
void MAIN_Delay1MS(uint8_t loops)
{
  /* Prevents the use of floating point libraries. Less accurate than the
     Delay10MS routine, but nessesary for many commands. The overhead required
     to call the routine is substantially less than the overhead required to
     calculate the float at compile time, so this actually saves execution time. */

	while (loops--)
	  _delay_ms(1);
}

/*
 NAME:      | MAIN_SetTargetResetLine
 PURPOSE:   | Changes the state of the target device's reset line
 ARGUMENTS: | New line state (MAIN_RESET_ACTIVE or MAIN_RESET_INACTIVE)
 RETURNS:   | None
*/
void MAIN_SetTargetResetLine(const uint8_t ActiveInactive)
{
	/* ActiveInactive controls the /Reset line to the target AVR device. If the reset polarity parameter
	   is a 0 then Buttload is interfacing with AT89x devices (which have an active high rather than an
	   active low reset) and this is managed automaticaly. The reset pin is tristated when inactive unless
	   the SPIResetMode option is changed in the OPTIONS mode.                                             */
	
	uint8_t ResetPolarity = eeprom_read_byte(&EEPROMVars.ResetPolarity);
	uint8_t ResetMode     = eeprom_read_byte(&EEPROMVars.SPIResetMode);
	
	switch (ActiveInactive)
	{
		case MAIN_RESET_ACTIVE:                  // The target RESET line may be either active high or low.
			DDRF |= (1 << 6);
		
			if (ResetPolarity)                   // Translate to correct active logic level for target device type
			  PORTF &= ~(1 << 6);
			else
			  PORTF |=  (1 << 6);
		
			break;
		case MAIN_RESET_INACTIVE:                // Must determine what to do for inactive RESET.
			if (ResetMode)                       // FLOAT mode reset
			{
				DDRF  &= ~(1 << 6);

				PORTF &= ~(1 << 6);
			}
			else                                 // ACTIVE mode reset
			{
				DDRF |= (1 << 6);
			
				if (ResetPolarity)               // Translate to correct inactive logic level for target device type
				  PORTF |=  (1 << 6);
				else
				  PORTF &= ~(1 << 6);	
			}
	}
}

/*
 NAME:      | MAIN_WaitForJoyRelease
 PURPOSE:   | Loops until the Butterfly's joystick is released
 ARGUMENTS: | None
 RETURNS:   | None
*/
void MAIN_WaitForJoyRelease(void)
{
	if (JoyStatus == JOY_INVALID)                // If invalid value used to force menu drawing, reset value and exit
	{
		JoyStatus = 0;
		return;
	}

	for (;;)
	{
		while (JoyStatus)                        // Wait until joystick released
		  SLEEPCPU(SLEEP_POWERSAVE);
		  
		MAIN_Delay10MS(2);

		if (!(JoyStatus))                        // Joystick still released (not bouncing), return
		{
			MAIN_Delay10MS(15);
			return;
		}
	}
}

/*
 NAME:      | MAIN_IntToStr
 PURPOSE:   | Converts an integer value to ASCII, with leading zeros
 ARGUMENTS: | Integer value (range 0-999), pointer to destination string buffer
 RETURNS:   | None
*/
void MAIN_IntToStr(uint16_t IntV, char *Buff)
{	
	uint8_t Temp = 0;

	while (IntV >= 100)
	{
		Temp++;
		IntV -= 100;
	}

	Buff[0] = '0' + Temp;
	
	Temp = 0;
	
	while (IntV >= 10)
	{
		Temp++;
		IntV -= 10;
	}
		
	Buff[1] = '0' + Temp;
	Buff[2] = '0' + IntV;
	Buff[3] = '\0';
}

/*
 NAME:      | MAIN_ShowError
 PURPOSE:   | Shows the specified error text to the LCD, prefixed by "E>", and waits for the joystick to be pressed
 ARGUMENTS: | Pointer to error string located in flash memory
 RETURNS:   | None
*/
void MAIN_ShowError(const char *ErrorStr)
{
	char    ErrorBuff[LCD_TEXTBUFFER_SIZE + 3];  // New buffer, LCD text buffer size plus space for the "E>" prefix and null-termination
	uint8_t CurrLedStatus = (MAIN_STATUSLED_PORT & MAIN_STATLED_ORANGE);
	
	ErrorBuff[0] = 'E';
	ErrorBuff[1] = '>';

	strcpy_P(&ErrorBuff[2], ErrorStr);
	
	LCD_PutStr(ErrorBuff);
	MAIN_SETSTATUSLED(MAIN_STATLED_RED);	
	TG_PlayToneSeq(TONEGEN_SEQ_ERROR);
	
	TIMSK1 = (1 << OCIE1A);                      // Enable compare match channel A interrupt
	OCR1A  = TIMEOUT_HZ_TO_COMP(8, TIMEOUT_SRC_CPU, 1024); // Compare rate of 8Hz at 7372800Hz system clock, 1024 prescaler
	TCCR1B = ((1 << WGM12) | (1 << CS12) | (1 << CS10));   // Start timer at Fcpu/1024 speed in CTC mode, flash the red status LED
	
	MAIN_WaitForJoyRelease();
	while (!(JoyStatus & JOY_PRESS)) { SLEEPCPU(SLEEP_IDLE); }; // Wait until center button pushed before continuing
	MAIN_WaitForJoyRelease();

	TCCR1B = 0;
	TIMSK1 = 0;                                  // Turn off compare match interrupt
	MAIN_SETSTATUSLED(CurrLedStatus);            // Restore previous LED status
}

// ======================================================================================

/*
 NAME:      | PCINT0_vect/PCINT1_vect (ISR, non-blocking, aliased)
 PURPOSE:   | ISR to handle the changes of state of the Butterfly's onboard joystick
 ARGUMENTS: | None
 RETURNS:   | None
*/

#if defined(ISR_ALIASOF)
  ISR(PCINT0_vect, ISR_ALIASOF(PCINT1_vect));
#else
  ISR_ALIAS_COMPAT(PCINT0_vect, PCINT1_vect);
#endif
ISR(PCINT1_vect, ISR_NOBLOCK)                    // Joystick routine; PCINT0_vect is bound to this also
{
	JoyStatus = (~PINB & JOY_BMASK)
	          | (~PINE & JOY_EMASK);

	TIMEOUT_SLEEP_TIMEOUT_RESET();
}

/*
 NAME:      | TIMER1_COMPA_vect (ISR, naked)
 PURPOSE:   | ISR to handle the flashing of the red status LED during an error
 ARGUMENTS: | None
 RETURNS:   | None
*/
ISR(TIMER1_COMPA_vect, ISR_NAKED)                // Used for status LED flashing during an error
{
	MAIN_TOGGLESTATUSLED(MAIN_STATLED_RED);      // Compiles down to a SBI - the PIN register toggles a bit on the MEGA169 if the bit is set as an output
	reti();                                      // No register changes, so ISR is naked - RETI must be put here explicitly
}

// ======================================================================================

/*
 NAME:      | MAIN_MenuSleep
 PURPOSE:   | Checks to see if deep sleep (timeout exceeded) is required, else does a light sleep
 ARGUMENTS: | None
 RETURNS:   | None
*/
void MAIN_MenuSleep(void)
{
	if (SecsBeforeAutoSleep && (SleepTimeOutSecs >= SecsBeforeAutoSleep)) // Check to see if timeout has expired
	  MAIN_SleepMode();                          // Deep sleep
	else
	  SLEEPCPU(SLEEP_POWERSAVE);                 // Light sleep while awating input
}

/*
 NAME:      | MAIN_SleepMode
 PURPOSE:   | Powers down the Butterfly into a deep sleep until joystick is pushed upwards
 ARGUMENTS: | None
 RETURNS:   | None
*/
void MAIN_SleepMode(void)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		while (!(LCDCRA & (1<<LCDIF)));          // Wait for new frame
		LCDCRA |= ((1 << LCDIF) | (1 << LCDBL)); // Blank LCD to discharge all segments and clear LCD interrupt flag
		while (!(LCDCRA & (1<<LCDIF)));          // Wait for blanking to be effective
	}

	LCDCRA &= ~(1 << LCDEN);
	PRR    |=  (1 << PRLCD);

	TG_PlayToneSeq(TONEGEN_SEQ_SLEEP);
	MAIN_SETSTATUSLED(MAIN_STATLED_OFF);
	TIMEOUT_SLEEP_TIMER_OFF();

	while (!(JoyStatus & JOY_UP))
	  SLEEPCPU(SLEEP_POWERDOWN);
	   
	TOUT_SetupSleepTimer();
	MAIN_SETSTATUSLED(MAIN_STATLED_GREEN);
	TG_PlayToneSeq(TONEGEN_SEQ_RESUME);

	PRR    &= ~(1 << PRLCD);
	LCDCRA |=  (1 << LCDEN);
	
	LCDCRA &= ~(1 << LCDBL);
	
	MAIN_WaitForJoyRelease();
}

/*
 NAME:      | MAIN_ShowAbout (static)
 PURPOSE:   | Shows the Buttload information strings onto the LCD (scroll via joytick up/down)
 ARGUMENTS: | None
 RETURNS:   | None
*/
static void MAIN_ShowAbout(void)
{
	uint8_t InfoNum = 0;
	
	JoyStatus = JOY_INVALID;                     // Use an invalid joystick value to force the program to write the
	                                             // name of the default command onto the LCD			
	for (;;)
	{
		if (JoyStatus)
		{
			if (JoyStatus & JOY_UP)
			  (InfoNum == 0)? InfoNum = ARRAY_UPPERBOUND(AboutTextPtrs) : InfoNum--;
			else if (JoyStatus & JOY_DOWN)
			  (InfoNum == ARRAY_UPPERBOUND(AboutTextPtrs))? InfoNum = 0 : InfoNum++;
			else if (JoyStatus & JOY_LEFT)
			  return;

			LCD_PutStr_f((char*)pgm_read_word(&AboutTextPtrs[InfoNum]));

			MAIN_WaitForJoyRelease();
		}

		MAIN_MenuSleep();
	}
}

/*
 NAME:      | MAIN_AVRISPMode (static)
 PURPOSE:   | Enter AVRISP mode
 ARGUMENTS: | None
 RETURNS:   | None
*/
static void MAIN_AVRISPMode(void)
{
	USART_Init();
	LCD_PutStr_f(AVRISPModeMessage);
	
	V2P_RunStateMachine(AICI_InterpretPacket);
}

/*
 NAME:      | MAIN_ProgramAVR (static)
 PURPOSE:   | Enter PROGRAM AVR mode
 ARGUMENTS: | None
 RETURNS:   | None
*/
static void MAIN_ProgramAVR(void)
{
	uint8_t ProgMode = 0;
	
	MAIN_WaitForJoyRelease();

	JoyStatus = JOY_INVALID;                     // Use an invalid joystick value to force the program to write the
	                                             // name of the default command onto the LCD
	for (;;)
	{
		if (JoyStatus)
		{
			if (JoyStatus & JOY_PRESS)
			{
				if (ProgMode == 1)
				  PM_ChooseProgAVROpts();
				else
				  PM_StartProgAVR();
			}
			else if (JoyStatus & (JOY_UP | JOY_DOWN))
			{
				ProgMode ^= 1;
			}
			else if (JoyStatus & JOY_LEFT)
			{
				return;
			}

			LCD_PutStr_f(ProgramAVROptions[ProgMode]);

			MAIN_WaitForJoyRelease();
		}

		MAIN_MenuSleep();
	}
}

/*
 NAME:      | MAIN_StoreProgram (static)
 PURPOSE:   | Enter STORE PRGM mode
 ARGUMENTS: | None
 RETURNS:   | None
*/
static void MAIN_StoreProgram(void)
{
	SPI_SPIInit();
	DF_ENABLEDATAFLASH(TRUE);

	if (!(DF_CheckCorrectOnboardChip()))
	  return;
			
	USART_Init();
	LCD_PutStr_f(StorageText);
	V2P_RunStateMachine(SM_InterpretAVRISPPacket);
	
	DF_ENABLEDATAFLASH(FALSE);
	SPI_SPIOFF();
}

/*
 NAME:      | MAIN_ChangeSettings (static)
 PURPOSE:   | Enters a submenu for viewing and changing ButtLoad's settings
 ARGUMENTS: | None
 RETURNS:   | None
*/
static void MAIN_ChangeSettings(void)
{
	uint8_t CurrSFunc = 0;
	
	MAIN_WaitForJoyRelease();

	JoyStatus = JOY_INVALID;                     // Use an invalid joystick value to force the program to write the
	                                             // name of the default command onto the LCD
	for (;;)
	{
		if (JoyStatus)
		{
			if (JoyStatus & JOY_UP)
			  (CurrSFunc == 0)? CurrSFunc = ARRAY_UPPERBOUND(SettingFunctionPtrs) : CurrSFunc--;
			else if (JoyStatus & JOY_DOWN)
			  (CurrSFunc == ARRAY_UPPERBOUND(SettingFunctionPtrs))? CurrSFunc = 0 : CurrSFunc++;
			else if (JoyStatus & JOY_PRESS)
			  ((FuncPtr)pgm_read_word(&SettingFunctionPtrs[CurrSFunc]))();
			else if (JoyStatus & JOY_LEFT)
			  return;

			LCD_PutStr_f((char*)pgm_read_word(&SettingFunctionNames[CurrSFunc]));

			MAIN_WaitForJoyRelease();
		}

		MAIN_MenuSleep();
	}
}

/*
 NAME:      | MAIN_StorageInfo (static)
 PURPOSE:   | Enters a submenu which gives information about the data currently stored in the onboard dataflash
 ARGUMENTS: | None
 RETURNS:   | None
*/
static void MAIN_StorageInfo(void)
{
	uint8_t SelectedItem = 0;

	MAIN_WaitForJoyRelease();

	JoyStatus = JOY_INVALID;                     // Use an invalid joystick value to force the program to write the
	                                             // name of the default command onto the LCD
	for (;;)
	{
		if (JoyStatus)
		{
			if (JoyStatus & (JOY_UP | JOY_DOWN))
			{
				SelectedItem ^= 1;
			}
			else if (JoyStatus & JOY_LEFT)
			{
				return;
			}
			else if (JoyStatus & JOY_PRESS)
			{
				if (SelectedItem == 1)           // View storage tags
				{
					SPI_SPIInit();
					DF_ENABLEDATAFLASH(TRUE);

					if (eeprom_read_byte(&EEPROMVars.StoredData) != TRUE)
					  MAIN_ShowError(PSTR("NO STORED PRGM"));
					else if (DF_CheckCorrectOnboardChip())
					  TM_ShowTags();

					DF_ENABLEDATAFLASH(FALSE);
					SPI_SPIOFF();
				}
				else                             // View stored data sizes
				{
					PM_ShowStoredItemSizes();
				}
			}
			
			LCD_PutStr_f(SIFONames[SelectedItem]);

			MAIN_WaitForJoyRelease();
		}

		MAIN_MenuSleep();
	}
}

/*
 NAME:      | MAIN_ClearMem (static)
 PURPOSE:   | Clears all ButtLoad settings (which change back to defaults)
 ARGUMENTS: | None
 RETURNS:   | None
*/
static void MAIN_ClearMem(void)
{
	LCD_PutStr_f(PSTR("CONFIRM"));
	LCD_WAIT_FOR_SCROLL_DONE();

	LCD_PutStr_f(PSTR("<N Y>"));

	JoyStatus = JOY_INVALID;                     // Use an invalid joystick value to force the program to write the
	                                             // name of the default command onto the LCD	
	for (;;)
	{
		if (JoyStatus)
		{
			if (JoyStatus & JOY_LEFT)
			  return;
			else if (JoyStatus & JOY_RIGHT)
			  break;
		}

		MAIN_MenuSleep();
	}

	MAIN_WaitForJoyRelease();

	LCD_PutStr_f(BusyText);
	MAIN_SETSTATUSLED(MAIN_STATLED_ORANGE);

	for (uint16_t EAddr = 0; EAddr < sizeof(EEPROMVars); EAddr++)
	  eeprom_write_byte((uint8_t*)EAddr, 0xFF);
	
	SPI_SPIInit();
	DF_ENABLEDATAFLASH(TRUE);

	VAMM_EraseAVRMemory(VAMM_ERASE_ERASEDATA);

	DF_ENABLEDATAFLASH(FALSE);
	SPI_SPIOFF();

	eeprom_write_word(&EEPROMVars.MagicNumber, MAGIC_NUM);
	eeprom_write_byte(&EEPROMVars.VersionNumber, ((VERSION_MAJOR << 4) | VERSION_MINOR));
		
	MAIN_SETSTATUSLED(MAIN_STATLED_GREEN);
	LCD_PutStr_f(PSTR("MEM CLEARED"));
	LCD_WAIT_FOR_SCROLL_DONE();
}

/*
 NAME:      | MAIN_GoBootloader (static)
 PURPOSE:   | Enables JTAG when joystick held down, and jumps to bootloader when joystick is released
 ARGUMENTS: | None
 RETURNS:   | None
*/
static void MAIN_GoBootloader(void)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		register volatile uint8_t MD asm("r24") = (MCUCR & ~(1 << JTD)); // Forces compiler to use IN, AND plus two OUTs rather than two lots of IN/AND/OUTs
		MCUCR = MD;                              // Turn on JTAG via code
		MCUCR = MD;
	}

	TIMEOUT_SLEEP_TIMER_OFF();
	
	LCD_PutStr_f(PSTR("*JTAG ON*"));
	
	MAIN_WaitForJoyRelease();

	WDTCR = ((1<<WDCE) | (1<<WDE));              // Enable Watchdog Timer to give reset after minimum timeout

	for (;;)                                     // Eternal loop - when watchdog resets the AVR it will enter the bootloader,
	  SLEEPCPU(SLEEP_POWERSAVE);                 // assuming the BOOTRST fuse is programmed (otherwise app will just restart)
}

// ======================================================================================

#ifdef DEBUG_MEMFILLON
	void MAIN_Util_RAMFill(void)
	{
		/* Debugging aid. Fills ram up with the recognisable constant DC (my initials) on program start.
		   this makes it easier to look for stack overflows and other memory related problems.           */

		extern uint8_t __data_start; // Linker variable - filled at compile time
	
		for (uint8_t* RamLoc = (uint8_t*)&__data_start; RamLoc < (uint8_t*)RAMEND; RamLoc++)
		  *RamLoc = 0xDC;
	}
#endif

#ifdef DEBUG_ISRCATCHALL
	ISR(BADISR_vect, ISR_NAKED)                      // Bad ISR routine; should never be called, here for safety
	{
		SPI_SPIOFF();
		USI_SPIOff();
		USART_OFF();
		TIMEOUT_PACKET_TIMER_OFF();
		TIMEOUT_SLEEP_TIMER_OFF();
			
		for (;;)
		{
			MAIN_ShowError(PSTR("BADISR"));
			SLEEPCPU(SLEEP_POWERSAVE);
		};
	}
#endif

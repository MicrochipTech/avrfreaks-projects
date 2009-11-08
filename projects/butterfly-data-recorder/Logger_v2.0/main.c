//***************************************************************************
//
//  File........: main.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: AVR-GCC 4.1.1; avr-libc 1.4.5
//
//  Description.: AVR Butterfly main module
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20030116 - 1.0  - Created                                       - KS
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas (*)
//  20031204          fixed imcompatibility with sleep-modes        - mt
//  20040218          fixed 'logical and' in calibration            - shc/mt
//  20040827          fixed avr-libc iom169.h compatibility
//  20050726          minor cleanup                                 - mt
//  20070129          update with code from Rev07, using 
//                    avr-gcc 4.1.1/avr-libc 1.4.5 (WinAVR_20070122)- mt
//  20070131          avr-libc sleep-functions/macros               - mt
//  20090718		  retargeted to support data recorder function  - j.landowski
//  20090827		  add setting of record interval to menu        - j.landowski
//
//***************************************************************************

// (*) Martin Thomas, Kaiserslautern, Germany, e-mail: mthomas(at)rhrk.uni-kl.de 
// or eversmith(at)heizung-thomas.de
//
// I'm not working for ATMEL.
// The port is based on REV_06 of the ATMEL-Code (for IAR-C) with fixes/updates
// from REV_07.
// Initially I marked my changes with "// mt" or enclosed them with "// mtA" and 
// "// mtE" but forgot this for some changes esp. during debugging. 'diff' 
// against the original code to see everything that has been changed.


#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/version.h>

#if __AVR_LIBC_VERSION__ < 10405UL
#  warning "avr-libc >= version 1.4.5 recommended"
#  warning "This code has not been tested with older versions."
#endif

#include "main.h"
#include "LCD_functions.h"
#include "LCD_driver.h"
#include "button.h"
#include "RTC.h"
#include "timer0.h"
#include "BCD.h"
#include "usart.h"
#include "ADC.h"
#include "dataflash.h"
#include "record.h"
#include "menu.h"

#define pLCDREG_test (*(char *)(0xEC))

char PowerSaveTimeout  = 30;             // Initial value, enable power save mode after 30 min
BOOL AutoPowerSave     = TRUE;           // Variable to enable/disable the Auto Power Save func
char PowerSave         = FALSE;

unsigned char state;  				 // holds the current state, according to "menu.h"
volatile uint8_t gNewSession = TRUE; // Flag start of new data record session

/*****************************************************************************
*
*   Function name : main
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Contains the main loop of the program
*
*****************************************************************************/

int main(void)
{    

    unsigned char nextstate;
    PGM_P statetext;
    char (*pStateFunc)(char);
    char input;
    uint8_t i, j; 
    char buttons;
    char last_buttons;

    last_buttons='\0';

    // Initial state variables
    state      = ST_AVRBF;
    nextstate  = ST_AVRBF;
    statetext  = MT_AVRBF;
    pStateFunc = NULL;

    // Program initalization
    Initialization();
    sei(); 

    for (;;)  // Main loop
    {
        if(!PowerSave)    // Do not enter main loop in power-save
        {
            // Plain menu text
            if (statetext)
            {
                LCD_puts_f(statetext, 1);
                LCD_Colon(0);
                statetext = NULL;
            }
 
            input = getkey();   // Read buttons
    
            if (pStateFunc)
            {
                // When in this state, we must call the state function
                nextstate = pStateFunc(input);
            }
            else if (input != KEY_NULL)
            {
                // Plain menu, clock the state machine
                nextstate = StateMachine(state, input);
            }
    
            if (nextstate != state)
            {
                state = nextstate;
                for (i=0; (j=pgm_read_byte(&menu_state[i].state)); i++)
                {
                    if (j == state)
                    {
                        statetext =  (PGM_P) pgm_read_word(&menu_state[i].pText);
                        pStateFunc = (PGM_VOID_P) pgm_read_word(&menu_state[i].pFunc);
                        break;
                    }
                }
            }
        }
        
		if( gRecording )  // If in record mode, 
		{
			if( gRecordTimer == gRecordInterval ) // check the counter
			{
				recordData();		// record a data entry
				gRecordTimer = 0;	// reset counter to start next interval
			}
		}

        //enable ATmega169 power save mode if auto powersave
        if(AutoPowerSave)
        {
            if(gPowerSaveTimer >= PowerSaveTimeout)
            {
                state = ST_AVRBF;
                gPowerSaveTimer = 0;
                PowerSave = TRUE;
            }
        }
                
        // Check if the joystick has been in the same position for some time, 
        // then activate auto press of the joystick

        buttons = ((~PINB) & PINB_MASK) | ((~PINE) & PINE_MASK);

        if( buttons != last_buttons ) 
        {
            last_buttons = buttons;
            gAutoPressJoystick = FALSE;
        }
        else if( buttons )
        {
            if( gAutoPressJoystick == TRUE)
            {
                PinChangeInterrupt();
                gAutoPressJoystick = AUTO;
            }
            else    
                gAutoPressJoystick = AUTO;
        }

        if(PowerSave)
            cbiBF(LCDCRA, 7); 		// disable LCD            

        // go to SLEEP
        set_sleep_mode(SLEEP_MODE_PWR_SAVE);
		// The timer0 interrupt wakes sleep up every second allowing one
		// pass thru the main loop then back to sleep, etc...
        sleep_mode();

        if(PowerSave)
        {
            if(!(PINB & (1 << PINB6)))     // press UP to wake from SLEEP
            {
                PowerSave = FALSE;
				if( gRecording )		   // are we in record mode?
				{
					gRecording = FALSE;	   // Stop recording
					memFlush();			   // and write the remaining data to flash
				}
                
                for(i = 0; i < 20; i++ )   // set all LCD segment register
                {						   //  to the variable ucSegments
                    *(&pLCDREG_test + i) = 0x00;
                }
                
                sbiBF(LCDCRA, 7);           // enable LCD
                input = getkey();           // Read buttons
            }
        }

    } //End Main loop

    return 0;
}


/*****************************************************************************
*
*   Function name : StateMachine
*
*   Returns :       nextstate
*
*   Parameters :    state, stimuli
*
*   Purpose :       Shifts between the different states
*
*****************************************************************************/
unsigned char StateMachine(char state, unsigned char stimuli)
{
    unsigned char nextstate = state;    // Default stay in same state
    unsigned char i, j;


    for (i=0; ( j=pgm_read_byte(&menu_nextstate[i].state) ); i++ )
    {
        if ( j == state && pgm_read_byte(&menu_nextstate[i].input) == stimuli)
        {
            // This is the one!
            nextstate = pgm_read_byte(&menu_nextstate[i].nextstate);
            break;
        }
    }

    return nextstate;
}


/*****************************************************************************
*
*   Function name : Initialization
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Initializate the different modules
*
*****************************************************************************/
void Initialization(void)
{
	gRecordTimer = 0;
	gRecording = FALSE;

    OSCCAL_calibration();       // calibrate the OSCCAL byte
        
	// This sets the CPU clock for the application
    CLKPR = (1<<CLKPCE);	// Clock Prescaler Change Enable
    CLKPR = 3;  			// 1 Mhz,	8Mhz/2^3
    //CLKPR = 2;  			// 2 Mhz,	8Mhz/2^2
    //CLKPR = 1;  			// 4 Mhz,	8Mhz/2^1
    //CLKPR = 0;  			// 8 Mhz,	8Mhz/2^0

    ACSR = (1<<ACD);                  // Disable Analog Comparator (power save)
    DIDR0 = (7<<ADC0D); 		      // Disable Digital input on PF0-2 (power save)

    PORTB = (15<<PB0);          // Enable pullup on PB0-PB3
    PORTE = (15<<PE4);			// Enable pullup on PE4-PE7

    sbiBF(DDRB, 5);             // set OC1A as output (piezo device)
    sbiBF(PORTB, 5);            // set OC1A high
            
    Button_Init();              // Initialize pin change interrupt on joystick
    
    RTC_init();                 // Start timer2 asynchronous, used for RTC clock

    Timer0_Init();              // General event timer

    LCD_Init();                 // initialize the LCD

	DF_SPI_init();				// Data flash init

}


/*****************************************************************************
*
*   Function name : RecordFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Start logging data. (Temp, Light, Voltage) 
*                   
*
*****************************************************************************/

char RecordFunc(char input)
{
	static char enter = 1;    
    
    if(enter)
    {
        enter = 0;
        LCD_puts_f(PSTR("Press enter to start"), 1);
    }
    else if(input == KEY_ENTER)
    {
		gRecordTimer = 0;
		recordDataHeader();
		gRecording = TRUE;
        PowerSave = TRUE;
        enter = 1;
        return ST_AVRBF;
    }
    else if (input == KEY_PREV)
    {
        enter = 1;
        return ST_RECORD_START;
    }
        
    return ST_RECORD_START_FUNC;

}

/*****************************************************************************
*
*   Function name : ShowInterval
*
*   Returns :       void
*
*   Parameters :    void
*
*   Purpose :       Displays current record interval (1-99) on LCD 
*                   
*
*****************************************************************************/
void ShowInterval(void)
{
	char PH,PL; 
	
	PH = CHAR2BCD2(gRecordInterval);
	PL = (PH & 0x0F) + '0';
	PH = (PH >> 4) + '0';
	
	LCD_putc(0, PH);
	LCD_putc(1, PL);
	LCD_putc(2, ' ');
	LCD_putc(3, 'S');
	LCD_putc(4, 'e');
	LCD_putc(5, 'c');
	LCD_putc(6, '\0');
	
	LCD_UpdateRequired(TRUE, 0);
}
/*****************************************************************************
*
*   Function name : setIntervalFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Start logging data. (Temp, Light, Voltage) 
*                   
*
*****************************************************************************/

char setIntervalFunc(char input)
{
	static char enter = 1;    
    
    if(enter)
    {
        enter = 0;
        ShowInterval();
    }
    else if(input == KEY_ENTER)
    {
        enter = 1;
        return ST_RECORD_SETINTV;
    }
    else if (input == KEY_PREV)
    {
        enter = 1;
        return ST_RECORD_SETINTV;
    }
	else if (input == KEY_PLUS)
    {
        gRecordInterval++;
         
        if(gRecordInterval > 99)
        {
            gRecordInterval = 1;
        }
		ShowInterval();
    }
    else if (input == KEY_MINUS)
    {
        gRecordInterval--;
         
        if(gRecordInterval < 1)
        {
            gRecordInterval = 99;
        }
		ShowInterval();
    }

    return ST_RECORD_SETINTV_FUNC;

}



/*****************************************************************************
*
*   Function name : RecordFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Start logging data. (Temp, Light, Voltage) 
*                   
*
*****************************************************************************/

char memDumpFunc(char input)
{
	static char enter = 1;    
    
    if(enter)
    {
        enter = 0;
        LCD_puts_f(PSTR("Press enter to start"), 1);
    }
    else if(input == KEY_ENTER)
    {
		LCD_puts_f(PSTR("Dumpn"), 0);
		memDump();
        enter = 1;
        return ST_RECORD_MDUMP;
    }
    else if (input == KEY_PREV)
    {
        enter = 1;
        return ST_RECORD_MDUMP;
    }
        
    return ST_RECORD_MDUMP_FUNC;

}

/*****************************************************************************
*
*   Function name : BootFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Reset the ATmega169 which will cause it to start up in the 
*                   Bootloader-section. (the BOOTRST-fuse must be programmed)
*
*****************************************************************************/

char BootFunc(char input)
{
    static char enter = 1;
    
    if(enter)
    {
        enter = 0;
        LCD_puts_f(PSTR("Jump to bootloader"), 1);
    }
    else if(input == KEY_ENTER)
    {
        WDTCR = (1<<WDCE) | (1<<WDE);   // Enable Watchdog Timer to give reset
        while(1);   					// wait for watchdog-reset, since the BOOTRST-fuse is 
                    					// programmed, the Boot-section will be entered upon reset.
    }
    else if (input == KEY_PREV)
    {
        enter = 1;
        return ST_OPTIONS_BOOT;
    }
    
    return ST_OPTIONS_BOOT_FUNC;
}



/*****************************************************************************
*
*   Function name : PowerSaveFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable power save
*
*****************************************************************************/

char PowerSaveFunc(char input)
{
    static char enter = 1;    
    
    if(enter)
    {
        enter = 0;
        LCD_puts_f(PSTR("Press enter to sleep"), 1);
    }
    else if(input == KEY_ENTER)
    {
        PowerSave = TRUE;
        enter = 1;
        return ST_AVRBF;
    }
    else if (input == KEY_PREV)
    {
        enter = 1;
        return ST_OPTIONS_POWER_SAVE;
    }
        
    return ST_OPTIONS_POWER_SAVE_FUNC;

}


/*****************************************************************************
*
*   Function name : AutoPower
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable/Disable auto power save
*
*****************************************************************************/

void AutoPowerShowMin(void)
{
	char PH,PL; 
	
	PH = CHAR2BCD2(PowerSaveTimeout);
	PL = (PH & 0x0F) + '0';
	PH = (PH >> 4) + '0';
	
	LCD_putc(0, 'M');
	LCD_putc(1, 'I');
	LCD_putc(2, 'N');
	LCD_putc(3, ' ');
	LCD_putc(4, PH);
	LCD_putc(5, PL);
	LCD_putc(6, '\0');
	
	LCD_UpdateRequired(TRUE, 0);
}

char AutoPower(char input)
{
    static char enter = 1;    
    
    if(enter)
    {
        enter = 0;
        
        if(AutoPowerSave)  
            AutoPowerShowMin();
        else
            LCD_puts_f(PSTR("Off"),1);
    }
    else if(input == KEY_ENTER)
    {
         enter = 1;

         return ST_OPTIONS_AUTO_POWER_SAVE;
    }
    else if (input == KEY_PLUS)
    {

        PowerSaveTimeout += 1;
         
        if(PowerSaveTimeout > 90)
        {
            PowerSaveTimeout = 90;
        }
        else
        {    
            AutoPowerSave = TRUE;
            AutoPowerShowMin();
        }
    }
    else if (input == KEY_MINUS)
    {
        if(PowerSaveTimeout)
            PowerSaveTimeout -= 1;

        if(PowerSaveTimeout < 1)
        {
            AutoPowerSave = FALSE;
            PowerSaveTimeout = 0;
            LCD_puts_f(PSTR("Off"),1);
        }
        else
        {   
            AutoPowerSave = TRUE;
            AutoPowerShowMin();
        }
    }
        
    return ST_OPTIONS_AUTO_POWER_SAVE_FUNC;    
}


/*****************************************************************************
*
*   Function name : Delay
*
*   Returns :       None
*
*   Parameters :    unsigned int millisec
*
*   Purpose :       Delay-loop
*
*****************************************************************************/
void Delay(unsigned int millisec)
{

	uint8_t i;

	while (millisec--) {
		for (i=0; i<125; i++) {		// assumes Clk = 1Mhz
			asm volatile ("nop"::);
		}
	}
}


/*****************************************************************************
*
*   Function name : OSCCAL_calibration
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Calibrate the internal OSCCAL byte, using the external 
*                   32,768 kHz crystal as reference
*
*****************************************************************************/
void OSCCAL_calibration(void)
{
    unsigned char calibrate = FALSE;
    int temp;
    unsigned char tempL;

    CLKPR = (1<<CLKPCE);              // set Clock Prescaler Change Enable
    CLKPR = (1<<CLKPS1) | (1<<CLKPS0);// set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
    
    TIMSK2 = 0;             //disable OCIE2A and TOIE2

    ASSR = (1<<AS2);        //select asynchronous operation of timer2 (32,768kHz)
    
    OCR2A = 200;            // set timer2 compare value 

    TIMSK0 = 0;             // delete any interrupt sources
        
    TCCR1B = (1<<CS10);     // start timer1 with no prescaling
    TCCR2A = (1<<CS20);     // start timer2 with no prescaling

    while((ASSR & 0x01) | (ASSR & 0x04));       //wait for TCN2UB and TCR2UB to be cleared

    Delay(1000);    // wait for external crystal to stabilise
    
    while(!calibrate)
    {
        cli(); // mt __disable_interrupt();  // disable global interrupt
        
        TIFR1 = 0xFF;   // delete TIFR1 flags
        TIFR2 = 0xFF;   // delete TIFR2 flags
        
        TCNT1H = 0;     // clear timer1 counter
        TCNT1L = 0;
        TCNT2 = 0;      // clear timer2 counter
           
        while ( !(TIFR2 & (1<<OCF2A)) );   // wait for timer2 compareflag

        TCCR1B = 0; // stop timer1

        sei(); // __enable_interrupt();  // enable global interrupt
    
        if ( (TIFR1 & (1<<TOV1)) )
        {
            temp = 0xFFFF;      // if timer1 overflows, set the temp to 0xFFFF
        }
        else
        {   // read out the timer1 counter value
            tempL = TCNT1L;
            temp = TCNT1H;
            temp = (temp << 8);
            temp += tempL;
        }
    
        if (temp > 6250)
        {
            OSCCAL--;   // the internRC oscillator runs to fast, decrease the OSCCAL
        }
        else if (temp < 6120)
        {
            OSCCAL++;   // the internRC oscillator runs to slow, increase the OSCCAL
        }
        else
            calibrate = TRUE;   // the interRC is correct

        TCCR1B = (1<<CS10); // start timer1
    }
}



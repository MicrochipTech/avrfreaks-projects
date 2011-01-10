//*****************************************************************************
//
//  File........: RTC.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: AVR-GCC 4.1.1; avr-libc 1.4.5
//
//  Description.: Real Time Clock (RTC)
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20021015 - 1.0  - Created                                       - LHM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//  20051107          minior correction (volatiles)                 - mt
//  20070129          SIGNAL->ISR                                   - mt
//*****************************************************************************

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "button.h"
#include "main.h"
#include "RTC.h"
#include "LCD_functions.h"
#include "BCD.h"
#include "LDR.h"

volatile uint8_t  gSECOND;
volatile uint8_t  gMINUTE;
volatile uint8_t  gHOUR;
volatile uint8_t  gDAY;
volatile uint8_t  gMONTH;
volatile uint16_t gYEAR;
volatile uint8_t gPowerSaveTimer = 0;
volatile double gTIME;

// Lookup table holding the length of each mont. The first element is a dummy.
// mt this could be placed in progmem too, but the arrays are accessed quite
//    often - so leaving them in RAM is better...
char MonthLength[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/******************************************************************************
*
*   Function name:  RTC_init
*
*   returns:        none
*
*   parameters:     none
*
*   Purpose:        Start Timer/Counter2 in asynchronous operation using a
*                   32.768kHz crystal.
*
*******************************************************************************/
void RTC_init(void)
{
    Delay(1000);            // wait for 1 sec to let the Xtal stabilize after a power-on,

    cli(); // mt __disable_interrupt();  // disabel global interrupt

    cbiBF(TIMSK2, TOIE2);             // disable OCIE2A and TOIE2

    ASSR = (1<<AS2);        // select asynchronous operation of Timer2

    TCNT2 = 0;              // clear TCNT2A
    TCCR2A |= (1<<CS22) | (1<<CS20);             // select precaler: 32.768 kHz / 128 = 1 sec between each overflow

    while((ASSR & 0x01) | (ASSR & 0x04));       // wait for TCN2UB and TCR2UB to be cleared

    TIFR2 = 0xFF;           // clear interrupt-flags
    sbiBF(TIMSK2, TOIE2);     // enable Timer2 overflow interrupt

    sei(); // mt __enable_interrupt();                 // enable global interrupt

    // initial time and date setting
    gSECOND  = 0;
    gMINUTE  = 0;
    gHOUR    = 16;

    gDAY     = 8;
    gMONTH   = 1;
    gYEAR    = 11;

    NewDay();
}


/*****************************************************************************
*
*   Function name : ShowClock
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Shows the clock on the LCD
*
*****************************************************************************/
char ShowClock(char input)
{
    uint8_t HH, HL, MH, ML, SH, SL;

    HH = CHAR2BCD2(gHOUR);
        
    HL = (HH & 0x0F) + '0';
    HH = (HH >> 4) + '0';

    MH = CHAR2BCD2(gMINUTE);
    ML = (MH & 0x0F) + '0';
    MH = (MH >> 4) + '0';

    SH = CHAR2BCD2(gSECOND);
    SL = (SH & 0x0F) + '0';
    SH = (SH >> 4) + '0';

    LCD_putc(0, HH);
    LCD_putc(1, HL);
    LCD_putc(2, MH);
    LCD_putc(3, ML);
    LCD_putc(4, SH);
    LCD_putc(5, SL);
    LCD_putc(6, '\0');

    LCD_Colon(1);

    LCD_UpdateRequired(TRUE, 0);

    if (input == KEY_PREV)
        return ST_TIME_CLOCK;
    else if (input == KEY_NEXT)
        return ST_TIME_CLOCK_ADJUST;
      
    return ST_TIME_CLOCK_FUNC;
}

#define HOUR       0
#define MINUTE     1
#define SECOND     2


/*****************************************************************************
*
*   Function name : SetClock
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Adjusts the clock
*
*****************************************************************************/
char SetClock(char input)
{
    static char enter_function = 1;
    static uint8_t time[3];
    static uint8_t mode = HOUR;
    uint8_t HH, HL, MH, ML, SH, SL;

    if (enter_function)
    {
        time[HOUR] = gHOUR;
        time[MINUTE] = gMINUTE;
        time[SECOND] = gSECOND;
    }

    HH = CHAR2BCD2(time[HOUR]);
        
    HL = (HH & 0x0F) + '0';
    HH = (HH >> 4) + '0';

    MH = CHAR2BCD2(time[MINUTE]);
    ML = (MH & 0x0F) + '0';
    MH = (MH >> 4) + '0';

    SH = CHAR2BCD2(time[SECOND]);
    SL = (SH & 0x0F) + '0';
    SH = (SH >> 4) + '0';

    LCD_putc(0, HH | ((mode == HOUR) ? 0x80 : 0x00));
    LCD_putc(1, HL | ((mode == HOUR) ? 0x80 : 0x00));
    LCD_putc(2, MH | ((mode == MINUTE) ? 0x80 : 0x00));
    LCD_putc(3, ML | ((mode == MINUTE) ? 0x80 : 0x00));
    LCD_putc(4, SH | ((mode == SECOND) ? 0x80 : 0x00));
    LCD_putc(5, SL | ((mode == SECOND) ? 0x80 : 0x00));
    LCD_putc(6, '\0');

    LCD_Colon(1);

    if (input != KEY_NULL)
        LCD_FlashReset();

    LCD_UpdateRequired(TRUE, 0);
    
    enter_function = 1;

    // Increment/decrement hours, minutes or seconds
    if (input == KEY_PLUS)
        time[mode]++;
    else if (input == KEY_MINUS)
        time[mode]--;
    else if (input == KEY_PREV)
    {
        if (mode == HOUR)
            mode = SECOND;
        else
            mode--;
    }
    else if (input == KEY_NEXT)
    {
        if (mode == SECOND)
            mode = HOUR;
        else
            mode++;
    }
    else if (input == KEY_ENTER)
    {
        // store the temporary adjusted values to the global variables
        cli(); // mt __disable_interrupt();
        gHOUR = time[HOUR];
        gMINUTE = time[MINUTE];
        gSECOND = time[SECOND];
        sei(); // mt __enable_interrupt();
        mode = HOUR;
        return ST_TIME_CLOCK_FUNC;
    }

    /* OPTIMIZE: Can be solved by using a modulo operation */
    if (time[HOUR] == 255)
        time[HOUR] = 23;
    if (time[HOUR] > 23)
        time[HOUR] = 0;

    if (time[MINUTE] == 255)
        time[MINUTE] = 59;
    if (time[MINUTE] > 59)
        time[MINUTE] = 0;

    if (time[SECOND] == 255)
        time[SECOND] = 59;
    if (time[SECOND] > 59)
        time[SECOND] = 0;

    enter_function = 0;
    return ST_TIME_CLOCK_ADJUST_FUNC;
}





/*****************************************************************************
*
*   Function name : ShowDate
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Shows the date on the LCD
*
*****************************************************************************/
char ShowDate(char input)
{
    char YH, YL, MH, ML, DH, DL;

    YH = CHAR2BCD2(gYEAR);
    YL = (YH & 0x0F) + '0';
    YH = (YH >> 4) + '0';

    MH = CHAR2BCD2(gMONTH);
    ML = (MH & 0x0F) + '0';
    MH = (MH >> 4) + '0';

    DH = CHAR2BCD2(gDAY);
    DL = (DH & 0x0F) + '0';
    DH = (DH >> 4) + '0';

	LCD_putc( 0, DH);
    LCD_putc( 1, DL);
    LCD_putc( 2, MH);
    LCD_putc( 3, ML);
    LCD_putc( 4, YH);
    LCD_putc( 5, YL);

    LCD_putc(6, '\0');

    LCD_Colon(1);

    LCD_UpdateRequired(TRUE, 0);


    if (input == KEY_PREV)
        return ST_TIME_DATE;
    else if (input == KEY_NEXT)
        return ST_TIME_DATE_ADJUST;
    else   
        return ST_TIME_DATE_FUNC;
}

#define YEAR        0
#define MONTH       1
#define DAY         2


/*****************************************************************************
*
*   Function name : SetDate
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Adjusts the date
*
*****************************************************************************/
char SetDate(char input)
{
	static char enter_function = 1;
	static uint8_t date[3];    // table holding the temporary date setting
	static uint8_t mode = DAY;
	uint8_t YH, YL, MH, ML, DH, DL;
	uint8_t MonthLength_temp;
	uint8_t LeapMonth;

    if (enter_function)
    {
        date[YEAR] = gYEAR;
        date[MONTH] = gMONTH;
        date[DAY] = gDAY;
    }

    if (mode == YEAR)
    {
        YH = CHAR2BCD2(date[YEAR]);
        YL = (YH & 0x0F) + '0';
        YH = (YH >> 4) + '0';
        
        LCD_putc( 0, ' ');
        LCD_putc( 1, ' ');   
        LCD_putc( 2, 'Y');
        LCD_putc( 3, 'Y');        
        LCD_putc( 4, YH);
        LCD_putc( 5, YL);
    }
    else if (mode == MONTH)
    {
        MH = CHAR2BCD2(date[MONTH]);
        ML = (MH & 0x0F) + '0';
        MH = (MH >> 4) + '0';

        LCD_putc( 0, ' ');
        LCD_putc( 1, ' ');   
        LCD_putc( 2, 'M');
        LCD_putc( 3, 'M');        
        LCD_putc( 4, MH);
        LCD_putc( 5, ML);
    }
    else if (mode == DAY)
    {
        DH = CHAR2BCD2(date[DAY]);
        DL = (DH & 0x0F) + '0';
        DH = (DH >> 4) + '0';

        LCD_putc( 0, ' ');
        LCD_putc( 1, ' ');   
        LCD_putc( 2, 'D');
        LCD_putc( 3, 'D');        
        LCD_putc( 4, DH);
        LCD_putc( 5, DL);
    }

    LCD_putc(6, '\0');

    LCD_Colon(0);

    if (input != KEY_NULL)
        LCD_FlashReset();

    LCD_UpdateRequired(TRUE, 0);


    enter_function = 1;

    // Increment/decrement years, months or days
    if (input == KEY_PLUS)
        date[mode]++;
    else if (input == KEY_MINUS)
        date[mode]--;
    else if (input == KEY_PREV)
    {
        if (mode == YEAR)
            mode = DAY;
        else
            mode--;
    }
    else if (input == KEY_NEXT)
    {
        if (mode == DAY)
            mode = YEAR;
        else
            mode++;
    }
    else if (input == KEY_ENTER)
    {
        // store the temporary adjusted values to the global variables
        cli(); // mt __disable_interrupt();
        gYEAR = date[YEAR];
        gMONTH = date[MONTH];
        gDAY = date[DAY];
        sei(); // mt __enable_interrupt();
        mode = YEAR;
        return ST_TIME_DATE_FUNC;
    }

    /* OPTIMIZE: Can be solved by using a modulo operation */
    if (date[YEAR] == 255)
        date[YEAR] = 99;
    if (date[YEAR] > 99)
        date[YEAR] = 0;

    if (date[MONTH] == 0)
        date[MONTH] = 12;
    if (date[MONTH] > 12)
        date[MONTH] = 1;

    // Check for leap year, if month == February
    if (gMONTH == 2)
        if (!(gYEAR & 0x0003))              // if (gYEAR%4 == 0)
            if (gYEAR%100 == 0)
                if (gYEAR%400 == 0)
                    LeapMonth = 1;
                else
                    LeapMonth = 0;
            else
                LeapMonth = 1;
        else
            LeapMonth = 0;
    else
        LeapMonth = 0;

    if (LeapMonth)
        MonthLength_temp = 29;
    else
        MonthLength_temp = MonthLength[date[MONTH]];
    
    if (date[DAY] == 0)
        date[DAY] = MonthLength_temp;
    if (date[DAY] > MonthLength_temp)
        date[DAY] = 1;

    enter_function = 0;
    
    return ST_TIME_DATE_ADJUST_FUNC;
}


/******************************************************************************
*
*   Timer/Counter2 Overflow Interrupt Routine
*
*   Purpose: Increment the real-time clock
*            The interrupt occurs once a second (running from the 32kHz crystal)
*
*******************************************************************************/
// mtA
// #pragma vector = TIMER2_OVF_vect
// __interrupt void TIMER2_OVF_interrupt(void)
// SIGNAL(SIG_OVERFLOW2)
ISR(TIMER2_OVF_vect)
// mtE
{
    static char LeapMonth;

    gSECOND++;               // increment second

    if (gSECOND == 60)
    {
        gSECOND = 0;
        gMINUTE++;
        
        gPowerSaveTimer++;
        
        if (gMINUTE > 59)
        {
            gMINUTE = 0;
            gHOUR++;
            
            if (gHOUR > 23)
            {
                
                gHOUR = 0;
                gDAY++;

                // Check for leap year if month == February
                if (gMONTH == 2)
                    if (!(gYEAR & 0x0003))              // if (gYEAR%4 == 0)
                        if (gYEAR%100 == 0)
                            if (gYEAR%400 == 0)
                                LeapMonth = 1;
                            else
                                LeapMonth = 0;
                        else
                            LeapMonth = 1;
                    else
                        LeapMonth = 0;
                else
                    LeapMonth = 0;

                // Now, we can check for month length
                if (gDAY > (MonthLength[gMONTH] + LeapMonth))
                {
                    gDAY = 1;
                    gMONTH++;

                    if (gMONTH > 12)
                    {
                        gMONTH = 1;
                        gYEAR++;
                    }
                }

                NewDay();
            }
        }
    }

    gTIME = gHOUR + gMINUTE / 60.0 + gSECOND / 3600.0 ;
    MeasureLight ();
}


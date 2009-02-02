/*****************************************************************************
**   Initial Author(s)...: ATMEL Norway
**   Rewritten for global BCD display buffer (RTC_buffer[])
**   the digits of which provide system-wide timers for:
**   1 second, 10 seconds, 1 minute, 10 minutes, 1 hour, 10 hours
**   Jim Remington sjames_remington at yahoo.com
**   March 2008
******************************************************************************/

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile char RTC_buffer[] = "000000"; 	//BCD clock buffer

void RTC_Init(void);  					// function prototypes
void ShowClock(void);
void Delay(unsigned int millisec);

/*
** Delay(ms)
*/

void Delay(unsigned int millisec)
{
    while (millisec--)
        _delay_ms(1);
}

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

void RTC_Init(void)
{
    Delay(1000);            // wait for 1 sec to let the Xtal stabilize after a power-on,
    cli();					// disable global interrupts
    TIMSK2 &= ~(1<<TOIE2);	// disable Timer 2 interrupts
    ASSR = (1<<AS2);		// select asynchronous operation of Timer2
    TCNT2 = 0;              // clear TCNT2A
    TCCR2A |= (1<<CS22) | (1<<CS20);		// select precaler: 32.768 kHz / 128 = 1 sec between each overflow
    while((ASSR & 0x01) | (ASSR & 0x04));	// wait for TCN2UB and TCR2UB to be cleared
    TIFR2 = 0xFF;			// clear interrupt-flags
    TIMSK2 |= (1<<TOIE2);	// enable Timer2 overflow interrupt
    sei();					// enable global interrupt
}

/*****************************************************************************
*
*   Function name : ShowClock
*
*****************************************************************************/
void ShowClock(void)
{

	LCD_puts(RTC_buffer);
    LCD_ShowColons(true);
}

/*
** handle overflow (1 per second) interrupt from 32kHz watch crystal
*/

ISR (TIMER2_OVF_vect)
{
	uint8_t i;
	char t[6];

	for (i=0;i<6;i++) t[i]=RTC_buffer[i];	//make a copy, so don't change digit more than once on rollover

    t[5]++;		// increment second

    if (t[5] > '9')
	{
		t[5]='0';	// increment ten seconds
		t[4]++;
	 	if ( t[4] > '5')
		{
			t[4]='0';
			t[3]++;			// increment minutes
    		if (t[3] > '9')
    		{
				t[3]='0';
				t[2]++;		// increment ten minutes
        
		 		if (t[2] > '5')
				{
					t[2]='0';
					t[1]++;			// increment hours
 					char b = t[0]; // tens of hours, handle rollover at 19 or 23
	     			if ( ((b < '2') && (t[1] > '9')) || ((b=='2') && (t[1] > '3')) )
		 		   	{
						t[1]='0';
						t[0]++;		// increment ten hours
						if (t[0] > '2') t[0]='0';
    				}
				}
			}
		}
	}
	for (i=0;i<6;i++) RTC_buffer[i]=t[i];  //copy updated time
}

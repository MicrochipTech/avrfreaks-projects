/**
 * timing.c
 *
 * timing tools
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "timing.h"
#include "lcd.h"

#define CLOCK8MHZ

/**
 * System time:
 * interrupt overflow every 0.25 secs,
 * maximum of 34 years
 *
 * starting from 1 allows all the (ticks!=0)-enabled calculations
 */
uint32_t ticks = 1;

/** date/time structure **/
struct time t = {0, 0, 0, 2, 7, 2010};
/** general. system ticks, tick = 1/8 sec. **/

void InitTimer2(void)
{
	/** external signal on TOSC 1 only **/
   ASSR |= (1<<EXCLK);
	ASSR |= (1<<AS2);
   /** 16 overflow per second **/
   TCCR2B = 0x02;
	/** counter reset **/
	TCNT2 = 0;
	/** timer 2 go **/
   TIMSK2 |= (1<<TOIE2);
}

/**
 * check for leap year
 */
static char not_leap(void)
{
	if (!(t.year%100))
		return (char)(t.year%400);
	else
		return (char)(t.year%4);
}

static void IncreaseDateTime (void)
{
	t.second=0;
	if (++t.minute==60)
	{
		t.minute=0;
		if (++t.hour==24)
		{
			t.hour=0;
			switch(++t.day) {
				case 32: // tutti gli altri ne han 31
					t.month++;
					t.day=1;
				break;
				case 31: // 30 giorni ha Novembre con Aprile Giugno e Settembre
					if ((t.month==4) || (t.month==6) || (t.month==9) || (t.month==11))
					{
						t.month++;
						t.day=1;
					}
				break;
				case 30:
					if(t.month==2)
					{
						t.month++;
						t.day=1;
					}
				break;
				case 29: // di 28 c'e' ne uno
					if((t.month==2) && (not_leap()))
					{
						t.month++;
						t.day=1;
					}
				break;
			}
			if (t.month==13)
			{
				t.month=1;
				t.year++;
			}
		}
	}
}

#define SECOND_STEP	0xf

ISR (TIMER2_OVF_vect)
{
	ticks++;

	if ((ticks&0xf)==SECOND_STEP)
	{
		if (++t.second==60) IncreaseDateTime();
	}
}

void usleep(int usec)
{
#ifdef CLOCK8MHZ
	while (usec--)
	{
		__asm__ volatile (
			  "nop	\r\n"
			  "nop	\r\n"
			  "nop	\r\n"
			  "nop	\r\n"
			  "nop	\r\n"
			  "nop	\r\n"
			  "nop	\r\n"
			  "nop	\r\n"
			 );
	}
#endif
}

void msleep(int msec)
{
	while (msec--) usleep(1000);
}

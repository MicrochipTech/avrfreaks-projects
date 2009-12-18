/*



*/
/*********************************************************/
/*             SCHWED-OSEK operating system              */
/*                                                       */
/*   (C) by Joerg Schwedes 2009, All rights reserved     */
/*********************************************************/

#include <os.h>
#include <task.h> /* import of Init Task for Application */

/*********** ATmega8/16 port for SCHWED-OSEK   ************/

/* Configuration for AVR clock frequency = 8 Mhz */ 


/* timebase interrupt */
#define OS_TIMEBASE_VECT       TIMER1_COMPA_vect

#define OS_TIMEBASE_request(x) if(!x){TIFR |= (1<<OCF1A);} else {TIFR &=~(1<<OCF1A);}
#define OS_TIMEBASE_enable(x)  if(x){TIMSK |= (1<<OCIE1A);} else {TIMSK &=~(1<<OCIE1A);}

void OS_FuncInitHook(void)
{
  /*
  Hook for Application and SCHWED-OSEK AVR Port Init.
  Finally the RTOS scheduling will be started.
  */
  /* init of OS Timer interrupt */
  OS_TIMEBASE_request(0);
  OCR1A = 1000; /* next interrupt in 1ms */
  /* Compare match / Pins disconnected */
  /* set prescaler 1/8 and start timer
     (TCCR1B |=1) */
  TCCR1A=0;

  TCCR1B |= 1<<CS11;

  OS_TIMEBASE_enable(1);


  /* Application Init Task */
  TASK_RST(); /* usually defined in Task.c */

  /* now RTOS will start */
  /* RTOS will also enable global interrupts */
}
ISR(OS_TIMEBASE_VECT)
{
  /*
  Time Interrupt for OS RTOS Scheduling
  */
  /* never enable global interrupts here !!!!! */

  /* accurate 1ms interrupt */
  os_u16 temp;
  temp = TCNT1;
  temp -= OCR1A;
  temp = 1000 - temp;
  OCR1A  += temp;
  /* --> ( OCR1A = OCR1A + (1000-(TCNT1-OCR1A)); ) */

  /* faster but not accurate */
  /* OCR1A += 1000;*/   /* next interrupt in 1ms */
  /* call OS Timebase */
  OS_TIMEBASE();


}

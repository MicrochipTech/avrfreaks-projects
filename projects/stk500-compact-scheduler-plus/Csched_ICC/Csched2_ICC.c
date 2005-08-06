/*****************************************************************************
    Author:   Steve Duckworth, with contributions from Andy Gayne.
    Customer: GD Technik internal
    Target:   Originally - Tiny26 (or any other SRAM enabled AVR).
    Project:  Very small task scheduler for Atmel Tiny 26.
    Date:     12/02/2003 - 08/09/2003.
    Notes:    Array based with limit on maximum number of tasks allowed. 
              Demonstration based on STK500 and ATtiny26.
              Connect LEDs to PORTB and switches to PORTA.
    Modified: July 2005 by John Edwards (Johned@Inducedmotion.com)
              converted for Imagecraft ICCAVRV7 and standard 90S8515 and STK500.
              Uses demo STK500 Cable placement for Switches and LED's.
              Also renamed CreateTask and DestroyTask to TaskOn & TaskOff
			  
              Further Modified to incorporate independent timer 
              ticks for two tasks which demonstrates a basic asychronous delay 
			  
              Added the ability for a task (other than task0) to remove itself
              from the list. This means a task can run, set a delay and then
              start another task to run when the delay expires
						  
              ICC Project assumes C;\projects\
*******************************************************************************/
#include <io8515v.h>
#include <stdlib.h>
#include <macros.h>
/*******************************************************************************

    Hardware defines for STK500 with 8515 
	
*******************************************************************************/
#define LED_ON(x)          (PORTB &= ~(1<<x))
#define LED_OFF(x)         (PORTB |=  (1<<x))
#define LED_TOGGLE(x)      (PORTB ^=  (1<<x))
#define TOGGLE_LED(x)      (PORTB ^=  (1<<x))
#define SW(x)              (PIND & (1<<x))
/*******************************************************************************

    Task Scheduler Setup Values
	
*******************************************************************************/
#define maxtasks  8           /* max tasks allowed on queue.                  */

unsigned char tasks = 0;      /* keep count of number of active tasks in list.*/

char restart = 0;             /* flag to restart task execution list          */
                              /* Whenever a task is turned off                */

void(*x[maxtasks+1])(void);   /* task array, with space for null terminator.  */

typedef void(*fp)(void);      /* function pointer typedef                     */
/*******************************************************************************

    Task Global Variables
	
*******************************************************************************/
/*
    Independent delay counters, accessable by all tasks
    ....Delay contains the starting tick delay value  
    ....Count contains the running down count maintained by the timer interrupt
*/
unsigned int NextDelay = 40;  /* Task restart delay when SW5>4>3>5 pushed     */
unsigned int NextCount = 80;   
unsigned int Task6Delay = 10; /* LED 6 toggle delay                           */
unsigned int Task6Count = 10;                     
unsigned int Task7Delay = 10; /* LED 7 toggle delay                           */
unsigned int Task7Count = 10;
/*******************************************************************************
 
    Function prototypes                                                   

*******************************************************************************/
unsigned  char  FindTask      (void(*fp)(void));
void            TaskOn        (void(*task)(void));
void            TaskOff       (void(*fp)(void));
void            task0         (void);
void            task1         (void);
void            task2         (void);
void            task3         (void);
void            task4         (void);
void            task5         (void);
void            task6         (void);
void            task7         (void);
void            timer0_ovf_isr(void);
/*******************************************************************************

    Task Scheduler Main Routines
     operation based on executing tasks from the list with the following code 
     in main: while(x[n]!=NULL) x[n++]();
	 
*******************************************************************************/
/* 
    Scan task list to see if task exists, if not then add task to the end
    then shift NULL pointer to end.                                                 
*/
void  TaskOn(void(*task)(void))
{
  unsigned char n;

  if(!FindTask(task))     /* check task is not present                        */
  {   
    for(n=0;n<maxtasks || x[n]!=NULL ;n++ ) /* find position of null          */
      if (x[n] == NULL)
      {
        x[n] = task;      /* add task to queue                                */
        x[++n] = NULL;    /* add new null terminator                          */
        return;
      }
  } 
}
/*
    Removes task from list if found                                           
*/
void TaskOff(void(*task)(void))
{
  unsigned  char  n;

  for(n=0;n<maxtasks;n++)
    if(x[n] == task)              /* task found.                              */
    {
      do
      {
         x[n] = x[n+1];           /* shuffle tasks down in array              */
         n++;                          
      }
      while(x[n] != NULL);        /* keep shuffle going until NULL            */
      return;
    } 
  restart = 1;  /* removing a task forces execution of the task list to go    */
                /* back to the beginning. This allows tasks other than Task0  */
                /* to arbitrarily turn off other tasks or themselves          */
}
/*
   checks to see if a task already exists on the queue... returns 0 if 
   task not found, else returns array index of task if found.    
*/
unsigned char FindTask(void(*fp)(void))
{
  unsigned  char  n;

  for(n=0;n<maxtasks || x[n] != NULL;n++)
    if(x[n] == fp) return 1;
  return 0;
}
/*******************************************************************************

    Tasks that can be entered in the task list
	
*******************************************************************************/
/*
   Task 0 Enters and Removes task2 from list if sw0 or sw1 pressed respectively
*/
void task0(void)
{
  if (!SW(0))           /* if sw0 pressed                                     */        
  {
    TaskOn(task2);      /* no need to debounce - can be called repeatedly     */
    LED_ON(0);
    LED_OFF(1);
  }
  else if (!SW(1))      /* if sw1 pressed                                     */
  {
    TaskOff(task2);
    LED_OFF(0);
    LED_ON(1);
  }
}
/* 
    if Task2 is entered into the active list, turns led2 on when sw2 pressed  
*/  
void task2(void)
{
  if (!SW(2)) 
  {
    LED_ON(2);
    Task6Delay = 10;        /* Reset blink rates. Since toggle is used the    */
    Task7Delay = 10;        /* LED's may be out of phase                      */
  }
  else LED_OFF(2);
}
void task1(void){}
/*
    Tasks 3, 4 & 5 monitor their switches and if pressed immediately turn off 
	the LED, reset NextDelay, start the next task in the ring and turn 
	themselves off.	
*/
void task3(void)
{
  if (NextCount == 0)            /* Test inside the task implements a delay   */
  {
    LED_ON(3);
    if (!SW(3))
    {
      LED_OFF(3);
	
      NextDelay += 5;            /* Slow it down each time switch pressed     */
      NextCount = NextDelay;     /* Setup Delay for next task                 */
      TaskOn(task5);             /* Turn the next one on                      */
      TaskOff(task3);            /* Turn me off                               */
    }
  }
}
/*
*/
void task4(void)
{
  if (NextCount == 0)            /* Test inside the task implements a delay   */
  {
    LED_ON(4);
    if (!SW(4))
    {
      LED_OFF(4);
	
      NextDelay += 5;            /* Slow it down each time switch pressed     */
      NextCount = NextDelay;     /* Setup Delay for next task                 */
      TaskOn(task3);             /* Turn the next one on                      */
      TaskOff(task4);            /* Turn me off                               */
    }
  }
}
/*
*/
void task5(void)
{
  if (NextCount == 0)            /* Test inside the task implements a delay   */
  {
    LED_ON(5);
    if (!SW(5))
    {
      LED_OFF(5);
      
      NextDelay += 5;            /* Slow it down each time switch pressed     */	
      NextCount = NextDelay;     /* Setup Delay for next task                 */
      TaskOn(task4);             /* Turn the next one on                      */
      TaskOff(task5);            /* Turn me off                               */
    }  
  }
}
/*
    Task6 Controls blinking on LED6
*/
void task6(void)
{
  if (Task6Count == 0)
    {
      Task6Count = Task6Delay;    /* Reset for next delay cycle               */
      TOGGLE_LED(6);              /* Action based on delay                    */
      if (!SW(6)) Task6Delay++;   /* Switch changes the delay value (slower)  */
    }
}
/*
    Task7 Controls blinking on LED7
*/
void task7(void)
{
  if (Task7Count == 0)
    {
      Task7Count = Task7Delay;    /* Reset for next delay cycle               */
      TOGGLE_LED(7);              /* Action based on delay                    */
      if (!SW(7)) Task7Delay++;   /* Switch changes the delay value (slower)  */
    }
}
/*******************************************************************************

    AVR Hardware Initialization Routine
	
*******************************************************************************/
void Init_AVR(void)
{
  CLI();                        /* disable all interrupts                     */
  
  DDRB =  0xff;                 /* PORT B is outputs                          */
  DDRD =  0x00;                 /* PORT D is inputs                           */
  PORTB = 0xff;                 /* all LEDs off                               */
/*  
   TIMER0 initialize - prescale: 1024, Clock: 3.6864Mhz
   actual value: 102.857Hz (2.8% from 100Hz)
*/
  TCCR0 = 0x00;                 /* stop timer0                                */
  TCNT0 = 0xDD;                 /* set count value for 100Hz                  */
  TCCR0 = 0x05;                 /* start timer0 with CK/1024 prescaler        */
  
  MCUCR = 0x00;
  GIMSK = 0x00;
  TIMSK = 0x02;                 /* T0 Overflow Interrupt Enabled (TOIE0)      */
  
  SEI();
}
/*******************************************************************************
    Main initializes ports and task list, then indexes through task list
	executing all entered tasks in sequence 
*******************************************************************************/
void main(void)
{
  unsigned char n = 0;
  
  Init_AVR();

  x[0] = NULL;        /* always NULL the first array element before we use it.*/
 
  TaskOn(task0);      /* add task zero to queue (must have at least one task) */ 
  TaskOn(task5);      /* Starter task for 3 button task shifting              */
  TaskOn(task6);      /* Names and numbers don't have anything to do with     */
  TaskOn(task7);      /* Task order on the list                               */ 
  while(1)            /* loop forever                                         */
  {
    restart = 0;
    while ((x[n]!=NULL)&&(restart == 0)) x[n++]();  /* execute tasks.         */
    n = 0;                        /* reset task list index.                   */
  }
}
/******************************************************************************* 
 
    Timer Interrupt Routine
	
*******************************************************************************/
#pragma interrupt_handler timer0_ovf_isr:8
void timer0_ovf_isr(void)
{
  if (NextCount > 0)  NextCount--;
  if (Task6Count > 0) Task6Count--;   /* Countdown each delay to zero         */
  if (Task7Count > 0) Task7Count--;
   
  TCNT0 = 0xDD;                       /* reload counter value                 */
}

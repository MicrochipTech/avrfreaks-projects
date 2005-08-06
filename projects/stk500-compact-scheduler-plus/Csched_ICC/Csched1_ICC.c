/*****************************************************************************
    Author:   Steve Duckworth, with contributions from Andy Gayne.
    Customer: GD Technik internal
    Target:   Tiny26 (or any other SRAM enabled AVR).
    Project:  Very small task scheduler for Atmel Tiny 26.
    Date:     12/02/2003 - 08/09/2003.
    Notes:    Array based with limit on maximum number of tasks allowed. 
              Demonstration based on STK500 and ATtiny26.
              Connect LEDs to PORTB and switches to PORTA.
    Modified: July 2005 by John Edwards (Johned@Inducedmotion.com)
              converted to an ICCAVRV7 project for standard 8515 and STK500.
              Uses demo STK500 Cable placement for Switches and LED's.
              Also changed CreateTask and DestroyTask to TaskOn & TaskOff
*******************************************************************************/

#include <iom8515v.h>
#include <stdlib.h>
#include <macros.h>

#define LED0_ON            PORTB &= ~0x01
#define LED0_OFF           PORTB |=  0x01
#define LED1_ON            PORTB &= ~0x02
#define LED1_OFF           PORTB |=  0x02
#define LED2_ON            PORTB &= ~0x04
#define LED2_OFF           PORTB |=  0x04
#define LED3_ON            PORTB &= ~0x08
#define LED3_OFF           PORTB |=  0x08

#define SW0               (PIND & 0x01)
#define SW1               (PIND & 0x02)
#define SW2               (PIND & 0x04)
#define SW3               (PIND & 0x08)

#define maxtasks  3         /* max tasks allowed on queue.  */

unsigned char tasks = 0;    /* keep count of number of active tasks.          */
void(*x[maxtasks+1])(void); /* task array, with space for null terminator.    */

typedef void(*fp)(void);    /* function pointer typedef                       */

/* 
    Function prototypes                                                   
*/
unsigned  char  FindTask    (void(*fp)(void));
void            TaskOn      (void(*task)(void));
void            TaskOff     (void(*fp)(void));
void            task0       (void);
void            task1       (void);
void            task2       (void);
void            task3       (void);
void            task4       (void);
void            task5       (void);

/* 
    Scan array to see if task exists, if not then add task to the end
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

/*
   Enters and Removes task1 in list if sw0 or sw1 pressed respectively
*/
void task0(void)
{
  if (!SW0)             /* if sw0 pressed                                     */        
  {
    TaskOn(task1);
    LED0_ON;
    LED1_OFF;
  }
  else if (!SW1)        /* if sw1 pressed                                     */
  {
    TaskOff(task1);
    LED0_OFF;
    LED1_ON;
  }
}

/* 
    if entered into the active task list, turns led3 on when sw3 pressed  
*/            
void task1(void)
{
  if (!SW3) LED3_ON;
  else LED3_OFF;
}

void task2(void){}
void task3(void){}
void task4(void){}
void task5(void){}

/*
    Main initializes ports and task list, then indexes through task list
	executing all entered tasks in sequence 
*/
void main(void)
{
  unsigned char n = 0;

  DDRB = 0xff;                  /* PORT B is outputs                          */
  DDRD = 0x00;                  /* PORT D is inputs                           */
  PORTB = 0xff;                 /* all LEDs off                               */
  
  x[0] = NULL;        /* always NULL the first array element before we use it.*/
 
  TaskOn(task0);      /* add task zero to queue (must have at least one task) */   
 
  while(1)                      /* loop forever                               */
  {
    while(x[n]!=NULL) x[n++](); /* execute tasks.                             */
    n = 0;                      /* reset index.                               */
  }
}

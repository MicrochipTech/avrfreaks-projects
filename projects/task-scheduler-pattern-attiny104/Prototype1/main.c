/* ********** SCHEDULER APP ********** */
/*                                     */
/* Contains the scheduler for the app, */
/* the time base for the system        */
/* and the configuration for the       */
/* processor clock.                    */
/*                                     */
/* Author: Dan Vahle - Secaplius       */
/*                                     */
/* ********** May 10, 2016 *********** */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <stdbool.h>
#include "Schedule.h"
#include "LEDtask.h"
#include "Buttontask.h"
#include "Modetask.h"

/* CONFIGURE THE 8 MHZ PROCESSOR CLOCK */

/* Unlock code for the Configuration Change Protection Register */
#define CCP_UNLOCK_CODE            (0xD8)

/* Clock Prescaler Register divisor of 1 for 8MHz */
#define CLKPSR_DIVISOR_OF_1        (0x00)

/* CONFIGURE THE TIMER FOR OVERFLOW INTERRUPTION - TICK GENERATION */
/* 
 * Load the value for an overflow
 *  (65536 - CLOCK_INTERRUPT_RELOAD) / (7812) for the
 *  overflow interval in seconds for an 8MHz clock source / 1024 (prescaler for timer)
 */
#define CLOCK_INTERRUPT_RELOAD		(65497)   /* 200 Hz timer overflow rate */

/* This variable contains the tick count from the 200 Hz overflow timer */
volatile uint16_t vSystemTime = 0U;

/* TASK SCHEDULER */

/* Main loop forever definition */
#define LOOP_FOREVER			(true)

/* This variable is set to true when a timer overflow occurs */
volatile bool vTickFlag = false;

/* 
 * This variable contains a bit to set for each task 
 * that is scheduled to run. (T_TASK_NAME enumeration)
 * The bit is cleared when task execution is complete. 
 */
volatile uint8_t vTasksToRun = NO_TASKS_TO_RUN;

/* POST ERROR TRAP */

/* Set to true if the application should trap the POST error */
#define TRAP_POST_ERROR       (true)

/* Program Memory addresses and sizes in bytes */
#define PROG_MEM_CHECKSUM_SPACE         (0x03FE)
#define PROG_MEM_SPACE_START_ADDR       (0x4000)
#define PROG_MEM_CHECKSUM_ADDR          (PROG_MEM_CHECKSUM_SPACE + PROG_MEM_SPACE_START_ADDR)

/* Main initialization function */
static void MainInit(void);

/* *** Main Loop ********************* */
int main(void)
{  
     /* Initialization main */
     MainInit();
     
     /* LOOP FOREVER - EXECUTION LOOP FOR THE APPLICATION */
	while(LOOP_FOREVER)
	{
           /* Call the task tick functions for all tasks that need the tick count */
          if(vTickFlag)
          {
               vTickFlag = false;
               LEDTick();
               ButtonTick();
               ModeTick();
               
               /* 
                * Call the Watchdog Reset
                * this will insure a reset occurs if the 
                * loop is delayed by a task that is too long
                * or if the tick timer ISR fails or is too long
                */
               wdt_reset();
          }
          
          /* Check to see if there are tasks scheduled to run */
          else if (vTasksToRun != NO_TASKS_TO_RUN)
          { 
               if (vTasksToRun & LED_TASK)
               {
                    vTasksToRun &= ~(LED_TASK);
                    
                    /* Call the Task function */
                    LEDTask();
               }
               
               if (vTasksToRun & BUTTON_TASK)
               {
                    vTasksToRun &= ~(BUTTON_TASK);
                    
                    /* Call the Task function */
                    ButtonTask();
               }
               
               if (vTasksToRun & MODE_TASK)
               {
                    vTasksToRun &= ~(MODE_TASK);
                    
                    /* Call the Task function */
                    ModeTask();
               }
          }
          
          /* Call the Low Power Function here to rest for the remaining duration of the tick */
          else
          {
               sleep_enable();
               sleep_cpu();
               sleep_disable();
          }               
	}
}

/* *** ScheduleTask ****************** */
void ScheduleTask(T_TASK_NAME task_to_schedule)
{
     cli();
     vTasksToRun |= task_to_schedule;
     sei();
}

/* *** ScheduleTaskISR *************** */
void ScheduleTaskISR(T_TASK_NAME task_to_schedule)
{
     vTasksToRun |= task_to_schedule;
}

/* *** ScheduleTime ****************** */
uint16_t ScheduleTime(void)
{
uint16_t return_value = 0U;

     /* Return the system tick value.
      * Disable interrupts to prevent alteration 
      * of vSystemTime in the ISR while reading 
      */
     cli();
     return_value = vSystemTime;
     sei();
     
     return(return_value);
}

/* *** Timer Overflow Interrupt ****** */
/* ISR                                 */
/* To maintain a constant tick         */
/* Reload TCNT0 of Timer Counter 0     */
/*                                     */
/* Increment the system time counter   */
/* Set the flag variable               */
/* *********************************** */
ISR(TIM0_OVF_vect)
{
	/* Reload the timer counter 0 register for the next overflow event */
	TCNT0 = CLOCK_INTERRUPT_RELOAD;

     /* Increment the system timer count */
     vSystemTime++;

     /* Set the tick flag */
     vTickFlag = true;
}

/* *** Main Initialization function ** */
/*                                     */
/* Called in main right away           */
/* Setup processor clock               */
/* Enable the tick timer               */
/* Call the init for each task         */
/* Enable interrupts                   */
/* *********************************** */

static void MainInit(void)
{
     /* CONFIGURE THE POST FAIL INDICATOR LED */
     
     /* Configure LED pin as output */
     DDRA |= (1<<PINA5);
     
     /* switch on POST fail LED */
     PORTA &= ~(1<<PORTA5);
     
     /* CHECK FOR WATCHDOG RESET CONDITION */
     
     /* Check for the Watchdog Reset flag */
     if (RSTFLR & (1<<WDRF))
     {
          /* 
           * Prevent the application from running if a watchdog reset has occurred
           * The LED will flash here from a continuous Watchdog Reset cycle
           * If a solid LED is desired, disable the watchdog before the while loop
           */
          while(TRAP_POST_ERROR);
     }
     
     /* CONFIGURE THE 8 MHZ PROCESSOR CLOCK */
     
	/* Unlock the protection for the clock registers - No division of the clock for 8MHz */
	CCP = CCP_UNLOCK_CODE;
     /* Load the Clock prescaler register */
	CLKPSR = CLKPSR_DIVISOR_OF_1;
     
     /* VERIFY THE INTEGRITY OF FLASH MEMORY */
     
     uint16_t calc_checksum = 0x0000;
     uint16_t* read_checksum;
     volatile uint8_t *test_byte;
     uint16_t index = 0x0000;
     
     read_checksum = (uint16_t*)(PROG_MEM_CHECKSUM_ADDR);
     test_byte = (uint8_t*)(PROG_MEM_SPACE_START_ADDR);
     
     /* 
      * Calculate the checksum from flash memory - exclude the 
      * last two bytes where the hex file checksum is stored 
      */
     for (index = 0x0000; index < PROG_MEM_CHECKSUM_SPACE; index++)
     {
          calc_checksum += (uint16_t) (*test_byte);
          test_byte++;
     }
     
     /* Compare the calculated checksum with the hex file checksum */          
     if (calc_checksum != *read_checksum)
     {
          /* Prevent the application from running if checksum is invalid */
          while(TRAP_POST_ERROR);
     }
     
     /* The POST has passed so switch off POST fail LED */
     PORTA |= (1<<PORTA5);  
     
     /* CONFIGURE THE TIMER FOR OVERFLOW INTERRUPTION */
     
	/* Load the value for an overflow in the Timer/Counter 0 register */
	TCNT0 = CLOCK_INTERRUPT_RELOAD;
	/* Enable the interrupt for the overflow in the Timer/Counter0 Interrupt Mask register */
	TIMSK0 |= (1<<TOIE0);
	/* Set the prescaler for the timer to the system clock divided by 1024 or 8 MHz / 1024 or 7812 counts per second */
	TCCR0B |= ((1<<CS00) + (1<<CS02));

     /* CALL THE INITIALIZATION FOR ALL OF THE TASKS */
     
     LEDtaskInit();
     ButtontaskInit();
	
	/* ENABLE INTERRUPTS */
     
	sei();
     
     /* ENABLE THE WATCHDOG */
     
     /* Enable for the shortest period of 16 mS */
     WDTCSR |= (1<<WDE);
}

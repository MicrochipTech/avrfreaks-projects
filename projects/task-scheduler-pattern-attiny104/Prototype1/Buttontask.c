/* ******* Buttontask **************** */
/*                                     */
/* Contains the code for the           */
/*     Button task                     */
/*                                     */
/* Author: Dan Vahle - Secaplius       */
/*                                     */
/* ********** May 10, 2016 *********** */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "Schedule.h"
#include "Buttontask.h"
#include "LEDtask.h"

/* State definitions or the button management state machine */
typedef enum
{
     BUTTON_START = 0x00,
     BUTTON_ACTIVE = 0x01,
     BUTTON_ACTIVE_2 = 0x02
}T_BUTTON_STATE;

/* State machine variable */
static T_BUTTON_STATE ButtonState = 0x00;

/* Button On Timer */
static uint16_t ButtonOnTicks = 0x0000;

/* Button event buffer */
static T_BUTTON_EVENT ButtonActivityEvent = BUTTON_NO_EVENT;

/* Button release tick count */
#define BUTTON_OFF_TICKS           (0U)

/* Button debounce tick count */
#define BUTTON_DEBOUNCE_TICKS      (4U)

/* Button press minimum tick count */
#define BUTTON_PRESS_TICKS         (200U)

/* *** ButtontaskInit ******************* */
void ButtontaskInit(void)
{    
    /* Set pull up on input button */
    PUEA |= (1<<PINA7);
}

/* *** ButtonTick *********************** */
void ButtonTick(void)
{
     /* Every Tick - schedule the task */
     ScheduleTask(BUTTON_TASK);
}

/* *** ButtonTask *********************** */
void ButtonTask(void)
{
     if (PINA & (1<<PINA7))
     {
          ButtonOnTicks = 0x0000;
     }
     else
     {
          ButtonOnTicks++;
     }
     
     switch (ButtonState)
     {
          case BUTTON_START:
          {
               /* Check for a button press */
               if (ButtonOnTicks >= BUTTON_DEBOUNCE_TICKS)
               {
                    ButtonState = BUTTON_ACTIVE;
               }
               break;
          }
          
          case BUTTON_ACTIVE:
          {    
               /* Check for a button release */
               if (ButtonOnTicks == BUTTON_OFF_TICKS)
               {
                    ButtonState = BUTTON_START;
                    ButtonActivityEvent = BUTTON_RELEASE;
               }
               /* Check for button press minimum ticks */
               else if (ButtonOnTicks >= BUTTON_PRESS_TICKS)
               {
                    ButtonState = BUTTON_ACTIVE_2;
                    ButtonActivityEvent = BUTTON_PRESS;
               }
               break;
          }
          
          case BUTTON_ACTIVE_2:
          {
               if (ButtonOnTicks == BUTTON_OFF_TICKS)
               {
                    ButtonState = BUTTON_START;
                    ButtonActivityEvent = BUTTON_RELEASE;
               }
               break;
          }
     }     
}

/* *** ButtonEvent *********************** */
T_BUTTON_EVENT ButtonEvent(void)
{
     T_BUTTON_EVENT return_value = ButtonActivityEvent;
     
     ButtonActivityEvent = BUTTON_NO_EVENT;
     return(return_value);
}
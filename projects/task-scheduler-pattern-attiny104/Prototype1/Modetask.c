/* ******* Modetask **************** */
/*                                     */
/* Contains the code for the           */
/*     Mode task                     */
/*                                     */
/* Author: Dan Vahle - Secaplius       */
/*                                     */
/* ********** May 10, 2016 *********** */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "Schedule.h"
#include "Modetask.h"
#include "Buttontask.h"
#include "LEDtask.h"

/* State definitions for the button management state machine */
typedef enum
{
     MODE_START = 0x00,
     MODE_CHANGE_START = 0x01,
     MODE_FLASH_CHOICES = 0x02
}T_MODE_STATE;

/* State machine variable */
static T_MODE_STATE ModeState = 0x00;

/* State definitions or the button management state machine */
typedef enum
{
     DEVICE_MODE_ONE = 0x01,
     DEVICE_MODE_TWO = 0x02,
     DEVICE_MODE_THREE = 0x03
}T_DEVICE_MODE;

/* Current mode of the device */
static T_DEVICE_MODE DeviceMode = DEVICE_MODE_ONE;

/* Selection for mode change of the device */
static T_DEVICE_MODE SelectMode = DEVICE_MODE_TWO;

/* Mode event buffer */
static T_BUTTON_EVENT EventFromButtonTask = BUTTON_NO_EVENT;

/* Task execution period timeout in ticks */
#define MODE_TASK_EXECUTION_PERIOD      (300U)

/* Mode period timer */
static uint16_t ModePeriodTimer = 0x0000;

/* Mode state machine timer */
static uint8_t ModeStateTimer = 0x00;

/* Mode short timeout tick count */
#define MODE_SHORT_TIMEOUT         (0x03)

/* Mode long timeout tick count */
#define MODE_LONG_TIMEOUT          (0x06)

/* *** ModeTick *********************** */
void ModeTick(void)
{
     ModePeriodTimer++;
     
     if (ModePeriodTimer >= MODE_TASK_EXECUTION_PERIOD)
     {
          ModePeriodTimer = 0x0000;
          
          /* Every two seconds - schedule the task */
          ScheduleTask(MODE_TASK);
     }
}

/* *** ModeTask *********************** */
void ModeTask(void)
{
     /* Temporary variable for mode selection */
     T_DEVICE_MODE temp_mode = DEVICE_MODE_ONE;
     
     /* Get the last button event */
     EventFromButtonTask = ButtonEvent();
     
     /* Coarse timer based on the number of calls to ModeTask */
     ModeStateTimer++;
     
     /* Mode state machine */
     switch(ModeState)
     {
          case MODE_START:
          {
               /* Display the current Device mode */
               if (EventFromButtonTask == BUTTON_RELEASE)
               {
                    LEDFlash(DeviceMode, LED_BLUE);
               }
               /* Display the mode change indicator */
               else if (EventFromButtonTask == BUTTON_PRESS)
               {
                    LEDFlash(0x01, LED_RED);
                    ModeState = MODE_CHANGE_START;
               }
               
               /* Reset the Mode Timer always in this state */
               ModeStateTimer = 0x00;
               break;
          }
                   
          case MODE_CHANGE_START:
          {
               /* Start flashing the mode selection choices */
               if (EventFromButtonTask == BUTTON_RELEASE)
               {
                    LEDFlash(SelectMode, LED_YELLOW);
                    ModeState = MODE_FLASH_CHOICES;
                    ModeStateTimer = 0x00;
               }
               /* Timeout for this state if user does not release button */
               else if (ModeStateTimer >= MODE_SHORT_TIMEOUT)
               {
                    ModeState = MODE_START;
               }
               break;
          }
          
          case MODE_FLASH_CHOICES:
          {
               /* Process the new mode selection press */
               if (EventFromButtonTask == BUTTON_RELEASE)
               {
                    temp_mode = DeviceMode;
                    DeviceMode = SelectMode;
                    SelectMode = temp_mode;
                    ModeState = MODE_START;
               }
               /* Timeout for this state if user does not release button */
               else if (ModeStateTimer >= MODE_LONG_TIMEOUT)
               {
                    ModeState = MODE_START;
               }
               /* Select the next mode choice and display the mode count */
               else
               {
                    for(temp_mode = DEVICE_MODE_ONE; temp_mode <= DEVICE_MODE_THREE; temp_mode++)
                    {
                         if ((temp_mode != SelectMode) && (temp_mode != DeviceMode))
                         {
                              SelectMode = temp_mode;
                              break;
                         }
                    }
                    LEDFlash(SelectMode, LED_YELLOW);
               }                                   
               break;
          }
     }
}

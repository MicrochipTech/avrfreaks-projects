/* ******* LEDtask ******************* */
/*                                     */
/* Contains the code for the LEDtask   */
/*                                     */
/* Author: Dan Vahle - Secaplius       */
/*                                     */
/* ********** May 10, 2016 *********** */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "Schedule.h"
#include "LEDtask.h"

/* Number of ticks (175 ms) for the ON time */
#define LED_PERIOD_TIME       (35U)
static uint8_t PeriodTimer = LED_PERIOD_TIME;

/* Color of LED to Display */
static uint8_t LEDColor = LED_PORTA_OFF;

/* Number of LED Flashes */
static uint8_t FlashCount = 0x00;

/* Flash States */
typedef enum
{
     FLASH_START = 0x00,
     FLASH_OFF = 0x01
}T_FLASH_STATE;

/* LED Flash State */
static T_FLASH_STATE FlashState = FLASH_START;

/* *** LEDtaskInit ******************* */
void LEDtaskInit(void)
{
    /* Configure LED pins as outputs */
    DDRA |= (LED_YELLOW | LED_BLUE | LED_YELLOW_OB | LED_RED);
    
    /* switch off the LEDs */
    PORTA |= LED_PORTA_OFF;
}

/* *** LEDTick *********************** */
void LEDTick(void)
{
     /* Every Tick decrement the PeriodTimer count - when zero get a new count */
     if (PeriodTimer > 0x00)
     {
          PeriodTimer--;
     }
     else
     {
          ScheduleTask(LED_TASK);
          PeriodTimer = LED_PERIOD_TIME;
     }
}

/* *** LEDTask *********************** */
void LEDTask(void)
{
     switch (FlashState)
     {
          case FLASH_START:
          {
               /* Check for flash in progress */
               if (FlashCount)
               {
                    /* Turn LED ON */
                    PORTA &= ~LEDColor;
                    FlashState = FLASH_OFF;
                    FlashCount--;
               }
               break;
          }
          
          case FLASH_OFF:
          {
               /* Turn LED OFF */
               PORTA |= LED_PORTA_OFF;
               FlashState = FLASH_START;
               break;
          }          
     }
}

/* *** LEDFlash *********************** */
void LEDFlash(uint8_t flash_count, T_LED_NAME color)
{
     LEDColor = color;
     FlashCount = flash_count;      
}
/* ******* LEDtask ******************* */
/*                                     */
/* Contains the interface for the      */
/*    LEDtask                          */
/*                                     */
/* Author: Dan Vahle - Secaplius       */
/*                                     */
/* ********** May 10, 2016 *********** */

#ifndef LED_TASK_H
#define LED_TASK_H

/* List of LED names and their bit values */
typedef enum
{
     LED_PORTA_OFF = 0x78,
     LED_YELLOW = 0x08,
     LED_BLUE = 0x10,
     LED_YELLOW_OB = 0x20,
     LED_RED = 0x40
}T_LED_NAME;

/* *** LEDtaskInit ******************* */
/*                                     */
/* Initialize the task and the         */
/*      interrupt                      */
/* *********************************** */
void LEDtaskInit(void);

/* *** LEDTick *********************** */
/*                                     */
/* Determine if the task should run    */
/* *********************************** */
void LEDTick(void);

/* *** LEDTask *********************** */
/*                                     */
/* Execute the task function           */
/* *********************************** */
void LEDTask(void);

/* *** LEDFlash ********************** */
/*                                     */
/* Set the number of flashes and the   */
/* LED color                           */
/* *********************************** */
void LEDFlash(uint8_t flash_count, T_LED_NAME color);

#endif /* LED_TASK_H */

/* ******* Buttontask **************** */
/*                                     */
/* Contains the interface for the      */
/*    Buttontask                       */
/*                                     */
/* Author: Dan Vahle - Secaplius       */
/*                                     */
/* ********** July 30, 2016 *********** */

#ifndef BUTTON_TASK_H
#define BUTTON_TASK_H

/* Definitions for the button events */
typedef enum
{
     BUTTON_NO_EVENT = 0x00,
     BUTTON_RELEASE = 0x01,
     BUTTON_PRESS = 0x02
}T_BUTTON_EVENT;

/* *** ButtontaskInit **************** */
/*                                     */
/* Initialize the task                 */
/* *********************************** */
void ButtontaskInit(void);

/* *** ButtonTick ******************** */
/*                                     */
/* Determine if the task should run    */
/* *********************************** */
void ButtonTick(void);

/* *** ButtonTask ******************** */
/*                                     */
/* Execute the task function           */
/* *********************************** */
void ButtonTask(void);

/* *** ButtonEvent ******************* */
/*                                     */
/* Return the current button event     */
/* *********************************** */
T_BUTTON_EVENT ButtonEvent(void);

#endif /* BUTTON_TASK_H */

/*****************************************************************
RTOS demo using pumpkin salvo tiny RTOS w/ICCAVR C compiler
Blink 8 leds at different rates using 8 tasks
Not rocket science, just basic rtos configuration can be used
as basis for more complicated projects


JCC KI0BK
******************************************************************/

#include <iccioavr.h>
#include "rtos.h"
#include "board.h"

//Globals

//tasks
/*************************************************
Task1
*************************************************/
void Task1 (void)
{
    for(;;) //forever
    {
        ToggleBit(PORTB, PB0); //blink led
        OS_Delay(10);
    }
}

/*************************************************
Task2
*************************************************/
void Task2 (void)
{
    for(;;) //forever
    {
        ToggleBit(PORTB, PB1); //blink led
        OS_Delay(20);
    }
}

/*************************************************
Task3
*************************************************/
void Task3 (void)
{
    for(;;)
    {
        ToggleBit(PORTB, PB2); //blink led
        OS_Delay(30);
    } //while end
}

/*************************************************
 Task4
*************************************************/
void Task4 (void)
{
    for(;;)
    {
        ToggleBit(PORTB, PB3); //blink led
        OS_Delay(40);
    } //while end
}

/*************************************************
Task5
*************************************************/
void Task5 (void)
{
    for(;;) //forever
    {
        ToggleBit(PORTB, PB4); //blink led
        OS_Delay(50);
    }
}

/*************************************************
Task6
*************************************************/
void Task6 (void)
{
    for(;;) //forever
    {
        ToggleBit(PORTB, PB5); //blink led
        OS_Delay(60);
    }
}

/*************************************************
Task7
*************************************************/
void Task7 (void)
{
    for(;;) //forever
    {
        ToggleBit(PORTB, PB6); //blink led
        OS_Delay(70);
    }
}

/*************************************************
Task8
*************************************************/
void Task8 (void)
{
    for(;;) //forever
    {
        ToggleBit(PORTB, PB7); //blink led
        OS_Delay(80);
    }
}

/***********************************************************
// main
// init HW, init OS, create tasks
// start multitasking
***********************************************************/
void main (void)
{
    // Hardware-dependent initilization.
    panel_init();

    // Initialize Salvo.
    OSInit();

    // Create Salvo tasks.
    OSCreateTask(Task1, TASK_1_P, PRIO_1);
    OSCreateTask(Task2, TASK_2_P, PRIO_2);
    OSCreateTask(Task3, TASK_3_P, PRIO_3);
    OSCreateTask(Task4, TASK_4_P, PRIO_4);
    OSCreateTask(Task5, TASK_5_P, PRIO_5);
    OSCreateTask(Task6, TASK_6_P, PRIO_6);
    OSCreateTask(Task7, TASK_7_P, PRIO_7);
    OSCreateTask(Task8, TASK_8_P, PRIO_8);

    SEI(); //enable timer interrupts
    // Start multitasking.
    for(;;)
    {
        OSSched();
    }
}

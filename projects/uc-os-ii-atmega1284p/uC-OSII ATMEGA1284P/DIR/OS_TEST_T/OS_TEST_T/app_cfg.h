/*
 * app_cfg.h - Archivo de cabecera donde se definen todas las constantes utilizadas por el 
			   programa principal
 
				Header file where all the constants used by the main file are defined
 *
 * Created: 03/04/2011 04:07:01 a.m.
 *  Author: neo_zx
 */ 

/*
**************************************************************************************************************
*                                               TAMAÑOS DE LAS PILAS - STACK SIZES
**************************************************************************************************************
*/


#define  START_STK_SIZE          128	/*Tamaño de la pila de la tarea de inicio
										  Stack size from the start task*/
			
#define  TASK1_STK_SIZE          128	/*Tamaño de la pila de la tarea 1
										  Stack size from the task 1*/

#define  TASK2_STK_SIZE          128	/*Tamaño de la pila de la tarea 2
										  Stack size from the task 2*/

/*
**************************************************************************************************************
*                                             PRIORIDADES DE LAS TAREAS - TASK PRIORITIES
**************************************************************************************************************
*/

#define  OS_TASK_TMR_PRIO                  4	/*Prioridad de la interrupción del TIMER0 del
												  dispositivo, que es el que genera el reloj del 
												  sistema operativo.
												  
												  Priority of the interruption of TIMER0 of the
												  device, which is the one that generates the clock
												  of the operating system*/

#define  OS_START_STK_PRIO				   5	/*Prioridad de la tarea de inicio
												  Start task priority*/

#define  OS_TASK1_STK_PRIO				   6	/*Prioridad de la tarea 1
												  Task 1 priority*/

#define  OS_TASK2_STK_PRIO				   7	/*Prioridad de la tarea 2
												  Task 2 priority*/




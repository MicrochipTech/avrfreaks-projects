/*
 * OSTEST_T.c -- Programa que crea 2 tareas, donde una de ellas coloca el dato 0x55
				en el puerto B durante 1 segundo mientras que la otra coloca el
				dato 0xAA en el mismo puerto B durante 1 segundo.
				
				Program that creates 2 tasks, one of them sets the data 0x55 in PORTB
				during 1 second while the other task sets the data 0xAA in the same
				PORTB during 1 second
 *
 * Created: 03/04/2011 03:17:40 a.m.
 *  Author: neo_zx
 */ 

#include "includes.h"	/*Declaracion de la libreria global
						  Declaration of global library*/


/*Definicion de las pilas de las tareas
  Tasks stacks definition*/

OS_STK	TaskStk1[TASK1_STK_SIZE];				
OS_STK	TaskStk2[TASK2_STK_SIZE];				
OS_STK	TaskStartStk[START_STK_SIZE];	


OS_EVENT	*PORTBAccSem;	/*Declaracion del semáforo de acceso al puerto B
							  Declaration of port B access semaphore*/


/*Definicion de las prototipos de las tareas
  Tasks prototypes definition*/

void  Task1(void *data);
void  Task2(void *data);
void  TaskStart(void *data);


int main (void)
{	
	OSInit();	/*Configuracion inicial de uC-OSII 
			      uC-OSII initial configuration*/
	  
	OSTaskCreate(TaskStart, (void *)0, (void *)&TaskStartStk[START_STK_SIZE - 1], OS_START_STK_PRIO);	/*Creacion de la tarea inicial
																										  Initial task creation*/
	OSStart();	/*Inicializacion de uC-OSII
				  uC-OSII initialization*/ 

	do 
	{
		
		/*Nunca llega a este punto
		  Never reaches this point*/
		
	} while (1);
	
	return 0;	
}


void TaskStart (void *data)
{
	OS_ENTER_CRITICAL();	/*Deshabilita interrupciones ya que entra a seccion critica
							  Disables interrupts since enters to a critical section*/	
	
	
	/*Configuraciones iniciales de registros del microcontrolador
	  Initial configurations of microcontroller registers*/
	
	DDRB=0xFF;	/*Declara el puerto B como salida
				  Declares port B as output*/
	
	TCCR0B=0x05;	/*Declara el TIMER0 como reservado y aplica un preescalador de 1024 al reloj del sistema
					  Declares TIMER0 as reserved and applies a 1024 preescaler to system clock*/																		
	
	TIMSK0=_BV(TOIE0);	/*Habilita la interrupcion de desbordamiento del TIMER0
						  Enables TIMER0 overflow interruption*/															
	
	TCNT0=256-(CPU_CLOCK_HZ/OS_TICKS_PER_SEC/1024);	/*Asigna un valor inicial al registro TCNT0, este valor
													  se encuentra ademas en el archivo os_cpu_a.s
													  
													  Assigns an initial value to register TCNT0, this value
													  is also in the file os_cpu_a.s*/	
	
	
	OS_EXIT_CRITICAL();	/*Habilita interrupciones ya que sale de seccion critica
						  Enables interrupts since exits from a critical section*/

	OSTaskCreate(Task1, (void *)0, (void *)&TaskStk1[TASK1_STK_SIZE-1], OS_TASK1_STK_PRIO);	/*Creacion de la tarea 1
																							  Task 1 creation*/
	
	OSTaskCreate(Task2, (void *)0, (void *)&TaskStk2[TASK2_STK_SIZE-1], OS_TASK2_STK_PRIO);	/*Creacion de la tarea 2
																							  Task 2 creation*/
	
	PORTBAccSem=OSSemCreate(1);	/*Inicializacion del semaforo PORTBAccSem
								  PORTBAccSem semaphore initialization*/

	OSTaskDel(OS_PRIO_SELF);	/*Autodestruccion de la tarea TaskStart
								  Self-destruction of TaskStart task*/
}


void Task1 (void *data)
{
	INT8U err;	/*Declaracion de la variable local de error
				  Error local variable declaration*/
	
	while(1)
	{
		OSSemPend(PORTBAccSem,0,&err);	/*La tarea Task1 se apropia del recurso PORTB con ayuda del semaforo
										  PORTBAccSem y espera indefinidamente hasta la liberacion del mismo
										  
										  Task1 task appropriates of the PORTB resource aided by the semaphore
										  PORTBAccSem and waits indefinitely until its liberation*/
		
		PORTB=0x55;	/*Escribe en el puerto B
					  Writes in port B*/
		
		OSTimeDlyHMSM(0,0,1,0); /*Duerme a la tarea Task1 durante 1 segundo
								  Sleeps Task1 task for 1 second*/
		
		OSSemPost(PORTBAccSem);	/*La tarea Task1 libera el recurso PORTB
								  Task1 task liberates PORTB resource*/
	}
}

void Task2 (void *data){
	INT8U err;	/*Declaracion de la variable local de error
				  Error local variable declaration*/
	
	while(1)
	{
		OSSemPend(PORTBAccSem,0,&err);	/*La tarea Task2 se apropia del recurso PORTB con ayuda del semaforo
										  PORTBAccSem y espera indefinidamente hasta la liberacion del mismo
										  
										  Task2 task appropriates of the PORTB resource aided by the semaphore
										  PORTBAccSem and waits indefinitely until its liberation*/
		
		PORTB=0xAA;	/*Escribe en el puerto B
					  Writes in port B*/
		
		OSTimeDlyHMSM(0,0,1,0);	/*Duerme a la tarea Task1 durante 1 segundo
								  Sleeps Task1 task for 1 second*/
		
		OSSemPost(PORTBAccSem);	/*La tarea Task1 libera el recurso PORTB
								  Task1 task liberates PORTB resource*/
	}
}

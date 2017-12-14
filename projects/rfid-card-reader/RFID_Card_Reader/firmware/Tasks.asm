;*****DON'T PUT INCLUDE FILES BEFORE TASK 1 DEFINITION
#define KB_BUFF_SIZE 15

#define RS232_OUTPUT_BUFF_SIZE  2

#define LCD_UPDATE_EVENT 0
#define RS232_READY_EVENT 1
#define WIEGAND_READY_EVENT 2

#if WIEGAND_READY_EVENT==4
.message "---------------------------YES-------------------------"
#endif

System_Task:
 //***turn off Watch dog reset for testing purpous
 cli
 ldi temp, (1<<WDTOE)+(1<<WDE)
 out WDTCR, temp
 ldi temp, (1<<WDTOE)
 out WDTCR, temp
 sei

_THRESHOLD_BARRIER_WAIT InitTasksBarrier,TASKS_NUMBER 


main1:
_YIELD_TASK
rjmp main1  


Task_2:		

 _THRESHOLD_BARRIER_WAIT InitTasksBarrier,TASKS_NUMBER 
	sbi DDRB,PORTB0
	sbi PORTB,PORTB0
main2:
	sbi PORTB,PORTB0	
	_SLEEP_TASK 200
	_SLEEP_TASK 200
	_SLEEP_TASK 200
	cbi PORTB,PORTB0
	_SLEEP_TASK 200
	_SLEEP_TASK 200
	_SLEEP_TASK 200
rjmp main2  	



.include "include/lcdtask.asm"

.include "include/cardtask.asm"

.include "include/rs232task.asm"



.EXIT










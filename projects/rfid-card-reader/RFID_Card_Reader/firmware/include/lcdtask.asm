.def    argument=r17   
.def    return = r18
.def    t1=r19
.def    t2=r20
.def    counter=r21

.include "include/LCD4bitWinstarDriver.asm"

LCD_Task:

	_THRESHOLD_BARRIER_WAIT InitTasksBarrier,TASKS_NUMBER 
	
	_SLEEP_TASK 250
	rcall lcd4_init

main3:

	_SLEEP_TASK 150
	;_EVENT_WAIT LCD_UPDATE_EVENT

	
	;ldi argument,LCD_DISPLAY_CLEAR
	;rcall lcd4_command

	ldi argument,LCD_LINE_1 
	rcall lcd4_command


	lds argument,RxByte
	rcall lcd4_hex_out

    lds argument,dataBits+1	;most significat byte		
	rcall lcd4_hex_out

    lds argument,dataBits+2
	rcall lcd4_hex_out

    lds argument,dataBits+3
	rcall lcd4_hex_out


	ldi argument,LCD_LINE_2 
	rcall lcd4_command

    lds argument,facilityCode
	rcall lcd4_hex_out

    lds argument,cardCode
	rcall lcd4_hex_out

	lds argument,cardCode+1
	rcall lcd4_hex_out
rjmp main3

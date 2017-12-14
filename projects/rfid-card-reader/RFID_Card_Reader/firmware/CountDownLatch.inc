/***********************COUNT DOWN LATCH*****************************
AUTHOR:sergei_iliev@yahoo.com
Implementation of a non reusable LATCH
RAM structure of one bytes:

; ________________
;|x|x|x|x|x|x|x|x|
;~~~~~~~~~~~~~~~~~
;xxxx ->total number of events to happen before the latch is opened.

256 possible events to wait for.

*/


;----------------------------------------------------------------
;USER MODE API
;INPUT: @0 - latch name in RAM       
;       @1 - number of events to wait on
;USAGE: r16(temp)
;DANGER: latch must be initialized before its usage - either in task init or system init.
;----------------------------------------------------------------

.MACRO _INITIALIZE_LATCH
    push temp
	ldi temp,@1
	sts @0,temp
	pop temp
.ENDMACRO

;----------------------------------------------------------------
;USER AND KERNEL MODE API
;INPUT: @0 - latch name in RAM       
;       @1 - context{0=task;0xFF=interrupt}
;USAGE: r16(temp)
;KEEP the code with interrupts disabled as short as possible
;----------------------------------------------------------------

.MACRO _COUNT_DOWN_LATCH
	.IF @1==TASK_CONTEXT   ;no interrupt is possible if in interrupt,sei is NOT used
    cli
	.ENDIF

    push temp 
    
    lds temp,@0
	tst temp
	breq exit
    dec temp
	sts @0,temp  

exit:
    pop temp

	.IF @1==TASK_CONTEXT   ;enable interrupts if in user land
    sei
	.ENDIF  
.ENDMACRO

;----------------------------------------------------------------
;USER MODE API
;INPUT: @0 - latch name in RAM       
;USAGE: r16(temp)
;----------------------------------------------------------------

.MACRO _WAIT_LATCH
	push temp

loop:	
	lds temp,@0
    tst temp	
	breq exit      
    _YIELD_TASK
	rjmp loop

exit:
	pop temp  
.ENDMACRO 

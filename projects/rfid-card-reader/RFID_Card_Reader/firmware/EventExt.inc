/*
AUTHOR:sergei_iliev@yahoo.com
Extended event system.
Provide Contention and deadlock free event system.
N threads could wait on an event which could be signaled by N threads.
*/




/*
USER API
Event API - Contention free synchronization type event.
Called in the current task context.
INPUT:		@0 - event name
USED: temp,r17(saved on current task stack)
*/
.SET EVENT_SIGNAL_BIT=7  

.MACRO _WAIT_EVENT_EX
	push temp
	push r17
;INIT PART
    cli
    lds r17,@0+1        ;read global ticket which will be mine.
	mov temp,r17        ;backup ticket for later use
    inc r17 
    cpi r17,0x0F        ;calculate next available ticket 
    brlo weevjmp
	clr r17		
weevjmp:	   
    sts @0+1,r17        ;save in global ticket variable which will be the next another task to reserve
	sei



;SPINNING PART
;1.wait until your ticket becomes valid
;2.wait until event flag is reised(=1)

	sbr temp,(1<<EVENT_SIGNAL_BIT)       ; Set MSB (which is the signal flag) in local ticket
weespin:
    lds r17,@0         ;current valid ticket and event signal	
    cp r17,temp
    breq go_to_exit_code
    _YIELD_TASK
	rjmp weespin

	

;EXIT PART
;next event signal could be missed(overriden) here if it happens between the read and write part of the event signal flag.
go_to_exit_code:	
    andi temp,0x0F       ;reset event flag
    inc temp 
    cpi temp,0x0F        ;calculate next valid ticket 
    brlo weeexit
	clr temp		
weeexit:    	
	sts @0,temp  	
    
    pop r17
    pop temp 

.ENDMACRO

/*
USER and KERNEL mode API.
If called in KERNEL mode - arbitrary task context - mind the stack usage.
INPUT: @0 - event name
       @1 - context{0=task;0xFF=interrupt}
USED: temp(saved on current <arbitrary> task stack)
*/
.MACRO  _SET_EVENT_EX
	.IF @1==TASK_CONTEXT   ;no interrupt is possible if in interrupt,sei is NOT used
    cli
	.ENDIF
	
	push temp    
    
    lds temp,@0
	sbr temp,(1<<EVENT_SIGNAL_BIT)        ;signal flag bit 
    sts @0,temp  
    
	pop temp
    
	.IF @1==TASK_CONTEXT   ;enable interrupts if in user land
    sei
	.ENDIF
.ENDMACRO

/*
USER mode API
NEVER use it in KERNEL mode.
INPUT: @0 - event name
USED: temp(saved on current task stack)
*/
.MACRO  _SET_EVENT_AND_WAIT_EX    
	push temp    

	cli
;raise event flag    
    lds temp,@0
	sbr temp,(1<<EVENT_SIGNAL_BIT)        ;signal flag bit 
    sts @0,temp  
    
    sei


;wait for the listenning task to get the event signal
sewspin:
    _YIELD_TASK
    lds temp,@0
    sbrc temp,EVENT_SIGNAL_BIT
	rjmp sewspin

	pop temp 
.ENDMACRO


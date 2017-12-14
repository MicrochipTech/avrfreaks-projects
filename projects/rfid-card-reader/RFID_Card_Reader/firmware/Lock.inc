/***********BASIC LOCK*****************************************
AUTHOR:sergei_iliev@yahoo.com
Lock synchronization implementation.
Basic Ticket algorithm implementation.
RAM structure of two bytes:

1 byte  - Valid Number
1 byte  - Ticket Number

The lock is contention free and fair.
No reentrancy and timeout capabilities.
256 possible tickets which limits the number of waiting threads on the lock to 256.
*/
     
;----------------------------------------------------------------
;USER MODE API
;INPUT: @0 - lock name in RAM       
;USAGE: r16(temp)
;		r17
;----------------------------------------------------------------

.MACRO _LOCK_ACQUIRE
	push r16
	push r17

;INIT next available ticket and reserve current for me
    cli
    lds r17,@0+1        ;read global ticket which will be mine.
	mov r16,r17        ;backup ticket for later use
    inc r17             ;create next available ticket 	   
    sts @0+1,r17        ;save in global ticket variable which will be the next another task to reserve
    sei

;WAIT until your ticket becomes valid
lockspin:
    lds r17,@0         ;current valid ticket 
    cp r17,r16
    breq go_to_exit
    _YIELD_TASK
    rjmp lockspin

go_to_exit:
    pop r17
    pop r16
.ENDMACRO


;----------------------------------------------------------------
;USER MODE API
;INPUT: @0 - lock name in RAM       
;USAGE: r17
;----------------------------------------------------------------
.MACRO _LOCK_RELEASE
;EXIT - enable the next wating thread in order (fairness)
;should it be atomic - no need if only current thread writes to @0
    push r17

    lds r17,@0    
    inc r17    
    sts @0,r17

	pop r17
.ENDMACRO

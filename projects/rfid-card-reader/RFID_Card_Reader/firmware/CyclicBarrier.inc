/***********BASIC BARRIER*****************************************
AUTHOR:sergei_iliev@yahoo.com
Cyclic Barrier - implementation of a basic reusable barrier.
Atomic Counter algorithm implementation.
RAM structure of one bytes:

; ________________
;|y|x|x|x|x|x|x|x|
;~~~~~~~~~~~~~~~~~
;y->GO bit 
;xxxx ->total number of tasks to wait on the barrier

128 possible waitng tasks on a barrier.

Counter algorithm: 
BEGIN
	local.go=go
	counter=counter+1;

IF counter==n THEN
	counter=0
	go=1-go
ELSE AWAIT(local.go<>go) FI
END
*/

.SET GO_BIT=7

;----------------------------------------------------------------
;USER MODE API
;INPUT: @0 - barrier name in RAM       
;       @1 - number of tasks to wait on
;USAGE: r16(temp)
;		r17	
;DANGER: the number of barrier wait calls MUST be equal to the second parameter of the macro.
;----------------------------------------------------------------
.MACRO _CYCLICBARRIER_WAIT
	push temp
	push r17

;INIT	
	cli
	lds temp,@0             ;local storage in temp
	mov r17,temp
	andi r17,0x7F           ;isolate number
	inc r17
    sbrc temp,GO_BIT 
    sbr r17,(1<<GO_BIT)
	sts @0,r17
	sei
	
;NOTIFY if predicate holds
	andi r17,0x7F			;isolate number	
    cpi r17,@1
	brne wait_go       
    
	
	com temp                 ;toggle go bit
	andi temp,(1<<GO_BIT)    ;preserve the GO bit and clear the rest
    sts @0,temp
    rjmp go_to_exit

;WAIT	
;temp holds the old GO bit
wait_go:
	andi temp,(1<<GO_BIT)    ;isolate GO bit     
loop_go:
    _YIELD_TASK
    lds r17,@0         
    andi r17,(1<<GO_BIT)    ;isolate global bit 
	
	cp temp,r17
    breq loop_go           ;same bit -> go on looping, else leave barrier 
        
go_to_exit:      
    pop r17 
	pop temp	  
.ENDMACRO

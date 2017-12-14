;*************************************************************
;User mode API
;Barrier - synchronization premitive.It makes sure that all tasks start relatively at one time.  
;Very simple implementation.Each barier is represented by one byte in SRAM
;Simple lead-follower implementation
;INPUT: @0 - barier name
;       @1 - total number of tasks
;USAGE: temp
;WARNING: Barrier byte is not reusable!!!
       
.MACRO _THRESHOLD_BARRIER_WAIT	
	push temp
	
	cli
	lds temp,@0
	sbrc temp,7        ;is there a leader?
    rjmp no_leader        
	sbr temp,(1<<7)|(1<<0)      ;mark as a leader and start from 1
	sts @0,temp
	sei
leader_loop:
;***yield processing
    _YIELD_TASK	
	lds temp,@0
	andi temp,0x1F      ;up to 0x10 = 16 
	cpi temp,@1
	brne leader_loop
                         ;notify followers we are done
    clr temp    
    sts @0,temp

    rjmp exit_barrier 
no_leader:    
    andi temp,0x1F     ;isolate the barrier tasks counter
    inc temp
    sbr temp,1<<7      ;there is a leader(still an atomic sequence of operations)
    sts @0,temp
	sei
follower_loop:
;***yield processing
    _YIELD_TASK		
	lds temp,@0
	andi temp,0x1F
	tst temp
	brne follower_loop  ;wait the signal from leader	  

exit_barrier:
    pop temp 
.ENDMACRO

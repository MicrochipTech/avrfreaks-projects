;Look at http://www.acorn-kernel.net/ for sample Applications
;3 demo tasks are included to demostrate sleep mode integration
;sergei_iliev@yahoo.com
;Ask questions or give ideas!!!
.include "m16def.inc"
.include "16bitMath.inc"






.include "INTERRUPTS.inc" 
.include "Kernel.inc"

.cseg
.def    temp=r16    ;temp reg.

.include "HARDWARE.inc"

RESET:     
    ;One rcall depth for the stack during system init 
	ldi     temp,high(RAMEND-2)        ;Set stack pointer to bottom of RAM-2 for system init
    out     SPH,temp
    ldi     temp,low(RAMEND-2)
    out     SPL,temp
     	       
;clear SRAM
	ldi XL,low(RAMEND+1)
	ldi XH,high(RAMEND+1)    		   
    clr r0
initos:
	st -X,r0
	cpi XH,high(SRAM_START) 
    brne initos
    cpi XL,low(SRAM_START)
	brne initos
	 
;****************init Task_1
	.IFDEF TCB_1
	_REGISTER_TASK System_Task,1,TCB_1 
    .ENDIF
;********************init Task_2
    .IFDEF TCB_2
	_REGISTER_TASK Task_2,2,TCB_2 
	.ENDIF
;********************init Task_3
	.IFDEF TCB_3
	_REGISTER_TASK Task_3,3,TCB_3 
	.ENDIF
;********************init Task_4
	.IFDEF TCB_4
	_REGISTER_TASK Task_4,4,TCB_4 
	.ENDIF
;********************init Task_5
	.IFDEF TCB_5
	_REGISTER_TASK Task_5,5,TCB_5 
	.ENDIF
;********************init Task_6
	.IFDEF TCB_6
	_REGISTER_TASK Task_6,6,TCB_6 
	.ENDIF
;********************init Task_7
	.IFDEF TCB_7
	_REGISTER_TASK Task_7,7,TCB_7 
	.ENDIF

;********************init Task_8
	.IFDEF TCB_8
	_REGISTER_TASK Task_8,8,TCB_8 
	.ENDIF
;********************init Task_9
	.IFDEF TCB_9
	_REGISTER_TASK Task_9,9,TCB_9 
	.ENDIF
;********************init Task_10
	.IFDEF TCB_10
	_REGISTER_TASK Task_10,10,TCB_10 
	.ENDIF
;********************init Task_11
	.IFDEF TCB_11
	_REGISTER_TASK Task_11,11,TCB_11 
	.ENDIF
;********************init Task_12
	.IFDEF TCB_12
	_REGISTER_TASK Task_12,12,TCB_12 
	.ENDIF
;********************init Task_13
	.IFDEF TCB_13
	_REGISTER_TASK Task_13,13,TCB_13 
	.ENDIF
;********************init Task_14
	.IFDEF TCB_14
	_REGISTER_TASK Task_14,14,TCB_14 
	.ENDIF
;********************init Task_15
	.IFDEF TCB_15
	_REGISTER_TASK Task_15,15,TCB_15 
	.ENDIF
;********************init Task_16
	.IFDEF TCB_16
	_REGISTER_TASK Task_16,16,TCB_16 
	.ENDIF

;set up Timer0
    _INIT_TASKSHEDUAL_TIMER
;start timers

;start Timer0(Schedualing and time ticking)	
	_ENABLE_TASKSHEDUAL_TIMER


;initialize current task pointer with Task_1
	ldi     temp,high(RAMEND)
    out     SPH,temp
    ldi     temp,low(RAMEND)
    out     SPL,temp

    ldi XL,low(TCB_1) 
	
	sts pxCurrentTCB,XL
	
	sei      


//***curent stack pointer is at the begining of Task_1
.include "Tasks.asm"


DispatchDPCExtension:
	      ;restore temp and SREG to its task's context value
	_POST_INTERRUPT

TaskSchedular:

	_keOS_SAVE_CONTEXT
;start LIMBO state 
    ;_DISABLE_TASKSHEDUAL_TIMER
    
	_keSWITCH_TASK

    ;_ENABLE_TASKSHEDUAL_TIMER
;end LIMBO state
	_keOS_RESTORE_CONTEXT

reti

SystemTickInt:
  _PRE_INTERRUPT   
   
  _kePROCESS_SLEEP_INTERVAL	
  
  _POST_INTERRUPT
rjmp TaskSchedular


.EXIT




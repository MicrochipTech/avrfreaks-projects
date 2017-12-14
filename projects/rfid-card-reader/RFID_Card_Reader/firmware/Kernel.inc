/*
AUTHOR:sergei_iliev@yahoo.com
Optimized for use with devices of more then 256 bytes of RAM!
Acorn introduces 2 basic logical levels of code execution:
1.USER mode - the code being executed by the tasks
2.KERNEL mode - the code being executed by the interrupt procedures
User mode is interuptible , it could be preempted by the task schedular or interupt
Kernel mode is NOT interruptible - MCU clears the global interrupt flag on each interrupt so 
any code within interrupt procedure is executed in an atomic fashion.Use Deffered Procedure Call - like
dispatching to execute interrupt related code at DEVICE level and keep interrupt handle time shorter.
There are 2 priority levels:
1.PASSIVE level
2.DEVICE level

Help,Support,Praise to
sergei_iliev@yahoo.com
*/
#define OS MEGAACORN

#define VERSION 2.5

/*
*Define the number of tasks. Up to 16 for X and Mega kernel 
*/
#define TASKS_NUMBER 6
#if (TASKS_NUMBER>16)
	#error Mega Acorn kernel maximum tasks number is 16
#endif

;Comment out in production mode!!!!!!!!!!!!!!!!!!!!!!
#define DEBUG 

#ifndef SYSTEM_CLOCK  
.equ SYSTEM_CLOCK= 8000000   ;Hz
#endif

#ifndef SYSTEM_TICK
 #define SYSTEM_TICK  100000    ;us 
#endif

/*
Define Synchronization macros if needed.
*/
#define THRESHOLD_BARRIER_EXTENTION

;#define EVENT_EXTENTION

;#define LOCK_EXTENTION

;#define  CYCLIC_BARRIER_EXTENTION

;#define LATCH_EXTENTION

/*
Enable sleep mode 
*/
;#define SLEEP_MODE
;.EQU CONS_TIMER_COMPARE=((SYSTEM_CLOCK/8)/(1000000/SYSTEM_TICK))   ;devider =8

.EQU CONS_TIMER_COMPARE=57000

.EQU TASK_TCB_SIZE=4   ;TCB size is see below

;Compared to previous release - Task ID as added, which implies 16 tasks max
;@CHANGED
;-----------------------;
;1.LSB
;-----------------------;
;2.MSB
;-----------------------;
;3.Task status byte
;6bit->Is task schedulable(0-schedulable(default);1-not schedulable;Idle task is always schedulable)
;@NOTUSED(use IDF instead)
;7bit->Task priority(0-Normal level(default),task could be descheduled;1-INTERRUPT level)
;0-3bit-> Task ID number(up to 16 tasks)
;-----------------------
;4.Sleep Timeout
;-----------------------;


;is high priority
;.SET TASK_PRIORITY_BIT=7   ;not used - use global IDF instead
;is task shedulable 
.SET TASK_SCHEDULABLE_BIT=6

;***priorityt related
;.EQU TASK_PRIORITY_NORMAL=0

;.EQU TASK_PRIORITY_INTERRUPT=1

;***scheduling related
.EQU TASK_SCHEDULABLE_ON=0

.EQU TASK_SCHEDULABLE_OFF=1


.SET TASK_CONTEXT=0x00

.SET INTERRUPT_CONTEXT=0x255

/*
MUST be bigger then TASK_STACK_REGISTER_DEPTH=35(32reg+SREG+2 bytes Return address)
*/

.EQU TASK_STACK_DEPTH=55  ;MUST be bigger then TASK_STACK_REGISTER_DEPTH

.EQU TASK_STACK_REGISTER_DEPTH=35

/*
;Not all devicess start from 0x060 , at90USB starts from 0x100 for example
;I will personally come to your place and flog you if you change the 
;system RAM layout!!!!!!!!!
*/
.dseg
pxCurrentTCB: .byte 1   ;keep address of the current TCB



#if TASKS_NUMBER>=1
TCB_1: .byte TASK_TCB_SIZE     ;Stack pointer LSB:MSB to the top of the task stack
#endif


#if TASKS_NUMBER>=2  
TCB_2: .byte TASK_TCB_SIZE
#endif


#if TASKS_NUMBER>=3
TCB_3: .byte TASK_TCB_SIZE
#endif    


#if TASKS_NUMBER>=4
TCB_4: .byte TASK_TCB_SIZE
#endif



#if TASKS_NUMBER>=5
TCB_5: .byte TASK_TCB_SIZE
#endif


#if TASKS_NUMBER>=6
TCB_6: .byte TASK_TCB_SIZE
#endif


#if TASKS_NUMBER>=7
TCB_7: .byte TASK_TCB_SIZE
#endif


#if TASKS_NUMBER>=8
TCB_8: .byte TASK_TCB_SIZE
#endif


#if TASKS_NUMBER>=9
TCB_9: .byte TASK_TCB_SIZE
#endif


#if TASKS_NUMBER>=10
TCB_10: .byte TASK_TCB_SIZE
#endif


#if TASKS_NUMBER>=11
TCB_11: .byte TASK_TCB_SIZE
#endif


#if TASKS_NUMBER>=12
TCB_12: .byte TASK_TCB_SIZE
#endif


#if TASKS_NUMBER>=13
TCB_13: .byte TASK_TCB_SIZE
#endif


#if TASKS_NUMBER>=14
TCB_14: .byte TASK_TCB_SIZE
#endif


#if TASKS_NUMBER>=15
TCB_15: .byte TASK_TCB_SIZE
#endif


#if TASKS_NUMBER>=16
TCB_16: .byte TASK_TCB_SIZE
#endif
.cseg

#message OS  VERSION " using " TASKS_NUMBER " tasks."

.MACRO  _keOS_SAVE_CONTEXT
	push r0
	in r0,SREG
	push r0
	push r1
	push r2  
	push r3
	push r4
	push r5 
	push r6
	push r7
	push r8 
	push r9
	push r10
	push r11 
	push r12
	push r13
	push r14 
	push r15
	push r16
	push r17
	push r18
	push r19
	push r20 
	push r21
	push r22
	push r23
	push r24
	push r25
	push r26 
	push r27
	push r28
	push r29
	push r30
	push r31

    lds r26, pxCurrentTCB 
    ;lds r27, pxCurrentTCB + 1 
    ldi r27,0

	in r0, SPL
    st x+, r0 
    in r0, SPH
    st x, r0											 
.ENDMACRO

;new context is in pxCurentTCB
.MACRO  _keOS_RESTORE_CONTEXT
	lds r26, pxCurrentTCB
    ;lds r27, pxCurrentTCB + 1
	ldi r27,0
	
	ld r28, x+ 
	out SPL, r28 
    ld r29, x 
	out SPH, r29
    
	pop r31
	pop r30
	pop r29
	pop r28
    pop r27
	pop r26
	pop r25
	pop r24
    pop r23
	pop r22
	pop r21
	pop r20
    pop r19
	pop r18
	pop r17
	pop r16
    pop r15
	pop r14
	pop r13
	pop r12
    pop r11
	pop r10
	pop r9
	pop r8
    pop r7
	pop r6
	pop r5
	pop r4
    pop r3
	pop r2
    pop r1 
    pop r0 
    out SREG, r0 
    pop r0
.ENDMACRO

;@CHANGED(version=2.1)------------------------------
;KERNEL MODE API
;USAGE:	Used during kernel initialization,use any register
;INPUT:	@0  task label
;		@1  task number   
;		@2  task control RAM block - TCB
;---------------------------------------------------
.MACRO  _REGISTER_TASK
;1.Save initial return address points at Tasks' begining
	ldi XL,low(@0)
	ldi XH,high(@0)
	
	ldi ZL,low((@1-1)*TASK_STACK_DEPTH)
	ldi ZH,high((@1-1)*TASK_STACK_DEPTH)
	rcall InitStack 
;3.DEBUG marker form RAM layout defragmentation(marks end of tasks stack)

#ifdef DEBUG
    ldi temp,low(RAMEND)
	mov YL,temp
	ldi temp,high(RAMEND)
	mov YH,temp
    SUBI16 YL,YH,((@1-1)*TASK_STACK_DEPTH)+TASK_STACK_DEPTH-1    
	ldi temp,0xAA
	st Y,temp
#endif 


;2.Set stack pointer to stacks TCB
    ldi XL,low(@2)
	ldi XH,high(@2)    

	ldi ZL,low(((@1-1)*TASK_STACK_DEPTH)+TASK_STACK_REGISTER_DEPTH)
	ldi ZH,high(((@1-1)*TASK_STACK_DEPTH)+TASK_STACK_REGISTER_DEPTH)
    
	ldi r17,(@1-1)
	rcall InitTcb

.ENDMACRO 

;@INPUT: X task label
;        Z initial tasks stack pointer    
InitStack:
;position at the beginning of stack
    ldi temp,low(RAMEND)
	mov YL,temp
	ldi temp,high(RAMEND)
	mov YH,temp

	SUB16 YL,YH,ZL,ZH
;save inital return address to task begining
	mov r0,XL
	st Y,r0
	mov r0,XH
	st -Y,r0    
ret

;@INPUT: X - task control RAM block - TCB
;        Z - set the end of task stack = 32regs+SREAG+previous PC
;		 r17  - task ID	
InitTcb:
;position at the beginning of stack
    ldi temp,low(RAMEND)
	mov YL,temp
	ldi temp,high(RAMEND)
	mov YH,temp
;initial stack pointer position
    SUB16 YL,YH,ZL,ZH
    
	mov r0,YL
	st X+,r0
	mov r0,YH
	st X+,r0  
;set task ID
    ;ldi temp,@1     ;set task's ID
	st X,r17 
ret

;----------------------------------
;USER MODE API
;USAGE:	Yield task (relinquish the quantum) to the next one.
;		RETI will activate the interrupt
;----------------------------------
.MACRO  _YIELD_TASK
    cli
 	rcall TaskSchedular
.ENDMACRO

;-------------------------------------------------------
;USER MODE API
;Start executive mode - 
;1.elivate task to DEVICE_LEVEL
;2.disable scheduling 
;Use this in rear cases like 1-wire protocol.
;@WARNING: using _INTERRUPT_WAIT/_INTERRUPT_END is not allowed since DPC flag is always on!
;-------------------------------------------------------
.MACRO  _START_EXECUTIVE_MODE    
	push temp
	
	cli
	lds temp,InterruptDispatchFlag
	sbr temp,1<<@0
    sts InterruptDispatchFlag,temp
	sei

	_DISABLE_TASKSHEDUAL_TIMER
	pop temp
.ENDMACRO

.MACRO  _END_EXECUTIVE_MODE
	push temp

	cli
    lds temp,InterruptDispatchFlag  
	cbr temp,1<<@0
    sts InterruptDispatchFlag,temp 
	sei

	_ENABLE_TASKSHEDUAL_TIMER
	pop temp
.ENDMACRO

;----------------------------------------------------------
;USER MODE API
;USAGE:	Save time in units to wait on in current task TCB
;		Used in defined thread context
;USED:	temp,YL,YH(saved on current task stack)
;INPUT:   @0 - time in units
;WARNING:  NOT USED IN INTERRUPT or DEVICE level ,only current thread calls it while in PASSIVE mode!!!!!!!!!!!
;----------------------------------------------------------
.MACRO  _SLEEP_TASK
	push temp
	push YL
	push YH 

	lds YL, pxCurrentTCB
	ldi YH,0               ;keep TCB structures in first 256 bytes of RAM!

	ldi temp,@0
	std Y+3,temp

;***wait for it
wait:
;***yield processing
    _YIELD_TASK
    ldd temp,Y+3    
    tst temp
	breq end
    rjmp wait
end:

	pop YH
	pop YL 
	pop temp
.ENDMACRO



;----------------------------------------------------------
;USER MODE API
;USAGE:	Save time in units to wait on in current task TCB
;		Used in defined thread context.
;       Allows to implement simple PWM  
;USED:	16<rX<31,
;       YL,YH(saved on current task stack)
;       temp(saved on current task stack)  

;INPUT:   @0(16<rX<31) - time in units
;WARNING:  NOT USED IN INTERRUPT,only current thread calls it!!!!!!!!!!!
;----------------------------------------------------------
.MACRO _SLEEP_TASK_EX
	push YL
	push YH 
    push temp
	 

	lds YL, pxCurrentTCB
	ldi YH,0               ;keep TCB structures in first 256 bytes of RAM!

	std Y+3,@0

;***wait for it
wait:
;***yield processing
    _YIELD_TASK
    ldd temp,Y+3    
    tst temp

	breq end
    rjmp wait
end:
    pop temp
	pop YH
	pop YL 
.ENDMACRO
;----------------------------------------------------------
;KERNEL MODE API 
;USAGE:	Decrement timeout byte in each task's TCB if<>0
;		Walks through all TCB and decrements timeout byte
;		Fired by 16bit system timer      
;USED:	temp,YL,YH(saved on current task stack,temp is saved in _PRE_INTERRUPT)
;INPUT: none
;WARNING: USED IN INTERRUPT ONLY!!!
;---------------------------------------------------------- 

.MACRO _kePROCESS_SLEEP_INTERVAL
	push YL
	push YH 

	ldi YL,low(TCB_1)        
    ldi YH,high(TCB_1)

loop:
    ldd temp,Y+3
    tst temp
	breq skip
    dec temp
    std Y+3,temp    
skip:
	/*
	ldd temp,Y+2 
    andi temp,0x0F
	cpi temp,TASKS_NUMBER-1
	breq end
	*/	   
	cpi YL,low(TCB_1+(TASK_TCB_SIZE*(TASKS_NUMBER-1)))      
    breq end

	adiw YH:YL,TASK_TCB_SIZE
	rjmp loop  
end:
	pop YH
	pop YL 

.ENDMACRO
;-------------------------------------------------------------
;USER MODE API 
;USAGE:	 suspend current task or current task suspends itself.
;		 Sets current task schedulability bit to OFF.
;USED:	 YL,YH,temp(saved on current task stack)
;WARNING: CALLED IN CURRENT TASK CONTEXT 
;-------------------------------------------------------------
.MACRO _SUSPEND_TASK
    cli	
	push temp
	push YL
	push YH 
	
	lds YL, pxCurrentTCB
	ldi YH,0

    ;***modify SCHEDULABILITY bit    
	ldd temp,Y+2
    sbr temp,(1<<TASK_SCHEDULABLE_BIT)
	std Y+2,temp	   
    
	pop YH
	pop YL 
	pop temp	
	sei
    _YIELD_TASK           ;yield in task context only
.ENDMACRO
;@CHANGED-------------------------------------------------------------
;KERNEL and USER MODE API
;Sets any task schedulability bit to ON
;INPUT:  @0 - context{0=task;0xFF=interrupt}
;        @1 - task number#{1,2,3,4......n} 
;USED: XL,XH,temp(saved on current task stack)
;	   temp is saved on the stack by __PRE_INTERRUPT 
;WARNING:  CALLED IN ANY CONTEXT(arbitrary task) if in interrupt handler
;-------------------------------------------------------------
.MACRO _RESUME_TASK
    .IF @0==TASK_CONTEXT   ;no interrupt is possible if in interrupt,sei is NOT used
    cli
	push temp 
	.ENDIF
	
	push XL
	push XH
    ;calculate the target TCB and the byte offset where the scedualability bit is
    calculate_target_tcb_offset (@1-1)*TASK_TCB_SIZE,2

    ld temp,X	
    cbr temp,(1<<TASK_SCHEDULABLE_BIT)
    st X,temp

	pop XH
	pop XL 

	.IF @0==TASK_CONTEXT  ;no interrupt is possible if in interrupt, sei is NOT used
	pop temp
	sei
	.ENDIF
.ENDMACRO
;PRIVATE helper macro - never call outside of kernel.inc
;INPUT:  @0 - task number#{0,1,2,3,4......n}
;		 @1 - ofset within TCB 	 
;USED: XL,XH
.MACRO calculate_target_tcb_offset
	ldi XL,low((TCB_1+@0)+@1)
	ldi XH,high((TCB_1+@0)+@1)
.ENDMACRO

;*******************EVENT*************************************
;Event system for up to 8 events kept in EVENT_STATE RAM byte.Requires 1 bit per event.
;use event #{0-7} to identify the event.
;set 1 to event # to tell the rest,the event happened
;set 0 to event # to clear event
;Notification type of event - one task waits on it while many other tasks could signal(awaken) it. 
;It is possible to lose signal if many tasks signal the event

;----------------------------------------------------------------
;KERNEL AND USER MODE API
;INPUT: @0 - event #
;       @1 - context{0=task;0xFF=interrupt}
;USAGE: temp(saved on current task stack)
;----------------------------------------------------------------
.MACRO  _EVENT_SET 
    .IF @1==TASK_CONTEXT   ;no interrupt is possible if in interrupt sei is NOT used
    cli
	.ENDIF
	
    push temp
	
	lds temp,EVENTS_STATE
	sbr temp,1<<@0


	sts EVENTS_STATE,temp
	
	pop temp

	.IF @1==TASK_CONTEXT  ;no interrupt is possible if in interrupt sei is NOT used
	sei
	.ENDIF
.ENDMACRO



;USER MODE API
;Used in the context of a thread only!
;Wait indefinitly on an event.
;Auto reset event.
;INPUT: @0 - event #
;USAGE: temp(saved on current task stack)
;WARNING:  A new signal from onather task could come (if more then one task notifies the waiting task) between 
;          code points 1 and 2! The second signal will be swallowed up so mind this when more then one task notifies the waiting task!
            
.MACRO _EVENT_WAIT
    push temp

mloop:
	cli
	lds temp,EVENTS_STATE
	sbrc temp,@0
	rjmp event_is_set
	sei
	_YIELD_TASK
	rjmp mloop

event_is_set:               ;1  - a signal has come 
	cbr temp,1<<@0

	sts EVENTS_STATE,temp
	sei
	
	pop temp                  
.ENDMACRO

;USER MODE API
;Used in the context of a thread only!
;Manual reset event.
;INPUT: @0 - event #
;USAGE: temp(saved on current task stack)

.MACRO _EVENT_RESET
	push temp
	cli
	lds temp,EVENTS_STATE
	cbr temp,1<<@0
	sts EVENTS_STATE,temp
	sei
	pop temp
.ENDMACRO
;CHANGED**********************MUTEX*********************************
;It is more like a LOCK then MUTEX.
;A mutex object is a synchronization object whose state is set to signaled when it is not owned by any task, and nonsignaled when it is owned.
;Initial state is signaled - not owned by any task
;Wait for a mutex until it gets signaled.{0->signaled;
;                                          1->not signal}
;Up to 8 different mutexes could be defined in the static MUTEX_STATE byte.


;USER MODE API 
;		Set the flag to not signaled on entering the guarded section.
;		called in the context of a thread to mark the BEGIN of the protected code section.
;INPUT: @0 - mutex #
;USAGE: temp(saved on current task stack)

.MACRO   _MUTEX_WAIT
	push temp   

mloop:
	cli
   	lds temp,MUTEX_STATE
	sbrs temp,@0
	rjmp mutex_is_signaled
	sei
	_YIELD_TASK
    rjmp mloop 
mutex_is_signaled:

;set  MUTEX to not signaled    
	lds temp,MUTEX_STATE
	set
	bld temp,@0
	sts MUTEX_STATE,temp	
	sei	

	pop temp
.ENDMACRO

;USER MODE API 
;		Set the flag to signaled on exiting the guarded section.
;		called in the context of a thread after the call to _MUTEX_WAIT to mark the END of the protected code section.
;INPUT: @0 - mutex #
;USAGE: temp(saved on current task stack)

.MACRO   _MUTEX_RELEASE
	cli						;write-protect MUTEXT_STATE byte!	
	
	push temp 	

	lds temp,MUTEX_STATE
	clt
	bld temp,@0
	sts MUTEX_STATE,temp
	
	pop temp	

	sei
.ENDMACRO

;KERNEL API
;Context switch from one task to onather.
;There are 2 priority levels
;			1.DEVICE LEVEL processing
;			2.NORMAL LEVEL processing 
;A List queue like structure could have been used for the 2 priority levels but this implies RAM usage.
;Finds next task - round robin with 2 priority levels(DEVICE and NORMAL)
;LIMBO state - use any register yet mind that we are at the recently deschadualed task STACK region!
;WARNING:Having too many tasks executing in DEVICE priority level for a long time will leed to NORMAL level tasks starvation!
;@DANGER - the scheduler will loop forever if all tasks suspended at the same time - so keep one task(Idle Task) free of suspention as a fallback!
;@WORKAROUND - keep at least one/make sure you have at least one task free of suspention!Usualy System Task will do this for you

.MACRO _keSWITCH_TASK
;1.DEVICE level priority based in regard to IDF byte
devicelevel:
	lds temp,InterruptDispatchFlag
	tst temp
	breq normallevel    ; goto passivelevel
    ldi XL,low(IDT-1)
	ldi XH,high(IDT-1)
loop:
    adiw XH:XL,1
	lsl temp
	brcc loop
	
	ldi YH,high(TCB_1)   ;possible if all TCB structures reside within same 256 RAM page
	ld YL,X 
    
	;there is an interrupt but task to dispatch to is in sleep -> waik it up!	 
    clr temp    
	std Y+3,temp

	rjmp end


;2.NORMAL level round robin(eternal loop if no task is ready,will block if all tasks suspended)
normallevel:
	lds YL, pxCurrentTCB
	ldi YH,0

	ldd temp,Y+2	;position on priority bit byte
la:	
	andi temp,0x0F          ;isolate task ID
	cpi temp,TASKS_NUMBER-1
    brlo  laa
;round robin to first TCB
	ldi YL, low(TCB_1)
    ldi YH, high(TCB_1)	
    rjmp  lab
laa:
;round robin to next TCB
	adiw YH:YL,TASK_TCB_SIZE            
lab:    
	ldd temp,Y+2
	sbrc temp,TASK_SCHEDULABLE_BIT
    rjmp la	

end:	
	sts pxCurrentTCB,YL
.ENDMACRO



;**********************INTERRUPT PROCESSOR***************************
/*
What is interrupt Processing Task - a task dedicated to processing an interrupt.
The task uses 2 APIs:
   1._INTERRUPT_WAIT - waits on interrupt that will come from an interrupt dispatch.
   2._INTERRUPT_END  - called at the end of the task.It lowers the task priority level and makes 
                       the task schedulable so it can continue to execute at PASSIVE level. 
The portion of code confined within these 2 macros is executed in DEVICE priority level.
A task executes at DEVICE level until it volantarily exits it through _INTERRUPT_END macro. 
It is a good practise to disable the respective interrupt source during the current interrupt processing. 
@WARNING:looping forever between _INTERRUPT_WAIT and _INTERRUPT_END(while in DEVICE level) will break the kernel!
*/
;--------------------------------------------------------------------
;KERNEL MODE API
;Activates the "interrupt processing task" by setting the corresponding 
;interrupt dispatch flag(index).
;Executed as atomic operation - only within interrupt procedure
;Used in interrupt context which happens in arbitrary thread context.
;Mind register usage!!!!
;INPUT: @0 - interrupt dispatch priority index
;USED: r17,temp(saved on current arbitrary task stack)
;@EXAMPLE:

;intX:
;  _PRE_INTERRUPT

  ;Do your stuff here (disable interrupt most likely...).Don't forget to push/pop used registers 
  
;  _keDISPATCH_INTERRUPT

;  _POST_INTERRUPT

;   reti

  ;as you can see THERE IS a reti instraction!Task scheduling timer is set to activate descheduling  
  ;where the real dispatching occure. Dispatching may not happen if interrupt of higher priority 
  ;is being processed.
;---------------------------------------------------------------------
/*
.MACRO _keDISPATCH_INTERRUPT
	;push temp  saved in _PRE_INTERRUPT macro ->save stack usage!              ;
	push r17

	lds temp,InterruptDispatchFlag
    mov r17,temp
	sbr temp,1<<@0
    sts InterruptDispatchFlag,temp
	cpi r17,1<<@0
    brsh  nodeschedual    

    _SET_TASKSHEDUAL_TIMER	temp     ;DESCHEDUAL only if higher priority

nodeschedual:

    pop r17
    ;pop temp   restored in _POST_INTERRUPT macro       
.ENDMACRO
*/
;-----------------------------------------------------------------
;USER MODE API
;Registers the calling task with Interrupt Dispatcher.
;Makes the calling task - interrupt handling one.
;Must be called in the INIT part of the task while the interrupt 
;source is still disabled.
;INPUT:
;        @0 - temp register,
;        @1 - interrupt dispatch priority index(7/highest/ through 0/lowest/)
;-----------------------------------------------------------------

.MACRO _INTERRUPT_DISPATCHER_INIT
	lds @0,pxCurrentTCB
	sts IDT+(7-@1),@0    
.ENDMACRO

;-----------------------------------------------------------------------
;USER MODE API
;Wait on interrupt dispatch.
;Used in "interrupt processing task",activated by INTERRUPT DISPATCH API
;INPUT: @0 - interrupt dispatch priority index
;USED: YL,YH,temp(saved on current task stack)
;-----------------------------------------------------------------------
.MACRO  _INTERRUPT_WAIT
	push temp
	push YL
	push YH 

	lds YL, pxCurrentTCB
    ldi YH, 0
	
;1.make task unscheduleable
    cli
	ldd temp,Y+2
	sbr temp,(1<<TASK_SCHEDULABLE_BIT)
	std Y+2,temp
	sei
;2.wait for INTERRUPT DISPATCH priority flag
wait:
	lds temp,InterruptDispatchFlag 
    sbrc temp,@0 
	rjmp end
;***yield processing
    _YIELD_TASK
    rjmp wait
end:
	pop YH
	pop YL 
	pop temp
.ENDMACRO

;-----------------------------------------------------------------
;USER MODE API
;Reduce current task level form TASK_PRIORITY_INTERRUPT to TASK_PRIORITY_NORMAL
;Used at the end of each "interrupt processing task"!!!!
;INPUT: @0 - interrupt dispatch priority index
;USED: YL,YH,temp(saved on current task stack)
;-----------------------------------------------------------------
.MACRO _INTERRUPT_END	
	push temp
	push YL
	push YH 
    cli
	
	lds YL, pxCurrentTCB
	ldi YH,0
	
    		
	;1.***modify SCHEDULABILITY bit    
	ldd temp,Y+2
    cbr temp,(1<<TASK_SCHEDULABLE_BIT) ;set to 0=activate task(we want to run in NORMAL LEVEL where active flag is checked by the schedular)
	std Y+2,temp    

	;2 *** mark IDF off
    lds temp,InterruptDispatchFlag  
	cbr temp,1<<@0
    sts InterruptDispatchFlag,temp 
    
	sei
	pop YH
	pop YL 
	pop temp
	_YIELD_TASK    ;Add YIELD_TASK so that waiting DPC of lower priority may be executed
.ENDMACRO

;-----------------------------------------------------
;Kernel Mode API
;Call at the beginning of interrupt handler procedure
;INPUT: none
;USAGE: temp
;-----------------------------------------------------
.MACRO _PRE_INTERRUPT
    push temp
	in temp,SREG
    push temp	
.ENDMACRO

;-----------------------------------------------------
;Kernel Mode API
;Call at the end of interrupt handler procedure
;INPUT: none
;USAGE: temp 
;-----------------------------------------------------
.MACRO _POST_INTERRUPT
	pop temp
    out SREG,temp
	pop temp
.ENDMACRO 

/*
@EXPERIMENTAL
In search of a faster interrupt dispatcher to dispatch to interrupt handling task as fast as possible
Differered Procedure Call/DPC semantic
@EXAMPLE:

intX:
  _PRE_INTERRUPT

  ;Do your stuff here (disable interrupt most likely...).Don't forget to push/pop used registers 
  ;never forget that interrupt happens in arbitrary tasks context!
  _keDISPATCH_DPC

  Two points to note:
  1.DPC is queued.Interrupt handling is sort of extended to task schedular where the real dispatching occure.(RET is used) 
  2.DPC is queued.Dispatching may not happen if interrupt of higher priority  is being processed. (RETI)
*/

.MACRO _keDISPATCH_DPC
	;push temp  saved in _PRE_INTERRUPT macro ->save stack usage!
	
	push r17
	lds temp,InterruptDispatchFlag
    mov r17,temp
	sbr temp,1<<@0
    sts InterruptDispatchFlag,temp
	cpi r17,1<<@0
    pop r17
	
	brsh  nodeschedual       ;avoid dispatching if DPC of higher priority is being processed
	/*
	ldi temp,low(DispatchDPCExtension)
    push temp
	ldi temp,high(DispatchDPCExtension)
	push temp
    ret           ;1.deschedual/dispatch right away - notice that interrupt flag in SREG is still OFF
	              ;reti in TaskSchedular will enable it              
				  ;2.temp and SREG will be restored between labels "DispatchDPCExtension" and "TaskSchedular"

    */
	pop temp      ;restore temp and SREG to its task's context value/make sure _PRE_INTERRUPT is invoked first!!!!!!!!!!!!!
    out SREG,temp
	pop temp

	jmp DispatchDPCExtension

nodeschedual:    
    pop temp      ;restore temp and SREG to its task's context value/make sure _PRE_INTERRUPT is invoked first!!!!!!!!!!!!!
    out SREG,temp
	pop temp
	reti          ;return to interrupted task  
.ENDMACRO 



.dseg
/*
Interrupt processing is executed at DEVICE level as DPC (Deffred Procedure call).
Instead of puting processing code in the regular interrupt handler, we dispatch the 
processing to a DPC which is executed a bit later at DEVICE level.
Interrupt Dispatch Table (IDT) keeps record of all registered interrupt processing tasks by preserving LSB address of TCB.
Most of the AVRs has SRAM starting from 0x060 but others like AT90USB162 has it at 0x100.
KEEPING ONLY THE LSB WORKS FOR ALL AVRs IF TCB_1 - TCB_16 RESIDE WITHIN SAME 256 byte PAGE=>MSB IS THE SAME, SO NO NEED TO KEEP IT,JUST EXTRACT IT FROM TCB_1 
Interrupt activity flag records which interrupt is being processed(active) or pending processing. 
Interrupt processing tasks are executed at DEVICE level. The processor remains at 
DEVICE level until all present interrupts are handled.
Each item in ITD will keep Task ID that is meant to process the interrupt.
IDT dispatches interrupts to tasks in sequential order from the highest to lowest index.
Each index is selected by the schedular untill the interrupt processing task behind it is finished - which basically masks
the processing of the interrupts of lower index value.

---------------------
1. Interrupt Index Activity        
------IDT------------
N. Task ID 
---------------------
*/

InterruptDispatchFlag:           .byte 1 ;up to 8 interrupt processing tasks

IDT:   							 .byte 8

EVENTS_STATE: .byte 1           ;up to 8 events applic;ation wide

MUTEX_STATE: .byte 1            ;up to 8 mutexes application wide 

#ifdef THRESHOLD_BARRIER_EXTENTION
.include "ThresholdBarrier.inc" 
;BARRIER straucture
; ________________
;|y|-|-|-|x|x|x|x|
;~~~~~~~~~~~~~~~~~
;y->leader bit
;---not used 
;xxxx ->total number of tasks to wait on the barrier
InitTasksBarrier:   .byte 1
#endif

/*
Extended Event system definition
*/
#ifdef EVENT_EXTENTION
.include "EventExt.inc"  
;Extended event straucture
; ________________
;|y|-|-|-|v|v|v|v|
;|-|-|-|-|t|t|t|t|
;~~~~~~~~~~~~~~~~~
;y  	->signaled/not signaled bit
;-  	->not used 
;vvvv 	->next valid ticket number
;tttt	->ticket number




GUI_EVENT:           .byte 2 
#endif

/*
Lock synchronization definition
*/
#ifdef LOCK_EXTENTION
.include "Lock.inc"

datalock:            .byte 2

#endif

/*
Cyclic Barrier synchronization definition 
*/
#ifdef CYCLIC_BARRIER_EXTENTION
.include "CyclicBarrier.inc"

demobarrier:         .byte 1

#endif

/*
Latch synchronization definition 
*/
#ifdef LATCH_EXTENTION
.include "CountDownLatch.inc"

demolatch:         .byte 1

#endif

/*
SLEEP mode
*/
#ifdef SLEEP_MODE
.include "SLEEP.inc"
#endif

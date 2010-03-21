;---------------------------------------------------------------------------
; FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
;---------------------------------------------------------------------------

	.sect ".int02"
	.short SWI0	

; Global data import	
	.global sCriticalCount
	.global pstCurrentTask
	
; global function export	
	.global Task_StartTasks
	.global SWI0
	.global Tick
	
; global function import	
	.global Task_Tick
	.global Task_TimerISR
	.global Task_Switch
	.global Task_SetScheduler
	.global KernelSWI_Clear

;---------------------------------------------------------------------------
;! Save the context of the task
Task_SaveContext	.macro
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
    mov.w	&pstCurrentTask, r12
    mov.w	r1, 0(r12)
	.endm

;---------------------------------------------------------------------------
;! Restore the context of the task
Task_RestoreContext	.macro
	mov.w  &pstCurrentTask, r12
	mov.w  @r12, r1
	pop  r15
	pop  r14
	pop  r13
	pop  r12
	pop  r11
	pop  r10
	pop  r9
	pop  r8
	pop  r7
	pop  r6
	pop  r5
	pop  r4
    bic.w  #0x00F0, 0(SP)
	.endm
	
	.text
	
;---------------------------------------------------------------------------
; Start the task
Task_StartTasks:
	mov.b	#1,	r12
	call #Task_SetScheduler
	call #Task_Switch
	Task_RestoreContext
	reti

;---------------------------------------------------------------------------
; Software interrupt

SWI0:	
	Task_SaveContext
	call #KernelSWI_Clear	
	call #Task_Switch
	Task_RestoreContext
	reti

	.end
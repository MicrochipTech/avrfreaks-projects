;---------------------------------------------------------------------------
; FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
;---------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Macro for saving a task's context
;-----------------------------------------------------------------------------
.macro TASK_SAVE_CONTEXT
	   st -y, r0
	   in r0, 0x3F
	   cli
	   st -y, r0
	   in r0, 0x3D
	   st -y, r0
	   in r0, 0x3E
	   st -y, r0
	   st -y, r1
	   st -y, r2
	   st -y, r3
	   st -y, r4
	   st -y, r5
	   st -y, r6
	   st -y, r7
	   st -y, r8
	   st -y, r9
	   st -y, r10
	   st -y, r11
	   st -y, r12
	   st -y, r13
	   st -y, r14
	   st -y, r15
	   st -y, r16
	   st -y, r17
	   st -y, r18
	   st -y, r19
	   st -y, r20
	   st -y, r21
	   st -y, r22
	   st -y, r23
	   st -y, r24
	   st -y, r25
	   st -y, r26
	   st -y, r27
	   st -y, r30
	   st -y, r31
	   lds r26, _pstCurrentTask
	   lds r27, _pstCurrentTask + 1
	   st	x+, r28
	   st	x+, r29
.endmacro

;-----------------------------------------------------------------------------
; Macro to restore a task's context
;-----------------------------------------------------------------------------
.macro TASK_RESTORE_CONTEXT
	   lds r26, _pstCurrentTask
	   lds r27, _pstCurrentTask + 1
	   ld r28, x+
	   ld r29, x+
	   ld r31, y+
	   ld r30, y+
	   ld r27, y+
	   ld r26, y+
	   ld r25, y+
	   ld r24, y+
	   ld r23, y+
	   ld r22, y+
	   ld r21, y+
	   ld r20, y+
	   ld r19, y+
	   ld r18, y+
	   ld r17, y+
	   ld r16, y+
	   ld r15, y+
	   ld r14, y+
	   ld r13, y+
	   ld r12, y+
	   ld r11, y+
	   ld r10, y+
	   ld r9, y+
	   ld r8, y+
	   ld r7, y+
	   ld r6, y+
	   ld r5, y+
	   ld r4, y+
	   ld r3, y+
	   ld r2, y+
	   ld r1, y+
	   ld r0, y+
	   out 0x3E, r0
	   ld r0, y+
	   out 0x3D, r0
	   ld r0, y+
	   out 0x3F, r0
	   ld r0, y+ 
.endmacro

;-----------------------------------------------------------------------------
; Interrupt handler for the RTOS Task switch SWI
;-----------------------------------------------------------------------------
.area text
_RTOS_TASK_HANDLER::
	   TASK_SAVE_CONTEXT
	   CALL _Task_Switch
	   TASK_RESTORE_CONTEXT
	   reti
	   
;-----------------------------------------------------------------------------
; Necessary to get the module to link in
;-----------------------------------------------------------------------------
_LINKME::
       ret
	   
;-----------------------------------------------------------------------------
; Assembler interrupt vector table
;-----------------------------------------------------------------------------
.area interrupt_vectors(abs) 
        .org 4				 	  ; INT0 vector address 
		JMP _RTOS_TASK_HANDLER	  ; Assign the ISR to the vector 	 
	 	   	    
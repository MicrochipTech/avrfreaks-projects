#include        "avrx.inc"
/*
Copyright ©1998 - 2002 Larry Barello (larry@barello.net)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA  02111-1307, USA.

http://www.gnu.org/copyleft/lgpl.html


        Revision History
20020907 - Added _SFR_IO_ADDR() macro to all I/O accesses for GCC 3.2
20020603- Initial rewrite of _Prolog to skip context push when in idle task.
	- modify _QueuePid to return status if top of run queue changed.

20010515- Added support for IAR c_stack (initialize Y in task def)
20010403- Ported to avrx-ctoasm.inc

20010307- Moved AvrXStack into RAM variable so it can be set from main().
        - Added function AvrXSetKernelStack

*/
        _MODULE(avrx_tasking)
;+
;----------------------------------------------------
; Critical Data Structures (now a data structure, see avrx.inc for offsets
;
; - RunQueue is the head of the run queue.
; - Running is current running task - once inside a Prolog/Epilog
;   section, the RunQueue might get re-arranged so we need to track
;   who we are.
; - Syslevel is a counter tracking how many times we have reentered
;   the kernel, or, how many interrupts are nested (same thing).
; - Offsets are for index access to kernel data - this is probably
;   broken for IAR and maybe even GCC if the assemblers don't pack
;   the data on byte boundaries.
;-
        _DATASECTION
	_GLOBAL(AvrXKernelData, AvrXKernelDataSz)
        _CODESECTION
;+
; --------------------------------------------------
; void IntProlog(void)
;
; Pushes entire register context onto the stack, returning a frame pointer
; to the saved context.  If running in user mode upon entry (SysLevel == 0xFF)
; then switches the stack to the kernel and stores the frame pointer in the
; current processes PID.
;
; PASSED:       Nothing
; RETURN:       Y = Frame Pointer (See AvrX.inc)
;               R0 = pid.state (Single Step Flag) or 0
; ASSUMES:
; USES:         R14-R18, R26-R32, SysLevel
; CALLS:
; NOTES:        Does not muck with R16-R25 so passed parameters from C to
;               AvrX functions can be accessed directly after a call to Prolog.
;
;		Special case for IDLE: Don't save context or restore context when
;		transitioning from USER to Kernel context - it has no context when in
;		in user mode.  Don't put any code into the IDLE task - it will break!
;
;-
	_FUNCTION(IntProlog)

IntProlog:                      ; 3 cycles
	push	R29
	push	R28
	push	R27
	push	R26
	push	R25

	ldi	Yh, hi8(AvrXKernelData)
	ldi	Yl, lo8(AvrXKernelData)

	in	R25, _SFR_IO_ADDR(SREG)		; Save flags

	ldd	Xl, Y+SysLevel
	subi	Xl, lo8(-1)		; Carry set if results 0
	std	Y+SysLevel, Xl		; if already in kernel, then save context
	brcs	SaveContext

	ldd	Xl, Y+Running+NextL	; Check if current task = IDLE
	ldd	Xh, Y+Running+NextH
	or	Xh, Xl
	brne	SaveContext		; Carry cleared if results 0
        ;
        ; When interrupting IDLE, just reset the stack pointer to PRIOR the interrupt.
        ; in Epilog, if still IDLE, control will return to the start of the IDLE loop.
        ; I.E. we don't need the return address to IDLE, just the return address to the
        ; caller of Prolog() (the interrupt handler).
        ;
        ; The code, below, is obscure, but cycle efficient.  The net effect is to reset
        ; the stack to AvrXStack before returning to the interrupt handler.
        ;
        in	Yl, _SFR_IO_ADDR(SPL)	;Idle task: don't bother saving registers, etc.
	in	Yh, _SFR_IO_ADDR(SPH)
	ldd	Zh, Y+6         	; Get return address
	ldd	Zl, Y+7
	adiw	Yl, 9			; Adjust pointer
	out	_SFR_IO_ADDR(SPL), Yl  	; This is cycle efficient, but obscure.
	out	_SFR_IO_ADDR(SPH), Yh
	ijmp				; ~37 cycles for IDLE task.

SaveContext:
	push	R24
        push    R23
        push    R22
        push    R21
        push    R20
        push    R19
        push    R18
        push    R17
        push    R16
        push    R15
        push    R14
        push    R13
        push    R12
        push    R11
        push    R10
        push    R9
        push    R8
        push    R7
        push    R6
        push    R5
        push    R4
        push    R3
        push    R2
        push    R1
        push    R0
        push    R25		; SREG

	in	Yl, _SFR_IO_ADDR(SPL)
	in	Yh, _SFR_IO_ADDR(SPH)         ; Grab frame pointer

	ldd	R25, Y+_R25	; Need to restore this register (used for SREG)
	ldd	tmp1, Y+_RetLo
	ldd	tmp2, Y+_RetHi
        std     Y+_R30, Zl
        std     Y+_R31, Zh      ; Swap return address with Z

#ifdef SINGLESTEPSUPPORT
	ldi	Xl, 0
        mov	R0, Xl
#endif /* SINGLESTEPSUPPORT */
	brcs	AlreadyInKernel ; Flags, still good, from above.

	ldi	Zh, hi8(AvrXKernelData)
	ldi	Zl, lo8(AvrXKernelData)
	ldd	Yh, Z+Running+NextH    ; Point to running task
	ldd	Yl, Z+Running+NextL
	in	Xh, _SFR_IO_ADDR(SPH)
	in	Xl, _SFR_IO_ADDR(SPL)
	std	Y+PidSP+NextH, Xh
	std	Y+PidSP+NextL, Xl

#ifdef  SINGLESTEPSUPPORT
        ldd     tmp0, Y+PidState ; Return to caller SingleStepFlag.
        sbrs    tmp0, SingleStep
        rjmp    SkipDequeue

        ldd     R1, Y+PidNext+NextH  ; Dequeue current task
        std     Z+RunQueue+NextH, R1
        ldd     R1, Y+PidNext+NextL
        std     Z+RunQueue+NextL, R1
        mov     R0, tmp0        ; Return to caller SingleStepFlag.
        cbr     tmp0, BV(SingleStep)
        std     Y+PidState, tmp0 ; Clear flag in PID
SkipDequeue:
#endif /* SINGLESTEPSUPPORT */

        ldd	tmp0, Z+AvrXStack+NextL
	out	_SFR_IO_ADDR(SPL), tmp0
	ldd	tmp0, Z+AvrXStack+NextH
	out	_SFR_IO_ADDR(SPH), tmp0       ; Swap to kernel stack
	mov	Yl, Xl
	mov	Yh, Xh		; restore frame pointer

AlreadyInKernel:                ; (85/102)
	clr     R1              ; R1 = __Zero__ for Avr-gcc
        mov     Zl, tmp1        ; scratch in IARICC
        mov     Zh, tmp2
	ijmp			; Return to caller (89/106)
        _ENDFUNC

/*+
; --------------------------------------------------
; _Epilog
;
; Restore previous context (kernel or user).
; If task has SingleStep flag set, then generate an interrupt
; before returning to the task.
;
; PASSED:
; RETURN:
; ASSUMES:      SysLevel >= 0 (running on kernel stack)
; USES:         SysLevel
; CALLS:
-*/
        _EXTERN(GenerateInterrupt)
        _FUNCTION(Epilog)
Epilog:
        pop     R30
        pop     R30
        _ENDFUNC
	_FUNCTION(_Epilog)
_Epilog:
	ldi	Zl, lo8(AvrXKernelData)
	ldi	Zh, hi8(AvrXKernelData)
        BeginCritical
        ldd     R16, Z+SysLevel    ; Interrupts off..
        dec     R16
        std	Z+SysLevel, R16
        brge    SkipTaskSwap

        ldd     Yh, Z+RunQueue+NextH
        ldd     Yl, Z+RunQueue+NextL
        std     Z+Running+NextH, Yh
        std     Z+Running+NextL, Yl   ; Update current running task
        adiw    Yl, 0
        breq    _IdleTask

        ldd     Xh, Y+PidSP+NextH
        ldd     Xl, Y+PidSP+NextL
        out     _SFR_IO_ADDR(SPL), Xl
        out     _SFR_IO_ADDR(SPH), Xh         ; 20 cycles
#ifdef  SINGLESTEPSUPPORT
        ldd     R0, Y+PidState  ; X, Y and R0 available
        sbrc    R0, SingleStep
        rcall   GenerateInterrupt
#endif
SkipTaskSwap:                   ; 20/6
        pop     R0
        out     _SFR_IO_ADDR(SREG), R0
        pop     R0
        pop     R1
        pop     R2
        pop     R3
        pop     R4
        pop     R5
        pop     R6
        pop     R7
        pop     R8
        pop     R9
        pop     R10
        pop     R11
        pop     R12
        pop     R13
        pop     R14
        pop     R15
        pop     R16
        pop     R17
        pop     R18
        pop     R19
        pop     R20
        pop     R21
        pop     R22
        pop     R23
        pop     R24
        pop     R25
        pop     R26
        pop     R27
        pop     R28
        pop     R29
        pop     R30
        pop     R31
        EndCriticalReturn       ; 97/83 cycles with interrupts off

; Jump here if there are no entries in the _RunQueue.  Never return.  Epilog will
; take care of that.  NB - this code has *NO* context.  Do not put anything in
; here that uses registers as you will get hosed every time an interrupt occurs.

_IdleTask:
;	ldd	Yl, Z+AvrXStack+NextL
;	ldd	Yh, Z+AvrXStack+NextH
;	out	SPL, Yl
;	out	SPL, Yh		; Reset kernel stack
        BeginIdle               ; Any interrupt will EndIdle
        EndCritical             ; Enable interrupts
        sleep                   ; Power Down..
        EndIdle
        rjmp    _IdleTask
        _ENDFUNC

/*+
;-------------------------------------------------
; void * AvrXSetKernelStack(char * newstack);
;
; PASSED: Pointer to end of new stack or NULL
; RETURN: pointer to end of stack
;
-*/
        _FUNCTION(AvrXSetKernelStack)

AvrXSetKernelStack:
        pop     R31
        pop     R30
        subi    p1l, 0		; May not be a high pair in IAR
        sbci    p1h, 0
        brne    sks1
        in      p1l, _SFR_IO_ADDR(SPL)
        in      p1h, _SFR_IO_ADDR(SPH)
sks1:
        sts     AvrXKernelData+AvrXStack+NextL, p1l
        sts     AvrXKernelData+AvrXStack+NextH, p1h
        ijmp

        _ENDFUNC
/*+
; --------------------------------------------------
; *PID AvrXInitProcess(*TaskControlBlock)
; *PID AvrXRunTask(*TaskControlBlock)
;
; Initialize the PID based upon the TCB table.  Run Task
; "resumes" the process as well
;
; PASSED:       R25:R24 = TCB
; RETURNS:      R25:R24 = PID
; USES:         X, Z and R0
; CALLS:
; NOTES:        TCB, Layout, below:
;               2 PID
;               1 Flags
;               2 STACK
;               2 Entry
-*/
        _FUNCTION(AvrXRunTask)

AvrXRunTask:
        rcall   AvrXInitTask
        rjmp    AvrXResume
        _ENDFUNC

        _FUNCTION(AvrXInitTask)

AvrXInitTask:
#ifdef __IAR_SYSTEMS_ASM__
        mov     tmp0, Xl
        mov     tmp1, Xh
#endif
        mov     Zl, p1l
        mov     Zh, p1h

        rcall   lpm_inc         ; Get Stack pointer
        mov     Xl, R0
        rcall   lpm_inc
        mov     Xh, R0

#ifdef  TASKEXIT
        _EXTERN(AvrXTaskExit)
        ldi     tmp2, lo8_pm(AvrXTaskExit)
        st      X, tmp2          ; !Pre-dec is not a PUSH!
        ldi     tmp2, hi8_pm(AvrXTaskExit)
        st      -X, tmp2          ; Push task exit onto stack

        rcall   lpm_inc
        st      -X, R0
        rcall   lpm_inc
        st      -X, R0          ; Push task entry point
#else
        rcall   lpm_inc
        st      X, R0
        rcall   lpm_inc
        st      -X, R0          ; Push task entry point
#endif /* TASKEXIT */

        ldi     tmp2, 0
#ifdef __IAR_SYSTEMS_ASM__
        st      -x, tmp2        ; R31
        st      -x, tmp2        ; R30
        rcall   lpm_inc
        mov     tmp3, R0
        rcall   lpm_inc
        st      -x, R0          ; Frame is stored in reverse order
        st      -x, tmp3        ; lo8(c_stack)
        ldi     tmp3, 29        ; R0-R27 and SREG get "pushed"
#else
        ldi     tmp2, 0
        ldi     tmp3, 33        ; R0-R31 and SREG get "pushed"
#endif
PushRegisters:
        st      -X, tmp2
        dec     tmp3            ; Fill rest of context with Zeros
        brne    PushRegisters   ; Decrement stack pointer

        sbiw    Xl, 1           ; because pre dec is not a push!

        rcall   lpm_inc
        mov     r1l, R0          ; Get PID -> R23:22
        rcall   lpm_inc
        mov     r1h, R0
        rcall   lpm_inc         ; Priority -> R0
        mov     Zl, r1l
        mov     Zh, r1h

        std     Z+PidSP+NextL, Xl
        std     Z+PidSP+NextH, Xh     ; Stash context frame pointer

        std     Z+PidPriority, R0
        ldi     Xl, BV(SuspendedBit) | BV(SuspendBit)
        std     Z+PidState, Xl  ; Mark state so we can "resume" pid
        std     Z+PidNext+NextH, tmp2
        std     Z+PidNext+NextL, tmp2; Clear pid.next
#ifdef __IAR_SYSTEMS_ASM__
        mov     Xh, tmp1
        mov     Xl, tmp0
#endif
        ret

lpm_inc:                        ; Tiny helper...
        lpm
        adiw    Zl, 1
        ret
        _ENDFUNC
/*+
; --------------------------------------------------
; void AvrXResume(pProcessID)
;
; Takes a PID and inserts it into the run queue.
;
; Uses two flags, Suspended and Suspend to determine if a task is
; actually suspended, as opposed to just being marked for suspension.
; Mark _SUSP to make QueuePid not queue, Mark _SUSPENDED when attempt
; to queue is made (e.g. not blocked on a semaphore).  This is because
; we can't "resume" something that is blocked on a semaphore, even though
; it is marked for suspension.
;
; PASSED:       R25:24 = PID to queue
; RETURNS:
; USES:
; ASSUMES:
; NOTES:
-*/
        _FUNCTION(AvrXResume)

AvrXResume:                     ; User Entry Point
        AVRX_Prolog

        mov     Zl, p1l
        mov     Zh, p1h

        ldd     tmp0, Z+PidState
        cbr     tmp0, BV(SuspendBit)
        std     Z+PidState, tmp0  ; clear Suspend flag

        sbrs    tmp0, SuspendedBit
        rjmp    ar00

        cbr     tmp0, BV(SuspendedBit)  ; If removed from run queue,
        std     Z+PidState, tmp0  ; Clear flag and queue it up.

        rcall   _QueuePid       ; If found, then add to run queue
ar00:
        rjmp    _Epilog
        _ENDFUNC
/*+
; --------------------------------------------------
; _QueuePid
;
; Takes a PID and inserts it into the run queue.  The run queue is sorted
; by priority.  Lower numbers go first.  If there are multiple tasks of equal
; priority, then the new task is appended to the list of equals (round robin)
;
; PASSED:       p1h:p1l = PID to queue
; RETURNS:      r1l:	-1 = suspended
;			0  = Top of run queue
;			1-N= Depth in run queue
; USES:         Z, tmp0-3 and SREG, RunQueue
; ASSUMES:
; NOTES:        Returns with interrupts on.
;
-*/
        _FUNCTION(_QueuePid)

_QueuePid:                              ; Kernel entry point only
        mov     Zl, p1l
        mov     Zh, p1h
        ldi	tmp1, lo8(-1)

        ldd     tmp0, Z+PidState          ; Xh = Priority & Flags
        andi    tmp0, (BV(SuspendBit) | BV(IdleBit)) ; if marked Suspended or idle
        brne    _qpSUSPEND

        push	Yl
        mov	tmp0, Yh		; preserve Y

        ldd     tmp2, Z+PidPriority
        ldi     Yl, lo8(AvrXKernelData+RunQueue)
        ldi     Yh, hi8(AvrXKernelData+RunQueue)
        BeginCritical
_qp00:
	inc	tmp1     		; Tmp1 = counter of insertion point.
    	mov     Zl, Yl                 	; 0 = head of run queue.
        mov     Zh, Yh
        ldd     Yl, Z+PidNext+NextL
        ldd     Yh, Z+PidNext+NextH     ; Z = current, X = Next
        adiw    Yl, 0
        breq    _qp01                   ; End of queue, continue
        ldd     tmp3, Y+PidPriority
        cp      tmp2, tmp3
        brsh    _qp00                   ; Loop until pri > PID to queue
_qp01:
        std     Z+NextH, p1h
        std     Z+NextL, p1l    ; Prev->Next = Object
        mov     Zh, p1h
        mov     Zl, p1l
        std     Z+NextH, Yh     ; Object->Next = Next
        std     Z+NextL, Yl
        pop     Yl
        mov     Yh, tmp0
        mov	r1l, tmp1
        EndCriticalReturn

_qpSUSPEND:
	mov	r1l, tmp1
        sbr     tmp0, BV(SuspendedBit)  ; Mark suspended and return
        std     Z+PidState, tmp0
        EndCriticalReturn

        _ENDFUNC
        _END


#include        "avrx.inc"
/*
	Copyright 2002, Larry Barello
	larry@barello.net

        Revision History
	20020604 - Modified to use AvrXKernelData structure
*/
        _MODULE(avrx_reschedule)
;+-------------------------------------------------------------------
;
; void AvrXIntReschedule(void)
;
; Yield function: to allow cooperative tasking amoung like priority tasks.
;
; NOTE: Can only be called from AvrX Kernel mode (e.g. inside an interrupt 
; handler).  Reschedules the top of the run queue, which, as noted below, may
; not actually be the current running task.
;
AvrXIntReschedule:
	lds	Zl, AvrXKernelData+RunQueue+NextL	; Grab the top of the run queue
	lds	Zh, AvrXKernelData+RunQueue+NextH
	adiw	Zl, 0
	brne	1f
	ret				; Exit if empty
1:
	ldd	p1l, Z+NextL		; Point to the next
	ldd	p1h, Z+NextH
	sts	RunQueue+NextL, p1l
	sts	RunQueue+NextH, p1h
	rjmp	_QueuePid

;+------------------------------------------------------------------
;
; void AvrXYield(void)
;
; Removes self from run queue and re-queues.  If other tasks of the 
; same priority are on the queue, self will queue behind them (round robin)
;
; Note: we remove "Running" which may or may not be the top of the queue 
; depending upon whatever other interrupt activity might be going on.  If 
; we just removed the top of the run queue, it would work virutally all the 
; time, but technically, it could fail to reschedule the correct task (only 
; once in a blue moon...)
;-

	_FUNCTION(AvrXYield)

AvrXYeild:
	AVRX_Prolog
	ldi	Zl, lo8(AvrXKernelData+RunQueue)
	ldi	Zh, hi8(AvrXKernelData+RunQueue)
	ldd	p2l, Z+Running+NextL
	ldd	p2h, Z+Running+NextH
	rcall	_RemoveObject		; Can't fail, so don't bother checking
	EndCritical
	mov	p1l, p2l
	mov	p1h, p2h
	rcall	_QueuePid		; requeue ourselves
	rjmp	_Epilog			; jump to the new top of the queue

	_ENDFUNC

	_END

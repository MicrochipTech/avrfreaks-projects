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

        20020604 - Modified to use AvrXKernelData structure

*/
        _MODULE(avrx_terminate)
        _EXTERN(IntProlog)
        _EXTERN(_RemoveObject)
        _EXTERN(AvrXKernelData)
        _EXTERN(_Epilog)
        _EXTERN(AvrXIntSetObjectSemaphore)

        _CODESECTION

/*+
; --------------------------------------------------
; AvrXTaskExit
; AvrXTerminate
;
; This is where tasks that "return" come to.  Set the IDLE bit so
; it can not be queued again.  Then attempt to Dequeue from run queue.
; Can not do anything more than that if it is queued on a semaphore...
;
; PASSED:       R25:R24 = Pid to terminate
; RETURNS:
; USES:
; CALLS:
; ASSUMES:
; NOTES:
-*/
        _FUNCTION(AvrXTaskExit)

AvrXTaskExit:                   ; Entry point for killing self
        AVRX_Prolog             ; Entire state of dying task is on stack
        lds     p1h, AvrXKernelData+Running+NextH
        lds     p1l, AvrXKernelData+Running+NextL
        rjmp    at00
        _ENDFUNC

        _FUNCTION(AvrXTerminate)
AvrXTerminate:                  ; Entry point for killing other tasks.
        AVRX_Prolog
at00:
;       rcall   AvrXIntSetObjectSemaphore       ; Later: Signal parent task
;       subi    p1l, lo8(-2)
;       sbci    p1h, hi8(-2)
        mov     Zh, p1h
        mov     Zl, p1l
        ldi     Xl, BV(IdleBit) ; Mark task dead
        std     Z+PidState, Xl
        mov     p2h, p1h
        mov     p2l, p1l
        ldi     Zh, hi8(AvrXKernelData+RunQueue)
        ldi     Zl, lo8(AvrXKernelData+RunQueue)
        BeginCritical
        rcall   _RemoveObject           ; Attempt to remove from run queue
        rjmp    _Epilog

        _ENDFUNC

        _END



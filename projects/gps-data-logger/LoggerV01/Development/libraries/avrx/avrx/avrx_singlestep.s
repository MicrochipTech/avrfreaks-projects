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


*/
        _MODULE(avrx_singlestep)

        _EXTERN(IntProlog)
        _EXTERN(RunQueue)
        _EXTERN(_QueuePid)
        _EXTERN(_Epilog)

        _CODESECTION
;+
;-----------------------------------------------------------
; unsigned char AvrXSingleStepNext(pProcessID)
;
; Marks a suspended task for single step support
; Jams it on the front of the run queue.  Returns error
; if task is not suspended.
;
; PASSED:       R25:R24 = Pid to single step
; RETURN:       R24 = 1 if task not suspended (error)
; USES:         FLAGS (except T)
;
; 20020604 - Changed to use AvrXKernelData structure.
;-
        _FUNCTION(AvrXSingleStepNext)

AvrXSingleStepNext:
        AVRX_Prolog

        mov     Zh, p1h
        mov     Zl, p1l
        ldd     Xl, Z+PidState
        sbrs    Xl, SuspendedBit
        rjmp    AssnXErr

        sbr     Xl, BV(SingleStep) ; Mark for single stepping
        std     Z+PidState, Xl

        ldi	Zl, lo8(AvrXKernelData)
        ldi	Zh, hi8(AvrXKernelData)
        ldd     Xh, Z+RunQueue+NextH   ; Put task in front of run queue
        ldd     Xl, Z+RunQueue+NextL
        std     Z+RunQueue+NextH, p1h
        std     Z+RunQueue+NextL, p1l
        mov	Zh, p1h
        mov	Zl, p1l
        std     Z+NextH, Xh
        std     Z+NextL, Xl
        rjmp    AssnXOk
        _ENDFUNC

/*+
; -----------------------------------------------------------
; unsigned char AvrXStepNext(pProcessID)
;
; Unsuspends a task, adds it to the run queue
; then resuspends the task.
;
; PASSED:       R25:R24 = Pid to single step
; RETURN:       R24 = 1 if task not suspended (error)
; USES:
;
-*/
        _FUNCTION(AvrXStepNext)

AvrXStepNext:
        AVRX_Prolog

        mov     Zh, p1h
        mov     Zl, p1l
        ldd     Xl, Z+PidState
        sbrs    Xl, SuspendedBit
        rjmp    AssnX

        cbr     Xl, BV(SuspendBit) | BV(SuspendedBit)
        std     Z+PidState, Xl
        rcall   _QueuePid               ; Add to run queue
; <BUG BUG - QueuePid trashes Z?, yeah, reloads with R25:R24
        ldd     Xl, Z+PidState
        sbr     Xl, BV(SuspendBit)
        std     Z+PidState, Xl
AssnXOk:
        ldi     r1l, lo8(0)     ; Ok Return
        rjmp    AssnX
AssnXErr:
        ldi     r1l, lo8(1)     ; Err Return
AssnX:
        std     Y+_r1l, r1l
        rjmp    _Epilog

        _ENDFUNC
        _END



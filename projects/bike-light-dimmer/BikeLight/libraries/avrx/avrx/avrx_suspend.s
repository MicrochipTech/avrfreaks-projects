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

        _MODULE(avrx_suspend)
        _EXTERN(_Epilog)
        _EXTERN(IntProlog)
        _EXTERN(Running)
        _EXTERN(RunQueue)
        _EXTERN(_RemoveObject)
        
        _CODESECTION

/*+
; --------------------------------------------------
; AvrXSuspend
;
; Mark a pid for suspension and attempt to remove from the run queue
;
; AvrXBreakpoint
;
; Suspend Self (Running) if previously suspended (single stepped)
;
; PASSED:       R25:R24 = PID
; RETURNS:      
; USES: 
; ASSUMES:
; NOTES:        Marks PID for suspension.  If removed from the run
;               queue, then marks suspended.  _QueuePid will take care
;               of this for pids queued elsewhere when they become
;               eligable for running.
;
; 20020604 - modified to use AvrXKernelData structure
-*/
        _FUNCTION(AvrXBreakpoint)
        
AvrXBreakpoint:
        AVRX_Prolog
        lds     Zh, AvrXKernelData+Running+NextH
        lds     Zl, AvrXKernelData+Running+NextL
        ldd     tmp0, Z+PidState
        sbrc    tmp0, SuspendBit
        rjmp    as00
        rjmp    _Epilog
        _ENDFUNC

        _FUNCTION(AvrXSuspend)

AvrXSuspend:                    ; User Entry Point
        AVRX_Prolog
        mov     Zh, p1h
        mov     Zl, p1l
        ldd     tmp0, Z+PidState
        sbr     tmp0, BV(SuspendBit)      ; Mark process for suspending
        std     Z+PidState, tmp0
as00:
        mov     p2h, Zh
        mov     p2l, Zl
        ldi     Zh, hi8(AvrXKernelData+RunQueue)
        ldi     Zl, lo8(AvrXKernelData+RunQueue)
        BeginCritical
        rcall   _RemoveObject           ; Attempt to remove from run queue
        mov     Yl, tmp0
        mov     Yh, tmp1
        adiw    Yl, 0
        breq    as01
        
        ldd     Zl, Y+PidState
        sbr     Zl, BV(SuspendedBit)    ; If found, then mark suspended.
        std     Y+PidState, Zl
as01:
        rjmp    _Epilog
        _ENDFUNC
        _END


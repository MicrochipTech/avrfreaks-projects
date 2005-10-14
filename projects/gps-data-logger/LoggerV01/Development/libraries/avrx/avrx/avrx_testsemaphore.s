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
        _MODULE(avrx_testsemaphore)
        _CODESECTION

/*+
;-----------------------------------------------
; AvrXTestSemaphore
; AvrXTIntTestSemaphore
;
; Non blocking call to check state of the Semaphore.
;
; AvrXTestObjectSemaphore
; AvrXTestTimer
; AvrXTestPid
; AvrXTestMessageAck
;
; Non blocking version of AvrXWaitObjectSemaphore
;
; PASSED:       R25:R24 = Semaphore/Object
; RETURNS:      R25:R24 = 0 - _PEND
;                       = 1 - _DONE
;                       = 2 - something is waiting on it.
; USES:         Z, X
; STACK:
; SIDE EFFECTS: 
; NOTES:        Since we don't block, the only valid
;               condition is _DONE.  So, reset to 
;               _PEND and return.
-*/
        _FUNCTION(AvrXTestTimer)
        _PUBLIC(AvrXTestPid)
        _PUBLIC(AvrXTestMessageAck)
        _PUBLIC(AvrXTestObjectSemaphore)
AvrXTestTimer:
AvrXTestPid:
AvrXTestMessageAck:
AvrXTestObjectSemaphore:
        subi    p1l, lo8(-2)
        sbci    p1h, hi8(-2)
        _ENDFUNC
        
        _FUNCTION(AvrXIntTestSemaphore)
        _PUBLIC(AvrXTestSemaphore)
AvrXIntTestSemaphore:
AvrXTestSemaphore:
        mov     Zh, p1h
        mov     Zl, p1l
        ldi     p1l, lo8(_DONE)     ; Preload _DONE return
        ldi     p1h, hi8(_DONE)
        
        BeginCritical
        ldd     tmp0, Z+NextL
        ldd     tmp1, Z+NextH
        sub     tmp0, p1l
        sbc     tmp1, p1h
        breq    aits01
        
        brlo    aits00          ; if > _DONE return 2
        subi    r1l, lo8(-2)
;        sbci    r1h, hi8(-2)
aits00:
        subi    r1l, lo8(1)
;        sbci    r1h, hi8(1)    ; if < _DONE return 0
        EndCriticalReturn
aits01:
        std     Z+NextH, tmp0    ; Store _PEND (0)
        std     Z+NextL, tmp1    ; Return _DONE (R25:R24)
        EndCriticalReturn

        _ENDFUNC
        _END
        


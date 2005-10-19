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


REVISION HISTORY
        20010403 - Initial update for avrx-ctoasm.inc port

*/
        _MODULE(avrx_timequeue)
//        _PUBLIC(_TimerHandler)
        _EXTERN(AvrXIntSendMessage)
        _EXTERN(AvrXIntSetObjectSemaphore)
        _EXTERN(AvrXSetObjectSemaphore)
        _EXTERN(AvrXIntResetObjectSemaphore)
        _EXTERN(IntProlog)
        _EXTERN(_Epilog)
        _EXTERN(AvrXWaitTimer)
;
; AvrX Time Queue Manager
;
        _DATASECTION

        _GLOBAL(_TimerQueue, 2)       ; Head of the timer queue
        _GLOBAL(_TimQLevel,  1)       ; Recurse level of timer interrupts

        _CODESECTION
/*+
; -----------------------------------------------
; void AvrXDelay(pTcb, unsigned)
;
; Passed:       p1h:p1l = TCB
;               p2h:p2l = Count
; Returns:      
; Uses:         
; Stack: 
; Notes:        See AvrXStartTimer, AvrXWaitTimer
-*/
        _FUNCTION(AvrXDelay)
        
AvrXDelay:
        rcall   AvrXStartTimer
        rjmp    AvrXWaitTimer
        _ENDFUNC
/*+
; -----------------------------------------------
; void AvrXStartTimer(pTcb, unsigned)
;
; Passed:       p1h:p1l = TCB
;               p2h:p2l = Count
; Returns:
; Uses:
; Stack:        
; Notes:        Should check and halt if TCB already queued
;               Resets TCB Semaphore  If count Zero, just flag
;               semaphore and return
-*/
        _FUNCTION(AvrXStartTimer)
        
AvrXStartTimer:
        subi    p2l, lo8(-0)
        sbci    p2h, hi8(-0)
        brne    CountNotZero
        rjmp    AvrXSetObjectSemaphore
        _PUBLIC(CountNotZero)
CountNotZero:        
        AVRX_Prolog

        rcall   AvrXIntResetObjectSemaphore

        ldi     Zl, lo8(_TimerQueue)
        ldi     Zh, hi8(_TimerQueue)
        
        BeginCritical
        lds     tmp0, _TimQLevel
        dec     tmp0
        sts     _TimQLevel, tmp0
        EndCritical

ast00:
        mov     Yl, Zl          ; Y -> Previous
        mov     Yh, Zh
        ldd     Zl, Y+NextL     ; Z -> Current
        ldd     Zh, Y+NextH
        adiw    Zl, 0
        breq    ast01           ; End of queue, Wrap up.

        ldd     tmp0, Z+TcbCount+NextL; tmp = Current count
        ldd     tmp1, Z+TcbCount+NextH; p2 = our count

        sub     p2l, tmp0
        sbc     p2h, tmp1         ; Subtract it from us
        brsh    ast00           ; Its less than us, continue walking the list

        add     p2l, tmp0         ; Restore us
        adc     p2h, tmp1
        sub     tmp0, p2l         ; Subtract us from it
        sbc     tmp1, p2h

        std     Z+TcbCount+NextL, tmp0
        std     Z+TcbCount+NextH, tmp1  ; Put it back out and insert us in front.
;
; Wrap up: Z->Current, P1->Tcb, Y->Prev, P2 = Count
; Insert Tcb into chain
;
ast01:
        std     Y+NextH, p1h
        std     Y+NextL, p1l    ; Prev.next = NewTCB
        mov     Yh, p1h
        mov     Yl, p1l
        std     Y+NextH, Zh     ; NewTCB.next = Current
        std     Y+NextL, Zl
        std     Y+TcbCount+NextL, p2l
        std     Y+TcbCount+NextH, p2h ; NewTCB.Count = count

        rcall   TimerHandler   ; process any nested timer interrupts

        rjmp    _Epilog
        _ENDFUNC

/*+
; -----------------------------------------------
; void AvrXTimerHandler(void)
;
; THIS IS INTERRUPT CODE and must be called within an
; _Prolog/_Epilog section.
;
; The register variable _TimQLevel is used to count re-
; entry into the code.  It is also a semaphore that prevents
; timer interrupts from hosing the AvrXStartTimer code, above.
; Thus, the entire timer handler chain can be run with interrupts
; enabled.
;
; Since this can be called from C code (avrx 2.5/IAR) gotta preserve everything
; but Z and tmp0-4.  System calls within can and do trash the trashable
; registers, hence all the push/pop's
-*/
        _FUNCTION(AvrXTimerHandler)
        
AvrXTimerHandler:
        BeginCritical
        lds     tmp0, _TimQLevel
        subi    tmp0, 1          ; Can't use "dec" because doesn't affect 
        sts     _TimQLevel, tmp0 ; carry flag.
        EndCritical
        brcs    ati00
        ret                     ; Do not branch on rollover (0->FF)
ati00:
        push    Yl              ; Gotta save these since we do not know
        push    Yh              ; who called us (C code)
        push    Xl
        push    Xh
        lds     Yh, _TimerQueue+NextH
        lds     Yl, _TimerQueue+NextL
        adiw    Yl, 0
        breq    ati02           ; Empty queue, return
;
; Y points to the first element in the queue.
;
        ldd     Zh, Y+TcbCount+NextH
        ldd     Zl, Y+TcbCount+NextL        
        sbiw    Zl, 1           ; Y->Count--
        std     Y+TcbCount+NextH, Zh
        std     Y+TcbCount+NextL, Zl
ati01:
        or      Zl, Zh          ; While (Y->Count == 0)
        brne    ati02           ; {

        ldd     Xl, Y+NextL
        ldd     Xh, Y+NextH
        sts     _TimerQueue+NextH, Xh ; Point queue to (new) first element
        sts     _TimerQueue+NextL, Xl
        std     Y+NextH, Zh     ;   Zero out link
        std     Y+NextL, Zl
        
;        push    Zl
;        push    Zh
        ldd     p1l, Y+TcbSemaphore+NextL
        ldd     p1h, Y+TcbSemaphore+NextH
        subi    p1l, lo8(TIMERMESSAGE_EV)
        sbci    p1h, hi8(TIMERMESSAGE_EV)
        brne    ati04
        ldd     p1l, Y+TcbQueue+NextL
        ldd     p1h, Y+TcbQueue+NextH
        mov     p2h, Yh
        mov     p2l, Yl
        rcall   AvrXIntSendMessage
        rjmp    ati03
ati04:
        mov     p1l, Yl
        mov     p1h, Yh
        rcall   AvrXIntSetObjectSemaphore
ati03:
;        pop     Zh
;        pop     Zl

        adiw    Xl, 0           ;   If Next Tcb == 0 (list empty)
        breq    ati02           ;       break;
                                ;   else
        mov     Yl, Xl
        mov     Yh, Xh
        ldd     Zh, Y+TcbCount+NextH ;       load up count and loop.
        ldd     Zl, Y+TcbCount+NextL
        rjmp    ati01           ; }
ati02:
        pop     Xh
        pop     Xl
        pop     Yh
        pop     Yl
TimerHandler:
        BeginCritical
        lds     tmp0, _TimQLevel
        inc     tmp0
        sts     _TimQLevel, tmp0
        EndCritical
        brne    ati00
        ret
        _ENDFUNC
        
        _END

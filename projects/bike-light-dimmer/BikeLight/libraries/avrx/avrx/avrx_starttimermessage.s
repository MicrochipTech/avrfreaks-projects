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
;
; AvrX Time Queue Manager
;
        _MODULE(avrx_starttimermessage)
        _EXTERN(CountNotZero)
        _EXTERN(AvrXSendMessage)
        
        _CODESECTION
/*+
;
;------------------------------------------------
; void AvrXStartTimerMessage(pTimerMessageBlock, uint16_t, pMessageQueue)
;
; PASSED:       R25:R24 = TCB
;               R23:R22 = Timeout
;               R20:R21 = MessageQueue
; Returns:
; Uses:
;-
-*/
        _FUNCTION(AvrXStartTimerMessage)

AvrXStartTimerMessage:
#ifdef __IAR_SYSTEMS_ASM__
        ld      tmp2, Y+
        ld      tmp3, Y+                ; recover P3 and clear stack
#endif
        subi    p2l, lo8(-0)
        sbci    p2h, hi8(-0)
        brne    axstm0
        mov     p2l, p1l
        mov     p2h, p1h
        mov     p1l, tmp2               ; tmp3:tmp2 == P3 for GCC
        mov     p1h, tmp3
        rjmp    AvrXSendMessage         ; Count Zero, just queue onto message queue
axstm0:
        mov     Zl, p1l
        mov     Zh, p1h
        std     Z+TcbQueue+NextH, tmp3
        std     Z+TcbQueue+NextL, tmp2       ; Stuff the queue
        ldi     tmp2, lo8(TIMERMESSAGE_EV)
        ldi     tmp3, hi8(TIMERMESSAGE_EV)
        std     Z+TcbSemaphore+NextH, tmp3
        std     Z+TcbSemaphore+NextL, tmp2   ; Cruft up bogus semaphore
        rjmp    CountNotZero            ; This is in avrx_timequeue.s

        _ENDFUNC
        _END


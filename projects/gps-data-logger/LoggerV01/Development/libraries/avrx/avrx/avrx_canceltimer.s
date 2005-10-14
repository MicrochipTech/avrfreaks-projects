#include        "avrx.inc"
/*
        Copyright 1999-2000, Larry Barello
        larry@barello.net

*/
;
; AvrX Time Queue Manager
;
        _MODULE(avrx_canceltimer)
        _EXTERN(_TimerQueue)
        _EXTERN(_RemoveObject)
        _EXTERN(_Epilog)
        _EXTERN(IntProlog)
        _EXTERN(AvrXIntSetObjectSemaphore)

        _CODESECTION
/*+
; -----------------------------------------------
; pTimerControlBlock
; AvrXCancelTimer(pTimerControlBlock)
;
; Passed:       R25:R24 = TCB
; Returns:      Pointer to removed timer, or 0 if failure
; Uses:
; Stack:        
; Note: walk list looking for TCB.  If found, and not at
;       end of list, then add count to Next tcb.
;
-*/
        _FUNCTION(AvrXCancelTimer)
        
AvrXCancelTimer:
        AVRX_Prolog
        rcall   AvrXIntSetObjectSemaphore

        ldd     p2l, Y+_p1l
        ldd     p2h, Y+_p1h
        ldi     Zl, lo8(_TimerQueue)
        ldi     Zh, hi8(_TimerQueue)

        BeginCritical
        rcall   _RemoveObject   ; Z = next item, R23:R22 = obj.
        subi    tmp0, lo8(0)
        sbci    tmp1, hi8(0)    ; Test if in timer queue
        breq    act00           ; No, just return

        mov     Yl, p2l         ; Y is restored in _Epilog
        mov     Yh, p2h
        ldd     p2l, Y+TcbCount+NextL
        ldd     p2h, Y+TcbCount+NextH
        ldd     r1h, Z+TcbCount+NextH ; Next
        ldd     r1l, Z+TcbCount+NextL
        add     r1l, p2l
        adc     r1h, p2h
        std     Z+TcbCount+NextL, r1l
        std     Z+TcbCount+NextH, r1h
        rjmp    _Epilog
act00:
        std     Y+_p1l, tmp0      ; Return 0, failure.
        std     Y+_p1h, tmp1
        rjmp    _Epilog
        
        _ENDFUNC
        _END


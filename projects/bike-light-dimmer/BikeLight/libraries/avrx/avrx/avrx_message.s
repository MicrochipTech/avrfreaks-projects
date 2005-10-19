#include        "avrx.inc"
/*
        Copyright 1999-2001, Larry Barello
        larry@barello.net

REVISION HISTORY
        20010403 - Initial update for avrx-ctoasm.inc port

*/
        _MODULE(avrx_message)

        _EXTERN(IntProlog)
        _EXTERN(_Epilog)
        _EXTERN(_AppendObject)
        _EXTERN(AvrXIntSetObjectSemaphore)
        _EXTERN(_RemoveFirstObject)
        _EXTERN(AvrXWaitObjectSemaphore)
        _EXTERN(AvrXResetSemaphore)
        _EXTERN(AvrXResetObjectSemaphore)

        _CODESECTION
/*+
; --------------------------------------------------
; void AvrXSendMessage(pMessageQueue, pMessageControlBlock)
;
; PASSED:       p1h:p1l = Queue head
;               p2h:p2l = Message
; RETURNS:      
; USES:         
; CALLS:
; ASSUMES:
; NOTES:
-*/
        _FUNCTION(AvrXSendMessage)
        
AvrXSendMessage:
        AVRX_Prolog
        rcall   AvrXIntSendMessage
        rjmp    _Epilog
        _ENDFUNC
/*+
; --------------------------------------------------
; void AvrXIntSendMessage(pMessageQueue, pMessageControlBlock)
;
; PASSED:       p1h:p1l = Queue head
;               p2h:p2l = Message
; RETURNS:      
; USES:         Z, Y & some others.  See AvrXIntSetObjectSemaphore
; CALLS:
; ASSUMES:      Called within an _Epilog/_Prolog wrapper
; NOTES:        This could be called from interrupt code
;               but you would need to make sure there is 
;               only one source of messages for that queue
;               or wrap _appendObject in a critical section.
-*/
        _FUNCTION(AvrXIntSendMessage)
        
AvrXIntSendMessage:
;        rcall   AvrXResetObjectSemaphore
        mov     Zh, p1h
        mov     Zl, p1l
        rcall   _AppendObject   ; Append the message onto the queue
        rjmp    AvrXIntSetObjectSemaphore
        _ENDFUNC
/*+
; --------------------------------------------------
; pMessageControlBlock AvrXWaitMessage(pMessageQueue)
;
; PASSED:       p1h:p1l = Queue head
; RETURNS:      r1h:r1l = Message
; USES:         Z, flags (See _RemoveFirstObject)
; CALLS:
; ASSUMES:      Null terminated list
; NOTES:        AvrXRecvMessage is atomic for append-only queues
;               just push registers to be saved
-*/
        _FUNCTION(AvrXWaitMessage)

AvrXWaitMessage:
        mov     Zl, p1l
        mov     Zh, p1h
        BeginCritical
        rcall   _RemoveFirstObject
        EndCritical
        brne    _rm01

        rcall   AvrXWaitObjectSemaphore
        subi    p1l, lo8(2)
        sbci    p1h, hi8(2)             ; p1 buggered by WaitObject
        rjmp    AvrXWaitMessage
_rm01:
        rcall   AvrXResetObjectSemaphore      ; Clear possible _PEND
        mov     p1l, p2l
        mov     p1h, p2h
        ret
        _ENDFUNC
        _END


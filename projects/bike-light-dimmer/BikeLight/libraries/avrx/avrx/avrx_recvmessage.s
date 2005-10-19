#include        "avrx.inc"
/*
        Copyright 1999-2001, Larry Barello
        larry@barello.net

*/
        _MODULE(avrx_recvmessage)

        _EXTERN(_RemoveFirstObject)
        _EXTERN(AvrXResetSemaphore)

        _CODESECTION
/*+
; --------------------------------------------------
; pMessage AvrXRecvMessage(pMessageQueue)
;
; Removes the first item in a queue. Return Zero if empty.
;
; PASSED:       R25, R24 = Queue head
; RETURNS:      R25, R24 = Object = 0 if list empty
; USES:         Z, X, Flags, R23:R22
; CALLS:
; ASSUMES:      Null terminated list
; NOTES:        If queue empty, then reset queue semaphore
;               
;
-*/
        _FUNCTION(AvrXRecvMessage)
        
AvrXRecvMessage:
        mov     Zl, p1l
        mov     Zh, p1h
        BeginCritical
        rcall   _RemoveFirstObject
        subi    p1l, lo8(-2)
        sbci    p1h, hi8(-2)
        rcall   AvrXResetSemaphore      ; Note, interrupt enabled here
        mov     r1l, p2l
        mov     r1h, p2h
        EndCriticalReturn

        _ENDFUNC
        _END


#include        "avrx.inc"
/*
        Copyright 1999-2002, Larry Barello
        larry@barello.net

	20020604 - Modified to use AvrXKernelData Structure
*/
        _MODULE(avrx_priority)

        _EXTERN(AvrXKernelData)

        _CODESECTION
/*+
; --------------------------------------------------
; uint8_t AvrXChangePriority(pProcessID, uint8_t)
; uint8_t AvrXPriority(void)
; pProcessID AvrXSelf(void)
;
; PASSED:       
; RETURNS:      
; USES: 
; CALLS:
; ASSUMES:	Top of RunQueue = current running task
; NOTES:	Should use "Running" rather than RunQueue.
-*/
        _FUNCTION(AvrXChangePriority)

AvrXChangePriority:
        mov     Zl, p1l
        mov     Zh, p1h
        ldd     r1l, Z+PidPriority
        std     Z+PidPriority, p2l
        ret
        _ENDFUNC
        
        _FUNCTION(AvrXSelf)

AvrXSelf:
        lds     r1h, AvrXKernelData+RunQueue+NextH
        lds     r1l, AvrXKernelData+RunQueue+NextL
        ret
        _ENDFUNC
        
        _FUNCTION(AvrXPriority)

AvrXPriority:
        lds     Zh, AvrXKernelData+RunQueue+NextH
        lds     Zl, AvrXKernelData+RunQueue+NextL
        ldd     r1l, Z+PidPriority
        ret
        _ENDFUNC
        _END
        

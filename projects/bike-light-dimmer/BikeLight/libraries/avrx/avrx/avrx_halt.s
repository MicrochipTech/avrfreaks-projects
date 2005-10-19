#include        "avrx.inc"
/*
        Copyright 1999-2001, Larry Barello
        larry@barello.net

*/
        _MODULE(avrx_halt)
        _CODESECTION
/*+
; --------------------------------------------------
; AvrXHalt
;
; Locks up the system.
;
; PASSED:       
; RETURNS:      
; USES: 
; CALLS:
; ASSUMES:
; NOTES:        
-*/
        _FUNCTION(AvrXHalt)

AvrXHalt:
        BeginCritical
        rjmp    AvrXHalt
        _ENDFUNC
        _END


#include "avrx.inc"

/*
        Copyright 1999-2001, Larry Barello
        larry@barello.net

*/

/*+
; --------------------------------------------------
; Generate Interrupt (Single Step Support)
;
; PASSED:       Nothing
; USES:         X, Y and R0
; RETURNS:      Nothing
; NOTES:        Generates an interrupt using Timer0.  Assumes global
;               interrupt flag is cleared.
;
;	NB: This routine is called by AvrX in the Epilog code.
;       This routine must be modfied to fit your particular
;	requirements.  The shipped code works with 8515 varients.
;
;	9/07/02 - added SFR_IO_ADDR() macro for GCC 3.2 I/O access
;	5/31/02 - checked for new and improved GCC bit names
;		this should work with most varients.
;
;
-*/
        _MODULE(avrx_generatesinglestepinterrupt)
        _CODESECTION
        _FUNCTION(GenerateInterrupt)

GenerateInterrupt:
#ifdef CS0
#    define CS00 CS0	/* Cover mega128 fubar with changed bit names */
#endif
        ldi     Yl, 1<<CS00     ; 1x prescale
        ser     Xh
        in      R0, _SFR_IO_ADDR(TCCR0)       ; Previous prescaler
        in      Yh, _SFR_IO_ADDR(TCNT0)       ; Previous count
        out     _SFR_IO_ADDR(TCNT0), Xh       ; Set maximum
        out     _SFR_IO_ADDR(TCCR0), Yl       ; Interrupt generated NOW
        out     _SFR_IO_ADDR(TCCR0), R0       ; Restore orignal prescaler
        out     _SFR_IO_ADDR(TCNT0), Yh       ; Restore original count
        ret
        _ENDFUNC
        _END


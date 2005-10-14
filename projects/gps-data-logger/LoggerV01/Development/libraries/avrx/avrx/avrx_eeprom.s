#include        "avrx.inc"
/*
        Copyright 1999-2002, Larry Barello
        larry@barello.net

2002-09-07 Added _SFR_IO_ADDR() macros for all I/O access
2002-05-30 lb changed EEARH to EEARL+1 in one routine.

*/
        _MODULE(avrx_eeprom)

        _EXTERN(AvrXWaitSemaphore)
        _EXTERN(AvrXSetSemaphore)

        _DATASECTION

        _GLOBAL(EEPromMutex, MtxSz)

        _CODESECTION

/*+
; -----------------------------------------------
; uint8_t AvrXReadEEProm(uint16_t)
;
; Access to the EEPROM by interrupt routines is
; prohibited.
;
; PASSED:       R25:R24 = Address
; RETURNS:      R24 = data
; USES:         EEPromMutex
; NOTE:         Might block waiting for access to EEPROM
-*/
        _FUNCTION(AvrXReadEEProm)

AvrXReadEEProm:
        mov     p2l, p1l
        mov     p2h, p1h
        ldi     p1h, hi8(EEPromMutex)
        ldi     p1l, lo8(EEPromMutex)
        rcall   AvrXWaitSemaphore       ; craps on Z and tmp0, tmp1
are01:
        sbic    _SFR_IO_ADDR(EECR), EEWE      ; Make sure EEPROM is ready
        rjmp    are01

        out     _SFR_IO_ADDR(EEARL), p2l
        out     _SFR_IO_ADDR(EEARL+1), p2h    ; For the 8515
        sbi     _SFR_IO_ADDR(EECR), EERE      ; Flag a read operation
        in      R0, _SFR_IO_ADDR(EEDR)
        push    R0
        rcall   AvrXSetSemaphore
        pop     r1l
        ret
        _ENDFUNC

/*+
; -----------------------------------------------
; unsigned AvrXReadEEPromWord(void *)
;
; Access to the EEPROM by interrupt routines is
; prohibited.
;
; PASSED:       p1 = Address
; RETURNS:      r1 = data
; USES:         EEPromMutex
; NOTE:         Might block waiting for access to EEPROM
-*/
        _FUNCTION(AvrXReadEEPromWord)

AvrXReadEEPromWord:
        mov     p2l, p1l
        mov     p2h, p1h
        ldi     p1h, hi8(EEPromMutex)
        ldi     p1l, lo8(EEPromMutex)
        rcall   AvrXWaitSemaphore       ; craps on Z and tmp0, tmp1
arew01:
        sbic    _SFR_IO_ADDR(EECR), EEWE      ; Make sure EEPROM is ready
        rjmp    arew01

        out     _SFR_IO_ADDR(EEARL), p2l
        out     _SFR_IO_ADDR(EEARL+1), p2h    ; For the 8515
        sbi     _SFR_IO_ADDR(EECR), EERE      ; Flag a read operation
        in      R0, _SFR_IO_ADDR(EEDR)
        push    R0
        subi    p2l, lo8(-1)
        sbci    p2h, hi8(-1)
        out     _SFR_IO_ADDR(EEARL), p2l
        out     _SFR_IO_ADDR(EEARL+1), p2h
        sbi     _SFR_IO_ADDR(EECR), EERE
        in      R0, _SFR_IO_ADDR(EEDR)
        push    R0
        rcall   AvrXSetSemaphore
#if NextL == 1
        pop     r1l
        pop     r1h
#elif NextL == 0
	pop	r1h
	pop	r1l
#else
#error "Byte order not defined"
#endif
        ret
        _ENDFUNC

/*+
; -----------------------------------------------
; void AvrXWriteEEProm(uint16_t, uint8_t)
;
; PASSED:       R25:R24 = Address
;               R23 = data
; RETURNS:
; USES:         EEPromMutex
; NOTE:         Might block waiting for access to EEPROM
;
-*/
        _FUNCTION(AvrXWriteEEProm)

AvrXWriteEEProm:
        mov     tmp2, p2l         ; Temp save of data
        mov     p2l, p1l
        mov     p2h, p1h
        ldi     p1h, hi8(EEPromMutex)
        ldi     p1l, lo8(EEPromMutex)
        rcall   AvrXWaitSemaphore
awe00:
        sbic    _SFR_IO_ADDR(EECR), EEWE      ; Make sure EEPROM is ready
        rjmp    awe00
        out     _SFR_IO_ADDR(EEARL), p2l
        out     _SFR_IO_ADDR(EEARL+1), p2h    ; For the 8515
        out     _SFR_IO_ADDR(EEDR), tmp2
        sbi     _SFR_IO_ADDR(EECR), EEMWE
        sbi     _SFR_IO_ADDR(EECR), EEWE

        rjmp    AvrXSetSemaphore; R25:24 preserved in WaitSem
        _ENDFUNC
        _END



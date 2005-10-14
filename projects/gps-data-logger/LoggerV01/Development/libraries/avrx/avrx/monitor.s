#include        "avrx.inc"
/*
File:   Monitor.asm

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

Author: Larry Barello
        larry@barello.net

        REVISION HISTORY

6/4/2002 - Fixed memory address increment bug in modify memory command
	- added command to terminate a process
	
5/2001 - rewrote to be mostly C compatible.  R4-R15
used for locals (no statics)

*/
;------------------------------------------------
;
        _MODULE(monitor)

        _EXTERN(EEPromMutex)
        _EXTERN(AvrXSetSemaphore)
        _EXTERN(AvrXWriteEEProm)
        _EXTERN(AvrXReadEEProm)
        _EXTERN(AvrXReadEEPromWord)
        _EXTERN(PutHexByte)
        _EXTERN(PutHexWord)
        _EXTERN(GetHexByte)
        _EXTERN(GetHexWord)
        _EXTERN(AvrXInitTask)
        _EXTERN(AvrXSingleStepNext)
        _EXTERN(AvrXStepNext)
        _EXTERN(AvrXCancelTimer)
        _EXTERN(AvrXResume)
        _EXTERN(AvrXSuspend)
        _EXTERN(PutChar)
        _EXTERN(AvrXHalt)
        _EXTERN(PutSpace)
        _EXTERN(PutCR)
        _EXTERN(_GetChar)
        _EXTERN(_PrintString)
        _EXTERN(IsWhite)
        _EXTERN(IsHex)

        _EXTERN(_RxByte)        /* From serial.c */

#define Addressl        R4
#define Addressh        R5
#define EEAddressl      R6
#define EEAddressh      R7
#define ObjectIDl       R8
#define ObjectIDh       R9
#define DumpLength      R10
#define MonFlags        R11
#define TempWl          R12
#define TempWh          R13
#define TempB           R14

#define WORDFLAG 0
#define EEFLAG 1
#define NL "\n"
#define CR "\r"

        _CODESECTION
;-------------------------------------------------
; Monitor: program for examining and modifying
; registers, data, i/o and eeprom.
;
; NB    {opt, opt}      Must select one of the options
;       [opt, opt]      Optional input - can be ignored
;
; Command summary:
;
;       cmd                     Action
;       --------------------    ------
;       m{s,e}[w][aaaa]         Modify SRAM/EEram by byte/word starting at AAAA
;       d{s,e}[w][aaaa] [xx]    Dump XX Bytes/Words of Sram/EERam, starting at AAAA
;       h[XX]                   Halt process XX
;       s[XX]                   Step Process XX (run until next kernel entry)
;       n[XX]                   Step Process till next instruction
;       r[XX]                   Resume Process XX
;       x[XX]                   display context of XX (use 's' to modify)
;       i[xx]                   Signal (set) Semaphore
;       c[xx]                   cancel timer XX
;	k[xx]			Kill process XX
;       p[xxxx]                 Initialize Process (AvrXInitTask)
;
;
; Stack Usage:
;       12 or more bytes
;
; Register Usage:
;       Everything
;
; Global data information
;
;
Hello:  _DB  CR
        _DB  "AvrX Monitor V2", CR
        _DB  CR
        _DB  "Modify = m{s,e}[w][aaaa]", CR
        _DB  "Dump   = d{s,e}[w][aaaa] [xx]", CR
        _DB  "Halt   = h[oooo]", CR
        _DB  "Step   = s[oooo]", CR
#ifdef  SINGLESTEPSUPPORT
        _DB  "Next   = n[oooo]", CR
#endif
        _DB  "Resume = r[oooo]", CR
        _DB  "Context= x[oooo]", CR
        _DB  "Signal = i[oooo]", CR
        _DB  "Cancel = c[oooo]", CR
        _DB  "KillTsk= k[oooo]",CR
        _DB  "Initask= p[aaaa]", CR
        _DB  "Quit   = q", CR
        _DB  "\000"
        _ALIGN

        _FUNCTION(Monitor)

Monitor:
        ldi     p1l, lo8(EEPromMutex)
        ldi     p1h, hi8(EEPromMutex)
        rcall   AvrXSetSemaphore        ; Initialize EEProm Mutex

        ldi     p1l, lo8(Hello)
        ldi     p1h, hi8(Hello)
Hello1:
        rcall   _PrintString
        subi    r1l, lo8(-1)
        sbci    r1h, hi8(-1)            ; Keep looping until
        mov     Zl, r1l
        mov     Zh, r1h
        lpm                             ; until all strings out.
        tst     R0
        brne    Hello1

        ldi     tmp0, lo8(0x60)
        mov     Addressl, tmp0
        ldi     tmp0, hi8(0x60)
        mov     Addressh, tmp0
        clr     ObjectIDl
        clr     ObjectIDh
        clr     MonFlags
        clr     DumpLength
        clr     EEAddressl
        clr     EEAddressh
        _ENDFUNC

        _FUNCTION(DebugMonitor)

DebugMonitor:
        rcall   PutCR
        ldi     p1l, '>'
        rcall   PutChar
        rcall   m_Parse
        rjmp    DebugMonitor

m_Parse:
        rcall   _GetChar
        cpi     r1l, 'm'     ; Modify Memory
        breq    m_ModifyMemory
        cpi     r1l, 'd'     ; Dump RAM
        breq    m_DumpMemory
        cpi     r1l, 'h'     ; Halt (suspend) Process
        breq    m_HaltProcess
        cpi     r1l, 's'     ; Step process
        breq    m_StepProcess
        cpi	r1l, 'k'     ; Terminate Process
        breq	m_Terminate
#ifdef  SINGLESTEPSUPPORT
        cpi     r1l, 'n'     ; Next instruction
        breq    m_SingleStep
#endif
        cpi     r1l, 'r'     ; Resume Process
        breq    m_ResumeProcess
        cpi     r1l, 'i'     ; Signal Semaphore
        breq    m_SetSemaphore
        cpi     r1l, 'c'     ;
        breq    m_CancelTimer
        cpi     r1l, 'x'
        breq    m_PrintProcessContext
        cpi     r1l, 'p'
        breq    m_InitTask
        cpi     r1l, 'q'
        breq    m_quit
m_err:
        ldi     p1l, '?'
        rjmp    PutChar

m_quit:
        pop     Xl      ; Hack to allow user i/f to call monitor
        pop     Xl
        ret

m_ModifyMemory:
        rcall   GetFlagsAndAddress
        brtc    m_err
        rjmp    _ModifyMemory
m_DumpMemory:
        rcall   GetFlagsAndAddress
        mov     TempWl, p1l
        mov     TempWh, p1h
        brtc    m_err           ; Flags in Xh, Addr in Z
        lds     tmp0, _RxByte
        cpi     tmp0, 0x0D       ; See if terminated with a CR
        breq    m1b
        rcall   GetDumpLength   ; Leave count in Xl
        brtc    m_err
m1b:
        mov     p2l, TempWl
        mov     p2h, TempWh
        mov     p1l, DumpLength
        rjmp    _DumpMemory

m_InitTask:
        rcall   GetHexWord      ; This is a word address, not byte.
        brtc    m_err
        rjmp    AvrXInitTask

m_HaltProcess:
        rcall   GetObjectID
        brtc    m_err
        rjmp    AvrXSuspend

m_ResumeProcess:
        rcall   GetObjectID
        brtc    m_err
        rjmp    AvrXResume

m_CancelTimer:
        rcall   GetObjectID
        brtc    m_err
        rjmp    AvrXCancelTimer

m_PrintProcessContext:
        rcall   GetObjectID
        brtc    m_err
        rjmp    _PrintContext

m_SetSemaphore:
        rcall   GetObjectID
        brtc    m_err
        rjmp    AvrXSetSemaphore

m_Terminate:
	rcall	GetObjectID
	brtc	m_err
	rjmp	AvrXTerminate

m_StepProcess:
        rcall   GetObjectID
        brtc    m_err
        rcall   AvrXStepNext
        tst     r1l
        brne    ms_NOTSUSPENDED
        ret

#ifdef  SINGLESTEPSUPPORT

m_SingleStep:
        rcall   GetObjectID
        brtc    m_err
NextStep:
        rcall   AvrXSingleStepNext      ; This will return after
        tst     r1l
        brne    ms_NOTSUSPENDED         ; the step is done

        mov     Zh, ObjectIDh
        mov     Zl, ObjectIDl
        ldd     Yl, Z+PidSP+NextL       ; Y = Frame Pointer
        ldd     Yh, Z+PidSP+NextH

        ldd     p1l, Y+_RetLo+2         ; Y = Address of Next inst
        ldd     p1h, Y+_RetHi+2

        ldd     Xh, Z+bp1+NextH         ; Breakpoints are stored in
        ldd     Xl, Z+bp1+NextL         ; the PID..
        adiw    Xl, 0
        breq    BreakPoint
        cp      p1l, Xl
        cpc     p1h, Xh
        breq    BreakPoint

        ldd     Xh, Z+bp2+NextH
        ldd     Xl, Z+bp2+NextL
        adiw    Xl, 0
        breq    NextStep                ; If we dont hit the breakpoint
        cp      p1l, Xl                 ; the monitor hangs here
        cpc     p1h, Xh
        brne    NextStep

BreakPoint:
        rjmp    PutHexWord              ; Print out our breakpoint address
#endif

ms_NOTSUSPENDED:
        rcall   PutCR
        mov	p1l, ObjectIDl
        mov	p1h, ObjectIDh
        rcall   PutHexWord
        ldi     p1l, lo8(NotSuspended)
        ldi     p1h, hi8(NotSuspended)
        rjmp    _PrintString
        _ENDFUNC

NotSuspended:
        _DB  " - Not Suspended!\r"
        _ALIGN
;
; void * GetObjectID()
;
; Little helper routine to get and return a byte value with
; carry set if all Ok.  e.g. entered 2 hex digits, or terminated
; with whitespace.
;
; Puts the ID in the R25:R24 register because that is what all the
; AvrX routines expect.
;
        _FUNCTION(GetObjectID)
GetObjectID:
        rcall   GetHexWord
        brts    goi00
        rcall   IsWhite
        brtc    goi02
goi00:
        tst     p2l
        brne    goi01           ;
        mov     p1l, ObjectIDl  ; Restore default
        mov     p1h, ObjectIDh
goi01:
        mov     ObjectIDl, p1l
        mov     ObjectIDh, p1h
goi02:
        ret
        _ENDFUNC
;
; unsigned char GetDumpLength()
;
; Another helper to manage defaults with the dump length
; counter
;
        _FUNCTION(GetDumpLength)
GetDumpLength:
        rcall   GetHexByte      ; get a dump counter
        brts    gdl00           ; If term invalid,
        rcall   IsWhite         ;    check for whitespace
        brtc    gdl02
gdl00:
        tst     p2l             ; If no user entry
        brne    gdl01
        mov     p1l, DumpLength  ;   load up default
gdl01:
        mov     DumpLength, p1l
gdl02:
        ret
        _ENDFUNC
;
; void * GetFlagsAndAddress()
;
; Parse input stream for byte/word flag and address.  All are optional
; as long as the input stream is terminated with whitespace.
;
        _FUNCTION(GetFlagsAndAddress)
GetFlagsAndAddress:
        clt
        rcall   _GetChar
        mov     tmp0, MonFlags
        cpi     r1l, 's'
        brne    gbTRYEERAM
        cbr     tmp0, 1<<EEFLAG; If "s" , clear flag
        rjmp    gbCONTINUE
gbTRYEERAM:
        cpi     r1l, 'e'     ; if "e", set flag
        brne    gbDONE
        sbr     tmp0, 1<<EEFLAG
gbCONTINUE:
        mov     MonFlags, tmp0
        rcall   GetHexWord      ; Go for the address
        mov     tmp0, MonFlags
        brtc    gbCHECKWORDFLAG
        cbr     tmp0, 1<<WORDFLAG; Full address entered, no "w"
        mov     MonFlags, tmp0
        rjmp    gbUPDATEADDRESS
gbCHECKWORDFLAG:
        cbr     tmp0, 1<<WORDFLAG
        mov     MonFlags, tmp0
        lds     tmp1, _RxByte
        cpi     tmp1, 'w'
        brne    gbCHECKFORWHITESPACE; Only "w" allowed if not address
        sbr     tmp0, 1<<WORDFLAG
        mov     MonFlags, tmp0
        rcall   GetHexWord      ; Try again for address
        brts    gbUPDATEADDRESS
gbCHECKFORWHITESPACE:
        rcall   IsWhite
        brtc    gbDONE          ; Error if not whitespace
gbUPDATEADDRESS:
        tst     p2l             ; see if any user input
        breq    gbDONE
        sbrs    MonFlags, EEFLAG
        rjmp    gbAddress
        mov     EEAddressl, p1l
        mov     EEAddressh, p1h
        rjmp    gbDONE
gbAddress:
        mov     Addressl, p1l
        mov     Addressh, p1h
gbDONE:
        mov     p1l, Addressl   ; Restore previous...
        mov     p1h, Addressh
        sbrc    MonFlags, EEFLAG
        mov     p1l, EEAddressl
        sbrc    MonFlags, EEFLAG
        mov     p1h, EEAddressh
        ret
        _ENDFUNC
;
;---------------------------------------------
; void _DumpMemory(char, void *)
;
; Passed:       p1 = Start Address
;               p2 = number of bytes to dump
; Returns:      Nothing
; Uses:         MonFlags (register)
;
        _FUNCTION(_DumpMemory)
_DumpMemory:
        sbrs    MonFlags, WORDFLAG
        rjmp    m10f

        cbr     p2l, 0x01        ; force even address to start.
        sbrc    p1l, 0x00        ; Add one for even count
        inc     p1l
m10f:
        mov     TempWl, p2l
        mov     TempWh, p2h
        mov     R1, p1l
m10c:
        rcall   PutCR
        mov     p1h, TempWh          ; Spit out address
        mov     p1l, TempWl
        rcall   PutHexWord
m10d:
        rcall   PutSpace
        rcall   GetByteWord
        sbrc    MonFlags, WORDFLAG
        rjmp    m11
        rcall   PutHexByte
        ldi     p1l, lo8(1)
        ldi     p1h, hi8(1)
        rjmp    m12
m11:
        rcall   PutHexWord
        ldi     p1l, lo8(2)
        ldi     p1h, hi8(2)
m12:
        add     TempWl, p1l
        adc     TempWh, p1h
        sub     R1, p1l
        breq    m10exit
m10e:
        mov     Zl, TempWl
        cbr     Zl, 0xF0
        breq    m10c            ; New line every 16 bytes.
        rjmp    m10d
m10exit:
        ret
        _ENDFUNC
;
; void _ModifyMemory(void *)
;
; PASSED:       p1 = address
;
; Uses:         MonFlags (register)
;
        _FUNCTION(_ModifyMemory)
_ModifyMemory:
        mov     TempWl, p1l
        mov     TempWh, p1h
_ModifyMemory_:
        rcall   PutCR
        mov     p1l, TempWl
        mov     p1h, TempWh
        rcall   PutHexWord
        rcall   PutSpace
_mmNEXT:
        rcall   GetByteWord
        sbrc    MonFlags, WORDFLAG
        rcall   PutHexWord
        sbrs    MonFlags, WORDFLAG
        rcall   PutHexByte
_mmGETDATA:
        ldi     p1l, '-'
        rcall   PutChar
        sbrs    MonFlags, WORDFLAG
        rcall   GetHexByte
        sbrc    MonFlags, WORDFLAG
        rcall   GetHexWord
        brtc    _mmNODATA
_mmWRITE:
        rcall   PutByteWord
        rcall   PutSpace
_mmGOTONEXT:
        ldi     Zl, lo8(1)
        ldi     Zh, hi8(1)
        sbrc    MonFlags, WORDFLAG
        adiw    Zl, 1
        add     TempWl, Zl
        adc     TempWh, Zh
        mov     Zl, TempWl
        cbr     Zl, 0xF0
        breq    _ModifyMemory_   ; Start a new line every 16 bytes
        rjmp    _mmNEXT
_mmNODATA:
        rcall   IsWhite
        brtc    _mmDONE         ; If not white, then error
        tst     p2l
        brne    _mmWRITE        ; If some data entered, then write
        lds     tmp0, _RxByte
        cpi     tmp0, 0x0D       ; if CR then exit
        brne    _mmGOTONEXT     ; otherwize, just go to next byte
_mmDONE:
        ret
        _ENDFUNC
;----------------------------------------------
; void _PrintContext(ObjectID *)
;
; Prints a pretty picture of the context of a process.
;
; PASSED:       nothing
; RETURNS:
; USES:         ObjectID (register)
;
ctdt0:  _DB  "\rPID Flg Pri Frame SR\r"
ctdt1:  _DB  "R0  "
ctdt2:  _DB  "R16 "
ctdt3:  _DB  "\r     PC  "
        _ALIGN

        _FUNCTION(_PrintContext)
_PrintContext:

        ldi     p1h, hi8(ctdt0)
        ldi     p1l, lo8(ctdt0)
        rcall   _PrintString

        mov     p1l, ObjectIDl
        mov     p1h, ObjectIDh
        rcall   PutHexWord      ; Print PID
        rcall   PutSpace

        mov     Zl, ObjectIDl
        mov     Zh, ObjectIDh
        ldd     p1l, Z+PidState
        rcall   PutHexByte
        rcall   PutSpace
        mov     Zl, ObjectIDl
        mov     Zh, ObjectIDh
        ldd     p1l, Z+PidPriority
        rcall   PutHexByte
        rcall   PutSpace
        rcall   PutSpace

        mov     Zl, ObjectIDl
        mov     Zh, ObjectIDh
        ldd     Yl, Z+PidSP+NextL   ; Z -> Frame
        ldd     Yh, Z+PidSP+NextH
        mov     p1l, Yl
        mov     p1h, Yh
        rcall   PutHexWord      ; Print Frame Pointer
        rcall   PutSpace
        rcall   PutSpace

        adiw    Yl, 1           ; Remember Stack are post dec!

        ld      p1l, Y+         ; SREG
        rcall   PutHexByte
        ;
        ; Print R0-R15
        ;
Loop:
        rcall   Print16RegPre
        ldi     p1h, hi8(ctdt1)
        ldi     p1l, lo8(ctdt1)
        rcall   Print16Reg
        rcall   Print16RegPre
        ldi     p1h, hi8(ctdt2)
        ldi     p1l, lo8(ctdt2)
        rcall   Print16Reg
        ldi     p1h, hi8(ctdt3)
        ldi     p1l, lo8(ctdt3)
        rcall   _PrintString
        ld      p1h, Y+
        ld      p1l, Y
        rcall   PutHexWord
        ret

Print16RegPre:
        rcall   PutCR
        mov     p1l, Yl
        mov     p1h, Yh
        rcall   PutHexWord
        rjmp    PutSpace

Print16Reg:
        rcall   _PrintString
        ldi     Zl, 16
        mov     R1, Zl
p16r0:
        ld      p1l, Y+
        rcall   PutHexByte
        rcall   PutSpace
        dec     R1
        brne    p16r0
        ret
        _ENDFUNC
;-----------------------------------------------
; unsigned GetByteWord(void *)
;
; PASSED:       p1 = address
; RETURNS:      r1 = data
; USES:         MonFlags
;
        _FUNCTION(GetByteWord)
GetByteWord:
        sbrc    MonFlags, EEFLAG
        rjmp    gbwEEPROM

        mov     Zl, TempWl
        mov     Zh, TempWh

        ldd     r1l, Z+0
        sbrs    MonFlags, WORDFLAG
        ret

        ldd     r1h, Z+NextH
        ldd     r1l, Z+NextL
        ret
gbwEEPROM:                      // With new calls must preserve Z
        mov     p1h, TempWh
        mov     p1l, TempWl
        sbrs    MonFlags, WORDFLAG
        rcall   AvrXReadEEProm
        sbrc    MonFlags, WORDFLAG
        rcall   AvrXReadEEPromWord
gwbEXIT:
        ret
        _ENDFUNC
;
; void PutByteWord(unsigned, void *)
;
; Passed:       p1 - value
;               p2 - pointer
; Returned      void
; Uses          MonFlags (eeprom vs RAM)
;
        _FUNCTION(PutByteWord)
PutByteWord:
        sbrc    MonFlags, EEFLAG
        rjmp    pbwEEPROM

        mov     Zl, TempWl
        mov     Zh, TempWh
        std     Z+0, p1l
        sbrs    MonFlags, WORDFLAG
        ret

        std     Z+NextH, p1h
        std     Z+NextL, p1l
        ret
pbwEEPROM:
        mov     TempB, p1h
        mov     p2l, p1l
        mov     p1h, TempWh
        mov     p1l, TempWl
        rcall   AvrXWriteEEProm
        sbrs    MonFlags, WORDFLAG
        rjmp    pbwEXIT

        mov     p1l, TempWl
        mov     p1h, TempWh
        subi    p1l, lo8(-1)
        sbci    p1h, hi8(-1)
        mov     p2l, TempB
        rcall   AvrXWriteEEProm
pbwEXIT:
        ret
        _ENDFUNC

        _END

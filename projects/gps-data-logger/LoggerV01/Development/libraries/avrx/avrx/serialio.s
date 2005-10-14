/*
File:   Serialio.asm

Copyright ©1998 - 2002 Larry Barello

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


Revision:
20020907 - Delete all header file references.  Fix this in "avrx.inc"
	 - Added _SFR_IO_ADDR() macro to all i/o access (3.2 compat)
8/27/02	- Changed "io.h" to "io-avr.h" as the former includes some C only header files.

6/4/02	- Changed Rx and UDRE interrupt routines to use the #defined
	names to conform with the new GCC interrupt vector table handling
	mechanism (this breaks IAR support in general for the mega stuff).
	- fixed alias support for UCR and USR for bigger mega chips.

6/13/01 - Added aliases for UCR and USR as the mega series
        changed the names for some reason...

1/31/01 - Changed InitSerialIO parameter register to conform to new
        GCC calling conventions (R25 -> R24)

5/30/02 - Fixed re-defines of various USART registers.  What a mess.  Added
	code to initialize mega128 upper baud rate register.

NOTE: This code is might be C compatible. I have not tested it or gone over it
carefully to insure compatibility.   In general the Monitor and the
serial routines should run in their own task space and not be called by
any C code.

*/
#include        "avrx.inc"
// #include	"io-avr.h"	/* this is included in avrx.inc */
// #include	"avrx-signal.h" /* this is obsolete > 3.02 */

        _MODULE(serialio)
        _EXTERN(_Epilog)
        _EXTERN(IntProlog)
        _EXTERN(AvrXWaitSemaphore)
        _EXTERN(AvrXIntSetSemaphore)
;
; Needed for USART interrupt
; Asynch, 8 bit, no parity, one stop, USART 0. is assumed in this code
;
#define UCR_INIT  lo8((1<<TXEN) | (1<<RXEN) | (1<<RXCIE))
#define UCRC_INIT (BV(UCSZ1) | BV(UCSZ0))	/* mega128 only */

// UBRR_INIT is an external value defined by user application.

//#define UBRR_INIT lo8((CPUCLK)/(16 * BAUDRATE) - 1)

#define CharPushed 7

; Mega chips re-define the UCR/USR.  The 161 has two usarts
; can't do the easy thing since IAR doesn't "#define" the io addresses
; The below is a cop-out: if the alternate names are "defined" then
; simply redefine the old names to match.

#if defined(UCSRB)
#	define UCR UCSRB
#endif
#if defined(UCSRA)
#	define USR UCSRA
#endif
#if defined(UCSR0B)
#	define UCR UCSR0B
#endif
#if defined(UCSR0A)
#	define USR UCSR0A
#endif
#if defined(UDR0)
#	define UDR UDR0
#endif
#if defined(UBRR0L)
#	define UBRR UBRR0L
#endif
#if defined(SIG_UART0_RECV)
#    define SIG_UART_RECV SIG_UART0_RECV
#    define SIG_UART_DATA SIG_UART0_DATA
#endif

// A special just for IAR

#if defined(__AT90Mega163__) || defined(__AT90Mega83__)
#  define UBRR UBRRLO
#endif


        _DATASECTION

        _GLOBAL(SioFlags,1)
        _GLOBAL(_RxByte, 1)
        _GLOBAL(RxC_Flag, MtxSz)
        _GLOBAL(TxIntFlag, MtxSz)

        _CODESECTION
;+
;------------------------------------------
; PushChar
;
; Set a flag so that the next call to GetChar just returns.
; in effect pushing the last returned charactor to be retrieved
; by the next routine
;-
        _FUNCTION(PushChar)
PushChar:
        push    Xl
        lds     Xl, SioFlags
        sbr     Xl, 1<<CharPushed
        sts     SioFlags, Xl
        pop     Xl
        ret
        _ENDFUNC
;+
;------------------------------------------
; GetChar
;
; Need to make this draw from a buffer so that multiple
; chars can be pushed... and so different input streams
; can be attached to the buffer.
;-
        _FUNCTION(GetChar)
#ifdef  __IAR_SYSTEMS_ASM__
        PUBLIC  ?C_GETCHAR
?C_GETCHAR:     ; Need this label for simulator I/O
#endif
GetChar:
        lds     tmp0, SioFlags
        sbrs    tmp0, CharPushed
        rjmp    gc00
        cbr     tmp0, 1<<CharPushed
        rjmp    gc01
        sts     SioFlags, tmp0
        rjmp    gc01
gc00:
        ldi     p1h, hi8(RxC_Flag)
        ldi     p1l, lo8(RxC_Flag)
        rcall   AvrXWaitSemaphore       ; Wait for a charactor, or flag from System
gc01:
        lds     r1l, _RxByte
        ret
        _ENDFUNC
;+
;------------------------------------------
; _GetChar
;
; Special version that echos
;
; RETURNS:      RxByte (Register)
;-
        _FUNCTION(_GetChar)

_GetChar:
        rcall   GetChar
        rjmp    PutChar
        _ENDFUNC
;+
;------------------------------------------
;
; PutCR
;
; Put a CR to the output stream
;-
        _FUNCTION(PutCR)
PutCR:
        ldi     p1l, 0x0D
        rjmp    PutChar
        _ENDFUNC
;+
;------------------------------------------
;
; PutSpace
;
; Put a space to the output stream
;-
        _FUNCTION(PutSpace)

PutSpace:
        ldi     p1l, ' '
        rjmp    PutChar
        _ENDFUNC
;+
;------------------------------------------
;
; const char * PutString(const char *)
;
; passed:
;       near pointer to string in FLASH memory
; Returns:
;       pointer to next char after NULL
;-
        _FUNCTION(_PrintString)

_PrintString:
        mov     Zl, p1l
        mov     Zh, p1h
_foo:
        lpm
        tst     R0
        breq    _ps00
        mov     p1l, R0
        push    Zl
        push    Zh
        rcall   PutChar
        pop     Zh
        pop     Zl
        adiw    Zl, 1
        rjmp    _foo
_ps00:
        mov     r1l, Zl
        mov     r1h, Zh
        ret
        _ENDFUNC
;+
;-------------------------------------------
; void InitSerialIO(unsigned)
;
; PASSED:       R25:24 = UBBR
;		p1h:p1l = UBBR
;
; History: parameter used to be passed in R25 l.b. 1/31/01
;	   Changed to unsigned for mega chips. l.b. 5/30/02
;
        _FUNCTION(InitSerialIO)
InitSerialIO:
        out     _SFR_IO_ADDR(UBRR), p1l
#if defined(UBRR0H)
	sts	UBRR0H, p1h	// THIS appears to be a no-op
#endif
#if defined(UCSR0C)
	ldi	tmp0, UCRC_INIT	// SET word length.
	sts	UCSR0C, tmp0	// Extended register in mega128
#endif
        ldi     tmp0, UCR_INIT
        out     _SFR_IO_ADDR(UCR), tmp0
        ret
        _ENDFUNC
;
;-----------------------------------------------
; UDRE_Interrupt
;
; Disables the interrupt (UDR is empty) signal waiting
; process to stuff the UDR
;
        _FUNCTION(SIG_UART_DATA)
SIG_UART_DATA:
        cbi     _SFR_IO_ADDR(UCR), UDRIE      ; Disable the interrupt

        rcall   IntProlog

        ldi     p1l, lo8(TxIntFlag)
        ldi     p1h, hi8(TxIntFlag)
        rcall   AvrXIntSetSemaphore     ; Signal we are ready for more.

        rjmp    _Epilog
        _ENDFUNC

;
;-----------------------------------------------
; RxC_Interrupt
;
; Reads the USART and flags UserTask, Char In semaphore
;
        _FUNCTION(SIG_UART_RECV)
SIG_UART_RECV:
        cbi     _SFR_IO_ADDR(UCR), RXCIE      ; disable interrupt

        rcall   IntProlog      ; Switch to kernel stack

        in      Xl, _SFR_IO_ADDR(UDR)
        sts     _RxByte, Xl

        ldi     p1h, hi8(RxC_Flag)
        ldi     p1l, lo8(RxC_Flag)
        rcall   AvrXIntSetSemaphore

        sbi     _SFR_IO_ADDR(UCR), RXCIE      ; Re-enable interrupts
        rjmp    _Epilog
        _ENDFUNC
;+
;------------------------------------------
; PutChar
;
; Passed: p1l "TxByte"
;
; Stuffs "TxByte" into usart.  The usart is double buffered.
; when the data register is full we wait on the semaphore
; until the Usart interrupts indicating the register is ready.
;-
        _FUNCTION(PutChar)
#ifdef __IAR_SYSTEMS_ASM__
        _PUBLIC(?C_PUTCHAR)     ; Enable terminal I/O
?C_PUTCHAR:
#endif
PutChar:
        sbic    _SFR_IO_ADDR(USR), UDRE
        rjmp    pc00

        push    p1l
        sbi     _SFR_IO_ADDR(UCR), UDRIE      ; Enable interrupts and
        ldi     p1l, lo8(TxIntFlag)
        ldi     p1h, hi8(TxIntFlag)
        rcall   AvrXWaitSemaphore; Wait until data register is ready
        pop     p1l
pc00:
        out     _SFR_IO_ADDR(UDR), p1l
        ret
        _ENDFUNC

;-----------------------------------------
; _GetHex
;
; PASSED:       p1l = Maximum number of digits
; RETURNS:      r1 = val, Tbit, p2l = Number of digits entered
; USES:         R24, R25
; STACK:        4
;
        _FUNCTION(_GetHex)
_GetHex:
        push    R10
        push    R11
        push    R12
        push    R13

        mov     R13, p1l
        clr     R10
        clr     R11
        clr     R12
gh_00:
        rcall   _GetChar
        rcall   IsHex
        brtc    gh_xx
        cpi     r1l, '9'+1
        brge    gh_01
        subi    r1l, '0'
        rjmp    gh_02
gh_01:
        cbr     r1l, 0x20
        subi    r1l, 'A'-10
gh_02:
        lsl     R10
        rol     R11
        lsl     R10
        rol     R11
        lsl     R10
        rol     R11
        lsl     R10
        rol     R11
        add     R10, r1l
        inc     R12
        dec     R13
        brne    gh_00
gh_xx:
        mov     p2l, R12
        mov     r1l, R10
        mov     r1h, R11

        pop     R13
        pop     R12
        pop     R11
        pop     R10
        ret
        _ENDFUNC


;-----------------------------------------
; GetHexWord
;
        _FUNCTION(GetHexWord)

GetHexWord:
        ldi     p1l, 4
        rjmp    _GetHex
        _ENDFUNC
;-----------------------------------------
; GetHexByte
;
        _FUNCTION(GetHexByte)

GetHexByte:
        ldi     p1l, 2
        rjmp    _GetHex
        _ENDFUNC
;-----------------------------------------
; GetHexByte
;
        _FUNCTION(GetHexChar)

GetHexChar:
        ldi     p1l, 1
        rjmp    _GetHex
        _ENDFUNC
;-----------------------------------------
; IsHex
;
; Tests RxByte for ascii hex char
; Returns: T bit
;
        _FUNCTION(IsHex)

IsHex:
        push    p1l
        lds     p1l, _RxByte
        clt
        cpi     p1l, '0'
        brlt    ihno
        cpi     p1l, '9'+1
        brlt    ihyes
        cpi     p1l, 'A'
        brlt    ihno
        cpi     p1l, 'F'+1
        brlt    ihyes
        cpi     p1l, 'a'
        brlt    ihno
        cpi     p1l, 'f'+1
        brge    ihno
ihyes:
        set
ihno:
        pop     p1l
        ret
        _ENDFUNC
;-----------------------------------------
; IsWhite
;
; Tests RxByte for space or CR
; Returns: T bit
;
        _FUNCTION(IsWhite)

IsWhite:
        push    p1l
        lds     p1l, _RxByte
        clt
        cpi     p1l, ' '
        breq    ihyes
        cpi     p1l, 0x0D
        breq    ihyes
        rjmp    ihno
        _ENDFUNC
;-----------------------------------------
; void _PutHex(unsigned val, char digits)
;
; Passed:       p2 = Value, p1l = # digits
; RETURN:       Nothing
; USES:         R25, TxByte
; STACK:        2
;
        _FUNCTION(_PutHex)

_PutHex:
        ldi     tmp2, 4          ; Maximum number of digits.
        mov     tmp3, p2l
        mov     p2l, p1l
        mov     p2h, p1h
ph00:
        lsl     p2l
        rol     p2h
        rol     p1l
        lsl     p2l
        rol     p2h
        rol     p1l
        lsl     p2l
        rol     p2h
        rol     p1l
        lsl     p2l
        rol     p2h
        rol     p1l

        cp      tmp3, tmp2
        brlt    ph02

        cbr     p1l, 0xF0
        subi    p1l, lo8(-'0')          ; Addi 0
        cpi     p1l, lo8('9' + 1)       ; if > 9
        brlt    ph01                    ; subi 9 and add A
        subi    p1l, lo8('9' + 1 - 'A')
ph01:
        rcall   PutChar
ph02:
        dec     tmp2
        brne    ph00

        ret
        _ENDFUNC
;-----------------------------------------
; Put out a hex word
;
; Passed: Y = word
;
        _FUNCTION(PutHexWord)

PutHexWord:
        ldi     p2l, 4
        rjmp    _PutHex
        _ENDFUNC
;-----------------------------------------
; Put out a two ascii hex byte
; Yl = byte
;
        _FUNCTION(PutHexByte)

PutHexByte:
        ldi     p2l, 2
        rjmp    _PutHex
        _ENDFUNC
        _END


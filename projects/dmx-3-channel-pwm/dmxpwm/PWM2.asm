; (C) Copyright 2004 Chay Donohoe
; DMX reciever portion based on code by Kathy Quinlan (C) Oct 2002.
; This code is free for non-commercial use.
; For commercial use, please contact chay.donohoe@googlemail.com for details.
;
; Project Notes
; PD0 - DMX In
; PD1 - DMX Out
; PD3 - Red LED (PWM Channel 1)
; PD4 - Green LED (PWM Channel 2)
; PD5 - Blue LED (PWM Channel 3)

; PD2 - Error (Red) LED
; PD6 - DMX (Green) LED
;
; PB0 - PB7 - DMX Address (Inverted, I think...) - Not implemented yet.


.include    "2313def.inc"
.DEVICE     AT90S2313

.def    RAddr       = r5
.def    GAddr       = r6
.def    BAddr       = r7
.def    RVal        = r8
.def    GVal        = r9
.def    BVal        = r10
.def    Temp        = r16
.def    Master      = r17
.def    RPWM        = r18
.def    GPWM        = r19
.def    BPWM        = r20
.def    Status      = r21
.def    DMXCount    = r22
.def    BaseAddress = r23
.def    Data        = r24

; Interrupt Vectors
.org    0x000
    rjmp    RESET       ; RST
    reti                ; INT0
    reti                ; INT1
    reti                ; Timer 1 Capture
    reti                ; Timer 1 Compare
    reti                ; Timer 1 Overflow
    reti                ; Timer 0 Overflow
    rjmp    GET_BYTE    ; UART RX Complete
    reti                ; UART Data Register Empty
    reti                ; UART TX Complete
    reti                ; Analog Compare

.org 0x00b
; ----------------------------------------------------
; Start Here...
; ----------------------------------------------------

RESET:
    cli
    ldi     Temp, RAMEND
    out     SPL, Temp ; Init stack pointer

    ldi     Temp, 0xFF
    out     DDRD, Temp ; Port D all outputs

    ldi     Temp, 0x00
    out     PORTD, Temp ; Port D all off.
    out     DDRB, Temp ; Port B all inputs
    out     PORTB, Temp ; Port B all off.

    ; Zero All User Registers
    mov     Master, Temp
    mov     RPWM, Temp
    mov     GPWM, Temp
    mov     BPWM, Temp
    mov     RVal, Temp
    mov     GVal, Temp
    mov     BVal, Temp
    mov     Data, Temp
    mov     Status, Temp
    mov     DMXCount, Temp

    ; Load Addresses (DMX Addrs currently fixed at R=1 G=2 B=3, need to change so switch settings are read)
    ldi     Temp, 0x01
    mov     RAddr, Temp
    ldi     Temp, 0x02
    mov     GAddr, Temp
    ldi     Temp, 0x03
    mov     BAddr, Temp

    ldi     Temp, 1
    out     UBRR, Temp
    sbi     UCR, RXEN ; RX Enable
    cbi     USR, FE     ;clear frame error flag
    sbi     UCR, CHR9
    sbi     UCR, RXCIE ; Receive INTs enabled
    sei

; ----------------------------------------------------
; Main Program Loop
; ----------------------------------------------------

MAINLOOP:
    rcall   PWMLOOP
    rjmp    MAINLOOP

; ----------------------------------------------------
; Receive Interrupt Handler
; ----------------------------------------------------
GET_BYTE:

        in      Temp, SREG ; Save State
        push    Temp

        in      Data, UDR
        sbic    USR, OR     ;Overrun - Lost Sequence - Start again
        rjmp    OVERRUN

        sbic    USR, FE     ; Frame error flag set - Wait for Startcode
        rjmp    FRAME_ERROR

        sbis    UCR, RXB8 ; Second stop bit is not set, still in break - Wait for startcode
        rjmp    FRAME_ERROR
        
        cpi     Status, 0x00
        breq    OK
        rjmp    OVERRUN

OK:
        cpi     DMXCount, 0x00 ; It's a startcode
        breq    STARTCODE

        cp      DMXCount, RAddr ; Reached Red Address
        breq    LOADRED

        cp      DMXCount, GAddr ; Reached Green Address
        breq    LOADGREEN

        cp      DMXCount, BAddr ; Reached Blue Address
        breq    LOADBLUE

        cpi     DMXCount, 0xFF ; Last Address - Go back to wait for reset.
        breq    OVERRUN

        inc     DMXCount
        pop     Temp
        out     SREG, Temp
        reti

LOADRED:
        mov     RVal, Data
        inc     DMXCount
        pop     Temp
        out     SREG, Temp
        reti
            
LOADGREEN:
        mov     GVal, Data
        inc     DMXCount
        pop     Temp
        out     SREG, Temp
        reti

LOADBLUE:
        mov     BVal, Data
        inc     DMXCount
        pop     Temp
        out     SREG, Temp
        reti

STARTCODE:
        cpi     Data, 0x00
        brne    RETURN
        inc     DMXCount
        pop     Temp
        out     SREG, Temp
        reti

FRAME_ERROR:
        cbi     USR, FE
        ldi     DMXCount, 0x00
        ldi     Status, 0x00
        pop     Temp
        out     SREG, Temp
        reti

OVERRUN:
        ldi     Status, 0x01
        cbi     USR, OR
RETURN:
        ldi     DMXCount, 0x00
        pop     Temp
        out     SREG, Temp
        reti

; -----------------------------
; PWM Core
; -----------------------------
PWMLOOP:
    mov     RPWM, RVal
    mov     GPWM, GVal
    mov     BPWM, BVal
    ldi     Master, 0xFF
    cpi     RPWM, 0x00
    brne    SETREDON
    cbi     PORTD, 3
    rjmp    SETREDOFF
SETREDON:
    sbi     PORTD, 3
SETREDOFF:
    cpi     GPWM, 0x00
    brne    SETGREENON
    cbi     PORTD, 4
    rjmp    SETGREENOFF
SETGREENON:
    sbi     PORTD, 4
SETGREENOFF:
    cpi     BPWM, 0x00
    brne    SETBLUEON
    cbi     PORTD, 5
    rjmp    CHECKRED
SETBLUEON:
    sbi     PORTD, 5

CHECKRED:
    cpi     RPWM, 0x00
    breq    REDOFF
    dec     RPWM
    brne    CHECKGREEN

REDOFF:
    cbi     PORTD, 3

CHECKGREEN:
    cpi     GPWM, 0x00
    breq    GREENOFF
    dec     GPWM
    brne    CHECKBLUE

GREENOFF:
    cbi     PORTD, 4

CHECKBLUE:
    cpi     BPWM, 0x00
    breq    BLUEOFF
    dec     BPWM
    brne    CHECKMASTER
BLUEOFF:
    cbi     PORTD, 5

CHECKMASTER:
    dec     Master
    brne    CHECKRED

    ret

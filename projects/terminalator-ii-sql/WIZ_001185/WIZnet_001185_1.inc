;       File name: WIZnet_001185_1.inc
;
;       project title: "Terminalator II - The SQL"
;       LCD stuff for the AVR Butterfly
;
;       written by Chuck Baird, service <at> cbaird.net
;       this code may be used for any purpose, fair or foul
;       good manners would dictate attribution to the author
;       no warranties expressed or implied - use at your own risk!
;
; ----------- usage
;       rcall   lcd_init        ; do LCD initialization
;       sei                     ; let the interrupts rip
;       ...
;       rcall   lcd_clear       ; clear the buffer
;       rcall   put_in_LCD      ; shift R1 into the LCD buffer from the right
; -----------------------------------------------------------

        .equ     bufsize = 12   ; ours is a 6 character LCD, 2 bytes/char

        .dseg                   ; what follows is SRAM data
lcd_yup: .byte  1               ; if nonzero, update LCD
lbuf:   .byte   bufsize         ; the world famous LCD buffer

        .cseg                   ; what follows is flash code
; -----------------------------------------------------------
;       lcd_fmsg - put out a 6 character flash based message
;
;       z - points to first of six bytes
;           incremented on return

lcd_fmsg:
        push    r1
        push    r16

        ldi     r16,6           ; we'll do 6 chars
llp:    lpm     r1,z+           ; get next character
        rcall   put_in_lcd      ; shift it into lcd
        dec     r16             ; keep count
        brne    llp             ; and repeat if more

        pop     r16
        pop     r1
        ret

; -----------------------------------------------------------
;       i_lcd_sof - LCD start of frame interrupt handler

i_lcd_sof:
        push    r20             ; save r20
        in      r20,sreg        ; copy status flags
        push    r20             ; and save them

        lds     r20,lcd_yup     ; flag that buffer has changed
        tst     r20
        breq    nope            ; nothing has changed

; the main routine has changed the buffer - rewrite it to LCD
        push    r0              ; save some more regs
        push    r16
        push    r17
        push    ZH
        push    ZL

        clr     r20
        sts     lcd_yup,r20     ; clear flag

        ldi     r20,6           ; we'll shoot them 6 characters
        ldi     ZH,high(lbuf)   ; stating at this location
        ldi     ZL,low(lbuf)
        clr     r0              ; which char it is (0 - 5)

again:  ld      r17,Z+          ; low byte of segment bits
        ld      r16,Z+          ; high byte of segment bits
        rcall   lcd_char        ; put it away
        inc     r0              ; character position counter
        dec     r20             ; loop counter
        brne    again

        pop     ZL              ; restore our second batch
        pop     ZH
        pop     r17
        pop     r16
        pop     r0

nope:   pop     r20             ; save status flags
        out     sreg,r20        ; restore them
        pop     r20             ; restore r20
        reti

; -----------------------------------------------------------
;       lcd_char - write segment definition to LCDDR0 register
;
;       parameters:
;         r0 - character position (0 - 5)
;         r16 - high byte of segment bits
;         r17 - low byte of segment bits
;       returns:
;         none
;
;       note: the Atmel documentation counts the LCD characters
;             as starting with position 2.  their 7 is our 5.

lcd_char:
        push    YL
        push    YH
        push    r1
        push    r17

        ldi     YL,low(lcddr0)  ; Y -> where lowest nibble goes
        ldi     YH,high(lcddr0)
        sbrc    r0,1
        adiw    YH:YL,1         ; add 1 if r0 = 2 or 3
        sbrc    r0,2
        adiw    YH:YL,2         ; add 2 if r0 = 4 or 5 

        clr     r1              ; flag for special handling, first time
        rcall   lcd_proc1       ; do byte in r17
        mov     r17,r16         ; r17 needs the argument
        rcall   lcd_proc1       ; do low order byte

        pop     r17
        pop     r1
        pop     YH
        pop     YL
        ret

; -----------------------------------------------------------
;       lcd_proc1 - helper function, write segments to LCD
;
;       parameters:
;         r0 - character position (starting with 0)
;         r1 - special case flag (0 for first call to lcd_proc2)
;         r17 - 8 segment bits, 2 nibbles
;         Y - points into our spot in LCDDR0 register set
;       returns:
;         Y advanced to next register for segments

lcd_proc1:
        push    r17
        push    r2

        mov     r2,r17          ; save r17
        andi    r17,0x0F        ; keep low nibble
        rcall   lcd_proc2       ; go put nibble away
        inc     r1              ; turn off special case

        mov     r17,r2          ; get original
        swap    r17             ; exchange low and high nibbles
        andi    r17,0x0F        ; keep what was high nibble
        rcall   lcd_proc2       ; go put it away

        pop     r2
        pop     r17
        ret

; -----------------------------------------------------------
;       lcd_proc2 - second helper function, write segments to LCD
;
;       parameters:
;         r0 - character position (starting with 0)
;         r1 - zer0 for special case, 1 for normal
;         r17 - 4 segment bits (0 - 3)
;         Y - points to our spot in LCDDR0 register set
;       returns:
;         Y advanced to next register for segments
;
;       this gets called 4 times to put away the segment bits
;       for one character.  on the first call r1 = 0, and we
;       do a tiny bit of special handling

lcd_proc2: 
        push    r18
        push    r17

        ld      r18,Y           ; what's already where ours goes
        sbrc    r0,0            ; is r0 odd or even?
        swap    r18             ; odd - we want high nibble

        tst     r1              ; is this our special case?
        breq    yes
        andi    r18,0xF0        ; no - drop bits 0 to 3
        rjmp    onnow

yes:    andi    r18,0xF6        ; yes - drop bits 0 and 3
        andi    r17,0x09        ; keep only 0 and 3 of incoming

onnow:  or      r18,r17         ; put ours in their place
        sbrc    r0,0            ; check odd/even again
        swap    r18             ; and if odd, reverse them back
        st      Y,r18           ; put the segment data away

        adiw    YH:YL,5         ; seg register areas are 5 bytes long

        pop     r17
        pop     r18
        ret

; -----------------------------------------------------------
;       put_in_LCD - shift a character into the LCD buffer from the right
;
;       parameters:
;         R1 - ASCII character
;       returns:
;         none
;
;       the buffer holds 2 bytes per character.  we'll move
;       lbuf[2] -> lbuf[0], lbuf[3] -> lbuf[1], etc.,
;       and then put the segment definitions for the character
;       in R1 into lbuf[10] and lbuf[11].  Finally we'll let the
;       interrupt handler know it changed by setting lcd_yup = 1

put_in_LCD:
        push    ZH
        push    ZL
        push    r17
        push    r18

        ldi     ZH,high(lbuf)   ; make Z point to lbuf
        ldi     ZL,low(lbuf)

        ldi     r17,bufsize - 2 ; number of bytes that move

ploop:  ldd     r18,Z+2         ; pick up byte from lbuf array
        st      Z+,r18          ; move it 2 bytes to the "left"
        dec     r17             ; knock one off the counter
        brne    ploop           ; and repeat

;       now we're ready to put the new data into lbuf+10 and lbuf+11,
;       but we need to convert from ASCII to segment bits using the
;       segs table in flash
done:   mov     r18,r1          ; move character to r18
        cpi     r18,'a'         ; so we can do immediate compares
        breq    conv            ; convert 'a' to upper case
        brcs    check           ; < 'a' gets checked
        cpi     r18,'z'         ; upper limit of LC
        breq    conv            ; 'z' is ok
        brcs    conv            ; < 'z' is LC that needs converting
        rjmp    bad             ; everything else is out of range

check:  cpi     r18,'*'         ; first character we'll accept
        breq    aok             ; '*' is ok
        brcs    bad             ; < '*' is not
        cpi     r18,'_'         ; last character we'll accept
        breq    aok             ; '_' is ok
        brcs    aok             ; > '_' is ok
bad:    clr     r18             ; bad dog - no segments
        sts     lbuf+10,r18
        sts     lbuf+11,r18
        rjmp    rtn

conv:   subi    r18,32          ; convert LC -> UC
aok:    ldi     ZH,high(segs<<1) ; it's a flash address
        ldi     ZL,low(segs<<1)
        subi    r18,'*'         ; convert ASCII to zero based number
        lsl     r18             ; multiply by 2 (2 bytes per .dw)
       
        add     ZL,r18          ; add offset to ZL
        brcc    ncarry          ; done if no carry
        inc     ZH              ; otherwise increment ZH

ncarry: lpm     r18,Z+          ; grab low byte of segment word
        sts     lbuf+10,r18     ; put it into lbuf at right end
        lpm     r18,Z           ; and the high byte
        sts     lbuf+11,r18     ; and put it into lbuf, too

rtn:    ldi     r18,1
        sts     lcd_yup,r18     ; let the interrupt handler know

        pop     r18
        pop     r17
        pop     ZL
        pop     ZH
        ret

; -----------------------------------------------------------
;       lcd_clear - blank the LCD
;
;       parameters and returns: none

lcd_clear:
        push    r16
        push    zh
        push    zl
        push    r0

;       zero out the buffer which holds our 6 LCD characters' segments
        ldi     r16,bufsize     ; number of bytes in buffer
        ldi     ZH,high(lbuf)   ; high byte of address
        ldi     ZL,low(lbuf)    ; low byte of address
        clr     r0

iloop:  st      Z+,r0           ; zero out buffer
        dec     r16
        brne    iloop
        ldi     r16,1
        sts     lcd_yup,r16     ; mark data as changed

        pop     r0
        pop     zl
        pop     zh
        pop     r16
        ret

; -----------------------------------------------------------
;       lcd_init - LCD initialization
;
;       parameters and returns: none

lcd_init:
        push    r16

        rcall   lcd_clear       ; empty the buffer
        ldi     r16,0x12        ; contrast (4 bits)
        sts     lcdccr,r16

        ldi     r16,(1<<lcdcs) | (3<<lcdmux0) | (7<<lcdpm0)
        sts     lcdcrb,r16      ; external clock, all MUX pins & segs

        ldi     r16,(1<<lcdps0)
        sts     lcdfrr,r16      ; prescale to lcd clk/64

        ldi     r16,(1<<lcdie) | (1<<lcden)
        sts     lcdcra,r16      ; enable lcd and start of frame interrupts

        pop     r16
        ret

; -----------------------------------------------------------
;
;       The following data were modified (borrowed, stolen) from
;       the Atmel Butterfly code.  They are the LCD segment data for
;       the various ASCII codes within our acceptable range.

segs:   .dw 0x0A51      ; '*' (?)
        .dw 0x2A80      ; '+'
        .dw 0x0000      ; ',' (Not defined)
        .dw 0x0A00      ; '-'
        .dw 0x0A51      ; '.' Degree sign
        .dw 0x0000      ; '/' (Not defined)
        .dw 0x5559      ; '0'
        .dw 0x0118      ; '1'
        .dw 0x1e11      ; '2
        .dw 0x1b11      ; '3
        .dw 0x0b50      ; '4
        .dw 0x1b41      ; '5
        .dw 0x1f41      ; '6
        .dw 0x0111      ; '7
        .dw 0x1f51      ; '8
        .dw 0x1b51      ; '9'
        .dw 0x0000      ; ':' (Not defined)
        .dw 0x0000      ; ';' (Not defined)
        .dw 0x0000      ; '<' (Not defined)
        .dw 0x0000      ; '=' (Not defined)
        .dw 0x0000      ; '>' (Not defined)
        .dw 0x0000      ; '?' (Not defined)
        .dw 0x0000      ; '@' (Not defined)
        .dw 0x0f51      ; 'A' (+ 'a')
        .dw 0x3991      ; 'B' (+ 'b')
        .dw 0x1441      ; 'C' (+ 'c')
        .dw 0x3191      ; 'D' (+ 'd')
        .dw 0x1e41      ; 'E' (+ 'e')
        .dw 0x0e41      ; 'F' (+ 'f')
        .dw 0x1d41      ; 'G' (+ 'g')
        .dw 0x0f50      ; 'H' (+ 'h')
        .dw 0x2080      ; 'I' (+ 'i')
        .dw 0x1510      ; 'J' (+ 'j')
        .dw 0x8648      ; 'K' (+ 'k')
        .dw 0x1440      ; 'L' (+ 'l')
        .dw 0x0578      ; 'M' (+ 'm')
        .dw 0x8570      ; 'N' (+ 'n')
        .dw 0x1551      ; 'O' (+ 'o')
        .dw 0x0e51      ; 'P' (+ 'p')
        .dw 0x9551      ; 'Q' (+ 'q')
        .dw 0x8e51      ; 'R' (+ 'r')
        .dw 0x9021      ; 'S' (+ 's')
        .dw 0x2081      ; 'T' (+ 't')
        .dw 0x1550      ; 'U' (+ 'u')
        .dw 0x4448      ; 'V' (+ 'v')
        .dw 0xc550      ; 'W' (+ 'w')
        .dw 0xc028      ; 'X' (+ 'x')
        .dw 0x2028      ; 'Y' (+ 'y')
        .dw 0x5009      ; 'Z' (+ 'z')
        .dw 0x0000      ; '[' (Not defined)
        .dw 0x0000      ; '\' (Not defined)
        .dw 0x0000      ; ']' (Not defined)
        .dw 0x0000      ; '^' (Not defined)
        .dw 0x0000      ; '_'




;       File name: WIZnet_001185_2.inc
;
;       project title: "Terminalator II - The SQL"
;
;       written by Chuck Baird, service <at> cbaird.net
;       this code may be used for any purpose, fair or foul
;       good manners would dictate attribution to the author
;       no warranties expressed or implied - use at your own risk!
;
;       support routines for the USART:
;         usart_init - initialize the USART
;         getchar    - read a character from the USART, if available
;         sendchar   - write a character to the USART
;

; -----------------------------------------------------------
;       usart_crlf - send a CRLF to the USART

usart_crlf:
        push    r16

        ldi     r16,'\r'        ; CR
        rcall   sendchar
        ldi     r16,'\n'        ; LF
        rcall   sendchar

        pop     r16
        ret

; -----------------------------------------------------------
;       usart_flash - send a null terminated string in flash to the USART
;
;       z - points to flash byte address
;           advanced on return

usart_flash:
        push    r16

ulp:    lpm     r16,z+          ; grab next character
        tst     r16             ; check for null
        breq    ufd             ; quit if it is
        rcall   sendchar        ; otherwise send it out
        rjmp    ulp             ; and repeat

ufd:    pop     r16
        ret


; -----------------------------------------------------------
;       usart_init - initialize the USART

usart_init:
        push    r20

        ldi     r20,25          ; 19,200 bps constant
        sts     UBRRL,r20       ; into low order
        clr     r20             ; zero
        sts     UBRRH,r20       ; into high order
        sts     UCSRA,r20       ; set normal speed

        ldi     r20,(1<<RXEN) | (1<<TXEN) ; enable rcvr and xmit
        sts     UCSRB,r20

        ldi     r20,(1<<USBS) | (3<<UCSZ0) ; 8-N-2
        sts     UCSRC,r20

        pop     r20             ; restore register
        ret

; -----------------------------------------------------------
;       getchar - read a character from the USART, perhaps
;
;	returns:
;	  zero flag set if there's no character
;	  zero flag cleared if there's a character, and it
;	    will be in r16

getchar:
        push    r21

        lds     r21,UCSRA       ; get USART flags
        sbrs    r21,RXC         ; is a character ready?
        rjmp    nochar          ; jump if not
        lds     r16,UDR         ; yes - read it
        clz                     ; clear the zero flag
        rjmp    outnow

nochar: sez                     ; set the zero flag
outnow: pop     r21
        ret

; -----------------------------------------------------------
;       sendchar - send a character to the USART
;
;       character to send is in r16

sendchar:
        push    r21

snd2:   lds     r21,UCSRA       ; get USART flags
        sbrs    r21,UDRE        ; is transmitter empty?
        rjmp    snd2            ; if not, go check again
        sts     UDR,r16         ; fire away

        pop     r21
        ret

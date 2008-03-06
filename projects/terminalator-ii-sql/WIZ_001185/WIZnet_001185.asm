;       File name: WIZnet_001185.asm
;
;       Project Title: "Terminalator II - The SQL"
;
;       written by Chuck Baird, service <at> cbaird.net
;       this code may be used for any purpose, fair or foul
;       good manners would dictate attribution to the author
;       no warranties expressed or implied - use at your own risk!

        .nolist
        .include "m169def.inc"  ; definitions for the Atmega 169
        .list

; ------------------------------------------------------
;       W5100 register addresses
#define W_MR    0x0000          ; mode register
#define W_GWR   0x0001          ; gateway address
#define W_IMR   0x0016          ; interrupt mask
#define S0_MR   0x0400          ; socket 0 mode register
#define S0_CMD  0x0401          ; socket 0 command register
#define S0_IR   0x0402          ; socket 0 interrupt register
#define S0_SR   0x0403          ; socket 0 status register
#define S0_PORT 0x0404          ; socket 0 port address, high byte
#define S0_RCNT 0x0426          ; socket 0 recv byte count (S0_RX_RSR)
#define S0_RPTR 0x0428          ; socket 0 recv read pointer (S0_RX_RD)
#define S0_TSZ  0x0420          ; socket 0 trans free size (S0_TX_FSR)
#define S0_TPTR 0x0424          ; socket 0 trans write pointer (S0_TX_WR)

;       other stuff
#define S0_RX   0x6000          ; socket 0 base address, receive buffer
#define S0_TX   0x4000          ; socket 0 base address, transmit buffer
#define S0_RMSK 0x1fff          ; 8K mask - socket 0 receive
#define S0_ROVR 0x80            ; overflow mask, high byte, recv socket 0
#define S0_TMSK 0x1fff          ; 8K mask - socket 0 transmit
#define S0_TOVR 0x20            ; overflow mask, high byte, xmit socket 0

#define STS_CLOSED 0x00         ; SOCK_CLOSED status, S0_SR
#define STS_INIT   0x13         ; SOCK_INIT status, S0_SR
#define STS_EST    0x17         ; SOCK_ESTABLISHED status, S0_SR
#define STS_CWAIT  0x1c         ; SOCK_CLOSE_WAIT status, S0_SR

#define CMD_OPEN   0x01         ; OPEN command, S0_CR
#define CMD_CLOSE  0x10         ; CLOSE command, S0_CR
#define CMD_CON    0x04         ; CONNECT command, S0_CR
#define CMD_RECV   0x40         ; receive processed command, S0_CR
#define CMD_SEND   0x20         ; transmit command, S0_CR
#define CMD_DISC   0x08         ; DISCON command, S0_CR

        .dseg                   ; data segment
xot:    .byte   1               ; number of bytes in obuf
ibuf:   .byte   256             ; the input buffer (from PC to us)
obuf:   .byte   128             ; the output buffer (from terminal, to PC)

        .cseg                   ; code segment
; ------------------------------------------------------
;       the interrupt vectors are at 0x0000
;       we don't need full vectors, but our mother taught us to be polite
        .org 0x0000
        jmp     main            ; reset
        jmp     nullvector      ; IRQ0 handler
        jmp     nullvector      ; PCINT0 handler
        jmp     nullvector      ; PCINT1 handler
        jmp     nullvector      ; Timer 2 compare handler
        jmp     nullvector      ; Timer 2 overflow handler
        jmp     nullvector      ; Timer 1 capture handler
        jmp     nullvector      ; Timer 1 compare A handler
        jmp     nullvector      ; Timer 1 compare B handler
        jmp     nullvector      ; Timer 1 overflow handler
        jmp     nullvector      ; Timer 0 compare handler
        jmp     nullvector      ; Timer 0 overflow handler
        jmp     nullvector      ; SPI transfer complete handler
        jmp     nullvector      ; USART Rx complete handler
        jmp     nullvector      ; USART UDR empty handler
        jmp     nullvector      ; USART Tx complete handler
        jmp     nullvector      ; USI start condition handler
        jmp     nullvector      ; USI overflow handler
        jmp     nullvector      ; analog comparator handler
        jmp     nullvector      ; ADC conversion complete handler
        jmp     nullvector      ; EEPROM ready handler
        jmp     nullvector      ; SPM ready handler
        jmp     i_lcd_sof       ; LCD start of frame handler

nullvector:
        reti

msg1:   .db     " INIT "
msg2:   .db     "Initialization"
        .dw     0
msg3:   .db     "Connection terminated "
        .dw     0
msg4:   .db     "Opening as client "
        .dw     0
msg5:   .db     "Ready for input..."
        .dw     0
msg6:   .db     " COLD "
msg7:   .db     " WARM "

main:   cli                      ; not necessary, but so what
        ldi     r16,high(RAMEND) ; set up stack pointer
        out     SPH,r16
        ldi     r16,low(RAMEND)
        out     SPL,r16

        rcall   USART_init      ; wake up the USART
        rcall   lcd_init        ; and kick the LCD
        sei                     ; start up the band

        ldi     zh,high(msg1<<1) ; let them know we're alive
        ldi     zl,low(msg1<<1)
        rcall   lcd_fmsg
        rcall   usart_crlf       ; space down a bit on terminal
        rcall   usart_crlf
        ldi     zh,high(msg2<<1)
        ldi     zl,low(msg2<<1)
        rcall   usart_flash      ; put a message on the terminal
        rcall   usart_crlf

        rcall   hard_reset_W5100 ; hardware reset of WIZ

        ldi     zh,high(msg6<<1) ; let them know we're alive
        ldi     zl,low(msg6<<1)
        rcall   lcd_fmsg

        rcall   init_SPI         ; set up SPI and I/O bits

        ldi     zh,high(msg7<<1) ; let them know we're alive
        ldi     zl,low(msg7<<1)
        rcall   lcd_fmsg

        rcall   init_WIZnet     ; and the WIZnet
        clr     r5              ; r5 is the shutdown flag
        sts     xot,r5          ; use it to zero buffer count

        ldi     zh,high(msg4<<1)
        ldi     zl,low(msg4<<1)
        rcall   usart_flash     ; put a message on the terminal
        rcall   usart_crlf

        rcall   open_client     ; get going as a client
        tst     r17
        brne    died

        ldi     zh,high(msg5<<1)
        ldi     zl,low(msg5<<1)
        rcall   usart_flash     ; put a message on the terminal
        rcall   usart_crlf
        rcall   usart_crlf

mloop:  rcall   user_input      ; see if the monkeys have written Hamlet
        rcall   host_input      ; process any incoming traffic
        rcall   terminations    ; any of various terminations?
        tst     r17
        breq    mloop            ; we're still alive

died:   rcall   usart_crlf
        ldi     zh,high(msg3<<1) ; "terminated" message
        ldi     zl,low(msg3<<1)
        rcall   usart_flash
        rcall   usart_crlf

here:   rjmp    here            ; die now

; ------------------------------------------------------
;       user_input - see if they've typed something
;
;       we'll do some simple editing and processing, but basically
;       stuff characters into the output buffer until we have either
;       a CR or 128 characters. We'll send CRs, ignore LFs, handle
;       backspaces and escapes, and echo other printables.
;
;       note: this trashes r16

user_input:
        rcall   getchar         ; any soup in the pot?
        brne    inchr
        ret

inchr:  push    zh              ; we have a character in r16
        push    zl
        push    r20

        cpi     r16,0x1b        ; ESC
        breq    c_esc
        cpi     r16,0x0d        ; CR
        breq    c_cr
        cpi     r16,0x08        ; BS
        breq    c_bs
        cpi     r16,0x20        ; see if it's printable
        brlo    ignore
        cpi     r16,0x7f        ; other printable limit
        brsh    ignore

c_cr:   rcall   sendchar        ; echo it
        lds     r20,xot         ; previous output count
        ldi     zh,high(obuf)   ; output buffer base address
        ldi     zl,low(obuf)
        add     zl,r20          ; add #chars in buffer
        brcc    noc
        inc     zh              ; add in carry
noc:    st      z,r16           ; put new character away
        inc     r20             ; count this one
        sts     xot,r20         ; and update   

        cpi     r16,0x0d        ; CR?
        breq    eol             ; yes - transmit the buffer contents
        sbrc    r20,7           ; is the buffer full (cnt = 128?)
        rjmp    eol2            ; jump if yes
        rjmp    ignore          ; all done with this one

eol:    ldi     r16,0x0a        ; CR --> CRLF for display
        rcall   sendchar
eol2:   ldi     zh,high(obuf)   ; send buffer to WIZ
        ldi     zl,low(obuf)
        mov     r16,r20         ; number of bytes to send
        rcall   SRAM_write      ; shoot it out
        clr     r16
        sts     xot,r16         ; buffer is empty now
        rjmp    ignore          ; say goodnight, gracie

;       we're going to assume the terminal can handle a backspace
c_bs:   lds     r20,xot         ; number of chars already
        tst     r20
        breq    ignore          ; if none, ignore backspace
        dec     r20             ; one fewer now
        sts     xot,r20
        rcall   sendchar        ; here's hoping...
        rjmp    ignore

;       ESC just tosses the whole line
c_esc:  ldi     r16,'\\'        ; signal killing the line
        rcall   sendchar
        rcall   usart_crlf      ; new line
        clr     r16
        sts     xot,r16         ; empty the buffer

ignore: pop     r20
        pop     zl
        pop     zh
        ret

; ------------------------------------------------------
;       process_rx - process incoming data
;
;       if something's in the WIZ input buffer, this simply
;       copies it as is to the terminal. hopefully the host
;       did CRLFs, because we don't.
;
;       note: trashes r16

host_input:
        push    zh
        push    zl
        push    r20

        ldi     zh,high(ibuf)   ; input buffer
        ldi     zl,low(ibuf)
        ldi     r16,255         ; max bytes to read
        rcall   recv_read
        tst     r16             ; number actually read
        breq    noin            ; jump if no input
        mov     r20,r16         ; save the value

        ldi     zh,high(ibuf)   ; input buffer
        ldi     zl,low(ibuf)
mo:     ld      r16,z+
        rcall   sendchar
        dec     r20             ; count down
        brne    mo

noin:   pop     r20
        pop     zl
        pop     zh
        ret

; ------------------------------------------------------
;       open_client - open socket 0 and make client connection
;
;       returns: r17 = 0 if good, nonzero if failed

open_client:
        push    yh
        push    yl
        ldi     yh,high(S0_MR)  ; socket mode register address
        ldi     yl,low(S0_MR)
        ldi     r17,0x01        ; set TCP mode
        rcall   WIZ_write
        pop     yl
        pop     yh

        ldi     r17,CMD_OPEN    ; open command
        rcall   WIZ_cmd
wtinit: rcall   get_status      ; check on status
        cpi     r17,STS_CLOSED  ; is it closed?
        breq    fail
        cpi     r17,STS_INIT    ; init ok?
        brne    wtinit

        ldi     r17,CMD_CON     ; connect command
        rcall   WIZ_cmd
wtcon:  rcall   get_status      ; check on status
        cpi     r17,STS_CLOSED  ; did we time out?
        breq    fail
        cpi     r17,STS_EST     ; connection established?
        brne    wtcon

        clr     r17             ; all looks good
        ret

fail:   ser     r17             ; failure of some sort
        ret

; ------------------------------------------------------
;       terminations - see if we need to shut down and do it
;
;       the other end can request it, we can request it, or something
;       may have happened to our connection. who knows, who cares?
;
;       r5 - if nonzero, terminate us (internal request)
;       returns:
;         r17 = 0 if we're still alive
;         r17 nonzero if we shut things down

terminations:
        rcall   get_status      ; check on status
        cpi     r17,STS_CLOSED  ; is it closed?
        breq    closem
        cpi     r17,STS_CWAIT   ; SOCK_CLOSE_WAIT?
        breq    disc            ; yup, disconnect
        tst     r5              ; do our guys want to quit?
        brne    disc            ; jump if yuppers

        clr     r17             ; we live another day
        ret

disc:   ldi     r17,CMD_DISC    ; DISCON command
        rcall   WIZ_cmd
pwait:  rcall   get_status      ; check on status
        cpi     r17,STS_CLOSED  ; is it closed?
        brne    pwait           ; wait until it is

closem: ldi     r17,CMD_CLOSE   ; close command
        rcall   WIZ_cmd
        ser     r17             ; nonzero return
        ret

; ------------------------------------------------------
;       get_status - return the socket 0 status
;
;       r17 - returned status byte

get_status:
        push    yh
        push    yl

        ldi     yh,high(S0_SR)  ; socket 0 status register
        ldi     yl,low(S0_SR)
        rcall   WIZ_read        ; read it

        pop     yl
        pop     yh
        ret

; ------------------------------------------------------
;       wait10ms - wait 10 milliseconds or so
;
;       we're running at 8MHz, so that's 80,000 clock cycles

wait10ms:
        push    xl
        push    xh

        ldi     xl,low(25000)   ; make it 100K cycles
        ldi     xh,high(25000)
ww:     sbiw    xh:xl,1         ; 2 cycles
        brne    ww              ; usually 2 cycles

        pop     xh
        pop     xl
        ret

; ------------------------------------------------------
;       init_SPI - set up SPI for dataflash and WIZnet
;
;       the Butterfly's dataflash memory sits on the SPI bus, which
;       the WIZ810MJ shares. The W5100's SPI_EN is pulled high via a
;       resistor on the breakout board, but must be low to release the
;       SPI bus. Consequently the Butterfly's dataflash may not be
;       accessed without a hardware change to allow MCU control of
;       the SPI_EN line.
;
;       the output ports that need a high signal are dingled prior to
;       setting them output, so that when the switch occurs there won't be
;       a low spike. it does temporarily turn on the pullups, but no matter.
;
;       E4 input  (INT, WIZnet)
;       E5 output (~SCS, WIZnet. 0 to select)
;       E6 output (WIZ810MJ reset line. pull low to reset)
;       E7 output (reset, dataflash. 0 to reset, 10 microseconds)
;       B0 output (~CS, dataflash. 0 to select)
;       B1 output (SCK)
;       B2 output (MOSI)
;       B3 input  (MISO)

init_SPI:
        push    r16

        cbi     ddre,porte4     ; E4 is an input (INT, WIZnet)

        sbi     porte,porte5    ; ~SCS high to deselect WIZnet
        sbi     ddre,porte5     ; E5 is an output (~SCS, WIZnet, 0 = select)

        sbi     porte,porte6    ; WIZnet reset (high = no reset)
        sbi     ddre,porte6     ; E6 is an output (WIZnet reset, 0 = reset)

        sbi     porte,porte7    ; dataflash reset (high = no reset)
        sbi     ddre,porte7     ; E7 is an output (dataflash reset, 0 = reset)

        sbi     portb,portb0    ; ~CS high to deselect dataflash
        sbi     ddrb,portb0     ; B0 is an output (dataflash ~CS)

        sbi     ddrb,portb1     ; B1 is an output (SCK, low when idle - mode 0)

        sbi     ddrb,portb2     ; B2 is an output (MOSI)
        cbi     ddrb,portb3     ; B3 is an input (MISO)

        rcall   hard_reset_W5100 ; hit it with a stick

        ldi     r16,(1<<spi2x)  ; SPI double speed
        out     spsr,r16

        ldi     r16,(1<<spe)|(1<<mstr)|(0<<cpha)|(0<<cpol)
        out     spcr,r16        ; enable SPI, master, mode 0

        pop     r16
        ret

; ------------------------------------------------------
;       WIZnet_32 - do a 32 bit SPI dingle with the WIZnet
;
;       y   - W5100 address to read or write (incremented on return)
;       r16 - lead in byte (read or write command - 0x0f or 0xf0)
;       r17 - following byte (byte to write or don't care for read)
;             returned as don't care for write or value for read

WIZnet_32:
        push    r18

        cbi     porte,porte5    ; ~SCS low to select WIZnet
        mov     r18,r16         ; read or write command
        rcall   WIZ_out
        mov     r18,yh          ; high address
        rcall   WIZ_out
        mov     r18,yl          ; low address
        rcall   WIZ_out
        mov     r18,r17         ; data or don't care
        rcall   WIZ_out

        in      r17,spdr        ; grab incoming byte
        sbi     porte,porte5    ; ~SCS high to deselect WIZnet
        adiw    yh:yl,1         ; increment address

        pop     r18
        ret

; ------------------------------------------------------
;       WIZ_out - exchange byte with WIZnet
;
;       r18 - byte going out - garbaged on return

WIZ_out:
        out     spdr,r18        ; going out
WZ_wt:  in      r18,spsr        ; watch spif flag
        sbrs    r18,spif        ; 0 means busy
        rjmp    WZ_wt

        ret

; ------------------------------------------------------
;       WIZ_read - read a byte from the WIZ, plus overhead
;
;       y - address (incremented on return)
;       r17 - returned value

WIZ_read:
        push    r16

        ldi     r16,0x0f        ; read command
        rcall   WIZnet_32       ; send 4 bytes

        pop     r16
        ret

; ------------------------------------------------------
;       WIZ_write - write a byte to the WIZ, plus overhead
;
;       y - address (incremented on return)
;       r17 - byte to write

WIZ_write:
        push    r16
        push    r17

        ldi     r16,0xf0        ; write command
        rcall   WIZnet_32       ; send 4 bytes

        pop     r17
        pop     r16
        ret

; ------------------------------------------------------
;       WIZ_cmd - send socket 0 command to WIZ
;
;       r17 - command to send

WIZ_cmd:
        push    yh
        push    yl

        ldi     yh,high(S0_CMD) ; socket 0 command register
        ldi     yl,low(S0_CMD)
        rcall   WIZ_write

        pop     yl
        pop     yh
        ret

; ------------------------------------------------------
;       hard_reset_W5100 - hardware reset on the W5100 chip
;
;       generate a 2 microsecond low on reset line, then wait 10 ms
;       we'll be generous on all counts

hard_reset_W5100:
        push    r16

        cbi     porte,porte6    ; low = reset
        ldi     r16,7           ; kill a little time for W5100 reset pulse
resdly: dec     r16             ; only need a couple of microseconds, or 16 clocks
        brne    resdly
        sbi     porte,porte6    ; end the WIZnet reset pulse

        rcall   wait10ms        ; now go twiddle thumbs for 10 ms or so

        pop     r16
        ret

; ------------------------------------------------------
;       soft_reset_W5100 - software reset on the W5100 chip
;
;       send 0x80 to the mode register, and wait for high
;       bit to go to zero

soft_reset_W5100:
        push    yh
        push    yl
        push    r17

        ldi     yh,high(W_MR)   ; mode register address
        ldi     yl,low(W_MR)
        ldi     r17,0x80        ; reset bit high
        rcall   WIZ_write       ; and write the mode register

rloop:  ldi     yh,high(W_MR)   ; mode register address
        ldi     yl,low(W_MR)
        rcall   WIZ_read        ; get register value
        andi    r17,0x80        ; want MSB
        brne    rloop           ; still in reset

        pop     r17
        pop     yl
        pop     yh
        ret

; ------------------------------------------------------
;       WIZ_string - write a flash string to WIZnet
;
;       z - flash byte address (incremented on return)
;       y - beginning WIZ address (incremented on return)
;       r16 - count of bytes (trashed on return)

WIZ_string:
        push    r17

smore:  lpm     r17,z+          ; next byte from flash
        rcall   WIZ_write       ; write it to WIZ
        dec     r16             ; one less bell to answer
        brne    smore           ; rinse and repeat

        pop     r17
        ret 

; ------------------------------------------------------
;       init_WIZnet - set up the TCP stuff
;
;       highjacked MAC address: 00-0f-66-d5-83-34
;       IP address: 192.168.1.200
;       gateway: 192.168.1.1
;       subnet mask: 255.255.255.0
;       socket mem: 0 - 8K rx/tx, all others 1K (not used)
;       source port: 19001 (0x4a39)
;       destination MAC: 00-17-3f-9b-2e-54
;       destination IP: 192.168.1.100
;       destination port: 19002 (0x4a3a)

init_WIZnet:
        push    zh
        push    zl
        push    yh
        push    yl
        push    r16

        rcall   soft_reset_W5100 ; do a software reset

        ldi     yh,high(W_GWR)  ; gateway register address
        ldi     yl,low(W_GWR)
        ldi     zh,high(istr_1 << 1) ; the first initialization string
        ldi     zl,low(istr_1 << 1)
        ldi     r16,18          ; string length
        rcall   WIZ_string      ; write string to WIZ

        ldi     yh,high(W_IMR)  ; interrupt mask register address
        ldi     yl,low(W_IMR)
        ldi     zh,high(istr_2 << 1) ; the second initialization string
        ldi     zl,low(istr_2 << 1)
        ldi     r16,6           ; string length
        rcall   WIZ_string      ; write string to WIZ

        ldi     yh,high(S0_PORT) ; source port, socket 0
        ldi     yl,low(S0_PORT)
        ldi     zh,high(istr_3 << 1) ; the third initialization string
        ldi     zl,low(istr_3 << 1)
        ldi     r16,16          ; string length
        rcall   WIZ_string      ; write string to WIZ

        pop     r16
        pop     yl
        pop     yh
        pop     zl
        pop     zh

        ret

; string 1 is: gateway (4), subnet (4), mac (6), IP (4), total = 18
istr_1: .dw 0xa8c0,0x0101,0xffff,0x00ff,0x0f00,0xd566,0x3483,0xa8c0,0xc801

; string 2 is: interrupt mask (1), timeout value (2), timeout count (1),
;              rx mem (1), tx mem (1), total = 6
istr_2: .dw 0x0f00,0x08a0,0x0303

; string 3 is: source port (2), dest MAC (6), dest IP (4), dest port (2),
;              max seg size (2), total 16
istr_3: .dw 0x394a,0x1700,0x9b3f,0x542e,0xa8c0,0x6401,0x3a4a,0xb405

; ------------------------------------------------------
;       recv_read - read from W5100 receive buffer, socket 0
;
;       z   - where to put returned values (SRAM address)
;       r16 - maximum bytes to read (1 - 255)
;             returned as bytes read (may be zero)
;
;       note: z is returned pointing at next available location,
;       so multiple calls may be made to concatenate data

recv_read:
        tst     r16
        brne    rok             ; yes, a skip on the flags would be better
        ret                     ; request for zero bytes

rok:    push    yh
        push    yl
        push    xh
        push    xl
        push    r0
        push    r17
        push    r24
        push    r25

        ldi     yh,high(S0_RCNT) ; recv byte count, high address
        ldi     yl,low(S0_RCNT)
        rcall   WIZ_read
        mov     xh,r17          ; read high byte of count
        rcall   WIZ_read
        mov     xl,r17          ; then low byte of count
        or      r17,xh          ; put them together
        breq    nodata          ; if zero, receiver empty

        ldi     yh,high(S0_RPTR) ; recv pointer, high address
        ldi     yl,low(S0_RPTR)
        rcall   WIZ_read
        mov     r25,r17         ; read high byte of address
        rcall   WIZ_read
        mov     r24,r17         ; then low byte of address
        movw    yh:yl,r25:r24   ; drop raw address into y

;       we can play a little loose with the mask and add here because
;       the numbers are so well behaved. when we're done, y will
;       be the actual WIZ address of the next byte of receiver data.
        andi    yh,high(S0_RMSK) ; mask to get offset
        ori     yh,high(S0_RX)  ; add the recv buffer base addr
        clr     r0              ; our byte counter

nxtrcv: rcall   WIZ_read        ; grab next byte
        st      z+,r17          ; put it away
        inc     r0              ; count it
        dec     r16             ; the number they asked for
        breq    rdone           ; jump if we got them all
        sbiw    xh:xl,1         ; the number available
        breq    rdone           ; jump if we read them all
        mov     r17,yh          ; see if we hit top of recv buf
        andi    r17,S0_ROVR
        breq    nxtrcv          ; jump if we did not top out
        ldi     yh,high(S0_RX)  ; or, back to beginning of buffer
        ldi     yl,low(S0_RX)
        rjmp    nxtrcv

rdone:  mov     r16,r0          ; returned count
        clr     r0
        add     r24,r16         ; add count to buf pointer
        adc     r25,r0

        ldi     yh,high(S0_RPTR) ; recv pointer, high address
        ldi     yl,low(S0_RPTR)
        mov     r17,r25
        rcall   WIZ_write       ; write high byte of address
        mov     r17,r24
        rcall   WIZ_write       ; and low byte of address

        ldi     r17,CMD_RECV    ; receive done command
        rcall   WIZ_cmd
        rjmp    rbye

nodata: clr     r16

rbye:   pop     r25
        pop     r24
        pop     r17
        pop     r0
        pop     xl
        pop     xh
        pop     yl
        pop     yh
        ret

; ------------------------------------------------------
;       SRAM_write - write to the W5100 from SRAM
;
;       z   - where the data is (SRAM address)
;       r16 - maximum bytes to write (1 - 255)
;             returned as # transmitted (0 or original value - no partials)
;
;       note: z is returned pointing at next character location,
;             so multiple calls may be made for contiguous data

SRAM_write:
        push    r18

        clr     r18
        rcall   trans_write

        pop     r18
        ret

; ------------------------------------------------------
;       flash_write - write to the W5100 from flash
;
;       z   - where the data is (flash byte address)
;       r16 - maximum bytes to write (1 - 255)
;             returned as # transmitted (0 or original value - no partials)
;
;       note: z is returned pointing at next character location,
;             so multiple calls may be made for contiguous data

flash_write:
        push    r18

        ser     r18
        rcall   trans_write

        pop     r18
        ret

; ------------------------------------------------------
;       trans_write - write to the W5100 receive buffer, socket 0
;
;       z   - where the data is (SRAM or flash)
;       r16 - maximum bytes to write (1 - 255)
;             returned as # transmitted (0 or original value - no partials)
;       r18 - zero for SRAM, nonzero for flash
;
;       note: z is returned pointing at next available location,
;             so multiple calls may be made to concatenate data

trans_write:
        tst     r16             ; check write count
        brne    tok
        ret                     ; ignore request for zero bytes

tok:    push    yh
        push    yl
        push    xh
        push    xl
        push    r0
        push    r17
        push    r24
        push    r25

        mov     r0,r16          ; save original count
        ldi     yh,high(S0_TSZ) ; transmit free size, high address
        ldi     yl,low(S0_TSZ)
        rcall   WIZ_read
        tst     r17
        brne    tszok           ; if nonzero, it's big enough
        rcall   WIZ_read
        cp      r17,r16
        brlo    small           ; not big enough - sorry

tszok:  ldi     yh,high(S0_TPTR) ; trans write ptr, high address
        ldi     yl,low(S0_TPTR)
        rcall   WIZ_read
        mov     r25,r17         ; read high byte of address
        rcall   WIZ_read
        mov     r24,r17         ; then low byte of address
        movw    yh:yl,r25:r24   ; drop raw address into y

;       we can play a little loose with the mask and add here because
;       the numbers are so well behaved. when we're done, y will
;       be the WIZ address to write the next byte of transmitted data.
        andi    yh,high(S0_TMSK) ; mask to get offset
        ori     yh,high(S0_TX)  ; add the xmit buffer base addr

nxttx:  tst     r18             ; SRAM or flash?
        breq    inram
        lpm     r17,z+          ; read from flash
        rjmp    ntx
inram:  ld      r17,z+          ; read from SRAM
ntx:    rcall   WIZ_write       ; hand it off to the WIZ
        dec     r16             ; the number they asked for
        breq    tdone           ; jump if we got them all
        mov     r17,yh          ; see if we hit top of txmit buf
        andi    r17,S0_TOVR
        breq    nxttx           ; jump if we did not top out
        ldi     yh,high(S0_TX)  ; or, back to beginning of buffer
        ldi     yl,low(S0_TX)
        rjmp    nxttx

tdone:  clr     r16             ; redundant, but who cares?
        add     r24,r0          ; add count to buf pointer
        adc     r25,r16

        ldi     yh,high(S0_TPTR) ; write pointer, high address
        ldi     yl,low(S0_TPTR)
        mov     r17,r25
        rcall   WIZ_write       ; write updated pointer, high byte
        mov     r17,r24
        rcall   WIZ_write       ; and low byte of pointer

        ldi     r17,CMD_SEND    ; shoot it out
        rcall   WIZ_cmd
        rjmp    tbye

small:  clr     r0
tbye:   mov     r16,r0          ; original value or zero

        pop     r25
        pop     r24
        pop     r17
        pop     r0
        pop     xl
        pop     xh 
        pop     yl
        pop     yh
        ret

; ------------------------------------------------------

#include "WIZnet_001185_1.inc"
#include "WIZnet_001185_2.inc"

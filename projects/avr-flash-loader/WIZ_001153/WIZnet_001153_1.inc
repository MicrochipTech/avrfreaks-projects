;	File name: WIZnet_001153_1.inc
;
;	Project Title: "AVR Flash Loader"
;
;       these are the W5100 routines, more or less
;
;       written by Chuck Baird, service <at> cbaird.net
;       this software may be used for any purpose, fair or foul
;       good manners would dictate attribution to the author
;       no warranties, expressed or implied - use at your own risk!
;
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
        rcall   wait10ms        ; not necessary, but so what?
wtinit: rcall   get_status      ; check on status
        cpi     r17,STS_CLOSED  ; is it closed?
        breq    fail
        cpi     r17,STS_INIT    ; init ok?
        brne    wtinit

        ldi     r17,CMD_CON     ; connect command
        rcall   WIZ_cmd
        rcall   wait10ms        ; ditto on above
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
        breq    closed
        cpi     r17,STS_CWAIT   ; SOCK_CLOSE_WAIT?
        breq    disc            ; yup, disconnect
        tst     r5              ; do our guys want to quit?
        brne    wtdisc          ; jump to shutdown
        clr     r17             ; we live another day
        ret

wtdisc: ldi     r17,25          ; gonna wait about 1/4 second first
wsm:    rcall   wait10ms        ; twiddle thumbs
        dec     r17
        brne    wsm

disc:   ldi     r17,CMD_DISC    ; DISCON command
        rcall   WIZ_cmd
        rcall   wait10ms        ; docs and experience disagree
        ldi     r17,CMD_CLOSE   ; close command
        rcall   WIZ_cmd
closed: ser     r17             ; nonzero return
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
        rcall   wait10ms        ; and be leisurely about it

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

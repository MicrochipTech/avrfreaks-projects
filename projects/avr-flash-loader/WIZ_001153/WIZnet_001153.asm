;       File name: WIZnet_001153.asm
;
;       Project Title: "AVR Flash Loader"
;
;       written by Chuck Baird, service <at> cbaird.net
;       this software may be used for any purpose, fair or foul
;       good manners would dictate attribution to the author
;       no warranties, expressed or implied - use at your own risk!
;
; ------------------------------------------------------
;       command                action              originates
;       -------                ------              ----------
;         X                    erase app memory    host (PC)
;         F page <data>        write page flash    host
;         E addr count <data>  write bytes EEPROM  host
;         f page               read page flash     host
;         e page               read page EEPROM    host
;         J                    jump to app         host
;         A                    ack                 client
;         <data>               returned data       client
;
;       the client sends an ack when it starts up.
;       pages are numbered 0-111 for flash and 0-3 for EEPROM.
;       pages are 64 words (128 bytes) long.
;       EEPROM addresses are 0 - 511. requested/written data is limited
;          to 128 bytes.
;       EEPROM addresses are 2 bytes, high first.
;       page numbers are 1 byte.
;       maximum message length from client to host is 128 bytes.
;       maximum message length from host to client is 132 bytes.
;       <data> is usually 128 bytes, but can be less for eeprom writes.
; ------------------------------------------------------
;
;       note: this code is specific to the ATmega169.  changes
;             would be necessary for other configurations of sram,
;             page size, etc.  interrupts must be disabled in the
;             bootloader, or else the spm instructions need to be
;             protected & the vector relocation taken care of.
;
        .nolist
        .include "m169def.inc"   ; definitions for the Atmega 169
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
xin:    .byte   1               ; number of bytes in ibuf
xot:    .byte   1               ; number of bytes in obuf
ibuf:   .byte   134             ; the input buffer (from PC to us)
obuf:   .byte   130             ; the output buffer (from us to PC)

        .cseg
; ------------------------------------------------------
;       these are the "real" interrupt vectors at 0x0000
;       this isn't needed, but is here for completeness
       .org     0x0000

        jmp     appmain         ; application entry point
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
        jmp     nullvector      ; LCD start of frame handler

nullvector:
        reti

appmain:
        ldi     r16,0x20        ; pin 5
        mov     r17,r16         ; save a copy
        out     ddrb,r16        ; piezo is output
        ldi     zh,high(cnt<<1) ; address of count
        ldi     zl,low(cnt<<1)
        lpm     xl,z+           ; pick up count
        lpm     xh,z

more:   movw    yh:yl,xh:xl
        out     portb,r16
        eor     r16,r17         ; toggle piezo pie
wt:     sbiw    yh:yl,1
        nop
        nop
        brne    wt              ; finish countdown
        rjmp    more            ; repeat

cnt:    .dw     2000            ; frequency

; ------------------------------------------------------
;       this is the bootloader section
        .org    0x1C00
        jmp     bootloader      ; relocated reset comes here

; a polite person would put jumps to the various user callable routines
; here, simulating interrupt vectors so the application would have access
; to the WizNet capabilities that exist in the bootloader

; ------------------------------------------------------
;       bootloader entry point

bootloader:
        cli                      ; not necessary, but so what
        ldi     r16,high(RAMEND) ; set up stack pointer
        out     SPH,r16
        ldi     r16,low(RAMEND)
        out     SPL,r16
        clr     r5              ; used to terminate connection

        rcall   hard_reset_W5100 ; hardware reset of WIZ
        rcall   init_SPI        ; set up SPI and I/O bits
        rcall   init_WIZnet     ; and the WIZnet
        rcall   open_client     ; get going as a client
        tst     r17
        brne    failed

        rcall   ack             ; tell them we're alive
        rcall   outgoing        ; send it
        rcall   wait10ms        ; loiter

mloop:  rcall   incoming        ; process any incoming traffic
        rcall   outgoing        ; send anything that might be waiting
        rcall   terminations    ; check for various terminations
        tst     r17
        breq    mloop           ; nope - keep going

failed: ldi     r17,CMD_CLOSE   ; close command
        rcall   WIZ_cmd
        clr     zl              ; jump to application via reset vector
        clr     zh
        icall                   ; and fly away

; ------------------------------------------------------
;       incoming - process things coming at us
incoming:
        push    zh
        push    zl
        push    r20

        ldi     zh,high(ibuf)   ; input buffer
        ldi     zl,low(ibuf)
        ldi     r16,134         ; max bytes to read
        rcall   recv_read
        tst     r16             ; number actually read
        breq    noin            ; jump if no input

        sts     xin,r16         ; save size in case
        ldi     zh,high(ibuf)   ; input buffer
        ldi     zl,low(ibuf)
        ld      r20,z+          ; the command, z-->next char

        cpi     r20,'E'         ; check for a match
        brne    t1
        rcall   write_eeprom
        rjmp    ackgo

t1:     cpi     r20,'F'
        brne    t2
        rcall   write_flash
        rjmp    ackgo

t2:     cpi     r20,'e'
        brne    t3
        rcall   read_eeprom
        rjmp    noin            ; no ack on reads

t3:     cpi     r20,'f'
        brne    t4
        rcall   read_flash
        rjmp    noin            ; no ack on reads

t4:     cpi     r20,'X'
        brne    t5
        rcall   erase
        rjmp    ackgo

t5:     cpi     r20,'J'
        brne    noin            ; don't know - ignore it
        rcall   wait_rdy        ; make sure all spm/eeprom stuff is done
        rcall   unlock_rww      ; and that we didn't lock things up
        inc     r5              ; this will shut us down eventually

ackgo:  rcall   ack             ; ack the command
noin:   pop     r20
        pop     zl
        pop     zh
        ret

; ------------------------------------------------------
;       write_eeprom - write EEPROM block
;
;       command: E
;       [z] -->  addr (0-511, hi/low) count (1-128) data (max 128 bytes)
;       r20 may be used for scratch

write_eeprom:
        push    r16
        push    yh
        push    yl

        ld      yh,z+           ; eeprom address to write
        ld      yl,z+
        ld      r20,z+          ; count

        rcall   wait_rdy        ; make sure all is ready
elp:    andi    yh,0x01         ; paranoid - keep address legal
        out     eearh,yh
        out     eearl,yl
        ld      r16,z+          ; get byte from buffer
        out     eedr,r16        ; write it to output register
        sbi     eecr,eemwe      ; master enable bit on
        sbi     eecr,eewe       ; set the write bit

wte1:   sbic    eecr,eewe       ; watch EEWE for completion
        rjmp    wte1            ; and wait

        adiw    yh:yl,1         ; incr eeprom address
        dec     r20
        brne    elp

        pop     yl
        pop     yh
        pop     r16
        ret

; ------------------------------------------------------
;       write_flash - write flash block
;
;       command: F
;       [z] -->  page (0-111) data (128 bytes)
;       r20 may be used for scratch

write_flash:
        push    r0
        push    r1
        push    r16
        push    yh
        push    yl

        ld      r20,z+          ; flash page to write
        clr     r16             ; start with word 0

ldl:    ld      r0,z+           ; low byte
        ld      r1,z+           ; high byte
        rcall   word_to_temp    ; put it in temp page buffer
        inc     r16             ; next word
        sbrs    r16,6           ; are we to 64 yet?
        rjmp    ldl             ; repeat if not

        mov     r16,r20         ; page we want to write
        rcall   write_temp      ; and write page buf to flash

        rcall   wait_rdy        ; let it finish
        rcall   unlock_rww      ; and unlock flash
        pop     yl
        pop     yh
        pop     r16
        pop     r1
        pop     r0
        ret

; ------------------------------------------------------
;       read_eeprom - read EEPROM page
;
;       command: e
;       [z] -->  page (0-3)
;       put 128 bytes into output buffer
;       r20 may be used for scratch

read_eeprom:
        push    r16
        push    yh
        push    yl

        ld      yh,z            ; page to read
        andi    yh,0x03         ; paranoid
        clr     yl              ; convert to byte address
        lsr     yh              ; bit into carry
        ror     yl              ; and into yl[7]
        ldi     r20,128         ; byte count

        sts     xot,r20         ; outgoing count
        ldi     zl,low(obuf)    ; output buffer
        ldi     zh,high(obuf)

        rcall   wait_rdy        ; make sure everything's done
xelp:   out     eearh,yh
        out     eearl,yl

        sbi     eecr,eere       ; initiate the read
        in      r16,eedr        ; and pick up incoming byte
        st      z+,r16
        adiw    yh:yl,1         ; incr eeprom address
        dec     r20
        brne    xelp

        pop     yl
        pop     yh
        pop     r16
        ret

; ------------------------------------------------------
;       read_flash - read flash block
;
;       command: f
;       [z] -->  page (0-111)
;       put 128 bytes into output buffer
;       r20 may be used for scratch

read_flash:
        push    r16
        push    yh
        push    yl

        ld      r16,z           ; page to read
        rcall   page_to_z       ; spm address is also lpm address
        ldi     r16,128         ; byte count
        sts     xot,r16         ; outgoing count
        ldi     yl,low(obuf)    ; output buffer
        ldi     yh,high(obuf)
        rcall   wait_rdy        ; make sure all spm activity done

rfl:    lpm     r20,z+          ; move them bytes
        st      y+,r20
        dec     r16
        brne    rfl

        pop     yl
        pop     yh
        pop     r16
        ret

; ------------------------------------------------------
;       word_to_temp - write a word into the temp page buffer
;
;       r16 - word address (0 - 63)
;       r1:r0 - word to write

word_to_temp:
        push    zh
        push    zl
        push    r17

        clr     zh              ; page address is ignored
        mov     zl,r16          ; word address
        lsl     zl              ; scoot address left 1 bit

        rcall   wait_rdy        ; be sure last one is finished
        ldi     R17,1<<SPMEN    ; no special cases
        out     SPMCSR,R17
        spm                     ; write r1:r0

        pop     r17
        pop     zl
        pop     zh
        ret

; ------------------------------------------------------
;       write_temp - write temp page to flash memory
;
;       r16 - page (0 - 111)

write_temp:
        push    zh
        push    zl
        push    r17

        rcall   page_to_z       ; build z address
        rcall   wait_rdy        ; be sure last one is finished
        ldi     R17,(1<<PGWRT)|(1<<SPMEN)
        out     SPMCSR,R17
        spm                     ; blast away

        pop     r17
        pop     zl
        pop     zh
        ret

; ------------------------------------------------------
;       erase - erase all of application memory
;
;       we need to erase 112 pages, 0 - 111

erase:  push    r16

        clr     r16
nxter:  rcall   erase_page      ; go do one page
        inc     r16
        cpi     r16,112
        brne    nxter
        rcall   wait_rdy        ; let last one finish
        rcall   unlock_rww      ; re-enable flash

        pop     r16
        ret

; ------------------------------------------------------
;       erase_page - erase a single page (64 words) of lower flash
;
;       r16 - page number (0 - 111)
;
;       note: application memory is 112 pages, 0x0000 to 0x0BFF (words)
;             bootloader memory is 16 pages, 0x1C00 to 0x1FFF (words)
;

erase_page:
        push    zh
        push    zl
        push    r17

        rcall   page_to_z       ; build z address
        rcall   wait_rdy        ; be sure last one is finished
        ldi     R17,(1<<PGERS)|(1<<SPMEN)
        out     SPMCSR,R17
        spm                     ; fire in the hole

        pop     r17
        pop     zl
        pop     zh
        ret

; ------------------------------------------------------
;       wait_rdy - wait for spm and/or eeprom ops to complete

wait_rdy:
        push    r17

wt1:    in      R17,spmcsr
        sbrc    R17,spmen       ; SPMEN bit high means busy
        rjmp    wt1             ; wait some more
wt2:    sbic    eecr,eewe       ; wait for eeprom writes to finish
        rjmp    wt2

        pop     r17
        ret

; ------------------------------------------------------
;       unlock_rww - re-enable RWW memory

unlock_rww:
        push    r17

        ldi     r17,(1<<rwwsre)|(1<<spmen)
        out     spmcsr,r17
        spm

        pop     r17
        ret

; ------------------------------------------------------
;       page_to_z - build z from page number for spm
;
;       r16 - page number (0 - 111)
;       returns: z register in spm format
;
;       Z addressing for spm: 00pp pppp pbbb bbb0
;            where p = 7 bit page address
;                  b = 6 bit buffer word address
;                 64 word pages (128 bytes)

page_to_z:
        mov     zh,r16          ; max value is 0x6f
        clr     zl
        lsr     zh              ; align zh
        ror     zl              ; and drop carry into zl[7]

        ret

; ------------------------------------------------------
;       ack - send an ACK
;
;       put ack (A) into the output buffer as its only character

ack:    push    r16

        ldi     r16,'A'         ; an ACK
        sts     obuf,r16        ; put it in the output buffer
        ldi     r16,1           ; char count
        sts     xot,r16

        pop     r16
        ret

; ------------------------------------------------------
;       outgoing - send stuff back if we have any
outgoing:
        push    r16

        lds     r16,xot
        tst     r16
        breq    none

        push    zh
        push    zl

        ldi     zh,high(obuf)   ; output buffer
        ldi     zl,low(obuf)
        rcall   SRAM_write      ; send it out
        clr     r16
        sts     xot,r16        ; it's gone

        pop     zl
        pop     zh
none:   pop     r16
        ret

; ------------------------------------------------------

#include        "WIZnet_001153_1.inc"

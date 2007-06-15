;	File Name: AT3283_1.asm
;
;	Copyright 2006 by Chuck Baird, http:www.cbaird.net
;	All rights reserved
;
;	Definitions and interrupt vectors
;
; ==============================================
;
; The famous command language:
;
;   commands are case sensitive
;   acks are '!' and naks are '?'
;   #'s in the response column are hex digits:
;       # is one digit, ## is 2, ### is 1 to 6 digits
;       positions are in 24 bit 2's complement numbers.
;       if abbreviation is on ('a'), leading zeros and leading F's are
;         removed (one '-' indicates deleted leading F's)
;       if abbreviation is off ('A'), 6 digits are always returned
;   'B' returns bit flags: 000XYLMR where X and Y are overflow flags,
;     L, M, and R are current state (1 = clicked) of buttons
;   buttons are L, M, and R; Ports are A-D, Pins are 0-7
;
; Command     Example   Response        Comments
; -------     -------   --------        --------
; A or a      A         ack             Abbreviate (a, default) or expand (A)
;                                       numeric position output
; V           V         VVack		Version number ("VV" is version string)
; e or E      E         ack		Enable (E) or disable (e) echo
; 0,1, or 2   0         ack/nak         Select active mouse
; I           I         ack/nak         Initialize mouse
; S           S         ack/nak         Set the current location as the origin
; X,Y, or Z   X         ###ack or nak   Read the current offset
; *           *         ##ack or nak    Read & reset error report
;                                       bits: YX###### where YX are overflows,
;                                       low order is rejected packet count
;
; B           B         #ack or nak     Read the buttons
;                                       Bits: 0LMR, =1 if clicked 
;
; xBtn,yBtn,  xL        ###ack or nak   Read the saved offset from when
; or zBtn     ym                        button last changed. UC button
;                                       is H->L, LC button is L->H
;
; PPort or    PC        ack or nak	Set port (P) or pin (p) to
; pPortPin    pD2                       input with pullups enabled
;
; QPort or    QC        ack or nak	Set port (Q) or pin (q) to
; qPortPin    qD2                       input with pullups disabled
;
; DPort or    DC        ack or nak	Set port (D) or pin (d) to
; dPortPin    dD2                       output
;
; WportPin    WC6       ack or nak      Write a zero (w) or one (W) to the
; wPortPin    wB1                       specified port/pin
;
; Rport       RA        #ack or nak     Read high (R) or low (r) nibble
; rPort       rC                        from specified port (PINx)
;
; Tport       RA        #ack or nak     Read high (R) or low (r) nibble
; tPort       rC                        from specified port (PORTx)
;
; L...        LMC13     ack or nak      Lock a button to a port and pin in
;                                       the specified mode
;
; UBtn        UM	ack or nak      Unlock the specified button
;
; The L command specifies the button (L, M, or R), the port (A to D),
; the pin (0 to 7), and the mode in that order. Implemented modes are:
;    0  unlock (default, no association to any pin)
;    1  reflect (pin is 1 if button is pressed, 0 otherwise)
;    2  invert (pin is 0 if button is pressed, 1 otherwise)
;    3  toggle (pin toggles when button changes)
;    4  toggle on click (pin toggles when button is clicked)
;    5  toggle on release (pin toggles when button is released)
;
; The U command unlocks a button (sets its mode to 0).  The same effect
; can be achieved using the L command for any pin and port with mode 0.
;
; ==============================================
.nolist
.include	"m16def.inc"
.list

;	the string "wiring" defines the pins & ports for the mice -- see below

.set	nak	= '?'			; the negative acknowledge character
.set	ack	= '!'			; the acknowledge character


;	the following are the ucsrb setup values
.set	u_int	= (1<<txen)|(1<<rxen)|(1<<rxcie)|(1<<udrie)
.set	u_noint	= (1<<txen)|(1<<rxen)|(1<<rxcie)

;	these determine USART buffer sizes 
.set	rbsize	= 5			; number of bits in receiver head/tail
.set	xbsize	= 7			; number of bits in transmitter head/tail
.set	rbufsize = 1<<rbsize		; size of receiver buffer
.set	xbufsize = 1<<xbsize		; size of transmitter buffer
.set	rmask	= rbufsize - 1		; receive buffer offset mask
.set	xmask	= xbufsize - 1		; transmit buffer offset mask

;	these determine the mouse receive buffers sizes
.set	mbsize	= 5			; number of bits in mouse buffer mask
.set	mbufsize = 1<<mbsize		; size of each mouse buffer
.set	mrmask	= mbufsize - 1		; mouse receive buffer head/tail mask

.set	mblksize = 58			; how big a mouse desriptor block is
;	the following tell where things are in moff() for each mouse
.set	mo_sts	= 0			; line status
.set	mo_hd	= 1			; head of list in mbuf()
.set	mo_t	= 2			; tail of list in mbuf()
.set	mo_s	= 3			; send character
.set	mo_sp	= 4			; send parity
.set	mo_sc	= 5			; send bit count
.set	mo_r	= 6			; receive character
.set	mo_rp	= 7			; receive parity
.set	mo_rc	= 8			; receive bit count
.set	mo_id	= 9			; mouse ID (0 = normal, 3 = scroll)
.set	mo_but	= 10			; button & overflow status
.set	mo_chg	= 11			; buttons that changed
.set	mo_msgsz = 12			; message size: 3 or 4, 0 = no init
.set	mo_msg	= 13			; start of 4 byte mouse msg buffer
;	the following is a 4 byte timer block
;	the following 3 offsets are used in timer0 int handler
.set	ztlo	= 1
.set	zthi	= 2
.set	zerr	= 3
.set	mo_tmode = 17			; timer mode (0=stopped, 1=8 bit, 2=16 bit)
.set	mo_tlo	= mo_tmode+ztlo		; low timeout timer
.set	mo_thi	= mo_tmode+zthi		; high timeout timer
.set	mo_err	= mo_tmode+zerr		; line timing error

;	the following 9 bytes need to be in this order
.set	mo_x0	= 21			; low order X offset
.set	mo_x1	= 22			; middle X offset
.set	mo_x2	= 23			; high order X offset
.set	mo_y0	= 24			; low order Y offset
.set	mo_y1	= 25			; middle Y offset
.set	mo_y2	= 26			; high order Y offset
.set	mo_z0	= 27			; low order Z offset
.set	mo_z1	= 28			; middle Z offset
.set	mo_z2	= 29			; high order Z offset
;	this is 3 sets of 6 byte lock definitions (LMR)
;	order is: pin, port, ddr, and msk, or msk, mode
.set	mo_llock = 30			; left btn PIN byte
.set	mo_mlock = 36			; middle btn PIN byte
.set	mo_rlock = 42			; right btn PIN byte
;	the 5 data bytes must be followed by the 5 clock bytes:
.set	wd_pin	= 48			; data line PIN address
.set	wd_port = 49			; data line PORT address
.set	wd_ddr	= 50			; data line DDR address
.set	wd_and	= 51			; data AND mask (1 bit off)
.set	wd_or	= 52			; data OR mask (1 bit on)
.set	wc_pin	= 53			; clock line PIN address
.set	wc_port = 54			; clock line PORT address
.set	wc_ddr	= 55			; clock line DDR address
.set	wc_and	= 56			; clock AND mask (1 bit off)
.set	wc_or	= 57			; clock OR mask (1 bit on)


.set	w_pin	= 0			; offsets within subblocks, above.
.set	w_port	= 1			; these are used by unfold2 to get
.set	w_ddr	= 2			; values into the correct places
.set	w_and	= 3			; as define above. those who would
.set	w_or	= 4			; be tempted to alter, don't!
.set	w_mode	= 5			; used only in lock definitions

;	the button transition save area is 54 bytes long per mouse
;	3 bytes per axis, 3 axes per transition, 2 transitions per button,
;	3 buttons per mouse, 3 mice supported
.set	btblksz	= 9			; one save size (3 * 3 bytes)
.set	btsavsize = 6 * btblksz		; 54 bytes (one mouse)

;	these are the timing limits. documentation says a mouse clock is
;	10 to 16.7 KHz, so an 11 bit character should come in in under a
;	millisecond.  we make a guess that successive packer bytes are
;	sent fairly quickly in sequence.  the timers have a millisecond
;	of slop since a timer can be ready to roll over right when it's set.
.set	pbtval	= 3			; milliseconds from start of
;					; byte n to start of byte n+1
.set	ptval3	= pbtval * 3		; 3 byte packet timer value
.set	ptval4	= pbtval * 4		; 4 byte packet timer value

;	the packet manager uses the following, one per mouse
.set	packsize = 12			; size of a packet definition
.set	psts	= 0			; packet status (0 = init, 1 = idle, 2 = saving)
.set	perr	= 1			; packet error flag
.set	psize	= 2			; packet size (copy of moff(mo_msgsize))
.set	pbytes	= 3			; number of bytes received
.set	phead	= 4			; buffer head save area
.set	preject	= 5			; packet reject count
.set	pready	= 6			; number of packets ready
.set	ptimer	= 7			; packet timer
.set	pbtimer = 8			; packet byte timer
.set	ptrun	= 9			; flag if packet timer is running
.set	pbtrun	= 10			; flag if packet byte timer is running
.set	ptval	= 11			; timer value for packet timer

.dseg

cmdbuf:	.byte	8			; incoming command buffer
bigcheese: .byte 1			; the active mouse
;	the button transition save area is 54 bytes long per mouse
;	order is: left, hi->lo, x0,1,2,y0,1,2,z0,1,2
;	          left, lo->hi, same order
;		  middle, hi->lo, etc.
btsav:	.byte	btsavsize * 3		; button transition save area
cbuf:	.byte	50			; output string buffer
;	xbuf and rbuf are circular buffers, sized to a power to 2
;	xmask and rmask keep the heads and tails within their sizes
xbuf:	.byte	xbufsize		; USART transmit buffer
rbuf:	.byte	rbufsize		; USART receive buffer
xhead:	.byte	1			; transmit buf head offset
xtail:	.byte	1			; transmit buf tail offset
rhead:	.byte	1			; receive buf head offset
rtail:	.byte	1			; receive buf tail offset
;	mbuf contains 3 mouse receive circular buffers, int0 first
;	moff contains 3 sets of the following:
;		+0 - mouse status
;			0 - no activity
;			1 - initiating host to mouse
;			2 - send in progress
;			3 - waiting ACK
;			4 - receiving
;			5 - within stop bit
;		+mo_hd - head of buffer offset
;		+mo_t  - tail of buffer offset
;		+mo_s  - send byte
;		+mo_sp - send parity
;		+mo_sc - send bit count
;		+mo_r  - receive byte
;		+mo_rp - receive parity
;		+mo_rc - receive bit count
;		data line descriptor (5 bytes, see below)
;			+wd_XXXX
;		clock line descriptor (5 bytes)
;			+wc_XXXX:
;			    PIN address (incl $20 offset)
;			    PORT address (incl $20 offset)
;			    DDR address (incl $20 offset)
;			    AND mask (1 bit off)
;			    OR mask (1 bit on)

;	mrmask keeps heads and tails within proper sizes
mbuf:	.byte	3 * mbufsize		; 3 mouse buffers
moff:	.byte	3 * mblksize		; 3 mouse descriptor blocks
pindef:	.byte	5			; unfold2 buffer area
echo232: .byte	1			; echo to host (0 = off)
abbrev:	.byte	1			; abbreviate output (0 = yes)
pnum:	.byte	6			; expansion buffer for numeric output
pack:	.byte	3 * packsize		; packet manager block
take5:	.byte	1			; special 5 ms timer

; ====================== Interrupt vectors ========================
.cseg
.org	0x0000
	jmp	i_reset			; reset
	jmp	i_irq0			; external 0
	jmp	i_irq1			; external 1
	jmp	i_tim2c			; timer 2 compare
	jmp	i_tim2o			; timer 2 overflow
	jmp	i_tim1cpt		; timer 1 capture
	jmp	i_tim1ca		; timer 1 compare a
	jmp	i_tim1cb		; timer 1 compare b
	jmp	i_tim1o			; timer 1 overflow
	jmp	i_tim0o			; timer 0 overflow
	jmp	i_spi_tc		; spi xfer complete
	jmp	i_usartrx		; usart rx complete
	jmp	i_usartun		; usart udr empty
	jmp	i_usarttx		; usart tx complete
	jmp	i_adc			; adc complete
	jmp	i_eerdy			; eeprom ready
	jmp	i_anacomp		; analog comparator
	jmp	i_twsi			; two wire serial
	jmp	i_irq2			; external 2
	jmp	i_tim0c			; timer 0 compare
	jmp	i_spm			; store prog mem rdy

; ==============================================
;
version: .db	"10"			; version number (2 chars sent)

;	crec defines the command recognition. it consists of pairs of chars,
;	the first being the command name and the second being the number of
;	bytes in the command.  the position in the list determines the cmd's
;	jump table entry (i.e., 'B' is jumptable + 3). '~' ends list.
crec:	.db	"011121B1D2d3L5P2R2r2S1W3w3X1x2Y1y2Z1z2I1e1E1p3Q2q3U2V1A1a1T2t2*1~~"

;	wiring defines how the 3 mice are hooked up, 4 bytes per mouse, with separation.
;	order is: data port, data pin, clock port, clock pin, separator, repeat
;	the clock pins are fixed (int0,1,& 2), so this must match "--D2.--D3.--B2"
wiring:	.db	"D4D2.D5D3.B0B2"

;	these are the mouse initialization strings
iseq1:	.db	"sFFrFAAA00wsFFrFAAA00wsFFrFAAA00wx"		; 3 resets
iseq2:	.db	"sF3rFAsC8rFAsF3rFAs64rFAsF3rFAs50rFAsF2rFAix"	; scroll mouse mode
iseq3:	.db	"sE8rFAs03rFAsE6rFAsF3rFAs28rFAsF4rFAxx"	; res=3, scale 1:1, samp=40, enable
;
; ====================== Reset comes here ========================

i_reset:
	ldi	r16,high(RAMEND)	; set up stack pointer
	out	SPH,r16
	ldi	r16,low(RAMEND)
	out	SPL,r16
	rjmp	monster			; main routine

; ==============================================

i_tim2c:		; interrupt: timer 2 compare
i_tim2o:		; interrupt: timer 2 overflow
i_tim1cpt:		; interrupt: timer 1 capture
i_tim1ca:		; interrupt: timer 1 compare a
i_tim1cb:		; interrupt: timer 1 compare b
i_tim1o:		; interrupt: timer 1 overflow
i_tim0o:		; interrupt: timer 0 overflow
i_spi_tc:		; interrupt: spi xfer complete
i_usarttx:		; interrupt: usart tx complete
i_adc:			; interrupt: adc complete
i_eerdy:		; interrupt: eeprom ready
i_anacomp:		; interrupt: analog comparator
i_twsi:			; interrupt: two wire serial
i_spm:			; interrupt: store prog mem rdy
		reti	; mr. paranoid

; ==============================================

.include	"AT3283_2.inc"
.include	"AT3283_3.inc"
.include	"AT3283_4.inc"
.include	"AT3283_5.inc"
.include	"AT3283_6.inc"
.include	"AT3283_7.inc"

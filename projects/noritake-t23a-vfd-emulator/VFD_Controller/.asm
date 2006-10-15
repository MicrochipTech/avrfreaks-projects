;VFD controller processor. 
;This will control an AS VFD or Application Specific Vacuum Flourescent Display. 
;Such as the one used in a car stereo, etc..
;Emulates the Noritake T23A/T28A instruction set
;by Mike Bates

;Pin Definements for VFD Shift Registers
;PA0	-	Strobe
;PA1	-	Blanking
;PA2	-	Alpha-Shift-Clock
;PA3	-	Alpha-Shift-Data
;PA4	-	Bar-Shift-Clock
;PA5	-	Bar-Shift-Data
;PA6	-	Grid-Shift-Clock
;PA7	-	Grid-Shift-Data

;PortC used as inputs for parallel loading. 8bit
;PD3	-	Filement Clock
;PD4	-	Chip Select
;PD5	-	Write Enable
;PD6	-	Busy

.equ	Strobe	=	0
.equ	Blank	=	1
.equ 	AlphaClock	=	2
.equ	AlphaData	=	3
.equ	BarClock	=	4
.equ	BarData		=	5
.equ	GridClock	=	6
.equ	GridData	=	7


.equ	Filament	=	3
.equ	CS			=	4
.equ	WE			=	5
.equ	Busy		=	6

;include ATmega163 definition files
.include "m163def.inc"
.device ATmega163

.def	temp	=	r16
.def	count1	=	r17
.def	count2  = 	r18
.def	scancount  =   r19
.def	charposition	=	r20
.def	RefreshPointer	=	r21
.def	zero			=	r22
.def	Row				=	r23
.def	GridL			=	r24



;Define SRAM registers:

.dseg
.org 0x0060
RefreshBuff:	.byte 40		;begin segment refreshbuffer in SRAM at 0060
BarBuff:		.byte 40		;begin Bar Refresh Register
.cseg


;Begin interrupt Jump Table

.org 0

	jmp 	RESET		 ; Reset Handler
	jmp 	0			 ; IRQ0 Handler
	jmp 	0		 	; IRQ1 Handler
	jmp 	0			 ; Timer2 Compare Handler
	jmp 	0		 	; Timer2 Overflow Handler
	jmp 	0			 ; Timer1 Capture Handler
	jmp 	0			 ; Timer1 Compare A Handler
	jmp 	0 			; Timer1 Compare B Handler
	jmp 	0	 		; Timer1 Overflow Handler
	jmp 	scan		 ; Timer0 Overflow Handler
	jmp 	0			 ; SPI Transfer Complete Handler
	jmp 	0			 ; UART RX Complete Handler
	jmp 	0			 ; UDR Empty Handler
	jmp 	0			 ; UART TX Complete Handler
	jmp 	0	 		; ADC Conversion Complete Interrupt Handler
	jmp 	0	 		; EEPROM Ready Handler
	jmp 	0			; Analog Comparator Handler
	jmp 	0		 ; Two-wire Serial Interface Interrupt Handler





RESET: ;begin program.
	cli							;turn off interrupts

	ldi 	temp, high(RAMEND) 	; Main program start
	out 	SPH, temp 			; Set stack pointer to top of RAM
	ldi 	temp, low(RAMEND)
	out 	SPL, temp
	ldi		temp, $FF
	out		DDRA, temp			;set portA as outputs.
	out		DDRB, temp			;fuck it, lets set port B as outputs also. :-)
	ldi		temp, $00
	out 	DDRC, temp			;set portC as inputs
	sbi		DDRD, 6				;set portD.6 as output, rest inputs
	sbi		DDRD, 3
	sbi		portD, 4
	sbi		portD, 5
	ldi		scancount, $FF		;Set at $FF so when first interrupt hits, it starts at $00
	out		portC, scancount	;turn on the internal pull-ups
	clr		refreshpointer
	clr		zero
	clr		charposition
	clr		row

	ldi 	temp, $02		;Use temp to initaize timer prescaler..
	out 	TCCR0, temp
	ldi 	temp,$00		;Use temp to initialize counter.
	out 	TCNT0,temp
	ldi 	temp, $01		;Use temp to initialize TIMSK.
	out 	TIMSK,temp
	

clear:							
	ldi		YH, High(RefreshBuff)			;Clear the RAM
	ldi		YL, Low(refreshBuff)
	ldi		count1, $50
lopp:
	ldi		temp, $00
	st		Y, temp
	adiw	YL, 1
	dec		count1
	brne	lopp						
	sei						;enable the interrupts
	cbi		portD, Busy



;wait for incoming parallel command, otherwise the interrupt will scan display. 
IDLETOP:
	
	sbic	PIND, CS		;check to see if chip select is high, if not, check again. 
	rjmp	IDLETOP
	sbic	PIND, WE		;Check and see if the write enable is held high, if not, start over.
	rjmp	IDLETOP

;command recieved from parallel port, read and go process
Main:
	clr 	temp			;read the command from portC
	in		temp, PINC
checkloop:	
	sbis	PIND, WE		;check and see if rising edge of Write pulse. if so, then process, otherwise, wait
	rjmp	checkloop
	rcall 	processcommand	;go process the read command. 
	rjmp	IDLETOP			;start over again.


ProcessCommand:				;process the byte that was read
	sbi		portD, Busy		;Set the busy bit to tell CPU we are processing display commands. 
	cpi		temp, $1B		;check for escapechar
	brne	A2
	cbi		portD, Busy
	rjmp	ProcessEscape	;go process the escape command
A2:
	cpi		temp, $08		;check for backspace command
	brne	A3	
	rcall	Action2			;go perform backspace action
	rjmp	endprocess		;end command processing
A3:
	cpi		temp, $09		;check for tab command	
	brne	A4
	rcall	Action3
	rjmp	endprocess
A4:
	cpi		temp, $0C		;check for form-feed command. we return to home. $00
	brne	A5	
	ldi		charposition, $00
	ldi		row, $00
	rjmp	endprocess
A5:
	cpi		temp, $0D		;check for carrage return command, returns cursur back to home. $00
	brne	A6
	cpi		Row, $01		;check if we are on the second row.
	breq	pc+3 
	ldi		charposition, $00	;home our cursor on first row
	rjmp	endprocess
	ldi		charposition, $14	;home our cursor on second row
	ldi		row, $01
	rjmp	endprocess
A6:
	cpi		temp, $0E		;check for clear screen command. cursur doesnt move. 
	brne	A7
	rjmp	clear
	rjmp	endprocess
A7:
	cpi		temp, $0A		;check for row feed. move to next row, if we are on next row, move back to first. 
	brne	A8
	rcall	Action4			;go call our line feed command
	rjmp	endprocess
A8:							;assume any nonmatching commands are ascii, print ascii on display, advance cursur. 
	rcall	asciitoseg
	rcall	writetorefresh	;write ASCII char into refresh RAM
	cpi		charposition, $27
	breq	resetchar
	inc		charposition
	rjmp	endprocess

resetchar:
	clr		charposition

EndProcess:	
	clr		temp
	cbi		portD, busy		;we are done processing commands. 
	ret



;this subroutine processes the T23 escape command. $1B. read more bytes for more actions
ProcessEscape:
	;grab next byte
	sbic	PIND, CS		;check to see if chip select is high, if not, check again. 
	rjmp	ProcessEscape
	sbic	PIND, WE		;Check and see if the write enable is held high, if not, start over.
	rjmp	ProcessEscape
	clr		temp
	in		temp, PINC
	sbis	pinD, WE		;wait until rising edge of clock pulse
	rjmp	pc-1
	sbi		portD, Busy		;set busy flag high
	
	;interpret next byte and perform action
	cpi		temp, $48
	breq	performaction
	cpi		temp, $49
	brne	endescape
	out		portD, zero
	rjmp	RESET

performaction:
	cbi		portD, busy
	rcall	action1			;go perform action 1

endescape:
	clr		temp
	cbi		portD, busy
	ret





;action statement that moves the charposition/curser
Action1:
	;grab next byte
	sbic	PIND, CS		;check to see if chip select is high, if not, check again. 
	rjmp	Action1
	sbic	PIND, WE		;Check and see if the write enable is held high, if not, start over.
	rjmp	Action1
	clr		temp
	in		temp, PINC
	sbis	pinD, WE		;wait until rising edge of clock pulse
	rjmp	pc-1
	sbi		portD, Busy

	;interpret next byte and perform action
	clr		charposition
	add		charposition, temp	
	ret

;skip the cursur 1 place backwards
Action2:
	cpi		charposition, $00
	brne	pc+2
	ret
	cpi		charposition, $13
	breq	pc+3
	subi	charposition, $01
	ret
	clr		row
	subi	charposition, $01
	ret

;skip the cursur 1 place forwards
Action3:
	cpi		charposition, $27
	brne	pc+2
	ret
	cpi		charposition, $13
	breq	pc+4
	ldi		temp, $01
	add 	charposition, temp
	ret
	ldi		row, $01
	mov		temp, row
	add		charposition, temp
	ret


;this subroutine moves cursur to the next row, leaving the column intact. if we are second row, moves back to first.
Action4:
	cpi		row, $00		;see if we are on the first row. 
	brne	Row1
row0:
	ldi		temp, $14
	add		charposition, temp
	ldi		row, $01
	ret
row1:
	ldi		temp, $14
	sub		charposition, temp
	clr		row
	ret


;this subroutine converts the ascii value in TEMP, and stores segment data into Y
ASCIItoSeg:
	cpi		temp, '1'
	brne	n01
	ldi		YH, 0b00000000
	ldi		YL, 0b00000110
	ret
n01:
	cpi		temp, '2'
	brne	n02
	ldi		YH, 0b00000000
	ldi		YL, 0b11011011
	ret
n02:
	cpi		temp, '3'
	brne	n03
	ldi		YH, 0b00000000
	ldi		YL, 0b10001111
	ret
n03:
	cpi		temp, '4'
	brne	n04
	ldi		YH, 0b00000000
	ldi		YL, 0b11100110
	ret
n04:
	cpi		temp, '5'
	brne	n05
	ldi		YH, 0b00001000
	ldi		YL, 0b01101001
	ret
n05:
	cpi		temp, '6'
	brne	n06
	ldi		YH, 0b00000000
	ldi		YL, 0b11111100
	ret
n06:
	cpi		temp, '7'
	brne	n07
	ldi		yh, 0b00000000
	ldi		YL, 0b00000111
	ret
n07:
	cpi		temp, '8'
	brne	n08
	ldi		yh, 0b00000000
	ldi		yl, 0b11111111
	ret
n08:
	cpi		temp, '9'
	brne	n09
	ldi		yh, 0b00000000
	ldi		yl, 0b11100111
	ret
n09:
	cpi		temp, '0'
	brne	n10
	ldi		yh, 0b00100100
	ldi		yl, 0b00111111
	ret
n10:
	cpi		temp, 'A'
	breq	pc+3
	cpi		temp, 'a'
	brne	n11
	ldi		yh, 0b00000000
	ldi		yl, 0b11110111
	ret
n11:
	cpi		temp, 'B'
	breq	pc+3
	cpi		temp, 'b'
	brne	n12
	ldi		yh, 0b00010010
	ldi		yl, 0b10001111
	ret
n12:
	cpi		temp, 'C'
	breq	pc+3
	cpi		temp, 'c'
	brne	n13
	ldi		yh, 0b00000000
	ldi		yl, 0b00111001
	ret
n13:
	cpi		temp, 'D'
	breq	pc+3
	cpi		temp, 'd'
	brne	n14
	ldi		yh, 0b00010010
	ldi		yl, 0b00001111
	ret
n14:
	cpi		temp, 'E'
	breq	pc+3
	cpi		temp, 'e'
	brne	n15
	ldi		yh, 0b00000000
	ldi		yl, 0b01111001
	ret
n15:
	cpi		temp, 'F'
	breq	pc+3
	cpi		temp, 'f'
	brne	n16
	ldi		yh, 0b00000000
	ldi		yl, 0b01110001
	ret
n16:
	cpi		temp, 'G'
	breq	pc+3
	cpi		temp, 'g'
	brne	n17
	ldi		yh, 0b00000000
	ldi		yl, 0b10111101
	ret
n17:
	cpi		temp, 'H'
	breq	pc+3
	cpi		temp, 'h'
	brne	n18
	ldi		yh, 0b00000000
	ldi		yl, 0b11110110
	ret
n18:
	cpi		temp, 'I'
	breq	pc+3
	cpi		temp, 'i'
	brne	n19
	ldi		yh, 0b00010010
	ldi		yl, 0b00001001
	ret
n19:
	cpi		temp, 'J'
	breq	pc+3
	cpi		temp, 'j'
	brne	n20
	ldi		yh, 0b00000000
	ldi		yl, 0b00011110
	ret
n20:
	cpi		temp, 'K'
	breq	pc+3
	cpi		temp, 'k'
	brne	n21
	ldi		yh, 0b00001100
	ldi		yl, 0b01110000
	ret
n21:
	cpi		temp, 'L'
	breq	pc+3
	cpi		temp, 'l'
	brne	n22
	ldi		yh, 0b00000000
	ldi		yl, 0b00111000
	ret
n22:
	cpi		temp, 'M'
	breq	pc+3
	cpi		temp, 'm'
	brne	n23
	ldi		yh, 0b00000101
	ldi		yl, 0b00110110
	ret
n23:
	cpi		temp, 'N'
	breq	pc+3
	cpi		temp, 'n'
	brne	n24
	ldi		yh, 0b00001001
	ldi		yl, 0b00110110
	ret
n24:
	cpi		temp, 'O'
	breq	pc+3
	cpi		temp, 'o'
	brne	n25
	ldi		yh, 0b00000000
	ldi		yl, 0b00111111
	ret
n25:
	cpi		temp, 'P'
	breq	pc+3
	cpi		temp, 'p'
	brne	n26
	ldi		yh, 0b00000000
	ldi		yl, 0b11110011
	ret
n26:
	cpi		temp, 'Q'
	breq	pc+3
	cpi		temp, 'q'
	brne	n27
	ldi		yh, 0b00001000
	ldi		yl, 0b00111111
	ret
n27:
	cpi		temp, 'R'
	breq	pc+3
	cpi		temp, 'r'
	brne	n28
	ldi		yh, 0b00001000
	ldi		yl, 0b11110011
	ret
n28:
	cpi		temp, 'S'
	breq	pc+3
	cpi		temp, 's'
	brne	n29
	ldi		yh, 0b00000000
	ldi		yl, 0b11101101
	ret
n29:
	cpi		temp, 'T'
	breq	pc+3
	cpi		temp, 't'
	brne	n30
	ldi		yh, 0b00010010
	ldi		yl, 0b00000001
	ret
n30:
	cpi		temp, 'U'
	breq	pc+3
	cpi		temp, 'u'
	brne	n31
	ldi		yh, 0b00000000
	ldi		yl, 0b00111110
	ret
n31:
	cpi		temp, 'V'
	breq	pc+3
	cpi		temp, 'v'
	brne	n32
	ldi		yh, 0b00100100
	ldi		yl, 0b00110000
	ret
n32:
	cpi		temp, 'W'
	breq	pc+3
	cpi		temp, 'w'
	brne	n33
	ldi		yh, 0b00101000
	ldi		yl, 0b00110110
	ret
n33:
	cpi		temp, 'X'
	breq	pc+3
	cpi		temp, 'x'
	brne	n34
	ldi		yh, 0b00101101
	ldi		yl, 0b00000000
	ret
n34:
	cpi		temp, 'Y'
	breq	pc+3
	cpi		temp, 'y'
	brne	n35
	ldi		yh, 0b00010101
	ldi		yl, 0b00000000
	ret
n35:
	cpi		temp, 'Z'
	breq	pc+3
	cpi		temp, 'z'
	brne	n36
	ldi		yh, 0b00100100
	ldi		yl, 0b00001001
	ret
n36:
	cpi		temp, '.'
	brne	n37
	ldi		yh, 0b01000000
	ldi		yl, 0b00000000
	ret
n37:
	cpi		temp, ':'
	brne	n38
	ldi		YH, 0b11000000
	ldi		YL, $00
	ret
n38:
	cpi		temp, '-'
	brne	n39
	ldi		yh, 0b00000000
	ldi		yl, 0b11000000
	ret
n39:
	cpi		temp, '='
	brne	n40
	ldi		yh, 0b00000000
	ldi		yl, 0b11001001
	ret
n40:
	cpi		temp, '{'
	breq	pc+5
	cpi		temp, '['
	breq	pc+3
	cpi		temp, '('
	brne	n41
	ldi		yh, 0b00001100
	ldi		yl, 0b01000000
	ret
n41:
	cpi		temp, '}'
	breq	pc+5
	cpi		temp, ']'
	breq	pc+3
	cpi		temp, ')'
	brne	n42
	ldi		yh, 0b00100001
	ldi		yl, 0b10000000
	ret
n42:
	cpi		temp, $01
	brne	n43
	ldi		Yh, 0b00000000
	ldi		yl, 0b00000001
	ret
n43:
	cpi		temp, $02
	brne	n44
	ldi		yh, 0b00000000
	ldi		yl, 0b00000011
	ret
n44:
	cpi		temp, $03
	brne	n45
	ldi		yh, $00
	ldi		yl, 0b00000111
	ret
n45:
	cpi		temp, $04
	brne	n46
	ldi		yh, $00
	ldi		yl, 0b00001111
	ret
n46:
	cpi		temp, $05
	brne	n47
	ldi		yh, $00
	ldi		yl, 0b00011111
	ret
n47:
	cpi		temp, $06
	brne	n48
	ldi		yh, $00
	ldi		yl, 0b00111111
	ret
n48:
	cpi		temp, $07
	brne	n49
	ldi		yh, $00
	ldi		YL, 0b01111111
	ret
n49:
	cpi		temp, ','
	brne	n50
	ldi		yh, 0b00100000
	ldi		yl, 0b00000000
	ret
n50:
	cpi		temp, $27
	brne	n51
	ldi		yh, 0b00000100
	ldi		yl, 0b00000000
	ret

n51:
	cpi		temp, $FF
	brne	n52
	ldi		yh, $FF
	ldi		yl, $FF
	ret

n52:
	cpi		temp, '*'
	brne	n53
	ldi		yh, 0b00111111
	ldi		yl, 0b11000000
	ret
n53:
	cpi		temp, '&'
	brne	n54
	ldi		yh, 0b00010010
	ldi		yl, 0b11111001
	ret
n54:
	ldi		yl, $00
	ldi		yh, $00
	ret


;this subroutine writes the segment data in Y, to refreshram with an offset of charposition X.
WriteToRefresh:

	ldi		XH, High(refreshbuff)
	ldi		XL, Low(refreshbuff)

	ldi		temp, $02
	mul		temp, charposition
	mov		temp, R0
	add		XL, temp
	st		X, YH
	inc		XL
	st		X, YL
	ret


;This subroutine scans the VFD display from the left to the right. Uses Round-Robin based timing. 
;Each row that is scanned, the data for this row is in Refresh Ram beginning at 0100.
Scan:

	sbi		PortD, Filament
	push 	temp			;push our working registers on the stack before wiping them out here. 
	push	ZL
	push    ZH
	push	XL
	push	XH
	push	YL
	push	YH
	lds		temp, SREG
	push 	temp


	inc		scancount 		;point to the next grid row
	cpi		scancount, $0C	;only 12 rows on the VFD to scan. if we hit 12($0B), reset back to row 1 ($00)
	brne	DoScan			;if we are between rows 1 and 12, then Do Scan of Next Row. 
	clr		scancount		;if we are done scanning one frame, reset back to beginning for next frame. 
	clr		RefreshPointer
	cbi		portA, Strobe				;turn off the output latches of VFD	
	sbi		portA, Blank				;blank the display
	clr		temp
	rcall	gridshift
	ori		temp, 0b00000001
	rcall	GridShift					;setup the first bit in the grid. 
	rjmp	Continue

;Scan the display with segment data in Refresh Ram.
DoScan:

;load Alphanumeric data and write it to the current row. 	
	cbi		portA, Strobe				;turn off the output latches of VFD	
	sbi		portA, Blank				;blank the display
		
;	nop
	sbi		portA, GridClock			;shift gridbit one place over. (ring counter)
;	nop
	cbi		portA, GridClock			
	cpi		scancount, $00
	brne	continue
	clr		temp
	rcall	gridshift
	ori		temp, 0b00000001
	rcall	GridShift					;setup the first bit in the grid. 	

Continue:
	ldi		YH, HIGH(RefreshBuff)		;load the refreshbuffer pointer 0x0060 into Y for pointer processing
	ldi		YL, LOW(refreshbuff)

	ldi		XH, HIGH(BarBuff)			;load Bargraph refresh buffer into X
	ldi		XL, LOW(BarBuff)
	
	add		YL, RefreshPointer			;Add the pointer to Y. so if Y is 0060, and pointer is 1, then 0061
	adc		YH, zero

	add		XL, RefreshPointer			;add the pointer to X also for Bargraph Buffer. 
	adc		XH, zero

	ld		temp, Y						;load Upper 8 bits of segment data from SRAM.
	rcall	AlphaShift					;shift out the first byte. MSB to LSB High byte first, then Low byte
	
	ld		temp, X						;load bar data
	rcall	BarShift					;shift out the upper 8 bits of bar data. (empty).
	
	inc		RefreshPointer				;increment pointer to load lower 8 bits from SRAM
	adiw	YL, 1
	adiw	XL, 1

	ld		temp, Y						;load lower 8 bits of segment data from SRAM.
	rcall 	AlphaShift

	ld		temp, X
	rcall	BarShift					;shift out lower 8 bits of bar data

	inc		refreshpointer				;increment pointer again. 

	sbi		portA, Strobe				;load the latches with new data
	cbi		portA, Blank				;unblank the VFD. 

	rjmp	endofscan					;scan is ended. exit interrupt. 







;shiftregister control loops below
;Each subroutine below controlls each of the 3 shift-registers. 

AlphaShift:
	ldi		count1, $08
Shiftloop1:
	cbi		portA, AlphaData			;clear the data pin
	lsl		temp						;shift the MSB into the carry. 
	brcc	s1							;if carry clear, then dont set data bit. otherwise set data bit.
	sbi		portA, AlphaData			;set data bit high. 
s1:
;	nop
	sbi		portA, AlphaClock			;set clock high. 
;	nop
	cbi		portA, AlphaClock			
	dec		count1						;decrement counter
	brne	shiftloop1					;branch back if all bits not sent.
	cbi		portA, AlphaData
	ret


BarShift:
	ldi		count1, $08
Shiftloop2:
	cbi		portA, BarData			;clear the data pin
	lsl		temp						;shift the MSB into the carry. 
	brcc	s2							;if carry clear, then dont set data bit. otherwise set data bit.
	sbi		portA, BarData			;set data bit high. 
s2:
;	nop
	sbi		portA, BarClock			;set clock high. 
;	nop
	cbi		portA, BarClock			
	dec		count1						;decrement counter
	brne	shiftloop2					;branch back if all bits not sent.
	cbi		portA, BarData
	ret

GridShift:
	ldi		count1, $08
Shiftloop3:
	cbi		portA, GridData			;clear the data pin
	lsl		temp						;shift the MSB into the carry. 
	brcc	s3							;if carry clear, then dont set data bit. otherwise set data bit.
	sbi		portA, GridData			;set data bit high. 
s3:
;	nop
	sbi		portA, GridClock			;set clock high. 
;	nop
	cbi		portA, GridClock			
	dec		count1						;decrement counter
	brne	shiftloop3					;branch back if all bits not sent.
	cbi		portA, GridData
	ret





endofScan:

	pop		temp
	sts		SREG, temp
	pop		YH
	pop		YL
	pop		XH
	pop		XL
	pop		ZH
	pop		ZL
	pop		temp
	sbi		portD, Filament
	reti


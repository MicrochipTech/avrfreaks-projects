;
; CAN2.asm
;
; Created: 07/01/2022 13:37:04
; Author : Manama
;


; PA1 = MOSI
; PA2 = MISO
; PA3 = SCK
; PA4 = SS
; PA5 = receive interrupt
.equ mask1 = 0xff					;all filters are enabled
.equ mask2 = 0x00					;all filters are disabled
.equ fclk = 10000000
.equ TXB0CTRL = 0x30                ;0b00000000  ; transmit request TXREQ bit3 is set after data is loaded in buffer & low priority
.equ TXB1CTRL = 0x40				;0b00000010   ; transmit request TXREQ bit3 is set after data is loaded in buffer & medium priority
.equ TXB2CTRL = 0x50				;0b00000011   ; transmit request TXREQ bit3 is set after data is loaded in buffer & high priority
.equ TXB0SIDH = 0x31				;TRANSMIT BUFFER 0 STANDARD IDENTIFIER REGISTER HIGH
.equ TXB1SIDH = 0x41				;TRANSMIT BUFFER 1 STANDARD IDENTIFIER REGISTER HIGH
.equ TXB2SIDH = 0x51				;TRANSMIT BUFFER 2 STANDARD IDENTIFIER REGISTER HIGH
.equ TXB0SIDL = 0x32				;0b00100000 , 1,TRANSMIT BUFFER 0 STANDARD IDENTIFIER REGISTER LOW
.equ TXB1SIDL = 0x42				;0b01000000 , 2,TRANSMIT BUFFER 1 STANDARD IDENTIFIER REGISTER LOW
.equ TXB2SIDL = 0x52				;0b01100000 , 3,TRANSMIT BUFFER 2 STANDARD IDENTIFIER REGISTER LOW
.equ TXB0DLC  = 0x35				;TRANSMIT BUFFER n DATA LENGTH CODE REGISTER,0b00000001  ; data length 1 byte
.equ TXB1DLC  = 0x45				;TRANSMIT BUFFER n DATA LENGTH CODE REGISTER,0b00000010  ; data length 2 byte
.equ TXB2DLC  = 0x55				;TRANSMIT BUFFER n DATA LENGTH CODE REGISTER,0b00001000  ; data length 8 byte
.equ TXB0D1   = 0x36
.equ TXB0D2	  = 0x37
.equ TXB0D3	  = 0x38
.equ TXB0D4   = 0x39
.equ TXB0D5	  = 0x3a
.equ TXB0D6   = 0x3b
.equ TXB0D7   = 0x3e
.equ TXB0D8	  = 0x3d
.equ TXB1D1	  = 0x46
.equ TXB1D2	  = 0x47
.equ TXB1D3   = 0x48
.equ TXB1D4   = 0x49
.equ TXB1D5   = 0x4a
.equ TXB1D6   = 0x4b
.equ TXB1D7   = 0x4c
.equ TXB1D8	  = 0x4d
.equ TXB2D1   = 0x56
.equ TXB2D2	  = 0x57
.equ TXB2D3	  = 0x58
.equ TXB2D4   = 0x59
.equ TXB2D5	  = 0x5a
.equ TXB2D6	  = 0x5b
.equ TXB2D7	  = 0x5c
.equ TXB2D8   = 0x5d
.equ CNF1	  = 0x2a
.equ CNF2     = 0x29
.equ CNF3     = 0x28
.equ CANINTE  = 0x2b
.equ CANINTF  = 0x2c
.equ CANCTRL  = 0x0F
.equ CANSTAT  = 0x0E
.equ EFLG     = 0x2d
.equ RRESET   = 0b11000000
.equ READ     = 0b00000011
;.equ READ_RX  = 0b10010xx0
.equ WRITE    = 0b00000010
;.equ LOAD_TX  = 0b01000abc
.equ RTS0      = 0b10000001
.equ RTS1      = 0b10000010
.equ RTS2      = 0b10000100
.equ READ_STATUS = 0b10100000
.equ RX_STATUS	= 0b10110000
.equ BIT_MODIFY = 0b00000101
.equ TXREQ = 0b00001000				; transmit request TXREQ bit3 is set
.equ RXB0CTRL = 0x60
.equ RXB1CTRL = 0x70
.equ RXB0SIDH = 0x61
.equ RXB1SIDH = 0x71
.equ RXB0SIDL = 0x62
.equ RXB1SIDL = 0x72
.equ RXB0DLC = 0x65
.equ RXB1DLC = 0x75
.equ RXB0D1 = 0x66
.equ RXB0D2 = 0x67
.equ RXB0D3 = 0x68
.equ RXB0D4 = 0x69
.equ RXB0D5 = 0x6a
.equ RXB0D6 = 0x6b
.equ RXB0D7 = 0x6c
.equ RXB0D8 = 0x6d
.equ RXB1D1 = 0x66
.equ RXB1D2 = 0x66
.equ RXB1D3 = 0x66
.equ RXB1D4 = 0x66
.equ RXB1D5 = 0x66
.equ RXB1D6 = 0x66
.equ RXB1D7 = 0x66
.equ RXB1D8 = 0x66
.equ RXF0SIDH = 0x00
.equ RXF0SIDL = 0x01
.equ RXF1SIDH = 0x04
.equ RXF1SIDL = 0x05
.equ RXF2SIDH = 0x08
.equ RXF2SIDL = 0x09
.equ RXF3SIDH = 0x10
.equ RXF3SIDL = 0x11
.equ RXF4SIDH = 0x14
.equ RXF4SIDL = 0x15
.equ RXF5SIDH = 0x18
.equ RXF5SIDL = 0x19
.equ RXM0SIDH = 0x20
.equ RXM0SIDL = 0x21
.equ RXM1SIDH = 0x24
.equ RXM1SIDL = 0x25
.equ filter1h = 0x00
.equ filter1l = 0x01
.equ filter2h = 0x00
.equ filter2l = 0x02
.equ filter3h = 0x00
.equ filter3l = 0x03
.equ filter4h = 0x00
.equ filter4l = 0x04
.equ filter5h = 0x00
.equ filter5l = 0x05
.equ filter6h = 0x00
.equ filter6l = 0x06
.equ filter7h = 0x00
.equ filter7l = 0x07
.equ filter8h = 0x00
.equ filter8l = 0x08
.equ blockfilter = 0xff
.equ BAUD = 9600
.equ fBAUD = ((64 * fclk) /(16 * BAUD)+0.5)



.macro millis					; macro for delay in ms
ldi YL,low(@0)
ldi YH,high(@0)
rcall delayYx1mS
.endm

.macro micros					; macro for delay in us
ldi temp,@0
rcall delayTx1uS
.endm


.def data = r17
.def address = r18
.def mask = r19
.def temp = r16

.dseg

data1: .byte 1
data2: .byte 1
data3: .byte 1
data4: .byte 1
data5: .byte 1
data6: .byte 1
data7: .byte 1
data8: .byte 1
RTRHI: .byte 1
RTRLO: .byte 1
statereg: .byte 1
display1: .byte 1
display2: .byte 1

.cseg
.org 0x00

reset:
		rcall MHZ10						; increase processor speed to 10MHZ
		rcall UART_SETUP				; setup UART engine for communication with terminal
		rcall spi_init					; setup SPI engine
		rcall ms10						; 10 milli delay
		rcall MCP_RESET					; reset MCP2515 to enter configuration mode
		rcall ms10						; delay of 10 milli second
		ldi address,CANCTRL				; prepare to  send address of CANCTRL regiter
		ldi data,0x80					; CAN cofiguration mode command
		rcall SPI_WRITE					; write to MCP2515
		rcall ms10						; 10 milli delay
		rcall SPEED125					; call subroutine to set 125kbps speed
		rcall ms10						; 10 milli delay
		ldi address,CANCTRL				; prepare to  send address of CANCTRL regiter
		ldi data,0x00					; CAN normal mode command
		rcall SPI_WRITE					; call subroutine to write to MCP2515
		rcall ms10						; 10 milli delay
loop1:
		ldi address,TXB0CTRL			; load transmit buffer0 control register address
		rcall SPI_READ					; call subroutine to read contents of above MCP register(if bit3 is 0 buffer empty)
		ldi address,TXB0SIDH			; load address register with address of transmit buffer0 standard id higher register
		ldi data,0x00					; load 0x00 as our id is 0x003 (high part is  0x00)
		rcall SPI_WRITE					; call subroutine to write to MCP2515
		ldi address,TXB0SIDL			; load address register with address of transmit buffer0 standard id lower register
		ldi data,0b01100000				; load 011 in the upper nibble which holds the lsb of the standard id
		rcall SPI_WRITE					; call subroutine to write to MCP2515
		ldi address,TXB0DLC				; load address register the MCP transmit buffer0 data length register address
		ldi data,0x08					; load the number of bytes we intend to send ( 0 to 8)
		rcall SPI_WRITE					; call subroutine to write to MCP2515
		ldi address,TXB0D1				; load address register the address of transmit buffer0 1st data byte register
		ldi data,0x00					; load to data register of attiny with ascii S
		rcall SPI_WRITE					; call subroutine to write to MCP2515
		ldi address,TXB0D2
		ldi data,'2'
		rcall SPI_WRITE
		ldi address,TXB0D3
		ldi data,'3'
		rcall SPI_WRITE
		ldi address,TXB0D4
		ldi data,'4'
		rcall SPI_WRITE
		ldi address,TXB0D5
		ldi data,'5'
		rcall SPI_WRITE
		ldi address,TXB0D6
		ldi data,'6'
		rcall SPI_WRITE
		ldi address,TXB0D7
		ldi data,'7'
		rcall SPI_WRITE
		ldi address,TXB0D8
		ldi data,'8'
		rcall SPI_WRITE
		rcall SEND0
		ldi address,TXB0CTRL
		rcall SPI_READ
;		ldi address,TXB0CTRL
;		ldi data,0b00000011
;		rcall SPI_WRITE
		rcall ms1000


		ldi address,TXB0SIDH			; load address register with address of transmit buffer0 standard id higher register
		ldi data,0x20					; load 0x20 as our id is 0x100 (0x100 is LSH 5 position to align with address bits)
		rcall SPI_WRITE					; call subroutine to write to MCP2515
		ldi address,TXB0SIDL			; load address register with address of transmit buffer0 standard id lower register
		ldi data,0b00000000				; load 011 in the upper nibble which holds the lsb of the standard id
		rcall SPI_WRITE					; call subroutine to write to MCP2515
		ldi address,TXB0DLC				; load address register the MCP transmit buffer0 data length register address
		ldi data,0x08	
		ldi address,TXB0D1				; load address register the address of transmit buffer0 1st data byte register
		ldi data,0x01					; load to data register of attiny with ascii S
		rcall SPI_WRITE					; call subroutine to write to MCP2515
		ldi address,TXB0D2
		ldi data,'b'
		rcall SPI_WRITE
		ldi address,TXB0D3
		ldi data,'c'
		rcall SPI_WRITE
		ldi address,TXB0D4
		ldi data,'d'
		rcall SPI_WRITE
		ldi address,TXB0D5
		ldi data,'e'
		rcall SPI_WRITE
		ldi address,TXB0D6
		ldi data,'f'
		rcall SPI_WRITE
		ldi address,TXB0D7
		ldi data,'g'
		rcall SPI_WRITE
		ldi address,TXB0D8
		ldi data,'h'
		rcall SPI_WRITE
		rcall SEND0
		ldi address,TXB0CTRL
		rcall SPI_READ
		rcall ms1000
		rjmp loop1



		
MHZ10:
		ldi r16,0Xd8
		out CPU_CCP,r16
		ldi r16,0x01			; clk prescaler of 2, 20Mhz/2 = 10Mhz
		STS CLKCTRL_MCLKCTRLB,R16
		ret


spi_init:
		ldi r16,0b00011010		; PA4,PA3,PA1 are outputs- SS,SCK & MOSI respectievly
		out VPORTA_DIR,r16
		sbi VPORTA_OUT,4		; SS HI ,set BIT4, PA4 =SS
		ldi r16,0b00100001		; 7=reserved,dord =0=msb,master=1,CLK2X=0,3= reserved,2:1= 0x00 clk/4,enabe=1
		sts SPI0_CTRLA,r16
		ldi r16,0b00000100		; buffer= disable,SSD =1 ,mode =0
		sts SPI0_CTRLB,r16
		ret


SS_LO:	cbi VPORTA_OUT,4		; SS low ,clear bit4 ,PA4 =SS
		ret

SS_HI:	sbi VPORTA_OUT,4		; SS HI ,set BIT4, PA4 =SS
		ret

SPI_TX:	        
		sts SPI0_DATA,address	; write address to SPI data register
		rcall checkif
		sts SPI0_DATA,data		; write data to SPI data register
		rcall checkif
		ret 
checkif:lds r16,SPI0_INTFLAGS		; copy SPI0 flag register
		andi r16,0b10000000		; and with 0x80 to see if bit 7 is set after all bits are transmitted
		breq checkif			; if IF flag not set sit in a tight loop
		ret
 
SPI_RX:
		sts SPI0_DATA,address	; write address to SPI data register
		rcall checkif
		ldi data,0xff			; junk/stuffing byte loaded into data willl be transmitted to receive on miso
		sts SPI0_DATA,data		; write data to SPI data register
		rcall checkif
		mov r1,data				; received data copied to r5
		ret 

SPI_WRITE:
		rcall SS_LO
		push address
		ldi address,WRITE
		sts SPI0_DATA,address	; 
		rcall checkif
		pop address
		rcall SPI_TX
		rcall SS_HI
		ret 
SPI_READ:
		rcall SS_LO
		push address
		ldi address,READ
		sts SPI0_DATA,address	; 
		rcall checkif
		pop address
		rcall SPI_RX
		rcall SS_HI
		ret 
MCP_RESET:
		rcall SS_LO
		ldi address,RRESET
		sts SPI0_DATA,address
		rcall checkif
		rcall SS_HI
		ret
SPI_BIT_MODIFY:
		rcall SS_LO
		push address
		ldi address,BIT_MODIFY
		sts SPI0_DATA,address
		rcall checkif
		pop address 
		sts SPI0_DATA,address	; write address to SPI data register
		rcall checkif
		sts SPI0_DATA,mask		; write data to SPI data register
		rcall checkif
		sts SPI0_DATA,data		; write data to SPI data register
		rcall checkif
		rcall SS_HI
		ret

SPEED125:
		ldi address, CNF1
		ldi data,0x41		; SJW=1 =00 ,BRP = 4 (3+1) = 000011  ,,,0b00000011
		rcall SPI_WRITE
		ldi address, CNF2
		ldi data,0xf1	 	; BLT =1 (how PS2 is calculated),SAM=0, PS1 = 3=010 , PRSEG=1=000  ===0b10010000
		rcall SPI_WRITE
		ldi address,CNF3
		ldi data,0x85		; WAKFIL=0,PS2=3 =010 (ps2+1)   ====0b00000010
		rcall SPI_WRITE
		ret
SEND0:
		rcall SS_LO
		ldi address,RTS0
		sts SPI0_DATA,address	; 
		rcall checkif
		rcall SS_HI
		ret 

SEND1:
		rcall SS_LO
		ldi address,RTS1
		sts SPI0_DATA,address	; 
		rcall checkif
		rcall SS_HI
		ret 

SEND2:
		rcall SS_LO
		ldi address,RTS2
		sts SPI0_DATA,address	; 
		rcall checkif
		rcall SS_HI
		ret 



TXch:	ldi address,TXB0CTRL
		rcall SPI_READ
		mov r16,r1
		andi r16,8
		brne TXch
		ret




	; ============================== Time Delay Subroutines =====================
; Name:     delayYx1mS
; Purpose:  provide a delay of (YH:YL) x 1 mS
; Entry:    (YH:YL) = delay data
; Exit:     no parameters
; Notes:    the 16-bit register provides for a delay of up to 65.535 Seconds
;           requires delay1mS

delayYx1mS:
    rcall    delay1mS                        ; delay for 1 mS
    sbiw    YH:YL, 1                        ; update the the delay counter
    brne    delayYx1mS                      ; counter is not zero

; arrive here when delay counter is zero (total delay period is finished)
    ret
; ---------------------------------------------------------------------------
; Name:     delayTx1mS
; Purpose:  provide a delay of (temp) x 1 mS
; Entry:    (temp) = delay data
; Exit:     no parameters
; Notes:    the 8-bit register provides for a delay of up to 255 mS
;           requires delay1mS

delayTx1mS:
    rcall    delay1mS                        ; delay for 1 mS
    dec     temp                            ; update the delay counter
    brne    delayTx1mS                      ; counter is not zero

; arrive here when delay counter is zero (total delay period is finished)
    ret

; ---------------------------------------------------------------------------
; Name:     delay1mS
; Purpose:  provide a delay of 1 mS
; Entry:    no parameters
; Exit:     no parameters
; Notes:    chews up fclk/1000 clock cycles (including the 'call')

delay1mS:
    push    YL                              ; [2] preserve registers
    push    YH                              ; [2]
    ldi     YL, low(((fclk/1000)-18)/4)     ; [1] delay counter              (((fclk/1000)-18)/4)
    ldi     YH, high(((fclk/1000)-18)/4)    ; [1]                            (((fclk/1000)-18)/4)

delay1mS_01:
    sbiw    YH:YL, 1                        ; [2] update the the delay counter
    brne    delay1mS_01                     ; [2] delay counter is not zero

; arrive here when delay counter is zero
    pop     YH                              ; [2] restore registers
    pop     YL                              ; [2]
    ret                                     ; [4]

; ---------------------------------------------------------------------------
; Name:     delayTx1uS
; Purpose:  provide a delay of (temp) x 1 uS with a 16 MHz clock frequency
; Entry:    (temp) = delay data
; Exit:     no parameters
; Notes:    the 8-bit register provides for a delay of up to 255 uS
;           requires delay1uS

delayTx1uS:
    rcall    delay10uS                        ; delay for 1 uS
    dec     temp                            ; decrement the delay counter
    brne    delayTx1uS                      ; counter is not zero

; arrive here when delay counter is zero (total delay period is finished)
    ret

; ---------------------------------------------------------------------------
; Name:     delay10uS
; Purpose:  provide a delay of 1 uS with a 16 MHz clock frequency ;MODIFIED TO PROVIDE 10us with 1200000cs chip by Sajeev
; Entry:    no parameters
; Exit:     no parameters
; Notes:    add another push/pop for 20 MHz clock frequency

delay10uS:
    ;push    temp                            ; [2] these instructions do nothing except consume clock cycles
    ;pop     temp                            ; [2]
    ;push    temp                            ; [2]
    ;pop     temp                            ; [2]
    ;ret                                     ; [4]
     nop
     nop
     nop
     ret

; ============================== End of Time Delay Subroutines ==============

ms1:
		millis 1
		ret
ms10:
		millis 10
		ret
ms50:
		millis 50
		ret
ms100:
		millis 100
		ret
ms500:
		millis 500
		ret
ms1000:
		millis 1000
		ret

UART_SETUP:
	lds r16,PORTB_DIR	
	ori r16,(1<<2)
	sts PORTB_DIR,r16			;set portB PB2 direction as output
	lds r16,PORTB_OUT
	ori r16,(1<<2)
	sts PORTB_OUT,r16			;set portB PB2 as 1 or +ve
	ldi r16,low(fBAUD)			;load low value of fBAUD as calculated in the formula provided above
	ldi r17,high(fBAUD)			;load high value of fBAUD as calculated in the formula provided above
	sts USART0_BAUD,r16			;store low fBAUD in BAUD register
	sts USART0_BAUD + 1,r17		;store low fBAUD in BAUD register
	ldi r16,USART_TXEN_bm	    ;0b01000000 ,(1<<6) ,bitmask value to enable USART transmit 
	sts USART0_CTRLB,r16		;store TXEN in USART_CTRLB register
;	rjmp TXUSART				;jump to TXUSART address (main routine)
	ret

sendbyte:
	lds r16,USART0_STATUS		;copy USART status register to r16
	andi r16,USART_DREIF_bm     ;(0b00100000) AND with DATA REGISTER EMPTY FLAG bitmask (position5) to check flag status 0= not empty 1= empty 
	sbrs r16,5					;skip next instruction if bit 5 is 1 (means flag set for data transmit buffer ready to receive new data )
	rjmp sendbyte				;if DREIF = 0 ,bit 5 in r16 is 0 then loop back to sendbyte until DREIF = 1
	mov r16,r20					;copy data to be transmitted from r20 to r16
	sts USART0_TXDATAL,r16		;store r16 in TXDATAL transmit data low register 
	ret

TXstring1:
	ldi r21,0x70
	ldi address,0x00
	
loop:
	rcall SPI_READ
	mov r20,r1
	rcall sendbyte
	inc address
	dec r21
	breq exit					;if null/0x00 the end of string has reached
	rjmp loop					;jump to loop back through the string until a null is encountered
	rcall ms500
exit:ret


TXstring:
	ldi ZL,low(2*string)
	ldi ZH,high(2*string)	
loop2:
	lpm r20,Z+
	rcall sendbyte
	cpi r20,0x00
	breq exit1					;if null/0x00 the end of string has reached
	rjmp loop2					;jump to loop back through the string until a null is encountered
exit1:ret


TXUSART:
	rcall TXstring				;call routine to transmit a null terminated string
	rcall TXstring1
	ret
string: .db "Hello, world", '\n', '\r' , 0


hex_to_ascii:
	mov temp,r21
	andi temp,0xf0
	swap temp
	cpi temp,10
	brsh letter1
	ori temp,0x30
	sts display1,temp
	rjmp low_nibble
letter1:
	ori temp,0x37
	sts display1,temp
low_nibble:
	mov temp,r21
	andi temp,0x0f
	cpi temp,10
	brsh letter2
	ori temp,0x30
	sts display2,temp
	rjmp bye
letter2:
	ori temp,0x37
	sts display2,temp
bye:
	ret

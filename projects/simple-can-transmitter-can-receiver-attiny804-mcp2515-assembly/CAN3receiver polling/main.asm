;
; CAN3.asm
;
; Created: 12/01/2022 20:06:42
; Author : Manama
; simple CAN receiver for standard id using MCP2151 and attiny804
; tested with message sent from arduino uno with id 0x003 for LED on and 0x100 for LED off , CoryJFowler library used to send message to attiny 804
; MCP in filtered mode 00 receives only ids 0x100,0x003,0x001,0x002, 0x101,0x102,0x103
; uses bus speed of 125kbps with MCP2151 chinese board having 8MHZ crystal
; no interrupts used only simple polling of RXB0 receive buffer

; PA1 = MOSI
; PA2 = MISO
; PA3 = SCK
; PA4 = SS
; PA5 = receive interrupt
; PB3 = LED
; PB2 = UART OUT
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
.equ RXB1D1 = 0x76
.equ RXB1D2 = 0x77
.equ RXB1D3 = 0x78
.equ RXB1D4 = 0x79
.equ RXB1D5 = 0x7a
.equ RXB1D6 = 0x7b
.equ RXB1D7 = 0x7c
.equ RXB1D8 = 0x7d
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
.equ TEC = 0x1c
.equ REC = 0x1d


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
.def display1 = r24
.def display2 = r25

.dseg
RIDH0: .byte 1
RIDL0: .byte 1
DLC0: .byte 1
data1: .byte 1
data2: .byte 1
data3: .byte 1
data4: .byte 1
data5: .byte 1
data6: .byte 1
data7: .byte 1
data8: .byte 1
TECounter: .byte 1
RECounter: .byte 1
EFLAG: .byte 1
statereg: .byte 1
RTRHI: .byte 1
RTRLO: .byte 1
DLC1: .byte 1
RIDH1: .byte 1
RIDL1: .byte 1


U1: .byte 1
U2: .byte 1
U3: .byte 1
U4: .byte 1
U5: .byte 1
U6: .byte 1
U7: .byte 1
U8: .byte 1
U9: .byte 1
U10: .byte 1
U11: .byte 1
U12: .byte 1
U13: .byte 1
U14: .byte 1
U15: .byte 1
U16: .byte 1
U17: .byte 1
U18: .byte 1
U19: .byte 1
U20: .byte 1
U21: .byte 1
U22: .byte 1
U23: .byte 1
U24: .byte 1
U25: .byte 1
U26: .byte 1
U27: .byte 1
U28: .byte 1
U29: .byte 1
U30: .byte 1
U31: .byte 1
U32: .byte 1
U33: .byte 1
U34: .byte 1
.cseg
.org 0x00

reset:
		rcall MHZ10						; increase processor speed to 10MHZ
		rcall UART_SETUP				; setup UART engine for communication with terminal
		rcall spi_init					; setup SPI engine
		lds temp,PORTB_DIR				; set PB3 as output for LED
		ori temp,0x08
		sts PORTB_DIR,temp
		lds temp,PORTB_OUT				; set PB3 to source volt
		ori temp,0x08
		sts PORTB_OUT,temp
		rcall LED_ON
		rcall ms1000
		rcall LED_OFF
		rcall ms10						; 10 milli delay
		rcall MCP_RESET					; reset MCP2515 to enter configuration mode
		rcall ms10						; delay of 10 milli second
		ldi address,CANCTRL				; prepare to  send address of CANCTRL regiter
		ldi data,0x80					; CAN cofiguration mode command
		rcall SPI_WRITE					; write to MCP2515
		rcall ms10						; 10 milli delay
		rcall KBPS125					; call subroutine to set 125kbps speed
		rcall ms10						; 10 milli delay
		rcall SET_RECEIVE				; set up receive mode and clear buffers/data registers
		rcall SET_MASK0					; set mask for buffer0
		rcall SET_MASK1					; set mask for buffer1 (each bit 1= enable mask , 0 = disable mask)
		rcall SET_FILTER0				; set top priority ID filter for RX buffer0
		rcall SET_FILTER1				; set next priority ID filter for RX buffer0
		rcall SET_FILTER2				; set id filter for RX buffer1
		rcall SET_FILTER3				; set id filter for RX buffer1
		rcall SET_FILTER4				; set id filter for RX buffer1
		rcall SET_FILTER5				; set id filter for RX buffer1
		ldi address,CANCTRL				; prepare to  send address of CANCTRL regiter
		ldi data,0x00					; CAN normal mode command
		rcall SPI_WRITE					; call subroutine to write to MCP2515
		rcall ms10						; 10 milli delay
loop1:
		lds temp,data1					; data1 in sram holds the 1st byte of received message, if 0x01 = led on , if 0x00 = led off
		cpi temp,1
		breq LED_ON1
		cpi temp,0
		breq LED_OFF1
LED_ON1:
		rcall LED_ON					; call subroutine to switch on LED on PB3
		rcall CAN_RECEIVE				; check for new message in RXBUF0
		rcall convertaddress			; convert received id by shifting right 5 places 0x2000 bec0mes 0x100 and 0x0060 becomes 0x003
		rcall POINTER					; this OR s the value stored in SRAM registers with 0x30 and stores in UART display buffer
		rcall TXUSART					; transmit USART messages toterminal for debugging
		rcall ms1000					; delay 1 second
		rjmp loop1						; loop again
LED_OFF1:
		rcall LED_OFF					; call subroutine to switch on LED on PB3
		rcall CAN_RECEIVE
		rcall convertaddress
		rcall POINTER
		rcall TXUSART
		rcall ms1000
		rjmp loop1

MHZ10:
		ldi r16,0Xd8			; reconfigure internal oscillator of MCU to 10MHZ
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
		lds r1,SPI0_DATA		; received data copied to r5
		ret 

SPI_WRITE:
		rcall SS_LO
		push address
		ldi address,WRITE		; write command for MCP2515, data will be written to internal registers through SPI
		sts SPI0_DATA,address	; 
		rcall checkif
		pop address
		rcall SPI_TX			; subroutine stores data in address and data registers to MCP registers
		rcall SS_HI
		ret 
SPI_READ:
		rcall SS_LO
		push address
		ldi address,READ		; READ command for MCP251, data will be taken out from MCP to external chip as needed
		sts SPI0_DATA,address	; 
		rcall checkif
		pop address
		rcall SPI_RX			; after READ command executed the MCP clocks out data from requested internal registers , data in r1
		rcall SS_HI
		ret 
MCP_RESET:
		rcall SS_LO
		ldi address,RRESET		; puts MCP 2515 in reset mode for programming internal registers, for entering configuration first reset chip
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

KBPS125:
		ldi address, CNF1	; setting CNF1,CNF2,CNF3 will decide the CAN bit timing and bus speed , we set here to 125kbps
		ldi data,0x81		; SJW=3 =10 ,BRP = 1 (1+1) = 000001  ,,,0b10000001
		rcall SPI_WRITE
		ldi address, CNF2
		ldi data,0xf1	 	; BLT =1 (how PS2 is calculated),SAM=1, PS1 = 6=110 (6+1) , PRSEG=2=001  ===0b11110001
		rcall SPI_WRITE
		ldi address,CNF3
		ldi data,0x85		; SOF = 1,WAKFIL=0,PS2=5 =101 (ps2+1)=6   ====0b10000101
		rcall SPI_WRITE
		ret


SET_RECEIVE:
		ldi address,CANINTE				; disable interrupts
		ldi data,0x00
		rcall SPI_WRITE
		ldi address,RXB0CTRL
		ldi data,0x00					;0x00 accept only filtered msg, 0x60 will accept all msgs,0x64 will also allow roll over
		rcall SPI_WRITE
		ldi address,RXB1CTRL
		ldi data,0x00					;0x00 accept only filtered msg, 0x60 will accept all msgs
		rcall SPI_WRITE
		ldi address,RXB0D1
		ldi data,0x00
		rcall SPI_WRITE
		inc address
		clr data
		rcall SPI_WRITE
		inc address
		clr data
		rcall SPI_WRITE
		inc address
		clr data
		rcall SPI_WRITE
		inc address
		clr data
		rcall SPI_WRITE
		inc address
		clr data
		rcall SPI_WRITE
		inc address
		clr data
		rcall SPI_WRITE
		inc address
		clr data
		rcall SPI_WRITE

		ldi address,RXB1D1
		ldi data,0x00
		rcall SPI_WRITE
		inc address
		clr data
		rcall SPI_WRITE
		inc address
		clr data
		rcall SPI_WRITE
		inc address
		clr data
		rcall SPI_WRITE
		inc address
		clr data
		rcall SPI_WRITE
		inc address
		clr data
		rcall SPI_WRITE
		inc address
		clr data
		rcall SPI_WRITE
		inc address
		clr data
		rcall SPI_WRITE
		ret
clearflag:
		ldi address,CANINTF
		ldi data,0x00					; clear CAN interrupt flags
		rcall SPI_WRITE
		ret

SET_MASK0:
		ldi address,RXM0SIDH
		ldi data,0xdf					; mask 0xdf9f will allow (id0x100 << 5 = 0x2000),(id 0x003 << 5 = 0x0060) ,(id0x001 << 5 = 0x0020) & (id0x002 << 5 = 0x0040) to pass
		rcall SPI_WRITE
		inc address
		ldi data,0x9f					; we are using id 0x003 and 0x100 to receive blink LED control status. id 0x003 = on signal , id 0x100 = off signal
		rcall SPI_WRITE
		ret
SET_MASK1:
		ldi address,RXM1SIDH
		ldi data,mask1
		rcall SPI_WRITE
		inc address
		ldi data,mask1
		rcall SPI_WRITE
		ret

SET_FILTER0:
		ldi address,RXF0SIDH
		ldi data,0x00               ;0x003 id when shifted lhs 5 position to suit the registers will become 0x0060, 0x00 in hi register
		rcall SPI_WRITE
		inc address
		ldi data,0x60               ;0x60 of 0x0060 in low register (FILTER0 associated with RXB0) message will be in buffer0
		rcall SPI_WRITE
		ret
SET_FILTER1:
		ldi address,RXF1SIDH
		ldi data,0x20				;0x100 id when shifted lhs 5 position to suit the registers will become 0x2000, 0x20 in hi register
		rcall SPI_WRITE
		inc address
		ldi data,0x00				;0x00 of 0x2000 in low register (FILTER1 associated with RXB0)message will be in buffer0
		rcall SPI_WRITE
		ret
SET_FILTER2:
		ldi address,RXF2SIDH
		ldi data,blockfilter
		rcall SPI_WRITE
		inc address
		ldi data,blockfilter
		rcall SPI_WRITE
		ret
SET_FILTER3:
		ldi address,RXF3SIDH
		ldi data,blockfilter
		rcall SPI_WRITE
		inc address
		ldi data,blockfilter
		rcall SPI_WRITE
		ret
SET_FILTER4:
		ldi address,RXF4SIDH
		ldi data,blockfilter
		rcall SPI_WRITE
		inc address
		ldi data,blockfilter
		rcall SPI_WRITE
		ret
SET_FILTER5:
		ldi address,RXF5SIDH
		ldi data,blockfilter
		rcall SPI_WRITE
		inc address
		ldi data,blockfilter
		rcall SPI_WRITE
		ret

CAN_RECEIVE:
		ldi address,RXB0SIDH
		rcall SPI_READ
		sts RIDH0,r1
		ldi address,RXB0SIDL
		rcall SPI_READ
		sts RIDL0,r1
		ldi address,RXB0DLC
		rcall SPI_READ
		mov temp,r1
		sts DLC0,temp
		ldi address,RXB0D1
		rcall SPI_READ
		mov temp,r1
		sts data1,temp
		inc address
		rcall SPI_READ
		mov temp,r1
		sts data2,temp
		inc address
		rcall SPI_READ
		mov temp,r1
		sts data3,temp
		inc address
		rcall SPI_READ
		mov temp,r1
		sts data4,temp
		inc address
		rcall SPI_READ
		mov temp,r1
		sts data5,temp
		inc address
		rcall SPI_READ
		mov temp,r1
		sts data6,temp
		inc address
		rcall SPI_READ
		mov temp,r1
		sts data7,temp
		inc address
		rcall SPI_READ
		mov temp,r1
		sts data8,temp

		ldi address,TEC
		rcall SPI_READ
		ldi address,REC
		rcall SPI_READ
		ldi address,EFLG
		rcall SPI_READ
		ldi address,EFLG
		ldi data,0x00
		rcall SPI_WRITE
		rcall clearflag
		ret
/*
RTR_PROC:
		ldi address,RXB0SIDH
		rcall SPI_READ
		sts RTRHI,r1
		ldi address,RXB0SIDL
		rcall SPI_READ
		sts RTRLO,r1
		lds temp,statereg
		ori temp,0x80					; bit 7 set in statereg is a pending RTR 
		sts statereg,temp
		ldi address,CANINTF
		ldi data,0x00
		rcall SPI_WRITE
		ret
*/

LED_ON:
		lds temp,PORTB_OUTCLR
		ori temp,0b00001000
		sts PORTB_OUTCLR,temp
		ret


LED_OFF:
		lds temp,PORTB_OUTSET
		ori temp,0b00001000
		sts PORTB_OUTSET,temp
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
	ldi r21,33
	ldi YL,low(U1)
	ldi YH,high(U1)
	
loop:
	ld r20, Y+
	rcall sendbyte
	dec r21
	breq exit					;if null/0x00 the end of string has reached
	rjmp loop					;jump to loop back through the string until a null is encountered
	rcall ms10
exit:ret


TXstring:
	ldi ZL,low(2*string)
	ldi ZH,high(2*string)	
loop2:
	lpm r20,Z+
	rcall sendbyte
	cpi r20,0x00
	breq exit2					;if null/0x00 the end of string has reached
	rjmp loop2					;jump to loop back through the string until a null is encountered
exit2:ret


TXUSART:
	rcall TXstring				;call routine to transmit a null terminated string
	rcall TXstring1
	ret
string: .db "Mid dlc DATA FEILD 8", '\n', '\r' , 0

POINTER:
	ldi XL,low(RIDH0)
	ldi XH,high(RIDH0)
;	ldi YL,low(U1)
;	ldi YH,high(U1)
	ldi r22,11
loop3:
	rcall hex_to_ascii
	sts U1,display1
	sts U2,display2
	rcall hex_to_ascii
	sts U3,display1
	sts U4,display2		; id

	ldi temp,0x20			; space
	sts U5,temp

	rcall hex_to_ascii		; DLC
	sts U6,display1
	sts U7,display2

	ldi temp,0x20			; space
	sts U8,temp

	rcall hex_to_ascii		; data1
	sts U9,display1
	sts U10,display2

	ldi temp,0x20			; space
	sts U11,temp

	rcall hex_to_ascii		; data2
	sts U12,display1
	sts U13,display2

	ldi temp,0x20			; space
	sts U14,temp

	rcall hex_to_ascii		; data3
	sts U15,display1
	sts U16,display2

	ldi temp,0x20			; space
	sts U17,temp

	rcall hex_to_ascii		; data4
	sts U18,display1
	sts U19,display2

	ldi temp,0x20			; space
	sts U20,temp

	rcall hex_to_ascii		; data5
	sts U21,display1
	sts U22,display2

	ldi temp,0x20			; space
	sts U23,temp

	rcall hex_to_ascii		; data6
	sts U24,display1
	sts U25,display2

	ldi temp,0x20			; space
	sts U26,temp

	rcall hex_to_ascii		; data7
	sts U27,display1
	sts U28,display2

	ldi temp,0x20			; space
	sts U29,temp

	rcall hex_to_ascii		; data8
	sts U30,display1
	sts U31,display2
	
	ldi temp, '\n'
	sts U32,temp
	ldi temp,'\r'
	sts U33,temp
	ret


hex_to_ascii:
	ld r21,X+
	mov temp,r21			; mov data in r21 to temp
	andi temp,0xf0			; and data in temp with immediate value 0xF0 to isolate upper nibble
	swap temp				; swap nibbles , upper bcomes lower nibble
	cpi temp,10				; compare data in temp to number 10, 0-9 is numbers and 10-16 is alphabets of HEX
	brsh letter1			; branch to label letter1 if compare is same or higher indicating lettrs/alphabet
	ori temp,0x30			; if less than 10 OR with 0x30 (decimal 0 in ASCII) to convert to ASCII number between 0-9
	mov display1,temp				; store in display register /buffer that holds higher position/nibble
	rjmp low_nibble			; jump to low_nibble label to do conversion on the lower nibbble of the data in r21
letter1:
	subi temp,-(0x37)		; reach here if above comparison with 10 is equal or greater. add 0x37 with data derives the correct alpabet in ASCII
	mov display1,temp				; store in high nibble display buffer
low_nibble:
	mov temp,r21			; mov data in r21 to temp
	andi temp,0x0f			; and data in temp with immediate value 0x0F to isolate lower nibble
	cpi temp,10				; compare data in temp to number 10, 0-9 is numbers and 10-16 is alphabets of HEX
	brsh letter2			; branch to label letter1 if compare is same or higher indicating lettrs/alphabet
	ori temp,0x30			; if less than 10 OR with 0x30 (decimal 0 in ASCII) to convert to ASCII number between 0-9
	mov display2,temp				; store in display register /buffer that holds lower position/nibble
	rjmp bye				; jump to exit
letter2:
	subi temp,-(0x37)		; reach here if above comparison with 10 is equal or greater. add 0x37 with data derives the correct alpabet in ASCII;
	mov display2,temp				; store in low nibble display buffer
bye:
	ret



convertaddress:
	lds temp,RIDH0
	lds r17,RIDL0
	lsr temp
	ror r17
	lsr temp
	ror r17
	lsr temp
	ror r17
	lsr temp
	ror r17
	lsr temp
	ror r17
	sts RIDH0,temp
	sts RIDL0,r17
	ret	



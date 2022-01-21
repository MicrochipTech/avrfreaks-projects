;
; CAN4.asm
;
; Created: 15/01/2022 18:05:12
; Author : Manama
;


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
MCPINTFLAG: .byte 1
RTRHI: .byte 1
RTRLO: .byte 1
DLC1: .byte 1
RIDH1: .byte 1
RIDL1: .byte 1


.cseg
.org 0x00
rjmp reset

.org 0x03
PORTA_ISR:
		push address
		push data
		push temp
		ldi address,CANINTF        ; looad address with MCP interrupt flag register
		rcall SPI_READ			   ; read data from flag register
		sts MCPINTFLAG,r1			; store data in SRAM
		rcall CAN_RECEIVE			; perform read of RX0ID,DLC0, 8 data registers and save it to sram and clear MCP int flags
		lds temp,PORTA_INTFLAGS		; copy ATTINY804 interrupt flag register
		ori temp ,0b00100000		; write 1 to bit 5 to clear PA5 interrupt flag
		sts PORTA_INTFLAGS,temp		; store the value in portA int flags register
		pop temp
		pop data
		pop address
		reti
		 

reset:
		rcall MHZ10						; increase processor speed to 10MHZ
		rcall spi_init					; setup SPI engine
		lds temp,PORTB_DIR				; set PB3 as output for LED
		ori temp,0x08
		sts PORTB_DIR,temp
		lds temp,PORTB_OUT				; set PB3 to source volt
		ori temp,0x08
		sts PORTB_OUT,temp
		rcall LED_ON					; test for LED on
		rcall ms1000					; delay 1 second
		rcall LED_OFF					; test LED off
		ldi temp,0x05					; pullup disabled, sense low
		sts PORTA_PIN5CTRL,temp			; save above value in portA pin5 control register
		rcall ms10						; 10 milli delay
		rcall MCP_RESET					; reset MCP2515 to enter configuration mode
		rcall ms10						; delay of 10 milli second
		rcall CONFIG_MODE				; cal to subroutine that enables MCP2515 configuration mode
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
		rcall ENABLE_INTERRUPTS			; call routine to set RX0 interrupt
		rcall NORMAL_MODE				; call proc to enable MCP2515 normal mode
		rcall ms10						; 10 milli delay
		sei								; enable interrupts globally
		
loop1:
		
		lds temp,data1					; data1 in sram holds the 1st byte of received message, if 0x01 = led on , if 0x00 = led off
		sbrc temp,0
		rcall LED_ON
		sbrs temp,0
		rcall LED_OFF
		rjmp loop1						; loop again


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

NORMAL_MODE:
		ldi address,CANCTRL				; prepare to  send address of CANCTRL regiter
		ldi data,0x00					; CAN normal mode command
		rcall SPI_WRITE					; call subroutine to write to MCP2515
		ret

CONFIG_MODE:
		ldi address,CANCTRL				; prepare to  send address of CANCTRL regiter
		ldi data,0x80					; CAN cofiguration mode command
		rcall SPI_WRITE					; write to MCP2515
		ret

ENABLE_INTERRUPTS:
		ldi address,CANINTE				; enable rx0 interrupts
		ldi data,0x01					; RX0 interrupt is bit 0 so 0x01 to enable it
		rcall SPI_WRITE
		ret
DISABLE_INTERRUPTS:
		ldi address,CANINTE				; disable interrupts
		ldi data,0x00
		rcall SPI_WRITE
		ret

SET_RECEIVE:
		ldi address,RXB0CTRL
		ldi data,0x00					;0x00 accept only filtered msg, 0x60 will accept all msgs,0x64 will also allow roll over
		rcall SPI_WRITE
		ldi address,RXB1CTRL
		ldi data,0x00					;0x00 accept only filtered msg, 0x60 will accept all msgs
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
		sts DLC0,r1
		ldi address,RXB0D1
		rcall SPI_READ
		sts data1,r1
		inc address
		rcall SPI_READ
		sts data2,r1
		inc address
		rcall SPI_READ
		sts data3,r1
		inc address
		rcall SPI_READ
		sts data4,r1
		inc address
		rcall SPI_READ
		sts data5,r1
		inc address
		rcall SPI_READ
		sts data6,r1
		inc address
		rcall SPI_READ
		sts data7,r1
		inc address
		rcall SPI_READ
		sts data8,r1
		ldi address,TEC
		rcall SPI_READ
		sts TECounter,r1
		ldi address,REC
		rcall SPI_READ
		sts RECounter,r1
		ldi address,EFLG
		rcall SPI_READ
		sts EFLAG,r1
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





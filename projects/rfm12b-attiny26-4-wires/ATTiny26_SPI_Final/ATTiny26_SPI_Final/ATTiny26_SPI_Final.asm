;*******************************************************************************
; File: ATTiny26_SPI_Final.asm
;
; Description:
; SPI master demonstration, to get data from and send data to a RFM12B module
; The setup requires everything DOUBLE, so TWO ATTiny26's and TWO RFM12B modules
; 
; 
; 
; 
; ATTiny26 @ 8MHz Internal RC.oscillator
; 
; Hookup: PINB.0 --------- DI (in) to pin 12 on RFM12B (out) (MCU pin 1)
;         PINB.1 --------- DO (out) to pin 1 on FRM12B (in) (MCU pin 2)
;         PINB.2 --------- SCK (out) to pin 13 on RFM12B (in) (MCU pin 3)
;         PINB.3 --------- SS (out) to pin 14 on RFM12B (in) (MCU pin 4)
;         PINB.4 --------- RED led (out) (MCU pin 7) with series resistor
;         PINB,5 --------- GREEN led (out) (MCU pin 8) with series resistor
; 
; Note: The RFM12B module expects 16-bits data
; 
; Fuses: High 0xF7
;        Low  0xE4
; 
; Revision:
;   1.0 Created final version            May, 2011 (c) JCvR, All nights reserved
;*******************************************************************************

.nolist
.include "tn26def.inc"
.list

.dseg
m_TXbuffer:
.byte $10
m_RXbuffer:
.byte $0A
m_databuffer:
.byte $08
m_frameTX:
.byte $01
m_frameRX:
.byte $01
m_frameCur:
.byte $01
m_bufsize:
.byte $01

.eseg											; Variables in EEPROM

.def	DelayReg	= r15						; Delay register
.def	TEMP1		= r16						; General temporary register 1
.def	TEMP2		= r17						; General temporary register 2
.def	TEMP3		= r18						; General temporary register 3
.def	m_flags		= r19						; bit0 = Checksum failed
												; bit1 = 
												; bit2 = 
												; bit3 = 
												; bit4 = 
												; bit5 = 
												; bit6 = 
												; bit7 = 

.def	ChkSum		= r20						; Checksum register

.cseg
		rjmp		init						; Start of program

;Interrupt vector table
.org	INT0addr								; External Interrupt 0
		reti									; 
.org	PCI0addr								; Pin Change Interrupt
		reti									; 
.org	OC1Aaddr								; Timer/Counter1 Compare Match 1A
		reti									; 
.org	OC1Baddr								; Timer/Counter1 Compare Match 1B
		reti									; 
.org	OVF1addr								; Timer/Counter1 Overflow
		reti									; 
.org	OVF0addr								; Timer/Counter0 Overflow
		reti									; 
.org	USI_STARTaddr							; USI Start
		reti									; 
.org	USI_OVFaddr								; USI Overflow
		reti									; 
.org	ERDYaddr								; EEPROM Ready
		reti									; 
.org	ACIaddr									; Analog Comparator
		reti									; 
.org	ADCCaddr								; ADC Conversion Complete
		reti									; 

;Initialisation code
;-------------------------------------------------------------------------------------
init:
		ldi			TEMP1,RAMEND				; Initiate Stack pointer
		out			SP,TEMP1

		ldi			TEMP1,0b00111110
		out			DDRB,TEMP1					; Set PORTB outputs

		sbi			PORTB,3						; disable SS

		ldi			TEMP1,(1<<USIWM0)|(1<<USICS1)|(1<<USICLK)
		out			USICR,TEMP1					; Init USI for 3-wire mode

		ldi			TEMP1,$0A					; Fill m_RXbuffer
		ldi			TEMP2,$00					; Ten bytes (0x00)
		ldi			YL,low(m_RXbuffer)			; 
		ldi			YH,high(m_RXbuffer)			; 

ClearRXBuffer:
		st			Y+,TEMP2
		dec			TEMP1
		brne		ClearRXBuffer

		ldi			TEMP1,$00
		sts			m_frameCur,TEMP1

		clr			m_flags

		rcall		StartRFM12B					; Setup the radio module
		
;-------------------------------------------------------------------------------------
;Main Code
; For the Main code I used two versions, A and B. This is done to allow one unit (Tiny + RFM)
; to startup  as a transmitter, while the other starts as a receiver, to prevent synchronizing
; problems. So one Tiny is flashed with the software as it is now, the other one is
; flashed with the software with two lines marked 'Version A' commented out, and the
; line marked 'Version B' uncommented. Version B will be the first to transmit, Version A
; will start as a receiver.
;-------------------------------------------------------------------------------------
Main:
		ldi			TEMP2,$29					; 2 seconds delay to show MCU is working (Version A)
;		ldi			TEMP2,$B0					; 4 seconds delay (Version B)
		sbi			PORTB,4						; red led ON
		rcall		DelayLoop
		cbi			PORTB,4						; and off again...

		rjmp		Receive						; jump to receive (Version A)

Send:
		ldi			TEMP2,$58					; create a small delay
		rcall		DelayLoop

NextFrame:
		rcall		LoadData					; Load 'new ' data and increase frame counter

SameFrame:
		rcall		SendData					; Send data

		rcall		RcvChkSum					; Switch to RX and receive the checksum

		ldi			ZL,low(m_TXbuffer)			; Point to the TX buffer, add 13 dec
		ldi			ZH,high(m_TXbuffer)			; to find the checksum that was sent.
		adiw		Z,$0D

		ldi			YL,low(m_RXbuffer)			; Point to the RX buffer, add one
		ldi			YH,high(m_RXbuffer)			; to find the checksum received.
		adiw		Y,1

		ld			TEMP1,Z						; Get them both
		ld			TEMP2,Y

		cp			TEMP1,TEMP2					; Compare, and if they are not the same
		brne		SameFrame					; send the same frame again

Receive:
		rcall		RcvData						; Then switch to receive

		rcall		CheckSum					; Compare own calculate checksum with the
												; one received
		rcall		SendChkSum					; Send own calculated checksum, regardless

		sbrc		m_flags,0					; If the own calculated and received checksum
		rjmp		Receive						; do not match, bit0 will be set, so receive again

;Send:
		rjmp		Send						; else start send again

;-------------------------------------------------------------------------------------
;                       ************* SUB-ROUTINES *************
;-------------------------------------------------------------------------------------
; DelayLoop
; Note: Generates a delay defined by content of TEMP2
; TEMP2 = $0A gives about 0.25 sec delay
;         $0F gives about 0,37 sec delay
;         $11 gives about 0,42 sec delay
;         $14 gives about 0,5  sec delay
;         $1E gives about 0,75 sec delay
;         $29 gives about 1,00 sec delay
;         $58 gives about 2,00 sec delay
;         $B0 gives about 4,00 sec delay
;
;-------------------------------------------------------------------------------------
DelayLoop:
		push		TEMP1						; save TEMP1
		in			TEMP1,SREG					; as well as SREG
		push		TEMP1						; and the rest
		push		TEMP2

		mov			DelayReg,TEMP2				; store initial delay value in DelayReg (R15) to allow access
												; from different program routines. R15 is not
												; used anywhere else.

ReloadHigh:
		ldi			TEMP2,$FF					; Fill the two other loops
ReloadLow:
		ldi			TEMP1,$FF
DecLow:
		dec			TEMP1						; and start counting down
		brne		DecLow

		dec			TEMP2
		brne		ReloadLow

		dec			DelayReg
		brne		ReloadHigh

		pop			TEMP2						; restore registers tampered with
		pop			TEMP1
		out			SREG,TEMP1
		pop			TEMP1

		ret										; and return
;-------------------------------------------------------------------------------------
; SPITransfer
; This routine will transfer one byte, contained in TEMP1 to the RFM12B module.
; Since it is a transfer, upon exiting the received byte will be in TEMP1 also.
;-------------------------------------------------------------------------------------
SPITransfer:
		out			USIDR,TEMP1					; data in data register
		ldi			TEMP1,(1<<USIOIF)			; clear overflow flag
		out 		USISR,TEMP1

SPITransfer_loop:
		sbi			USICR,USITC					; toggle the clock
		sbis		USISR,USIOIF				; 16 bits done yet?
		rjmp		SPITransfer_loop			; if not, do the next

		in			TEMP1,USIDR					; and get the data received in TEMP1

		ret
;-------------------------------------------------------------------------------------
; Do_TX
; This routine will fill the TX (transmitter) buffer with:
; 
; - 2 bytes pre-amble (0xAAAA)
; - 1 byte (0x2D) for synchronising the receiver and the transmitter
; - 1 byte (0xD4) for synchronising the receiver and the transmitter
; - 1 byte (0xXX) for the frame number (in Checksum)
; - 8 bytes of data, maximum (in Checksum), taken from m_databuffer (this number will vary between 1 and 8)
; - 1 byte for the Checksum itself
; - 2 bytes (0xAAAA) to keep the transmitter on for a little while
; =====
;  16 bytes total maximum, 9 bytes minimum
; 
; Then, Do_TX will issue a TX_ON command, send these bytes to the RFM12B module and
; then give a TX_OFF command and finally return, so prior to calling this routine,
; the data to be sent, must be in m_databuffer. and the number of data bytes must be
; in m_bufsize.
;-------------------------------------------------------------------------------------
Do_TX:
		clr			ChkSum						; 

		ldi			TEMP1,$AA					; Pre-amble byte
		ldi			YL,low(m_TXbuffer)			; Point to beginning of buffer
		ldi			YH,high(m_TXbuffer)		; 

		st			Y+,TEMP1					; Store byte 1
		st			Y+,TEMP1					; then store byte 2

		ldi			TEMP1,$2D					; Store sync byte 1
		st			Y+,TEMP1

		ldi			TEMP1,$D4					; Store sync byte 2
		st			Y+,TEMP1

		lds			TEMP1,m_frameTX				; Get frame number and store
		st			Y+,TEMP1					; in buffer

		add			ChkSum,TEMP1				; Next, calculate new Checksum value

		lds			TEMP2,m_bufsize				; Point to beginning of actual data
		ldi			ZL,low(m_databuffer)		; and expect 'm_bufsize' bytes
		ldi			ZH,high(m_databuffer)

FillTXbuffer:
		ld			TEMP1,Z+					; Transfer 8 bytes from one buffer
		st			Y+,TEMP1					; to another and
		add			ChkSum,TEMP1				; update the Checksum value

		dec			TEMP2
		brne		FillTXbuffer

		st			Y+,ChkSum					; Next, store the Checksum value in the buffer

		ldi			TEMP1,$AA					; followed by two bytes 0xAA
		st			Y+,TEMP1
		st			Y+,TEMP1

		cbi			PORTB,3						; Set SS
		sbi			PORTB,4						; Red LED ON

		ldi			ZL,low(TX_ON*2)				; TX on command
		ldi			ZH,high(TX_ON*2)
		lpm			TEMP1,Z
		adiw		Z,1

		rcall		SPITransfer					; Send through SPI

		lpm			TEMP1,Z

		rcall		SPITransfer					; Send through SPI

		sbi			PORTB,3						; SS high (de-activated)


		lds			TEMP3,m_bufsize				; Point to buffer
		subi		TEMP3,-8					; Add 8 (dec) for various bytes also in the buffer
		ldi			ZL,low(m_TXbuffer)			; and expect that number of bytes to send
		ldi			ZH,high(m_TXbuffer)

WaitSDIT1:
		cbi			PORTB,3						; Set SS

		sbis		PINB,0						; Wait for RFM12B to be ready (SDO high)
		rjmp		WaitSDIT1

		ldi			TEMP1,$B8					; data send command

		rcall		SPITransfer

		ld			TEMP1,Z+					; send 1 byte of actual data

		rcall		SPITransfer					; Do the transfer

		sbi			PORTB,3						; Clear SS

		dec			TEMP3						; next byte?

		brne		WaitSDIT1

		ldi			ZL,low(TX_OFF*2)			; TX off command
		ldi			ZH,high(TX_OFF*2)
		lpm			TEMP1,Z
		adiw		Z,1

WaitSDIT2:
		cbi			PORTB,3						; Set SS

		sbis		PINB,0						; Wait for RFM12B to be ready (SDO high)
		rjmp		WaitSDIT2

		rcall		SPITransfer					; Send TX off command

		lpm			TEMP1,Z
		rcall		SPITransfer

		sbi			PORTB,3						; Clear SS
		cbi			PORTB,4						; Red LED off

		ret
;-------------------------------------------------------------------------------------
; Do_RX
; This routine will receive a data stream consisting of:
; 
; - 1 byte (0xXX) for the frame number (in Checksum)
; - 8 bytes of data, maximum (in Checksum), this number will vary between 1 and 8
; - 1 byte for the Checksum itself
; =====
;  10 bytes total maximum, 3 bytes minimum
; 
; Initially, prior to reception, a RX_ON command will be sent to the RFM12B module, and
; after reception a RX_OFF command will be sent. Received data can be collected from
; the m_RXbuffer.
;-------------------------------------------------------------------------------------
Do_RX:
		cbi			PORTB,3						; Set SS

		clr			ChkSum						; Clear Checksum

		ldi			temp2,$06					; Four bytes to activate RX
		ldi			ZL,low(RX_ON*2)				; RX on
		ldi			ZH,high(RX_ON*2)

Set_RX:
		lpm			TEMP1,Z						; lpm r,Z+ is not supported by this MCU
		adiw		Z,1

		rcall		SPITransfer					; Send byte through SPI

		dec			TEMP2						; Loop counter...
		brne		Set_RX

		sbi			PORTB,3						; Clear SS

		lds			TEMP2,m_bufsize				; Byte counter for received bytes to be placed in buffer
		subi		TEMP2,-2					; add room for two more bytes (Frame number and ChkSum)
		ldi			ZL,low(m_RXbuffer)			; Set pointer to buffer
		ldi			ZH,high(m_RXbuffer)
		
		cbi			PORTB,3						; Set SS

		rcall		Delay125usec				; Mandatory undocumented delay required for RX mode

WaitSDIR1:
		cbi			PORTB,3						; Set SS

		sbis		PINB,0						; Wait for RFM12B to be ready (SDO high)
		rjmp		WaitSDIR1

		sbi			PORTB,5						; Light green LED upon reception

		ldi			TEMP1,$B0					; data receive command

		rcall		SPITransfer					; send the command byte

		ldi			TEMP1,$00					; data receive command

		rcall		SPITransfer					; exchange the data byte

		st			Z+,TEMP1					; place byte received in the buffer and increase the pointer

		cpi			TEMP2,$01					; Check for the last byte
		breq		NoAddRX						; If it is the last, do NOT

		add			ChkSum,TEMP1				; add this value to the Checksum value

NoAddRX:
		sbi			PORTB,3						; Clear SS

		dec			TEMP2						; Loop counter
		brne		WaitSDIR1

WaitSDIR2:
		cbi			PORTB,3						; Set SS

		sbis		PINB,0						; Wait for RFM12B to be ready (SDO high)
		rjmp		WaitSDIR2

		cbi			PORTB,5						; Green led off

		ldi			ZL,low(RX_OFF*2)			; RX OFF command
		ldi			ZH,high(RX_OFF*2)

		lpm			TEMP1,Z
		adiw		Z,1

		rcall		SPITransfer					; Send through SPI
;
		lpm			TEMP1,Z
		adiw		Z,1

		rcall		SPITransfer					; Send through SPI

		sbi			PORTB,3						; Clear SS

		ret
;-------------------------------------------------------------------------------------
StartRFM12B:
		ldi			ZL,low(InitRFM12B*2)		; point to init string
		ldi			ZH,high(InitRFM12B*2)

		ldi			TEMP2,$1C					; 28 bytes long

		cbi			PORTB,3						; enable SS

SendString:
		lpm			TEMP1,Z						; get byte and
		adiw		Z,1							; increase pointer (Z+ not supported)

		rcall		SPITransfer					; Send through SPI

		dec			TEMP2						; Next byte
		brne		SendString					; Done yet?

		sbi			PORTB,3						; disable SS

		ret

;-------------------------------------------------------------------------------------
CheckSum:
		ldi			ZL,low(m_RXbuffer)			; Point to RX buffer
		ldi			ZH,high(m_RXbuffer)

		ld			TEMP1,Z+					; First byte is the frame number
		sts			m_frameRX,TEMP1				; Store appropriately

		clr			TEMP1						; Increase the pointer with the
		lds			TEMP2,m_bufsize				; buffer length, it then points to the Checksum

		add			ZL,TEMP2					; This is a 16-bits add
		adc			ZH,TEMP1

		ld			TEMP1,Z						; Get the Checksum value

		cp			TEMP1,ChkSum				; Is it good?

		brne		ChkSumFail					; If not, indicate 'FAIL' by means of the flag

		cbr			m_flags,1					; or else clear the flag and exit.

		lds			TEMP1,m_frameRX				; REMOVE THESE TWO LINES if the frame
		sts			m_frameCur,TEMP1			; counter must count 'double' (each TX on a unit 
												; will be independently numbered of the other)
		ret

ChkSumFail:
		sbr			m_flags,1					; Set the flag and

		sbi			PORTB,4						; show the world that there
		ldi			TEMP2,$0A					; was a hickup by means of 
		rcall		DelayLoop					; the red led. (not mandatory)
		cbi			PORTB,4
		ret

;-------------------------------------------------------------------------------------
LoadData:
		lds			TEMP1,m_frameCur			; Get the current frame number
		inc			TEMP1						; increase by one and
		sts			m_frameCur,TEMP1			; place it back

		ldi			TEMP2,$08
		ldi			ZL,low(Example*2)			; TEST DATA
		ldi			ZH,high(Example*2)
		ldi			YL,low(m_databuffer)		; TEST DATA
		ldi			YH,high(m_databuffer)

DummyData:
		lpm			TEMP1,Z						; Get the data and store in the buffer
		adiw		Z,1
		st			Y+,TEMP1
		dec			TEMP2
		brne		DummyData

		ret
;-------------------------------------------------------------------------------------
SendData:
		ldi			TEMP1,$08					; Set the buffer size
		sts			m_bufsize,TEMP1

		lds			TEMP1,m_frameCur			; Pick up the frame number
		sts			m_frameTX,TEMP1

		rcall		Do_TX

		ret
;-------------------------------------------------------------------------------------
SendChkSum:
		ldi			TEMP1,$01					; Set the buffer size
		sts			m_bufsize,TEMP1

		lds			TEMP1,m_frameRX				; and the frame number received
		sts			m_frameTX,TEMP1

		ldi			ZL,low(m_databuffer)		; Put the checksum of the last received
		ldi			ZH,high(m_databuffer)		; frame in the buffer

		st			Z,ChkSum					; 

		rcall		Do_TX						; and send it back

		ret
;-------------------------------------------------------------------------------------
RcvData:
		ldi			TEMP1,$08					; Set the buffer size
		sts			m_bufsize,TEMP1

		rcall		Do_RX						; and start receiving

		ret
;-------------------------------------------------------------------------------------
RcvChkSum:
		ldi			TEMP1,$01					; Set the buffer size
		sts			m_bufsize,TEMP1

		rcall		Do_RX						; and receive the checksum value

		ret
;-------------------------------------------------------------------------------------
Delay125usec:
		ldi			TEMP1,$04					; This will create a delay of 125 usec.
		ldi			TEMP3,$52					; (beware of registers in use (no push))
Smalloop:
		dec			TEMP3
		brne		Smalloop
		ldi			TEMP3,$52
		dec			TEMP1
		brne		Smalloop

		ret
;-------------------------------------------------------------------------------------
InitRFM12B:
.db $80,$E7			; internal data reg enabled, FIFO enabled, 868 Mc, 10pF crystal load
.db $82,$08			; enable rcv chain, rcv switch on sep, power amp off, synth on, osc on, clock out off
.db $A6,$90			; set freq 868.400 MHz
.db $C6,$09			; set bit rate to 34.4 kbps (TX), (RX not quite clear)
;.db $C6,$47		; set bit rate to 4.789 kbps (TX), (RX not quite clear)
.db $94,$A0			; RX VDI output, fast, BW 134 kHz, LNA 0 dB, RSSI -103 dBm
.db $C2,$AC			; CR start in fast mode, then slow mode (16 bits preamble), digital filter, rest unknown
.db $CA,$81			; FIFO IT to 8 bits, synchron pattern 0x2DD4, disable reset
.db $CE,$D4			; 2nd sync byte to 0xD4
.db $C4,$83			; Keep foffset only during receiving (VDI=high)
;.db $98,$50		; TX FSK params, TX output 0 dBm
;.db $98,$54		; TX FSK params, TX output -12 dBm
.db $98,$57			; TX FSK params, TX output -21 dBm
.db $CC,$17			; PLL setting command
.db $E0,$00			; Wakeup timer to zero
.db $C0,$00			; Low battery detector command, MCU clock divider CK/1
.db $00,$00			; Read Status Register
;-------------------------------------------------------------------------------------

TX_ON:
.db $82,$39		; Activate TX

TX_OFF:
.db $82,$08		; De-activate TX

RX_ON:
.db $CA,$81		; Activate RX and reset FIFO
.db $CA,$83
.db $82,$99

RX_OFF:
.db $82,$08		; De-activate RX

Example:
.db 'K','e','i','z','e','r',' ',' '



;***************************************************************************
;*
;*
;***************************************************************************

#include <avr\io.h>

		.data
		.global Servo1
		.global Servo2
		.global Servo3
		.global Prescaler8
		.global Prescaler64
		.global Prescaler20ms
		.global Wating20ms
		.global TXBuffer
		.global TXCountBuffer
		.global PutHex	
		.global	EepromRead
		.global	PutCRNL
#ifdef EEPROM_WRITE
		.global	EepromWrite
#endif

#define IOBASE 0x20

Prescaler8:
		.space	1
Prescaler64:
		.space	1
Prescaler20ms:
		.space	1
Wating20ms:
		.space	1

Servo1:
		.space	1
Servo2:
		.space	1
Servo3:
		.space	1

PreServo1:
		.space	1
PreServo2:
		.space	1
PreServo3:
		.space	1
TXBuffer:
		.space	1
TXCountBuffer:
		.space	1
RXdata:
		.space	1
RXdata_Incomming:
		.space	1
RXdata_Counter:
		.space	1
RXdata_BitLevel:
		.space	1
RXdata_State:
		.space	1
RXdata_Ready:
		.space	1

		.text				; switches to the .text section (code and ROM constants)
		.global __vector_6
		.global Puts
		.global PutChar
		.global GetChar

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;		Suport for the serial Transmit data
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Puts:
		movw 	R30,R24				; Copy the Pointer to the Z register
Puts1:
		lpm		R24,Z+
		tst		R24
		breq	PutCharEnd			; return is the char is 0;
		rcall	PutChar	
		rjmp 	Puts1
PutCRNL:							; Will send the \r\n 
		ldi		R24,'\r'
		rcall	PutChar
		ldi		R24,'\n'
PutChar:
		lds		R16,TXCountBuffer
		tst		R16
		brne	PutChar
		sts		TXBuffer,R24
		ldi		R16,11				; Set the number of bits
		sts		TXCountBuffer,R16
PutCharEnd:
		ret
PutHex:
		push	R24
		swap 	R24					; This will send the Hi Nibble
		andi	R24,0x0f
		ldi		R25,'0'
		add		R24,R25
		cpi		R24,'9'+1
		brlo	Hex_Hi
		ldi		R25,'A'-'0'-10
		add		R24,R25
Hex_Hi:
		rcall   PutChar
		pop 	R24
		andi	R24,0x0f			; This will send the Low Nibble
		ldi		R25,'0'
		add		R24,R25
		cpi		R24,'9'+1
		brlo	Hex_Low
		ldi		R25,'A'-'0'-10
		add		R24,R25
Hex_Low:
		rjmp    PutChar
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;		Suport for the serial Recive data
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
GetChar:
		lds		R16,RXdata_Ready
		tst		R16
		brne	GetChar
		lds		R24,RXdata
		ldi		R16,1
		sts		RXdata_Ready,R16
		ret 		
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
__vector_6:  
		push	R16 
		in  	R16,SREG-IOBASE
		push	R16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;		Start of Timer interrupt
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This part will recive a bit to the RS232 device
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		lds		R16,RXdata_State
		cpi		R16,9
		brlo	WaitForDataBit
WaitForStartBit:
		lds		R16,RXdata_Counter
		sbis	PINB-IOBASE,1				
		ldi		R16,8
		tst		R16
		breq	StartBitEnded
		dec		R16
		sts		RXdata_Counter,R16
		rjmp	EndRXD
StartBitEnded:	
		ldi		R16,8
		sts		RXdata_Counter,R16
		sts		RXdata_State,R16
		rjmp	EndRXD
WaitForDataBit:		
		lds		R16,RXdata_BitLevel
		sbic	PINB-IOBASE,1				
		inc		R16		
		sts		RXdata_BitLevel,R16
		lds		R16,RXdata_Counter
		dec		R16
		sts		RXdata_Counter,R16
		brne	EndRXD
		lds		R16,RXdata_BitLevel
		cpi		R16,4
		lds		R16,RXdata_Incomming
		ror		R16
		sts		RXdata_Incomming,R16
		ldi		R16,0
		sts		RXdata_BitLevel,R16
		ldi		R16,8
		sts		RXdata_Counter,R16
		lds		R16,RXdata_State
		dec		R16
		sts		RXdata_State,R16
		brne	EndRXD
		ldi		R16,9
		sts		RXdata_State,R16
		ldi		R16,RXdata_Incomming
		sts		RXdata,R16
RXDataByteReady:
		ldi		R16,9					; Wait for start bit
		sts		RXdata_State,R16
		lds		R16,RXdata_Incomming	; Shift register to Data Register 
		sts		RXdata,R16
		ldi		R16,0					; Signal the Ready state
		sts		RXdata_Ready,R16
EndRXD:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;		End of Recive data byte
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

		lds		R16,Prescaler8		
		dec		R16
		sts		Prescaler8,R16
		breq    APrescaler8
		rjmp	NoPrescaler8
APrescaler8:
		ldi		R16,8					; Reload for the 104 us Timer
		sts		Prescaler8,R16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This Part will once every 8 interupts  104.06 us 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This part will send a bit to the RS232 device
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		lds		R16,TXCountBuffer
		cpi		R16,11					; Start bit
		brne	TXOutputBit
; 		This will set the line as output before making a StartBit
		cbi		PORTB-IOBASE,0			; TXD Pin Inverted
		sbi		DDRB-IOBASE,0				; TXD Pin As output
		rjmp	TXputBitEnd
TXOutputBit:
		tst		R16
		breq	TXNoStopBit				; Ready Sending the Byte and stop bits
		cpi 	R16,10
		brne 	TXNoStartBit				
		sbi		PORTB-IOBASE,0			; TXD Pin StartBit Inverted
		rjmp    TXputBitEnd
TXNoStartBit:
		lds		R16,TXBuffer
		sec
		ror		R16
		sts		TXBuffer,R16
		brcc	TXputbit0				;If carry set
		cbi		PORTB-IOBASE,0			; TXD Pin Inverted ; send a '1'
		rjmp	TXputBitEnd				;else	
TXputbit0:	
		sbi		PORTB-IOBASE,0			; TXD Pin Inverted ; send a '0'
TXputBitEnd:
		lds		R16,TXCountBuffer
		dec		R16
		sts		TXCountBuffer,R16
		rjmp	TXEND
TXNoStopBit:
		cbi		DDRB-IOBASE,0				; TXD Pin As input
TXEND:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;		End of Transmit data byte
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

		lds		R16,Prescaler64		
		dec		R16
		sts		Prescaler64,R16
NoPrescaler8_2:
		brne    NoPrescaler64
		ldi		R16,8
		sts		Prescaler64,R16
; This Part will once every 64 interupts  833.28 us 

		lds		R16,Prescaler20ms		
		dec		R16
		sts		Prescaler20ms,R16
		brne    NoPrescaler20ms
		ldi		R16,25			; Reload the 20 ms timer
		sts		Prescaler20ms,R16

; This Part will once every 24 interupts  20ms
		lds		R16,Wating20ms
		tst		R16
		breq 	NoWaitAnyMore
		dec		R16
		sts		Wating20ms,R16
NoWaitAnyMore:
		sbi 	PORTB-IOBASE,2	; Servo 1
		sbi 	PORTB-IOBASE,3	; Servo 2
		sbi 	PORTB-IOBASE,4	; Servo 3

		lds		R16,Servo1
		sts 	PreServo1,R16
		lds		R16,Servo2
		sts 	PreServo2,R16
		lds		R16,Servo3
		sts 	PreServo3,R16

NoPrescaler20ms:
NoPrescaler64:
NoPrescaler8:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This part make the Servo Pulse
;      _____
; ____|     |__________________________________________________________
;
; The pulse is max 256 * 13.02 Us = 3.3 ms long and every 20ms 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This Part will once every interupts  13.02 us 
		lds		R16,PreServo1
		tst		R16
		brne	NoSerovo1reset
		cbi 	PORTB-IOBASE,2	; Servo 1
		inc		R16
NoSerovo1reset:
		dec		R16
		sts		PreServo1,R16

		lds		R16,PreServo2
		tst		R16
		brne	NoSerovo2reset
		cbi 	PORTB-IOBASE,3	; Servo 2
		inc		R16
NoSerovo2reset:
		dec		R16
		sts		PreServo2,R16

		lds		R16,PreServo3
		tst		R16
		brne	NoSerovo3reset
		cbi 	PORTB-IOBASE,4	; Servo 3
		inc		R16
NoSerovo3reset:
		dec		R16
		sts		PreServo3,R16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 		End of interupt 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		pop		R16
		out		SREG-IOBASE,R16
		pop		R16 
		reti 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 		Eeprom Support
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; EepromRead(a);
; R24 is the Addres
; R24 will return the Readed data
EepromRead:
; Wait for completion of previous write
		sbic EECR-IOBASE,EEPE
		rjmp EepromRead
; Set up address (R24) in address register
		out EEARL-IOBASE, R24
; Start eeprom read by writing EERE
		sbi EECR-IOBASE,EERE
; Read data from data register
		in R24,EEDR-IOBASE
		ret

#ifdef EEPROM_WRITE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;EepromWrite(a,d);
; R24/R25 will have the addres
; R23 will have the data
EepromWrite:
; Wait for completion of previous write
		sbic EECR-IOBASE,EEPE
		rjmp EepromWrite
; Set Programming mode
		ldi R25, (0<<EEPM1)|(0<<EEPM0)
		out EECR-IOBASE, R25
; Set up address (R24) in address register
		out EEARL-IOBASE, R24
; Write data (R23) to data register
		out EEDR-IOBASE,R23
; Write logical one to EEMPE
		sbi EECR-IOBASE,EEMPE
; Start eeprom write by setting EEPE
		sbi EECR-IOBASE,EEPE
		ret		
#endif

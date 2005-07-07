

.INCLUDE "m16def.inc" 



	jmp 	RESET 			; Reset Handler
 	reti 					;jmp EXT_INT0 ; IRQ0 Handler
 	reti 					;jmp EXT_INT1 ; IRQ1 Handler
 	reti 					;jmp TIM2_COMP ; Timer2 Compare Handler
 	reti 					;jmp TIM2_OVF ; Timer2 Overflow Handler
 	reti 					;jmp TIM1_CAPT ; Timer1 Capture Handler
 	reti 					;jmp TIM1_COMPA ; Timer1 CompareA Handler
 	reti 					;jmp TIM1_COMPB ; Timer1 CompareB Handler
 	reti 					;jmp TIM1_OVF ; Timer1 Overflow Handler
 	reti 					;jmp TIM0_OVF ; Timer0 Overflow Handler
 	reti 					;jmp SPI_STC ; SPI Transfer Complete Handler
 	reti 					;jmp USART_RXC ; USART RX Complete Handler
 	reti 					;jmp USART_UDRE ; UDR Empty Handler
 	reti 					;jmp USART_TXC ; USART TX Complete Handler
 	reti 					;jmp ADC ; ADC Conversion Complete Handler
 	reti 					;jmp EE_RDY ; EEPROM Ready Handler
 	reti 					;jmp ANA_COMP ; Analog Comparator Handler
 	reti 					;jmp TWSI ; Two-wire Serial Interface Handler
 	reti 					;jmp EXT_INT2 ; IRQ2 Handler
 	reti 					;jmp TIM0_COMP ; Timer0 Compare Handler
 	reti 					;jmp SPM_RDY ; Store Program Memory Ready Handler


RESET: 

;		.DEF temp=R16 

		.EQU monitor_w = $58
		.EQU monitor_r = $59
		
		.EQU ioexp1_w = $40
		.EQU ioexp1_r = $41
		
		.EQU ioexp2_w = $42
		.EQU ioexp2_r = $43

		.EQU mem_w = $AE
		.EQU mem_r = $AF
		
		.DEF dev_adrs = r17
		.DEF adrs = r18
		.DEF dat1 = r19
		.DEF dat2 = r20
		.DEF cbyt = r21
		.DEF chr  = r25
		
;-----------------------------------
		
		ldi	r16,high(RAMEND) 	; Main program start
		out	SPH,r16 			; Set Stack Pointer to top of RAM
		ldi	r16,low(RAMEND)
		out	SPL,r16

		
		clr	r16
		out ddrd, r16			;DirD input
		out ddra, r16			;DirA input
		out ddrc, r16			;DirC input


		ser r16
		out ddrb, r16			;DirB output
		ldi r16, $FE			; 1 LED on
		out portb, r16			;PortB high

;---------xxxxxxx------------xxxxxxxxxxxx-------------xxxxxxxxxxx---------xxxxxxxxx

main:
		rcall twi_setup
		rcall usart_setup
		;rcall init_io1
		;rcall init_io2
recv:	call disp_1

		call U_R
		cpi dat1, $52		; R
		breq read_sub
		cpi dat1, $72		; r
		breq read_sub
		cpi dat1, $57		; W
		breq write_sub
		cpi dat1, $77		; w
		breq write_sub
		;out portb, dat1
		jmp recv

	

		
;-----------------------------
read_sub:
		call disp_r
		call get_char			;get device adrs
		mov dev_adrs, chr
;		out portb, chr
		call disp_adrs			
		call get_char			;get adrs
		mov adrs, chr
;		out portb, adrs
		call read_i2cbyte		;read from i2c
		out portb, dat1
		call U_T
		jmp recv
		


;----------------------------
;----------------------------
write_sub:
		call disp_r
		call get_char			; get device adrs
		mov dev_adrs,chr
		;out portb, dev_adrs
		call disp_adrs
		call get_char			;get adrs
		mov adrs, chr
		;out portb, adrs
		call disp_dat
		call get_char			;get data1
		mov dat1, chr
		;out portb, dat1
		call write_i2cbyte		; write i2c
		jmp recv
stop:
		rjmp stop
;-----------------------------




;-----------------------------
disp_r:
		ldi XH, $00		; R27
		ldi XL, $1F		; R26
again2:
		rcall ee_read
		rcall U_T
		inc XL
		cpi XL, $34
		brne again2
		ret
;-----------------------------
;------------------------------
disp_adrs:
		ldi XH, $00		; R27
		ldi XL, $32		; R26
again4:
		rcall ee_read
		rcall U_T
		inc XL
		cpi XL, $3E
		brne again4
		ret

;-------------------------------
;-------------------------------
disp_dat:
		ldi XH, $00		; R27
		ldi XL, $3C		; R26
again5:
		rcall ee_read
		rcall U_T
		inc XL
		cpi XL, $48
		brne again5
		ret
;-------------------------------
;-------------------------------
get_char:

		call U_R
		mov r16, dat1
		andi r16, $F0		;XXXX 0000
		cpi r16, $40		;0010 0000
		breq ltr
		mov r16, dat1
		andi r16, $0F		;0000 XXXX
		mov chr, r16		;0000 XXXX
		swap chr			;XXXX 0000
		jmp cont

ltr:
		mov r16, dat1
		andi r16, $0F		;0000 XXXX
		cpi r16,$01	
		breq A
		cpi r16,$02	
		breq B
		cpi r16,$03	
		breq C
		cpi r16,$04
		breq D
		cpi r16,$05	
		breq E
		cpi r16,$06	
		breq F

A:		ldi chr, $A0
		jmp cont
B:		ldi chr, $B0
		jmp cont
C:		ldi chr, $C0
		jmp cont
D:		ldi chr, $D0
		jmp cont
E:		ldi chr, $E0
		jmp cont
F:		ldi chr, $F0
		jmp cont

				
cont:	
		call U_R
		mov r16, dat1
		andi r16, $F0		;XXXX 0000
		cpi r16, $40		;0010 0000
		breq ltr2
		mov r16, dat1
		andi r16, $0F		;0000 XXXX
		or	chr, r16
		jmp	chrdone

ltr2:
		mov r16, dat1
		andi r16, $0F		;0000 XXXX
		cpi r16,$01	
		breq AA
		cpi r16,$02	
		breq BB
		cpi r16,$03	
		breq CC
		cpi r16,$04
		breq DD
		cpi r16,$05	
		breq EE
		cpi r16,$06	
		breq FF

AA:		ori chr, $0A
		jmp chrdone
BB:		ori chr, $0B
		jmp chrdone
CC:		ori chr, $0C
		jmp chrdone
DD:		ori chr, $0D
		jmp chrdone
EE:		ori chr, $0E
		jmp chrdone
FF:		ori chr, $0F
		jmp chrdone


chrdone:
		ret


;------------------WRITE I2C-WORD----------------

write_i2cword:

		ldi r16, (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)	;Transmit start cond
		out TWCR, r16

		rcall tw_wait
		
		;in r16,TWSR
		;andi r16, $F8
		;cpi r16, $08			;Read SReg. $08 received (START sent)
		;brne ERROR
;---------------------------
									
		out TWDR, dev_adrs				;device  address + W
		ldi r16, (1<<TWINT) | (1<<TWEN)
		out TWCR, r16

		rcall tw_wait

		;in r16,TWSR				;Read SReg. $18 received (Acknowledge)
		;andi r16,$F8
		;cpi r16, $18
		;brne ERROR
;--------------------------

		out TWDR, cbyt				; load command byte
		ldi r16, (1<<TWINT) | (1<<TWEN)
		out TWCR, r16

		rcall tw_wait

		;in r16,TWSR				;Read SReg. $28 received (Acknowledge)
		;andi r16, 0xF8
		;cpi r16, $28
		;brne ERROR
;-----------------------

		out TWDR, dat1
		ldi r16, (1<<TWINT) | (1<<TWEN)
		out TWCR, r16

		rcall tw_wait

		;in r16,TWSR				;Read SReg. $28 received (Acknowledge)
		;andi r16, 0xF8
		;cpi r16, $28
		;brne ERROR
;------------------------------

		out TWDR, dat2
		ldi r16, (1<<TWINT) | (1<<TWEN)
		out TWCR, r16

		rcall tw_wait

		;in r16,TWSR				;Read SReg. $28 received (Acknowledge)
		;andi r16, 0xF8
		;cpi r16, $28
		;brne ERROR
;------------------------------

		
		ldi r16, (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)	;Transmit stop cond
		out TWCR, r16

		ret
		
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


;------------------WRITE I2C-BYTE---------------------------

write_i2cbyte:

		ldi r16, (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)	;Transmit start cond
		out TWCR, r16

		rcall tw_wait
		
		;in r16,TWSR
		;andi r16, $F8
		;cpi r16, $08				;Read SReg. $08 received (START sent)
		;brne ERROR
;-------------------

		out TWDR, dev_adrs					;device address + W
		ldi r16, (1<<TWINT) | (1<<TWEN)
		out TWCR, r16

		rcall tw_wait

		;in r16,TWSR				;Read SReg. $18 received (Acknowledge)
		;andi r16,$F8
		;cpi r16, $18
		;brne ERROR
;---------------------
		;ldi r16, 
		out TWDR, adrs				; load  address
		ldi r16, (1<<TWINT) | (1<<TWEN)
		out TWCR, r16

		rcall tw_wait

		;in r16,TWSR				;Read SReg. $28 received (Acknowledge)
		;andi r16, 0xF8
		;cpi r16, $28
		;brne ERROR
;-----------------------

									;load  Data
		out TWDR, dat1
		ldi r16, (1<<TWINT) | (1<<TWEN)
		out TWCR, r16

		rcall tw_wait

		;in r16,TWSR				;Read SReg. $28 received (Acknowledge)
		;andi r16, 0xF8
		;cpi r16, $28
		;brne ERROR
;------------------------------
		ldi r16, (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)	;Transmit stop cond
		out TWCR, r16

		ret
		

;XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

		
	

;------------------READ I2C-BYTE----------------


read_i2cbyte:

		ldi r16, (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)	;Transmit start cond
		out TWCR, r16

		rcall tw_wait
		
		;in r16,TWSR
		;andi r16, $F8
		;cpi r16, $08			;Read SReg. $08 received (START sent)
		;brne ERROR
;---------------------------------

		out TWDR, dev_adrs			;device address + W
		ldi r16, (1<<TWINT) | (1<<TWEN)
		out TWCR, r16

		rcall tw_wait

		;in r16,TWSR				;Read SReg. $40 received (Acknowledge)
		;andi r16,$F8
		;cpi r16, $18
		;brne ERROR
;---------------------------------

				
		out TWDR, adrs				;send Byte Address
		ldi r16, (1<<TWINT) | (1<<TWEN)
		out TWCR, r16

		rcall tw_wait

		;in r16,TWSR				;Read SReg. $28 received (Acknowledge)
		;andi r16, 0xF8
		;cpi r16, $28
		;brne ERROR
;---------------------------------

		ldi r16, (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)	;Transmit start cond
		out TWCR, r16

		rcall tw_wait
		
		;in r16,TWSR
		;andi r16, $F8
		;cpi r16, $10				;Read SReg. $10 received (Rep-START sent)
		;brne ERROR
;-----------------------------------
		ldi r16, $01
		add dev_adrs, r16				;add 1 to dev.adrs to make  read adrs
		out TWDR, dev_adrs				;device address + R
		ldi r16, (1<<TWINT) | (1<<TWEN)
		out TWCR, r16

		rcall tw_wait

		;in r16,TWSR				;Read SReg. $40 received (Acknowledge)
		;andi r16,$F8
		;cpi r16, $40
		;brne ERROR
;-----------------------------------
		ldi r16, (1<<TWINT) | (1<<TWEN)			;Read Data byte
		out TWCR, r16

		rcall tw_wait

		in dat1, twdr
		;in r16,TWSR				;Read SReg. $58 received (Acknowledge)
		;andi r16,$F8
		;cpi r16, $58

;---------------------------------

		ldi r16, (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)	;Transmit stop cond
		out TWCR, r16
		ret
;-------------------------------------------

	

;------------------READ I2C-WORD----------------


read_i2cword:

		ldi r16, (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)	;Transmit start cond
		out TWCR, r16

		rcall tw_wait
		
		;in r16,TWSR
		;andi r16, $F8
		;cpi r16, $08			;Read SReg. $08 received (START sent)
		;brne ERROR
;---------------------------------

		out TWDR, dev_adrs			;device address + W
		ldi r16, (1<<TWINT) | (1<<TWEN)
		out TWCR, r16

		rcall tw_wait

		;in r16,TWSR				;Read SReg. $40 received (Acknowledge)
		;andi r16,$F8
		;cpi r16, $18
		;brne ERROR
;---------------------------------

				
		out TWDR, adrs				;send Byte Address
		ldi r16, (1<<TWINT) | (1<<TWEN)
		out TWCR, r16

		rcall tw_wait

		;in r16,TWSR				;Read SReg. $28 received (Acknowledge)
		;andi r16, 0xF8
		;cpi r16, $28
		;brne ERROR
;---------------------------------

		ldi r16, (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)	;Transmit start cond
		out TWCR, r16

		rcall tw_wait
		
		;in r16,TWSR
		;andi r16, $F8
		;cpi r16, $10				;Read SReg. $10 received (Rep-START sent)
		;brne ERROR
;-----------------------------------
		ldi r16, $01
		add dev_adrs, r16				;add 1 to dev.adrs to make  read adrs
		out TWDR, dev_adrs				;device address + R
		ldi r16, (1<<TWINT) | (1<<TWEN)
		out TWCR, r16

		rcall tw_wait

		;in r16,TWSR				;Read SReg. $40 received (Acknowledge)
		;andi r16,$F8
		;cpi r16, $40
		;brne ERROR
;-----------------------------------
		ldi r16, (1<<TWINT) | (1<<TWEN)			;Read Data byte
		out TWCR, r16

		rcall tw_wait

		in dat1, twdr
		;in r16,TWSR				;Read SReg. $58 received (Acknowledge)
		;andi r16,$F8
		;cpi r16, $58

;---------------------------------

		ldi r16, (1<<TWINT) | (1<<TWEN)			;Read Data byte
		out TWCR, r16

		rcall tw_wait

		in dat2, twdr
		;in r16,TWSR				;Read SReg. $58 received (Acknowledge)
		;andi r16,$F8
		;cpi r16, $58

;---------------------------------	
		ldi r16, (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)	;Transmit stop cond
		out TWCR, r16
		ret
;-------------------------------------------

;--------------ERROR-----------

ERROR:
		in r16,TWSR
		andi r16, $F8
		ldi r16, $AA
		out portb, r16


		ret

;---------END ERROR------------

;--------------TWI SETUP---------------
twi_setup:
		ldi r16, $0A
		out twbr, r16			; Bit Rate Reg = 10 d.
		ldi r16, $00
		out twsr, r16			; TW status Reg.
		clr r16
		out twar, r16			; TW address Reg.
		ldi r16, $04
		out twcr, r16			; TW control Reg
		ret
;------------END TWI SETUP--------------

;------------TWI WAIT-----------

tw_wait:						; wait for int. flag to set
		in r16,TWCR
		sbrs r16,TWINT
		rjmp tw_wait
		ret

;-------------END TWI WAIT------

;-----------------IOEXP_1 setup----------------

init_io1:
		ldi dev_adrs, ioexp1_w
		ldi cbyt, $06				;Direction Regs   [0=o/p, 1=i/p]
		ldi dat1, $F0
		ldi dat2, $4F
		rcall write_i2cword

		ldi cbyt, $00				;GPIO Regs
		ldi dat1, $0E
		ldi dat2, $00
		rcall write_i2cword
		ret
		
;-----------------IOEXP_2 setup---------------
init_io2:
		ldi dev_adrs, ioexp2_w
		ldi cbyt, $06				;Direction Regs   [0=o/p, 1=i/p]
		ldi dat1, $FF
		ldi dat2, $00
		rcall write_i2cword

		ldi cbyt, $00				;GPIO Regs
		ldi dat1, $00
		ldi dat2, $02
		rcall write_i2cword
		ret

;-----------------------------------------
;--------------USART_Transmit-------------
U_T:
		sbis UCSRA,UDRE				; Wait for empty transmit buffer
		rjmp U_T
									; Put data (r16) into buffer, sends the data
		out UDR,dat1
		ret
;-------------USART_RECEIVE---------------

U_R:									
		sbis UCSRA, RXC				; Wait for data to be received
		rjmp U_R
									; Get and return received data from buffer
		in dat1, UDR
		ret
;----------------END----------------------

;----------EEPROM WRITE-----------------

ee_write:
		sbic EECR,EEWE				; Wait for completion of previous write
		rjmp ee_write

		out EEARH, XH				; Set up address (r18:r17) in address register
		out EEARL, XL

		out EEDR,dat1				; Write data (r16) to data register

		sbi EECR,EEMWE 				; Write logical one to EEMWE

		sbi EECR,EEWE				; Start eeprom write by setting EEWE
		ret
;---------EEPROM WRITE END-------------
;-----------EEPROM READ---------------
ee_read:

		sbic EECR,EEWE				; Wait for completion of previous write
		rjmp ee_read

		out EEARH, XH				; Set up address (r18:r17) in address register
		out EEARL, XL

		sbi EECR,EERE				; Start eeprom read by writing EERE
		;rcall delay
		in dat1,EEDR					; Read data from data register
		ret
;----------EEPROM READ END-----------

;----------DISPLAY1-----------	
disp_1:
		ldi XH, $00		; R27
		ldi XL, $00		; R26
again1:
		rcall ee_read
		rcall U_T
		inc XL
		cpi XL, $21
		brne again1
		ret
;----------disp1 end-----------







;---------------------Delay ----------------
delay:
		ldi r22, $05
wait0:	ldi r23, $FF
wait1:	ldi r24, $FF
wait2:	dec r24
		brne wait2
		
		dec r23
		brne wait1

		dec	r22
		brne wait0

		ret	
;--------------------Delay End------------------

;---------------USART SETUP---------------------	

usart_setup:
		ldi r17,$00				;Buad rate H
		ldi r16,$01				;Baud rate L = 115200
		
		out UBRRH,r17
		out UBRRL,r16		
		
		ldi r16,(0<<U2X)|(0<<MPCM)
		out UCSRA,r16

		ldi r16,(0<<RXCIE)|(0<<TXCIE)|(0<<UDRIE)|(1<<RXEN)|(1<<TXEN)|(0<<UCSZ2)
		out UCSRB,r16

		ldi r16,(1<<URSEL)|(0<<UMSEL)|(0<<UPM1)|(0<<UPM0)|(0<<USBS)|(1<<UCSZ1)|(1<<UCSZ0)|(1<<UCPOL)
		out UCSRC,r16
		ret

;------------USART SETUP END------------------		


.ESEG 
;			 n/l	 p	 r	 e	 s	 s		 R	 	 t	 o		r	 e	 a	 d		 W		 t	 o		 w	 r	 i	 t	 e	.	.	.	 n/l 	 i	 /	 p		 D	 e	 v	 i	 c	 e		 a	 d	 r	 s	 .	 .	n/l		 i	 /	 p	 	 a	 d	 r	 s	 n/l	 i	 /	 p		 d	 a	 t	 a	 n/l
const1: .DB $0A,$0D,$70,$72,$65,$73,$73,$20,$52,$20,$74,$6F,$20,$72,$65,$61,$64,$20,$57,$20,$74,$6F,$20,$77,$72,$69,$74,$65,$2E,$2E,$2E,$0A,$0D,$69,$2F,$70,$20,$44,$65,$76,$69,$63,$65,$20,$61,$64,$72,$73,$2E,$2E,$0A,$0D,$69,$2F,$70,$20,$61,$64,$72,$73,$0A,$0D,$69,$2F,$70,$20,$64,$61,$74,$61,$0A,$0D
;			 0	 1	 2	 3	 4	 5	 6	 7	 8	 9	 a	 b	 c	 d	 e	 f	 10	 11	 12	 13	 14	 15	 16	 17	 18	 19	 1a	 1b	 1c	 1d	 1e	 1f	 20	 21	 22	 23	 24	 25	 26	 27	 28	 29	 2a	 2b	 2c	 2d	 2e	 2f	 30	 31	 32	 33	 34  35	 36	 37	 38	 39	 3a	 3b	 3c	 3d	 3e	 3f	 40	 41	 42	 43	 44	 45	 46	 47

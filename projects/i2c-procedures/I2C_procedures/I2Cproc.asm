
;nastavení pro procedury, které pracují s I2C sbernicí
;.equ I2CDataOPort = PORTB
;.equ I2CDataIPort = PINB
;.equ I2CDirPort = DDRB
;.equ I2CSCL = 0
;.equ I2CSDA = 1
;.equ NoACK = 1
;.equ ACK = 0

.macro Wait
	rjmp Wait1
	Wait1:
	rjmp Wait2
	Wait2:
	rjmp Wait3
	Wait3:
.endmacro

.macro I2CByteWrite
	rcall StartBitI2C
	ldi temp,@0
	rcall TransmitByteI2C
	ldi temp,@1
	rcall TransmitByteI2C
	ldi temp,@2
	rcall TransmitByteI2C
	rcall StopBitI2C
.endmacro

.macro I2CSetAddress
	rcall StartBitI2C
	ldi temp,@0
	rcall TransmitByteI2C
	ldi temp,@1
	rcall TransmitByteI2C
	rcall StopBitI2C
.endmacro

.macro I2CPageWrite
	rcall StartBitI2C
	ldi temp,@0
	rcall TransmitByteI2C
	ldi temp,@1
	rcall TransmitByteI2C
	ldi temp,@2
	rcall TransmitByteI2C
	ldi temp,@3
	rcall TransmitByteI2C
	ldi temp,@4
	rcall TransmitByteI2C
	ldi temp,@5
	rcall TransmitByteI2C
	ldi temp,@6
	rcall TransmitByteI2C
	ldi temp,@7
	rcall TransmitByteI2C
	ldi temp,@8
	rcall TransmitByteI2C
	ldi temp,@9
	rcall TransmitByteI2C
	rcall StopBitI2C
.endmacro

.macro I2CByteRead	;ète z aktuálnì nastavené adresy v EEPROM 1 byte a vrací ho v temp registru
	rcall StartBitI2C
	ldi temp,@0
	rcall TransmitByteI2C
	ldi temp,NoACK
	rcall ReceiveByteI2C
	rcall StopBitI2C
.endmacro

;------------------------------------  Start bit pro I2C sbìrnici  ---------------------------
StartBitI2C:
	;CLK = OUT
	sbi I2CDirPort,I2CSCL
	;data = OUT
	sbi I2CDirPort,I2CSDA
	
	;clk & data = 1
	sbi I2CDataOPort,I2CSDA
	sbi I2CDataOPort,I2CSCL
	
	Wait
	
	;data = 0
	cbi I2CDataOPort,I2CSDA
	
	Wait
	
	;clk = 0
	cbi I2CDataOPort,I2CSCL
	
	Wait
	
	ret
;------------------------------------  Stop bit pro I2C sbìrnici  ---------------------------
StopBitI2C:
	;data = OUT
	sbi I2CDirPort,I2CSDA
	
	;clk = 1
	sbi I2CDataOPort,I2CSCL
	
	Wait
	
	;data = 1
	sbi I2CDataOPort,I2CSDA

	Wait
	
	ret
;------------------------------------  Vyslání bytu na I2C sbìrnici  ---------------------------
; parametry: 	vstup temp registr obsahující vysílaný byte
;		výstup temp registr obsahující ACK signál
; využítí registrù: temp + R0
TransmitByteI2C:
	;data = OUT
	sbi I2CDirPort,I2CSDA
	;kopie vysílaného bytu do r0, které se bude rotovat a vysílat
	mov r0,temp
	ldi temp,8
TransmitByteI2C_Loop:
	sbrs r0,7
		;data = 0
		cbi I2CDataOPort,I2CSDA
	sbrc r0,7
		;data = 1
		sbi I2CDataOPort,I2CSDA
	;ostrobování CLK signálem
	;clk = 1
	sbi I2CDataOPort,I2CSCL
	Wait
	;clk = 0
	cbi I2CDataOPort,I2CSCL
	
	;posun datového registru
	lsl r0
	
	dec temp
	brne TransmitByteI2C_Loop
	
	Wait
	
	;prectu si Acknowledge signál od SLAVE prvku ACK
	;data = IN
	cbi I2CDirPort,I2CSDA
	;data = 0
	cbi I2CDataOPort,I2CSDA
	
	;clk = 1
	sbi I2CDataOPort,I2CSCL
	
	;pøeètu ACK signál z SDA do tempu a vrátím ho z funkce ven
	sbis I2CDataIPort,I2CSDA
		ldi temp,0		;ACK = 0
	sbic I2CDataIPort,I2CSDA
		ldi temp,1		;ACK = 1
	Wait
	
	;clk = 0
	cbi I2CDataOPort,I2CSCL
	
	ret
;------------------------------------  Pøijmutí bytu z I2C sbìrnice  ---------------------------
; parametry: 	vstup temp registr obsahující vysílaný ACK signál
;		výstup pøíjatý byte
; využítí registrù: temp + R0 + R1
ReceiveByteI2C:
	;data = IN
	cbi I2CDirPort,I2CSDA
	;data = 0
	cbi I2CDataOPort,I2CSDA
	;kopie vysílaného ACK do r0, které se bude vysílat jako 9 bit
	mov r0,temp
	ldi temp,8
	mov r1,temp
ReceiveByteI2C_Loop:
	;ostrobování CLK signálem
	;clk = 1
	sbi I2CDataOPort,I2CSCL
	Wait
	;pøeètu data, zapíšu do temp registru 
	lsl temp
	;SDA = 0 tak se nic nedìje
	sbic I2CDataIPort,I2CSDA
		;SDA = 1
		ori temp,1
	;clk = 0
	cbi I2CDataOPort,I2CSCL
	
	dec r1
	brne ReceiveByteI2C_Loop
	
	Wait
	
	;vyslu ACK signál z r0
	;data = OUT
	sbi I2CDirPort,I2CSDA
		
	;zapíšu ACK signál z r0
	sbrs r0,0
		;ACK = 0
		;data = 0
		cbi I2CDataOPort,I2CSDA
	sbrc r0,0
		;ACK = 1
		;data = 1
		sbi I2CDataOPort,I2CSDA
	
	;clk = 1
	sbi I2CDataOPort,I2CSCL
	
	Wait
	
	;clk = 0
	cbi I2CDataOPort,I2CSCL
	ret
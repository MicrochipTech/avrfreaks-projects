
;nastaven� pro procedury, kter� pracuj� s I2C sbernic�
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

.macro I2CByteRead	;�te z aktu�ln� nastaven� adresy v EEPROM 1 byte a vrac� ho v temp registru
	rcall StartBitI2C
	ldi temp,@0
	rcall TransmitByteI2C
	ldi temp,NoACK
	rcall ReceiveByteI2C
	rcall StopBitI2C
.endmacro

;------------------------------------  Start bit pro I2C sb�rnici  ---------------------------
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
;------------------------------------  Stop bit pro I2C sb�rnici  ---------------------------
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
;------------------------------------  Vysl�n� bytu na I2C sb�rnici  ---------------------------
; parametry: 	vstup temp registr obsahuj�c� vys�lan� byte
;		v�stup temp registr obsahuj�c� ACK sign�l
; vyu��t� registr�: temp + R0
TransmitByteI2C:
	;data = OUT
	sbi I2CDirPort,I2CSDA
	;kopie vys�lan�ho bytu do r0, kter� se bude rotovat a vys�lat
	mov r0,temp
	ldi temp,8
TransmitByteI2C_Loop:
	sbrs r0,7
		;data = 0
		cbi I2CDataOPort,I2CSDA
	sbrc r0,7
		;data = 1
		sbi I2CDataOPort,I2CSDA
	;ostrobov�n� CLK sign�lem
	;clk = 1
	sbi I2CDataOPort,I2CSCL
	Wait
	;clk = 0
	cbi I2CDataOPort,I2CSCL
	
	;posun datov�ho registru
	lsl r0
	
	dec temp
	brne TransmitByteI2C_Loop
	
	Wait
	
	;prectu si Acknowledge sign�l od SLAVE prvku ACK
	;data = IN
	cbi I2CDirPort,I2CSDA
	;data = 0
	cbi I2CDataOPort,I2CSDA
	
	;clk = 1
	sbi I2CDataOPort,I2CSCL
	
	;p�e�tu ACK sign�l z SDA do tempu a vr�t�m ho z funkce ven
	sbis I2CDataIPort,I2CSDA
		ldi temp,0		;ACK = 0
	sbic I2CDataIPort,I2CSDA
		ldi temp,1		;ACK = 1
	Wait
	
	;clk = 0
	cbi I2CDataOPort,I2CSCL
	
	ret
;------------------------------------  P�ijmut� bytu z I2C sb�rnice  ---------------------------
; parametry: 	vstup temp registr obsahuj�c� vys�lan� ACK sign�l
;		v�stup p��jat� byte
; vyu��t� registr�: temp + R0 + R1
ReceiveByteI2C:
	;data = IN
	cbi I2CDirPort,I2CSDA
	;data = 0
	cbi I2CDataOPort,I2CSDA
	;kopie vys�lan�ho ACK do r0, kter� se bude vys�lat jako 9 bit
	mov r0,temp
	ldi temp,8
	mov r1,temp
ReceiveByteI2C_Loop:
	;ostrobov�n� CLK sign�lem
	;clk = 1
	sbi I2CDataOPort,I2CSCL
	Wait
	;p�e�tu data, zap�u do temp registru 
	lsl temp
	;SDA = 0 tak se nic ned�je
	sbic I2CDataIPort,I2CSDA
		;SDA = 1
		ori temp,1
	;clk = 0
	cbi I2CDataOPort,I2CSCL
	
	dec r1
	brne ReceiveByteI2C_Loop
	
	Wait
	
	;vyslu ACK sign�l z r0
	;data = OUT
	sbi I2CDirPort,I2CSDA
		
	;zap�u ACK sign�l z r0
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
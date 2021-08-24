;
; DS1307withmenu4.asm
;
; Created: 7/24/2021 9:46:00 PM
; Author : pappan
;press menu button for a moment to enter menu function
;press next button to change values of of the selected/displayed option (values increment and start from begining)
;press save button to save selected value , the next option will be automatically loaded one by one (hour,min,day,month,date,year > time)
;PA2 next button active 0 idle 1
;PA1 save button active 0 idle 1
;PA3 menu button active 0 idle 1

.equ data_command1 = 0b00001001		; data control nibble ,led on P3, EN 0 on P2, R/W 0 (write) in P1 , RS 1 (0 instruction, 1 data) = 1001  =0x09
.equ data_command2 = 0b00001101		; data control nibble , 1101  = 0x0D   - EN goes hi=1
.equ data_command3 = 0b11111011		; data control nibble , 1001  = 0x09   - EN goes low=0
.equ inst_command1 = 0b00001000		;instruction control nibble ,  led on en-lo,Rw-0,rs =0   = 1000   = 0x08
.equ inst_command2 = 0b00001100		;instruction control nibble ,  led on,EN hi , rs/RW 0    = 1100   = 0x0C
.equ inst_command3 = 0b11111011		;instruction control nibble  , led on, EN lo ,rs/rw 0    = 1000   = 0x08
.EQU SLAVE_ADDRESSW = 0X4E			; OLED = 0X78 ,1602 =4E
.equ SLAVE_ADDRESSR = 0x4F			; 1602 adress + read command
.equ fclk = 10000000
.equ DS1307WAD = 0xD0
.equ DS1307RAD = 0xD1
.equ read_data_len = 8
.DEF  SLAVE_REG = R17
.DEF  TEMP = R16
.def data = r3

.macro micros					; macro for delay in us
ldi temp,@0
rcall delayTx1uS
.endm

.macro millis					; macro for delay in ms
ldi YL,low(@0)
ldi YH,high(@0)
rcall delayYx1mS
.endm

.macro pos						; macro for sending cursor position to LCD 
ldi r16,@1
mov r6,r16
ldi r16,@0
rcall posi
.endm

.macro arrayread				; macro for sending a string to LCD
ldi ZL,low(2*@0)
ldi ZH,high(2*@0)
rcall array
.endm

.dseg
BUFFER: .byte 8
PAD: .BYTE 1
PAD1: .byte 1
HOUR1stdigit: .byte 1
HOUR2nddigit: .byte 1
COLON1: .byte 1
MINUTES1stdigit: .byte 1 
MINUTES2nddigit: .byte 1
COLON2: .byte 1
SECOND1stdigit: .byte 1
SECOND2nddigit: .byte 1
BLANK: .byte 1
DATE1stdigit: .byte 1
DATE2nddigit: .byte 1
SLASH1: .byte 1
MONTH1stdigit: .byte 1
MONTH2nddigit: .byte 1
SLASH2: .byte 1
YEAR1stdigit: .byte 1
YEAR2nddigit: .byte 1
SLASH3: .byte 1
DAY1: .byte 1
DAY2: .byte 1
PERIOD: .byte 1
minbcd: .byte 1
hourbcd: .byte 1
weekbcd: .byte 1
datebcd: .byte 1
monbcd: .byte 1
yearbcd: .byte 1
smachine: .byte 1

.cseg
.ORG 0X00
rjmp reset
.org 0x03
rjmp PORTA_ISR

PORTA_ISR:
push r16
lds r16,CPU_SREG
push r16
ldi r16,0x08
sts PORTA_INTFLAGS,r16
ldi r16,0x01
sts smachine,r16
pop r16
out cpu_sreg,r16
pop r16
reti

reset:
rcall PROT_WRITE					; changes clock speed to 10mhz
rcall TWI_INIT						; initialize TWI
LDI SLAVE_REG,SLAVE_ADDRESSW		; LCD write address is transmitted
rcall TWI_START						; TWI start routine
rcall LCD_INIT						; LCD initialization routine
pos 0 , 0							; cursor on the first line left most position
arrayread message					; displays message "hello"
millis 1000							; delay 1000 ms
rcall TWI_STOP						; stop TWI transmission to LCD




main:
	ldi r16,0b00001110
	sts PORTA_DIRCLR,r16			; PA1,PA2,PA3 becomes inputs
	ldi r16,0b00001101				; initialise PORTA pin3 pullup resistor and pin control for low sense 00001101
	sts PORTA_PIN3CTRL,r16
	ldi r16,0b00001000				; enable pullup on PA2,PA3 for buttons ,idle 1 , active 0
	sts PORTA_PIN2CTRL,r16
	sts PORTA_PIN1CTRL,r16
	sei								; enable global interrupts
SM:
    lds r16,smachine				; copy smachine to r16
	sbrc r16,0						; skip next instruction if bit 0 is cleared , menu flag is set(0x01) if menu button is depressed
    rjmp select						; if menu flag is set jump to label "select" for time adjustment
SMret:
	rcall measure					; if flag not set in smachine call measure function to read current time
	rjmp SM							; repeat the loop

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;below routine reads time from rtc and displays it on LCD
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

measure:
rcall inforead						; routine to read DS1307 time registers
rcall BCD_TO_ASCII					; routine to convert the BCD values of RTC to ASCII for LCD
rcall TWI_INIT						; initialize TWI for LCD 
LDI SLAVE_REG,SLAVE_ADDRESSW
rcall TWI_START
rcall NO1line						; calls pos 0,0 macro routine for 1 line in LCD
rcall update1						; looped procedure to read SRAM time address hour,min & seconds (to reduce code size)
pos 1, 0							; macro to position LCD cursor on 2nd line 1st position
rcall update2						; looped procedure to read SRAM month,date,year and day address(reduced code size)
rcall TWI_STOP						; stop TWI transmission
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;PROTECTED WRITE for processor speed
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PROT_WRITE:
		ldi r16,0Xd8
		out CPU_CCP,r16
		ldi r16,0x01						; clk prescaler of 2, 20Mhz/2 = 10Mhz
		STS CLKCTRL_MCLKCTRLB,R16
		RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;I2C ROUTINES
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


TWI_INIT:
		ldi r16,80
		sts TWI0_MBAUD,R16
		LDI R16,0b00000011			;SMEN,ENABLE
		STS TWI0_MCTRLA,R16
		LDI R16,0b00001000			;FLUSH ADDR & DATA REGISTERS
		STS TWI0_MCTRLB,R16
		LDI R16,0b00000001			;FORCE IDLE
		STS TWI0_MSTATUS,R16
		ret
		


TWI_START:
		MOV R16,SLAVE_REG			;SLAVE_REG IS R17, READ OR WRITE ADDRESS SHOULD BE LOADED HERE PRIOR TO CALL
		STS TWI0_MADDR,R16
		RCALL WAIT_WIF
		rcall CHECKACK				;checks whether slave has acked
		ret

TWI_WRITE:
		MOV R16,SLAVE_REG
		STS TWI0_MDATA,R16
		RCALL WAIT_WIF
		rcall CHECKACK
		micros 10
		ret

CHECKACK:
		clr r19
	ACK:
		inc r19
		cpi r19,30
		breq NOACK
		LDS R16,TWI0_MSTATUS
		SBRC R16,4
		rjmp ACK
	NOACK:	RET



TWI_READ:
		ldi SLAVE_REG,DS1307WAD
		rcall TWI_START
		ldi SLAVE_REG,0x00			; send instruction/READ_ADDRESS to THE SLAVE FROM WHICH DATA IS READ ,first register of DS1307 is 0x00
		rcall TWI_WRITE
		ldi r16,0x00				;loading 0 in ACKACT bit enables master to send ack after reading data register
		sts TWI0_MCTRLB,r16
		ldi r16,DS1307RAD			; repeated start ;  I2C slave address + read bit (1) SHOULD BE LOADED HERE FOR READING DATA FROM SLAVE READ_ADDRESS GIVEN ABOVE
		STS TWI0_MADDR,R16
		rcall WAIT_RIF

		ldi r16,read_data_len		;load r16 with number of bytes to be read
		cpi r16,0x02				;is num of bytes less than or greater than 2
		brlo BYYTE					;if less than 2 branch to 1BYTE as NACK+STOP will be loaded prior to read
		dec r16						; decreace one count from the total count to get loop value,NACK should be sent before the last byte read
		mov r5,r16					; move the count -1 value to counter r5
loop_read:
		LDS R16,TWI0_MDATA			;MDATA REGISTER IS COPIED TO R16,DATA IS RECIVED INTO MDATA FROM SLAVE
		ST X+,R16					;DATA IN R16 IS STORED IN SRAM BUFFER FOR LATER USE. 
		RCALL WAIT_RIF				;wait for read flag
		dec r5						;decrease counter after each read
		brne loop_read				;go throug loop till {count - 1} is finished
BYYTE: 
		LDI R16,0b00000111			;CLEAR ACKACT BIT BEFORE READING LAST BYTE AND ISSUE A STOP = NACK+STOP
		STS TWI0_MCTRLB,R16
		LDS R16,TWI0_MDATA			;MDATA REGISTER IS COPIED TO R16,THIS THE LAST DATA IS RECEIVED  FROM SLAVE
		ST X+ ,R16					;DATA IN R16 IS STORED IN SRAM BUFFER FOR LATER USE. 
		RET


TWI_STOP:
		LDI R16,0b00000011          ;STOP
		STS TWI0_MCTRLB,R16
		RET


WAIT_WIF:
		LDS R16,TWI0_MSTATUS
		SBRS R16,6					;CHECK WIF IS SET,IF SET SKIP NEXT INSTRUCTION (write interrupt flag)
		RJMP WAIT_WIF
		RET


WAIT_RIF:
		LDS R16,TWI0_MSTATUS
		SBRS R16,7					;CHECK RIF IS SET,IF SET SKIP NEXT INSTRUCTION (read interrupt flag)
		RJMP WAIT_RIF
		RET


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;LCD ROUTINES for sending commands and data
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
COMMAND_WRITE:
		STS PAD,R17					;copy SLAVE_REG to SRAM address PAD for processing
		ANDI R17,0XF0				;preserve upper nibble of SLAVE_REG by anding with 0xF0 ,lower 4 becomes 0
		ORI R17,inst_command1		;add instruction_command1 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE				;call TWI_WRITE routine to transmit command data to LCD
		rcall twentyms
		ORI R17,inst_command2		;add instruction_command2 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE				;call TWI_WRITE routine to transmit command data to LCD
		rcall twentyms
		ANDI R17,inst_command3		;add instruction_command3 to lower nibble of r17 by OR ing it
		RCALL TWI_WRITE				;call TWI_WRITE routine to transmit command data to LCD
		rcall twentyms		
		LDS R17,PAD					;copy back data from PAD to r17 for processing the remaining lower nibble
		SWAP R17					;swap the nibbles in R17 so thlower nibble will occupy the upper area of reg
		ANDI R17,0XF0				;preserve upper nibble of SLAVE_REG by anding with 0xF0
		ORI R17,inst_command1		;add instruction_command1 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE				;call TWI_WRITE routine to transmit command data to LCD
		rcall twentyms
		ORI R17,inst_command2		;add instruction_command2 to lower nibble of r17 by OR ing it
		RCALL TWI_WRITE				;call TWI_WRITE routine to transmit command data to LCD
		rcall twentyms
		ANDI R17,inst_command3		;add instruction_command3 to lower nibble of r17 by OR ing it
		RCALL TWI_WRITE				;call TWI_WRITE routine to transmit command data to LCD
		rcall twentyms
		RET	


DATA_WRITE:
		STS PAD,R17					;copy SLAVE_REG to SRAM address PAD for processing
		ANDI R17,0XF0				;preserve upper nibble of SLAVE_REG by anding with 0xF0,lower 4 becomes 0
		ORI R17,data_command1		;add data_command1 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE				;call TWI_WRITE routine to transmit command data to LCD
		rcall twentyms
		ORI R17,data_command2		;add data_command2 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE				;call TWI_WRITE routine to transmit command data to LCD
		rcall twentyms
		ANDI R17,data_command3		;add data_command3 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE				;call TWI_WRITE routine to transmit command data to LCD
		rcall twentyms
		LDS R17,PAD					;copy back data from PAD to r17 for processing the remaining lower nibble
		SWAP R17					;swap the nibbles in R17 so thlower nibble will occupy the upper area of reg
		ANDI R17,0XF0				;preserve upper nibble of SLAVE_REG by anding with 0xF0
		ORI R17,data_command1		;add data_command1 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE				;call TWI_WRITE routine to transmit command data to LCD
		rcall twentyms
		ORI R17,data_command2		;add data_command2 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE				;call TWI_WRITE routine to transmit command data to LCD
		rcall twentyms
		ANDI R17,data_command3		;add data_command3 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE				;call TWI_WRITE routine to transmit command data to LCD
		rcall twentyms
		RET	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Hitachi LCD 1602 initialisation procedure
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

LCD_INIT:
		rcall fiftyms				; call 50ms prewritten code with 50ms macro to reduce code size due to repetition of macro
		LDI R17,0b00111100
		RCALL TWI_WRITE
		rcall twentyms				; call 20ms prewritten code with 20ms macro to reduce code size due to repetition of macro
		LDI R17,0B00111000
		RCALL TWI_WRITE
		rcall fiftyms				; call 50ms prewritten code with 50ms macro to reduce code size due to repetition of macro
 		

		
		LDI R17,0b00111100
		RCALL TWI_WRITE
		rcall twentyms				; call 20ms prewritten code with 20ms macro to reduce code size due to repetition of macro
		LDI R17,0B00111000
		RCALL TWI_WRITE
		rcall fiftyms				; call 50ms prewritten code with 50ms macro to reduce code size due to repetition of macro
 		

		
		LDI R17,0b00111100
		RCALL TWI_WRITE
		rcall twentyms				; call 20ms prewritten code with 20ms macro to reduce code size due to repetition of macro
		LDI R17,0B00111000
		RCALL TWI_WRITE
		rcall fiftyms				; call 50ms prewritten code with 50ms macro to reduce code size due to repetition of macro
 		

		
		LDI R17,0b00101100
		RCALL TWI_WRITE
		rcall twentyms				; call 20ms prewritten code with 20ms macro to reduce code size due to repetition of macro
		LDI r17,0b00101000
		RCALL TWI_WRITE
		rcall fiftyms				; call 50ms prewritten code with 50ms macro to reduce code size due to repetition of macro
 		

		
		LDI R17,0b00101000
		RCALL COMMAND_WRITE
		rcall twentyms				; call 20ms prewritten code with 20ms macro to reduce code size due to repetition of macro

		
		LDI R17,0b00001100
		RCALL COMMAND_WRITE
		rcall twentyms				; call 20ms prewritten code with 20ms macro to reduce code size due to repetition of macro

		
		LDI R17,0b00000110
		RCALL COMMAND_WRITE
		rcall twentyms				; call 20ms prewritten code with 20ms macro to reduce code size due to repetition of macro

		
		LDI R17,0b00000001
		RCALL COMMAND_WRITE
		rcall twentyms				; call 20ms prewritten code with 20ms macro to reduce code size due to repetition of macro
		RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;the below routines are for recurring delays used in the program
;using macros will increase code size so created these subroutines to be called
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

fiftyms:
millis 50
ret

twentyms:
micros 20
ret

twotoums:                           
millis 2000
ret	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; below routines used part of macro pos and macro arrayread for cursor position and reading array messages
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

posi:
		cpi r16,0x00					;check the first parameter of macro is 0 or a higher number
		breq line1						;if zero go to label first line
		ldi r16,0xc0					; if not zero the input is for 2nd line. load 0xC0 in r16 which is address of 2nd line first position DDRAM
		add r6,r16						; add with horizontal postion on 2nd line to get the correct DDRAM address
		mov SLAVE_REG,r6				; copy new LCD DDRAM address to SLAVE_REG
		rcall COMMAND_WRITE				; call TWI transmit command
		ret
	line1:
		ldi r16,0x80					; if Y = 0 which means 1st line of LCD load address of 1st line 1st position DDRAM =0x80
		add r6,r16						; add 0x80 to X position saved in r6 to get the start postion on 1st line
		mov SLAVE_REG,r6				; copy new LCD DDRAM address to SLAVE_REG
		rcall COMMAND_WRITE				; call TWI transmit command
		ret


array:
		lpm SLAVE_REG,Z+				;load from program memory to r17 data pointed by Z pointer
		cpi SLAVE_REG,0					; check for null terminator in the string
		breq exit						; if zero go to exit
		rcall DATA_WRITE				; transmit copied data to LCD via TWI
		rjmp array						; jump back to array until null
	exit: 	ret


NO1line:                            ; subroutine created for cursor to pos 0,0 as using macro will increase code size
pos 0, 0
ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




inforead:					; function to read time registers to buffer
ldi XL,low(BUFFER)			; setting pointer to buffer
ldi XH,high(BUFFER)
ldi SLAVE_REG,DS1307WAD		; DS1307 write address
rcall TWI_START				; issues start and sends write address
ldi SLAVE_REG,0x00     		; DS1307 has its time register addresses starting from 0x00
rcall TWI_WRITE				; function used to transmit above data
rcall TWI_READ				; function to read DS1307 registers
ret




BCD_TO_ASCII:			; routine to convert BCD to ASCII and store in appropriate registers
ldi XL,low(BUFFER)		; pointer to the start address of buffer
ldi XH,high(BUFFER)
ld r16,X+
sts PAD1,r16			; copy seconds data to SRAM address PAD1 for temprerory storage while processing
swap r16				; 
andi r16,0x3f			; AND with 0b00111111 and only swapped seconds data remains ,tenth position is in lower nibble and unit position in higher nibble
ori r16,0x30			; OR with ascii for 0 which is 0x30 (0b00110000) , higher nibble is invalidated and only lower nibble which is 10th seconds gets converted to ascii
sts SECOND1stdigit,r16	; store the 10th seconds ascii digit in SRAM location SECOND1stdigit			
lds r16,PAD1			; copy previously stored seconds data from PAD1
andi r16,0x3f			; AND with 0b00111111 and seconds data remains with higher nibble as 10th place and lower nibble as unit place
ori r16,0x30			; OR the above value with ASCII0 (0b00110000) 0x30, upper nibble becomes 0x3 and lower nibble will remain as it is, which is unit part of seconds data
sts SECOND2nddigit,r16	; copy his ASCII unit seconds value to SECOND2nddigit in SRAM
ldi r16,' '				; load ascii blank   in r16
sts BLANK,r16			; store colon in SRAM location BLANK which will be used to print on LCD after the seconds digits


ld r16,X+
sts PAD1,r16			; copy minutes data to SRAM address PAD1 for temprerory storage while processing
swap r16				; 
andi r16,0x3f			; AND with 0b00111111 and only swapped minutes data remains ,tenth position is in lower nibble and unit position in higher nibble
ori r16,0x30			; OR with ascii for 0 which is 0x30 (0b00110000) , higher nibble is invalidated and only lower nibble which is 10th minutes gets converted to ascii
sts MINUTES1stdigit,r16	; store the 10th minutes ascii digit in SRAM location MINUTES1stdigit			
lds r16,PAD1			; copy previously stored minutes data from PAD1
andi r16,0x3f			; AND with 0b00111111 and minutes data remains with higher nibble as 10th place and lower nibble as unit place
ori r16,0x30			; OR the above value with ASCII0 (0b00110000) 0x30, upper nibble becomes 0x3 and lower nibble will remain as it is, which is unit part of minutes data
sts MINUTES2nddigit,r16	; copy his ASCII unit minutes value to MINUTES2nddigit in SRAM
ldi r16,':'				; load ascii :  (colon) in r16
sts COLON2,r16			; store colon in SRAM location COLON1 which will be used to print on LCD after the minutes digits



ld r16,X+
sts PAD1,r16			; copy hour data to SRAM address PAD1 for temprerory storage while processing
swap r16				; 
andi r16,0x3f			; AND with 0b00111111 and only swapped hour data remains ,tenth position is in lower nibble and unit position in higher nibble
ori r16,0x30			; OR with ascii for 0 which is 0x30 (0b00110000) , higher nibble is invalidated and only lower nibble which is 10th hour gets converted to ascii
sts HOUR1stdigit,r16	; store the 10th hour ascii digit in SRAM location HOUR1stdigit			
lds r16,PAD1			; copy previously stored hour data from PAD1
andi r16,0x3f			; AND with 0b00111111 and hour data remains with higher nibble as 10th place and lower nibble as unit place
ori r16,0x30			; OR the above value with ASCII0 (0b00110000) 0x30, upper nibble becomes 0x3 and lower nibble will remain as it is, which is unit part of hour data
sts HOUR2nddigit,r16	; copy his ASCII unit hour value to HOUR2nddigit in SRAM
ldi r16,':'				; load ascii :  (colon) in r16
sts COLON1,r16			; store colon in SRAM location COLON1 which will be used to print on LCD after the hour digits




ld r16,X+
andi r16,0x07			; AND with 0b00000111 so that only lower 3 bits remain , 1 monday to 7 sunday
add r16,r16
ldi r30,low(2*day)		; initiate Z pointer to address DAY which holds the alphabetical representation of week days 1-7
ldi r31,high(2*day)
clr r18					; clear r18 for carry propagation
clc						; clear carry in sreg
add r30,r16				; add low register of Z pointer with value in r16 (day 1-7)to determine the address of the day string
adc r31,r18				; add with carry the high register of Z pointer
lpm r18,Z+				; load from program memory ASCII value of first letter that indicates the day stored in the new address after above calculation
sts DAY1,r18			; store in SRAM address DAy1 the first alphabet to be dispalyed in LCD
lpm r18,Z+				; Z incremented earlier now points to next address of the 2nd alphaber of the week day (MO,Tu,WE,TH,FR,SA,SU)
sts DAY2,r18			; store in SRAM address DAY2 the 2nd alphabet to be displayed on the lcd
ldi r16,'.'				; load ascii period in r16 to be displayed after day
sts PERIOD,r16 			; store the ascii period in SRAM adress PERIOD



ld r16,X+
sts PAD1,r16			; copy date data to SRAM address PAD1 for temprerory storage while processing
swap r16				; 
andi r16,0x3f			; AND with 0b00111111 and only swapped date data remains ,tenth position is in lower nibble and unit position in higher nibble
ori r16,0x30			; OR with ascii for 0 which is 0x30 (0b00110000) , higher nibble is invalidated and only lower nibble which is 10th date gets converted to ascii
sts DATE1stdigit,r16	; store the 10th date ascii digit in SRAM location DATE1stdigit			
lds r16,PAD1			; copy previously stored date data from PAD1
andi r16,0x3f			; AND with 0b00111111 and date data remains with higher nibble as 10th place and lower nibble as unit place
ori r16,0x30			; OR the above value with ASCII0 (0b00110000) 0x30, upper nibble becomes 0x3 and lower nibble will remain as it is, which is unit part of date data
sts DATE2nddigit,r16	; copy this ASCII unit date value to DATE2nddigit in SRAM
ldi r16,'/'				; load ascii /   in r16
sts SLASH1,r16			; store / in SRAM location BLANK which will be used to print on LCD after the date digits




ld r16,X+
sts PAD1,r16			; copy month data to SRAM address PAD1 for temprerory storage while processing
swap r16				; 
andi r16,0x3f			; AND with 0b00111111 and only swapped month data remains ,tenth position is in lower nibble and unit position in higher nibble
ori r16,0x30			; OR with ascii for 0 which is 0x30 (0b00110000) , higher nibble is invalidated and only lower nibble which is 10th month gets converted to ascii
sts MONTH1stdigit,r16	; store the 10th month ascii digit in SRAM location MONTH1stdigit			
lds r16,PAD1			; copy previously stored month data from PAD1
andi r16,0x3f			; AND with 0b00111111 and month data remains with higher nibble as 10th place and lower nibble as unit place
ori r16,0x30			; OR the above value with ASCII0 (0b00110000) 0x30, upper nibble becomes 0x3 and lower nibble will remain as it is, which is unit part of month data
sts MONTH2nddigit,r16	; copy this ASCII unit date value to MONTH2nddigit in SRAM
ldi r16,'/'				; load ascii /   in r16
sts SLASH2,r16			; store / in SRAM location BLANK which will be used to print on LCD after the month digits




ld r16,X+
sts PAD1,r16			; copy year data to SRAM address PAD1 for temprerory storage while processing
swap r16				; 
andi r16,0x3f			; AND with 0b00111111 and only swapped year data remains ,tenth position is in lower nibble and unit position in higher nibble
ori r16,0x30			; OR with ascii for 0 which is 0x30 (0b00110000) , higher nibble is invalidated and only lower nibble which is 10th year gets converted to ascii
sts YEAR1stdigit,r16	; store the 10th year ascii digit in SRAM location YEAR1stdigit			
lds r16,PAD1			; copy previously stored year data from PAD1
andi r16,0x3f			; AND with 0b00111111 and year data remains with higher nibble as 10th place and lower nibble as unit place
ori r16,0x30			; OR the above value with ASCII0 (0b00110000) 0x30, upper nibble becomes 0x3 and lower nibble will remain as it is, which is unit part of year data
sts YEAR2nddigit,r16	; copy this ASCII unit date value to YEAR2nddigit in SRAM
ldi r16,' '				; load ascii ' '   in r16
sts SLASH3,r16			; store  ' ' in SRAM location BLANK which will be used to print on LCD after the year digits

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



day:
.db 0x00,0x00,"M","O","T"."U","W","E","T","H","F","R","S","A","S","U"

number:
.db 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40,0x41,0x42,0x43,0x45,0x46,0x47,0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x00
message:
.db "Hello! ",0 
message1:
.db "adj hour ",0
message2:
.db "adj min ",0
message3:
.db "adj day ",0
message4:
.db "adj month ",0
message5:
.db "adj year ",0
message6:
.db "adj date ",0



hex_ascii:          ; routine to convert BCD values selected for time adjustment to be displayed on LCD
rcall NO1line		; subroutine for cursor placement on 1st line 1st position
lpm r20,Z+			; load from program memory the first value from  "number" label
mov r17,r20			; copy to r17 which is used to transmit over twi
andi r17,0xf0		; upper nibble preserved
swap r17			; swap nibble of the BCD l;oaded from table
ori r17,0x30		; OR with 0x30 to convert to ASCII
rcall DATA_WRITE	; the ASCII value will be the first number of the BCD that will be displayed
mov r17,r20			; copy again the BCD for the second digit to be processed to ascii
andi r17,0x0f		; lower nibble preserved
ori r17,0x30		; or with 0x30 to convert to ASCII
rcall DATA_WRITE	; transmit over TWI, as lcd pointer is increased automatically the 2nd digit is displayed
ret

	





clear_screen:		; routine to clear LCD screen and reposition cursor to 0,0
LDI R17,0b00000001
RCALL COMMAND_WRITE	; call routine to transmit LCD commands
micros 20
RET


select:						; time adjustment routine
	rcall TWI_INIT
	LDI SLAVE_REG,SLAVE_ADDRESSW
	rcall TWI_START
	rcall clear_screen		; routine to clear LCD screen and reposition cursor to 0,0
	rcall NO1line			; subroutine for cursor placement on 1st line 1st position
	arrayread message1		; macro array read for displaying array "adj hour"
	rcall twotoums			; subroutine for 2000ms
	rcall clear_screen		; subroutine to clear contents on the LCD screen
	rcall NO1line			; subroutine for cursor placement on 1st line 1st position
	clr r20
hour:						; ajust hour routine
	ldi r18,24				; counter for hours 24
	ldi ZL,low(2*number)	; set pointer to address "number" which has all possible BCD values needed
	ldi ZH,high(2*number)
	
			
scroll_hour:
	rcall hex_ascii			; table value will be displayed in LCD
	dec r18					; decrease counter ,here 24 hours so 24 steps
scan_button:				; polls save button 
	lds r16,PORTA_IN		; copy PORTA pin status/values
	andi r16,0x02			; and with 0x02 for PA1 . if 0 button pressed if 0x02 button idle
	sbrs r16,1				; if idle (0x02) skip next instruction which is save th current number as hour
	rjmp save_hour			; if button pressed jump to save routine
scan_next:					; polls next button (to increment value)
	lds r16,PORTA_IN		; copy PORTA pin status/values
	andi r16,0x04			; and with 0x04 for PA2 . if 0 button pressed if 0x04 button idle
	sbrc r16,2				; if 0 (pressed) skip next instruction else loop until either save or next button is pressed
	rjmp scan_button		; keep polling both buttons until either one is pressed
	cpi r18,0x00			; check counter is 0, if zero start from begining (0-24 hours)
	breq hour				; if counter 0 jump to hour label to reset pointer to begining
	millis 250				; delay for visual and human needs
	rjmp scroll_hour		; next value loaded to LCD for decision making
save_hour:
	rcall twotoums			; delay for visual needs ; subroutine for 2000ms
	;subi r20,1				;
	sts hourbcd,r20			; store the selected hour BCD value in SRAM reserved location "hourbcd".later from here will be copied to RTC register.
	rcall clear_screen		; clear screen of LCD for next option to be displayed
	rjmp adjminute			; jump to next option which is ajustment of minutes



adjminute:					; adjust minute routine
	rcall NO1line			; subroutine for cursor placement on 1st line 1st position
	arrayread message2		; macro array read for displaying array "adj minute"
	rcall twotoums			; subroutine for 2000ms
	rcall clear_screen		; routine to clear LCD screen and reposition cursor to 0,0
	
minute:
	ldi r18,60				; 60 minutes
	ldi ZL,low(2*number)
	ldi ZH,high(2*number)
;	lpm r20,Z+
scroll_minute:
	rcall hex_ascii			; routine to convert BCD values selected for time adjustment to be displayed on LCD
	dec r18
scan_button1:				; polls save button 
	lds r16,PORTA_IN
	andi r16,0x02
	sbrs r16,1
	rjmp save_minute
scan_next1:					; polls next button (to increment value)
	lds r16,PORTA_IN
	andi r16,0x04
	sbrc r16,2
	rjmp scan_button1
	cpi r18,0x00
	breq minute
	millis 250
	rjmp scroll_minute
save_minute:
	rcall twotoums			; subroutine for 2000ms
;	subi r20,1
	sts minbcd,r20
	rcall clear_screen		; routine to clear LCD screen and reposition cursor to 0,0
	rjmp adjday



adjday:						; adjust day of the week routine
	rcall NO1line			; subroutine for cursor placement on 1st line 1st position
	arrayread message3		; macro array read for displaying array "adj day"
	rcall twotoums			; subroutine for 2000ms
	rcall clear_screen		; routine to clear LCD screen and reposition cursor to 0,0
	
	
daye:
	ldi r18,8
	ldi ZL,low(2*number)
	ldi ZH,high(2*number)
;	lpm r20,Z+
scroll_day:
	rcall hex_ascii			; routine to convert BCD values selected for time adjustment to be displayed on LCD
	dec r18
scan_button2:				; polls save button 
	lds r16,PORTA_IN
	andi r16,0x02
	sbrs r16,1
	rjmp save_day
scan_next2:					; polls next button (to increment value)
	lds r16,PORTA_IN
	andi r16,0x04
	sbrc r16,2
	rjmp scan_button2
	cpi r18,0x00
	breq daye
	millis 250
	rjmp scroll_day
save_day:
	rcall twotoums			; subroutine for 2000ms
;	subi r20,1
	sts weekbcd,r20
	rcall clear_screen		; routine to clear LCD screen and reposition cursor to 0,0
	rjmp adjmonth



adjmonth:					; adjust month routine
	rcall NO1line			; subroutine for cursor placement on 1st line 1st position
	arrayread message4		; macro array read for displaying array "adj month"
	rcall twotoums			; subroutine for 2000ms
	rcall clear_screen		; routine to clear LCD screen and reposition cursor to 0,0
	
	
month:
	ldi r18,13
	ldi ZL,low(2*number)
	ldi ZH,high(2*number)
;	lpm r20,Z+
scroll_month:
	rcall hex_ascii			; routine to convert BCD values selected for time adjustment to be displayed on LCD
	dec r18
scan_button3:				; polls save button 
	lds r16,PORTA_IN
	andi r16,0x02
	sbrs r16,1
	rjmp save_month
scan_next3:					; polls next button (to increment value)
	lds r16,PORTA_IN
	andi r16,0x04
	sbrc r16,2
	rjmp scan_button3
	cpi r18,0x00
	breq month
	millis 250
	rjmp scroll_month
save_month:
	rcall twotoums			; subroutine for 2000ms
;	subi r20,1
	sts monbcd,r20
	rcall clear_screen		; routine to clear LCD screen and reposition cursor to 0,0
	rjmp adjyear



adjyear:					; adjust year routine
	rcall NO1line			; subroutine for cursor placement on 1st line 1st position
	arrayread message5		; macro array read for displaying array "adj year"
	rcall twotoums			; subroutine for 2000ms
	rcall clear_screen		; routine to clear LCD screen and reposition cursor to 0,0
	
	
year:
	ldi r18,60
	ldi ZL,low(2*number)
	ldi ZH,high(2*number)
;	lpm r20,Z+
scroll_year:
	rcall hex_ascii			; routine to convert BCD values selected for time adjustment to be displayed on LCD
	dec r18
scan_button4:				; polls save button 
	lds r16,PORTA_IN
	andi r16,0x02
	sbrs r16,1
	rjmp save_year
scan_next4:					; polls next button (to increment value)
	lds r16,PORTA_IN
	andi r16,0x04
	sbrc r16,2
	rjmp scan_button4
	cpi r18,0x00
	breq year
	millis 250
	rjmp scroll_year
save_year:
	rcall twotoums			; subroutine for 2000ms
;	subi r20,1
	sts yearbcd,r20
	rcall clear_screen		; routine to clear LCD screen and reposition cursor to 0,0
	rjmp adjdate



adjdate:					; adjust date routine
	rcall NO1line			; subroutine for cursor placement on 1st line 1st position
	arrayread message6		; macro array read for displaying array "adj date"
	rcall twotoums			; subroutine for 2000ms
	rcall clear_screen		; routine to clear LCD screen and reposition cursor to 0,0
	
	
date:
	ldi r18,32
	ldi ZL,low(2*number)
	ldi ZH,high(2*number)
;	lpm r20,Z+
scroll_date:
	rcall hex_ascii			; routine to convert BCD values selected for time adjustment to be displayed on LCD
	dec r18
scan_button5:				; polls save button 
	lds r16,PORTA_IN
	andi r16,0x02
	sbrs r16,1
	rjmp save_date
scan_next5:					; polls next button (to increment value)
	lds r16,PORTA_IN
	andi r16,0x04
	sbrc r16,2
	rjmp scan_button5
	cpi r18,0x00
	breq date
	millis 250
	rjmp scroll_date
save_date:
	rcall twotoums			; subroutine for 2000ms
;	subi r20,1
	sts datebcd,r20
	rcall clear_screen		; routine to clear LCD screen and reposition cursor to 0,0
	ldi r16,0x00			; load 0 in r16 to clear smachine flag , if flag set menu function will not exit
	sts smachine,r16		; clear smachine menu flag
	rjmp clockwrite			; as all data has been modified and saved in SRAM ,go to clockwrite routine to write changes to DS1307



clockwrite:					; routine to copy adjusted time from SRAM to DS1307
	rcall TWI_INIT
	ldi SLAVE_REG,DS1307WAD	; I2C address for writing of DS1307 RTC chip
	rcall TWI_START
	ldi SLAVE_REG,0x00     	; DS1307 has its time register addresses staring from 0x00
	rcall TWI_WRITE
	ldi SLAVE_REG,0x80		; bit 7 is CH bit if written 1 clock is stopped. if clock not stopped before write doesnt update
	rcall TWI_WRITE
	rcall TWI_STOP
	rcall TWI_INIT
	ldi SLAVE_REG,DS1307WAD	; I2C address for writing of DS1307 RTC chip , reloading this will set to DS1307 pointer to start
	rcall TWI_START
	ldi SLAVE_REG,0x00     	; DS1307 has its time register addresses staring from 0x00 auto incremented
	rcall TWI_WRITE
	ldi SLAVE_REG,0x00		; 00 seconds to senconds register
	rcall TWI_WRITE
	rcall loopwrite			; subroutine to copy values from consecutive SRAM address minbcd to yearbcd and write it to ds1307 rtc
	rcall TWI_STOP
	rjmp SM					; jump to main routine as time adjustment has finished





		
			
loopwrite:	ldi r20,6				; subroutine to copy values from consecutive SRAM address minbcd to yearbcd and write it to ds1307 rtc
			ldi ZL,low(minbcd)		; 
			ldi ZH,high(minbcd)
loopwrite1:	ld SLAVE_REG,Z+
			rcall TWI_WRITE
			dec r20
			brne loopwrite1
			ret


update1:ldi r20,9					; subroutine to copy values in SRAM time register and write to #1 line of LCD
			ldi ZL,low(HOUR1stdigit)
			ldi ZH,high(HOUR1stdigit)
updateloop1:ld SLAVE_REG,Z+
			rcall DATA_WRITE
			dec r20
			brne updateloop1
			ret

update2:ldi r20,12					; subroutine to copy values in SRAM date/month/year/week register and write to #2 line of LCD
			ldi ZL,low(DATE1stdigit)
			ldi ZH,high(DATE1stdigit)
updateloop2:ld SLAVE_REG,Z+
			rcall DATA_WRITE
			dec r20
			brne updateloop2
			ret
			
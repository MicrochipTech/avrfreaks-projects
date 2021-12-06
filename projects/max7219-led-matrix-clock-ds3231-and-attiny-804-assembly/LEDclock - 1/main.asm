;
; LEDclock.asm
;
; Created: 03/12/2021 21:35:35
; Author : Manama
;

; PA1 = MOSI
; PA2 = MISO
; PA3 = SCK
; PA4 = SS

;press menu button for a moment to enter menu function
;press next button to change values of of the selected/displayed option (values increment and start from begining)
;press save button to save selected value , the next option will be automatically loaded one by one (hour,min,day,month,date,year > time)
;PA5 next button active 0 idle 1
;PA6 save button active 0 idle 1
;PA7 menu button active 0 idle 1
.equ no_op =  0x00
.equ digit0 = 0x01
.equ digit1 = 0x02
.equ digit2 = 0x03
.equ digit3 = 0x04
.equ digit4 = 0x05
.equ digit5 = 0x06
.equ digit6 = 0x07
.equ digit7 = 0x08
.equ decode = 0x09
.equ bright = 0x0A
.equ scanlimit = 0x0B 
.equ shutdown =   0x0C
.equ dsplytest = 0x0F
.equ matrixoff = 0x0c00
.equ matrixon =  0x0c01
.equ nodecode =  0x0900
.equ decodeall = 0x09FF
.equ fullbright =0x0A0F
.equ halfbright = 0x0A07
.equ row =       0x0b07
.equ dsplytestmode = 0x0F01
.equ dsplynormal =   0x0F00
.equ fclk = 10000000
.equ DS1307WAD = 0xD0
.equ DS1307RAD = 0xD1
.equ read_data_len = 19
.DEF SLAVE_REG = R17
.DEF TEMP = R16
.def address = r20
.def data = r21
.def array_start = r10
.def ASCII = r11
.def address_counter = r22
.def seven = r4

.macro micros					; macro for delay in us
ldi temp,@0
rcall delayTx1uS
.endm

.macro millis					; macro for delay in ms
ldi YL,low(@0)
ldi YH,high(@0)
rcall delayYx1mS
.endm


.macro matrix_Cwrite			; macro for MAX7219 command write (initialization) , call matrix_Cwrite @
ldi data,low(@0)
ldi address,high(@0)
rcall matrix_Cwrite1
.endm

.macro write_to_all
ldi data,low(@0)
ldi address,high(@0)
rcall write_to_all1
.endm

.macro matrix_Dwrite
ldi address,@0
ldi data,@1
rcall SPI_TX
.endm

.macro matrix_singlewrite			; macro to write to a single matrix (1 of 8) , call matrix_singlewrite @0 @1    ,@0= matrix number ,@1= ascii character
ldi r16,@0
mov r3,r16
ldi r16,@1
sts PAD1,r16
rcall single_matrix_write
.endm

.macro string_write				; macro to write a string with length of 1 byte only , call string_write @0  , @0 = label name of string in flash
ldi ZL,low(@0 * 2)
ldi ZH,high(@0 * 2)
rcall string_write1
.endm


.dseg
PAD1: .byte 1					; 1 byte  reserved to store data
PAD2: .byte 1					; 1 byte  reserved to store data
BUFFER: .byte 20				; 20 bytes reserved in sram to store DS3231 time,date, week data retrieved via I2C for processing later
HOUR1stdigit: .byte 1			; ASCII data of hour bcd stored for sending to screen
HOUR2nddigit: .byte 1			; ASCII data of hour bcd stored for sending to screen
COLON1: .byte 1					; ASCII colon to be displayed stored here
MINUTES1stdigit: .byte 1		; ASCII data of minute bcd stored for sending to screen 
MINUTES2nddigit: .byte 1		; ASCII data of minute bcd stored for sending to screen
COLON2: .byte 1					; ASCII colon to be displayed between minute and seconds stored here
SECOND1stdigit: .byte 1			; ASCII data of seconds bcd stored for sending to screen
SECOND2nddigit: .byte 1			; ASCII data of seconds bcd stored for sending to screen
DATE1stdigit: .byte 1			; ASCII data of date bcd stored for sending to screen
DATE2nddigit: .byte 1			; ASCII data of date bcd stored for sending to screen
SLASH1: .byte 1					; ASCII slash stored here to be displayed between date and month
MONTH1stdigit: .byte 1			; ASCII data of month bcd stored for sending to screen
MONTH2nddigit: .byte 1			; ASCII data of month bcd stored for sending to screen
SLASH2: .byte 1					; ASCII slash stored here to be displayed between month and year
YEAR1stdigit: .byte 1			; ASCII data of year bcd stored for sending to screen
YEAR2nddigit: .byte 1			; ASCII data of year bcd stored for sending to screen
SLASH3: .byte 1
DAY1: .byte 1					; ASCII character of week/day
DAY2: .byte 1					; ASCII character of week/day
DAY3: .byte 1					; ASCII character of week/day
DAY4: .byte 1					; ASCII character of week/day
DAY5: .byte 1					; ASCII character of week/day
DAY6: .byte 1					; ASCII character of week/day
DAY7: .byte 1					; ASCII character of week/day
DAY8: .byte 1					; ASCII character of week/day
PERIOD: .byte 1
minbcd: .byte 1					; user saved value for hour is stored here during time and date adjustment
hourbcd: .byte 1				; user saved value for hour is stored here during time and date adjustment
weekbcd: .byte 1				; user saved value for hour is stored here during time and date adjustment
datebcd: .byte 1				; user saved value for hour is stored here during time and date adjustment
monbcd: .byte 1					; user saved value for hour is stored here during time and date adjustment
yearbcd: .byte 1				; user saved value for hour is stored here during time and date adjustment
smachine: .byte 1
count: .byte 1					; used as a counter value storage space , this value determines how many time displays before a date display is made
alarm2_2: .byte 1				; alarm selection storage
alarm2_3: .byte 1				; alarm selection storage
alarm2_4: .byte 1				; alarm selection storage
alarm_ctrl: .byte 1				; alarm selection storage
alarm_ctrl_status: .byte 1		; alarm selection storage
screen_buffer: .byte 64			; 64 bytes of sram reserved to do screen manipulation like smooth scroll , fade in , fade out etc


.cseg
.ORG 0X00
rjmp reset

reset:
	ldi r16,0Xd8			; unlock protect write
	out CPU_CCP,r16
	ldi r16,0x01			; clk prescaler of 2, 20Mhz/2 = 10Mhz
	STS CLKCTRL_MCLKCTRLB,R16
setup:
	ldi r16,0x20			; 0x21 is the value of first font stored in fonts array
	mov array_start,r16		; 0x21 is stored in dedicated register array_start
	ldi temp,7				; load temp with 7 (used in subtraction with 7)
	mov seven,temp			; copy temp to seven to be used in adding operations while writing to LED matrix
	ldi r16,0b11100100		; bit mask to clear port direction register to make them inputs
	sts PORTA_DIRCLR,r16	; PA7,PA6,PA5,PA2 becomes inputs
	ldi r16,0b00001000		; initialise PORTA  pullup resistor 
	sts PORTA_PIN7CTRL,r16	; enable pullup on PA7,PA6,PA5,PA2 for buttons ,idle 1 , active 0
	sts PORTA_PIN6CTRL,r16
	sts PORTA_PIN5CTRL,r16
	sts PORTA_PIN2CTRL,r16
	ldi r16,0b00011010		; PA4,PA3,PA1 are outputs- SS,SCK & MOSI respectievly (SPI engine initialization)
	out VPORTA_DIR,r16
	sbi VPORTA_OUT,4		; SS HI ,set BIT4, PA4 =SS
	ldi r16,0b00100001		; 7=reserved,dord =0=msb,master=1,CLK2X=0,3= reserved,2:1= 0x00 clk/4,enabe=1
	sts SPI0_CTRLA,r16
	ldi r16,0b00000100		; buffer= disable,SSD =1 ,mode =0
	sts SPI0_CTRLB,r16		; start SPI engine
	rcall matrix_init		; initialize LED matrix
	rcall TWI_INIT			; initialize I2C engine
	

SM:
	lds r16,PORTA_IN		; when no switch pressed bit 7,6,5 should be set
	andi r16,0b10100000		; and with 0xA0 so that 7&5 bits remain,if menu and next buttons are pressed together 0x00 results,only menu button pressed = 0x20,only next pressed = 0x80
	breq jump_alarm_routine	; jump to alarm adjustment routine if code available , MENU & NEXT button pressed together
	cpi r16,0x20			; if r16 is 0x20
	breq jump_adjust_time	; jump to time adjustment routine
SMret:
	rcall measure			; if flag not set in smachine call measure function to read current time
	lds temp,count			; copy counter value to r16 for sram count register
	cpi temp,218			; if counter is 218
	breq display_date		; branch to label display_date which will make ajump to date display routine
enter1:	inc temp			; if counter not 218 increase counter value
	sts count,temp			; stored increased value back to sram space count
	rjmp SM					; repeat the loop

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;below routine reads time from rtc and displays it on LCD
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

measure:
	rcall inforead				; call subroutine which reads DS3231 clock registers and store in buffer (sram)
	rcall BCD_TO_ASCII			; convert bcd values retrieved from clock module to ASCII and store in relevent sram spaces reserved for the job
	rcall clear_matrix1			; call subroutine to clear the 8 MAX7219 LEDMATRIX (8 of them connected)
	rcall update_matrix1		; routine that reads the time sram registers and copies the font elements fron flash and writes all 8 LED matrices
	rcall ms250					; 250 ms delay
	ret
	
display_date:
	rcall clear_matrix1			; call subroutine to clear the 8 MAX7219 LEDMATRIX (8 of them connected)
	rcall update_matrix2		; routine that reads the date sram registers and copies the font elements fron flash and writes all 8 LED matrices
	rcall ms2000				; 2 seconds display time for date:month:year
	rcall clear_matrix1			; call subroutine to clear the 8 MAX7219 LEDMATRIX (8 of them connected)
	rcall show_week				; call subroutine that reads sram space DAY1 to DAY8 and displays the week data (eg MONDAY)
	rcall ms2000				; 2 seconds display time
	clr temp					; load 0 in r16
	sts count,temp				; store 0 in count to reset counter for time display, if reach 218 will revisit here again
	rjmp enter1					; jump back to measure routine to display time


jump_adjust_time:
	string_write string0		; macro called to display message stored at label string0 , "ADJ TIME"
	rcall ms2000				; 2 seconds display time
	rjmp adjust_time			; jump to time adjust routine (reach here if MENU button is pressed)
	

jump_alarm_routine: 
	string_write string1
	rcall ms2000
	rjmp SM

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
		rcall ACK				;checks whether slave has acked
		ret

TWI_WRITE:
		MOV R16,SLAVE_REG
		STS TWI0_MDATA,R16
		RCALL WAIT_WIF
		rcall ACK
		micros 10
		ret


		
ACK:
		LDS R16,TWI0_MSTATUS
		SBRC R16,4
		rjmp ACK
		RET



TWI_READ:
		ldi SLAVE_REG,DS1307WAD
		rcall TWI_START
		ldi SLAVE_REG,0x00		; send instruction/READ_ADDRESS to THE SLAVE FROM WHICH DATA IS READ ,first register of DS1307 is 0x00
		rcall TWI_WRITE
		ldi r16,0x00			;loading 0 in ACKACT bit enables master to send ack after reading data register
		sts TWI0_MCTRLB,r16
		ldi r16,DS1307RAD		; repeated start ;  I2C slave address + read bit (1) SHOULD BE LOADED HERE FOR READING DATA FROM SLAVE READ_ADDRESS GIVEN ABOVE
		STS TWI0_MADDR,R16
		rcall WAIT_RIF

		ldi r16,read_data_len	;load r16 with number of bytes to be read
		cpi r16,0x02			;is num of bytes less than or greater than 2
		brlo BYYTE				;if less than 2 branch to 1BYTE as NACK+STOP will be loaded prior to read
		dec r16					; decreace one count from the total count to get loop value,NACK should be sent before the last byte read
		mov r5,r16				; move the count -1 value to counter r5
loop_read:
		LDS R16,TWI0_MDATA		;MDATA REGISTER IS COPIED TO R16,DATA IS RECIVED INTO MDATA FROM SLAVE
		ST X+,R16				;DATA IN R16 IS STORED IN SRAM BUFFER FOR LATER USE. 
		RCALL WAIT_RIF			;wait for read flag
		dec r5					;decrease counter after each read
		brne loop_read			;go throug loop till {count - 1} is finished
BYYTE: 
		LDI R16,0b00000111		;CLEAR ACKACT BIT BEFORE READING LAST BYTE AND ISSUE A STOP = NACK+STOP
		STS TWI0_MCTRLB,R16
		LDS R16,TWI0_MDATA		;MDATA REGISTER IS COPIED TO R16,THIS THE LAST DATA IS RECEIVED  FROM SLAVE
		ST X+ ,R16				;DATA IN R16 IS STORED IN SRAM BUFFER FOR LATER USE. 
		RET


TWI_STOP:
		LDI R16,0b00000011       ;STOP
		STS TWI0_MCTRLB,R16
		RET


WAIT_WIF:
		LDS R16,TWI0_MSTATUS
		SBRS R16,6				;CHECK WIF IS SET,IF SET SKIP NEXT INSTRUCTION (write interrupt flag)
		RJMP WAIT_WIF
		RET


WAIT_RIF:
		LDS R16,TWI0_MSTATUS
		SBRS R16,7				;CHECK RIF IS SET,IF SET SKIP NEXT INSTRUCTION (read interrupt flag)
		RJMP WAIT_RIF
		RET



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;the below routines are for recurring delays used in the program
;using macros will increase code size so created these subroutines to be called
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


us20:micros 20
		ret

ms2000:millis 2000
		ret
ms500: millis 500
		ret
ms250: millis 250
		ret
ms50: millis 50
		ret
ms20:  millis 20
		ret
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; function to read time registers to buffer
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

inforead:					
	ldi XL,low(BUFFER)		; setting pointer to buffer
	ldi XH,high(BUFFER)
	ldi SLAVE_REG,DS1307WAD	; DS3231 write address
	rcall TWI_START			; issues start and sends write address
	ldi SLAVE_REG,0x00     	; DS1307 has its time register addresses starting from 0x00
	rcall TWI_WRITE			; function used to transmit above data
	rcall TWI_READ			; function to read DS1307 registers
	ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; routine to convert BCD to ASCII and store in appropriate registers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


BCD_TO_ASCII:				
	ldi XL,low(BUFFER)		; pointer to the start address of buffer
	ldi XH,high(BUFFER)
	ld r16,X+
	sts PAD1,r16			; copy seconds data to SRAM address PAD1 for temprerory storage while processing
	swap r16				; swapp upper and lower nibbles of r16
	andi r16,0x3f			; AND with 0b00111111 and only swapped seconds data remains ,tenth position is in lower nibble and unit position in higher nibble
	ori r16,0x30			; OR with ascii for 0 which is 0x30 (0b00110000) , higher nibble is invalidated and only lower nibble which is 10th seconds gets converted to ascii
	sts SECOND1stdigit,r16	; store the 10th seconds ascii digit in SRAM location SECOND1stdigit			
	lds r16,PAD1			; copy previously stored seconds data from PAD1
	andi r16,0x3f			; AND with 0b00111111 and seconds data remains with higher nibble as 10th place and lower nibble as unit place
	ori r16,0x30			; OR the above value with ASCII0 (0b00110000) 0x30, upper nibble becomes 0x3 and lower nibble will remain as it is, which is unit part of seconds data
	sts SECOND2nddigit,r16	; copy his ASCII unit seconds value to SECOND2nddigit in SRAM


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
	sts weekbcd,r16
	inc r16					; increase r16 (day), this is done to align address to day , correct values being 1-7 , we start with 0 from table, this is taken care of
	ldi ZL,low(2*WEEK0)		; initiate Z pointer to address WEEK0 string  which display "WEEK" when selection is 0 in adj menu  , "MONDAY" when selection is 1
	ldi ZH,high(2*WEEK0)
WLS:subi ZL,-8				; increase pointer by 8 bits (all message strings are 8 bits as we have only 8 matrices), 
	dec r16					; decrease day value back to original value loaded from weekbcd either by time adjust procedure or copied from DS3231
	brne WLS				; check if r16 is 0 after decreasing , if not 0 loop to label WLS which will inrease pointer to next string+1 than what is stored in weekbcd
	subi ZL,8				; subtract 8 from pointer so that it points to what weekbcd represents. 1=monday,2=tuesday,3=wednesday....7=SUNDAY
	lpm r18,Z+				; load from program memory ASCII value of first letter that indicates the day stored in the new address after above calculation
	sts DAY1,r18			; store in SRAM address DAy1 the first alphabet to be dispalyed in LED
	lpm r18,Z+				; Z incremented earlier now points to next address of the 2nd alphaber of the week day (MO,Tu,WE,TH,FR,SA,SU)
	sts DAY2,r18			; store in SRAM address DAY2 the 2nd alphabet to be displayed on the led
	lpm r18,Z+				; load from program memory ASCII value of first letter that indicates the day stored in the new address after above calculation
	sts DAY3,r18			; store in SRAM address DAy3 the 3rd alphabet to be dispalyed in LED
	lpm r18,Z+				; load from program memory ASCII value of first letter that indicates the day stored in the new address after above calculation
	sts DAY4,r18			; store in SRAM address DAy4 the 4th alphabet to be dispalyed in LED
	lpm r18,Z+				; load from program memory ASCII value of first letter that indicates the day stored in the new address after above calculation
	sts DAY5,r18			; store in SRAM address DAy5 the 5th alphabet to be dispalyed in LED
	lpm r18,Z+				; load from program memory ASCII value of first letter that indicates the day stored in the new address after above calculation
	sts DAY6,r18			; store in SRAM address DAy6 the 6th alphabet to be dispalyed in LED
	lpm r18,Z+				; load from program memory ASCII value of first letter that indicates the day stored in the new address after above calculation
	sts DAY7,r18			; store in SRAM address DAy7 the 7th alphabet to be dispalyed in LED
	lpm r18,Z+				; load from program memory ASCII value of first letter that indicates the day stored in the new address after above calculation
	sts DAY8,r18			; store in SRAM address DAy8 the 8th alphabet to be dispalyed in LED


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


loopwrite:	ldi r23,6				; subroutine to copy values from consecutive SRAM address minbcd to yearbcd and write it to ds1307 rtc
			ldi ZL,low(minbcd)		; 
			ldi ZH,high(minbcd)
loopwrite1:	ld SLAVE_REG,Z+
			rcall TWI_WRITE
			dec r23
			brne loopwrite1
			ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;FONTS   fonts below 5bytes ,assembler will add one byte of padding with 0. hence array lenth =6
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
fonts:
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 ; space
.db 0x00,0x10,0x10,0x10,0x10,0x00,0x10,0x00} ; !
.db 0x36,0x36,0x12,0x24,0x00,0x00,0x00,0x00} ; "   
.db 0x14,0x14,0x7f,0x14,0x7f,0x14,0x14,0x00} ; # 3  0x23
.db 0x14,0x3f,0x54,0x54,0x3e,0x15,0x7e,0x14} ; $ 4  0x24
.db 0x00,0x62,0x64,0x08,0x10,0x26,0x46,0x00} ; % 5  0x25
.db 0x1c,0x22,0x12,0x3c,0x48,0x4a,0x3c,0x08} ; & 6  0x26
.db 0x18,0x18,0x08,0x10,0x00,0x00,0x00,0x00} ; ' 7  0x27
.db 0x00,0x10,0x20,0x20,0x20,0x20,0x10,0x00} ; ( 8  0x28
.db 0x00,0x10,0x08,0x08,0x08,0x08,0x10,0x00} ; )  0x29
.db 0x00,0x2a,0x1c,0x08,0x1c,0x2a,0x00,0x00} ; * 10 0x2A
.db 0x00,0x10,0x10,0x7c,0x10,0x10,0x00,0x00} ; + 11 0x2B
.db 0x00,0x00,0x00,0x00,0x30,0x30,0x10,0x20} ; , 12 0x2C
.db 0x00,0x00,0x00,0x7c,0x00,0x00,0x00,0x00} ; - 13 0x2D
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00} ; . 14 0x2E
.db 0x00,0x04,0x08,0x10,0x20,0x40,0x80,0x00} ; / 15 0x2F
.db 0x00,0x38,0x4c,0x54,0x64,0x44,0x38,0x00} ;0
.db 0x00,0x10,0x30,0x10,0x10,0x10,0x7c,0x00} ;1
.db 0x00,0x30,0x48,0x08,0x10,0x20,0x78,0x00} ;2
.db 0x00,0x7c,0x08,0x38,0x04,0x04,0x78,0x00} ;3
.db 0x00,0x08,0x18,0x28,0x7c,0x08,0x08,0x00} ;4
.db 0x00,0x7c,0x40,0x78,0x04,0x04,0x78,0x00} ;5
.db 0x00,0x38,0x40,0x78,0x44,0x44,0x38,0x00} ;6
.db 0x00,0x7c,0x08,0x10,0x20,0x20,0x20,0x00} ;7
.db 0x00,0x38,0x44,0x38,0x44,0x44,0x38,0x00} ;8
.db 0x00,0x38,0x44,0x44,0x3c,0x04,0x38,0x00} ;9
.db 0x00,0x10,0x00,0x00,0x00,0x00,0x10,0x00} ;:
.db 0x00,0x18,0x18,0x00,0x18,0x18,0x08,0x10} ; ; 27 0x3B
.db 0x00,0x10,0x20,0x40,0x40,0x20,0x10,0x00} ; < 28 0X3C
.db 0x00,0x00,0x7c,0x00,0x7c,0x00,0x00,0x00} ; = 29 0X3D
.db 0x00,0x00,0x08,0x04,0x02,0x04,0x08,0x00} ; > 30 0X3E
.db 0x00,0x38,0x44,0x04,0x18,0x10,0x00,0x10} ; ? 31 0X3F
.db 0x3c,0x42,0x99,0xa5,0xa5,0x9f,0x40,0x3f} ; @ 32 0X40
.db 0x10,0x28,0x44,0x44,0x7c,0x44,0x44,0x00} ;A
.db 0x00,0x70,0x48,0x78,0x44,0x44,0x78,0x00} ;B
.db 0x00,0x38,0x40,0x40,0x40,0x40,0x38,0x00} ;C
.db 0x00,0x78,0x44,0x44,0x44,0x44,0x78,0x00} ;D
.db 0x00,0x78,0x40,0x78,0x40,0x40,0x7c,0x00} ;E
.db 0x00,0x78,0x40,0x78,0x40,0x40,0x40,0x00} ;F
.db 0x00,0x38,0x40,0x40,0x5c,0x54,0x34,0x00} ;G
.db 0x00,0x44,0x44,0x7c,0x44,0x44,0x44,0x00} ;H
.db 0x00,0x38,0x10,0x10,0x10,0x10,0x7c,0x00} ;I
.db 0x00,0x7c,0x10,0x10,0x10,0x50,0x30,0x00} ;J
.db 0x00,0x48,0x50,0x60,0x50,0x48,0x44,0x00} ;K
.db 0x00,0x40,0x40,0x40,0x40,0x40,0x7c,0x00} ;L
.db 0x00,0x44,0x6c,0x54,0x44,0x44,0x44,0x00} ;M
.db 0x00,0x42,0x62,0x52,0x4a,0x46,0x42,0x00} ;N
.db 0x00,0x3c,0x42,0x42,0x42,0x42,0x3c,0x00} ;O
.db 0x00,0x78,0x44,0x44,0x78,0x40,0x40,0x00} ;P
.db 0x00,0x38,0x44,0x44,0x54,0x4c,0x3c,0x02} ;Q
.db 0x00,0x78,0x44,0x44,0x78,0x50,0x48,0x00} ;R
.db 0x00,0x3c,0x40,0x3c,0x02,0x02,0x7c,0x00} ;S
.db 0x00,0x7c,0x10,0x10,0x10,0x10,0x10,0x00} ;T
.db 0x00,0x44,0x44,0x44,0x44,0x44,0x38,0x00} ;U
.db 0x00,0x44,0x44,0x44,0x44,0x28,0x10,0x00} ;V
.db 0x00,0x42,0x42,0x42,0x5a,0x66,0x42,0x00} ;W
.db 0x00,0x42,0x24,0x18,0x18,0x24,0x42,0x00} ;X
.db 0x00,0x44,0x44,0x28,0x10,0x10,0x10,0x00} ;Y
.db 0x00,0x7c,0x08,0x10,0x20,0x40,0x7c,0x00} ;Z
.db 0x3c,0x20,0x20,0x20,0x20,0x20,0x20,0x3c} ; [ 59 0X5B
.db 0x00,0x40,0x20,0x10,0x08,0x04,0x02,0x00} ; \ 60 0X5C
.db 0x1c,0x04,0x04,0x04,0x04,0x04,0x04,0x1c} ; ] 61 0X5D
.db 0x08,0x14,0x22,0x00,0x00,0x00,0x00,0x00} ; ^ 62 0X5E
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7e} ; _ 63 0X5F


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; LED matrix update subroutine (8 peices of 64LED matrix is used to display time).
; sends 8 bytes of data for each column from column0 to column7, 1st all 8 column 0's are sent with 1st byte of the font. then 1,2,3...7
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
update_matrix1:
	clr r23				; counter used by screen_write routine to select correct font byte for each column (we write all similar columns 1 time)
	clr r9				; register used to add with carry propagation	
	ldi address,digit0		; initialize address register to address of column0,later we increase this register to address concurrent columns
column_select:
	ldi address_counter,8		; counter used by screen_write routine(counts how many LED matrix,we use 8 for time display)
	ldi XL,low(HOUR1stdigit)	; set pointer X to sram register HOUR1stdigit which is first data to be transmitted to LED matrix
	ldi XH,high(HOUR1stdigit)	; set pointer X to sram register HOUR1stdigit which is first data to be transmitted to LED matrix
	rcall SS_LO			; make SS pin low to activate MAX7219 and start SPI transfer
	rcall screen_write1		; call routine screen_write which will write data(font byte for the concerned column)
	rcall SS_HI			; make SS pin HI to latch the 8x8 bytes transmitted to the addressed columns of 8 LEDmatrices 
	inc address			; increase the address by 1 , all column addresses are concecutive, to send data to next column of 8 matrix
	inc r23				; increase byte count of the font array, if 1st byte was sent earlier ,incerasing r23 will send 2nd byte for all columns
	cpi r23,0x09			; if r23 is 0x09 all columns has been addressed and the screen is updated will all bytes of the fonts to be displayed
	brne column_select		; if r23 has not reached value 0x09 loop back till all columns have been addressed
	ret
screen_write1:
	ld ASCII,X+
	rcall find_font
	clc
	add ZL,r23
	adc ZH,r9
	lpm data ,Z
	rcall SPI_TX
	dec address_counter
	brne screen_write1
	ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Command write for matrix - writes command to 8 max7912 in one call ,USE WITH macro matrix_Cwrite
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

matrix_Cwrite1:
	ldi address_counter,8
	rcall SS_LO
Cloop:	rcall SPI_TX
	dec address_counter
	brne Cloop
	rcall SS_HI
	ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;MAX7219 initialization sequence , caled with macro
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
matrix_init:
	matrix_Cwrite matrixon
	matrix_Cwrite nodecode
	;matrix_Cwrite fullbright
	matrix_Cwrite halfbright
	matrix_Cwrite row
	matrix_Cwrite dsplytestmode
	rcall ms500
	matrix_Cwrite dsplynormal
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;find_font function   uses registers- temp,array_start,ASCII,
;characters to be printed has to be passed into register ASCII as ASCII values
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
find_font:
	ldi ZL,low(2*fonts)	     ; low address of fonts
	ldi ZH,high(2*fonts)	 ; high address of font
	sub ASCII,array_start    ; result of subtraction will be position of first byte of the character
	breq ASCII0				 ; if above subtraction is 0 then character is first row in the array
	clr temp				 ; clear r16
	clc						 ; clear carry bit in sreg
multiply:
	subi ZL,low(-8)          ; adding immediate not supported. immediate extends beyond 8bit.using subi & sbci with -ve number will do & loading with hi &lo will propagate carry.
	sbci ZH,high(-8)	     ; add with carry 8
	inc temp		         ; increase array counter 
	cp temp,ASCII		     ; compare counter wih ASCII register
	brne multiply		     ; if not equal jump 8 elements of array
ASCII0:
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;											
; SPI routines
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; PA1 = MOSI
; PA2 = MISO
; PA3 = SCK
; PA4 = SS


SS_LO:	cbi VPORTA_OUT,4		; SS low ,clear bit4 ,PA4 =SS
		ret

SS_HI:	sbi VPORTA_OUT,4		; SS HI ,set BIT4, PA4 =SS
		ret

SPI_TX:	        
		sts SPI0_DATA,address		; write address to SPI data register
		rcall checkif
		sts SPI0_DATA,data		; write data to SPI data register
		rcall checkif
		ret 
checkif:lds r16,SPI0_INTFLAGS		; copy SPI0 flag register
		andi r16,0b10000000		; and with 0x80 to see if bit 7 is set after all bits are transmitted
		breq checkif			; if IF flag not set sit in a tight loop
		ret
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;clear matrix- clears all matrices by writing 0 to all max7219
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
clear_matrix1:
	clr r23						; counter used by screen_write routine to select correct font byte for each column (we write all similar columns 1 time)
	clr r9						; register used to add with carry propagation	
	ldi address,digit0			; initialize address register to address of column0,later we increase this register to address concurrent columns
column_select1:
	ldi address_counter,8		; counter used by screen_write routine(counts how many LED matrix,we use 8 for time display)
	clr data					; data register loaded with 0 to be written to columns
	rcall SS_LO					; make SS pin low to activate MAX7219 and start SPI transfer
clear_screen:
	rcall SPI_TX				; transmit 0x00 written in data register to the column address selected for all matrices
	dec address_counter			; address counter was loaded with 8 ,so 0x00 will be transmitted 8 times for 1 column address
	brne clear_screen			; transmit till 8 bytes are clocked out ,1 column of all 8 matrix will be erased
	rcall SS_HI					; make SS pin HI to latch the 8x8 bytes transmitted to the addressed columns of 8 LEDmatrices 
	inc address					; increase the address by 1 , all column addresses are concecutive, to send data to next column of 8 matrix
	inc r23						; increase byte count of the font array, if 1st byte was sent earlier ,incerasing r23 will send 2nd byte for all columns
	cpi r23,0x09				; if r23 is 0x09 all columns has been addressed and the screen is updated will all bytes of the fonts to be displayed
	brne column_select1			; if r23 has not reached value 0x09 loop back till all columns have been addressed
	ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;single_matrix_write
;use with macro - matrix_singlewrite , matrix number from msb, ascii data . eg matrix_singlewrite 3,A  will write to 3rd matrix from lhs letter A
;remaining matrices will be unchanged as nop codes passed
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 single_matrix_write:
	clr r23						; counter used by screen_write routine to select correct font byte for each column (we write all similar columns 1 time)
	clr r9						; register used to add with carry propagation	
	ldi address,digit0
SMW_loop:
	lds ASCII,PAD1
	rcall SS_LO
	rcall single_write
	rcall SS_HI
	inc address
	inc r23
	cpi r23,9
	brne SMW_loop
	ret
	
single_write:	
	ldi address_counter,8		; counter used by screen_write routine(counts how many LED matrix,we use 8 for time display)
SWloop:
	cp r3,address_counter
	brne skip_matrix
	rcall find_font
	clc
	add ZL,r23
	adc ZH,r9
	lpm data ,Z
	rcall SPI_TX
	dec address_counter
	brne SWloop
	ret
skip_matrix:
	push address
	ldi address,no_op
	rcall SPI_TX
	pop address
	dec address_counter
	brne SWloop
	ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; LED matrix update subroutine (8 peices of 64LED matrix is used to display time).
; sends 8 bytes of data for each column from column0 to column7, 1st all 8 column 0's are sent with 1st byte of the font. then 1,2,3...7
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
update_matrix2:
	clr r23				; counter used by screen_write routine to select correct font byte for each column (we write all similar columns 1 time)
	clr r9				; register used to add with carry propagation	
	ldi address,digit0		; initialize address register to address of column0,later we increase this register to address concurrent columns
column_select2:
	ldi address_counter,8		; counter used by screen_write routine(counts how many LED matrix,we use 8 for time display)
	ldi XL,low(DATE1stdigit)	; set pointer X to sram register HOUR1stdigit which is first data to be transmitted to LED matrix
	ldi XH,high(DATE1stdigit)	; set pointer X to sram register HOUR1stdigit which is first data to be transmitted to LED matrix
	rcall SS_LO			; make SS pin low to activate MAX7219 and start SPI transfer
	rcall screen_write2		; call routine screen_write which will write data(font byte for the concerned column)
	rcall SS_HI			; make SS pin HI to latch the 8x8 bytes transmitted to the addressed columns of 8 LEDmatrices 
	inc address			; increase the address by 1 , all column addresses are concecutive, to send data to next column of 8 matrix
	inc r23				; increase byte count of the font array, if 1st byte was sent earlier ,incerasing r23 will send 2nd byte for all columns
	cpi r23,0x09			; if r23 is 0x09 all columns has been addressed and the screen is updated will all bytes of the fonts to be displayed
	brne column_select2		; if r23 has not reached value 0x09 loop back till all columns have been addressed
	ret
screen_write2:
	ld ASCII,X+
	rcall find_font
	clc
	add ZL,r23
	adc ZH,r9
	lpm data ,Z
	rcall SPI_TX
	dec address_counter
	brne screen_write2
	ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; display_week , subroutine to display week characters stored in SRAM registers DAY1 to DAY8
; monday to sunday
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
display_week:
	ldi r16,8					; counter value used by subroutine single_matrix_write to count number of matrix written (we use 8 led blocks here so value 8)
	mov r3,r16					; move value to matrix counter r3
	ldi YL,low(DAY1)			; set Y pointer to DAY1 which holds 1st character of the week day selected
	ldi YH,high(DAY1)
WLL:ld ASCII,Y+					; load from flash the charcter to ASCII register
	sts PAD1,ASCII				; copy to sram space PAD1 
	rcall single_matrix_write	; call subroutine to write to LED block pointed by r3, 8 points to MSB 1st from left and 1 points to last/8th from lhs
	dec r3						; decrease matrix pointer to write from lhs to rhs each iteration till r3 reaches 0
	brne WLL					; loop to WLL until r3 is 0
	ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;string_write, routine writes a 8 character string to LED matrix , call with macro string_write & string label
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
string_write1:
	ldi temp,8					; number of matrices in screen 8 
	mov r3,temp					; copy to matrix counter used by single_matrix_write
SL:	lpm ASCII,Z+				; load from flash value pointed by Z to ASCII register. Z is initialized by macro "string_write string0"
	sts PAD1,ASCII				; copy ASCII to sram register PAD1 to be used inside routine single_matrix_write
	push ZL						; push ZL to stack as Z pointer is changed inside single_matrix_write
	push ZH						; push ZH to stack as Z pointer is changed inside single_matrix_write
	rcall single_matrix_write	; call routine single_matrix_write to write to each matrix each character in the string (max 8 chars)
	pop ZH						; pop back ZH stored earlier which has the address of next character in the string
	pop ZL						; pop back ZH stored earlier which has the address of next character in the string
	dec r3						; decrease the matrix counter which writess from MSB (matrix8 to matrix1) to LSB
	brne SL						; loop through SL untill all matrices are written
	ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;clockwrite  subroutine to update clock chip what is selected in menu time ,date and day of week
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


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
	clr r16
	sts weekbcd,r16
	ret
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;ALL ROUTINES BELOW FOR TIME & DATE ADJUST MENU
;	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; routine to convert BCD values selected to ascii for time adjustment to be displayed on matrix
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

bcd_ascii:          	
	lpm r5,Z+			; load from program memory the first value from  "number" label
	mov r17,r5			; copy to r17 which is used to transmit over twi
	andi r17,0xf0		; upper nibble preserved
	swap r17			; swap nibble of the BCD l;oaded from table
	ori r17,0x30		; OR with 0x30 to convert to ASCII
	st X+,r17			; the ASCII value will be stored in memory location pointed by X
	mov r17,r5		    ; copy again the BCD for the second digit to be processed to ascii
	andi r17,0x0f		; lower nibble preserved
	ori r17,0x30		; or with 0x30 to convert to ASCII
	st X,r17
	ret

adjust_time:
	ldi temp,0x30			; load r16 0x30 = ASCII 0 /zero
	sts HOUR1stdigit,r16	; store 0x30 in sram register HOUR1stdigit
	sts HOUR2nddigit,r16	; store 0x30 in sram register HOUR2nddigit
	sts MINUTES1stdigit,r16	; store 0x30 in sram register MINUTE1stdigit
	sts MINUTES2nddigit,r16	; store 0x30 in sram register MINUTE2nddigit
	sts SECOND1stdigit,r16	; store 0x30 in sram register SECOND1stdigit
	sts SECOND2nddigit,r16	; store 0x30 in sram register SECOND2nddigit
	sts DAY1,r16
	sts DAY2,r16
	sts DAY3,r16
	sts DAY4,r16
	sts DAY5,r16
	sts DAY6,r16
	sts DAY7,r16
	sts DAY8,r16
	ldi r24,0x01			; initialize state register to 0x01 indicating adjust hour procedure

adjusthour:
	ldi ZL,low(2*number)	; load Zpointer low with address of number array
	ldi ZH,high(2*number)	; load Zpointer high with address of number array (higher part)
	ldi XL,low(HOUR1stdigit) ; load Xpointer with low address of HOUR1stdigit(register to store ASCII to be displayed)
	ldi XH,high(HOUR1stdigit);load Xpointer with high address of HOUR1stdigit(register to store ASCII to be displayed)
	ldi r18,24				; load r18 array element counter with 24 (for 24 hours)
proc0:
	rcall bcd_ascii			; call subroutine to convert BCD value to equivalent ASCII
	dec r18					; decrease element counter of array
	push ZL					; push ZL to stack as this value will change when update_matrix1 is called
	push ZH					; push ZH to stack as this value will change when update_matrix1 is called
	dec XL					; decrease XL 1 count to point to the start address as this was increneted to store the first value
	push XL					; push XL to stack as this value will change when update_matrix1 is called
	push XH					; push XH to stack as this value will change when update_matrix1 is called
	rcall update_matrix1	; call procedure to write ASCII vakues stored in hour & minutes register on LED matrix
	pop XH					; pop back XH
	pop XL					; pop XL
	pop ZH					; pop ZH
	pop ZL					; pop ZL
	
scan_button0:				; polls SAVE button 
	lds r16,PORTA_IN		; copy PORTA pin status/values,PA6
	andi r16,0x40			; and with 0x40 for PA6 . if 0 button pressed if 0x02 button idle
	sbrs r16,6				; if idle (0x40) skip next instruction which is save th current number as hour
	rjmp savehour			; if button pressed jump to save routine
scan_next0:					; polls NEXT button (to increment value)
	lds r16,PORTA_IN		; copy PORTA pin status/values
	andi r16,0x20			; and with 0x20 for PA5 . if 0 button pressed if 0x20 button idle
	sbrc r16,5				; if 0 (pressed) skip next instruction else loop until either save or next button is pressed
	rjmp scan_button0		; keep polling both buttons until either one is pressed
	cpi r18,0x00			; check counter is 0, if zero start from begining (0-24 hours)
	breq adjusthour			; if select values exhausted branch to adjusthour to start again until a value is selected and saved
	millis 250				; delay for visual and human needs
	rjmp proc0
savehour:
	sts hourbcd,r5			; store the selected hour BCD value in SRAM reserved location "hourbcd".later from here will be copied to RTC register.
	inc r24					; increase 1 count, 0x01 becomes 0x02, houradj becomes minute adj (used as state register for future use)
	rcall ms500				; 500ms delay (if delay not provided code will skip through minute proc and save 0 before even we understand whats ahppening) gives time to release finger
	

adjustminute:
	ldi ZL,low(2*number)	; load Zpointer low with address of number array
	ldi ZH,high(2*number)	; load Zpointer high with address of number array (higher part)
	ldi XL,low(MINUTES1stdigit) ; load Xpointer with low address of MINUTES1stdigit(register to store ASCII to be displayed)
	ldi XH,high(MINUTES1stdigit);load Xpointer with high address of MINUTES1stdigit(register to store ASCII to be displayed)
	ldi r18,60				; load r18 array element counter with 60 (for 60 minutes)
	
proc1:
	rcall bcd_ascii			; call subroutine to convert BCD value to equivalent ASCII
	dec r18					; decrease element counter of array
	push ZL					; push ZL to stack as this value will change when update_matrix1 is called
	push ZH					; push ZH to stack as this value will change when update_matrix1 is called
	dec XL					; decrease XL 1 count to point to the start address as this was increneted to store the first value
	push XL					; push XL to stack as this value will change when update_matrix1 is called
	push XH					; push XH to stack as this value will change when update_matrix1 is called
	rcall update_matrix1	; call procedure to write ASCII vakues stored in hour & minutes register on LED matrix
	pop XH					; pop back XH
	pop XL					; pop XL
	pop ZH					; pop ZH
	pop ZL					; pop ZL
	
scan_button1:				; polls save button 
	lds r16,PORTA_IN		; copy PORTA pin status/values,PA6
	andi r16,0x40			; and with 0x40 for PA6 . if 0 button pressed if 0x02 button idle
	sbrs r16,6				; if idle (0x40) skip next instruction which is save th current number as hour
	rjmp saveminute			; if button pressed jump to save routine
scan_next1:					; polls next button (to increment value)
	lds r16,PORTA_IN		; copy PORTA pin status/values
	andi r16,0x20			; and with 0x20 for PA5 . if 0 button pressed if 0x20 button idle
	sbrc r16,5				; if 0 (pressed) skip next instruction else loop until either save or next button is pressed
	rjmp scan_button1		; keep polling both buttons until either one is pressed
	cpi r18,0x00			; check counter is 0, if zero start from begining (0-24 hours)
	breq adjustminute
	millis 250				; delay for visual and human needs
	rjmp proc1
saveminute:
	sts minbcd,r5			; store the selected hour BCD value in SRAM reserved location "hourbcd".later from here will be copied to RTC register.
	ldi r24,0x04			; increase 1 count, 0x01 becomes 0x02, houradj becomes minute adj
	rcall ms500				; gives time to release finger(to be increase if more time needed to save)

adjustdate:
	rcall clear_matrix1		; procedure to clear LED display
	string_write string1	; procedure to display string1 "ADJ TIME"
	rcall ms2000			; dealy 2 seconds for visual
	rcall update_matrix2	; call routine which display date/month/year registers on LED screen ( this was initialized to 0 at begining, will display 00:00:00)
YY:	ldi XL,low(DATE1stdigit); initialize XL to DATE1stdigit SRAM address which holds the date value 
	ldi XH,high(DATE1stdigit);initialize XH to DATE1stdigit SRAM address which holds the date value
	ldi r18,31				; load counter value of 31 for 31 days in the month
	ldi ZL,low(2 * number)	; initialize Z pointer to address number which has the BCD values that need to be loaded (the Z pointer was changed during string write)
	ldi ZH,high(2 * number)	; initialize Z pointer to address number which has the BCD values that need to be loaded
proc2:
	rcall bcd_ascii			; call subroutine to convert BCD value to equivalent ASCII
	dec r18					; decrease element counter of array
	push ZL					; push ZL to stack as this value will change when update_matrix2 is called
	push ZH					; push ZH to stack as this value will change when update_matrix2 is called
	dec XL					; decrease XL 1 count to point to the start address as this was increneted to store the first value
	push XL					; push XL to stack as this value will change when update_matrix2 is called
	push XH					; push XH to stack as this value will change when update_matrix2 is called
	rcall update_matrix2	; call procedure to write ASCII vakues stored in date,month & year register on LED matrix
	pop XH
	pop XL
	pop ZH
	pop ZL
scan_button2:				; polls save button 
	lds r16,PORTA_IN		; copy PORTA pin status/values,PA6
	andi r16,0x40			; and with 0x40 for PA6 . if 0 button pressed if 0x02 button idle
	sbrs r16,6				; if idle (0x40) skip next instruction which is save th current number as hour
	rjmp savedate			; if button pressed jump to save routine
scan_next2:					; polls next button (to increment value)
	lds r16,PORTA_IN		; copy PORTA pin status/values
	andi r16,0x20			; and with 0x20 for PA5 . if 0 button pressed if 0x20 button idle
	sbrc r16,5				; if 0 (pressed) skip next instruction else loop until either save or next button is pressed
	rjmp scan_button2		; keep polling both buttons until either one is pressed
	cpi r18,0x00			; check counter is 0, if zero start from begining (0-24 hours)
	breq YY
	millis 250				; delay for visual and human needs
	rjmp proc2
savedate:
	sts datebcd,r5			; store the selected hour BCD value in SRAM reserved location "hourbcd".later from here will be copied to RTC register.
	ldi r24,0x08			; increase 1 count, 0x01 becomes 0x02, houradj becomes minute adj
	rcall ms500				; gives time to release finger(to be increase if more time needed to save)

adjustmonth:
	ldi ZL,low(2*number)	; load Zpointer low with address of number array
	ldi ZH,high(2*number)	; load Zpointer high with address of number array (higher part)
	ldi XL,low(MONTH1stdigit) ; initialize XL to  MONTH1stdigit (to store month value selected)
	ldi XH,high(MONTH1stdigit); initialize XH to  MONTH1stdigit (to store month value selected)
	ldi r18,13				  ; load counter value of 13 for 12 days in the year , 0 is ignored
	
proc3:
	rcall bcd_ascii			; call subroutine to convert BCD value to equivalent ASCII
	dec r18					; decrease element counter of array
	push ZL					; push ZL to stack as this value will change when update_matrix2 is called
	push ZH					; push ZH to stack as this value will change when update_matrix2 is called
	dec XL					; decrease XL 1 count to point to the start address as this was increneted to store the first value
	push XL					; push XL to stack as this value will change when update_matrix2 is called
	push XH					; push XH to stack as this value will change when update_matrix2 is called
	rcall update_matrix2	; call procedure to write ASCII vakues stored in date,month & year register on LED matrix
	pop XH
	pop XL
	pop ZH
	pop ZL
scan_button3:				; polls save button 
	lds r16,PORTA_IN		; copy PORTA pin status/values,PA6
	andi r16,0x40			; and with 0x40 for PA6 . if 0 button pressed if 0x02 button idle
	sbrs r16,6				; if idle (0x40) skip next instruction which is save th current number as hour
	rjmp savemonth			; if button pressed jump to save routine
scan_next3:					; polls next button (to increment value)
	lds r16,PORTA_IN		; copy PORTA pin status/values
	andi r16,0x20			; and with 0x20 for PA5 . if 0 button pressed if 0x20 button idle
	sbrc r16,5				; if 0 (pressed) skip next instruction else loop until either save or next button is pressed
	rjmp scan_button3		; keep polling both buttons until either one is pressed
	cpi r18,0x00			; check counter is 0, if zero start from begining (0-24 hours)
	breq adjustmonth
	millis 250				; delay for visual and human needs
	rjmp proc3
savemonth:
	sts monbcd,r5			; store the selected hour BCD value in SRAM reserved location "hourbcd".later from here will be copied to RTC register.
	ldi r24,0x10			; increase 1 count, 0x01 becomes 0x02, houradj becomes minute adj
	rcall ms500				; gives time to release finger(to be increase if more time needed to save)
	
adjustyear:
	ldi ZL,low(2*number)	; load Zpointer low with address of number array
	ldi ZH,high(2*number)	; load Zpointer high with address of number array (higher part)
	ldi XL,low(YEAR1stdigit)  ;  initialize XL to  YEAR1stdigit (to store year value selected)
	ldi XH,high(YEAR1stdigit) ;  initialize XH to  YEAR1stdigit (to store year value selected)
	ldi r18,60				  ; load counter value of 60 for the year , the clock will die before that

proc4:
	rcall bcd_ascii			; call subroutine to convert BCD value to equivalent ASCII
	dec r18					; decrease element counter of array
	push ZL					; push ZL to stack as this value will change when update_matrix2 is called
	push ZH					; push ZH to stack as this value will change when update_matrix2 is called
	dec XL					; decrease XL 1 count to point to the start address as this was increneted to store the first value
	push XL					; push XL to stack as this value will change when update_matrix2 is called
	push XH					; push XH to stack as this value will change when update_matrix2 is called
	rcall update_matrix2	; call procedure to write ASCII vakues stored in date,month & year register on LED matrix
	pop XH
	pop XL
	pop ZH
	pop ZL
scan_button4:				; polls save button 
	lds r16,PORTA_IN		; copy PORTA pin status/values,PA6
	andi r16,0x40			; and with 0x40 for PA6 . if 0 button pressed if 0x02 button idle
	sbrs r16,6				; if idle (0x40) skip next instruction which is save th current number as hour
	rjmp saveyear			; if button pressed jump to save routine
scan_next4:					; polls next button (to increment value)
	lds r16,PORTA_IN		; copy PORTA pin status/values
	andi r16,0x20			; and with 0x20 for PA5 . if 0 button pressed if 0x20 button idle
	sbrc r16,5				; if 0 (pressed) skip next instruction else loop until either save or next button is pressed
	rjmp scan_button4		; keep polling both buttons until either one is pressed
	cpi r18,0x00			; check counter is 0, if zero start from begining (0-12 years)
	breq adjustyear
	millis 250				; delay for visual and human needs
	rjmp proc4
saveyear:
	sts yearbcd,r5			; store the selected hour BCD value in SRAM reserved location "hourbcd".later from here will be copied to RTC register.
	ldi r24,0x20			; increase 1 count, 0x01 becomes 0x02, houradj becomes minute adj
	rcall ms500				; gives time to release finger(to be increase if more time needed to save)


adjustweek:
	rcall clear_matrix1		; procedure to clear LED display
	string_write string2	; procedure to display string1 "ADJ WEEK"
	rcall ms2000			; 2 seconds display time
XX:	ldi XL,low(DAY1)		; initialize X poointer to DAY1 in sram
	ldi XH,high(DAY1)
	ldi r18,8				; r18 is loaded with 8 , we have 7 days and we are not eliminating 0 from the count as i am lazy 
	ldi ZL,low(2 * number)	; initialize Z pointer to arry "number"
	ldi ZH,high(2 * number)
proc5:
	lpm r5,Z+				; load from flash the first number from array and increase pointer
	sts weekbcd,r5			; store selected number in bcd format to weekbcd in sram
	dec r18					; decrease element counter of array
	push r18				; r18 is pushed to stack as this register will be used by show_week routine
	push ZL					; push ZL to stack as this value will change when update_matrix1 is called
	push ZH					; push ZH to stack as this value will change when update_matrix1 is called
	push XL					; push XL to stack as this value will change when update_matrix1 is called
	push XH					; push XH to stack as this value will change when update_matrix1 is called
	rcall show_week     	; call procedure to write ASCII vakues stored in hour & minutes register on LED matrix
	pop XH					; pop back XH
	pop XL					; pop XL
	pop ZH					; pop ZH
	pop ZL					; pop ZL
	pop r18
scan_button5:				; polls save button 
	lds r16,PORTA_IN		; copy PORTA pin status/values,PA6
	andi r16,0x40			; and with 0x40 for PA6 . if 0 button pressed if 0x02 button idle
	sbrs r16,6				; if idle (0x40) skip next instruction which is save th current number as hour
	rjmp saveweek			; if button pressed jump to save routine
scan_next5:					; polls next button (to increment value)
	lds r16,PORTA_IN		; copy PORTA pin status/values
	andi r16,0x20			; and with 0x20 for PA5 . if 0 button pressed if 0x20 button idle
	sbrc r16,5				; if 0 (pressed) skip next instruction else loop until either save or next button is pressed
	rjmp scan_button5		; keep polling both buttons until either one is pressed
	cpi r18,0x00			; check counter is 0, if zero start from begining (0-24 hours)
	breq XX
	millis 250				; delay for visual and human needs
	rjmp proc5
saveweek:
	sts weekbcd,r5			; store the selected hour BCD value in SRAM reserved location "hourbcd".later from here will be copied to RTC register.
	ldi r24,0x01			; increase 1 count, 0x01 becomes 0x02, houradj becomes minute adj
	rcall ms500				; gives time to release finger(to be increase if more time needed to save)
		
	rcall clockwrite		; call routine clockwrite to store hour,minute,second,date,month,year values stored in SRAM to DS3231 RTC registers
	rjmp SM					; jump to SM as time and date settings has finished


show_week:
	lds r16,weekbcd
	andi r16,0x07			; AND with 0b00000111 so that only lower 3 bits remain , 1 monday to 7 sunday
	inc r16
	ldi ZL,low(2*WEEK0)		; initiate Z pointer to address DAY which holds the alphabetical representation of week days 1-7
	ldi ZH,high(2*WEEK0)
WL:	subi ZL,-8
	dec r16
	brne WL
	subi ZL,8
	lpm r18,Z+				; load from program memory ASCII value of first letter that indicates the day stored in the new address after above calculation
	sts DAY1,r18			; store in SRAM address DAy1 the first alphabet to be dispalyed in LCD
	lpm r18,Z+				; Z incremented earlier now points to next address of the 2nd alphaber of the week day (MO,Tu,WE,TH,FR,SA,SU)
	sts DAY2,r18			; store in SRAM address DAY2 the 2nd alphabet to be displayed on the lcd
	lpm r18,Z+				; load from program memory ASCII value of first letter that indicates the day stored in the new address after above calculation
	sts DAY3,r18			; store in SRAM address DAy1 the first alphabet to be dispalyed in LCD
	lpm r18,Z+				; load from program memory ASCII value of first letter that indicates the day stored in the new address after above calculation
	sts DAY4,r18			; store in SRAM address DAy1 the first alphabet to be dispalyed in LCD
	lpm r18,Z+				; load from program memory ASCII value of first letter that indicates the day stored in the new address after above calculation
	sts DAY5,r18			; store in SRAM address DAy1 the first alphabet to be dispalyed in LCD
	lpm r18,Z+				; load from program memory ASCII value of first letter that indicates the day stored in the new address after above calculation
	sts DAY6,r18			; store in SRAM address DAy1 the first alphabet to be dispalyed in LCD
	lpm r18,Z+				; load from program memory ASCII value of first letter that indicates the day stored in the new address after above calculation
	sts DAY7,r18			; store in SRAM address DAy1 the first alphabet to be dispalyed in LCD
	lpm r18,Z+				; load from program memory ASCII value of first letter that indicates the day stored in the new address after above calculation
	sts DAY8,r18			; store in SRAM address DAy1 the first alphabet to be dispalyed in LCD
	rcall display_week
	rcall ms500
	ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

number:.db 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09
number1:.db 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19
number2:.db 0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29
number3:.db 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39
number4:.db 0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49
number5:.db 0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59		

string0: .db "ADJ TIME"
string1: .db "ADJ DATE"
string2: .db "ADJ WEEK"		

WEEK0: .db "NEXT    "
WEEK1: .db "MONDAY  "
WEEK2: .db "TUESDAY "
WEEK3: .db "WEDNSDAY"
WEEK4: .db "THURSDAY"
WEEK5: .db "FRIDAY  "
WEEK6: .db "SATURDAY"
WEEK7: .db "SUNDAY  "
WEEK8: .db "XXXXXXXX"
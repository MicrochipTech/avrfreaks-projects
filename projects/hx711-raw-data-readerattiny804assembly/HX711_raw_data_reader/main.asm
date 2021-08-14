;
; HX711_raw_data_reader.asm
;
; Created: 8/14/2021 6:00:27 PM
; Author : pappan
;
;
;
; 
; 
;This program is used for displaying the HX711 raw output(2's complement)on the LCD screen
;whatever binary value is read from the HX711 is converted into ASCII form and displayed .
;This program is useful to find out how much is the ADC count is generated by the sensor for
;a particular known weight. By placing known weights such as 5g,10g,15g,20g on the sensor
;and noting the value output on the LCD when each weight is placed,the raw ADC count for 5gm
;can be calculated. For example I got roughly 2000 counts increments for each additional 5gm
;car/automobile wheel balance weights placed on the sensor. So 2000 counts was the ADC count
;for 5gm for this specific sensor i had. Dividing 2000/5 = 400 counts for 1 gram.
;This 400 is my divisor to get the actual weight from a RAW ADC count the sensor outputs
;actual weight measured = (raw ADC count - sensor offset )x 1/400
;1/400=0.0025

;tested and verified on hardware

;LCD display Hitachi1602with I2C backpack (ali express)
;CHIPSET = HX711
;min reading = 0x800000 (2's comlpement)
;max reading = 0x7FFFFF (2's comlpement)
;y=mx+b
;m=(y-b)/x
;pin PA1 of attiny804 used to cycle PDSCK of HX711
;pin PA2 of attiny804 used to cycle DOUT of HX711 

.equ data_command1 = 0b00001001		; data control nibble ,led on P3, EN 0 on P2, R/W 0 (write) in P1 , RS 1 (0 instruction, 1 data) = 1001  =0x09
.equ data_command2 = 0b00001101		; data control nibble , 1101  = 0x0D   - EN goes hi=1
.equ data_command3 = 0b00001001		; data control nibble , 1001  = 0x09   - EN goes low=0
.equ inst_command1 = 0b00001000		;instruction control nibble ,  led on en-lo,Rw-0,rs =0   = 1000   = 0x08
.equ inst_command2 = 0b00001100		;instruction control nibble ,  led on,EN hi , rs/RW 0    = 1100   = 0x0C
.equ inst_command3 = 0b00001000		;instruction control nibble  , led on, EN lo ,rs/rw 0    = 1000   = 0x08
.EQU SLAVE_ADDRESSW = 0X4E   ; OLED = 0X78 ,1602 =4E
.equ fclk = 10000000
.DEF  SLAVE_REG = R17
.DEF  TEMP = R16
.def ZERO = r25


.macro micros
ldi temp,@0
rcall delayTx1uS
.endm

.macro millis
ldi YL,low(@0)
ldi YH,high(@0)
rcall delayYx1mS
.endm

.macro pos
ldi r16,@1
mov r6,r16
ldi r16,@0
rcall posi
.endm

.macro arrayread
ldi ZL,low(2*@0)
ldi ZH,high(2*@0)
rcall array
.endm


.DSEG
BUF4: .byte 1
BUF3: .byte 1
BUF2: .byte 1
BUF1: .byte 1
PAD1: .byte 1
PAD: .byte 1
longword: .byte 10

.CSEG
rcall PROT_WRITE
rcall i2c_init
rcall LCD_INIT
ldi r16,0b00001000
sts PORTA_DIRCLR,r16
ldi r16,0b00001000
sts PORTA_PIN3CTRL,r16  ; enable pullup resistor for pin3 portA
lds r16,PORTA_DIR
ORI r16,(1<<1)
sts PORTA_DIR,r16	; enable PA1  as output for pulling low PDSCK pin of HX711 to enable 
lds r16,PORTA_OUT
andi r16,0xFD
measure:
sts PORTA_OUT,r16	;PA1 is pulled low to activate HX711, PDSCK should be low
micros 10

rcall checkresultready
rcall readresult	;result in r13,r12,r11
mov r16,r13
andi r16,0x80		; check bit 7 is set . if set value -ve and cleared value postive
sbrc r16,7
rcall negative
rcall bin32_ascii
rcall displayresult
millis 1000
rjmp measure



negative:
clr r11
clr r12
clr r13
ret




checkresultready:
push r16
lds r16,PORTA_IN		;copy POARTA_IN value to see whether DOUT has gone low (DOUT high = data not ready)
sbrc r16,2			;check PORTA_IN value copied in r16 has bit2 cleared (DOUT low = data ready) ,PA2 is connected to DOUT
rjmp checkresultready	 	; sit in a tight loop until DOUT is ready
pop r16
ret

readresult:
ldi r16,25			;load counter value 25 = # of clock pulses
mov r10,r16			;copy to r10 which is PDSDK counter
clr r11				;data LSB
clr r12				;1st higher byte
clr r13				;2nd higher byte

read:
rcall PDSCK_HI		;raise PDSDK
rcall PDSCK_LO		;lower PDSDK
dec r10				;decrease counter
breq exitread		;If counter reach 0 branch to exit
clc					;clear SREG carry bit as base condition
lds r16,PORTA_IN	;copy PORTA_PIN register to r16 which has input value
sbrc r16,2			;skip next instruction if r16 bit2 is 0 , we cleared sreg C bit at staring
sec					;if r16 bit 2 is 1 set sreg carry bit to 1
rol r11				;rotate left through carry,bit in SREG carry will shift to BUF1 lsb and BUF1 msb will shift to carry
rol r12				;rotate left through carry,bit in SREG carry will shift to BUF2 lsb and BUF2 msb will shift to carry
rol r13				;rotate left through carry,bit in SREG carry will shift to BUF3 lsb and BUF3 msb will shift to carry
rjmp read
exitread:ret




PDSCK_HI:
push r16
lds r16,PORTA_OUT
ori r16,(1<<1)
sts PORTA_OUT,r16
pop r16
ret

PDSCK_LO:
push r16
lds r16,PORTA_OUT
andi r16,0xFD
sts PORTA_OUT,r16
pop r16
ret

rezero:
rcall checkresultready
rcall readresult 
sts BUF1,r11
sts BUF2,r12
sts BUF3,r13
ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;PROTECTED WRITE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PROT_WRITE:
		ldi r16,0Xd8
		out CPU_CCP,r16
		ldi r16,0x01			; clk prescaler of 2, 20Mhz/2 = 10Mhz
		STS CLKCTRL_MCLKCTRLB,R16
		RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;I2C ROUTINES
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

TWI_INIT:
		ldi r16,80				;load baud rate for I2C , (fclck/2*bitrate)-10 , freq = 10Mhz
		sts TWI0_MBAUD,R16		; store into baud register
		LDI R16,0b00000001		;ENABLE
		STS TWI0_MCTRLA,R16
		LDI R16,0b00001000		;FLUSH ADDR & DATA REGISTERS
		STS TWI0_MCTRLB,R16
		LDI R16,0b00000001		;FORCE IDLE
		STS TWI0_MSTATUS,R16
		ret

TWI_START:
		ldi SLAVE_REG,SLAVE_ADDRESSW
		MOV R16,SLAVE_REG		; SLAVE_REG IS R17, READ OR WRITE ADDRESS SHOULD BE LOADED HERE PRIOR TO CALL INIT
		STS TWI0_MADDR,R16
		RCALL WAIT_WIF
		RET

TWI_WRITE:
		;rcall CHK_CLKHOLD
		MOV R16,SLAVE_REG		;mov data from SLAVE_REG/r17 to r16
		STS TWI0_MDATA,R16		;copy r16 to MDATA register for I2C transmission
		RCALL WAIT_WIF			;call WAIT_WIF subroutine to check write interrupt flag is set after writing finished
		micros 30
		RET




TWI_STOP:
		LDI R16,0b00000011       ;STOP
		STS TWI0_MCTRLB,R16
		RET


WAIT_WIF:
		LDS R16,TWI0_MSTATUS
		SBRS R16,6			;CHECK WIF IS SET,IF SET SKIP NEXT INSTRUCTION
		RJMP WAIT_WIF
		ret


i2c_init:
		LDI SLAVE_REG,SLAVE_ADDRESSW
 		RCALL TWI_INIT
		rcall TWI_START
		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;LCD ROUTINES
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
COMMAND_WRITE:
		STS PAD,R17				;copy SLAVE_REG to SRAM address PAD for processing
		ANDI R17,0XF0			;preserve upper nibble of SLAVE_REG by anding with 0xF0 ,lower 4 becomes 0
		ORI R17,inst_command1	;add instruction_command1 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE			;call TWI_WRITE routine to transmit command data to LCD
		micros 20
		ORI R17,inst_command2	;add instruction_command2 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE			;call TWI_WRITE routine to transmit command data to LCD
		micros 20
		ORI R17,inst_command3	;add instruction_command3 to lower nibble of r17 by OR ing it
		RCALL TWI_WRITE			;call TWI_WRITE routine to transmit command data to LCD
		micros 20		
		LDS R17,PAD				;copy back data from PAD to r17 for processing the remaining lower nibble
		SWAP R17				;swap the nibbles in R17 so thlower nibble will occupy the upper area of reg
		ANDI R17,0XF0			;preserve upper nibble of SLAVE_REG by anding with 0xF0
		ORI R17,inst_command1	;add instruction_command1 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE			;call TWI_WRITE routine to transmit command data to LCD
		micros 20
		ORI R17,inst_command2	;add instruction_command2 to lower nibble of r17 by OR ing it
		RCALL TWI_WRITE			;call TWI_WRITE routine to transmit command data to LCD
		micros 20
		ORI R17,inst_command3	;add instruction_command3 to lower nibble of r17 by OR ing it
		RCALL TWI_WRITE			;call TWI_WRITE routine to transmit command data to LCD
		micros 20
		RET	


DATA_WRITE:
		STS PAD,R17				;copy SLAVE_REG to SRAM address PAD for processing
		ANDI R17,0XF0			;preserve upper nibble of SLAVE_REG by anding with 0xF0,lower 4 becomes 0
		ORI R17,data_command1	;add data_command1 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE			;call TWI_WRITE routine to transmit command data to LCD
		micros 20
		ORI R17,data_command2	;add data_command2 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE			;call TWI_WRITE routine to transmit command data to LCD
		micros 20
		ORI R17,data_command3	;add data_command3 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE			;call TWI_WRITE routine to transmit command data to LCD
		micros 20
		LDS R17,PAD				;copy back data from PAD to r17 for processing the remaining lower nibble
		SWAP R17				;swap the nibbles in R17 so thlower nibble will occupy the upper area of reg
		ANDI R17,0XF0			;preserve upper nibble of SLAVE_REG by anding with 0xF0
		ORI R17,data_command1	;add data_command1 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE			;call TWI_WRITE routine to transmit command data to LCD
		micros 20
		ORI R17,data_command2	;add data_command2 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE			;call TWI_WRITE routine to transmit command data to LCD
		micros 20
		ORI R17,data_command3	;add data_command3 to lower nibble of r17 by OR ing it 
		RCALL TWI_WRITE			;call TWI_WRITE routine to transmit command data to LCD
		micros 20
		RET	

LCD_INIT:
		millis 50
		LDI R17,0b00111100
		RCALL TWI_WRITE
		micros 20
		LDI R17,0B00111000
		RCALL TWI_WRITE
		millis 50		; value loaded here (20) decides the number of milli seconds in the delay below
 		

		
		LDI R17,0b00111100
		RCALL TWI_WRITE
		micros 20
		LDI R17,0B00111000
		RCALL TWI_WRITE
		millis 50		; value loaded here (20) decides the number of milli seconds in the delay below
 		

		
		LDI R17,0b00111100
		RCALL TWI_WRITE
		micros 20
		LDI R17,0B00111000
		RCALL TWI_WRITE
		millis 50		; value loaded here (20) decides the number of milli seconds in the delay below
 		

		
		LDI R17,0b00101100
		RCALL TWI_WRITE
		micros 20
		LDI r17,0b00101000
		RCALL TWI_WRITE
		millis 50		; value loaded here (20) decides the number of milli seconds in the delay below
 		

		
		LDI R17,0b00101000
		RCALL COMMAND_WRITE
		micros 20

		
		LDI R17,0b00001100
		RCALL COMMAND_WRITE
		micros 20

		
		LDI R17,0b00000110
		RCALL COMMAND_WRITE
		micros 20

		
		LDI R17,0b00000001
		RCALL COMMAND_WRITE
		micros 20
		RET



posi:
		cpi r16,0x00		;check the first parameter of macro is 0 or a higher number
		breq line1			;if zero go to label first line
		ldi r16,0xc0		; if not zero the input is for 2nd line. load 0xC0 in r16 which is address of 2nd line first position DDRAM
		add r6,r16			; add with horizontal postion on 2nd line to get the correct DDRAM address
		mov SLAVE_REG,r6	; copy new LCD DDRAM address to SLAVE_REG
		rcall COMMAND_WRITE	; call TWI transmit command
		ret
	line1:
		ldi r16,0x80		; if Y = 0 which means 1st line of LCD load address of 1st line 1st position DDRAM =0x80
		add r6,r16			; add 0x80 to X position saved in r6 to get the start postion on 1st line
		mov SLAVE_REG,r6	; copy new LCD DDRAM address to SLAVE_REG
		rcall COMMAND_WRITE	; call TWI transmit command
		ret


array:
		lpm SLAVE_REG,Z+	;load from program memory to r17 data pointed by Z pointer
		cpi SLAVE_REG,0		; check for null terminator in the string
		breq exit			; if zero go to exit
		rcall DATA_WRITE	; transmit copied data to LCD via TWI
		rjmp array			; jump back to array until null
	exit: 	ret


cleardisplay:
		LDI R17,0b00000001
		RCALL COMMAND_WRITE
		micros 20
		RET



displayresult:
pos 0, 0
arrayread message3
pos 1, 0
ldi r18,10
ldi     ZH,high(longword)
ldi		ZL,low(longword)
writeloop:
ld r17,Z+
rcall DATA_WRITE
dec r18
brne writeloop
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

message1: .db "Calibraing scale to zero ", 0
message2: .db " Readyxxfor weighing " , 0
message3: .db " weight " ,0



;*************************************************************************
;*                                                                       *
;*                      Convert unsigned 32 bit to ASCII                 *
;*                                                                       *
;*              Author: Peter Dannegger                                  *
;*                      danni@specs.de                                   *
;*                                                                       *
;*************************************************************************
;
;input: R31, R30, R29, R28 = 32 bit value 0 ... 4294967295
;output: R25, R24, R23, R22, R21, R20, R19, R18, R17, R16 = 10 digits (ASCII)
;
bin32_ascii:
		mov r28,r11
		mov r29,r12
		mov r30,r13
		clr r31

        ldi     r19, -1 + '0'
		mov		r0,r19
_bcd1:  inc     r0
        subi    r29, byte2(1000000000)  ;-1000,000,000 until overflow
        sbci    r30, byte3(1000000000)
        sbci    r31, byte4(1000000000)
        brcc    _bcd1

        ldi     r19, 10 + '0'
		mov		r1,r19
_bcd2:  dec     r1
        subi    r29, byte2(-100000000)  ;+100,000,000 until no overflow
        sbci    r30, byte3(-100000000)
        sbci    r31, byte4(-100000000)
        brcs    _bcd2

        ldi     r19, -1 + '0'
		mov		r2,r19
_bcd3:  inc     r2
        subi    r28, byte1(10000000)    ;-10,000,000
        sbci    r29, byte2(10000000)
        sbci    r30, byte3(10000000)
        sbci    r31, 0
        brcc    _bcd3

        ldi     r19, 10 + '0'
		mov		r3,r19
_bcd4:  dec     r3
        subi    r28, byte1(-1000000)    ;+1,000,000
        sbci    r29, byte2(-1000000)
        sbci    r30, byte3(-1000000)
        brcs    _bcd4

        ldi     r19, -1 + '0'
		mov		r4,r19
_bcd5:  inc     r4
        subi    r28, byte1(100000)      ;-100,000
        sbci    r29, byte2(100000)
        sbci    r30, byte3(100000)
        brcc    _bcd5

        ldi     r19, 10 + '0'
		mov		r5,r19
_bcd6:  dec     r5
        subi    r28, byte1(-10000)        ;+10,000
        sbci    r29, byte2(-10000)
        sbci    r30, byte3(-10000)
        brcs    _bcd6

        ldi     r19, -1 + '0'
		mov     r6,r19
_bcd7:  inc     r6
        subi    r28, byte1(1000)          ;-1000
        sbci    r29, byte2(1000)
		brcc    _bcd7

        ldi     r19, 10 + '0'
		mov     r7,r19
_bcd8:  dec     r7
        subi    r28, byte1(-100)          ;+100
        sbci    r29, byte2(-100)
        brcs    _bcd8

        ldi     r19, -1 + '0'
		mov		r8,r19
_bcd9:  inc     r8
        subi    r28, 10                 ;-10
        brcc    _bcd9

        subi    r28, -10 - '0'
        mov     r9, r28
		ldi     ZH,high(longword)
		ldi		ZL,low(longword)
		st		Z+,r0
		st		Z+,r1
		st		Z+,r2
		st		Z+,r3
		st		Z+,r4
		st		Z+,r5		
		st		Z+,r6
		st		Z+,r7
		st		Z+,r8
		st		Z+,r9
		ret
;-------------------------------------------------------------------------

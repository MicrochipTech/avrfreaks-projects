;**** PID CONTROLLER ************************
;* Title:		PID Controller
;* Author:		John Brooke
;* Version:		PIDv2_5
;* Last updated:	6 December 2001
;* Target:		AT90S4433
;* Support E-mail:	brookej@student.gvsu.edu
;***************************************************************************
; DIETCOKEWARE LICENSE - THIS SOFTWARE MAY BE USED BY OTHERS IF AND ONLY IF
;  THEY BUY ME DIET COKE.  AT LEAST ONE 12 OUNCE CAN PER MICROPROCESSOR
;  APPLICATION.  OTHER NEGOTIABLE TERMS APPLY IF YOU MAKE A PROFIT.
;***************************************************************************
;     Vcc                #################################    +--+---+---+---+---+--+
;      |                 #Varitronix  | SPxxx.xu/m MANL  #    | S|  S|  S|  S|  S|  |
;   ########             #  LCD-64    | PVxxx.x %Out99.9 #    | W+  W+  W+  W+  W+  |
;   #MCP130#             #------------+------------------#    | 5 / 4 / 3 / 2 / 1 / |
;   #   RST#-+      Vcc  #G V C D     D D D D D D D D V g# Vcc|  /   /   /   /   / _|_
;   # -475 # |       |   #n c o I   E 0 1 2 3 4 5 6 7 c n#  | | P|  M|  S|  I|  D|  =
;   ######## |       |   #d#c#n#######################c#d#  | | [3] [3] [3] [3] [3]
;     _|_    +       +----|-+ | | | | | | | | | | | | +-|---+ | [K] [K] [K] [K] [K]
;      =      \      |  +-+---|-|-+-|-+-+-+-+-|-|-|-|---+-----+  |R1 |R2 |R3 |R4 |R5
;RS232         \ R10[5] |     | +---|------+--|-|-|-|------------+   |   |   |   |      
;Tx >------+    \   [1][5]R11 +     |      |  | | | +----------------+   |   |   |
;Rx <-----+ \    \  [0][1]   /    +-+      |  | | +-|--------------------+   |   |
;Gnd--+    \ \    \  |  |   /    /         |  | +-|-|------------------------+   |  
;    _|_    \ \    \ +--+--+    /AT90S4433 |  +-|-|-|----------------------------+
;     =      \ \    \          + ######### |  | | | |  Analog Input:    Analog Output:
;       +-----\-\--+ +---------|-#Rst PC5#-|--|-|-|-|-----< 1-5         +---+------>
;    R16|  +---\-\--\----------|-#PD0 PC4#-+  | | | |  +--<             |   |   2-10V
;      [4][4]   \ \  +---------|-#PD1 PC3#----|-|-|-+  |                |  [2]
; Vcc  [7][7]R17 \ \    +--+   | #PD2 PC2#----|-|-+   _|_               |  [4]R8-Load1
;  | +--+  +----+ + +  _|_ | +-|-#PD3 PC1#----|-+      =                |  [9] 4-20mA
;  | |  ####### | | |C3___ | | +-#PD4 PC0#----+                         |   |
;  | +--#Ti Ro#-+ | |.1u|F +-|---#Vcc AGn#----------------------+     +-|---+------>
;  +--+ #   Tx#---+ |   +----|---#Gnd ARf#----+     LTC1661     |     | |   |    1-5V
; _|_ | #   Rx#-----+  _|_  +|---#XL1 AVc#----+    #########    |     | |  [2]
; ___ | #   V-#---+     =  / +   #XL2 PB5#----|----#CS\ VOA#----|--+  | |  [4]R9-Load2
;.1|uF| #   C-#---|-+     / /    #PD5 PB4#----|----#SCK Gnd#----+  |  | |  [9] 4-20mA
;C1+--|-#Gn C+#-+ | |    + + +---#PD6 PB3#----|----#Din Vcc#-+  | [1] | |   | 
;  |  | MAX233# | | |    | | | +-#PD7 PB2#-+  |  +-#Vrf VOB# |  | [0] | |   +------>
;  |  +-#Vcc  # | | |    | | | | #PB0 PB1# |  |  | ######### |  | [K] | |   |    Com
;  +----#Gn V-#-|-+ +    + | | | ######### |  |  +-----+-----+  |  |R7| |  _|_
; _|_ +-#C- C+#-+  /    /  | | |           |  |        | Vcc |  |  |  | |   =     +12V
;  =  | #######   /    /   + | +    +--+   |  |L1 10uH |  |  |  |  |  | |           |
;     +----------+    +   /  |  \  _|_ |   |  +-\_/\_/-+--+  |  |  |  | | ######### |
;Vcc                  |  /   |   \  =  |   |  |        |     |  |  |  | +-#Ot1 +Vs#-+
; |  +------------+   | +    |    +    |   | _|_    R6[2]    |  |  |  +---#In- Ot2#-
; +--+  ######### |   + | R13| R14| R15|   | ___C4    [K]    |  |  +------#In+ In-#-
;   _|_ #    Vcc#-+  / [3]  [3]  [3]  [1]  |  |.1uF   _|_   _|_ | _|_   +-#-Vs In+#-
; C2___ #CTX154 #   /  [K]  [K]  [K]  [K]  | _|_      \ / C5___ | ___C6 | #########
;.1uF|  #3.68MHz#  / R12|    |    |    |   |  =   LED1-+-.1uF|  |  |.1uF|   LM358
;    +--#Gnd CLK#-+     v    v    v    v   |      ALARM|     +--+--+    |
;   _|_ #########   Rem/Loc Decr Incr Com  +-----------+       _|_    -12V
;    =                 HARDWIRE TERM                            =
;**********************************************************************************
;* FP MATH DESCRIPTION:  valueExp:valueHigh:valueLow
;*   +/-  [10^[+/- 6bit exponent]  *  . [bcd3] [bcd2] [bcd1] [bcd0] 
;*  [bit0]    [bit7.1 signed]		 [high byte]   [low byte]
;*
;*think of math routines working like an HP calculator with RPN logic:
;* pushx=Enter, popx=DownArrow, fpmuly_x= TimesKey, fpaddy_x= PlusKey,
;* fpsuby_x= MinusKey, fprec1_x= RecipicleKey, fpsqrt_x= SquareRootKey, etc
;* (unfortunately fpsqrt wipes out entire fpstack, and don't like negative exponents)
;*
;* zero is defined as .00000 * 10^-9  fpE:fpH:fpL = EE:0000
;* one is defined as .10000 * 10^1  fpE:fpH:fpL = 02:199A
;*
;* fpadd - 4bcdEn + 4bcdEn addition
;* fpsub - 4bcdEn - 4bcdEn subtraction
;* fpmul - 4bcdEn * 4bcdEn multiplication
;* fprec - 1 / 4bcdEn division, this routine does not use the fpstack.
;* fpsqrt - sqrt(4bcdEn), this routine uses entire stack, only fpx variable
;*            is preserved in ^fpy up (fpt=fpz=fpy= original fpx)
;***************************************************************************
.def	fpzL	=r0		;temporary floating point variable z
.def	fpzH	=r1
.def	fpzE	=r2
.def	fptL	=r3		;temporary floating point variable t
.def	fptH	=r4
.def	fptE	=r5

.def	STATUS	=r6		;status byte

.def	spL	=r7		;floating point setpoint
.def	spH	=r8
.def	spE	=r9
.def	pvL	=r10		;floating point process value
.def	pvH	=r11
.def	pvE	=r12
.def	outL	=r13		;floating point output
.def	outH	=r14

.def	nctr	=r15		;number counter
.def	ntmp	=r15		;temporary byte

.def	fpxL	=r16		;temporary floating point variable x
.def	fpxH	=r17 
.def	fpxE	=r18
.def	fpyL	=r23		;temporary floating point variable y
.def	fpyH	=r24
.def	fpyE	=r25
 
.include "4433def.inc"

	rjmp	RESET		;reset handler
	rjmp	EXT_INT0	;IRQ0 handler
	rjmp	EXT_INT1	;IRQ1 handler
	rjmp	TIM1_CAPT	;Timer1 Capture handler
	rjmp	TIM1_COMP	;Timer1 Compare handler
	rjmp	TIM1_OVF	;Timer1 Overflow handler
	rjmp	TIM0_OVF	;Timer0 Overflow handler
	rjmp	SPI_STC		;SPI Transfer Complete handler
	rjmp	UART_RXC	;UART RX Complete handler
	rjmp	UART_DRE	;UDR Empty handler
	rjmp	UART_TXC	;UART TX Complete handler
	rjmp	ADC		;ADC Conversion Complete Interrupt handler
	rjmp	EE_RDY		;EEPROM Ready handler
	rjmp	ANA_COMP	;Analog Comparator handler

;**** initialize lcd *******************************************************
init_lcd:
			;change PC4.0 to outputs to drive the LCD.
	ldi	r16,0x20	;PC5  PC4  PC3  PC2  PC1  PC0
	out	PORTC,r16	; 1    0    0    0    0    0
	ldi	r16,0x1F	;Ana5  RS  DB7  DB6  DB5  DB4 
	out	DDRC,r16	; in  out  out  out  out  out

	rcall	delay100ms
	ldi	r17,0x02	;command to set lcd to 4-bit operation
	rcall	lcd_push8	;ditto
	ldi	r16,0x00	;command to turn on display, cursor, blink
	ldi	r17,0x0E
	rcall	lcd_push4
	ldi	r16,0x02	;command to define 2 lines, 5x7 matrix
	ldi	r17,0x08
	rcall	lcd_push4
	ldi	r16,0x00	;command for entry mode, increment cursor
	ldi	r17,0x06	; and no display shift
	rcall	lcd_push4
	ldi	r16,0x00	;command to clear display
	ldi	r17,0x01
	rcall	lcd_push4
				;change PC4.0 back to inputs.
	ldi	r16,0x00	;PC5  PC4  PC3  PC2  PC1  PC0
	out	DDRC,r16	; in   in   in   in   in   in
	ldi	r16,0x3F	;Ana5  RS  DB7  DB6  DB5  DB4 
	out	PORTC,r16	; 1pu  1pu  1pu  1pu  1pu  1pu
	ret

;**** output to LCD and UART ************************************************
;EACH byte of the SRAM DISPLAY is sent to the LCD and to the UART RX at 9600
; baud.  A carraige return is sent on the UART at the end of the 32 bytes.
; This code drives a 16x2 Hiatachi 44780 based LCD and since only the write
; mode is used, it does not have to be present.  There is a 1.75ms delay
; between LCD nibble commands. Any faster and the LCD-64 Varitronix freaks
; out.  The 9600 baud rate takes 1.04us for 10 bits, so 3.33ms gives enough
; time for the 9600baud rate to finish up between bytes.

.equ	LCDLINE1=$60		;start address of LCD upper line
.equ	LCDLINE2=$70		;start address of LCD lower line
.def	n	=r18		;loop counter: length of LCD line

out_lcd:		;change PC4.0 to outputs to drive the LCD.
	ldi	r16,0x20	;PC5  PC4  PC3  PC2  PC1  PC0
	out	PORTC,r16	; 1    0    0    0    0    0
	ldi	r16,0x1F	;Ana5  RS  DB7  DB6  DB5  DB4 
	out	DDRC,r16	; in  out  out  out  out  out
	rcall	delay300us	;give port time to switch over

	ldi	r16,0x08	;command to set DDRAM Address
	ldi	r17,0x00
	rcall	lcd_push4

	ldi	ZH,high(LCDLINE1)	;init z-pointer to lcd line#1
	ldi	ZL,low(LCDLINE1)
	ldi	n,16
lcd_line1:
	ld	r16,Z		;get data from LCDLINE1
	out	UDR,r16				;WRITE DATA TO UART
	swap	r16		;create high nibble
	andi	r16,0x0F
	ori	r16,0x10	;set D/I bit
	ld	r17,Z+		;create low nibble
	andi	r17,0x0F
	ori	r17,0x10	;set D/I bit
	rcall	lcd_push4	
	dec	n
	brne	lcd_line1	;if not done, loop more
	ldi	r16,0x0C	;command to set DDRAM Address
	ldi	r17,0x00
	rcall	lcd_push4
	ldi	n,16
lcd_line2:
	ld	r16,Z		;get data from LCDLINE2
	out	UDR,r16				;WRITE DATA TO UART
	swap	r16		;create high nibble
	andi	r16,0x0F
	ori	r16,0x10	;set D/I bit
	ld	r17,Z+		;create low nibble
	andi	r17,0x0F
	ori	r17,0x10	;set D/I bit
	rcall	lcd_push4	
	dec	n
	brne	lcd_line2	;if not done, loop more
				;change PC4.0 back to inputs.
	ldi	r16,0x00	;PC5  PC4  PC3  PC2  PC1  PC0
	out	DDRC,r16	; in   in   in   in   in   in
	ldi	r16,0x3F	;Ana5  RS  DB7  DB6  DB5  DB4 
	out	PORTC,r16	; 1pu  1pu  1pu  1pu  1pu  1pu
	
	ldi	r16,13			;SEND CARRAIGE RETURN TO UART
	out	UDR,r16	
	ret
	
;**** lcd_push *********************************************************
lcd_push4:
	sbi	PORTD,4		;toggle E high
	nop
	out	PORTC,r16	;send high nibble
	nop
	cbi	PORTD,4		;toggle E low
lcd_push8:
	sbi	PORTD,4		;toggle E high
	nop
	out	PORTC,r17	;send low nibble or high byte (8-bit) command
	nop
	cbi	PORTD,4		;toggle E low
	rcall	delay1_75ms	;LCD takes .059sec to update (1.5ms too fast)
	ret
	
;**** initialize adc ********************************************************
init_adc:
	ldi	r16,0x05	;select adc input channel PC5
	out	ADMUX,r16
	ldi	r16,0xA3	;enable adc, free run mode, divide by 128
	out	ADCSR,r16	;  set bits ADEN, ADFR, ADPS2.0=110 
	sbi	ADCSR,ADSC	;start adc conversion
	ret

;**** initialize uart *******************************************************
init_uart:
	ldi	r16,0x9A	;select uart rcv interupt & rcv/xmt enable
	out	UCSRB,r16	;	RXCIE+RXEN+TXEN
	ldi	r16,23		;23=9600   95=2400	
	out	UBRR,r16	;baud rate for 9600bps @ 3.6864MHz
	ret

;**** fractional second delays **********************************************
delay100ms:		;used init_lcd 		3.6864MHz = .27us per clock
	ldi	r20,10		;linear term            10 * 100^2 iterations
	ldi	r21,100		;square term		~103ms
	rjmp	delay		;3/100 =  ~30ms
;delay14_9ms:			;this makes lcd update exactly 1.00s
;	ldi	r20,144		;		        144 * 10^2 iterations
;	ldi	r21,10		;72/10 =  7.5ms
;	rjmp	delay
delay5ms:
	ldi	r20,48		;96/10 = ~10.0ms
	ldi	r21,10		;9/10  =  ~.96ms
	rjmp	delay		;29/10 =  ~3.0ms
delay1_75ms:			;this is a more stable speed for the LCD-64
	ldi	r20,187		; update time for LCD = .059sec
	ldi	r21,3		;162/3 = ~1.50ms 	215/3 = ~2.00ms
	rjmp	delay		;179/3 = ~1.67ms is the fastest the LCD-64 can
				; be stroked. Update time for LCD = .056sec
delay1ms:
	ldi	r20,108		;108/3 = ~1.00ms
	ldi	r21,3
	rjmp	delay
delay300us:
	ldi	r20,32		;32/3 =  ~300us
	ldi	r21,3		;11/3 =  ~100us
	rjmp	delay
		
delay:	dec	r20
	breq	delaydone
	mov	r22,r21
delay2:	dec	r22
	breq	delay
	mov	r23,r21
delay3:	dec	r23
	breq	delay2
	rjmp	delay3
delaydone:
	ret

;***************************************************************************
;***************************************************************************
;   M   M   AA   TTT  H  H     RRR    OO   U  U  TTT III  N  N  EEE   SSS
;   MM MM  A  A   T   H  H     R  R  O  O  U  U   T   I   NN N  E    S
;   M M M  A  A   T   HHHH     R  R  O  O  U  U   T   I   N NN  EEE   SS
;   M M M  AAAA   T   H  H     RRR   O  O  U  U   T   I   N NN  E       S
;   M   M  A  A   T   H  H     R  R   OO    UUU   T  III  N  N  EEE  SSS
;***************************************************************************
;     all fp routines written by john brooke   (see dietcokeware license)
;***************************************************************************
;***** Subroutine Register Variables
.def	rsL	=r16		;rsL - final fp result low byte
.def	rsH	=r17		;rsH - final fp result high byte
.def	rsE	=r18		;rsE - final fp result exponent

.def	adL	=r16		;adL - addend low byte
.def	adH	=r17		;adH - addend high byte
.def	adE	=r18		;adE - addend exponent+sign

.def	sbL	=r16		;sbL - subtrahend low byte
.def	sbH	=r17		;sbH - subtrahend high byte
.def	sbE	=r18		;sbE - subtrahend exponent+sign

.def	mpL	=r16		;mpL - multiplier low byte
.def	mpH	=r17		;mpH - multiplier high byte
.def	mpE	=r18		;mpE - mulitplier exponent+sign

.def	dvL	=r16		;dvL - divisor low byte
.def	dvH	=r17		;dvH - divisor high byte
.def	dvE	=r18		;dvE - divisor exponent+sign

.def	dctr	=r15		;loop counter for div16u
.def	mctr	=r15		;loop counter for mul16u
.def	qtmp	=r15		;qtmp - temporary swap byte
.def	cpy0	=r15		;cpy0 - copy of rs0 (free register?) <<<<<<<
			;r15 can be used only if out has no or an assumed exponent
.def	cpy1	=r16		;cpy1 - copy of rs1 for mul32x10
.def	cpy2	=r17		;cpy2 - copy of rs2 for mul32x10
.def	cpy3	=r18		;cpy3 - copy of rs3 for mul32x10

			;product 32bit, quotient 16bit, sum 17bit, difference 16bit
.def	rs0	=r19		;rs0 - result byte 0 (init as mpL) 
.def	rs1	=r20		;rs1 - result byte 1 (init as mpH)
.def	rs2	=r21		;rs2 - result byte 2
.def	rs3	=r22		;rs3 - result byte 3 (MSB)

.def	nL	=r19		;nL - divisor (scaling factor) used in divx_n  (x/n)
.def	nH	=r20		;nH
.def	rmL	=r21		;rmL - remainder used in divx_n
.def	rmH	=r22		;rmH

.def	agL	=r23		;agL - augend low byte
.def	agH	=r24		;agH - augend high byte
.def	agE	=r25		;agE - augend exponent+sign

.def	miL	=r23		;miL - minuend low byte
.def	miH	=r24		;miH - minuend high byte
.def	miE	=r25		;miE - minuend exponent+sign

.def	mcL	=r23		;mcL - multiplicand low byte
.def	mcH	=r24		;mcH - multiplicand high byte
.def	mcE	=r25		;mcE - multiplicand exponent+sign

.def	ddL	=r23		;ddL - dividend low byte
.def	ddH	=r24		;ddH - dividend high byte
.def	ddE	=r25		;ddE - dividend exponent+sign

.def	xexp	=r28		;temp storage for x-exponent
.def 	atmp	=r28		;temp working storage
.def	yexp	=r29		;temp storage for y-exponent
.def	sign	=r29		;temp storage for sign
.def	lctr	=r29		;loop counter for mul16u

;***** floating point subtract **********************************************
fpsuby_x:			;diff = [mi] - sb
				; fpx =  fpy - fpx     fpy = fpz     fpz = fpt
	ldi	atmp,0x01	;to subtract, must only change sign of addend
	eor	sbE,atmp			;    which is, the subtrahend
;***** floating point add ***************************************************
fpaddy_x:			;sum  = [ag] + ad
	mov	xexp,adE	; fpx =  fpy + fpx     fpy = fpz     fpz = fpt
	asr	xexp		;decode exponents these must be equal to add.
	mov	yexp,agE
	asr	yexp
	cp	xexp,yexp	;compare exponents, want larger exp in fpyE
	brlt	eqlz_exp	; if fpxE < fpyE then branch
	rcall	swapy_x		; if not, then place larger exponent in fpy
	mov	qtmp,xexp
	mov	xexp,yexp
	mov	yexp,qtmp	; also swap decoded exponents
eqlz_exp:
	cp	xexp,yexp	;compare exponents, equalize fpx 
	breq	tst_mant	; if exponents equalized, next test mantissas
	ldi	nL,10		;load divisor of 10
	clr	nH
	rcall	divx_n		; if not, divide fpx-mantissa by 10
	inc	xexp		;  and increment decoded fpx-exponent by 1
	rjmp	eqlz_exp
tst_mant:		;x,y
	cp	adL,agL		;compare mantissas, want larger mantissa in fpx
	cpc	adH,agH		;  this must be an unsigned compare!!!!!
	brsh	chk_signs	;if fpx >= fpy, then good to go
	rcall	swapy_x		;if not, swap larger into fpx
chk_signs:
	andi	adE,0x01	;mask sign of final result, keep in adE
	eor	agE,adE		;compute same or different signs in agE
	sbrc	agE,0		;check if signs same or different
	rjmp	must_sub	; if agE bit0=1 then signs differnt, must subtract
must_add:		
	add	adL,agL		; else add because signs same
	adc	adH,agH
	brcc	calcE		;test if carry clear, if so mantissa valid
		; if carry set must divide 17bit mantissa by 10 and increment exponent
	ror	adH		;if carry was set divide by 2 to 16bit result
	ror	adL
	ldi	nL,5		; then divide by 5 with divide x/n unsigned divide 
	clr	nH
	rcall	divx_n
	inc	xexp		; and increment decoded fpx-exponent by 1
calcE:
	lsl	xexp		;reencode sign
	or	adE,xexp
popfpaddstack:
	mov	fpyL,fpzL		; copy fpy <= fpz
	mov	fpyH,fpzH
	mov	fpyE,fpzE
	mov	fpzL,fptL		; copy fpz <= fpt
	mov	fpzH,fptH
	mov	fpzE,fptE	
	ret
must_sub:		;if must subtract then popfpsubstack is done the fpmuly_x
	sub	adL,agL		;subtract because signs different
	sbc	adH,agH
	lsl	xexp		;reencode sign
	or	adE,xexp	
	rjmp	fpsubnormlz_x	;must normalize since can be very small

;***************************************************************************
;* "divx_10" - 16/16 Bit Unsigned Division  (adapted from an Atmel AppNote)
;*This routine divides the mantissa of x by n, used to create an unnormalized
;  mantissa needed for addition and subtraction.  Does not use exponent.
;***************************************************************************
divx_n:			;fpx = [ad] / n     (x=x/n)     fpt,fpz,fpy unchanged
	ldi	rmL,17		;init loop counter (need to use lower byte)
	mov	dctr,rmL	; ditto
	clr	rmL		;clear remainder Low byte
	sub	rmH,rmH		;clear remainder High byte and carry
	rjmp	dnxt_2
dloop:	rol	rmL		;shift dividend into remainder
	rol	rmH
	sub	rmL,nL		;remainder = remainder - divisor
	sbc	rmH,nH		;
	brcc	dnxt_1		;if result negative
	add	rmL,nL		;   restore remainder
	adc	rmH,nH
	clc			;   clear carry to be shifted into result
	rjmp	dnxt_2		;else
dnxt_1:	sec			;   set carry to be shifted into result
dnxt_2:	rol	adL		;shift left dividend
	rol	adH
	dec	dctr		;decrement counter
	brne	dloop		;if not done, loop
	ret			; else return

;******* normalization ******************************************************
;to normalize a number improperly entered, multiply it times a floating point
; 1.000 (all mantissas need be between .1000 (199A) and .9999 (FFFF), or zero
; (0000).  The multiply routine shifts the mantissa by a factor of ten and
; the exponent until one of these values is obtained.  fpt value lost
fptimes1_0:			;prod = [mc] * mp ,  where mc=1.000
normalize_x:			; fpx =  fpy * fpx     fpy=fpy   fpz=fpz   fpt=fpz
	rcall	loady_x		;push numbers up fpstack
fpsubnormlz_x:		;subtraction can produce non-standard numbers, so
			; must normalize to .1000 to .9999 mantissa
	ldi	mcL,0x9A	;load floating point +1.000
	ldi	mcH,0x19
	ldi	mcE,0x02
	rjmp	fpmuly_x	;multiply times x,  x = 1.0000 * x

;***** floating point multiply ********************************************** 
;since all numbers are represented by a matissa between FFFF (.99999) and 
; 199A (.10000), except zero 0000, (.00000).  Result is found in the upper 16
; bits.  If the answer is less than 199A then the answer must be multiplied by
; 10 and exponent decremented to obtain a valid mantissa. 10*199A^2=199A.6668.
; Floating point routine then moves the result to rsH:rsL:rsE
;
;***** 16bit floating point multiply:  mcE:mcH:mcL * mpE:mpH:mpL = rsE:rsH:rsL
fpmuly_x:			;prod = [mc] * mp
 	rcall	mul16u		; fpx =  fpy * fpx     fpy = fpz     fpz = fpt
			;exponent is bits7.1 (bit7 is exponent sign)
	mov	xexp,mcE	;to convert exponent to addable number: asr xx	
	asr	xexp		;decode exponent
	mov	yexp,mpE
	asr	yexp		;decode exponent
 	add	xexp,yexp	;add exponents
			;sign is bit0, 1=negative, 0=positive
	mov	sign,mcE	;calculate sign	 - * - = +   1 + 1 = 0
	add	sign,mpE	;                - * + = -   1 + 0 = 1
	andi	sign,0x01	;                + * + = +   0 + 0 = 0
testzero:
	cpi	rs3,0x00	;test if mantissa is zero
	brne	chk_man
	cpi	rs2,0x00	;ditto
	brne	chk_man
	cpi	rs1,0x00	;ditto
	brne	chk_man
	cpi	rs0,0x00	;ditto
	breq	mak_zero
chk_man:		;check if matissa is less than .1000
	cpi	rs3,0x19	;compare result high byte with high(6554) 
	brlo	shiftfp		;branch if rs3 < 0x19
	cpi	rs3,0x19
	brne	man_ok		;branch if rs3 > 0x19
			;at this point rs3=0x19; check lower byte
	cpi	rs2,0x9A	;compare result low byte with low(6554)
	brsh	man_ok		;branch if rs2 >= 0x9A
shiftfp:
	rcall	mul32x10	;if matissa less than .1000, multiply by 10
	dec	xexp		;decrement exponent if was multiplied by 10
	rjmp	chk_man
man_ok:			;mantissa ok	
	mov	rsL,rs2		;move mantissa to proper result registers
	mov	rsH,rs3
			;if exponent is less than -9 make zero
	cpi	xexp,-9	;compare exponent
	brge	exp_ok		;branch if exp >-9
mak_zero:
			; zero having an exponent of 0000e0 causes add errors
	ldi	xexp,0xEE	; if not make number small zero
	clr	sign		;  (if xexp=0 then can get fpadd errors, so
	clr	rsL		;    zero must be defined with a very low
	clr	rsH		;    exponent, which is: .0000e-9 = 0000EE
exp_ok:			;assemble exponent
	lsl	xexp		;reencode expsign+exponent  seeeeee0
	mov	rsE,xexp
	or	rsE,sign	;reencode sign	            seeeeeeS
popfpmulstack:
popfpsubstack:		;this is where negative adds & subtracts get fppop'd
	mov	fpyL,fpzL		; copy fpy <= fpz
	mov	fpyH,fpzH
	mov	fpyE,fpzE
	mov	fpzL,fptL		; copy fpz <= fpt
	mov	fpzH,fptH
	mov	fpzE,fptE
	ret

;***** 16bit unsigned multiply:  mcH:mcL * mpH:mpL = rs3:rs2:rs1:rs0
mul16u:	
	ldi	rs0,16		;initialize counter
	mov	mctr,rs0
	mov	rs0,mpL		;initialize (modified from an Atmel AppNote)
	mov	rs1,mpH
	clr	rs2		;clear result bytes rs2 & rs3
	clr	rs3
	lsr	rs1
	ror	rs0
mul_1p:	brcc	mul_sh		;if bit 0 of multiplier set
	add	rs2,mcL		;add multiplicand mcL to result rs2
	adc	rs3,mcH		;add multiplicand mcH to result rs3
mul_sh:	ror	rs3		;shift right result byte3
	ror	rs2		;rotate right result byte2
	ror	rs1		;rotate result rs1 and multiplier mpH
	ror	rs0		;rotate result rs0 and multiplier mpL
	dec	mctr		;decrement counter
	brne	mul_1p		;if not done, loop again
	ret

;***** 32bit unsigned times ten:  rs3:rs2:rs1:rs0 = 10 * rs3:rs2:rs1:rs0
mul32x10:
	lsl	rs0		;multiply result times two
	rol	rs1		; lowest byte only multiplied times 8
	rol	rs2
	rol	rs3
	mov	cpy0,rs0	;save copy of 2 * rs3:rs2:rs1:rs0
	mov	cpy1,rs1
	mov	cpy2,rs2
	mov	cpy3,rs3
	lsl	rs0		;multiply result times four
	rol	rs1
	rol	rs2
	rol	rs3
	lsl	rs0		;multiply result times eight
	rol	rs1
	rol	rs2
	rol	rs3
	add	rs0,cpy0	;make result times ten rs= (2*rs)+(8*rs)
	adc	rs1,cpy1	;  change to adc (not add) if can find
	adc	rs2,cpy2	;  available register for cpy0
	adc	rs3,cpy3
	ret

;***** 16bit floating point divide is done by multiplying by 1/divisor ******* 
;Division is handle in two steps, first the recipicle of the divisor is taken
; (1/n), then multiply times the dividend.  This forces the number to never
; be greater than (.9999Ex), otherwise m/n division of numbers from .9999 to
; .1000 would result in answers from 9.9999 to .1000.  This also permits
; operation with 4-register 32bit numbers. Divide by zero = .9999*10^16.
; This subroutine does not normalize:	rcall normalize	   ;if needed
;					rcall fprec1_x
;
;***** 16bit recipicle (1/10n):  rsE:rsH:rsL = 1 / dvE:dvH:dvL
fprec1_x:			;quot = [dd] / dv,  where dd=1
				; fpx =   1 / fpx    fpy=fpy    fpz=fpx    fpt=fpt
			;result will be found in rs1:rs0; fpy register unaffected
			;exponent = 1-e, (2-e if 199A)    for 1.000  not_1
	mov	xexp,dvE	;decode exponent     in +4 ->   -2   -3
	asr	xexp		;	                +3 ->   -1   -2
test_zero:			;			+2 ->    0   -1
	cpi	dvH,0x00	;test if not zero	+1 ->   +1    0
	brne	test_one	;			 0 ->   +2   +1
	cpi	dvL,0x00	;ditto			-1 ->   +3   +2
	brne	test_one	;			-2 ->   +4   +3
is_zero:			;			-3 -> ovrfl  +4
	ldi	dvL,0xFF	;default store infinity (FFFFe+15) as result
	ldi	dvH,0xFF	;
	ldi	dvE,0x20	;sign=+ exponent=16 (0010000b)
	ret
test_one:
	cpi	dvH,0x19	;test if not one
	brne	rec_begin	;
	cpi	dvL,0x9A	;ditto
	brne	rec_begin
is_one:			;if mantissa is one, don't divide, answer is 199A
	ldi	rs0,0x9A	;default store one as result (199A)
	ldi	rs1,0x19
	dec	xexp		;decrease exponent so exp(rs) = 2-exp(n)
	rjmp 	rec_exp		; 		1-(e-1) = 2-e
rec_begin:
	ldi	rs0,16		;initialize counter dctr
	mov	dctr,rs0
	ldi	rs0,0xFF	;load .099999999999 - the selection of this
				; dividend prevents invalid results, but is
	ldi	rs1,0xFF	;     .099999999999/.10000 = .99999
	ldi	rs2,0x9A	;     .099999999999/.99999 = .10000
	ldi	rs3,0x19
	lsr	fpxH		;downscale by 2 so 17 iterations aren't needed,
	ror	fpxL		; also solves certain problems with numbers >.7500
rec_loop:		;this is going to have 16 iterations
	sub	rs2,dvL		;subtract terms highest to lowest order.
	sbc	rs3,dvH		;carry contains result (highest order first)
	brcs	rec_readd	;if c=set borrow occured
	sec			; if not, then push 1 into result
	rjmp	rec_next
rec_readd:
	add	rs2,dvL		; readd any subtraction where a borrow occured
	adc	rs3,dvH		;   back into the dividend.
	clc			; if so, then push 0 into result
rec_next:
	rol	rs0		;roll result into rs1:rs0 <-C
	rol	rs1
	rol	rs2		;shift dividend rs3:rs2<-rs1:rs0 to process
	rol	rs3		; lower terms of current remainder
	dec	dctr		;decrement counter
	brne	rec_loop	;if not done, loop again
rec_exp:
	mov	sign,dvE	;extract and store sign
	andi	sign,0x01	;
	ldi	dvE,0x01	;create exponent:  exp(rs) = 1-exp(n)
	sub	dvE,xexp	; exponent for 1.000 premodified
	lsl	dvE		;reencode expsign+exponent  seeeeee0
	or	dvE,sign	;reencode sign	            seeeeeeS
	mov	dvL,rs0		;move mantissa into proper result registers
	mov	dvH,rs1	
	ret

;***************************************************************************
;****** SQUARE ROOT ***** this calculates the square root based on the equation
;			; r=(r+(n/r))/2, where a test r is iterated til it limits
;  successive		; to the correct answer.  calculates r=sqrt(n)
;  approximation	; all registers in the floating point stack are used
			; so no other parameters can be saved.  signs ignored.
			;SEEMS TO NEED DENORMALIZED NUMBERS, negative exponent
			; seem to generate strange results, but 0x0001e0 works
fpsqrt_x:			; r = sqrt[n] 
				;fpx = sqrt[fpx]    fpy=n    fpz=n    fpt=n
	rcall	fillstack_x		;^fpx=n, where n number to be square rooted
	mov	xexp,fpxE	;compute test root by dividing exponent by 2
	andi	fpxE,0x01
	lsr	xexp
	andi	xexp,0xFE
	or	fpxE,xexp	;^fpy=n, fpx=test_r
	ldi	ZL,6		;use ZL for loop counter, since not used during
sqrtloop:
	rcall	loady_x		;^fpz=n, fpy=fpx=r		>sqrt iteration
	rcall	fprec1_x	;^fpz=n, fpy=r, fpx=1/r
	rcall	swapz_y		;^fpt=n, fpz=r, fpy=n, fpx=1/r
	rcall	fpmuly_x	;^fpz=n, fpy=r, fpx=n/r
	rcall	fpaddy_x	;^fpy=n, fpx=r+(n/r)
	rcall	loady_x		;^fpz=n, fpy=fpx=r+(n/r)
	ldi	fpxL,0x01	;load fpx=.5000
	ldi	fpxH,0x80
	ldi	fpxE,0x00	;^fpz=n, fpy=r+(n/r), fpx=.5000
	rcall	fpmuly_x	;^fpy=n, fpx=(r+(n/r))/2
	dec	ZL
	brne	sqrtloop
	ret

;***************************************************************************
;***************************************************************************
;	BBB    CC  DDD      RRR    OO   U  U  TTT III  N  N  EEE   SSS
;	B  B  C    D  D     R  R  O  O  U  U   T   I   NN N  E    S
;	BBB   C    D  D     R  R  O  O  U  U   T   I   N NN  EEE   SS
;	B  B  C    D  D     RRR   O  O  U  U   T   I   N NN  E       S
;	BBB    CC  DDD      R  R   OO    UUU   T  III  N  N  EEE  SSS
;***************************************************************************
;*      all bcd routines written by john brooke   (see dietcokeware license)
;* "bin2bcd" - 16-bit Binary to 4-BCD conversion
;* This subroutine converts a 16-bit number (binH:binL) to a 4-digit 
;* unpacked BCD number represented by 4-bytes (BCD3:BCD2:BCD1:BCD0).
;* 	Number of words:__ 		Number of cycles: ~50/~220 (Min/Max)
;*
;* "bcd2bin" - 16-bit Binary to 4-BCD conversion.   Y (fpy is destroyed)
;* This subroutine converts a 4-digit unpacked BCD number represented by
;* 4-bytes (BCD3:BCD2:BCD1:BCD0) to a 16-bit number (binH:binL).
;*  
;* 	Number of words:__ 		Number of cycles: ~50/~220 (Min/Max)
;***************************************************************************
.def	binL	=r16	;binary value Low byte			;definitions
.def 	binH	=r17	;binary value High byte
.def	binE	=r18	;bit7=exponent sign     bit6-1=exponent    bit0=sign
.def	bcd0	=r19	;decimal ones digit
.def	bcd1	=r20	;decimal tens digit
.def	bcd2	=r21	;decimal hundreds digit
.def	bcd3	=r22	;decimal thousands digit
.def	numL	=r23	;temp low byte
.def	numH	=r24	;temp high byte
.def	numE	=r25	;temp exponent byte

.def	char	=r23	;temp ascii character for display (not during bcd)
.def	digit	=r24	;temp ascii digit for display (not during bcd)
.def	bctr	=r25	;temp loop counter for display (not during bcd)
;******** binary to bcd conversion subroutine ******************************
bin2bcd:		;this subr converts: binH:binL (.bbbbbbbbbbbbbbbb)
			;		 to: bcd3:bcd2:bcd1:bcd0   (.dddd)	
	clr	bcd0		;lsd - eventual result
	clr	bcd1
	clr	bcd2
	clr	bcd3		;msd
	ldi	numL,low(6554)	;this is .1000 decimal
	ldi	numH,high(6554)
bcd3loop:
	sub	binL,numL	;subtract .1000 from binH:binL
	sbc	binH,numH
	brcs	bcdnext2	;if c=set borrow occured, 
	inc	bcd3		;increment bcd digit +1000
	rjmp	bcd3loop
bcdnext2:
 	add  	binL,numL	;restore binH:binL
	adc	binH,numH
	rcall	times8		;multiply remainder by scale factor 8
	ldi	numL,low(5243)	;this is .0100 decimal (scaled)
	ldi	numH,high(5243)
bcd2loop:
	sub	binL,numL	;subtract .0100 from binH:binL
	sbc	binH,numH
	brcs	bcdnext1	;if c=set borrow occured
	inc	bcd2		;increment bcd digit +100
	rjmp	bcd2loop
bcdnext1:
 	add  	binL,numL	;restore binH:binL
	adc	binH,numH
	rcall	times8		;multiply remainder by scale factor 8
	ldi	numL,low(4194)	;this is .0010 decimal (double scaled)
	ldi	numH,high(4194)
bcd1loop:
	sub	binL,numL	;subtract .0010 from binH:binL
	sbc	binH,numH
	brcs	bcdnext0	;if c=set borrow occured 
	inc	bcd1		;else increment bcd digit +10
	rjmp	bcd1loop
bcdnext0:
 	add  	binL,numL	;restore binH:binL
	adc	binH,numH
	rcall	times8		;multiply remainder by scale factor 8
	ldi	numL,low(3355)	;this is .0001 decimal (triple scaled)
	ldi	numH,high(3355)
bcd0loop:
	sub	binL,numL	;subtract .0001 from binH:binL
	sbc	binH,numH
	brcs	bcddone		;if c=set borrow occured 
	inc	bcd0		;else increment bcd digit +1
	rjmp	bcd0loop
bcddone:			;binH:binL is garbage now
	ret
;******** bcd to binary conversion *****************************************
bcd2bin:		;this subr converts: bcd3:bcd2:bcd1:bcd0   (.dddd)
			;                to: binH:binL (.bbbbbbbbbbbbbbbb)
	clr	binL		;eventual result
	clr	binH
	ldi	numL,low(3355)	;ones digit .0001 (triple scaled)
	ldi	numH,high(3355)
bin0loop:
	cpi	bcd0,0		;test if digit is zero?
	breq	binnext1	; if so: go to tens digit
	dec	bcd0		; loop thru digit
	add  	binL,numL	; if not: add .0001 to binH:binL
	adc	binH,numH
	rjmp	bin0loop
binnext1:
	rcall	divby8		;divide tally by scale factor 8
	ldi	numL,low(4194)	;tens digit .0010 (double scaled)
	ldi	numH,high(4194)
bin1loop:
	cpi	bcd1,0		;test if digit is zero?
	breq	binnext2	; if so: go to hundreds digit
	dec	bcd1		; loop thru digit
	add  	binL,numL	; if not: add .0010 to binH:binL
	adc	binH,numH
	rjmp	bin1loop
binnext2:
	rcall	divby8		;divide tally by scale factor 8
	ldi	numL,low(5243)	;hundreds digit .0100 (single scaled)
	ldi	numH,high(5243)
bin2loop:
	cpi	bcd2,0		;test if digit is zero?
	breq	binnext3	; if so: go to thousands digit
	dec	bcd2		; loop thru digit
	add  	binL,numL	; if not: add .0100 to binH:binL
	adc	binH,numH
	rjmp	bin2loop
binnext3:
	rcall	divby8		;divide tally by scale factor 8
	ldi	numL,low(6554)	;hundreds digit .0100 (single scaled)
	ldi	numH,high(6554)
bin3loop:
	cpi	bcd3,0		;test if digit is zero?
	breq	bindone		; if so: done
	dec	bcd3		; loop thru digit
	add  	binL,numL	; if not: add .1000 to binH:binL
	adc	binH,numH
	rjmp	bin3loop
bindone:
	ret	
;**** write BCD number to memory pointer ************************************
display_pv:				;DISPLAY PROCESS VALUE TO SRAM DISPLAY
	rcall	loadx_pv	;load PV into fpx
	ldi 	ZL,low($70)	;load pointer of PV into Z
	ldi 	ZH,high($70)
	ldi	char,'P'	;draw "P" in SRAM display
	st	Z+,char		;
	ldi	char,'V'	;draw "V" in SRAM display
	st	Z+,char		;
	rjmp	display
display_sp:				;DISPLAY SETPOINT TO SRAM DISPLAY
	rcall	loadx_sp	;load SP into fpx
	ldi 	ZL,low($60)	;load pointer of SP into Z
	ldi 	ZH,high($60)
	ldi	char,'S'	;draw "S" in SRAM display
	st	Z+,char		;
	ldi	char,'P'	;draw "P" in SRAM display
	st	Z+,char		;
	rjmp	display
display:						;uses fpy registers
	rcall	bin2bcd		;convert to bcd
	ldi	YL,low($0017)	;must be same address as bcd3+1 0x17= 23
	ldi	YH,high($0017)	;must be same address as bcd3+1 
	ori	bcd0,0x30	;convert to ascii
	ori	bcd1,0x30
	ori	bcd2,0x30
	ori	bcd3,0x30
	ldi	bctr,5		;initialize loopcounter for 5 digit number
	asr	binE		;extract exponent, put sign (bit0) in carry
	brcc	dis_next1	; if sign (C) is positive (C=0) branch
	ldi	char,'-'	; if negative, write minus "-" sign
	st	Z+,char
	dec	bctr		; decrement counter
dis_next1:
	cpi	binE,5		;check if exponent > 4
	brge	overflow	; if so overflow condition exists
	tst	binE		;test if exponent is zero
	breq	draw_Ezer	; if so, branch to display decimal
	tst	binE		;test if exponent is negative
	brmi	draw_Eneg	; if so, branch to write leading zero
draw_Epos:		;exponent is positive
	ld	digit,-Y	;load next digit
	st	Z+,digit	;draw digit in SRAM display
	dec	bctr		;decrement counter
	breq	dis_done	; if zero, then all digits displayed	
	dec	binE		;decrement exponent
	brne	draw_Epos	; if not zero, then still pos, draw more digits
					; if so, then exponent is zero, draw decimal
draw_Ezer:		;exponent is zero
	ldi	char,'.'	;load decimal "."
	st	Z+,char		;draw decimal in SRAM display
	dec	bctr		;decrement counter
	breq	dis_done	; if zero, then all digits displayed
	rjmp	draw_rDig	; if not zero, then still pos, draw more digits
overflow:
	ldi	char,'*'	;load asterisk "*" meaning overflow
	st	Z+,char		;draw asterisk in SRAM display
	dec	bctr		;decrement counter
	breq	dis_done	; if zero, then all overflow digits displayed
	rjmp	overflow	; if not zero, then still pos, draw more digits
draw_Eneg:		;exponent is negative
	ldi	char,'.'	;load decimal "."
	st	Z+,char		;draw decimal in SRAM display
	dec	bctr		;decrement counter (dont need to test bctr yet)
draw_Lzer:		;draw leading zeros
	ldi	char,'0'	;load zero "0"
	st	Z+,char		;draw leading zeros after decimal in SRAM
	dec	bctr		;decrement counter
	breq	dis_done	; if zero, then all digits displayed
	inc	binE		;increment exponent
	brne	draw_Lzer	; if not zero, still negative, draw more zeros
				; if zero, goto draw remaining digits
draw_rDig:		;draw remaining digits
	ld	digit,-Y	;load next digit
	st	Z+,digit	;draw digit in SRAM display
	dec	bctr		;decrement counter
	brne	draw_rDig	; if not zero, still positive, draw more digits
dis_done:			; if zero, then all digits displayed
	ret
;**** write output to display ***********************************************
display_out:				;DISPLAY PROCESS VALUE TO SRAM DISPLAY
			;does not suppress leading zeros.............
	rcall	loadx_out	;load PV into fpx
	ldi 	ZL,low($77)	;load pointer of PV into Z
	ldi 	ZH,high($77)
	ldi	char,' '	;draw " " in SRAM display
	st	Z+,char		;
	ldi	char,'%'	;draw "%" in SRAM display
	st	Z+,char		;
	ldi	char,'O'	;draw "O" in SRAM display
	st	Z+,char		;
	ldi	char,'u'	;draw "u" in SRAM display
	st	Z+,char		;
	ldi	char,'t'	;draw "t" in SRAM display
	st	Z+,char		;
	rcall	bin2bcd		;convert to bcd
	ori	bcd1,0x30	;convert to ascii (only need 3 digits)
	ori	bcd2,0x30
	ori	bcd3,0x30
	st	Z+,bcd3		;draw integer digits to SRAM display
	st	Z+,bcd2
	ldi	char,'.'	;draw decimal "." in SRAM display
	st	Z+,char		;
	st	Z+,bcd1		;draw fractional tenth digit to SRAM display
	ret

;**** binary multiplication *************************************************
times8:
	lsl	binL			;multiply binH:binL times 8
	rol	binH
	lsl	binL			;multiply binH:binL times 4
	rol	binH
	lsl	binL			;multiply binH:binL times 2
	rol	binH
	ret

;**** binary division *******************************************************
divby8:
	lsr	binH			;divide binH:binL by 8
	ror	binL
	lsr	binH			;divide binH:binL by 4
	ror	binL
	lsr	binH			;divide binH:binL by 2
	ror	binL
	ret
	
;***************************************************************************
;***************************************************************************
;* "flash2ram"
;*
;* This subroutine copies a block of data from the Program memory (Flash) 
;* to the internal SRAM. The following parameters must be set up prior to 
;* calling the subroutine:
;* Z-pointer: 	Flash block start address x 2 (WORD oriented code segment)
;* Y-pointer:	ram block start address
;* romsize:	block size
;*
;* Number of words	:5 + return
;* Number of cycles	:10 x block size + return
;* Low Registers used	:1 (r0)
;* High Registers used	:1 (flashsize)
;* Pointers used	:Y, Z
;***** Subroutine Register variables****************************************
.def	flashsize=r20		;size of block to be copied
		
flash2ram:
	lpm			;get constant, implied: lpm r0,Z
	st	Y+,r0		;store in SRAM and increment Y-pointer
	adiw	ZL,1		;increment Z-pointer
	dec	flashsize
	brne	flash2ram	;if not end of table, loop more
	ret

;*****************************************************************************
;***** FLOATING POINT MOVE ROUTINES ******************************************
;;think of math routines working like a HP calculator with RPN logic:
; load/push=Enter, pop=DownArrow, fpmuly_x= TimesKey, fpaddy_x= PlusKey,
; fpsuby_x= MinusKey, fprec1_x= RecipicleKey, fpsqrt_x= SquareRootKey

; simplified floating point move, copy, and swap routines:
;  Basic idea. 	x is a primary working storage register where results
;		   end up: sums, products, and quotients
;  		y is a primary register for multiplicands, dividends,
;		   augends, and minuends.
;		t and z are secondary storage for intermediate results.

.def	mtmp	=r15		;temp move variable

;the loadx subroutine must be accessed by loading the Z-register with the SRAM
; parameter address before calling loadx
;	ldi	ZH,high(PARAMETER)	;load fpx <- PARAMETER
;	ldi	ZL,low(PARAMETER)
;	rcall	loadx_zptr		;or, rcall loadY: fpy <- PARAMETER
loadx_zptr:
loadx_parameter:
	rcall	push_fpXstack
	ld	fpxL,Z+		;load low mantissa byte
	ld	fpxH,Z+		;load high mantissa byte
	ld	fpxE,Z+		;loaa exponent byte
	ret
loady_zptr:
loady_parameter:
	rcall	push_fpYstack
	ld	fpyL,Z+		;load low mantissa byte
	ld	fpyH,Z+		;load high mantissa byte
	ld	fpyE,Z+		;load exponent byte
	ret
loadxw_zptr:
loadxword_zptr:		;routine to load parameter to fpx with default zero exponent
	rcall	push_fpXstack
	ld	fpxL,Z+		;load low mantissa byte
	ld	fpxH,Z+		;load high mantissa byte
	ldi	fpxE,0x00	;load exponent byte = zero
	ret
loadyw_zptr:
loadyword_zptr:		;routine to load parameter to fpy with default zero exponent
	rcall	push_fpYstack
	ld	fpyL,Z+		;load low mantissa byte
	ld	fpyH,Z+		;load high mantissa byte
	ldi	fpyE,0x00	;loaa exponent byte = zero
	ret
loadx_pv:
	rcall	push_fpXstack
	mov	fpxL,pvL 		; fpx <- pv
	mov	fpxH,pvH
	mov	fpxE,pvE
	ret
loady_pv:
	rcall	push_fpYstack
	mov	fpyL,pvL 		; fpy <- pv
	mov	fpyH,pvH
	mov	fpyE,pvE
	ret
loadx_sp:
	rcall	push_fpXstack
	mov	fpxL,spL 		; fpx <- sp
	mov	fpxH,spH
	mov	fpxE,spE
	ret
loady_sp:
	rcall	push_fpYstack
	mov	fpyL,spL		; fpy <- sp
	mov	fpyH,spH
	mov	fpyE,spE
	ret
loadx_out:			;out is a number between .0000 and .9999 only
	rcall	push_fpXstack
	mov	fpxL,outL 		; fpx <- out
	mov	fpxH,outH
	clr	fpxE			; exponent always zero
	ret
loady_out:			;out is a number between .0000 and .9999 only
	rcall	push_fpYstack
	mov	fpyL,outL 		; fpy <- out
	mov	fpyH,outH
	clr	fpyE			; exponent always zero
	ret

loadx_fp0:			;manually load fpx=0.000
	rcall	push_fpXstack
	ldi	fpxL,0x00
	ldi	fpxH,0x00
	ldi	fpxE,0xEE
	ret
loadx_fp1:			;manually load fpx=1.000
	rcall	push_fpXstack
	ldi	fpxL,0x9A
	ldi	fpxH,0x19
	ldi	fpxE,0x02
	ret

storx_zptr:
storx_parameter:
	st	Z+,fpxL		;load low mantissa byte
	st	Z+,fpxH		;load high mantissa byte
	st	Z+,fpxE		;loaa exponent byte
	ret
storx_pv:
	mov	pvL,fpxL		; pv <- fpx
	mov	pvH,fpxH
	mov	pvE,fpxE
	ret
storx_sp:
	mov	spL,fpxL		; sp <- fpx
	mov	spH,fpxH
	mov	spE,fpxE
	ret
storx_out:
	mov	outL,fpxL		; out <- fpx
	mov	outH,fpxH
	ret
popx:
downloadx_y:
pop_fpstack:
	mov	fpxL,fpyL		; fpx <- fpy
	mov	fpxH,fpyH
	mov	fpxE,fpyE
	mov	fpyL,fpzL		; fpy <- fpz
	mov	fpyH,fpzH
	mov	fpyE,fpzE
	mov	fpzL,fptL		; fpz <- fpt
	mov	fpzH,fptH
	mov	fpzE,fptE
	ret
pushx:
loady_x:
push_fpXstack:		;for loadx_?? calls
			; a pushX without loading copies fpy <-fpx
	mov	fptL,fpzL		; fpt <- fpz
	mov	fptH,fpzH
	mov	fptE,fpzE
	mov	fpzL,fpyL		; fpz <- fpy
	mov	fpzH,fpyH
	mov	fpzE,fpyE
	mov	fpyL,fpxL		; fpy <- fpx
	mov	fpyH,fpxH
	mov	fpyE,fpxE
	ret
pushy:
loadz_y:
push_fpYstack:		;for loady_?? calls
	mov	fptL,fpzL		; fpt <- fpz
	mov	fptH,fpzH
	mov	fptE,fpzE
	mov	fpzL,fpyL		; fpz <- fpy
	mov	fpzH,fpyH
	mov	fpzE,fpyE
	ret
	
fillstack_x:			; fill entire stack with fpx
	mov	fpyL,fpxL		; fpt,fpz,fpy <- fpx
	mov	fpyH,fpxH
	mov	fpyE,fpxE
fillstack_y:			; fill upper stack with fpy
	mov	fpzL,fpyL		; fpt,fpz <- fpy
	mov	fpzH,fpyH
	mov	fpzE,fpyE
fillstack_z:			; fill top stack with fpz
	mov	fptL,fpzL		; fpt <- fpz
	mov	fptH,fpzH
	mov	fptE,fpzE
	ret
swapy_x:
	mov	mtmp,fpxL		; fpy <-> fpx
	mov	fpxL,fpyL
	mov	fpyL,mtmp
	mov	mtmp,fpxH
	mov	fpxH,fpyH
	mov	fpyH,mtmp
	mov	mtmp,fpxE
	mov	fpxE,fpyE
	mov	fpyE,mtmp	
	ret
swapz_y:
	mov	mtmp,fpyL		; fpz <-> fpy
	mov	fpyL,fpzL
	mov	fpzL,mtmp
	mov	mtmp,fpyH
	mov	fpyH,fpzH
	mov	fpzH,mtmp
	mov	mtmp,fpyE
	mov	fpyE,fpzE
	mov	fpzE,mtmp	
	ret

;******************************************************************************
;********************** INTERUPT HANDLER ROUTINES *****************************
UART_RXC:	;UART RX Complete handler. this is where the program restarts
		; when the external comm sends a control byte. All interrupts
		; kill the Global Interrupt Flag, this is reset after the LCD
		; driver is run (which is the next in program execution).
		;    bit7   bit6   bit5   bit4   bit3   bit2   bit1   bit0
		;     n/a    n/a    n/a    PGM   MODE  SELECT  INCR   DECR
		; these will work:         'P'    'H'    'D'    'B'    'A'

	in 	r27,UDR		;read uart byte	
	andi 	r27,0x1F	;mask downloaded control byte
	pop 	r16		;clear return address off of stack
	pop 	r16		; no need to reenable global interrupts
	rjmp 	RESTART

;	also there is a potential problem, global interupts must only be enabled
;	 in the main program or stack creep may occur.............

EXT_INT0:	;IRQ0 handler
EXT_INT1:	;IRQ1 handler
TIM1_CAPT:	;Timer1 Capture handler
TIM1_COMP:	;Timer1 Compare handler
TIM1_OVF:	;Timer1 Overflow handler
TIM0_OVF:	;Timer0 Overflow handler
SPI_STC:	;SPI Transfer Complete handler
UART_DRE:	;UDR Empty handler
UART_TXC:	;UART TX Complete handler
ADC:		;ADC Conversion Complete Interrupt handler
EE_RDY:		;EEPROM Ready handler
ANA_COMP:	;Analog Comparator handler
	reti
;****************************************************************************
;			M     M    A    III  N   N
;			MM   MM   A A    I   NN  N
;			M M M M  A   A   I   N N N
;			M  M  M  AAAAA   I   N  NN
;			M     M  A   A  III  N   N
;***** Main Program Register variables***************************************

.equ	SRAM00	=$60		;start address of SRAM DISPLAY line1
.equ	SRAM10  =$70		;start address of SRAM DISPLAY line2
.equ	PGAIN	=$80		;proportional gain			>>>> CONTROL
.equ	IGAIN	=$83		;integral gain
.equ	DGAIN	=$86		;derivative gain
.equ	TC	=$89		;time constant
.equ	DZ	=$8C		;deadzone
.equ	CLAMP	=$8F		;clamp on move

.equ	INMAX	=$92		;input calibration maximum		>>>> INPUT
.equ	INMIN	=$95		;input calibration minumum
.equ	A	=$98		;3rd order calibration term
.equ	B	=$9B		;2nd order calibration term
.equ	C	=$9E		;1st order calibration term (slope)
.equ	D	=$A1		;0th order calibration term (intercept)
.equ	E	=$A4		;square root calibration term
.equ	FILTR	=$A7		;WORD - input filter factor
.equ	NUMTC	=$A9		;BYTE - number of time constants byte

.equ	MGAIN	=$AA		;WORD - manual gain for incr/decr moves	>>>> OUTPUT
.equ	XSTAT	=$AC		;BYTE - downloaded status byte
.equ	AGAIN	=$AD		;automatic gain for increase decrease moves

.equ	SPUL	=$B0		;setpoint upper limit			>>>> LIMITS
.equ	SPLL	=$B3		;setpoint lower limit
.equ	INUL	=$B6		;input upper limit
.equ	INLL	=$B9		;input lower limit
.equ	OUTUL	=$BC		;WORD - output upper limit

.equ	PINSLAST=$BE		;BYTE - debounced button parameter
.equ	OUTLL	=$BF		;WORD - output lower limit
.equ	PINSNOW	=$C1		;BYTE - debounced button parameter

.equ	BADUL	=$C2		;input bad upper limit			>>>> ALARMS
.equ	BADLL	=$C5		;input bad lower limit
.equ	PCLAMP	=$C8		;protection clamp

.equ	SUMERR	=$CB		;integral term SUMMATION(ERROR) history term
.equ	LERR	=$CE		;last error, difference between setpoint and error
.equ	MINDX	=$D1		;BYTE - program menu index#
.equ	SINDX	=$D2		;BYTE - program select index#

.equ	ALM	=2		;PB2 is the LED alarm output

.def	temp	=r16		;temporary variable
.def	MIDX	=r24		;temporary program menu index byte (local MINDX)
.def	SIDX	=r25		;temporary program select index byte (local SINDX)
.def	WCB	=r24		;temporary working control byte
.def	LCB	=r25		;temporary local control byte
.def	MCB	=r26		;static master control byte
.def	XCB	=r27		;static external control byte (from uart)

RESET:			;***** initialize pid controller
	ldi	temp,low(RAMEND)
	out	SP,temp
			;PB5.3 drive tha D/A converter.
			;PB2 is the Alarm LED
			;PB1.0 are reserved for alternate input timer
	ldi	r16,0x27	;CS\  SCK  Din  ALM  OC1  ICP
	out	PORTB,r16	; 1    0    0    1    1pu  1pu
	ldi	r16,0x3C	;PB5  PB4  PB3  PB2  PB1  PB0
	out	DDRB,r16	;out  out  out  out   in   in

			;make PC5.0 inputs; PC5 is analog input
			; PC4.0 are outputs when driving the LCD.
				;Ana5 PGM  MODE SEL  INCR DECR
	ldi	r16,0x3F	;Ana5  RS  DB7  DB6  DB5  DB4 
	out	PORTC,r16	; 1pu  1pu  1pu  1pu  1pu  1pu
	ldi	r16,0x00	;PC5  PC4  PC3  PC2  PC1  PC0
	out	DDRC,r16	; in   in   in   in   in   in

			;PD7,6,3 are  remote hardwire interface
			;PD5 is reserved for alternate input timer
			;PD4 is the LCD command strobe
			;PD2 is unused
			;PD1.0 are the UART inputs
	ldi	r16,0xEF	;INCR DECR  T1   E   R/L  N/A  TXD  RXD
	out	PORTD,r16	; 1pu  1pu  1pu  0    1pu  1pu  1pu  1pu
	ldi	r16,0x10	;PD7  PD6  PD5  PD4  PD3  PD2  PD1  PD0
	out	DDRD,r16	; in   in   in  out   in   in   in   in

	clr	MCB		;clear control bytes
	clr	XCB
	
	clr	pvL		;zero out process value
	clr	pvH
	clr	pvE
	
	clr	spL		;zero out setpoint
	clr	spH
	clr	spE
	clr	outL		;zero out output
	clr	outH

	nop		;need to download EEPROM into SRAM
			; and status bits into r6
;	rcall init_eeprom		; or use below code <<<<<<<<<<<<<<<<<

;***** Copy n bytes ROM -> RAM	>>>> alternate initialization values
		;This code copies a block of data from the Program memory
			;  to the SRAM area beginning at location SRAM00.
	ldi	ZH,high(oper_dis*2)
	ldi	ZL,low(oper_dis*2)	;init Z-pointer
	ldi	YH,high(SRAM00)
	ldi	YL,low(SRAM00)		;init Y-pointer
	ldi	flashsize,114		;n
	rcall	flash2ram		;copy n bytes
	
	lds	STATUS,XSTAT		;load status to testable register
	
;rjmp 	input	;<<<<<<<<<<<<<<<<<<<<<<<<<<< TEMP DEBUGGING TO SKIP LONG DELAYS

	rcall init_lcd		;initialize lcd, must init before adc
	rcall init_adc		;initialize adc - must be after init_lcd
	rcall init_uart		;initialize uart to receive


RESTART:
	cli			;kill global interrupts for LCD write
	cbi	ADCSR,ADEN	;kill adc conversion while using PORTC
	rcall 	out_lcd		;output lcd display to uart and lcd
	sbi	ADCSR,ADEN	;resume adc read
	sbi	ADCSR,ADSC
				;MUST WAIT TIL ADC READ IS DONE <<<<<<<<<<<<<<
	rcall	delay5ms	;try long delay <<<<<<<< was 10ms <<<<<<<<<<<<<<<<


input:
	in 	fpxL,ADCL	;read adc5 - must read low byte first
	in 	fpxH,ADCH	;          - then immediately the high byte
	ldi 	fpxE,6		;load counter
adc_prescale:		;raw adc value will convert from .0000-.9999
	lsl 	fpxL		;multiply R17:R18 times 2^6
	rol 	fpxH
	dec 	fpxE		;upon end r18=0 which is exponent
	brne	adc_prescale
	clr	fpxE				;<<<<<<<<<NOT NEEDED? already 0
	
;	ldi	fpxL,0x02	;>>>>>>>>>> TEMP LOAD .5000 >>>>>>>>>>>>>>>>TEMP
;	ldi	fpxH,0x80	;>>>>>>>>>> TEMP load in a midrange PV

	rcall	fillstack_x	;^fpx=rawPV (shorthand for fpt=fpz=fpy=fpx=rawPV)
convert_pv:		;rawPV is a value between 0000 and FFC0
	sbrc	STATUS,1	;test INTYP if timer or frequency input
	rjmp	time_freq		;if STATUS bit3.0= --1- use non-linear calib
	sbrc	STATUS,3	;test INALG if using non-linear conversion
	rjmp	nonlinear		;if STATUS bit3.0= 1--- use linear calib
linear:
	sbrs	STATUS,0	;test INTYP if using linear 1-5V conversion
	rjmp	linear0_5v		;if STATUS bit3.0= 0-00
linear1_5v:				;else            = 0-01
	ldi	fpxL,0x34	;manual load fpx=.2000
	ldi	fpxH,0x33
	ldi	fpxE,0x00	;^fpy=rawPV, fpx=.2000
	rcall	fpsuby_x	;^fpy=rawPV, fpx=rawPV-.2
	rcall	pushx		;^fpz=rawpV, fpy=fpx=rawpV-.2
	ldi	fpxL,0x00	;manual load fpx=1.250
	ldi	fpxH,0x20
	ldi	fpxE,0x02	;^fpz=rawPV, fpy=rawPV-.2, fpx= 1.25
	rcall	fpmuly_x	;^fpy=rawPV, fpx=(rawPV-.2)*1.25, as correctPV	
linear0_5v:
	ldi	ZH,high(INMAX)
	ldi	ZL,low(INMAX)
	rcall	loadx_zptr	;^fpy=rawPV, load fpx=INMAX
	ldi	ZH,high(INMIN)
	ldi	ZL,low(INMIN)
	rcall	loadx_zptr	;^fpz=rawPV, fpy=INMAX, load fpx=INMIN
	rcall	fpsuby_x	;^fpy=rawPV, fpx=INMAX-INMIN which is slope
	rcall	fpmuly_x	;^fpy=rawPV, fpx=rawPV(INMAX-INMIN)
	ldi	ZH,high(INMIN)
	ldi	ZL,low(INMIN)
	rcall	loadx_zptr	;^fpz=rawPV, fpy=rawPV(INMAX-INMIN), load fpx=INMIN
	rcall	fpaddy_x	;^fpy=rawpV, fpx=rawPV(INMAX-INMIN)+INMIN
	rjmp	badlimits	;PV = rawPV(INMAX-INMIN) + INMIN
time_freq:				;if STATUS bit1=1	
	nop			;TIMERS & COUNTERS & FREQUENCY INPUTS
	nop			;FOR FUTURE DEVELOPMENT
	nop
	nop
	nop			; use non-linear conversion?
	rjmp	badlimits
nonlinear:			;define rawPV as v (this is .0000 to .9999)
	rcall	fpsqrt_x	;^fpy=v, fpx=v^.5 -this fills stack with Volts
	ldi	fpyL,0x01	;manually load fpy=25.00
	ldi	fpyH,0x40
	ldi	fpyE,0x04	;^fpz=v, fpy=25.00, fpx=v^.5
	rcall	fpmuly_x	;^fpy=v, fpx=(5v)^.5, or V^.5, V defined as Volts
	ldi	ZH,high(E)
	ldi	ZL,low(E)
	rcall	loadx_zptr	;^fpz=v, fpy=V^.5, load fpx=E
	rcall	fpmuly_x	;^fpy=v, fpx=EV^.5
	rcall	swapy_x		;^fpz=v, fpy=EV^.5, fpx=v
	rcall	pushx		;^fpt=v, fpy=EV^.5, fpy=fpx=v
	ldi	fpyL,0x02	;manually load fpy=5.000
	ldi	fpyH,0x80
	ldi	fpyE,0x02	;^fpt=v, fpz=EV^.5, fpy=v, fpx=5.000
	rcall	fpmuly_x	;^fpz=v, fpy=EV^.5, fpx=5v=V, defined as V for Volts
	rcall	swapy_x		;^fpz=v, fpy=V, fpx=EV^.5
	rcall	fillstack_y	;^fpy=V, fpx=EV^.5
	rcall	swapy_x		;^fpz=V, fpy=EV^.5, fpx=V
	rcall	pushx		;^fpt=V, fpz=EV^.5, fpy=fpx=V
	rcall	fpmuly_x	;^fpz=V, fpy=EV^.5, fpx=V^2
	rcall	swapz_y		;^fpt=V, fpz=EV^.5, fpy=V, fpx=V^2
	rcall	fpmuly_x	;^fpz=V, fpy=EV^.5, fpx=V^3
	ldi	ZH,high(A)
	ldi	ZL,low(A)
	rcall	loadx_zptr	;^fpt=V, fpz=EV^.5, fpy=V^3, load fpx=A
	rcall	fpmuly_x	;^fpz=V, fpy=EV^.5, fpx=AV^3	
	rcall	fpaddy_x	;^fpy=V, fpx=AV^3+EV^.5
	rcall	swapy_x		;^fpz=V, fpy=AV^3+EV^.5, fpx=V
	rcall	loady_x		;^fpt=V, fpz=AV^3+EV^.5, fpy=fpx=V
	rcall	fpmuly_x	;^fpz=V, fpy=AV^3+EV^.5, fpx=V^2
	ldi	ZH,high(B)
	ldi	ZL,low(B)
	rcall	loadx_zptr	;^fpt=V, fpz=AV^3+EV^.5, fpy=V^2, load fpx=B
	rcall	fpmuly_x	;^fpz=V, fpy=AV^3+EV^.5, fpx=BV^2
	rcall	fpaddy_x	;^fpy=V, fpx=AV^3+BV^2+EV^.5
	rcall	swapy_x		;^fpz=V, fpy=AV^3+BV^2+EV^.5, fpx=V
	ldi	ZH,high(C)
	ldi	ZL,low(C)
	rcall	loadx_zptr	;^fpt=V, fpz=AV^3+BV^2+EV^.5, fpy=V, load fpx=C
	rcall	fpmuly_x	;^fpz=V, fpy=AV^3+BV^2+EV^.5, fpx=CV
	rcall	fpaddy_x	;^fpy=V, fpx=AV^3+BV^2+CV+EV^.5
	ldi	ZH,high(D)
	ldi	ZL,low(D)
	rcall	loadx_zptr	;^fpz=V, fpy=AV^3+BV^2+CV+EV^.5, load fpx=D
	rcall	fpaddy_x	;^fpy=V, fpx=AV^3+BV^2+CV+D+EV^.5, defined as PV
	rjmp	badlimits	; PV = AV^3 + BV^2 + CV + D + EV^.5	
badlimits:
	rcall	fillstack_x	;^fpx=PV
badupperlim:
	ldi	ZH,high(BADUL)	;test if process input upper limit exceeded
	ldi	ZL,low(BADUL)
	rcall	loady_zptr	;^fpz=PV, fpy=BADUL, fpx=PV
	rcall	fpsuby_x	;^fpy=PV, fpx=BADUL-PV, input bad if result negative
	sbrs	fpxE,0		;test if above result is negative (bit0=sign)
	rjmp	badlowerlim	; if positive, PV is valid
	cbi	PORTB,ALM	; else turn on LED alarm
	rcall	popx		;^fpx=PV
	rcall	storx_pv	;store PV=bad value
	rjmp	keypanel	; do not apply limits if in alarm mode
badlowerlim:
	rcall	popx		;^fpx=PV, insure PV is loaded
	ldi	ZH,high(BADLL)	;test if process input lower limit exceeded
	ldi	ZL,low(BADLL)
	rcall	loadx_zptr	;^fpy=PV, fpx=BADLL
	rcall	fpsuby_x	;^fpy=PV, fpx=PV-BADLL, overlimit if result negative
	sbrs	fpxE,0		;test if above result is negative (bit0=sign)
	rjmp	infilter	; if positive, PV is valid
	cbi	PORTB,ALM	; else turn on LED alarm
	rcall	popx		;^fpx=PV
	rcall	storx_pv	;store PV=bad value
	rjmp	keypanel	; do not apply limits if in alarm mode
infilter:
	rcall	popx		;^fpy=PV
	rcall	loadx_fp1	;^fpy=PV, fpx=1.000
	ldi	ZH,high(FILTR) 	;manually load FILTR, default exponent=0
	ldi	ZL,low(FILTR)
	rcall	loadxw_zptr	;^fpz=PV, fpy=1.000, fpx=FILTR
	rcall	fpsuby_x	;^fpy=PV, fpx=1-FILTR
	rcall	loadx_pv	;^fpz=PV, fpy=1-FILTR, fpx=oldPV
	rcall	fpmuly_x	;^fpy=PV, fpx=(1-FILTR)*oldPV	
	rcall	swapy_x		;^fpz=PV, fpy=(1-FILTR)*oldPV, fpx=PV
	ldi	ZH,high(FILTR) 	;manually load FILTR, default exponent=0
	ldi	ZL,low(FILTR)
	rcall	loadxw_zptr	;^fpt=PV, fpz=(1-FILTR)*oldPV, fpy=PV, fpx=FILTR
	rcall	fpmuly_x	;^fpz=PV, fpy=(1-FILTR)*oldPV, fpx=FILTR*PV
	rcall	fpaddy_x	;^fpy=PV, fpx=((1-FILTR)*oldPV)+(FILTR*PV),
				;  which is filteredPV (filPV)
	rcall	fillstack_x	;^fpx=filPV
	rcall	storx_pv	;PV = ( (1-FILTR) * oldPV ) + ( FILTR * PV )
inlimits:
inupperlim:
	rcall	loadx_pv	;^fpx=PV, insure PV is loaded
	ldi	ZH,high(INUL)	;test if process input upper limit exceeded
	ldi	ZL,low(INUL)
	rcall	loady_zptr	;^fpz=filPV, fpy=INUL, fpx=filPV
	rcall	fpsuby_x	;^fpy=filPV, fpx=INUL-filPV,
				;  overlimit if result negative
	sbrs	fpxE,0		;test if above result is negative (bit0=sign)
	rjmp	inlowerlim	; if positive, filPV is valid
	ldi	ZH,high(INUL)	; skip here if negative, must clamp filPV to INUL
	ldi	ZL,low(INUL)
	rcall	loadx_zptr	;^fpz=filPV, fpy=INUL-filPV fpx=INUL
	rcall	storx_pv	;PV = fpx (INUL)
inlowerlim:
	rcall	loadx_pv	;^fpy=???, fpx=filPV, insure PV is loaded
	ldi	ZH,high(INLL)	;test if process input lower limit exceeded
	ldi	ZL,low(INLL)
	rcall	loadx_zptr	;^fpz=???, fpy=filPV, fpx=INLL
	rcall	fpsuby_x	;^fpy=???, fpx=filPV-INLL,
				;  overlimit if result negative
	sbrs	fpxE,0		;test if above result is negative (bit0=sign)
	rjmp	keypanel	; if positive, PV is valid
	ldi	ZH,high(INLL)	; skip here if negative, PV must be clamped to INLL
	ldi	ZL,low(INLL)
	rcall	loadx_zptr	;^fpz=???, fpy=filPV-INLL, fpx=INLL
	rcall	storx_pv	;PV = fpx (INLL)
	
keypanel:
	sei			;check global interrupts.  All checks must be with
	nop			; stack clear, or stack might overwrite parameters.
	nop			;
	cli			;resume kill global interrupts

;**** CREATE MASTER CONTROL BYTE
;   [ext/loc][auto/man][hardwire][program] [mode][select][increase][decrease]
;     1=usb    1=auto    1=on    1=pressed   "       "       "       "
;    0=local  0=manual   0=off     0=not     "       "       "       "

				;r24 is WCB working control byte
				;r25 is LCB local control byte
				;r26 is MCB master control byte
				;r27 is XCB external control byte
	rcall	delay1ms	;wait for PORTC to switchover (could be less?)
				;>>>>>>>THIS DELAY MAY NOT BE NEEDED HERE ANYMORE
 
 	sei			;check global interrupts.  All checks must be with
	nop			; stack clear, or stack might overwrite parameters.
	nop			;
	cli			;resume kill global interrupts

readbuttons:
	in 	LCB,PINC	;load local keypanel control byte (complemented)
	sts 	PINSNOW,LCB	;store LCB NOW\ copy in memory
	com	LCB		;invert input pin sense
	
	lds	WCB,PINSLAST	;load LCB LAST\ from memory          >debounce LCB
	and	WCB,LCB		;create debounced LCB in WCB ---PMS--
	andi	WCB,0x1C	;mask debounced LCB in WCB 000PMS00
	andi	LCB,0x03	;mask LCB increase/decrease bits 000000ID
	or	LCB,WCB		;create debounce LCB 000PMSID (WCB=junk)

	mov	WCB,XCB
	andi	WCB,0x1C	;mask XCB 000PMS00 		         >add XCB
	or	LCB,WCB		;merge LCB 000PMSID with XCB 000PMS00 (WCB=junk)
	andi	XCB,0x03	;clear XCB so PMS not read next iteration
				; but save 000000ID if needed in external mode
	lds	WCB,PINSNOW	;move LCBNOW\ to LCBLAST\ for next iteration
	sts	PINSLAST,WCB
	andi	MCB,0xC0	;mask MCB XA------ and clear Hardwire bit
	or	MCB,LCB		;CREATE MASTER CONTROL BYTE
				; merge new button pressed info into MCB

testhard:		;HARDWIRE OVERRIDES ALL: PROGRAM, EXTERNAL, & ALARM MODES
	sbic	PIND,3		;is unit in hardwire remote (PD3)?
	rjmp	testpgm		; if not, PD3=1 exit
	sbr	MCB,0x20	; if so, PD3=0, set hardwire bit (MCB bit5)
	andi	MCB,0xFC	;mask off local Increase/Decrease bits
	sbis	PIND,7		;test if Increase Button pressed (PD7)
	sbr	MCB,0x02	; if so, PD7=0, set I bit (MCB bit1)
	sbis	PIND,6		;test if Decrease Button pressed (PD6)
	sbr	MCB,0x01	; if so, PD6=0, set D bit (MCB bit0)
	ldi	r16,0x48	;'H'	;start building labels
	sts	$6B,r16
	ldi	r16,0x4D	;'M'
	sts	$6C,r16
	ldi	r16,0x41	;'A'
	sts	$6D,r16
	ldi	r16,0x4E	;'N'
	sts	$6E,r16
	rjmp 	manual					;are in "HMAN"
			;**********************************************************
testpgm:		;TEST IF IN PROGRAMMING MODE, OR ENTERING PROGRAM MODE
	sbrc	STATUS,7	;test if already in programming mode
	rjmp	inpgmmode	; if set, execute programming
	sbrs	MCB,4		;test if program button pressed?
	rjmp	testmod		; if not, goto test if mode button pressed
pgmpress:			; if so, handle program button
	ldi	r16,0x80				;INITIALIZE PROGRAMMING MODE
	or	STATUS,r16	;set in_programming_mode bit (STATUS bit7)
	clr	MIDX
	sts	MINDX,MIDX	;initialize menu index to MINDX=0
	clr	SIDX
	sts	SINDX,SIDX	;initialize select index to SINDX=0
	rjmp	prgmdis
inpgmmode:					;>>>> ALREADY IN PROGRAMMING MODE
	lds	MIDX,MINDX	;load in program menu index
					;***************************************
					;>>>>>>>>>>>> TEST IF MODE BUTTON PRESSED
	sbrs	MCB,3		;test if mode button pressed?   >>>> PROGRAM MODE
	rjmp	menunotpressed	; if not continue, test pgm button
pgmmenupressed:					;>>>> SELECT PROGRAM MENU ITEM
	inc	MIDX		; else increment mode
ctlgrp:	cpi	MIDX,6			;test if wrap around CONTRL group
	brne	ingrp		; pg_msg:	.db	"PGAIN "	;mindx=00
	ldi	MIDX,0		; ig_msg:	.db	"IGAIN "	;mindx=01
				; dg_msg:	.db	"DGAIN "	;mindx=02
				; tc_msg:	.db	"TC    "	;mindx=03
				; dz_msg:	.db	"DZ    "	;mindx=04
				; cl_msg:	.db	"CLAMP "	;mindx=05
ingrp:	cpi	MIDX,14			;test if wrap around INPUT group
	brne	outgrp		; imx_msg:	.db	"INMAX "	;mindx=06
	ldi	MIDX,6		; imn_msg:	.db	"INMIN "	;mindx=07
				; a_msg:	.db	"Ax^3  "	;mindx=08
				; b_msg:	.db	"Bx^2  "	;mindx=09
				; c_msg:	.db	"Cx    "	;mindx=10
				; d_msg:	.db	"D     "	;mindx=11
				; e_msg:	.db 	"Ex^.5 "	;mindx=12
				; fil_msg:	.db	"FILTR "	;mindx=13
outgrp:	cpi	MIDX,16			;test if wrap around OUTPUT group
	brne	limgrp		; mg_msg:	.db	"MGAIN "	;mindx=14
	ldi	MIDX,14		; ag_msg:	.db	"AGAIN "	;mindx=15
limgrp:	cpi	MIDX,22			;test if wrap around LIMITS group
	brne	almgrp		; sul_msg:	.db	"SPUL  "	;mindx=16
	ldi	MIDX,16		; sll_msg:	.db	"SPLL  "	;mindx=17
				; iul_msg:	.db	"INUL  "	;mindx=18
				; ill_msg:	.db	"INLL  "	;mindx=19
				; oul_msg:	.db	"OUTUL "	;mindx=20
				; oll_msg:	.db	"OUTLL "	;mindx=21
almgrp:	cpi	MIDX,25			;test if wrap around ALARMS group
	brne	modgrp		; bul_msg:	.db	"BADUL "	;mindx=22
	ldi	MIDX,22		; bll_msg:	.db	"BADLL "	;mindx=23
				; pc_msg:	.db	"PC    "	;mindx=24
modgrp:	cpi	MIDX,31			;test if wrap around MODES group
	brlt	stmidx		; uni_msg:	.db	"UNITS "	;mindx=25
	ldi	MIDX,25		; ntc_msg:	.db	"NUMTC "	;mindx=26
				; spm_msg:	.db	"SPMOD "	;mindx=27
				; oty_msg:	.db	"OUTYP "	;minxx=28
				; ial_msg:	.db	"INALG "	;mindx=29
				; ity_msg:	.db	"INTYP "	;mindx=30
stmidx:	sts	MINDX,MIDX		;store new program menu index
	clr	SIDX		;initialize SELECT index for each new menu item
	sts	SINDX,SIDX
	rjmp	prgmdis
						;************************************
menunotpressed:					;>>>>> TEST IF PROGRAM BUTTON PRESSED
	sbrs	MCB,4		;test if program button pressed?
	rjmp	pgmnotpressed	; if not continue
pgmgrouppressed:				;>>>>> SELECT PROGRAM GROUP
			;MODES:  25=UNITS 26=NUMTC 27=SPMOD 28=OUTYP 29=INALG 30=INTYP
	cpi	MIDX,25		;test if in MODES group
	brlt	prgm1		; if not continue
exitpgmmode:			; if so, exit prgm mode
	ldi	r16,0x7F				;EXIT PROGRAMMING MODE
	and	STATUS,r16	;clear in_programming_mode bit (STATUS bit7)
	sts	XSTAT,STATUS	;store updated STATUS byte to SRAM parameter XSTAT
	
			;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>NEED TO STORE EEPROM HERE
	
	rjmp	killtime	; or to makedisplay??????
			;ALARMS: 22=BADUL 23=BADLL 24=PC
prgm1:	cpi	MIDX,22		;test if in ALARMS group
	brlt	prgm2		; if not continue
	ldi	MIDX,25		; if so, go to MODES group
	rjmp	updatepgmgroup
			;LIMITS: 16=SPUL 17=SPLL 18=INUL 19=INLL 20=OUTUL 21=OUTLL
prgm2:	cpi	MIDX,16		;test if in LIMITS group
	brlt	prgm3		; if not continue
	ldi	MIDX,22		; if so, go to ALARMS group
	rjmp	updatepgmgroup
			;OUTPUT: 14=MGAIN 15=AGAIN
prgm3:	cpi	MIDX,14		;test if in OUTPUT group
	brlt	prgm4		; if not continue
	ldi	MIDX,16		; if so, go to LIMITS group
	rjmp	updatepgmgroup
			;INPUT: 6=INMAX 7=INMIN 8=A 9=B 10=C 11=D 12=E 13=FILTR			
prgm4:	cpi	MIDX,6		;test if in INPUT group
	brlt	prgm5		; else, in CONTRL group
	ldi	MIDX,14		; if so, go to OUTPUT group
	rjmp	updatepgmgroup
			;CONTRL: 0=PGAIN  1=IGAIN  2=DGAIN  3=TC  4=DZ  5=CLAMP
prgm5:	ldi	MIDX,6		;are in CONTRL group, go to INPUT group
	rjmp	updatepgmgroup
updatepgmgroup:
	sts	MINDX,MIDX	;  and store new MENU index
	clr	SIDX		;initialize program SELECT index
	sts	SINDX,SIDX
	rjmp	prgmdis
					;*****************************************
pgmnotpressed:				; >>>>>>>> DO PROGRAM SELECT / SIGN BUTTON
	sbrs	MCB,2		;test if select button pressed?
	rjmp	selnotpressed	; if not continue, test pgm button
pgmselpressed:							;>>>> SELECT / SIGN
spmpar:	cpi	MIDX,0x27	;check if select button used on SPMOD
	brne	otypar
	ldi	r16,0x40	;toggle STATUS bit6 between 1=Track and 0=Hold
	eor	STATUS,r16
	rjmp	prgmdis
otypar:	cpi	MIDX,0x28	;check if select button used on OUTYP
	brne	ialpar
	ldi	r16,0x10	;toggle STATUS bit4 between 1=4-20mA and 0=0-20mA
	eor	STATUS,r16
	rjmp	prgmdis
ialpar:	cpi	MIDX,0x29	;check if select button used on INALG
	brne	itypar
	ldi	r16,0x08	;toggle STATUS bit3 between 1=nOrder and 0=Linear
	eor	STATUS,r16
	rjmp	prgmdis
itypar:	cpi	MIDX,0x30	;check if select button used on INTYP
	brne	unipar
	inc	STATUS		;toggle thru four input type modes
	mov	r16,STATUS
	andi	r16,0x07	;mask off input type in temp
	cpi	r16,4		;check if INTYP is valid
	brlo	jumprgmdis	; if INTYP is between 0 and 3, then it's valid
invalidintype:
	mov	r16,STATUS
	andi	r16,0xF8	;clear INTYP bits210
	mov	STATUS,r16
jumprgmdis:
	rjmp	prgmdis
unipar:	cpi	MIDX,25		;check if select button is used on UNITS
	brne	selsign		; if not, goto select sign on signed parameters
	lds	SIDX,SINDX		
	inc	SIDX
	cpi	SIDX,3		;check if SIDX above maximum value
	brlo	stsidx		; if not, continue
	clr	SIDX		; if so, reset SIDX select index
stsidx:	sts	SINDX,SIDX	;store the select index pointer
	rjmp	prgmdis
selsign:
	cpi	MIDX,3		;check if TC
	breq	neverneg
	cpi	MIDX,4		;check if DZ
	breq	neverneg
	cpi	MIDX,5		;check if CLAMP
	breq	neverneg
	cpi	MIDX,13		;check if FILTR
	breq	neverneg
	cpi	MIDX,14		;check if MGAIN
	breq	neverneg
	cpi	MIDX,15		;check if AGAIN
	breq	neverneg
	cpi	MIDX,20		;check if OUTUL
	breq	neverneg
	cpi	MIDX,21		;check if OUTLL
	breq	neverneg
	cpi	MIDX,24		;check if PC
	breq	neverneg
	cpi	MIDX,26		;check if NUMTC
	breq	neverneg
togglesign:			;is signed parameter, must change sign
	mov	r16,MIDX	;multiply MIDX times three
	mov	r17,r16
	lsl	r17
	add	r16,r17
	clr	r17		;>>>> multiply MIDX times 3 and add to $80 below
	ldi	ZL,$82		;create parameter address
	clr	ZH
	add	ZL,r16
	adc	ZH,r17
	ldi	r16,0x01	;create toggle mask
	ld	fpxE,Z		;load parameter into fpx
	eor	fpxE,r16	;toggle sign by exclusive or
	st	Z,fpxE		;store parameter from fpx
neverneg:
	rjmp	prgmdis				;**********************************
selnotpressed:					;>>>>>>>> PROGRAM INCREASE/DECREASE
	sbrc	MCB,1		;test if increase button pressed?
	rjmp	pgmincrpressed	; if so, handle increase program parameter function
	sbrc	MCB,0		;test if decrease button pressed?
	rjmp	pgmdecrpressed	; if so, handle decrease program parameter function
	rjmp	prgmdis		; if not, nothing to change, goto program display
pgmincrpressed:
pgmdecrpressed:
	cpi	MIDX,13		;check if FILTR
	breq	changeword
	cpi	MIDX,14		;check if MGAIN
	breq	changeword
	cpi	MIDX,20		;check if OUTUL
	breq	changeword
	cpi	MIDX,21		;check if OUTLL
	breq	changeword
	rjmp	morechange1
changeword:		;change percent values:
	mov	r16,MIDX	;multiply MIDX times three
	mov	r17,r16
	lsl	r17
	add	r16,r17
	clr	r17		;>>>> multiply MIDX times 3 and add to $80 below
	ldi	ZL,$80		;create parameter address
	clr	ZH
	add	ZL,r16
	adc	ZH,r17
	ld	YL,Z+		;load parameter into fpx
	ld	YH,Z
incwd:	sbrs	MCB,1		;test if increase button pressed?
	rjmp	decwd
	adiw	YH:YL,16	; if so, increment word
	brcc	endwd		;check if overflow
	ldi	YH,0xFF		; if so, set to maximum value
	ldi	YL,0xFF
	rjmp	endwd
decwd:	sbrs	MCB,0		;test if decrease button pressed?
	rjmp	endwd
	sbiw	YH:YL,16	; if so, decrement word
	brcc	endwd		;check if underflow
	ldi	YH,0x00		; if so, set to minimum value
	ldi	YL,0x00
endwd:	st	Z,YH		;store parameter
	st	-Z,YL
	rjmp	prgmdis

morechange1:
unichg:	cpi	MIDX,25
	breq	changeunits
	rjmp	morechange2
changeunits:		;change units of measure
	lds	SIDX,SINDX
	ldi	ZH,high($67)
	ldi	ZL,low($67)	;init Z-pointer
	cpi	SIDX,0
	breq	ldunits
	ldi	ZH,high($68)
	ldi	ZL,low($68)	;init Z-pointer
	cpi	SIDX,1
	breq	ldunits
	ldi	ZH,high($69)
	ldi	ZL,low($69)	;init Z-pointer
ldunits:
	ld	r16,Z		;load ascii unit character into working
incuni:	sbrc	MCB,1		;test if increase button pressed?
	inc	r16		; if so, increment word
decuni:	sbrc	MCB,0		;test if decrease button pressed?
	dec	r16		; if so, decrement word
	cpi	r16,27		;make a minumum ascii value
	brsh	stunit
	ldi	r16,27
stunit:	st	Z,r16		;store ascii unit character
	rjmp	prgmdis
morechange2:
ntcpar:	cpi	MIDX,26		;check if number of time constants
	breq	changenumtc
	rjmp	changeregpara
changenumtc:		;change number of time constants
	lds	r16,NUMTC
incntc:	sbrs	MCB,1		;test if increase button pressed?
	rjmp	decntc
	inc	r16		; if so, increment word
	cpi	r16,10
	brlo	endntc
	ldi	r16,9
	rjmp	endntc
decntc:	sbrs	MCB,0		;test if decrease button pressed?
	rjmp	endntc
	dec	r16		; if so, decrement word
	cpi	r16,2
	brsh	endntc
	ldi	r16,2
endntc:	sts	NUMTC,r16	;store ascii unit character
	rjmp	prgmdis

changeregpara:		;change regular parameter
	mov	r16,MIDX	;multiply MIDX times three
	mov	r17,r16
	lsl	r17
	add	r16,r17
	clr	r17		;>>>> multiply MIDX times 3 and add to $80 below
	ldi	ZL,$80		;create parameter address
	clr	ZH
	add	ZL,r16
	adc	ZH,r17
	ld	fpxL,Z+		;load parameter into fpx
	ld	fpxH,Z+
	ld	fpxE,Z+
incpar:	sbrs	MCB,1		;test if increase button pressed?
	rjmp	decpar
	ldi	fpyL,0xA9	;fpy=1.0025
	ldi	fpyH,0x19
	ldi	fpyE,0x02
	rjmp	chgpar
decpar:	sbrs	MCB,0		;test if decrease button pressed?
	rjmp	endpar
	ldi	fpyL,0x49	;fpy=0.9975
	ldi	fpyH,0xFF
	ldi	fpyE,0x00
chgpar:	rcall	fpmuly_x
endpar:	st	-Z,fpxE		;store parameter from fpx
	st	-Z,fpxH
	st	-Z,fpxL
	rjmp	prgmdis
	
prgmdis:		; pgm_dis:	.db	"group_ u/m PRGM  item_=   +NNN.N"
	;as compared to: oper_dis:	.db	"SPxxx.xu/m MANL PVxxx.x %OUTnn.n"
	lds	MIDX,MINDX
	ldi	r16,0x20	;' '			>>>build program display
	sts	$66,r16
	ldi	r16,0x50	;'P'
	sts	$6B,r16
	ldi	r16,0x52	;'R'
	sts	$6C,r16
	ldi	r16,0x47	;'G'
	sts	$6D,r16
	ldi	r16,0x4D	;'M'
	sts	$6E,r16
	ldi	r16,0x20	;' '
	sts	$70,r16
	ldi	r16,0x3D	;'='
	sts	$76,r16
	ldi	r16,0x20	;' '
	sts	$77,r16
	ldi	r16,0x20	;' '
	sts	$78,r16
	ldi	r16,0x20	;' '
	sts	$79,r16
	ldi	r16,0x20	;' '
	sts	$7A,r16
pgmmodes:					; mod_msg:	.db	"MODES "
	cpi	MIDX,25
	brlt	pgmalarms
	ldi	ZH,high(mod_msg*2)
	ldi	ZL,low(mod_msg*2)	;init Z-pointer
	rjmp	writepgmgroup
pgmalarms:					; alm_msg:	.db	"ALARMS"
	cpi	MIDX,22
	brlt	pgmlimits
	ldi	ZH,high(alm_msg*2)
	ldi	ZL,low(alm_msg*2)	;init Z-pointer
	rjmp	writepgmgroup
pgmlimits:					; lim_msg:	.db	"LIMITS"
	cpi	MIDX,16
	brlt	pgmoutput
	ldi	ZH,high(lim_msg*2)
	ldi	ZL,low(lim_msg*2)	;init Z-pointer
	rjmp	writepgmgroup
pgmoutput:					; out_msg:	.db	"OUTPUT"
	cpi	MIDX,14
	brlt	pgminput
	ldi	ZH,high(out_msg*2)
	ldi	ZL,low(out_msg*2)	;init Z-pointer
	rjmp	writepgmgroup
pgminput:					; in_msg:	.db	"INPUT "
	cpi	MIDX,6
	brlt	pgmcontrol
	ldi	ZH,high(in_msg*2)
	ldi	ZL,low(in_msg*2)	;init Z-pointer
	rjmp	writepgmgroup	
pgmcontrol:					; ctl_msg:	.db	"CONTRL"
	ldi	ZH,high(ctl_msg*2)
	ldi	ZL,low(ctl_msg*2)	;init Z-pointer
	rjmp	writepgmgroup
writepgmgroup:			;write program group label to the SRAM
	ldi	YH,high(SRAM00)
	ldi	YL,low(SRAM00)		;init Y-pointer
	ldi	flashsize,6		;n
	rcall	flash2ram		;copy n bytes
writepgmitem:
	mov	r16,MIDX		;multiply MIDX times six
	lsl	r16
	mov	r17,r16
	lsl	r17
	add	r16,r17
	clr	r17		;>>>> multiply MIDX times 6 and add to Z below
	ldi	ZH,high(pg_msg*2)
	ldi	ZL,low(pg_msg*2)	;init Z-pointer
	add	ZL,r16
	adc	ZH,r17
	ldi	YH,high(SRAM10)
	ldi	YL,low(SRAM10)		;init Y-pointer
	ldi	flashsize,6		;n
	rcall	flash2ram		;copy n bytes

writepgmparameter:
	mov	r16,MIDX	;multiply MIDX times three
	mov	r17,r16
	lsl	r17
	add	r16,r17
	clr	r17		;>>>> multiply MIDX times 3 and add to $80 below
	ldi	ZL,$80		;create parameter address
	clr	ZH
	add	ZL,r16
	adc	ZH,r17
	ld	fpxL,Z+		;load parameter into fpx
	ld	fpxH,Z+
	ld	fpxE,Z+
	mov	r16,MIDX	;multiply MIDX times three
	cpi	r16,13		;check if parameter is FILTR
	breq	expiszero
	cpi	r16,14		;check if parameter is MGAIN
	breq	expiszero
	cpi	r16,20		;check if parameter is OUTUL
	breq	expiszero
	cpi	r16,21		;check if parameter is OUTLL
	breq	expiszero
	cpi	r16,25		;check if parameter is a flag or irregular
	brge	irregularparameter
	rjmp	normalparameter
expiszero:
	ldi	fpxE,0x00	;special case for exponentless parameters
	rjmp	normalparameter

irregularparameter:
unitsofmeas:
	cpi	r16,25		;check if parameter is UNITS
	brne	numberoftimeconst
	lds	r16,$67		;copy units of measure over to subparameter
	sts	$7A,r16
	lds	r16,$68
	sts	$7B,r16
	lds	r16,$69
	sts	$7C,r16	
	rjmp 	mergederase
numberoftimeconst:
	cpi	r16,26		;check if parameter is NUMTC
	brne	setpointmode
	lds	r16,NUMTC
	ori	r16,0x30	;write number of time constants to subparameter
	sts	$7A,r16
	ldi	r16,0x20	;' '
	sts	$7B,r16
	ldi	r16,0x20	;' '
	sts	$7C,r16
mergederase:
	ldi	r16,0x20	;' '
	sts	$7D,r16
	ldi	r16,0x20	;' '
	sts	$7E,r16
	ldi	r16,0x20	;' '
	sts	$7F,r16
	rjmp 	finishparameter
setpointmode:
	cpi	r16,27		;check if parameter is SPMOD
	brne	outputtype
	ldi	ZH,high(spm0_msg*2)	;test if subparameter "Hold  " (STATUS bit6=0)
	ldi	ZL,low(spm0_msg*2)
	sbrs	STATUS,6
	rjmp	writesubparameter
	ldi	ZH,high(spm1_msg*2)	;test if subparameter "Track " (STATUS bit6=1)
	ldi	ZL,low(spm1_msg*2)
	rjmp 	writesubparameter
outputtype:			;check if parameter is OUTYP
	cpi	r16,28
	brne	inputalgo
	ldi	ZH,high(oty0_msg*2)	;test if subparameter "0-20mA" (STATUS bit4=0)
	ldi	ZL,low(oty0_msg*2)
	sbrs	STATUS,4
	rjmp	writesubparameter
	ldi	ZH,high(oty1_msg*2)	;test if subparameter "4-20mA" (STATUS bit4=1)
	ldi	ZL,low(oty1_msg*2)
	rjmp 	writesubparameter
inputalgo:			;check if parameter is INALG
	cpi	r16,29
	brne	inputtype
	ldi	ZH,high(ial0_msg*2)	;test if subparameter "Linear" (STATUS bit3=0)
	ldi	ZL,low(ial0_msg*2)
	sbrs	STATUS,3
	rjmp	writesubparameter
	ldi	ZH,high(ial1_msg*2)	;test if subparameter "nOrder" (STATUS bit3=1)
	ldi	ZL,low(ial1_msg*2)
	rjmp 	writesubparameter
inputtype:			;check if parameter is INTYP
	cpi	r16,30
	brne	error
	sbrs	STATUS,1	;check if subparameter is a voltage input
	rjmp	isvoltintype	; if so, branch
	ldi	ZH,high(ity2_msg*2)	;test if subparameter "Timer " (STATUS bit1=1)
	ldi	ZL,low(ity2_msg*2)	;                              (STATUS bit0=0
	sbrs	STATUS,0	;check if subparameter is Timer or Freq
	rjmp	writesubparameter
	ldi	ZH,high(ity3_msg*2)	;test if subparameter "Freq  " (STATUS bit1=1)
	ldi	ZL,low(ity3_msg*2)	;                              (STATUS bit0=1)
	rjmp	writesubparameter
isvoltintype:
	ldi	ZH,high(ity0_msg*2)	;test if subparameter "0-5V  " (STATUS bit1=0)
	ldi	ZL,low(ity0_msg*2)	;                              (STATUS bit0=0
	sbrs	STATUS,0	;check if subparameter is 0-5V or1-5V
	rjmp	writesubparameter
	ldi	ZH,high(ity1_msg*2)	;test if subparameter "1-5V  " (STATUS bit1=0)
	ldi	ZL,low(ity1_msg*2)	;                              (STATUS bit0=1)
	rjmp	writesubparameter
writesubparameter:		;write subparameter to SRAM display
	ldi	YH,high($7A)
	ldi	YL,low($7A)		;init Y-pointer
	ldi	flashsize,6		;n
	rcall	flash2ram		;copy n bytes
	rjmp 	finishparameter

error:	ldi	fpxL,0xFF		;this code should never execute
	ldi	fpxH,0xFF
	ldi	fpxE,0x7E
	rjmp	normalparameter

normalparameter:
	ldi 	ZL,low($7B)	;load pointer of SP into Z
	ldi 	ZH,high($7B)
	rcall	display		;draw parameter to display
finishparameter:
	rjmp	killtime
;*****************************


testmod:								;MODE
	sbrs	MCB,3		;test if mode button pressed?
	rjmp	testsel
modpress:
	sbis	PORTB,ALM 	;test if in alarm mode, (PB2=0)
	rjmp	modinALM	; if so, reset PB2=1, leave in manual
modnotinALM:
	ldi	WCB,0x40	; else if not, toggle mode
	eor	MCB,WCB		;toggle master control mode bit
	rjmp	testsel		; if MCB bit7=0, then in manual, continue
modinALM:		;this routine attempts to clear the alarm mode
	sbi	PORTB,ALM	;clear alarm bit (1=off)
	cbr	MCB,0x40	;reset mode to manual
	rjmp	testsel
testsel:							;SELECT
	sbrs	MCB,2		;test if select button pressed?
	rjmp	testxID		; if not, goto mode
selpress:			; if so, handle select button
	ldi	WCB,0x80
	eor	MCB,WCB		;toggle master control select bit
testxID:
	sbrs	MCB,7		;test if in external mode?
	rjmp	testalm		; if not, goto test alarm mode
	andi	MCB,0xFC	; if so, mask out (erase) local incr/decr
	or	MCB,XCB		;merge external incr/decr into MASTER CONTROL BYTE
	clr	XCB		;clear XCB so ID not read next iteration
testalm:
	sbic	PORTB,2		;is unit in alarm mode (PB2)?
	rjmp	testext		; if not, PB2=1 exit
inALRM:				; if so, PB2=0, flag in alarm
	cbr	MCB,0x40	;trip controller into manual mode. this variation
				; permits manual increase/decrease moves while
				; in alarm mode.  press 'M' to clear alarm
	ldi	r16,0x41	;'A'
	sts	$6B,r16
	ldi	r16,0x4C	;'L'
	sts	$6C,r16
	ldi	r16,0x52	;'R'
	sts	$6D,r16
	ldi	r16,0x4D	;'M'
	sts	$6E,r16
	rjmp	manual						;are in "ALRM"
testext:
	sbrs	MCB,7		;test if in external mode?
	rjmp	inLocal		; if not, goto local
inExternal:
	sbrs	MCB,6		;test if in auto/manual
	rjmp	inUMAN
inUAUT:
	ldi	r16,0x55	;'U'
	sts	$6B,r16
	ldi	r16,0x41	;'A'
	sts	$6C,r16
	ldi	r16,0x55	;'U'
	sts	$6D,r16
	ldi	r16,0x54	;'T'
	sts	$6E,r16
	rjmp	auto						;are in "UAUT"
inUMAN:
	ldi	r16,0x55	;'U'
	sts	$6B,r16
	ldi	r16,0x4D	;'M'
	sts	$6C,r16
	ldi	r16,0x41	;'A' 
	sts	$6D,r16
	ldi	r16,0x4E	;'N'
	sts	$6E,r16
	rjmp	manual						;are in "UMAN"
inLocal:
	sbrs	MCB,6		;test if in auto/manual
	rjmp	inMANL
inAUTO:
	ldi	r16,0x41	;'A'
	sts	$6B,r16
	ldi	r16,0x55	;'U'
	sts	$6C,r16
	ldi	r16,0x54	;'T'
	sts	$6D,r16
	ldi	r16,0x4F	;'O'
	sts	$6E,r16
	rjmp	auto						;are in "AUTO"
inMANL:
	ldi	r16,0x4D	;'M'
	sts	$6B,r16
	ldi	r16,0x41	;'A' 
	sts	$6C,r16
	ldi	r16,0x4E	;'N' 
	sts	$6D,r16
	ldi	r16,0x4C	;'L'
	sts	$6E,r16
	rjmp	manual						;are in "MANL"

manual:
	rcall	loadx_fp0
	ldi	ZH,high(SUMERR)	;zero out summation of error if not in auto
	ldi	ZL,low(SUMERR)
	rcall	storx_zptr
	ldi	ZH,high(LERR)	;zero out last error if not in auto
	ldi	ZL,low(LERR)
	rcall	storx_zptr
testspmode:
	sbrs	STATUS,6	;test output type OUTYP: 0=0-20mA, 1=4-20mA
	rjmp	ifsphold	;if STATUS bit6=0 use hold last setpoint
ifsptrack:			; else bit6=1, use track process value to setpoint
	rcall	loadx_pv	;set SP equal to PV
	rcall	storx_sp
ifsphold:		;if hold SP in manual, nothing needs to be done
domanual:
	rcall	loadx_out	;^fpy=???, fpx=OUT
	ldi	ZH,high(MGAIN) 	;manually load MGAIN, default exponent=0
	ldi	ZL,low(MGAIN)
	rcall	loadxw_zptr	;^fpz=???, fpy=OUT, fpx=MGAIN
	sbrc	MCB,1		;test if increase requested
	rjmp	manincr
	sbrc	MCB,0		;test if decrease requested
	rjmp	mandecr
	rjmp	outlimits
manincr:
	add	outL,fpxL	;increment output by unit
	adc	outH,fpxH
	brcc	endmanincr	;test if exceeded FFFF (.9999)
	ser	fpxL		; if so, set to FFFF
	ser	fpxH
	rcall	storx_out
endmanincr:
	rjmp	outlimits
mandecr:
	sub	outL,fpxL	;decrement output by unit
	sbc	outH,fpxH
	brcc	endmandecr	;test if exceeded 0000 (.0000)
	clr	fpxL		; if so, set to 0000
	clr	fpxH
	rcall	storx_out
endmandecr:
	rjmp	outlimits

auto:
	rcall	loadx_sp	;load fpx <- SP
	ldi	ZH,high(AGAIN)
	ldi	ZL,low(AGAIN)
	rcall	loadx_zptr	;load fpy <- SP, fpx <- AGAIN
	sbrc	MCB,1		;test if setpoint increase requested
	rjmp	autincr
	sbrc	MCB,0		;test if setpoint decrease requested
	rjmp	autdecr
	rjmp	calculate
autincr:		;if increase required
	rcall	fpaddy_x	;fpx = SP + AGAIN	>increment setpoint
	rcall	storx_sp	;SP = fpx
	ldi	ZH,high(SPUL)	;test if setpoint upper limit exceeded
	ldi	ZL,low(SPUL)
	rcall	loady_zptr	;fpy = SPUL, fpx = SP
	rcall	fpsuby_x	;fpx = SPUL - SP, overlimit if result negative
	sbrs	fpxE,0		;test if above result is negative (bit0=sign)
	rjmp	calculate	; if positive, SP is valid
	ldi	ZH,high(SPUL)	; skip here if negative, SP must be clamped to SPUL
	ldi	ZL,low(SPUL)
	rcall	loadx_zptr	;fpx = SPUL
	rcall	storx_sp	;SP = fpx (SPUL)
	rjmp	calculate
autdecr:
	rcall	fpsuby_x	;fpx = SP - AGAIN	>decrement setpoint
	rcall	storx_sp	;SP = fpx
	ldi	ZH,high(SPLL)	;test if setpoint lower limit exceeded
	ldi	ZL,low(SPLL)
	rcall	loadx_zptr	;fpy = SP, fpx = SPLL
	rcall	fpsuby_x	;fpx = SP - SPLL, overlimit if result negative
	sbrs	fpxE,0		;test if above result is negative (bit0=sign)
	rjmp	calculate	; if positive, SP is valid
	ldi	ZH,high(SPLL)	; skip here if negative, SP must be clamped to SPLL
	ldi	ZL,low(SPLL)
	rcall	loadx_zptr	;fpx = SPLL
	rcall	storx_sp	;SP = fpx (SPLL)
	rjmp	calculate

calculate:
pidalgorithm:		;COMPUTE CONTROL ALGORITHM HERE
	rcall	loadx_sp	;calculate error
	rcall	loadx_pv	;^fpz=???, fpy=SP, fpx=PV
	rcall	fpsuby_x	;^fpy=???, fpx=SP-PV, defined as ERR
	rcall	fillstack_x	;^fpt=ERR
deadzone:
	rcall	loadx_fp0	;^fpy=ERR, fpx=.0000
	rcall	swapy_x		;^fpz=ERR, fpy=.0000, fpx=ERR 
	andi	fpxE,0xFE	;take absolute value of error,
				;^fpz=ERR, fpy=.0000, fpx=ABS(ERR)
	ldi	ZH,high(DZ)	;load deadzone
	ldi	ZL,low(DZ)
	rcall	loadx_zptr	;^fpt=ERR, fpz=.0000, fpy=ABS(ERR), fpx=DZ
	rcall	fpsuby_x	;^fpz=ERR, fpy=.0000, fpx=ABS(ERR)-DZ
	sbrs	fpxE,0		;test if above result is negative (bit0=sign)
	rcall	popx		; if positive, error is greater than deadzone,
				;  double pop fpstack, use fpx=ERR=ERR
	rcall	popx		; if negative skip, error is less than deadzone,
				;  single pop fpstack, use fpx=ERR=.0000
	rcall	fillstack_x	;^fpx=deadzonedERR, defined as ERR

protectionclamp:
	andi	fpxE,0xFE	;take absolute value of error,
				;^fpy=ERR, fpx=ABS(ERR)
	ldi	ZH,high(PCLAMP)	;test if protection clamp exceeded
	ldi	ZL,low(PCLAMP)
	rcall	loady_zptr	;^fpz=ERR, fpy=PC, fpx=ABS(ERR)
	rcall	fpsuby_x	;^fpy=ERR, fpx=PC-ABS(ERR),
				; protection clamp overlimit if result negative
	sbrs	fpxE,0		;test if above result is negative (bit0=sign)
	rjmp	outclamp	; if positive, error is less than protection clamp,
				;   continue with PID algorithm
	cbi	PORTB,ALM	; else if negative turn on LED alarm
	rjmp	pcviolated	; do not execute PID algorithm, suspend control
outclamp:
	rcall	popx		;^fpx=ERR
	andi	fpxE,0xFE	;take absolute value of error,
				;^fpy=ERR, fpx=ABS(ERR)
	ldi	ZH,high(CLAMP)	;test if output clamp exceeded
	ldi	ZL,low(CLAMP)
	rcall	loady_zptr	;^fpz=ERR, fpy=CLAMP, fpx=ABS(ERR)
	rcall	fpsuby_x	;^fpy=ERR, fpx = CLAMP-ABS(ERR)
				;  clamp overlimit if result negative
	sbrs	fpxE,0		;test if above result is negative (bit0=sign)
	rjmp	clampvalid	; if positive, ERR is valid
clampviolated:		;set error equal to clamp, but must retain sign
	rcall	popx		;^fpx=ERR
	ldi	ZH,high(CLAMP)	; if negative, ERR exceeds CLAMP, limit to CLAMP
	ldi	ZL,low(CLAMP)
	rcall	loadx_zptr	;^fpy=ERR, fpx=CLAMP
	andi	fpxE,0xFE	;apply masks to compute sign of error
	andi	fpyE,0x01	;
	or	fpxE,fpyE	;^fpz=ERR, fpy=???, fpx=CLAMPEDERR
	rcall	fillstack_x	;set ^fpx=CLAMPEDERR, redefined as ERR
	rjmp	pidalgo
clampvalid:
	rcall	popx		;set ^fpx=ERR

pidalgo:		;here the PID algorithm is computed to determine the change
			; to the output.
proportional:
	ldi	ZH,high(PGAIN)	;load proportional gain
	ldi	ZL,low(PGAIN)
	rcall	loadx_zptr	;^fpy=ERR, load fpx=PGAIN
	rcall	fpmuly_x	;^fpy=ERR, fpx=PGAIN*ERR, defined as %DOUT (1st term)
	rcall	swapy_x		;^fpz=ERR, fpy=%DOUT, fpx=ERR
derivative:
	ldi	ZH,high(LERR)	;load last error
	ldi	ZL,low(LERR)
	rcall	loadx_zptr	;^fpt=ERR, fpz=%DOUT, fpy=ERR, fpx=LERR
	rcall	fpsuby_x	;^fpz=ERR, fpy=%DOUT, fpx=ERR-LERR, defined as DERR
	ldi	ZH,high(DGAIN)	;load derivative gain
	ldi	ZL,low(DGAIN)
	rcall	loadx_zptr	;^fpt=ERR, fpz=%DOUT, fpy=DERR, fpx=DGAIN
	rcall	fpmuly_x	;^fpz=ERR, fpy=%DOUT, fpx=DERR*DGAIN (3rd term)
	rcall	fpaddy_x	;^fpy=ERR, fpx=%DOUT+(DERR*DGAIN), redef as %DOUT
			;  %DOUT = (PGAIN*ERR) + (DGAIN*DERR)
	rcall	swapy_x		;^fpz=ERR, fpy=%DOUT, fpx=ERR
	ldi	ZH,high(LERR)	;store last error for next iteration
	ldi	ZL,low(LERR)
	rcall	storx_zptr	;^fpz=ERR, fpy=%DOUT, fpx=ERR

integral:
	rcall	pushx		;^fpt=ERR, fpz=%DOUT, fpy=fpx=ERR
	lds	fpyL,0x00
	lds	fpyH,NUMTC
	inc	fpyH
	swap	fpyH		;make f(NUMTC+1)
	lds	fpyE,0x00	;^fpt=ERR, fpz=%DOUT, fpy=f(NUMTC-1), fpx=ERR
	lds	fpxL,0x00
	lds	fpxH,NUMTC
	swap	fpxH		;make f(NUMTC)

			;should be f(NUMTC)= ((NUMTC*TC)-1)/NUMTC*TC
			;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>must redo


	lds	fpxE,0x00	;^fpt=ERR, fpz=%DOUT, fpy=f(NUMTC-1), fpx=f(NUMTC)
	rcall	fprec1_x	;^fpt=ERR, fpz=%DOUT, fpy=f(NUMTC-1), fpx=1/f(NUMTC)
	rcall	fpmuly_x	;^fpz=ERR, fpy=%DOUT, fpx=(NUMTC-1)/NUMTC
	ldi	ZH,high(SUMERR)	;load summation of errors
	ldi	ZL,low(SUMERR)
	rcall	loadx_zptr	;^fpt=ERR, fpz=%DOUT, fpy=(NUMTC-1)/NUMTC, fpx=SUMERR
	rcall	fpmuly_x	;^fpz=ERR, fpy=%DOUT, fpx=SUMERR*(NUMTC-1)/NUMTC
	rcall	swapz_y		;^fpt=ERR, fpz=%DOUT, fpy=ERR, fpx=f(SUMERR)
	rcall	fpaddy_x	;^fpz=ERR, fpy=%DOUT, fpx=f(SUMERR)+ERR
	ldi	ZH,high(SUMERR)	;store summation of error for next iteration
	ldi	ZL,low(SUMERR)
	rcall	storx_zptr	;SUMERR=SUMERR*(NUMTC-1)/NUMTC + ERR, redef as SUMMERR
	ldi	ZH,high(IGAIN)	;load integral gain
	ldi	ZL,low(IGAIN)
	rcall	loadx_zptr	;^fpt=ERR, fpz=%DOUT, fpy=SUMERR, fpx=IGAIN
	rcall	fpmuly_x	;^fpz=ERR, fpy=%DOUT, fpx=SUMERR*IGAIN
	rcall	fpaddy_x	; %DOUT = (PGAIN*ERR) + (DGAIN*DERR) + (IGAIN*SUMERR)
			;^fpz=ERR, fpy=%DOUT, fpx=%DOUT+(SUMERR*IGAIN), redef as %DOUT
	rcall	fillstack_x	;^fpx=%DOUT
	ldi	fpxL,0x99	;manually load .1000e-2 (.0010) scale from
	ldi	fpxH,0x19	;  percent to real and tenth second to seconds
	ldi	fpxE,0xFC	;^fpy=%DOUT, fpx=.0010
	rcall	fpmuly_x	;^fpy=%DOUT, fpx=DOUT
	ldi	ZH,high(TC)	;load time constant (in units of tenths of seconds)
	ldi	ZL,low(TC)
	rcall	loadx_zptr	;^fpz=%DOUT, fpy=DOUT, fpx=TC
	rcall	fprec1_x	;^fpz=%DOUT, fpy=DOUT, fpx=1/TC
	rcall	fpmuly_x	;^fpy=%DOUT, fpx=DOUT/TC
	rcall	loadx_out	;^fpz=%DOUT, fpy=DOUT/TC, fpx=OUT
	rcall	fpaddy_x	;^fpy=%DOUT, fpx=OUT+DOUT/TC, redefined as OUT

;	sts	$D2,fpxL			;>>>>>>>>>>>>>> TEMP STORE
;	sts	$D3,fpxH
;	sts	$D4,fpxE
	
cleanupout:
	cpi	fpxE,0x01	;check if new output greater than 100% (>.9999)
	brge	makeoutFFFF
	sbrc	fpxE,0		;check if new output is less than 0% (<.0000)
	rjmp	makeout0000
denormalizeout:
	cpi	fpxE,0x00	;check if new exponent normalized (if fpxE=0)
	breq	endcleanupout	;if exponent=0, then OK (.0000 < OUT < .9999)
	ldi	nL,10		;if not (.0000 < OUT < .1000),
	clr	nH		;  setup a divide mantissa by ten loop
	rcall	divx_n		;divide mantissa by 10
	inc	fpxE		;increment exponent fpxE=fpxE+1
	cpi	fpxL,0x00
	brne	denormalizeout
	cpi	fpxH,0x00
	brne	denormalizeout
	rjmp	makeout0000
makeoutFFFF:
	ser	fpxL		;make output 99.9% (FFFF)
	ser	fpxH
	rcall	storx_out
	rjmp	outlimits
makeout0000:
	clr	fpxL		;make output 0.0% (0000)
	clr	fpxH
	rcall	storx_out
	rjmp	outlimits
endcleanupout:
	rcall	storx_out
	rjmp	outlimits

outlimits:
	rcall	loadx_out	;reload output (limit checks follow)
	rcall	fillstack_x	;^fpx=OUT
outupperlimit:
	ldi	ZH,high(OUTUL)	;load output upper limit
	ldi	ZL,low(OUTUL)
	rcall	loadx_zptr	;^fpy=OUT, fpx=OUTUL
	ldi	fpxE,0x00	;need exponent
	rcall	pushx		;^fpz=OUT, fpy=fpx=OUTUL
	sub	fpxL,fpzL	;subtract fpyH:fpyL from fpxH:fpxL
	sbc	fpxH,fpzH	;^fpz=OUT, fpy=OUTUL, fpx=OUTUL-OUT
	brcc	outlowerlimit	;test if OUT exceeds OUTUL, borrow occurs if so
				; branch if no borrow occured (OUTUL>OUT)
	rcall	popx		;^fpy=OUT, fpx=OUTUL
	rcall	storx_out	; else store upper output limit as OUT=OUTLL
	rjmp	output
outlowerlimit:
	rcall	loadx_out	;reload output (limit checks follow)
	rcall	fillstack_x	;^fpx=OUT
	ldi	ZH,high(OUTLL)	;load output upper limit
	ldi	ZL,low(OUTLL)
	rcall	loadx_zptr	;^fpy=OUT, fpx=OUTLL
	ldi	fpxE,0x00	;need exponent
	rcall	pushx		;^fpz=OUT, fpy=fpx=OUTLL
	sub	fpxL,fpzL	;subtract fpyH:fpyL from fpxH:fpxL
	sbc	fpxH,fpzH	;^fpz=OUT, fpy=OUTLL, fpx=OUTLL-OUT
	brcs	output		;test if OUTLL exceeds OUT, no borrow occurs if so
				; branch if borrow occured (OUTLL<OUT)
	rcall	popx		;^fpy=OUT, fpx=OUTLL
	rcall	storx_out	; else store upper output limit as OUT=OUTLL
	rjmp	output

output:			;OUTPUT TO THE LTC1661 DIGITAL TO ANALOG CONVERTER
.equ	CS	=5		;PB5 is the D/A load enable (1->0) & execute (0->1)
.equ	SCK	=4		;PB4 is the strobe clock to the D/A Converter
.equ	Din	=3		;PB3 is the data input to the D/A Converter

	cbi	PORTB,Din	;ensure Din output starts low, set PB3=0
	cbi	PORTB,SCK	;ensure SCK output starts low, set PB4=0
	rcall	loadx_out	;^fpy=???,fpx=OUT
	rcall	fillstack_x	;^fpx=OUT
testouttype:
	sbrs	STATUS,4	;test output type OUTYP: 0=0-20mA, 1=4-20mA
	rjmp	if0_20ma	;if STATUS bit4=0 use 0-20
if4_20ma:		;if 4-20mA, rescale output .2000 to .9999
	ldi	fpxL,0xCC	;manually load .79999
	ldi	fpxH,0xCC
	ldi	fpxE,0x00	;^fpy=OUT, fpx=.79999 (.8)
	rcall	fpmuly_x	;^fpy=OUT, fpx=.8(OUT)
	rcall	pushx		;^fpz=OUT, fpy=.8(OUT)
	ldi	fpxL,0x33	;manually load .19999
	ldi	fpxH,0x33
	ldi	fpxE,0x00	;^fpz=OUT, fpy=.8(OUT), fpx=.19999 (.2)
	rcall	fpaddy_x	;^fpy=OUT, fpx=.8(OUT)+.2
if0_20ma:		;if 0-20mA, nothing needs to be done
makeoutput:		;prepare output parameter for download to LTC1661
	lsr	fpxH		;data to be sent is in the format;
	ror	fpxL		;	fpxH=dddddddd fpxL=dd------
	lsr	fpxH		;roll data through carry to accomplish:
	ror	fpxL		;	fpxH -> C -> fpxL
	lsr	fpxH		;because data must be reformatted to be outputted:
	ror	fpxL		;	fpxH=1111dddd fpxL=dddddd--
	lsr	fpxH
	ror	fpxL
	ori	fpxH,0xF0	;add command 1111 to load both D/A VoutA and VoutB
	cbi	PORTB,CS	;enable loading of the D/A, set PB5=0
	ldi	fpxE,16		;use unused Exponent Byte to load loop counter
	mov	nctr,fpxE
outloop:
	sbrc	fpxH,7		;test bit15 of output word, skip next if zero
 	sbi	PORTB,Din	; if next data output bit high, set PB3=1
	nop
	sbi	PORTB,SCK	;toggle D/A clock high, set PB4=1 active rising
	nop
	cbi	PORTB,Din	;reset data output bit low again, set PB3=0
	nop
	cbi	PORTB,SCK	;toggle D/A clock low, set PB4=0 inactive falling
				;roll bits through fpxL to carry to fpxH
	lsl	fpxL		;		C <- fpxL
	rol	fpxH		;	fpxH <- C, bit15 now ready for next read
	dec	nctr		;loop through all 16 output bits
	brne	outloop
	sbi	PORTB,CS	;disable CS\, which instructs D/A to upload
				; requested voltage to output on LTC1661 VoutA


pcviolated:
makedisplay:
	rcall	display_sp	;create operator display in SRAM $60-$7F
	rcall	display_pv
	rcall	display_out

	;HERE MUST KILL TIME to operate at .10 sec timeperiod
killtime:
;xend:	rjmp	input	;<<<<<<<<<<<<<<<<<<<<<<< TEMP DEBUGGING TO ONLY RUN ONCE


	rcall	delay5ms		;kill .005sec
 	sei			;check global interrupts.  All checks must be with
	nop			; stack clear, or stack might overwrite parameters.
	cli			;resume kill global interrupts
	rcall	delay5ms		;kill .010sec
 	sei			;check global interrupts.  All checks must be with
	nop			; stack clear, or stack might overwrite parameters.
	cli			;resume kill global interrupts
	rcall	delay5ms		;kill .015sec
 	sei			;check global interrupts.  All checks must be with
	nop			; stack clear, or stack might overwrite parameters.
	cli			;resume kill global interrupts
	rcall	delay5ms		;kill .020sec
 	sei			;check global interrupts.  All checks must be with
	nop			; stack clear, or stack might overwrite parameters.
	cli			;resume kill global interrupts
	rcall	delay5ms		;kill .025sec
 	sei			;check global interrupts.  All checks must be with
	nop			; stack clear, or stack might overwrite parameters.
	cli			;resume kill global interrupts
	rcall	delay5ms		;kill .030sec
 	sei			;check global interrupts.  All checks must be with
	nop			; stack clear, or stack might overwrite parameters.
	cli			;resume kill global interrupts
	rcall	delay5ms		;kill .035sec
 	sei			;check global interrupts.  All checks must be with
	nop			; stack clear, or stack might overwrite parameters.
	cli			;resume kill global interrupts
	rcall	delay5ms		;kill .040sec
 	sei			;check global interrupts.  All checks must be with
	nop			; stack clear, or stack might overwrite parameters.
	cli			;resume kill global interrupts
	rcall	delay5ms		;kill .045sec
 	sei			;check global interrupts.  All checks must be with
	nop			; stack clear, or stack might overwrite parameters.
	cli			;resume kill global interrupts
	rcall	delay5ms		;kill .050sec
 	sei			;check global interrupts.  All checks must be with
	nop			; stack clear, or stack might overwrite parameters.
	cli			;resume kill global interrupts
	rcall	delay5ms		;kill .055sec
 	sei			;check global interrupts.  All checks must be with
	nop			; stack clear, or stack might overwrite parameters.
	cli			;resume kill global interrupts
	rcall	delay5ms		;kill .060sec
 	sei			;check global interrupts.  All checks must be with
	nop			; stack clear, or stack might overwrite parameters.
	cli			;resume kill global interrupts

forever:rjmp	RESTART			;eternal loop	

;******************************************************************************
;****** factory defaults, all are stored in word blocks, not bytes

oper_dis:	.db	"SPxxx.x *F MANL PVxxx.x %OUTnn.n"
				;"u/m"             $67 units of measure
parameters:							;zero is 0000eEE
	.db	0x66,0x26	;+1.500  +199Ae1   $80 PGAIN - proportional gain
	.db	0x02,0x00	;+0.000  +199Ae1   $83 IGAIN - integral gain
	.db	0x00,0xEE

	.db	0x00,0x80	;+5.000  +199Ae1   $86 DGAIN - derivtive gain
	.db	0x02,0xCE	;+30.00  +199Ae1   $89 TC - time constant (only+)
	.db	0x4C,0x04

	.db	0x9A,0x19	; 1.000  +199Ae0   $8C DZ - deadzone (only+)
	.db	0x02,0xC2	;+999.0  +FFC3e3   $8F CLAMP - output clamp (on;y+)
	.db	0xFF,0x06

	.db	0x9A,0x19	;+100.0  +199Ae3   $92 INMAX - input maximum value
	.db	0x06,0x00	; .0000  0000e-9   $95 INMIN - input minimum value
	.db	0x00,0xEE

	.db	0x00,0x00	; .0000  0000e-9   $98 A - cubed coefficient
	.db	0xEE,0x00	; .0000  0000e-9   $9B B - cubed coefficient
	.db	0x00,0xEE

	.db	0x9A,0x19	;+100.0  +199Ae3   $9E C - linear coefficient (slope)
	.db	0x06,0x00	;-37.50  0000e-9   $A1 D - intercept coefficient
	.db	0x00,0xEE

	.db	0x00,0x00	; .0000  0000e-9   $A4 E - square root coefficient
	.db	0xEE,0x00	; .1250  8002[e0]  $A7 FILTR - input filter factor
	.db	0x20,0x00	;     0b00000101   $A9 NUMTC - #of time constants

	.db	0x48,0x01	;+.0050	 0148[e0]  $AA MGAIN - manual gain (word)
	.db	0x51,0x01	;     0b01010001   $AC XSTAT - status byte
			;bit6 SPMODE: 1=Track    bit4   OUTYP: 1=4-20ma
			;bit3 INALG:  0=Linear   bit2-0 INTYP: 1=1-5V
	.db	0x40,0x00	;+.2500	 +199Ae0   $AD AGAIN - automatic gain

	.db	0x01,0x40	;+250.0  +FFC3e2   $B0 SPUL - setpoint upper limit
	.db	0x06,0x02	;-50.00  -FFC3e2   $B3 SPLL - setpoint lower limit
	.db	0x80,0x05

	.db	0xC2,0xFF	;+999.0  +FFC3e3   $B6 INUL - input upper limit
	.db	0x06,0xC2	;-999.0  -FFC3e3   $B3 INLL - input lower limit
	.db	0xFF,0x07

	.db	0xFF,0xFF	; .9500  F333[e0]  $BC OUTUL - output upper limit
	.db	0x00,0xCD	;     0b--------   $BE PINSLAST (init 0x00)
				; .0500  0CCC[e0]  $BF OUTLL - output lower limit
	.db	0x00,0x00       ;     0b--------   $C1 PINSNOW (init 0x00)

	.db	0xCE,0x4C	;+300.0  +FFBDe3   $C2 BADUL - bad PV upper limit
	.db	0x06,0x02	;-50.00  -FFBDe3   $C5 BADLL - bad PV lower limit
	.db	0x80,0x05

	.db	0xFF,0xFF	;+9999.  +FFBDe4   $C8 PCLAMP - protect clamp (only+)
	.db	0x08,0x00	; ---.-	           $CB SUMERR - summation of error
	.db	0x00,0x00

	.db	0x00,0x00	; ---.-            $CE LERR - last error
	.db	0x00,0x00	;     0b00000000   $D1 MINDX

;pgm_dis:	.db	"group_ u/m PRGM  item_=   +NNN.N"

groups:
 ctl_msg:	.db	"CONTRL"
 in_msg:	.db	"INPUT "
 out_msg:	.db	"OUTPUT"
 lim_msg:	.db	"LIMITS"
 alm_msg:	.db	"ALARMS"
 mod_msg:	.db	"MODES "	;add this as a group??

items:
 pg_msg:	.db	"PGAIN "	;mindx=00
 ig_msg:	.db	"IGAIN "	;mindx=01
 dg_msg:	.db	"DGAIN "	;mindx=02
 tc_msg:	.db	"TC    "	;mindx=03
 dz_msg:	.db	"DZ    "	;mindx=04
 cl_msg:	.db	"CLAMP "	;mindx=05
 imx_msg:	.db	"INMAX "	;mindx=06
 imn_msg:	.db	"INMIN "	;mindx=07
 a_msg:		.db	"Ax^3  "	;mindx=08
 b_msg:		.db	"Bx^2  "	;mindx=09
 c_msg:		.db	"Cx    "	;mindx=10
 d_msg:		.db	"D     "	;mindx=11
 e_msg:		.db 	"Ex^.5 "	;mindx=12
 fil_msg:	.db	"FILTR "	;mindx=13
 mg_msg:	.db	"MGAIN "	;mindx=14
 ag_msg:	.db	"AGAIN "	;mindx=15
 sul_msg:	.db	"SPUL  "	;mindx=16
 sll_msg:	.db	"SPLL  "	;mindx=17
 iul_msg:	.db	"INUL  "	;mindx=18
 ill_msg:	.db	"INLL  "	;mindx=19
 oul_msg:	.db	"OUTUL "	;mindx=20
 oll_msg:	.db	"OUTLL "	;mindx=21
 bul_msg:	.db	"BADUL "	;mindx=22
 bll_msg:	.db	"BADLL "	;mindx=23
 pc_msg:	.db	"PC    "	;mindx=24    PC is an invalid label, use PCLAMP
 
 uni_msg:	.db	"UNITS "	;mindx=25 	irregular parameters
 ntc_msg:	.db	"NUMTC "	;mindx=26
 spm_msg:	.db	"SPMOD "	;mindx=27
 oty_msg:	.db	"OUTYP "	;minxx=28
 ial_msg:	.db	"INALG "	;mindx=29
 ity_msg:	.db	"INTYP "	;mindx=30

specialflagparameters:
 spm0_msg:	.db	"Hold  "	;mindx=27 STATUS bit6=0
 spm1_msg:	.db	"Track "	;mindx=27 STATUS bit6=1
 oty0_msg:	.db	"0-20mA"	;mindx=28 STATUS bit4=0
 oty1_msg:	.db	"4-20mA"	;mindx=28 STATUS bit4=1
 ial0_msg:	.db	"Linear"	;mindx=29 STATUS bit3=0
 ial1_msg:	.db	"nOrder"	;mindx=29 STATUS bit3=1
 ity0_msg:	.db	"0-5V  "	;mindx=30 STATUS bit210=000
 ity1_msg:	.db	"1-5V  "	;mindx=30 STATUS bit210=001
 ity2_msg:	.db	"Timer "	;mindx=30 STATUS bit210=010
 ity3_msg:	.db	"Freq  "	;mindx=30 STATUS bit210=011

;******* EEPROM FILE *******************************************************
.ESEG
ep_pgain:	.db	"PGAIN"				;0x00
		.dw	0x199A		;+1.000
		.db	0x01
ep_igain:	.db	"IGAIN"				;0x08
		.dw	0x199A		;+1.000
		.db	0x01
ep_dgain:	.db	"DGAIN"				;0x10
		.dw	0x199A		;+1.000
		.db	0x01
ep_tc:		.db	"TC   "				;0x18
		.dw	0x199A		;+1.000
		.db	0x01
ep_dz:		.db	"DZ   "				;0x20
		.dw	0x0000		; .0000
		.db	0xEE
ep_clamp:	.db	"CLAMP"				;0x28
		.dw	0xFFC3		;+999.0
		.db	0x06
ep_inmax:	.db	"INMAX"				;0x30
		.dw	0x199A		;+100.0
		.db	0x06
ep_inmin:	.db	"INMIN"				;0x38
		.dw	0x0000		; .0000
		.db	0xEE
ep_a:		.db	"Ax^3 "				;0x40
		.dw	0x0000		; .0000
		.db	0xEE		
ep_b:		.db	"Bx^2 "				;0x48
		.dw	0x0000		; .0000
		.db	0xEE
ep_c:		.db	"Cx   "				;0x50
		.dw	0x4001		;+25.00
		.db	0x04
ep_d:		.db	"D    "				;0x58
		.dw	0x4001		;-25.00
		.db	0x05
ep_e:		.db 	"Ex^.5"				;0x60
		.dw	0x0000		; .0000
		.db	0xEE
ep_fil:		.db	"FILTR"				;0x68
		.dw	0x5555		; .3333
		.db	0x00
ep_mgain:	.db	"MGAIN"				;0x70
		.dw	0x0290		; .0100
		.db	0x00
ep_again:	.db	"AGAIN"				;0x78
		.dw	0x4001		;+.2500
		.db	0x00
ep_spul:	.db	"SPUL "				;0x80
 		.dw	0xFFC3		;+999.0
		.db	0x06	
ep_spll:	.db	"SPLL "				;0x88
		.dw	0xFFC3		;-999.0
		.db	0x07
ep_inul:	.db	"INUL "				;0x90
		.dw	0xFFC3		;+999.0
		.db	0x06
ep_inll:	.db	"INLL "				;0x98
		.dw	0xFFC3		;-999.0
		.db	0x07
ep_outul:	.db	"OUTUL"				;0xA0
		.dw	0xFFFF		; .9999
		.db	0x00
ep_outll:	.db	"OUTLL"				;0xA8
		.dw	0x0000		; .0000
		.db	0x00
ep_badul:	.db	"BADUL"				;0xB0
		.dw	0xFFC3		;+999.0
		.db	0x06
ep_badll:	.db	"BADLL"				;0xB8
		.dw	0xFFC3		;-999.0
		.db	0x07
ep_pc:		.db	"PC   "				;0xC0
		.dw	0xFFC3		;+999.0
		.db	0x06
				;special parameters (not in .ddddEn format)
ep_units:	.db	"UNITS"				;0xC8 - special
		.db	"u/m"
ep_numtc:	.db	"NUMTC"				;0xD0 - special
		.db	0x05
		.db	0x11	;options byte default=0b00010001
				;bit7	  SPMODE   0= Hold     1= Track
				;bit4	  OUTYP    0= 0-20ma   1= 4-20ma
				;bit3     INALG    0= Linear   1= non-Linear
				;bit2-0   INTYP    000= 0-5V   001= 1-5V
				;                  010= Timer  011= Freq
				;                  100= Resis  101=
				;                  110=        111=
		.db	0x00

ep_spmod:	.db	"SPMOD"				;0xD8 - label only
		.dw	-1
		.db	-1
ep_outyp:	.db	"OUTYP"				;0xE0 - label only
		.dw	-1
		.db	-1
ep_intyp:	.db	"INALG"				;0xE8 - label only
		.dw	-1
		.db	-1
ep_inalg:	.db	"INTYP"				;0xF0 - label only
		.dw	-1
		.db	-1
		


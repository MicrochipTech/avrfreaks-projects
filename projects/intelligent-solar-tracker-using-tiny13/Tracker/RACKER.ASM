;		Tiny13 Led Solar Tracker @ 9.6 Mhz

;-----         
.include		"tn13def.inc"


;------------------------------------------------------------------------------------------------
.def 	RemLo 		= r6			; 16 Bit Division Remainder	
.def 	RemHi 		= r7

.def 	LiteLo 		= r8			; 16 Bit Light Level
.def 	LiteHi 		= r9	
	
.def 	StorLo 		= r10			; 16 Bit Division Temporary Store Register
.def 	StorHi 		= r11

.def	ValLo		= r12			; 16 Bit A2D Average
.def	ValHi		= r13	

.def	BytOut		= r14			; 8 Bit Serial Out Register
.def	LupCnt		= r15			; 256 A2D Sample Counter	

.def	Temp		= r16			; Work Register A
.def	Work		= r17			; Work Register B	

.def	BitCnt		= r18			; Serial Output Bit Counter	
.def	SrtCnt		= r18			; Sort Routine Loop Count

.def	BitDly		= r19			; 104 uS Delay Constant
.def 	DivCnt 		= r19			; 16 Bit Division Routine Bit Count
	
.def 	ConCnt 		= r20			; A2D Loop Count
.def 	DisPin 		= r21			; Discharge Pin

.def 	LuxLo 		= r20			; 2 Highest Light 8 Bit Values
.def 	LuxHi 		= r21	

.def 	DivrLo 		= r20			; 16 Bit Divider
.def 	DivrHi 		= r21

.def 	WordLo 		= r20			; 16 Bit Serial Out Word
.def 	WordHi 		= r21	

.def 	DirLo 		= r20			; 2 Highest Direction 8 Bit Values
.def 	DirHi 		= r21

.def 	DivdLo		= r22			; 16 Bit Dividend
.def 	DivdHi 		= r23

.def 	HoldB		= r22			; Sort Temporary Hold Registers
.def 	HoldA 		= r23

.def 	ResLo 		= r22			; 16 Bit Result
.def 	ResHi 		= r23	

.def 	MulLo		= r22			; 16 Bit Multiplicand
.def 	MulHi 		= r23

.def 	DirInd		= r22			; Compass Direction Select Register	
.def 	AdcSel 		= r23			; A2D Multiplexor Select Register	


;------------------------------------------------------------------------------------------------        
.Cseg
         
.Org	$0000


;------------------------------------------------------------------------------------------------      
Reset:			    						
      	ldi	Temp,	RamEnd		
     	out	Spl,	Temp			; Set Stack Pointer

	sbi	AcsR,	Acd			; Disable Comparator 

	Ldi	Temp,	$3c			; Disable all AdcD Digital Inputs  
	out	DidR0,	Temp		     
      		
	ldi	Temp,	$87			; Init A2D + 75 Khz Sample Rate
	out	AdCsrA,	Temp

	ldi	YH,	$00			; Set YH Constant to Zero

	clr	LupCnt				; 256 Sample Counter


;------------------------------------------------------------------------------------------------        
Main:	
	ldi	YL,	LuxBuf+$00		; Point YL to Light Value Start
		
	ldi	AdcSel,	$60			; Adc0 + ( AdLar = 1 ) + 1.1V Ref

	ldi	DirInd,	$00			; Direction Identifier

	ldi	DisPin,	$01 			; Mark on PB0 + Discharge Led 

	clr	LiteHi				; Clear 16-Bit  Light Level		 
	clr	LiteLo

	ldi	ConCnt,	4			; Conversion Loop Count = 4 Leds

	rcall	Convert				; Do A2D Conversion
	dec	ConCnt
	brne	pc-2				; 4 * 256 samples finished ?
	
	rcall	Sort				; Bubble Sort Results in Ascending order 

	rcall	Intensity			; Get the 2 Brightest Light Values

	rcall	Mul90				; Multiply Result by Ninety

	rcall	Divide				; 16 Bit Division
	
	rcall	Compass				; Get Compass Quadrant in Degrees

	rcall	Send				; Transmit Data Stream
	
	rjmp	Main				; Go back


;------------------------------------------------------------------------------------------------ 
Convert:
	
;* This routine gets 256* A2D, 8-Bit samples on each Led. These samples are then averaged and 
;* integrated in "ValHi:ValLo" and the final result is divided by 256 and is stored in "Valhi"

;* This value is then placed in the Ram locations 
;* starting @ "LuxBuf+$00" using Y register Post Indexing. 

;* The Compass Point Direction "DirInd" 8-Bit values (N,E,S,W) are similarly stored 
;* in the Ram locations starting @ "LuxBuf+$00" using Y register offset by + $04 

;* The total light incidence falling on the 4 Leds is likewise averaged over 256 samples and is 
;* stored in Registers "LiteHi:LiteLo"

;* "AdcSel" is incremented after each pass and is used to select the appropriate A2D Mux Input
;* (00,01,02,03)

;* "DisPin" is  also incremented after each pass and is used to "discharge" the appropriate 
;* Led by bringing the corresponding Mux Input Low


	out	AdMux,	AdcSel			; AdcN + (AdLar=1) + Int Voltage Ref
				
	clr	ValLo				; Clr Result Registers
	clr	ValHi

	clr	LiteLo				; Clr Accumulated Light Registers
	clr	LiteHi

ConLup:	
	sbi	AdCsrA,	Adsc 			; En A/D Conv	
         
      	sbis	AdCsrA,	Adif			; Conv Comp ? 	
	rjmp	pc-1

	sbi	AdCsrA,	Adif			; Clr Conversion Flag

      	in	Temp,	AdcH			; Read Hi order 8 Bits

	adc	ValLo,	Temp			; Integrate
	adc	ValHi,	YH

	dec	LupCnt				; 256 samples done ?
	brne	ConLup

	st	Y+,	ValHi			; Store Value in LuxBuf+$nn + Increment Y

	std	Y+4,	DirInd			; Put Direction in LuxBuf+$nn offset by 4

	add	LiteLo,	ValHi			; Update accumulated Light Level
	adc	LiteHi,	YH			; Adc Null Constant

	out	DdrB,	DisPin			; Discharge Direction Led	

	ldi	Temp,	$01			; Disharge Led Voltage
	out	PortB,	Temp	
	
	out	DdrB,	Temp			; ReEnable Mark on PB0
	out	PortB,	Temp	

	inc	AdcSel				; Select Next Led

	inc	DisPin				; Select Next Discharge Pin

	inc	DirInd				; Select Next Direction Indicator

	ret


;------------------------------------------------------------------------------------------------
Sort:					

;* This routine Bubble Sorts the Four 8-Bit Light Values starting @ Ram Location "LuxBuf+0".
;* The corresponding Direction Indicators starting @ Ram Location "LuxBuf+4" are also sorted. 

	ldi	SrtCnt,	$04			; Sort Loop Counter = 4

OutLup:	
	ldi	YL,	LuxBuf+3		; Point to End of Light values
		
 	ld 	HoldA,	Y 			; Get First Value in HoldA
					
	ld 	HoldB,	-Y 			; Get 2nd Value in HoldB
					
	cp 	HoldA,	HoldB 			; Compare both values and ...
	brlo 	ChkLup 					

	st 	Y,	HoldA			; Swap A and B if A>B		
	std 	Y+1,	HoldB

	ldd	HoldA,	Y+5			; Do same with Direction Indicators
	ldd	HoldB ,	Y+4

	std	Y+4,	HoldA			; Swap Indicators			
	std	Y+5,	HoldB 

ChkLup: 	
	cpi	YL,	LuxBuf+0		; Loop Inner if not finished
	brne 	OutLup+1 

	dec 	SrtCnt				; Loop to back OutLup if NOT finished
	brne 	OutLup

	ret


;------------------------------------------------------------------------------------------------
Intensity:

;* This routine fetches the 2 Brightest Light Intensities and prepares 
;* the result in Degrees according to the Formula 
;*            
;* ((Highest Lux Value / (Highest Lux Value + 2nd Highest Value)) * 90) + Compass point


	ld	LuxHi,	Y+			; Highest + 2nd Highest Light Values
	ld	LuxLo,	Y

	mov	MulLo,	LuxHi			; Copy LuxHi into MulLo

	add	LuxLo,	LuxHi			; Add Highest to 2nd Highest Light Value
	clr	LuxHi
	adc	LuxHi,	YH			; Adc Zero Constant

	ret

		
;------------------------------------------------------------------------------------------------
Mul90:

;* This routine multiplies the 8-bit number "MulLo" by 90
;* using just Shifts and Adds. The 16-bit result is placed in "MulHi:MulLo".
 

	clr	StorLo				; Temp Holding Store
	clr	StorHi

	clr	MulHi				; Clear High Byte Result

	lsl	MulLo				; Multiplicand * 2
	rol	MulHi
	add	StorLo,	MulLo			; Add to Holding 
	adc	StorHi,	MulHi

	lsl	MulLo				; Multiplicand * 8
	rol	MulHi
	lsl	MulLo
	rol	MulHi
	add	StorLo,	MulLo			; Add to Holding 
	adc	StorHi,	MulHi

	lsl	MulLo				; Multiplicand * 16
	rol	MulHi
	add	StorLo,	MulLo			; Add to Holding 
	adc	StorHi,	MulHi

	lsl	MulLo				; Multiplicand * 64
	rol	MulHi
	lsl	MulLo
	rol	MulHi
	add	MulLo,	StorLo			; Result = Holding + Multiplicand
	adc	MulHi,	StorHi

	ret

	
;------------------------------------------------------------------------------------------------
Divide:			

;* This routine divides the two 16-bit numbers 
;* "DivdHi:DivdLo" (Dividend) and "DivrHi:DivrLo" (Divisor). 
;* The result is placed in "ResHi:ResLo" and the Remainder in "RemHi:RemLo".
	
	clr	RemHi				; Clear Remainder Registers
	clr	RemLo				
	
	ldi	DivCnt,16			; Init Loop Counter
			
	lsl	DivdLo				; Shift Left Dividend
	rol	DivdHi
	rol	RemLo				; Rotate Dividend into Remainder
	rol	RemHi
		
	cp	RemLo,	DivrLo			; Remainder ? Divisor
	cpc	RemHi,	DivrHi
	brcs	pc+4				; If Result Negative then Skip
		
	inc	DivdLo				; Increment Dividend Low 
	sub	RemLo,	DivrLo		
	sbc	RemHi,	DivrHi			; Remainder = Remainder - Divisor
		
	dec	DivCnt				; If not done then Iterate
	brne	pc-11

	ret


;------------------------------------------------------------------------------------------------
Compass:

;* This routine gets the Compass Quadrant as a 16-Bit Degree Value
;* This Value is then added to the accumulated Sum in ResHi:ResLo

	lds	Work,	LuxBuf+$04		; Get Brightest Direction

	lds	Temp,	LuxBuf+$05		; Get Second Brightest Direction

	cpi	Work,	$00			; = North ?
	breq	North 

	cpi	Work,	$01			; = East ?
	breq	East

	cpi	Work,	$02			; = South ?
	breq	South

West:	
	cpi	Temp,	$02			; = South ?
	breq	SWest

NWest:	
	ldi	Temp,	$0e			; = 270 Degrees
	ldi	Work,	$01
	rjmp	Exit

SWest:	
	ldi	Temp,	$b4			; = 180 Degrees
	rjmp	Bye

SEast:	
	ldi	Temp,	$5a			; = 90 Degrees
	rjmp	Bye	

NEast:	
	clr	Temp				; = 0 Degrees

Bye:	
	clr	Work				; = 0 Always
	
Exit:	
	add	ResLo,	Temp			; Add Compass Point to accumulated Sum
	adc	ResHi,	Work
	ret					; Return

North:	
	cpi	Temp,	$03			; = West ?
	breq	NWest
	rjmp	NEast

East:	
	cpi	Temp,	$02			; = South ?
	breq	SEast
	rjmp	NEast

South:	
	cpi	Temp,	$01			; = East ?
	breq	SEast
	rjmp	SWest


;------------------------------------------------------------------------------------------------
Send:						

;* This Software Uart routine running @ 9600 Bps sends 3* 16-Bit values to the Host. 
;* The data stream is transmitted Byte wise using (1 Start,8 Data,1 Stop Bits)

;* The Info Packet consists of the Sun's orientation in Degrees "ResHi:ResLo"
;* The Modulo Remainder of the division routine in "RemHi:RemLo"
;* The total incident sunlight in "LiteHi:LiteLo"

;* If so desired the Delay Constant value "BitDly" can be adjusted to allow other
;* data transmission speeds.


	movw	WordHi:WordLo,ResHi:ResLo	; 16 Bit Degree Result		
	rcall	PrWord				

	movw	WordHi:WordLo,RemHi:RemLo	; 16 Bit Remainder 
	rcall	PrWord

	movw	WordHi:WordLo,LiteHi:LiteLo	; 16 Bit Light Value

;------
PrWord:						
	mov	BytOut,	WordHi
	rcall	PrByt				; Send High 8 Byte

	mov	BytOut,	WordLo			; Send Low  8 Byte

;------
PrByt:						
	mov	Temp,	BytOut			; Fetch BytOut

	swap	Temp
	rcall	NibAsc				; Nibble to Ascii

	mov	Temp,	BytOut			; Fetch BytOut once more

;------
NibAsc:						
	andi	Temp,	$0f			; Mask High Nibble

	cpi	Temp,	$0a			; > 10 ?
	brcs	pc+2

	subi	Temp,	-$07			; If No then Add 48+7 to Temp

	subi	Temp,	-$30			; If No then Add only 48 to Temp
	
;------
PrChr:
	ldi	BitCnt,	9
	
	clc					; Start Bit

	rcall	BiTime				; Out Bit

	lsr	Temp				; Shift next bit out
	dec	Bitcnt				
	brne	pc-3				; Loop if Not Zero

	sec					; Stop Bit

;-----
BiTime:	
	brcs	pc+3				; Goto Mark

	cbi	PortB,	 PortB0			; Space Bit on PB0
	rjmp	Delay

	sbi	PortB,	 PortB0			; Mark Bit on PB0
	nop

;-----
Delay:			
	ldi  	BitDly,	$f8			; Delay 104 uS

	nop
  	dec	BitDly				; Decrement Count and Loop
        brne 	pc-2	
	ret


;------------------------------------------------------------------------------------------------
.dseg

.org $0060

LuxBuf:		.byte	8			; 8 Bytes for Light Values

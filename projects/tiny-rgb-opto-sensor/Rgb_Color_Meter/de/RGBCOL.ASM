;Tiny13 Opto Sensor @ 9.6 Mhz

.include "tn13def.inc"

;----- 
.def     HoldLo= r6            ; 24-Bit Holding Register
.def     HoldMd= r7
.def     HoldHi= r8

.def     FilLo = r9
.def     FilMd = r10           ; 20-Bit Filter Register
.def     FilHi = r11

.def     TmpLo = r12           ; 20-Bit Temporary Register
.def     TmpMd = r13
.def     TmpHi = r14

.def     Spare = r15           ; Spare Register

.def     Temp  = r16           ; General Purpose Work Registers
.def     TempA = r17
.def     TempB = r18

.def     RedLo=  r20           ; 16-Bit Holding Register
.def     RedHi=  r21

.def     GrnLo=  r22           ; 16-Bit Holding Register
.def     GrnHi=  r23

.def     Spare = r24           ; Spare Register

.def     ValLo = r25            ; 10-Bit A2D Value
.def     ValMd = r26
.def     ValHi = r27

.def     SampLo= r28           ; Conversion Sample Counter
.def     SampHi= r29

;------ 
         .Cseg

.Org     $0000

;----- 
Reset:  
        sbi AcsR, Acd         			; Disable Comparator 

        ldi Temp, $3c         			; Disable All A2Ds as Digital Inputs
        out DidR0,Temp 

        ldi Temp, $87        			; Init + 75Khz A2D Sample Rate
        out AdCsrA,Temp 

;----- 
Main:  
	ldi Temp, $1f				; Enable All Pins as OutPuts
        out DdrB, Temp 

	sbi PortB, PortB0     			; Discharge Led Capacitance
	
	sbiw SampLo, 1				; Discharge Delay
	brne pc-1

	clr HoldHi				; Clear Holding Register
	clr HoldMd
	clr HoldLo

	ldi Temp, $03				; Enable Pb0,Pb1 as OutPuts
        out DdrB, Temp 

	sbi PortB, PortB0     			; Set Mark Bit High on Pb0 + Enable Leds
	
        ldi Temp, $41				; Get Red Value
	rcall ConVrt 

	movw RedHi:RedLo,TmpMd:TmpLo		; Store in Red Register

	ldi Temp, $42				; Get Green Value
	rcall ConVrt

	movw GrnHi:GrnLo,TmpMd:TmpLo		; Store in Green Register
	
	ldi Temp, $43				; Get Blue Value
	rcall ConVrt

	rcall Process				; Calculate Percentages
       
	ldi Temp, $0d				; Send Cret
	rcall PrChar

        rjmp Main              			; Do Again

;------
ConVrt:  
	out AdMux, Temp	     			; Get Digital Value on Adc(n)

        ldi SampHi, $02      			; Set Sample count to 561 
        ldi SampLo, $31

        clr TmpHi            			; Clear TmpHi Register 

        clr FilHi            			; Clear Filter Registers
	clr FilMd
	clr FilLo

;------ 
A2D:     
        sbi AdCsrA, Adsc     			; En A/D Conv 

        sbis AdCsrA, Adif    			; Conv Comp ? 
        rjmp pc-1

        sbi AdCsrA, Adif     			; Clr Conversion Flag

        in ValLo, AdcL       			; Read Lo order 8 Bits

        in ValHi, AdcH       			; Read Hi order 2 Bits

	movw TmpMd:TmpLo,FilHi:FilMd  		; Filter shifted 8*Right

        sub FilLo, TmpLo     			; Filter = Filter - Shift
        sbc FilMd, TmpMd
        sbc FilHi, TmpHi

        add FilLo, ValLo     			; Filter = Filter + Input
        adc FilMd, ValHi
        adc FilHi, TmpHi

        sbiw SampLo, 1       			; Dec Sample Loop Count
        brne A2D

	movw TmpMd:TmpLo,FilHi:FilMd  		; Filter shifted 8*Right

	add HoldLo,TmpLo			; Accumulate 3*Results		
	adc HoldMd,TmpMd

	ret

;------
Process:					; Result = (Value*100/Result) %

	movw TmpMd:TmpLo,RedHi:RedLo		; Process Red Value				
	rcall PerCent

	mov SampHi, ValLo			; Send Red Value
	rcall	PrBcd

	movw TmpMd:TmpLo,GrnHi:GrnLo		; Process Green Value
	rcall PerCent				

	add SampHi, ValLo			; Send Green Value
	rcall	PrBcd

	ldi Temp, 100				; Process Blue Value
	sub Temp, SampHi

	mov ValLo, Temp				; Send Blue Value
	rjmp	PrBcd

;------
PerCent:					; Multiply by 100					
	lsl TmpLo				
	rol TmpMd				; *4
	rol TmpHi	
	lsl TmpLo
	rol TmpMd
	rol TmpHi	

	mov ValLo, TmpLo			
	mov ValMd, TmpMd			; Save to Store
	mov ValHi, TmpHi

	lsl TmpLo				; *32
	rol TmpMd
	rol TmpHi		
	lsl TmpLo
	rol TmpMd
	rol TmpHi	
	lsl TmpLo
	rol TmpMd
	rol TmpHi	

	add ValLo, TmpLo			; Add to Store
	adc ValMd, TmpMd
	adc ValHi, TmpHi

	lsl TmpLo
	rol TmpMd
	rol TmpHi				; *64

	add ValLo, TmpLo			; Add to Store
	adc ValMd, TmpMd
	adc ValHi, TmpHi

;------
Divide:

;* This routine divides the 24-bit number "ValHi:ValMd:ValLo" (dividend)
;* By the 24-bit number "HoldHi:HoldMd:HoldLo" (divisor). 
;* The result is placed in "ValLo"
;* The remainder in"TempB:TempA:Temp".

	clr	TempB				; Clear Remainder
	clr 	TempA
	clr	Temp 				
 
	ldi 	SampLo,24			; Init loop counter

	lsl 	ValLo 				; Shift left dividend
	rol	ValMd
	rol 	ValHI

	rol 	Temp 				; Shift left Remainder
	rol 	TempA
	rol 	TempB
 
	cp  	Temp, HoldLo 			; Compare Remainder with Divisor
	cpc	TempA,HoldMd
	cpc 	TempB,HoldHi 
	brcs 	PC+5
 
	inc 	ValLo
 
	sub 	Temp ,HoldLo			; Remainder = Remainder - Divisor
	sbc	TempA,HoldMd
	sbc 	TempB,HoldHi

	dec 	SampLo				; Decrement loop counter
 	brne 	PC-15

	ret

;------
PrBcd:	
	clr 	ValHi				; Set 10's register to Zero

	inc	ValHi
	subi	ValLo,	10			; Subtract 10 from if Carry Clear
	brcc	pc-2

	dec	ValHi				; Adjust by adding 10
	subi	ValLo,	246
					
	mov	Temp,  ValHi			; Send	10's
	ori 	Temp,  $30			
	rcall	PrChar				; Ascii it	

	mov	Temp,  ValLo			; Send	Units
	ori 	Temp,  $30			
	rcall	PrChar				; Ascii it	

	ldi	Temp,	$25			; Send "Percent" Character
	rcall	PrChar

	ldi	Temp,	$20			; Send "Space" Character

;------
PrChar:  
        ldi TempA, 9         			; Bit Counter

        clc                  			; Start Bit Out

        rcall SetBit         			; Out Bit

        lsr Temp             			; Shift next bit out

        dec TempA            			; Dec BitCnt + Loop If Not Zero
        brne pc-3

        sec                  			; Stop Bit Out

;-----
SetBit:  
         brcs pc+3 

         cbi PortB, PortB0    ; Space Bit out on Pb0 if Carry Set
         rjmp Delay

         sbi PortB, PortB0    ; Mark Bit out on Pb0 if Carry Clear
         nop

;-----
Delay:   
         ldi TempB, 248       ; Delay 104 uS:

         nop
         dec TempB            ; Dec BiTime Count and Loop If NZ
         brne pc-2 

         ret


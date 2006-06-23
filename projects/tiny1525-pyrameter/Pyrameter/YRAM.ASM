
;Pyrameter

;------
.include "tn15def.inc"

;------
.equ	BiTime		= 50			; 104 uSec Delay Constant

.equ	BitCnt		= 9			; 9 Bit Counter

.equ	Calib		= $7e			; RC Calibration Byte

;-----
.def	OffLo		= r4			; 10-Bit Offset Value
.def	OffHi		= r5

.def	ValLo		= r6			; 10-Bit Light Intensity Value
.def	ValHi		= r7

.def	Null		= r8			; Zero Constant
.def	FilLo		= r9			
.def	FilMd		= r10			; 20-Bit Filter Register	
.def	FilHi		= r11
	
.def	TmpLo		= r12			; 20-Bit Temporary Register
.def	TmpMd		= r13			
.def	TmpHi		= r14 
.def	BytOut		= r15 			; Byte to be transmitted
    
.def	Temp		= r16			; General Purpose Work Registers
.def	TempA		= r17			
.def	TempB		= r18	

.def	TimeA		= r19			; General Purpose Timer Registers
.def	TimeB		= r20			
.def	TimeC		= r21	

.def	SampLo		= r24			; 561 Times Sample Counter
.def	SampHi		= r25		

;------         
.Cseg
         
.Org	$0000

;-----         
Reset:			    						
      	ldi	Temp,	RamEnd		
     	out	Spl,	Temp			; Set Stack Pointer

	ldi	Temp,	Calib			; Calibrate Internal Oscillator
	out	OscCal,	Temp

	sbi	DdrB,	Ddb1			; Mark High				
	sbi	PortB,	Pb1	

	sbi	AcsR,	Acd			; Disable Comparator 
			           		
	ldi	Temp,	$85			; Init + 50Khz Sample Rate
	out	AdCsr,	Temp
		
	clr	Null				; Null = 0

;-----         
Main:	
	ldi	Temp,	$c7			; AdLar=0 + IntRef + Diff*20 + Bypass Cap 
	out	AdMux,	Temp
	
	rcall	SamPle				; Get Light Values

	ldi	Temp,	$c5			; AdLar=0 + IntRef + Bypass Cap + Offset 
	out	AdMux,	Temp

	rcall	Conv				; Get Offset

	sub	FilMd,	ValLo			; Subtract Offset from Filter
	sbc	FilHi,	ValHi

Mul:	lsl	FilMd				; Multiply by 10	
	rol	FilHi

	mov	TempA,	FilMd			; Temporary Store
	mov	TempB,	FilHi

	lsl	FilMd				; Using Left Shifts and Add
	rol	FilHi	
	lsl	FilMd		
	rol	FilHi

	add	FilMd,	TempA	
	adc	FilMd,	TempB

Div:	lsr	FilHi				; Divide by 8
	ror	FilMd
	lsr	FilHi				; Using 3 Right Shifts
	ror	FilMd
	lsr	FilHi				; Same as multiplying by 1.25
	ror	FilMd

	mov	BytOut,	FilHi			; Send Hi Order 2 Bits
	rcall	PrByte

	mov	BytOut,	FilMd			; Send Lo Order 8 Bits
	rcall	PrByte				

	ldi	Temp,	' '			; Send Space
	rcall	PrChar

	rjmp	Main				; Go back

;------
SamPle:
	ldi	SampHi,	$02			; Set Sample count to 561			
	ldi	SampLo,	$31

Sp:	rcall	Conv

	mov	TmpMd,	FilHi			; Filter Register shifted 8*Right
	mov	TmpLo,	FilMd

	sub	FilLo,	TmpLo			; Filter Register = Filter Register - Shift
	sbc	FilMd,	TmpMd
	sbc	FilHi,	Null

	add	FilLo,	ValLo			; Filter Register = Filter Register + Input
	adc	FilMd,	ValHi
	adc	FilHi,	Null

	dec	SampLo				; Decrement Sample Loop Count
	brne	Sp
	dec	SampHi
	brne	Sp

	ret
	

;-----
Conv:		
	sbi	AdCsr,	Adsc 			; En A/D Conv	
         
      	sbis	AdCsr,	Adif			; Conversion Complete ? 	
	rjmp	pc-1

	in	ValLo,	AdcL			; Read Lo order 8 Bits

      	in	ValHi,	AdcH			; Read Hi order 2 Bits

	sbi	AdCsr,	Adif			; Clr Conversion Flag

	ret

;------
PrByte:						
	mov	Temp,	BytOut			; Fetch	BytOut

	swap	Temp				; Reverse Nibbles
	rcall	NibAsc

	mov	Temp,	BytOut			; Fetch	BytOut once more

;------
NibAsc:						
	andi	Temp,	$0f			; Mask Off Least significant Nibble

	cpi	Temp,	$0a			; > 10 ?
	brcs	pc+2

	subi	Temp,	-$07			; Adi 7

	subi	Temp,	-$30			; Adi 48
	
;------
PrChar:
	ldi	TempA,	BitCnt			; Bit Counter
	
	clc					; Start Bit Out

	rcall	SetBit				; Out Bit

	lsr	Temp				; Shift next bit out

	dec	TempA				; Decrement BitCnt Counter and Loop If Not Zero
	brne	pc-3

	sec					; Stop Bit Out

;-----
SetBit:	
	brcs	pc+3				

	cbi	PortB,	 Pb1			; Space Bit if Carry Clear
	rjmp	Delay

	sbi	PortB,	 Pb1			; Mark Bit if Carry Set
	nop

;-----
Delay:			
	ldi  	TempB,	BiTime			; Delay 104 uS:

  	dec	TempB				; Decrement BiTime Counter and Loop If Not Zero
        brne 	pc-1	

	ret



;Tiny13 Http Server @ 9.6 Mhz
         
.include		"tn13def.inc"

;-----	
.def	ValLo		= r7			; 10-Bit A2D Value		
.def	ValHi		= r8

.def	FilLo		= r9		
.def	FilMd		= r10			; 20-Bit Filter Register	
.def	FilHi		= r11
	
.def	TmpLo		= r12			; 20-Bit Temporary Register
.def	TmpMd		= r13			
.def	TmpHi		= r14

.def	Null		= r15			; Zero Constant
    
.def	Temp		= r16			; General Purpose Work Registers
.def	TempA		= r17			
.def	TempB		= r18	

.def	LupCnt		= r19			; Conversion Counter	

.def	Space		= r20			; Ascii "Space"  Value
		
.def	Period		= r21			; Ascii "Period" Value

.def	SampLo		= r24			; Conversion Sample Counter
.def	SampHi		= r25	

.def	BinLo		= r26			; Ascii Holding Registers
.def	BinHi		= r27		

;------         
.Cseg
         
.Org	$0000

	rjmp	Reset

;-----         
CharIn:					; Int0 interrupt Routine
	reti

;-----         
Reset:		    						
	sbi	DdrB,	DdB0			; Set Mark Bit High on Pb0	
	sbi	PortB,	PortB0

	sbi	AcsR,	Acd			; Disable Comparator 

	ldi	Temp,	$3c			; Disable All A2Ds as Digital Inputs
	out	DidR0,	Temp			
			           		
	ldi	Temp,	$87			; Init + 75Khz A2D Sample Rate
	out	AdCsrA,Temp	

	ldi	Period,$2e			; "Period"
	
	ldi	Space,	$20			; "Space"
		
;-----         
Main: 
	ldi	ZL,	TrxBuf+0		; Set Y Register to TrxBuf Start
	
	ldi	LupCnt,$00			; Adc0 + (AdLar=0) + VCC Analog Ref 

	out	AdMux,	LupCnt			; Get Digital Value on Adc(n)
	rcall	ConVrt				
	inc	LupCnt				; Adc1,2,3

	cpi	LupCnt,$04			; 4*Adc(n)?	
	brne	pc-4

	ldi	Temp,	$40			; Enable ExtInt
	out	GimSk,	Temp	
	
	ldi	Temp,	$30			; En Sleep + Lo Level Interrupt
	out	McuCr,	Temp

	sei					; Enable Global Interrupts

	sleep					; PowerDown

	out	McuCr,	Null			; Wake Up

	cli					; Disable Global Interrupts

	ldi	ZL,	TrxBuf+0		; ReSet Y Register to TrxBuf Start

	ld	Temp,	Z+			; Send next Data Char to PC
	rcall	PrChar				

	cpi	ZL,	TrxBuf+28		; 4*7 Data Characters Output ?
	brne	pc-3

	rjmp	Main				; Do Again
	
;------
ConVrt:	
	ldi	SampHi,	$02		; Set Sample count to 561			 
	ldi	SampLo,	$31

	clr	TmpHi				; Clear TmpHi Register	

	clr	TempB				; Clear TempB Register

;------	
A2D:
	sbi	AdCsrA,	Adsc 		; En A/D Conv	
         
      	sbis	AdCsrA,	Adif		; Conv Comp ? 	
	rjmp	pc-1

	sbi	AdCsrA,	Adif		; Clr Conversion Flag

	in	ValLo,	AdcL			; Read Lo order 8 Bits

      	in	ValHi,	AdcH			; Read Hi order 2 Bits

	movw	TmpMd:TmpLo,FilHi:FilMd	; Filter shifted 8*Right

	sub	FilLo,	TmpLo			; Filter = Filter - Shift
	sbc	FilMd,	TmpMd
	sbc	FilHi,	Null

	add	FilLo,	ValLo			; Filter = Filter + Input
	adc	FilMd,	ValHi
	adc	FilHi,	Null

	sbiw	SampLo,	1		; Dec Sample Loop Count
	brne	A2D

	movw	TmpMd:TmpLo,FilHi:FilMd	; Filter shifted 8*Right

;------
Process:	    				; Result = Result * (1250/256) 
	lsl	TmpLo
	rol	TmpMd				; = Result * 4.8828125			
	
	movw	TempA:Temp,TmpMd:TmpLo	; Store Result * 2 to Temp Reg	

	lsl	TmpLo
	rol	TmpMd				; Result * 32
	lsl	TmpLo
	rol	TmpMd				
	lsl	TmpLo
	rol	TmpMd				
	lsl	TmpLo
	rol	TmpMd				

	add	Temp,	TmpLo			; Add Result to Temp Reg			
	adc	TempA,	TmpMd	

	lsl	TmpLo
	rol	TmpMd				; Result * 64
	rol	TmpHi

	add	Temp,	TmpLo			; Add Result to Temp Reg			
	adc	TempA,	TmpMd	
	adc	TempB,	TmpHi	

	lsl	TmpLo
	rol	TmpMd				; Result * 128
	rol	TmpHi

	add	Temp,	TmpLo			; Add Result to Temp Reg			
	adc	TempA,	TmpMd	
	adc	TempB,	TmpHi	

	lsl	TmpLo
	rol	TmpMd				; Result * 1024
	rol	TmpHi
	lsl	TmpLo
	rol	TmpMd				
	rol	TmpHi
	lsl	TmpLo
	rol	TmpMd				
	rol	TmpHi

	add	TmpLo,	Temp			; Add Result back to TmpHi Regs			
	adc	TmpMd,	TempA	
	adc	TmpHi,	TempB	

;-----
BinAsc:	
        ldi 	BinHi,	High(1000) 		; First with Thousands
        ldi 	BinLo,	Low (1000)
        rcall 	BinDig	 		; Calculate Digit

	cpi	Temp,	$30			; Leading Zero?
	breq	pc+3

	st	Z+,	Temp			; No, Store Thousands Digit in Buffer
	rjmp	pc+2

	st	Z+,	Space			; Yes, Store "Space" Char in TxtBuf

	st	Z+,	Period			; Store "Period" Char in TxtBuf

        ldi 	BinHi,	High(100) 		; Next with Hundreds
        ldi 	BinLo,	Low (100)
        rcall 	BinDig 		; Calculate Digit

	st	Z+,	Temp			; Store Hundreds Digit in TxtBuf

        ldi 	BinHi,	High(10) 		; Next with Tens
        ldi 	BinLo,	Low (10)
        rcall 	BinDig 		; Calculate Digit

	st	Z+,	Temp			; Store Tens Digit in TxtBuf

	mov	Temp,	TmpMd			; Next with Ones

	ori	Temp,	$30			; Convert Ones Digit to Ascii

	st	Z+,	Temp			; Store Ones Digit in Buffer

	st	Z+,	Space			; Store 2 * "Space" Char in TxtBuf
	st	Z+,	Space

	ret

;------
BinDig:
        ldi 	Temp,	$30 			; Digit count = Ascii "0"

Ba:	cp 	TmpHi,	BinHi 			; Number Greater than decimal?

        brcs 	Bc	 			; MSB Less than decimal

        brne 	Bb	 			; MSB Greater than decimal

        cp 	TmpMd,	BinLo 			; LSB Greater or equal decimal

        brcs 	Bc	 			; LSB Less than decimal

Bb:	sub 	TmpMd,	BinLo 			; Subtract LSB decimal

        sbc 	TmpHi,	BinHi 			; Subtract MSB decimal

        inc 	Temp 				; Increment digit count

        rjmp 	Ba	 			; Next loop

Bc:	ret

;------
PrChar:
	ldi	TempA,	9			; Bit Counter
	
	clc					; Start Bit Out

	rcall	SetBit				; Out Bit

	lsr	Temp				; Shift next bit out

	dec	TempA				; Dec BitCnt + Loop If Not Zero
	brne	pc-3

	sec					; Stop Bit Out

;-----
SetBit:	
	brcs	pc+3				

	cbi	PortB,	 PortB0		; Space Bit out on Pb0 if Carry Set
	rjmp	Delay

	sbi	PortB,	 PortB0		; Mark Bit out on Pb0 if Carry Clear
	nop

;-----
Delay:			
	ldi  	TempB,	248			; Delay 104 uS:

	nop
  	dec	TempB				; Dec BiTime Count and Loop If NZ
        brne 	pc-2	

	ret

;-----
.dseg

.org $0060

TrxBuf:		.byte	28		; 4*7 Character Transmit Values

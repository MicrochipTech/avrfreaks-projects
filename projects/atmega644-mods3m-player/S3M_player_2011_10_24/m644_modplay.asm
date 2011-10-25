;compiler: avrasm
;************************************************************************************************
; Embeded MOD/S3M player for AVR ATmega644.
;
; module: main player
;************************************************************************************************
; Licence: This source code is free to use for any non-comercial purposes.
;          (C) 2011, Stanislav Maslan, s.maslan@seznam.cz 
;************************************************************************************************
; version:           V1.00, 2011-10-22
; www:               http://www.elektronika.kvalitne.cz
;                    http://www.elektronika.kvalitne.cz/ATMEL/S3Mplayer/S3Mplayer.html
;                    http://www.elektronika.kvalitne.cz/ATMEL/S3Mplayer/S3Mplayer_eng.html
;           
; supported formats: MOD, preconverted S3M (see www for more info)
;                    therotecially up to 32 channels, limited with sample rate
;                    typically up to 48 instruments (can be extended) 
; clock:             typically 20MHz, my ATmega644 works up to 32MHz
; LCD:               standard 2x20 with HD44780 compatible controller
; audio output:      I2S or TDA1545 compatible stereo 16-bit stream at XTAL/2 clock
;                    sampling rate depends on channels count/quality settings/XTAL
; audio formats:     4-48kHz, 16-bit, mono/stereo, linear or no interpolation resampling
;                    typical rates for XTAL=20MHz, stereo, linear interpolation:        
;                    35kHz for 4-channels
;                    21kHz for 8-channels
;                    17kHz for 10-channels  
; storage:           IDE inerface (CF card or HDD)
;                    FAT16 in first partition, max. 255 subfolders and files in each folder 
; serial loader:     8-bit, no parity, no flow ctrl, transfer rate up to XTAL/16
;
;************************************************************************************************
; FUSE BITS: CKOPT should be set if crystall is used and JTAG should be disabled because portc 
;            should have all pins in low impedance mode
;************************************************************************************************

	.include "m644def.inc"	;ATmega644 defines
	.include "macros.asm"	;macros
	
	;basic settings
	.equ	XTAL=20000000	;XTAL frequency [Hz]
	.equ	HCOC=XTAL>24000000	;SRAM wait cycle if hardcore overclock (usualy if XTAL>24MHz) (1/0 - on/off)
	.equ	USRDIV=0	;USART baudrate divisor (baud_rate=XTAL/(16*(USRDIV+1)))
	.equ	MEMTST=0	;full range ext. SRAM test (1-yes, 0-no)
	.equ	STORA=1	;enable storage (1-yes, 0-no)
	.equ	LCDON=1	;enable LCD (1-yes, 0-no)
	.equ	LING=0	;text language (0 - english, 1 - cesky)
	
	;MOD player settings
	.equ	MAXCH=8	;maximum valid count of channels (4-32)
	.equ	MAXINS=48	;maximum valid count of instruments (default 48)
	.equ	DAC=1545	;1543 for I2S or 1545 for TDA1545 DAC
	.equ	SRATE=21000	;NEEDED sample rate [Hz]
	.equ	STEREO=1	;stereo/mono mixing (1/0 - stereo/mono)
	.equ	LINRES=1	;linear interpolation (1/0 - on/off, a 'bit' more CPU power needed)
	.equ	MAMPLD=MAXCH<=4	;channel amplitude (1/0 - full/half), full amplitude should be set only for 4 channel MODs
  	.equ	DRAWVU=1	;draw VU meters !!! +5% CPU usage (detecting peeks in resampling loop) !!! (1/0 - on/off)
		


;************************************************************************************************
;	Signal definitions
;************************************************************************************************	
	;IDE signals
	.equ	IORD=3	;IDE read strobe
	.equ	pIORD=portd
	.equ	IOWR=7	;IDE write strobe
	.equ	pIOWR=portd
	.equ	IOA0=5	;IDE A0 on porta (physicaly conencted at high address latch)
	.equ	IOA1=6	;IDE A1 on porta (physicaly conencted at high address latch)
	.equ	IOA2=7	;IDE A2 on porta (physicaly conencted at high address latch)
		
	;LCD signals
	.equ	LRS=1	;LCD register selection (1/0 - data/instructions)
	.equ	pLRS=portb
	.equ	LCK=0	;LCD clock (positive pulse)
	.equ	pLCK=portb
	
	;DAC signals
	.equ	DWS=2	;DAC channel multiplex 
	.equ	pDWS=portb
	.equ	DCK=7	;DAC clock 
	.equ	pDCK=portb
	.equ	DDT=5	;DAC data 
	.equ	pDDT=portb
	.equ	DSS=4	;SPI SS (must be configured as output!)
	.equ	pDSS=portb
	
	;SRAM signals
	.equ	RWR=3	;SRAM write (negative pulse)
	.equ	pRWR=portb
	.equ	ROE=4	;SRAM output enable (negative active)
	.equ	pROE=portd
	.equ	RMA=6	;SRAM medium address latch clock (rissing edge sensitive)
	.equ	pRMA=portd
	.equ	RHA=5	;SRAM high address latch clock (rissing edge sensitive)
	.equ	pRHA=portd
	
	;buttons
	.equ	BCO=2	;Buttons common signal (active low)
	.equ	pBCO=portd
	.equ	dBCO=ddrd

	
	

	;RAM SEGMENT
	;most of variabels defined here
	.dseg
	.org	0x0100	;user RAM start
	.equ	STSIZE=256	;reserved stack memory (cause error if free memory less than STSIZE)
	
BtnInh:	.byte	1	;Buttons inhibitor
BtnDiv:	.byte	1	;Buttons irq divider
BtnLast:	.byte	1	;Last buttons state
BtnAutoS:	.byte	1	;buttons auto repeater step
BtnAutoD:	.byte	1	;buttons auto repeater delay counter
BtnAutoR:	.byte	1	;buttons auto repeater rate variable
SRAMsize:	.byte	1	;external SRAM size in 64kiB blocks
MODflag:	.byte	1	;MOD playback flags
	;0 - leave playback (playback done)
	;1 - previous/next file direction
	;2 - playback running
	;3 - folder selection mode
	;4 - change song
	;5 - no storage found
	;6 - load from COM port
MODs3m:	.byte	1	;MOD type (0 - MOD, 1 - modified S3M) 
MODname:	.byte	20	;MOD name
MODinsn:	.byte	1	;MOD instruments count
MODchnn:	.byte	1	;MOD channels count
MODordpos:	.byte	2	;MOD order list offset in SRAM
MODpatpos:	.byte	2	;MOD pattern data offset
MODpatn:	.byte	1	;MOD orders count (song len in patterns)
MODpats:	.byte	1	;MOD pattern count (real pattern count)
	.equ	MODsmplSize=1+3+3+3+1+2
MODsmpl:	.byte	MODsmplSize*MAXINS	;MOD sample info for max instruments count
	; +0 vol [0-64]
	; +1 begin [Bytes]
	; +4 end [Bytes]
	; +7 repeat [Bytes]
	;+10 repeat [1-on, 0-off]
	;+13 transposition constant [fx 8.8]
MODspd:	.byte	1	;MOD speed (ticks per note cycle)
MODbpm:	.byte	1	;MOD beats per minute
MODtdiv:	.byte	2	;MOD tick divisor (2.5*sampling_rate/bpm)
MODtdivf:	.byte	1	;MOD tick divisor fraction (2.5*sampling_rate/bpm)
MODtfcnt:	.byte	1	;MOD tick divisor fraction counter
MODtimc:	.byte	1	;MOD timing changed flag (1 - speed, 2 - tempo)
MODpat:	.byte	1	;MOD actual order list postion
MODpln:	.byte	1	;MOD actual pattern row
MODpnln:	.byte	1	;MOD new line for pattern break effect
MODncnt:	.byte	1	;MOD note rate divisor counter
MODpatp:	.byte	3	;MOD actual note pointer (ext. SRAM)

	
	;--- MOD playback structure for each channel ---
	; volume slide chunk
	; +0 effect active
	; +1 signed step 
	.equ	MODevolsSize=1+1
	; portamento chunk
	; +0 bit0: active, bit1: direction, bit2: tone-portamento 
	; +1 step (unsigned)
	.equ	MODeportSize=1+1
	; arpeggio chunk
	; +0 active
	; +1 tone-id counter
	; +2 second period
	; +4 third period
	.equ	MODearpSize=1+1+2+2
	; vibrato
	; +0 active
	; +1 waveform step
	; +2 waveform amplitude
	; +3 waveform position
	.equ	MODevibSize=1+1+1+1
	; retrigger chunk
	; +0 active/type 
	; +1 countdown counter
	; +2 countdown counter reload value (delay/rate) or delayed note instrument id
	; +3 delayed note period
	; +5 delayed note volume command
	.equ	MODeretSize=1+1+1+2+1
	; position effect (sample position offset in 0x100 multiples) 
	.equ	MODeposSize=1
	; tremolo chunk
	; +0 active
	; +1 waveform step
	; +2 waveform amplitude
	; +3 waveform position
	.equ	MODetrmSize=1+1+1+1
	;common playback structure chunk
	; +0 playback activity 0/255
	; +1 channel volume <0;64>
	; +2 resampling coeff. [fx 8.8]
	; +4 position in sample [fx 24.8]
	; +8 end of sample
	;+11 loop len
	;+14 loop activity 0/255
	;+15 actual instrument id
	;+16 actual period
	;+18 final period for tone-portamento
	;+20 panning <0;128>
	;+21 left channel mix volume <0;64>
	;+22 right channel mix volume <0;64>
	;+23 actual note id / orig. period
	.equ	MODmplSize=1+1+2+4+3+3+1+1+2+2+1+1+1+2	
	.equ	MODpbckSize=MODmplSize+(MODevolsSize+MODeportSize+MODearpSize+MODevibSize+MODeretSize+MODeposSize+MODetrmSize)
MODpbck:	.byte	MODpbckSize*MAXCH	
	.equ	mePB=0	;main playback data offset
	.equ	meVS=mePB+MODmplSize	;volume slide offset
	.equ	meTP=meVS+MODevolsSize	;tone portamento offset
	.equ	meAR=meTP+MODeportSize	;arpeggio offset
	.equ	meVI=meAR+MODearpSize	;vibrato offset
	.equ	meRT=meVI+MODevibSize	;retrigger offset
	.equ	mePS=meRT+MODeretSize	;sample position offset
	.equ	meTR=mePS+MODeposSize	;tremolo offset
	
	.if	DRAWVU==1
MODbars:	.byte	MAXCH	;MOD VU-meter peaks for each channel (peak detection buffer)
MODbarsInt:	.byte	MAXCH	;MOD VU-meter peaks for each channel (slow decay buffer)
	.endif
FileLen:	.byte	3	;MOD file len [Byte]
TimOut:	.byte	1	;timer1 timeout flag
TimOutDiv:	.byte	1	;timeout timer divisor 
FIFOtwr:	.byte	1	;FIFO free to write [Byte]
FIFOrd:	.byte	2	;FIFO read pointer
FIFOwr:	.byte	2	;FIFO write pointer
	.equ	FIFOsize=2000	;FIFO size (multiple of 4!)
FIFO:	.byte	FIFOsize	;FIFO buffer
	.equ	DBuff=FIFO	;data buffer (overlapped with FIFO, size at least 1024B)


	;CODE SEGMENT
	.cseg
	.org	0x0000	;reset
	rjmp	Init
	.org	0x0012	;timer 2 ctc
	rjmp	Timer2ctc
	.org	0x001A	;timer 1 ctc
	rjmp	Timer1ctc
	.org	0x0020	;timer 0 ctc
	rjmp	Timer0ctc
	

;************************************************************************************************
;	FIFO output
;************************************************************************************************
	;some realy usefull constants
	.if	XTAL/(8*SRATE)-1>255	;ocr constant higher than 255?
	.equ	SRDIV=64	;yeah - set timer 0 divisor /64
	.else
	.equ	SRDIV=8	;no - set timer 0 divisor /8
	.endif
	.equ	SROCR=XTAL/(SRDIV*SRATE)-1	;timer 0 ocr constant for SRATE sampling freq. and SRDIV divisor 
	.equ	SREAL=XTAL/(SRDIV*(SROCR+1))	;REAL sampling rate for SROCR ocr constant [Hz]
	.equ	PALF=256*3546895/SREAL	;magic constant for resampling coeff. calcualtion (PAL/2*real_sample_rate [fx ?.8])
	.equ	MINT=30	;minimal allowed period (usually 54)
	.equ	MAXT=4095	;maximum allowed period (usually 1814)
	
	
	;period to resamplimg coeff calculation R16:R17=PALF/T, where T=R16:R17
	.macro	T2RC
	.if	PALF<=65535
	;for PALF [fx 8.8]
	ldi	R16,low(PALF)
	ldi	R17,high(PALF)
	call	div16u
	.else
	;for PALF [fx 16.8]
	push	R13
	push	R20
	push	R21
	mov	R20,R19
	mov	R19,R18
	ldi	R21,0
	ldi	R16,low(PALF)
	ldi	R17,high(PALF)
	ldi	R18,byte3(PALF)
	call	div24u
	pop	R21
	pop	R20
	pop	R13
	.endif	
	.endm

	
	;irq DAC output routine (timer0 ocra interrupt)
Timer0ctc:	.if	DAC==1543
	;---------;
	; TDA1543 ;
	;---------;
	out	ocr0b,R16	;push
	in	R16,sreg
	out	gpior0,R16	;push
	out	gpior1,R30	;push
	out	gpior2,R31	;push
	;
	ldsw	R30,FIFOrd	;FIFO read pointer
	;
	ld	R16,Z+
	out	spdr,R16	;high byte left
	;15 cycles pause
	ldi	R16,5
	dbnz	R16,PC-1
	;
	ld	R16,Z+
	out	spdr,R16	;low byte left
	;12 cycles pause
	ldi	R16,3
	dbnz	R16,PC-1
	cpiwh	R30,R16,FIFO+FIFOsize-2
	;
	sbi	pDWS,DWS	;toggle WS
	nop
	ld	R16,Z+
	out	spdr,R16	;high byte right
	ld	R16,Z+
	;15 cycles pause
	brne	PC+1+2+1
	ldiw	R30,FIFO	;reset read pointer if FIFO overflow
	rjmp	PC+1+1+1
	adiw	R30,0	;2x nop
	nop
	stsw	FIFOrd,R30	;store FIFO read pointer	
	ldi	R16,2
	dbnz	R16,PC-1	;6 cycles
	;
	out	spdr,R16	;low byte right
	;15 cycles pause
	ldi	R16,2
	dbnz	R16,PC-1	;6 cycles
	nop
	in	R31,gpior2	;pop
	in	R30,gpior1	;pop
	in	R16,gpior0	;pop
	out	sreg,R16
	in	R16,ocr0b	;pop
	;
	cbi	pDWS,DWS	;toggle WS
	

	.elif	DAC==1545
	;---------;
	; TDA1545 ;
	;---------;
	;irq DAC output routine (timer0 ocra interrupt)
	out	ocr0b,R16	;push
	in	R16,sreg
	out	gpior0,R16	;push
	out	gpior1,R30	;push
	out	gpior2,R31	;push
	;
	ldsw	R30,FIFOrd	;FIFO read pointer
	;
	ld	R16,Z+
	out	spdr,R16	;high byte left
	;15 cycles pause
	ldi	R16,5
	dbnz	R16,PC-1
	;
	ld	R16,Z+
	out	spdr,R16	;low byte left
	;15 cycles pause
	ldi	R16,3
	dbnz	R16,PC-1
	nop
	cpiwh	R30,R16,FIFO+FIFOsize-2	
	;
	ld	R16,Z+
	sbi	pDWS,DWS	;negate WS
	out	spdr,R16	;high byte right
	ld	R16,Z+
	;15 cycles pause
	brne	PC+1+2+1
	ldiw	R30,FIFO	;reset FIFO read pointer if overflow
	rjmp	PC+1+1+1
	adiw	R30,0	;2x nop
	nop
	stsw	FIFOrd,R30	;store FIFO read pointer
	ldi	R30,2
	dbnz	R30,PC-1	;6 cycles
	;
	out	spdr,R16	;low byte right
	;15 cycles pause
	ldi	R30,3
	dbnz	R30,PC-1
	nop
	in	R31,gpior2	;pop
	in	R30,gpior1	;pop
	in	R16,gpior0	;pop
	out	sreg,R16
	in	R16,ocr0b	;pop
	;
	cbi	pDWS,DWS	;negate WS
	
	.else
	.error	"INVALID DAC SELECTION"
	.endif
	reti
	

	;start DAC output
	.macro	DArun
	lds	R16,timsk0
	sbr	R16,1<<ocie0a
	sts	timsk0,R16
	.endm
	
	;stop DAC output
	.macro	DAstop
	lds	R16,timsk0
	cbr	R16,1<<ocie0a
	sts	timsk0,R16
	.endm

	;FIFO clear
FIFOclr:	ldiw	R30,FIFO
	ldiw	R24,FIFOsize
	ldi	R16,0
FIFOclrL:	st	Z+,R16
	dbnzwh	R24,FIFOclrL
	ret

	;synchronous FIFO clear
FIFOclrSync:	brid	FIFOclrSyncS	;skip if global irq disabled
	lds	R16,timsk0
	andi	R16,1<<ocie0a
	breq	FIFOclrSyncS	;skip if FIFO output irq disabled
	;main sync fill
	ldi	R20,high(FIFOsize)
	inc	R20
	ldiw	R16,0	;0 sample
	ldi	R18,high(FIFO+FIFOsize)
	ldsw	R28,FIFOwr	;FIFO write pointer
FIFOclrSyncVS:	cli		;critical section begin
	ldsw	R24,FIFOrd	;FIFO read pointer
	sei		;critical section end
	subw	R24,R28	;free space
	brcc	PC+1+2
	subiw	R24,-FIFOsize	;if negative	
	cpi	R25,1	;at least 256Bytes there? 
	brlo	FIFOclrSyncVS	;no - try again
	;fill 256 bytes
	ldi	R19,256/4
FIFOclrSyncFDW:	stw	Y+,R16
	stw	Y+,R16	;store stereo sample
	cpi	R28,low(FIFO+FIFOsize)
	cpc	R29,R18
	brlo	PC+1+2
	ldiw	R28,FIFO
	dbnz	R19,FIFOclrSyncFDW	;next sample
	subi	R20,1
	brcc	FIFOclrSyncVS	;next 256 bytes
	;done
FIFOclrSyncS:	ret



;************************************************************************************************
;	Timeout timer
;************************************************************************************************
	;timeout timer irq
Timer1ctc:	pusht	R16
	in	R17,sreg
	;
	lds	R16,TimOutDiv
	inc	R16
	cpi	R16,10
	brlo	PC+1+1+1+2
	clr	R16
	ldi	R18,0xFF
	sts	TimOut,R18
	sts	TimOutDiv,R16
	;
	out	sreg,R17
	popt	R16
	reti
	
	;timeout start
	.macro	ToRun
	lds	R16,timsk1
	sbr	R16,1<<ocie1a
	sts	timsk1,R16
	.endm
	
	;timeout stop
	.macro	ToStop
	lds	R16,timsk1
	cbr	R16,1<<ocie1a
	sts	timsk1,R16
	.endm
	
	;timeout set time (in [10ms], up to 33.500s @ XTAL=20MHz)
	.macro	ToTime
	ldi	R16,high(((XTAL/1024)*@0)/1000)
	sts	ocr1ah,R16
	ldi	R16,low(((XTAL/1024)*@0)/1000)
	sts	ocr1al,R16
	.endm
	
	;timeout reset
	.macro	ToRes
	clr	R16
	sts	tcnt1h,R16
	sts	tcnt1l,R16
	sts	TimOut,R16
	sts	TimOutDiv,R16
	.endm
	

;************************************************************************************************
;	Buttons timer
;************************************************************************************************
	;start buttons irq
	.macro	BtRun
	lds	R16,timsk2
	sbr	R16,1<<ocie2a
	sts	timsk2,R16
	.endm
	
	;stop buttons irq
	.macro	BtStop
	lds	R16,timsk2
	cbr	R16,1<<ocie2a
	sts	timsk2,R16
	.endm
	
	;set inhibition state
	.macro	BtInh
	push	R16
	ldi	R16,@0
	sts	BtnInh,R16
	pop	R16
	.endm
	
	;timer 2 ocra irq
Timer2ctc:	push	R16
	in	R16,sreg
	push	R16
	
	BtStop
	sei		;enable golbal irq
	
	;do button stuff?
	lds	R16,BtnInh
	tst	R16
	rjne	Timer2ctcS	;skip if inhibited
	decs	R16,BtnDiv
	rjne	Timer2ctcS	;skip if not 0
	stsi	BtnDiv,4	;restore divisor const
	;do button stuff
	pusht	R17
	
	in	R17,ddra
	in	R18,porta	;store button port state
	outi	ddra,0x00	;inputs
	outi	porta,0xFF	;pull-ups
	sbi	dBCO,BCO	;set BCO as output
	adiw	R30,0	;nop,nop
	adiw	R30,0	;nop,nop
	lds	R16,BtnLast	;get last btn states
	com	R16
	in	R19,pina	;get actual states
	adiw	R30,0	;nop,nop
	cbi	dBCO,BCO	;BCO as input
	out	porta,R18
	out	ddra,R17	;restore button port state
	
	sts	BtnLast,R19	;store actual state as last one
	com	R19
	eor	R16,R19	;make differences
	and	R16,R19	;only key down events
	
	;some edge?
	push	R19
	tst	R16
	breq	Timer2ctcNE
	;yeah - reset buttons auto repeater
	ldi	R17,50
	sts	BtnAutoD,R17
	ldi	R17,0
	sts	BtnAutoS,R17
	
	rcall	BtnAct	;and do button action
		
	;no - check for button state
Timer2ctcNE:	pop	R19
	tst	R19
	breq	Timer2ctcNS
	;some button active
	decs	R16,BtnAutoD
	brne	Timer2ctcNS
	;delay done - reset delay counter with repeat rate
	lds	R16,BtnAutoR
	sts	BtnAutoD,R16
	
	lds	R16,BtnAutoS
	cpi	R16,0
	breq	PC+1+3
	stsi	BtnAutoS,1
	
	;and do button action
	mov	R16,R19
	rcall	BtnAct	;and do button action
	
Timer2ctcNS:	popt	R17
Timer2ctcS:	BtRun
	pop	R16
	out	sreg,R16
	pop	R16
	reti
	

	;------ button actions list ------
TBtnActList:	.dw	BtnAct0
	.dw	BtnAct1
	.dw	BtnAct2
	.dw	BtnAct3
	.dw	BtnAct4
	.dw	BtnAct5
	.dw	BtnAct6
	.dw	0


	;------ do button actions ------
BtnAct:	;select action (first non-zero bit from LSB)
	pushw	R30
	ldiw	R30,TBtnActList*2	;action list
	ldi	R17,8
BtnActAL:	lsr	R16
	brcc	BtnActNA
	;button pressed
	lpmw	R16,Z+
	movw	R30,R16
	adiw	R30,0
	breq	BtnActAD
	icall		;do action
	rjmp	BtnActAD
BtnActNA:	adiw	R30,2
	dbnz	R17,BtnActAL
	ldi	R16,30
BtnActAD:	popw	R30
	sts	BtnAutoR,R16	;store buttons auto repeater rate const.
	;
	ret

	;------ button actions ------
	;button 0 - file/folder selection (previous)
BtnAct0:	lds	R16,MODflag
	sbr	R16,1<<0|1<<4	;playback done
	cbr	R16,1<<1	;previous
	sts	MODflag,R16
	;
	ldi	R16,30
	ret

	;button 1 - file/folder selection (next) 
BtnAct1:	lds	R16,MODflag
	sbr	R16,1<<0|1<<1|1<<4	;playback done, next
	sts	MODflag,R16
	;
	ldi	R16,30
	ret

	;button 2 - pattern selection (previous)
BtnAct2:	lds	R16,MODpln
	cpi	R16,64
	breq	BtnAct2S
	lds	R16,MODpat
	subi	R16,2
	cpi	R16,-1
	brge	PC+1+1
	ldi	R16,-1
	sts	MODpat,R16
	stsi	MODpln,64	;previous pattern
BtnAct2S:	;
	ldi	R16,9
	ret
		
	;button 3 - pattern selection (next)
BtnAct3:	lds	R16,MODpat
	lds	R17,MODpatn
	dec	R17
	cp	R16,R17
	brlo	PC+1+1+2
	mov	R16,R17
	dec	R16
	sts	MODpat,R16
	stsi	MODpln,64	;new pattern
	;
	ldi	R16,9
	ret
	
	;button 4 - stop
BtnAct4:	lds	R16,MODflag
	bst	R16,2
	brtc	BtnAct4FLD
	;playing
	cbr	R16,1<<2	;stop
	rjmp	BtnAct4STP
	;stopped
BtnAct4FLD:	bst	R16,5
	brts	PC+1+1+1+1	;skip if no storage	
	ldi	R17,1<<3
	eor	R16,R17	;toggle folder selection flag	
BtnAct4STP:	sbr	R16,1<<0	;playback done
	sts	MODflag,R16
	;
	ldi	R16,30
	ret
	
	;button 5 - play
BtnAct5:	lds	R16,MODflag
	bst	R16,3
	brts	BtnAct5PLA
	;folder menu
	sbr	R16,1<<0|1<<3|1<<2	;playback done, play mode, play
	rjmp	BtnAct5FLD
	;play mode
BtnAct5PLA:	bst	R16,2
	brts	BtnAct5FLD
	sbr	R16,1<<0|1<<2	;playback done, play
BtnAct5FLD:	sts	MODflag,R16
	;
	ldi	R16,30
	ret
	
	;button 6 - load from COM
BtnAct6:	lds	R16,MODflag
	bst	R16,3
	brtc	BtnAct6N
	sbr	R16,1<<0|1<<6
	cbr	R16,1<<4
BtnAct6N:	sts	MODflag,R16
	;
	ldi	R16,30
	ret	
	
	

	
	
	
	
	
	
	



	
	
		
;************************************************************************************************
;************************************************************************************************
;	Initialization
;************************************************************************************************
;************************************************************************************************
Init:	;init ports/pins
	outi	ddrb,0b10111111
	cbi	pLCK,LCK	;LCD E=0
	cbi	pDWS,DWS	;DAC WS=0
	sbi	pRWR,RWR	;SRAM RW=1
	outi	ddrd,0b11111000
	sbi	pROE,ROE	;SRAM OE=1
	sbi	pRMA,RMA	;SRAM MA=1
	sbi	pRHA,RHA	;SRAM HA=1
	sbi	pIORD,IORD	;IDE IORD=1
	sbi	pIOWR,IOWR	;IDE IOWR=1
	cbi	pBCO,BCO	;BCO=0
	outi	ddra,0x00
	outi	ddrc,0x00
	
	;set stack
	outiw	sp,ramend
		
	;usart - rx, tx, 8bit, 1stop, no parity
	stsi	ucsr0a,0
	stsi	ucsr0b,1<<rxen0|1<<txen0
	stsi	ucsr0c,1<<ucsz00|1<<ucsz01
	stsiw	ubrr0,USRDIV
	
	;SPI - DAC output
	outi	spcr,1<<spe|1<<mstr
	outi	spsr,1<<spi2x	;10Mbit/s @ 20MHz
	stsiw	FIFOrd,FIFO
	stsiw	FIFOwr,FIFO	;reset FIFO pointers
	
	;timer 0 - FIFO output
	outi	tccr0a,1<<wgm01
	.if	SRDIV==8
	outi	tccr0b,1<<cs01	;/8
	.elif	SRDIV==64
	outi	tccr0b,1<<cs01|1<<cs00	;/64
	.else
	.error	"SAMPLING RATE OUT OF RANGE!!!"
	.endif
	outi	ocr0a,SROCR
	
	;timer 1 - timeout
	stsi	tccr1a,0
	stsi	tccr1b,1<<wgm12|1<<cs10|1<<cs12	;ocra, /1024
	
	;timer 2 - buttons
	stsi	tccr2a,1<<wgm21
	stsi	tccr2b,1<<cs20|1<<cs21|1<<cs22
	stsi	ocr2a,10*(XTAL/1024)/1000
	stsi	BtnDiv,4	;divisor const
	stsi	BtnInh,0	;not inhibited

	clrst	FileLen,R16
	
	;------ initialize LCD ------
	.if	LCDON!=0
	ldi	R16,20
	call	Delay10ms
	rcall	LCDinit
	.endif
		
	;------ ext SRAM test ------
	call	SRAMtest
			
	;------ global IRQ enabled ------
	sei	
	
	;------ controll flags ------
	stsi	MODflag,0	;clear MOD flags
		
	;------ init storage ------
	;IDE
	.if	STORA!=0
	rcall	IDEinit	;initialize IDE device
	brts	STRGideOk
	.endif
	;no storage device
	sbsi	MODflag,1<<0|1<<2|1<<3|1<<5|1<<6	;no storage, play mode, play, playb. done, load COM  
	rjmp	MODplNew	;goto play menu
STRGideOk:	
	;FAT
	rcall	FATinit	;init FAT
	brts	STRGfatOk
	;invalid file system
	sbsi	MODflag,1<<0|1<<2|1<<3|1<<5|1<<6	;no storage, play mode, play, playb. done, load COM
	rjmp	MODplNew	;goto play menu
STRGfatOk:
		

	;--- enable buttons ---
	BtRun




	;-----------------------------------
	;------ folder selection menu ------
	;-----------------------------------
	.if	STORA!=0
	;scan root for folders
	stsi	FATrfld,0	;0. folder 
FMstp:	rcall	FATscanRFld	;scan root
	ldiw	R30,FATsrt
	lds	R25,FATrfldc
	cpi	R25,0
	brne	FMstpNoFld
	;no folder there
	sbsi	MODflag,1<<0|1<<2|1<<3|1<<5|1<<6	;no storage, play mode, play, playb. done, load COM
	rjmp	MODplNew	;goto play menu
FMstpNoFld:	rcall	FATsortNLst	;sort root folders
	
	;scan selected folder for files
FMcrf:	ldiw	R30,FATsrt
	lds	R16,FATrfld
	rcall	FATscanFld	;scan selected folder
	
	;print files count in selected folder
	rcall	LCDclrL2
	lds	R16,FATflec
	rcall	LCDwrBCD3sz
	ldiw	R30,TSngs*2
	rcall	LCDwrStrFlash
	

	;--- folder menu controll loop ---
FMctrll:	BtRun
	ldi	R16,5
	call	Delay1ms
	BtStop
	
	lds	R16,MODflag	;ctrl flags
	bst	R16,3	;play mode?
	brtc	FMnp
	;play mode selected
	lds	R17,FATflec
	cpi	R17,1	;something to play?
	brsh	PC+1+1+2+1
	;no - stay in menu
	cbr	R16,1<<3|1<<0	;not play
	sts	MODflag,R16
	rjmp	PC+1+1
	rjmp	FMplay	;yeah - play selected folder
FMnp:	
	
	;--- change folder selection ---
	bst	R16,0	;playback done flag?
	brtc	FMncf
	;yeah - change folder
	cbr	R16,1<<0
	sts	MODflag,R16	;not playback done
	
	lds	R17,FATrfldc
	cpi	R17,2
	brlo	FMncf	;do nothing if 1 folder only 
	
	;direction?
	bst	R16,1	;up/down?
	brts	FMcfu
	;previous folder
	lds	R17,FATrfld
	cpi	R17,0
	brne	PC+1+2
	lds	R17,FATrfldc
	dec	R17
	sts	FATrfld,R17
	rjmp	FMcrf	;changed folder
	;next folder
FMcfu:	lds	R17,FATrfld
	inc	R17
	lds	R18,FATrfldc
	cp	R17,R18
	brlo	PC+1+1
	ldi	R17,0
	sts	FATrfld,R17
	rjmp	FMcrf	;changed folder
FMncf:	rjmp	FMctrll	;stay in menu
	

	
	;--- play mode selected ---	
FMplay:	ldiw	R30,FATsrt
	clr	R0
	lds	R16,FATrfld
	add	R30,R16
	adc	R31,R0
	ld	R16,Z
	sts	FATrflds,R16	;selected folder index (sorted)

	ldiw	R30,FATsrt
	lds	R25,FATflec
	rcall	FATsortNLst	;sort files in selected folder
	
	lds	R16,MODflag
	cbr	R16,1<<4	;not next song
	sbr	R16,1<<0	;playback done
	sts	MODflag,R16
	stsi	FATfle,0	;0. file selected
	.endif




	;---------------------------------
	;------ file selection menu ------
	;---------------------------------
MODplNew:	BtRun
	ldi	R16,5
	call	Delay1ms
	BtStop

	lds	R16,MODflag
	.if	STORA!=0
	bst	R16,3	;leave this menu?
	brts	PC+1+1+2+1
	;yeah
	cbr	R16,1<<0
	sts	MODflag,R16
	rjmp	FMstp	;goto folder selection
	.endif

	
	bst	R16,0	;some other action to do?
	brtc	MODplNew
	;yeah
	cbr	R16,1<<0	;clear playback done flag
	sts	MODflag,R16
	
	bst	R16,4	;change song?
	brtc	MODplNewNPE	;no - skip file change
	;yeah - change it
	cbr	R16,1<<4	;clear it
	sts	MODflag,R16
	bst	R16,5
	brts	MODplNewNPE	;skip if no storage
	bst	R16,6
	brts	MODplNewNPE	;or COM load selected
	
	
	.if	STORA!=0
	bst	R16,1	;direction?
	brts	MODplNewFP
	;previous file
	lds	R17,FATfle
	cpi	R17,0
	brne	PC+1+2
	lds	R17,FATflec
	dec	R17
	sts	FATfle,R17	;previous file
	rjmp	MODplNewNPE
	;next file
MODplNewFP:	lds	R18,FATfle
	inc	R18
	lds	R17,FATflec
	cp	R18,R17
	brlo	PC+1+1
	ldi	R18,0
	sts	FATfle,R18	;next file
	.endif	
MODplNewNPE:	
			
	;--- MOD song into SRAM ---
	lds	R16,MODflag
	bst	R16,6
	brts	MODplNewCOMF
	bst	R16,5	;no storage?
	brtc	MODplNewFLE
	;yeah - from COM port
	rjmp	MODplNewCOMNL
MODplNewCOMF:	cbr	R16,1<<6
	sts	MODflag,R16
	
MODplNewCOMLA:	call	LoadData	;time critical section - stop all irqs longer than few cycles!
	lds	R16,MODflag
	brts	MODplNewCOMLO
	bst	R16,5
	brtc	MODplNewFLE	;skip if COM load failed but storage ok
	;COM load failed, no storage
	ldst	R17,FileLen
	clr	R20
	tstt	R17,R20
	breq	MODplNewCOMLA
MODplNewCOMLO:
	bst	R16,5
	brts	PC+1+1+2
	;storage ok
	sbr	R16,1<<2	;play if storage dev. ok 
	sts	MODflag,R16
	
MODplNewCOMNL:	call	MODprnNameE
	rjmp	MODplNewCOM
	
	;from file
MODplNewFLE:	.if	STORA!=0
	clr	R18	;play - full song load
	lds	R16,MODflag
	bst	R16,2	;play/stop flag?
	brts	PC+1+1
	ser	R18	;stop - only song name printout
	lds	R16,FATrflds
	lds	R17,FATfle
	ldiw	R30,FATsrt
	rcall	FATrdFle	;load 'FATfle' file from 'FATrflds' folder
	.endif	 
MODplNewCOM:			
	
	;--- play? ---
	lds	R16,MODflag
	bst	R16,2
	rjtc	MODplNew	;no - file selection
	;yeah
	cbr	R16,1<<0|1<<4	;not playback done
	sts	MODflag,R16
		
	;clear status line
	.if	LCDON!=0
	rcall	LCDclrL2
	.endif
	
	
	;--- init and identify MOD ---
	call	MODload	;load MOD
	brts	MODplNewLDO
	;load failed
	lds	R16,MODflag
	sbr	R16,1<<0|1<<4	;forward, next song
	bst	R16,5
	bld	R16,6	;load again if no storage
	sts	MODflag,R16
	rjmp	MODplNew
	
MODplNewLDO:	;allow controls
	BtRun		;allow buttons
	
	;and goto playback loop 
	rcall	MODplay
	
	;done, leave
	rjmp	MODplNew
	


	



	
	
	;OTHER MODULES
	.include "usart.asm"	;USART rouines
	.include "LCD.asm"	;LCD routines
	.include "storage.asm"	;IDE storage routiens
	.include "mod_decoder.asm"	;MOD decoder routines
	.include "ext_sram.asm"	;External SRAM routines
	.include "math.asm"	;Some math routines
	.include "mod_tables.asm"	;MOD decoder constant tables
	.include "text.asm"	;Just some text


;************************************************************************************************
;	Other routines
;************************************************************************************************
;------------------------------------------------------------------------------------------------
;	R16*1ms delay (0=~256ms)
;------------------------------------------------------------------------------------------------
Delay1ms:	;~1ms
	ldi	R17,XTAL/((100*3)*1000)
	;100*3 cycles
Delay1msL2:	ldi	R18,100
Delay1msL1:	dbnz	R18,Delay1msL1
	dbnz	R17,Delay1msL2
	dbnz	R16,Delay1ms	
	ret
	
;------------------------------------------------------------------------------------------------
;	R16*10ms delay (0=~2.56s)
;------------------------------------------------------------------------------------------------
Delay10ms:	mov	R15,R16
	ldi	R16,10
	rcall	Delay1ms	;10ms delay
	dbnz	R15,PC-1-1-1
	ret

	

	
	
	
;################################################################################################
;	THIS MUST BE AT VERY END OF MAIN MODULE AFTER ALL INCLUDES
;################################################################################################
	.dseg
REND:	.byte	0
	.if	REND>=RAMEND-STSIZE
	.error	"RAM IS FULL!!!"
	.endif
	.cseg
	
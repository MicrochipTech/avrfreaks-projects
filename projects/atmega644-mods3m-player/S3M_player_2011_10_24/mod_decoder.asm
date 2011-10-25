;compiler: avrasm

;************************************************************************************************
; Embeded MOD/S3M player for AVR ATmega644.
;
; module: MOD/S3M decoder
;************************************************************************************************
; Licence: This source code is free to use for any non-comercial purposes.
;          (C) 2011, Stanislav Maslan, s.maslan@seznam.cz 
;************************************************************************************************




;************************************************************************************************
;	MOD decoder loop
;	This is main playback loop where are all effects applied and samples written
;	into audio FIFO buffer. It fills FIFO with new sound data until MOD is
;	finished. It can be also terminated with MODflag bit0.
;************************************************************************************************	
MODplay:	;initiate audio output
	stsiw	FIFOrd,FIFO
	stsiw	FIFOwr,FIFO	;reset FIFO pointers
	call	FIFOclr	;clear FIFO garbage
	DArun		;run DAC output

	;config ports/signals
	outi	ddra,0xFF	;address port as output
	outi	ddrc,0x00	;data port as input
	outi	portc,0xFF	;with pull-ups
	cbi	pROE,ROE	;SRAM OE=0
			
	;------ MOD PLAYBACK LOOP BEGIN ------
MODplLoop:	;------ TICK CYCLE BEGIN ------
	ldsw	R24,MODtdiv
	;samples/tick fine tune
	lds	R16,MODtfcnt	;tick_rate_fraction_counter
	lds	R17,MODtdivf	;tick_rate_fraction
	inc	R16	;tick_rate_fraction_counter++
	andi	R16,0x0F	;tick_rate_fraction_counter to <0;15>
	cp	R16,R17
	brsh	PC+1+1
	adiw	R24,1	;add one sample to samples/tick ratio if: tick_rate_fraction_counter<tick_rate_fraction
	sts	MODtfcnt,R16
	pushw	R24	;store tick rate counter
		
	;draw VUs
	.if	LCDON!=0 && DRAWVU!=0
	lds	R16,MODncnt
	bst	R16,0
	brts	PC+1+1
	rcall	LCDwrMODvu
	.endif
	
		
	;--- effects ---
	ldiw	R28,MODpbck
	clr	R2	;0
	lds	R24,MODchnn
MODplEL:	
	
	;RETRIGGER
	lds	R16,MODncnt
	cpi	R16,1
	brne	PC+1+1
	std	Y+meRT+0,R2	;new line in this tick, stop retrigger
	
	ldd	R16,Y+meRT+0
	cpi	R16,0
	breq	MODplENR	;skip if no effect active
		
	;get sample pointer
	ldd	R20,Y+15	;load actual sample id
	ldi	R16,MODsmplSize
	mul	R16,R20
	movw	R26,R0
	subiw	R26,-(MODsmpl-MODsmplSize)	;sample pointer
	
	ldd	R16,Y+meRT+0
	cpi	R16,0x09
	brne	MODplENRR
	;--- note retrigger
	cpi	R20,0	;sample id not specified yet?
	breq	MODplENR	;skip if no sample specified
	;some sample there
	
	ldd	R18,Y+meRT+1
	dec	R18
	brne	MODplENRRn
	;retrigger
	lds	R16,MODncnt
		
	;set note parameters
	adiw	R26,1
	ldtb	R18,X+	;get sample begin
	lddw	R16,Y+2	;resample_coeff
	ldi	R21,0
	sub	R21,R16
	sbc	R18,R17
	sbc	R19,R2
	sbc	R20,R2	;sample_begin-=resample_coeff	
	std	Y+4+0,R21	;store its fraction part		
	stdt	Y+4+1,R18	;store full part
	;retrigger note
	ldi	R16,0xFF
	std	Y+0,R16
	
	ldd	R18,Y+meRT+2
MODplENRRn:	std	Y+meRT+1,R18
	rjmp	MODplENR
	
MODplENRR:	cpi	R16,0x0C
	brne	MODplENRC
	;--- note cut off
	ldd	R18,Y+meRT+1
	dec	R18
	std	Y+meRT+1,R18
	brne	MODplENR
	;cut off note
	std	Y+meRT+0,R2	;no retrigger effect
	std	Y+1,R2	;0 volume
	rjmp	MODplENR
	
MODplENRC:	cpi	R16,0x0D
	brne	MODplENR
	;--- note delay
	ldd	R18,Y+meRT+1
	dec	R18
	std	Y+meRT+1,R18
	brne	MODplENR
	;trigger note
	std	Y+meRT+0,R2	;no retrigger effect

	ldd	R20,Y+meRT+2	;sample id
	lddw	R21,Y+meRT+3	;sample period
	ldi	R23,0	;no effect
	ldd	R26,Y+meRT+5	;volume command
	rcall	MODrPatPCNT	;trigger sample
MODplENR:	

	
	;VIBRATO
	ldd	R16,Y+meVI+0
	bst	R16,0	;running?
	rjtc	MODplEVBS
	;yeah - vibrato active
	ldd	R18,Y+meVI+3	;load waveform position
	ldd	R20,Y+meVI+1	;load waveform step
	mov	R30,R18
	clr	R31
	subiw	R30,-TMODsine64*2	;waveform table
	lpm	R16,Z+	;read waveform value u 
	add	R18,R20	;add step
	andi	R18,0x3F	;check overrun <0;63>
	std	Y+meVI+3,R18	;store position pointer
		
	;apply amplitude to waveform
	ldd	R17,Y+meVI+2	;amplitude
	clr	R18
	mulsu	R16,R17	;u*amplitude [fx 16.8]
	brcc	PC+1+1
	dec	R18	;sign extension
	lddw	R20,Y+16	;load central period
	mov	R19,R0
	add	R20,R1
	adc	R21,R18	;sum it with freq. modulation delta [fx 16.8]
			
	;period limits
	brmi	PC+1+3+1
	cpiwh	R20,R16,MINT
	brpl	PC+1+3
	ldit	R19,MINT<<8	;check low limit
	cpiwh	R20,R16,MAXT
	brmi	PC+1+3
	ldit	R19,MAXT<<8	;check high limit
	;resampling coeff. (calculated in 24-bit precision instead of usual 16-bit)	
	ldit	R16,((3546895*256*2)/SREAL)*(128/4)
	lsrt	R19
	lsrt	R19	;period/=4
	call	div24u
	stdw	Y+2,R16	;store resampling coeff. [fx 8.8]
	
MODplEVBS:		
	;ARPEGGIO
	ldd	R16,Y+meAR+0
	bst	R16,0
	brtc	MODplARPS	;skip if not active
	;arpeggio active
	ldd	R20,Y+meAR+1	;load counter
	cpi	R20,0
	brne	MODplARPT0S
	;0. tone (base one)
	lddw	R18,Y+16
	rjmp	MODplARPTLD
MODplARPT0S:	cpi	R20,1
	brne	MODplARPT1S
	;1. tone
	lddw	R18,Y+meAR+2
	rjmp	MODplARPTLD
	;2. tone
MODplARPT1S:	lddw	R18,Y+meAR+4
MODplARPTLD:	;ldiw	R16,256*3546895/SREAL	;PAL/(2*SREAL) [fx 8.8]
	;rcall	div16u	;PAL/(2*SREAL)/T
	T2RC		;PAL/(2*SREAL)/T
	stdw	Y+2,R16	;store resampling coeff
	;new tone
	inc	R20
	cpi	R20,3
	brlo	PC+1+1
	ldi	R20,0	;reset counter
	std	Y+meAR+1,R20	;store counter
MODplARPS:	

	lds	R16,MODncnt
	cpi	R16,1
	rjeq	MODplEPORTS	;skip if new line
	
	;VOLUME SLIDE
	ldd	R16,Y+meVS+0
	cpi	R16,0
	breq	MODplEVSS	;not active - skip
		
	;effect active
	ldd	R16,Y+meVS+1	;slide step
	ldd	R17,Y+1	;channel volume
	add	R17,R16	;add step
	cpi	R17,64
	brlt	MODplEVSLT
	;overflow
	ldi	R17,64
	std	Y+meVS+0,R2	;stop effect
	rjmp	MODplEVSGE	
MODplEVSLT:	cpi	R17,0
	brge	MODplEVSGE
	;underflow
	ldi	R17,0
	std	Y+meVS+0,R2	;stop effect
MODplEVSGE:	std	Y+1,R17	;store channel volume	
MODplEVSS:
	
	
	;PORTAMENTO
	ldd	R25,Y+meTP+0
	bst	R25,0	;activity bit
	rjtc	MODplEPORTS	;skip if not active
	;portamento active
	lddw	R18,Y+16	;load period
	ldd	R16,Y+meTP+1	;portamento step
	ldi	R17,0
	bst	R25,2	;tone-portamento bit
	brtc	MODplEPORTN
	
	;tone-portamento active (3)
	lddw	R20,Y+18	;load final period
	bst	R25,1	;portamento direction
	brtc	MODplEPORTTU
	;period down
	subw	R18,R16	;sub step
	brpl	PC+1+2
	ldiw	R18,MINT	;check underflow
	cpiwh	R18,R22,MINT
	brsh	PC+1+2
	ldiw	R18,MINT	;check lowest period	
	cpw	R18,R20
	brsh	MODplEPORTHOK	
	movw	R18,R20	;final reached
	ldd	R16,Z+0
	cbr	R16,1<<0
	std	Y+meTP+0,R16	;stop effect
	rjmp	MODplEPORTHOK
	;period up
MODplEPORTTU:	addw	R18,R16	;add step
	cpw	R18,R20
	brlo	MODplEPORTHOK	
	movw	R18,R20	;final reached
	ldd	R16,Y+meTP+0
	cbr	R16,1<<0
	std	Y+meTP+0,R16	;stop effect
	rjmp	MODplEPORTHOK
 	
	
	;portamento active (1/2)
MODplEPORTN:	bst	R25,1	;portamento direction
	brtc	MODplEPORTU
	;portamento up
	negw	R16	;negative step
MODplEPORTU:	addw	R18,R16	;add step to period
	cpiwh	R18,R16,MINT
	brsh	MODplEPORTLOK
	;to low period
	ldiw	R18,MINT
	std	Y+meTP+0,R2	;stop effect
	rjmp	MODplEPORTHOK	
MODplEPORTLOK:	cpiwh	R18,R16,MAXT
	brlo	MODplEPORTHOK
	;to high period
	ldiw	R18,MAXT
	std	Y+meTP+0,R2	;stop effect
MODplEPORTHOK:	stdw	Y+16,R18	;store period
	;calculate new resampling coeff
	;ldiw	R16,256*3546895/SREAL
	;rcall	div16u
	T2RC		;PAL/(2*SREAL)/T
	stdw	Y+2,R16	;store resampling coeff	
MODplEPORTS:	
	adiw	R28,MODpbckSize	;skip playback channel
	djnz	R24,MODplEL
	
	
	

	;------ TICK CYCLE END ------
	decs	R24,MODncnt
	rjne	MODplSNote
	lds	R24,MODspd
	sts	MODncnt,R24	;store note rate counter
	;------ NOTE CYCLE BEGIN ------
	
	
	;stop some effects
	ldiw	R28,MODpbck
	clr	R0
	lds	R24,MODchnn
MODplNCCE:	;stop volume slide effect
	std	Y+meVS+0,R0
	;stop portamento effect
	ldd	R16,Y+meTP+0
	cbr	R16,1<<0
	std	Y+meTP+0,R16
	;stop retrigger effects
	std	Y+meRT+0,R0	;activity
	;
	adiw	R28,MODpbckSize	;next channel
	dbnz	R24,MODplNCCE

	

	;playback controll
	lds	R16,MODflag
	bst	R16,0
	brts	MODplPD	;forced end
	
	;read line
	rcall	MODrPat	;read pattern line
	brtc	MODplNEX
	;playback done - song end
	lds	R16,MODflag
	sbr	R16,1<<0|1<<1|1<<4	;forward, next song
	bst	R16,5
	bld	R16,6	;load COM again if no storage
	sts	MODflag,R16
MODplPD:	;playback done - forced end
	popw	R24	;pop tick counter
	;config ports/signals
	outi	ddra,0x00	;address port as inputs
	sbi	pROE,ROE	;SRAM OE=1
	call	FIFOclrSync	;clear DAC FIFO synchronously
	DAstop		;and stop it
	ret		;leave	
MODplNEX:
		

	;write status (line/pattern/total patterns)
	.if	LCDON!=0
	rcall	LCDwrMODln
	.endif
		
	;test for speed/tempo modification
	lds	R20,MODtimc
	bst	R20,0	;speed changed?
	brtc	MODplSMN
	;yeah - reset note cycle counter
	lds	R16,MODspd
	sts	MODncnt,R16
	cbr	R20,1<<0	;clear flag
	;
MODplSMN:	bst	R20,1	;tempo changed?
	brtc	MODplSTN
	;yeah - reset tick cycle counter
	popw	R16	;load tick cycle counter
	ldsw	R16,MODtdiv
	pushw	R16	;store tick cycle counter
	cbr	R20,1<<1	;clear flag
MODplSTN:	sts	MODtimc,R20


	;------ NOTE CYCLE END ------
MODplSNote:	
	;------ POST NOTE TICK CYCLE BEGIN ------

	;calculate left/right volumes from pan
	ldiw	R28,MODpbck
	lds	R24,MODchnn
MODplPacL:	ldd	R16,Y+1	;vol <0;64>

	;TREMOLO
	ldd	R17,Y+meTR+0
	bst	R17,0
	brtc	MODplPtcNV
	;do tremolo
	ldd	R18,Y+meTR+3	;waveform pos
	ldd	R19,Y+meTR+1	;waveform step
	mov	R30,R18
	clr	R31
	subiw	R30,-TMODsine64*2
	lpm	R17,Z+	;u=waveform value <-127;+127>
	add	R18,R19
	andi	R18,0x3F	;waveform pos <0;63>
	std	Y+meTR+3,R18	;store new wavefrom pos
	;
	ldd	R18,Y+meTR+2	;amplitude <0;15>
	lsl	R18	;amplitude <0;30>
	fmulsu	R17,R18	;R1=(u*amplitude)<<1 <-29;+29>
	;apply to channel volume
	add	R16,R1	;vibrated volume
	brpl	PC+1+1
	ldi	R16,0	;low limit
	cpi	R16,64
	brlo	PC+1+1
	ldi	R16,64	;high limit
MODplPtcNV:	

	.if	STEREO==1
	;stereo
	ldd	R17,Y+20	;pan <0;128>
	fmul	R16,R17
	std	Y+21,R1	;vol_left=(pan*vol)>>7 <0;64>
	ldi	R18,128
	sub	R18,R17	;128-pan
	fmul	R16,R18
	std	Y+22,R1	;vol_right=((128-pan)*vol)>>7 <0;64>
	.else
	;mono
	std	Y+21,R16	;vol_left=vol <0;64>
	.endif
	adiw	R28,MODpbckSize	;next channel
	dbnz	R24,MODplPacL
		
	
	;------ POST NOTE TICK CYCLE END ------
	popw	R24	;restore note_rate_counter	
MODplSTick:
	;------ SAMPLES BLOCK BEGIN ------

	;some constants
	clr	R8	;0
	ldi	R21,1	;1
	lds	R20,MODchnn	;channels count
	ldi	R22,high(FIFO+FIFOsize)
	
	;wait for some place in FIFO
	ldsw	R28,FIFOwr	;FIFO write pointer	
MODplSmplBL:	cli		;critical section begin
	ldsw	R26,FIFOrd	;FIFO read pointer
	sei		;critical section end
	subw	R26,R28	;free_bytes=FIFOrd-FIFOwr
	brcc	PC+1+2+2
	;rd<wr
	ldiw	R26,FIFO+FIFOsize
	subw	R26,R28	;free_bytes=FIFOsize-FIFOwr
	breq	MODplSmplBL	;check again if free_bytes==0 
	lsrw	R26
	lsrw	R26	;max_samples_to_write=free_bytes/4	

	;calc samples to actually write (R24:R25) and samples to next tick (R30:R31)
	movw	R30,R24
	subw	R30,R26	;samples_before_tick-=max_samples_to_write
	brpl	PC+1+2+1
	ldiw	R30,0	;samples_before_tick=0 if max_samples_to_write>=samples_before_tick
	rjmp	PC+1+1
	movw	R24,R26	;samples_to_write=max_samples_to_write
	pushw	R30	;store samples_before_tick (note rate counter)
	
	;movw	R16,R24
	;call	UsTxWord

	;------ SAMPLE CYCLE BEGIN ------
MODplSmplL:
	.if	DRAWVU==1
	ldiw	R26,MODbars+MAXCH
	.endif
	ldiw	R30,MODpbck	;playback structure
	clrw	R12	;clear left/common sample
	.if	STEREO==1
	movw	R14,R12	;clear right sample
	.endif
	mov	R23,R20	;channels to mix
	;for each channel
MODplMPC:	ldd	R16,Z+0	;playback activity
	cpi	R16,0	;playback stopped?
	.if	DRAWVU==1
	brne	PC+1+1+1
	sbiw	R26,1	;previous VU channel
	rjmp	MODplMPSC	;yeah - skip channel
	.else
	rjeq	MODplMPSC
	.endif
	lddw	R6,Z+2	;load resampling coeff.
	lddd	R2,Z+4	;load sample data position
	lddt	R9,Z+8	;load sample data end
	
	addw	R2,R6
	adc	R4,R8
	adc	R5,R8	;add resample coeff
	cpt	R3,R9	;test for sample end
	brlo	MODplNE
	;end, test for loop
	ldd	R16,Z+14	;loop activity
	cpi	R16,0
	breq	MODplMPE
	;loop active - reset pointer
	lddt	R16,Z+11	;load offset (relative!)
	subt	R3,R16
	rjmp	MODplNE		
	;no loop
MODplMPE:	std	Z+0,R8	;stop playback!
	stdd	Z+4,R2	;store sample data position
	.if	DRAWVU==1
	sbiw	R26,1	;previous VU channel
	.endif
	rjmp	MODplMPSC	;skip rest of channel!		
MODplNE:	stdd	Z+4,R2	;store sample data position
	
	;now we have valid pointer in R3:R4:R5
	;read sample
	out	porta,R5
	cbi	pRHA,RHA
	sbi	pRHA,RHA
	out	porta,R4
	cbi	pRMA,RMA
	sbi	pRMA,RMA
	out	porta,R3
	.if	LINRES==1
	add	R3,R21
	adc	R4,R8
	adc	R5,R8	;pointer to next sample
	.if	HCOC==1
	nop
	.endif
	.else
	ldd	R18,Z+21	;load vol_left
	.if	STEREO==1
	ldd	R19,Z+22	;load vol_right
	.else
	.if	HCOC==1
	adiw	R30,0	;nop,nop
	.else
	nop
	.endif
	.endif
	.endif
	in	R16,pinc	;u1=u[t], where t=R3:R4:R5
		
	;read next sample	
	.if	LINRES==1
	out	porta,R5
	cbi	pRHA,RHA
	sbi	pRHA,RHA
	out	porta,R4
	cbi	pRMA,RMA
	sbi	pRMA,RMA
	out	porta,R3
	ldd	R18,Z+21	;load vol_left
	.if	STEREO==1
	ldd	R19,Z+22	;load vol_right
	.else
	.if	HCOC==1
	adiw	R30,0	;nop,nop
	.else
	nop
	.endif
	.endif
	in	R17,pinc	;u2=u[t+1], where t=R3:R4:R5
	
	;linear interpolation ;-D
	sub	R17,R16	;du=u2-u1
	brge	PC+1+1
	sub	R16,R2	;u1-=fraction if du<0, where fraction=R2 [fx 0.8]
	mul	R17,R2
	add	R16,R1	;u1+=(du*fraction)>>8
	.endif
		
	;apply volume
	.if	STEREO==1
	.if	MAMPLD==1
	fmulsu	R16,R18	;apply volume: R0:R1=2*u1*volume (max +16256/-16384), where volume=R20 [0-64]
	addw	R12,R0
	fmulsu	R16,R19	;apply volume: R0:R1=2*u1*volume (max +16256/-16384), where volume=R20 [0-64]
	addw	R14,R0
	.else
	mulsu	R16,R18	;apply volume: R0:R1=u1*volume (max +8128/-8192), where volume=R20 [0-64]
	addw	R12,R0
	mulsu	R16,R19	;apply volume: R0:R1=u1*volume (max +8128/-8192), where volume=R20 [0-64]
	addw	R14,R0
	.endif
	
	.else
	mulsu	R16,R18	;apply volume: R0:R1=u1*volume (max +8128/-8192), where volume=R20 [0-64]
	.if	MAMPLD==0
	asrw	R0	;apply volume: R0:R1=u1*volume (max +4064/-4096), where volume=R20 [0-64]	
	.endif
	addw	R12,R0
	.endif
	
	.if	DRAWVU==1
	;peak detection
	ld	R17,-X
	cp	R16,R17
	brlt	PC+1+1
	st	X,R16
	.endif
	
MODplMPSC:	adiw	R30,MODpbckSize	;to next channel
	djnz	R23,MODplMPC
	
	;store sample
	.if	STEREO==1
	st	Y+,R15
	st	Y+,R14
	st	Y+,R13
	st	Y+,R12
	.else
	st	Y+,R13
	st	Y+,R12
	st	Y+,R13
	st	Y+,R12
	.endif
	;
	sbiw	R24,1
	rjne	MODplSmplL	;next sample in block?
	;
	cpi	R28,low(FIFO+FIFOsize)
	cpc	R29,R22
	brlo	PC+1+2
	ldiw	R28,FIFO	;reset FIFO write pointer if end reached
	;
	popw	R24	;restore samples_to_tick counter
	sbiw	R24,0
	rjne	MODplSmplBL	;next samples block?
	;no - go to tick
	stsw	FIFOwr,R28	;store FIFO write pointer

	;------ SAMPLE CYCLE END------
	;------ SAMPLES BLOCK END------
	;------ MOD PLAYBACK LOOP END ------
	rjmp	MODplLoop	;playback loop
	
	
	
	
	
	


;************************************************************************************************
;	MOD pattern decoder routines
;************************************************************************************************
;------------------------------------------------------------------------------------------------
;	Process MOD pattern row, returns T=1 if playback done
;------------------------------------------------------------------------------------------------
MODrPat:	clr	R5
	inc 	R5
	clrw	R6	;SRAM step=+1

	lds	R16,MODpln	;load line id
	cpi	R16,64
	brne	MODrPatNNL
	;new line
	lds	R16,MODpnln	;reset line counter
	sts	MODpln,R16	;store line counter
	stsi	MODpnln,0
	
	;new pattern
	incs	R16,MODpat	;next pattern
	lds	R17,MODpatn	;song len
	cp	R16,R17
	brlo	MODrPatNNP
	;all patterns played - stop flag
	set		;finish
	ret		;finish
MODrPatNNP:	;------ generate pattern data pointer for actual order and row ------
	lds	R16,MODpat	;order id
	rcall	MODgetOrd	;get order (pattern to play)
	lds	R17,MODpln	;initial row
	rcall	MODpat2adr	;get address for pattern+row
	stst	MODpatp,R18	;store address
	
	;------ main pattern reading ------
MODrPatNNL:	ldst	R2,MODpatp	;load note pointer (SRAM)
		
	ldiw	R28,MODpbck	;playback structure pointer
	lds	R25,MODchnn	;channels
	;for each channel
MODrPatNC:	rcall	MODrPatNt	;read note from pattern
		
	;check for note delay effect
	cpi	R23,0x0E
	brne	MODrPatNND
	mov	R16,R24
	andi	R16,0xF0
	cpi	R16,0xD0
	brne	MODrPatNND
	;note delay effect
	mov	R16,R24
	andi	R16,0x0F
	breq	MODrPatNND	;skip if zero delay
	;non-zero delay
	std	Y+meRT+1,R16	;store delay
	ldi	R16,0x0D
	std	Y+meRT+0,R16	;store effect code
	std	Y+meRT+2,R20	;store sample id value
	stdw	Y+meRT+3,R21	;store sample period
	std	Y+meRT+5,R26	;store volume command
	rjmp	MODrPatND	;skip note trigger
MODrPatNND:		
	;--- process note ---
	rcall	MODrPatPCNT
MODrPatND:

	;------ effects ------
	
	;--- stop vibrato if no vibrato effect done (0x04/0x06/'U') ---
	;--- and reset resampl. coeff. to central freq. ---
	cpi	R23,0x04
	breq	MODrPatNSV
	cpi	R23,0x06
	breq	MODrPatNSV
	cpi	R23,'U'
	breq	MODrPatNSV
	;yeah - stop it
		
	ldd	R16,Y+meVI+0
	bst	R16,0
	brtc	MODrPatNSV	;not running, leave
	;running - stop it
	cbr	R16,1<<0
	std	Y+meVI+0,R16
	;and restore resampl. coeff.
	lddw	R18,Y+16	;load central period
	;ldiw	R16,256*3546895/SREAL	;PAL/(2*SREAL) [fx 8.8]
	;rcall	div16u	;PAL/(2*SREAL)/T
	T2RC		;PAL/(2*SREAL)/T
	stdw	Y+2,R16	;store resampling coeff
MODrPatNSV:		
	
	
	;--- main effect code ---
	cpi	R23,0x00
	rjne	MODrPatNE0
	;0 - Arpeggio
	cpi	R24,0
	rjeq	MODrPatNE0	;leave if no parameter
	
	rcall	MODisS3M
	brts	MODrPatE0i3
	lddw	R21,Y+23	;load original period
	cpiwh	R21,R16,1712+1
	rjsh	MODrPatNE0	;leave if period higher than C-0
MODrPatE0i3:	
	ldd	R16,Y+meAR+0	;status byte
	bst	R16,0	;activity bit
	brts	MODrPatE0R	;skip if active
	;not running now
	std	Y+meAR+1,R7	;clear tone counter
	sbr	R16,1<<0	;active
	std	Y+meAR+0,R16	;store it
MODrPatE0R:	tst	R11
	breq	MODrPatENN
	;new note
	std	Y+meAR+1,R7	;clear tone counter
MODrPatENN:
	rcall	MODisS3M
	brtc	MODrPatE0M
	;S3M
	ldd	R21,Y+23	;note id
	cpi	R21,0
	rjeq	MODrPatNE0	;leave if not defined
	ldd	R20,Y+15	;instrument id
	cpi	R20,0
	rjeq	MODrPatNE0	;leave if not defined
		
	;2. tone
	mov	R16,R24
	swap	R16
	andi	R16,0x0F
	add	R21,R16	;2. tone note id
	brcc	PC+1+1
	ldi	R21,0xF0	;upper limit
	rcall	MODnt2per
	stdw	Y+meAR+2,R21	;store 2. tone period
	
	;3. tone
	mov	R16,R24
	andi	R16,0x0F
	ldd	R21,Y+23	;note id
	add	R21,R16	;2. tone note id
	brcc	PC+1+1
	ldi	R21,0xF0	;upper limit
	rcall	MODnt2per
	stdw	Y+meAR+4,R21	;store 2. tone period
	rjmp	MODrPatEDONE
	
	;MOD
MODrPatE0M:	;identify current note
	ldiw	R30,TMODarpHS*2
	ldi	R26,0	;0 note defaultly	
MODrPatEFL:	lpmw	R16,Z+	;read first note
	cpw	R16,R21	;compare period with table item
	brsh	MODrPatEFD	;note identified if table item same or higher than period   
	;continue
	inc	R26	;next note
	rjmp	MODrPatEFL
MODrPatEFD:	
	;calculate 1. tone
	mov	R17,R24
	swap	R17
	andi	R17,0x0F
	mov	R30,R26
	sub	R30,R17	;1. tone id
	brge	PC+1+1
	ldi	R30,0	;arpeggio out of valid range
	clr	R31
	lsl	R30
	subiw	R30,-TMODarpHS*2
	lpmw	R21,Z+	;read 1. tone period
	rcall	MODperFt
	stdw	Y+meAR+2,R21	;store 1. tone period
					
	;calculate 2. tone
	mov	R17,R24
	andi	R17,0x0F
	mov	R30,R26
	sub	R30,R17	;2. tone id
	brge	PC+1+1
	ldi	R30,0	;arpeggio out of valid range
	clr	R31
	lsl	R30
	subiw	R30,-TMODarpHS*2
	lpmw	R21,Z+	;read 2. tone period
	rcall	MODperFt
	stdw	Y+meAR+4,R21	;store 2. tone period
	rjmp	MODrPatEDONE

	;arpeggio not active - stop it
MODrPatNE0:	ldd	R16,Y+meAR+0
	bst	R16,0
	cbr	R16,1<<0	;stop
	std	Y+meAR+0,R16
	brtc	MODrPatNE0SFR
	;stop
	tst	R11
	brne	MODrPatNE0SFR	
	;no new note - restore last pariod
	;ldiw	R16,256*3546895/SREAL	;PAL/(2*SREAL) [fx 8.8]
	lddw	R18,Y+16
	;rcall	div16u	;PAL/(2*SREAL)/T
	T2RC		;PAL/(2*SREAL)/T
	stdw	Y+2,R16	;store resample coeff [fx 8.8] 	
MODrPatNE0SFR:	

	cpi	R23,0x01
	brne	MODrPatNE1
	;1 - Portamento up
	ldi	R16,1<<0|1<<1|0<<2	;portamento active, up
	std	Y+meTP+0,R16	;store it
	std	Y+meTP+1,R24	;store step
	rjmp	MODrPatEDONE

MODrPatNE1:	cpi	R23,0x02
	brne	MODrPatNE2
	;2 - Portamento down
	ldi	R16,1<<0|0<<1|0<<2	;portamento active, down
	std	Y+meTP+0,R16	;store it
	std	Y+meTP+1,R24	;store step
	rjmp	MODrPatEDONE

MODrPatNE2:	cpi	R23,0x03
	brne	MODrPatNE3
	;3 - Tone portamento
				
	cpi	R24,0
	brne	MODrPatE3N
	;no parameter
	ldd	R24,Y+meTP+1			
MODrPatE3N:	;some parameter specified
	std	Y+meTP+1,R24	;store slide step
	
	lddw	R21,Y+18	;final period
	lddw	R16,Y+16	;load channel period
	cpw	R16,R21	;compare with final period
	brne	MODrPatE3TSM
	;periods are same - nothing to do
	std	Y+meTP+0,R7	;stop effect	
	rjmp	MODrPatEDONE
MODrPatE3TSM:	brlo	MODrPatE3TLO
	;actual higher than final
	ldi	R16,1<<0|1<<1|1<<2
	std	Y+meTP+0,R16
	rjmp	MODrPatEDONE
	;actual lower than final
MODrPatE3TLO:	ldi	R16,1<<0|0<<1|1<<2
	std	Y+meTP+0,R16
	rjmp	MODrPatEDONE	
	

MODrPatNE3:	cpi	R23,0x04
	rjne	MODrPatNE4
	;4 - Vibrato
	rcall	MODrPatE4R
	rjmp	MODrPatEDONE
	
MODrPatE4R:	;lddw	R21,Y+16	;load last used period
	
	;check amplitude parameter
	mov	R18,R24
	andi	R18,0x0F
	brne	PC+1+1+1
	;missing
	ldd	R18,Y+meVI+2	;load last one
	rjmp	MODrPatE4map	
	;ok
	cpi	R23,0x04
	brne	PC+1+1+1
	lsl	R18
	lsl	R18	;*=4 if new parameter and vibrato
	std	Y+meVI+2,R18	;store it as current
MODrPatE4map:
	;check speed parameter
	mov	R19,R24
	swap	R19
	andi	R19,0x0F
	brne	PC+1+1
	;missing
	ldd	R19,Y+meVI+1	;load last one
	;ok
	std	Y+meVI+1,R19	;store it as current
	
	cpi	R18,0
	breq	MODrPatE4D
	cpi	R19,0
	breq	MODrPatE4D
	;continue if both parameters non zero
	ldd	R16,Y+meVI+0
	bst	R16,0	;activity bit
	sbr	R16,1<<0
	std	Y+meVI+0,R16	;vibrato active
	
	brts	MODrPatE4SRTG
	;first run - retrigger
	ldi	R16,0
	std	Y+meVI+3,R16	;store waveform position
MODrPatE4SRTG:	
MODrPatE4D:	ret
	
MODrPatNE4:	cpi	R23,5
	brne	MODrPatNE5
	;5 - Continue Tone-Portamento and volume slide (0x0A)
	
	;first volume slide
	rcall	MODrPatEAR
	
	;now do tone-portamento
MODrPatE5D:	ldi	R24,0	;no parameter
	ldi	R23,0x03	;tone-portamento
	rjmp	MODrPatNE2	;run it	
	


MODrPatNE5:	cpi	R23,0x06
	brne	MODrPatNE6
	;6 - continue vibrato and volume slide
	;first volume lisde
	rcall	MODrPatEAR

	;then vibrato
	ldi	R24,0	;no parameter
	rcall	MODrPatE4R
	rjmp	MODrPatEDONE
		

MODrPatNE6:	cpi	R23,0x07
	brne	MODrPatE7D
	;7 - tremolo
	
	;check amplitude
	mov	R18,R24
	andi	R18,0x0F
	brne	PC+1+1
	;not defined yet
	ldd	R18,Y+meTR+2	;get last amplitude
	std	Y+meTR+2,R18	;store as actual
	
	;check speed
	swap	R24
	andi	R24,0x0F
	brne	PC+1+1
	;not defined yet
	ldd	R24,Y+meTR+1	;get last speed
	std	Y+meTR+1,R24	;store as actual
	
	cpi	R18,0
	breq	MODrPatE7D
	cpi	R24,0
	breq	MODrPatE7D
	;both parameters defined
	ldd	R16,Y+meTR+0
	bst	R16,0
	sbr	R16,1<<0	;activate
	std	Y+meTR+0,R16
	brts	PC+1+1+2
	;wasn't active - retrigger
	ldi	R16,0
	std	Y+meTR+3,R16	;retrigger waveform
	;
	rjmp	MODrPatEDONE
	;stop tremolo
MODrPatE7D:	ldi	R16,0
	std	Y+meTR+0,R16	;not active
	
MODrPatNE7:	cpi	R23,0x08
	brne	MODrPatNE8
	;8 - set panning
	cpi	R24,128
	brlo	PC+1+1
	ldi	R24,128	;cut to <0;128>
	std	Y+20,R24	;set it
	rjmp	MODrPatEDONE
		

MODrPatNE8:	cpi	R23,0x09
	brne	MODrPatEN9
	;9 - Set sample offset
	cpi	R24,0
	brne	MODrPatE9YP	;skip if some argument
	;no argument, use last one
	ldd	R24,Y+mePS+0	;load it
	cpi	R24,0
	rjeq	MODrPatEDONE	;leave if 0 yet (indefined)
MODrPatE9YP:	std	Y+mePS+0,R24	;store actual arg. as last one
	
	ldd	R16,Y+0
	cpi	R16,0
	rjeq	MODrPatEDONE	;leave if no sample playing now
	ldd	R16,Y+15	;last instrument (actual)
	cpi	R16,0
	rjeq	MODrPatEDONE	;leave if no sample specified yet
	ldi	R17,MODsmplSize
	mul	R16,R17
	movw	R30,R0
	subiw	R30,-(MODsmpl-MODsmplSize)	;sample	
	
	lddt	R16,Z+1	;sample start
	add	R17,R24
	adc	R18,R7	;add offset*0x100
	lddt	R19,Z+4	;sample end
	cpt	R16,R19	;is offset in valid range?
	rjsh	MODrPatEDONE	;no - leave
	;ok, sample_begin+offset*0x100<sample_end
	lddw	R19,Y+2	;load resampling coeff 
	clr	R15	;0 fraction part
	subw	R15,R19
	sbc	R17,R7
	sbc	R18,R7	;-=resampling_coeff
	stdd	Y+4,R15	;store new sample position	
	rjmp	MODrPatEDONE
	
	

MODrPatEN9:	cpi	R23,0x0A
	brne	MODrPatENA
	;A - volume slide
	;effect pointer 
	rcall	MODrPatEAR
	rjmp	MODrPatEDONE
	

MODrPatEAR:	rcall	MODisS3M
	brtc	MODrPatEARmd
	;S3M - test for fine volume slides
	mov	R16,R24
	andi	R16,0x0F
	cpi	R16,0x0F
	brne	MODrPatEARs3p1
	;par==0x?F
	andi	R24,0xF0
	breq	PC+1+1+1
	;?!=0 - fine volume slide up
	swap	R24
	rjmp	MODrPatEEAr
	;par=0x0F - volume slide down
	neg	R16
	std	Y+meVS+1,R16	;store slide speed
	ldi	R16,0xFF
	std	Y+meVS+0,R16	;slide active
	ret
	;par!=0x?F
MODrPatEARs3p1:	mov	R16,R24
	andi	R16,0xF0
	cpi	R16,0xF0
	brne	MODrPatEARmd	;regular volume slide
	;par==0xF?
	andi	R24,0x0F
	breq	PC+1+1
	;?!=0 - fine volume slide down
	rjmp	MODrPatEEBr
	;par==0xF0 - volume slide up
	swap	R16
	std	Y+meVS+1,R16	;store slide speed
	ldi	R16,0xFF
	std	Y+meVS+0,R16	;slide active
	ret	
	;MOD
MODrPatEARmd:	mov	R16,R24
	andi	R16,0x0F
	breq	MODrPatEAX5
	;y!=0 -> volume down
	neg	R16
	std	Y+meVS+1,R16	;store slide speed
	ldi	R16,0xFF
	std	Y+meVS+0,R16	;slide active	
	ret
	;y==0 -> volume up
MODrPatEAX5:	mov	R16,R24
	swap	R16
	andi	R16,0x0F
	cpi	R16,0
	breq	MODrPatEAE	;skip if zero
	std	Y+meVS+1,R16	;store slide speed
	ldi	R16,0xFF
	std	Y+meVS+0,R16	;slide active
MODrPatEAE:	ret
	
	
MODrPatENA:	cpi	R23,0x0B
	brne	MODrPatENB
	;B - go to order
	lds	R16,MODpat
	inc	R16
	cp	R24,R16
	brlo	MODrPatEBN
	;allow only forward jumps
	lds	R16,MODpatn
	cp	R24,R16
	brsh	MODrPatEBN
	;allow only in range jumps
	dec	R24
	sts	MODpat,R24	;next order id-1
	stsi	MODpln,63	;last row
	stsi	MODpnln,0	;begin on first row
MODrPatEBN:	rjmp	MODrPatEDONE 
	
MODrPatENB:	cpi	R23,0x0C
	brne	MODrPatENC
	;C - set volume
	cpi	R24,64
	brlo	PC+1+1
	ldi	R24,64	;cut volume upto 64 because some composers using 0-64 as decadic value 
			;(in fact it's coded as hexa, valid volumes are {C00,C01,...,C3F,C40})
	std	Y+1,R24	;store volume
	rjmp	MODrPatEDONE

MODrPatENC:	cpi	R23,0x0D
	brne	MODrPatEND
	;D - pattern break
	stsi	MODpln,63	;last line
	cpi	R24,64
	rjsh	MODrPatEDONE
	sts	MODpnln,R24	;new line id		
	rjmp	MODrPatEDONE


MODrPatEND:	cpi	R23,0x0E
	rjne	MODrPatENE
	;E - Extended effects
	mov	R23,R24
	swap	R23
	andi	R23,0x0F	;subeffect code
	andi	R24,0x0F	;subeffect parameter
	
	cpi	R23,0x01
	brne	MODrPatEEN1
	;E1 - Fineslide up
	lddw	R18,Y+16	;load period
	sub	R18,R24
	sbc	R19,R7	;decrease period once
	cpiwh	R18,R16,MINT
	brsh	MODrPatEEN1O
	;to low - cut off
	ldiw	R18,MINT
MODrPatEEN1O:	stdw	Y+16,R18	;store period
	;ldiw	R16,256*3546895/SREAL	;PAL/(2*SREAL) [fx 8.8]
	;rcall	div16u	;PAL/(2*SREAL)/T
	T2RC		;PAL/(2*SREAL)/T
	stdw	Y+2,R16	;store new resample coeff
	rjmp	MODrPatEDONE
		
MODrPatEEN1:	cpi	R23,0x02
	brne	MODrPatEEN2
	;E2 - Fineslide down
	lddw	R18,Y+16	;load period
	add	R18,R24
	adc	R19,R7	;increase period once
	cpiwh	R18,R16,MAXT
	brlo	MODrPatEEN2O
	;to high - cut off
	ldiw	R18,MAXT
MODrPatEEN2O:	stdw	Y+16,R18	;store period
	;ldiw	R16,256*3546895/SREAL	;PAL/(2*SREAL) [fx 8.8]
	;rcall	div16u	;PAL/(2*SREAL)/T
	T2RC		;PAL/(2*SREAL)/T
	stdw	Y+2,R16	;store new resample coeff
	rjmp	MODrPatEDONE

MODrPatEEN2:	cpi	R23,0x09
	brne	MODrPatEEN9
	;E9 - Sample retrigger
	
	cpi	R24,0
	brne	PC+1+1
	ldi	R24,1	;1 if x==0
	std	Y+meRT+1,R24	;store retrigger counter
	std	Y+meRT+2,R24	;repeat value
	std	Y+meRT+0,R23	;store effect code
	rjmp	MODrPatEDONE
	
MODrPatEEN9:	cpi	R23,0x0A
	brne	MODrPatEENA
	;EA - Fine volume slide up
	rcall	MODrPatEEAr
	rjmp	MODrPatEDONE
	
MODrPatEEAr:	ldd	R16,Y+1	;load volume
	add	R16,R24	;increment volume once
	cpi	R16,64
	brlo	MODrPatEEAO
	;too high - cut off
	ldi	R16,64
MODrPatEEAO:	std	Y+1,R16	;store volume back
	ret

MODrPatEENA:	cpi	R23,0x0B
	brne	MODrPatEENB	
	;EB - Fine volume slide down
	rcall	MODrPatEEBr
	rjmp	MODrPatEDONE
	
MODrPatEEBr:	ldd	R16,Y+1	;load volume
	sub	R16,R24	;increment volume once
	brge	MODrPatEEBO
	;too low - cut off
	ldi	R16,0
MODrPatEEBO:	std	Y+1,R16	;store volume back	
	ret
	

MODrPatEENB:	cpi	R23,0x0C
	brne	MODrPatEENC
	;EC - Note cut off
			
	cpi	R24,0
	brne	MODrPatEEC
	;immeadiate cut off
	std	Y+1,R7	;0 volume
	rjmp	MODrPatEDONE
MODrPatEEC:	;delayed cut off
	std	Y+meRT+1,R24	;store retrigger counter
	std	Y+meRT+0,R23	;store effect code
	rjmp	MODrPatEDONE 
MODrPatEENC:	rjmp	MODrPatEDONE


MODrPatENE:	cpi	R23,0x0F
	brne	MODrPatENF
	;F - set speed/tempo
	cpi	R24,0
	breq	MODrPatEDONE	;invalid parameter - abort
	cpi	R24,32
	brsh	MODrPatEFT
	;set speed
	sts	MODspd,R24
	sbs	MODtimc,0	;speed modification flag
	rjmp	MODrPatEDONE
	;set tempo
MODrPatEFT:	sts	MODbpm,R24	;store beats per minute
	;recalculate tick divisor
	rcall	MODsetTickRt
	
	;ldit	R16,5*SREAL	;5*sample_rate(real one!)
	;mov	R19,R24
	;clrw	R20
	;lslw	R19	;2*bpm
	;rcall	div24u	;R16:R17=sample_rate/(0.4*bpm)
	;stsw	MODtdiv,R16	;store it
	;sbs	MODtimc,1	;tempo modification flag
	rjmp	MODrPatEDONE

MODrPatENF:	cpi	R23,'U'
	brne	MODrPatENU
	;'U' - S3M fine vibrato
	rcall	MODrPatE4R
	rjmp	MODrPatEDONE

MODrPatENU:	cpi	R23,'A'
	brne	MODrPatENsA
	;'A' - S3M speed
	sts	MODspd,R24
	sbs	MODtimc,0	;speed modification flag
	rjmp	MODrPatEDONE
	
MODrPatENsA:	cpi	R23,'T'
	brne	MODrPatENsT
	;'T' - S3M tempo
	cpi	R24,32
	brsh	MODrPatEsAd	;direct value
	;incremental
	bst	R24,0
	lds	R24,MODbpm	;get last one
	dec	R24	;--
	brtc	PC+1+1
	subi	R24,-2	;++
	brpl	PC+1+1
	ldi	R24,255	;max tempo
	cpi	R24,32
	brsh	PC+1+1
	ldi	R24,32	;min tempo
MODrPatEsAd:	rjmp	MODrPatEFT	;set contants
	
	
MODrPatENsT:	

	;effects done
MODrPatEDONE:	
	adiw	R28,MODpbckSize	;next channel 
	djnz	R25,MODrPatNC
	stst	MODpatp,R2	;store note pointer (SRAM)
	;
	incs	R16,MODpln	;line++
	clt		;not finish
	ret
	

;------------------------------------------------------------------------------------------------
;	Set MOD tick rate from bpm value
;------------------------------------------------------------------------------------------------
MODsetTickRt:	ldit	R16,(5*SREAL)<<3	;5*sample_rate [fx 12.4]
	lds	R19,MODbpm	;bpm
	clrw	R20
	rcall	div24u	;5*sample_rate/bpm [fx 12.4]
	mov	R19,R16
	andi	R19,0x0F
	lsrt	R16
	lsrw	R16
	lsrw	R16
	lsrw	R16	
	stsw	MODtdiv,R16	;samples/tick (full part)
	sts	MODtdivf,R19	;samples/tick (fractional part) <0;15>
	ret
	

;------------------------------------------------------------------------------------------------
;	Get MOD tracker order (pattern to play) 
;	inputs:
;	 R16 - order id
;	outputs:
;	 R16 - pattern id
;------------------------------------------------------------------------------------------------
MODgetOrd:	ldsw	R2,MODordpos
	clr	R4
	add	R2,R16
	adc	R3,R4
	rcall	RamGetByteA	;pattern_id
	ret
	

;------------------------------------------------------------------------------------------------
;	Get pattern+row address in SRAM
;	inputs:
;	 R16 - pattern id
;	 R17 - initial row
;	outputs:
;	 R18:R19:R20 - selected pattern+row address
;------------------------------------------------------------------------------------------------
MODpat2adr:	rcall	MODisS3M
	brts	MODpat2adrS3
	;MOD	
	lds	R18,MODchnn
	mul	R16,R18
	ldsw	R18,MODpatpos	;pattern_data
	clr	R20
	addw	R19,R0	;pattern_data+=4*64*channels*pattern_id		
	ret
	;S3M
MODpat2adrS3:	ldi	R18,3
	mul	R16,R18
	ldsw	R2,MODpatpos	;pattern_offsets_list
	addw	R2,R0
	clr	R4	;pattern_offsets_list+=3*pattern_id
	rcall	RamGetByteA
	mov	R18,R16
	rcall	RamGetByteA
	mov	R19,R16
	rcall	RamGetByteA
	mov	R20,R16	;pattern_data
	ret
	

;------------------------------------------------------------------------------------------------
;	Read note from MOD pattern
;	inputs:
;	 R2:R3:R4 - pattern data pointer
;	 R5:R6:R7 - SRAM step (fixed +1)
;	 Y - playback structure pointer for current channel
;	outputs:
;	 R20 - instrument id
;	 R21 - note id
;	 R23 - command
;	 R24 - command parameter
;	 R26 - volume command
;------------------------------------------------------------------------------------------------
	.equ	PATCOMPRESS=1	;pattern compression for S3M (0/1 - no/yes)
	;
MODrPatNt:	rcall	MODisS3M	;MOD/S3M
	brts	MODrPatNtS3M
	;--- MOD ---
	rcall	RamGetByteA
	mov	R20,R16
	andi	R20,0xF0
	mov	R22,R16
	andi	R22,0x0F	;period hi
	rcall	RamGetByteA
	mov	R21,R16	;period lo
	rcall	RamGetByteA
	mov	R23,R16
	andi	R23,0x0F	;effect id
	swap	R16
	andi	R16,0x0F
	or	R20,R16	;sample id
	rcall	RamGetByteA
	mov	R24,R16	;effect parameter
	ldi	R26,0	;no volume command
	;
	cpiwh	R21,R16,0
	breq	PC+1+2
	stdw	Y+23,R21	;store original period
	rcall	MODperFt	;fine-tune correction	
	ret
	
	;--- S3M ---
MODrPatNtS3M:	
	.if	PATCOMPRESS==1
	;set default return values
	ldi	R20,0	;no instrument code
	ldiw	R21,0	;no period
	ldi	R23,0	;no command
	ldi	R24,0	;no command parameter
	ldi	R26,0	;no volume command
		
	;check flags
	rcall	RamGetByteA
	cpi	R16,0
	brne	MODrPatNtS3
	;end of row - do nothing
	cpi	R25,1
	breq	PC+1+3	;last channel - move to next pattern item
	;not last channel - stay on current pattern item
	subt	R2,R5
	ret
	;valid item
MODrPatNtS3:	lds	R17,MODchnn
	sub	R17,R25	;current channel id
	mov	R15,R16
	andi	R16,0x1F	;pattern item channel id
	cp	R16,R17
	breq	PC+1+3+1
	;not equal - do nothing
	subt	R2,R5	;stay on current item
	ret
	
	;instrument+note?
	bst	R15,5
	brtc	MODrPatNtS3nni
	rcall	RamGetByteA
	mov	R20,R16	;instrument id
	andi	R20,0x7F
	bst	R16,7
	brtc	PC+1+1+1
	rcall	RamGetByteA
	mov	R21,R16	;note id
MODrPatNtS3nni:	
	;volume command?
	bst	R15,6
	brtc	PC+1+1+1
	rcall	RamGetByteA
	mov	R26,R16	;volume command
	
	;command+parameter?
	bst	R15,7
	brtc	MODrPatNtS3ncp
	rcall	RamGetByteA
	mov	R23,R16	;command
	andi	R23,0x7F
	bst	R16,7
	brtc	PC+1+1+1
	rcall	RamGetByteA
	mov	R24,R16	;command parameter
MODrPatNtS3ncp:
	;uncompressed version
	.else
	rcall	RamGetByteA
	mov	R21,R16	;note id
	rcall	RamGetByteA
	mov	R20,R16	;instrument id
	rcall	RamGetByteA
	mov	R26,R16	;volume command
	rcall	RamGetByteA
	mov	R23,R16	;command
	rcall	RamGetByteA
	mov	R24,R16	;command parameter
	.endif
	
		
	cpi	R21,0xFE
	brne	MODrPatNtS3Mnc
	;note cut
	std	Y+0,R7	;stop channel playback
	std	Y+1,R7	;volume=0
	ldiw	R21,0	;T=0
	ret
MODrPatNtS3Mnc:	cpi	R21,0x00
	brne	PC+1+2+1
	;no note
	ldiw	R21,0	;T=0
	ret
	;some note
	std	Y+23,R21	;store note id
	push	R20
	cpi	R20,0
	brne	PC+1+1	;new instrument specified
	;no instr defined, try to load last one (just for note to period conversion!)
	ldd	R20,Y+15	;last instrument
	rcall	MODnt2per	;convert note to period
	pop	R20
	ret
	
	
;------------------------------------------------------------------------------------------------
;	Convert note code to period (S3M)
;	inputs: 
;	 R20 - instrument
;	 R21 - note
;	outputs:
;	 R21:R22 - period
;------------------------------------------------------------------------------------------------
MODnt2per:	cpi	R20,0
	brne	PC+1+2+1
	;no instr specified
	ldiw	R21,0	;no period
	ret
	;some instr defined - convert note to period
	;get basic period T
	mov	R16,R21
	ldi	R17,12
	call	div8u	;R16=note/12, R15=note%12
	lsl	R15
	mov	R30,R15
	clr	R31
	subiw	R30,-TS3Moctper*2
	lpmw	R12,Z+	;T
	push	R16
		
	;get transpositions constant
	ldi	R16,MODsmplSize
	mul	R16,R20
	movw	R30,R0
	subiw	R30,-(MODsmpl-MODsmplSize)	;instr pointer
	lddw	R14,Z+11	;get constant: C4c=(8*8363)<<8/C4f [fx 8.8]

	;calc transposed period
	clr	R11
	mul	R12,R14
	movw	R16,R0
	mul	R13,R15
	movw	R18,R0
	mul	R13,R14
	addw	R17,R0
	adc	R19,R11
	mul	R12,R15
	addw	R17,R0
	adc	R19,R11	;C4c*T [24.8]		
	pop	R16	;octave oct
	cpi	R16,0
	breq	MODnt2perO0
	;oct>0
MODnt2perOs:	lsrt	R17
	dbnz	R16,MODnt2perOs	;T=R17:R18=(C4c*T)>>oct
MODnt2perO0:	mov	R21,R17
	mov	R22,R18	;transposed T
	ret
	

;------------------------------------------------------------------------------------------------	
;	Apply fine tune value to current period and instrument
;	inputs: 
;	 R20 - instrument
;	 R21:R22 - period
;	 Y - playback structure pointer for actual channel
;	outputs:
;	 R21:R22 - transposed period
;------------------------------------------------------------------------------------------------
MODperFt:	push	R20
	push	R23
	;
	cpiwh	R21,R16,0
	breq	MODperFtD	;skip if no period defined
	;
	cpi	R20,0
	brne	PC+1+1+1+1
	;no instrument specified
	ldd	R20,Y+15	;load last instrument
	cpi	R20,0
	breq	MODperFtD	;leave if no instrument defined yet	
	
	;get finetune constant
	ldi	R16,MODsmplSize
	mul	R16,R20
	movw	R30,R0
	subiw	R30,-(MODsmpl-MODsmplSize)	;instr pointer
	ldd	R20,Z+11+0
	ldd	R23,Z+11+1	;get constant: FTc=8287/freq(fine_tune) [fx 1.15]
	
	;calc transposed period
	clr	R11
	fmul	R21,R20
	movw	R16,R0
	fmul	R22,R23
	movw	R18,R0
	fmul	R22,R20
	addw	R17,R0
	adc	R19,R11
	fmul	R21,R23
	addw	R17,R0
	adc	R19,R11	;(FTc*T)<<1 [fx 16.16]
	lsl	R17
	adc	R18,R11
	adc	R19,R11	;round
	mov	R21,R18
	mov	R22,R19	;transposed T
	;	
MODperFtD:	pop	R23
	pop	R20
	ret
	

;------------------------------------------------------------------------------------------------
;	Process MOD pattern note (basic processing without most of effects),
;	can be called from pattern reader or retrigger effect (playback loop). 
;	inputs:
;	 R20 - sample id
;	 R21:R22 - sample period
;	 R23 - effect id
;	 Y - playback structure pointer for actual channel
;	outputs:
;	 R11 - non-zero if note on line
;------------------------------------------------------------------------------------------------
MODrPatPCNT:	clr	R15

	cpi	R20,0	;is there sample id?
	breq	MODrPatNN
	;note id non zero
	ldd	R16,Y+15
	std	Y+15,R20	;store sample id
	cp	R16,R20
	breq	PC+1+1
	dec	R15	;sample id changed
	;pointer to sample R20 
	ldi	R16,MODsmplSize
	mul	R16,R20
	movw	R30,R0
	subiw	R30,-(MODsmpl-MODsmplSize)	;sample	
	
	;reset volume
	ldd	R16,Z+0	;sample volume
	std	Y+1,R16	;store it
MODrPatNN:		
	;volume command
	cpi	R26,1
	brlo	MODrPatVCNC
	cpi	R26,64+1+1
	brsh	MODrPatVCV
	;set volume
	dec	R26	;volume <0;64>
	std	Y+1,R26	;set it
	rjmp	MODrPatVCNC
	;set panning
MODrPatVCV:	subi	R26,128	;panning <0;64>
	lsl	R26	;panning <0;128>
	std	Y+20,R26	;set it
MODrPatVCNC:	
	
	clr	R11	;no note on this line
	cp	R21,R11
	cpc	R22,R11	;is there sample period?
	rjeq	MODrPatNP
	;yeah, some period specified
	ldd	R20,Y+15	;load actual sample id
	cpi	R20,0	;not specified yet?
	brne	MODrPatNP1	
	;yeah - leave
	stdw	Y+16,R21	;store period
	rjmp	MODrPatNP
MODrPatNP1:	;some sample specified
	;pointer to sample R20
	ldi	R16,MODsmplSize
	mul	R16,R20
	movw	R30,R0
	subiw	R30,-(MODsmpl-MODsmplSize)	;sample
		
	;test for tone-portamento
	set
	tst	R15
	brne	MODrPatTPN3	;no tone-portamento if instrument if changed
	cpi	R23,0x03
	breq	MODrPatTP3
	cpi	R23,0x05
	breq	MODrPatTP3
MODrPatTPN3:	clt
MODrPatTP3:
	
	stdw	Y+18,R21	;store tone-port. final period
	
	;set note playback rate
	brtc	MODrPatTPN
	;tone-portamento active
	lddw	R18,Y+16	;load last period instead of new specified!!!
	rjmp	MODrPatTP3D		
MODrPatTPN:	stdw	Y+16,R21	;store new period
	mov	R18,R21
	mov	R19,R22	;new T	
MODrPatTP3D:	;ldiw	R16,256*3546895/SREAL	;PAL/(2*SREAL) [fx 8.8]
	;rcall	div16u	;PAL/(2*SREAL)/T
	T2RC		;PAL/(2*SREAL)/T
	stdw	Y+2,R16	;store resample coeff [fx 8.8] 	

	;set note parameters
	brtc	MODrPatPAS2
	ldd	R26,Y+0	;playback activity?
	cpi	R26,0x00
	brne	MODrPatPAS1
MODrPatPAS2:	lddt	R18,Z+1	;sample begin
	ldi	R26,0	;its fraction part (R26:R18:R19:R20 [fx 24.8])
	sub	R26,R16
	sbc	R18,R17
	sbc	R19,R11
	sbc	R20,R11	;sample_begin-=resample_coeff	
	std	Y+4+0,R26	;store its fraction part		
	stdt	Y+4+1,R18	;store full part
MODrPatPAS1:	lddt	R16,Z+4	;sample end
	stdt	Y+8,R16	;store it
	lddt	R16,Z+7	;sample repeat
	stdt	Y+11,R16	;store it
	ldd	R16,Z+10	;sample loop activity
	std	Y+14,R16	;store it
	;start playback
	ldi	R16,0xFF
	std	Y+0,R16	;playback activity
	inc	R11	;note on this line
MODrPatNP:	ret






;************************************************************************************************
;	Other routines
;************************************************************************************************
;------------------------------------------------------------------------------------------------
;	print MOD name to line 2 from Y pointed file data (only first sector) 
;------------------------------------------------------------------------------------------------
MODprnName:	ldiw	R30,MODname
	ldi	R17,20
	ldi	R16,' '
	st	Z+,R16	;clear string first
	dbnz	R17,PC-1-1
	;
	ldiw	R30,MODname
	ldi	R17,20
	ld	R16,Y+
	cpi	R16,0
	breq	PC+1+1+1+1	;name end reached
	st	Z+,R16	;store char
	dbnz	R17,PC-1-1-1-1-1
	;
	.if	LCDON!=0
	call	LCDposL2
	ldiw	R30,MODname
	ldi	R17,20
	call	LCDwrStrRam
	.endif
	ret
	
;------------------------------------------------------------------------------------------------
;	print MOD name to line 2 from external SRAM 
;------------------------------------------------------------------------------------------------
MODprnNameE:	;config ports/signals
	BtInh	1
	outi	ddra,0xFF	;outputs
	outi	portc,0xFF	;pull-up
	cbi	pROE,ROE	;SRAM OE=0

	ldiw	R30,MODname
	ldi	R17,20
	ldi	R16,' '
	st	Z+,R16	;clear string first
	dbnz	R17,PC-1-1
		
	;SRAM address steping (R5:R6:R7)
	clr	R5
	inc	R5
	clrw	R6	;+1
	;SRAM address (R2:R3:R4)=0
	clrt	R2
	
	ldiw	R30,MODname
	ldi	R17,20
	rcall	RamGetByteA
	cpi	R16,0
	breq	PC+1+1+1+1	;name end reached
	st	Z+,R16	;store char
	dbnz	R17,PC-1-1-1-1-1
	;
	.if	LCDON!=0
	call	LCDclrL1
	ldiw	R30,TCPmod*2
	call	LCDwrStrFlash
	
	call	LCDposL2
	ldiw	R30,MODname
	ldi	R17,20
	call	LCDwrStrRam
	.endif
	
	;config ports/signals
	outi	ddra,0x00	;inputs
	outi	portc,0x00	;hi-z
	sbi	pROE,ROE	;SRAM OE=1
	BtInh	0
	ret
	
	
	
	
	
	
	
;************************************************************************************************
;	MOD loader, initialization, etc.
;************************************************************************************************	
;------------------------------------------------------------------------------------------------
;	Load MOD file (identify, fill playback structures/variables. etc.),
;	returns T=1 if successfull	
;------------------------------------------------------------------------------------------------
	;------ magic constants/routines/macros ------
DMod4Chn:	.db	4,"M.K.","M!K!","M&K!","N.T."	;4chn, 31instr.
DMod8Chn:	.db	2,"CD81","OKTA"	;8chn, 31instr.
DMod16Cn:	.db	1,"16CN"	;16chn, 31instr.
DMod32Cn:	.db	1,"32CN"	;32chn, 31instr.
DModS3M:	.db	1,"S3MP"	;modified S3M
	;compare m.c.'s 
MODidentCSS:	lpm	R24,Z+
	clt
MODidentCSSL1:	lpmd	R20,Z+
	cpd	R16,R20
	brne	PC+1+1+1
	set
	rjmp	PC+1+2
	dbnz	R24,MODidentCSSL1
	ret

	;compare m.c.
	.macro	mCPmc3
	cpi	@0,@2
	ldi	@1,@3
	cpc	@0H,@1
	ldi	@1,@4
	cpc	@0B3,@1
	.endm
	;compare m.c.
	.macro	mCPmc2
	cpi	@0,@2
	ldi	@1,@3
	cpc	@0H,@1
	.endm
	
	
	;====== MAIN ROUTINE ======
MODload:	;config ports/signals
	outi	ddra,0xFF	;outputs
	outi	portc,0xFF	;pull-up
	cbi	pROE,ROE	;SRAM OE=0
	
	;initialize variables first 
	rcall	MOdinitVars	
		
	;SRAM address steping (R5:R6:R7)
	clr	R5
	inc	R5
	clrw	R6	;+1
	
	;SRAM address (R2:R3:R4)=0
	clrt	R2
	
	;------ get song title ------
	;clear string
	ldiw	R30,MODname
	ldi	R16,20
	ldi	R17,' '
LoadMODnC:	st	Z+,R17
	dbnz	R16,LoadMODnC
	;read name 
	ldiw	R30,MODname
	ldi	R17,20	
LoadMODnL:	rcall	RamGetByteA
	cpi	R16,0
	breq	LoadMODeN
	st	Z+,R16
	dbnz	R17,LoadMODnL
LoadMODeN:	
	;print song title
	ldiw	R30,TModN*2
	call	UsTxStrFlash
	ldiw	R30,MODname
	ldi	R16,20
	call	UsTxStrRam
	ldi	R16,0x0D
	call	UsTxChar
	
	.if	LCDON!=0
	call	LCDclrL1
	ldiw	R30,MODname
	ldi	R17,20
	call	LCDwrStrRam
	.endif
	
	
	
	;------ test MOD format ------
	stsi	MODinsn,31	;instruments
	stsi	MODs3m,0	;ProTracker format
	stsiw	MODpatpos,20+31*30+1+1+128+4
	;get magic constant
	ldiw	R16,1080
	movw	R2,R16
	clr	R4
	rcall	RamGetByteA
	push	R16
	rcall	RamGetByteA
	mov	R17,R16
	rcall	RamGetByteA
	mov	R18,R16
	rcall	RamGetByteA
	mov	R19,R16
	pop	R16
	;	
	ldiw	R30,DMod4Chn*2
	rcall	MODidentCSS
	brtc	LoadMODin4C
	;4 channel
	ldi	R30,4	;4chn
	rjmp	LoadMODid
		
LoadMODin4C:	ldiw	R30,DMod8Chn*2
	rcall	MODidentCSS
	brtc	LoadMODin8C
	;8 channel
	ldi	R30,8	;8chn
	rjmp	LoadMODid
		
LoadMODin8C:	mCPmc3	R16,R20,'F','L','T'
	breq	PC+1+5+1
	mCPmc3	R16,R20,'T','D','Z'
	brne	LoadMODinFLT
	cpi	R19,'4'
	brlo	LoadMODinFLT
	cpi	R19,'9'+1
	brsh	LoadMODinFLT
	;"FLTx", "TDZx", where x={4..9}
	mov	R30,R19
	subi	R30,'4'-4
	rjmp	LoadMODid
	
LoadMODinFLT:	mCPmc3	R17,R20,'C','H','N'
	brne	LoadMODinCHN
	cpi	R16,'4'
	brlo	LoadMODinCHN
	cpi	R16,'9'+1
	brsh	LoadMODinCHN
	;"xCHN", where x={4..9}
	mov	R30,R16
	subi	R30,'4'-4
	rjmp	LoadMODid

LoadMODinCHN:	mCPmc2	R18,R20,'C','H'
	brne	LoadMODinCH
	cpi	R16,'1'
	breq	PC+1+1+1
	cpi	R16,'2'
	brne	LoadMODin12CH
	cpi	R17,'0'
	brlo	LoadMODin12CH
	cpi	R17,'9'+1
	brsh	LoadMODin12CH
	;"xxCH", where xx={10..29}
LoadMODiy3CH:	subi	R16,'1'-1
	ldi	R18,10
	mul	R16,R18
	mov	R30,R0
	subi	R17,'0'
	add	R30,R17
	rjmp	LoadMODid
	
LoadMODin12CH:	cpi	R16,'3'
	brne	LoadMODinCH
	cpi	R17,'0'
	brlo	LoadMODinCH
	cpi	R17,'2'+1
	brlo	LoadMODiy3CH	;"xxCH", where xx={30..32}
	
LoadMODinCH:	ldiw	R30,DMod16Cn*2
	rcall	MODidentCSS
	brtc	LoadMODin16CN
	;16 channel
	ldi	R30,16	;16chn
	rjmp	LoadMODid
	
LoadMODin16CN:	ldiw	R30,DMod32Cn*2
	rcall	MODidentCSS
	brtc	LoadMODin32CN
	;32 channel
	ldi	R30,32	;32chn
	rjmp	LoadMODid
	
LoadMODin32CN:	ldiw	R30,DModS3M*2
	rcall	MODidentCSS
	brtc	LoadMODinS3M
	;S3M modification
	ldi	R16,20
	mov	R2,R16
	clr	R3
	rcall	RamGetByteA
	mov	R30,R16	;channels
	rcall	RamGetByteA
	sts	MODinsn,R16	;instruments used
	stsi	MODs3m,1	;S3M format
	rjmp	LoadMODid
	
	;not identified
LoadMODinS3M:	stsi	MODinsn,15	;instruments
	stsiw	MODpatpos,20+15*30+1+1+128
	ldi	R30,4
LoadMODid:	sts	MODchnn,R30
				
	
	cpi	R30,MAXCH+1
	brlo	LoadMODvalf
	;invalid format
LoadMODinvf:	sts	FileLen+0,R6
	sts	FileLen+1,R6
	sts	FileLen+2,R6		
	clt
	ret		;leave if not valid fmt
LoadMODvalf:

	;------ read S3M extended info ------
	rcall	MODisS3M	;S3M?
	brtc	LoadMODnes3m
	;yeah
		
	;load speed/tempo
	rcall	RamGetByteA
	sts	MODspd,R16	;get default speed
	rcall	RamGetByteA
	sts	MODbpm,R16	;get default tempo
	rcall	MODsetTickRt
	;mov	R19,R16
	;ldit	R16,5*SREAL	;5*sample_rate(real one!)
	;clrw	R20
	;lslw	R19	;2*bpm
	;rcall	div24u	;R16:R17=sample_rate/(0.4*bpm)
	;stsw	MODtdiv,R16	;store it
		
	;load panning presets
	ldiw	R28,MODpbck
	lds	R24,MODchnn
LoadMODs3mPPL:	rcall	RamGetByteA	;get pan
	std	Y+20,R16	;store it into playback structure
	adiw	R28,MODpbckSize	;next channel
	dbnz	R24,LoadMODs3mPPL
	
	;load C4 transposition constants
	ldiw	R28,MODsmpl
	lds	R24,MODinsn
LoadMODs3mC4L:	rcall	RamGetByteA
	std	Y+11+0,R16
	rcall	RamGetByteA
	std	Y+11+1,R16	;store constant
	adiw	R28,MODsmplSize
	dbnz	R24,LoadMODs3mC4L
	
	;store pattern offsets position
	stsw	MODpatpos,R2
	
LoadMODnes3m:	
	

	;------ read MOD orders ------
	;create address
	rcall	MODisS3M	;S3M?
	brtc	LoadMODnplS3M
	;S3M
	ldiw	R16,950
	movw	R2,R16	;orders
	rjmp	LoadMODplS3M
	;MOD
LoadMODnplS3M:	lds	R16,MODinsn
	ldi	R18,20
	mov	R2,R18
	clr	R3	;sample list offset
	ldi	R17,30	;sample list item len
	mul	R16,R17
	addw	R2,R0	;orders
LoadMODplS3M:
	
	;read song len (orders)
	rcall	RamGetByteA
	sts	MODpatn,R16
	
	ldi	R16,1
	add	R2,R16
	adc	R3,R4	;skip end jump item
	stsw	MODordpos,R2	;store order list offset
	
	;calc. real pattern count
	clr	R18	;lowest pattern ID
	ldi	R24,128
LoadMODpL1:	rcall	RamGetByteA	;get order
	cp	R16,R18
	brlo	PC+1+1
	;same or higher id - store as real pattern count
	mov	R18,R16
	cpi	R16,128
	rjsh	LoadMODinvf	;leave if number >= 128 
	dbnz	R24,LoadMODpL1
	inc	R18	;++ (addressing from 0)
	sts	MODpats,R18	;store it
	
	
	;------ read sample informations ------
	;samples data start (R11:R12:R13)
	rcall	MODisS3M	;T=S3M
	brtc	LoadMODnss3m	;S3M?
	;S3M
	ldiw	R16,1084
	movw	R2,R16	;goto 1084 (after magic constant)
	;
	rcall	RamGetByteA
	mov	R11,R16
	rcall	RamGetByteA
	mov	R12,R16
	rcall	RamGetByteA
	mov	R13,R16	;get sample data offset
	rjmp	LoadMODss3m
	;MOD
LoadMODnss3m:	ldi	R16,20
	mov	R2,R16
	clr	R3	;sample list offset
	;sample data offset
	ldsw	R11,MODpatpos
	mov	R13,R4
	lds	R16,MODpats	;patterns
	lds	R17,MODchnn	;channels
	mul	R16,R17
	addw	R12,R0	;add pattern len	
LoadMODss3m:		
		
	;for each sample
	ldiw	R30,MODsmpl	;sample structure pointer
	lds	R24,MODinsn
LoadMODsL:	ldi	R16,22
	add	R2,R16
	adc	R3,R4	;skip sample title
	;get sample len
	clr	R10
	brtc	PC+1+1+1
	rcall	RamGetByteA
	mov	R10,R16
	rcall	RamGetByteA
	mov	R9,R16
	rcall	RamGetByteA
	mov	R8,R16
	
	ldi	R16,2
	clr	R15
	cp	R8,R16
	cpc	R9,R4	
	cpc	R10,R4	;at least 2bytes?
	brsh	LoadMODlNS
	clrw	R8	;=0, not a sample (just some Amiga times garbage!)
	dec	R15	;invalid sample
LoadMODlNS:	brts	PC+1+3
	lslt	R8	;sample_len*=2
	addt	R8,R11	;sample_len+=sample_data_offset
	mov	R17,R11
	movw	R18,R12	;sample start address
	brts	PC+1+3
	subit	R17,-2	;skip first 2 bytes (R17:R18:R19 - start address)
	pusht	R8	;store new sample address 
	;finetune
	rcall	RamGetByteA	;finetune
	brts	LoadMODnft
	;MOD
	andi	R16,0x0F	;to unsigned
	lsl	R16
	movw	R0,R30
	mov	R30,R16
	clr	R31
	subiw	R30,-TMODfint*2
	lpmw	R26,Z+	;get fine-tune freq
	movw	R30,R0
	stdw	Z+11,R26	;store transposition const.
LoadMODnft:	;get volume
	rcall	RamGetByteA
	cpi	R16,64
	brlo	PC+1+1
	ldi	R16,64	;cut vol 0-64
	std	Z+0,R16	;store volume
	stdt	Z+1,R17	;store sample start address	
	;get repeat offset
	clr	R21
	brtc	PC+1+1+1
	rcall	RamGetByteA
	mov	R21,R16
	rcall	RamGetByteA
	mov	R20,R16
	rcall	RamGetByteA
	mov	R19,R16
	brts	PC+1+3
	lslt	R19	;sample_repeat*=2
	;get loop len
	clr	R18
	brtc	PC+1+1+1
	rcall	RamGetByteA
	mov	R18,R16
	rcall	RamGetByteA
	mov	R17,R16
	rcall	RamGetByteA
	clr	R14	;loop off (default)
	cpi	R16,2
	cpc	R17,R4
	cpc	R18,R4
	brlo	LoadMODrn
	;loop active
	dec	R14	;loop on
	brts	PC+1+3
	lslt	R16	;loop_len*=2
	stdt	Z+7,R16	;store loop len
	addt	R16,R19	;loop_len+=repeat_offset
	addt	R16,R11	;loop_len+=sample_data_offset
	subit	R16,-1	;sample len +=1
	movw	R8,R16
	mov	R10,R18	;rewrite sample end
LoadMODrn:	popt	R11	;new sample start address
	ldi	R16,1
	brtc	PC+1+3
	add	R11,R16
	adc	R12,R4
	adc	R13,R4	;new sample start ++ (for S3M)
	sub	R8,R16
	sbc	R9,R4
	sbc	R10,R4	;sample len -=1
	stdt	Z+4,R8	;store sample stop address
	std	Z+10,R14	;store loop activity (0x00/0xFF => loop off/on)
	
	tst	R15
	brne	PC+1+1	;skip if invalid sample
	rcall	MODcorSmpl	;sample data correction
	
	;lddt	R17,Z+7
	;mov	R16,R17
	;call	UsTxChar
	;mov	R16,R18
	;call	UsTxChar
	;mov	R16,R19
	;call	UsTxChar
		
	
	adiw	R30,MODsmplSize
	djnz	R24,LoadMODsL

	;config ports/signals
	outi	ddra,0x00	;inputs
	outi	portc,0x00	;hi-z
	sbi	pROE,ROE	;SRAM OE=1
	set
	ret
	
	
;------------------------------------------------------------------------------------------------
;	Is tracker S3M (T=1)?  
;------------------------------------------------------------------------------------------------
MODisS3M:	push	R16
	lds	R16,MODs3m
	bst	R16,0	;T=1 if S3M
	pop	R16
	ret
	

;------------------------------------------------------------------------------------------------
;	Init MOD decoder variables/structures
;------------------------------------------------------------------------------------------------
MODinitVars:	;reset playback structure/effects	
	ldiw	R30,MODpbck
	clr	R0
	ldi	R24,MAXCH
	mov	R25,R24
	inc	R25
MODipCPBCK:	;playback
	std	Z+0,R0	;stop playback
	std	Z+2+0,R0
	std	Z+2+1,R0	;0 resample coeff
	std	Z+15,R0	;no sample specified
	std	Y+23,R0	;no note id
	;pos
	std	Z+mePS+0,R0	;not specified
	;retrigger
	std	Z+meRT+0,R0	;activity
	;vibrato
	std	Z+meVI+0,R0	;activity
	std	Z+meVI+1,R0	;no speed
	std	Z+meVI+2,R0	;no amplitude
	;tremolo
	std	Z+meTR+0,R0	;activity
	std	Z+meTR+1,R0	;no speed
	std	Z+meTR+2,R0	;no amplitude
	;arpeggio
	std	Z+meAR+0,R0	;activity
	;portamento
	std	Z+meTP+0,R0
	ldiw	R16,856
	stdw	Z+meTP+2,R16
	;volume slide
	std	Z+meVS+0,R0
	;default pannings
	mov	R17,R25
	sub	R17,R24
	bst	R17,1
	ldi	R16,64+32
	brts	PC+1+1
	ldi	R16,64-32
	std	Z+20,R16
	adiw	R30,MODpbckSize
	dbnz	R24,MODipCPBCK

	;default speed/tempo
	stsi	MODspd,6
	stsi	MODbpm,125
	stsi	MODtimc,0

	;calculate tick divisor
	rcall	MODsetTickRt
	;ldit	R16,5*SREAL	;5*sample_rate(real one!)
	;lds	R19,MODbpm
	;clrw	R20
	;lslw	R19	;2*bpm
	;rcall	div24u	;R16:R17=sample_rate/(0.4*bpm)
	;stsw	MODtdiv,R16	;store it

	;init pattern reader vars
	stsi	MODpat,-1	;initial pattern (no pattern code)
	stsi	MODpln,64	;initial pattern line (new line code)
	stsi	MODpnln,0	;first line of new pattern (used for pattern break effect)
	
	;main MOD player init
	stsi	FIFOtwr,0	;0 free bytes in FIFO
	stsi	MODncnt,1
	stsi	MODtfcnt,0	;tick rate fraction caunter
	ret	
	
;------------------------------------------------------------------------------------------------
;	Sample loops corrections/ramps
;------------------------------------------------------------------------------------------------
MODcorSmpl:	BtInh	1
	pusht	R2
	;--- loop correction ---
	;if linear interpolation is enabled it's necessary to duplicate first sample of loop data
	;to address that lies just above last loop data sample (linear interpolation always
	;needs 1 aditional sample which can lie outside of valid data)
	ldd	R16,Z+10
	cpi	R16,0xFF
	brne	MODcorSmplNL
	;loop active, try to duplicate sample
	lddt	R2,Z+4	;sample data end address
	cp	R2,R6
	cpc	R3,R6
	lds	R17,SRAMsize
	cpc	R4,R17
	brsh	MODcorSmplLOR	;skip if outside of valid data
	
	;read first loop sample
	lddt	R18,Z+7	;loop len
	subt	R2,R18	;first sample of loop data
	rcall	RamGetByteA	;read it
	mov	R15,R16
	
	;write it above last loop sample
	sbi	pROE,ROE	;OE=1
	outi	ddrc,0xFF	;ouputs
	;
	lddt	R2,Z+4	;first sample above loop data
	mov	R16,R15
	rcall	RamPutByteA	;store it
	;
	outi	ddrc,0x00	;inputs
	outi	portc,0xFF	;pull-up
	cbi	pROE,ROE	;SRAM OE=0
			
MODcorSmplLOR:
MODcorSmplNL:		
	;--- ramp generation ---
	.equ	MRLEN=16	;ramp len in samples
	
	ldd	R16,Z+10
	cpi	R16,0xFF
	brne	MODcorSmplR1NL
	;loop active, ramp can be generated only before loop start
	lddt	R16,Z+4
	lddt	R19,Z+7
	subt	R16,R19	;loop start
	lddt	R19,Z+1	;sample start
	subt	R19,R16	;max ramp len
	rjmp	MODcorSmplR1L
	;loop inactive, ramp can be generated only before first half of sample
MODcorSmplR1NL:	lddt	R16,Z+1	;sample start
	lddt	R19,Z+4	;sample end
	subt	R19,R16
	lsrt	R19	;max ramp len	
MODcorSmplR1L:	;
	cpith	R19,R16,MRLEN
	rjlo	MODcorSmplR1NC
	;can do start ramp
	ldiw	R16,255<<8
	ldi	R18,MRLEN
	clr	R19
	rcall	div16u
	movw	R14,R16	;volume step [fx 8.8]
	clrw	R18	;volume [fx 8.8]
	;
	lddt	R2,Z+1
	lds	R17,SRAMsize
	ldi	R23,MRLEN
MODcorSmplR1rL:	rcall	MODcorSRamp
	addw	R18,R14	;volume step
	dbnz	R23,MODcorSmplR1rL
	;
	ldd	R16,Z+10
	cpi	R16,0xFF
	breq	MODcorSmplR1NC
	;loop inactive - end ramp allowed
	lddt	R2,Z+4
	ldiw	R16,MRLEN
	subw	R2,R16
	sbc	R4,R6
	;
	ldiw	R18,255<<8
	lds	R17,SRAMsize
	ldi	R23,MRLEN
MODcorSmplR2rL:	rcall	MODcorSRamp
	subw	R18,R14	;volume step
	dbnz	R23,MODcorSmplR2rL
	;
MODcorSmplR1NC:	popt	R2
	BtInh	0
	ret
	;ramp generator - signed sample volume R19={0..255}  
MODcorSRamp:	cp	R2,R6
	cpc	R3,R6
	cpc	R4,R17
	brsh	MODcorSRampS	;skip if invalid SRAM address
	;
	clr	R5
	rcall	RamGetByteA
	inc	R5
	;	
	sbi	pROE,ROE	;OE=1
	outi	ddrc,0xFF	;ouputs
	;
	mulsu	R16,R19	;apply volume
	mov	R16,R1
	rcall	RamPutByteA	;store sample
	;
	outi	ddrc,0x00	;inputs
	outi	portc,0xFF	;pull-up
	cbi	pROE,ROE	;SRAM OE=0
MODcorSRampS:	ret	
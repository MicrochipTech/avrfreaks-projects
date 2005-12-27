; Hardware setup
;---------------

.org $0000
rjmp setup


;------------------------------------------------------------------------
;Setup routine.
;--------------
; Set al hardware to the correct modus (ports, timpers, registers, ...)

setup:	

.Dseg	
Table: 	.BYTE 120			;Reserve memory space for sampledata (120 bytes)

.cseg
	.device AT90S2313
	.include "2313def.inc"

	.def firstvalue = R1	;Used in calibration routine
	.def secondvalue = R2	;Used in calibration routine

	.def temp1 = R16		;Define symbolix names for registers
	.def temp2 = R17		;Temporary variable for filter routine
	.def status = R18		;Uarts status
	.def offsetD3= R19		;Samle offset of rising edge on D3
	.def offsetD2 = R20		;Sample offset of rising edge on D2
	.def offsetdif = R21	;OffsetD2 - OffsetD3 or OffsetD3 - OffsetD2
	.def samples = R22		;Nr of samples the signal is stable
	.def halfsample = R23	;Half of sample
	.def cal = R24			;Calibration value
	.def wait = R25			;Register used for waitstates
		
;	.def Xlow = R26			;X low byte
;	.def Xhi  = R27			;X high byte

	
	
	ldi temp1, 0b00011111	;Set PORTB
	out ddrb, temp1
	ldi temp1, 0b00011111
	out portb, temp1

	ldi temp1, 0b00000010	;Set PORTD
	out ddrd, temp1
	ldi temp1, 0b01110000
	out portd, temp1

	ldi temp1, 0b00001000	;Set up serial port to transmit only
	out ucr, temp1
	ldi temp1, 0b01000000	;10Mhz: UBRR=60 for 9600baud
	out UBRR, temp1

	ldi temp1, $DE			;Init stack pointer (Last byte in ram reserved for other purposes)
	out spl, temp1


	

;-------------------------------------------------------------------------

;Main program loop
;-----------------
;The byte at $DF contains the code to determine the type af sample we have (see documentation) 
;First we need to calibrate the measurement.
;1. Sample the data
;2. Clean the data up (remove all unwanted signals)
;3. (Debug step) Dump the meory contents to the PC to check calculations
;4. Count the number of samples the signal stays constant (frequency calibration). This routine
;	also store the offset value for PD3.
;5. Calculate the halfsample value. Valus is used to see if the calibration value has to
;   be added or subtracted from the offset to calibrate the signals for a 45° phaseshift
;6. Since we already know the sampleoffset for PD3, we can now check for the next edge on PD2
;	and store the offset value in offsetD2


main:

rcall 	sample				;Sample the data on portd
rcall	cleanup				;Clean out all unwanted signals
;rcall 	dumpmemory			;Send the contents of the SRAM to the serial port
rcall	countsamples		;Count the nr of samples that are '1' in a row on PD3
rcall	calchalfsample;		;Calculate the 'halfsample' value
rcall	checksampletype		;Check witch kind of sample we have and store the type in $DF
							;This routine only check for ABC, D, E or FGH
rcall	calcoffsetD2		;We will now calculate the sample offset for D2
rcall	calcoffsetdif		;calculate the offset difference between PD3 and PD2 and further
							;define the sampletype
rcall	calccal				;calculate the CAL value.
;rcall	dumpcal				;dum the calculations to the serial port


loop:

rcall 	sample				;sample the data on port D
rcall 	cleanup				;Clean the memory
rcall	countsamples		;Calulate offsetD3
rcall	calchalfsample		;Calculate halfsample value
rcall	checksampletype		;Check witch kind of sample we have and store the type in $DF
							;This routine only check for ABC, D, E or FGH
rcall	calcoffsetD2		;We will now calculate the sample offset for D2
rcall	calcoffsetdif		;calculate the offset difference between PD3 and PD2 and further
							;define the sampletype
rcall	shiftphase			;calculate the virtual phase shift
;rcall	dumpmeasurement		;display measurement
;rcall	setleds
rcall 	filter

rjmp	loop


;-------------------------------------------------------------------------
;-------------------------------------------------------------------------

;Sampling routine (tested ok).
;-----------------
; RAM space starts at $0060, so make sure X register is load 
; with $0060 before you start the sampling routine

;R27 is X hibyte, R26 is X lobyte

sample:
		clr XH					;Clear the hi byte
		ldi XL,$60				;Load location of first RAM space into X lobyte
	sampleloop:
		in temp1, pind 			;Load portd into temp register
		st x+, temp1			;Store temp register in memory using x register as index and post increment
		cpi	XL,$D7				;Compare X lobyte with last sample adress
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		brne sampleloop			;If last sample not reached, continue sampling
		ret						;Return from subroutine

;-------------------------------------------------------------------------
;Send data to the serial port (tested ok)
;----------------------------
; Dump the complete memory contents to the pc. To indicate the end of the transmission, we will send FF 

dumpmemory:
		clr XH					;Clear the hi byte
		ldi XL,$60				;Load location of first RAM space into X lobyte
	nextnumber:
		ld temp1, x+			;Load a sample from datamemory to temp register with X as index and post increment
		out UDR, temp1			;Put temp register in uart
		rcall RS232wait
		cpi XL, $D7				;Check if last sample has been reached
		brne nextnumber			;If not, send next character
		ldi temp1, $ff			;Load temp register with $FF
		out udr, temp1			;Just send the $FF
		rcall RS232wait
		ret						;Return from subroutine

;-------------------------------------------------------------------------
;Clean up the mory so that only the two relevant signals are contained in the memory (tested ok)
;-----------------------------------------------------------------------------------

cleanup:
		clr XH					;Clear the hi byte
		ldi XL,$60				;Load location of first RAM space into X lobyte
	cleanuploop:
		ld temp1, X				;Load portd into temp register
		andi temp1,0b00001100
		st x+, temp1			;Store temp register in memory using x register as index and post increment
		cpi	XL,$D7				;Compare X lobyte with last sample adress
		brne cleanuploop		;If last sample not reached, continue sampling
		ret						;Return from subroutine


;-------------------------------------------------------------------------
;Wait for the serial port to become ready to send. (tested ok, but work with other delays!)
;-------------------------------------------------
	
RS232wait:
		in status, USR			;Load USR in status register.
		cpi status, 0b01100000	;Check if the uart has sent the previous number
		brne rs232wait			;If not, do a loop and check again.
		
		ldi wait, $00			;Next is a simple waitloop. 
					
	waitloop1:
		inc wait
		cpi wait, $ff
		brne waitloop1
	
	waitloop2:
		inc wait
		cpi wait, $ff
		brne waitloop2

	waitloop3:
		inc wait
		cpi wait, $ff
		brne waitloop3

	waitloop4:
		inc wait
		cpi wait, $ff
		brne waitloop4

	waitloop5:
		inc wait
		cpi wait, $ff
		brne waitloop5

	waitloop6:
		inc wait
		cpi wait, $ff
		brne waitloop6
	
	waitloop7:
		inc wait
		cpi wait, $ff
		brne waitloop7

	waitloop8:
		inc wait
		cpi wait, $ff
		brne waitloop8

	waitloop9:
		inc wait
		cpi wait, $ff
		brne waitloop9

	waitloop10:
		inc wait
		cpi wait, $ff
		brne waitloop10
	
	ret

;-------------------------------------------------------------------------
;Count the nr of samples that remain 1 on PD3 (tested ok)
;--------------------------------------------
;First we check if the first bit is 1. If it is 1, we will wait until the signal goes to 0 and
;back to 1. When it goes back to 1, we will also store the offsetD3 register.!

countsamples:					;Just clear the nr of samples and set X index to point to first sample

		ldi samples, $00		;Clear the number of samples (R22)
		clr XH					;Clear the hi byte
		ldi XL,$60				;Load location of first RAM space into X lobyte
	
	countsamples1:				;Wait until PD3 is 0. If the first bit is 0, the step is skipped
		ld temp1, x+			;Load temp register with a byte (R16)
		andi temp1, 0b00001000	;Filter out PD3
		cpi temp1, 0b00001000	;Check if this sample is '1'
		breq countsamples1		;If it is 1, then wait for the next sample
		dec xl					;Decrease X with 1. if not, we will loose one sample (not very important)

	countsamples2:				;Do this loop until you reach the first '1'
		ld temp1, x+			;Load sample and post increment X (R16)
		andi temp1,0b0001000	;Filter out PD3 only
		cpi temp1, 0b0000000	;Check if current sample is '0'
		breq countsamples2		;If it is '0', then wait for next sample
		dec xl					;decrease X so as not to loose a sample
		mov offsetd3, XL		;Store the offset for the first rising edge on D3 (offsetD3=R19)

	countsamples3:				;Here we count the nr of samples that remain '1'
		ld temp1, x+			;Load sample (R16)
		andi temp1,0b0001000	;Filter out PD3 only
		cpi temp1, 0b0001000	;Check if current sample is '1'
		brne countsamples4		;If sample is not '1', we've reached the last sample. 
		inc samples				;Increase the number of samples
		rjmp countsamples3
	
	countsamples4:
		ret

;-------------------------------------------------------------------------
;Calculate the 'halfsample' value (tested ok)
;--------------------------------

calchalfsample:

		mov halfsample, samples	;Load the nr of samples in the 'halfsample' register		
		lsr halfsample			;divide halfsample by 2 (Logical shift right)

ret

;-------------------------------------------------------------------------
;Check the type of signal we sampled (tested ok)
;-----------------------------------

;First check for sampletype D or E (easiest thing to do)
;Next check for ABC
;then check for FGH

Checksampletype:
		
	checkD:						;Check for type D sample (signals in counterfase) - Works ok
		clr XH					;Clear the hi byte
		mov XL,offsetd3			;Copy the sampleoffset for PD3 in the XL register
		dec XL					;Decrease XL register so as to get the sample before the 0->1 transistion
		ld temp1, X+			;Load sample
		cpi temp1, $04			;Check if the sample before the rising edge equals $04
		brne checkE				;If not, check for the next type of sample
		ld temp1, X				;Load the sample after the first rising edge
		cpi temp1, $08			;Check if it is $08
		brne checkE				;if not, check fot the next type
		ldi temp1, $08			;Sample type D has code $08 (0b00001000)
		sts $00DF, temp1		;Store the sample type in $DF (last byte of sram space)
		ret

	checkE:						;Check for type E sample (signals in phase) - Works OK
		clr XH					;Clear the hi byte
		mov XL,offsetd3			;Copy the sampleoffset for PD3 in the XL register
		dec XL					;Decrease XL register so as to get the sample before the 0->1 transistion
		ld temp1, X+			;Load sample
		cpi temp1, $00			;Check if the sample before the rising edge equals $00
		brne checkABC			;If not, check for the next type of sample
		ld temp1, X				;Load the sample after the first rising edge
		cpi temp1, $0c			;Check if it is $0C
		brne checkABC			;if not, check fot the next type
		ldi temp1, $10			;Sample type D has code $10 (0b00010000)
		sts $00DF, temp1		;Store the sample type in $DF (last byte of sram space)
		ret

	checkABC:
		clr XH					;Clear the hi byte
		mov XL,offsetd3			;Copy the sampleoffset for PD3 in the XL register
		dec XL					;Decrease XL register so as to get the sample before the 0->1 transistion
		ld temp1, X+			;Load sample
		cpi temp1, $00			;Check if the sample before the rising edge equals $00
		brne checkFGH			;If not, check for the next type of sample
		ld temp1, X				;Load the sample after the first rising edge
		cpi temp1, $08			;Check if it is $08
		brne checkFGH			;if not, check fot the next type
		ldi temp1, $07			;Sample type ABC has code $07 (0b00000111)
		sts $00DF, temp1		;Store the sample type in $DF (last byte of sram space)
		ret

	checkFGH:
		clr XH					;Clear the hi byte
		mov XL,offsetd3			;Copy the sampleoffset for PD3 in the XL register
		dec XL					;Decrease XL register so as to get the sample before the 0->1 transistion
		ld temp1, X+			;Load sample
		cpi temp1, $04			;Check if the sample before the rising edge equals $00
		brne checkerror			;If not, something went wrong and we need to restart
		ld temp1, X				;Load the sample after the first rising edge
		cpi temp1, $0C			;Check if it is $08
		brne checkerror			;If not, something went wrong and we need to restart
		ldi temp1, $E0			;Sample type ABC has code $E0 (0b11100000)
		sts $00DF, temp1		;Store the sample type in $DF (last byte of sram space)
		ret

	checkerror:
		rjmp	main

;-------------------------------------------------------------------------
;Here we will calculate the sampleoffset for PD2. (tested ok)
;We only need to determine the sample offset for sample types ABC and FGH.
;Sampleoffset for type E is the same as D3, and for type D it is the same 
;as offsetD3 + sample size.


calcoffsetD2:						;First we will see what sampletype we have to count accordingly
	
		lds temp1, $00DF			;Load the code of the sampletype
		cpi temp1, $08				;check if we have a type D sample
		breq calcoffsetD
		cpi temp1, $10				;check if we have a type E sample
		breq calcoffsetE
		cpi temp1, $07				;check if we have a type ABC sample
		breq calcoffsetABC
		cpi temp1, $E0				;check if we have a type FGH sample
		breq calcoffsetFGH
		rjmp main					;If nothing adds up, restart because something went wrong

	calcoffsetD:
		mov offsetD2, offsetD3		;OffsetD2 = OffsetD3
		add offsetd2, samples		;increase offset with the nr of samples
		ret

	calcoffsetE:
		mov	offsetD2, offsetD3		;OffsetD2 = OffsetD3
		ret

	calcoffsetABC:
		clr XH						;Clear the hi byte
		mov XL,offsetD3				;Load location of D3 first '1' byte
		mov offsetd2, offsetD3		;Copy the offsetD3 register to the offsetD2 register
					
		calcoffsetABC1:				;Do this loop until you reach the first '1'	
			ld temp1, x+			;Load sample and post increment X
			andi temp1,0b0000100	;Filter out PD2 only
			cpi temp1, 0b0000000	;Check if current sample is '0'
			brne calcoffsetend		;If it is not '0', we've reached the last sample
			inc offsetD2			;increase the offsetD2 register
			rjmp calcoffsetABC1		;Go to the next sample

		
	calcoffsetFGH:
		clr XH						;Clear the hi byte
		mov XL,offsetD3				;Load location of D3 first '1' byte
		mov offsetd2, offsetD3		;Copy the offsetD3 register to the offsetD2 register
			
		calcoffsetFGH1:				;Do this loop until you reach the first '0'	
			ld temp1, -x			;Load sample and predecrement X
			andi temp1,0b0000100	;Filter out PD2 only
			cpi temp1, 0b0000100	;Check if current sample is '0'
			brne calcoffsetend		;If it is not '1', we've reached the first sample of D2
			dec offsetD2			;decrease the offsetD2 register
			rjmp calcoffsetFGH1		;Go to the next sample	
					
	calcoffsetend:
		ret	

;-------------------------------------------------------------------------
;calculate the offsetdifference bewteen PD3 and PD2. We have 4 type of signals
;to check so far: type ABC, type D, type E and type FGH. 
;this routine also defines further witch exact type of signal we have

;tested ok: sample type A,B,C,D,E,F,G,H

calcoffsetdif:
		
		lds temp1, $00DF		;Load the code of the sampletype
		cpi temp1, $08			;check if we have a type D sample
		breq calcD
		cpi temp1, $10			;check if we have a type E sample
		breq calcE
		cpi temp1, $07			;check if we have a type ABC sample
		breq calcABC
		cpi temp1, $E0			;check if we have a type FGH sample
		breq calcFGH
		rjmp main				;If nothing adds up, restart because something went wrong

	calcD:	
		mov offsetdif, samples	;the offsetdifference is the same as the number of samples
		ret

	calcE:
		ldi offsetdif, $00		;the samples are in phase, so no offset difference
		ret


	calcABC:					;Calulate OffsetD2-OffsetD3=Offsetdif
		mov offsetdif, offsetd2	
		sub offsetdif, offsetd3
		cp offsetdif, halfsample	;Check if the offsetdif is smaller than halfsample (type A)
		brlo definetypeA		
		cp halfsample, offsetdif	;Check if halfsample is smaller than the offsetdif (type C)
		brlo definetypeC
		cp halfsample, offsetdif 	;check if halfsample equals the offsetdif (type B)
		breq definetypeB
		
	calcFGH:						;Calculate offsetD3-OffsetD2:Offsetdif
		mov offsetdif, offsetd3
		sub offsetdif, offsetd2
		cp halfsample, offsetdif	;Check if halfsample is smaller than the offsetdif (type F)
		brlo definetypeF
		cp offsetdif, halfsample  	;Check if the offsetdif is smaller than halfsample (type H)
		brlo definetypeH
		cp halfsample, offsetdif	;Check if halfsample equals the offsetdif (type G)
		breq definetypeG
		
	definetypeA:
		ldi temp1, $01			;Sample type ABC has code $01 (0b00000001)
		sts $00DF, temp1		;Store the sample type in $DF (last byte of sram space)
		ret
		
	definetypeB:
		ldi temp1, $02			;Sample type ABC has code $E0 (0b00000010)
		sts $00DF, temp1		;Store the sample type in $DF (last byte of sram space)
		ret

	definetypeC:
		ldi temp1, $04			;Sample type ABC has code $E0 (0b00000100)
		sts $00DF, temp1		;Store the sample type in $DF (last byte of sram space)
		ret

	definetypeF:
		ldi temp1, $20			;Sample type ABC has code $E0 (0b00100000)
		sts $00DF, temp1		;Store the sample type in $DF (last byte of sram space)
		ret

	definetypeG:
		ldi temp1, $40			;Sample type ABC has code $E0 (0b01000000)
		sts $00DF, temp1		;Store the sample type in $DF (last byte of sram space)	
		ret

	definetypeH:
		ldi temp1, $80			;Sample type ABC has code $E0 (0b10000000)
		sts $00DF, temp1		;Store the sample type in $DF (last byte of sram space)
		ret

;----------------------------------------------------------------
;Calculate the CAL value.
;------------------------

calccal:
		lds temp1, $00DF			;Load the code of the sampletype
		cpi temp1, $01				;check if we have a type A sample
		breq calA
		cpi temp1, $02				;check if we have a type B sample
		breq calB
		cpi temp1, $04				;check if we have a type C sample
		breq calC
		cpi temp1, $08				;check if we have a type D sample
		breq calD
		cpi temp1, $10				;check if we have a type E sample
		breq calE
		cpi temp1, $20				;check if we have a type F sample
		breq calF
		cpi temp1, $40				;check if we have a type G sample
		breq calG
		cpi temp1, $80				;check if we have a type H sample
		breq calH
		rjmp main					;If nothing adds up, restart because something went wrong

	calA:							;tested ok						
		mov cal, halfsample
		sub cal, offsetdif
		ret
	calB:							;tested ok
		ldi cal, $00				
		ret
	calC:							;tested ok
		mov cal, offsetdif
		sub cal, halfsample
		ret
	calD:							;tested ok
		mov cal, halfsample
		ret
	calE:							;tested ok
		mov cal, halfsample
		ret
	calF:							;tested ok
		mov cal, offsetdif
		add cal, halfsample
		ret
	calG:							;tested ok
		mov cal, samples
		ret
	calH:							;tested ok
		mov cal, offsetdif
		add cal, halfsample
		ret

;----------------------------------------------------------------
;Send calculations to the serial port
;------------------------------------
; To indicate the end of the transmission, we will send FF 

dumpcal:
		clr XH					;Clear the hi byte
		ldi XL,$FD				;Load location of sampletype in X register
		ld temp1, x				;Load sampletype
		out UDR, temp1			;Put temp register in uart
		rcall RS232wait
		out udr, cal
		rcall RS232wait
		out udr, samples
		rcall RS232wait
		out udr, halfsample
		rcall RS232wait
		out udr, offsetD2
		rcall RS232wait
		out udr, offsetd3
		rcall RS232wait
		out udr, offsetdif
		rcall RS232wait
		ret						;Return from subroutine

;----------------------------------------------------------------
;Shift the phase of PD2
;----------------------

shiftphase:

		lds temp1, $00DF			;Load the code of the sampletype
		cpi temp1, $01				;check if we have a type A sample
		breq shiftAEFGH
		cpi temp1, $02				;check if we have a type B sample
		breq shiftB
		cpi temp1, $04				;check if we have a type C sample
		breq shiftCD
		cpi temp1, $08				;check if we have a type D sample
		breq shiftCD
		cpi temp1, $10				;check if we have a type E sample
		breq shiftAEFGH
		cpi temp1, $20				;check if we have a type F sample
		breq shiftAEFGH
		cpi temp1, $40				;check if we have a type G sample
		breq shiftAEFGH
		cpi temp1, $80				;check if we have a type H sample
		breq shiftAEFGH
		rjmp main					;If nothing adds up, restart because something went wrong

	shiftAEFGH:
		add offsetd2, cal
		ret

	shiftB:
		ret

	shiftCD:
		sub offsetD2, cal
		ret

;----------------------------------------------------------------
;Dump real time measurements to the serial port.
;-----------------------------------------------

dumpmeasurement:
		;out udr, samples
		;rcall RS232wait
		mov offsetdif, offsetd2
		sub offsetdif, offsetd3
		out UDR, offsetdif
		rcall RS232wait
		ret
	
;----------------------------------------------------------------
;Set the output leds according to the measurement (unfiltered data).
;-------------------------------------------------------------------

setleds:

	cp halfsample, offsetdif	;Check if halfsample is smaller than offsetdif 
	brlo toohigh

	cp offsetdif, halfsample	;Check if offsetdif is smaller than halfsample 
	brlo toolow

	cp halfsample, offsetdif	;Check if halfsample is smaller than the offsetdif 
	breq ok

	toohigh:
		ldi temp1, 0b00011110	;Too high
		out portb, temp1
		ret
	
	ok:
		ldi temp1, 0b00011101	;ok
		out portb, temp1
		ret
	
	toolow:
		ldi temp1, 0b00011011	;too low
		out portb, temp1
		ret

;----------------------------------------------------------------
;Filter the measurements and dump filtered data after filter is ready
;--------------------------------------------------------------------

;load 200 measurements in the Z register, and after 255 measurements
;divide the Z register by 200.

filter:
		mov offsetdif, offsetd2
		sub offsetdif, offsetd3
		inc temp2
		adc YL, offsetdif
		brcs incYH
	checkfilterend:
		cpi temp2, $FF
		breq dumpfilter
		ret
	
	incYH:
		inc YH
		rjmp checkfilterend
		
	dumpfilter:
		mov YL, YH
		out udr, Yl
		rcall RS232wait
		rcall setledsfiltered
		clr temp2
		clr YH
		clr YL
		ret


;----------------------------------------------------------------
;Set the output leds according to the measurement (unfiltered data).
;-------------------------------------------------------------------

setledsfiltered:

	cp halfsample, YL		;Check if halfsample is smaller than YL
	brlo filteredtoohigh

	cp YL, halfsample		;Check if YL is smaller than halfsample 
	brlo filteredtoolow

	cp halfsample, YL		;Check if halfsample is smaller than the YL 
	breq filteredok

	filteredtoohigh:
		;DIR = PB.3, STEP = PB.4
		in temp1, pind
		andi temp1, 0b00100000
		cpi temp1, 0b00100000
		breq stepandledhi
		ldi temp1, 0b00011110	;Too high
		out portb, temp1
		ret
	stepandledhi:
		ldi temp1, 0b00000110
		out portb, temp1
		ldi temp1, 0b00010110		
		out portb, temp1
		ret

	
	filteredok:
		ldi temp1, 0b00011101	;ok
		out portb, temp1
		ret
	
	filteredtoolow:
		;DIR = PB.3, STEP = PB.4
		in temp1, pind
		andi temp1, 0b00100000
		cpi temp1, 0b00100000
		breq stepandledlo
		ldi temp1, 0b00011011	;too low
		out portb, temp1
		ret
	stepandledlo:
		ldi temp1, 0b00001110
		out portb, temp1
		ldi temp1, 0b00011110		
		out portb, temp1
		ret





	
			

	


	







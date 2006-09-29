; Timer stuff to provide interrupts at 10ms and 1sec
;  assuming 8MHz clocking
;  So count up, prescaler of /256
;  CTC mode using OCR1A and a count of 31,250 for a 1 sec period
;  and OCR3A set to 312 for the 10ms irq,
;
;	Note: the Wait_x10ms routine should not be called from within an IRQ, as it's not re-entrant


.equ tc1_ctrlA = 0x00		;disconnect the outputs
.equ tc1_ctrlB = 0x0C		;clock prescaler /256, CTC mode using OCR1A
.equ tc1_ctrlC = 0x00		;no need to mess with force-compares
.equ tc_imask = 0x10		;use output compare A, note that this should be OR'ed in

.equ tc3_ctrlA = 0x00		;disconnect the outputs
.equ tc3_ctrlB = 0x0C		;clock prescaler /256, CTC mode using OCR3A
.equ tc3_ctrlC = 0x00		;no need to mess with force-compares
.equ tc_etimask = 0x10		;use output compares A & B, note that this should be OR'ed in

.equ count_1sec = 31250		;count value for 1 second
.equ count_10ms = 312		;count value for 100ms

.equ Test_Chk_Interval = 5	;wait 5 seconds between test check calls

.dseg					;some sram variables
Second_Ctr:	.DB	0x00		;used by timer interrupts
Test_Period_Ctr:	.DB	0
Test_Chk_Flg:	.DB 0
VMon_Chk_Flg:	.db 0
Wait_Ctr:	.DB 0		;counter for 100ms wait intervals
Waiting_Flg:	.db 0	;flag to indicate we are in the wait routine

.cseg
InitTimer:
	in Temp, SREG			;save the status reg for a bit
	st	-Y, Temp
	cli						;and disable irqs so that word writes are safe

	ldi Temp, HIGH(count_1sec)
	out	OCR1AH, Temp
	ldi	Temp, LOW(count_1sec)
	out OCR1AL, Temp

	ldi	Temp, HIGH(count_10ms)
	sts OCR3AH, Temp
	ldi	Temp, LOW(count_10ms)
	sts OCR3AL, Temp

	ldi	Temp, tc1_ctrlA
	out TCCR1A, Temp
	ldi	Temp, tc1_ctrlB
	out TCCR1B, Temp
	in	Temp, TIMSK
	ori	Temp, tc_imask
	out	TIMSK, Temp

	ldi	Temp, tc3_ctrlA
	sts TCCR3A, Temp
	ldi	Temp, tc3_ctrlB
	sts TCCR3B, Temp
	lds	Temp, ETIMSK
	ori	Temp, tc_etimask
	sts	ETIMSK, Temp

	clr	Temp
	sts	Second_Ctr, Temp
	sts	Test_Period_Ctr, Temp
	sts	Test_Chk_Flg, Temp
	sts	VMon_Chk_Flg, Temp
	sts	Waiting_Flg, Temp

	ld	Temp, Y+
	out SREG, Temp			;restore the status reg (this may re-enable irqs)
	ret


IRQ_1sec:			;for now just keep track and call the test check ever Test_Chk_Interval
	st	-Y, Temp			;save what you need to
	in	Temp, SREG
	st	-Y, Temp

	lds	Temp, Test_Period_Ctr				;look at the period counter to see if it's time for a test check
	inc Temp
	sts	Test_Period_Ctr, Temp
	cpi	Temp, Test_Chk_Interval
	brne	IRQ_1sec_1

	clr	Temp
	sts	Test_Period_Ctr, Temp				;reset the interval counter
	lds	Temp, Test_Chk_Flg
	inc Temp								;read and incr the flag so that it can be used as a counter
											; to timeout serial wait loops
	sts	Test_Chk_Flg, Temp					;have to set the flag since it's way too nasty to do in an isr


IRQ_1sec_1:
	ld	Temp, Y+			;restore what you saved
	out	SREG, Temp
	ld	Temp, Y+

	reti

IRQ_10ms:  ;set the VMon_chk flag update all of the delay counters

	st	-Y, Temp			;save what you need to
	in	Temp, SREG
	st	-Y, Temp
	st	-Y, T2

	ldi	Temp, 1
	sts	VMon_Chk_Flg, Temp

	lds	Temp, Waiting_Flg		;if we are in the wait loop, do a wdr in this isr
	sbrs	Temp, 0
	rjmp	IRQ_10ms_1

	wdr

	lds	Temp, Wait_Ctr
	inc Temp
	sts	Wait_Ctr, Temp	;update the wait counter

IRQ_10ms_1:
	ld	T2, Y+
	ld	Temp, Y+
	out	SREG, Temp
	ld Temp, Y+
	reti

Wait_x10ms:		;wait for the number of 10ms intervals indicated in Temp
					; Note that IRQ's will happen, but no checking or other actions will, so be careful here
	st	-Y, T2
	mov	T2, Temp
	clr Temp
	sts	Wait_Ctr, Temp	;clear the wait counter

	inc Temp
	sts	Waiting_Flg, Temp	;set the waiting flag so that the 10ms isr will reset the WD

	inc T2			;increment the end point to ensure at least time delay

Wait_Lp:			;
	lds	Temp, Wait_Ctr
	cp	Temp, T2
	brlo	Wait_Lp

	clr Temp
	sts	Waiting_Flg, Temp	;clear the waiting flag to end the isr WDR's

	ld	T2, Y+
	ret


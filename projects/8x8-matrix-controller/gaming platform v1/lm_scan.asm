

; ----------------------------------------------
; LED MATRIX SCANNING SUBROUTINES
; ----------------------------------------------


; *********************************************************************************************************************************
; * interrupt service routine for timer counter overflow
; *********************************************************************************************************************************
; scan led matrix line by line
; cycles for a full scan: max 20062 - min 20039

scan_frame:
	push temp					;
	push data1					;
	push data2					; save register values
	push data3					;
	push data4					;

	clr temp					;
	out TCCR0B, temp			; stop timer counter clock
	sei							; re-enable interrupts

input_control:
    sbic btn_flags, 5           ; check if buttons are locked
	rjmp input_disabled			; if set, skip button checking
	rcall check_buttons			; activate, check and deactivate buttons
	rjmp input_control_end		; skip button-unlocking mechanism
input_disabled:
	inc unlock_cnt				; if buttons are locked, increase scan counter
	cpi unlock_cnt, btn_spd		; check if counter has reached the unlocking limit
	brlo input_control_end		; if not, proceed to next task without unlocking buttons
	cbi btn_flags, 5    		; clear lock-flag (unlock buttons)
	clr unlock_cnt				; reset scan counter (keeps time until unlocking buttons)
input_control_end:


	ldi	XH, SRAM_pri_fb_hi		; prepare X pointer for primary frame buffer
	ldi	XL, SRAM_pri_fb_lo		;

	ldi pb_data,0b00000001		; activate line 1 (topmost)
	rcall mibam
	clr pb_data
	ldi pb_data,0b00100000		; activate line 2
	rcall mibam
	clr pb_data
	ldi pd_data,0b00100000		; activate line 3
	rcall mibam
	clr pd_data
	ldi pb_data,0b00001000		; activate line 4
	rcall mibam
	clr pb_data
	ldi pd_data,0b00000001		; activate line 5
	rcall mibam
	clr pd_data
	ldi pd_data,0b00010000		; activate line 6
	rcall mibam
	clr pd_data
	ldi pd_data,0b00000010		; activate line 7
	rcall mibam
	clr pd_data
	ldi pd_data,0b00000100		; activate line 8 (lowermost)
	rcall mibam

	andi pd_data, 0b11111011	;
	out PORTD, pd_data			; turn this line off now!


	in temp, TIMSK				; compare match interrupts have to be disabled now
	andi temp, 0b11111000		; clear three lower bits
	ori temp, 0b00000010 		; set bit 1
	out TIMSK, temp				; enable overflow int

	ldi temp, refresh_speed		; set speed of matrix refresh
	out TCNT0, temp				; (next refresh on timer counter overflow interrupt)
	ldi temp, 0b00000101 		; set bit 1 and 2
	out TCCR0B, temp			; start timer counter clock (prescaller 1/1024)

    inc draw_cnt				; increase scan counter (keeps time until redrawing frame)

	pop data4					;
	pop data3					;
	pop data2					; restore register values
	pop data1					;
	pop temp					;
	reti

; *********************************************************************************************************************************
; * MyBAM babe... MIBAM!
; *********************************************************************************************************************************
; cycles for rcall + routine + ret: 3 + 2492 + 4 = 2499

mibam:
	ld	data1, X+				;
	ld	data2, X+				; read line data from frame buffer (4bytes)
	ld	data3, X+				;
	ld	data4, X+				;

	clr temp					;
	out TCNT0, temp				; reset timer counter
	ldi temp, 10				;
	out OCR0A, temp				; set initial compare match A value (5)
	ldi temp, 80				;
	out OCR0B, temp				; set initial compare match B value (80)

	in temp, TIMSK				;
	andi temp, 0b11111000		; clear three lower bits...
	ori temp, 0b00000001 		; set bit 0
	out TIMSK, temp				; enable compare match A int
	ldi temp, 0b00000010 		; set bit 1
	out TCCR0B, temp			; start timer counter clock (prescaller 1/8)

; - BAM cycle -------------------
	push data1
	push data2
	push data3
	push data4

	rcall bit_1
	rcall bit_1
	rcall bit_1
	rcall bit_1

; - MAB cycle -------------------
	pop data4
	pop data3
	pop data2
	pop data1

	rcall bit_4

	in temp, TIMSK				;
	andi temp, 0b11111000		; clear three lower bits...
	ori temp, 0b00000100 		; set bit 2
	out TIMSK, temp				; disable compare match A int and enable compare match B int

	rcall bit_4
	rcall bit_4
	rcall bit_4

; - MIBAM complete --------------
	rcall col_reset
	rcall data_out

	clr temp					;
	out TCCR0B, temp			; stop timer counter clock

	in temp, TIMSK				;
	andi temp, 0b11111000		; clear three lower bits...
	out TIMSK, temp				; disable compare match B int
	ret
	
; *********************************************************************************************************************************
; * transform buffer data to port data
; *********************************************************************************************************************************
; bit_1,2,3,4: cycles for rcall + routine + ret: 3 + 31 + 4 = 38

bit_1:
	rcall col_reset
	sbrc data1, 4				; column 1
	cbr pb_data,0b00010000
	sbrc data1, 0				; column 2
	cbr pa_data,0b00000010
	sbrc data2, 4				; column 3
	cbr pa_data,0b00000001
	sbrc data2, 0				; column 4
	cbr pb_data,0b00000010
	sbrc data3, 4				; column 5
	cbr pd_data,0b00001000
	sbrc data3, 0				; column 6
	cbr pb_data,0b00000100
	sbrc data4, 4				; column 7
	cbr pb_data,0b01000000
	sbrc data4, 0				; column 8
	cbr pb_data,0b10000000
	lsr data1
	lsr data2
	lsr data3
	lsr data4
	sleep
	ret
bit_4:
	rcall col_reset
	sbrc data1, 7				; column 1
	cbr pb_data,0b00010000
	sbrc data1, 3				; column 2
	cbr pa_data,0b00000010
	sbrc data2, 7				; column 3
	cbr pa_data,0b00000001
	sbrc data2, 3				; column 4
	cbr pb_data,0b00000010
	sbrc data3, 7				; column 5
	cbr pd_data,0b00001000
	sbrc data3, 3				; column 6
	cbr pb_data,0b00000100
	sbrc data4, 7				; column 7
	cbr pb_data,0b01000000
	sbrc data4, 3				; column 8
	cbr pb_data,0b10000000
	lsl data1
	lsl data2
	lsl data3
	lsl data4
	sleep
	ret

; *********************************************************************************************************************************
; * output new port data
; *********************************************************************************************************************************
; cycles for rcall + routine + ret: 3 + 3 + 4 = 10

data_out:
	out PORTA, pa_data
	out PORTB, pb_data
	out PORTD, pd_data
	ret

; *********************************************************************************************************************************
; * reset port data registers, so that all columns are deactivated
; *********************************************************************************************************************************
; cycles for rcall + routine + ret: 3 + 8 + 4 = 15

col_reset:
	ori pa_data,0b00010011
	ori pd_data,0b00001000
	ori pb_data,0b11010110
	ret

; *********************************************************************************************************************************
; * interrupt service routines for timer counter compare matches
; *********************************************************************************************************************************
; bam_step: increasing delays
; mab_step: decreasing delays

bam_step:
	rcall data_out				; output previously prepared data
	clr temp					;
	out TCNT0, temp				; reset timer counter

	in temp, OCR0A				;
	lsl temp					; OCR0A = OCR0A x 2
	out OCR0A, temp				;
	reti

mab_step:
	rcall data_out				; output previously prepared data
	clr temp					;
	out TCNT0, temp				; reset timer counter

	in temp, OCR0B				;
	lsr temp					; OCR0B = OCR0B / 2
	out OCR0B, temp				;
	reti

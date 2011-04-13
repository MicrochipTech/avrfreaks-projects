

; ----------------------------------------------
; BUTTON CONTROL SUBROUTINES
; ----------------------------------------------


; *********************************************************************************************************************************
; * take action for buttons pressed
; *********************************************************************************************************************************

button_act:
	in temp, mode
	andi temp, 0b11110000
	tst temp
	breq mode_selection

; ---------------------------------------------------------------------------------------------------------------------------------
; ---------------------------------------------------------------------------------------------------------------------------------

	in temp, btn_flags
	sbrc temp, 3				; check upper-left button
	rjmp ul_btn
	sbrc temp, 2				; check left button
	rjmp dl_btn
	sbrc temp, 1				; check right button
	rjmp dr_btn
	sbrc temp, 0				; check upper-right button
	rjmp ur_btn

ul_btn:							; ------------------------------------- up left	(act like escape)
	ldi temp, 4					; increase frame drawing rate (exit should be immediate)
	out draw_spd, temp			; 4 -> 35 frames per second (drawing rate = 143 / draw_spd)
	rcall clr_bf				; clear frame buffers
	clr temp					; select "wait for selection" mode
	rjmp btn_act_end

dl_btn:							; ----------------------------------- down left
	in temp, mode				;
	andi temp, 0b11101000		;
	cpi temp,  0b01100000		; check if in game mode and vehicle is still alive
	brne gm_btn_act_end			;

	rcall erase_vehicle

	mov temp, sq_f_col			;
	cpi temp, 1					; check if there is room to go left
	breq gm_btn_act_end			;
	dec sq_f_col				;
	ser temp					;
	mov sq_l_col, temp			; vehicle position changed
	rjmp gm_btn_act_end

dr_btn:							; ---------------------------------- down right
	in temp, mode				;
	andi temp, 0b11101000		;
	cpi temp,  0b01100000		; check if in game mode and vehicle is still alive
	brne gm_btn_act_end			;

	rcall erase_vehicle

	mov temp, sq_f_col			;
	cpi temp, 6					; check if there is room to go right
	breq gm_btn_act_end			;
	inc sq_f_col				;
	ser temp					;
	mov sq_l_col, temp			; vehicle position changed
	rjmp gm_btn_act_end

ur_btn:							; ------------------------------------ up right	(act like reset)
	rcall clr_bf				; clear frame buffers
	in temp, mode				;
	andi temp, 0b11100000		; reset selected mode (change to first sub-mode)
	rjmp btn_act_end



gm_btn_act_end:					; ------------------------------------ special tn routine exit for gaming mode
	ldi temp, 0b00100000 		;
	out btn_flags, temp			; clear all button flags exept lock-flag (cleared in scan routine, when the time is right ;-)
	ret
	
; ---------------------------------------------------------------------------------------------------------------------------------
; ---------------------------------------------------------------------------------------------------------------------------------

mode_selection:
	in data1, TCNT1L			; read current timer/counter1 value
	in data2, TCNT1H			;
	mov seed_lo, data1			; use it as feed for random number generation
	mov seed_hi, data2			;

	rcall clr_bf				; clear frame buffers

	in temp, btn_flags
	sbrc temp, 3				; upper-left button ->
	ldi temp, 0b00100000		; select "alien morohing" mode
	sbrc temp, 2				; left button ->
	ldi temp, 0b01000000		; select "game of life" mode
	sbrc temp, 1				; right button ->
	ldi temp, 0b01100000		; select "need for speed" mode
	sbrc temp, 0				; upper-right button ->
	ldi temp, 0b10000000		; select "cylon fader" mode

btn_act_end:
	out mode, temp
	ldi temp, 0b00100000 		; clear all button flags exept lock-flag (cleared in scan routine, when the time is right ;-)
	out btn_flags, temp			;
	ret

; *********************************************************************************************************************************
; * enable buttons, check if pressed and deactivate
; *********************************************************************************************************************************
; cycles for rcall + routine + ret: 3 + 19 + 4 = 26

check_buttons:
	ldi temp, 0b11001100		; mask for portd data-direction register
	out DDRD, temp				; configure appropriate portd pins as inputs
	sbi PORTD, 6				; power-up buttons

    clr temp

	sbic PIND, 0				; check upper-left button
	ori temp, 0b00111000
	sbic PIND, 1				; check left button
	ori temp, 0b00110100
	sbic PIND, 4				; check right button
	ori temp, 0b00110010
	sbic PIND, 5				; check upper-right button
	ori temp, 0b00110001

	out btn_flags, temp

	cbi PORTD, 6				; power-down buttons
	ser temp					;
	out DDRD, temp				; configure all portd pins as outputs
	ret
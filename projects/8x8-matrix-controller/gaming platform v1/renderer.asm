
; *********************************************************************************************************************************
; * render new image according to mode of operation
; *********************************************************************************************************************************

render:
	in temp, mode					; load mode of operation in temp
	andi temp, 0b11100000			; keep most insteresting part (general mode of operation)

	cpi temp, 0b00100000
	breq gt_alien_morphing

	cpi temp, 0b01000000
	breq gt_game_of_life

	cpi temp, 0b01100000
	breq gt_need_for_speed

	cpi temp, 0b10000000
	breq gt_cylon_fader

	rjmp waiting_selection 			; if none of the above modes was detected, wait for user input

gt_alien_morphing:
	rjmp alien_morphing
gt_game_of_life:
	rjmp game_of_life
gt_need_for_speed:
	rjmp need_for_speed
gt_cylon_fader:
	rjmp cylon_fader

; ---------------------------------------------------------------------------------------------------------------------------------
; ---------------------------------------------------------------------------------------------------------------------------------

waiting_selection:
	ldi temp, 7						; 7 -> 15.14 frames per second (drawing rate = 106 / draw_spd)
	out draw_spd, temp				; speed up for morphng...

	ldi	XH, SRAM_sec_fb_hi			; prepare pointer for secondary buffer
	ldi	XL, SRAM_sec_fb_lo			;
	adiw XH:XL, 20					; target position of first indicator
	ld data3, X						; load current value of indicator

	ldi data4, 0b11110000			; assume indicators have to be saturated
	in temp, mode					; load mode of operation in temp
	tst temp						; check if saturation or desaturation needed
	breq fade_indicators_do			;
	clr data4						; indicators have to be desaturated

fade_indicators_do:
	rcall morphing					; approximate data3 to data4 (fade in or out)

	st X, data3						;
	swap data3						;
	adiw XH:XL, 3 					;
	st X, data3						;
	adiw XH:XL, 5					; draw new state of keypad indicators
	st X, data3						;
	swap data3						;
	adiw XH:XL, 3					;
	st X, data3						;
	swap data3						;

	ldi data1, 0b00000001			; value for toggling bit 0 of mode register

	cp data4, data3					; check if pixel reached the final value
	brne fade_indicators_end		; if not, proceed to end
	eor temp, data1					; if yes, toggle sub-mode
	out mode, temp					;
fade_indicators_end:
	ret

; ---------------------------------------------------------------------------------------------------------------------------------
; ---------------------------------------------------------------------------------------------------------------------------------

; compare data of tertiary and primary buffers
; write new data in secondary buffer

alien_morphing:
	ldi temp, 5						; 5 -> 21.2 frames per second (drawing rate = 106 / draw_spd)
	out draw_spd, temp				; speed up for morphng...

	in temp, mode					; load mode of operation in temp
	andi temp, 0b00001111			; extract pointer from sub-mode

	swap temp						;
	lsl temp						; multiply temp with 32
	andi temp, 0b11100000			; calculate reading offset

	ser data4						; target tertiary buffer
	rcall flash2ram					; copy alien figure from flash table to tertiary buffer

	rcall morphing_bf				; do the morphing babe!
	
	tst data2						; check if time to change sub-mode (alien couple)
	brne alien_morphing_end			;

	ldi temp, 212					; 212 -> 0.5 frames per second (drawing rate = 106 / draw_spd)
	out draw_spd, temp				;

	in temp, mode
	inc temp						; proceed to next alien couple
	cpi temp, 0b00100101			; check if mode has illegal value
	brlo store_morphing_mode		; if not, proceed
	ldi temp, 0b00100000			; otherwise, correct the value
store_morphing_mode:
	out mode, temp					; store mode value

alien_morphing_end:
	ret

; ---------------------------------------------------------------------------------------------------------------------------------
; ---------------------------------------------------------------------------------------------------------------------------------

game_of_life:
	in temp, mode					; load mode of operation in temp

	cpi temp, 0b01000001			; check if initial population exists and if morphing is done
	breq new_generation				; if yes, calculate the next generation

	cpi temp, 0b01000011			; check if next generation is calculated
	breq morph_generations			; if yes, morph to the new one

	ldi temp, 106					; 106 -> 1 frame per second (drawing rate = 106 / draw_spd)
	out draw_spd, temp				; slow down for generation to be observed...

	rcall clr_bf					; clear buffers

	ldi temp, 16					;
init_rnd_pop:						; initial population consists of 18 pixels (at most)
	rcall rnd_pixel					;
	dec temp						;
	brne init_rnd_pop				;

	ldi temp, 0b01000001			; initial population completed
	out mode, temp					; update sub-mode
	ret

; =================================== morph through generations
morph_generations:
	ldi temp, 5						; 5 -> 21.2 frames per second (drawing rate = 106 / draw_spd)
	out draw_spd, temp				; speed up for morphng...

	rcall morphing_bf				; do the mporphing babe!

	tst data2						; check if morphing is completed
	brne more_morphing				; if not, more morphing steps are neede

	ldi temp, 0b01000001			;
	out mode, temp					; otherwise, calculate next generation

	ldi temp, 106					; 106 -> 1 frame per second (drawing rate = 106 / draw_spd)
	out draw_spd, temp				; slow down for generation to be observed...
more_morphing:
	ret

; =================================== create new generation
new_generation:
	clr data1						; row number [0:upper matrix row  -->  7:lower matrix row]

	clr data2						; col number [0:leftmost matrix col  -->  7:rightmost matrix col]

ng_atom:							; begin calculations for square_of_interest coordinates...
	mov sq_f_row, data1				;
	mov sq_l_row, data1				;
	mov sq_f_col, data2				;
	mov sq_l_col, data2				;

first_row:							
	ldi temp, 0b11111111			; lower limit
	dec sq_f_row					; calculate starting row for square_of_interest
	cp temp, sq_f_row				;
	brne firts_col					;
	clr sq_f_row					;
firts_col:
	dec sq_f_col					; calculate starting column for square_of_interest
	cp temp, sq_f_col				;
	brne last_row					;
	clr sq_f_col					;

last_row:
	ldi temp, 0b00000111			; upper limit
	inc sq_l_row					; calculate ending row for square_of_interest
	cp temp, sq_l_row				;
	brsh last_col					;
	dec sq_l_row					;
last_col:
	inc sq_l_col					; calculate ending column for square_of_interest
	cp temp, sq_l_col				;
	brsh do_sq_of_interest			;
	dec sq_l_col					;



do_sq_of_interest:
	mov sq_tmp, sq_f_col			; will be needed because of "scanning"
	clr sq_alive					; number of live neighbors



sq_of_interest: 					; check neighbors' condition in square_of_interest
	mov data3, sq_f_row				;
	swap data3						; check neighbor's condition
	andi data3, 0b11110000			;
	or data3, sq_f_col				;
	clr data4						; set for reading
	rcall rw_pixel_data				;
	tst data4						; check if neighbor is alive
	breq next_neighbor				; if not, proceed to another one
	inc sq_alive					; if yes, increase live_neighbors counter

next_neighbor:						; proceed to next neighbor
	inc sq_f_col					; increase column number
	mov temp, sq_l_col				;
	cp temp, sq_f_col				; check if reached upper limit
	brsh sq_of_interest				; if not, repeat for next column

	mov sq_f_col, sq_tmp			; if yes, reset column number
	inc sq_f_row					; increase row number
	mov temp, sq_l_row				;
	cp temp, sq_f_row				; check if reached upper limit
	brsh sq_of_interest				; if not, repeat for next row



	mov data3, data1				; check condition for pixel_of_interest
	swap data3						;
	andi data3, 0b11110000			;
	or data3, data2					;
	clr data4						; set for reading
	rcall rw_pixel_data				;

	mov temp, sq_alive				; copy number of live neighbors

	tst data4						;
	brne alive4now					; He is alive... for now ;-)

alreadydead:
	cpi temp, 3						; he is dead...
	brne next_atom					; he gets resurrected, if he has exactly 3 live neighbors
resurrecthim:
	mov data3, data1				;
	swap data3						; change condition for pixel_of_interest
	andi data3, 0b11110000			;
	or data3, data2					;
	ser data4						; set for writting
	rcall rw_pixel_data				;
	rjmp next_atom

alive4now:
	dec temp						; he was accounted as a live neighbor...
	cpi temp, 2						;
	breq next_atom					; he gets to live on, if he has exactly 2 or 3 live neighbors
	cpi temp, 3						;
	breq next_atom					;
killhim:
	mov data3, data1				;
	swap data3						; change condition for pixel_of_interest
	andi data3, 0b11110000			;
	or data3, data2					;
	ldi data4, 0b11000000			; set for writting
	rcall rw_pixel_data				;



next_atom:							; proceed to next pixel_of_interest
	inc data2						; increase column number
	cpi data2, 8					; check if reached upper limit
	brne proceed_ng_atom			; if not, repeat for next column
	
	clr data2						; reset column number
	inc data1						; increase row number
	cpi data1, 8					; check if reached upper limit
	brne proceed_ng_atom			; if not, repeat for next row


do_morphing:
	ldi temp, 0b01000011			;
	out mode, temp					; next generation is calculated... morphing time!
	ret

proceed_ng_atom:
	rjmp ng_atom


; ---------------------------------------------------------------------------------------------------------------------------------
; ---------------------------------------------------------------------------------------------------------------------------------

need_for_speed:
	ldi temp, 5						; 5 -> 21.2 frames per second (drawing rate = 106 / draw_spd)
	out draw_spd, temp				; speed up for morphng...

	in temp, mode					; check if starting map has to be drawn
	sbrc temp, 4					;
	rjmp map_roll					; if not, make the map roll...

; =================================== create starting map and initialize game special variables

	clr data1
next_row_of_bricks:
	clr data2
next_brick_in_row:
	mov data3, data2			;
	mov temp, data1				; prepare coordinats for buffer writing...
	swap temp					;
	or data3, temp				;

	cpi data2, 0				; check if brick is in first col
	breq brick_on				; if it is, proceed to brick_on
	cpi data2, 7				; check if brick is in last col
	breq brick_on				; if it is, proceed to brick_on

brick_off:
	rcall put_brick_off			; don't draw a brick here
	rjmp next_brick				;
brick_on:
	rcall put_brick_on			; draw a brick here

next_brick:
	inc data2					;
	cpi data2, 8				;
	brne next_brick_in_row		; repeat for all bricks in row
	inc data1					;
	cpi data1, 8				;
	brne next_row_of_bricks		; repeat for all rows

	ldi temp, 0b01110000		; initial map drawn
	out mode, temp				; proceed to next sub-mode

	ldi temp, 3					;
	mov sq_f_col, temp			; position (column) of vehicle
	clr sq_l_col				; when set, position (column) of vehicle has changed
	clr sq_f_row				; previously used map tile

	ldi temp, 0b10000000		;
	mov sq_tmp, temp			; special sub-mode variable (msbit set when adding to tile ptr, cleared when substracting)
								; lsbits: count add-substruct cycle (for level changing)
	ret

; =================================== shift down map and create a new line

map_roll:
	in temp, mode					;
	sbrc temp, 3					; check if already crashed...
	rjmp game_end					;

	tst sq_l_col					; check if vehicle has moved
	brne already_deleted			; if it has, previous position is already erased
	rcall erase_vehicle				; otehrwise, erase vehicle now (so that it will not shift downwards and will not interfere with collision detection)

already_deleted:
	mov temp, sq_tmp 				;
	andi temp, 0b00011111			; get lower nibble of sq_tmp
	tst temp						;
	breq do_roll_the_map			; map is rolled when lower nibble of sq_tmp equals zero


	mov data1, sq_tmp				;
	andi data1, 0b11100000			; if not zero, get upper nibble of sq_tmp

	inc temp						; increase lower nibble of sq_tmp
	cpi temp, 6						; check if reached upper limit
	brne to_vehicle					; proceed to vehicle manipulation
	clr temp						; otherwise, clear lower nibble of sq_tmp

to_vehicle:
	or data1, temp					; combine lower with upper nibble of sq_tmp
	mov sq_tmp, data1				; put combination back in sq_tmp
	rjmp put_vehicle				; proceed to vehicle manipulation



do_roll_the_map:
	inc sq_tmp						; increase lower nibble of sq_tmp (rolling will not repeat until sq_tmp reaches a certain value)

	rcall shift_bf					; shift all rows downwards

	ldi ZH, high (maptiles * 2)		; starting address of flash table with tiles
	ldi ZL, low (maptiles * 2)		; starting address of flash table with tiles
	rcall randomizer				; get random number (data4:data3)

	andi data3, 0b00000011			;
	cpi data3, 3					; prepare a random number in the range [0..2]
	brne rnd_offset_ready			;
	ldi data3, 2					;

rnd_offset_ready:
	sbrc sq_tmp, 7					; if flag is set...
	add sq_f_row, data3				; add random number to offset pointer
	
	sbrs sq_tmp, 7					; if flag is cleared...
	sub sq_f_row, data3				; substract random number from offset pointer
	
	mov temp, sq_f_row				;
	cpi temp, 17					; check offset pointer
	brlo get_tile					; if lower than max value, proceed to reading from flash

	sbrs sq_tmp, 7					; if in substracting mode...
	clr sq_f_row					; clear offset pointer 

	ldi temp, 17					;
	sbrc sq_tmp, 7					; if in adding mode...
	mov sq_f_row, temp				; set offset pointer to max value

	ldi temp, 0b10000000			;
	eor sq_tmp, temp				; toggle mode of offset pointer manipulation


get_tile:
	clr data2
	add ZL, sq_f_row				; add tile offset to base pointer
	adc ZH, data2					;

	lpm								; read from flash
	mov temp, r0					; put random tile data in temp

read_bit:
	mov data3, data2				; coordinates for writing (row = 0 / col = data2)
	rol temp						;
	brcc brick_no					; put bit in C and check it

brick_yes:
	rcall put_brick_on				; if bit was set, put brick
	rjmp next_bit
brick_no:
	rcall put_brick_off				; if bit was cleared, don't put brick

next_bit:
	inc data2
	cpi data2, 8
	brne read_bit

; =================================== draw vehicle and make a hit-test...

put_vehicle:
	mov data3, sq_f_col			; vehicle x = sq_f_col (column)
	ori data3, 0b01010000		; vehicle y = 5 (row)
	ldi data4, 0b00010000		; read from secondary buffer
	rcall rw_pixel_data			;
	mov temp, data4

	rcall draw_vehicle			; draw vehicle in (new) position
	clr sq_l_col				; vehicle drawn in (new) position

	tst temp					; check for collision (don't worry, data collected before drawing vehicle)
	breq put_vehicle_done		;
	ldi temp, 0b01111000		; if collision was detected, change sub-mode...
	out mode, temp				;
put_vehicle_done:
	ret

; =================================== game ending...

game_end:
	ldi data4, 0b10001100		; draw vehicle (dot) in secondary buffer (high intensity)
	in temp, mode				; check if vehicle should fade in or out...
	sbrc temp, 0				;
	ldi data4, 0b10000011		; draw vehicle (dot) in secondary buffer (low intensity)

	ldi data1, 0b00000001		; value for toggling bit 0 of sub-mode
	eor temp, data1				; if yes, toggle sub-mode
	out mode, temp				;

	rcall vehicle				; draw vehicle dot
	ret





draw_vehicle:
	ldi data4, 0b10001100		; draw vehicle (dot) in secondary buffer
	rjmp vehicle
erase_vehicle:
	ldi data4, 0b10000000		; erase vehicle (dot) in secondary buffer
vehicle:
	mov data3, sq_f_col			; vehicle x = sq_f_col (column)
	ori data3, 0b01010000		; vehicle y = 5 (row)
	rcall rw_pixel_data			;
	ret

put_brick_on:
	ldi data4, 0b10000010
	rjmp put_brick
put_brick_off:
	ldi data4, 0b10000000
put_brick:
	rcall rw_pixel_data
	ret

; ---------------------------------------------------------------------------------------------------------------------------------
; ---------------------------------------------------------------------------------------------------------------------------------

cylon_fader:
	ldi temp, 15					; 15 -> 7.06 frames per second (drawing rate = 106 / draw_spd)
	out draw_spd, temp				;

	in temp, mode					; check if vibrator has to be intialized or not
	sbrc temp, 0					;
	rjmp cylon_fader_roll			; if not, let it roll...

	ori temp, 1						; change sub-mode
	out mode, temp					;
	clr data2						; vibrator initialized!
	rcall clr_bf					; clear buffers


cylon_fader_roll:
	ldi	ZH, SRAM_sec_fb_hi
	ldi	ZL, SRAM_sec_fb_lo

	adiw ZH:ZL, 16					;
	ldi temp, 4						; prepare for reading/writting fader row
	clr data4						;

roll:
	ld data3, Z						;
	rcall morphing					;
	rcall morphing					; fade out all pixels of row
	st Z+, data3					;
	dec temp						;
	brne roll						;


	in temp, mode					; check for waiting sub-mode (step 1)
	sbrs temp, 2					;
	rjmp select_direction			; 
step1:
	sbrc temp, 3					; check for waiting sub-mode (step 2)
	rjmp step2						; 
	ori temp, 0b00001000			;
	rjmp cylon_fader_end			;
step2:
	sbrc temp, 4					; check for waiting sub-mode (step 3)
	rjmp step3						; 
	ori temp, 0b00010000			;
	rjmp cylon_fader_end			;
step3:
	andi temp, 0b11100011			; disable waiting sub-modes
	rjmp cylon_fader_end			;


select_direction:
	ldi data3, 0b01000000 			; load row pointer (row 4)
	or data3, data2					; load col pointer (data2)
	ldi data4, 0b10001111			; set for wtitting on secondary buffer (high nibble 1100) / data to write (low nibble)
	rcall rw_pixel_data				; put the bright dot in fader row!

	sbrc temp, 1					; is it going to the right or to the left?
	rjmp roll_to_the_left			; 

roll_to_the_right:
	cpi data2, 7					; is there is room to move?
	breq toggle_direction			; if not, change direction
	inc data2						; if yes, move to the right
	ret

roll_to_the_left:
	cpi data2, 0					; is there is room to move?
	breq toggle_direction			; if not, change direction
	dec data2						; if yes, move to the left
	ret

toggle_direction:
	ldi data1, 0b00000110			;
	eor temp, data1					; change direction and enable wait sub-mode
cylon_fader_end:
	out mode, temp
	ret
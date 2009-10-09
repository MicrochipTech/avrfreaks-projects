PLOT_TRIANGLE:

push	temp		
push	temp1		
push	scroll
push	pointer
push	column
push	row
push	char
push	flags
push	temp2
push	temp3
push	pageY	

clr	flags
;local flags
.equ	dy_negative = 0
.equ	dz_negative = 1			;of local flags

	
;Load the three points 

	lds	temp,new
	lds	temp1,new+1
	lds	temp2,current
	lds	temp3,current+1
	lds	row,old
	lds	column,old+1

;Sort the three points by X value
;is new to the left of current?

	cp	temp1,temp3
	brlt	NEW_LEFT_CURRENT
	brne	NEW_RIGHT_CURRENT
	cp	temp,temp2
	brlo	NEW_LEFT_CURRENT

NEW_RIGHT_CURRENT:
;is new to the left of old?
	cp	temp1,column
	brlt	CURRENT_NEW_OLD
	brne	OLD_LEFT_NEW
	cp	temp,row
	brlo	CURRENT_NEW_OLD

OLD_LEFT_NEW:
;is current to the left of old
	cp	temp3,column
	brlt	CURRENT_OLD_NEW
	brne	OLD_CURRENT_NEW
	cp	temp2,row
	brlo	CURRENT_OLD_NEW
	rjmp	OLD_CURRENT_NEW

NEW_LEFT_CURRENT:
;is new to the left of old?
	cp	temp1,column
	brlt	NEW_LEFT_OLD
	brne	OLD_NEW_CURRENT
	cp	temp,row
	brlo	NEW_LEFT_OLD
	rjmp	OLD_NEW_CURRENT

NEW_LEFT_OLD:
;is current to the left of old
	cp	temp3,column
	brlt	NEW_CURRENT_OLD
	brne	NEW_OLD_CURRENT
	cp	temp2,row
	brlo	NEW_CURRENT_OLD
	rjmp	NEW_OLD_CURRENT



;Six possible orders of the points



NEW_CURRENT_OLD:

	rcall	LOAD_NEW
	rcall	SAVE_START
	rcall	LOAD_CURRENT
	rcall	SAVE_MID
	rcall	LOAD_OLD
	rcall	SAVE_END
	rjmp	DRAW_TRIANGLE

NEW_OLD_CURRENT:

	rcall	LOAD_NEW
	rcall	SAVE_START
	rcall	LOAD_OLD
	rcall	SAVE_MID
	rcall	LOAD_CURRENT
	rcall	SAVE_END
	rjmp	DRAW_TRIANGLE

OLD_NEW_CURRENT:

	rcall	LOAD_OLD
	rcall	SAVE_START
	rcall	LOAD_NEW
	rcall	SAVE_MID
	rcall	LOAD_CURRENT
	rcall	SAVE_END
	rjmp	DRAW_TRIANGLE

CURRENT_OLD_NEW:

	rcall	LOAD_CURRENT
	rcall	SAVE_START
	rcall	LOAD_OLD
	rcall	SAVE_MID
	rcall	LOAD_NEW
	rcall	SAVE_END
	rjmp	DRAW_TRIANGLE

CURRENT_NEW_OLD:

	rcall	LOAD_CURRENT
	rcall	SAVE_START
	rcall	LOAD_NEW
	rcall	SAVE_MID
	rcall	LOAD_OLD
	rcall	SAVE_END
	rjmp	DRAW_TRIANGLE

OLD_CURRENT_NEW:

	rcall	LOAD_OLD
	rcall	SAVE_START
	rcall	LOAD_CURRENT
	rcall	SAVE_MID
	rcall	LOAD_NEW
	rcall	SAVE_END


DRAW_TRIANGLE:

;Calculate four delta terms and two initial error terms

	lds	temp,tstartx
	lds	temp1,tstartx+1		;temp=tstartx

	lds	temp2,tendx
	lds	temp3,tendx+1

	sub	temp2,temp
	sbc	temp3,temp1			;tendx-tstartx (must be zero or positive)

	sts	dx,temp2
	sts	dx+1,temp3			;saved

	asr	temp3
	ror	temp2				; divide by 2

	sts	yerror,temp2
	sts	yerror+1,temp3		;store y error

;temp, temp1 still xstart

	lds	temp2,tmidx
	lds	temp3,tmidx+1

	sub	temp2,temp
	sbc	temp3,temp1			;tmidx-tstartx (must be zero or positive)

	sts	da,temp2
	sts	da+1,temp3			;saved

	asr	temp3
	ror	temp2				; divide by 2

	sts	zerror,temp2
	sts	zerror+1,temp3		;store z error

	lds	temp,tstarty
	lds	temp1,tstarty+1		;temp=tstarty

	lds	temp2,tendy
	lds	temp3,tendy+1

	sub	temp2,temp
	sbc	temp3,temp1			;tendy-tstarty

	brpl	SKIP_45
	sbr	flags,(1<<dy_negative)
	com	temp3
	neg	temp2
	brcs	SKIP_45
	inc	temp3
	SKIP_45:				;got absolute value of dy

	sts	dy,temp2
	sts	dy+1,temp3			;saved

;temp, temp1 still ystart

	lds	temp2,tmidy
	lds	temp3,tmidy+1

	sub	temp2,temp
	sbc	temp3,temp1			;tmidy-tstarty

	brpl	SKIP_46
	sbr	flags,(1<<dz_negative)
	com	temp3
	neg	temp2
	brcs	SKIP_46
	inc	temp3
	SKIP_46:				;got absolute value of dz

	sts	dz,temp2
	sts	dz+1,temp3			;saved




;So we now have the first four delta terms and two error terms.

	ldi	ZL,low(tstartx)
	ldi	ZH,high(tstartx)

	rcall	WIND_LOAD
	brne	SKIP_67			;in window?
	rcall	GADDR				;address the point
	lds	pixel_mask,ZMASK		;load pixel mask
	rcall	PLBYTE			;and plot it!!!

SKIP_67:

;plot the first section of the triangle between startx and midx

rcall	TRIANGLE_LOOP_ONE

;plot the second section of the triangle between midx and endx
;this means replacing the 'z' delta and error terms

	lds	temp,tmidx
	lds	temp1,tmidx+1		;temp=tstartx

	lds	temp2,tendx
	lds	temp3,tendx+1

	sub	temp2,temp
	sbc	temp3,temp1			;tendx-tstartx (must be zero or positive)

	sts	da,temp2
	sts	da+1,temp3			;saved

	asr	temp3
	ror	temp2				; divide by 2

	sts	zerror,temp2
	sts	zerror+1,temp3		;store new z error

	cbr	flags,(1<<dz_negative)
	lds	temp,tmidy
	lds	temp1,tmidy+1		;temp=tmidy
			
	lds	temp2,tendy
	lds	temp3,tendy+1

	sub	temp2,temp
	sbc	temp3,temp1			;tmidy-tstarty

	brpl	SKIP_96
	sbr	flags,(1<<dz_negative)
	com	temp3
	neg	temp2
	brcs	SKIP_96
	inc	temp3
	SKIP_96:				;got absolute value of dz

	sts	dz,temp2
	sts	dz+1,temp3			;saved

;******* Now replace mid with end (

	lds	temp2,tendx
	lds	temp3,tendx+1

	sts	tmidx,temp2
	sts	tmidx+1,temp3






rcall	TRIANGLE_LOOP_ONE


pop	pageY	
pop	temp3
pop	temp2
pop	flags
pop	char
pop	row
pop	column
pop	pointer
pop	scroll
pop	temp1		
pop	temp		

rjmp	FINISH_PLOT

;********	Triangle plotting 

TRIANGLE_LOOP_ONE:
	lds	temp2,tmidx
	lds	temp3,tmidx+1		;= middle point
	lds	temp,tstartx
	lds	temp1,tstartx+1		;temp=xstart

	cp	temp1,temp3
	brmi	INCREMENT_X			;is less than middle point
	breq	SKIP_70
	ret
SKIP_70:
	cp	temp,temp2
	brlo	SKIP_71
	ret
SKIP_71:

INCREMENT_X:
	inc	temp
	brne	SKIP_49
	inc	temp1				; x := x+1
	sts	tstartx+1,temp1
SKIP_49:
	sts	tstartx,temp

	lds	temp2,yerror
	lds	temp3,yerror+1
	lds	row,dy
	lds	column,dy+1

	sub	temp2,row
	sbc	temp3,column		;yerror=yerror-dy
TEST_FOR_Y_INCREMENT:
	lds	temp,tstarty
	lds	temp1,tstarty+1

	tst	temp3
;	brpl	SKIP_69
;	tst	temp2
	brpl	DONE_Y_INCREMENT		;if yerror>=0
	
SKIP_69:
	sbrs	flags,dy_negative
	rjmp	SKIP_62
	dec	temp
	brne	SKIP_63
	dec	temp1
	rjmp	SKIP_63
SKIP_62:
	inc	temp
	brne	SKIP_63
	inc	temp1
SKIP_63:

	sts	tstarty,temp
	sts	tstarty+1,temp1	

	lds	temp,dx
	lds	temp1,dx+1

	add	temp2,temp
	adc	temp3,temp1
	
	rjmp	TEST_FOR_Y_INCREMENT	

DONE_Y_INCREMENT:

	sts	yerror,temp2
	sts	yerror+1,temp3

;Now do the Z incrementing

	lds	temp2,zerror
	lds	temp3,zerror+1
	lds	row,dz
	lds	column,dz+1
	sub	temp2,row
	sbc	temp3,column		;zerror=zerror-dy

TEST_FOR_Z_INCREMENT:
	lds	temp,tstartz
	lds	temp1,tstartz+1



	tst	temp3
;	brpl	SKIP_72
;	tst	temp2
	brpl	DONE_Z_INCREMENT		;if yerror>=0
SKIP_72:	
		
	sbrs	flags,dz_negative
	rjmp	SKIP_65
	dec	temp
	brne	SKIP_66
	dec	temp1
	rjmp	SKIP_66
SKIP_65:
	inc	temp
	brne	SKIP_66
	inc	temp1
SKIP_66:

	sts	tstartz,temp
	sts	tstartz+1,temp1	

	lds	temp,da
	lds	temp1,da+1

	add	temp2,temp
	adc	temp3,temp1
	
	rjmp	TEST_FOR_Z_INCREMENT	

DONE_Z_INCREMENT:

	sts	zerror,temp2
	sts	zerror+1,temp3


;	lds	fill_block+2,tstarty
;	lds	fill_block+3,tstarty+1
;	lds	fill_block+4,tstartx
;	lds	fill_block+5,tstartx+1
;	lds	fill_block+6,tstartz
;	lds	fill_block+7,tstartz+1

	ldi	ZL,low(zerror)
	ldi	ZH,high(zerror)		;zerror is two bytes before the data we need

	rcall	VERTICAL_LINE_LOAD


	rjmp	TRIANGLE_LOOP_ONE

;**********  End of triangle plotting loop


LOAD_NEW:

	ldi	ZL,low(new)
	ldi	ZH,high(new)
	rcall	LOAD
	ret

LOAD_CURRENT:

	ldi	ZL,low(current)
	ldi	ZH,high(current)
	rcall	LOAD
	ret

LOAD_OLD:

	ldi	ZL,low(old)
	ldi	ZH,high(old)
	rcall	LOAD
	ret


SAVE_START:

	sts	tstartx,temp
	sts	tstartx+1,temp1
	sts	tstarty,temp2
	sts	tstarty+1,temp3
	sts	tstartz,temp2
	sts	tstartz+1,temp3
	ret

SAVE_MID:

	sts	tmidx,temp
	sts	tmidx+1,temp1
	sts	tmidy,temp2
	sts	tmidy+1,temp3
	ret

SAVE_END:

	sts	tendx,temp
	sts	tendx+1,temp1
	sts	tendy,temp2
	sts	tendy+1,temp3
	ret

BITMAP:
;char byte to display



	call	SEND_INSTRUCTION
	call	SET_PAGE				;sets page to pagey (0-7)
	mov	temp,temp3
	call	SET_BYTE				;sets byte to temp (0-127)
	call	SEND_DATA
	out	PORTC,char
	call	CLOCK
	inc	temp3
SKIP_78:
	cpi	temp3,0x80
	brlo	NEXT_BITMAP_BYTE

	clr	temp3					;back to left side
	inc	pagey
	cpi	pagey,0x08
	breq	BITMAP_DONE

NEXT_BITMAP_BYTE:
	jmp	RS232_DONE


BITMAP_DONE:
	cbr	flags,(1<<dotted)			;Show we are done
	call	RST_WINDOWS
	jmp	RS232_DONE

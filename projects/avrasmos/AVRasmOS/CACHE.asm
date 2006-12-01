;************************************************
;|WRITER_low_adr|READER_low_adr|SIZE|BUFFER|
;Быстрые, монопольные операции. Должны быть выровнены по странице
;@0 - начальный адрес кэша  @1-размер кэша @2 - регистр данных
;возвращает СARRY SET если не удалось выполнить операцию
;************************************************
.MACRO	____CACHE_INIT
	ldi	r24,low(@0+3)
	sts	@0,r24
	sts	@0+1,r24
	ldi	r24,0x00
	sts	@0+2,r24
.ENDMACRO

.MACRO	____CACHE_GET_SIZE			;@0 - начальный адрес кэша @1 регистр с размером
	lds	@1,@0+2
.ENDMACRO

.MACRO	____INT_CACHE_WRITE		;@0 - начальный адрес кэша @1размер кэша @2дата-рег
	//increment size
	lds	Zl,@0+2			;2 read SIZE
	cpi 	Zl,@1			;1	
	brne 	____FCW_no_overflow	;2
	sec	
	rjmp	____FCW_no_mem
____FCW_no_overflow:
	inc	Zl			;1
	sts	@0+2,Zl			;2
	//evaluate pointer
	lds	Zl,@0			;2 read WRITER offset
	ldi	Zh,high(@0)		;1
	st	Z+,@2			;2
	cpi	Zl,low(@0+@1)+3		;1
	brne	____FCW_no_correct	;1
	ldi	Zl,low(@0)+3		;1
____FCW_no_correct:
	sts	@0,Zl			;2
	clc				;1
____FCW_no_mem:
.ENDMACRO				;19 clock 15команд

//-------------------------------------------------------------
.MACRO	____INT_CACHE_READ		;@0 - начальный адрес кэша @1размер кэша @2дата-рег
	//increment size
	lds	Zl,@0+2			;2 read SIZE
	cpi 	Zl,1			;1	!!!хитрость!!!
	brcs 	____FCR_no_data		;2
	dec	Zl			;1
	sts	@0+2,Zl			;2
	//evaluate pointer
	ldi	Zh,high(@0)		;1
	lds	Zl,@0+1			;2 read READER pos
	ld	@2,Z+			;2
	cpi	Zl,low(@0+@1)+3		;1
	brne	____FCR_no_correct	;1
	ldi	Zl,low(@0)+3		;1
____FCR_no_correct:
	sts	@0+1,Zl			;2
	clc				;1
____FCR_no_data:			//19 тактов 13команд

.ENDMACRO
;**************************************************
.CSEG
.MACRO	____CACHE_WRITE		;@0 - начальный адрес кэша @1размер кэша 
	lds	r25,@0+2	;2
	cpi	r25,@1		;1
	sec
	breq	____CW_no_mem	;1
	ldi	Yh,high(@0)
	ldi	Yl, low(@0)
	ldi	r25,low(@0+@1+3)
	rcall	____OS_CACHE_WRITE
____CW_no_mem:	
.ENDMACRO

____OS_CACHE_WRITE:
	cli
	ldd	REG_INT_TEMP,Y+2;2	
	inc	REG_INT_TEMP	;1
	std	Y+2,REG_INT_TEMP;2	save size
	mov	Zh,Yh		;1
	ld	Zl,Y		;2	;writer poz
	st	Z+,r24		;2
	mov	r24,Zl		;1
	sei			;1	/12 тактов 
	//evaluate pointer
	cp	r25,r24
	brne 	____CW_no_correction
	ldi	r24,0x03
	add	r24,Yl		;из-за выравнивания перепoлнения не будет
____CW_no_correction:
	st	Y,r24
	;	clc		равен 0 по идее
	RET
//--------------------------------------------------
.MACRO	____CACHE_READ			;@0 - начальный адрес кэша @1размер кэша
	ldi	Yh,high(@0)
	ldi	Yl, low(@0)
	ldi	r25,low(@0+@1+3)
	rcall	____OS_CACHE_READ
.ENDMACRO

____OS_CACHE_READ:
	//evaluate pointer
	cli
	ldd	r24,Y+2		;2
	subi	r24,0x01
	brcs	____CR_no_read	
	std	Y+2,r24		;2
	mov	Zh,Yh		;1
	ldd	Zl,Y+1		;2	;reader poz
	ld	r24,Z+		;2
	cp	r25,Zl
	mov	r25,Zl
	sei
	brne	____CR_no_correction
	ldi	r25,0x03
	add	r25,Yl		;из-за выравнивания перепoлнения не будет
____CR_no_correction:	
	std	Y+1,r25
____CR_no_read:
	RETI

.EXIT
/********************************************************/

; file: LCDisme.asm
; routines pour utiliser l'écran à cristaux liquides
; by the mighty Roger Buser
;
; Pour afficher sur votre LCD:
;		1. initialiser le LCD avec "rcall	LCD_init" dans le reset
;		2. inclure ce fichier APRES le "rjmp main" 
;		3. insérer des lignes comme celles qui suivent dans le main:
;
; 	texte:	
;		msg:	.db		"inserez votre texte ici",0	; entre les guillemets doivent se trouver des caractéres en nombre impaire
;			TXT_LCD2	msg, 0x02, 0x02			; quoi, colonne, ligne
;	
;	décimal:	rcall	putdec					; dans a0 ce qu'on veut afficher
;	hexadécimal:	rcall 	puthex
;	
;	

;/////////////////////////////////////////////////////////////////
; ici vous definissez vos branchements physiques
.equ	LCD_PORT	= PORTB	; LCD port
.equ	LCD_RS		= 5		; LCD register select
.equ	LCD_E		= 4		; LCD enable pulse
.equ	LCD_D4		= 3		; LCD data4
.equ	LCD_D5		= 2		; LCD data5
.equ	LCD_D6		= 1		; LCD data6
.equ	LCD_D7		= 0		; LCD data7


;/////////////////////////////////////////////////////
; MACRO
.macro 	TXT_LCD2			; syntaxe:  TXT_LCD @0, @1, @2
					; ecrit sur le LCD, @0=étiquette du message @1=position @2=ligne
					; sauvegarde a0, affecte w
		push	a0
		ldi	a0, @2
		subi	a0, 0x01
		rol	a0		; 2ème ligne = 0x40 à 0x4f
		rol	a0		; on multiplie @2 par 64
		rol	a0
		rol	a0
		rol	a0
		rol	a0
		ldi	w, @1
		add	a0, w
		rcall	LCD_pos
		ldi	zl, low(2*@0)	;charge le pointeur dans z
		ldi	zh,high(2*@0)
		rcall	LCD_puts
		pop	a0
		
.endmacro

; routines for implementing a 4-bit LCD interface 

LCD_init:
; according to HITACHI doc on HD447890U (p.46)
	in	w,LCD_PORT-1	; read DDRx = PORTx-1
	
	ori	w,0b11111111	; make E,RS,D4-D7 output
	
	out	LCD_PORT-1,w	; write DDRx

	in	w,LCD_PORT		; read PORTx
	andi	w,0b11000000	; make E,RS,D4-D7 =0
	out	LCD_PORT,w		; write PORTx

	WAIT_MS	16
	
	ldi		w, 0b00110000	; 8-bit interface
	rcall	lcd_put_nibble	; dans lcd_put_nibble ce ne sont que les poids forts qui sont transmis
	WAIT_MS	8
	
	ldi		w, 0b00110000	; 8-bit interface
	rcall	lcd_put_nibble
	WAIT_MS	1
	
	ldi		w, 0b00110000	; 8-bit interface
	rcall	lcd_put_nibble
	WAIT_MS	1

	ldi		w, 0b00100000	; 4-bit interface
	rcall	lcd_put_nibble
	WAIT_MS	2

; 	this displays 8chars ($00..$07) followed by 8chars ($40..$47)
	ldi		w,0b00101000	; 4-bits, 2-line 5x8-dots
	rcall	lcd_wr_ir

	ldi		w,0b00001111	; disply=1, cursor=0, blink=0
	rcall	lcd_wr_ir
		
	ldi		w,0b00000001	; display clear
	rcall	lcd_wr_ir
	WAIT_MS	2
	ret

LCD_clear:		JW	LCD_wr_ir, 0b00000001	; clear display
LCD_home:		JW	LCD_wr_ir, 0b00000010	; return home
LCD_cursor_left:	JW	LCD_wr_ir, 0b00010000	; move cursor to left
LCD_cursor_right:	JW	LCD_wr_ir, 0b00010100	; move cursor to right
LCD_display_left:	JW	LCD_wr_ir, 0b00011000	; shifts display to left
LCD_display_right:	JW	LCD_wr_ir, 0b00011100	; shifts display to right
LCD_blink_on:		JW	LCD_wr_ir, 0b00001101	; Display=1,Cursor=0,Blink=1
LCD_blink_off:		JW	LCD_wr_ir, 0b00001100	; Display=1,Cursor=0,Blink=0
LCD_cursor_on:		JW	LCD_wr_ir, 0b00001110	; Display=1,Cursor=1,Blink=0
LCD_cursor_off:		JW	LCD_wr_ir, 0b00001100	; Display=1,Cursor=0,Blink=0
		
LCD_pos:
; in	a0 = position (0x00..0x0f first line, 0x40..0x4f second line)
	mov		w,a0
	ori		w,0b10000000
LCD_wr_ir:
; in	w	data byte to write to IR
	cbi		LCD_PORT,LCD_RS	; address IR
	rjmp	lcd_put_byte

LCD:
LCD_putc:
; in	a0	character to put to LCD
	JK	a0,CR,LCD_home	; Jump if a0=CR
	mov	w,a0	

LCD_wr_dr:
	sbi	LCD_PORT,LCD_RS	; address DR

lcd_put_byte:
; in	w	byte to put to LCD 4-bit interface
	push	w		; saveguarde w
	rcall	lcd_put_nibble	; put high nibble (D4..D7)
	pop	w		; restore w
	swap	w		; put low nibble (D0..D3)
; (fall through)

lcd_put_nibble:

	OUTB	LCD_PORT,LCD_D4, w,4
	OUTB	LCD_PORT,LCD_D5, w,5
	OUTB	LCD_PORT,LCD_D6, w,6
	OUTB	LCD_PORT,LCD_D7, w,7
		
	sbi	LCD_PORT,LCD_E	; pulse E=1
	WAIT_US	100
	cbi	LCD_PORT,LCD_E	; pulse E=0
	WAIT_MS	2
	ret


;////////////////////////////////////////////////////////////////////////
; affiche la valeur d'un registre pour pouvoir être lu directement en hex
puthex:
	push	a0
	swap	a0
	andi	a0, 0x0f
	rcall	puthex1
	pop	a0
	andi	a0, 0x0f
	rcall	puthex1
	ret
	
puthex1:
	cpi	a0, 10
	brsh	_af
	subi	a0,-'0'
	rcall	LCD_putc
	ret
_af:	subi	a0, -('a'-10)
	rcall	LCD_putc
	ret

;////////////////////////////////////////////////////////////////////////
; affiche la valeur d'un registre pour pouvoir être lu directement en dec
putdec:
; put decimal value 
; in 	a0	(value to convert)
;	putc 	(address of a routine to "write" the character)

	mov	u,a0		; number to convert is kept in u
	ldi	a0,'0'-1	; preload a0 (digit)
	ldi	w,100		; load the "hundreds"
_putdec2:	
	inc	a0
	sub	u,w		; subtract 100
	brsh	_putdec2	; until the result is negative
	add	u,w		; undo the last substraction
	push	u
	rcall	LCD_putc	; display the digit2
	pop	u
	ldi	a0,'0'-1	; preaload a0 (digit)
	ldi	w,10		; load the "tens"
_putdec1:	
	inc	a0
	sub	u,w		; subtract 10
	brsh	_putdec1	; until the result is negative
	add	u,w		; undo the last substraction
	push	u
	rcall	LCD_putc	; display digit1
	pop	u
	ldi	a0,'0'	
	add	a0,u
	rcall	LCD_putc	; display digit0
	ret

putdecs:
; put signed decimal value
; in 	a0	(value to convert)
;	putc 	(address of a routine to "write" the character)
	tst	a0		; test a0
	brpl	putdec		; if positive got to putdec
	push	a0		; save a0 temporarily 
	ldi	a0,'-'
	rcall	LCD_putc	; display the negative sign
	pop	a0		; restore a0
	neg	a0		; negate a0
	rjmp	putdec	
	
;------------------------------------------------------------------
; pour écrire plusieurs caractères dans le LCD

LCD_puts:
; put string imm.
	lpm						; transfert du caract dans r0
	tst		r0				; fin de la chaine = "0"
	breq	puts_fini		
	mov		a0,r0
	rcall	LCD_putc		; on balance le caractère
	adiw	zl,1			; incrément du pointer z
	rjmp	LCD_puts			; on affiche tant qu'il n'y a pas de "0"
puts_fini:
	ret

putsi:
	POPZ
	MUL2Z					; pointeur de byte (4 bits) ou mots (8 bits)
	rcall	LCD_putc
	DIV2Z
	adiw	zl,1
	ijmp
	
	
	
	


.include "m128def.inc"

 
 
 .equ FIFOSIZE = 8 ; størrelse på buffer
 .equ f_osc = 3686000 ; oscillator frekvens
 .equ timerverdi = (f_osc/64) ; hvor mange ganger timeren skal telle 
  							  ; før generert interupt (57593,75)

 .dseg
 .org 0x100 ; start sram

	sekunder      : .byte 1 ; init 59
	minutter      :	.byte 1 ; init 59
	timer         :	.byte 1 ; init 23
	fifo_buff     : .byte FIFOSIZE
	innpek        : .byte 2
	utpek         : .byte 2
	antall_i_buff : .byte 1


 
	 
 

 .cseg
 .org 0

 	rjmp init

 .org OC1Aaddr  ; output compare match
    rjmp timer1_avbrudd

 .org URXC1addr ; USART1 , Rx complete
	rjmp usart_avbrudd

 .org 0x46 

 init :

 	;init stakkpeker:

	ldi r16 , low(ramend)
	out spl , r16
	ldi r16 , high(ramend)
	out sph , r16


	; init porter , 0 = inn , 1 = ut

	clr r16
	out DDRA , r16 ; setter port A til inn
	ldi r16 ,0xff
	out DDRB , r16 ; port B ut

	; init klokke verdi

	ldi r16 , 1
	sts sekunder , r16
	ldi r16 , 30
	sts minutter , r16
	ldi r16 , 23
	sts timer , r16


;initialisering av timer 1 til å gi avbrudd hvert sekund
	

	;hva telleren skal telle til :
	ldi r16 , high(timerverdi)
	out OCR1AH , r16 ; skrive high , så low . motsatt på lesing
	ldi r16 , low(timerverdi)
	out OCR1AL , r16


	; setter prescaling til en 64 del
	ldi r16 , (1<<CS11)|(1<<CS10) ; eventuetlt 0x03
	out TCCR1B , r16
	

	; sørger for at telleren begynner å telle fra null
	clr r16
	out TCNT1H , r16
	out TCNT1L , r16


	; enable interrupt ; interrupt vektor : OC1Aadrr
	ldi r16 , (1<<OCIE1A) ; Output compare A match interrupt enable
	out TIMSK , r16 ; timer/counter interrupt mask register

;init usart :
 	call init_usart

;global interrupt enable
	sei


 loop :

 	;lese brytere
	
	; dersom bryter 0 er inne vis  minutter på lampene
	call les_brytere
	mov r16 , r0

	andi r16 , 0b00000001 ; er bryteren inne skal resultet bli 1
	brne vis_min
	jmp sjekkbryter1

	vis_min:
	call skriv_min
	rjmp slutt_loop
	
	
	; dersom bryter 1 er inne vis timer på lampene

	sjekkbryter1 :

	call les_brytere
	mov r16 , r0

	andi r16 , 0b00000010; er bryteren inne skal resultet bli 1
	brne vis_timer
	rjmp vis_sekunder

	vis_timer :
	call skriv_timer
	rjmp slutt_loop


	vis_sekunder:
	call skriv_sekunder
	
	
	

	;har det kommet en karakter ?
	lds r16 , antall_i_buff
	cpi r16 , 0
	breq slutt_loop ; gå til slutt hvis tomt

	
	;er karakteren en "t" ?

	call get_buf ; legger karakter i buffer i r0
	mov r16 , r0
	cpi r16 , 't'
	breq skriv_terminal

	;hvis ikke en t , bare skriv vanlig

	call skriv_usart ; skriver ut karakter i r0 ut på usart1
	rjmp slutt_loop


	skriv_terminal:
	
		;skriv ut klokka på terminal
	call skriv_klokke_terminal	


	slutt_loop:
	jmp loop




;***************************************************
;						SUBRUTINER
;***************************************************


;###################################################
;
; Subrutine les_brytere
;
; Leser av bryterene koblet til port A
;
; Inn : intet
;
; Ut R0 = avlest bitmønster
;
; Ingen andre registre forandres
;###################################################

 les_brytere :
 
    push r16

 	in r16 , pina
	com r16 ; HW
	mov r0 , r16
	
	pop r16
	
	ret
;---------------------------------------------------


;###################################################
;
; Subrutine skriv_lamper
;
; Skriver på lampene koblet til port B
;
; Inn : R0
;
; Ut : intet
;
; Ingen andre registre forandres
;###################################################

 skriv_lamper :

 	push r16

	mov r16 ,r0
	com r16 ; HW
	out portb , r16
	
	pop r16

	ret
;---------------------------------------------------

;###################################################
;
; Subrutiner for å skrive ut sekunder , minutter og
; timer på lampene
;
; Inn : sekunder , minutter , timer
; Ut : ( r0 får verdi )
;
; Ingen registre forandres
;###################################################

 skriv_sekunder :

	push r16
	lds r16 , sekunder
	mov r0 , r16
	call skriv_bcd
	pop r16
	ret
;---------------------------------------------------

 skriv_min :

	push r16
	lds r16 , minutter
	mov r0 , r16
	call skriv_bcd
	pop r16
	ret
;---------------------------------------------------

 skriv_timer :

	push r16
	lds r16 , timer
	mov r0 , r16
	call skriv_bcd
	pop r16
	ret

;---------------------------------------------------

;###################################################
;
; Subrutine skriv_bcd
; skriver et tall på lampene koblet til port B
; Inn : R0 , bitmønster som skal skrives
; Ut :intet
;
; Ingen registre forandres
;###################################################

 skriv_bcd :
	
	push r16
	in r16 , sreg
	push r16 ; enere
	push r17 ; tiere
	

	mov r16 , r0
	clr r17
	
	; større eller mindre enn ti ?
 tierloop :
	
	;sjekk tiere
	cpi r16 , 10
	brlo ferdig ; mindre enn ti -> ferdig

	;trekk fra ti og øk tiere med en
	subi r16 , 10
	inc r17
	rjmp tierloop

	ferdig: ;R17:R16 = BCD

	
	lsl r17
	lsl r17
	lsl r17
	lsl r17

	or r17 , r16

	mov r0 , r17
	call skriv_lamper


	pop r17
	pop r16
	out sreg , r16
	pop r16

	ret

;---------------------------------------------------



;###################################################
;
; Subrutine oppdater_klokke
;
; øker klokkeslettet med 1 sekund
;
; Inn : intet
;
; Ut : intet
;
; Ingen andre registre forandres
;###################################################

 oppdater_klokke :

 	push r16 
	in r16 , sreg
	push r16

	
	;sjekk sekunder
	lds r16 , sekunder
	cpi r16 , 59
	brsh reset_sek_inc_min	
	
	;øk sekunder med en :
	inc r16
	sts sekunder , r16
	rjmp slutt_oppdater


	reset_sek_inc_min :

	; reset sek
	clr r16
	sts sekunder , r16

	;sjekk min
	lds r16 , minutter
	cpi r16 , 59
	brsh  reset_min_inc_timer


	;øk minutter med en
	inc r16
	sts minutter , r16
	rjmp slutt_oppdater



	reset_min_inc_timer:
	
	;reset min
	clr r16
	sts minutter , r16
	 	

	;sjekk timer
	lds r16 , timer
	cpi r16 , 23
	brsh reset_timer
	
	;øk timer
	inc r16 
	sts timer , r16
	
	reset_timer :
	
	clr r16
	sts timer , r16

	 
	slutt_oppdater :
	
	pop r16
	out sreg , r16
	pop r16

	ret

;---------------------------------------------------







;***************************************************
;
; avbrudds rutien timer 1 output compare match
;
; øker "teller" med en for hvert interrupt
;
; Ingen registre forandres
;***************************************************

 timer1_avbrudd:

	push r16
	in r16 , sreg
	push r16
	push r17
	push r18
	
	;oppdater klokke
	call oppdater_klokke
	
	; legge til timerverdi til OCR1A , slik at det går et sekund 
	; til neste avbrudd	

	; henter OCR verdi
	
	in r16 , OCR1AL ; lese low så high
	in r17 , OCR1AH ; R17:R16 = OCR

	; legger til timerverdi til OCR

	ldi r18 , low(timerverdi)
	add r16 , r18
	ldi r18 , high(timerverdi)
	adc r17 , r18 ; R17:R16 = OCR + timerverdi

	; skrive tilbake til OCR , high så low

	out OCR1AH , r17
	out OCR1AL , r16

	pop r18
	pop r17
	pop r16
	out sreg , r16
	pop r16

	reti

;-----------------------------------------------------


;
;##############################################
;
; subrutine init_usart
;
; Initialiserer USART1 til 8 databits , 1 stop-
; bit , ingen paritet , 9600 baud . 
; Det generes avbrudd når en karakter er mottatt
;##############################################

 init_usart :

	;setter usart til 8 databits. 1 stopbit og ingen paritet er dafault ; null
		
	push r16

	ldi r16 , (1<<UCSZ11)|(1<<UCSZ10)
	sts UCSR1C , r16
	
	;setter baudrate til 9600 baud (U2X = 0 , ikke dobbel hastigheet)
	
	ldi r16 , high(23)
	sts UBRR1H , r16 ; skriv high , så low
	ldi r16 , low(23)
	sts UBRR1L , r16

	; USART recieve enable ,transmitt enable , Recieve complete interrupt enable

	ldi r16 , (1<<RXCIE1)|(1<<RXEN1)|(1<<TXEN1)
	sts UCSR1B , r16
	
	; nullsette usart status register a

	clr r16
	sts UCSR1A , r16

	pop r16
	ret
;----------------------------------------------



;##############################################
;
; subrutine skriv_usart
;
; skriver karakteren i r0 på USART1
;##############################################

 skriv_usart :
	
	push r16

	vent_send :

 	lds r16 , UCSR1A ; her ligger flagget UDRE1 ,usart data register empty
	sbrs r16 , UDRE1 ; hopper over neste instruksjone hvis bittet er satt
	rjmp vent_send


	;call get_buf
	mov r16 , r0
	sts UDR1 , r16

	pop r16

	ret

;-----------------------------------------------



;**********************************************
; Avbruddsrutine Usart_avbrudd
; 
;Leser den mottatte karakteren fra USART1 og legger
; den i variabelen usart_buffer
;Setter flagget Usart_buffer_fullt

;**********************************************

 usart_avbrudd :

	push r16
	in r16 , sreg
	push r16
	
	;leser inn karakter som er motatt i r0 -> buffer
	lds r0 , UDR1
	call put_buf
	
	

	pop r16
	out sreg , r16
	pop r16

	reti

;---------------------------------------------


;#######################################################
;
; Subrutine skriv_tall_usart
; 
;skriver det 8-bits binærtallet i R0 som et tosifret desimaltall
; på usart 1
;to karakterer skrives med ascii kode
;(forutsetter at tallet er mindre enn 100)
;#######################################################

 skriv_tall_usart :
 
	push r16 ;enere
	in r16 , sreg
	push r16
	push r17 ;tiere
	push r18

	clr r17
	mov r16 , R0

    tierloop_usart :
	; er tallet mindre enn ti ?
	
	cpi r16 , 10
	brlo ferdig_tierloop
	
	;trekk fra ti og øk tiere med en
	
	subi r16 , 10
	inc r17
	rjmp tierloop_usart

	ferdig_tierloop :
	
	;skriv ut de to karkaterene (enere og tiere)
	 ;skriver ut tiere på usart 1 :
	ldi r18 , 0x30
	add r17 , r18
	mov r0 , r17
	call skriv_usart
	
	 ;skriv ut enere
	add r16 , r18
	mov r0 , r16
	call skriv_usart
	
	pop r18
	pop r17
	pop r16
	out sreg , r16
	pop r16
	
	ret
	
;------------------------------------------

;###########################################
;
; Subrutine skriv_klokke_terminal	
;
;skriver klokken ut på terminalen
;
;inn : timer , minutter , sekunder
;ut : intet
;###########################################

 skriv_klokke_terminal :
 

 	push r16
	;timer
	lds r16 , timer
	mov r0 , r16
	call skriv_tall_usart ; skriver r0 på BCD
	ldi r16 , ':'
	mov r0 , r16
	call skriv_usart
	;minutter
	lds r16 , minutter
	mov r0 , r16
	call skriv_tall_usart
	ldi r16 , ':'
	mov r0 , r16
	call skriv_usart
	;sekunder
	lds r16 , sekunder
	mov r0 , r16
	call skriv_tall_usart

	ldi r16 , 0x0A ; linjeskift
	mov r0 , r16
	call skriv_usart
	ldi r16 , 0x0D ; vognretur
	mov r0 , r16
	call skriv_usart

 	pop r16

	ret
;-----------------------------------------------

;###########################################
;
; Subrutine init_fifo	
;
;inn : intet
;ut : intet
;ingen registre forandres
;###########################################
 
 init_fifo :

	push r16

	; setter innpek og utpek i begynnelse av fifo_buffer
	ldi r16 , low(fifo_buff) ; mist signifikante 
							 ; bit i addr til fifo_buff
	sts innpek , r16
	sts utpek , r16

	ldi r16 , high(fifo_buff)
	sts innpek+1 , r16 ; mest sig. bit addr i høyeste byte
	sts utpek+1 , r16

	; setter antall_i_buff = 0
	clr r16
	sts antall_i_buff , r16

	pop r16
	ret
;-------------------------------------------



;###########################################
;
; Subrutine put_buf	
;
;skriver en karakter inn i fifo-buffer
;Dersom bufferet er fullt gjøres intet
;
;inn : R0 , karakter
;ut : intet
;ingen registre forandres
;###########################################

 put_buf:
	
	push r16
	push r17
	push XL
	push XH
	
	; er det plass i bufferet ?
	
	lds r16 , antall_i_buff
	cpi r16 , FIFOSIZE	
	brsh put_buf_slutt
	
		;hvis det er plass skal karakter lagres der innpek peker og innpek økes med en
		 ; setter X lik innpek

		 lds XL , innpek
		 lds XH , innpek+1

		 ;lagrer karakter

		 st X+ , r0 ; lagerer der x peker og øker x med en

		 ; øker antall i buff med en
		 inc r16
		 sts antall_i_buff , r16

		 ; er vi kommet til enden av bufferet ?

		 ldi r16 , low(fifo_buff + FIFOSIZE)
		 ldi r17 , high(fifo_buff + FIFOSIZE)
		 cp XL , r16
		 cpc XH , r17
		 brne fortsett
		
		; hvis vi er kommet til enden av bufferet må vi sette
		; pekeren i begynnelsen
		; begynnelsen av buffer = fifo_buff

		 ldi XL , low(fifo_buff)
		 ldi XH , high(fifo_buff)


		 
		 fortsett: ; har ikke kommet til enden av bufferert
		 			; setter innpek lik X

		 sts innpek , XL
		 sts innpek+1 , XH
		 

  put_buf_slutt :
	pop XH
	pop XL
	pop r17
	pop r16
	ret


;-----------------------------------------------


;###########################################
;
; Subrutine get_buf	
;
;Henteren karakter fra fifo-bufferet .
;Dersom bufferet er tomt ventes det til en
;karakter er skrevet i bufferet
;
;inn : intet
;ut : R0 , karakter
;ingen registre forandres
;###########################################

 get_buf :

 	push r16
	push r17
	push XL
	push XH

		; er bufferet tomt ?
		tomt:
		lds r16 , antall_i_buff
		cpi r16 ,0
		brne tomt

		;setter X lik utpek

		lds XL , utpek ; adressen er 16 bit , innholdet 8 bit
		lds XH , utpek+1
	
		;skriver karakteren som ligger der X peker til R0 og øker X med en ( peker på neste verdi som skal ut)

		ld r0 , X+
		
		; minker antall i buffer med en siden karakteren nå er skrevet ut
		
		dec r16
		sts antall_i_buff , r16

		; er pekeren i bunnen av bufferet ?
		
		lds r16 , low(fifo_buff + FIFOSIZE)
		lds r17 , high(fifo_buff + FIFOSIZE)
		cp XL , r16
		cpc XH , r16
		brlo ok_fortsett ; er ikke pekerern kommet til bunne kan
						 ; utpek settes lik X , hvis ikke settes utpek lik 
						 ; fifo_buf ( start på buffer)
		
			; setter utpek lik fifo_buff
			lds r16 , low(fifo_buff)
			lds r17 , high(fifo_buff)
			sts utpek , r16
			sts utpek+1 , r17
			jmp get_buf_slutt 
	
	
	ok_fortsett :
	;setter utpek lik X

	sts utpek ,XL
	sts utpek+1 , XH

	get_buf_slutt :

	pop XH
	pop XL
	pop r17
	pop r16
	ret
;--------------------------------------------



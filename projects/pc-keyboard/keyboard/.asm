.include "m48def.inc"

;takt 20MHz
;efuse=0xFF
;hfuse=0xDC	(BOD 4,1-4,5 V)
;lfuse=0xF7

;pokazivac na notu je Z
;pokazivac na ulaz u bafer je X
;pokazivac na izlaz iz bafera je Y

;PR-prihvatni registri(74hc165)
;SR-scan registri(74hc164)

.equ     ainnum=6		;broj analognih ulaza
.equ     minvel=0		;minimalni velocity
.equ     t2flag=0

.ESEG
.org 0x0000
midich: 	.DB  1		;midi kanal
	.DB  0		;nista za sada
ccnum:	.DB  16		;cc brojevi dodeljeni potovima
	.DB  17
	.DB  18
	.DB  19

.DSEG
.org 0x0100
buffer:  .BYTE 256	;bafer za UART
.org 0x200
notes:   .BYTE 128  	;niz za velocity
.org 0x0280
ains:	.BYTE ainnum	;niz za rezultate AD konverzija - od 0x0280 do 0x002FFF-max dubina steka

.def     adcdly=r12	;za usporavanje AD konvertora(3*(6*96us)=1728us po potu)
.def     mchan=r13	;ovde je midi kanal
.def     vmax0=r14	;ovde cuvas 127
.def     vmax1=r15	;ovde cuvac 255
.def     var1=r16
.def     var2=r17
.def     var3=r18
.def     var4=r19
.def     var5=r20
.def     adcnt=r21 	;tekuci kanal AD konvertora
.def     data1=r22	;za prenos u izlazni bafer
.def     data2=r23	;	-||-
.def     data3=r24	;	-||-
.def     flags=r25	;za razne flegove


;*************************************************************************************
;*************************************************************************************
.CSEG
.org 0
	rjmp   Reset
	reti
	reti
	reti
	reti
	reti
	reti
	rjmp   T2OCA	;skeniranje svakih 256 us
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	reti
	rjmp   UTxC	;podatak poslat
	rjmp   ADCC	;konverzija zavrsena
	reti
	reti
	reti
	reti

;*************************************************************************************
Reset:   
				;stek se automatski inicijalizuje
	ldi    var1,low(midich)	;ucitaj midi kanal iz eeproma
	out    EEARL,var1
	sbi    EECR,EERE
	in     mchan,EEDR
	ldi    var1,127		;inicijalizuj konstante
	mov    vmax0,var1
	ldi    var1,255
	mov    vmax1,var1
	ldi    YH,high(buffer)	;bafer pocinje od pocetka RAM-a(i prazan je)
	ldi    YL,low(buffer)	;  X je ulaz u bafer
	movw   XL,YL		;  Y je izlaz iz bafera
	clr    adcnt
	clr    adcdly
	
	ldi    var1,0b00000000	;inicijalizacija portova
	out    DDRB,var1
	ldi    var1,0b00111110
	out    PORTB,var1
	ldi    var1,0b00000000
	out    DDRC,var1
	out    PORTC,var1		;  u inic. ADC-a ces iskljuciti PORTC dig. bafere
	ldi    var1,0b00010111
	out    DDRD,var1
	ldi    var1,0b11100000
	out    PORTD,var1

	ldi    var1,19		;inicijalizacija T2
	sts    OCR2A,var1		;  OCR2A=0x19
	ldi    var1,0x02
	sts    TCCR2A,var1	;  CTC,otkacen sa izlaza
	sts    TIMSK2,var1	;  OCIE2A=1
	ldi    var1,0x06
	sts    TCCR2B,var1	;  1:256 preskaler,T=Tclk*256*(1+19)=256us

	ldi    var1,39		;inicijalizacija USART-a
	sts    UBRR0L,var1	;  31250kb/s
	ldi    var1,0x48
	sts    UCSR0B,var1	;  TXCIE,TXEN tj. int se generise kada je podatak poslat a u UDR0 nije upisan novi podatak(u UDR0 se podatak upisuje samo kada je bafer prazan a dolazi novi podatak,inace u UTxC int proceduri)

	ldi    var1,0x3F		;inicijalizacija AD konvertora
	sts    DIDR0,var1		;  iskljuci digitalne bafere
	ldi    var1,0x60
	sts    ADMUX,var1		;  ADLAR=1,Vref=AVcc
	ldi    var1,0x8F
	sts    ADCSRA,var1	;  ADIE=1(ADC jos nije startovan),takt je 1:128

	rcall  Wait		;pauza 2sek
	rcall  InitSR		;inicijalizacija scan registra
	rcall  InitRAM 		;inicijalizacija nota(sve vel na 127,flegove na 0)
	rcall  InitADC		;pocetna konverzija svih ulaza

	sbi    TIFR2,OCF2A
	
	sei

;*************************************************************************************
;*************************************************************************************
Start:   
	rjmp   Start		;ovde stavi ocitavanje funkc.tastera,displej itd.
;*************************************************************************************
;*************************************************************************************
T2OCA:				;ovde dolazis svakih 256 us
	push   var2
	push   var1
	in     var1,SREG		;sacuvaj SREG
	push   var1
	push   ZL
	push   ZH
	ldi    ZH,high(notes)	;note pocinju od pola RAM-a
	ldi    ZL,low(notes)
	sbrc   flags,t2flag	;da T2 ne bi prekidao sam sebe,ako je t2flag=1 onda je ova procedura vec u toku
	rjmp   t2end
	sbr    flags,t2flag
	sei			;da USART i ADC ne bi cekali
nxtrw:	cbi    PORTD,4		;ucitaj red
	sbi    PORTD,4		;pa pomeri RS(ubacena 1)
	;dly(za smirenje signala na PIND3/PIND4?)
nxtnt:	ld     var1,Z		;var1=(Z)
	bst    var1,7		;T=flag
	sbis   PIND,3		;u kojoj je poziciji taster?
	rjmp   pos1
	sbis   PINB,0
	rjmp   pos2
	rjmp   pos3
pos1:	brtc   PC+3		;pozicija 1
	st     Z+,vmax1		;  ako je flag=1 onda vel=127,flag=1 i idi na sledecu notu
	rjmp   cont
	cli			;  ako je flag=0 salji notu(Note Off,broj,velocity)
	ldi    data1,0x80
	mov    data2,ZL
	mov    data3,var1
	rcall  Send3
	st     Z+,vmax1		;  		vel=127,flag=1
	sei
	rjmp   cont
pos2:    brts   PC+3		;pozicija 2
	st     Z+,vmax0		;  ako je flag=0 vel=127,flag=0
	rjmp   cont
	cli
	ldi    data1,0x90		;  ako je flag=1 salji notu(Note On,broj,velocity(sa flegom=0))
	mov    data2,ZL
	mov    data3,var1
	cbr    data3,0b10000000
	rcall  Send3
	st     Z+,vmax0		;		vel=127,flag=0
	sei
	rjmp   cont
pos3:	mov    var2,var1		;pozicija 3
	cbr    var2,0b10000000	;  ako velocity nije minvel,dekrementiraj velocity(flag ne diraj)
	cpi    var2,minvel
	breq   PC+2
	dec    var1
	st     Z+,var1
cont:	sbi    PORTD,2		;pomeri PR
	;dly
	cbi    PORTD,2
	;dly(za smirenje signala)?
	ldi    var1,0b00000111	;ako nota(mod 8) nije 0
	and    var1,ZL
	breq   PC+2
	rjmp   nxtnt		;  onda idi na sledecu notu
	sbrs   ZL,7		;  ako jeste,ali nije 128
	rjmp   nxtrw		;  onda idi na sledeci red
	cbi    PORTD,0		;  ako je nota 128 onda se vrati na prvi red
	cbi    PORTD,4		;  (plus ucitas u PR ali to se nece koristiti)
	sbi    PORTD,4
	sbi    PORTD,0		;opet postaviti ulaz RS na 1
	cbr    flags,t2flag
t2end:	pop    ZH
	pop    ZL
	pop    var1		;vrati SREG
	out    SREG,var1
	pop    var1
	pop    var2
	reti
;************************************************************************************
;************************************************************************************
Send3:				;ovde ulazis sa data1,data2,data3
	mov    var1,YL		;da li ima mesta u baferu?
	sub    var1,XL
	andi   var1,3		;  (tj da li je Y-X razlicito od 1,2,3)
	brpl   PC+2
	ret	  		;  nema mesta,izadji
	add    data1,mchan	;  ima mesta,spremi prvi podatak(status)
	cp     XL,YL		;  da li je bafer prazan?
	brne   PC+4
	sts    UDR0,data1		;    da,upisi prvi podatak u UDR
	rjmp   PC+3		
	st     X,data1		;    ne,upisi prvi podatak u bafer
	inc    XL
	st     X,data2		;    upisi drugi podatak u bafer
	inc    XL
	st     X,data3		;    upisi treci podatak u bafer
	inc    XL
	ret
;************************************************************************************
;************************************************************************************
UTxC:				;UART poslao podatak a u UDR-u nema novih podataka 
	push   var1
	in     var1,SREG		;sacuvaj SREG
	push   var1
	cp     XL,YL		;da li je bafer prazan?
	breq   PC+5		;  da,izadji(TXC0 se autom.resetovao)
	ld     var1,Y		;  ne,prebaci (Y+) u UDR
	sts    UDR0,var1
	inc    YL
	pop    var1		;vrati SREG
	out    SREG,var1
	pop    var1
	reti
;************************************************************************************
;************************************************************************************
ADCC:				;konverzija gotova,interapti nisu dozvoljeni
	push   var1
	push   var2
	in     var1,SREG		;sacuvaj SREG
	push   var1
	push   ZL
	push   ZH

	inc    adcdly		;usporavanje AD konvertora(3 puta)
	mov    var1,adcdly
	cpi    var1,2
	breq   PC+5		;koristi se tek treca konverzija
	ldi    var1,0xCF		;startuj novu konverziju 
	sts    ADCSRA,var1
	rjmp   adcend
	clr    adcdly

	lds    var2,ADCH
	lsr    var2		;teba ti samo najvisih 7 bita
	ldi    ZL,low(ains)
	ldi    ZH,high(ains)
	add    ZL,adcnt		;vrednosti kanala se cuvaju pocev od adrese 0x0280 tj mozes imati 128 kanala a da se ZH ovde nece menjati
	ld     var1,Z
	cp     var1,var2		;da li se vrednost promenila?
	breq   adcont		;  ne,idi na adcont
	st     Z,var2		;  da,prvo sacuvaj novu vrednost
	ldi    ZL,low(adjmp)
	ldi    ZH,high(adjmp)
	clr    var1		;   !!!!!!!ostavi samo 2. red ako se nece prevrteti
	add    ZL,adcnt
	adc    ZH,var1
	ijmp   
adjmp:	rjmp   mod		;mod
	rjmp   pitch		;pitch
	rjmp   cc		;pot1
	rjmp   cc		;pot2
	rjmp   cc		;pot3
	rjmp   cc		;pot4
mod:     ldi    data1,0xB0
	ldi    data2,1
	mov    data3,var2
	rcall  Send3
	rjmp   adcont
pitch:	ldi    data1,0xE0
	ldi    data2,0		;LSB
	mov    data3,var2		;MSB
	rcall  Send3
	rjmp   adcont
cc:	ldi    data1,0xB0
	out    EEARL,adcnt
	sbi    EECR,EERE
	in     data2,EEDR
	mov    data3,var2
	rcall  Send3
adcont:	inc    adcnt		;predji na sledeci kanal
	cpi    adcnt,ainnum	;  ako je prosao poslednji onda se vrati na prvi
	brne   PC+2
	clr    adcnt
	ldi    var1,0x60
	or     var1,adcnt
	sts    ADMUX,var1
	;treba li ovde nop(procitaj sledeci komentar,za sada, posto radis 2 prazne konverzije pre prave, nop ti ne treba)
	ldi    var1,0xCF		;startuj konverziju(!!!!!!!!!start konverzije pocinje nakon prve uzlazne ivice ADCclk, a semplovanje nakon 1.5 ADCclk,tako da za 1:128 imas oko 10us da se ulaz u ADC stabilizuje?) 
	sts    ADCSRA,var1
adcend:	pop    ZH
	pop    ZL
	pop    var1
	out    SREG,var1
	pop    var2
	pop    var1
	reti

;*************************************************************************************
;*************************************************************************************
Wait:				;~data1 * 10ms (@20MHz)
	ldi    data1,255
	ldi    data2,255
	ldi    data3,255
	dec    data3
	brne   PC-1
	dec    data2
	brne   PC-4
	dec    data1
	brne   PC-7
	ret

Dly:	
	ldi    data1,50
	ldi    data2,255
	ldi    data3,255
	dec    data3
	brne   PC-1
	dec    data2
	brne   PC-4
	dec    data1
	brne   PC-7
	ret

;***********************************
InitSR:  			;inicijalizacija SR
	;push   var1
	sbi    PORTD,0		;postavljas ga na prvi red
	ldi    var1,15
	cbi    PORTD,4
	sbi    PORTD,4
	dec    var1
	brne   PC-3
	cbi    PORTD,0
	cbi    PORTD,4
	sbi    PORTD,4
	sbi    PORTD,0
	;pop    var1
	ret
;***********************************
InitRAM: 			;inicijalizacija nota
	;push   var1
	;push   ZH
	;push   ZL
	ldi    ZH,high(notes)
	ldi    ZL,low(notes)
	ldi    var1,255		;svi tasteri su u pocetnom polozaju(pos1),flag=1
	st     Z,var1
	inc    ZL
	brpl   PC-2
	;pop    ZL
	;pop    ZH
	;pop    var1
	ret
;***********************************
InitADC: 			;inicijalizacija pocetnih vrednosti analognih ulaza
	clr    adcnt
	;push   var1
	;push   ZH
	;push   ZL
	ldi    ZL,low(ains)
	ldi    ZH,high(ains)
iadc1:	ldi    var1,0x60
	or     var1,adcnt
	sts    ADMUX,var1		;postavi kanal
	ldi    var1,0xCF
	sts    ADCSRA,var1	;startuj konverziju 
	lds    var1,ADCSRA	;kraj konverzije?
	sbrc   var1,ADSC
	rjmp   PC-3		;ne,goto PC-3(lds je 2 word-a)
	lds    var1,ADCH		;da,rezultat konverzije ce biti pocetna vrednost(ovo se ne salje na midi out vec se ceka prva promena)
	asr    var1		;	treba ti samo 7 MSB-a
	st     Z+,var1		;        i to sacuvaj u tabeli vrednosti analognih ulaza(pocinje na adresi ains)
	inc    adcnt
	cpi    adcnt,6
	brne   iadc1
	ldi    var1,0xDF		;startuj konverziju 
	sts    ADCSRA,var1
	clr    adcnt
	;pop    ZL
	;pop    ZH
	;pop    var1
	ret
;*************************************************************************************
;*************************************************************************************

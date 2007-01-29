
.def X0	= r8
.def X1	= r9
.def X2	= r10
.def X3	= r11
.def Y0	= r12
.def Y1	= r13
.def Y2	= r14
.def Y3	= r15
.def W0 = r16
.def W1 = r17
.def W2 = r18
.def W3 = r19



#define lsl_Y	lsl Y0	\
				rol Y1	\
				rol Y2	\
				rol Y3 

#define lsr_Y	lsr Y3	\
				ror Y2	\
				ror Y1	\
				ror Y0 

#define load_X	ldd X0,Z+0	\
				ldd X1,Z+1	\
				ldd X2,Z+2	\
				ldd X3,Z+3

#define clr_Y	clr Y0	\
				clr Y1	\
				clr Y2	\
				clr Y3


// (X,W) = X * Y (non signés),X et Y doivent être initialisés, utilise aussi (xh,xl), X est bien sûr détruit
mulu32:
	clr xh
//X0
	mov xl,X0
	mul xl,Y0
	mov W0,r0
	mov W1,r1
	clr W2
	clr W3
	clr X0
	mul xl,Y1
	add W1,r0
	adc W2,r1
	adc W3,xh
	mul xl,Y2
	add W2,r0
	adc W3,r1
	adc X0,xh
	mul xl,Y3
	add W3,r0
	adc X0,r1
//X1
	mov xl,X1
	clr X1
	mul xl,Y0
	add W1,r0
	adc W2,r1
	adc W3,xh
	adc X0,xh
	adc X1,xh
	mul xl,Y1
	add W2,r0
	adc W3,r1
	adc X0,xh
	adc X1,xh
	mul xl,Y2
	add W3,r0
	adc X0,r1
	adc X1,xh
	mul xl,Y3
	add X0,r0
	adc X1,r1
//X2
	mov xl,X2
	clr X2
	mul xl,Y0
	add W2,r0
	adc W3,r1
	adc X0,xh
	adc X1,xh
	adc X2,xh
	mul xl,Y1
	add W3,r0
	adc X0,r1
	adc X1,xh
	adc X2,xh
	mul xl,Y2
	add X0,r0
	adc X1,r1
	adc X2,xh
	mul xl,Y3
	add X1,r0
	adc X2,r1
//X3
	mov xl,X3
	clr X3
	mul xl,Y0
	add W3,r0
	adc X0,r1
	adc X1,xh
	adc X2,xh
	adc X3,xh
	mul xl,Y1
	add X0,r0
	adc X1,r1
	adc X2,xh
	adc X3,xh
	mul xl,Y2
	add X1,r0
	adc X2,r1
	adc X3,xh
	mul xl,Y3
	add X2,r0
	adc X3,r1
	ret
 
/*********************************************************************************/

// W = X / Y (non signés),X et Y doivent être initialisés, utilise aussi (xh,xl)
// erreur = CY
div32par32:
	mov xl,Y0
	or xl,Y1
	or xl,Y2
	or xl,Y3
	breq div0
//*********************************************
	clr W0
	clr W1
	clr W2
	clr W3
	ldi xh,1
cp_XY:
	cp	X0,Y0
	cpc	X1,Y1
	cpc	X2,Y2
	cpc X3,Y3
	brcs suite_divis
	lsr X3
	ror X2
	ror X1
	ror X0
	ror W3
	ror W2
	ror W1
	ror W0
	inc xh
	rjmp cp_XY
suite_divis:
//*********************************************
	clc
	clr xl
	rjmp decal_G
soustr:
	sub	X0,Y0
	sbc	X1,Y1
	sbc	X2,Y2
	sbc X3,Y3
	brcc add_2pN
	add	X0,Y0
	adc	X1,Y1
	adc	X2,Y2
	adc X3,Y3
	clc
decal_G:
	rol W0
	rol W1
	rol W2
	rol W3
	rol X0
	rol X1
	rol X2
	rol X3
	dec xh
	brne soustr
	clc
	ret
add_2pN:
	sec
	rjmp decal_G
div0:
overflow:
	ser xh
	mov W0,xh
	mov W1,xh
	mov W2,xh
	mov W3,xh
	sec
	ret



//calcul Y=racine(*X) : X pointé par (zh,zl) sur 4 octets
sqrt:
	load_X
	ldi xl,30
	sbrc X3,7
	rjmp fin_decal
//test si nul
	mov xh,X0
	or xh,X1
	or xh,X2
	or xh,X3
	brne non_nul
	clr_Y
	ret
non_nul:
// Y=X
	movw Y0,X0
	movw Y2,X2
decal_gauche:
	lsl_Y
	dec xl
	sbrs Y3,7
	rjmp decal_gauche
fin_decal:
	clr_Y
	inc xl
	mov xh,xl
	lsr xh
	subi xh,8
	brcc sup_7
	subi xh,(256-8)
	brne inc_Y0
	inc Y0
	rjmp calc_suite
inc_Y0:
	inc Y0
dg_Y0:
	lsl Y0
	dec xh
	brne dg_Y0
	sbrs xl,0		//si xl impair, ajouter Y/2
	rjmp calc_suite
	mov xh,Y0
	lsr xh
	add Y0,xh
	rjmp calc_suite
sup_7:
	brne inc_Y1
	inc Y1
	sbrs xl,0		//si xl impair, ajouter Y/2
	rjmp calc_suite
	set
	bld Y0,7
	rjmp calc_suite
inc_Y1:
	inc Y1
dg_Y1:
	lsl Y1
	dec xh
	brne dg_Y1
	sbrs xl,0		//si xl impair, ajouter Y/2
	rjmp calc_suite
	mov xh,Y1
	lsr xh
	add Y1,xh
calc_suite:			//Yn+1=(Yn + X/Yn)/2
	rcall div32par32
					//stop lorsque |X/Yn - Yn|<2
	movw X0,Y0
	movw X2,Y2
	sub X0,W0
	sbc X1,W1
	sbc X2,W2
	sbc X3,W3
	brcs tst_neg
	or X3,X2
	or X3,X1
	brne n_plus_1
	mov xl,X0
	subi xl,1
	brcc n_plus_1
	rjmp last_time
tst_neg:
	ldi xl,2
	clr xh
	add X0,xl
	adc X1,xh
	adc X2,xh
	adc X3,xh
	brcc n_plus_1
last_time:
	add Y0,W0
	adc Y1,W1
	adc Y2,W2
	adc Y3,W3
	lsr_Y
	ret
n_plus_1:
	add Y0,W0
	adc Y1,W1
	adc Y2,W2
	adc Y3,W3
	lsr_Y
	load_X
//arrondi pour div : X=X+Y/2
	movw W0,Y0
	movw W2,Y2
	lsr W3
	ror	W2
	ror	W1
	ror	W0
	add X0,W0
	adc X1,W1
	adc X2,W2
	adc X3,W3
	brcc calc_suite
	ser xl
	mov X0,xl
	mov X1,xl
	movw X2,X0
	rjmp calc_suite



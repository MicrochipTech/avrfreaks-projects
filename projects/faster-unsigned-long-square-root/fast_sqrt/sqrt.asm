
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



#define lsl_X	lsl X0	\
				rol X1	\
				rol X2	\
				rol X3 


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


/*
calcul (zh,zl)=racine(X) : unsigned long X; W : remainder sur 3 octets

    d'après INTSQRT.C (Merci EdPugh :o)

    Given a uint32 number, n, it calculates m, the INTEGER square root of n, and
    the "remainder", k, such that m^2 + k = n for the largest non-negative m and
    smallest non-negative k.

    Note:
    If you want to ignore the "remainder" and get the CLOSEST integer square
    root, m, where ABS((m^2)-n) is the smallest possible value, then:

    If the "remainder" calculated by this program is strictly larger than the
    calculated square root, then add 1 to the square root.

    SO:
    if ("remainder" <= square root) leave square root as is.
    if ("remainder > square root) increment the square root by 1.
 */
sqrt:
	ldi xl,32		//bit counter
	clr W0			//REMAINDER = 0;
	clr W1
	clr W2
//recherche du (couple de) bit(s) de poids forts
msb_bit:
	lsl_X
	rol W0
	lsl_X
	rol W0
	subi xl,2
	brcs fin_sqrt0
	tst W0
	breq msb_bit
//valeurs initiales de boucle
	dec W0			//REMAINDER -= 1ul;
	ldi zl,1		//squareRoot = 1; : racine en (zh,zl)
	clr zh
	tst xl
	breq fin_sqrt1 
while_bitCount:
	lsl_X
	rol	W0
	rol	W1
	rol	W2
	lsl_X
	rol	W0
	rol	W1
	rol	W2
	movw Y0,zl		//calcul Y = testValue = squareRoot*4 + 1, sur 3 octets
	clr Y2
	lsl Y0
	rol Y1
	rol Y2 
	lsl Y0
	rol Y1
	rol Y2 
	inc Y0			//Y0 est pair après "lsl" donc CY=0
	lsl zl
	rol zh			//squareRoot <<= 1
	cp W0,Y0		//if ( testValue <= REMAINDER )
	cpc W1,Y1
	cpc W2,Y2
	brcs tst_bit_count	//if not
	sub W0,Y0		//REMAINDER -= testValue;
	sbc W1,Y1
	sbc W2,Y2
	adiw zl,1		//squareRoot++;
tst_bit_count:
	subi xl,2
	brne while_bitCount
	ret

fin_sqrt0:
	clr zl
	clr zh
fin_sqrt1:
	ret

arrondi:
	clr xl
	cp zl,W0
	cpc zh,W1
	cpc xl,W2
	brcc fin_sqrtn
	adiw zl,1
fin_sqrtn:
	ret

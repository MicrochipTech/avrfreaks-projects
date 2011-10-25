;compiler: avrasm

;************************************************************************************************
; Embeded MOD/S3M player for AVR ATmega644.
;
; module: some math routines
;************************************************************************************************
; Licence: This source code is free to use for any non-comercial purposes.
;          (C) 2011, Stanislav Maslan, s.maslan@seznam.cz 
;************************************************************************************************




;------------------------------------------------------------------------------------
; div8u - 8/8 bit unsigned division, max. speed optimized
; deleni dvou neznaminkovych 8-bit cisel, maximalne optimalizovano na rychlost
;
; dDd8u <- dDd8u div dDs8u
; dRe8u <- dDd8u mod dDs8u
;
; words:  43 + ret (puvodni od ATMELu: 66 + ret)
; cycles: 43 + ret (puvodni od ATMELu: 50/58/66 + ret)
;
; author: Stanislav Maslan, s.maslan@seznam.cz
;------------------------------------------------------------------------------------
	.def	dRe8u=R15
	.def	dDd8u=R16
	.def	dDs8u=R17
	;
div8u:	sub	dRe8u,dRe8u
	rol	dDd8u
	rol	dRe8u
	sub	dRe8u,dDs8u
	brcc	d8u_1
	add	dRe8u,dDs8u
d8u_1:	rol	dDd8u
	rol	dRe8u
	sub	dRe8u,dDs8u
	brcc	d8u_2
	add	dRe8u,dDs8u
d8u_2:	rol	dDd8u
	rol	dRe8u
	sub	dRe8u,dDs8u
	brcc	d8u_3
	add	dRe8u,dDs8u
d8u_3:	rol	dDd8u
	rol	dRe8u
	sub	dRe8u,dDs8u
	brcc	d8u_4
	add	dRe8u,dDs8u
d8u_4:	rol	dDd8u
	rol	dRe8u
	sub	dRe8u,dDs8u
	brcc	d8u_5
	add	dRe8u,dDs8u
d8u_5:	rol	dDd8u
	rol	dRe8u
	sub	dRe8u,dDs8u
	brcc	d8u_6
	add	dRe8u,dDs8u
d8u_6:	rol	dDd8u
	rol	dRe8u
	sub	dRe8u,dDs8u
	brcc	d8u_7
	add	dRe8u,dDs8u
d8u_7:	rol	dDd8u
	rol	dRe8u
	sub	dRe8u,dDs8u
	brcc	d8u_8
	add	dRe8u,dDs8u
d8u_8:	rol	dDd8u
	com	dDd8u
	ret

;------------------------------------------------------------------------------------------
; div16u - 16/16 bit unsigned division, max. speed optimized
; deleni dvou neznaminkovych 16-bit cisel, maximalni optimalizace na rychlost
;
; dDd16r0u:dDd16r1u <- dDd16r0u:dDd16r1u div dDs16r0u:dDs16r1u
; dRe16r0u:dRe16r1u <- dDd16r0u:dDd16r1u mod dDs16r0u:dDs16r1u
;
; words:  142 + ret                        (puvodni od ATMELu: 196 + ret)
; cycles: 126/134/142 + ret [min/avg/max]  (puvodni od ATMELu: 148/172/196 + ret)
;
; Author: Pavel Borovsky, profesor@ti.cz, Czech Republic, Europe 3.6.2000
;------------------------------------------------------------------------------------------
	.def	dRe16r0u=R14
	.def	dRe16r1u=R15
	.def	dDd16r0u=R16
	.def	dDd16r1u=R17
	.def	dDs16r0u=R18
	.def	dDs16r1u=R19
	;
div16u:	clr	dRe16r0u
	sub	dRe16r1u,dRe16r1u
	rol	dDd16r0u
	rol	dDd16r1u
	rol	dRe16r0u
	sub	dRe16r0u,dDs16r0u
	sbc	dRe16r1u,dDs16r1u
	brcc	d16u_1
	add	dRe16r0u,dDs16r0u
	adc	dRe16r1u,dDs16r1u
d16u_1:	rol	dDd16r0u
	rol	dDd16r1u
	rol	dRe16r0u
	sub	dRe16r0u,dDs16r0u
	sbc	dRe16r1u,dDs16r1u
	brcc	d16u_2
	add	dRe16r0u,dDs16r0u
	adc	dRe16r1u,dDs16r1u
d16u_2:	rol	dDd16r0u
	rol	dDd16r1u
	rol	dRe16r0u
	sub	dRe16r0u,dDs16r0u
	sbc	dRe16r1u,dDs16r1u
	brcc	d16u_3
	add	dRe16r0u,dDs16r0u
	adc	dRe16r1u,dDs16r1u
d16u_3:	rol	dDd16r0u
	rol	dDd16r1u
	rol	dRe16r0u
	sub	dRe16r0u,dDs16r0u
	sbc	dRe16r1u,dDs16r1u
	brcc	d16u_4
	add	dRe16r0u,dDs16r0u
	adc	dRe16r1u,dDs16r1u
d16u_4:	rol	dDd16r0u
	rol	dDd16r1u
	rol	dRe16r0u
	sub	dRe16r0u,dDs16r0u
	sbc	dRe16r1u,dDs16r1u
	brcc	d16u_5
	add	dRe16r0u,dDs16r0u
	adc	dRe16r1u,dDs16r1u
d16u_5:	rol	dDd16r0u
	rol	dDd16r1u
	rol	dRe16r0u
	sub	dRe16r0u,dDs16r0u
	sbc	dRe16r1u,dDs16r1u
	brcc	d16u_6
	add	dRe16r0u,dDs16r0u
	adc	dRe16r1u,dDs16r1u
d16u_6:	rol	dDd16r0u
	rol	dDd16r1u
	rol	dRe16r0u
	sub	dRe16r0u,dDs16r0u
	sbc	dRe16r1u,dDs16r1u
	brcc	d16u_7
	add	dRe16r0u,dDs16r0u
	adc	dRe16r1u,dDs16r1u
d16u_7:	rol	dDd16r0u
	rol	dDd16r1u
	rol	dRe16r0u
	sub	dRe16r0u,dDs16r0u
	sbc	dRe16r1u,dDs16r1u
	brcc	d16u_8
	add	dRe16r0u,dDs16r0u
	adc	dRe16r1u,dDs16r1u
d16u_8:	rol	dDd16r0u
	rol	dDd16r1u
	rol	dRe16r0u
	rol	dRe16r1u
	sub	dRe16r0u,dDs16r0u
	sbc	dRe16r1u,dDs16r1u
	brcc	d16u_9
	add	dRe16r0u,dDs16r0u
	adc	dRe16r1u,dDs16r1u
d16u_9:	rol	dDd16r0u
	rol	dDd16r1u
	rol	dRe16r0u
	rol	dRe16r1u
	sub	dRe16r0u,dDs16r0u
	sbc	dRe16r1u,dDs16r1u
	brcc	d16u_10
	add	dRe16r0u,dDs16r0u
	adc	dRe16r1u,dDs16r1u
d16u_10:	rol	dDd16r0u
	rol	dDd16r1u
	rol	dRe16r0u
	rol	dRe16r1u
	sub	dRe16r0u,dDs16r0u
	sbc	dRe16r1u,dDs16r1u
	brcc	d16u_11
	add	dRe16r0u,dDs16r0u
	adc	dRe16r1u,dDs16r1u
d16u_11:	rol	dDd16r0u
	rol	dDd16r1u
	rol	dRe16r0u
	rol	dRe16r1u
	sub	dRe16r0u,dDs16r0u
	sbc	dRe16r1u,dDs16r1u
	brcc	d16u_12
	add	dRe16r0u,dDs16r0u
	adc	dRe16r1u,dDs16r1u
d16u_12:	rol	dDd16r0u
	rol	dDd16r1u
	rol	dRe16r0u
	rol	dRe16r1u
	sub	dRe16r0u,dDs16r0u
	sbc	dRe16r1u,dDs16r1u
	brcc	d16u_13
	add	dRe16r0u,dDs16r0u
	adc	dRe16r1u,dDs16r1u
d16u_13:	rol	dDd16r0u
	rol	dDd16r1u
	rol	dRe16r0u
	rol	dRe16r1u
	sub	dRe16r0u,dDs16r0u
	sbc	dRe16r1u,dDs16r1u
	brcc	d16u_14
	add	dRe16r0u,dDs16r0u
	adc	dRe16r1u,dDs16r1u
d16u_14:	rol	dDd16r0u
	rol	dDd16r1u
	rol	dRe16r0u
	rol	dRe16r1u
	sub	dRe16r0u,dDs16r0u
	sbc	dRe16r1u,dDs16r1u
	brcc	d16u_15
	add	dRe16r0u,dDs16r0u
	adc	dRe16r1u,dDs16r1u
d16u_15:	rol	dDd16r0u
	rol	dDd16r1u
	rol	dRe16r0u
	rol	dRe16r1u
	sub	dRe16r0u,dDs16r0u
	sbc	dRe16r1u,dDs16r1u
	brcc	d16u_16
	add	dRe16r0u,dDs16r0u
	adc	dRe16r1u,dDs16r1u
d16u_16:	rol	dDd16r0u
	rol	dDd16r1u
	com	dDd16r0u
	com	dDd16r1u
	ret

	
;------------------------------------------------------------------------------------------
; div24u - 24/24 bit unsigned division, max. speed optimized
; deleni dvou neznaminkovych 24-bit cisel, maximalne optimalizovano na rychlost
;
; dDd24r0u:dDd24r1u:dDd24r2u <- dDd24r0u:dDd24r1u:dDd24r2u div dDs24r0u:dDs24r1u:dDs24r2u
; dRe24r0u:dRe24r1u:dRe24r2u <- dDd24r0u:dDd24r1u:dDd24r2u mod dDs24r0u:dDs24r1u:dDs24r2u
;
; words:   297 + ret
; cycles:  249/273/297 + ret [min/avg/max]
;
; author: Stanislav Maslan, s.maslan@seznam.cz
;------------------------------------------------------------------------------------------
	.def	dRe24r0u=R13
	.def	dRe24r1u=R14
	.def	dRe24r2u=R15
	.def	dDd24r0u=R16
	.def	dDd24r1u=R17
	.def	dDd24r2u=R18
	.def	dDs24r0u=R19
	.def	dDs24r1u=R20
	.def	dDs24r2u=R21
	;
div24u:	clr	dRe24r0u
	clr	dRe24r1u
	sub	dRe24r2u,dRe24r2u
	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_1
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_1:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_2
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_2:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_3
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_3:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_4
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_4:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_5
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_5:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_6
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_6:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_7
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_7:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_8
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_8:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	rol	dRe24r1u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_9
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_9:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	rol	dRe24r1u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_10
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_10:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	rol	dRe24r1u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_11
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_11:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	rol	dRe24r1u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_12
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_12:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	rol	dRe24r1u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_13
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_13:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	rol	dRe24r1u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_14
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_14:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	rol	dRe24r1u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_15
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_15:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	rol	dRe24r1u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_16
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_16:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	rol	dRe24r1u
	rol	dRe24r2u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_17
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_17:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	rol	dRe24r1u
	rol	dRe24r2u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_18
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_18:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	rol	dRe24r1u
	rol	dRe24r2u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_19
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_19:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	rol	dRe24r1u
	rol	dRe24r2u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_20
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_20:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	rol	dRe24r1u
	rol	dRe24r2u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_21
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_21:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	rol	dRe24r1u
	rol	dRe24r2u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_22
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_22:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	rol	dRe24r1u
	rol	dRe24r2u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_23
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_23:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	rol	dRe24r0u
	rol	dRe24r1u
	rol	dRe24r2u
	sub	dRe24r0u,dDs24r0u
	sbc	dRe24r1u,dDs24r1u
	sbc	dRe24r2u,dDs24r2u
	brcc	d24u_24
	add	dRe24r0u,dDs24r0u
	adc	dRe24r1u,dDs24r1u
	adc	dRe24r2u,dDs24r2u
d24u_24:	rol	dDd24r0u
	rol	dDd24r1u
	rol	dDd24r2u
	com	dDd24r0u
	com	dDd24r1u
	com	dDd24r2u
	ret
	

	

;------------------------------------------------------------------------------------------------------------------------
; bin2BCD8u2 - 8-bit binary to 2-digit BCD conversion (0..99)
; prevod 8-bit binarniho cisla na 2-ciferne cislo v BCD
;
; BCD16res0:BCD16res1 <- bin16r0
; BCD(0x01 0x02) <- bin(12)
;      ^
;  BCD16res1 
;
; words:  5 + ret
; cycles: 5/41 + ret [min/max]
;
; author: Stanislav Maslan, s.maslan@seznam.cz
;------------------------------------------------------------------------------------------------------------------------
	.def	BCD8res0=R16	; !!! BCD8res0 = bin8r0 !!!
	.def	BCD8res1=R17
	.def	bin8r0=R16
	;
bin2BCD8u2:	ser	BCD8res1
BB8u2_L10:	inc	BCD8res1
	subi	bin8r0,10
	brcc	BB8u2_L10
	subi	bin8r0,-10
	ret
	
;------------------------------------------------------------------------------------------------------------------------
; bin2BCD8u3 - 8-bit binary to 3-digit BCD conversion (0..255)
; prevod 8-bit binarniho cisla na 3-ciferne cislo v BCD
;
; BCD16res0:BCD16res1:BCD16res2 <- bin16r0
; BCD(0x01 0x02 0x03) <- bin(123)
;      ^
;  BCD16res2
;
; words:  10 + ret
; cycles: 10/50 + ret [min/max]
;
; author: Stanislav Maslan, s.maslan@seznam.cz
;------------------------------------------------------------------------------------------------------------------------
	.def	BCD8res0=R16	; !!! BCD8res0 = bin8r0 !!!
	.def	BCD8res1=R17
	.def	BCD8res2=R18
	.def	bin8r0=R16
	;
bin2BCD8u3:	ser	BCD8res2
	ser	BCD8res1
BB8u3_L100:	inc	BCD8res2
	subi	bin8r0,100
	brcc	BB8u3_L100
	subi	bin8r0,-100
BB8u3_L10:	inc	BCD8res1
	subi	bin8r0,10
	brcc	BB8u3_L10
	subi	bin8r0,-10
	ret
	
;------------------------------------------------------------------------------------------------------------------------
; bin2BCD24p8 - 24-bit binary to 8-digit packed BCD conversion (0..16777215)
; prevod 24-bit binarniho cisla na osmiciferne cislo v BCD
;
; BCD24res0:BCD24res1:BCD24res2:BCD24res3 <- bin24r0:bin24r1:bin24r2
; BCD(0x12 0x34 0x56 0x78) <- bin(12345678)
;      ^              ^
; BCD16res3      BCD16res0
;
; words:  59 + ret
; cycles: 59/347  + ret [min/max]
;
; author: Stanislav Maslan, s.maslan@seznam.cz
;------------------------------------------------------------------------------------------------------------------------
	.def	BCD24res0=R12
	.def	BCD24res1=R13
	.def	BCD24res2=R14
	.def	BCD24res3=R15
	.def	bin24r0=R16
	.def	bin24r1=R17
	.def	bin24r2=R18
	.def	BB24_cnt1=R30
	.def	BB24_cnt2=R31

bin2BCD24p8:	ser	BB24_cnt2
	ldi	BB24_cnt1,(256-16)
BB24p8_L10M:	subi	BB24_cnt1,(-16)
	subi	bin24r0,0x80
	sbci	bin24r1,0x96
	sbci	bin24r2,0x98	; -10000000
	brcc	BB24p8_L10M
	subi	bin24r0,0x80
	sbci	bin24r1,0x69
	sbci	bin24r2,0x67	; +10000000
	mov	BCD24res3,BB24_cnt2
BB24p8_L1M:	inc	BCD24res3
	subi	bin24r0,0x40
	sbci	bin24r1,0x42
	sbci	bin24r2,0x0F	; -1000000
	brcc	BB24p8_L1M
	subi	bin24r0,0xC0
	sbci	bin24r1,0xBD
	sbci	bin24r2,0xF0	; +1000000
	or	BCD24res3,BB24_cnt1
	ldi	BB24_cnt1,(256-16)
BB24p8_L100k:	subi	BB24_cnt1,(-16)
	subi	bin24r0,0xA0
	sbci	bin24r1,0x86
	sbci	bin24r2,0x01	; -100000
	brcc	BB24p8_L100k
	subi	bin24r0,0x60
	sbci	bin24r1,0x79
	sbci	bin24r2,0xFE	; +100000
	mov	BCD24res2,BB24_cnt2
BB24p8_L10k:	inc	BCD24res2
	subi	bin24r0,0x10
	sbci	bin24r1,0x27
	sbci	bin24r2,0x00	; -10000
	brcc	BB24p8_L10k
	subi	bin24r0,0xF0
	sbci	bin24r1,0xD8	; +10000
	or	BCD24res2,BB24_cnt1
	ldi	BB24_cnt1,(256-16)
BB24p8_L1k:	subi	BB24_cnt1,(-16)
	subi	bin24r0,low(1000)
	sbci	bin24r1,high(1000)	; -1000
	brcc	BB24p8_L1k
	subi	bin24r0,low(-1000)
	sbci	bin24r1,high(-1000)	; +1000
	mov	BCD24res1,BB24_cnt2
BB24p8_L100:	inc	BCD24res1
	subi	bin24r0,low(100)
	sbci	bin24r1,high(100)	; -100
	brcc	BB24p8_L100
	subi	bin24r0,low(-100)	; +100
	or	BCD24res1,BB24_cnt1
	ldi	BB24_cnt1,(256-16)
BB24p8_L10:	subi	BB24_cnt1,(-16)
	subi	bin24r0,10	; -10
	brcc	BB24p8_L10
	subi	bin24r0,-10	; +10
	mov	BCD24res0,bin24r0
	or	BCD24res0,BB24_cnt1
	ret
/***************************************************************************
;*
;* Bin4BCD == 32-bit Binary to BCD conversion
;*
;* Important:   You have to disable the autamtic global register allocation in the compiler settings.
;*              Only (R0,R1,R22,R23 ,R24,R25 ,R26,R27,R30,R31) can be used in ASM routines.
;*              you have to save the other registers in the stack if wanna use them.
;*
;**************************************************************************/
//R0,R1, R22,R23  R24,R25 ,R26,R27   R30,R31
#define	fbin0	r22	; binary value byte 0 (LSB)
#define	fbin1	r23	; binary value byte 1
#define	fbin2	r24	; binary value byte 2
#define	fbin3	r25	; binary value byte 3 (MSB)
#define	tBCD0	r16	; BCD value digits 0 and 1
#define	tBCD1	r17	; BCD value digits 2 and 3
#define	tBCD2	r18	; BCD value digits 4 and 5
#define	tBCD3	r19	; BCD value digits 6 and 7
#define	tBCD4	r20	; BCD value digits 8 and 9 (MSD)
#define	TEMP    r22


Bin4BCD:
        push    r16
        push    r17
        push    r18
        push    r19
        push    r20

        ldd     fbin3,Y+5 ;//loading the unsigned long
        ldd     fbin2,y+4 ;
        ldd     fbin1,Y+3 ;
        ldd     fbin0,y+2 ;

Bin4BCD16:	clr	tBCD0		;initial result (5 bytes)
		clr	tBCD1		;	& shift
		clr	tBCD2		;	       loop
		ldi	tBCD3,0xfe	;		   counter
		ldi	tBCD4,0xff	;			  too
		rjmp	binbcd_jump	;for speed-up and skip of MSD corr
binbcd_876:	subi	tBCD4,-0x03	;add 0x03 to digit 8 only
		sbrs	tBCD4,3 	;if bit 3 clear
		subi	tBCD4,0x03	;	sub 3
		subi	tBCD3,-0x33	;add 0x33 to digits 7 and 6
		sbrs	tBCD3,3 	;if bit 3 clear
		subi	tBCD3,0x03	;	sub 3
		sbrs	tBCD3,7 	;if bit 7 clear
		subi	tBCD3,0x30	;	sub $30
binbcd_54:	subi	tBCD2,-0x33	;add 0x33 to digits 5 and 4
		sbrs	tBCD2,3 	;if bit 3 clear
		subi	tBCD2,0x03	;	sub 3
		sbrs	tBCD2,7 	;if bit 7 clear
		subi	tBCD2,0x30	;	sub $30
binbcd_3210:	subi	tBCD1,-0x33	;add 0x33 to digits 3 and 2
		sbrs	tBCD1,3 	;if bit 3 clear
		subi	tBCD1,0x03	;	sub 3
		sbrs	tBCD1,7 	;if bit 7 clear
		subi	tBCD1,0x30	;	sub $30
		subi	tBCD0,-0x33	;add 0x33 to digits 1 and 0
		sbrs	tBCD0,3 	;if bit 3 clear
		subi	tBCD0,0x03	;	sub 3
		sbrs	tBCD0,7 	;if bit 7 clear
		subi	tBCD0,0x30	;	sub $30
binbcd_jump:	lsl	fbin0		;
		rol	fbin1		;
		rol	fbin2		;
		rol	fbin3		;shift input value
		rol	tBCD0		;through all bytes
		rol	tBCD1		;
		rol	tBCD2		;
		rol	tBCD3		;
		rol	tBCD4		;
		brcs	binbcd_3210	;16_lsl w/o correction of dig_87654
		inc	fbin0		;
		brpl	binbcd_54	;+7_lsl w/o correction of dig_876
		sbrs	fbin2,0 	;
		rjmp	binbcd_876	;32_lsl in total (fbin = 0x1ffff)


        ldd     r31,y+1//loading the buffer's address
        ld      r30,y

		MOV		temp,tBCD0
		ANDI	tBCD0,0X0F
		SUBI	tBCD0,-0x30
        push    tBCD0

		ANDI	temp,0XF0
		SWAP	temp
		SUBI	temp,-0x30
        push    temp

		MOV		temp,tBCD1
		ANDI	tBCD1,0X0F
		SUBI	tBCD1,-0x30
        push    tBCD1

		ANDI	temp,0XF0
		SWAP	temp
		SUBI	temp,-0x30
        push    temp

		MOV		temp,tBCD2
		ANDI	tBCD2,0X0F
		SUBI	tBCD2,-0x30
        push    tBCD2

		ANDI	temp,0XF0
		SWAP	temp
		SUBI	temp,-0x30
        push    temp

		MOV		temp,tBCD3
		ANDI	tBCD3,0X0F
		SUBI	tBCD3,-0x30
        push    tBCD3

		ANDI	temp,0XF0
		SWAP	temp
		SUBI	temp,-0x30
        push    temp

		MOV		temp,tBCD4
		ANDI	tBCD4,0X0F
		SUBI	tBCD4,-0x30
        push    tBCD4

		ANDI	temp,0XF0
		SWAP	temp
		SUBI	temp,-0x30
        push    temp

        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
//Recovering the registers //Other registers will recover by the compiler
        pop     r20
        pop     r19
        pop     r18
        pop     r17
        pop     r16

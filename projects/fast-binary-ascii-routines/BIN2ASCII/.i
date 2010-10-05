
#pragma used+
sfrb TWBR=0;
sfrb TWSR=1;
sfrb TWAR=2;
sfrb TWDR=3;
sfrb ADCL=4;
sfrb ADCH=5;
sfrw ADCW=4;      
sfrb ADCSRA=6;
sfrb ADCSR=6;     
sfrb ADMUX=7;
sfrb ACSR=8;
sfrb UBRRL=9;
sfrb UCSRB=0xa;
sfrb UCSRA=0xb;
sfrb UDR=0xc;
sfrb SPCR=0xd;
sfrb SPSR=0xe;
sfrb SPDR=0xf;
sfrb PIND=0x10;
sfrb DDRD=0x11;
sfrb PORTD=0x12;
sfrb PINC=0x13;
sfrb DDRC=0x14;
sfrb PORTC=0x15;
sfrb PINB=0x16;
sfrb DDRB=0x17;
sfrb PORTB=0x18;
sfrb PINA=0x19;
sfrb DDRA=0x1a;
sfrb PORTA=0x1b;
sfrb EECR=0x1c;
sfrb EEDR=0x1d;
sfrb EEARL=0x1e;
sfrb EEARH=0x1f;
sfrw EEAR=0x1e;   
sfrb UBRRH=0x20;
sfrb UCSRC=0X20;
sfrb WDTCR=0x21;
sfrb ASSR=0x22;
sfrb OCR2=0x23;
sfrb TCNT2=0x24;
sfrb TCCR2=0x25;
sfrb ICR1L=0x26;
sfrb ICR1H=0x27;
sfrb OCR1BL=0x28;
sfrb OCR1BH=0x29;
sfrw OCR1B=0x28;  
sfrb OCR1AL=0x2a;
sfrb OCR1AH=0x2b;
sfrw OCR1A=0x2a;  
sfrb TCNT1L=0x2c;
sfrb TCNT1H=0x2d;
sfrw TCNT1=0x2c;  
sfrb TCCR1B=0x2e;
sfrb TCCR1A=0x2f;
sfrb SFIOR=0x30;
sfrb OSCCAL=0x31;
sfrb TCNT0=0x32;
sfrb TCCR0=0x33;
sfrb MCUCSR=0x34;
sfrb MCUCR=0x35;
sfrb TWCR=0x36;
sfrb SPMCR=0x37;
sfrb TIFR=0x38;
sfrb TIMSK=0x39;
sfrb GIFR=0x3a;
sfrb GICR=0x3b;
sfrb OCR0=0X3c;
sfrb SPL=0x3d;
sfrb SPH=0x3e;
sfrb SREG=0x3f;
#pragma used-

#asm
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x80
	.EQU __sm_mask=0x70
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0x60
	.EQU __sm_ext_standby=0x70
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif
#endasm

void utoa16(unsigned int num,char *buff);
void utoa32(unsigned long num,char *buff);

void utoa16(unsigned int num,char *buff)
{

#asm

#define	fbinL	r30	; binary value Low byte
#define	fbinH	r31	; binary value High byte
#define	temp    r22
#define	tBCD0	r24	; BCD value digits 0 and 1
#define	tBCD1	r25	; BCD value digits 2 and 3
#define	tBCD2	r26	; BCD value digit 4 (MSD is lowermost nibble)

    ldd     r31,Y+3 ;//loading the unsigned integer
    ldd     r30,y+2 ;
Bin2BCD:
Bin2BCD16:	ldi	tBCD2,0xff	;initialize digit 4
binbcd_4:	inc	tBCD2		;
		subi	fbinL,0x10;low(10000);subiw fbin,10000
		sbci	fbinH,0x27;high(10000)
		brcc	binbcd_4	;
		ldi	tBCD1,0x9f	;initialize digits 3 and 2
binbcd_3:	subi	tBCD1,0x10	;
		subi	fbinL,0x18;low(-1000);addiw fbin,1000
		sbci	fbinH,0xfc;high(-1000)
		brcs	binbcd_3	;
binbcd_2:	inc	tBCD1		;
		subi	fbinL,0x64;low(100)	;subiw fbin,100
		sbci	fbinH,0x00;high(100) ;
		brcc	binbcd_2	;
		ldi	tBCD0,0xa0	;initialize digits 1 and 0
binbcd_1:	subi	tBCD0,0x10	;
		subi	fbinL,-10	;addi fbin,10
		brcs	binBCD_1	;
		add	tBCD0,fbinL	;LSD
binBCD_ret:

        ldd     r31,y+1// loading buffer's address
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

#endasm
}

void utoa32(unsigned long num,char *buff)
{

#asm
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
#endasm
}

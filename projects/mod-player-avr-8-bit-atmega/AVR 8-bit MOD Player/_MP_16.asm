
;AVR MOD Player
;	v.003
;
;(c)2010 TS-Labs inc.


//----------------- Setup -----------------

#define		chip	16						;16 - ATmega16; 128 - ATmega128
#define		xmem	0						;1 - Use memory addressing >64k (for MCU's >64 Flash)
#define		rs232	0						;1 - use USART (115200, 8 data, 1 stop, no parity)
#define		f_clk	16000					;Main CLK, kHz
#define		baud	115200					;RS232 USART Baud Rate

;#define	f_dac	44100					;Desired DAC freq, kHz
#define		f_dac	(int(f_clk*1000/p_int))	;Or use calculated samplerate from predefined p_int
#define		p_int	512						;Number of CLK's for 1 DAC cycle
											;comment, if you want it to be calculated
											;together with uncommented 'f_dac'

.include 	"header.asm"

//----------------- Main Program -----------------

.cseg
.org 		0
			rjmp MAIN

.org OC1Aaddr
			rjmp DAC

#if			rs232==1
.org URXC1addr
			rjmp U_RX
#endif

.org INT_VECTORS_SIZE

.include 	"tabs.asm"

MAIN:
			ldi a, high(RAMEND)
			out SPH, a
			ldi a, low(RAMEND)	;Main program start
			out SPL, a 			;Set Stack Pointer to top of RAM

#if			rs232==1
			call U_INIT
			msg M_INTRO
;			msg M_HELP
#endif
			
			call SONG_INIT
			
			sbi L_DAC_d,L_DAC.b
			sbi R_DAC_d,R_DAC.b
			
			ldi a,$80			;Neutral values for PWM
			out L_OCR,a
			out R_OCR,a
	
			;Run Fast-PWM mode on timers
			ldi a,((1<<WGM01)|(1<<WGM00)|(1<<COM01)|(0<<COM00)|(1<<CS00))
			out L_TCCR,a
			out R_TCCR,a

			ldi a,(1<<OCIE1A)	;INT on OC1A enabled
			out TIMSK,a					

			ldi a,high(p_int)	;Set interrupts = DAC frequency
			out (OCR1AH),a
			ldi a,low(p_int)
			out (OCR1AL),a

			ldi a,((0<<WGM11)|(0<<WGM10))			;Run CTC mode on timer1
			out TCCR1A,a
			ldi a,((0<<WGM13)|(1<<WGM12)|(1<<CS10))
			out TCCR1B,a
			
			sei

			clr al				;For counting of free CLK's, debug
			clr ah
MAIN1:
			adiw al,1
			nop
			nop
			nop
			nop
			nop
			nop
			rjmp MAIN1

QUANT:
			sts FREE,al			;For counting of free CLK's, debug
			sts FREE+1,ah
/*
			call U_WT
			ldi a,$AA
 			sts UDR1,a
			call U_WT
 			sts UDR1,al
			call U_WT
			sts UDR1,ah
*/
			lds a,TICK
			dec a
			brne QT1
			call ROW_P			;Process ROW
			lds a,SPEED
QT1:
			sts TICK,a			

			ldi yl,low(IVT0)
			ldi yh,high(IVT0)
			call CHAN			;Process one channel per quant
			ldi yl,low(IVT1)
			ldi yh,high(IVT1)
			call CHAN
			ldi yl,low(IVT2)
			ldi yh,high(IVT2)
			call CHAN
			ldi yl,low(IVT3)
			ldi yh,high(IVT3)
			call CHAN

			clr al				;For counting of free CLK's, debug
			clr ah
			ret

ROW_P:		;Row processing

			lds a,POS
			ldi zl,low(M_POS_TAB)
			ldi zh,high(M_POS_TAB)
			add zl,a
			clr a
			adc zh,a
			lpm zh,Z
			add zh,zh
			add zh,zh
			ldi zl,low(M_PAT1)
			ldi a,high(M_PAT1)
			add zh,a			;Z = addr of pattern
			ldi a,N_CHN*4
			lds b,ROW
			mul a,b
			add zl,r0
			adc zh,r1			;Z = addr of row
			
			clr r7
			sts PUF,r7			;Reset Position Update Flag
			inc r7				;SUF mask initial value = 1
			
			ldi yl,low(IVT0)
			ldi yh,high(IVT0)
			call NOTE
			ldi yl,low(IVT1)
			ldi yh,high(IVT1)
			call NOTE
			ldi yl,low(IVT2)
			ldi yh,high(IVT2)
			call NOTE
			ldi yl,low(IVT3)
			ldi yh,high(IVT3)
			call NOTE

			lds a,N_ROW
			lds b,N_POS
			lds r0,PUF			;Check PUF
			sbrc r0,1
			rjmp RO3			;if PUF.1 is set then update ROW/POS and DON'T clear LP_x (i.e. Loop jump)
			sbrc r0,0
			rjmp RO1			;if PUF.0 is set then update ROW/POS, but clear LP_x (i.e. Bxx/Dxx jump)

			lds a,ROW			;if PUF=0 then ROW & POS autoincrement and clear LP_x
			mov b,a
			inc a
			cpi a,P_LEN
			brne RO2

			clr a				;ROW = 0
			lds b,POS			;if end of pattern, inc POS
			inc b
			lds r0,S_LEN
			cp b,r0
			brne RO1
			clr b				;if end of song, zero POS
RO1:
			call NUL_LP
RO3:
			sts POS,b
RO2:
			sts ROW,a
			ret

NOTE:		;Single channel note processing
			;Y is a pointer to the IVT (Index Variables Table)
			;for individual channel

			clr a
			std Y+APG,a			;Nulling APG
			std Y+APG_F,a
			std Y+SL_F,a		;Nulling Slides Flags

			;Read Note
			lpm bh,Z+
			ldi b,$F0
			and b,bh
			andi bh,$F
			lpm bl,Z+			;bh:bl = Note period
			lpm a,Z+
			ldi cl,$F
			and cl,a			;cl = effect
			andi a,$F0
			swap a
			or b,a				;b = sample
			lpm ch,Z+			;ch = effect parameter

			push zh
			push zl

			clr r8				;Mask for sample triggering (0 - no trig)

			mov r2,b

			mov a,bl
			or a,bh
			brne NT2			
								;Note period is unspecified
			com r7
			lds a,SUF
			and a,r7
			sts SUF,a			;SUF bit reset
			com r7
			rjmp NT4
NT2:	
			std Y+PER_LIM,bl	;Store note period as limit for slide
			std Y+PER_LIM+1,bh

			clr a				;Nulling Tremolo and Vibrato phases
			std Y+V_PH,a
			std Y+T_PH,a

			tst b
			brne NT3
			ldd b,Y+SAMP		;If sample=0, then use last one
NT3:
			std Y+SAMP,b		;Store current sample as last
			inc r8				;Sample should be triggered - flag set
NT4:
			tst r2
			breq NT5
									;If samp>0 then def sample value SHOULD be set as VOL,
									;even if sample is NOT updated!
									;(Fucking undocumented MOD features!!!)
			ldi xl,low(S_VOL_TAB)
			ldi xh,high(S_VOL_TAB)
			add xl,r2
			clr a
			adc xh,a
			ld a,X				;a = samp default VOL value
			std Y+C_VOL,a
NT5:
			;Effect processing
			ldi zl,EFF_VEC
			add zl,cl
			clr zh
			push r2
			push b
			icall
			pop b
			pop r2

			mov a,bl
			or a,bh
			breq NT6

			std Y+PER,bl		;Store sample period
			std Y+PER+1,bh
NT6:
			sbrc r8,7
			rjmp NT8			;Slide effect happened
			sbrs r8,0
			rjmp NT7			;No slide, no trig
			rjmp NT9			;No slide, trig
NT8:
			sbrc r8,0
			rjmp NT7			;Slide, no trig
			tst r2
			breq NT7			;Sample was 0 - no trig
			cp r2,b
			breq NT7			;Sample was same as last - no trig

NT9:		;Sample triggering

			ldi a,ST_SIZE
			mul b,a
			ldi xl,low(S_TAB)
			ldi xh,high(S_TAB)
			add xl,r0
			adc xh,r1			;X = addr of sample in S_TAB
			
			ld a,X+				;Store addr params of sample
			std Y+N_AD,a
			ld a,X+
			std Y+N_AD+1,a
if_xm		ld a,X+
if_xm		std Y+N_AD+2,a

			ld a,X+
			std Y+N_REP,a
			ld a,X+
			std Y+N_REP+1,a
if_xm		ld a,X+
if_xm		std Y+N_REP+2,a

			ld a,X+
			std Y+N_EN,a
			ld a,X+
			std Y+N_EN+1,a
if_xm		ld a,X+
if_xm		std Y+N_EN+2,a

			ld a,X+				;Store enability of instrument and loop
			std Y+N_CH_EN,a
			ld a,X+
			std Y+N_LP_EN,a

			lds a,SUF
			or a,r7
			sts SUF,a			;SUF bit set
NT7:
			lsl r7				;Shift SUF mask
			
			pop zl
			pop zh
			ret


			;Effects init subroutines

_0xx:							;Arpeggio
			std Y+APG,ch
			clr a
			std Y+APG_F,a
			ret

_1xx:		;Tone slide up
			ser a
			ldi al,low(PER_MIN)
			ldi ah,high(PER_MIN)
_11:
			std Y+PER_LIM,al
			std Y+PER_LIM+1,ah
_12:
			std Y+T_SL,ch
_13:
			std Y+TS_SG,a
			ldi a,1				;Slide flags: tone
			std Y+SL_F,a
			ret

_2xx:		;Tone slide down
			ldi al,low(PER_MAX)
			ldi ah,high(PER_MAX)
			clr a
			tst ch
			breq _13
			rjmp _11

_3xx:		;Tone portamento
			com r8				;Slide effect flag set
			ldd al,Y+PER
			ldd ah,Y+PER+1
			cp bl,al
			cpc bh,ah
			sbc a,a
			movw bl,al
			rjmp _12

_4xx:		;Vibrato
			ldd b,Y+VIBR
			mov a,ch
			andi a,$F
			breq _42
			andi b,$F0
			or b,a
_42:
			andi ch,$F0
			breq _41
			andi b,$F
			or b,ch
_41:
			std Y+VIBR,b
			ldi a,4				;Slide flags: vibrato
			std Y+SL_F,a
			ret

_5xx:		;Tone portamento + volume slide
			com r8				;Slide effect flag set
			ldi a,3				;Slide flags: tone + volume
			ldd bl,Y+PER
			ldd bh,Y+PER+1
			rjmp _A1

_6xx:		;Vibrato + volume slide
			ldi a,6				;Slide flags: volume + vibrato
			rjmp _A1

_7xx:		;Tremolo
			ldd b,Y+TREM
			mov a,ch
			andi a,$F
			breq _72
			andi b,$F0
			or b,a
_72:
			andi ch,$F0
			breq _71
			andi b,$F
			or b,ch
_71:
			std Y+TREM,b
			ldi a,8				;Slide flags: tremolo
			std Y+SL_F,a
			ret

_8xx:
			ret

_9xx:
			ret

_Axx:		;Volume slide
			ldi a,2				;Slide flags: volume
_A1:
			std Y+SL_F,a
			std Y+V_SL,ch
			ret

_Bxx:		;Position jump
			sts N_POS,ch		;Set N_POS
			lds a,PUF
			sbrc a,1
			rjmp ED4			;If PUF.1 is set (i.e. Loop is active) then ignore Bxx
			ori a,8				;Set PUF.3 (i.e. N_POS is set)
			sbrc a,2
			rjmp PUF_BD
			clr r0				;If PUF.2 is not set then set N_ROW=0
			sts N_ROW,r0
			rjmp PUF_BD

_Cxx:		;Set volume
			std Y+C_VOL,ch
			ret

_Dxx:		;Break pattern and jump to next position to the specified row
			lds a,PUF
			sbrc a,1
			rjmp ED4			;If PUF.1 is set (i.e. Loop is active) then ignore Dxx
			
			mov b,ch			;BCD conversion
			swap b				;Fucking manual writers !!!!!!!!!!!!!!
			andi b,$F
			ldi cl,10
			mul cl,b
			andi ch,$F
			add ch,r0
			
			sbrc a,3
			rjmp ED2
			lds b,POS			;If PUF.3 is not set then inc POS
			lds r0,S_LEN
			inc b
			cp b,r0
			brne ED3
			clr b				;if end of song, zero POS
			call NUL_LP
			clr ch				;also, set N_ROW=0 - I don't know why, but in OpenMPT
ED3:							;this effect works this way...
			sts N_POS,b
ED2:
			sts N_ROW,ch		;Set N_ROW
			ori a,4				;Set PUF.2 (i.e. N_ROW is set)
PUF_BD:
			ori a,1
			sts PUF,a
ED4:
			ret

_Exx:		;Extended effects processing
			mov zl,ch
			andi zl,$F0
			swap zl
			subi zl,-EFE_VEC
			clr zh
			andi ch,$F		;Mask effect parameter with $F
			ijmp

_Fxx:		;Set speed/tempo
			tst ch
			breq EF3			;If xx=0 then skip (fool proof)
			cpi ch,$21
			brcs EF1
								;If xx>$20 then set TEMPO (OpenMPT behavior)
			sts TEMPO,ch
			mov r5,ch
EF2:		clr r6
			ldi a,low(p_qnt)
			mov r3,a
			ldi a,high(p_qnt)
			mov r4,a
			call DIV
			add r1,r1
			adc r2,r2
			sts N_QNT,r1
			sts N_QNT+1,r2
			ret
EF1:							;If param<$21 then set SPEED (OpenMPT behavior)
			sts SPEED,ch
EF3:
			ret

_E0x:
			ret

_E1x:		;Fine slide up
			ldd a,Y+PER			;NO CHECK FOR OVERFLOW!!!
			ldd b,Y+PER+1
			clr r0
			sub a,ch
			sbc b,r0
EE11:
			std Y+PER,a
			std Y+PER+1,b
			ret

_E2x:		;Fine slide down
			ldd a,Y+PER			;NO CHECK FOR OVERFLOW!!!
			ldd b,Y+PER+1
			clr r0
			add a,ch
			adc b,r0
			rjmp EE11

_E3x:
			ret

_E4x:
			ret

_E5x:
			ret

_E6x:		;Set loop or jump to loop
			tst ch
			breq EE60			;If x=0 then set Loop Row

			ldd a,Y+LP_C
			tst a
			brne EE63
			std Y+LP_C,ch		;Set LP_C to x
			rjmp EE61
EE63:
			dec a				;If LP_C>0 then dec LP_C
			std Y+LP_C,a
			breq EE62			;If LP_C=0 then exit Loop
EE61:							;If LP_C=0
			ldd a,Y+LP_R
			sts N_ROW,a			;Set N_ROW to Loop Row

			lds a,POS
			sts N_POS,a			;Set N_POS=POS (for case, if Bxx or Dxx preceded)

			lds a,PUF
			ori a,2
			sts PUF,a			;Set PUF.1 (i.e. N_ROW is set by Loop)
EE62:
			ret
EE60:
			lds a,ROW
			std Y+LP_R,a		;Set Loop Row to ROW
			ret

_E7x:
			ret

_E8x:
			ret

_E9x:
			ret

_EAx:		;Fine volume slide up
			ldd a,Y+C_VOL
			add a,ch
			cpi a,65
			brcs EEA1
			ldi a,64
EEA1:
			std Y+C_VOL,a
			ret

_EBx:		;Fine volume slide down
			ldd a,Y+C_VOL
			sub a,ch
			brcc EEA1
			clr a
			rjmp EEA1

_ECx:
			ret

_EDx:
			ret

_EEx:
			ret

_EFx:
			ret

CHAN:
			ldd r5,Y+PER
			ldd r6,Y+PER+1

			ldd a,Y+C_VOL
			std Y+N_VOL,a

			ldd b,Y+APG
			tst b
			breq CN4			;APG=0, no Arpeggio, go to slides

			;Arpeggio processing

			ldd a,Y+APG_F
			tst a
			breq CN5			;'zero' cycle of Arp
			
			dec a
			sbrs a,0
			swap b
			andi b,$F			;b = semitones value
			ldi zl,(SEMI_TAB*2)
			add zl,b
			add zl,b
			clr zh				;Z = addr for semitone'd period
			lpm r7,Z+
			lpm r8,Z
			add r5,r5
			adc r6,r6
			call MULX
			mov r5,r3
			mov r6,r4

			ldd a,Y+APG_F
CN5:
			inc a
			cpi a,3
			brcs CN6
			clr a
CN6:
			std Y+APG_F,a
			rjmp CN7			;After Arpeggio no effects processed
CN4:			
			;Slides
			lds a,TICK
			lds r0,SPEED
			cp a,r0
			ldd r0,Y+SL_F		;Read flags
			breq CN11			;If 1st tick in row, then skip slides, go to Tremolo/Vibrato

			;Tone Slide processing
			sbrs r0,0
			rjmp CN3			;Skip if SL_F.0=0
			
			ldd cl,Y+PER_LIM
			ldd ch,Y+PER_LIM+1

			clr b
			ldd a,Y+TS_SG
			tst a
			ldd a,Y+T_SL
			brne CN1

			add r5,a			;If slide positive (i.e. Down)
			adc r6,b
			cp r5,cl
			cpc r6,ch
			brcs CN2			;if <PER_LIM
			rjmp CN10
CN1:							;If slide negative (i.e. Up)
			sub r5,a
			sbc r6,b
			brcs CN10			;if <0, overflow
			cp cl,r5
			cpc ch,r6
			brcs CN2			;if >PER_LIM
CN10:
			mov r5,cl
			mov r6,ch
CN2:							;Store PER
			std Y+PER,r5
			std Y+PER+1,r6

CN3:		;Volume Slide processing
			sbrs r0,1
			rjmp CN11			;Skip if SL_F.1=0

			ldd r1,Y+V_SL
			ldd b,Y+C_VOL
			mov a,r1
			andi a,$F0
			breq CN8
								;VS Up
			swap a
			add b,a
			cpi b,65
			brcs CN9
			ldi b,64
			rjmp CN9
CN8:							;VS Down
			sub b,r1
			brcc CN9
			clr b
CN9:		std Y+C_VOL,b

CN11:		;Vibrato processing
			sbrs r0,2
			rjmp CN12			;Skip if SL_F.2=0

			ldd b,Y+VIBR
			ldd a,Y+V_PH
			call GET_SIN
			std Y+V_PH,a

			clr a
			tst r1
			brmi CN15
			add r5,r1
			adc r6,a
			rjmp CN7			;After Vibrato no effects processed
CN15:
			neg r1
			sub r5,r1
			sbc r6,a
			rjmp CN7			;After Vibrato no effects processed

CN12:
			;Tremolo processing
			sbrs r0,3
			rjmp CN7			;Skip if SL_F.3=0

			ldd b,Y+TREM
			ldd a,Y+T_PH
			call GET_SIN
			std Y+T_PH,a

			ldd a,Y+C_VOL
			add a,r1
			brpl CN14
			clr a
			rjmp CN13
CN14:
			cpi a,65
			brcs CN13
			ldi a,64
CN13:
			std Y+N_VOL,a
CN7:
			;Calculate INC for sample in channel
			ldd b,Y+SAMP
			ldi xl,low(S_FT_TAB)
			ldi xh,high(S_FT_TAB)
			add xl,b
			clr a
			adc xh,a
			ld a,X				;a = samp FT value
			ldi zl,(C5_TAB*2)
			add zl,a
			add zl,a
			clr zh
			lpm r3,Z+
			lpm r4,Z
			call DIV
			std Y+N_INC,r0
			std Y+N_INC+1,r1
			std Y+N_INC+2,r2

			ret


GET_SIN:	;Get sinus value from table
			;a - phase, b - spd+depth,
			;adder:	r1 = sin * (depth * 2 + 1)/64
			mov r4,b
			andi b,$F

			ldi zl,low(SIN_TAB*2)
			add zl,a
			clr zh
			lpm cl,Z			;cl=sin
			add b,b
			inc b				;b=depth*2+1
			mulsu cl,b
			rol r0
			rol r1
			rol r0
			rol r1
			sbrc r1,7
			inc r1				;r1=cl*b/64

			lds r2,TICK
			lds r3,SPEED
			cp r2,r3
			breq GS1			;If 1st tick in row, then skip phase inc

			mov b,r4
			swap b
			andi b,$F
			add a,b
			cpi a,65
			brcs GS1
			subi a,64			;a=mod(a+spd,64)
GS1:
			ret


SONG_INIT:	;Module Initialisation

			;Set previous sampling byte to $80, avoiding start clicks
			ldi a,$80
			sts IVT0+PSM,a
			sts IVT1+PSM,a
			sts IVT2+PSM,a
			sts IVT3+PSM,a

			;Set default SPEED
			ldi a,def_spd
			sts SPEED,a

			;Set default TEMPO
			ldi a,def_tmp
			sts TEMPO,a

			;Set default N_QNT at 125bpm
			mov r5,a
			call EF2

			;Set TICK=1 so the first QUANT cycle will call NOTE procedure
			ldi a,1
			sts TICK,a
			;Set QNT_C=1 so the first DAC cycle will jump to QUANT procedure
			sts QNT_C,a
			ldi a,0
			sts QNT_C+1,a

			;Store	song length
			ldi zl,low(M_LEN)
			ldi zh,high(M_LEN)
			lpm a,Z
			sts S_LEN,a
			
			clr a

			;Position to 0
			sts POS,a

			;Row to 0
			sts ROW,a

			;Nulling Slides, Tremolo, Vibrato
			sts IVT0+V_SL,a

			;Nulling VOL's
			sts IVT0+VOL,a
			sts IVT1+VOL,a
			sts IVT2+VOL,a
			sts IVT3+VOL,a
			sts IVT0+C_VOL,a
			sts IVT1+C_VOL,a
			sts IVT2+C_VOL,a
			sts IVT3+C_VOL,a

			;Nulling last used sample
			sts IVT0+SAMP,a
			sts IVT1+SAMP,a
			sts IVT2+SAMP,a
			sts IVT3+SAMP,a

			;Nulling update flags
			sts SUF,a

			call NUL_LP

			;Turn off channels
			ldi yl,low(IVT0+CH_EN)
			ldi yh,high(IVT0+CH_EN)
			call NC1
			call NC1
			ldi yl,low(IVT1+CH_EN)
			ldi yh,high(IVT1+CH_EN)
			call NC1
			call NC1
			ldi yl,low(IVT2+CH_EN)
			ldi yh,high(IVT2+CH_EN)
			call NC1
			call NC1
			ldi yl,low(IVT3+CH_EN)
			ldi yh,high(IVT3+CH_EN)
			call NC1
			call NC1

			;Calculate number of patterns (max(M_POS_TAB)+1)
			ldi zl,low(M_POS_TAB)
			ldi zh,high(M_POS_TAB)
			ldi i,N_MPT			
			clr b
SI1:
			lpm a,Z+
			cp b,a
			brcc PC+2
			mov b,a
			dec i
			brne SI1
			inc b
			sts N_PAT,b

			;Parsing the table of samples

			;Parsing default volumes
			ldi zl,low(M_SAM_TAB+MST_VOL)
			ldi zh,high(M_SAM_TAB+MST_VOL)
			ldi xl,low(S_VOL_TAB)
			ldi xh,high(S_VOL_TAB)

			clr a
			st X+,a					;VOL=0 for 'zero' sample

			ldi i,N_MST
SI2:
			lpm a,Z
			st X+,a
			adiw zl,MST_SIZE
			dec i
			brne SI2

			;Parsing finetune values
			ldi zl,low(M_SAM_TAB+MST_FT)
			ldi zh,high(M_SAM_TAB+MST_FT)
			ldi xl,low(S_FT_TAB)
			ldi xh,high(S_FT_TAB)

			clr a				;'zero' sample
			st X+,a

			ldi i,N_MST
SI3:
			lpm a,Z
			andi a,$F				;Mask for Finetune value (fool-proof protection)
			st X+,a
			adiw zl,MST_SIZE
			dec i
			brne SI3

			;Parsing sample addresses
			
			ldi zl,low(M_SAM_TAB+MST_LEN)		;samp table in module
			ldi zh,high(M_SAM_TAB+MST_LEN)

			ldi yl,low(S_TAB)					;samp tables in SRAM
			ldi yh,high(S_TAB)

			;Init 'zero' sample
			call NUL_CN

if_xm_		clr r0
			ldi al,low(M_PAT1)
			ldi ah,high(M_PAT1)
if_xm_		clr r1
			lds a,N_PAT
			add ah,a
if_xm		adc r1,r1
			add ah,a
if_xm		adc r1,r1
			add ah,a
if_xm		adc r1,r1
			add ah,a							;ah:al - addr of 1st sample in module
if_xm		adc r1,r1							;r1:ah:al - addr of 1st sample in module

			ldi i,N_MST
SI4:		
if_xm_		push r1
			push ah
			push al

			clr r5				;Null CH_EN & LP_EN
			clr r6

			lpm dh,Z+			;Read samp SampLen (swapped words)
			lpm dl,Z+
if_xm_		clr r4
			add dl,dl
			adc dh,dh			;dh:dl - length
if_xm		adc r4,r4			;r4:dh:dl - length

			adiw zl,2
			lpm bh,Z+			;Read samp RepOffset (swapped words)
			lpm bl,Z+
if_xm_		clr r2
			add bl,bl
			adc bh,bh			;bh:bl - rep point
if_xm		adc r2,r2			;r2:bh:bl - rep point

			lpm ch,Z+			;Read samp RepLen (swapped words)
			lpm cl,Z+
if_xm_		clr r3
			add cl,cl
			adc ch,ch			;ch:cl - rep length
if_xm		adc r3,r3			;r3:ch:cl - rep length

			mov a,dl
			or a,dh
			breq SI7			;Len=0, sample empty

			inc r5				;CH_EN=1
			st Y+,al			;Store start addr
			st Y+,ah
if_xm		st Y+,r1

			tst ch
			brne SI9			;high(RepLen)>0, loop is ON
			cpi cl,3
			brcs SI5			;low(RepLen)<=2, loop is OFF
			
SI9:		;Case when loop is ON
			inc r6				;LP_EN=1
			add al,bl
			adc ah,bh
if_xm		adc r1,r2
			st Y+,al			;Store rep addr
			st Y+,ah
if_xm		st Y+,r1
			add al,cl
			adc ah,ch
if_xm		adc r1,r3
			sbiw al,1
if_xm		sbc r1,r0
SI10:
			st Y+,al			;Store end addr
			st Y+,ah
if_xm		st Y+,r1
			st Y+,r5			;Store CH_EN
			st Y+,r6			;Store LP_EN
SI6:
			pop al
			pop ah
if_xm_		pop r1

			add al,dl
			adc ah,dh
if_xm		adc r1,r4

			adiw zl,MST_LEN
			dec i
			brne SI4
			ret

SI5:		;Case when loop is OFF
			add al,dl
			adc ah,dh
if_xm		adc r1,r4
			sbiw al,1
if_xm		sbc r1,r0
			st Y+,al			;Store rep addr
			st Y+,ah
if_xm		st Y+,r1
			rjmp SI10

SI7:		;Case when length=0 - sample is empty
			call NUL_CN
			rjmp SI6

NUL_CN:		;Nulling CH_EN
			adiw yl,ST_SIZE-2
NC1:
			clr a
			st Y+,a				;Store CH_EN
			st Y+,a				;Store LP_EN
			ret

NUL_LP:		;Nulling Loop Row's and Loop Counters, Tremolo and Vibrato phases
			clr r0
			sts IVT0+LP_R,r0
			sts IVT0+LP_C,r0
			sts IVT0+T_PH,r0
			sts IVT0+V_PH,r0

			sts IVT1+LP_R,r0
			sts IVT1+LP_C,r0
			sts IVT1+T_PH,r0
			sts IVT1+V_PH,r0

			sts IVT2+LP_R,r0
			sts IVT2+LP_C,r0
			sts IVT2+T_PH,r0
			sts IVT2+V_PH,r0

			sts IVT3+LP_R,r0
			sts IVT3+LP_C,r0
			sts IVT3+T_PH,r0
			sts IVT3+V_PH,r0
			ret

MULX:		;Procedure for multiplication (16)*(16)->(24)
			;r4:r2 <- r6:r5 * r8:r7
			;r0,r1 - used as temp registers

			mul r6,r8
			mov r4,r1
			mov r3,r0

			mul r5,r7
			mov r2,r1
			
			mul r6,r7
			add r2,r0
			adc r3,r1
			clr r0
			adc r4,r0

			mul r5,r8
			add r2,r0
			adc r3,r1
			clr r0
			adc r4,r0
			ret

DIV:		;Procedure for division (16)/(16)->(24)
			;r2:r1:r0 <- r4:r3 / r6:r5
			;a,b - used as temp registers

			clr r2
			clr r1
			clr r0
			inc r0
			clr b
			clr a
			clc
DIV1:
			rol r3
			rol r4
			rol b
			rol a
			brcs DIV2
			cp b,r5
			cpc a,r6
			brcs DIV3
DIV2:
			sub b,r5
			sbc a,r6
			sec
			rjmp DIV4
DIV3:
			clc
DIV4:
			rol r0
			rol r1
			rol r2
			brcc DIV1
			ret

.macro		RD_DAC				;Read from S_ADn address the sample and multiplies it by VOLn
								;Increments address on S_INCn (fractional)
								;Check if the S_ENn addr reached and loop at S_REPn point
			lds b,IVT@0+CH_EN
			sbrs b,0
			rjmp L1					;If channel disabled, then b=0, skip

			lds zl,IVT@0+S_AD
			lds zh,IVT@0+S_AD+1
if_xm		lds r2,IVT@0+S_AD+2		;r2:zh:zl = sample address
if_xm		out rampz,r2
									;INTERPOLATION (ta-da-da-daa!)
;/*
			lds c,IVT@0+PSM			;read sample_1
			lds r0,IVT@0+S_SAD+1	;read higher sub-address (used as 'middle point')
			com r0
			mul c,r0
			add r0,c
			clr b
			adc b,r1				;b = sample_1 * (1 - middle point between samples 1 and 2)

if_xm		elpm c,Z				;read sample_2
if_no_xm	lpm c,Z
			subi c,128
			lds r0,IVT@0+S_SAD+1
			mul c,r0
			add r0,c
			adc b,r1				;b = b + sample_2 * (middle point between samples 1 and 2)
			subi b,128				;make it unsigned (to calculate volume later)
;*/

/*
if_xm		elpm b,Z				;read sample (NO interpolation)
if_no_xm	lpm b,Z
*/

			lds r0,IVT@0+S_SAD		;inc sample sub-address
			lds r1,IVT@0+S_INC
			add r0,r1
			sts IVT@0+S_SAD,r0
			
			lds r0,IVT@0+S_SAD+1
			lds r1,IVT@0+S_INC+1
			adc r0,r1
			sts IVT@0+S_SAD+1,r0
			brcc L2
			sts IVT@0+PSM,c			;if inc of real address, store PSM
L2:			
			lds r1,IVT@0+S_INC+2	;inc sample address
			adc zl,r1
			tst r1
			breq L3
			sts IVT@0+PSM,c			;if inc of real address, store PSM
L3:
			clr r1
			adc zh,r1
if_xm		adc r2,r1
			lds c,IVT@0+S_EN		;check if beyond sample end address
			cp c,zl
			lds c,IVT@0+S_EN+1
			cpc c,zh
if_xm		lds c,IVT@0+S_EN+2
if_xm		cpc c,r2
			brcc L0
			
			lds zl,IVT@0+S_REP		;if end of sample, restore address to its loop point
			lds zh,IVT@0+S_REP+1
if_xm		lds r2,IVT@0+S_REP+2
			
			lds c,IVT@0+LP_EN
			sts IVT@0+CH_EN,c
L0:
			sts IVT@0+S_AD,zl		;store sample address
			sts IVT@0+S_AD+1,zh
if_xm		sts IVT@0+S_AD+2,r2

			lds c,IVT@0+VOL
			add c,c					;Multiply by 2
L1:
			mulsu b,c
.endmacro

.macro		INC_NEW				;Set new increment for channel
			lds c,IVT@0+N_INC
			sts IVT@0+S_INC,c
			lds c,IVT@0+N_INC+1
			sts IVT@0+S_INC+1,c
			lds c,IVT@0+N_INC+2
			sts IVT@0+S_INC+2,c
L0:
.endmacro

.macro		SA_NEW				;Sets new sample addresses,
								;SUF is used to check (if bit=0 then skip update)
			sbrs al,@0
			rjmp L0

			lds c,IVT@0+N_AD
			sts IVT@0+S_AD,c
			lds c,IVT@0+N_AD+1
			sts IVT@0+S_AD+1,c
if_xm		lds c,IVT@0+N_AD+2
if_xm		sts IVT@0+S_AD+2,c

			lds c,IVT@0+N_REP
			sts IVT@0+S_REP,c
			lds c,IVT@0+N_REP+1
			sts IVT@0+S_REP+1,c
if_xm		lds c,IVT@0+N_REP+2
if_xm		sts IVT@0+S_REP+2,c

			lds c,IVT@0+N_EN
			sts IVT@0+S_EN,c
			lds c,IVT@0+N_EN+1
			sts IVT@0+S_EN+1,c
if_xm		lds c,IVT@0+N_EN+2
if_xm		sts IVT@0+S_EN+2,c

			lds c,IVT@0+N_CH_EN
			sts IVT@0+CH_EN,c
			lds c,IVT@0+N_LP_EN
			sts IVT@0+LP_EN,c

			cbr al,(1<<@0)
L0:
.endmacro

DAC:		;Procedure of playing samples into DAC's (DAC DMA)
			;Registers used:			c,b,al,ah,zl,zh,r0,r1
			;Registers NOT preserved:	c

			in c,SREG			;Store SREG
			push c
			push b
			push zl
			push zh
			push r0
			push r1
			push r2
			push al
			push ah

			rd_dac 0			;Read sample in channel 0
			movw al,r0
			rd_dac 3			;Read sample in channel 3
			add al,r0
			adc ah,r1
			subi ah,$80		;S > U
			out L_OCR,ah		;Write into Left DAC Channel
;			sts UDR1,ah			;Write to USART, debug

			rd_dac 1			;Read sample in channel 1
			movw al,r0
			rd_dac 2			;Read sample in channel 2
			add al,r0
			adc ah,r1
			subi ah,$80		;S > U
			out R_OCR,ah		;Write into Right DAC Channel
			
			lds al,QNT_C		;Check, if a quant counter exceeded
			lds ah,QNT_C+1
			sbiw al,1
			breq DAC4

			sts QNT_C,al
			sts QNT_C+1,ah

			pop ah
			pop al
			pop r2
			pop r1
			pop r0
			pop zh
			pop zl
			pop b
			pop c
			out SREG,c
			reti

DAC4:
			inc_new 0
			inc_new 1
			inc_new 2
			inc_new 3

			lds al,SUF
			sa_new 0
			sa_new 1
			sa_new 2
			sa_new 3
			sts SUF,al

			lds a,IVT0+N_VOL
			sts IVT0+VOL,a
			lds a,IVT1+N_VOL
			sts IVT1+VOL,a
			lds a,IVT2+N_VOL
			sts IVT2+VOL,a
			lds a,IVT3+N_VOL
			sts IVT3+VOL,a
								;Set the initial value of quant counter
			lds a,N_QNT
			sts QNT_C,a
			lds a,N_QNT+1
			sts QNT_C+1,a

			pop ah
			pop al
			pop r2
			pop r1
			pop r0
			pop zh
			pop zl
			pop b
			pop c
			out SREG,c
								;Push the QUANT procedure into stack
			ldi c,low(QUANT)
			push c
			ldi c,high(QUANT)
			push c
			reti

#if			rs232==1
.include	"usart.asm"
#endif

MOD:
			.include "mod.asm"


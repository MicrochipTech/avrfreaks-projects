//----------------- Macros -----------------

.macro		msg
			ldi zl,low(@0*2)
			ldi zh,high(@0*2)
			call U_MSG
.endmacro

.macro		sndb
			call U_WT
			sts UDR1,a
.endmacro


#if			chip==16
#include	"m16def.inc"
#elif		chip==128
#include	"m128def.inc"
#endif

.macro		if_no_xm
#if	xmem==0
			@0,@1
#endif
.endmacro

.macro		if_xm
#if	xmem==1
			@0,@1
#endif
.endmacro

.macro		if_xm_
#if	xmem==1
			@0
#endif
.endmacro


//----------------- Symbols Definitions -----------------

#define		dl		r14				;16 bit accumulator's LSB
#define		dh		r15				;16 bit accumulator's MSB
#define		a		r16				;8 bit accumulator
#define		b		r17				;8 bit accumulator
#define		c		r18				;8 bit accumulator
#define		i		r19				;8 bit counter (20 years on counting scene! :)
#define		cl		r20				;16 bit accumulator's LSB
#define		ch		r21				;16 bit accumulator's MSB
#define		bl		r22				;16 bit accumulator's LSB
#define		bh		r23				;16 bit accumulator's MSB
#define		al		r24				;16 bit accumulator's LSB
#define		ah		r25				;16 bit accumulator's MSB

#if			chip==16
#define		L_DAC 	PORTB			;For mega16
#define		L_DAC.b	3
#define		L_DAC_d	DDRB
#define		R_DAC 	PORTD
#define		R_DAC.b	7
#define		R_DAC_d	DDRD

#elif		chip==128
#define		L_DAC 	PORTB			;For mega128
#define		L_DAC.b	4
#define		L_DAC_d	DDRB
#define		R_DAC 	PORTB
#define		R_DAC.b	7
#define		R_DAC_d	DDRB
#endif

#define		L_OCR	OCR0
#define		R_OCR	OCR2
#define		L_TCCR	TCCR0
#define		R_TCCR	TCCR2


//----------------- Calculations -----------------

#if !defined p_int
#define		p_int	(int(f_clk*1000/f_dac))	;INT period, CLK's
#endif

#define		p_qnt	(int(f_dac*125/100))	;Period of QUANT, samples, at BPM=1 div'ed by 2 to fit 16
											;(at BPM=125, once upon 1/50 sec
											;the effects are processed, QUANT procedure)
#define		def_spd	6						;Default speed
#define		def_tmp	125						;Default tempo
#define		P_LEN	64						;Len of pattern
#define		N_CHN	4						;Number of channels
#define		PER_MIN $001C					;Minimal period
#define		PER_MAX $06B0					;Maximal period


//----------------- Definitions of MOD structures -----------------

#define		M_SAM_TAB	(MOD*2+20)
#define		MST_LEN		22
#define		MST_FT		24
#define		MST_VOL		25
#define		MST_REP		26
#define		MST_RLEN	28
#define		MST_SIZE	30		;Size of STable
#define		N_MST		31		;Number of samples

#define		M_LEN		(MOD*2+950)

#define		M_POS_TAB	(MOD*2+952)
#define		N_MPT		128

#define		M_MAGIC		(MOD*2+1080)
#define		M_PAT1		(MOD*2+1084)


//----------------- SRAM variables ---------------------

.dseg
.org SRAM_START

FREE:		.byte 2


IVT0:		;Index Variables Table
S_SAD0:		.byte 2		;Sample addr (fractional part)

S_AD0:		.byte 2+xmem	;Sample addr
S_REP0:		.byte 2+xmem	;Sample rep start addr
S_EN0:		.byte 2+xmem	;Sample end addr
CH_EN0:		.byte 1			;1 = Channel Enabled
LP_EN0:		.byte 1			;1 = Loop Enabled
N_CH_EN0:	.byte 1			;1 = Channel Enabled (new)
N_LP_EN0:	.byte 1			;1 = Loop Enabled (new)
N_AD0:		.byte 2+xmem	;Sample NEW addr
N_REP0:		.byte 2+xmem	;Sample NEW rep addr
N_EN0:		.byte 2+xmem	;Sample NEW end addr
S_INC0:		.byte 3			;Sample increments
N_INC0:		.byte 3			;Sample NEW increments
SAMP0:		.byte 1			;Last used sample
PER0:		.byte 2			;Current sample period
SL_F0:		.byte 1			;Slides flags (bit0-tone, bit1-volume, bit2-vibrato, bit3-tremolo)
APG0:		.byte 1			;Parameters for Arpeggio
APG_F0:		.byte 1			;Flags for Arpeggio. bits1..0 - counter (0 to 2)
V_SL0:		.byte 1			;Volume slide value
T_SL0:		.byte 1			;Tone slide value
TS_SG0:		.byte 1			;Slide sign (0-pos, 1-neg)
PER_LIM0:	.byte 2			;Portamento final period
VIBR0:		.byte 1			;Vibrato speed/depth
TREM0:		.byte 1			;Tremolo speed/depth
V_PH0:		.byte 1			;Vibrato phase
T_PH0:		.byte 1			;Tremolo phase
LP_R0:		.byte 1			;Loop row
LP_C0:		.byte 1			;Loop counter
VBT_SP0:	.byte 1			;Vibrato speed
VBT_DP0:	.byte 1			;Vibrato depth
PSM0:		.byte 1			;Previous sampling byte, used for interpolation, (!) unsigned
VOL0:		.byte 1			;Current volume for DAC
N_VOL0:		.byte 1			;New volume for DAC
C_VOL0:		.byte 1			;New volume for channel


.equ		IVT_SIZE=C_VOL0-IVT0+1

.equ		S_SAD=S_SAD0-IVT0
.equ		S_AD=S_AD0-IVT0
.equ		S_REP=S_REP0-IVT0
.equ		S_EN=S_EN0-IVT0
.equ		CH_EN=CH_EN0-IVT0
.equ		LP_EN=LP_EN0-IVT0
.equ		N_CH_EN=N_CH_EN0-IVT0
.equ		N_LP_EN=N_LP_EN0-IVT0
.equ		N_AD=N_AD0-IVT0
.equ		N_REP=N_REP0-IVT0
.equ		N_EN=N_EN0-IVT0
.equ		S_INC=S_INC0-IVT0
.equ		N_INC=N_INC0-IVT0
.equ		SAMP=SAMP0-IVT0
.equ		PER=PER0-IVT0
.equ		SL_F=SL_F0-IVT0
.equ		APG=APG0-IVT0
.equ		APG_F=APG_F0-IVT0
.equ		T_SL=T_SL0-IVT0
.equ		TS_SG=TS_SG0-IVT0
.equ		PER_LIM=PER_LIM0-IVT0
.equ		VIBR=VIBR0-IVT0
.equ		TREM=TREM0-IVT0
.equ		V_PH=V_PH0-IVT0
.equ		T_PH=T_PH0-IVT0
.equ		LP_R=LP_R0-IVT0
.equ		LP_C=LP_C0-IVT0
.equ		V_SL=V_SL0-IVT0
.equ		VBT_SP=VBT_SP0-IVT0
.equ		VBT_DP=VBT_DP0-IVT0
.equ		PSM=PSM0-IVT0
.equ		VOL=VOL0-IVT0
.equ		N_VOL=N_VOL0-IVT0
.equ		C_VOL=C_VOL0-IVT0

IVT1:		.byte IVT_SIZE
IVT2:		.byte IVT_SIZE
IVT3:		.byte IVT_SIZE


S_LEN:		.byte 1			;Song length
N_PAT:		.byte 1			;Number of patterns
POS:		.byte 1			;Current song position
N_POS:		.byte 1			;New song position
ROW:		.byte 1			;Current row in pattern
N_ROW:		.byte 1			;New row in pattern
SPEED:		.byte 1			;Current speed, default = 6
TEMPO:		.byte 1			;Current tempo, default = 125
N_QNT:		.byte 2			;Current number of samples per quant
QNT_C:		.byte 2			;Counter of samples left per quant (BPM parameter)
TICK:		.byte 1			;Counter of ticks (speed parameter)
SUF:		.byte 1			;Sample Update Flags: if bit=1, update needed
							;(bit3 - CH3, ..., bit0 - CH0)
PUF:		.byte 1			;'Position Changed' flag
							;If an effect has done any row/position change action, flag is set,
							;so ROW/POS autoincrement at the end of the row is skipped
							;bit0 - Bxx or Dxx triggered; bit1 - E6x triggered
							;bit2 - N_ROW was set by Dxx; bit3 - N_POS was set by Bxx

#if	xmem==1
#define 	ST_SIZE	11		;Size of the table for 24 bit
#else
#define 	ST_SIZE	8		;Size of the table for 16 bit
#endif

S_TAB:		.byte ST_SIZE*(N_MST+1)	;Table of sample addresses
;.equ 		S_ADR =	S_TAB+0		;Start
;.equ		S_REP =	S_TAB+2/3	;Rep
;.equ		S_EN =	S_TAB+4/6	;End
;.equ		CH_EN =	S_TAB+6/9	;Sample Enabled
;.equ		LP_EN =	S_TAB+7/10	;Loop Enabled

S_VOL_TAB:	.byte 1*(N_MST+1)	;Table of sample default volumes
S_FT_TAB:	.byte 1*(N_MST+1)	;Table of sample Finetune values


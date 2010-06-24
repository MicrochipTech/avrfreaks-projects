C5_TAB:		;Finetuned values for increment calculation
			;Value from offset 0-15 (FTUNE(8.8)) should be divided by
			;tone period PERn(16.0). The result is S_INCn(8.16)

			.dw int(8287*428*256/f_dac)	;0
			.dw int(8345*428*256/f_dac)	;1
			.dw int(8404*428*256/f_dac)	;2
			.dw int(8465*428*256/f_dac)	;3
			.dw int(8526*428*256/f_dac)	;4
			.dw int(8588*428*256/f_dac)	;5
			.dw int(8650*428*256/f_dac)	;6
			.dw int(8714*428*256/f_dac)	;7
			.dw int(7821*428*256/f_dac)	;-8
			.dw int(7881*428*256/f_dac)	;-7
			.dw int(7934*428*256/f_dac)	;-6
			.dw int(7997*428*256/f_dac)	;-5
			.dw int(8051*428*256/f_dac)	;-4
			.dw int(8107*428*256/f_dac)	;-3
			.dw int(8172*428*256/f_dac)	;-2
			.dw int(8229*428*256/f_dac)	;-1

SEMI_TAB:	;Multipliers for period to calculate new (period + n semitones)
			;Value from offset 0-15 is multiplied with period*2.
			.dw 32768	;0
			.dw 30928	;1
			.dw 29192	;2
			.dw 27554	;3
			.dw 26007	;4
			.dw 24548	;5
			.dw 23170	;6
			.dw 21870	;7
			.dw 20642	;8
			.dw 19483	;9
			.dw 18390	;A
			.dw 17358	;B
			.dw 16384	;C
			.dw 15464	;D
			.dw 14596	;E
			.dw 13777	;F

SIN_TAB:	;Sinus table for Tremolo/Vibrato
			;adder=sin*(depth*2+1)/64
			.db	0,6,12,19,25,31,36,41
			.db	46,51,54,58,60,63,64,65
			.db	66,65,64,63,60,58,54,51
			.db	46,41,36,31,25,19,12,6
			.db	0,-6,-12,-19,-25,-31,-36,-41
			.db	-46,-51,-54,-58,-60,-63,-64,-65
			.db	-66,-65,-64,-63,-60,-58,-54,-51
			.db	-46,-41,-36,-31,-25,-19,-12,-6

EFF_VEC:	;Table of vectors for effects procedures
			rjmp _0xx
			rjmp _1xx
			rjmp _2xx
			rjmp _3xx
			rjmp _4xx
			rjmp _5xx
			rjmp _6xx
			rjmp _7xx
			rjmp _8xx
			rjmp _9xx
			rjmp _Axx
			rjmp _Bxx
			rjmp _Cxx
			rjmp _Dxx
			rjmp _Exx
			rjmp _Fxx

EFE_VEC:	;Table of vectors for Ex effects procedures
			rjmp _E0x
			rjmp _E1x
			rjmp _E2x
			rjmp _E3x
			rjmp _E4x
			rjmp _E5x
			rjmp _E6x
			rjmp _E7x
			rjmp _E8x
			rjmp _E9x
			rjmp _EAx
			rjmp _EBx
			rjmp _ECx
			rjmp _EDx
			rjmp _EEx
			rjmp _EFx

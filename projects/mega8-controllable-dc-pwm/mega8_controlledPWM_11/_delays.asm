;******************************************************************************
;_Delays.asm Version 1.0a
;******************************************************************************
;
;Written for  : Atmel "AVR Assembler" and "AVR Assembler 2"
;        when : 2006 May 10
;        where: BRUSSELS - BELGIUM - EUROPE
;        who  : Jan Huygh
;       E-mail: jan.huygh@skynet.be

;******************************************************************************
;ABOUT VERSION 0.0a
;
;This is the 2nd posted version.
;
;  KNOWN BUGS :
;    * None so far
;  OPPORTUNITIES FOR IMPROVEMENT :
;    * a version that can be used as a subroutine taking in to acount the time
;      for the call and for the return.
;    * a version that takes the input from the stack so it can take a variable
;      for the input.
;    * a version that is less precise but takes less bytes of program memory
;		

;******************************************************************************
;.INCLUDE "_Delays.asm" enables you to use the following macro in your code
;
;	_Waiste_us n
;		With  (1.000.000/F_CPU) <= n <= 25769803779/Your_CPU_frequency_in_MHz
;
; 	     This macro will generate a wait loop from 1 탎 to 25.769.803.779 탎
;            Depending on the CPU-speed set in F_CPU here below you will need to
;            respect a minimum value for n.  (n_minimum) = 1.000.000/F_CPU).
;            This is needed because we can not loop for less then 1
;            instruction cycle.
;	     25.769.803.779 탎, that's over 3h30 when you use a CPU running at 20 MHz.
;        _Waiste_us is absolutely exact down to (1/Your_CPU_frequency_in_MHz) 탎
;        _Waiste_us will pick the smallest code for your delay (Maximum 13 words)

;******************************************************************************
;How to use this in your code :
;
; A) Pick the correct ".equ F_CPU = x" statement
;    AND
;    save this file to your disk
;
; B) Make sure it is in the same folder as the other files of your current project
;    OR
;    Make sure the folder where you saved this file is referenced in the
;    assembler options (Project=>Assembler Options=>Additional include path)
;
; C) Include the statement .INCLUDE "_Delays.asm" at the top of your program
;
; D) Use the statement _Waiste_us n, in your code
;
;Here is a code example :
;
;	.INCLUDE "_Waiste.asm"	;Make the macro _Waiste_us n available in your code
;	_Waiste_us 100		;loop for       100 탎 (that's  0,1 ms)
;	_Waiste_us 16000   	;loop for    16.000 탎 (that's 16   ms)
;	_Waiste_us 7000000	;loop for 7.000.000 탎 (that's  7   s )
;

;******************************************************************************
;The CPU-speed needs to be set before you can use _Waiste_us.
;Find your CPU-speed in the list here below and uncomment it (remove the ";")
;then press Ctrl^S to save to disk otherwise the include file used by AVR studio
;will be the unchanged version on your disk.
;
;											    
;	.equ	F_CPU =  1000000 ;Hz
;	.equ	F_CPU =  2000000 ;Hz
;	.equ	F_CPU =  3579545 ;Hz
;	.equ	F_CPU =  3686400 ;Hz
;	.equ	F_CPU =  4000000 ;Hz
;	.equ	F_CPU =  4032000 ;Hz
;	.equ	F_CPU =  4096000 ;Hz
;	.equ	F_CPU =  4194304 ;Hz
;	.equ	F_CPU =  4433619 ;Hz
;	.equ	F_CPU =  4915200 ;Hz
;	.equ	F_CPU =  5000000 ;Hz
;	.equ	F_CPU =  5068800 ;Hz
;	.equ	F_CPU =  5990400 ;Hz
;	.equ	F_CPU =  6000000 ;Hz
;	.equ	F_CPU =  6144000 ;Hz
;	.equ	F_CPU =  6500000 ;Hz
;	.equ	F_CPU =  7372800 ;Hz
;	.equ	F_CPU =  7680000 ;Hz
	.equ	F_CPU =  8000000 ;Hz
;	.equ	F_CPU =  9000000 ;Hz
; 	.equ	F_CPU =  9400000 ;Hz
;	.equ	F_CPU =  9216000 ;Hz
;	.equ	F_CPU =  9830400 ;Hz
;	.equ	F_CPU = 10000000 ;Hz
;	.equ	F_CPU = 10240000 ;Hz
;	.equ	F_CPU = 11000000 ;Hz
;	.equ	F_CPU = 11059200 ;Hz
;	.equ	F_CPU = 11520000 ;Hz
;	.equ	F_CPU = 12000000 ;Hz
;	.equ	F_CPU = 12000393 ;Hz
;	.equ	F_CPU = 12288000 ;Hz
;	.equ	F_CPU = 13500000 ;Hz
;	.equ	F_CPU = 14318180 ;Hz
;	.equ	F_CPU = 14745600 ;Hz
;	.equ	F_CPU = 15000000 ;Hz
;	.equ	F_CPU = 15360000 ;Hz
;	.equ	F_CPU = 16000000 ;Hz
;	.equ	F_CPU = 16000312 ;Hz
;	.equ	F_CPU = 16257000 ;Hz
;	.equ	F_CPU = 16384000 ;Hz
;	.equ	F_CPU = 17734475 ;Hz
;	.equ	F_CPU = 18000000 ;Hz
;	.equ	F_CPU = 18432000 ;Hz
;	.equ	F_CPU = 19660800 ;Hz
;	.equ	F_CPU = 20000000 ;Hz


.Macro _Waiste_us ;n
;	Calculate the number of CPU-cycles needed to generate the requested delay
;	taking in to account the frequency you are using for your CPU.
;   You can simplify the next 9 lines of code before "_Cycle_Waister Cycles_Needed"
;   to "Cycles_Needed = @0 * F_CPU / 1000000". If you do so _Waist_us will
;   generate the exact delay you requested or the delay obtained by executing the
;   number of cpu-cycles that brings just less then the delay you requested.
;   This might be a good option if you need a certain delay but need to be sure
;   you never have a delay that is even a fraction of a CPU-cycle longer than the
;   delay you requested.
;	The 9 lines of code here below do NOT use any space in the actual program
;   memory of your AVR.
;	The code here below will actually pick the number of CPU-cycles needed to
;   generate EXACTLY the delay you requested and when that can not be achieved
;   it will select the number of cycles that generate the delay that is the best 
;   approximation for the delay you requested even if that is just above the 
;   delay you requested.
	.set	Fraction = @0*F_CPU/1000000					;
	.set 	Fraction = Fraction * 100
	.set	Fraction = @0*F_CPU/10000 - Fraction
	.if		Fraction >= 50
		.set 	Cycles_Needed = (@0 * F_CPU /1000000)+1
	.endif
	.if		Fraction < 50
		.set 	Cycles_Needed = (@0 * F_CPU /1000000)
	.endif
	_Cycle_Waister Cycles_Needed
.endmacro

.macro _Cycle_Waister
	.If @0 == 1
		nop
	.EndIf
	.If @0 == 2
		nop
		nop
	.EndIf
	.If ((@0 > 2) && (@0 < 769))
	;C=3n
	;769=(3 * (2^8)) + 1
		.set	Loops_Needed = @0/3
		ldi		R22,Low(Loops_Needed)
  	   Loop:
  		dec		R22
		brne	Loop
		.set	Cycles_Left_To_Do = @0 - (3*Loops_Needed)
		_Cycle_Waister Cycles_Left_To_Do
	.EndIf
	.If (@0 > 768) && (@0 < 262146)
	;C=1+4n
	;262146 = 1 + (4 * 2^16) + 1
		.set	Loops_Needed = (@0 - 1)/4
		ldi		ZL,Low(Loops_Needed)
		ldi		ZH,High(Loops_Needed)
  	   Loop:	
  		sbiw	ZL,1
		brne	Loop
		.set Cycles_Left_To_Do = (@0 - (4*Loops_Needed))-1
		_Cycle_Waister Cycles_Left_To_Do
	.EndIf
	.If (@0 > 262145) && (@0 < 83886083)
	;C=2+5n
	;83.886.083 = 2 + (5 * 2^24) +1
		.set	Loops_Needed = (@0 - 2)/5
		ldi		R22,Low(Loops_Needed)
		ldi		ZL,Byte2(Loops_Needed)
		ldi		ZH,Byte3(Loops_Needed)
  	   Loop:
  		subi	R22,1
		sbci	ZL,0
		sbci	ZH,0
		brne	Loop
		.set Cycles_Left_To_Do = (@0 - (5*Loops_Needed))-2
		_Cycle_Waister Cycles_Left_To_Do
	.EndIf
	.If (@0 > 83886082) && (@0 < 25769803780)
	;C=3+6n
	;25769803780 = 3 + (6 * 2^32) +1
		.set	Loops_Needed = (@0 - 3)6
		ldi		XL,Low(Loops_Needed)
		ldi		XH,Byte2(Loops_Needed)
		ldi		ZL,Byte3(Loops_Needed)
		ldi		ZH,Byte4(Loops_Needed)
  	   Loop:
   		sbiw	Xl
		sbci	ZL,0
		sbci	ZH,0
		brne	Loop
		.set Cycles_Left_To_Do = (@0 - (6*Loops_Needed)) - 3
		_Cycle_Waister Cycles_Left_To_Do
	.endif
.endmacro
























;<$DS_HEADER>
;+--------------------------------------------------------------------------------+              
;|João D´Artagnan A. Oliveira Programmer and Author; 				  |
;|										  |
;| All rights reserved.								  |
;|										  |
;|   Redistribution and use in source and binary forms, with or without		  |
;|  modification, are permitted provided that the following conditions are met:	  |
;|										  |
;|   * Redistributions of source code must retain the above copyright		  |
;|     notice, this list of conditions and the following disclaimer.		  |
;|										  |
;|   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"  |
;|   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE    |
;|   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   |
;|   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE     |
;|   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR	  |
;|   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF	  |
;|   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS	  |
;|   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN	  |
;|   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)	  |
;|   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE   |
;|   POSSIBILITY OF SUCH DAMAGE.						  |
;|										  |
;|   Brasília, Brazil November 30,2015 	06:28 am				  |
;+--------------------------------------------------------------------------------+
;</$DS_HEADER>
;+--------------------------------------------------------------+---+-----------+
;|										|
;| DATE:23/01/2003 VERSION 1.0							|
;>--------------------------------------------------------------+---------------<
;| D O U B L E   F L O A T  P O I N T  R O U T I N E S		|
;| G E N E R A L  S I N E   					|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM		|
;+--------------------------------------------------------------+
;+----------------------------------------------------------------------+	
;| DOUBLE FLOAT POINT SINE 						|
;|									|
;| Input  : Float Acc X	precision loss if x>+/-1e16			|
;| Output : Float Acc sine of X,Acc Exception code			|
;| Destroy: Flags							|
;| Method : Describe below						|
;| 									|
;+----------------------------------------------+-----------------------+
;|						|
;| IMPLEMENTS A GENERAL SINE FUNCTION		|
;|						|
;| The function SERSIN compute a sine but only	|
;| range from -pi/2 to pi/2 radians, using this	|
;| function to calculate any angle we need to 	|
;| reduce to the first quadrant any input argu-	|
;| ment arg. to make this we perform the follo-	|
;| wing steps.					|
;|						|
;| 1o Divide arg by 2*pi to reduce a arg to 4   |
;| quadrants.					|
;|						|
;| 2o Get absolute value of arg			|
;|						|
;| 3o See if arg great than or igual 3*pi/2, if	|
;| yes compute -SERSIN(2*pi-arg)		|
;|						|
;| 4o See if arg great than or igual pi, if yes	|
;| compute SERSIN(arg-pi)			|
;|						|
;| 5o See if arg great than or igual pi/2, if	|
;| yes compute SERSIN(pi-arg)			|
;|						|
;| trigonometric circle				|
;|						|
;|		        |<-- SERSIN RANGE-->    |
;|                 *****|******                 |           
;|	       ****     |      ****             |           
;|	     ***        |         ***           |           
;|	   **2o quadrant|1o quadrant**          |          
;|	  **            |             **        |           
;|	 ** arg=pi-arg  | arg=arg      **       |           
;|	**              |               **      |           
;|	*               |                *      |           
;|   -------------------+-------------------	|
;|	*               |                *      |           
;|	**  3o quadrant |4o quadrant    **      |           
;|	 ** arg=arg-pi  |arg=2*pi-arg  **       |           
;|	  **            |             **        |           
;|	   **           |            **         |           
;|	     ***        |         ***           |           
;|	      ****      |     ****              |          
;|	          ******|*****           	|
;|						|
;+----------------------------------------------+
#ifndef __MATH_FLOAT_SINE__
	#define __MATH_FLOAT_SINE__
	.message "[ (MATH) FLOAT SINE ]"
	;+----------------------+
	;| NECESSARY INCLUDES	|
	;+----------------------+
	#ifndef __MATH_FLOAT_INFINIT__
		.INCLUDE	"MATH\FLOAT_DOUBLE\INFINIT_RESULT\DFINF.INC"
	#endif
	#ifndef __MATH_FLOAT_NORMALIZE__
		.INCLUDE	"MATH\FLOAT_DOUBLE\NORMALIZE\DFNORM.INC"
	#endif
	#ifndef __MATH_FLOAT_ADDSUB__
		.INCLUDE	"MATH\FLOAT_DOUBLE\ADDSUB\DFADDSUB.INC"
	#endif
	#ifndef __MATH_FLOAT_MULDIV__
		.INCLUDE	"MATH\FLOAT_DOUBLE\MULDIV\DFMULDIV.INC"
	#endif
	#ifndef __MATH_FLOAT_LOAD_STORE__
		.INCLUDE	"MATH\FLOAT_DOUBLE\LOAD_STORE\DFLDXSTX.INC"
	#endif
	#ifndef __MATH_FLOAT_COMPARE__
		.INCLUDE	"MATH\FLOAT_DOUBLE\COMPARE\DFCPM.INC"
	#endif
	#ifndef __MATH_FLOAT_SINE_SERIE__
		.INCLUDE	"MATH\FLOAT_DOUBLE\SERIES\SINE\SERIE_SINE.INC"
	#endif
	_DFSIN:
		push	_acc_s					;save arg signal
		rcall	_DFABS					;acc=abs arg
		rcall	_DFACCOP1				;op1=arg  perform acc=2*pi*(fraction(arg/2*pi))
		rcall	_DFLD2PI				;acc=2*pi
		rcall	_DFACCOP2				;op2=2*pi
		rcall	_DFDIVFRAC				;acc=fraction(op1/op2)
		rcall	_DFACCOP1				;op1=fraction(op1/op2)
		rcall	_DFLD2PI				;acc=2*pi again
		rcall	_DFACCOP2				;op2=2*pi
		rcall	_DFMUL					;acc=result
		rcall	_DFACCOP1				;op1=result
		rcall	_DFLD3PI2				;acc=3*pi/2
		rcall	_DFACCOP2				;op2=3*pi/2
		_DFJP_LT	_DFSIN10			;arg>=3*pi/2 ? branch if no
		rcall	_DFSIN_OP2OP1				;op2=op1	REDUCE 4o TO 1o QUADRANT
		rcall	_DFLD2PI				;acc=2*pi
		rcall	_DFACCOP1				;op2=2*pi
	_DFSIN00:
		rcall	_DFSUB					;acc=2*pi-arg
		rcall	_DFSERSIN				;acc=partial sine of arg
		rcall	_DFCHS					;acc=-acc
		rjmp	_DFSIN50				;last computation
	_DFSIN10:
		rcall	_DFLDPI					;acc=pi
		rcall	_DFACCOP2				;op2=pi
		_DFJP_LT	_DFSIN20			;arg>=pi ? branch if no
		rjmp	_DFSIN00				;else acc=arg-pi
	_DFSIN20:
		rcall	_DFLDPI2				;acc=pi/2	REDUCE 3o TO 1o QUADRANT
		rcall	_DFACCOP2				;op2=pi/2
		_DFJP_LT	_DFSIN30			;arg>pi/2 ? branch if no
		rcall	_DFSIN_OP2OP1				;op2=op1	REDUCE 2o TO 1o QUADRANT
		rcall	_DFLDPI					;acc=pi
		rcall	_DFACCOP1				;op2=pi
		rjmp	_DFSIN40
	_DFSIN30:	
		rcall	_DFLD0					;acc=0		AT 1o QUADRANT
		rcall	_DFACCOP2				;op2=0
	_DFSIN40:
		rcall	_DFSUB					;acc=arg-0
		rcall	_DFSERSIN				;acc=partial sine of arg
	_DFSIN50:
		pop	Acc					;see if arg<0
		tst	Acc					;branch if no
		brge	_DFSIN60
		rcall	_DFCHS					;else arg=-arg
	_DFSIN60:
		ret						;end of sine function
	;+-----------------+
	;| MOVE OP2<--OP1  |	
	;+-----------------+
	_DFSIN_OP2OP1:
		mov	_op2_0,_op1_0				;op2=arg
		mov	_op2_1,_op1_1
		mov	_op2_2,_op1_2
		mov	_op2_3,_op1_3
		mov	_op2_4,_op1_4
		mov	_op2_5,_op1_5
		mov	_op2_s,_op1_s
		mov	_op2_e,_op1_e
		ret
#endif
.exit


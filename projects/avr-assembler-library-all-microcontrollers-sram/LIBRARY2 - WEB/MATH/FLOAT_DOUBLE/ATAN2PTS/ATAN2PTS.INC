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
;+------------------------------------------------------------------------------+
;|										|
;| DATE:23/01/2003 VERSION 1.0							|
;>------------------------------------------------------------------------------<
;| D O U B L E   F L O A T  P O I N T  R O U T I N E S				|
;| G E N E R A L  A R C  T A N G E N T	B E T W E E N  T W O  P O I N T S 	|
;+--------------------------------------------------------------+---------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM		|
;+--------------------------------------------------------------+
;| Perform calculus using below code				|
;|
;| assume (x,y) a point and (cx,cy) a center point
;|
;|'
;|'if cx=x check if y above or under cy
;|'
;|   If cx = X Then
;|      If Y > cy Then
;|         MATH_ATN2PTS = 3 * P_PI / 2
;|      Else
;|        If Y < cy Then
;|           MATH_ATN2PTS = P_PI / 2
;|        Else
;|           MATH_ATN2PTS = 0
;|        End If
;|      End If
;|      Exit Function
;|   Else
;|'
;|'if cy=y check if x left or right cx
;|'
;|      If cy = Y Then
;|         If X > cx Then
;|            MATH_ATN2PTS = 0
;|         Else
;|            If X < cx Then
;|               MATH_ATN2PTS = P_PI
;|            Else
;|               MATH_ATN2PTS = 0
;|            End If
;|         End If
;|         Exit Function
;|      End If
;|   End If
;|'
;|'compute angle according quadrant
;|'
;|   dx = X - cx
;|   dy = Y - cy
;|   alfa = Atn(Abs(dy) / Abs(dx))
;|   If dx > 0 Then
;|      If dy > 0 Then
;|         MATH_ATN2PTS = 2 * P_PI - alfa
;|      Else
;|         MATH_ATN2PTS = alfa
;|      End If
;|   Else
;|      If dy > 0 Then
;|         MATH_ATN2PTS = alfa + P_PI
;|      Else
;|         MATH_ATN2PTS = P_PI - alfa
;|      End If
;|   End If
;|
;|
#ifndef __MATH_FLOAT_ATAN2PTS__
	
	#define __MATH_FLOAT_ATAN2PTS__
	.message "[ (MATH) FLOAT ARC TANGENT TWO POINTS]"
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
	
	#ifndef __MATH_FLOAT_UNARY__
		.INCLUDE	"MATH\FLOAT_DOUBLE\UNARY\UNARY.INC"
	#endif
	#ifndef __MATH_FLOAT_LOAD_STORE__
		.INCLUDE	"MATH\FLOAT_DOUBLE\LOAD_STORE\DFLDXSTX.INC"
	#endif
	
	#ifndef __MATH_FLOAT_COMPARE__
		.INCLUDE	"MATH\FLOAT_DOUBLE\COMPARE\DFCPM.INC"
	#endif
	#ifndef __MATH_FLOAT_ARC_TANGENT_SERIE__
		.INCLUDE	"MATH\FLOAT_DOUBLE\SERIES\ATN\SERIE_ATN.INC"
	#endif
	#ifndef __MATH_FLOAT_ARC_TANGENT__
		.INCLUDE	"MATH\FLOAT_DOUBLE\ATAN\ATAN.INC"
	#endif
	;@----------------@
	;| used variables |
	;@----------------@
	
	.DSEG
		_DFATAN2PTS_CENTER_X:	.BYTE	_FDOUBLE
		_DFATAN2PTS_CENTER_Y:	.BYTE	_FDOUBLE
		_DFATAN2PTS_POINT_X:	.BYTE	_FDOUBLE
		_DFATAN2PTS_POINT_Y:	.BYTE	_FDOUBLE
	.CSEG
	;+----------------------------------------------------------------------+	
	;| DOUBLE FLOAT SET ARC TANGENT BETWEEN 2 POINTS CENTER POINT X	 	|
	;| 									|
	;| Input  : Float Acc Center X						|
	;| Output : none							|
	;| Destroy: Flags							|
	;| Method : 								|
	;| 									|
	;+----------------------------------------------------------------------+
	_DFATAN2PTS_SET_CENTER_X:
		pushw	Z
		ldiw	Z,_DFATAN2PTS_CENTER_X			;store center X
	_DFATAN2PTS_SET_CENTER_X_EXIT:
		call 	_DFSTZ
		popw	Z
		ret
	;+----------------------------------------------------------------------+	
	;| DOUBLE FLOAT SET ARC TANGENT BETWEEN 2 POINTS CENTER POINT Y	 	|
	;| 									|
	;| Input  : Float Acc Center Y						|
	;| Output : none							|
	;| Destroy: Flags							|
	;| Method : 								|
	;| 									|
	;+----------------------------------------------------------------------+
	_DFATAN2PTS_SET_CENTER_Y:
		pushw	Z
		ldiw	Z,_DFATAN2PTS_CENTER_Y			;store center Y
		rjmp	_DFATAN2PTS_SET_CENTER_X_EXIT
	;+----------------------------------------------------------------------+	
	;| DOUBLE FLOAT SET ARC TANGENT BETWEEN 2 POINTS POINT X	 	|
	;| 									|
	;| Input  : Float Acc Point X						|
	;| Output : none							|
	;| Destroy: Flags							|
	;| Method : 								|
	;| 									|
	;+----------------------------------------------------------------------+
	_DFATAN2PTS_SET_POINT_X:
		pushw	Z
		ldiw	Z,_DFATAN2PTS_POINT_X			;store point X
		rjmp	_DFATAN2PTS_SET_CENTER_X_EXIT
	;+----------------------------------------------------------------------+	
	;| DOUBLE FLOAT SET ARC TANGENT BETWEEN 2 POINTS POINT Y	 	|
	;| 									|
	;| Input  : Float Acc Point Y						|
	;| Output : none							|
	;| Destroy: Flags							|
	;| Method : 								|
	;| 									|
	;+----------------------------------------------------------------------+
	_DFATAN2PTS_SET_POINT_Y:
		pushw	Z
		ldiw	Z,_DFATAN2PTS_POINT_Y			;store point Y
		rjmp	_DFATAN2PTS_SET_CENTER_X_EXIT		
	_DFATAN2PTS:
#endif
.exit


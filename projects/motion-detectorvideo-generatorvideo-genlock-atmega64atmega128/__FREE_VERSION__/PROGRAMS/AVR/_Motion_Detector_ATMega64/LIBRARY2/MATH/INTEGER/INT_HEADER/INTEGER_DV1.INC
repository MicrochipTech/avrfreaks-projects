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
;+--------------------------------------------------------------+---+
;|							|
;| DATE:24/05/2001 VERSION 1.0				|
;>------------------------------------------------------<
;| I N T E G E R  R O U T I N E S			|
;| A L L  I N T E G E R  F U N C T I O N S		|
;+------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM	|
;+------------------------------------------------------+
;+--------------------------------------------------------------------------+
;!                                                                          !
;!      Integer type memory formating					    !
;!                                                                          !
;| BYTE OR ASCII							    |
;|									    |	
;!      BITS --->       7 6 5 4 3 2 1 0                                     !
;!                     +-+-+-+-+-+-+-+-+                                    !
;!      Byte M+0       !m!m!m!m!m!m!m!m! 		                    !
;!                     +-+-+-+-+-+-+-+-+                                    !
;|	Natural numbers range    0..255					    |
;|	Relative numbers range -128..127				    |
;|									    |
;| INTEGER								    |
;|									    |	
;!      BITS --->       7 6 5 4 3 2 1 0                                     !
;!                     +-+-+-+-+-+-+-+-+                                    !
;!      Byte M+0       !m!m!m!m!m!m!m!m!  Lsb		                    !
;!                     +-+-+-+-+-+-+-+-+                                    !
;!      Byte M+1       !m!m!m!m!m!m!m!m!  Msb		                    !
;!                     +-+-+-+-+-+-+-+-+                                    !
;|	Natural numbers range       0..65535				    |
;|	Relative numbers range -32768..32767				    |
;|									    |
;| LONG INTEGER								    |
;|									    |	
;!      BITS --->       7 6 5 4 3 2 1 0                                     !
;!                     +-+-+-+-+-+-+-+-+                                    !
;!      Byte M+0       !m!m!m!m!m!m!m!m!  Lsb				    !
;!                     +-+-+-+-+-+-+-+-+                                    !
;!      Byte M+1       !m!m!m!m!m!m!m!m!				    !
;!                     +-+-+-+-+-+-+-+-+                                    !
;!      Byte M+2       !m!m!m!m!m!m!m!m! 				    !
;!                     +-+-+-+-+-+-+-+-+                                    !
;!      Byte M+3       !m!m!m!m!m!m!m!m!  Msb			            !
;!                     +-+-+-+-+-+-+-+-+                                    !
;|	Natural numbers range           0..4294967296			    |
;|	Integer numbers range -2147483648..2147483647			    |
;!   									    |
;+--------------------------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N S 		|
;+------------------------------------------------------+---------------------------------------+
;|												|
;| _DFINIT	Float Point Initialize		 						|
;|												|
;| Obs:		Use this function to initialize double float point routines			|
;+----------------------------------------------------------------------------------------------+
;| _DFUNDER	Set Fload Point Accumulator to zero and return Acc=_FEUNDER 			|
;+----------------------------------------------------------------------------------------------+
;| _DFOVER	Set Float Point Accumulator to max number positive or negative According a value|
;|		Return Acc=_FEOVER								|
;+----------------------------------------------------------------------------------------------+
;| _DFDIV0	Set Float Point Accumulator to max number positive or negative according a value|
;|		Return Acc=_FEDIV0								|
;+----------------------------------------------------------------------------------------------+
;| _DFNORM	Normalize Float Point Accumulator 						|
;+----------------------------------------------------------------------------------------------+
;| _DFLTOF	Convert long integer signed number to float point				|
;|												|
;| 		Input  : Acc:AccH:AccT:AccTH signed long integer  				|
;|		Output : Floating Accumulator		     					|
;|	    		 Acc=FPok or FUndeflow		     					|
;| 		Destroy: Flags	                             					|
;+----------------------------------------------------------------------------------------------+
;| _DFITOF	Convert integer signed number to float point					|
;|												|
;| 		Input  : Acc:AccH signed integer		  				|
;|		Output : Floating Accumulator		     					|
;|	    		 Acc=FPok or FUndeflow		     					|
;| 		Destroy: Flags	                             					|
;+----------------------------------------------------------------------------------------------+
;| _DFATOF	Convert signed byte number to float point					|
;| _DFBTOF											|
;| 		Input  : Acc signed byte			  				|
;|		Output : Floating Accumulator		     					|
;|	    		 Acc=FPok or FUndeflow		     					|
;| 		Destroy: Flags	                             					|
;+----------------------------------------------------------------------------------------------+
;| _DFFTOL	Convert float point to signed long integer number				|
;| 												|
;| 		Input  : Floating Accumulator			  				|
;|		Output : Acc:AccH:AccT:AccTH long integer     					|
;|	    		 Acc=FPok or FUndeflow		     					|
;| 		Destroy: Flags,Float Op1                            				|
;+----------------------------------------------------------------------------------------------+
;| _DFMUL	Float point multiply Op1 by Op2							|
;| _DFMMUL (MACRO)										|
;| 		Input  : Op1,Op2 Float number to multiply	  				|
;|		Output : Float Accumulator		     					|
;|	    		 Acc=Exception code		     					|
;| 		Destroy: Op1,Op2,Acc,AccH,Flags                        				|
;|		Operation: FAcc=Op1*Op2								|
;+----------------------------------------------------------------------------------------------+
;| _DFDIV	Float point divide Op1 by Op2							|
;| _DFMDIV (MACRO)										|
;| 		Input  : Op1,Op2 Float number to divide		  				|
;|		Output : Float Accumulator		     					|
;|	    		 Acc=Exception code		     					|
;| 		Destroy: Op1,Op2,Acc,AccH,Flags                        				|
;|		Operation: FAcc=Op1/Op2								|
;+----------------------------------------------------------------------------------------------+
;| _DFMUL2	Float point multiply Float Accumulator by 2					|
;| _DFMMUL2 (MACRO)										|
;| 		Input  : Float Accumulator			  				|
;|		Output : Float Accumulator		     					|
;|	    		 Acc=Exception code		     					|
;| 		Destroy: Op1,Op2,Acc,AccH,Flags                        				|
;|		Operation: FAcc=FAcc*2								|
;+----------------------------------------------------------------------------------------------+
;| _DFDIV2	Float point divide Float Accumulator by 2					|
;| _DFMDIV2 (MACRO)										|
;| 		Input  : Float Accumulator			  				|
;|		Output : Float Accumulator		     					|
;|	    		 Acc=Exception code		     					|
;| 		Destroy: Op1,Op2,Acc,AccH,Flags                        				|
;|		Operation: FAcc=FAcc/2								|
;+----------------------------------------------------------------------------------------------+
;| _DFADD	Float point add Op1 with Op2							|
;| _DFMADD (MACRO)										|
;| 		Input  : Op1,Op2 Float number to add		  				|
;|		Output : Float Accumulator		     					|
;|	    		 Acc=Exception code		     					|
;| 		Destroy: Op1,Op2,Acc,AccH,Flags                        				|
;|		Operation: FAcc=Op1+Op2								|
;+----------------------------------------------------------------------------------------------+
;| _DFSUB	Float point subtract Op2 from Op1						|
;| _DFMSUB (MACRO)										|
;| 		Input  : Op1,Op2 Float number to subtract	  				|
;|		Output : Float Accumulator		     					|
;|	    		 Acc=Exception code		     					|
;| 		Destroy: Op1,Op2,Acc,AccH,Flags                        				|
;|		Operation: FAcc=Op1-Op2								|
;+----------------------------------------------------------------------------------------------+
;+-------------------------+
;! CONSTANTS DEFINITION	   !
;+-------------------------+
.include	"\Dart\Avr\Library\Math\Defs\MathCons.inc"	;Constants, Registers and SRAM definitions
;+------------------+
;| INCLUDE LIBRARYS |
;+------------------+
.include	"\Dart\Avr\Library\Math\FloatDR\DFINF.inc"	;Infinit normalization routines
.include	"\Dart\Avr\Library\Math\FloatDR\DFNORM.inc"	;Normalization routine
.include	"\Dart\Avr\Library\Math\FloatDR\DFXTOF.inc"	;Integer to Float conversion routines
.include	"\Dart\Avr\Library\Math\FloatDR\DFFTOX.inc"	;Float to Integer conversion routines
.include	"\Dart\Avr\Library\Math\FloatDR\DFMULDIV.inc"	;Float Multiply and Divide routines
.include	"\Dart\Avr\Library\Math\FloatDR\DFADDSUB.inc"	;Float Addiction and Subtraction routines
.include	"\Dart\Avr\Library\Math\FloatDR\DFLDXSTX.inc"	;Float load and store variables routines
.include	"\Dart\Avr\Library\Math\FloatDR\DFSTOF.inc"	;String to Float routines
.include	"\Dart\Avr\Library\Math\FloatDR\DFFTOS.inc"	;Float to String with formatting routines
.include	"\Dart\Avr\Library\Math\FloatDR\DFCPM.inc"	;Float to Compare routines
.include	"\Dart\Avr\Library\Math\FloatDR\DFSQRT.inc"	;Float square root routin


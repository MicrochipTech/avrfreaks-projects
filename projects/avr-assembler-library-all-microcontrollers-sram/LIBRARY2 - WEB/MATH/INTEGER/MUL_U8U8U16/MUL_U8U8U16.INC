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
;/////////////////////////////////////////////////////////////////////////////////|
;/|										|/|
;/|			______  _____                            		|/|
;/|			|  _  \/  ___|                           		|/|
;/|			| | | |\ `--.  __ _ _   _  __ _ _ __ ___ 		|/|
;/|			| | | | `--. \/ _` | | | |/ _` | '__/ _ \		|/|
;/|			| |/ / /\__/ / (_| | |_| | (_| | | |  __/		|/|
;/|			|___/  \____/ \__, |\__,_|\__,_|_|  \___|		|/|
;/|		                 	 | |                     		|/|
;/|			                 |_|                     		|/|
;/|	 	       _____                     _             _       		|/|
;/|		      |_   _|                   | |           (_)      		|/|
;/|	  	  	| | ___  ___ _ __   ___ | | ___   __ _ _  __ _ 		|/|
;/|	  	  	| |/ _ \/ __| '_ \ / _ \| |/ _ \ / _` | |/ _` |		|/|
;/|		  	| |  __/ (__| | | | (_) | | (_) | (_| | | (_| |		|/|
;/|		 	\_/\___|\___|_| |_|\___/|_|\___/ \__, |_|\__,_|		|/|
;/|	          	       	                          __/ |        		|/|
;/|	       	        	                         |___/         		|/|
;/|		 	 _     _ _                          			|/|
;/|			| |   (_) |                         			|/|
;/|			| |    _| |__  _ __ __ _ _ __ _   _ 			|/|
;/|			| |   | | '_ \| '__/ _` | '__| | | |			|/|
;/|			| |___| | |_) | | | (_| | |  | |_| |			|/|
;/|			\_____/_|_.__/|_|  \__,_|_|   \__, |			|/|
;/|	                	               	       __/ |			|/|
;/|		                	              |___/ 			|/|
;/|										|/|
;/|			______      _           				|/|
;/|			|  _  \    (_)          				|/|
;/|			| | | |_ __ ___   _____ 				|/|
;/|			| | | | '__| \ \ / / _ \				|/|
;/|			| |/ /| |  | |\ V /  __/				|/|
;/|			|___/ |_|  |_| \_/ \___|				|/|
;/|										|/|
;/////////////////////////////////////////////////////////////////////////////////|
;+--------------------------------------------------------------------------------+              
;| DSquare Libraries,Drives routines can be freely used and redistributed,	  |
;| provided the following license conditions are met.				  |
;|										  |
;|Portions of DSquare are Copyright (c) 2000-2008				  |
;|										  |
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
;|   * Redistributions in binary form must reproduce the above copyright	  |
;|     notice, this list of conditions and the following disclaimer in		  |
;|     the documentation and/or other materials provided with the		  |
;|     distribution.								  |
;|										  |
;|   * Neither the name of the copyright holders nor the names of		  |
;|     contributors may be used to endorse or promote products derived            |
;|     from this software without specific prior written permission.		  |
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
;+--------------------------------------------------------------------------------+
;</$DS_HEADER>
;/////////////////////////////////////////////////////////////////////////////////|
;/|										|/|
;/|			______  _____                            		|/|
;/|			|  _  \/  ___|                           		|/|
;/|			| | | |\ `--.  __ _ _   _  __ _ _ __ ___ 		|/|
;/|			| | | | `--. \/ _` | | | |/ _` | '__/ _ \		|/|
;/|			| |/ / /\__/ / (_| | |_| | (_| | | |  __/		|/|
;/|			|___/  \____/ \__, |\__,_|\__,_|_|  \___|		|/|
;/|		                 	 | |                     		|/|
;/|			                 |_|                     		|/|
;/|	 	       _____                     _             _       		|/|
;/|		      |_   _|                   | |           (_)      		|/|
;/|	  	  	| | ___  ___ _ __   ___ | | ___   __ _ _  __ _ 		|/|
;/|	  	  	| |/ _ \/ __| '_ \ / _ \| |/ _ \ / _` | |/ _` |		|/|
;/|		  	| |  __/ (__| | | | (_) | | (_) | (_| | | (_| |		|/|
;/|		 	\_/\___|\___|_| |_|\___/|_|\___/ \__, |_|\__,_|		|/|
;/|	          	       	                          __/ |        		|/|
;/|	       	        	                         |___/         		|/|
;/|		 	_     _ _                          			|/|
;/|			| |   (_) |                         			|/|
;/|			| |    _| |__  _ __ __ _ _ __ _   _ 			|/|
;/|			| |   | | '_ \| '__/ _` | '__| | | |			|/|
;/|			| |___| | |_) | | | (_| | |  | |_| |			|/|
;/|			\_____/_|_.__/|_|  \__,_|_|   \__, |			|/|
;/|	                	               	       __/ |			|/|
;/|		                	              |___/ 			|/|
;/|										|/|
;/|			______      _           				|/|
;/|			|  _  \    (_)          				|/|
;/|			| | | |_ __ ___   _____ 				|/|
;/|			| | | | '__| \ \ / / _ \				|/|
;/|			| |/ /| |  | |\ V /  __/				|/|
;/|			|___/ |_|  |_| \_/ \___|				|/|
;/|										|/|
;/////////////////////////////////////////////////////////////////////////////////|
;+--------------------------------------------------------------------------------+              
;| DSquare Libraries,Drives routines can be freely used and redistributed,	  |
;| provided the following license conditions are met.				  |
;|										  |
;|Portions of DSquare are Copyright (c) 2000-2008				  |
;|										  |
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
;|   * Redistributions in binary form must reproduce the above copyright	  |
;|     notice, this list of conditions and the following disclaimer in		  |
;|     the documentation and/or other materials provided with the		  |
;|     distribution.								  |
;|										  |
;|   * Neither the name of the copyright holders nor the names of		  |
;|     contributors may be used to endorse or promote products derived            |
;|     from this software without specific prior written permission.		  |
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
;+--------------------------------------------------------------------------------+
;</$DS_HEADER>
;/////////////////////////////////////////////////////////////////////////|
;/|									|/|
;/|		______  _____                            		|/|
;/|		|  _  \/  ___|                           		|/|
;/|		| | | |\ `--.  __ _ _   _  __ _ _ __ ___ 		|/|
;/|		| | | | `--. \/ _` | | | |/ _` | '__/ _ \		|/|
;/|		| |/ / /\__/ / (_| | |_| | (_| | | |  __/		|/|
;/|		|___/  \____/ \__, |\__,_|\__,_|_|  \___|		|/|
;/|		                 | |                     		|/|
;/|		                 |_|                     		|/|
;/|	 	 _____                     _             _       	|/|
;/|		|_   _|                   | |           (_)      	|/|
;/|	  	  | | ___  ___ _ __   ___ | | ___   __ _ _  __ _ 	|/|
;/|	  	  | |/ _ \/ __| '_ \ / _ \| |/ _ \ / _` | |/ _` |	|/|
;/|		  | |  __/ (__| | | | (_) | | (_) | (_| | | (_| |	|/|
;/|		  \_/\___|\___|_| |_|\___/|_|\___/ \__, |_|\__,_|	|/|
;/|	          	                            __/ |        	|/|
;/|	       	                                   |___/         	|/|
;/|		 _     _ _                          			|/|
;/|		| |   (_) |                         			|/|
;/|		| |    _| |__  _ __ __ _ _ __ _   _ 			|/|
;/|		| |   | | '_ \| '__/ _` | '__| | | |			|/|
;/|		| |___| | |_) | | | (_| | |  | |_| |			|/|
;/|		\_____/_|_.__/|_|  \__,_|_|   \__, |			|/|
;/|	                               	       __/ |			|/|
;/|		                              |___/ 			|/|
;/|									|/|
;/|		______      _           				|/|
;/|		|  _  \    (_)          				|/|
;/|		| | | |_ __ ___   _____ 				|/|
;/|		| | | | '__| \ \ / / _ \				|/|
;/|		| |/ /| |  | |\ V /  __/				|/|
;/|		|___/ |_|  |_| \_/ \___|				|/|
;/|									|/|
;/////////////////////////////////////////////////////////////////////////|
;-------------------------------------------------------------------------/              
;</$DS_HEADER>
;/////////////////////////////////////////////////////////////////////////|
;/|									|/|
;/|		______  _____                            		|/|
;/|		|  _  \/  ___|                           		|/|
;/|		| | | |\ `--.  __ _ _   _  __ _ _ __ ___ 		|/|
;/|		| | | | `--. \/ _` | | | |/ _` | '__/ _ \		|/|
;/|		| |/ / /\__/ / (_| | |_| | (_| | | |  __/		|/|
;/|		|___/  \____/ \__, |\__,_|\__,_|_|  \___|		|/|
;/|		                 | |                     		|/|
;/|		                 |_|                     		|/|
;/|	 	 _____                     _             _       	|/|
;/|		|_   _|                   | |           (_)      	|/|
;/|	  	  | | ___  ___ _ __   ___ | | ___   __ _ _  __ _ 	|/|
;/|	  	  | |/ _ \/ __| '_ \ / _ \| |/ _ \ / _` | |/ _` |	|/|
;/|		  | |  __/ (__| | | | (_) | | (_) | (_| | | (_| |	|/|
;/|		  \_/\___|\___|_| |_|\___/|_|\___/ \__, |_|\__,_|	|/|
;/|	          	                            __/ |        	|/|
;/|	       	                                   |___/         	|/|
;/|		 _     _ _                          			|/|
;/|		| |   (_) |                         			|/|
;/|		| |    _| |__  _ __ __ _ _ __ _   _ 			|/|
;/|		| |   | | '_ \| '__/ _` | '__| | | |			|/|
;/|		| |___| | |_) | | | (_| | |  | |_| |			|/|
;/|		\_____/_|_.__/|_|  \__,_|_|   \__, |			|/|
;/|	                               	       __/ |			|/|
;/|		                              |___/ 			|/|
;/|									|/|
;/|		______      _           				|/|
;/|		|  _  \    (_)          				|/|
;/|		| | | |_ __ ___   _____ 				|/|
;/|		| | | | '__| \ \ / / _ \				|/|
;/|		| |/ /| |  | |\ V /  __/				|/|
;/|		|___/ |_|  |_| \_/ \___|				|/|
;/|									|/|
;/////////////////////////////////////////////////////////////////////////|
;-------------------------------------------------------------------------/              
;</$DS_HEADER>
;<$DS_HEADER>
;</$DS_HEADER>
;+--------------------------------------------------------------+---+---+
;|									|
;| DATE:17/12/2001 VERSION 1.0						|
;>----------------------------------------------------------------------<
;| I N T E G E R  R O U T I N E S	     				|
;| M U L T I P L Y 							|
;+----------------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM			|
;+----------------------------------------------------------------------+
;+----------------------------------------------------------------+
;| MUL_U8U8U16							  |
;| Multiply unsigned 8bits x unsigned 8bits result unsigned 16bits|
;|								  |
;| Input  : R0    1st operand					  |
;|	    R1    2nd operand					  |
;| Output : R2:R3 Result					  |
;| Destroy: R1,R4,Flags						  |
;| Time   : Min=63+3 rcall  Max=87+3 rcall			  | 
;+----------------------------------------------------------------+
#ifndef	__INTEGER_MUL_U8U8U16__
	#define __INTEGER_MUL_U8U8U16__
	.message "[ (MATH) INTEGER MUL U8U8U16 ]"
	_MUL_U8U8U16:
		push	Acc				;Save Acc
		clr	R2				;clear accumulator
		clr	R3
		clr	R4				;clear 2nd operand extend
		ldi	Acc,8				;bit counter
	_MUL_U8U8U16_00:
		ror	R0				;extract 1st operand bit
		brcc	_MUL_U8U8U16_10			;zero ?, branch
		add	R2,R1				;else accumulate
		adc	R3,R4
	_MUL_U8U8U16_10:
		lsl	R1				;next power of two of 2nd operand
		rol	R4
		dec	Acc				;until all bits done
		brne	_MUL_U8U8U16_00
		pop	Acc				;restore Acc
		ret
#endif
.exit


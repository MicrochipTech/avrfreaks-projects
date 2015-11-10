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
;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======> 		M O D U L E  -  S E T  I D I O M  T O  P O R T U G U E S E	 		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module set all message to portuguese						|
;	 -----------------------------------------------------------------------------------------------
	_MODULE_IDIOM_PORTUGUESE:
		call	_MODULE_SET_ONE_ITEM_MENU	;set one item
		popaw					;restore return address
		jmp	_MODULE_MENU			;restart MENU
;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======> 		M O D U L E  -  S E T  I D I O M  T O  E N G L I S H		 		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module set all message to english							|
;	 -----------------------------------------------------------------------------------------------
	_MODULE_IDIOM_ENGLISH:
		call	_MODULE_SET_ONE_ITEM_MENU	;set one item
		popaw					;restore return address
		jmp	_MODULE_MENU			;restart MENU
;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======> 		M O D U L E  -  S E T  I D I O M  T O  S P A N I S H 		 		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module set all message to spanish							|
;	 -----------------------------------------------------------------------------------------------
	_MODULE_IDIOM_SPANISH:
		call	_MODULE_SET_ONE_ITEM_MENU	;set one item
		popaw					;restore return address
		jmp	_MODULE_MENU			;restart MENU

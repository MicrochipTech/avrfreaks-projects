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
;=======> 	      M O D U L E  -  S E T  O N E  I T E M  M E N U			 		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description: set one item menu specified by X=MENU set ITEM by AccH SAVE and return		|											|
;	 -----------------------------------------------------------------------------------------------
	_MODULE_SET_ONE_ITEM_MENU:
		movw	ZL,XL						;Z=MENU
		push	AccH						;save item
		call	_MENU_GET_ITEM_LENGHT				;get item lenght
		mov	Temp,Acc					;temp=length
		inc	Temp						;to ease compare
		ldi	AccH,1						;set first item
	_MODULE_SET_ONE_ITEM_MENU_10:
		ldi	Acc,_OFF
		call	_MENU_SET_ITEM_CHECK				;set off
		inc	AccH						;next item
		cp	AccH,Temp
		brlo	_MODULE_SET_ONE_ITEM_MENU_10			;until all clear
		pop	AccH
		ldi	Acc,_ON
		call	_MENU_SET_ITEM_CHECK				;set this item to ON
		call	fn_SAVE_MENU
		ret

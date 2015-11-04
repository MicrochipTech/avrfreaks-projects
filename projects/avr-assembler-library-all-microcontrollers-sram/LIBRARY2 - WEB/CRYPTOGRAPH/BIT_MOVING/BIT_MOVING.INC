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
;|								|
;| BY JOÃO DARTAGNAN ANTUNES OLIVEIRA				|
;| DATA:21/05/2008						|
;>--------------------------------------------------------------<
;| B I T  M O V I N G  C R Y P T O G R A P H 			|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH MEGA AVRS WITH SRAM	|
;+--------------------------------------------------------------+
#ifndef	__CRYPTO_BIT_MOVING__
	
	#define	__CRYPTO_BIT_MOVING__
	.message "+---------------------+"
	.message "|                     |"
	.message "| CRYPTO BIT MOVING	|"
	.message "|                     |"
	.message "+---------------------+"
	;>--------------------------------------------------------------<
	;| CONSTANTS							|
	;>--------------------------------------------------------------<
	.EQU	_CRYPTO_CONSTANT	= 0X5A
	.EQU	_CRYPTO_KEYS_MASK	= 0X3F
	;>--------------------------------------------------------------<
	;| SRAM VARIABLES						|
	;>--------------------------------------------------------------<
	.DSEG
		_CRYPTO_ENCRYPT_KEY_INDEX:	.BYTE	1
		_CRYPTO_DECRYPT_KEY_INDEX:	.BYTE	1
	.CSEG
	;>--------------------------------------------------------------<
	;| OBJECT     : _CRYPTO_BIT_MOVING				|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Set encrypt start condition			|
	;>--------------------------------------------------------------<
	;| INPUT      : None						|
	;|		None						|
	;| OUTPUT     : None						|
	;| DESTROYED  :	---						|
	;| TIME	      :	---						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_CRYPTO_ENCRYPT_START:
	_CRYPTO_ENCRYPT_END:
		push	Acc
		ldi	Acc,0
		sts	_CRYPTO_ENCRYPT_KEY_INDEX,Acc
		pop	Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : _CRYPTO_BIT_MOVING				|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Set decrypt start condition			|
	;>--------------------------------------------------------------<
	;| INPUT      : None						|
	;|		None						|
	;| OUTPUT     : None						|
	;| DESTROYED  :	---						|
	;| TIME	      :	---						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_CRYPTO_DECRYPT_START:
	_CRYPTO_DECRYPT_END:
		push	Acc
		ldi	Acc,0
		sts	_CRYPTO_DECRYPT_KEY_INDEX,Acc
		pop	Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : _CRYPTO_BIT_MOVING				|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Set encrypt key					|
	;>--------------------------------------------------------------<
	;| INPUT      : Acc	new key					|
	;|		None						|
	;| OUTPUT     : None						|
	;| DESTROYED  :	---						|
	;| TIME	      :	---						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_CRYPTO_ENCRYPT_SET_KEY:
		sts	_CRYPTO_ENCRYPT_KEY_INDEX,Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : _CRYPTO_BIT_MOVING				|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Set decrypt key					|
	;>--------------------------------------------------------------<
	;| INPUT      : Acc	new key					|
	;|		None						|
	;| OUTPUT     : None						|
	;| DESTROYED  :	---						|
	;| TIME	      :	---						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_CRYPTO_DECRYPT_SET_KEY:
		sts	_CRYPTO_DECRYPT_KEY_INDEX,Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : _CRYPTO_BIT_MOVING				|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Encrypt a byte using bit moving by index moving	|
	;|		key						|
	;>--------------------------------------------------------------<
	;| INPUT      : Acc	byte to be encrypted			|
	;| OUTPUT     : Acc	encrypted byte				|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	53 clocks 3.3us at 16Mhz			|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_CRYPTO_BIT_MOVING_ENCRYPT:
		push	AccH						;save useds
		pushw	Z
		ldi	ZL,_CRYPTO_CONSTANT				;encrypt constant key 0x5a
		eor	Acc,ZL
		ldiw	Z,_CRYPTO_BIT_MOVING_KEYS_TABLE*2		;Z-->encrypt table
		lds	AccH,_CRYPTO_ENCRYPT_KEY_INDEX			;get encrypt key index
		andi	AccH,_CRYPTO_KEYS_MASK				;valid keys
		bst	AccH,0
		lsr	AccH						;get lower key
		add	ZL,AccH
		ldi	AccH,0
		adc	ZH,AccH
		lpm	AccH,Z						;get key
		brtc	PC+2
		swap	AccH						;else key high key
		andi	AccH,0x0f					;mask key
		ldiw	Z,_CRYPTO_BIT_MOVING_ENCRYPT_JUMP_TABLE		;Z-->jump table
		add	ZL,AccH
		ldi	AccH,0
		adc	ZH,AccH
		ijmp							;go to appropriate encrypting
	;>-------------------------------<
	;| encrypting jump table	 |
	;>-------------------------------<
	_CRYPTO_BIT_MOVING_ENCRYPT_JUMP_TABLE:
		rjmp	_CRYPT0_BIT_MOVING_ENCRYPT_0				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_ENCRYPT_1				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_ENCRYPT_2				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_ENCRYPT_3				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_ENCRYPT_4				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_ENCRYPT_5				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_ENCRYPT_6				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_ENCRYPT_7				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_ENCRYPT_8				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_ENCRYPT_9				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving encrypting # 0 76543210 |
	;|                       10762453 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_ENCRYPT_0:
		bst	Acc,0
		bld	AccH,6
		bst	Acc,1
		bld	AccH,7
		bst	Acc,2
		bld	AccH,3
		bst	Acc,3
		bld	AccH,0
		bst	Acc,4
		bld	AccH,2
		bst	Acc,5
		bld	AccH,1
		bst	Acc,6
		bld	AccH,4
		bst	Acc,7
		bld	AccH,5
		rjmp	_CRYPTO_BIT_MOVING_ENCRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving encrypting # 1 76543210 |
	;|                       53104672 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_ENCRYPT_1:
		bst	Acc,0
		bld	AccH,4
		bst	Acc,1
		bld	AccH,5
		bst	Acc,2
		bld	AccH,0
		bst	Acc,3
		bld	AccH,6
		bst	Acc,4
		bld	AccH,3
		bst	Acc,5
		bld	AccH,7
		bst	Acc,6
		bld	AccH,2
		bst	Acc,7
		bld	AccH,1
		rjmp	_CRYPTO_BIT_MOVING_ENCRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving encrypting # 2 76543210 |
	;|                       72536014 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_ENCRYPT_2:
		bst	Acc,0
		bld	AccH,2
		bst	Acc,1
		bld	AccH,1
		bst	Acc,2
		bld	AccH,6
		bst	Acc,3
		bld	AccH,4
		bst	Acc,4
		bld	AccH,0
		bst	Acc,5
		bld	AccH,5
		bst	Acc,6
		bld	AccH,3
		bst	Acc,7
		bld	AccH,7
		rjmp	_CRYPTO_BIT_MOVING_ENCRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving encrypting # 3 76543210 |
	;|                       14720356 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_ENCRYPT_3:
		bst	Acc,0
		bld	AccH,3
		bst	Acc,1
		bld	AccH,7
		bst	Acc,2
		bld	AccH,4
		bst	Acc,3
		bld	AccH,2
		bst	Acc,4
		bld	AccH,6
		bst	Acc,5
		bld	AccH,1
		bst	Acc,6
		bld	AccH,0
		bst	Acc,7
		bld	AccH,5
		rjmp	_CRYPTO_BIT_MOVING_ENCRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving encrypting # 4 76543210 |
	;|                       56143270 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_ENCRYPT_4:
		bst	Acc,0
		bld	AccH,0
		bst	Acc,1
		bld	AccH,5
		bst	Acc,2
		bld	AccH,2
		bst	Acc,3
		bld	AccH,3
		bst	Acc,4
		bld	AccH,4
		bst	Acc,5
		bld	AccH,7
		bst	Acc,6
		bld	AccH,6
		bst	Acc,7
		bld	AccH,1
		rjmp	_CRYPTO_BIT_MOVING_ENCRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving encrypting # 5 76543210 |
	;|                       70562413 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_ENCRYPT_5:
		bst	Acc,0
		bld	AccH,6
		bst	Acc,1
		bld	AccH,1
		bst	Acc,2
		bld	AccH,3
		bst	Acc,3
		bld	AccH,0
		bst	Acc,4
		bld	AccH,2
		bst	Acc,5
		bld	AccH,5
		bst	Acc,6
		bld	AccH,4
		bst	Acc,7
		bld	AccH,7
		rjmp	_CRYPTO_BIT_MOVING_ENCRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving encrypting # 6 76543210 |
	;|                       13704652 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_ENCRYPT_6:
		bst	Acc,0
		bld	AccH,4
		bst	Acc,1
		bld	AccH,7
		bst	Acc,2
		bld	AccH,0
		bst	Acc,3
		bld	AccH,6
		bst	Acc,4
		bld	AccH,3
		bst	Acc,5
		bld	AccH,1
		bst	Acc,6
		bld	AccH,2
		bst	Acc,7
		bld	AccH,5
		rjmp	_CRYPTO_BIT_MOVING_ENCRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving encrypting # 7 76543210 |
	;|                       52136074 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_ENCRYPT_7:
		bst	Acc,0
		bld	AccH,2
		bst	Acc,1
		bld	AccH,5
		bst	Acc,2
		bld	AccH,6
		bst	Acc,3
		bld	AccH,4
		bst	Acc,4
		bld	AccH,0
		bst	Acc,5
		bld	AccH,7
		bst	Acc,6
		bld	AccH,3
		bst	Acc,7
		bld	AccH,1
		rjmp	_CRYPTO_BIT_MOVING_ENCRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving encrypting # 8 76543210 |
	;|                       74520316 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_ENCRYPT_8:
		bst	Acc,0
		bld	AccH,3
		bst	Acc,1
		bld	AccH,1
		bst	Acc,2
		bld	AccH,4
		bst	Acc,3
		bld	AccH,2
		bst	Acc,4
		bld	AccH,6
		bst	Acc,5
		bld	AccH,5
		bst	Acc,6
		bld	AccH,0
		bst	Acc,7
		bld	AccH,7
		rjmp	_CRYPTO_BIT_MOVING_ENCRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving encrypting # 9 76543210 |
	;|                       16743250 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_ENCRYPT_9:
		bst	Acc,0
		bld	AccH,0
		bst	Acc,1
		bld	AccH,7
		bst	Acc,2
		bld	AccH,2
		bst	Acc,3
		bld	AccH,3
		bst	Acc,4
		bld	AccH,4
		bst	Acc,5
		bld	AccH,1
		bst	Acc,6
		bld	AccH,6
		bst	Acc,7
		bld	AccH,5
		rjmp	_CRYPTO_BIT_MOVING_ENCRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------------------------------------<
	;| OBJECT     : _CRYPTO_BIT_MOVING				|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Decrypt a byte using bit moving by index moving	|
	;|		key						|
	;>--------------------------------------------------------------<
	;| INPUT      : Acc	byte encrypted				|
	;| OUTPUT     : Acc	byte decrypted				|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	53 clocks 3.3us at 16Mhz			|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_CRYPTO_BIT_MOVING_DECRYPT:
		push	AccH						;save useds
		pushw	Z
		ldiw	Z,_CRYPTO_BIT_MOVING_KEYS_TABLE*2		;Z-->encrypt table
		lds	AccH,_CRYPTO_DECRYPT_KEY_INDEX			;get decrypt key index
		andi	AccH,_CRYPTO_KEYS_MASK				;only keys valid keys
		bst	AccH,0
		lsr	AccH						;get lower key
		add	ZL,AccH
		ldi	AccH,0
		adc	ZH,AccH
		lpm	AccH,Z						;get key
		brtc	PC+2
		swap	AccH						;else key high key
		andi	AccH,0x0f					;mask key
		ldiw	Z,_CRYPTO_BIT_MOVING_DECRYPT_JUMP_TABLE		;Z-->jump table
		add	ZL,AccH
		ldi	AccH,0
		adc	ZH,AccH
		ijmp							;go to appropriate encrypting
	;>-------------------------------<
	;| encrypting jump table	 |
	;>-------------------------------<
	_CRYPTO_BIT_MOVING_DECRYPT_JUMP_TABLE:
		rjmp	_CRYPT0_BIT_MOVING_DECRYPT_0				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_DECRYPT_1				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_DECRYPT_2				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_DECRYPT_3				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_DECRYPT_4				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_DECRYPT_5				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_DECRYPT_6				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_DECRYPT_7				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_DECRYPT_8				;*<NOCHANGE>*
		rjmp	_CRYPT0_BIT_MOVING_DECRYPT_9				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving decrypting # 0 76543210 |
	;|                       10762453 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_DECRYPT_0:
		bst	Acc,6
		bld	AccH,0
		bst	Acc,7
		bld	AccH,1
		bst	Acc,3
		bld	AccH,2
		bst	Acc,0
		bld	AccH,3
		bst	Acc,2
		bld	AccH,4
		bst	Acc,1
		bld	AccH,5
		bst	Acc,4
		bld	AccH,6
		bst	Acc,5
		bld	AccH,7
		rjmp	_CRYPTO_BIT_MOVING_DECRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving decrypting # 1 76543210 |
	;|                       53104672 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_DECRYPT_1:
		bst	Acc,4
		bld	AccH,0
		bst	Acc,5
		bld	AccH,1
		bst	Acc,0
		bld	AccH,2
		bst	Acc,6
		bld	AccH,3
		bst	Acc,3
		bld	AccH,4
		bst	Acc,7
		bld	AccH,5
		bst	Acc,2
		bld	AccH,6
		bst	Acc,1
		bld	AccH,7
		rjmp	_CRYPTO_BIT_MOVING_DECRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving decrypting # 2 76543210 |
	;|                       72536014 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_DECRYPT_2:
		bst	Acc,2
		bld	AccH,0
		bst	Acc,1
		bld	AccH,1
		bst	Acc,6
		bld	AccH,2
		bst	Acc,4
		bld	AccH,3
		bst	Acc,0
		bld	AccH,4
		bst	Acc,5
		bld	AccH,5
		bst	Acc,3
		bld	AccH,6
		bst	Acc,7
		bld	AccH,7
		rjmp	_CRYPTO_BIT_MOVING_DECRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving decrypting # 3 76543210 |
	;|                       14720356 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_DECRYPT_3:
		bst	Acc,3
		bld	AccH,0
		bst	Acc,7
		bld	AccH,1
		bst	Acc,4
		bld	AccH,2
		bst	Acc,2
		bld	AccH,3
		bst	Acc,6
		bld	AccH,4
		bst	Acc,1
		bld	AccH,5
		bst	Acc,0
		bld	AccH,6
		bst	Acc,5
		bld	AccH,7
		rjmp	_CRYPTO_BIT_MOVING_DECRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving decrypting # 4 76543210 |
	;|                       56143270 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_DECRYPT_4:
		bst	Acc,0
		bld	AccH,0
		bst	Acc,5
		bld	AccH,1
		bst	Acc,2
		bld	AccH,2
		bst	Acc,3
		bld	AccH,3
		bst	Acc,4
		bld	AccH,4
		bst	Acc,7
		bld	AccH,5
		bst	Acc,6
		bld	AccH,6
		bst	Acc,1
		bld	AccH,7
		rjmp	_CRYPTO_BIT_MOVING_DECRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving decrypting # 5 76543210 |
	;|                       70562413 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_DECRYPT_5:
		bst	Acc,6
		bld	AccH,0
		bst	Acc,1
		bld	AccH,1
		bst	Acc,3
		bld	AccH,2
		bst	Acc,0
		bld	AccH,3
		bst	Acc,2
		bld	AccH,4
		bst	Acc,5
		bld	AccH,5
		bst	Acc,4
		bld	AccH,6
		bst	Acc,7
		bld	AccH,7
		rjmp	_CRYPTO_BIT_MOVING_DECRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving decrypting # 6 76543210 |
	;|                       13704652 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_DECRYPT_6:
		bst	Acc,4
		bld	AccH,0
		bst	Acc,7
		bld	AccH,1
		bst	Acc,0
		bld	AccH,2
		bst	Acc,6
		bld	AccH,3
		bst	Acc,3
		bld	AccH,4
		bst	Acc,1
		bld	AccH,5
		bst	Acc,2
		bld	AccH,6
		bst	Acc,5
		bld	AccH,7
		rjmp	_CRYPTO_BIT_MOVING_DECRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving decrypting # 7 76543210 |
	;|                       52136074 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_DECRYPT_7:
		bst	Acc,2
		bld	AccH,0
		bst	Acc,5
		bld	AccH,1
		bst	Acc,6
		bld	AccH,2
		bst	Acc,4
		bld	AccH,3
		bst	Acc,0
		bld	AccH,4
		bst	Acc,7
		bld	AccH,5
		bst	Acc,3
		bld	AccH,6
		bst	Acc,1
		bld	AccH,7
		rjmp	_CRYPTO_BIT_MOVING_DECRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving decrypting # 8 76543210 |
	;|                       74520316 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_DECRYPT_8:
		bst	Acc,3
		bld	AccH,0
		bst	Acc,1
		bld	AccH,1
		bst	Acc,4
		bld	AccH,2
		bst	Acc,2
		bld	AccH,3
		bst	Acc,6
		bld	AccH,4
		bst	Acc,5
		bld	AccH,5
		bst	Acc,0
		bld	AccH,6
		bst	Acc,7
		bld	AccH,7
		rjmp	_CRYPTO_BIT_MOVING_DECRYPT_EXIT				;*<NOCHANGE>*
	;>--------------------------------<
	;| moving decrypting # 9 76543210 |
	;|                       16743250 |
	;>--------------------------------<
	_CRYPT0_BIT_MOVING_DECRYPT_9:
		bst	Acc,0
		bld	AccH,0
		bst	Acc,7
		bld	AccH,1
		bst	Acc,2
		bld	AccH,2
		bst	Acc,3
		bld	AccH,3
		bst	Acc,4
		bld	AccH,4
		bst	Acc,1
		bld	AccH,5
		bst	Acc,6
		bld	AccH,6
		bst	Acc,5
		bld	AccH,7
		rjmp	_CRYPTO_BIT_MOVING_DECRYPT_EXIT				;*<NOCHANGE>*
	 ;>------<
	 ;| EXIT |
	 ;>------<
	_CRYPTO_BIT_MOVING_ENCRYPT_EXIT:
		mov	Acc,AccH					;get encrypted value
		lds	AccH,_CRYPTO_ENCRYPT_KEY_INDEX			;pump encrypt key index
		inc	AccH
		andi	AccH,_CRYPTO_KEYS_MASK
		sts	_CRYPTO_ENCRYPT_KEY_INDEX,AccH
		popw	Z
		pop	AccH
		ret
	_CRYPTO_BIT_MOVING_DECRYPT_EXIT:
		mov	Acc,AccH					;get decrypted value
		ldi	AccH,_CRYPTO_CONSTANT				;restore constant
		eor	Acc,AccH
		lds	AccH,_CRYPTO_DECRYPT_KEY_INDEX			;pump decrypt key index
		inc	AccH
		andi	AccH,_CRYPTO_KEYS_MASK
		sts	_CRYPTO_DECRYPT_KEY_INDEX,AccH
		popw	Z
		pop	AccH
		ret
	;>---------------------------<
	;| Long encrypt keys table   |
	;>---------------------------<
	_CRYPTO_BIT_MOVING_KEYS_TABLE:
		.DB	0X27,0X18,0X28,0X18,0X28,0X45,0X90,0X42
		.DB	0X31,0X41,0X59,0X26,0X53,0X58,0X97,0X97
		.DB	0X16,0X18,0X03,0X39,0X88,0X74,0X98,0X91		
		.DB	0X14,0X14,0X21,0X35,0X62,0X37,0X30,0X98
#endif
.exit


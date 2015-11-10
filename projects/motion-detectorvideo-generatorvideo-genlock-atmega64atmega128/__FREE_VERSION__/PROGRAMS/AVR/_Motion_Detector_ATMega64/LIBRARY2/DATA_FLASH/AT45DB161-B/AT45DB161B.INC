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
;| DATA:19/05/2008						|
;>--------------------------------------------------------------<
;| A T 4 5 D B 1 6 1 B  D A T A  F L A S H  R O U T I N E S	|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM		|
;+--------------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N S 		|
;+------------------------------------------------------+---------------------------------------+
;| _AT45DB161B_INIT	INITIALIZE DEVICE USING BELOW PARAMETERS				|
;|												|
;| .EQU	_AT45DB161B_PORT_OUTPUT	= PORTx								|
;| .EQU	_AT45DB161B_PORT_DIR	= DDRx								|
;| .EQU	_AT45DB161B_PORT_INPUT	= PINx								|
;|												|
;| .EQU	_AT45DB161B_SO_BIT	= BITx								|
;| .EQU	_AT45DB161B_SI_BIT	= BITx								|
;| .EQU	_AT45DB161B_SCK_BIT	= BITx								|
;| .EQU	_AT45DB161B_CS_BIT	= BITx								|
;| .EQU	_AT45DB161B_WP_BIT	= BITx								|
;| .EQU	_AT45DB161B_RESET_BIT	= BITx								|
;|												|
;|			rcall	_AT45DB161B_INIT	;after above definition call this func-	|
;|							;tion to initialize device		|
;|												|
;+----------------------------------------------------------------------------------------------+
;+----------------------------------------------------------------------------------------------+
#ifndef	__AT45DB161B__
	
	#define	__AT45DB161B__
	#ifndef __TIME_DELAYS__ 
		.include "TIME\DELAYS\DELAYS.inc"	;need libraries
	#endif
	.message "+-----------------------------+"
	.message "|                             |"
	.message "| DATA FLASH AT45DB161B	|"
	.message "|                             |"
	.message "+-----------------------------+"
	;+----------------------+
	;| AT45DB161B COMMANDS	|
	;+----------------------+
	;READ COMMANDS
	.EQU	_AT45DB161B_SPI3_CMD_CONTINUOUS_ARRAY_READ		= 0XE8
	.EQU	_AT45DB161B_SPI3_CMD_MAIN_MEMORY_PAGE_READ		= 0XD2
	.EQU	_AT45DB161B_SPI3_CMD_BUFFER1_READ			= 0XD4
	.EQU	_AT45DB161B_SPI3_CMD_BUFFER2_READ			= 0XD6
	.EQU	_AT45DB161B_SPI3_CMD_STATUS_REGISTER_READ		= 0XD7
	;PROGRAM AND ERASE COMMANDS
	.EQU	_AT45DB161B_CMD_BUFFER1_WRITE				= 0X84
	.EQU	_AT45DB161B_CMD_BUFFER2_WRITE				= 0X87
	.EQU	_AT45DB161B_CMD_BUFFER1_PROGRAM_WITH_ERASE		= 0X83
	.EQU	_AT45DB161B_CMD_BUFFER2_PROGRAM_WITH_ERASE		= 0X84
	.EQU	_AT45DB161B_CMD_BUFFER1_PROGRAM_WITHOUT_ERASE		= 0X88
	.EQU	_AT45DB161B_CMD_BUFFER2_PROGRAM_WITHOUT_ERASE		= 0X89
	.EQU	_AT45DB161B_CMD_PAGE_ERASE				= 0X81
	.EQU	_AT45DB161B_CMD_BLOCK_ERASE				= 0X50
	.EQU	_AT45DB161B_CMD_PAGE_PROGRAM_THROUGH_BUFFER1		= 0X82
	.EQU	_AT45DB161B_CMD_PAGE_PROGRAM_THROUGH_BUFFER2		= 0X85
	;ADDITIONAL COMMANDS
	.EQU	_AT45DB161B_CMD_MAIN_MEMORY_PAGE_TO_BUFFER1_TRANSFER	= 0X53
	.EQU	_AT45DB161B_CMD_MAIN_MEMORY_PAGE_TO_BUFFER2_TRANSFER	= 0X55
	.EQU	_AT45DB161B_CMD_MAIN_MEMORY_PAGE_TO_BUFFER1_COMPARE	= 0X60
	.EQU	_AT45DB161B_CMD_MAIN_MEMORY_PAGE_TO_BUFFER2_COMPARE	= 0X61
	.EQU	_AT45DB161B_CMD_AUTO_PAGE_REWRITE_THROUGH_BUFFER1	= 0X58
	.EQU	_AT45DB161B_CMD_AUTO_PAGE_REWRITE_THROUGH_BUFFER2	= 0X59
	;+----------------------+
	;| AT45DB161B BITS	|
	;+----------------------+
	.EQU	_AT45DB161B_RDY_BIT	= 7
	.EQU	_AT45DB161B_COMP_BIT	= 6
	;>--------------------------------------------------------------<
	;| OBJECT     : _AT45DB161B					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Initialize					|
	;>--------------------------------------------------------------<
	;| INPUT      : None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	Interrupts are disable during initialization	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_AT45DB161B_INIT:
		push	Acc						;save useds
		in	Acc,SREG					;save SREG
		cli							;disable interrupts
		sbi	_AT45DB161B_PORT_OUTPUT,_AT45DB161B_RESET_BIT	;RESET=high
		sbi	_AT45DB161B_PORT_OUTPUT,_AT45DB161B_CS_BIT	;CS=high
		sbi	_AT45DB161B_PORT_OUTPUT,_AT45DB161B_WP_BIT	;WP=low(WRITE PROTECTED)
		sbi	_AT45DB161B_PORT_OUTPUT,_AT45DB161B_SCK_BIT	;SCK=high
		sbi	_AT45DB161B_PORT_OUTPUT,_AT45DB161B_SO_BIT	;SO=high input with pull up
		sbi	_AT45DB161B_PORT_OUTPUT,_AT45DB161B_SI_BIT	;SI=high
		sbi	_AT45DB161B_PORT_DIR,_AT45DB161B_RESET_BIT	;RESET=output
		sbi	_AT45DB161B_PORT_DIR,_AT45DB161B_CS_BIT		;CS=output
		sbi	_AT45DB161B_PORT_DIR,_AT45DB161B_WP_BIT		;WP=low(WRITE PROTECTED) output
		sbi	_AT45DB161B_PORT_DIR,_AT45DB161B_SCK_BIT	;SCK=output
		cbi	_AT45DB161B_PORT_DIR,_AT45DB161B_SO_BIT		;SO=input with pull up
		sbi	_AT45DB161B_PORT_DIR,_AT45DB161B_SI_BIT		;SI=output
		rcall	_AT45DB161B_RESET				;reset device
		out	SREG,Acc					;restore SREG
		pop	Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : _AT45DB161B					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Reset device					|
	;>--------------------------------------------------------------<
	;| INPUT      : None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	Interrupts are disable during initialization	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|			
	_AT45DB161B_RESET:
		push	Acc						;save useds
		in	Acc,SREG					;save SREG
		cli							;disable interrupts
		sbi	_AT45DB161B_PORT_OUTPUT,_AT45DB161B_CS_BIT	;CS=HIGH
		cbi	_AT45DB161B_PORT_OUTPUT,_AT45DB161B_RESET_BIT	;RESET=LOW
		pushaw
		_M_DELAY_MICROSECONDS 	20				;pulse low per 20us
		popaw
		sbi	_AT45DB161B_PORT_OUTPUT,_AT45DB161B_RESET_BIT	;RESET=HIGH
		_M_DELAY_MILLISECONDS	20				;wait 20 milliseconds
		out	SREG,Acc					;restore SREG state
		pop	Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : _AT45DB161B					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : send Data or Command to Memory			|
	;>--------------------------------------------------------------<
	;| INPUT      : Acc Data to be send				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	80 clock 5us at 16mhz				|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	Interrupts are disable during send of command	|
	;|		SPI Mode 3 is asserted				|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;| Waveform - Inactive Clock Polarity High and SPI Mode 3
	;|
	;|	CS -----+								+---------------+
	;|		|								|		|
	;|		+---------------------------------------------------------------+		+----------
	;|		:								:
	;|		<-tcss->:<-twl->:<-twh->:       <-------------tcsh-------------->
	;|	SCK ------------+	+-------+	+----------------------------------------------------------
	;|			|	|	|	|				:	
	;|	 		+-------+	+-------+				:
	;|			<tv>	:	:<tho>					<tdis>
	;|		high Z	+--+-----------------+---------------------------------------+ high Z
	;|	SO  ------------|  |	:	     |					     |---------------------
	;|			+--+-----------------+---------------------------------------+
	;|			<-tsu-->:<-th->
	;|	    ------------+-------------+------+------+------------------------------------------------------
	;|	SI  ############| Valid In    |######|	    |######################################################
	;|	    ------------+-------------+------+------+------------------------------------------------------
	;|
	;|	tcss	- CS setup time 	min=250ns
	;|	twl	- SCK low time		min= 22ns
	;|	twh	- SCK high time		min= 22ns
	;|	tcsh	- CS hold time		min=250ns
	;|	tv	- output valid		max= 20ns
	;|	tho	- output hold time	min=  0ns
	;|	tdis	- output disable time	max= 20ns
	;|	tsu	- data in setup time	min= 10ns
	;|	th	- data in hold time	min= 15ns
	;|
	.MACRO	_M_AT45DB161B_BIT_OUT
		andi	AccT,~(1<<_AT45DB161B_SCK_BIT)			;SCK=LOW
		out	_AT45DB161B_PORT_OUTPUT,AccT			;set SCK state
		andi	AccT,~(1<<_AT45DB161B_SI_BIT)			;DATA=LOW
		sbrc	Acc,@0						;skip if zero
		ori	AccT,(1<<_AT45DB161B_SI_BIT)			;else DATA=HIGH
		out	_AT45DB161B_PORT_OUTPUT,AccT			;set DATA state
		ori	AccT,(1<<_AT45DB161B_SCK_BIT)			;SCK=HIGH
		out	_AT45DB161B_PORT_OUTPUT,AccT
	.ENDMACRO
	_AT45DB161B_DATA_OUT:
		push	AccH						;save SREG
		push	AccT
		in	AccH,SREG
		cli							;disable interrupts
		in	AccT,_AT45DB161B_PORT_OUTPUT			;get actual state
		_M_AT45DB161B_BIT_OUT 7					;out data bit 7
		_M_AT45DB161B_BIT_OUT 6					;out data bit 6
		_M_AT45DB161B_BIT_OUT 5					;out data bit 5
		_M_AT45DB161B_BIT_OUT 4					;out data bit 4
		_M_AT45DB161B_BIT_OUT 3					;out data bit 3
		_M_AT45DB161B_BIT_OUT 2					;out data bit 2
		_M_AT45DB161B_BIT_OUT 1					;out data bit 1
		_M_AT45DB161B_BIT_OUT 0					;out data bit 0
		out	SREG,AccH					;restore SREG
		pop	AccT
		pop	AccH
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : _AT45DB161B					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : get Data from data flash			|
	;>--------------------------------------------------------------<
	;| INPUT      : None						|
	;| OUTPUT     :	Acc Data from data flash			|
	;| DESTROYED  :	None						|
	;| TIME	      :	44 clocks 2.75us at 16Mhz			|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	Interrupts are disable during geting data from	|
	;|		data flash.					|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	.MACRO	_M_AT45DB161B_BIT_IN
		cbi	_AT45DB161B_PORT_OUTPUT,_AT45DB161B_SCK_BIT	;SCK=LOW
		sbi	_AT45DB161B_PORT_OUTPUT,_AT45DB161B_SCK_BIT	;SCK=HIGH
		sbic	_AT45DB161B_PORT_INPUT,_AT45DB161B_SO_BIT	;skip if LOW
		ori	Acc,(1<<@0)					;else BIT N=HIGH
	.ENDMACRO
	_AT45DB161B_DATA_IN:
		push	AccT
		in	AccT,SREG					;save SREG
		cli
		ldi	Acc,0						;assume all data bits=0
		_M_AT45DB161B_BIT_IN	7				;get bit 7
		_M_AT45DB161B_BIT_IN	6				;get bit 6
		_M_AT45DB161B_BIT_IN	5				;get bit 5
		_M_AT45DB161B_BIT_IN	4				;get bit 4
		_M_AT45DB161B_BIT_IN	3				;get bit 3
		_M_AT45DB161B_BIT_IN	2				;get bit 2
		_M_AT45DB161B_BIT_IN	1				;get bit 1
		_M_AT45DB161B_BIT_IN	0				;get bit 0
		out	SREG,AccT					;restore Acct
		pop	AccT
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : _AT45DB161B					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : finish data transfer				|
	;>--------------------------------------------------------------<
	;| INPUT      : None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_AT45DB161B_DATA_END:
		sbi	_AT45DB161B_PORT_OUTPUT,_AT45DB161B_CS_BIT	;CS=HIGH
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : _AT45DB161B					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : start data transfer				|
	;>--------------------------------------------------------------<
	;| INPUT      : None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_AT45DB161B_DATA_START:
		cbi	_AT45DB161B_PORT_OUTPUT,_AT45DB161B_CS_BIT	;CS=LOW
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : _AT45DB161B					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Set Write protect state				|
	;>--------------------------------------------------------------<
	;| INPUT      : Acc=_ON protect _OFF=release			|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_AT45DB161B_SET_WRITE_PROTECT:
		cpi	Acc,_ON						;turn protect on ?
		brne	PC+3
		cbi	_AT45DB161B_PORT_OUTPUT,_AT45DB161B_WP_BIT	;set protected
		ret
		sbi	_AT45DB161B_PORT_OUTPUT,_AT45DB161B_WP_BIT	;set unprotected
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : _AT45DB161B					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Get status register				|
	;>--------------------------------------------------------------<
	;| INPUT      : None						|
	;| OUTPUT     :	Acc	Status					|
	;|		bit7 = RDY/~BUSY state 1=READY 0=BUSY		|
	;|		bit6 = COMP 0=buffer math memory		|
	;|		bit5 = 1					|
	;|		bit4 = 0					|
	;|		bit3 = 1					|
	;|		bit2 = 1					|
	;|		bit1 = x					|
	;|		bit0 = x					|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_AT45DB161B_GET_STATUS_REGISTER:
		rcall	_AT45DB161B_DATA_START				;start data transfer
		ldi	Acc,_AT45DB161B_SPI3_CMD_STATUS_REGISTER_READ	;status register read CMD
		rcall	_AT45DB161B_DATA_OUT				;send
		rcall	_AT45DB161B_DATA_IN				;get status
		rcall	_AT45DB161B_DATA_END				;finish data transfer
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : _AT45DB161B					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Send address to data flash			|
	;>--------------------------------------------------------------<
	;| INPUT      : X	Buffer Address				|
	;|		Y	Page Address				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|			
	_AT45DB161B_SET_ADDRESS:
		push	AccH
		mov	Acc,YH						;get page high address
		lsl	Acc						;adjust to right position
		lsl	Acc
		andi	Acc,0b00111100					;clear high bits
		mov	AccH,YL						;get page low address
		rol	AccH						;bit 7 to carry
		rol	AccH						;carry to bit0
		rol	AccH						;bit1,bit0=bit7,bit6
		andi	AccH,0b00000011					;preserve 7,6 bits
		or 	Acc,AccH					;bound high page with low page
		rcall	_AT45DB161B_DATA_OUT				;send high byte address
		mov	Acc,YL						;get page low address again
		lsl	Acc						;shift to right position
		lsl	Acc
		mov	AccH,XH						;get buffer high address
		andi	AccH,0b00000011					;preserve bits 7,6
		or	Acc,AccH					;bound low page with high buffer
		rcall	_AT45DB161B_DATA_OUT				;send middle byte address
		mov	Acc,XL						;get buffer low
		rcall	_AT45DB161B_DATA_OUT				;send low byte adress
		pop	Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : _AT45DB161B					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Send continuous array read command		|
	;>--------------------------------------------------------------<
	;| INPUT      : X	Start buffer address			|
	;|		Y	Start Page Address			|
	;| OUTPUT     : None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_AT45DB161B_CONTINUOUS_ARRAY_READ:
		push	Acc
		rcall	_AT45DB161B_DATA_START				;start data transfer
		ldi	Acc,_AT45DB161B_SPI3_CMD_CONTINUOUS_ARRAY_READ	;continuous array read CMD
		rcall	_AT45DB161B_DATA_OUT				;send command
		rcall	_AT45DB161B_SET_ADDRESS				;send address
		ldi	Acc,0						;4 none bytes
		rcall	_AT45DB161B_DATA_OUT
		rcall	_AT45DB161B_DATA_OUT
		rcall	_AT45DB161B_DATA_OUT
		rcall	_AT45DB161B_DATA_OUT
		pop	Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : _AT45DB161B					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Buffer1 write					|
	;>--------------------------------------------------------------<
	;| INPUT      : X	Start buffer address			|
	;| OUTPUT     : None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_AT45DB161B_BUFFER1_WRITE:
		push	Acc
		rcall	_AT45DB161B_DATA_START				;start data transfer
		ldi	Acc,_AT45DB161B_CMD_BUFFER1_WRITE		;buffer1 write
		rcall	_AT45DB161B_DATA_OUT				;send command
		rcall	_AT45DB161B_SET_ADDRESS				;send address
		pop	Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : _AT45DB161B					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Buffer1 read					|
	;>--------------------------------------------------------------<
	;| INPUT      : X	Start buffer address			|
	;| OUTPUT     : None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_AT45DB161B_BUFFER1_READ:
		push	Acc
		rcall	_AT45DB161B_DATA_START				;start data transfer
		ldi	Acc,_AT45DB161B_SPI3_CMD_BUFFER1_READ		;read write
		rcall	_AT45DB161B_DATA_OUT				;send command
		rcall	_AT45DB161B_SET_ADDRESS				;send address
		ldi	Acc,0
		rcall	_AT45DB161B_DATA_OUT				;dummy data
		pop	Acc
		ret
		
	;>--------------------------------------------------------------<
	;| OBJECT     : _AT45DB161B					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : program Buffer1 into Flash Page			|
	;>--------------------------------------------------------------<
	;| INPUT      : Y	Start Page Address			|
	;| OUTPUT     : None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_AT45DB161B_BUFFER1_WRITE_INTO_PAGE:
		push	Acc
		rcall	_AT45DB161B_DATA_START				;start data transfer
		ldi	Acc,_AT45DB161B_CMD_BUFFER1_PROGRAM_WITH_ERASE	;buffer1 program command
		rcall	_AT45DB161B_DATA_OUT				;send command
		rcall	_AT45DB161B_SET_ADDRESS				;send address
		call	_AT45DB161B_DATA_END				;finish
		pop	Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : _AT45DB161B					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : read Buffer1 from Flash Page			|
	;>--------------------------------------------------------------<
	;| INPUT      : Y	Start Page Address			|
	;| OUTPUT     : None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_AT45DB161B_BUFFER1_READ_FROM_PAGE:
		push	Acc
		rcall	_AT45DB161B_DATA_START				;start data transfer
		ldi	Acc,_AT45DB161B_CMD_MAIN_MEMORY_PAGE_TO_BUFFER1_TRANSFER	;buffer1 read command
		rcall	_AT45DB161B_DATA_OUT				;send command
		rcall	_AT45DB161B_SET_ADDRESS				;send address
		call	_AT45DB161B_DATA_END				;finish
		pop	Acc
		ret
#endif


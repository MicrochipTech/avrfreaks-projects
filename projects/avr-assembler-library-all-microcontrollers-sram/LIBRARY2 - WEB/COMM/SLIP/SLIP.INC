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
;+------------------------------------------------------+
;| BY JOÃO DARTAGNAN ANTUNES OLIVEIRA			|
;| DATA:26/11/2005					|
;>------------------------------------------------------<
;| A V R  S L I P  C O M M U N I C A T I O N  D R I V E |
;| V E R S I O N  1.0					|
;+------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM	|
;+------------------------------------------------------+-----------------------+
;|  A NONSTANDARD FOR TRANSMISSION OF IP DATAGRAMS OVER SERIAL LINES: SLIP	|
;| 										|
;|INTRODUCTION									|
;| 										|
;|   The TCP/IP protocol family runs over a variety of network media:		|
;|   IEEE 802.3 (ethernet) and 802.5 (token ring) LAN's, X.25 lines,		|
;|   satellite links, and serial lines.  There are standard encapsulations	|
;|   for IP packets defined for many of these networks, but there is no		|
;|   standard for serial lines.  SLIP, Serial Line IP, is a currently a de	|
;|   facto standard, commonly used for point-to-point serial connections	|
;|   running TCP/IP.  It is not an Internet standard.  Distribution of		|
;|   this memo is unlimited.							|
;| 										|
;|HISTORY									|
;| 										|
;|   SLIP has its origins in the 3COM UNET TCP/IP implementation from the	|
;|   early 1980's.  It is merely a packet framing protocol: SLIP defines a	|
;|   sequence of characters that frame IP packets on a serial line, and		|
;|   nothing more. It provides no addressing, packet type identification,	|
;|   error detection/correction or compression mechanisms.  Because the		|
;|   protocol does so little, though, it is usually very easy to		|
;|   implement.									|
;| 										|
;|   Around 1984, Rick Adams implemented SLIP for 4.2 Berkeley Unix and		|
;|   Sun Microsystems workstations and released it to the world.  It		|
;|   quickly caught on as an easy reliable way to connect TCP/IP hosts and	|
;|   routers with serial lines.							|
;| 										|
;|   SLIP is commonly used on dedicated serial links and sometimes for		|
;|   dialup purposes, and is usually used with line speeds between 1200bps	|
;|   and 19.2Kbps.  It is useful for allowing mixes of hosts and routers	|
;|   to communicate with one another (host-host, host-router and router-	|
;|   router are all common SLIP network configurations).			|
;| 										|
;|AVAILABILITY									|
;| 										|
;|   SLIP is available for most Berkeley UNIX-based systems.  It is		|
;|   included in the standard 4.3BSD release from Berkeley.  SLIP is		|
;|   available for Ultrix, Sun UNIX and most other Berkeley-derived UNIX	|
;|   systems.  Some terminal concentrators and IBM PC implementations also	|
;|   support it.								|
;| 										|
;|   SLIP for Berkeley UNIX is available via anonymous FTP from			|
;|   uunet.uu.net in pub/sl.shar.Z.  Be sure to transfer the file in		|
;|   binary mode and then run it through the UNIX uncompress program. Take	|
;|   the resulting file and use it as a shell script for the UNIX /bin/sh	|
;|   (for instance, /bin/sh sl.shar).						|
;| 										|
;|PROTOCOL									|
;| 										|
;|   The SLIP protocol defines two special characters: END and ESC. END is	|
;|   octal 300 (decimal 192) and ESC is octal 333 (decimal 219) not to be	|
;|   confused with the ASCII ESCape character; for the purposes of this		|
;|   discussion, ESC will indicate the SLIP ESC character.  To send a		|
;|   packet, a SLIP host simply starts sending the data in the packet.  If	|
;|   a data byte is the same code as END character, a two byte sequence of	|
;|   ESC and octal 334 (decimal 220) is sent instead.  If it the same as	|
;|   an ESC character, an two byte sequence of ESC and octal 335 (decimal	|
;|   221) is sent instead.  When the last byte in the packet has been		|
;|   sent, an END character is then transmitted.				|
;| 										|
;|   Phil Karn suggests a simple change to the algorithm, which is to		|
;|   begin as well as end packets with an END character.  This will flush	|
;|   any erroneous bytes which have been caused by line noise.  In the		|
;|   normal case, the receiver will simply see two back-to-back END		|
;|   characters, which will generate a bad IP packet.  If the SLIP		|
;|   plementation does not throw away the zero-length IP packet, the IP		|
;|   implementation certainly will.  If there was line noise, the data		|
;|   received due to it will be discarded without affecting the following	|
;|   packet.									|
;| 										|
;|   Because there is no 'standard' SLIP specification, there is no real	|
;|   defined maximum packet size for SLIP.  It is probably best to accept	|
;|   the maximum packet size used by the Berkeley UNIX SLIP drivers: 1006	|
;|   bytes including the IP and transport protocol headers (not including	|
;|   the framing characters).  Therefore any new SLIP implementations		|
;|   should be prepared to accept 1006 byte datagrams and should not send	|
;|   more than 1006 bytes in a datagram.					|
;| 										|
;|DEFICIENCIES									|
;| 										|
;|   There are several features that many users would like SLIP to provide	|
;|   which it doesn't.  In all fairness, SLIP is just a very simple		|
;|   protocol designed quite a long time ago when these problems were not	|
;|   really important issues.  The following are commonly perceived		|
;|   shortcomings in the existing SLIP protocol:				|
;| 										|
;|      - addressing:								|
;| 										|
;|         both computers in a SLIP link need to know each other's IP		|
;|         addresses for routing purposes.  Also, when using SLIP for		|
;|         hosts to dial-up a router, the addressing scheme may be quite	|
;|         dynamic and the router may need to inform the dialing host of	|
;|         the host's IP address.  SLIP currently provides no mechanism		|
;|         for hosts to communicate addressing information over a SLIP		|
;|         connection.								|
;| 										|
;|      - type identification:							|
;| 										|
;|         SLIP has no type field.  Thus, only one protocol can be run		|
;|         over a SLIP connection, so in a configuration of two DEC		|
;|         computers running both TCP/IP and DECnet, there is no hope of	|
;|         having TCP/IP and DECnet share one serial line between them		|
;|         while using SLIP.  While SLIP is "Serial Line IP", if a serial	|
;|         line connects two multi-protocol computers, those computers		|
;|         should be able to use more than one protocol over the line.		|
;| 										|
;|      - error detection/correction:						|
;| 										|
;|         noisy phone lines will corrupt packets in transit. Because the	|
;|         line speed is probably quite low (likely 2400 baud),			|
;|         retransmitting a packet is very expensive.  Error detection is	|
;|         not absolutely necessary at the SLIP level because any IP		|
;|         application should detect damaged packets (IP header and UDP		|
;|         and TCP checksums should suffice), although some common		|
;|         applications like NFS usually ignore the checksum and depend on	|
;|         the network media to detect damaged packets.  Because it takes	|
;|         so long to retransmit a packet which was corrupted by line		|
;|         noise, it would be efficient if SLIP could provide some sort of	|
;|         simple error correction mechanism of its own.			|
;| 										|
;|      - compression:								|
;| 										|
;|         because dial-in lines are so slow (usually 2400bps), packet		|
;|         compression would cause large improvements in packet			|
;|         throughput. Usually, streams of packets in a single TCP		|
;|         connection have few changed fields in the IP and TCP headers,	|
;|         so a simple compression algorithms might just send the changed	|
;|         parts of the headers instead of the complete headers.		|
;| 										|
;|   Some work is being done by various groups to design and implement a	|
;|   successor to SLIP which will address some or all of these problems.	|
;| 										|
;+------------------------------------------------------------------------------+---------------+
;| SLIP EXAMPLE											|
;|												|
;| 	Fortunately, one of the TCP/IP families of standards,SLIP, provides exactly this	|
;|	functionality. It uses simple escape codes inserted in the serial data stream to	|
;|	signal block boundaries as follows							|	
;|												|
;| - The end of each block is signaled by a special End byte, with a falue of 0xC0.		|
;| - If a data byte equal 0XC0, two bytes with the values 0xDB,0XDC are sent instead.		|
;| - if a data byte equal 0xDBH,two bytes with the values 0xDB,0xDD are sent instead.		|
;|												|
;| 	Additionally, most implementation send the End byte at the beginning of each block to	|
;| clear out garbage characters prior to starting the new message.				|
;|												|
;| SLIP FRAME											|
;|												|
;| +-----+------------------------------------------------------+-----+				|
;| | END |			DATA				| END |				|
;| | 0XC0|                  1-1006 BYTES			| 0XC0|				|
;| +-----+------------------------------------------------------+-----+				|
;|												|
;| 	There is effectively no limit to the size of the data block, but you have to decide	|
;|	on some value in order to dimension the data bufers. With old, slow serial links, 	|
;|	a maximum size of 256 bytres was generally used, but you'll be using faster links,	|
;|	and a larger size is better for minimizeng protocol overhead. By convention, 1006 	|
;|	bytes is oten used.									|
;|												|
;| 	The encoding method can best be illustrated by an example. Assume a six-byte block	|
;|	of data with the hex values BF C0 C1 DB DC is sent; it is expanded to 			|
;|	C0 BF DB DC C1 DB DD DC C0.								|
;|												|
;| SLIP TRANSMISSION SAMPLE									|
;|												|
;| 			+----+----+----+----+----+						|
;| block data flow 	|0XBF|0XC0|0XC1|0XDB|0XDC|						|
;| 			+----+----+----+----+----+						|
;|			  |     |    |    |    |						|
;|			  |	|    |	  |    |						|
;|			  |	|    |	  |    +--------------+					|
;|			  |	|    |	  +-----------+	      |					|
;|			  |	|    +--------+       |	      |					|
;| 			  |	+------+      |	      |	      |					|
;|			  |	       |      |	      |	      |					|
;|			  +-----+      |      |	      |	      |					|
;|  				|   +--+--+   |	   +--+--+    |					|
;|				V   V     V   V    V     V    V					|
;| 			+----+----+----+----+----+----+----+----+----+				|
;| SLIP coded flow 	|0XC0|0XBF|0XDB|0XDC|0XC1|0XDB|0XDD|0XDC|0XCO|				|
;| 			+----+----+----+----+----+----+----+----+----+				|
;|			 END					 END				|
;+----------------------------------------------------------------------------------------------+
;| SLIP OBSERVATION										|
;|												|
;|  												|
;| 	When connect SLIP device in serial port under WINDOWS, it send a ATE1<cr> modem command	|
;| and device replay OK<cr><lf>, after this the OS send DSVP packed to inform device about OS	|
;| resorces, device replay OK<cr><lf> again, after this OS send a SLIP protocol with TCP/IP	|
;|												|
;| Connections											|
;|												|
;| MODEM for SLIP protocol 									|
;|												|
;|	+-------+			+-------+						|
;|	| MODEM	|			|  PC   |						|
;|	|	RTS<----------------->CTS	|						|
;|	|	CTS<----------------->RTS	|						|
;|	|	DCD<----------------->DCD	|						|
;|	|	TXD<----------------->RXD	|						|
;|	|	RXD<----------------->TXD	|						|
;|	|	|			|	|						|
;|	+-------+			+-------+						|
;| 												|
;| Generic serial DEVICE for SLIP protocol 							|
;|												|
;|	+-------+			+-------+						|
;|	| MODEM	|			|  PC   |						|
;|	|	|		  +-->CTS	|						|
;|	|	|		  +-->RTS	|						|
;|	|	|		  +-->DCD	|						|
;|	|	TXD<----------------->RXD	|						|
;|	|	RXD<----------------->TXD	|						|
;|	|	|			|	|						|
;|	+-------+			+-------+						|
;|												|
;| CTS,RTS,DCD from PC side are connected together						|
;+----------------------------------------------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N S 		|
;+------------------------------------------------------+---------------------------------------+
;| 			SLIP INITIALIZATION SEQUENCE						|
;|												|
;|			ldiaw	RX_BUFFER_SIZE		;rx buffer size				|
;|			ldiw	Z,RX_BUFFER_PTR		;rx pointer				|
;|			RCALL	_SLIP_SET_RX_BUFFER	;set					|
;|			ldiaw	TX_BUFFER_SIZE		;tx buffer size				|
;|			ldiw	Z,RX_BUFFER_PTR		;tx pointer				|
;|		*	RCALL	_SLIP_SET_TX_BUFFER	;set					|
;|			ldiw	Z,RX_FUNC_ADDR		;set function rx address		|
;|			RCALL	_SLIP_SET_RX_ADDR						|
;|			ldiw	Z,TX_FUNC_ADDR		;set function tx address		|
;|			RCALL	_SLIP_SET_TX_ADDR						|
;|			ldiw	Z,TIMEOUT_FUNC_ADDR	;set timeout function address		|
;|			ldiaw	100			;set timeout to 100ms			|
;|			RCALL	_SLIP_SET_TIMEOUT_ADDR						|
;|												|
;|	SLIP_SET_TX_BUFFER not necessary to be setted during initialization, only must be setted|
;|	before _SLIP_SEND function								|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_SET_RX_BUFFER	SET SLIP RECEIVE DATA BUFFER FOR ADDRESS AND SIZE			|
;|												|
;|			ldiaw	SLIP_SIZE	;set SLIP data buffer size			|
;|			ldiw	Z,SLIP_BUFFER	;set SLIP data buffer pointer			|
;|			RCALL	_SLIP_SET_RX_BUFFER;SLIP initialize			 	|
;+----------------------------------------------------------------------------------------------+
;| _SLIP_GET_RX_BUFFER	GET SLIP RECEIVE DATA BUFFER ADDRESS AND SIZE				|
;|												|
;|			RCALL	_SLIP_GET_RX_BUFFER:get buffer size and pointer after this call	|
;|						;Z-->DATA BUFFER;AccH:Acc DATA BUFFER SIZE	|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_SET_TX_BUFFER	SET SLIP TRANSMITTER DATA BUFFER FOR ADDRESS AND SIZE			|
;|												|
;|			ldiaw	SLIP_SIZE	;set SLIP data buffer size			|
;|			ldiw	Z,SLIP_BUFFER	;set SLIP data buffer pointer			|
;|			RCALL	_SLIP_SET_TX_BUFFER;SLIP initialize			 	|
;+----------------------------------------------------------------------------------------------+
;| _SLIP_GET_TX_BUFFER	GET SLIP TRANSMITTER DATA BUFFER ADDRESS AND SIZE			|
;|												|
;|			RCALL	_SLIP_GET_TX_BUFFER:get buffer size and pointer after this call	|
;|						;Z-->DATA BUFFER;AccH:Acc DATA BUFFER SIZE	|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_SET_RX_ADDR	SET SLIP RX FUNCTION ADDDRESS						|
;|												|
;|			ldiw	Z,RX_FUNC_ADDR		;Z-->address of RX function that return	|
;|							;rx data into Acc register		|
;|			RCALL	_SLIP_SET_RX_ADDR	;					|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_SET_TX_ADDR	SET SLIP TX FUNCTION ADDDRESS						|
;|												|
;|			ldiw	Z,TX_FUNC_ADDR		;Z-->address of TX function with data	|
;|							;to send must be into Acc register	|
;|			RCALL	_SLIP_SET_TX_ADDR	;					|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_SET_TIMEOUT_ADDR	SET SLIP TIMEOUT FUNCTION ADDDRESS				|
;|												|
;|			ldiw	Z,TIMEOUT_FUNC_ADDR	;Z-->address of timeout function with 	|
;|							;timeout into AccH:Acc			|
;|			ldiaw	100			;settime out to 100ms			|
;|			RCALL	_SLIP_SET_TIMOUT_ADDR	;					|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_SET_INDEX	SET SLIP DATA INDEX							|
;|												|
;|			LDIAW	50			;SET INDEX TO 50nd data			|
;|			RCALL	_SLIP_SET_INDEX		;SET POINTER				|
;|							;cy=1 if request pointer>SLIP buf size	|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_GET_INDEX	GET SLIP DATA INDEX							|
;|												|
;|			RCALL	_SLIP_GET_DATA_PTR	;AccH:Acc INDEX	VALUE			|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_GET_DATA	GET DATA FROM SLIP BUFFER INDEXED BY LAST DATA_PTR SETTING		|
;|												|
;|			RCALL	_SLIP_GET_DATA		;Acc DATA				|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_NEXT_DATA	POINT TO NEXT DATA INTO SLIP BUFFER					|
;|												|
;|			RCALL	_SLIP_NEXT_DATA		;POINT TO NEXT DATA			|
;|							;cy=1 if no more data			|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_POLLING	POLLING RX LINE FOR SLIP PACKETS					|
;|												|
;|			RCALL	_SLIP_POLLING		;Acc=_OK if received SLIP block ok	|
;|							;AccTH:AccT number of bytes received	|
;|							;Acc=_NOTOK if received SLIP block fail	|
;|							:AccTH:AccT number of bytes before fail	|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_SEND		SEND DATA AT TX_BUFFER							|
;|												|
;|			RCALL	_SLIP_SEND		;send data into tx_buffer		|
;+----------------------------------------------------------------------------------------------+
#ifndef __SLIP_DRIVE__
	#define __SLIP_DRIVE__
	.message "+------------+"
	.message "|            |"
	.message "| SLIP DRIVE |"
	.message "|            |"
	.message "+------------+"
;				  ///////////////////////\\\\\\\\\\\\\\\\\\\\\\\\
;				 //						\\
;-------------------------------||	          S L I P  D R I V E   	 	||-----------------------------------
;				 \\						//
;				  \\\\\\\\\\\\\\\\\\\\\\\///////////////////////
	;>--------------------------------------------------------------<
	;| OBJECT     : SLIP						|
	;>--------------------------------------------------------------<
	;| DESCRIPTION:This object perform all resorces for implementa- |
	;|	       tion of SLIP protocol				|
	;>--------------------------------------------------------------<
	;>----------------------<
	;| OBJECT DEFINITIONS	|
	;>----------------------<
	;-------------------------< CONSTANT DEFINITONS >------------------------
	.EQU	_SLIP_CODE_END		= 0XC0		;SLIP start/end block code
	.EQU	_SLIP_CODE_ESC		= 0XDB		;SLIP prefix for encode byte 0XC0,0XDB
	.EQU	_SLIP_CODE_ESC_END	= 0XDC		;SLIP code for 0XDC
	.EQU	_SLIP_CODE_ESC_ESC	= 0XDD		;SLIP code for 0XDB
	.EQU	_SLIP_MSG_OK  		= 1		;message when SLIP block received ok
	.EQU	_SLIP_MSG_POL_END	= 2		;message when SLIP packet not yet received
	.EQU	_SLIP_MSG_TIMEOUT	= 3		;message when SLIP timeout occur
	.EQU	_SLIP_MSG_UNEXPECTED	= 4		;message when SLIP data expected ESC_END,but received other
	;-------------------------< VARIABLES DEFINITONS >------------------------
	.DSEG
		_SLIP_RX_ADDRESS:	.BYTE 2			;SLIP address for receiving function
		_SLIP_TX_ADDRESS:	.BYTE 2			;SLIP address for transmitting function
		_SLIP_RX_BUF_PTR:	.BYTE 2			;SLIP rx buffer data pointer
		_SLIP_RX_BUF_SIZE:	.BYTE 2			;SLIP rx buffer size
		_SLIP_RX_BUF_INDEX:	.BYTE 2			;SLIP rx data index
		_SLIP_RX_BYTES:		.BYTE 2			;SLIP received bytes
		_SLIP_TX_BUF_PTR:	.BYTE 2			;SLIP tx buffer data pointer
		_SLIP_TX_BUF_SIZE:	.BYTE 2			;SLIP tx buffer size
		
	.CSEG
	;>--------------------------------------------------------------<
	;| OBJECT     : SLIP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : SLIP_SET_TIMEOUT_ADDR				|
	;>--------------------------------------------------------------<
	;| PERFORM    : Setting address of rx function			|
	;>--------------------------------------------------------------<
	;| INPUT      :	AccH:Acc timeout value in mileseconds		|
	;|		Z-->address of rx function			|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	*						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;|		* Destroyed register and flags according setting|
	;|		  rx function					|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_SLIP_SET_TIMEOUT_ADDR:
		icall					;call set timeout routine
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : SLIP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : SLIP_SET_RX_BUFFER				|
	;>--------------------------------------------------------------<
	;| PERFORM    : Setting rx buffer and size			|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z-->rx buffer pointer				|
	;|		AccH:Acc buffer size				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION: ----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_SLIP_SET_RX_BUFFER:
		sts	_SLIP_RX_BUF_PTR+0,ZL		;store rx buffer pointer
		sts	_SLIP_RX_BUF_PTR+1,ZH
		sts	_SLIP_RX_BUF_SIZE+0,Acc		;store rx buffer size
		sts	_SLIP_RX_BUF_SIZE+1,AccH
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : SLIP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : SLIP_GET_RX_BUFFER				|
	;>--------------------------------------------------------------<
	;| PERFORM    : Getting rx buffer and size			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Z-->rx buffer pointer				|
	;|		AccH:Acc buffer size				|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION: ----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_SLIP_GET_RX_BUFFER:
		lds	ZL,_SLIP_RX_BUF_PTR+0			;get rx buffer pointer
		lds	ZH,_SLIP_RX_BUF_PTR+1
		lds	Acc,_SLIP_RX_BUF_SIZE+0			;get rx buffer size
		lds	AccH,_SLIP_RX_BUF_SIZE+1
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : SLIP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : SLIP_SET_TX_BUFFER				|
	;>--------------------------------------------------------------<
	;| PERFORM    : Setting tx buffer and size			|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z-->tx pointer					|
	;|		AccH:Acc buffer size				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION: ----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_SLIP_SET_TX_BUFFER:
		sts	_SLIP_TX_BUF_PTR+0,ZL		;store tx buffer pointer
		sts	_SLIP_TX_BUF_PTR+1,ZH
		sts	_SLIP_TX_BUF_SIZE+0,Acc		;store tx buffer size
		sts	_SLIP_TX_BUF_SIZE+1,AccH
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : SLIP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : SLIP_GET_TX_BUFFER				|
	;>--------------------------------------------------------------<
	;| PERFORM    : Getting rx buffer and size			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Z-->tx buffer pointer				|
	;|		AccH:Acc buffer size				|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION: ----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_SLIP_GET_TX_BUFFER:
		lds	ZL,_SLIP_TX_BUF_PTR+0			;get tx buffer pointer
		lds	ZH,_SLIP_TX_BUF_PTR+1
		lds	Acc,_SLIP_TX_BUF_SIZE+0			;get tx buffer size
		lds	AccH,_SLIP_TX_BUF_SIZE+1
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : SLIP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : SLIP_SET_RX_ADDR				|
	;>--------------------------------------------------------------<
	;| PERFORM    : Setting rx function address			|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z-->rx function address				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_SLIP_SET_RX_ADDR:
		sts	_SLIP_RX_ADDRESS+0,ZL		;store rx address
		sts	_SLIP_RX_ADDRESS+1,ZH
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : SLIP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : SLIP_GET_DATA_COUNT				|
	;>--------------------------------------------------------------<
	;| PERFORM    : Get received data value				|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	AccH:Acc received data				|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_SLIP_GET_DATA_COUNT:
		lds	Acc,_SLIP_RX_BYTES+0			;get received bytes
		lds	AccH,_SLIP_RX_BYTES+1
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : SLIP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : SLIP_SET_TX_ADDR				|
	;>--------------------------------------------------------------<
	;| PERFORM    : Setting tx function address			|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z-->tx function address				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_SLIP_SET_TX_ADDR:
		sts	_SLIP_TX_ADDRESS+0,ZL		;store rx address
		sts	_SLIP_TX_ADDRESS+1,ZH
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : SLIP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : SLIP_RX						|
	;>--------------------------------------------------------------<
	;| PERFORM    : Receiver a byte from defined receiver routine	|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Acc  character received				|
	;|		cy=1 if timeout occur				|
	;| DESTROYED  :	*						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;| 		calling RX function must be return character	|
	;|		into Acc register				|
	;|		this function pressed only Z register		|
	;|		* destroyed register according address of tx	|
	;|		function 					|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_SLIP_RX:
		pushw	Z				;save used register
		lds	ZL,_SLIP_RX_ADDRESS+0		;get address
		lds	ZH,_SLIP_RX_ADDRESS+1
		#ifndef __DEBUG_MODE__
			icall				;call rx routine
		#endif
		popw	Z
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : SLIP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : SLIP_TX						|
	;>--------------------------------------------------------------<
	;| PERFORM    : Transmit a byte to transmitting routine		|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc character to be transmited			|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	*						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;| 		calling TX function must be require character	|
	;|		to be send into Acc register			|
	;|		this function pressed only Z register		|
	;|		* destroyed register according address of tx	|
	;|		function 					|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_SLIP_TX:
		pushw	Z				;save used register
		lds	ZL,_SLIP_TX_ADDRESS+0		;get address
		lds	ZH,_SLIP_TX_ADDRESS+1
		#ifndef __DEBUG_MODE__
			icall				;call rx routine
		#endif
		popw	Z
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : SLIP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : SLIP_SET_INDEX					|
	;>--------------------------------------------------------------<
	;| PERFORM    : Set SLIP index of received byte			|
	;>--------------------------------------------------------------<
	;| INPUT      :	AccH:Acc data index ranging 			|
	;|			 from 0.._SLIP_BUF_SIZE			|
	;| OUTPUT     :	cy=1 if index>_SLIP_BUF_SIZE			|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	if index>=_REC_BYTES	 			|
	;|			then index=_REC_BYTES			|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_SLIP_SET_INDEX:
		push	AccT
		lds	AccT,_SLIP_RX_BYTES+0				;see if request>REC_BYTES
		cp	Acc,AccT
		lds	AccT,_SLIP_RX_BYTES+1
		cpc	AccH,AccT
		breq	_SLIP_SET_DATA_M00				;if no bytes
		brlo	_SLIP_SET_DATA_00				;branch if lower
		lds	Acc,_SLIP_RX_BYTES+0				;else set max size
		lds	AccH,_SLIP_RX_BYTES+1
		sbiw	Acc,1
	_SLIP_SET_DATA_M00:
		sec							;set end
		rjmp	_SLIP_SET_DATA_10
	_SLIP_SET_DATA_00:
		clc
	_SLIP_SET_DATA_10:
		in	AccT,SREG					;for correct setting
		cli
		sts	_SLIP_RX_BUF_INDEX+0,Acc			;set index
		sts	_SLIP_RX_BUF_INDEX+1,AccH
		out	SREG,AccT
		pop	AccT
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : SLIP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : SLIP_GET_INDEX					|
	;>--------------------------------------------------------------<
	;| PERFORM    : Get SLIP index of received byte			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	AccH:Acc data index value			|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_SLIP_GET_INDEX:
		push	Temp				;save SREG 
		in	Temp,SREG
		cli					;disable interrupts
		lds	Acc,_SLIP_RX_BUF_INDEX+0		;set index
		lds	AccH,_SLIP_RX_BUF_INDEX+1
		out	SREG,Temp
		pop	Temp
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : SLIP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : SLIP_GET_DATA					|
	;>--------------------------------------------------------------<
	;| PERFORM    : Get byte from SLIP buffer			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Acc data byte					|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_SLIP_GET_DATA:
		pushw	Z				;save useds
		push	AccH
		lds	ZL,_SLIP_RX_BUF_PTR+0		;Z-->SLIP RX BUFFER
		lds	ZH,_SLIP_RX_BUF_PTR+1
		rcall	_SLIP_GET_INDEX			;get data pointer
		add	ZL,Acc				;Acc=SLIP_DATA(Z+index)
		adc	ZH,AccH
		ld	Acc,Z
		pop	AccH
		popw	Z				;restore useds
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : SLIP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : SLIP_NEXT_DATA					|
	;>--------------------------------------------------------------<
	;| PERFORM    : Point SLIP data index to next data		|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	CY=1 if END of SLIP BUFFER			|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_SLIP_NEXT_DATA:
		push	Acc				;save useds
		push	AccH
		rcall	_SLIP_GET_INDEX			;get actual index value
		adiw	Acc,1				;point to next data
		rcall	_SLIP_SET_INDEX			;set new index value
		pop	AccH
		pop	Acc
		ret
	;>----------------------------------------------------------------------<
	;| OBJECT     : SLIP							|
	;>----------------------------------------------------------------------<
	;| FUNCTION   : SLIP_POLLING						|
	;>----------------------------------------------------------------------<
	;| PERFORM    : Polling a serial line and waiting CODE_END		|
	;>----------------------------------------------------------------------<
	;| INPUT      :	None							|
	;| OUTPUT     :	Acc=SLIP_MSG_OK     ,cy=0 if SLIP block received ok,	|
	;|		Acc=SLIP_MSG_POL_END,cy=1 SLIP packet not received 	|
	;|		Acc=SLIP_MSG_TIMEOUT,cy=1 if timeout occur		|
	;|		Acc=SLIP_MSG_UNEXPECTED,expected ESC_END,but received 	|
	;|					other code			|		
	;|		AccTH:AccT total received bytes if _OK			|
	;|		AccTH:AccT total received bytes until Error if _NOTOK 	|
	;| DESTROYED  :	None							|
	;| TIME	      :	----							|
	;>----------------------------------------------------------------------<
	;| OBSERVATION:	----							|
	;+----------------------------------------------------------------------+-----------------------------------------------------------------------\
	;|																		|
	_SLIP_POLLING:
		pushw	Z				;save useds
		push	Temp
		push	AccH
		ldiaw	0				;clear data index
		rcall	_SLIP_SET_INDEX
		sts	_SLIP_RX_BYTES+0,Acc		;clear bytes count
		sts	_SLIP_RX_BYTES+1,AccH
	_SLIP_POL_00:
		rcall	_SLIP_RX			;wait byte
		brcc	_SLIP_POL_20			;if byte received ok then branch
		;+-------------------------
		;| RECEPTON NOTOK
		;+-------------------------
	_SLIP_POL_NOTOK:	
		ldi	Acc,_SLIP_MSG_POL_END		;else return with cy=1
		sec
	_SLIP_POL_10:
		sts	_SLIP_RX_BYTES+0,AccT		;store received bytes count
		sts	_SLIP_RX_BYTES+1,AccTH
		pop	AccH
		pop	Temp				;restore useds
		popw	Z
		ret
		;+-------------------------
		;| RECEPTON OK
		;+-------------------------
	_SLIP_POL_OK:
		ldi	Acc,_SLIP_MSG_OK		;set cy=0
		clc
		rjmp	_SLIP_POL_10				
	_SLIP_POL_TIMEOUT:
		ldi	Acc,_SLIP_MSG_TIMEOUT		;set cy=1
		sec
		rjmp	_SLIP_POL_10				
	_SLIP_POL_UNEXPECTED:
		ldi	Acc,_SLIP_MSG_UNEXPECTED	;set cy=1
		sec
		rjmp	_SLIP_POL_10				
	_SLIP_POL_20:
		cpi	Acc,_SLIP_CODE_END		;start block code received ?
		brne	_SLIP_POL_UNEXPECTED		;no, then unexpected byte received 
		;+-------------------------
		;| START BLOCK RECEIVING
		;+-------------------------
		clr	Temp				;clear SLIP ESCAP key
		ldiawt	0				;clear SLIP byte count
		lds	ZL,_SLIP_RX_BUF_PTR+0		;Z-->SLIP RX BUFFER
		lds	ZH,_SLIP_RX_BUF_PTR+1
	_SLIP_POL_30:
		push	Temp				;save SLIP key,and count
		push	AccT				
		push	AccTH
		pushw	Z
		rcall	_SLIP_RX			;get rest of block bytes
		popw	Z
		pop	AccTH				;restore SLIP key and count
		pop	AccT
		pop	Temp
		brcc	_SLIP_POL_40			;if received ok branch
		rjmp	_SLIP_POL_TIMEOUT		;else set timeout error
	_SLIP_POL_40:
		cpi	Acc,_SLIP_CODE_END		;SLIP block end ?
		brne	_SLIP_POL_50
		rjmp	_SLIP_POL_OK			;else SLIP reception ok
	_SLIP_POL_50:
		cpi	Temp,_SLIP_CODE_ESC		;code ESC setted ?
		breq	_SLIP_POL_70			;yes, branch
		cpi	Acc,_SLIP_CODE_ESC		;code ESC ?
		brne	_SLIP_POL_60			;no, branch
		ldi	Temp,_SLIP_CODE_ESC		;else set key=CODE ESC
		rjmp	_SLIP_POL_30			;get next byte
	_SLIP_POL_60:
		subi	AccT,-1				;byte count++	
		sbci	AccTH,-1
		st	Z+,Acc				;else store received byte
		rjmp	_SLIP_POL_30			;get next byte
	_SLIP_POL_70:
		cpi	Acc,_SLIP_CODE_ESC_END		;code ESC END ?
		brne	_SLIP_POL_80			;no, branch
		clr	Temp				;else clear key
		ldi	Acc,_SLIP_CODE_END		;store code END
		rjmp	_SLIP_POL_60
	_SLIP_POL_80:
		clr	Temp				;clear key
		ldi	Acc,_SLIP_CODE_ESC		;store code ESC
		rjmp	_SLIP_POL_60
	;>----------------------------------------------------------------------<
	;| OBJECT     : SLIP							|
	;>----------------------------------------------------------------------<
	;| FUNCTION   : SLIP_SEND						|
	;>----------------------------------------------------------------------<
	;| PERFORM    : Sent data using SLIP format on serial line 		|
	;>----------------------------------------------------------------------<
	;| INPUT      :	None							|
	;| OUTPUT     :	None							|
	;| DESTROYED  :	None							|
	;| TIME	      :	----							|
	;>----------------------------------------------------------------------<
	;| OBSERVATION:								|
	;|		call _SLIP_SET_TX_BUFFER before				|
	;+----------------------------------------------------------------------+-----------------------------------------------------------------------\
	;|																		|
	_SLIP_SEND:
		push	Acc				;save useds
		push	Temp
		pushw	Z
		pushw	X
		rcall	_SLIP_GET_TX_BUFFER
		mov	XL,Acc				;X=data size
		mov	XH,AccH
		ldi	Acc,_SLIP_CODE_END		;start SLIP block code
		pushw	X				;only for warranty
		pushw	Z
		rcall	_SLIP_TX			;send
		popw	Z
		popw	X
	_SLIP_SEND_00:
		cpiw	X,0				;last data transmitted ?
		breq	_SLIP_SEND_30			;yes, branch
		ld	Acc,Z+				;get data
		cpi	Acc,_SLIP_CODE_END		;data=code end ?
		brne	_SLIP_SEND_10			;no,branch
		pushw	X
		pushw	Z
		ldi	Acc,_SLIP_CODE_ESC		;else send code esc+code esc end
		rcall	_SLIP_TX			;send
		ldi	Acc,_SLIP_CODE_ESC_END
		rcall	_SLIP_TX
		popw	Z
		popw	X
		rjmp	_SLIP_SEND_25			;next data
	_SLIP_SEND_10:
		cpi	Acc,_SLIP_CODE_ESC		;data=code esc ?
		brne	_SLIP_SEND_20			;no, branch
		pushw	Z
		pushw	X
		ldi	Acc,_SLIP_CODE_ESC		;else send code esc+code esc esc
		rcall	_SLIP_TX			;send
		ldi	Acc,_SLIP_CODE_ESC_ESC
		rcall	_SLIP_TX
		popw	X
		popw	Z
		rjmp	_SLIP_SEND_25			;next data
	_SLIP_SEND_20:
		pushw	Z
		pushw	X
		rcall	_SLIP_TX			;send data
		popw	X
		popw	Z
	_SLIP_SEND_25:
		sbiw	XL,1				;size--
		rjmp	_SLIP_SEND_00			;next data
	_SLIP_SEND_30:
		ldi	Acc,_SLIP_CODE_END		;end SLIP block code
		rcall	_SLIP_TX			;send
		popw	X				;restore useds
		popw	Z
		pop	Temp
		pop	Acc
		ret
#endif
.exit


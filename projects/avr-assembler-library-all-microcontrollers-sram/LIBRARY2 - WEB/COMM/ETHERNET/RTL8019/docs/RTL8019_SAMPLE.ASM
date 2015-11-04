;***************************************************************************
;
; File Name		:"RTL8019.asm"
; Title			:RTL8019AS Ethernet Controller driver
; Date			:2002.11.22.
; Version		:1.0.0
; Support telephone	:+36-70-333-4034,  old: +36-30-9541-658 VFX
; Support fax		:
; Support Email		:info@vfx.hu
; Target MCU		:AT90S8515
;
;***************************************************************************
;	D E S C R I P T I O N
;
; EGZ-03002-v1 Ethernet Driver
;
; Support Devices
;  RTL8019AS 		Ethernet Controller Card
;
;  Provides functions to initialize the Realtek 8019AS, and send and retreive
;  packets
;
;Ethernet Frame Header
;
;  +--------------------------------------+
;  | Destination |   Source    |   Frame  |
;  |   Address   |   Address   |   Type   |
;  |  (6 octets) |  (6 octets) |(2 octets)|
;  +--------------------------------------+
;


.EQU	NICBASE	= 0x4300h
; IRQ = 0
; BOOTROM = none

;******************************************************************
;*	REALTEK CONTROL REGISTER OFFSETS
;*   All offsets in Page 0 unless otherwise specified
;*	  All functions accessing CR must leave CR in page 0 upon exit
;******************************************************************
; RTL8019AS Register Def

.EQU	NICCR	= 0x00		;Command Register R/W
				;This register is used to select register pages,
				;enable or disable remote DMA operation and
				;issue commands.

.EQU	NICISR	= 0x07		;Interrupt Status Register, R/W in Page0
				;This register reflects the NIC status. The host
				;reads it to determine the cause of an interrupt
				;Individual bits are cleared by writing a "1"
				;into the corresponding bit.
				;It must be cleared after power up.

.EQU	NICIMR	= 0x0F		;Interrupt Mask Register, W in Page0, Type=R in Page2
				;All bits correspond to the bits in the ISR register
				;POWER UP=all 0s. Setting individual bits will
				;enable the corresponding interrupts.

.EQU	NICDCR	= 0x0E		;Data Configuration Register, Type=W in Page0, Type=R in Page2

.EQU	NICTCR	= 0x0D		;Transmit Configuration Register, Type=W in Page0, Type=R in Page2

.EQU	NICTSR	= 0x04		;Transmit Status Register, Type=R in Page0
				;This register indicates the status of a packet transmission
.EQU	NICRCR	= 0x0C		;Receive Configuration Register, Type=W in Page0, Type=R in Page2

.EQU	NICRSR	= 0x0C		;Receive Status Register, Type=R in Page0

.EQU	NICCLDA0 = 0x01
.EQU	NICCLDA1 = 0x02		;Current Local DMA Registers Type=R in Page0
				;These two registers can be read to get the
				;current local DMA address.

.EQU	NICPSTART = 0x01	;Page Start Register, Type=W in Page0, Type=R in Page 2
				;The Page Start register sets the start page
				;address of the receive buffer ring

.EQU	NICPSTOP = 0x02		;Page Stop Register, Type=W in Page0, Type=R in Page2
				;The Page Stop register sets the stop page
				;address of the receive buffer ring. In 8 bit
				;mode the PSTOP register should not exceed
				;to 0x60, in 16 bit mode the PSTOP register
				;should not exceed to 0x80.

.EQU	NICBNRY	= 0x03		;Boundary Register, Type=R/W in Page0
				;This register is used to prevent overwrite
				;of the receive buffer ring. It is typically
				;used as a pointer indicating the last receive
				;buffer page the host has read.

.EQU	NICTPSR = 0x04		;Transmit Page Start Register, Type=W in Page0
				;This register sets the start page address of the
				;packet to the transmitted

.EQU	NICTBCR0 = 0x05
.EQU	NICTBCR1 = 0x06		;Transmit Byte Count Registers, Type=W in Page0
				;These two registers set the byte counts of the
				;packet to be transmitted.

.EQU	NICNCR	= 0x05		;Number of Collisions Register, Type=R in Page0
				;The register records the number of collisions
				;a node experiences during a packet transmission.

.EQU	NICFIFO	= 0x06		;First In First Out Register, Type=R in Page0
				;This register allows the host to examine
				;the contents of the FIFO after loopback.

.EQU	NICCRDA0 = 0x08
.EQU	NICCRDA1 = 0x09		;Current Remote DMA Address registers, Type=R in Page0
				;These two registers contain the current address of remote DMA.

.EQU	NICRSAR0 = 0x08
.EQU	NICRSAR1 = 0x09		;Remote Start Address Registers, Type=W in Page0
				;These two registers set the start address of
				;remote DMA.

.EQU	NICRBCR0 = 0x0A
.EQU	NICRBCR1 = 0x0B		;Remote Byte Count Registers, Type=W in Page0
				;These two registers se the data byte counts of
				;remote DMA.

.EQU	NICCNTR0 = 0x0D		;Frame Alignment Error Tally Counter Register, Type=R in Page0

.EQU	NICCNTR1 = 0x0E		;CRC Error Tally Counter Register, Type=R in Page0

.EQU	NICCNTR2 = 0x0F		;Missed Packet Tally Counter Register, Type=R in Page0

.EQU	NICPAR0	= 0x01
.EQU	NICPAR1	= 0x02
.EQU	NICPAR2	= 0x03
.EQU	NICPAR3	= 0x04
.EQU	NICPAR4	= 0x05
.EQU	NICPAR5	= 0x06		;Physical Address Registers, Type=R/W in Page1
				;These registers contain my Ethernet node address
				;and are used to compare the destination adderss
				;of incoming packets for acceptation or rejection

.EQU	NICCURR = 0x07		;Current Page Register, Type=R/W in Page1
				;This register points to the page address of
				;the first receive buffer page to be used for
				;a packet reception.

.EQU	NICMAR0 = 0x08
.EQU	NICMAR1 = 0x09
.EQU	NICMAR2 = 0x0A
.EQU	NICMAR3 = 0x0B
.EQU	NICMAR4 = 0x0C
.EQU	NICMAR5 = 0x0D
.EQU	NICMAR6 = 0x0E
.EQU	NICMAR7 = 0x0F		;Multicast Address Register, Type=R/W in Page1
				;These registers provide filtering bits of
				;multicast addresses hashed by the CRC logic

.EQU	NICCR9346 = 0x01	;Page 3
.EQU	NICRDMAPORT = 0x10
.EQU	NICRSTPORT = 0x18



;*****************************************************************************
;
; RTL ISR Register Bits
;
;****************************************************************************/
.EQU	ISR_RST	= 7
.EQU	ISR_RDC	= 6
.EQU	ISR_OVW	= 4
.EQU	ISR_PTX	= 1
.EQU	ISR_PRX	= 0


;*****************************************************************************
;
;  RTL Register Initialization Values
;
;****************************************************************************
				;RCR : accept broadcast packets and packets destined to this MAC
				;      drop short frames and receive errors
.equ	RCR_INIT = 0x04

				;TCR : default transmit operation - CRC is generated
.equ	TCR_INIT = 0x00

				;DCR : allows send packet to be used for packet retreival
				;      FIFO threshold: 8-bits (works)
				;      8-bit transfer mode
.equ	DCR_INIT = 0x58

				;IMR : interrupt enabled for receive and overrun events
.equ	IMR_INIT = 0x11

				;buffer boundaries - transmit has 6 256-byte pages
				;   receive has 26 256-byte pages
				;   entire available packet buffer space is allocated
.equ	TXSTART_INIT = 0x40
.equ	RXSTART_INIT = 0x46
.equ	RXSTOP_INIT  = 0x60



;*********************************************************
;* Receive RTL8019's Ring Buffer Page Header Layout
;*   This is the 4-byte header that resides infront of the
;*   data packet in the receive buffer.
;*********************************************************
.equ	enetpacketstatus= 0x00
.equ	nextblock_ptr	= 0x01
.equ	enetpacketLenL	= 0x02
.equ	enetpacketLenH	= 0x03


; LANBUFSIZE: The size of the buffer that holds incoming and outgoing packets.
.equ	LANBUFSIZE = 1564


;*****************************************************************************
;
;  Ethernet constants
;
;****************************************************************************
.equ	ETHERNET_MIN_PACKET_LENGTH = 0x3C
.equ	ETHERNET_HEADER_LENGTH = 0x0E

.equ	IP_TCP_HEADER_LENGTH = 40
.equ	TOTAL_HEADER_LENGTH = IP_TCP_HEADER_LENGTH+ETHERNET_HEADER_LENGTH



;***************************************************************************
.ESEG

; ETHADDR: The Ethernet address
; 48 bit IEEE OUI (Organizationally Unique Identifier)

EETHADDR:	.db	"DEDO",0x00,0x01


;***************************************************************************
.DSEG

; packet[LANBUFSIZE]
;
; The packet array is used to hold incoming and outgoing packets.
; The device driver fills this with incoming packets.

pageheader:	.byte 4			;Rx RTL8019's page header
packet:		.byte LANBUFSIZE
nextPage:	.byte 1			;RTL8019's page header -> next page
currentRetreiveAddress:	.byte 2		;innen kell a kilvasast folytatni (pointer)


;pointers to locations in the RTL8019 receive buffer
uip_len:		.byte 2

ETHADDR:	.byte 6


;***************************************************************************

.CSEG

;*****************************************************
;** SetLANToActive
;**
;** IN: -
;**
;** Out: -
;*
;* Alt: R16
;*
;* Description: Sets LAN to Active & SRAM to Inactive
;*
;
SetLANToActive:
       		SBI	RAMCS_PORT,RAMCS
		nop
       		CBI	LANCS_PORT,LANCS
		nop
		ret

;*****************************************************
;** SetLANToInactive
;**
;** IN: -
;**
;** Out: -
;*
;* Alt: R16
;*
;* Description: Sets LAN to Inactive LANCS=H
;*
;
SetLANToInactive:
       		SBI	LANCS_PORT,LANCS
		nop
		ret


;*****************************************************
;** SetISAReset
;**
;** IN: R0 -> 1 ISA Reset, 0 Normal mode
;**
;** Out: -
;*
;* Alt: -
;*
;* Description: Sets ISA RESET Line to R0; 0 = low, 1 = hi
;*
;
SetISAReset:
		sbrs	R0,0
       		CBI	LANRES_PORT,LANRES
		sbrc	R0,0
       		SBI	LANRES_PORT,LANRES
		nop
		ret

;*****************************************************
;** Wait1ms
;**
;**  In: R16 - 16*1ms varakozas
;**
;**  Out: -
;**
;**
;** Alt R16, XL,XH
;**
;** Description:  wating for R16 * 1 ms
;*
Wait1ms:
		ldi	XL,low(SYSCLK/(5*1000))
		ldi	Xh,high(SYSCLK/(5*1000))
Waitx1:		sbiw	XL,1		;[2] \
		nop			;[1]  - 5 cycles in loop
		brne	Waitx1		;[2] /
		dec	R16
		brne	Wait1ms
		ret


;******************************************************
;**  RTLHW_Reset
;**
;** In: -
;**
;** Out: -
;**
;** ALt: R0, R16, XL, XH
;**
;** Description: Power-up initialization of the RTL8019 and ISA
;*
RTLHW_Reset:
		rcall	SetLANToInactive
		clr	R0
		inc	R0
		rcall	SetISAReset
		ldi	R16,10
		rcall	Wait1ms			;10ms warakozas
		clr	R0
		rcall	SetISAReset
		ret


;*****************************************************************************
;**  RTLreadReg
;**
;** IN: R16 - RTL_ADDRESS offset
;**
;** Out: R17 - register data
;**
;** Alt: -
;**
;** Description: Reads byte from RTL8019 register
;*
RTLReadReg:
		rcall	SetLANToActive
		push	XL
		push	XH
		ldi	XL,low(NICBASE)
		add	XL,R16
		ldi	XH,high(NICBASE)
		ldi	R17,0
		adc	XH,R17				;X = NIC real address to ISA
		ld	R17,X
		rcall	SetLANToInactive
		pop	XH
		pop	XL
		ret

;*****************************************************************************
;**  RTLWriteReg
;**
;**  IN: R16 - RTL_ADDRESS offset
;**	 R17 - RTL_DATA
;**
;**  Out: -
;**
;**  Alt: R0
;**
;** Description: Writes byte to RTL8019 register.
;*
RTLWriteReg:
		rcall	SetLANToActive
		push	XL
		push	XH
		clr	R0
		ldi	XL,low(NICBASE)
		add	XL,R16
		ldi	XH,high(NICBASE)
		adc	XH,R0				;X = NIC real address to ISA
		st	X,R17
		rcall	SetLANToInactive
		pop	XH
		pop	XL
		ret

;***********************************************************************
;** RTL8019_Init
;**
;** IN: -
;**
;** Out: -
;**
;** ALt: R0, R16, XL, XH
;**
;* Description:	Sets up the RTL8019 NIC hardware interface, and initializes
;*		the buffers and configuration of the NIC
;
RTL8019_Init:
		rcall	RTLHW_Reset
						;do soft reset & clear pending interrupt
		ldi	R16,NICISR		;clear Interrupt register
		rcall	RTLReadReg
		ldi	R16,NICISR
		rcall	RTLWriteReg

		ldi	R16,50			;50ms varakozas
		rcall	Wait1ms

		ldi	R16,NICCR		;stop the NIC, abort DMA, page 0
		ldi	R17,0x21
		rcall	RTLWriteReg
		ldi	R16,2			;Delay 2ms
		rcall	Wait1ms			;make sure nothing is coming in or going out


;	CONFIGx
		ldi	R16,NICCR
		ldi	R17,0xC0		;select Page3
		rcall	RTLWriteReg

		ldi	R16,NICCR9346		;Enable SW Config
		ldi	R17,0xC0
		rcall	RTLWriteReg

		ldi	R16,0x04
		ldi	R17,0b10000000		;IRQ0, Base 0x300
		rcall	RTLWriteReg

		ldi	R16,0x05
		ldi	R17,0b00100000		;AutoDetect, ROM Disabled
		rcall	RTLWriteReg

		ldi	R16,0x06
		ldi	R17,0b00010000		;Halfduplex, LED0 - link, LED1 - RX led2 - TX
		rcall	RTLWriteReg

		ldi	R16,0x0D
		ldi	R17,0x00
		rcall	RTLWriteReg

		ldi	R16,NICCR9346
		ldi	R17,0x00		;End Config
		rcall	RTLWriteReg

; !!!!

		ldi	R16,NICCR		;stop the NIC, page 0
		ldi	R17,0x01
		rcall	RTLWriteReg


		ldi	R16,NICDCR
		ldi	R17,DCR_INIT		;0x58 Normal Operation, FIFO Treshold, Auto Init Remote
		rcall	RTLWriteReg


		ldi	R16,NICRBCR0
		ldi	R17,0x00
		rcall	RTLWriteReg
		ldi	R16,NICRBCR1
		rcall	RTLWriteReg		;Remote DMA Byte Count =0x0000

		ldi	R16,NICRCR
		ldi	R17,0x04		;only packets with broadcast
		rcall	RTLWriteReg		;destination address are accepted

		ldi	R16,NICTPSR
		ldi	R17,TXSTART_INIT	;Transmit Page Start Register=40
		rcall	RTLWriteReg

		ldi	R16,NICTCR
		ldi	R17,0x02		;Internal loopback
		rcall	RTLWriteReg

		ldi	R16,NICPSTART
		ldi	R17,RXSTART_INIT	;page address of the receive buffer ring = 46
		rcall	RTLWriteReg
		ldi	R16,NICBNRY		;utolso olvasott page = 46
		rcall	RTLWriteReg

		ldi	R16,NICPSTOP
		ldi	R17,RXSTOP_INIT		;stop page address of the receive buffer ring = 60
		rcall	RTLWriteReg

		ldi	R16,NICCR
		ldi	R17,0x61		;Stop & Abort DMA
		rcall	RTLWriteReg
		ldi	R16,2			;2ms varakozas
		rcall	Wait1ms

		ldi	R16,NICCURR
		ldi	R17,RXSTART_INIT	;Rx Page = 46
		rcall	RTLWriteReg

		ldi	ZL,low(EETHADDR)	;MAC address from EEPROM
		ldi	ZH,high(EETHADDR)
		call	EERead
		sts	ETHADDR+0,R0
		mov	R17,R0
		ldi	R16,NICPAR0		;Setup our MAC Address
		rcall	RTLWriteReg

		adiw	ZL,1
		call	EERead
		sts	ETHADDR+1,R0
		mov	R17,R0
		ldi	R16,NICPAR1
		rcall	RTLWriteReg

		adiw	ZL,1
		call	EERead
		sts	ETHADDR+2,R0
		mov	R17,R0
		ldi	R16,NICPAR2
		rcall	RTLWriteReg

		adiw	ZL,1
		call	EERead
		sts	ETHADDR+3,R0
		mov	R17,R0
		ldi	R16,NICPAR3
		rcall	RTLWriteReg

		adiw	ZL,1
		call	EERead
		sts	ETHADDR+4,R0
		mov	R17,R0
		ldi	R16,NICPAR4
		rcall	RTLWriteReg

		adiw	ZL,1
		call	EERead
		sts	ETHADDR+5,R0
		mov	R17,R0
		ldi	R16,NICPAR5
		rcall	RTLWriteReg




		ldi	R16,NICCR
		ldi	R17,0x21		;Stop NIC, Abort DMA
		rcall	RTLWriteReg

		ldi	R16,NICDCR
		ldi	R17,DCR_INIT		;58 Normal operation...
		rcall	RTLWriteReg

		ldi	R16,NICCR
		ldi	R17,0x22		;Start Command, DMA abort
		rcall	RTLWriteReg

		ldi	R16,NICISR
		ldi	R17,0xFF
		rcall	RTLWriteReg		;Clear pending Interrupt

		ldi	R16,NICIMR
		ldi	R17,IMR_INIT		;11, Interrupt Enable
		rcall	RTLWriteReg		;packet received with no errors
						;receive buffer has been exhausted
		ldi	R16,NICTCR
		ldi	R17,TCR_INIT		;00 - Normal operation
		rcall	RTLWriteReg

		ldi	R16,NICCR
		ldi	R17,0x22
		rcall	RTLWriteReg		;start the NIC
		ret


;***********************************************************************
;** RTL8019endPacketSend
;**
;** In :  -
;**
;**
;** Alt :
;**
;* Description:	Ends a packet send operation and instructs the NIC to transmit
;*		the frame over the network
;
RTL8019endPacketSend:
		ldi	R16,NICCR	;send the contents of the transmit buffer onto the network
		ldi	R17,0x24
		rcall	RTLWriteReg
					;clear the remote DMA interrupt
		ldi	R16,NICISR
		ldi	R17,(1<<ISR_RDC)
		rcall	RTLWriteReg
		ret


;***********************************************************************
;** RTL8019sendPacketData
;**
;** In : X - local data buffer
;**		 Pointer to the beginning of the buffer to load into the NIC
;**      Z - Length
;**		 number of bytes to copy to the NIC
;**
;** Alt : XL, XH, R20, R16
;**
;* Description:	Loads length # of bytes from a local buffer to the transmit
;*		packet buffer.
;* Notes:	RTL8019beginPacketSend() must be called before sending any data.
;*		Several calls to RTL8019retreivePacketData() may be made to
;*		copy packet data from different buffers
;
RTL8019sendPacketData:
		ldi	R16,NICRDMAPORT
		ld	R17,X+
		rcall	RTLWriteReg
		sbiw	ZL,1
		brne	RTL8019sendPacketData
		ret


;***********************************************************************
;** RTL8019beginPacketSend
;**
;** In : R17:R16 - (packetLength) length of the Ethernet frame
;**
;** Out: R21:R20 - sendPacketLength
;** Alt : R18,R19,R20,R21
;**
;*  Description: Sets up the NIC to send a packet
;*  Notes:  The NIC will not send packets less than 60 bytes long (the min
;*          Ethernet frame length.  The transmit length is automatically
;*          increased to 60 bytes if packetLength is < 60
;
RTL8019beginPacketSend:
		movw	R20,R16		;sendPacketLength:=packetLength
		cpi	R16,low(ETHERNET_MIN_PACKET_LENGTH)
		ldi	R19,high(ETHERNET_MIN_PACKET_LENGTH)
		cpc	R17,R19
		brsh	Packetjo
		ldi	R20,low(ETHERNET_MIN_PACKET_LENGTH)
		ldi	R21,high(ETHERNET_MIN_PACKET_LENGTH)

Packetjo:
		push	R17		;R21:R20 - > sendPacketLength
		push	R16		;R17:R16 - > packetLength

		ldi	R16,NICCR
		ldi	R17,0x22
		rcall	RTLWriteReg	;start the NIC


packf:		ldi	R16,NICCR	;still transmitting a packet - wait for it to finish
		rcall	RTLReadReg	;while( readRTL(CR) & 0x04 );
		andi	R17,0x04
		brne	packf

					;load beginning page for transmit buffer
		ldi	R16,NICTPSR
		ldi	R17,TXSTART_INIT
		rcall	RTLWriteReg

					;set start address for remote DMA operation
		ldi	R16,NICRSAR0
		ldi	R17,0x00
		rcall	RTLWriteReg
		ldi	R16,NICRSAR1
		ldi	R17,0x40
		rcall	RTLWriteReg

					;clear the packet stored interrupt
		ldi	R16,NICISR
		ldi	R17,(1<<ISR_PTX)
		rcall	RTLWriteReg

					;load data byte count for remote DMA
		ldi	R16,NICRBCR0
		pop	R17
		rcall	RTLWriteReg
		ldi	R16,NICRBCR1
		pop	R17
		rcall	RTLWriteReg


		ldi	R16,NICTBCR0
		mov	R17,R20
		rcall	RTLWriteReg
		ldi	R16,NICTBCR1
		mov	R17,R21
		rcall	RTLWriteReg

					;do remote write operation
		ldi	R16,NICCR
		ldi	R17,0x12
		rcall	RTLWriteReg
		ret


;***********************************************************************
;** RTL8019retreivePacketData
;**
;** In :  X - local buffer - Pointer to the beginning of the buffer to store
;**			     the ethernet frame.
;**       Z - Lenght - number of bytes to copy to localBuffer
;**
;**
;** Alt :
;**
;* Description:	Loads length # of bytes from the receive packet buffer to
;*		a local buffer
;* Notes:	RTL8019beginPacketRetreive() must be called before retreiving
;*		any data.
;*		Several calls to RTL8019retreivePacketData() may be made to
;*		copy packet data to different buffers
;
RTL8019retreivePacketData:
		ldi	R16,NICRBCR0		;initiate DMA to transfer the data
		mov	R17,ZL
		rcall	RTLWriteReg
		ldi	R16,NICRBCR1
		mov	R17,ZH
		rcall	RTLWriteReg

		ldi	R16,NICRSAR0
		lds	R17,currentRetreiveAddress+0
		mov	R18,R17
		rcall	RTLWriteReg
		ldi	R16,NICRSAR1
		lds	R17,currentRetreiveAddress+1
		rcall	RTLWriteReg			;R17:R18 retriveaddr
							;Z - lenght

		add	R18,ZL
		adc	R17,ZH
		ldi	R16,low(0x6000)
		ldi	R19,high(0x6000)		;circular buffer turn
		cp	R18,R16
		cpc	R17,R19
		brcs	noturn

		ldi	R16,low(0x6000-0x4600)
		ldi	R19,high(0x6000-0x4600)
		sub	R18,R16
		sbc	R17,R19
noturn:
		sts	currentRetreiveAddress+0,R18
		sts	currentRetreiveAddress+1,R17

		ldi	R16,NICCR
		ldi	R17,0x0A
		rcall	RTLWriteReg			;Start DMA

readdata:
		ldi	R16,NICRDMAPORT
		rcall	RTLReadReg
		st	X+,R17
		sbiw	ZL,1
		brne	readdata

		ldi	R16,NICCR			;end the DMA operation
		ldi	R17,0x22
		rcall	RTLWriteReg
		rcall	WaitToDMA
		ret


;***********************************************************************
;** WaitToDMA
;**
;** In : -
;**
;** Out: -
;**
;** Alt :
; Description:	Waiting for DMA Completed
; Notes:
;
WaitToDMA:
		ldi	R19,200			;varunk, hogy a DMA befejezodjon
waitDMA1:	ldi	R16,NICISR
		rcall	RTLReadReg
		andi	R17,(1<<6)
		brne	DMAOk
		dec	R19
		brne	waitDMA1
DMAOk:
		ldi	R16,NICISR
		ldi	R17,(1<<6)
		rcall	RTLWriteReg		;ha torlodot, ha nem mi toroljuk
		ret

;***********************************************************************
;** RTL8019endPacketRetreive
;**
;** In :
;**
;** Out:
;**
;** Alt :
; Description:	Ends a packet retreive operation begun by calling
;		RTL8019beginPacketRetreive(). The NIC buffer space used by
;		the retreived packet is freed.
; Notes:	A packet may be removed from the buffer without being read
;		by calling RTL8019endPacketRetreive() after RTL8019beginPacketRetreive().
;
RTL8019endPacketRetreive:
		ldi	R16,NICCR		;end the DMA operation
		ldi	R17,0x22
		rcall	RTLWriteReg

		rcall	WaitToDMA
						;set the boundary register
						;to point to the start of
						;the next packet
		ldi	R16,NICBNRY
		lds	R17,nextPage
		rcall	RTLWriteReg
		ret


;********************************************************************
;** processRTL8019Interrupt
;**
;** In: -
;**
;** Out: -
;**
;** Alt: R16,R17,R20
;**

;* Description:	Reads the NIC's ISR register looking for a receive buffer
;*		overrun - which is then handled.
;* Notes:	The function does not need to be called in response to an
;*		interrupt. The function can be polled and the NIC's INT
;*		line not used. This function should be called before attempting
;*		to retreive a packet from the NIC
;
processRTL8019Interrupt:
		ldi	R16,NICISR
		rcall	RTLReadReg
		sbrs	R17,ISR_OVW		;Overrun?
			ret			;Skip if true

;*  Notes:	This function must be called before retreiving packets from
;*		the NIC if there is a buffer overrun
;
overrun:
		ldi	R16,NICCR
		rcall	RTLReadReg
		mov	R20,R17			;CR allpotat elmentjuk
		ldi	R16,NICCR
		ldi	R17,0x21		;Strop NIC, Abort DMA
		rcall	RTLWriteReg

		ldi	R16,2
		rcall	Wait1ms			;varunk, de mire?

		ldi	R16,NICRBCR0
		ldi	R17,0x00
		rcall	RTLWriteReg
		ldi	R16,NICRBCR1
		rcall	RTLWriteReg		;Remote Count = 0

		ldi	R16,NICTCR
		ldi	R17,0x02		;internal loopback
		rcall	RTLWriteReg

		ldi	R16,NICCR
		ldi	R17,0x22		;Start NIC, DMA Abort
		rcall	RTLWriteReg

		ldi	R16,NICBNRY
		ldi	R17,RXSTART_INIT
		rcall	RTLWriteReg

		ldi	R16,NICCR
		ldi	R17,0x62
		rcall	RTLWriteReg

		ldi	R16,NICCURR
		ldi	R17,RXSTART_INIT
		rcall	RTLWriteReg

		ldi	R16,NICCR
		ldi	R17,0x22
		rcall	RTLWriteReg

		ldi	R16,NICISR
		ldi	R17,0x10		;overflow bit torolve
		rcall	RTLWriteReg

		ldi	R16,NICTCR
		ldi	R17,TCR_INIT
		rcall	RTLWriteReg

		ldi	R16,NICISR
		ldi	R17,0xFF
		rcall	RTLWriteReg

		ret



;*****************************************************************************
;*  RTL8019beginPacketRetreive
;*
;*  In: -
;*
;*  Out: Z - length of the Ethernet frame (see note)
;*	 c flag = 0 nincs adat
;*
;*  Alt: R0, R16, R17, R18, R19, R20
;*
;* Description:	Sets up the NIC to retreive a packet
;* Notes:	The size returned is the size of all the data in the Ethernet
;*		frame minus the Ethernet checksum.  This may include unused
;*		trailer bytes appended if data is less than the minimum
;*		Ethernet frame length (60 bytes).  A size of zero indicates
;*		there are no packets available.
;*		A call to RTL8019beginPacketRetreive() must be followed by a
;*		call to RTL8019endPacketRetreive() regardless if data is
;*		retreived, unless 0 is returned.
;*****************************************************************************
;
RTL8019beginPacketRetreive:
		rcall	processRTL8019Interrupt	;check for and handle an overflow

		ldi	R16,NICCR		;read CURR from page 1
		ldi	R17,0x62
		rcall	RTLWriteReg
		ldi	R16,NICCURR
		rcall	RTLReadReg
		push	R17			;Current Rx page address

		ldi	R16,NICCR		;return to page 0
		ldi	R17,0x22
		rcall	RTLWriteReg
						;read the boundary register
						;pointing to the beginning of the packet
		ldi	R16,NICBNRY
		rcall	RTLReadReg
		mov	R20,R17			;R20=bnry Rx Buff vege page

		CLR	ZL
		CLR	ZH
		pop	R16			;Current Rx page
		cp	R16,R17
		brne	Packetvan
		ret				;return if there is no packet in the buffer

Packetvan:
		ldi	R16,NICISR		;clear the packet received interrupt flag
		ldi	R17,(1<<ISR_PRX)
		rcall	RTLWriteReg
						;the boundary pointer is invalid,
						;reset the contents of the buffer and exit
		ldi	R16,RXSTOP_INIT
		cp	R20,R16
		brsh	invpointer
		ldi	R16,RXSTART_INIT
		cp	R20,R16
		brsh	jopointer		;if((bnry >= RXSTOP_INIT) || (bnry < RXSTART_INIT))

invpointer:
		ldi	R16,NICBNRY
		ldi	R17,RXSTART_INIT
		rcall	RTLWriteReg
		ldi	R16,NICCR
		ldi	R17,0x62
		rcall	RTLWriteReg
		ldi	R16,NICCURR
		ldi	R17,RXSTART_INIT
		rcall	RTLWriteReg
		ldi	R16,NICCR
		ldi	R17,0x22
		rcall	RTLWriteReg
		clc
		ret				;Z= meg mindig 0!!

jopointer:
		ldi	R16,NICRBCR0		;initiate DMA to transfer the RTL8019 packet header
		ldi	R17,0x04
		rcall	RTLWriteReg
		ldi	R16,NICRBCR1
		ldi	R17,0x00
		rcall	RTLWriteReg		;Remote Byte Count = 4


		ldi	R16,NICRSAR0
		ldi	R17,0x00
		rcall	RTLWriteReg
		ldi	R16,NICRSAR1
		mov	R17,R20
		rcall	RTLWriteReg		;Remote start address= Start boundary page

		ldi	R16,NICCR
		ldi	R17,0x0A
		rcall	RTLWriteReg		;Start Remote Read DMA

		ldi	R18,4			;4 byte-ot olvasunk be
		ldi	XL,Low(pageheader)	;ide
		ldi	XH,high(pageheader)

Getph:		ldi	R16,NICRDMAPORT
		rcall	RTLReadReg
		st	X+,R17
		dec	R18
		brne	Getph

		ldi	R16,NICCR		;end the DMA operation
		ldi	R17,0x22
		rcall	RTLWriteReg

		rcall	WaitToDMA


		lds	ZL,pageheader+enetpacketLenL
		lds	ZH,pageheader+enetpacketLenH	;Z=rxlen

		lds	R16,pageheader+nextblock_ptr	;nextPage
		sts	nextPage,R16			;eltaroljuk ,aborthoz kell

		ldi	R17,4
		sts	currentRetreiveAddress+0,R17
		sts	currentRetreiveAddress+1,R20	;innen folytatjuk majd a kiolvasast (offset cim)

	;if the nextPage pointer is invalid, the packet is not ready yet - exit

		ldi	R18,RXSTOP_INIT
		cp	R16,R18
		brsh	pageErr1
		ldi	R18,RXSTART_INIT
		cp	R16,R18
		brcs	pageErr1	;if((nextPage >= RXSTOP_INIT) || (nextPage < RXSTART_INIT))
		clr	R18
		sub	ZL,R17
		sbc	ZH,R18		;rxlen-4
		sec
		ret

pageErr1:	clr	Zl
		clr	ZH
		clc
		ret




;*****************************************************************************
; Device Manager
; A foprogrambol ezek hivhatoak

;*****************************************************************************
;*  RTL8019dev_send
;*
;*  In:
;*
;*  Out:
;*
;*  Alt:
;*
;* Description:	Sends the packet contained in packet over the network
;
RTL8019dev_send:
		ldi	R16,low(LANBUFSIZE)
		ldi	R17,high(LANBUFSIZE)
RTL8019dev_sendA:
		push	R16
		push	R17
		rcall	RTL8019beginPacketSend

		;send packet, using data in ip_appdata if over the IP+TCP header size

		ldi	XL,Low(packet)
		ldi	XH,high(packet)
		pop	ZH
		pop	ZL
		rcall	RTL8019sendPacketData

		rcall	RTL8019endPacketSend
		ret


;*****************************************************************************
;*  RTL8019dev_poll
;*
;*  In: -
;*
;*  Out: Z = Length of the packet retreived, or zero if no packet retreived
;*	 Y = Address of retreived packet
;*	 c flag = 0, no packet
;*
;* Description:	Polls the RTL8019 looking for an overflow condition or a new
;*		packet in the receive buffer. If a new packet exists and will
;*		fit in packet, it is retreived, and the length is returned.
;*		A packet bigger than the buffer is discarded
;*****************************************************************************/
;
RTL8019dev_poll:
		rcall	RTL8019beginPacketRetreive ;Z = packetLength
		brcs	Vanpacket2		;if there's no packet or an error
						;exit without ending the operation
		ret
Vanpacket2:
		cpi	ZL,low(LANBUFSIZE+1)
		ldi	R16,high(LANBUFSIZE+1)	;drop anything too big for the buffer
		cpc	ZH,R16
		brcs	PacketSizejo
						;Ezt el kell dobni, tul nagy
		rcall	RTL8019endPacketRetreive
		clr	ZL
		clr	ZH
		clc
		ret

PacketSizejo:
		push	ZL			;copy the packet data into the IP packet buffer
		push	ZH
		ldi	XL,low(packet)
		ldi	XH,high(packet)
		rcall	RTL8019retreivePacketData
		rcall	RTL8019endPacketRetreive
		pop	ZH
		pop	ZL
		sts	pageheader+enetpacketLenL,ZL
		sts	pageheader+enetpacketLenH,ZH
		ldi	YL,low(packet)
		ldi	YH,high(packet)
		sec
		ret


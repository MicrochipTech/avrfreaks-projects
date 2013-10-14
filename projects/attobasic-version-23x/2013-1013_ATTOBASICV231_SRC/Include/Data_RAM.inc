;******************************************************************
;		MEMORY ALLOCATION
;******************************************************************
.dseg
.org		SRAM_START
;******************************************************************
;
.set	DATASTKSZ	= 16						;Data stack size
.equ	VARSTORSZ	= 26						;variable storage size
												; for A - Z
.set	READSTKSZ	= DATASTKSZ					;DATA statement size (w/o NRF)

.set	SCRPADSZ	= 0							;Scratchpad RAM size

.equ	LNBUFFSZ	= 64						;line buffer size

.equ	GOSUDEPTH	= 4							;GOSUB depth
.equ	GOSUCHUNK	= 2							;GOSUB chunk size (bytes per level)
.equ	GOSUSTKSZ	= (GOSUCHUNK * GOSUDEPTH)	;GOSUB return address stack size

.equ	LOOPDEPTH	= 4							;FOR_NEXT loop depth
.equ	LOOPCHUNK	= 4							;FOR_NEXT loop chunk size (bytes per level)
.equ	LOOPSTKSZ	= (LOOPCHUNK * LOOPDEPTH)	;FOR_NEXT loop stack size

.set	MISCVARSZ	= 7							;starting size of internal variables

.if ( NRF || OWR )
 .set	NRFBUFFSZ	= 0							;nRF24L01 TX & RX buffer sizes
 .set	READSTKSZ	= NRF24_MAX_MSG_LEN			;resize DATA statement for NRF use
 .set	DATASTKSZ	= NRF24_MAX_MSG_LEN			;re-size Data stack size
;  .set	MISCVARSZ	= MISCVARSZ + 1				;add NRF RAM usage
.endif

.if DHT
 .set	MISCVARSZ	= MISCVARSZ + 7				;add DHT RAM usage
.endif
;
.if RTC
 .set	MISCVARSZ	= MISCVARSZ + 5				;add RTC RAM usage
.endif

.if ICP
 .set	MISCVARSZ	= MISCVARSZ + 1				;add ICP RAM usage
.endif

.if DDS
 .set	MISCVARSZ	= MISCVARSZ + 7				;add DDS RAM usage
.endif

.if DATA
 .set	MISCVARSZ	= MISCVARSZ + 4				;add DATA RAM usage
.endif
;
.if EFS
 .set	MISCVARSZ	= MISCVARSZ + 2				;add EFS RAM usage
.endif

#if (USB)
 .equ	USBCONFSZ	= 16						;USB lib needs 16 bytes of RAM
#endif

;	VARIABLE STORAGE
;.equ	vartop		= LOW(VARSTOR+VARSTORSZ)	;First byte in variable space
;.equ	varbot		= LOW(VARSTOR)				;Last byte in varibale space
;.equ	varpage		= HIGH(VARSTOR)

;	LINE BUFFER
;.equ	lbufftop	= LOW(LNBUFF+LNBUFFSZ)		;First byte (top) of line buffer
;.equ	lbuffbot 	= LOW(LNBUFF)				;Last byte in terminal input buffer
;.equ	lbuffpage	= HIGH(LNBUFF)

;	DATA STACK
;.equ	dstacktop	= LOW(DATASTK+DATASTKSZ)	;First byte (top) of data stack
;.equ	dstackbot 	= LOW(DATASTK)				;Last byte in data stack
;.equ	dstackpage	= HIGH(DATASTK)				;Entire dstack must be on one page
;
.if (BIGRAM)
  #if (USB)
  .set	PROGMEMSZ	= 	( SRAM_SIZE - \
  						( MCUSTKSZ + \
						  DATASTKSZ + VARSTORSZ + \
						  READSTKSZ + LNBUFFSZ + \
						  GOSUSTKSZ + LOOPSTKSZ + \
						  SCRPADSZ + \
						  EFSBLKSZ + \
						  NRFBUFFSZ + \
						  MISCVARSZ + \
						  USBCONFSZ ))			;Program memory size
 
  #else
   #if ( 	defined(__ATmega88__) || \
		  defined(__ATmega88PA__) )

   .set	PROGMEMSZ	= 	( SRAM_SIZE - MCUSTKSZ ) -  \
						(( DATASTKSZ + VARSTORSZ + \
						   READSTKSZ + LNBUFFSZ) + \
						   GOSUSTKSZ + LOOPSTKSZ + \
						   MISCVARSZ ) 			;Program memory size
   #else
  .set	PROGMEMSZ	= 	( SRAM_SIZE - \
  						( MCUSTKSZ + \
						  DATASTKSZ + VARSTORSZ + \
						  READSTKSZ + LNBUFFSZ + \
						  GOSUSTKSZ + LOOPSTKSZ + \
						  SCRPADSZ + \
						  EFSBLKSZ + \
						  NRFBUFFSZ + \
						  MISCVARSZ ))			;Program memory size
   #endif
  #endif
.else
  .set	PROGMEMSZ = (EEPROMEND)
;
.endif
;
;******************************************************************
;					MEMORY ALLOCATION
;******************************************************************
;System RAM starts at the location just below the reserved stack
; space and grows donwward.  Thus the amount of program memory
; is determined by the amount of RAM used by AttoBASIC.
;
;The system memory layout is as follows:
; 
PROGMEM:	.byte	PROGMEMSZ	;PROGRAM memory storage
VARSTOR:	.byte	VARSTORSZ	;VARIABLE storage
READSTOR:	.byte	READSTKSZ	;DATA statement storage
SCRPAD:		.byte	SCRPADSZ	;Scratchpad RAM
DATASTK:	.byte	DATASTKSZ	;DATA stack
GOSUSTK:	.byte	GOSUSTKSZ	;DGOSUB-RETURN stack
LOOPSTK:	.byte	LOOPSTKSZ	;FOR-NEXT loop stack
LNBUFF:		.byte	LNBUFFSZ	;LINE buffer
;
;++++++++++++++++++++++++++++++++++++++++++
VARstart:	;beginning of RAM storage
PCLO:		.byte	1		;Marks position in program counter for virtual machine
PCHI:		.byte	1		;This pair points to next available space
;
;=============================================================================
ErrCode:	.byte	1			;holding location for error codes
CurrLine:	.byte	1			;holding location for current program line#
;DATAsize:	.byte	1			;holding location for DATA statement buffer (# of elements)
DATAptr:	.byte	2			;holding location for DATA statement pointer
								; plus number of elements
RNDSeed:	.byte	1			;save the current random # seed here
;
.if ICP
 ICPCfg:	.byte	1			;[0..2] 	= ICP gate time coefficient
.endif								;[7]	= fall/rise edge detect
;
.if RTC
 RTCMul:	.byte	1			;holds RTC time multiplier count ("N" x 1mS")
 RTCReg:	.byte	4			;reserve 32-bits for Real-time counter
.endif
;
.if DATA
 DFDfmt:	.byte	1			;[0..7] = 1 to 8 data values per record
 DFIbyte:	.byte	1			;value to use for bulk erase
 DFLaddr:	.byte	2			;internal data logger address counter
.endif
;
.if DHT
RH_I:		.byte	1			;%RH integer
RH_D:		.byte	1			;%RH decimal
RT_I:		.byte	1			;Temperature integer (degrees C)
RT_D:		.byte	1			;Temperature decimal (degrees C)
RT_TMR:		.byte	2			;16-bit DHT timer (in mS)
RT_NRC:		.byte	1			;DHT no-response counter
.endif
;
;=============================================================================
.if EFS	;EEP file system support
 EFSlab:	.byte	1			;last available block
 EFSblkadr:	.byte	1			;desired block number
 EFSbuff:	.byte	EFSBLKSZ	;reserve EFSBLKSZ bytes for buffer
.endif
;
;=============================================================================
;.if NRF	;nRF24L01 support
; NRFRXpipe:	.byte	1			;RX pipe #'s data is in the buffer
; NRFRX0sz:	.byte	1			;payload size of RX pipe 0
; NRFRX1sz:	.byte	1			;payload size of RX pipe 1
; NRFRX2sz:	.byte	1			;payload size of RX pipe 2
; NRFRX3sz:	.byte	1			;payload size of RX pipe 3
; NRFRX4sz:	.byte	1			;payload size of RX pipe 4
; NRFRX5sz:	.byte	1			;payload size of RX pipe 5
; NRFrxbuff:	.byte	NRFBUFFSZ	;reserve NRFBUFFSZ bytes for RX buffer
; NRFtxbuff:	.byte	NRFBUFFSZ	;reserve NRFBUFFSZ bytes for TX buffer
;.endif
;
;=============================================================================
.if DDS
DDSbcdL:	.byte	1			;packed BCD of desired DDS frequency
DDSbcdM:	.byte	1
DDSbcdH:	.byte	1

DDSaddrL:	.byte	1			;setup DDS accumulator adder low-byte
DDSaddrM:	.byte	1			;setup DDS accumulator adder mid-byte
DDSaddrH:	.byte	1			;setup DDS accumulator adder high-byte

DDSacch:	.byte	1			;DDS accumulator high-byte (ZL)
.endif
;
VARend:		;end of data storage
;=============================================================================
; USB Serial I/O support
;=============================================================================
USBstart:		;start of USB RAM usage
#if (USB)
.equ	USB_SERIAL_DTR		= 0			;bit 0
.equ	USB_SERIAL_RTS		= 1			;bit 1
.org 	USB_SERIAL_RAM		;start of USB serial I/O RAM usage
.org	cdc_line_coding
							.byte	8	;.data:0000
.org	usb_configuration
							.byte	1	;.bss
.org	cdc_line_rtsdtr
							.byte	1	;.bss
.org	transmit_flush_timer
							.byte	1	;.bss
.org	transmit_previous_timeout
							.byte	1	;.bss
.org	usb_suspended
							.byte	1	;.bss
.org	USB_SERIAL_RAM + 13
usb_irqstate:				.byte	1	;don't remove this, for internal use!
usb_openforuse:				.byte	1	;don't remove this, for internal use!
#endif
USBend:			;end of USB RAM usage
;
;=============================================================================
	.org	RAMEND-MCUSTKSZ+1		;reserve STACK space
MCUSTK:			.byte	MCUSTKSZ	;uP RETURN stack
;=============================================================================

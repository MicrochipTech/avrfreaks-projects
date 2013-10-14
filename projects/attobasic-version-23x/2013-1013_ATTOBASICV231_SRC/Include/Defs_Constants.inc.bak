;******************************************************************
; NOTE: Mega88 support is inherant but many routines are 
;		disabled below in order to fit into the 8K Program Space.  
;		Disable other features if any disabled features are needed.
;******************************************************************
;	Select your controller type here.  Don't forget to set the 
;	appropriate fuse bits.
;
.include "m2560def.inc"		;256K FLASH, 8K DATA, 4K E2P
;.include "usb1286def.inc"	;128K FLASH, 8K DATA, 4K E2P, USB
;.include "m32u4def.inc"		;32K FLASH, 2.5K DATA, 1K E2P, USB 
;.include "m328def.inc"		;32K FLASH, 2K DATA, 1K E2P
;.include "m168def.inc"		;16K FLASH, 1K DATA, 512 E2P
;.include "m168padef.inc"	;16K FLASH, 1K DATA, 512 E2P
;.include "m88def.inc"		;8K FLASH, 1K DATA, 512 E2P
;.include "m88padef.inc"	;8K FLASH, 1K DATA, 512 E2P
;.include "tn85def.inc"		;8K FLASH, 512 DATA, 512 E2P
;
#define BTLDR	1			;"1" to include boot loader code.
#define USB		1			;"1" to include USB support code (also 
							; disables the UART).  Only for M32U4
							; and USB1286!
#define TEENSY	0			;set to "1" to enable TEENSY++ 2.0 build
#define	DREC	0			;set to "1" to enable AVR Data Recorder specific code 
#define	TEST	0			;set to "1" to enable code for testing
;******************************************************************
;!!! Minimum clock frequency for 400K TWI operation is 6.4MHz !!!

#define	FCLK_PS  1		;System clock prescaler
;
#ifndef FCLK
; #define FCLK	4000000
;!!! Minimum clock frequency for 400K TWI operation is 6.4MHz !!!
#endif
#ifndef FCLK
; #define FCLK	(8000000/FCLK_PS)	//set Fclk (preprocessor variable)
#endif
#ifndef FCLK
; #define FCLK	(10000000/FCLK_PS)
#endif
#ifndef FCLK
; #define FCLK	(12000000/FCLK_PS)
#endif
#ifndef FCLK
 #define FCLK	(16000000/FCLK_PS)
#endif
#ifndef FCLK
 #define FCLK	(20000000/FCLK_PS)
#endif
;
;+++ Break Here +++ Do not move or remove this line, used for automated processing
;
;------------------------------------------------------------------
.equ    Version = 231    ;version 2.31 (100x so its an integer)
;------------------------------------------------------------------
.include "Include\Defs_Register.inc"	;registers defined
.include "Include\Defs_Pinout.inc"		;pinouts defined
.include "Include\Defs_Timers.inc"		;Timer specific dividers defined
;
;~~~~~ Code enabling feature ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; Code enabling feature.  Select "1" to enable code generation, 
;	"0" to disable.  Note: the size difference is processor dependant.
.set	DATAREC	= 0		;DON'T MODIFY THIS! (needed to process correct bootloader)
.set	DEBUG	= 1		;"1" to enable [EV]DUMP routines (+562 bytes)
.set	DDS		= 1		;"1" to enable DDS routines (+738 bytes)
.set	DSI		= 1		;"1" to enable DSI routines (+274 bytes)
.set	PWM		= 1		;"1" to enable PWM routines (+286 bytes)
.set	SPI		= 1		;"1" to enable SPI routines (+368 bytes)
.set	DATA	= 1		;"1" to enable DataFile routines (+502 bytes)
.set	TWI		= 1		;"1" to enable TWI routines (+518 bytes)
.set	OWR		= 1		;"1" to enable 1-Wire routines (+302 bytes)
.set	ICP		= 1		;"1" to enable ICP routines (+316 bytes)
.set	EFS		= 1		;"1" to enable E2P File System routines (+858 bytes)
.set	LPMD	= 1		;"1" to enable Low-Power routines (+162 bytes)
.set	DHT		= 1		;"1" to enable DHTxx routines (+962 bytes)
.set	RTC		= 1		;"1" to enable Real-time Counter routines (+550 bytes)
.set	NRF		= 1		;"1" to enable nRF24L01 routines (+1252 bytes)
.set	ADConv	= 1		;"1" to enable ADConv routines (+128 bytes)
.set	INTREG	= 1		;"1" to enable internal register routines (+82 bytes)
.set	CRC16R	= 0		;"1" to enable internal CCITT CRC16 routines (+62 bytes)
.set	HELP	= 1		;"1" to enable HELP command (+146 bytes)
.set	RENUMBR	= 0		;"1" to enable the Renumber routine (future release)
.set	BIGRAM	= 1		;"1" to allow more program RAM (+0 bytes) 
.set	SStst	= 1		;"1" to enable self-start test at RESET (+40 bytes)
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;
.set	f_clk	= FCLK	;set CPU clock frequency
.set	TESTING	= 0;
#if TEST
  #warning !! Test code enabled !!
  .set		TESTING	= 	1			;used for preprocessor and ASM compat.
#endif
;
;~~~~~~~ MCU Specific Restrictions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#if ( defined(__ATmega88__) || \
	  defined(__ATmega88PA__) )
.set	DATAREC	= 0		;disable DATARECORDER routines for Mega88
.set	DEBUG	= 0		;disable DEBUG routines for Mega88
.set	DSI		= 0		;disable DSI routines for Mega88
.set	DDS		= 0		;disable DDS routines for Mega88
.set	TWI		= 0		;disable TWI routines for Mega88
.set	ICP		= 0		;disable ICP routines for Mega88
.set	RTC		= 0		;disable RTC routines for Mega88
.set	DHT		= 0		;disable DHT routines for Mega88
.set	EFS		= 0		;disable E2P routines for Mega88
.set	NRF		= 0		;disable nRF24L01 routines for Mega88
.set	LPMD	= 0		;disable Low-Power routines 
.set	DATA	= 0		;disable DataFile routines for Mega88
.set	INTREG	= 0		;disable internal register routines for Mega88
.set	CRC16R	= 0		;disable internal CRC16 routines for Mega88
.set	RENUMBR	= 0		;disable Renumber routine for Mega88
#undef	DREC
#define	DREC	  0
;
#elif ( defined(__ATmega168__) || \
	  defined(__ATmega168PA__) )
.set	DATAREC	= 0		;disable Data Recorder code generation
#undef	DREC
#define	DREC	  0
.set	DSI		= 0		;disable DSI routines for Mega168
;
#elif defined(__AT90USB1286__) | \
	( defined(__ATmega32U4__) | \
	  defined(__ATmega2560__) )
.set	DATAREC	= 0		;disable Data Recorder code generation
#undef	DREC
#define	DREC	  0
#endif
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;
#if DREC	;enable/disable AVR Data Recorder features
 #undef FCLK
 #define FCLK	  8000000	;set Fclk (preprocessor variable)
 #define BTLDR	  1			;"1" to include boot loader code.
 .set	f_clk	= FCLK	;set CPU clock frequency
 .set	DATAREC	= 1			;needed to process correct bootloader
 .set	DEBUG	= 1			;disable [EV]DUMP routines
 .set	DSI		= 0			;disable DSI routines
 .set	TWI		= 0			;disable routines
 .set	ICP		= 0			;disable ICP routines
 .set	DDS		= 1			;disable DDS routines
 .set	PWM		= 1			;enable PWM routines
 .set	SPI		= 1			;enable SPI routines
 .set	DATA	= 1			;enable DataFile routines
 .set	DHT		= 1			;disable DataFile routines
 .set	LPMD	= 1			;enable Low-Power routines
 .set	RTC		= 1			;enable Real-time Counter routines
 .set	ADConv	= 1			;enable ADConv routines
 .set	BIGRAM	= 0			;program RAM must fit in EEPROM
 .set	SStst	= 1			;enable self-start test at RESET
;
 .equ	ADCGAIN	= PORTD		;Port D is gain select
 .equ	ADC0_R0	= PORTD2	;PD2 - Channel 0 gain bits 
 .equ	ADC0_R1	= PORTD3	;PD3
 .equ	ADC1_R0	= PORTD4	;PD4 - Channel 1 gain bits 
 .equ	ADC1_R1	= PORTD5	;PD5 
;
 .equ	AD536IO	= PORTC		;Port C is AD536 Output select
 .equ	AD536CH	= PORTC4	;AD536 Channel select bit
 .equ	AD536OS	= PORTC5	;AD536 output select bit

 .equ	DIGPORT	= PORTB		;digital out port
 .equ	DIGOUT	= PORTB0	;digital output pin
 .equ	DIGALF	= 'A'+(DIGPORT/3);alphanumeric character of port

 .if DHT
 .equ	DHTPIN	= PINB6		;Pin for DHT Data (use PCINT's)
 .equ	DHPORT	= PORTB		;Port for DHT data pin
 .endif
#endif
;
;* Debug Messages ***********************************************
;#if ((FCLK_PS != 1) && (FCLK_PS != 2) && (FCLK_PS != 4) && (FCLK_PS != 8) && (FCLK_PS != 16) && (FCLK_PS != 32) && (FCLK_PS != 64) && (FCLK_PS != 128) && (FCLK_PS != 256))
#if ((256 % FCLK_PS ) != 0)
  #error "!! Invalid System Clock Prescaler Value !!"
#else
  #message "System clock prescaler set to " FCLK_PS 
#endif

#if  FCLK == 1000000
 .message "Using FCLK @ 1MHz"
#elif  FCLK == 2000000
 .message "Using FCLK @ 2MHz"
#elif  FCLK == 4000000
 .message "Using FCLK @ 4MHz"
#elif  FCLK == 5000000
 .message "Using FCLK @ 5MHz"
#elif  FCLK == 6000000
 .message "Using FCLK @ 6MHz"
#elif  FCLK == 8000000
 .message "Using FCLK @ 8MHz"
#elif  FCLK == 10000000
 .message "Using FCLK @ 10MHz"
#elif  FCLK == 12000000
 .message "Using FCLK @ 12MHz"
#elif  FCLK == 12000000
 .message "Using FCLK @ 12MHz"
#elif  FCLK == 16000000
 .message "Using FCLK @ 16MHz"
#elif  FCLK == 20000000
 .message "Using FCLK @ 20MHz"
#else
 .message "No FCLK selected!"
#endif
;
;****************************************************************
.equ	PROMPT		= '>'		;here's our prompt character
.equ	LNLEN		= 54		;max line length for system msgs
;
.equ	CRLFCHAR	= '~'		;CR/LF character used by the PRINT command
.equ	LF			= 0x0a		;Line Feed character
.equ	CR			= 0x0d		;Carriage Return character
.equ	CTRL_C		= 0x03		;^C for halt
.equ	CTRL_S		= 0x13		;^S for suspend
;
; CRC-8 Polynomial Defs
.equ	CRCBufSz	= 32			;just the 1st 32 bytes
.equ	CRC8Init    = 0x00
.equ	CRC8Poly 	= 0x18		;CRC-8 Polynomial, 0x18 = (X^8 + X^5 + X^4 + 1)
;
;.equ	CRC16Poly 	= 0x8005	;CRC-16 Polynomial. (X^16 + X^15 + X^2 + 1)
;.equ	CRC16Init 	= 0x0000	;CRC-16 initial value, 0x0000
;.equ	CRC16Poly 	= 0x8408	;CRC-XMODEM Polynomial, (X^16 + X^15 + X^10 + X^3)
;.equ	CRC16Init 	= 0x0000	;CRC-16 initial value, 0x0000
.equ	CRC16Poly 	= 0x1021	;CRC-16 CCITT Polynomial, (X^16 + X^12 + X^5 + 1)
.equ	CRC16Init 	= 0xFFFF	;CRC-16 initial value, 0xFFFF
;
.equ	RNDPoly		= 0xB4		;polynomial to seed for RND command
;
.equ	DUMPBYTES	= 16		;# of bytes/line for "dump" command
;
.equ	EOP			= 0			;End of Program token
;
; RENUMBER command setup
.equ	STLNNO		= 5			;default starting line number
.equ	LNINC		= 10			;default line increment
;
;**** Character Cast Type Definitions ***************************
.equ	QC_OPER		= 0		;Operator (not one of the other casts
.equ	QC_DELIM	= 1		;Delimiter -space ($20) or comma ($2C)
.equ	QC_NUM		= 2		;Numeral 0..9 ($30 through $39)
.equ	QC_LTR		= 3		;Letter [A..Z] uppercase ($41 throught $5A)
.equ	QC_CR		= 4		;Carriage return
.equ	QC_QUOT		= 5		;quote character (string support) 
							;  or "@" for a constant
;****************************************************************
; TEENSY++ 2.0 support
;****************************************************************
#if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) || \
	defined(__ATmega2560__) || \
	defined(__ATmega32U4__) ) \
	&& TEENSY
 #undef		TEENSY
 #define	TEENSY	0			;insure TEENSY 2.0 ++ support code
 								; disabled on M88/168/328/M32u4 
#elif defined(__AT90USB1286__) \
	&& TEENSY
 #undef		USB
 #define	USB		1			;include USB serial I/O support code 

 #undef		BTLDR
 #define	BTLDR	0			;disable bootloader generation
 								; for TEENSY++ 2.0 loader
 #undef		FCLK
 #define	FCLK	(16000000/FCLK_PS)	;default TEENSY clock

 #message "Disabling BootLoader Support on TEENSY++ 2.0"
 #message "Enabling USB Serial I/O Support on TEENSY++ 2.0"
#endif
;
;****************************************************************
; USB support
;****************************************************************
#if (USB)
  .equ	USBConTmr	= 4		;USB connection timeout (N * 8 sec)
  .set	USBSUPP		= 1		;for AVRASM2 conditionals usage
  #if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) || \
	defined(__ATmega2560__) ) && USB
   #undef	USB
   #define	USB		0		;disable USB support code on M88/168/328 
  .set 	USBSUPP		= 0		;for AVRASM2 conditionals usage
  #endif

;!!!  DO NOT MODIFY THESE VALUES !!!
  #if defined(__ATmega32U4__) 
  .equ	USB_SERIAL_RAM	= 0x0AA0	;start of RAM variables
  #elif defined(__AT90USB1286__)
  .equ	USB_SERIAL_RAM	= 0x20A0	;start of RAM variables
  #endif
#else
  .set 	USBSUPP		= 0		;for AVRASM2 conditionals usage
#endif
;
#if ( (USB) && ( (FCLK != 8000000) && (FCLK != 16000000) ) )
  #error "USB support is enabled but FCLK is not 8MHz or 16MHz!"
#endif
;
;****************************************************************
; Boot-loader support
;****************************************************************
#if BTLDR
.equ	BLDMJKEY		= 0xE5		;majick byte to invoke bootloader
									; at system reset
#endif
#if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) ) && BTLDR;no bootloader support for M88
#undef	BTLDR
#define	BTLDR	0
#endif
;
; Fusebyte Constants (for bootloader support)
#if defined(__ATmega168__) || \
	defined(__ATmega168PA__)
.equ	FUSEBYTE		= 0x02		;Ext Fuse for M168 (0x02)
#elif ( defined(__ATmega328__) || \
	defined(__ATmega328PA__) || \
	defined(__ATmega32U4__) || \
	defined(__ATmega2560__) || \
	defined(__AT90USB1286__) )
.equ	FUSEBYTE		= 0x03		;High Fuse for M328/32U4,M2560
									; and USB1286 (0x03)
#endif
;
;****************************************************************
;				MEMORY ALLOCATION
; !!Refer to file "Include\Data_RAM.inc" for memory setup!!
;
;****************************************************************
 .set	MCUSTKSZ	= 80			;reserve space in RAM for stack growth
.ifdef RAMPZ					;need RAMPZ to access >64KW FLASH
  .set	MCUSTKSZ	= (MCUSTKSZ*2)	;reserve space in RAM for stack growth
.endif
;
;define flag bits in GPIOR0
.equ	STRGDET	= 0					;Flag0 = 1 = String detected
.equ	IGNCHAR	= 1					;Flag1 = 1 = ignore characters (parser)
.equ	NRFIRQ	= 2					;Flag2 = 1 = NRF generated interrupt
.equ	ACOIRQ	= 3					;Flag3 = 1 = ACO generated interrupt
.equ	USBIRQ	= 4					;Flag4 = 1 = USB generated interrupt
.equ	WDTIRQ	= 5					;Flag5 = 1 = WDT generated interrupt
.equ	DDSIRQ	= 6					;Flag6 = 1 = DDS generated interrupt
.equ	RTCIRQ	= 7					;Flag7 = 1 = RTC generated interrupt
;
;+++++++++++++++++++++++++++++++++++++
;define flag bits in GPIOR1
.if GPIOR1
 .equ	ACOCh0	= 0					;Flag[1:0] = ADC Channel for Analog Comp.
 .equ	ACOCh1	= 1					; 0=ADC0, 1=ADC1, 2=AIN-
 .equ	DHTUnit	= 2					;Flag2 = 0=DHT in Farenheit, 1=DHT in Celcius
 .equ	DHTBusy	= 3					;Flag3 = 0=DHT ready to read, 1=DHT Busy (1.5 sec between reads)
 .equ	DHTSign	= 4					;Flag4 = 0=ignore sign, 1=allow sign
 .equ	BIO_RW	= 5					;Flag5 = 0=read address, 1=write addressed (for PEEK,POKE,EER,EEW)
 .equ	EEP_WE	= 6					;Flag6 = 0=EEP write, 1=EEP erase
 .equ	AOVErr	= 7					;Flag7 = 0 = disable arithmetic 
									; overflow/underflow errors
.else
  .error "This MCU does not support the GPIOR1 register"
.endif
;
;+++++++++++++++++++++++++++++++++++++
;define flag bits in GPIOR2
.if GPIOR2
; .equ	NRF_EN	= 0				;Flag0 = 0=NRF disabled, 1=NRF enabled,
;.equ			= 1				;Flag1 = 
;.equ			= 2				;Flag2 = 
;.equ			= 3				;Flag3 = 
;.equ			= 4				;Flag4 = 
;.equ			= 5				;Flag5 = 
;.equ			= 6				;Flag6 = 
;.equ			= 7				;Flag7 = 
;
.else
  .error "This MCU does not support the GPIOR2 register"
.endif
;
;+++++++++++++++++++++++++++++++++++++
;define general purpose bit positions
.equ	Bit0	= 0
.equ	Bit1	= 1
.equ	Bit2	= 2
.equ	Bit3	= 3
.equ	Bit4	= 4
.equ	Bit5	= 5
.equ	Bit6	= 6
.equ	Bit7	= 7
;
;****************************************************************
;	Watchdog timer
;****************************************************************
.equ	WDMAX	= 9					;maximum prescaler value
;
;****************************************************************
;	EEPROM storage
;****************************************************************
;.equ	eeprogstortop	= LOW(E2END)		;First (top) byte in EEPROM
;.equ	eepromstortopage= HIGH(E2END)		;MSB of highest address in EEPROM 
;
.set	EFSHNDSZ	= 1					;for RAM calculations
.set 	EFSBLKSZ	= 0
.if EFS
  .set	EFSBLKSZ	= 32
  .set	EFSHNDSZ	= 1					;default 1 byte per handle
  .set	HANDLES_MAX	= ((E2END+1)/256)	;1 file handle per 256 
 											; bytes of EEPROM.
  .if (HANDLES_MAX > 8)
    .set	HANDLES_MAX	= 8					;8 handles max 
  .endif
  .equ	EFSIDXSZ	= EFSHNDSZ * 8		;reserve Index header
  .equ	EFSERAREC	= 0xFF				;Unused Record Type
  .equ	EFSDATREC	= 0x01 				;Data Record Type
  .equ	EFSPAYSZ	= EFSBLKSZ - 2		;size of data payload
  .equ	EFSMAXBLK	= ((E2END+1)/EFSBLKSZ);max # of blocks available
.endif

;****************************************************************
;	PWM Constants
;****************************************************************
.if PWM
	.include "Include\Defs_PWM.inc"			;include PWM timer defs
.endif
;
;****************************************************************
;	Self-Start Constants
;****************************************************************
#if defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__)
  .if SStst
    .if DATAREC	;for AVR Data Recorder
	  .equ	SS_PORT		= PINB		;self-start port
	  .equ	SS_PIN		= 7			;self-start pin
	.else
	  .equ	SS_PORT		= PINC		;self-start port
	  .equ	SS_PIN		= 3			;self-start pin
  	.endif
  .endif
#elif defined(__ATmega32U4__) || \
	defined(__ATmega2560__) || \
	defined(__AT90USB1286__)
  .if SStst
	.equ	SS_PORT		= PIND		;self-start port
	.equ	SS_PIN		= 7			;self-start pin
  .endif
#endif
;
;****************************************************************
;	 DS port pins
;****************************************************************
 #if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) )
  .if DSI
   .equ	DS_PORT			= PORTD		;DS on this port
   .equ	DS_SDA			= PORTD4	;port pin for ATTENTION
   .equ	DS_SDD			= PORTD3	;port pin for DATA
  .endif
 #elif defined(__ATmega32U4__)
  .if DSI
   .equ	DS_PORT			= PORTC		;DS on this port
   .equ	DS_SDA			= PORTC7	;port pin for ATTENTION
   .equ	DS_SDD			= PORTC6	;port pin for DATA
  .endif
 #elif 	defined(__AT90USB1286__) || \
 	defined(__ATmega2560__)
  .if DSI
   .equ	DS_PORT			= PORTC		;DS on this port
   .equ	DS_SDA			= PORTC5	;port pin for ATTENTION
   .equ	DS_SDD			= PORTC4	;port pin for DATA
  .endif
#endif
;
;**** Leave the following alone ********************************************
.if DSI
 .equ	DS_PUPS			= 1			;"1" to enable port pin pullups
 .equ	settletime		= $50		;Time for sampling, setting of data lines
 .equ	attline			= 4			;B4	Attention line
 .equ	dataline		= 5			;B5	Data line
.endif
;
;****************************************************************
; DHT Port Pins and such
;****************************************************************
#if !DREC	
 #if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) )
 .if DHT
  .equ	DHTPIN	= PIND7			;Pin for DHT Data (use PCINT's)
  .equ	DHPORT	= PORTD			;Port for DHT data pin
 .endif
 #elif defined(__ATmega32U4__) || \
	defined(__AT90USB1286__) || \
	defined(__ATmega2560__)
 .if DHT
  .equ	DHTPIN	= PINB2				;Pin for DHT Data (use PCINT's)
  .equ	DHPORT	= PORTB				;Port for DHT data pin
 .endif
 #endif
#endif
;
.if DHT
  .set	RTC		= 1					;insure Real-time Counter
  .include "Include/Defs_DHT.inc"	;include DHT definitions
.endif
;
;****************************************************************
; One-Wire Port Pins and such
;****************************************************************
.if OWR
  .include "Include/Defs_OWR.inc"	;include OWR definitions
.endif
;
;****************************************************************
; nRF24L01(+) Support
;****************************************************************
.equ	NRF24_MAX_MSG_LEN	= 32	;maximum message length of nRF24

.set	NRFBUFFSZ	= 0
.if NRF
  .set	SPI			= 1				;insure SPI routines
;  .equ	NRFARDLY	= 8				;10mS delay to allow for auto-tx 
  .equ	NRFARDLY	= 15			;10mS delay to allow for auto-tx 
  .include "Include/Defs_NRF.inc"	;include NRF definitions
.endif
;
;****************************************************************
;	SPI port pins
;****************************************************************
.if DATA
.set	SPI		= 1		;need SPI routines so enable them
.endif
#if defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__)
.if (SPI || DATA)
.equ	SPI_PORT		= PORTB		;SPI on this port
.equ	SPI_SS			= PORTB2	;port pin for SPI Slave select
.equ	SPI_SCK			= PORTB5	;port pin for SCK
.equ	SPI_MOSI		= PORTB3	;port pin for MOSI
.equ	SPI_MISO		= PORTB4	;port pin for MISO
.endif
#elif defined(__ATmega32U4__) || \
	defined(__ATmega2560__) || \
	defined(__AT90USB1286__)
.if (SPI || DATA)
.equ	SPI_PORT		= PORTB		;SPI on this port
.equ	SPI_SS			= PORTB0	;port pin for SPI Slave select
.equ	SPI_SCK			= PORTB1	;port pin for SCK
.equ	SPI_MOSI		= PORTB2	;port pin for MOSI
.equ	SPI_MISO		= PORTB3	;port pin for MISO
.endif
#endif
;
;****************************************************************
; RTC Support
;****************************************************************
.if RTC
  .include "Include/Defs_RTC.inc"			;include DEF's
.endif;
;
;****************************************************************
; DDS Constants
;****************************************************************
.if DDS
.include "Include\Defs_DDS.inc"		;include DDS defs
.endif
;

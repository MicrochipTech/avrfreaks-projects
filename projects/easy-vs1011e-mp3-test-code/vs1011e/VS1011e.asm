;VS1011e test
;please read the file "readthis.txt"
;this program will initialise the vs1011e, play the 
;MP3 file from flash and loop every 2 seconds.
;
.nolist
.include "m8515def.inc"
.list
;
;for atmega8515, program fuses for internal 8 MHz clock
;fuse low = 0xC4
;fuse high = 0xDF
;
;for long delays
.def	dcount  =r4
.def	dcount2 =r5
.def	dcount3 =r6
.def	dcount4 =r7		;for One second delay
;
.def	temp	= r16
;
;PORTB
.equ	DREQ	= 1	;PB1 (input) connect to DREQ on sparkfun board
.equ	xDCS  = 2	;PB2 connect to BSYNC on sparkfun board
.equ	XRESET= 3	;PB3 connect to RST on sparkfun board
.equ	xCS   = 4	;PB4 connect to CS on sparkfun board
.equ	MOSI	= 5	;PB5 connect to SI on sparkfun board
.equ	MISO	= 6	;PB6 (input) connect to SO on sparkfun board
.equ	SCK	= 7	;PB7 connect to SCLK on sparkfun board
;
;other connections needed to Sparkfun board:
;GND, 5V .
;
;connect Left,right and GBUF(gnd) to a stereo audio jack
;form there you can plug in an amplified speaker (PC speaker works good)
;or some headphones
;
.equ	SPI_DDR	= DDRB
.equ	SPI_PORT	= PORTB
.equ	SPI_PIN	= PINB
;
; VS1011 SCI Register
.equ SCI_MODE		= 0x00
.equ SCI_STATUS		= 0x01
.equ SCI_BASS		= 0x02
.equ SCI_CLOCKF		= 0x03
.equ SCI_DECODE_TIME	= 0x04
.equ SCI_AUDATA		= 0x05
.equ SCI_WRAM		= 0x06
.equ SCI_WRAMADDR		= 0x07
.equ SCI_HDAT0		= 0x08
.equ SCI_HDAT1		= 0x09
.equ SCI_AIADDR		= 0x0A
.equ SCI_VOL		= 0x0B
.equ SCI_AICTRL0		= 0x0C
.equ SCI_AICTRL1		= 0x0D
.equ SCI_AICTRL2		= 0x0E
.equ SCI_AICTRL3		= 0x0F
;
.equ read_cmd	= 0x03
.equ write_cmd	= 0x02
;-----------------------------------------------------------
.cseg
.org	0
	rjmp   Start	;
;-----------------------------------------------------------
Start:
	ldi	temp,LOW(RAMEND)  ;
	out	SPL, temp		;
	ldi	temp,HIGH(RAMEND) ;
	out	SPH, temp		;	

	in	temp,SPI_DDR
	sbr	temp,(1<<XRESET)|(1<<xDCS)|(1<<xCS)|(1<<MOSI)|(1<<SCK)	
	out	SPI_DDR,temp

	in	temp,SPI_PORT
	sbr	temp,(1<<XRESET)|(1<<xDCS)|(1<<xCS)|(1<<DREQ)|(1<<MISO)
	out	SPI_PORT,temp

	ldi 	temp,(1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0); Enable SPI, Master, set clock rate fck/128
	out 	SPCR,temp		;initialize Hardware SPI

	rcall	vs_init		;reset & initialize vs1011e		
;-----------------------------------------------------------
WriteMP3:
	ldi	ZH,HIGH(MP3_Data*2)
	ldi	ZL, LOW(MP3_Data*2)

WriteMP3Lp:
	cpi	ZL, LOW(MP3_Data_End*2)
	brne	WaitDreqReady	;compare current address to last address
	cpi	ZH,HIGH(MP3_Data_End*2)
	brne	WaitDreqReady	;of MP3 file
	rjmp	EndOfFile		;file end

WaitDreqReady:
	sbic	SPI_PIN,DREQ
	rjmp	SendMP3		;start sending MP3 if DREQ high

	sbi	SPI_PORT,XDCS	;
	rjmp	WaitDreqReady	;wait until VS10xx is ready for more data

SendMP3:
	cbi	SPI_PORT,XDCS	;start to send MP3 file			

SendMP3_1:
	lpm				;
	mov	temp, r0		;get byte
	adiw	ZL, 1			;next byte
	rcall	spi_write		;
	rjmp	WriteMP3Lp		;continue until end of file

EndOfFile:				;End of file - send 2048 zeros
	ldi	YH,0x08		;0800 = send 2048 zeros
	ldi	YL,0x00		;

SendNullLoop:
	rcall	WaitBusy		;wait until VS10xx ready
	ldi	temp,0		;
	rcall	spi_write		;
	sbiw	YL,1			;
	brne	SendNullLoop	;loop until all 2048 null bytes are sent

SendNullEnd:
	sbi	SPI_PORT,XDCS	;
	rcall	OneSecDelay		;delay 2 seconds before 
	rcall	OneSecDelay		;doing it all over again
	rjmp	WriteMP3		;
;-----------------------------------------------------------
;reset the vs1011e
vs_init:
	cbi	SPI_PORT,XRESET	;Hardware Reset VS10xx

	ldi	temp,10		;10mS delay
	mov	dcount,temp		;
	rcall	Delay			;

	sbi	SPI_PORT,XRESET	;reset high
;-----------------------------;reset done
	cbi	SPI_PORT,XCS	;
	ldi	temp,write_cmd	;
	rcall	spi_write		;

	ldi	temp,SCI_VOL	;set volume
	rcall	spi_write		;

	ldi	temp,0xFF		;
	rcall	spi_write		;
	ldi	temp,0xFF		;
	rcall	spi_write		;
	sbi	SPI_PORT,XCS	;
;-----------------------------;set volume done
	cbi	SPI_PORT,XCS	;
	ldi	temp,write_cmd	;
	rcall	spi_write		;

	ldi	temp,SCI_AUDATA	; Slow sample rate for slow analog part startup 
	rcall	spi_write		;

	ldi	temp,0		;
	rcall	spi_write		;
	ldi	temp,0x0A		;10 Hz
	rcall	spi_write		;

	sbi	SPI_PORT,XCS	;

	ldi	temp,100		;100mS delay
	mov	dcount,temp		;
	rcall	Delay			;
;-----------------------------;set AUDATA done
	cbi	SPI_PORT,XCS	;

	ldi	temp,write_cmd	;
	rcall	spi_write		;

	ldi	temp,SCI_VOL	;set volume
	rcall	spi_write		;

	ldi	temp,0xFE		;
	rcall	spi_write		;
	ldi	temp,0xFE		;
	rcall	spi_write		;

	sbi	SPI_PORT,XCS	;
;-----------------------------;set volume done
	cbi	SPI_PORT,XCS	;
	ldi	temp,write_cmd	;
	rcall	spi_write		;

	ldi	temp,SCI_AUDATA	;Switch on the analog parts
	rcall	spi_write		;

	ldi	temp,0x1F		;
	rcall	spi_write		;
	ldi	temp,0x40		; 8 KHz
	rcall	spi_write		;

	sbi	SPI_PORT,XCS	;
;-----------------------------;Switch on the analog parts done
	cbi	SPI_PORT,XCS	;
	ldi	temp,write_cmd	;
	rcall	spi_write		;

	ldi	temp,SCI_VOL	;Set initial volume (20 = -10dB)
	rcall	spi_write		;

	ldi	temp,0x14		;
	rcall	spi_write		;
	ldi	temp,0x14		;
	rcall	spi_write		;

	sbi	SPI_PORT,XCS	;
;-----------------------------------------------------
	ldi	temp,1		;1mS delay
	mov	dcount,temp		;
	rcall	Delay			;

	cbi	SPI_PORT,XCS	;
	ldi	temp,write_cmd	;
	rcall	spi_write		;

	ldi	temp,SCI_MODE	;
	rcall	spi_write		;

	ldi	temp,0x08		;Newmode, Soft Reset, No Layer 1,2
	rcall	spi_write		;
	ldi	temp,0x04		;
	rcall	spi_write		;

	sbi	SPI_PORT,XCS	;

	ldi	temp,1		;1mS delay
	mov	dcount,temp		;
	rcall	Delay			;

	rcall	WaitBusy		;wait until VS10xx ready

	cbi	SPI_PORT,XCS	;
	ldi	temp,write_cmd	;
	rcall	spi_write		;

	ldi	temp,SCI_CLOCKF	;Set clock register, doubler etc.
	rcall	spi_write		;

	ldi	temp,0x98		;0x9800 for vs1011
	rcall	spi_write		;using sparkfun board
	ldi	temp,0		;
	rcall	spi_write		;

	sbi	SPI_PORT,XCS	;

	ldi	temp,1		;1mS delay
	mov	dcount,temp		;
	rcall	Delay			;

	rcall	WaitBusy		;wait until VS10xx ready
	ret				;
;-----------------------------------------------------------
WaitBusy:
	sbis	SPI_PIN,DREQ	;wait until VS10xx ready
	rjmp	WaitBusy
	ret
;-----------------------------------------------------------
spi_write:
	out	SPDR, temp		;
spi_wait:
	sbis	SPSR, SPIF		;
	rjmp	spi_wait		;

	in	temp, SPDR		;read back byte
	ret				;
;-----------------------------------------------------------
.include "delay.inc"
;-----------------------------------------------------------
;
MP3_Data:
;
.include "hi.inc"			;this one is a womans voice saying "hi"			
;.include "mp3.inc"				
;					
MP3_Data_End:
;-----------------------------------------------------------
.exit
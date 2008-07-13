 //Program to read SD card and display contents via UART
 
 .include "m48def.inc"
 
 
 .def	Temp	=r16		;temporary register
 
 .def	Temp2	=r23
 .def	Temp3	=r24   //should be used locally only

 
;***** Initialization;
	.DSEG
	.org  0x0100
ShortFileName:		.BYTE	33
Partition1LBA:		.BYTE	4
Partition1ByteAddress:	.BYTE	4
ClusterBeginLBA:	.BYTE	4
LBA2BYTE:			.BYTE	4
BytesPerSector:		.BYTE	2
SectorsPerCluster:	.BYTE	1
ReservedSectors:	.BYTE	2
NumberOfFats:		.BYTE	1
SectorsPerFat:		.BYTE	4

Attribute:			.BYTE	1
ReadBuffer:			.BYTE	64
FatStartByteAddress: .BYTE	4

FATClusterNo:		.BYTE	4
ByteCount:			.BYTE	1
	.CSEG
.org $0000
; Reset- and Interrupt-vectors
;
;**** Interrupt Vectors ****	
rjmp Start	
rjmp	AINT0addr     ;01	External Interrupt0
rjmp	AINT1addr     ;02	external Interrupt1
rjmp	APCI0addr   ;03	Pin Change Interrupt0
rjmp	APCI1addr   ;04	Pin Change Interrupt1
rjmp	APCI2addr   ;05	Pin Change Interrupt2
rjmp	AWDTaddr	     ;06	Watchdog Timeout
rjmp	AOC2Aaddr     ;07	Timer/Counter2 Compare Match Interrupt
rjmp	AOC2Baddr     ;08	Timer/Counter2 Compare Match Interrupt	
rjmp	AOVF2addr     ;09	Overflow2 Interrupt
rjmp	AICP1addr     ;0a	Input Capture1 Interrupt 	
rjmp	AOC1Aaddr     ;0b	Output Compare1A Interrupt 
rjmp	AOC1Baddr     ;0c	Output Compare1B Interrupt 
rjmp	AOVF1addr     ;0d	Overflow1 Interrupt 
rjmp	AOC0Aaddr     ;0e	Timer/Counter0 Compare Match Interrupt
rjmp	AOC0Baddr     ;0f	Timer/Counter0 Compare Match Interrupt	
rjmp	AOVF0addr     ;10	Overflow0 Interrupt
rjmp	ASPIaddr      ;11	SPI Interrupt 	
rjmp	AURXCaddr     ;12	USART Receive Complete Interrupt 
rjmp	AUDREaddr     ;13	SART Data Register Empty Interrupt 
rjmp	AUTXCaddr     ;14	USART Transmit Complete Interrupt 
rjmp    AADCCaddr     ;15	ADC Conversion Complete Handle
rjmp	AERDYaddr     ;16	EEPROM write complete
rjmp	AACIaddr	     ;17	Analog Comparator Interrupt 
rjmp    ATWIaddr      ;18	TWI Interrupt Vector Address
rjmp	ASPMRaddr     ;19	Store Program Memory Ready Interrupt 


	


;
; ************** Interrupt service routines ********
;
; External Interrupt 0
;
AInt0addr:
	reti
;
; External Interrupt 1
;
AInt1addr:
	reti
;
; Timer/Counter 1, Capture event
;
APCI0addr:
reti
;
APCI1addr:



reti
;
APCI2addr:
reti
;

AWDTaddr:
	reti
;
; Watch dog timer interrupt
;
AOC2Aaddr:
	reti
;
; Timer/Counter 1, Compare match interrupt B
;
AOC2Baddr:
	reti
;
; Timer/Counter 1, Overflow interrupt
;
AOVF2addr:
	reti
;
; Timer/Counter 0, Overflow interrupt
;
AICP1addr:
	reti
;
; SPI Serial Transfer Complete interrupt
;
AOC1Aaddr:

	reti
;
; Uart Rx Complete Interrupt
;
AOC1Baddr:
	reti
;
; Uart Data register empty interrupt
;
AOVF1addr:
	reti
;
; Uart Tx complete interrupt
;
AOC0Aaddr:
	
	reti
;
; Analog comparator interrupt
;
AOC0Baddr:
	reti
;
AOVF0addr:
	reti
	;
ASPIaddr:
reti
;
AURXCaddr:
reti

AUDREaddr:
reti

AUTXCaddr:
reti

AADCCaddr:
reti

AERDYaddr:
reti

AACIaddr:
reti
ATWIaddr:
reti
ASPMRaddr:
reti

;

; **************** End of interrupt service routines ***********



EndMessage:	.db	"Directory End"	,0	;End of Directory Message

Start:
	ldi Temp,low(RAMEND)
	out SPL,Temp
	ldi Temp,high(RAMEND)
	out	SPH,Temp	;setting stack pointer
	sei

InitializeUART:		
	
	ldi	Temp,12	; Baud Rate = 4800	
	sts UBRR0L,Temp
	ldi Temp,0
	sts	UBRR0H,Temp

	; Set frame format: 8data, 1stop bit
	ldi r16,0
ldi r16, (0<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01)
sts UCSR0C,r16	; Enable receiver and transmitter
ldi r16,0
ldi r16, (0<<RXEN0)|(1<<TXEN0)
sts UCSR0B,r16

//	rcall InitializeSPI
				

InitializeSPI:


; Enable SPI, Master, set clock rate fck/4
ldi Temp,(1<<SPE)|(1<<MSTR)|(0<<SPR0)|(0<<SPR1);|(1<<CPOL)|(1<<CPHA)
out SPCR,Temp
;setting data direction register to output and PB2 to zero forslave select


ldi Temp,(1<<DDB0)|(1<<DDB2)|(1<<DDB3)|(1<<DDB5)|(0<<DDB4)
out	DDRB,Temp


rcall sd_idle_bytes
cbi PORTB,PORTB2
rcall sd_idle_bytes
sbi PORTB,PORTB2

rcall sd_idle_bytes


cbi PORTB,PORTB2
C0:
		rcall Command0		;Initialize SD card into SPI mode

Command0Wait:
rcall ReadIdleBytes
;rcall R1interpreter
cpi		Temp, 1
brne	C0
rcall sd_idle_bytes

sbi PORTB,PORTB2
rcall sd_idle_bytes
cbi PORTB,PORTB2



rcall command8				; Command 8 for new cards
rcall ReadIdleBytes
sbrs Temp,2



rcall sd_idle_bytes
sbi PORTB,PORTB2
rcall sd_idle_bytes
cbi PORTB,PORTB2




sbi PORTB,PORTB2
rcall sd_idle_bytes
cbi PORTB,PORTB2

command58loop:			;		Command 58 Read OCR to get operating voltage range
		rcall Command58
		rcall ReadIdleBytes
		rcall RecieveR3





ACommand411:

;ldi Temp,0xA			;		linefeed
;rcall UARTTransmit
;ldi Temp,0x44			;		ACommand 41
;rcall UARTTransmit

sbi PORTB,PORTB2
rcall sd_idle_bytes
cbi PORTB,PORTB2
		
rcall Command55
rcall ReadIdleBytes
sbi PORTB,PORTB2
rcall sd_idle_bytes
cbi PORTB,PORTB2
rcall Command41
RCALL ReadIdleBytes
					;rcall R1Interpreter


cpi Temp,0x0
brne ACommand411




sbi PORTB,PORTB2
rcall sd_idle_bytes
cbi PORTB,PORTB2





TypeG:



sbi PORTB,PORTB2
rcall sd_idle_bytes
cbi PORTB,PORTB2
rcall ReadPart1Addr			;Read Master Bott Record Sector and determine the start of Part1Address


rcall LBA2ByteAddr			;convert LBA Address(FAT 32 stores addresses in LBA format) to byte address understood by SPI system


rcall ReadVolBS

rcall ReadDirectory
rjmp TypeG

;Functions used By Program


sd_write_byte:				;	Transmits a character from Temp register to SPI port
	
	out		SPDR,Temp		

wait_transmit_write:
	in	Temp,SPSR
	sbrs Temp,SPIF				; 
	rjmp	wait_transmit_write

	in		Temp,SPDR		
	ret
UARTTransmit:
	sts	UDR0, Temp	; output char to UART Data Reg
UARTTransmitLoop:
	lds Temp,UCSR0A
	sbrs Temp,UDRE0
	rjmp UARTTransmitLoop
	ret

sd_idle_bytes:		;Transmits 10 idle bytes on SPI bus	
	
	
	ldi Temp2,0xA
wait_transmit_write2:
	ldi		Temp,0xFF		
	out 	SPDR,Temp
idleloop:
in 		Temp,SPDR
	in	Temp,SPSR
	sbrs Temp,SPIF				
	rjmp	idleloop

	in		Temp,SPDR		
dec	Temp2
cpi Temp2,0
brne Wait_transmit_write2
	ret



R1Interpreter:			;Debugging function to see what bytes are being transmitted
Push Temp
push Temp2				; Writes Binary value of an 8 bit number
mov Temp2,Temp

Loop6:
sbrc Temp2,7
rcall WriteOne
sbrs Temp2,7
rcall WriteZero
sbrc Temp2,6
rcall WriteOne
sbrs Temp2,6
rcall WriteZero
sbrc Temp2,5
rcall WriteOne
sbrs Temp2,5
rcall WriteZero
sbrc Temp2,4
rcall WriteOne
sbrs Temp2,4
rcall WriteZero
sbrc Temp2,3
rcall WriteOne
sbrs Temp2,3
rcall WriteZero
sbrc Temp2,2
rcall WriteOne
sbrs Temp2,2
rcall WriteZero
sbrc Temp2,1
rcall WriteOne
sbrs Temp2,1
rcall WriteZero
sbrc Temp2,0
rcall WriteOne
sbrs Temp2,0
rcall WriteZero

pop	Temp2
pop Temp
ret

WriteOne:					;Writes an Ascii "1" via UART
ldi Temp,0x31
rcall UARTTransmit
ret
WriteZero:					;Writes an Ascii "0" via UART
ldi Temp,0x30
rcall UARTTransmit
ret
WriteNL:
push Temp					;Transmits a NewLine Character via UART
ldi Temp,0xA
rcall UARTTransmit
ldi Temp,0xD
rcall UARTTransmit
pop Temp
ret


;Different SPI Commands for SD card..Not all of commands are used in this program

Command0:
	;
	;	initializing spi interface for SD card

ldi	Temp,0
	ldi	XL,0
	ldi	XH,0
	ldi	YL,0
	ldi	YH,0
	rcall	SPISendCommand

ret


Command1:

ldi	Temp,1
	ldi	XL,0
	ldi	XH,0
	ldi	YL,0
	ldi	YH,0
	rcall	SPISendCommand

ret

Command8:



ldi	Temp,8
	ldi	XL,0
	ldi	XH,0
	ldi	YL,0
	ldi	YH,0
	rcall	SPISendCommand
ret

Command10:		;	Send CID

ldi	Temp,0xA
	ldi	XL,0
	ldi	XH,0
	ldi	YL,0
	ldi	YH,0
	rcall	SPISendCommand

ret


Command12:		;	Stop transmission

ldi	Temp,12
	ldi	XL,0
	ldi	XH,0
	ldi	YL,0
	ldi	YH,0
	rcall	SPISendCommand
	cpi Temp,0
	breq Command12

ret


Command17:
	;		command 17 
	;	Read Single Block starting at address 0

ldi	Temp,0x11
	ldi	XL,0
	ldi	XH,0
	ldi	YL,0
	ldi	YH,0
	rcall	SPISendCommand

ret

Command18:
	;		command 18 
	;	Read Multiple Block starting at address 0

ldi	Temp,0x12
	ldi	XL,0
	ldi	XH,0
	ldi	YL,0
	ldi	YH,0
	rcall	SPISendCommand

ret




Command55:		;	write data block
	;		command 55 to preceed ACMD


ldi	Temp,55
	ldi	XL,0
	ldi	XH,0
	ldi	YL,0
	ldi	YH,0
	rcall	SPISendCommand

ret



Command58:

ldi	Temp,58
	ldi	XL,0
	ldi	XH,0
	ldi	YL,0
	ldi	YH,0
	rcall	SPISendCommand

ret



Command41:		;	write data block
	;		command 41 
;	will become ACMD when preceeded by Command 55
ldi	Temp,41
	ldi	XL,0
	ldi	XH,0
	ldi	YL,0
	ldi	YH,0
	rcall	SPISendCommand
ret

ReadIdleBytes:			//	ReadIdleBytes	will return non 0xff value in Temp variable
cpi Temp,0xff
brne RIBEnd
ldi Temp,0xff
rcall sd_write_byte

rjmp ReadIdleBytes
RIBEnd:
ret 


RecieveR2:				;Recieve 2 Bytes
rcall R1Interpreter
ldi Temp,0xff
rcall sd_write_byte
;rcall R1Interpreter
ret


RecieveR3:				;Recieve 5 Bytes
;rcall R1Interpreter
ldi Temp3,4
R3Loop:
ldi Temp,0xff
rcall sd_Write_Byte
;rcall R1Interpreter
dec Temp3
cpi Temp3,0
brne R3Loop
ret

RecieveR7:				;Recieve 5 Bytes
;rcall R1Interpreter
ldi Temp2,0x4
R7Loop:
ldi Temp,0xff
rcall sd_Write_Byte
;rcall R1Interpreter
dec Temp2
cpi Temp2,0
brne R7Loop
ret

ReceiveCID:				//Recieve Card ID register 16 bytes 
ldi Temp2,0xf
RCIDLoop:
ldi Temp,0xff
rcall sd_write_byte
rcall UARTTransmit
dec Temp2
cpi temp2,0
brne RCIDLoop
ret


;-----------------------------------------------------------------
;
; SPISendCommand
; Send command. Command is in rPARAM1, Parameter is in X (low) and Y (high).
; CRC is a constant 0x95 - this is the CRC for the initial CMD0 which gets us
; into SPI mode, from then on the CRC is ignored.

SPISendCommand:
	sbi	PORTB,PORTB2	; CS high during flushing

	push	Temp
	ldi	Temp,0xff
	rcall	sd_write_byte

	cbi	PORTB,PORTB2	; CS must be low to enable SPI mode
	
	pop	Temp
	ori	Temp,0x40
	rcall	sd_write_byte
	
	mov	Temp,YH
	rcall	sd_write_byte
	mov	Temp,YL
	rcall	sd_write_byte
	mov	Temp,XH
	rcall	sd_write_byte
	mov	Temp,XL
	rcall	sd_write_byte
	
	ldi	Temp,0x95	; CRC for init command, ignored otherwise
	rcall sd_write_byte
	
	;rcall sd_idle_bytes
	ret



ReadPart1Addr:
ldi	Temp,0x11	;command 17  read Single block
	ldi	XL,0
	ldi	XH,0
	ldi	YL,0
	ldi	YH,0
	rcall	SPISendCommand
	

	ldi Temp2,0xff
RPALoop1:	ldi Temp,0xff
	rcall sd_write_byte
	cpi Temp,0xFE
	brne RPALoop1
	ldi Temp2,0xff
RPALoop2:
	ldi Temp,0xff
	rcall sd_write_byte
	dec Temp2
	cpi Temp2,0
	brne RPALoop2

	ldi Temp2,199
RPALoop3:	ldi Temp,0xff
	rcall sd_write_byte
	
	dec Temp2
	cpi Temp2,0
	brne RPALoop3
	
RPALoop4:
	ldi Temp,0xff	
	rcall sd_write_byte
	sts Partition1LBA,Temp
	rcall sd_write_byte
	sts Partition1LBA+1,Temp
	rcall sd_write_byte
	sts Partition1LBA+2,Temp
	rcall sd_write_byte
	sts Partition1LBA+3,Temp
	ldi Temp2,70
RPALoop5:	ldi Temp,0xff
	rcall sd_write_byte
	dec Temp2
	cpi Temp2,0
	brne RPALoop5


ret





LBA2ByteAddr:			;Converts LBA address of partition 1 into Byte address

lds Temp,Partition1LBA
sts LBA2Byte,Temp

lds Temp,Partition1LBA+1
sts LBA2Byte+1,Temp

lds Temp,Partition1LBA+2
sts LBA2Byte+2,Temp

lds Temp,Partition1LBA+3
sts LBA2Byte+3,Temp

rcall ConvertLBA2Byte



lds Temp,LBA2Byte+3
sts Partition1ByteAddress+3,Temp
lds Temp,LBA2Byte+2
sts Partition1ByteAddress+2,Temp
lds Temp,LBA2Byte+1
sts Partition1ByteAddress+1,Temp
lds Temp,LBA2Byte
sts Partition1ByteAddress,Temp
ret


ReadVolBS:					;reading volume boot sector
push Temp2



lds Temp2,Partition1ByteAddress
	mov	XL,Temp2
lds Temp2,Partition1ByteAddress+1
	mov	XH,Temp2
lds Temp2,Partition1ByteAddress+2
	mov	YL,Temp2
lds Temp2,Partition1ByteAddress+3
	mov	YH,Temp2

ldi	Temp,0x11	;command 17  read Single block
	rcall	SPISendCommand
	

	
RVBLoop1:	ldi Temp,0xff
	rcall sd_write_byte
	cpi Temp,0xFE
	brne RVBLoop1
	rcall Read64Bytes;;;Reading 64 bytes into buffer
ldi XL,low(ReadBuffer)
ldi XH,high(ReadBuffer)
ldi Temp,11
adc XL,Temp
clr Temp
adc XH,Temp
	ld Temp,x+

	sts BytesPerSector,Temp

	ld Temp,x+

	sts BytesPerSector+1,Temp

	ld Temp,x+

	sts SectorsPerCluster,Temp

	ld Temp,x+
	
	sts ReservedSectors,Temp

	ld Temp,x+
	
	sts ReservedSectors+1,Temp

	ld Temp,x+
	
	sts NumberOfFats,Temp

	clc
	ldi Temp,19				
	adc XL,Temp
	clr Temp
	adc XH,Temp
	
	ld Temp,x+
	
	sts SectorsPerFat,Temp


	ld Temp,x+
	
	sts SectorsPerFat+1,Temp


	ld Temp,x+
	
	sts SectorsPerFat+2,Temp

	ld Temp,x+
	
	sts SectorsPerFat+3,Temp

ldi Temp2,0xff
FindEnd:		;loop until end is reached

ldi Temp,0xff
rcall sd_write_byte
dec Temp2
cpi Temp2,0
brne FindEnd
ldi Temp2,0xff
FindEnd2:

ldi Temp,0xff
rcall sd_write_byte
dec Temp2
cpi Temp2,0
brne FindEnd2
rcall sd_idle_bytes

lds Temp,ReservedSectors			;Finding Start of FAT
sts FatStartByteAddress,Temp
lds Temp,ReservedSectors+1			
sts FatStartByteAddress+1,Temp
ldi Temp,0
sts FatStartByteAddress+2,Temp
sts FatStartByteAddress+3,Temp
clc	
lds Temp,FatStartByteAddress
lds Temp2,Partition1LBA	
adc Temp,Temp2
sts FatStartByteAddress,Temp
lds Temp,FatStartByteAddress+1
lds Temp2,Partition1LBA	+1
adc Temp,Temp2
sts FatStartByteAddress+1,Temp
lds Temp,FatStartByteAddress+2
lds Temp2,Partition1LBA	+2
adc Temp,Temp2
sts FatStartByteAddress+2,Temp

lds Temp,FatStartByteAddress+3
lds Temp2,Partition1LBA	+3
adc Temp,Temp2
sts FatStartByteAddress+3,Temp			;At this point there should be correct LBA for FAT



lds Temp,FatStartByteAddress+3
sts LBA2Byte+3,Temp
lds Temp,FatStartByteAddress+2
sts LBA2Byte+2,Temp
lds Temp,FatStartByteAddress+1
sts LBA2Byte+1,Temp
lds Temp,FatStartByteAddress
sts LBA2Byte,Temp

rcall ConvertLBA2Byte

lds Temp,LBA2Byte+3
sts FatStartByteAddress+3,Temp
lds Temp,LBA2Byte+2
sts FatStartByteAddress+2,Temp
lds Temp,LBA2Byte+1
sts FatStartByteAddress+1,Temp
lds Temp,LBA2Byte
sts FatStartByteAddress,Temp			;Fat Start Byte Address should have correct Byte Address now

	FindingClusterBegin:			;finding start of free cluster
lds Temp,NumberOfFats
lds Temp2,SectorsPerFat
mul Temp,Temp2
sts ClusterBeginLBA,r0
push r1
lds Temp2,SectorsPerFat+1
mul Temp,Temp2
pop Temp2
adc r0,Temp2
sts ClusterBeginLBA+1,r0
clr Temp2
adc r1,Temp2
push r1
lds Temp2,SectorsPerFat+2
mul Temp,Temp2
pop Temp2
adc r0,Temp2
sts ClusterBeginLBA+2,r0
clr Temp2
adc r1,Temp2
lds Temp2,SectorsPerFat+3
adc r1,Temp2
sts ClusterBeginLBA+3,r1			;At this point cluster begin LBA = (no of fats * sectors per fat)
;rcall TypeFreeCBA
clc
lds Temp,ReservedSectors

lds Temp2,ClusterBeginLBA
adc Temp2,Temp
sts ClusterBeginLBA,Temp2
lds Temp2,ClusterBeginLBA+1
lds Temp ,ReservedSectors+1;
adc Temp2,Temp
sts ClusterBeginLBA+1,Temp2
lds Temp2,ClusterBeginLBA+2
clr Temp
adc Temp2,Temp
sts ClusterBeginLBA+2,Temp2
lds Temp2,ClusterBeginLBA+3
adc Temp2,Temp
sts ClusterBeginLBA+3,Temp2		;At this point Cluster Begin LBA=(No Of FATS*Sectors Per Fat)+Reserved Sectors
;rcall TypeFreeCBA
/////Multiply  CBA by 512 to get byte address relative to partition begin
clc

lds temp,ClusterBeginLBA+2
sts ClusterBeginLBA+3,Temp

lds temp,ClusterBeginLBA+1
sts ClusterBeginLBA+2,Temp

lds temp,ClusterBeginLBA
sts ClusterBeginLBA+1,Temp
clr Temp
sts ClusterBeginLBA,Temp

lds Temp,ClusterBeginLBA+1
lsl Temp
sts ClusterBeginLBA+1,Temp
clr Temp2
clr Temp3
adc Temp2,Temp3		;Temp2 has the carry bit now for LBA2
push Temp2
clr Temp2
lds Temp,ClusterBeginLBA+2
lsl Temp
adc Temp2,Temp3	;Temp2 has carry bit for LBA3
clc
lds Temp,ClusterBeginLBA+3
lsl Temp
add Temp,Temp2
sts ClusterBeginLBA+3,Temp
lds Temp,ClusterBeginLBA+2
lsl Temp
pop Temp2
add Temp,Temp2
 sts ClusterBeginLBA+2,Temp
;rcall TypeFreeCBA

/// At this point Cluster Begin LBA should have correct byte offset from Start of Partition1

clc
lds Temp,Partition1ByteAddress
lds Temp2,ClusterBeginLBA
adc Temp2,Temp
sts ClusterBeginLBA,Temp2
lds Temp,Partition1ByteAddress+1
lds Temp2,ClusterBeginLBA+1
adc Temp2,Temp
sts ClusterBeginLBA+1,Temp2
lds Temp,Partition1ByteAddress+2
lds Temp2,ClusterBeginLBA+2
adc Temp2,Temp
sts ClusterBeginLBA+2,Temp2
lds Temp,Partition1ByteAddress+3
lds Temp2,ClusterBeginLBA+3
adc Temp2,Temp
sts ClusterBeginLBA+3,Temp2		;At this point Cluster Begin LBA=PartitionBeginLBA+ReservedSectors+(FAT*Sector per Fat)



;rcall TypeFreeCBA

ldi Temp,0xff
rcall PassNBytes
ldi Temp,0xff
rcall PassNBytes

pop Temp2
ret


PassNBytes:		;takes number of bytes in Temp register to pass
push Temp3
push Temp2
push Temp
mov Temp3,Temp
PNBLoop1:
 ldi Temp2,0xff
 rcall sd_write_byte
 ;rcall R1Interpreter
 ;rcall WriteNL
dec Temp3
cpi Temp3,0
brne PNBLoop1
pop Temp
pop Temp2
pop Temp3
ret





ReadDirectory:


ldi Temp,0x2
sts FATClusterNo,Temp
ldi Temp,0
sts FATClusterNo+1,Temp
sts FATClusterNo+2,Temp
sts FATClusterNo+3,Temp
;rcall ReadFATEntry


lds Temp2,ClusterBeginLBA
	mov	XL,Temp2
lds Temp2,ClusterBeginLBA+1
	mov	XH,Temp2
lds Temp2,ClusterBeginLBA+2
	mov	YL,Temp2
lds Temp2,ClusterBeginLBA+3
	mov	YH,Temp2
ReadDirectoryBegin:
ldi	Temp,0x12	;command 18  read Multiple block
	rcall	SPISendCommand

lds Temp,SectorsPerCluster
push Temp



RDDDataStart:

ldi Temp,0xff
rcall sd_write_byte

cpi Temp,0xfe		;Wait for start of Data Marker
brne RDDDataStart


ldi Temp,16
push Temp
RDDisplay1:

				rcall ReadFileName
				
				rcall PrintFileName				

pop Temp
dec Temp
cpi Temp,0
breq ReadCRC
push Temp
rjmp RDDisplay1
ReadCRC:
ldi Temp,0xff
rcall sd_write_byte
ldi Temp,0xff
rcall sd_write_byte
Pop Temp
dec Temp
cpi Temp,0
breq ReadDirectoryClusterEnd
push Temp

rjmp RDDDataStart
ReadDirectoryClusterEnd:
rcall StopDataTransmission
ldi Temp,0xff
rcall sd_write_byte
RDCELoop1:ldi Temp,0xff
rcall sd_write_byte
cpi Temp,0
breq RDCELoop1


rcall ReadFATEntry				;ReadFatEntry
lds Temp,FATClusterNo
sts LBA2BYTE,Temp
lds Temp,FATClusterNo+1
sts LBA2BYTE+1,Temp
lds Temp,FATClusterNo+2
sts LBA2BYTE+2,Temp
lds Temp,FATClusterNo+3			
sts LBA2BYTE+3,Temp
rcall FatClusterNo2ByteAddress			;ConvertCluster to byte address
				
lds Temp2,LBA2BYTE
	mov	XL,Temp2
lds Temp2,LBA2BYTE+1
	mov	XH,Temp2
lds Temp2,LBA2BYTE+2
	mov	YL,Temp2
lds Temp2,LBA2BYTE+3
	mov	YH,Temp2
rjmp ReadDirectoryBegin			;Read Next Cluster
rcall ReadIdleBytes
ret

DisplayMBR:	
push Temp
push Temp2
				;This function will read sector 0 and display..For debugging only
ldi Temp,0x11
clr XL
clr XH
clr YL
clr YH
rcall SPISendCommand


DMBRLoop0:
ldi Temp2,0xff
rcall sd_Write_byte
cpi Temp,0xfe
brne DMBRLoop0

DisplayMBRLoop1:
ldi temp,0xff
rcall sd_Write_byte
rcall R1Interpreter
rcall WriteNL
dec Temp2
cpi Temp2,0
brne DisplayMBRLoop1

ldi Temp2,0xff
DisplayMBRLoop2:
ldi temp,0xff
rcall sd_Write_byte
rcall R1Interpreter
rcall WriteNL
dec Temp2
cpi Temp2,0
brne DisplayMBRLoop2
rcall sd_idle_bytes
pop Temp2
pop Temp
ret

DisplayPart1BootSec:
push Temp
push Temp2


lds Temp2,Partition1ByteAddress
	mov	XL,Temp2
lds Temp2,Partition1ByteAddress+1
	mov	XH,Temp2
lds Temp2,Partition1ByteAddress+2
	mov	YL,Temp2
lds Temp2,Partition1ByteAddress+3
	mov	YH,Temp2

ldi	Temp,0x11	;command 17  read Single block
	rcall	SPISendCommand
	

	
DP1BSLoop1:	ldi Temp,0xff
	rcall sd_write_byte
	cpi Temp,0xFE
	brne DP1BSLoop1
	
ldi Temp2,0xff

DP1BSLoop2:
ldi Temp,0xff
rcall sd_write_byte
rcall R1Interpreter
rcall WriteNL
dec Temp2
cpi Temp2,0
brne DP1BSLoop2


ldi Temp2,0xff

DP1BSLoop3:
ldi Temp,0xff
rcall sd_write_byte
rcall R1Interpreter
rcall WriteNL
dec Temp2
cpi Temp2,0
brne DP1BSLoop3
rcall sd_idle_bytes

pop Temp2
pop Temp
ret


TypeFreeCBA:



lds Temp,ClusterBeginLBA+3
rcall R1Interpreter
rcall WriteNL
lds Temp,ClusterBeginLBA+2
rcall R1Interpreter
rcall WriteNL
lds Temp,ClusterBeginLBA+1
rcall R1Interpreter
rcall WriteNL
lds Temp,ClusterBeginLBA
rcall R1Interpreter
rcall WriteNL

ret

Read64Bytes:

ldi Temp2,64

ldi Temp,0xff

ldi r26,low(ReadBuffer)
ldi r27,high(ReadBuffer)
R64BLoop1:
ldi Temp,0xff
rcall sd_write_byte
st x+,Temp
dec Temp2
cpi Temp2,0
brne R64BLoop1




ret

ConvertLBA2Byte:		;takes 4 byte LBA in LBA2Byte buffer and return BYTE address in same 

push Temp3				;assumes 512 byte block .This function ignores the Highest Byte of LBA
push Temp2
push Temp

lds Temp,LBA2Byte
ldi Temp2,0x02
mul Temp2,Temp

clr Temp3
push Temp3
push r0
push r1		;3 pushed

lds Temp,LBA2Byte+1
ldi Temp2,0x02
mul Temp2,Temp
pop Temp3
clc
adc r0,Temp3
push r0
clr Temp3
adc r1,Temp3
push r1		;4 pushed

lds Temp,LBA2Byte+2
ldi Temp2,0x02
mul Temp2,Temp
pop Temp3
clc
adc r0,Temp3
push r0		
;clr Temp3
;adc Temp2,Temp3
;push Temp2		;5 pushed.. As no 5 byte addresss the fifth byte is not pushed 

pop Temp
sts LBA2Byte+3,Temp
pop Temp
sts LBA2Byte+2,Temp
pop Temp
sts LBA2Byte+1,Temp
pop Temp
sts LBA2Byte,Temp

pop Temp
pop Temp2
pop Temp3


ret

ReadFileName:
push Temp
push Temp2

ldi Temp2,32


ldi r26,low(ShortFileName)
ldi r27,high(ShortFileName)
RFNLoop1:
ldi Temp,0xff
rcall sd_write_byte
st x+,Temp
dec Temp2
cpi Temp2,0
brne RFNLoop1

pop Temp2
pop Temp
ret



PrintFileName:
push Temp
push Temp2

lds Temp,ShortFileName+11
cpi Temp,0xF
breq PFNEnd


ldi Temp2,12

ldi Temp,0xff

ldi r26,low(ShortFileName)
ldi r27,high(ShortFileName)
PFNLoop1:

ld Temp,x+
cpi Temp2,12
brne PFNLoop2
cpi Temp,0
breq DirectoryEnd
cpi Temp,32
brcs PFNEnd
cpi Temp,128
brcc PFNEnd


PFNLoop2:
rcall UARTTransmit
dec Temp2
cpi Temp2,0
brne PFNLoop1
rcall WriteNL
PFNEnd:
pop Temp2
pop Temp
ret
DirectoryEnd:


rcall WriteNL
rcall WriteNL
ldi Temp2,13
ldi ZH,high(EndMessage<<1)
ldi ZL,low(EndMessage<<1)
DELoop1:
lpm Temp,Z+
cpi Temp,0
breq DELoop1End
rcall UARTTransmit
rjmp DELoop1
;dec Temp2
;cpi Temp2,0
;breq DELoop

rjmp DELoop1

DELoop1End:
rcall WriteNL
rcall WriteNL
Forever:
rjmp Forever				;loop forever
ret


StopDataTransmission:		;Will stop Block transmission in Multi Block Read mode
ldi XL,0
ldi XH,0
ldi YL,0
ldi YH,0

ldi	Temp,12	;command 12  Stop Block Transmission
	rcall	SPISendCommand


Ret

FatClusterNo2ByteAddress:		;This Function will take a FatCluster number in LBA2BYTE and convert into byte address for reading
Push Temp
Push Temp2

clc
clr Temp
clr Temp2
lds Temp,LBA2Byte				;
subi Temp,02					;Subtracting 02 from Cluster address
sts LBA2Byte,Temp
clr Temp
adc Temp2,Temp
lds Temp,LBA2Byte+1
sub Temp,Temp2					;Subtracting the carry bit
sts LBA2Byte+1,Temp
clc
clr Temp
clr Temp2
lds Temp,SectorsPerCluster		;Multiplying by sectors per cluster
lds Temp2,LBA2Byte
mul Temp,Temp2
push r0					
push r1
lds Temp2,LBA2Byte+1
mul Temp,Temp2
pop Temp2
adc Temp2,r0
push Temp2
clr Temp2
adc Temp2,r1
push Temp2
lds Temp2,LBA2Byte+2
mul Temp,Temp2
pop Temp2
adc Temp2,r0
push Temp2
clr Temp2
adc Temp2,r1
push Temp2
lds Temp,LBA2Byte+3
pop Temp2
adc Temp,Temp2
sts LBA2Byte+3,Temp
pop Temp
sts LBA2Byte+2,Temp
pop Temp
sts LBA2Byte+1,Temp
pop Temp
sts LBA2Byte,Temp

rcall ConvertLBA2Byte				;Converting to Byte Address 

clc
lds Temp,LBA2Byte
lds Temp2,ClusterBeginLBA
adc Temp2,Temp
sts LBA2Byte,Temp2
lds Temp,LBA2Byte+1
lds Temp2,ClusterBeginLBA+1
adc Temp2,Temp
sts LBA2Byte+1,Temp2
lds Temp,LBA2Byte+2
lds Temp2,ClusterBeginLBA+2
adc Temp2,Temp
sts LBA2Byte+2,Temp2
lds Temp,LBA2Byte+3
lds Temp2,ClusterBeginLBA+3
adc Temp2,Temp
sts LBA2Byte+3,Temp2				;Added the offset from Cluster Beginning 


pop Temp2
pop Temp
ret

ReadFATEntry:		;Will take a 4 byte index to Fat Entry and return a 4 byte Cluster Number 
					;The Inputs will be passed in FatClusterNo array
					;The OutPut will be returned in FatClusterNo array



lds Temp2,FATStartByteAddress
	mov	XL,Temp2
lds Temp2,FATStartByteAddress+1
	mov	XH,Temp2
lds Temp2,FATStartByteAddress+2
	mov	YL,Temp2
lds Temp2,FATStartByteAddress+3
	mov	YH,Temp2

ldi	Temp,0x12	;command 18  read Multiple block
	rcall	SPISendCommand


RFEStart:
;rcall R1Interpreter
;rcall WriteNL
ldi Temp,0xff
rcall sd_write_byte

cpi Temp,0xfe		;Wait for start of Data Marker
brne RFEStart






ldi Temp,128
sts BYTECOUNT,Temp

RFELoop1:

lds Temp,FatClusterNo
tst Temp
breq FCB0isZero						
dec Temp
sts FatClusterNo,Temp

RFELoop1End:
ldi Temp,0xff
rcall sd_write_byte
ldi Temp,0xff
rcall sd_write_byte
ldi Temp,0xff
rcall sd_write_byte
ldi Temp,0xff
rcall sd_write_byte
lds Temp,BYTECOUNT
dec Temp
cpi Temp,0
breq RFEReadCRC
sts BYTECOUNT,Temp
rjmp RFELoop1
FCB0isZero:
ldi Temp,0xff
sts FatClusterNo,Temp
lds Temp,FatClusterNo+1
tst Temp
breq FCB1isZero
dec Temp
sts FatClusterNo+1,Temp
rjmp RFELoop1End
FCB1isZero:
ldi Temp,0xff
sts FatClusterNo+1,Temp

lds Temp,FatClusterNo+2
tst Temp
breq FCB2isZero
dec Temp
sts FatClusterNo+2,Temp
rjmp RFELoop1End
FCB2isZero:
lds Temp,FatClusterNo+3
tst Temp
breq ReadFATEnd
ldi Temp,0xff
sts FatClusterNo+2,Temp
lds Temp,FatClusterNo+3
dec Temp
sts FatClusterNo+3,Temp
rjmp RFELoop1End
RFEReadCRC:
ldi Temp,0xff
rcall sd_write_byte
ldi Temp,0xff
rcall sd_write_byte


rjmp RFEStart
ReadFATEnd:

ldi Temp,0xff
rcall sd_write_byte
sts FatClusterNo,Temp
ldi Temp,0xff
rcall sd_write_byte
sts FatClusterNo+1,Temp
ldi Temp,0xff
rcall sd_write_byte
sts FatClusterNo+2,Temp
ldi Temp,0xff
rcall sd_write_byte
sts FatClusterNo+3,Temp

rcall StopDataTransmission
ldi Temp,0xff
rcall sd_write_byte
RFELoop2:ldi Temp,0xff
rcall sd_write_byte
cpi Temp,0
breq RFELoop2
/*							;For Debugging only
lds Temp,FatClusterNo		;This is just to show how I proceeded to debug this program
rcall R1Interpreter			;right now this is commented 
rcall WriteNL				;But you can un comment to use this	


lds Temp,FatClusterNo+1
rcall R1Interpreter
rcall WriteNL


lds Temp,FatClusterNo+2
rcall R1Interpreter
rcall WriteNL


lds Temp,FatClusterNo+3
rcall R1Interpreter
rcall WriteNL
rcall WriteNL
rcall WriteNL
;rcall ReadIdleBytes
*/

ret

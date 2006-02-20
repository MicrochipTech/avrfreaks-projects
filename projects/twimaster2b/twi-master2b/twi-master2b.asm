;TWI master routines version 2.0b rev14
;Leo Gijssel,sep 2005 - feb 2006
;lgijssel@neon-networking.nl

;TWI routines in assembly, built with AVRstudio4. A TWI master adressing multiple devices.
;This TWI service is intended to perform in a multi-tasking system while making very 
;little use of system resources. Hence, the program does not contain any wait-cycles or 
;delay loops. In fact, this was the most important reason for me to go through the trouble 
;of developing this code. 
;The basic idea is that each TWI device on the bus is served via a separate queue. (Q) that 
;basically consists of a string of bytes containing the data to be sent/received.
;Application programs can fill a device queue when needed, independent from other devices. The 
;IRQ will service them sequentially. Once a Q is filled, the application should wait until
;the request is serviced. Support routines are provided for handling application requests and
;for checking Q-status. 
;
;I intend to use this program for a robot platform that I am building using the ATmega32 but
;any AVR platform with sufficient resources should do the job. One of the last issues that I
;resolved was adjusting the TWI speed. In a system that is using multiple interrupts (I use 
;timer & adc interrupts as well) you cannot set the data rate too high.
;It is now running very stable at 50kHz but I assume that much higher speeds wil be possible 
;when TWI is used as the only interrupt on the system. Saying this, keep in mind that the Mega32
;on which the code runs uses an 8MHz internal clock.
;
;As said, each device that is connected has it's own data queue in RAM.
;Support routines are provided for the following purposes:
;
;	TWIwrtQ		Put data in a device-Q. Copy from RAM to RAM, uses X,Y
;	TWIreadQ	Read from a device-Q. Copy from RAM to RAM, uses X,Y
;	TWIchkQ		Check Q status
;	TWIreadQnull	Force Q to drop existing data
;	TWIpoll		Poll queues and activate the TWI-irq if there is data to be sent.
;			This is simply achieved by writing a start sequence to the TWI.
;	TWIflash2ram	copy fixed strings from flash to ram. Added from an existing lib
;			This type of routine is likely to be required in combination with the
;			TWI services.
;
;Easy adaption is made possible while each set of service routines is in fact a copy of the 
;rest. Eachy uses a unique letter combination in the labels and the other references to 
;memory locations. Example: TWIwrtSRF writes to the SRF08. Exactly the same routine
;is used for the CMPS03; It is only called TWIwrtCMPS. You can create a new set of routines by 
;copying the SRF08 support to notepad and do a search&replace for the capital letters SRF.
;Besides this cutting/pasting, all one should do to add a new device is defining the 
;required Q-space in RAM. 
;After the first session of code development I decided to add yet another routine:
;
;	TWIwrdfQ	Write directly from flash into the device-Q. This one uses Y,Z.
;			There are many devices that can be adequately controlled by using a 
;			set of predefined commands in flash memory. 
;			This routine saves the extra copy-sweep from ram to TWI-device-Q. 
;			Hence it also saves a few bytes of ram also while it is not needed to 
;			define an extra RAM-buffer for the temp store.
;
;It is important to remember that the first byte that's read from flash must contain the length 
;of the string-to-be-copied.
;
;Using the above set of routines, developers could use the TWI system as a separate entity in 
;the machine. Use TWIchk before reading or writing anything. It also helps when the application
;is more or less aware of what has been sent to the TWI and the time that will be needed to 
;process the request. You may use some sort of time-stamping to realize this. If you don't, the
;application may still end up polling a device for ages. 
;The service routines exit with carry set if the intended operation was not successfull.
;
;The TWI processing itself is entirely IRQ-based. TWIpoll must run in the main loop.
;All other routines should be called when data needs to be read or written or when a Q status 
;must be checked. For simplicity and performance sake, each device-Q has it's own service 
;routine with hard-coded adressing using labels. This means more code, but also (much) faster 
;performance. 
;
;TWIpoll
;When there is no data to be sent, the TWI logic does not generate IRQ's. The TWIpoll routine
;"polls" the queues in a round-robin fashion. If one of them ;contains data, it writes the 
;startadress of the Q in the X-reg temp store and starts the TWI by sending a start sequence. 
;This triggers the TWI-interrupt sequence to start. 
;
;If you want to use this code, just define a Q, code your command strings and use TWIwrt or 
;TWIwrdf to put them in the Q. Use TWIread to store your results at any desired location in 
;RAM. 
;
;Memory usage
;The required amount of RAM varies strongly depending on the kind of devices that are connected.
;In it's current configuration, (with 3 devices) the program uses less than 128 bytes.
;eight for temporary storage of registers and about 96 bytes for device queues.
;In the current version, the device queues should start near the beginning of a page ($0100, $0200)
;as the overflow of the low byte of the X-reg is not consistently checked. (Improvement?!)
;
;Appl-IRQ communication:
;Any IRQ driven program needs a method to signal between IRQ and application.
;This is achieved as follows:
;The first byte of each Q contains the length and bit 6,7 have special functions:
;bit 7 is set when IRQ has finished the Q  (Qprocessed)
;bit 6 is set when data is stored in the Q and IRQ service is required (Qbusy)
;The truth table is as follows:
;b7	b6	status
; 0	 0	Q is available
; 0	 1	Data is waiting to be processed by IRQ
; 1	 0	Q processing
; 1	 1	Q processed. The application must read the Q and return both bits to 
;		zero-state to re-enable it.
;
;The remaining six bits are used to indicate the length of the data, including the device address. 
;This means the maximal datalength is 62bytes. The actual length is set by twiwrtQ. 
;Based on the properties of the device, the Q-length can be shorter. Having longer queues than 
;63 bytes would not be desirable as they would / could monopolize the available TWI bandwidth. 
;
;Q-treatment varies depending on the type of device. When it's a write-only device like a 
;display the application should not bother about reading the Q-contents.
;For other devices, reading the data is generally required. The application may decide to drop 
;the contents of the Q by resetting bit 6 and 7 to zero without reading it's data first. 
;(this is exactly what TWIreadQnull does).
;
;Repeated start mode
;It was nescessary for me to implement this feature as many devices require this mode, 
;even for basic communication. It is implemented as follows:
;The data to be sent must be entered in the Q sequentially.
;Thus: Qlen, device-addr+W, register-nr, device-addr+R
;After receiving the ACK for the register (byte2) the program checks the next byte to be 
;sent. If this byte equals the start-adress+1 (=not_write-bit set) a repeated start sequence 
;is initiated without resetting the Q-len, so after the rep-start the start-adress+1 is sent. 
;After this sequence, the device will upload the contents of the desired register(s). 
;Beware: As the queue-length is not reset, the Q holds the following:
;0:Qlen 1:device-addr+W, 2:register number to read, 3:device-addr+R. 
;Returned data is stored from byte 4 onwards. 
;The application reading/writing the Q must take this into consideration. 
;
;
;Future enhancements:
;1: Add support for TWI slave-mode. This one is on my list for the moment when I will start to
;use a second controller for image-processing.
;
;2: Add support for multi-master environments (bus arbitration) Feel free!!
;
;3: The addition of drivers for other TWI-devices is not too difficult. Simply copy the 
;support routines for one device and rename the labels. The code will grow with it though.
;My previous efforts were on a program that uses the same support routines for all devices but
;this became too complex and it required too much CPU-cycles, especially in the IRQ code.
;I am aware that the code can be made shorter but this should not reduce stability.
;For the rest, I see this code as an ongoing project. The primary focus is on functionality
;and speed. It can still be shortened and improved. 
;The real difference is in the command-strings that are used to control a device. 
;I would appreciate other people's contributions to making this code better. 
;Please keep me posted!
;
;Known issues
;when writing to a device, the 3rd byte must not equal SLA+R. If it does, a repeated start will
;be generated. This does not seem to cause any problems in practical use but it is a bug.
;
;There is little doubdt in my mind that there will be other errors in the source. 
;The code has been tested before uploading but extensive tests are not that easy to perform. 
;So I am reluctantly handing this source to others and hope for the best. It is said that 
;this is common practice under software developers! ;-)
;Do not assume any liabilitly from my side for disasters or minor events that occur as a 
;direct or indirect result from using this code. 
;
;*
;*	License
;*	Free for non-commercial use
;*	(c)LG 2006
;
;Registers used:
;IRQ uses r24, r25, X-reg but preserves the original contents.
;status register is kept in r0. TWIpoll uses r25.
;The copy routines also require Y and/or Z-reg (How should they work otherwise?)
;Almost all register use is transparent, registers are preserved where this is desirable. 
;
;I have found it useful to designate one register (r13) to be always zero.
;This is used mostly for clearing-up string space, for the cpse instruction and for other 
;compare operations. r13 is cleared on reset. U can either take over this convention or adapt
;the source to fit your own specific demands.
;
;A word on register naming: It is apperently good practice among developers to give registers
;that are used in a source a name that designates their use. I have NOT done this for two 
;reasons: 
;1: It seems not to be helpful for me, I can't remember who's who anyway.
;2: When writing very large programs that consist of multiple modules it is better to have a 
;uniform naming convention for the registers that are used. 
;I prefer to use the default names for that.
;
;***********************************************************************************************

.equ baseaddr= (RAMSTART + $00a0) ;this offset allows placing the variables anywhere in RAM

.equ Qid0 = (baseaddr + $00)	;Q currently serviced, 0 when idle.
.equ Qlen = (baseaddr + $01)	;length of data excluding controlbyte
.equ XHtemp = (baseaddr + $02)	;pointer to start of Q
.equ XLtemp = (baseaddr + $03)
.equ XHtemp2 = (baseaddr + $04)	;copy of XL/XHtemp for recovery of startadress
.equ XLtemp2 = (baseaddr + $05)
.equ pollindex = (baseaddr + $06) ;index for twipoll
.equ TWIrsctl = (baseaddr + $07);Control byte for repeated start

;The device-Q table and it's variables must be near the 
;start of a page: 0x0100
.equ QtableRAM = (baseaddr + $00)

;hard coded device adresses, use brackets: ldi r16,(devCMPS03Q)
.equ devSRF08=$E0 
.equ devCMPS03=$C0
.equ devLCD32=$C6

;set the references to RAM adresses
.dseg
.org 0x0110
SRF08Q:

.org 0x0120
CPU02Q:

.org 0x0130
CMPS03Q:

.org 0x0140
LCD32Q:

.org 0x0170
SRF08Q1:

.org 0x0180
LCD32tempQ: ;also referenced as DHstr


.cseg

TWIreset:
;zeroize Q-space
ldi XH,high(QtableRAM)
ldi XL,low(QtableRAM + $7f)

TWIreset1:
st -X,r13
cp XL,r13
brge TWIreset1

;preset loopcontrol
ldi r16,$07
sts pollindex,r16

TWIreset2:
;Init TWSI
; set device adress to $FE, do not respond to general calls
; setting the adress is not really required as we will be master
ldi r16,$FE
out TWAR,r16
; Set speed for 100Khz, prescaler divides by 1
ldi r16,$00
out TWSR,r16
;80= 16+2(TWBR) > TWBR=32
;ldi r16,$20
ldi r16,$48 ;=50kHz
out TWBR,r16
;Enable TWI & TWIinterrupts
ldi r16,$05
out TWCR,r16
ret


TWSI:
;save status register
in r0,SREG
;save registers
push XH
push XL
push r25
push r24

;initialize X-reg
lds XL,XLtemp
lds XH,XHtemp

;check current status
;Master Transmitter mode
in r24,TWSR
andi r24,$f8
;start sent
cpi r24,$08
brne CHK02
jmp TWI01 ;send SLA+R/W

CHK02:
;repeated start sent
cpi r24,$10
brne CHK03
jmp TWI02

CHK03:
;SLA+W out, ack in
cpi r24,$18
brne CHK04
jmp TWI01 ;send next byte

CHK04:
;SLA+W out, nack in
cpi r24,$20
brne CHK05
jmp TWI03

CHK05:
;data out, ack in
cpi r24,$28
brne CHK06
jmp TWI04

CHK06:
;data out, nack in
cpi r24,$30
brne CHK07
jmp TWI03

CHK07:
;Master Receiver mode
;SLA+R out, ack in
cpi r24,$40
brne CHK08
jmp TWI05a

CHK08:
;data in, ack sent
cpi r24,$50
brne CHK09
jmp TWI05

CHK09:
;data in, nack sent
cpi r24,$58
brne CHK10
jmp TWI07

CHK10:
;SLA+R out, nack in
cpi r24,$48
brne CHK11
jmp TWI06

CHK11:
;All other state codes: clear IRQ & exit
;** Starting point for anyone who wants to implement bus arbitration!! **
;
lds r25,TWCR
;clear b5,b4 TWSTA,TWSTO
cbr r25,$30
;set b6,b7,b2 TWEA,TWINT,TWEN
sbr r25,$C4
out TWCR,r25 ;write TWCR
jmp TWIexit1 ;do not save X-reg

TWIexit:
out TWCR,r25 ;write TWCR

sts XLtemp,XL ;save X-reg
sts XHtemp,XH

TWIexit1: ;restore registers
pop r24
pop r25
pop XL
pop XH

;restore status register
out SREG,r0
reti

;*** Master Transmitter routines ***
;
TWI01: ;start sent ($08), send first byte (device address)
TWI02: ;repeated start sent ($10)
ld r24,X+
out TWDR,r24
;set TWCR & exit
in r25,TWCR
;clear b5, START
cbr r25,$20
;set b7,b6 TWINT,TWEA
sbr r25,$C0
jmp TWIexit


TWI03:
;SLA+W out, nack in, code $20 :Slave cannot accept data
;data out, nack in, code $30
rjmp TWI08 ;IRQ finished

TWI04:
;data byte sent,ack in, status =$28
;check if next byte is the 3rd
lds r24,TWIrsctl
inc r24
sts TWIrsctl,r24
brne TWI04a ;branch when X does not point to byte 3

ld r24,X ;load byte 3 containing potential SLA + R
lds r25,Qid0 ;Qid contains the slave address
sbr r25,1 ;set read-bit (0) in SLA and compare next byte to SLA+R
cpse r24,r25 ;skip next instr if repeated start=true
rjmp TWI04a ;jump if no repeated start must be sent

;on a repeated start, the next byte is not transmitted. 
;this is done after the rep start is sent via entry point TWI02
in r25,TWCR
;set TWINT, TWEA, TWSTA (init repeated start)
sbr r25,$E0
jmp TWIexit

TWI04a: ;data sent, ack in, no repeated start > it's a normal transmission
;continue with a check if this byte was the last to be sent 
mov r24,XL ;copy XL to r24
lds r25,XLtemp2 ;get original start value
inc r25 ;adjust for not counting byte 0 (ctrl-byte)
sub r24,r25 ;the diff is the number of bytes sent
lds r25,Qlen ;get Qlength
andi r25,$3f ;mask out status bits
cpse r24,r25 ;equal if the acked byte was the last
rjmp TWI04b

jmp TWI08 ;jump to exit routine

TWI04b:
;prepare to send next byte
ld r24,X+
out TWDR,r24 ;write the next byte and increment pointer
in r25,TWCR
;set TWINT, TWEA
sbr r25,$C0
jmp TWIexit

TWI05:
;*** Master Receiver routines ***
;
;data sent, ACK received, status $50
in r24,TWDR
st X+,r24 ;save data

TWI05a: ;SLA+R sent, ACK received, status $40
;byte is already in Q.
;check if next expected byte is the last
mov r24,XL ;copy XL to r24
lds r25,XLtemp2 ;get original start value
sub r24,r25 ;the diff is the number of bytes received
lds r25,Qlen ;get Qlength
andi r25,$3f ;mask out status bits
cpse r24,r25 ;equal if the next byte is the last
rjmp TWI05b ;skipped when next=last

;prepare to receive last byte, receive next & send NACK
in r25,TWCR
;clear TWEA(6) & set TWINT(7)
cbr r25,$40
sbr r25,$80
jmp TWIexit

TWI05b: ;not the last byte, receive next & send ACK
in r25,TWCR
;set TWEA(6) & TWINT(7)
sbr r25,$C0
jmp TWIexit


TWI06:
;SLA+R out, nack in, code $48, slave cannot fulfill request
;read extra copy of Qstartadress, saved by TWIwrite
lds XL,XLtemp2
lds XH,XHtemp2
ld r25,X
sbr r25,$C0 ;set bit6&7 to 1: Q processed
st X,r25 ;write result to Qlen
rjmp TWI08

TWI07:
;data in, nack sent, code $58
;this was the last byte for this Q to be received
in r24,TWDR
st X+,r24
;now continue with standard exit routine at TWI08

;*** Routines below are used to finsh a transmission ***
;*** after the Q-status has been updated             ***
;
TWI08: ;end of transmission, normal exit
;read extra copy of Qstartadress, saved by TWIwrite
lds XL,XLtemp2
lds XH,XHtemp2
ld r25,X
sbr r25,$C0 ;set bit6&7 to 1: Q processed
st X,r25 ;write result to Qlen
sts Qid0,r13 ;clear Qid0 to indicate irq is available
in r25,TWCR
;set TWSTO,TWEA,TWINT
sbr r25,$D0
jmp TWIexit


;Support routines
TWIchkSRF:
;just uses r24
lds r24,SRF08Q  ;get byte0 containing length & status bits
andi r24,$c0 ;mask status bits
ret 

TWIwrtSRF:
;Write data in the SRF08-Q
;Y-reg points to data source
call TWIchkSRF
breq TWIwrtSRF01 ;continue if r24=0

sec ;write failed, carry set
ret

TWIwrtSRF01:
;save X-register
push XL
push XH
;Q is available for write, X-reg points to start
ldi XL,low(SRF08Q)
ldi XH,high(SRF08Q)
call TWIram2q
;
lds r24,SRF08Q
sbr r24,$40 ;flag data waiting
sts SRF08Q,r24
;restore X-register
pop XH
pop XL
clc
ret ;write OK if r24:6=1, carry clear


;on entry, Y-reg must point to the data-destination
call TWIchkSRF
cpi r24,$C0
breq TWIreadSRF01 ;Test bit6,7 if "1" read Q-contents

sec ;set carry if read failed
ret ;exit, r24 contains the status

TWIreadSRF01:
;save X-register
push XL
push XH
;initalize X-reg. Y-reg must point to destination
ldi XL,low(SRF08Q)
ldi XH,high(SRF08Q)
call TWIq2ram ;copy the data
sts SRF08Q,r13 ;clear SRF-Q/byte0
;restore X-register
pop XH
pop XL
clc
ret

TWIreadSRFnull:
;Erase SRF write buffer
call TWIchkSRF
cpi r24,$C0
brne TWIreadSRF0a

sts SRF08Q,r13 ;clear SRF-Q

TWIreadSRF0a:
ret


TWIwrdfSRF:
;Write data from flash to the SRF08-Q
;write data to Q. Z-reg points to data source in flash.
call TWIchkSRF
breq TWIwrdfSRF01 ;continue if r24=0

sec
clr r24 ;write failed, carry set, r24:6=0
ret

TWIwrdfSRF01:
;Q is available for write
ldi YL,low(SRF08Q)
ldi YH,high(SRF08Q)
call TWIflash2ram	;this performs the actual copy
;
lds r24,SRF08Q
sbr r24,$40 ;flag data waiting
sts SRF08Q,r24
clc ;write OK, carry clear
ret

TWIchkCMPS:
;just uses r24
lds r24,CMPS03Q  ;get byte0 containing length & status bits
andi r24,$c0 ;mask status bits
ret ;exit state r24=$00, carry clear, zero flag set

TWIwrtCMPS:
;Write data in the CMPS03-Q
call TWIchkCMPS
breq TWIwrtCMPS01 ;continue if r24=0

sec
ret

TWIwrtCMPS01:
;write data to Q. Y-reg points to data source
;save X-register
push XL
push XH
;Q is available for write, X-reg points to start
ldi XL,low(CMPS03Q)
ldi XH,high(CMPS03Q)
call TWIram2q
lds r24,CMPS03Q
sbr r24,$40 ;flag data waiting
sts CMPS03Q,r24
;restore X-register
pop XH
pop XL
clc ;write OK,carry clear
ret

TWIreadCMPS:
;on entry, Y-reg must point to the data-destination (not preserved)
call TWIchkCMPS
cpi r24,$C0
breq TWIreadCMPS01 ;Test bit6,7 if "1" read Q-contents

sec ;set carry if read failed
ret ;exit, r24 contains the status

TWIreadCMPS01:
;save X-register
push XL
push XH
;read data from Q. Y-reg must point to destination
ldi XL,low(CMPS03Q)
ldi XH,high(CMPS03Q)
call TWIq2ram
sts CMPS03Q,r13 ;clear CMPS-Q/byte0
;restore X-register
pop XH
pop XL
clc
ret

TWIwrdfCMPS:
;Write data from flash to the CMPS08-Q
;write data to Q. Z-reg points to data source in flash.
call TWIchkCMPS
breq TWIwrdfCMPS01 ;continue if r24=0

sec
clr r24 ;write failed, carry set, r24:6=0
ret

TWIwrdfCMPS01:
;Q is available for write
ldi YL,low(CMPS03Q)
ldi YH,high(CMPS03Q)

call TWIflash2ram	;this performs the actual copy
;
lds r24,CMPS03Q
sbr r24,$40 ;flag data waitingl
sts CMPS03Q,r24

TWIwrdfCMPS02:
ret ;write OK if r24:6=1

TWIchkLCD:
;uses just r24
lds r24,LCD32Q  ;get byte0 containing length & status bits
andi r24,$C0 ;mask status bits
ret ;exit state r24=$00, zero flag set

TWIwrtLCD:
;Write data in the LCD32-Q
call TWIchkLCD
breq TWIwrtLCD01 ;continue if r24=0

sec 
ret

TWIwrtLCD01:
;write data to Q. Y-reg points to data source
;save X-register
push XL
push XH
;Q is available for write, X-reg points to start
ldi XL,low(LCD32Q)
ldi XH,high(LCD32Q)
call TWIram2q
lds r24,LCD32Q
sbr r24,$40 ;flag data waiting
sts LCD32Q,r24
;restore X-register
pop XH
pop XL
clc ;write OK, carry clear
ret

TWIreadLCD:
;altough an LCD display is generally used to write data to, there
;may be occasions where it is desirable to read some register settings
;of the LCD controller. On the LCD02 (the one I use) it is possible 
;to read the amount of space that's left in the IO buffer.
;
;on entry, Y-reg must point to the data-destination (not preserved)
call TWIchkLCD
cpi r24,$C0
breq  TWIreadLCD01 ;Test bit7, if "1" read Q-contents

sec
ret ;exit, r24 contains the status, carry set

TWIreadLCD01:
;save X-register
push XL
push XH
call TWIq2ram
sts LCD32Q,r13 ;clear LCD-Q/byte0
;restore X-register
pop XH
pop XL
clc
ret

TWIreadLCDnull:
;Erase LCD write buffer when IRQ is finished
call TWIchkLCD
cpi r24,$C0
brne TWIreadLCD0a

sts LCD32Q,r13 ;clear LCD-Q

TWIreadLCD0a:
ret

TWIwrdfLCD: ;Write data from flash to the LCD32-Q
;write data to Q. Z-reg points to data source in flash.
call TWIchkLCD
breq TWIwrdfLCD01 ;continue if r24=0

sec ;write failed if carry set
ret

TWIwrdfLCD01:
;Q is available for write
push YL
push YH
ldi YL,low(LCD32Q)
ldi YH,high(LCD32Q)

call TWIflash2ram	;this performs the actual copy
;
lds r24,LCD32Q
andi r24,$3f ;mask out b6,7 effectively clearing them (paranoid setting)
sbr r24,$40 ;flag data waiting, hand over to TWIpoll
sts LCD32Q,r24

TWIwrdfLCD02:
pop YH
pop YL
clc ;write OK, carry clear
ret

TWIchkCPU:
;just uses r24
lds r24,CPU02Q  ;get byte0 containing length & status bits
andi r24,$c0 ;mask status bits
ret 

TWIwrtCPU:
;Write data in the CPU02-Q
;Y-reg points to data source
call TWIchkCPU
breq TWIwrtCPU01 ;continue if r24=0

sec ;write failed, carry set
ret

TWIwrtCPU01:
;save X-register
push XL
push XH
;Q is available for write, X-reg points to start
ldi XL,low(CPU02Q)
ldi XH,high(CPU02Q)
call TWIram2q
;
lds r24,CPU02Q
sbr r24,$40 ;flag data waiting
sts CPU02Q,r24
;restore X-register
pop XH
pop XL
clc
ret ;write OK if r24:6=1, carry clear

TWIreadCPU:
;on entry, Y-reg must point to the data-destination
call TWIchkCPU
cpi r24,$C0
breq TWIreadCPU01 ;Test bit6,7 if "1" read Q-contents

sec ;set carry if read failed
ret ;exit, r24 contains the status

TWIreadCPU01:
;save X-register
push XL
push XH
;initalize X-reg. Y-reg must point to destination
ldi XL,low(CPU02Q)
ldi XH,high(CPU02Q)
call TWIq2ram ;copy the data
sts CPU02Q,r13 ;clear CPU-Q/byte0
;restore X-register
pop XH
pop XL
clc
ret

TWIwrdfCPU:
;Write data from flash to the CPU08-Q
;write data to Q. Z-reg points to data source in flash.
call TWIchkCPU
breq TWIwrdfCPU01 ;continue if r24=0

sec
clr r24 ;write failed, carry set, r24:6=0
ret

TWIwrdfCPU01:
;Q is available for write
ldi YL,low(CPU02Q)
ldi YH,high(CPU02Q)
call TWIflash2ram	;this performs the actual copy
;
lds r24,CPU02Q
sbr r24,$40 ;flag data waiting
sts CPU02Q,r24
clc ;write OK, carry clear
ret
 
TWIram2q: ;Block copy routine from Atmel app note AVR102: copy RAM-RAM
;copy from RAM to Q
;Y-reg points to data source
ld r24,Y  ;get byte0 containing length&status bits
inc r24
andi r24,$3f ;mask out status bits, max-len 63 bytes
;
ram2ram1:	;block copy routine
	ld	r25,Y+	;get data from memory
	st	X+,r25	;store data to Q
	dec	r24		;
	brne	ram2ram1	;if not done, loop more
ret

TWIq2ram: ;Block copy routine from Atmel app note AVR102: copy RAM-RAM
;copy from Q to RAM
;Y-reg points to data destination
ld r24,X
inc r24
andi r24,$3f ;mask out status bits, max-len 63 bytes: r24=data-len
;
ram2ram2:	;block copy routine
	ld	r25,X+	;get data from Q
	st	Y+,r25	;store data to memory
	dec	r24		;
	brne	ram2ram2		;if not done, loop more
ret ;All bytes are copied. This includes non-significant bytes 
;The application should be aware of this.


TWIflash2ram:
;This routine is added to enable copying fixed strings from flash
;Use the ram-ram routine afterwards to copy data to the Q
;
;Block copy routine adapted from Atmel app note 102 "flash2ram"
;* This subroutine copies a block of data from the Program memory (Flash) 
;* to the internal SRAM. The following parameters must be set up prior to 
;* calling the subroutine:
;* Z-pointer: 	Flash block start address x 2 (WORD oriented code segment)
;* Y-pointer:	ram block start address
;* flashsize:	block size
;*
;* Number of words	:5 + return
;* Number of cycles	:?
;* Low Registers used	:1 (r11)
;* High Registers used	:1 (flashsize,r16)
;* Pointers used	:Y, Z
;*
;***************************************************************************
;
;***** Subroutine Register variables
.def	flashsize=r16		;size of block to be copied
lpm r16,Z ;first byte equals length
inc r16
andi r16,$3f

flash2ram:
	lpm	r11,Z+		;get byte
	st	Y+,r11		;store in SRAM and increment Y-pointer
	dec	flashsize
	brne	flash2ram	;if not end of table, loop more

ret

TWIpoll: ;Check queues to see if there is data waiting.
lds r25,Qid0
and r25,r25
brne TWIpollend ;only continue when Qid0 is available 

TWIpoll2:
lds r25,pollindex
inc r25
andi r25,$07
sts pollindex,r25
cpi r25,0
breq TWIpoll00 ;SRF08
cpi r25,1
breq TWIpoll01 ;CMPS03
cpi r25,2
breq TWIpoll02 ;LCD32
cpi r25,3
breq TWIpoll03 ;CPU02
;cpi r25,4
;breq TWIpoll04
;cpi r25,5
;breq TWIpoll05
;cpi r25,6
;breq TWIpoll06
;cpi r25,7
;breq TWIpoll07
;repeat cycle
ldi r25,7 ;immediately start a new cycle when the last device was matched. 
sts pollindex,r25

TWIpollend:
ret

TWIpoll00: ;check SRF08
call TWIchkSRF
cpi r24,$40 ;state = data waiting
brne TWIpollend

ldi XL,low(SRF08Q)
ldi XH,high(SRF08Q)
rjmp TWIstartIRQ

TWIpoll01: ;check CMPS03 
call TWIchkCMPS
cpi r24,$40 ;state = data waiting
brne TWIpollend

ldi XL,low(CMPS03Q)
ldi XH,high(CMPS03Q)
rjmp TWIstartIRQ

TWIpoll02: ;check LCD Display
call TWIchkLCD
cpi r24,$40 ;state = data waiting
brne TWIpollend

ldi XL,low(LCD32Q)
ldi XH,high(LCD32Q)
rjmp TWIstartIRQ

TWIpoll03: ;check CPU02Q
call TWIchkCPU
cpi r24,$40 ;state = data waiting
brne TWIpollend

ldi XL,low(CPU02Q)
ldi XH,high(CPU02Q)
rjmp TWIstartIRQ

;other deviceQs here


TWIstartIRQ: ;activate TWI
sts XLtemp2,XL ;save startadress to enable IRQ-writes to the controlbyte
sts XHtemp2,XH
ld r24,X+  ;get byte0, the Qlength
sts XLtemp,XL ;write start+1 as initial pointer for IRQ
sts XHtemp,XH
sts Qlen,r24 ;store Qlength
ld r24,X
sts Qid0,r24 ;flag Qid0=busy
ldi r24,$ff
sts TWIrsctl,r24
;set TWINT, TWEA, TWSTA, TWEN
in r25,TWCR
sbr r25,$E4
out TWCR,r25
ret

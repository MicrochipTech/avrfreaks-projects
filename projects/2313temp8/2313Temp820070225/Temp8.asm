;***************************************************************************
;* AVR Temper8 10MHZ AT90S2313 version  20070225                           *
;* Reads up to eight Dallas DS1820/DS18S20 Temperature sensors connected   *
;* to an AVR port bit. The temperature (degrees C, ASCII, + CRLF) is sent  *
;* out the serial port when a Tn (where n is a single digit from 0 to 7)   *
;* is received by the serial port.                                         *
;* An R character returns a count of the devices found, plus a list of     *
;* their ROM IDs                                                           *
;* Vn (where n is a single digit from 0 to 7) sends the contents of the    *
;* selected DS1820's internal registers (in Hex) and the device's extended *
;* temperature.                                                            *
;* Here's an example:                                                      *
;* command sent : r                                                        *
;* program returns:                                                        *
;* 4                                                                       *
;* 0800080007CC0010                                                        *
;* C000080007728A10                                                        *
;* 0D00080007455E10                                                        *
;* 3300080007B60510                                                        *
;* 0000000000000000                                                        *
;* 0000000000000000                                                        *
;* 0000000000000000                                                        *
;* 0000000000000000                                                        *
;*                                                                         *
;* command sent: t0                                                        *
;* program returns:                                                        *
;* +023.50C                                                                *
;*                                                                         *
;* command sent: v1                                                        *
;* program returns:                                                        *
;* 3300A199FFFF0810C4                                                      *
;* +25.25C                                                                 *
;*                                                                         *
;* 1-Wire port bit:                                                        *
;* Port B bit 2 (PB2) -- The code turns on the pullup feature of the AVR   *
;* port bit used. This is enough to enable the 1-Wire bus.                 *
;* The 4.7K pullup resistor required in most applications (see the DS18S20 *
;* data sheet) can also be used without changing the software.             *
;* Acknowledgments                                                         *
;* This code was ported from my AT90S8515 program, 8515Temp8, which is     *
;* in turn a port from my 68HC11 program, LCDTemper8                       *
;* The ROMSrch (ROM Search) routine was ported from ML6805,a Public Domain *
;* assembly language example from Dallas Semiconductor.                    *
;* It was originally written for the 6805. I also used the code in         *
;* The Dallas/Maxim Application Note 162:                                  *
;* INTERFACING THE DS18X20/DS1822 1-WIRE TEMPERATURE SENSOR IN A           *
;* MICRO-CONTROLLER ENVIRONMENT as a guide.                                *
;* I also borrowed from a few Atmel AVR application notes                  *
;*                                                                         *
;* The DS1820 devices are read only after setup (to read their ROM codes)  *
;* and during a temperature conversion request.                            *
;* Results are sent to the serial port                                     *
;* FH 20030111  First mod of AT90S8515 version                             *
;* FH 20030112  Enabled the internal pullup resistor for Port B pin 2. The *
;*              4.7K pullup resistor on the DS line is no longer needed    *
;* FH 20051012  Fixed a bug that returned nonsense negative temperatures.  *
;*              Improved temperature accuracy. Fixed an output display bug *
;* FH 20070215  Thanks to Stephan Stadler for pointing out an arithmetic   *
;*              error that was producing incorrect temperature readings.   *
;* FH 20070219  Added the Verbose (V) command to display the register      *
;*              contents of the selected device. Changed speed to 10MHZ    *
;* FH 20070225  Updated the extended temperature calculations.             *
;*              The results now match the values shown in the datasheet    *
;*                                                                         *
;* Frank Henriquez frank@ucla.edu                                          *
;* Check http://frank.bol.ucla.edu/index.htm for the latest version        *
;* Please do not delete any of the text in this comment box.               *
;* Please do not redistribute modified versions of this code               *
;***************************************************************************

.include "2313def.inc"

;**********************************************
;* Equates                                    *
;**********************************************

.equ    clock       = 10000000 ;10MHZ clock frequency 
.equ    baudrate    = 9600     ;baudrate is 9600
.equ    baudconstant= (clock/(16*baudrate))-1

.equ    STACKTOP    = RAMEND - 100 

;**********************************************
;* Registers used                             *
;**********************************************

.def    Done        = r10      ;used in GetROMs
.def    ROMCount    = r11      ;number of devices found
.def    SaveSREG    = r12      ;used in Delay
.def    BitNum      = r13      ;used in ROMsrch
.def    LastDis     = r14      ;used in ROMsrch
.def    LasDisCpy   = r15      ;used in ROMsrch

.def    AReg        = r16      ;working reg, like the 68HC11 A register
.def    BReg        = r17      ;working reg, like the 68HC11 B register
.def    ScratchL    = r18
.def    ScratchH    = r19
.def    ByteCount   = r20      ;used in ShowROM
.def    Count8      = r21
.def    DelayVal    = r22
.def    MpyLo       = r24
.def    MpyHi       = r25


;**********************************************
;* DS1820 Command bytes                       * 
;**********************************************
.equ    ReadROM     = $33
.equ    MatchROM    = $55
.equ    SkipROM     = $cc
.equ    SearchROM   = $f0
.equ    SearchAlarm = $ec

.equ    ConvertTemp = $44
.equ    CpyScratch  = $48
.equ    WrScratch   = $4e
.equ    RdScratch   = $be
.equ    RecallEE    = $b8
.equ    ReadPS      = $84

;**********************************************
;* The following delay constants are based on * 
;* a 10MHZ clock (CLK)                        *
;* times are not exact.                       *
;**********************************************

.equ    d500us  = $b2          ;500us CLK/64
.equ    d100us  = $83          ;100us CLK/8
.equ    d44us   = $ca          ;44us  CLK/8
.equ    d15us   = $6b          ;15us  CLK/1
.equ    d2us    = $ec          ;2us   CLK/1


.equ    TSTOP   =0             ;Stop Timer/Counter
.equ    TCK1    =1             ;Timer/Counter runs from CK
.equ    TCK8    =2             ;Timer/Counter runs from CK / 8
.equ    TCK64   =3             ;Timer/Counter runs from CK / 64
.equ    TCK256  =4             ;Timer/Counter runs from CK / 256
.equ    TCK1024 =5             ;Timer/Counter runs from CK / 1024
.equ    TEXF    =6             ;Timer/Counter runs from external falling edge
.equ    TEXR    =7             ;Timer/Counter runs from external rising edge

.equ    DSBus   =2             ;PORTB bit used for the 1-wire bus


;**********************************************
;* 100 bytes of internal data RAM are used by *
;* the program                                *
;**********************************************
.dseg
.org RAMEND - 99 
                  
Buffer:        .byte 16
;**********************************************
;* 9 bytes below are the register contents    *
;* of the last DS18S20 read                   *
;**********************************************
TempLSB:       .byte 1
TempMSB:       .byte 1
THUB1:         .byte 1
TLUB2:         .byte 1
Res1:          .byte 1
Res2:          .byte 1
CountRem:      .byte 1
CntPerC:       .byte 1
CRC:           .byte 1

ExtTemp:       .byte 1
romoff:        .byte 2         ;Pointer to Sensor address         
romdta:        .byte 8         ;ROM address of urrent device
Sensor0:       .byte 8         ;DS1820 sensor 0 ID #
Sensor1:       .byte 8         ;DS1820 sensor 1 ID #
Sensor2:       .byte 8         ;DS1820 sensor 2 ID #
Sensor3:       .byte 8         ;DS1820 sensor 3 ID #
Sensor4:       .byte 8         ;DS1820 sensor 4 ID #
Sensor5:       .byte 8         ;DS1820 sensor 5 ID #
Sensor6:       .byte 8         ;DS1820 sensor 6 ID #
Sensor7:       .byte 8         ;DS1820 sensor 7 ID #


.cseg
.org    $000                   ;Reset Vector
        rjmp    Reset
   
 ;Interrupt vectors

.org    INT0addr               ;External Interrupt0 Vector
        reti
.org    INT1addr               ;External Interrupt1 Vector
        reti
.org    ICP1addr               ;Input Capture1 Interrupt Vector
        reti
.org	OC1addr                ;Output Compare1 Interrupt Vector Address
        reti
.org    OVF1addr               ;Overflow1 Interrupt Vector
        reti
.org    OVF0addr               ;Overflow0 Interrupt Vector
        rjmp    Tim0int
.org    URXCaddr               ;UART Receive Complete Interrupt Vector
        rjmp    ParseIn
.org    UDREaddr               ;UART Data Register Empty Interrupt Vector
        reti
.org    UTXCaddr               ;UART Transmit Complete Interrupt Vector
        reti
.org    ACIaddr                ;Analog Comparator Interrupt Vector
        reti
    

Reset:
        ldi    AReg,low(STACKTOP)
        out    SPL,AReg        ;set low byte for stack pointer
        rcall  Setup           ;set up DS1820 bits, Serial I/O

RPLoop:
        rjmp   RPLoop          ;wait here until something interesting happens
            
;********************************************** 
;* Setup                                      *
;* Initialize the 1-Wire bus  & serial port   *
;**********************************************
Setup:               
;** serial port setup **
        ldi    AReg,baudconstant     
        out    UBRR,AReg       ;load baudrate 
;**enable UART receiver & transmitter and IRQ on serial in:
        ldi    AReg, (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)
        out    UCR,AReg        

        ldi    AReg,TSTOP      ;Timer 0 off (just in case)
        out    TCCR0,AReg      ;Stop timer
        ldi    AReg,0b00000010 ;Enable Timer 0 interrupt
        out    TIMSK,AReg

        sei                    ;enable interrupts
        rcall  GetROMS
        ret

;**********************************************
;* DS1820 Routines                            *
;**********************************************

;**********************************************
;* DSReset                                    *
;* Sends a reset pulse to all devices.        *
;* Returns carry set if devices present.      *
;* Returns carry clear if no devices present. *
;**********************************************
DSReset: 
        sbi    DDRB,DSBus      ;DDRB bit 2 = 1 = output
        cbi    PORTB,DSBus     ;set 1-wire bus low
		ldi    ScratchH,TCK64  ;load prescaler
        ldi    DelayVal,d500us
        rcall  Delay           ;Wait 500us
        cbi    DDRB,DSBus      ;Switch DS bus bit to input (pulled up high)
        sbi    PORTB,DSBus     ;turn on pullup
 		ldi    ScratchH,TCK8  ;load prescaler
        ldi    DelayVal,d100us
        rcall  Delay           ;Wait 100 us for the DS1820 to respond
        clc                    ;exit with carry clear if no device
        sbis   PINB,DSBus      ;skip next if port B bit 2=1
        sec                    ;bit was clear (=0) so set carry flag  
		ldi    ScratchH,TCK64  ;load prescaler
        ldi    DelayVal,d500us
        rcall  Delay           ;Wait 500 us
        ret
                                
;**********************************************
;* GetBit                                     *
;* Carry holds the bit from the DS1820 bus    *
;**********************************************
GetBit:
 		ldi    ScratchH,TCK1   ;load prescaler
        ldi    DelayVal,d2us   ;wait for the 1820 data
        sbi    DDRB,DSBus      ;DDRB bit 2 = 1 = output
        cbi    PORTB,DSBus     ;set 1-wire bus low
        rcall  Delay
        cbi    DDRB,DSBus      ;DDRB bit 2 = 0 = input
        sbi    PORTB,DSBus     ;turn on pullup
  		ldi    ScratchH,TCK1   ;load prescaler
        ldi    DelayVal,d15us  ;wait 15us for the 1820 data
        rcall  Delay
        clc                    ;assume 1-Wire bus is low
        sbic   PINB,DSBus
        sec                    ;1-Wire bus is high, so set carry flag 
  		ldi    ScratchH,TCK8   ;load prescaler
        ldi    DelayVal,d44us
        rcall  Delay
        ret
        
;**********************************************
;* Write bit in CC to DS bus                  *
;**********************************************
PutBit:
        sbi    DDRB,DSBus      ;DDRB bit 2 = 1 = output
        cbi    PORTB,DSBus     ;Pull Port B, bit 2 low
 		ldi    ScratchH,TCK1   ;load prescaler
        ldi    DelayVal,d15us  ;wait for the 1820 sample window
        rcall  Delay           
        brcc   Put0            ;if carry = 0, send a 0
        cbi    DDRB,DSBus      ;otherwise, send a 1
        sbi    PORTB,DSBus     ;turn on pullup
Put0:
 		ldi    ScratchH,TCK8   ;load prescaler
        ldi    DelayVal,d44us  ;load 44us delay in delay register
        rcall  Delay           ;and wait
        cbi    DDRB,DSBus      ;this signals the end of a write 0
        sbi    PORTB,DSBus     ;turn on pullup
 		ldi    ScratchH,TCK1   ;load prescaler
        ldi    DelayVal,d2us   ;let the DS1820 rest & digest
        rcall  Delay
        ret

;**********************************************
;* GetByte                                    *
;* AReg returns the byte from the DS1820 bus  *
;**********************************************
GetByte:
        ldi    BReg,8          ;bit count
        clr    AReg
NxtBit:
        rcall  GetBit
        ror    AReg            ;shift carry bit into AReg
        dec    BReg
        brne   NxtBit          ;repeat for all 8 bits
        ret

;**********************************************
;* PutByte                                    *
;* data in AReg                               *
;**********************************************
PutByte:
        ldi    BReg,8          ;bit counter
ShiftB:
        lsr    AReg            ;shift bits in AReg into carry
        rcall  PutBit
NextB:
        dec    BReg
        brne   ShiftB
        ret

;**********************************************
;* Search for devices on DS bus               *
;* Based on Dallas Semiconductor's ML6805     *
;* code.                                      *
;**********************************************
ROM1st:
        ldi    AReg,$41        ;Point Rom Search to top (1st part).
        mov    LastDis,AReg
        rcall  ROMSrch
        ret

ROMSrch:
        push   XL
        push   XH

        rcall  DSReset         ;Reset 1-wire bus.
        brcs   r_look
        rjmp   NoDev
r_look:
        ldi    AReg,$40        ;Prepare to find 64 bits of ROM data.
        mov    BitNum,AReg
        ldi    Count8,8        ;Initial value for eight counter.
        mov    LasDisCpy, LastDis ;Save copy of last dis. from last search.
        ldi    XL,low(romdta)
        ldi    XH,high(romdta) ;point to 1st byte of ROM data
        sts    romoff,XL       ;buffer in rombuff.
        sts    romoff+1,XH
        ldi    AReg,SearchROM  ;Get ROM search command in AReg.
        rcall  PutByte         ;Send ROM search command to DOW bus.

NxtRBit:
        clr    AReg
        clr    BReg
        rcall  GetBit          ;1st read time slot.
        rol    BReg            ;reg BReg, bit0 has first result
        rcall  GetBit          ;2nd read time slot.
        rol    AReg
        cpi    BReg,1          ;Was the first bit received 1?
        brne   no_or           ;Branch if it wasn't
        ori    AReg,2          ;Set bit 1 in AReg

no_or:
        cpi    AReg,3          ;See if error condition occurred.
        breq   NoDev           ;Abort if error.
        cpi    AReg,0          ;Was there a disagreement on bus.
        brne   nosave          ;Branch if there wasn't.
        cp     LasDisCpy,BitNum ;Compare last dis to current position.        
        brne   lwhere          ;If its not equal look some more.
        ldi    AReg,1          ;We've been on this path and sent a 0.
        rjmp   wrtBit          ;now lets send a 1.

lwhere:
        brcs   sLstBit         ;Stay on previous path.
        clr    AReg            ;Past last dis. fall to the right.
        rjmp   savedis         ;Send 0 to bus.
sLstBit:
        lds    XL,romoff       ;Get offset of ROM data buffer in
        lds    XH,romoff+1  
        ld     AReg,X          ;index register, and byte in AReg.
        andi   AReg,1          ;Isolate lsb of AReg.
        cpi    AReg,1          ;See if it was a 1.
        breq   wrtBit          ;Don't save as dis position if so.
savedis: 
        mov    LastDis,BitNum  ;Get current position in x register.
                               ;Save current last disagreement pos.
        rjmp   wrtBit          ;Send the write bit time slot.
nosave:
        ldi    BReg,1
        eor    AReg,BReg       ;Flip lsb of AReg.
wrtBit:
        andi   AReg,1          ;Isolate lsb of AReg
        mov    BReg,AReg
        lsr    BReg
        rcall  PutBit          ;Send lsb of AReg to 1-wire bus.
        lds    XL,romoff 
        lds    XH,romoff+1     ;Get offset into ROM data buffer.
        ld     AReg,X          ;Get previous val of rombyte.
        ror    AReg            ;Get ROM bit in MSB of rombyte.
        st     X,AReg          ;Store new byte value & point to next byte 
        dec    Count8          ;Decrement byte counter.
        brne   bnfull          ;Branch if we don't have full byte.
        adiw   XH:XL,1         ;increment x
        sts    romoff,XL       ;buffer in rombuff.
        sts    romoff+1,XH
        ldi    Count8,8        ;reload the eight counter.
bnfull:
        dec    BitNum          ;Decrement 64 bit counter.
        brne   NxtRBit         ;Go get next rom bit.
        cp     LasDisCpy,LastDis ;Compare to last dis. copy
        brne   MoDows          ;If not equal clear AReg.
        ldi    AReg,$41        ;Point ROM search algorithm back
        mov    LastDis,AReg    ;to first part.
        ldi    AReg,1          ;Indicate this was last part on bus.
        rjmp   lastdow         ;Skip the clra statement and leave.
MoDows:
        clr    AReg            ;Indicate that more parts are on bus.        
lastdow:
        sec                    ;Indicate success.
        rjmp   DevFnd          ;Skip clear carry stuff.
NoDev:
        clc                    ;Indicate failure.
DevFnd:
        pop    XH
        pop    XL
        ret

;********************************************** 
;* GetROMS                                    *
;* Looks for up to 8 DS1820s, reads their     *
;* 64 bit ROM codes and saves them in memory  *
;**********************************************
GetROMS:
        clr    AReg
        clr    ROMCount        ;clear out ROMCount
        clr    Done            ;clear Done flag
        ldi    YH,high(romdta)
        ldi    YL,low(romdta)  ;Y points to romdta buffer
ClrOut:
        st     Y+,AReg         ;clear out the ROM ID storage area
        cpi    YL,low(Sensor7+8)
        brcs   ClrOut
LookROM:
        rcall  DSReset         ;Reset 1-wire bus
;*** to send out the list of ROMs found, uncomment the line below
        brcc   SetExit         ;Abort if no presence detect
;*** to NOT send out the list of ROMs found, uncomment the line below
;        brcc   SetExit2       ;Abort if no presence detect 

        rcall  ROM1st          ;look for the first ROM
        mov    ScratchL,AReg
        brcc   SetExit         ;if AReg = 0 then there are more devices
        ldi    XH,high(Sensor0)
        ldi    XL,low(Sensor0) ;X points to first device ID  
        rjmp   MR0             ;process them & exit
LR1:
        rcall  ROMSrch         ;look for other devices
        mov    ScratchL,AReg
        breq   MR0             ;if AReg = 0 then there are more devices
        inc    Done
MR0:
        ldi    YH,high(romdta+8)
        ldi    YL,low(romdta+8) ;Y points to ROM just read
MoveROM:
        ld     AReg,-Y         ;move byte from romdta
        st     X+,AReg         ;to SensorX
        cpi    YL,low(romdta)  ;move all 8 bytes
        brne   MoveROM         ;of the ROM ID
        inc    ROMCount        ;increment the ROM counter
        tst    ScratchL        ;repeat until the last device found
        breq   MR2
        inc    Done
MR2:
        tst    Done
        brne   SetExit
        cpi    XL,low(Sensor7+8) ;or until the end of the ROM
        brcs   LR1             ;storage area is reached                     
SetExit:
        rcall  ShowROM         ;then Display the ROM IDs
SetExit2:
        ret

;********************************************** 
;* ShowROM                                    *
;* Converts data in the Sensor area to ASCII  *
;* hex and send them out the serial port.     *
;**********************************************
ShowROM:
        ldi    XL,low(Sensor0) ;X points to Sensor ID area
        ldi    XH,high(Sensor0)
SRLoop:
        ldi    YL,low(Buffer)  ;Y points to ASCII data buffer
        ldi    YH,high(Buffer)
        ldi    ByteCount,8
ROMHex:
        ld     AReg,X+         ;convert 8 bytes to 16 ASCII Hex
        rcall  Put1Hex         ;characters. Y is incremented
        dec    ByteCount
        brne   ROMHex       

        ldi    YL,low(Buffer) 
        ldi    YH,high(Buffer)
ROMOut:
        ld     AReg,Y+         ;send characters in the buffer     
        rcall  SerOut          ;to the serial port

        cpi    YL,low(Buffer+16)
        brcs   ROMOut
        clr    AReg
        st     Y,AReg
 
        rcall  OutCRLF        
        cpi    XL,low(Sensor7+8) ;repeat for all eight ID locations
        brcs   SRLoop               
SRBye:
        rcall  OutCRLF
        ret

;********************************************** 
;* ShowData                                   *
;* Converts data in the register area to ASCII*
;* hex and send them out the serial port.     *
;**********************************************
ShowData:
        ldi    XL,low(TempLSB) ;X points to data area
        ldi    XH,high(TempLSB)
SDLoop:
        ldi    YL,low(Buffer)  ;Y points to ASCII data buffer
        ldi    YH,high(Buffer)
        ldi    ByteCount,8
DatHex:
        ld     AReg,X+         ;convert 8 bytes to 16 ASCII Hex
        rcall  Put1Hex         ;characters. Y is incremented
        dec    ByteCount
        brne   DatHex       

        ldi    YL,low(Buffer) 
        ldi    YH,high(Buffer)
DatOut:
        ld     AReg,Y+         ;send characters in the buffer     
        rcall  SerOut          ;to the serial port

        cpi    YL,low(Buffer+16)
        brcs   DatOut
		                       ;not enough RAM to display 18 bytes
							   ;so we display the 2 ASCII bytes for the CRC
							   ;in a separate operation 

        lds    AReg,CRC        ;AReg=CRC
        ldi    YL,low(Buffer)  ;Y points to ASCII data buffer
        ldi    YH,high(Buffer)
        rcall  Put1Hex         ;convert to hex 
        lds    AReg,Buffer     ;send characters in the buffer     
        rcall  SerOut          ;to the serial port
        lds    AReg,Buffer+1    
        rcall  SerOut
				           
        rcall  OutCRLF
        ret

;********************************************** 
;* ROMatch                                    *
;* Sends the MatchROM command, followed by    *
;* the 8 byte ROM ID                          * 
;**********************************************
ROMatch:
        lds    XL,romoff       ;point to device ROM
        lds    XH,romoff+1
        adiw   XH:XL,8         ;point to MSB                             
        ldi    Count8,8
        ldi    AReg,MatchROM   ;Match ROM command
        rcall  PutByte         ;send it
SndROM:
        ld     AReg,-X         ;send the ROM ID
        rcall  PutByte
        dec    Count8
        brne   SndROM 
        ret     

;**********************************************
;* Serial I/O and misc routines               *
;**********************************************

;**********************************************
;* Timer0 overflow interrupt handler          *
;**********************************************
Tim0int:
        cli                    ;disable interrupts
        set                    ;Set T flag
        ldi     ScratchH,TSTOP ;Stop Timer 0
        out     TCCR0,ScratchH
        sei                    ;re-enable interrupts
        reti                    

;**********************************************
;* Delay   time in us based on CLK value      *
;* DelayVal holds the delay value.            *
;* ScratchH has the prescaler value           *
;**********************************************
Delay:
        in      SaveSREG,SREG  ;Save status register
        out     TCNT0,DelayVal
        clt                    ;Clear T
        out     TCCR0,ScratchH ;Run timer
Dwait:  
        brtc    Dwait          ;Wait for timer 0 interrupt to set T
        out     SREG,SaveSREG
        ret

;**********************************************
;* SerOut  Data in AReg sent out the serial   *
;* port                                       *
;**********************************************
SerOut: 
        sbis    USR,UDRE       ;wait till register is cleared 
        rjmp    SerOut         ;not skipped, loop back and wait
        out     UDR,AReg       ;send the recived value 
        ret
 
;**********************************************
;* SerIn  Returns serial port input in AReg  *
;**********************************************
SerIn:
        sbis    USR,RXC        ;wait for a character
        rjmp    SerIn   
        in      AReg,UDR       ;read value
        ret

;**********************************************
;* OutCRLF Sends a CRLF out the serial port   *
;**********************************************
OutCRLF:
        ldi    AReg,$0d        ;CR
        rcall  SerOut
        ldi    AReg,$0a        ;LF
        rcall  SerOut
        ret

;********************************************** 
;* GetRegs                                    *
;* Reads the registers of the DS1820/18S20    *
;* device selected in A.                      * 
;* Result stored in the AReg storage area     *
;**********************************************
GetRegs:
        sei
        rcall  DSReset
        ldi    XL,low(Sensor0)
        ldi    XH,high(Sensor0)
        
GR0:
        mov    BReg,AReg       ;copy device # to BReg
        lsl    BReg            ;BReg x 2
        lsl    BReg            ;BReg x 4
        lsl    BReg            ;BReg x 8
        adc    XL,BReg
        brcc   GR1   
        inc    XH              ;carry set, so add to XH and point to device
GR1:
        sts    romoff,XL
        sts    romoff+1,XH     ;used to point to Temp result
        rcall  ROMatch         ;find it on the bus
        ldi    AReg,ConvertTemp ;convert Temperature command
        rcall  PutByte  
WaiTemp:
        rcall  GetBit          ;wait while busy           
        brcc   WaiTemp         ;DS18S20 will pull bus low while busy
        rcall  DSReset         ;reset bus
        rcall  ROMatch         ;select device again
        
        ldi    AReg,RdScratch  ;read scratchpad command
        rcall  PutByte
        ldi    XL,low(TempLSB) ;point to Scratchpad storage
        ldi    XH,high(TempLSB)  
RdMem:                         ;read in the 1820 scratchpad memory  
        rcall  GetByte         ;read result
        st     X+,AReg         ;ScratchL byte
        cpi    XL,low(TempLSB+9) ;read 9 bytes
        brcs   RdMem

 	    lds    AReg,TempMSB    ;Look at TempMSB
 	    sbrs   AReg,0          ;if bit 0 (or any bit) is 0
	    rjmp   GExit           ;then it's a positive temperature
	    lds    AReg,TempLSB    ;otherwise, it's a negative temp
	    neg    AReg            ;so do a 2's complement of the temp
	    sts    TempLSB,AReg    ;and save before continuing
GExit:
        cli
        ret 
		      
;********************************************** 
;* GetTemp                                    *
;* Reads the extended Temperatures from the   *
;* DS1820/18S20 device selected in A.         * 
;* Result stored in the AReg storage area     *
;**********************************************
GetTemp:
        rcall  GetRegs
;Get Temp falls through to calculate the extended temperature value

;***************************************************************************
;* CvtExt  converts the extended temperature into degrees C                *
;* In the DS18S20 data sheet, this formula is given as:                    *
;* Temp = Temp_Read - 0.25 + ( count_per_c - count_remain)/count_per_c     *
;* This can be simplified. In the DS18S20, count_per_c is always 16        *
;* Temp = Temp_Read - 0.25 + (16-count_remain)/16                          *
;* Multiply everything by 16 to simplify the code:                         *
;* 16*Temp = 16*Temp_Read - 4 + (16-count_remain) which becomes:           *
;* 16*Temp = 16*Temp_Read + 12 - count_remain                              *
;* Multiplying everything by 25 and dividing by 4 eliminates fixed point   *
;* numbers. The result is a 16 bit number with the temperature in the      *
;* MSB and the fractional (extended) temperature in the LSB                *
;* Convert the data in TempLSB to ASCII, add a decimal point in the output *
;* string, then convert ExtTemp  to ASCII                                  *
;***************************************************************************
CvtExt:
        ldi    ScratchL,8      ;TempLSB * 8 is equivalent to 
		clr    ScratchH        ;(TempLSB * 16) then shifted to the right  
		lds    MpyLo,TempLSB   ;once to eliminate the 0.5C bit
		clr    MpyHi
		rcall  mpy16u          ;16 x 16 multiply

		mov    MpyHi,ScratchH  ;16 bit multiply result in MpyHi:MpyLo
		mov    MpyLo,ScratchL

        lds    ScratchL,TempMSB
 		sbrc   ScratchL,0      ;skip next instruction if TempMSB bit 0 is positive
		sen                    ;TempMSB was negative, so set the negative flag
		adiw   MpyHi:MpyLo,12  ;16*TempLSB + 12
 
        lds    ScratchL,CountRem
		lds    ScratchH,TempMSB;Get the high order Temperature byte
		sbrc   ScratchH,0      ;skip next instruction if TempMSB bit 0 is positive
		sen                    ;TempMSB was negative, so set the negative flag
        sub    MpyLo,ScratchL  ;subtract count remaining from low byte of Temp result
		brcc   CE1
        dec    MpyHi           ;there was a carry, so subtract it from the MSB
CE1:
        ldi    ScratchL,25     ;multiply result by 25: 25*(16*TempLSB + 12 - CountRemain)
		clr    ScratchH        ;
		rcall  mpy16u          ;result is a 16 bit number.
        		
		lsr    ScratchH       ;divided by 2
		ror    ScratchL
CE13:
		lsr    ScratchH       ;divided by 4
		ror    ScratchL

 		sts    TempLSB,ScratchH  ;store temperature result
		sts    ExtTemp,ScratchL  ;store extended result

        ret

;**********************************************
;* TempCvt                                    *
;* converts the Temperature in degrees C into *
;* an ASCII string                            *
;**********************************************  
TempCvt:
        ldi    YL, low(Buffer)
        ldi    YH,high(Buffer)

        lds    AReg,TempMSB    ;Get the high order Temperature byte
        tst    AReg            ;Test high order byte
        brmi   TS0             ;ones indicate a negative Temperature
        ldi    AReg,'+'
        st     Y+,AReg         ;Save a + into the buffer
        rjmp   Cvt2BCD         
TS0:
        ldi    AReg,'-'
        st     Y+,AReg         ;Save a - into the buffer

Cvt2BCD:
        lds    BReg,TempLSB    ;convert temperature to 
        lds    AReg,ExtTemp    ;a packed BCD
        rcall  BIN2BCD16
        mov    AReg,r14        ;then convert the resulting 4 digits
		swap   AReg
        rcall  Put1Asc         ;to 4 ASCII characters
        mov    AReg,r14
        rcall  Put1Asc
        ldi    AReg,'.'        ;put a decimal point in the buffer
        st     Y+,AReg
        mov    AReg,r13
		swap   AReg
        rcall  Put1Asc
        mov    AReg,r13
		rcall  Put1Asc
        ldi    AReg,'C'  
        st     Y+,AReg

        clr    AReg
        st     Y,AReg
        ret

;**********************************************
;* "mpy16u" 16x16 Bit Unsigned Multiplication *
;* This subroutine multiplies the two 16-bit  *
;* register variables                         * 
;* ScratchH:ScratchL and BReg:AReg.           *
;* The result is placed in:                   *
;* r21:r20:ScratchH:ScratchL.                 *
;* The following registers are used:          *
;* MpyLo     ;multiplicand low byte           *   
;* MpyHi     ;multiplicand high byte          *
;* ScratchL ;multiplier low byte              *
;* ScratchH ;multiplier high byte             *
;* ScratchL ;result byte 0 (LSB)              *
;* ScratchH ;result byte 1                    *
;* r20      ;result byte 2                    *
;* r21      ;result byte 3 (MSB)              *
;* r22      ;loop counter                     *
;* Based on Atmel Application Note AVR 200    *
;**********************************************  
mpy16u:	
        clr    r21             ;clear 2 highest bytes of result
        clr    r20
        ldi    r22,16          ;init loop counter
        lsr    ScratchH
        ror    ScratchL

m16u_1:
        brcc   noad8           ;if bit 0 of multiplier set
        add    r20,MpyLo       ;add multiplicand Low to byte 2 of res
        adc    r21,MpyHi       ;add multiplicand high to byte 3 of res
noad8:
        ror    r21             ;shift right result byte 3
        ror    r20             ;rotate right result byte 2
        ror    ScratchH        ;rotate result byte 1 and multiplier High
        ror    ScratchL        ;rotate result byte 0 and multiplier Low
        dec    r22             ;decrement loop counter
        brne   m16u_1          ;if not done, loop more
        ret

;**********************************************
;* Bin2BCD16,16-bit Binary to BCD conversion  *
;* converts a 16 bit binary number to         *
;* a 5 digit packed BCD number                *
;* number to convert in BReg:AReg             *
;* returns MSB in ScratchH, LSB in ScratchL   *
;* The following registers are used:          *
;* r13 BCD value digits 1 and 0               *
;* r14 BCD value digits 3 and 2               *
;* r15 BCD value digit 4                      *
;* AReg r16 binary value Low byte             *
;* BReg r17 binary value High byte            *
;* ScratchL r18 loop counter                  *
;* ScratchH r19 temporary value               *
;* Based on Atmel Application Note AVR 204    *
;**********************************************  

.equ	AtBCD0	=13		;address of r13
.equ	AtBCD2	=15		;address of r14

bin2BCD16:
        ldi    ScratchL,16     ;Init loop counter	
        clr    r15             ;clear result (3 bytes)
        clr    r14		
        clr    r13		
        clr    ZH              ;clear ZH (not needed for AT90Sxx0x)
bBCDx_1:
        lsl    AReg            ;shift input value
        rol    BReg            ;through all bytes
        rol    r13
        rol    r14
        rol    r15
        dec    ScratchL        ;decrement loop counter
        brne   bBCDx_2         ;if counter not zero
        ret                    ;routine exit point

bBCDx_2:
        ldi    r30,AtBCD2+1    ;Z points to result MSB + 1
bBCDx_3:
        ld     ScratchH,-Z     ;get (Z) with pre-decrement
        subi   ScratchH,-$03   ;add 0x03
        sbrc   ScratchH,3      ;if bit 3 not clear
        st     Z,ScratchH      ;store back
        ld     ScratchH,Z      ;get (Z)
        subi   ScratchH,-$30   ;add 0x30
        sbrc   ScratchH,7      ;if bit 7 not clear
        st     Z,ScratchH      ;store back
        cpi    ZL,AtBCD0       ;done all three?
        brne   bBCDx_3         ;loop again if not
        rjmp   bBCDx_1		
  
;**********************************************
;* Convert the LSB nibble of AReg to an ASCII *
;* Hex character and stores it in the buffer  *
;* addressed by Y. Exit with Y pointing to    *
;* the next empty buffer location             *
;**********************************************
Put1Asc:
        andi   AReg,$0f
        cpi    AReg,$0a
        brmi   PTASC1
        ldi    BReg,7
        add    AReg,BReg
PTASC1:
        ldi    BReg,$30
        add    AReg,BReg
        st     Y+,AReg
        ret

;**********************************************
;* Converts the contents of AReg to 2 ASCII   *
;* Hex characters, storing them in the buffer *
;* addressed by Y. Exit with Y pointing to    *
;* the next empty buffer location             *
;**********************************************
Put1Hex:
        mov    r0,AReg
        lsr    AReg
        lsr    AReg
        lsr    AReg
        lsr    AReg
        rcall  Put1Asc
        mov    AReg,r0
        rcall  Put1Asc
        ret

;**********************************************
;* ParseIn  Scans incoming serial port data   *
;* and parses the commands:                   *
;* R: Outputs ROM count & ROM IDs to ser port *
;* Tn : Outputs the Temperature for sensor n  *
;* Vn : Outputs the register contents and     *
;*      temperature for sensor n              *
;**********************************************
ParseIn:
        cli                    ;block off interrupts
 
        rcall  SerIn           ;look at the incoming character
        andi   AReg,$5f        ;convert to uppercase
        cpi    AReg,'R'        ;ROM IDs found        
        brne   VData
         
        mov    AReg,ROMCount   ;it was an R - send # of devices found
        ldi    BReg,$30
        add    AReg,BReg       ;convert to an ASCII number
        rcall  SerOut          ;send it out the serial port
        rcall  OutCRLF
        rcall  ShowROM         ;send the ROM ID array out the serial port
        rjmp   PExit

VData:
        cpi    AReg,'V'        ;Verbose - sends DS1820 register contents & temp
        brne   TTest           ;exit if it isn't a Verbose Data command

        rcall  SerIn           ;read number of device
        subi   AReg,$30        ;convert it to binary
        cp     AReg,ROMCount   ;make sure it's <= to the device count
        brcc   PExit           ;exit otherwise

        rcall  GetRegs         ;read in the register values
        rcall  ShowData        ;convert it to ASCII and send out the serial port
        rcall  CvtExt          ;calculate the extended temperature value 
 		rjmp   TT1             ;finish off by displaying the temperature

TTest:  
        cpi    AReg,'T'        ;Calculate and send temperatire 
        brne   PExit

        rcall  SerIn           ;read number of device
        subi   AReg,$30        ;convert it to binary
        cp     AReg,ROMCount   ;make sure it's <= to the device count
        brcc   PExit           ;exit otherwise
        rcall  GetTemp         ;Get the extended temperature and
TT1:    
        rcall  TempCvt         ;convert it to ASCII

        ldi    XL,low(Buffer)  ;point to the converted Scratch buffer
        ldi    XH,high(Buffer) 
SenDat:
        ld     AReg,X+
        tst    AReg            ;output strings are NULL terminated
        breq   SDEnd           ;end of buffer; send a CRLF
        rcall  SerOut          ;send the buffer data until a 0 is detected
        rjmp   SenDat 
 
SDEnd:  
        rcall  OutCRLF
PExit:
        sei                    ;enable interrupts
        reti
    
;**********************************************
;* Code ends here.                            *
;**********************************************

;..VB6 RVK-BASIC Rev. 07.04.0043
;..8:17:27 AM  10/6/2005
;..founded on RVK Basic rev: 021113.0-rvk
.include "M88DEF.INC"
.def DUM_VAR        =r24 ;..register variable..DUM_VAR
.def RX             =r0  ;..register variable..RX
.def RY             =r1  ;..register variable..RY
.def RW             =r2  ;..register variable..RW
;***START OF INTERRUPT VECTOR TABLE********************
RJMP     RST_000              ;..reset location
RETI                          ;..ext interrupt0
RETI                          ;..ext interrupt1
RETI                          ;..Pin Change Interrupt Request 0
RETI                          ;..Pin Change Interrupt Request 1
RETI                          ;..Pin Change Interrupt Request 2
RETI                          ;..Watchdog Time-out Interrupt
RETI                          ;..timer2 compare A
RETI                          ;..timer2 compare B
RETI                          ;..timer2 overflow
RETI                          ;..timer1 capture
RETI                          ;..timer1 compare A
RETI                          ;..timer1 compare b
RETI                          ;..timer1 overflow
RETI                          ;..timer0 compare A
RETI                          ;..timer0 compare B
RETI                          ;..timer0 overflow
RETI                          ;..SPI Transfer Complete
RETI                          ;..USART RX complete..
RETI                          ;..USART Data Reg empty..
RETI                          ;..USART TX complete..
RETI                          ;..ADC complete
RETI                          ;..EEPROM ready..
RETI                          ;..analog comp...
RETI                          ;..2-wire serial interface
RETI                          ;..Store Program Memory Ready
;***END OF INTERRUPT VECTOR TABLE********************
;.......................................
RST_000:           ;...main program begins here..
  CLR  R31     
  OUT  ACSR,R31    ;..power up anlg comp...
  OUT  DDRB,R31
  OUT  DDRC,R31
  OUT  DDRD,R31
  LDI  R31,low(RAMEND)
  LDI  R30,high(RAMEND)
  OUT  SPL,R31     ;..init stack ptr..
  OUT  SPH,R30
 ;================START OF PROGRAM===============================
 ;                    Device ATmega88
 ;                    Mhz 3.686
 ;                    
 ;                    Revision   Mega88Spi.bas
 ;                    'This runs on the STK500
 ;                    'Connect the PortC to the Leds
 ;                    'Connect the DataFlash per instructions in the STK500 documentation
 ;                    
 ;                    Dim rX              As Register
 ;                    Dim rY              As Register
 ;                    Dim rW              As Register
 ;                    Dim cSCK            As Constant = 5
 ;                    Dim cMISO           As Constant = 4
 ;                    Dim cMOSI           As Constant = 3
 ;                    Dim cSS             As Constant = 2
 ;                    Dim WriteBuffer1    As Constant = &h84
 ;                    Dim ReadBuffer1     As Constant = &hd4
 ;                    Dim Buff1ToMemory   As Constant = &h83
 ;                    Dim ReadStatus      As Constant = &hd7
 ;                    Dim MemToBuffer1    As Constant = &h53
 ;                    Equ "B,cSS", FlashCS
 ;                    
 ;                    rW=0
  LDI  R26, LOW($0)
  LDI  R27,HIGH($0)
  MOV  RW,R26
 ;                    
 ;                    Dirport C,Out
  LDI  R26, LOW($FF)
  OUT  DDRC,R26
 ;                    Outport C,&hff
  LDI  R26, LOW($FF)
  LDI  R27,HIGH($FF)
  OUT  PORTC,R26
 ;                    
 ;                    Spi hardware Init ,master,high,setup,16 ',[clock rate] default is clk/128
  LDI  R26,(1<<PORTB3) | (1<<PORTB5) | (1<<PORTB2)
  OUT  DDRB,R26
  IN   R26,SPCR
  IN   R27,SPSR
  CBR  R27,(1<<SPI2X)
  OUT  SPSR,R27
  SBR  R26,(1<<CPHA) | (1<<CPOL) | (1<<SPR0) | (1<<SPR1) | (1<<MSTR) | (1<<SPE)
  OUT  SPCR,R26
 ;                    Setbit  FlashCS           'Disable CS
  SBI  PORTB,2
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    
 ;                    'Write 16 bytes to the external Flash buffer
 ;                    Clrbit FlashCS          'Enable CS
  CBI  PORTB,2
 ;                    Spi hardware Out WriteBuffer1
  LDI  R26, LOW($84)
  OUT  SPDR,R26
SPI01:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI01
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    For rX= 1 To 3          '24 bits of address
  LDI  R26, LOW($1)
  MOV  RX,R26
FOR01B:
  LDI  R28, LOW($3)
  MOV  R26,RX
  CP   R28,R26
  BREQ FOR02
  BRCC FOR02
  RJMP FOR01E
FOR02:
 ;                    Spi hardware Out 0
  LDI  R26, LOW($0)
  OUT  SPDR,R26
SPI02:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI02
 ;                    Next
FOR01N:
  INC  RX
  RJMP FOR01B
FOR01E:
 ;                    For rX= 1 To &h10
  LDI  R26, LOW($1)
  MOV  RX,R26
FOR04B:
  LDI  R28, LOW($10)
  MOV  R26,RX
  CP   R28,R26
  BREQ FOR05
  BRCC FOR05
  RJMP FOR04E
FOR05:
 ;                    Spi hardware Out rX
  MOV  R26,RX
  OUT  SPDR,R26
SPI03:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI03
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    Next
FOR04N:
  INC  RX
  RJMP FOR04B
FOR04E:
 ;                    Setbit FlashCS          'Disable CS
  SBI  PORTB,2
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    
 ;                    'write buffer 1 to memory
 ;                    Clrbit FlashCS            'Enable CS
  CBI  PORTB,2
 ;                    Spi hardware Out Buff1ToMemory
  LDI  R26, LOW($83)
  OUT  SPDR,R26
SPI04:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI04
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    For rX=1 To 3         '24 bits of addres
  LDI  R26, LOW($1)
  MOV  RX,R26
FOR07B:
  LDI  R28, LOW($3)
  MOV  R26,RX
  CP   R28,R26
  BREQ FOR08
  BRCC FOR08
  RJMP FOR07E
FOR08:
 ;                    Spi hardware Out 0
  LDI  R26, LOW($0)
  OUT  SPDR,R26
SPI05:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI05
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    Next
FOR07N:
  INC  RX
  RJMP FOR07B
FOR07E:
 ;                    Setbit  FlashCS           'Disable CS
  SBI  PORTB,2
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    
 ;                    'read status to determine that programing is done
 ;                    Clrbit FlashCS            'Enable CS
  CBI  PORTB,2
 ;                    Spi hardware Out ReadStatus
  LDI  R26, LOW($D7)
  OUT  SPDR,R26
SPI06:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI06
 ;                    ry=0
  LDI  R26, LOW($0)
  LDI  R27,HIGH($0)
  MOV  RY,R26
 ;                    While rY < &h7f
DL001B:
  LDI  R26, LOW($7F)
  MOV  R28,RY
  CP   R26,R28 
  BRLO IF001E
  RJMP IF001X
IF001E:
  RJMP DL001E
IF001X:
 ;                    Spi hardware Out 0
  LDI  R26, LOW($0)
  OUT  SPDR,R26
SPI07:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI07
 ;                    Spi hardware In rY
SPI08:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI08
  IN   R26,SPDR
  MOV  RY,R26
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    Loop
DL001L:
  RJMP DL001B
DL001E:
 ;                    Setbit  FlashCS           'Disable CS
  SBI  PORTB,2
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    
 ;                    'read memory to buffer 1
 ;                    Clrbit FlashCS            'Enable CS
  CBI  PORTB,2
 ;                    Spi hardware Out MemToBuffer1
  LDI  R26, LOW($53)
  OUT  SPDR,R26
SPI09:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI09
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    For rX=1 To 3         '24 bits of addres
  LDI  R26, LOW($1)
  MOV  RX,R26
FOR10B:
  LDI  R28, LOW($3)
  MOV  R26,RX
  CP   R28,R26
  BREQ FOR11
  BRCC FOR11
  RJMP FOR10E
FOR11:
 ;                    Spi hardware Out 0
  LDI  R26, LOW($0)
  OUT  SPDR,R26
SPI10:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI10
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    Next
FOR10N:
  INC  RX
  RJMP FOR10B
FOR10E:
 ;                    Setbit  FlashCS           'Disable CS
  SBI  PORTB,2
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    
 ;                    'read status to determine main memory to buffer xfer done
 ;                    Clrbit FlashCS            'Enable CS
  CBI  PORTB,2
 ;                    Spi hardware Out ReadStatus
  LDI  R26, LOW($D7)
  OUT  SPDR,R26
SPI11:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI11
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    ry=0
  LDI  R26, LOW($0)
  LDI  R27,HIGH($0)
  MOV  RY,R26
 ;                    While rY < &h7f
DL002B:
  LDI  R26, LOW($7F)
  MOV  R28,RY
  CP   R26,R28 
  BRLO IF004E
  RJMP IF004X
IF004E:
  RJMP DL002E
IF004X:
 ;                    Spi hardware Out 0
  LDI  R26, LOW($0)
  OUT  SPDR,R26
SPI12:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI12
 ;                    Spi hardware In rY
SPI13:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI13
  IN   R26,SPDR
  MOV  RY,R26
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    Loop
DL002L:
  RJMP DL002B
DL002E:
 ;                    Setbit  FlashCS           'Disable CS
  SBI  PORTB,2
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    
 ;                    Do
DL003B:
 ;                    'Read 16 bytes back from external Flash buffer
 ;                    Clrbit FlashCS        'Enable CS
  CBI  PORTB,2
 ;                    Spi hardware Out ReadBuffer1
  LDI  R26, LOW($D4)
  OUT  SPDR,R26
SPI14:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI14
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    For rX=1 To 3         '24 bits of addres
  LDI  R26, LOW($1)
  MOV  RX,R26
FOR13B:
  LDI  R28, LOW($3)
  MOV  R26,RX
  CP   R28,R26
  BREQ FOR14
  BRCC FOR14
  RJMP FOR13E
FOR14:
 ;                    Spi hardware Out 0
  LDI  R26, LOW($0)
  OUT  SPDR,R26
SPI15:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI15
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    Next
FOR13N:
  INC  RX
  RJMP FOR13B
FOR13E:
 ;                    For rX = 0 To &hf
  LDI  R26, LOW($0)
  MOV  RX,R26
FOR16B:
  LDI  R28, LOW($F)
  MOV  R26,RX
  CP   R28,R26
  BREQ FOR17
  BRCC FOR17
  RJMP FOR16E
FOR17:
 ;                    Spi hardware Out &Hff
  LDI  R26, LOW($FF)
  OUT  SPDR,R26
SPI16:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI16
 ;                    Spi hardware In rY
SPI17:
  IN   R24,SPSR
  SBRS R24,SPIF
  RJMP SPI17
  IN   R26,SPDR
  MOV  RY,R26
 ;                    'displaying them on the LEDs
 ;                    Pause 500
  LDI  R26, LOW($1)
  MOV  DUM_VAR,R26
FOR19B:
  LDI  R28, LOW($7)
  MOV  R26,DUM_VAR
  CP   R28,R26
  BREQ FOR20
  BRCC FOR20
  RJMP FOR19E
FOR20:
  LDI  R31,5
  LDI  R30,0
  RCALL PSE_00
FOR19N:
  INC  DUM_VAR
  RJMP FOR19B
FOR19E:
  LDI  R31,3
  LDI  R30,134
  RCALL PSE_00
  LDI  R31,1
  LDI  R30,233
  RCALL PSE_00
 ;                    rW=rW Xor &hff
  MOV  R26,RW
  CLR  R27
  LDI  R24, LOW($FF)
  LDI  R25,HIGH($FF)
  EOR  R26,R24
  EOR  R27,R25
  MOV  RW,R26
 ;                    Outport C,rY
  OUT  PORTC,RY
 ;                    If rW = 0 Then
  LDI  R26, LOW($0)
  MOV  R28,RW
  CPSE R26,R28
  RJMP IF007E
 ;                    Setbit C,5
  SBI  PORTC,5
 ;                    Else
  RJMP IF007X
IF007E:
 ;                    Clrbit C,5
  CBI  PORTC,5
 ;                    End If
IF007X:
 ;                    Next
FOR16N:
  INC  RX
  RJMP FOR16B
FOR16E:
 ;                    Setbit FlashCS        'Disable CS
  SBI  PORTB,2
 ;                    Pause .01
  LDI  R31,1
  LDI  R30,237
  RCALL PSE_00
 ;                    Loop
DL003L:
  RJMP DL003B
DL003E:
 ;                    
 ;                    End
END01:
  RJMP END01
 ;===================END OF MAIN PROGRAM=================
PSE_00:
  ;..pause subroutine
  OUT  TCCR0B,R31
  LDI  R31,1
  OUT  TCNT0,R31
  OUT  TIFR0,R31
  OUT  TCNT0,R30
PSE_01:
  IN   R31,TIFR0
  ANDI  R31,1
  BREQ PSE_01
  RET
 ;===================MEMORY DEFINITIONS=================
 .dseg
 .eseg
 .exit

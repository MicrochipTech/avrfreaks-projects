;---------------------------------------------------------------------------
; avr2313.inc - Include file based on 2313def.inc from Atmel, but with
;  additional comments and other handy reference material.
;                ____  ____
;       /Reset  |1   __  20| Vcc
;      rxd,pd0  |2       19| pb7,sck
;      txd,pd1  |3       18| pb6,miso
;    xtal2,out  |4       17| pb5,mosi
;     xtal1,in  |5       16| pb4
;     int0,pd2  |6       15| pb3,oc1
;     int1,pd3  |7       14| pb2
;       t0,pd4  |8       13| pb1,ain1
;       t1,pd5  |9       12| pb0,ain0
;          Gnd  |10      11| pd6,icp
;                ----------
;---------------------------------------------------------------------------

;***** Specify Device
.device AT90S2313

;***** Data Memory layout:
; 00-1FH, 32 Working registers(last 6 are XL,XH,YL,YH,ZL,ZH special)
; 20-5FH, 64 I/O registers for peripheral configuration(see below)
; 60-DFH, 128 RAM for stack and application variables
;    note register names are in decimal, so r16 is at 10H hexidecimal.

;***** Program Memory layout:
; 000-3FFH, flash memory

;***** I/O Register Definitions, $20 to $5f,
; io instructions use relative values to start of IO map in Data Memory
; see pg 60 for summary(!!!pg60, NOTE LEFT COLOUMN CONTAINS PAGE INDEXES!!!)
.equ  SREG   =$3f ; status register
.equ  SPL    =$3d ; stack pointer(this is critical!)(p13)
.equ  GIMSK  =$3b ; general interrupt mask reg
.equ  GIFR   =$3a ; general interrupt flag reg
.equ  TIMSK  =$39 ; timer Interrupt mask reg
.equ  TIFR   =$38 ; timer interrupt flag reg
.equ  MCUCR  =$35 ; MCU general control reg
.equ  MCUSR  =$34
.equ  TCCR0  =$33 ; timer0 control reg
.equ  TCNT0  =$32 ; timer0, 8-bit
.equ  TCCR1A =$2f ; timer1, control reg A
.equ  TCCR1B =$2e ; timer1, control reg B
.equ  TCNT1H =$2d ; timer1, high byte
.equ  TCNT1L =$2c ; timer1, low byte
.equ  OCR1AH =$2b ; output compare reg high byte
.equ  OCR1AL =$2a ; output compare reg low byte
.equ  ICR1H  =$25 ; timer1 input capture reg high byte
.equ  ICR1L  =$24 ; timer1 input capture reg low byte
.equ  WDTCR  =$21 ; watchdog timer control reg
.equ  EEAR   =$1e ; eeprom address reg
.equ  EEARL  =$1e ; 
.equ  EEDR   =$1d ; eeprom data reg
.equ  EECR   =$1c ; eeprom control reg
.equ  PORTB  =$18 ; port b(pb7..pb0)
.equ  DDRB   =$17 ; data direction reg, port b
.equ  PINB   =$16 ; input pins, port b
.equ  PORTD  =$12 ; data reg, port d(pd6..pd0)
.equ  DDRD   =$11 ; data direction reg, port d
.equ  PIND   =$10 ; input pins, port d
.equ  UDR    =$0c ; uart io data reg
.equ  USR    =$0b ; uart status reg
.equ  UCR    =$0a ; uart control reg
.equ  UBRR   =$09 ; uart baud reg
.equ  ACSR   =$08 ; analog comparator control and status reg

;***** Bit Definitions

;---  SREG   =$3f ; status register
.equ  _SRGIE =7 ; global interrupt enable
.equ  _SRBCS =6 ; bit copy storage(see BLD, BST instr)
.equ  _SRHCF =5 ; half carry flag
.equ  _SRS   =4 ; sign bit, xor of N & Z
.equ  _SRV   =3 ; Two's complement overflow
.equ  _SRN   =2 ; negative
.equ  _SRZ   =1 ; zero
.equ  _SRC   =0 ; carry

;--- SPL    =$3d ; stack pointer, p13
;  you need to initialize this to the top of RAM, since
;  the stack works down into ram as we push things on the stack,
;  you can use the RAMEND literal as end of ram.
.equ  SP7  =7
.equ  SP6  =6
.equ  SP5  =5
.equ  SP4  =4
.equ  SP3  =3
.equ  SP2  =2
.equ  SP1  =1
.equ  SP0  =0

;--- GIMSK  =$3b ; general interrupt mask reg, p17 
.equ  INT1  =7 ; external interrupt request 1 enable
.equ  INT0  =6 ; external interrupt request 0 enable

;--- GIFR   =$3a ; general interrupt flag reg, p18
.equ  INTF1  =7 ; external inter. flag1, r/w
.equ  INTF0  =6 ; external inter. flag0. r/w

;--- TIMSK  =$39 ; timer Interrupt mask reg, p18
.equ  TOIE1  =7 ; timer1 overflow interrupt enable, r/w
.equ  OCIE1A =6 ; timer1 output compare match, r/w
.equ  TICIE  =3 ; timer1 input capture interrupt enable, r/w
.equ  TOIE0  =1 ; timer0 overflow interrupt enable, r/w

;--- TIFR   =$38 ; timer interrupt flag reg, p18
.equ  TOV1  =7 ; timer1 overflow flag r/w
.equ  OCF1A =6 ; output compare flag 1A
.equ  ICF1  =3 ; input capture flag 1
.equ  TOV0  =1 ; timer0 overflow flag 1

;--- MCUCR  =$35 ; MCU general control reg, p19
.equ  SE    =5 ; Sleep Enable
.equ  SM    =4 ; Sleep Mode
.equ  ISC11 =3 ; Interrupt Sense Control 1 bit 1
.equ  ISC10 =2 ; Interrupt Sense Control 1 bit 0
.equ  ISC01 =1 ; Interrupt Sense Control 0 bit 1
.equ  ISC00 =0 ; Interrupt Sense Control 0 bit 0

.equ  EXTRF =1
.equ  PORF  =0

;--- TCCR0  =$33 ; timer/counter 0 control reg, p22
.equ  CS02  =2 ; clock select bit 2
.equ  CS01  =1 ; clock select bit 1
.equ  CS00  =0 ; clock select bit 0
; where 0=stop, 1=CK, 2=CK/8, 3=CK/64, 4=CK/256, 5=CK/1024,
;   6=ext pin T0-falling 7=ext pin T0-rising

;--- TCNT0  =$32 ; timer/counter 0, 8-bit
 ; byte value of timer

;---  TCCR1A =$2f ; timer/counter 1, control reg A
.equ  COM1A1 =7 ; Compare output mode1, bit 1
.equ  COM1A0 =6 ; Compare output mode1, bit 0
  ; where pin OC1, 0=disconnect, 1-toggle, 2=clear, 3=set
.equ  PWM11  =1 ; pulse width modulation select bit 1
.equ  PWM10  =0 ; pulse width modulation select bit 0
  ; where 0=disable, 1=timer1 8bit, 2=timer1 9bit, 3=timer1 10bit

;---  TCCR1B =$2e ; timer/counter 1, control reg B
.equ  ICNC1  =7 ; input capture noise canceler(4 CKs)
.equ  ICES1  =6 ; input capture edge select 0=falling edge, 1=rising
.equ  CTC1  =3 ; clear timer1 on compare match
.equ  CS12  =2 ; clock select1 bit 2(prescaler source of timer1)
.equ  CS11  =1 ; clock select1 bit 1
.equ  CS10  =0 ; clock select1 bit 0
; where 0=stop, 1=CK, 2=CK/8, 3=CK/64, 4=CK/256, 5=CK/1024,
;   6=ext pin T1-falling 7=ext pin T1-rising

;--- TCNT1H =$2d ; timer/counter 1, high byte, pg25
;--- TCNT1L =$2c ; timer/counter 1, low byte

;--- OCR1AH =$2b ; output compare reg high byte
;--- OCR1AL =$2a ; output compare reg low byte

;--- ICR1H  =$25 ; timer 1 input capture reg high byte
;--- ICR1L  =$24 ; timer 1 input capture reg low byte

;--- WDTCR  =$21 ; watchdog timer control reg, p28
.equ  WDTOE =4 ; watchdog turnoff enable
.equ  WDE   =3 ; watchdog enable
.equ  WDP2  =2 ; watchdog timer prescaler bit 2
.equ  WDP1  =1 ; watchdog timer prescaler bit 1
.equ  WDP0  =0 ; watchdog timer prescaler bit 0
; where WDP# 0=16K, 1=32K, 2=64K, 3=128K, 4=256K, 5=512K, 6=1024K, 7=2048K

;--- EEAR   =$1e ; eeprom address reg
;--- EEDR   =$1d ; eeprom data reg

;--- EECR   =$1c ; eeprom control reg
.equ  EEMWE =2 ; eeprom master write enable
.equ  EEWE  =1 ; eeprom write enable
.equ  EERE  =0 ; eeprom read enable

;--- PORTB  =$18 ; port b(pb7..pb0), pg38
.equ  PB7  =7 ; pin19, SCK, serial clock for SPI
.equ  PB6  =6 ; pin18, MISO, data out for SPI
.equ  PB5  =5 ; pin17, MOSI, data in for SPI
.equ  PB4  =4 ; pin16, 
.equ  PB3  =3 ; pin15, OC1, Timer1 output compare match output
.equ  PB2  =2 ; pin14, 
.equ  PB1  =1 ; pin13, AIN1, analog comparator negative input
.equ  PB0  =0 ; pin12, AIN0, analog comparator positive input

;--- DDRB   =$17 ; data direction reg, port b, pg38
.equ  DDB7  =7
.equ  DDB6  =6
.equ  DDB5  =5
.equ  DDB4  =4
.equ  DDB3  =3
.equ  DDB2  =2
.equ  DDB1  =1
.equ  DDB0  =0
  ; where DDBx=0 PORTBx=0 ---> Tri State
  ;       DDBx=0 PORTBx=1 ---> Will source current if ext. pulled low
  ;       DDBx=1 PORTBx=0 ---> push pull zero output
  ;       DDBx=1 PORTBx=1 ---> push pull one output

;--- PINB   =$16 ; input pins, port b
.equ  PINB7  =7
.equ  PINB6  =6
.equ  PINB5  =5
.equ  PINB4  =4
.equ  PINB3  =3
.equ  PINB2  =2
.equ  PINB1  =1
.equ  PINB0  =0

;--- PORTD  =$12 ; data reg, port d(pd6..pd0), pg43
.equ  PD6  =6 ; pin 11, ICP, timer1 input capture pin
.equ  PD5  =5 ; pin 9, T1, timer1 external input
.equ  PD4  =4 ; pin 8, T0, timer0 external input
.equ  PD3  =3 ; pin 7, INT1, ext. interrupt1 input
.equ  PD2  =2 ; pin 6, INT0, ext. interrupt0 input
.equ  PD1  =1 ; pin 3, TXD, transmit data output to UART
.equ  PD0  =0 ; pin 2, RXD, receive data input to UART

;--- DDRD   =$11 ; data direction reg, port d
.equ  DDD6  =6
.equ  DDD5  =5
.equ  DDD4  =4
.equ  DDD3  =3
.equ  DDD2  =2
.equ  DDD1  =1
.equ  DDD0  =0
  ; where DDDx=0 PORTDx=0 ---> Tri State
  ;       DDDx=0 PORTDx=1 ---> Will source current if ext. pulled low
  ;       DDDx=1 PORTDx=0 ---> push pull zero output
  ;       DDDx=1 PORTDx=1 ---> push pull one output


;--- PIND   =$10 ; input pins, port d
.equ  PIND6  =6
.equ  PIND5  =5
.equ  PIND4  =4
.equ  PIND3  =3
.equ  PIND2  =2
.equ  PIND1  =1
.equ  PIND0  =0

;--- UDR    =$0c ; uart io data reg. pg33

;--- USR    =$0b ; uart status reg
.equ  RXC  =7 ; uart receive complete
.equ  TXC  =6 ; uart transmit complete
.equ  UDRE =5 ; uart tx data holding register empty
.equ  FE   =4 ; framing error
.equ  OVR   =3 ; overrun

;--- UCR    =$0a ; uart control reg
.equ  RXCIE =7  ; rx complete interrupt enable
.equ  TXCIE =6  ; tx complete interrupt enable
.equ  UDRIE =5  ; uart data reg empty interrupt enable
.equ  RXEN  =4  ; receiver enable
.equ  TXEN  =3  ; transmitter enable
.equ  CHR9  =2  ; 9bit chars
.equ  RXB8  =1  ; receive data bit 8
.equ  TXB8  =0  ; transmit data bit 8

;--- UBRR   =$09 ; uart baud reg
  ; where baud = clk / (UBRR + 1)

;--- ACSR   =$08 ; analog comparator control and status reg,pg36
.equ  ACD   =7 ; analog comparator disable
.equ  ACO   =5 ; analog comparator output
.equ  ACI   =4 ; analog comparator input flag
.equ  ACIE  =3 ; analog comparator input enable
.equ  ACIC  =2 ; analog comparator capture enable
.equ  ACIS1 =1 ; analog comparator interrupt mode bit 1
.equ  ACIS0 =0 ; analog comparator interrupt mode bit 0
  ; where 0=interrupt on toggle, 1=reserved, 2=interrupt on falling, 3=rising

;--- special indexing registers
.def  XL  =r26
.def  XH  =r27
.def  YL  =r28
.def  YH  =r29
.def  ZL  =r30
.def  ZH  =r31

.equ    RAMEND  =$df    ;Last On-Chip SRAM Location

.equ  INT0addr=$001  ;External Interrupt0 Vector Address
.equ  INT1addr=$002  ;External Interrupt1 Vector Address
.equ  ICP1addr=$003  ;Input Capture1 Interrupt Vector Address
.equ  OC1addr =$004  ;Output Compare1 Interrupt Vector Address
.equ  OVF1addr=$005  ;Overflow1 Interrupt Vector Address
.equ  OVF0addr=$006  ;Overflow0 Interrupt Vector Address
.equ  URXCaddr=$007  ;UART Receive Complete Interrupt Vector Address
.equ  UDREaddr=$008  ;UART Data Register Empty Interrupt Vector Address
.equ  UTXCaddr=$009  ;UART Transmit Complete Interrupt Vector Address
.equ  ACIaddr =$00a  ;Analog Comparator Interrupt Vector Address

;--------------- Instruction Summary
;ARITHMETIC AND LOGIC INSTRUCTIONS
;ADD Rd, Rr Add two Registers Rd ¬ Rd + Rr Z,C,N,V,H 1
;ADC Rd, Rr Add with Carry two Registers Rd ¬ Rd + Rr + C Z,C,N,V,H 1
;ADIW Rdl,K Add Immediate to Word Rdh:Rdl ¬ Rdh:Rdl + K Z,C,N,V,S 2
;SUB Rd, Rr Subtract two Registers Rd ¬ Rd - Rr Z,C,N,V,H 1
;SUBI Rd, K Subtract Constant from Register Rd ¬ Rd - K Z,C,N,V,H 1
;SBIW Rdl,K Subtract Immediate from Word Rdh:Rdl ¬ Rdh:Rdl - K Z,C,N,V,S 2
;SBC Rd, Rr Subtract with Carry two Registers Rd ¬ Rd - Rr - C Z,C,N,V,H 1
;SBCI Rd, K Subtract with Carry Constant from Reg. Rd ¬ Rd - K - C Z,C,N,V,H 1
;AND Rd, Rr Logical AND Registers Rd ¬ Rd · Rr Z,N,V 1
;ANDI Rd, K Logical AND Register and Constant Rd ¬ Rd · K Z,N,V 1
;OR Rd, Rr Logical OR Registers Rd ¬ Rd v Rr Z,N,V 1
;ORI Rd, K Logical OR Register and Constant Rd ¬ Rd v K Z,N,V 1
;EOR Rd, Rr Exclusive OR Registers Rd ¬ Rd Å Rr Z,N,V 1
;COM Rd One’s Complement Rd ¬ $FF - Rd Z,C,N,V 1
;NEG Rd Two’s Complement Rd ¬ $00 - Rd Z,C,N,V,H 1
;SBR Rd,K Set Bit(s) in Register Rd ¬ Rd v K Z,N,V 1
;CBR Rd,K Clear Bit(s) in Register Rd ¬ Rd · ($FF - K) Z,N,V 1
;INC Rd Increment Rd ¬ Rd + 1 Z,N,V 1
;DEC Rd Decrement Rd ¬ Rd - 1 Z,N,V 1
;TST Rd Test for Zero or Minus Rd ¬ Rd · Rd Z,N,V 1
;CLR Rd Clear Register Rd ¬ Rd Å Rd Z,N,V 1
;SER Rd Set Register Rd ¬ $FF None 1
;BRANCH INSTRUCTIONS
;RJMP k Relative Jump PC ¬ PC + k + 1 None 2
;IJMP Indirect Jump to (Z) PC ¬ Z None 2
;RCALL k Relative Subroutine Call PC ¬ PC + k + 1 None 3
;ICALL Indirect Call to (Z) PC ¬ Z None 3
;RET Subroutine Return PC ¬ STACK None 4
;RETI Interrupt Return PC ¬ STACK I 4
;CPSE Rd,Rr Compare, Skip if Equal if (Rd = Rr) PC ¬ PC + 2 or 3 None 1 / 2
;CP Rd,Rr Compare Rd - Rr Z, N,V,C,H 1
;CPC Rd,Rr Compare with Carry Rd - Rr - C Z, N,V,C,H 1
;CPI Rd,K Compare Register with Immediate Rd - K Z, N,V,C,H 1
;SBRC Rr, b Skip if Bit in Register Cleared if (Rr(b)=0) PC ¬ PC + 2 or 3 None 1 / 2
;SBRS Rr, b Skip if Bit in Register is Set if (Rr(b)=1) PC ¬ PC + 2 or 3 None 1 / 2
;SBIC P, b Skip if Bit in I/O Register Cleared if (P(b)=0) PC ¬ PC + 2 or 3 None 1 / 2
;SBIS P, b Skip if Bit in I/O Register is Set if (R(b)=1) PC ¬ PC + 2 or 3 None 1 / 2
;BRBS s, k Branch if Status Flag Set if (SREG(s) = 1) then PC¬PC + k + 1 None 1 / 2
;BRBC s, k Branch if Status Flag Cleared if (SREG(s) = 0) then PC¬PC + k + 1 None 1 / 2
;BREQ k Branch if Equal if (Z = 1) then PC ¬ PC + k + 1 None 1 / 2
;BRNE k Branch if Not Equal if (Z = 0) then PC ¬ PC + k + 1 None 1 / 2
;BRCS k Branch if Carry Set if (C = 1) then PC ¬ PC + k + 1 None 1 / 2
;BRCC k Branch if Carry Cleared if (C = 0) then PC ¬ PC + k + 1 None 1 / 2
;BRSH k Branch if Same or Higher if (C = 0) then PC ¬ PC + k + 1 None 1 / 2
;BRLO k Branch if Lower if (C = 1) then PC ¬ PC + k + 1 None 1 / 2
;BRMI k Branch if Minus if (N = 1) then PC ¬ PC + k + 1 None 1 / 2
;BRPL k Branch if Plus if (N = 0) then PC ¬ PC + k + 1 None 1 / 2
;BRGE k Branch if Greater or Equal, Signed if (N Å V= 0) then PC ¬ PC + k + 1 None 1 / 2
;BRLT k Branch if Less Than Zero, Signed if (N Å V= 1) then PC ¬ PC + k + 1 None 1 / 2
;BRHS k Branch if Half Carry Flag Set if (H = 1) then PC ¬ PC + k + 1 None 1 / 2
;BRHC k Branch if Half Carry Flag Cleared if (H = 0) then PC ¬ PC + k + 1 None 1 / 2
;BRTS k Branch if T Flag Set if (T = 1) then PC ¬ PC + k + 1 None 1 / 2
;BRTC k Branch if T Flag Cleared if (T = 0) then PC ¬ PC + k + 1 None 1 / 2
;BRVS k Branch if Overflow Flag is Set if (V = 1) then PC ¬ PC + k + 1 None 1 / 2
;BRVC k Branch if Overflow Flag is Cleared if (V = 0) then PC ¬ PC + k + 1 None 1 / 2
;BRIE k Branch if Interrupt Enabled if (I = 1) then PC ¬ PC + k + 1 None 1 / 2
;BRID k Branch if Interrupt Disabled if (I = 0) then PC ¬ PC + k + 1 None 1 / 2
;-----------------------------------------------------------------
;Mnemonics Operands Description Operation Flags #Clocks
;DATA TRANSFER INSTRUCTIONS
;MOV Rd, Rr Move Between Registers Rd ¬ Rr None 1
;LDI Rd, K Load Immediate Rd ¬ K None 1
;LD Rd, X Load Indirect Rd ¬ (X) None 2
;LD Rd, X+ Load Indirect and Post-Inc. Rd ¬ (X), X ¬ X + 1 None 2
;LD Rd, - X Load Indirect and Pre-Dec. X ¬ X - 1, Rd ¬ (X) None 2
;LD Rd, Y Load Indirect Rd ¬ (Y) None 2
;LD Rd, Y+ Load Indirect and Post-Inc. Rd ¬ (Y), Y ¬ Y + 1 None 2
;LD Rd, - Y Load Indirect and Pre-Dec. Y ¬ Y - 1, Rd ¬ (Y) None 2
;LDD Rd,Y+q Load Indirect with Displacement Rd ¬ (Y + q) None 2
;LD Rd, Z Load Indirect Rd ¬ (Z) None 2
;LD Rd, Z+ Load Indirect and Post-Inc. Rd ¬ (Z), Z ¬ Z+1 None 2
;LD Rd, -Z Load Indirect and Pre-Dec. Z ¬ Z - 1, Rd ¬ (Z) None 2
;LDD Rd, Z+q Load Indirect with Displacement Rd ¬ (Z + q) None 2
;LDS Rd, k Load Direct from SRAM Rd ¬ (k) None 2
;ST X, Rr Store Indirect (X) ¬ Rr None 2
;ST X+, Rr Store Indirect and Post-Inc. (X) ¬ Rr, X ¬ X + 1 None 2
;ST - X, Rr Store Indirect and Pre-Dec. X ¬ X - 1, (X) ¬ Rr None 2
;ST Y, Rr Store Indirect (Y) ¬ Rr None 2
;ST Y+, Rr Store Indirect and Post-Inc. (Y) ¬ Rr, Y ¬ Y + 1 None 2
;ST - Y, Rr Store Indirect and Pre-Dec. Y ¬ Y - 1, (Y) ¬ Rr None 2
;STD Y+q,Rr Store Indirect with Displacement (Y + q) ¬ Rr None 2
;ST Z, Rr Store Indirect (Z) ¬ Rr None 2
;ST Z+, Rr Store Indirect and Post-Inc. (Z) ¬ Rr, Z ¬ Z + 1 None 2
;ST -Z, Rr Store Indirect and Pre-Dec. Z ¬ Z - 1, (Z) ¬ Rr None 2
;STD Z+q,Rr Store Indirect with Displacement (Z + q) ¬ Rr None 2
;STS k, Rr Store Direct to SRAM (k) ¬ Rr None 2
;LPM Load Program Memory R0 ¬ (Z) None 3
;IN Rd, P In Port Rd ¬ P None 1
;OUT P, Rr Out Port P ¬ Rr None 1
;PUSH Rr Push Register on Stack STACK ¬ Rr None 2
;POP Rd Pop Register from Stack Rd ¬ STACK None 2
;BIT AND BIT-TEST INSTRUCTIONS
;SBI P,b Set Bit in I/O Register I/O(P,b) ¬ 1 None 2
;CBI P,b Clear Bit in I/O Register I/O(P,b) ¬ 0 None 2
;LSL Rd Logical Shift Left Rd(n+1) ¬ Rd(n), Rd(0) ¬ 0 Z,C,N,V 1
;LSR Rd Logical Shift Right Rd(n) ¬ Rd(n+1), Rd(7) ¬ 0 Z,C,N,V 1
;ROL Rd Rotate Left Through Carry Rd(0)¬C,Rd(n+1)¬ Rd(n),C¬Rd(7) Z,C,N,V 1
;ROR Rd Rotate Right Through Carry Rd(7)¬C,Rd(n)¬ Rd(n+1),C¬Rd(0) Z,C,N,V 1
;ASR Rd Arithmetic Shift Right Rd(n) ¬ Rd(n+1), n=0..6 Z,C,N,V 1
;SWAP Rd Swap Nibbles Rd(3..0)¬Rd(7..4),Rd(7..4)¬Rd(3..0) None 1
;BSET s Flag Set SREG(s) ¬ 1 SREG(s) 1
;BCLR s Flag Clear SREG(s) ¬ 0 SREG(s) 1
;BST Rr, b Bit Store from Register to T T ¬ Rr(b) T 1
;BLD Rd, b Bit load from T to Register Rd(b) ¬ T None 1
;SEC Set Carry C ¬ 1 C 1
;CLC Clear Carry C ¬ 0 C 1
;SEN Set Negative Flag N ¬ 1 N 1
;CLN Clear Negative Flag N ¬ 0 N 1
;SEZ Set Zero Flag Z ¬ 1 Z 1
;CLZ Clear Zero Flag Z ¬ 0 Z 1
;SEI Global Interrupt Enable I ¬ 1 I 1
;CLI Global Interrupt Disable I ¬ 0 I 1
;SES Set Signed Test Flag S ¬ 1 S 1
;CLS Clear Signed Test Flag S ¬ 0 S 1
;SEV Set Twos Complement Overflow V ¬ 1 V 1
;CLV Clear Twos Complement Overflow V ¬ 0 V 1
;SET Set T in SREG T ¬ 1 T 1
;CLT Clear T in SREG T ¬ 0 T 1
;SEH Set Half Carry Flag in SREG H ¬ 1 H 1
;CLH Clear Half Carry Flag in SREG H ¬ 0 H 1
;NOP No Operation None 1
;SLEEP Sleep (see specific descr. for Sleep function) None 3
;WDR Watchdog Reset (see specific descr. for WDR/timer) None 1
;-------------------------------------------------------
;Order Info:
;4Mhz  2.7 - 6.0V AT90S2313-4PC 20P3 Commercial
;                 AT90S2313-4SC 20S  (0C to 70C)
;
;                 AT90S2313-4PI 20P3 Industrial
;                 AT90S2313-4SI 20S  (-40C to 85C)
;
;10Mhz 4.0 - 6.0V AT90S2313-10PC 20P3 Commercial
;                 AT90S2313-10SC 20S  (0C to 70C)
;
;                 AT90S2313-10PI 20P3 Industrial
;                 AT90S2313-10SI 20S  (-40C to 85C)
;----------------------------------------------------------
;AT90S2313 Register Summary
;Address Name Bit 7 Bit 6 Bit 5 Bit 4 Bit 3 Bit 2 Bit 1 Bit 0 Page
;$3F ($5F) SREG I T H S V N Z C 17
;$3E ($5E) Reserved
;$3D ($5D) SPL SP7 SP6 SP5 SP4 SP3 SP2 SP1 SP0 18
;$3C ($5C) Reserved
;$3B ($5B) GIMSK INT1 INT0 - - - - - -23
;$3A ($5A) GIFR INTF1 INTF0 23
;$39 ($59) TIMSK TOIE1 OCIE1A - - TICIE1 - TOIE0 -23
;$38 ($58) TIFR TOV1 OCF1A - -ICF1 -TOV0 -24
;$37 ($57) Reserved
;$36 ($56) Reserved
;$35 ($55) MCUCR - - SE SM ISC11 ISC10 ISC01 ISC00 25
;$34 ($54) Reserved
;$33 ($53) TCCR0 - - - - - CS02 CS01 CS00 28
;$32 ($52) TCNT0 Timer/Counter0 (8 Bit) 29
;$31 ($51) Reserved
;$30 ($50) Reserved
;$2F ($4F) TCCR1A COM1A1 COM1A0 - - - -PWM11 PWM10 30
;$2E ($4E) TCCR1B ICNC1 ICES1 . - CTC1 CS12 CS11 CS10 31
;$2D ($4D) TCNT1H Timer/Counter1 - Counter Register High Byte 32
;$2C ($4C) TCNT1L Timer/Counter1 - Counter Register Low Byte 32
;$2B ($4B) OCR1AH Timer/Counter1 - Compare Register High Byte 32
;$2A ($4A) OCR1AL Timer/Counter1 - Compare Register Low Byte 32
;$29 ($49) Reserved
;$28 ($48) Reserved
;$27 ($47) Reserved
;$26 ($46) Reserved
;$25 ($45) ICR1H Timer/Counter1 - Input Capture Register High Byte 33
;$24 ($44) ICR1L Timer/Counter1 - Input Capture Register Low Byte 33
;$23 ($43) Reserved
;$22 ($42) Reserved
;$21 ($41) WDTCR - - - WDTOE WDE WDP2 WDP1 WDP0 35
;$20 ($40) Reserved
;$1F ($3F) Reserved
;$1E ($3E) EEAR - EEPROM Address Register 36
;$1D ($3D) EEDR EEPROM Data register 37
;$1C ($3C) EECR - - - - - EEMWE EEWE EERE 37
;$1B ($3B) Reserved
;$1A ($3A) Reserved
;$19 ($39) Reserved
;$18 ($38) PORTB PORTB7 PORTB6 PORTB5 PORTB4 PORTB3 PORTB2 PORTB1 PORTB0 46
;$17 ($37) DDRB DDB7 DDB6 DDB5 DDB4 DDB3 DDB2 DDB1 DDB0 46
;$16 ($36) PINB PINB7 PINB6 PINB5 PINB4 PINB3 PINB2 PINB1 PINB0 46
;$15 ($35) Reserved
;$14 ($34) Reserved
;$13 ($33) Reserved
;$12 ($32) PORTD - PORTD6 PORTD5 PORTD4 PORTD3 PORTD2 PORTD1 PORTD0 51
;$11 ($31) DDRD - DDD6 DDD5 DDD4 DDD3 DDD2 DDD1 DDD0 51
;$10 ($30) PIND - PIND6 PIND5 PIND4 PIND3 PIND2 PIND1 PIND0 51
;$0F ($2F) Reserved
;$0E ($2E) Reserved
;$0D ($2D) Reserved
;$0C ($2C) UDR UART I/O Data Register 40
;$0B ($2B) USR RXC TXC UDRE FE OR - - - 40
;$0A ($2A) UCR RXCIE TXCIE UDRIE RXEN TXEN CHR9 RXB8 TXB8 41
;$09 ($29) UBRR UART Baud Rate Register 43
;$08 ($28) ACSR ACD - ACO ACI ACIE ACIC ACIS1 ACIS0 44
;… Reserved
;$00 ($20) Reserved
;
;;----------- other notes
; parallel programming, p46
; serial programming, p51
; electrical characteristics, p55
; register map summary, p60
; instruction summary, p61
; ordering info, p63
; packaging info, p64


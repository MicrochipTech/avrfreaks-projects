;CodeVisionAVR C Compiler V1.0.2.1b Evaluation
;(C) Copyright 1998-2001 Pavel Haiduc, HP InfoTech S.R.L.
;http://infotech.ir.ro
;e-mail:dhptechn@ir.ro , hpinfotech@xnet.ro

;Chip type           : AT90S8515
;Clock frequency     : 8.000000 MHz
;Memory model        : Small
;Optimize for        : Size
;Internal SRAM size  : 512
;External SRAM size  : 0
;Data Stack size     : 128
;Promote char to int : No
;char is unsigned    : Yes
;Global #define      : No
;Automatic register allocation : On
;Use AVR Studio Terminal I/O   : No

	.DEVICE AT90S8515
	.LISTMAC
	.EQU UDRE=0x5
	.EQU RXC=0x7
	.EQU UBRR=0x9
	.EQU UCR=0xA
	.EQU USR=0xB
	.EQU UDR=0xC
	.EQU EERE=0x0
	.EQU EEWE=0x1
	.EQU EEMWE=0x2
	.EQU SPSR=0xE
	.EQU SPDR=0xF
	.EQU EECR=0x1C
	.EQU EEDR=0x1D
	.EQU EEARL=0x1E
	.EQU EEARH=0x1F
	.EQU WDTCR=0x21
	.EQU MCUCR=0x35
	.EQU RAMPZ=0x3B
	.EQU SPL=0x3D
	.EQU SPH=0x3E
	.EQU SREG=0x3F
	.DEF R0X2=R2
	.DEF R0X3=R3
	.DEF R0X4=R4
	.DEF R0X5=R5
	.DEF R0X6=R6
	.DEF R0X7=R7
	.DEF R0X8=R8
	.DEF R0X9=R9
	.DEF R0XA=R10
	.DEF R0XB=R11
	.DEF R0XC=R12
	.DEF R0XD=R13
	.DEF R0XE=R14
	.DEF R0XF=R15
	.DEF R0X10=R16
	.DEF R0X11=R17
	.DEF R0X12=R18
	.DEF R0X13=R19
	.DEF R0X14=R20
	.DEF R0X15=R21
	.DEF R0X1A=R26
	.DEF R0X1B=R27
	.DEF R0X1C=R28
	.DEF R0X1D=R29
	.DEF R0X1E=R30
	.DEF R0X1F=R31
	.EQU __se_bit=0x20
	.EQU __sm_mask=0x10

	.MACRO __CPD1N
	CPI  R30,LOW(@0)
	LDI  R26,HIGH(@0)
	CPC  R31,R26
	LDI  R26,BYTE3(@0)
	CPC  R22,R26
	LDI  R26,BYTE4(@0)
	CPC  R23,R26
	.ENDM

	.MACRO __CPD2N
	CPI  R26,LOW(@0)
	LDI  R30,HIGH(@0)
	CPC  R27,R30
	LDI  R30,BYTE3(@0)
	CPC  R24,R30
	LDI  R30,BYTE4(@0)
	CPC  R25,R30
	.ENDM

	.MACRO __CPWRR
	CP   R@0,R@2
	CPC  R@1,R@3
	.ENDM

	.MACRO __CPWRN
	CPI  R@0,LOW(@2)
	LDI  R30,HIGH(@2)
	CPC  R@1,R30
	.ENDM

	.MACRO __ADDD1N
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	SBCI R22,BYTE3(-@0)
	SBCI R23,BYTE4(-@0)
	.ENDM

	.MACRO __ADDD2N
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	SBCI R24,BYTE3(-@0)
	SBCI R25,BYTE4(-@0)
	.ENDM

	.MACRO __SUBD1N
	SUBI R30,LOW(@0)
	SBCI R31,HIGH(@0)
	SBCI R22,BYTE3(@0)
	SBCI R23,BYTE4(@0)
	.ENDM

	.MACRO __SUBD2N
	SUBI R26,LOW(@0)
	SBCI R27,HIGH(@0)
	SBCI R24,BYTE3(@0)
	SBCI R25,BYTE4(@0)
	.ENDM

	.MACRO __ANDD1N
	ANDI R30,LOW(@0)
	ANDI R31,HIGH(@0)
	ANDI R22,BYTE3(@0)
	ANDI R23,BYTE4(@0)
	.ENDM

	.MACRO __ORD1N
	ORI  R30,LOW(@0)
	ORI  R31,HIGH(@0)
	ORI  R22,BYTE3(@0)
	ORI  R23,BYTE4(@0)
	.ENDM

	.MACRO __DELAY_USB
	LDI  R24,LOW(@0)
__DELAY_USB_LOOP:
	DEC  R24
	BRNE __DELAY_USB_LOOP
	.ENDM

	.MACRO __DELAY_USW
	LDI  R24,LOW(@0)
	LDI  R25,HIGH(@0)
__DELAY_USW_LOOP:
	SBIW R24,1
	BRNE __DELAY_USW_LOOP
	.ENDM

	.MACRO __CLRD1S
	CLR  R30
	STD  Y+@0,R30
	STD  Y+@0+1,R30
	STD  Y+@0+2,R30
	STD  Y+@0+3,R30
	.ENDM

	.MACRO __GETD1S
	LDD  R30,Y+@0
	LDD  R31,Y+@0+1
	LDD  R22,Y+@0+2
	LDD  R23,Y+@0+3
	.ENDM

	.MACRO __PUTD1S
	STD  Y+@0,R30
	STD  Y+@0+1,R31
	STD  Y+@0+2,R22
	STD  Y+@0+3,R23
	.ENDM

	.MACRO __POINTB1MN
	LDI  R30,LOW(@0+@1)
	.ENDM

	.MACRO __POINTW1MN
	LDI  R30,LOW(@0+@1)
	LDI  R31,HIGH(@0+@1)
	.ENDM

	.MACRO __POINTW1FN
	LDI  R30,LOW(2*@0+@1)
	LDI  R31,HIGH(2*@0+@1)
	.ENDM

	.MACRO __POINTB2MN
	LDI  R26,LOW(@0+@1)
	.ENDM

	.MACRO __POINTW2MN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	.ENDM

	.MACRO __GETD1N
	LDI  R30,LOW(@0)
	LDI  R31,HIGH(@0)
	LDI  R22,BYTE3(@0)
	LDI  R23,BYTE4(@0)
	.ENDM

	.MACRO __GETD2N
	LDI  R26,LOW(@0)
	LDI  R27,HIGH(@0)
	LDI  R24,BYTE3(@0)
	LDI  R25,BYTE4(@0)
	.ENDM

	.MACRO __GETD2S
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	LDD  R24,Y+@0+2
	LDD  R25,Y+@0+3
	.ENDM

	.MACRO __GETB1MN
	LDS  R30,@0+@1
	.ENDM

	.MACRO __GETW1MN
	LDS  R30,@0+@1
	LDS  R31,@0+@1+1
	.ENDM

	.MACRO __GETD1MN
	LDS  R30,@0+@1
	LDS  R31,@0+@1+1
	LDS  R22,@0+@1+2
	LDS  R23,@0+@1+3
	.ENDM

	.MACRO __GETBRMN
	LDS  R@2,@0+@1
	.ENDM

	.MACRO __GETWRMN
	LDS  R@2,@0+@1
	LDS  R@3,@0+@1+1
	.ENDM

	.MACRO __GETB2MN
	LDS  R26,@0+@1
	.ENDM

	.MACRO __GETW2MN
	LDS  R26,@0+@1
	LDS  R27,@0+@1+1
	.ENDM

	.MACRO __GETD2MN
	LDS  R26,@0+@1
	LDS  R27,@0+@1+1
	LDS  R24,@0+@1+2
	LDS  R25,@0+@1+3
	.ENDM

	.MACRO __PUTB1MN
	STS  @0+@1,R30
	.ENDM

	.MACRO __PUTW1MN
	STS  @0+@1,R30
	STS  @0+@1+1,R31
	.ENDM

	.MACRO __PUTD1MN
	STS  @0+@1,R30
	STS  @0+@1+1,R31
	STS  @0+@1+2,R22
	STS  @0+@1+3,R23
	.ENDM

	.MACRO __PUTBMRN
	STS  @0+@1,R@2
	.ENDM

	.MACRO __PUTWMRN
	STS  @0+@1,R@2
	STS  @0+@1+1,R@3
	.ENDM

	.MACRO __GETW1R
	MOV  R30,R@0
	MOV  R31,R@1
	.ENDM

	.MACRO __GETW2R
	MOV  R26,R@0
	MOV  R27,R@1
	.ENDM

	.MACRO __GETWRN
	LDI  R@0,LOW(@2)
	LDI  R@1,HIGH(@2)
	.ENDM

	.MACRO __PUTW1R
	MOV  R@0,R30
	MOV  R@1,R31
	.ENDM

	.MACRO __ADDWRN
	SUBI R@0,LOW(-@2)
	SBCI R@1,HIGH(-@2)
	.ENDM

	.MACRO __ADDWRR
	ADD  R@0,R@2
	ADC  R@1,R@3
	.ENDM

	.MACRO __SUBWRN
	SUBI R@0,LOW(@2)
	SBCI R@1,HIGH(@2)
	.ENDM

	.MACRO __SUBWRR
	SUB  R@0,R@2
	SBC  R@1,R@3
	.ENDM

	.MACRO __ANDWRN
	ANDI R@0,LOW(@2)
	ANDI R@1,HIGH(@2)
	.ENDM

	.MACRO __ANDWRR
	AND  R@0,R@2
	AND  R@1,R@3
	.ENDM

	.MACRO __ORWRN
	ORI  R@0,LOW(@2)
	ORI  R@1,HIGH(@2)
	.ENDM

	.MACRO __ORWRR
	OR   R@0,R@2
	OR   R@1,R@3
	.ENDM

	.MACRO __EORWRR
	EOR  R@0,R@2
	EOR  R@1,R@3
	.ENDM

	.MACRO __GETWRS
	LDD  R@0,Y+@2
	LDD  R@1,Y+@2+1
	.ENDM

	.MACRO __PUTWSR
	STD  Y+@2,R@0
	STD  Y+@2+1,R@1
	.ENDM

	.MACRO __MOVEWRR
	MOV  R@0,R@2
	MOV  R@1,R@3
	.ENDM

	.MACRO __INWR
	IN   R@0,@2
	IN   R@1,@2+1
	.ENDM

	.MACRO __OUTWR
	OUT  @2+1,R@1
	OUT  @2,R@0
	.ENDM

	.MACRO __CALL1MN
	LDS  R30,@0+@1
	LDS  R31,@0+@1+1
	ICALL
	.ENDM

	.MACRO __NBST
	BST  R@0,@1
	IN   R30,SREG
	LDI  R31,0x40
	EOR  R30,R31
	OUT  SREG,R30
	.ENDM


	.MACRO __PUTB1SN
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SN
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SN
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1SNS
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	ADIW R26,@1
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SNS
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	ADIW R26,@1
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SNS
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	ADIW R26,@1
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1PMN
	LDS  R26,@0
	LDS  R27,@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1PMN
	LDS  R26,@0
	LDS  R27,@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1PMN
	LDS  R26,@0
	LDS  R27,@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1PMNS
	LDS  R26,@0
	LDS  R27,@0+1
	ADIW R26,@1
	ST   X,R30
	.ENDM

	.MACRO __PUTW1PMNS
	LDS  R26,@0
	LDS  R27,@0+1
	ADIW R26,@1
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1PMNS
	LDS  R26,@0
	LDS  R27,@0+1
	ADIW R26,@1
	RCALL __PUTDP1
	.ENDM

	.CSEG
	.ORG 0x0D

	.INCLUDE "clock.inc"

__RESET:
	CLI
	CLR  R30
	OUT  EECR,R30
	OUT  MCUCR,R30

;DISABLE WATCHDOG
	LDI  R31,0x18
	OUT  WDTCR,R31
	LDI  R31,0x10
	OUT  WDTCR,R31

;CLEAR R2-R15
	LDI  R24,14
	LDI  R26,2
	CLR  R27
__CLEAR_REG:
	ST   X+,R30
	DEC  R24
	BRNE __CLEAR_REG

;CLEAR SRAM
	LDI  R24,LOW(0x200)
	LDI  R25,HIGH(0x200)
	LDI  R26,0x60
__CLEAR_SRAM:
	ST   X+,R30
	SBIW R24,1
	BRNE __CLEAR_SRAM

;GLOBAL VARIABLES INITIALIZATION
	LDI  R30,LOW(__GLOBAL_INI_TBL*2)
	LDI  R31,HIGH(__GLOBAL_INI_TBL*2)
__GLOBAL_INI_NEXT:
	LPM
	MOV  R1,R0
	ADIW R30,1
	LPM
	ADIW R30,1
	MOV  R22,R30
	MOV  R23,R31
	MOV  R31,R0
	MOV  R30,R1
	SBIW R30,0
	BREQ __GLOBAL_INI_END
	LPM
	MOV  R26,R0
	ADIW R30,1
	LPM
	MOV  R27,R0
	ADIW R30,1
	LPM
	MOV  R24,R0
	ADIW R30,1
	LPM
	MOV  R25,R0
	ADIW R30,1
__GLOBAL_INI_LOOP:
	LPM
	ST   X+,R0
	ADIW R30,1
	SBIW R24,1
	BRNE __GLOBAL_INI_LOOP
	MOV  R30,R22
	MOV  R31,R23
	RJMP __GLOBAL_INI_NEXT
__GLOBAL_INI_END:

;STACK POINTER INITIALIZATION
	LDI  R30,LOW(0x25F)
	OUT  SPL,R30
	LDI  R30,HIGH(0x25F)
	OUT  SPH,R30

;DATA STACK POINTER INITIALIZATION
	LDI  R28,LOW(0xDF)
	LDI  R29,HIGH(0xDF)

	RJMP _main

	.ESEG
	.ORG 1

	.DSEG
	.ORG 0xE0
;       1 /*********************************************
;       2 This program was produced by the
;       3 CodeWizardAVR V1.0.2.1b Evaluation
;       4 Automatic Program Generator
;       5 © Copyright 1998-2001
;       6 Pavel Haiduc, HP InfoTech S.R.L.
;       7 http://infotech.ir.ro
;       8 e-mail:dhptechn@ir.ro , hpinfotech@xnet.ro
;       9 
;      10 Project : Clock
;      11 Version : 
;      12 Date    : 7/20/2005
;      13 Author  : Freeware
;      14 Company : 
;      15 Comments: 
;      16 
;      17 
;      18 Chip type           : AT90S8515
;      19 Clock frequency     : 8.000000 MHz
;      20 Memory model        : Small
;      21 Internal SRAM size  : 512
;      22 External SRAM size  : 0
;      23 Data Stack size     : 128
;      24 *********************************************/
;      25 
;      26 #include <90s8515.h>
;      27 // CodeVisionAVR C Compiler
;      28 // (C) 1998-2000 Pavel Haiduc, HP InfoTech S.R.L.
;      29 
;      30 // I/O registers definitions for the AT90S8515
;      31 
;      32 #ifndef _90S8515_INCLUDED_
;      33 #define _90S8515_INCLUDED_
;      34 
;      35 #pragma used+
;      36 sfrb ACSR=8;
;      37 sfrb UBRR=9;
;      38 sfrb UCR=0xa;
;      39 sfrb USR=0xb;
;      40 sfrb UDR=0xc;
;      41 sfrb SPCR=0xd;
;      42 sfrb SPSR=0xe;
;      43 sfrb SPDR=0xf;
;      44 sfrb PIND=0x10;
;      45 sfrb DDRD=0x11;
;      46 sfrb PORTD=0x12;
;      47 sfrb PINC=0x13;
;      48 sfrb DDRC=0x14;
;      49 sfrb PORTC=0x15;
;      50 sfrb PINB=0x16;
;      51 sfrb DDRB=0x17;
;      52 sfrb PORTB=0x18;
;      53 sfrb PINA=0x19;
;      54 sfrb DDRA=0x1a;
;      55 sfrb PORTA=0x1b;
;      56 sfrb EECR=0x1c;
;      57 sfrb EEDR=0x1d;
;      58 sfrb EEARL=0x1e;
;      59 sfrb EEARH=0x1f;
;      60 sfrw EEAR=0x1e;   // 16 bit access
;      61 sfrb WDTCR=0x21;
;      62 sfrb ICR1L=0x24;
;      63 sfrb ICR1H=0x25;
;      64 sfrw ICR1=0x24;   // 16 bit access
;      65 sfrb OCR1BL=0x28;
;      66 sfrb OCR1BH=0x29;
;      67 sfrw OCR1B=0x28;  // 16 bit access
;      68 sfrb OCR1AL=0x2a;
;      69 sfrb OCR1AH=0x2b;
;      70 sfrw OCR1A=0x2a;  // 16 bit access
;      71 sfrb TCNT1L=0x2c;
;      72 sfrb TCNT1H=0x2d;
;      73 sfrw TCNT1=0x2c;  // 16 bit access
;      74 sfrb TCCR1B=0x2e;
;      75 sfrb TCCR1A=0x2f;
;      76 sfrb TCNT0=0x32;
;      77 sfrb TCCR0=0x33;
;      78 sfrb MCUSR=0x34;
;      79 sfrb MCUCR=0x35;
;      80 sfrb TIFR=0x38;
;      81 sfrb TIMSK=0x39;
;      82 sfrb GIFR=0x3a;
;      83 sfrb GIMSK=0x3b;
;      84 sfrb SPL=0x3d;
;      85 sfrb SPH=0x3e;
;      86 sfrb SREG=0x3f;
;      87 #pragma used-
;      88 
;      89 // Interrupt vectors definitions
;      90 
;      91 #define EXT_INT0 2
;      92 #define EXT_INT1 3
;      93 #define TIM1_CAPT 4
;      94 #define TIM1_COMPA 5
;      95 #define TIM1_COMPB 6
;      96 #define TIM1_OVF 7
;      97 #define TIM0_OVF 8
;      98 #define SPI_STC 9
;      99 #define UART_RXC 10
;     100 #define UART_DRE 11
;     101 #define UART_TXC 12
;     102 #define ANA_COMP 13
;     103 
;     104 #endif
;     105 
;     106 #include <delay.h>
;     107 // CodeVisionAVR C Compiler
;     108 // (C) 1998-2000 Pavel Haiduc, HP InfoTech S.R.L.
;     109 
;     110 #ifndef _DELAY_INCLUDED_
;     111 #define _DELAY_INCLUDED_
;     112 
;     113 #pragma used+
;     114 
;     115 void delay_us(unsigned int n);
;     116 void delay_ms(unsigned int n);
;     117 
;     118 #pragma used-
;     119 
;     120 #endif
;     121 // Declare your global variables here
;     122  /*   void delay() {
;     123                    unsigned char ch1,ch2;
;     124                    for(ch1=0;ch1=255;ch1++)
;     125                       for(ch2=0;ch2=255;ch2++);
;     126                       } */
;     127 void main(void)
;     128 {

	.CSEG
_main:
;     129 // Declare your local variables here
;     130         unsigned char sec=0,min=0,hour=12,d;
;     131 // Input/Output Ports initialization
;     132 // Port A
;     133 PORTA=0x00;
;	sec -> R16
;	min -> R17
;	hour -> R18
;	d -> R19
	LDI  R16,0
	LDI  R17,0
	LDI  R18,12
	CLR  R30
	OUT  0x1B,R30
;     134 DDRA=0xFF;
	LDI  R30,LOW(255)
	OUT  0x1A,R30
;     135 
;     136 // Port B
;     137 PORTB=0x00;
	CLR  R30
	OUT  0x18,R30
;     138 DDRB=0xFF;
	LDI  R30,LOW(255)
	OUT  0x17,R30
;     139 
;     140 // Port C
;     141 PORTC=0x00;
	CLR  R30
	OUT  0x15,R30
;     142 DDRC=0xFF;
	LDI  R30,LOW(255)
	OUT  0x14,R30
;     143 
;     144 // Port D
;     145 PORTD=0x00;
	CLR  R30
	OUT  0x12,R30
;     146 DDRD=0xFF;
	LDI  R30,LOW(255)
	OUT  0x11,R30
;     147 
;     148 // Timer/Counter 0 initialization
;     149 // Clock source: System Clock
;     150 // Clock value: Timer 0 Stopped
;     151 // Mode: Output Compare
;     152 // OC0 output: Disconnected
;     153 TCCR0=0x05;
	LDI  R30,LOW(5)
	OUT  0x33,R30
;     154 TCNT0=0xFF;
	LDI  R30,LOW(255)
	OUT  0x32,R30
;     155 
;     156 // Timer/Counter 1 initialization
;     157 // Clock source: System Clock
;     158 // Clock value: Timer 1 Stopped
;     159 // Mode: Output Compare
;     160 // OC1A output: Discon.
;     161 // OC1B output: Discon.
;     162 // Noise Canceler: Off
;     163 // Input Capture on Falling Edge
;     164 TCCR1A=0x00;
	CLR  R30
	OUT  0x2F,R30
;     165 TCCR1B=0x00;
	OUT  0x2E,R30
;     166 TCNT1H=0x1E;
	LDI  R30,LOW(30)
	OUT  0x2D,R30
;     167 TCNT1L=0x84;
	LDI  R30,LOW(132)
	OUT  0x2C,R30
;     168 OCR1AH=0x00;
	CLR  R30
	OUT  0x2B,R30
;     169 OCR1AL=0x00;
	OUT  0x2A,R30
;     170 OCR1BH=0x00;
	OUT  0x29,R30
;     171 OCR1BL=0x00;
	OUT  0x28,R30
;     172 
;     173 // External Interrupt(s) initialization
;     174 // INT0: Off
;     175 // INT1: Off
;     176 GIMSK=0x00;
	OUT  0x3B,R30
;     177 MCUCR=0x00;
	OUT  0x35,R30
;     178 
;     179 // Timer(s)/Counter(s) Interrupt(s) initialization
;     180 TIMSK=0x00;
	OUT  0x39,R30
;     181 
;     182 // Analog Comparator initialization
;     183 // Analog Comparator: Off
;     184 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     185 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     186 
;     187 while (1)
;     188       {
;     189       // Place your code here
;     190 
;     191       again:
_0x6:
;     192 	     //delay_us(100);
;     193                 TCNT0=0x05;
	LDI  R30,LOW(5)
	OUT  0x32,R30
;     194 	     if(sec>=59)
	CPI  R16,59
	BRLO _0x7
;     195 	    {	min++; sec=0; }
	SUBI R17,-1
	LDI  R16,LOW(0)
;     196 	      else
	RJMP _0x8
_0x7:
;     197 		  sec++;
	SUBI R16,-1
_0x8:
;     198 	     if(min>=59)
	CPI  R17,59
	BRLO _0x9
;     199 		{ hour++; min=0; 
	SUBI R18,-1
	LDI  R17,LOW(0)
;     200                       }
;     201 	     if(hour>=23)   hour=0;	
_0x9:
	CPI  R18,23
	BRLO _0xA
	LDI  R18,LOW(0)
;     202           PORTA=sec;
_0xA:
	OUT  0x1B,R16
;     203           PORTB=min;
	OUT  0x18,R17
;     204           PORTC=hour;
	OUT  0x15,R18
;     205               goto again;
	RJMP _0x6
;     206       };
;     207 }
_0xB:
	RJMP _0xB

	.ORG 0x00

;INTERRUPT VECTORS
	RJMP __RESET
	RJMP 0
	RJMP 0
	RJMP 0
	RJMP 0
	RJMP 0
	RJMP 0
	RJMP 0
	RJMP 0
	RJMP 0
	RJMP 0
	RJMP 0
	RJMP 0

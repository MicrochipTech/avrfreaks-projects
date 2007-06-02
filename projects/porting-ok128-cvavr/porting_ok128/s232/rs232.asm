;CodeVisionAVR C Compiler V1.24.0 Standard
;(C) Copyright 1998-2003 HP InfoTech s.r.l.
;http://www.hpinfotech.ro
;e-mail:office@hpinfotech.ro

;Chip type           : ATmega128
;Program type        : Application
;Clock frequency     : 16.000000 MHz
;Memory model        : Small
;Optimize for        : Size
;(s)printf features  : int, width
;(s)scanf features   : long, width
;External SRAM size  : 61184
;Ext. SRAM wait state: 0
;Data Stack size     : 1024 byte(s)
;Heap size           : 256 byte(s)
;Promote char to int : No
;char is unsigned    : Yes
;8 bit enums         : Yes
;Enhanced core instructions    : On
;Automatic register allocation : On
;Use AVR Studio 3 Terminal I/O   : No

	.LISTMAC
	.EQU UDRE=0x5
	.EQU RXC=0x7
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
	.EQU XMCRA=0x6D
	.EQU XMCRB=0x6C

	.DEF R0X0=R0
	.DEF R0X1=R1
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
	.DEF R0X16=R22
	.DEF R0X17=R23
	.DEF R0X18=R24
	.DEF R0X19=R25
	.DEF R0X1A=R26
	.DEF R0X1B=R27
	.DEF R0X1C=R28
	.DEF R0X1D=R29
	.DEF R0X1E=R30
	.DEF R0X1F=R31

	.EQU __se_bit=0x20
	.EQU __sm_mask=0x1C
	.EQU __sm_adc_noise_red=0x08
	.EQU __sm_powerdown=0x10
	.EQU __sm_powersave=0x18
	.EQU __sm_standby=0x14
	.EQU __sm_ext_standby=0x1C

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

	.MACRO __PUTW2R
	MOV  R@0,R26
	MOV  R@1,R27
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
	CALL __PUTDP1
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
	CALL __PUTDP1
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
	CALL __PUTDP1
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
	CALL __PUTDP1
	.ENDM

	.MACRO __GETB1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R30,Z
	.ENDM

	.MACRO __GETW1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R0,Z+
	LD   R31,Z
	MOV  R30,R0
	.ENDM

	.MACRO __GETD1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R0,Z+
	LD   R1,Z+
	LD   R22,Z+
	LD   R23,Z
	MOVW R30,R0
	.ENDM

	.MACRO __GETB2SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R26,X
	.ENDM

	.MACRO __GETW2SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	.ENDM

	.MACRO __GETD2SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R1,X+
	LD   R24,X+
	LD   R25,X
	MOVW R26,R0
	.ENDM

	.MACRO __GETBRSX
	MOVW R30,R28
	SUBI R30,LOW(-@1)
	SBCI R31,HIGH(-@1)
	LD   R@0,Z
	.ENDM

	.MACRO __GETWRSX
	MOVW R30,R28
	SUBI R30,LOW(-@2)
	SBCI R31,HIGH(-@2)
	LD   R@0,Z+
	LD   R@1,Z
	.ENDM

	.MACRO __LSLW8SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R31,Z
	CLR  R30
	.ENDM

	.MACRO __PUTB1SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X+,R30
	ST   X+,R31
	ST   X+,R22
	ST   X,R23
	.ENDM

	.MACRO __CLRW1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	CLR  R0
	ST   Z+,R0
	ST   Z,R0
	.ENDM

	.MACRO __CLRD1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	CLR  R0
	ST   Z+,R0
	ST   Z+,R0
	ST   Z+,R0
	ST   Z,R0
	.ENDM

	.MACRO __PUTB2SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z,R26
	.ENDM

	.MACRO __PUTW2SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z+,R26
	ST   Z,R27
	.ENDM

	.MACRO __PUTBSRX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z,R@1
	.ENDM

	.MACRO __PUTWSRX
	MOVW R30,R28
	SUBI R30,LOW(-@2)
	SBCI R31,HIGH(-@2)
	ST   Z+,R@0
	ST   Z,R@1
	.ENDM

	.MACRO __PUTB1SNX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SNX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SNX
	MOVW R26,R28
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X+,R31
	ST   X+,R22
	ST   X,R23
	.ENDM

	.MACRO __MULBRR
	MULS R@0,R@1
	MOV  R30,R0
	.ENDM

	.MACRO __MULBRRU
	MUL  R@0,R@1
	MOV  R30,R0
	.ENDM

	.CSEG
	.ORG 0

	.INCLUDE "rs232.vec"
	.INCLUDE "rs232.inc"

__RESET:
	CLI
	CLR  R30
	OUT  EECR,R30

;INTERRUPT VECTORS ARE PLACED
;AT THE START OF FLASH
	LDI  R31,1
	OUT  MCUCR,R31
	LDI  R31,0x80
	OUT  MCUCR,R31
	STS  XMCRA,R30
	STS  XMCRB,R30
	OUT  RAMPZ,R30

;DISABLE WATCHDOG
	LDI  R31,0x18
	OUT  WDTCR,R31
	LDI  R31,0x10
	OUT  WDTCR,R31

;CLEAR R2-R14
	LDI  R24,13
	LDI  R26,2
	CLR  R27
__CLEAR_REG:
	ST   X+,R30
	DEC  R24
	BRNE __CLEAR_REG

;CLEAR SRAM
	LDI  R24,LOW(0xED00)
	LDI  R25,HIGH(0xED00)
	LDI  R26,LOW(0x100)
	LDI  R27,HIGH(0x100)
__CLEAR_SRAM:
	ST   X+,R30
	SBIW R24,1
	BRNE __CLEAR_SRAM

;GLOBAL VARIABLES INITIALIZATION
	LDI  R30,LOW(__GLOBAL_INI_TBL*2)
	LDI  R31,HIGH(__GLOBAL_INI_TBL*2)
__GLOBAL_INI_NEXT:
	LPM  R0,Z+
	LPM  R1,Z+
	MOVW R22,R30
	MOVW R30,R0
	SBIW R30,0
	BREQ __GLOBAL_INI_END
	LPM  R26,Z+
	LPM  R27,Z+
	LPM  R24,Z+
	LPM  R25,Z+
__GLOBAL_INI_LOOP:
	LPM  R0,Z+
	ST   X+,R0
	SBIW R24,1
	BRNE __GLOBAL_INI_LOOP
	MOVW R30,R22
	RJMP __GLOBAL_INI_NEXT
__GLOBAL_INI_END:

;STACK POINTER INITIALIZATION
	LDI  R30,LOW(0xEDFF)
	OUT  SPL,R30
	LDI  R30,HIGH(0xEDFF)
	OUT  SPH,R30

;DATA STACK POINTER INITIALIZATION
	LDI  R28,LOW(0x500)
	LDI  R29,HIGH(0x500)

	JMP  _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0x500
;       1 /*********************************************
;       2 This program was produced by the
;       3 CodeWizardAVR V1.24.0 Standard
;       4 Automatic Program Generator
;       5 © Copyright 1998-2003 HP InfoTech s.r.l.
;       6 http://www.hpinfotech.ro
;       7 e-mail:office@hpinfotech.ro
;       8 
;       9 Project : 
;      10 Version : 
;      11 Date    : 2007-05-26
;      12 Author  : Gyeongsang
;      13 Company : 
;      14 Comments: 
;      15 
;      16 
;      17 Chip type           : ATmega128
;      18 Program type        : Application
;      19 Clock frequency     : 16.000000 MHz
;      20 Memory model        : Small
;      21 External SRAM size  : 0
;      22 Data Stack size     : 1024
;      23 *********************************************/
;      24 
;      25 #include <mega128.h>  
;      26 #include <ok128.h>
;      27 #include <delay.h>
;      28 
;      29 #define RXB8 1
;      30 #define TXB8 0
;      31 #define UPE 2
;      32 #define OVR 3
;      33 #define FE 4
;      34 #define UDRE 5
;      35 #define RXC 7
;      36 
;      37 #define FRAMING_ERROR (1<<FE)
;      38 #define PARITY_ERROR (1<<UPE)
;      39 #define DATA_OVERRUN (1<<OVR)
;      40 #define DATA_REGISTER_EMPTY (1<<UDRE)
;      41 #define RX_COMPLETE (1<<RXC)
;      42 
;      43 // USART0 Receiver buffer
;      44 #define RX_BUFFER_SIZE0 8
;      45 char rx_buffer0[RX_BUFFER_SIZE0];
_rx_buffer0:
	.BYTE 0x8
;      46 unsigned char rx_wr_index0,rx_rd_index0,rx_counter0;
;      47 // This flag is set on USART0 Receiver buffer overflow
;      48 bit rx_buffer_overflow0;
;      49 
;      50 // USART0 Receiver interrupt service routine
;      51 #pragma savereg-
;      52 interrupt [USART0_RXC] void usart0_rx_isr(void)
;      53 {

	.CSEG
_usart0_rx_isr:
;      54 char status,data;
;      55 #asm
	ST   -Y,R17
	ST   -Y,R16
;	status -> R16
;	data -> R17
;      56     push r26
    push r26
;      57     push r27
    push r27
;      58     push r30
    push r30
;      59     push r31
    push r31
;      60     in   r26,sreg
    in   r26,sreg
;      61     push r26
    push r26
;      62 #endasm

;      63 status=UCSR0A;
	IN   R16,11
;      64 data=UDR0;
	IN   R17,12
;      65 if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
	MOV  R30,R16
	ANDI R30,LOW(0x1C)
	BRNE _0x24
;      66    {
;      67    rx_buffer0[rx_wr_index0]=data;
	MOV  R26,R4
	CLR  R27
	SUBI R26,LOW(-_rx_buffer0)
	SBCI R27,HIGH(-_rx_buffer0)
	ST   X,R17
;      68    if (++rx_wr_index0 == RX_BUFFER_SIZE0) rx_wr_index0=0;
	INC  R4
	LDI  R30,LOW(8)
	CP   R30,R4
	BRNE _0x25
	CLR  R4
;      69    if (++rx_counter0 == RX_BUFFER_SIZE0)
_0x25:
	INC  R6
	LDI  R30,LOW(8)
	CP   R30,R6
	BRNE _0x26
;      70       {
;      71       rx_counter0=0;
	CLR  R6
;      72       rx_buffer_overflow0=1;
	SET
	BLD  R2,0
;      73       };
_0x26:
;      74    };
_0x24:
;      75 #asm
;      76     pop  r26
    pop  r26
;      77     out  sreg,r26
    out  sreg,r26
;      78     pop  r31
    pop  r31
;      79     pop  r30
    pop  r30
;      80     pop  r27
    pop  r27
;      81     pop  r26
    pop  r26
;      82 #endasm

;      83 }
	LD   R16,Y+
	LD   R17,Y+
	RETI
;      84 #pragma savereg+
;      85 
;      86 #ifndef _DEBUG_TERMINAL_IO_
;      87 // Get a character from the USART0 Receiver buffer
;      88 #define _ALTERNATE_GETCHAR_
;      89 #pragma used+
;      90 char getchar(void)
;      91 {
_getchar:
;      92 char data;
;      93 while (rx_counter0==0);
	ST   -Y,R16
;	data -> R16
_0x28:
	TST  R6
	BREQ _0x28
;      94 data=rx_buffer0[rx_rd_index0];
	MOV  R30,R5
	CLR  R31
	SUBI R30,LOW(-_rx_buffer0)
	SBCI R31,HIGH(-_rx_buffer0)
	LD   R16,Z
;      95 if (++rx_rd_index0 == RX_BUFFER_SIZE0) rx_rd_index0=0;
	INC  R5
	LDI  R30,LOW(8)
	CP   R30,R5
	BRNE _0x2B
	CLR  R5
;      96 #asm("cli")
_0x2B:
	cli
;      97 --rx_counter0;
	DEC  R6
;      98 #asm("sei")
	sei
;      99 return data;
	MOV  R30,R16
	LD   R16,Y+
	RET
;     100 }
;     101 #pragma used-
;     102 #endif
;     103 
;     104 // USART0 Transmitter buffer
;     105 #define TX_BUFFER_SIZE0 8
;     106 char tx_buffer0[TX_BUFFER_SIZE0];

	.DSEG
_tx_buffer0:
	.BYTE 0x8
;     107 unsigned char tx_wr_index0,tx_rd_index0,tx_counter0;
;     108 
;     109 // USART0 Transmitter interrupt service routine
;     110 #pragma savereg-
;     111 interrupt [USART0_TXC] void usart0_tx_isr(void)
;     112 {

	.CSEG
_usart0_tx_isr:
;     113 #asm
;     114     push r26
    push r26
;     115     push r27
    push r27
;     116     push r30
    push r30
;     117     push r31
    push r31
;     118     in   r26,sreg
    in   r26,sreg
;     119     push r26
    push r26
;     120 #endasm

;     121 if (tx_counter0)
	TST  R9
	BREQ _0x2D
;     122    {
;     123    --tx_counter0;
	DEC  R9
;     124    UDR0=tx_buffer0[tx_rd_index0];
	MOV  R30,R8
	CLR  R31
	SUBI R30,LOW(-_tx_buffer0)
	SBCI R31,HIGH(-_tx_buffer0)
	LD   R30,Z
	OUT  0xC,R30
;     125    if (++tx_rd_index0 == TX_BUFFER_SIZE0) tx_rd_index0=0;
	INC  R8
	LDI  R30,LOW(8)
	CP   R30,R8
	BRNE _0x2E
	CLR  R8
;     126    };
_0x2E:
_0x2D:
;     127 #asm
;     128     pop  r26
    pop  r26
;     129     out  sreg,r26
    out  sreg,r26
;     130     pop  r31
    pop  r31
;     131     pop  r30
    pop  r30
;     132     pop  r27
    pop  r27
;     133     pop  r26
    pop  r26
;     134 #endasm

;     135 }
	RETI
;     136 #pragma savereg+
;     137 
;     138 #ifndef _DEBUG_TERMINAL_IO_
;     139 // Write a character to the USART0 Transmitter buffer
;     140 #define _ALTERNATE_PUTCHAR_
;     141 #pragma used+
;     142 void putchar(char c)
;     143 {
_putchar:
;     144 while (tx_counter0 == TX_BUFFER_SIZE0);
_0x30:
	LDI  R30,LOW(8)
	CP   R30,R9
	BREQ _0x30
;     145 #asm("cli")
	cli
;     146 if (tx_counter0 || ((UCSR0A & DATA_REGISTER_EMPTY)==0))
	TST  R9
	BRNE _0x34
	SBIC 0xB,5
	RJMP _0x33
_0x34:
;     147    {
;     148    tx_buffer0[tx_wr_index0]=c;
	MOV  R26,R7
	CLR  R27
	SUBI R26,LOW(-_tx_buffer0)
	SBCI R27,HIGH(-_tx_buffer0)
	LD   R30,Y
	ST   X,R30
;     149    if (++tx_wr_index0 == TX_BUFFER_SIZE0) tx_wr_index0=0;
	INC  R7
	LDI  R30,LOW(8)
	CP   R30,R7
	BRNE _0x36
	CLR  R7
;     150    ++tx_counter0;
_0x36:
	INC  R9
;     151    }
;     152 else
	RJMP _0x37
_0x33:
;     153    UDR0=c;
	LD   R30,Y
	OUT  0xC,R30
_0x37:
;     154 #asm("sei")
	sei
;     155 }
	ADIW R28,1
	RET
;     156 #pragma used-
;     157 #endif
;     158 
;     159 // Standard Input/Output functions
;     160 #include <stdio.h>
;     161 
;     162 
;     163 void sendString(char *string)
;     164 {	
;     165 	while (string != NULL)
;     166 	{     
;     167 		putchar(*string);
;     168 		string++;
;     169 	}
;     170 }
;     171 
;     172 void sendFlashString(char flash *string)
;     173 {	
_sendFlashString:
;     174 	while (*string != '\0')
_0x48:
	LD   R30,Y
	LDD  R31,Y+1
	LPM  R30,Z
	CPI  R30,0
	BREQ _0x4A
;     175 	{     
;     176 		putchar(*string);
	LD   R30,Y
	LDD  R31,Y+1
	LPM  R30,Z
	ST   -Y,R30
	RCALL _putchar
;     177 		string++;
	LD   R30,Y
	LDD  R31,Y+1
	ADIW R30,1
	ST   Y,R30
	STD  Y+1,R31
;     178 	} 
	RJMP _0x48
_0x4A:
;     179 	
;     180 }
	ADIW R28,2
	RET
;     181 
;     182 // Declare your global variables here
;     183 char *menu = "1. Light On/Off \n 2. Ambulance \n 3. LCD \n";

	.DSEG
_menu:
	.BYTE 0x2
	.BYTE 0x2A
;     184 char tmp;   
;     185 int position = 0;
;     186 
;     187 void main(void)
;     188 {

	.CSEG
_main:
;     189 // Declare your local variables here
;     190 
;     191 // Input/Output Ports initialization
;     192 // Port A initialization
;     193 // Func0=In Func1=In Func2=In Func3=In Func4=In Func5=In Func6=In Func7=In 
;     194 // State0=T State1=T State2=T State3=T State4=T State5=T State6=T State7=T 
;     195 PORTA=0x00;
	CLR  R30
	OUT  0x1B,R30
;     196 DDRA=0x00;
	OUT  0x1A,R30
;     197 
;     198 // Port B initialization
;     199 // Func0=In Func1=In Func2=In Func3=In Func4=In Func5=In Func6=In Func7=In 
;     200 // State0=T State1=T State2=T State3=T State4=T State5=T State6=T State7=T 
;     201 PORTB=0x00;
	OUT  0x18,R30
;     202 DDRB=0x00;
	OUT  0x17,R30
;     203 
;     204 // Port C initialization
;     205 // Func0=In Func1=In Func2=In Func3=In Func4=In Func5=In Func6=In Func7=In 
;     206 // State0=T State1=T State2=T State3=T State4=T State5=T State6=T State7=T 
;     207 PORTC=0x00;
	OUT  0x15,R30
;     208 DDRC=0x00;
	OUT  0x14,R30
;     209 
;     210 // Port D initialization
;     211 // Func0=In Func1=In Func2=In Func3=In Func4=In Func5=In Func6=In Func7=In 
;     212 // State0=T State1=T State2=T State3=T State4=T State5=T State6=T State7=T 
;     213 PORTD=0x00;
	OUT  0x12,R30
;     214 DDRD=0x00;
	OUT  0x11,R30
;     215 
;     216 // Port E initialization
;     217 // Func0=In Func1=In Func2=In Func3=In Func4=In Func5=In Func6=In Func7=In 
;     218 // State0=T State1=T State2=T State3=T State4=T State5=T State6=T State7=T 
;     219 PORTE=0x00;
	RCALL SUBOPT_0x0
;     220 DDRE=0x00;
	OUT  0x2,R30
;     221 
;     222 // Port F initialization
;     223 // Func0=In Func1=In Func2=In Func3=In Func4=In Func5=In Func6=In Func7=In 
;     224 // State0=T State1=T State2=T State3=T State4=T State5=T State6=T State7=T 
;     225 PORTF=0x00;
	CLR  R30
	STS  0x62,R30
;     226 DDRF=0x00;
	STS  0x61,R30
;     227 
;     228 // Port G initialization
;     229 // Func0=In Func1=In Func2=In Func3=In Func4=In 
;     230 // State0=T State1=T State2=T State3=T State4=T 
;     231 PORTG=0x00;
	STS  0x65,R30
;     232 DDRG=0x00;
	STS  0x64,R30
;     233 
;     234 // Timer/Counter 0 initialization
;     235 // Clock source: System Clock
;     236 // Clock value: Timer 0 Stopped
;     237 // Mode: Normal top=FFh
;     238 // OC0 output: Disconnected
;     239 ASSR=0x00;
	OUT  0x30,R30
;     240 TCCR0=0x00;
	OUT  0x33,R30
;     241 TCNT0=0x00;
	OUT  0x32,R30
;     242 OCR0=0x00;
	OUT  0x31,R30
;     243 
;     244 // Timer/Counter 1 initialization
;     245 // Clock source: System Clock
;     246 // Clock value: Timer 1 Stopped
;     247 // Mode: Normal top=FFFFh
;     248 // OC1A output: Discon.
;     249 // OC1B output: Discon.
;     250 // OC1C output: Discon.
;     251 // Noise Canceler: Off
;     252 // Input Capture on Falling Edge
;     253 TCCR1A=0x00;
	OUT  0x2F,R30
;     254 TCCR1B=0x00;
	OUT  0x2E,R30
;     255 TCNT1H=0x00;
	OUT  0x2D,R30
;     256 TCNT1L=0x00;
	OUT  0x2C,R30
;     257 OCR1AH=0x00;
	OUT  0x2B,R30
;     258 OCR1AL=0x00;
	OUT  0x2A,R30
;     259 OCR1BH=0x00;
	OUT  0x29,R30
;     260 OCR1BL=0x00;
	OUT  0x28,R30
;     261 OCR1CH=0x00;
	STS  0x79,R30
;     262 OCR1CL=0x00;
	STS  0x78,R30
;     263 
;     264 // Timer/Counter 2 initialization
;     265 // Clock source: System Clock
;     266 // Clock value: Timer 2 Stopped
;     267 // Mode: Normal top=FFh
;     268 // OC2 output: Disconnected
;     269 TCCR2=0x00;
	OUT  0x25,R30
;     270 TCNT2=0x00;
	OUT  0x24,R30
;     271 OCR2=0x00;
	OUT  0x23,R30
;     272 
;     273 // Timer/Counter 3 initialization
;     274 // Clock source: System Clock
;     275 // Clock value: Timer 3 Stopped
;     276 // Mode: Normal top=FFFFh
;     277 // OC3A output: Discon.
;     278 // OC3B output: Discon.
;     279 // OC3C output: Discon.
;     280 TCCR3A=0x00;
	STS  0x8B,R30
;     281 TCCR3B=0x00;
	STS  0x8A,R30
;     282 TCNT3H=0x00;
	RCALL SUBOPT_0x1
;     283 TCNT3L=0x00;
;     284 OCR3AH=0x00;
	CLR  R30
	STS  0x87,R30
;     285 OCR3AL=0x00;
	STS  0x86,R30
;     286 OCR3BH=0x00;
	STS  0x85,R30
;     287 OCR3BL=0x00;
	STS  0x84,R30
;     288 OCR3CH=0x00;
	STS  0x83,R30
;     289 OCR3CL=0x00;
	STS  0x82,R30
;     290 
;     291 // External Interrupt(s) initialization
;     292 // INT0: Off
;     293 // INT1: Off
;     294 // INT2: Off
;     295 // INT3: Off
;     296 // INT4: Off
;     297 // INT5: Off
;     298 // INT6: Off
;     299 // INT7: Off
;     300 EICRA=0x00;
	STS  0x6A,R30
;     301 EICRB=0x00;
	OUT  0x3A,R30
;     302 EIMSK=0x00;
	OUT  0x39,R30
;     303 
;     304 // Timer(s)/Counter(s) Interrupt(s) initialization
;     305 TIMSK=0x00;
	OUT  0x37,R30
;     306 ETIMSK=0x00;
	STS  0x7D,R30
;     307 
;     308 // USART0 initialization
;     309 // Communication Parameters: 8 Data, 1 Stop, No Parity
;     310 // USART0 Receiver: On
;     311 // USART0 Transmitter: On
;     312 // USART0 Mode: Asynchronous
;     313 // USART0 Baud rate: 9600
;     314 UCSR0A=0x00;
	OUT  0xB,R30
;     315 UCSR0B=0xD8;
	LDI  R30,LOW(216)
	OUT  0xA,R30
;     316 UCSR0C=0x06;
	LDI  R30,LOW(6)
	STS  0x95,R30
;     317 UBRR0H=0x00;
	CLR  R30
	STS  0x90,R30
;     318 UBRR0L=0x67;
	LDI  R30,LOW(103)
	OUT  0x9,R30
;     319 
;     320 // Analog Comparator initialization
;     321 // Analog Comparator: Off
;     322 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     323 // Analog Comparator Output: Off
;     324 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     325 SFIOR=0x00;
	CLR  R30
	OUT  0x20,R30
;     326 
;     327 MCU_initialize();
	RCALL _MCU_initialize
;     328 LCD_initialize();
	RCALL _LCD_initialize
;     329 delay_ms(50);
	RCALL SUBOPT_0x2
;     330 
;     331 // Global enable interrupts
;     332 #asm("sei")       
	sei
;     333 
;     334 DDRB = 0xF0;
	LDI  R30,LOW(240)
	OUT  0x17,R30
;     335 
;     336 //Show the menu
;     337 while(1)
_0x4C:
;     338 {
;     339 	tmp = getchar();
	CALL _getchar
	MOV  R10,R30
;     340 	LCD_putchar(tmp);
	ST   -Y,R10
	RCALL _LCD_putchar
;     341 	putchar(tmp);
	ST   -Y,R10
	RCALL _putchar
;     342 		
;     343 	if (tmp == '1')
	LDI  R30,LOW(49)
	CP   R30,R10
	BRNE _0x4F
;     344 	{
;     345 		PORTB =~ PORTB;
	IN   R30,0x18
	COM  R30
	OUT  0x18,R30
;     346 	} 
;     347 	else	if (tmp == '2')
	RJMP _0x50
_0x4F:
	LDI  R30,LOW(50)
	CP   R30,R10
	BRNE _0x51
;     348 	{
;     349 		LCD_clear();
	RCALL _LCD_clear
;     350 	} 
;     351 	else	if (tmp == '3')
	RJMP _0x52
_0x51:
	LDI  R30,LOW(51)
	CP   R30,R10
	BRNE _0x53
;     352 	{		
;     353 		if (position == 0) position = 1;
	MOV  R0,R11
	OR   R0,R12
	BRNE _0x54
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	__PUTW1R 11,12
;     354 		else if (position == 1) position = 0;	 
	RJMP _0x55
_0x54:
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R11
	CPC  R31,R12
	BRNE _0x56
	CLR  R11
	CLR  R12
;     355 		
;     356 		LCD16_gotoxy_ok128(0, position);
_0x56:
_0x55:
	CLR  R30
	CLR  R31
	ST   -Y,R31
	ST   -Y,R30
	ST   -Y,R12
	ST   -Y,R11
	RCALL _LCD16_gotoxy_ok128
;     357 	}
;     358 	else	if (tmp == '4')
	RJMP _0x57
_0x53:
	LDI  R30,LOW(52)
	CP   R30,R10
	BRNE _0x58
;     359 	{		
;     360 		ambulance();
	RCALL _ambulance
;     361 	}
;     362 	else	if (tmp == '5')
	RJMP _0x59
_0x58:
	LDI  R30,LOW(53)
	CP   R30,R10
	BRNE _0x5A
;     363 	{		
;     364 		Beep();
	RCALL _Beep
;     365 	}
;     366 	else	if (tmp == '6')
	RJMP _0x5B
_0x5A:
	LDI  R30,LOW(54)
	CP   R30,R10
	BRNE _0x5C
;     367 	{		
;     368 		sendFlashString("[1] Light On/Off \a\n \\
;     369 					[2] Clear LCD \a\n        \\
;     370 					[3] Change Position \a\n    \\
;     371 					[4] Ambulance \a\n            \\
;     372 					[5] Beep \a\n\0");              
	__POINTW1FN _75,0
	ST   -Y,R31
	ST   -Y,R30
	RCALL _sendFlashString
;     373 	}	
;     374 }
_0x5C:
_0x5B:
_0x59:
_0x57:
_0x52:
_0x50:
	RJMP _0x4C
;     375 
;     376 }
_0x5D:
	RJMP _0x5D
_MCU_initialize:
	LDI  R30,LOW(128)
	OUT  0x35,R30
	LDI  R30,LOW(68)
	STS  0x6D,R30
	LDI  R30,LOW(128)
	STS  0x6C,R30
	LDI  R30,LOW(255)
	OUT  0x17,R30
	CLR  R30
	OUT  0x18,R30
	LDI  R30,LOW(255)
	OUT  0x11,R30
	CLR  R30
	OUT  0x12,R30
	LDI  R30,LOW(12)
	OUT  0x2,R30
	RCALL SUBOPT_0x0
	STS  0x61,R30
	CLR  R30
	STS  0x2200,R30
	RET
_Beep:
	SBI  0x3,2
	RCALL SUBOPT_0x2
	CBI  0x3,2
	RET
_LCD_command:
	CLR  R30
	RCALL SUBOPT_0x3
	LDI  R30,LOW(1)
	STS  0x2100,R30
	 PUSH  R0 
	 POP   R0 
	CLR  R30
	STS  0x2100,R30
	__DELAY_USW 200
	ADIW R28,1
	RET
_LCD_data:
	LDI  R30,LOW(2)
	RCALL SUBOPT_0x3
	LDI  R30,LOW(3)
	STS  0x2100,R30
	 PUSH  R0 
	 POP   R0 
	LDI  R30,LOW(2)
	STS  0x2100,R30
	__DELAY_USW 200
	RJMP _0x1A1
_LCD_putchar:
	LD   R26,Y
	LDI  R30,LOW(32)
	CALL __LTB12U
	PUSH R30
	LDI  R30,LOW(126)
	CALL __GTB12U
	POP  R26
	OR   R30,R26
	BREQ _0xB8
	CLR  R30
	CLR  R31
	RJMP _0x1A1
_0xB8:
	LD   R30,Y
	ST   -Y,R30
	RCALL _LCD_data
	CLR  R30
	CLR  R31
_0x1A1:
	ADIW R28,1
	RET
_LCD_clear:
	RCALL SUBOPT_0x4
	RET
_LCD_initialize:
	LDI  R30,LOW(3)
	STS  0x2100,R30
	LDI  R30,LOW(2)
	STS  0x2100,R30
	RCALL SUBOPT_0x5
	LDI  R30,LOW(56)
	ST   -Y,R30
	RCALL _LCD_command
	LDI  R30,LOW(12)
	ST   -Y,R30
	RCALL _LCD_command
	LDI  R30,LOW(6)
	ST   -Y,R30
	RCALL _LCD_command
	RCALL SUBOPT_0x4
	RET
_LCD16_gotoxy_ok128:
	LD   R30,Y
	LDD  R31,Y+1
	SBIW R30,0
	BRNE _0xC5
	LDD  R30,Y+2
	LDD  R31,Y+2+1
	SUBI R30,LOW(-128)
	SBCI R31,HIGH(-128)
	RCALL SUBOPT_0x6
	RJMP _0xC6
_0xC5:
	LD   R26,Y
	LDD  R27,Y+1
	CPI  R26,LOW(0x1)
	LDI  R30,HIGH(0x1)
	CPC  R27,R30
	BRNE _0xC7
	LDD  R30,Y+2
	LDD  R31,Y+2+1
	SUBI R30,LOW(-192)
	SBCI R31,HIGH(-192)
	RCALL SUBOPT_0x6
_0xC7:
_0xC6:
	ADIW R28,4
	RET
_ambulance:
	ST   -Y,R17
	ST   -Y,R16
	RCALL SUBOPT_0x7
	CLR  R30
	STS  0x89,R30
	RCALL SUBOPT_0x7
	RCALL SUBOPT_0x1
	LDI  R16,LOW(0)
_0x165:
	CPI  R16,5
	BRSH _0x166
	LDI  R30,LOW(26)
	STS  0x8A,R30
_0x168:
	LDS  R17,136
	LDS  R17,137
	CPI  R17,8
	BRSH _0x168
	LDI  R30,LOW(8)
	STS  0x81,R30
	LDI  R30,LOW(34)
	RCALL SUBOPT_0x8
_0x16B:
	LDS  R17,136
	LDS  R17,137
	CPI  R17,10
	BRSH _0x16B
	LDI  R30,LOW(10)
	STS  0x81,R30
	LDI  R30,LOW(217)
	RCALL SUBOPT_0x8
	SUBI R16,-1
	RJMP _0x165
_0x166:
	LDI  R30,LOW(24)
	STS  0x8A,R30
	LDI  R30,LOW(1000)
	LDI  R31,HIGH(1000)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
	LD   R16,Y+
	LD   R17,Y+
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x0:
	CLR  R30
	OUT  0x3,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1:
	CLR  R30
	STS  0x89,R30
	STS  0x88,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2:
	LDI  R30,LOW(50)
	LDI  R31,HIGH(50)
	ST   -Y,R31
	ST   -Y,R30
	JMP  _delay_ms

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3:
	STS  0x2100,R30
	LD   R30,Y
	STS  0x2000,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x4:
	LDI  R30,LOW(1)
	ST   -Y,R30
	RCALL _LCD_command
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	ST   -Y,R31
	ST   -Y,R30
	JMP  _delay_ms

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x5:
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	ST   -Y,R31
	ST   -Y,R30
	JMP  _delay_ms

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x6:
	ST   -Y,R30
	RCALL _LCD_command
	RJMP SUBOPT_0x5

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x7:
	LDI  R30,LOW(64)
	STS  0x8B,R30
	LDI  R30,LOW(26)
	STS  0x8A,R30
	CLR  R30
	STS  0x8C,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x8:
	STS  0x80,R30
	LDI  R30,LOW(500)
	LDI  R31,HIGH(500)
	ST   -Y,R31
	ST   -Y,R30
	JMP  _delay_ms

_delay_ms:
	ld   r30,y+
	ld   r31,y+
	adiw r30,0
	breq __delay_ms1
__delay_ms0:
	__DELAY_USW 0xFA0
	wdr
	sbiw r30,1
	brne __delay_ms0
__delay_ms1:
	ret

_fabs:
	ld   r30,y+
	ld   r31,y+
	ld   r22,y+
	ld   r23,y+
	cbr  r23,0x80
	ret

__LTB12U:
	CP   R26,R30
	LDI  R30,1
	BRLO __LTB12U1
	CLR  R30
__LTB12U1:
	RET

__GTB12U:
	CP   R30,R26
	LDI  R30,1
	BRLO __GTB12U1
	CLR  R30
__GTB12U1:
	RET

__SAVELOCR6:
	ST   -Y,R21
__SAVELOCR5:
	ST   -Y,R20
__SAVELOCR4:
	ST   -Y,R19
__SAVELOCR3:
	ST   -Y,R18
__SAVELOCR2:
	ST   -Y,R17
	ST   -Y,R16
	RET

__LOADLOCR6:
	LDD  R21,Y+5
__LOADLOCR5:
	LDD  R20,Y+4
__LOADLOCR4:
	LDD  R19,Y+3
__LOADLOCR3:
	LDD  R18,Y+2
__LOADLOCR2:
	LDD  R17,Y+1
	LD   R16,Y
	RET

__INITLOCB:
__INITLOCW:
	ADD R26,R28
	ADC R27,R29
__INITLOC0:
	LPM  R0,Z+
	ST   X+,R0
	DEC  R24
	BRNE __INITLOC0
	RET


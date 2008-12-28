
;CodeVisionAVR C Compiler V1.25.3 Standard
;(C) Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type              : ATmega32
;Program type           : Application
;Clock frequency        : 8.000000 MHz
;Memory model           : Small
;Optimize for           : Size
;(s)printf features     : int, width
;(s)scanf features      : int, width
;External SRAM size     : 0
;Data Stack size        : 512 byte(s)
;Heap size              : 0 byte(s)
;Promote char to int    : No
;char is unsigned       : Yes
;8 bit enums            : Yes
;Word align FLASH struct: No
;Enhanced core instructions    : On
;Smart register allocation : On
;Automatic register allocation : On

	#pragma AVRPART ADMIN PART_NAME ATmega32
	#pragma AVRPART MEMORY PROG_FLASH 32768
	#pragma AVRPART MEMORY EEPROM 1024
	#pragma AVRPART MEMORY INT_SRAM SIZE 2048
	#pragma AVRPART MEMORY INT_SRAM START_ADDR 0x60

	.EQU UDRE=0x5
	.EQU RXC=0x7
	.EQU USR=0xB
	.EQU UDR=0xC
	.EQU SPSR=0xE
	.EQU SPDR=0xF
	.EQU EERE=0x0
	.EQU EEWE=0x1
	.EQU EEMWE=0x2
	.EQU EECR=0x1C
	.EQU EEDR=0x1D
	.EQU EEARL=0x1E
	.EQU EEARH=0x1F
	.EQU WDTCR=0x21
	.EQU MCUCR=0x35
	.EQU GICR=0x3B
	.EQU SPL=0x3D
	.EQU SPH=0x3E
	.EQU SREG=0x3F

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

	.EQU __se_bit=0x80
	.EQU __sm_mask=0x70
	.EQU __sm_adc_noise_red=0x10
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0x60
	.EQU __sm_ext_standby=0x70

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

	.MACRO __ADDB1MN
	SUBI R30,LOW(-@0-(@1))
	.ENDM
	.MACRO __ADDB2MN
	SUBI R26,LOW(-@0-(@1))
	.ENDM
	.MACRO __ADDW1MN
	SUBI R30,LOW(-@0-(@1))
	SBCI R31,HIGH(-@0-(@1))
	.ENDM
	.MACRO __ADDW2MN
	SUBI R26,LOW(-@0-(@1))
	SBCI R27,HIGH(-@0-(@1))
	.ENDM
	.MACRO __ADDW1FN
	SUBI R30,LOW(-2*@0-(@1))
	SBCI R31,HIGH(-2*@0-(@1))
	.ENDM
	.MACRO __ADDD1FN
	SUBI R30,LOW(-2*@0-(@1))
	SBCI R31,HIGH(-2*@0-(@1))
	SBCI R22,BYTE3(-2*@0-(@1))
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

	.MACRO __ANDBMNN
	LDS  R30,@0+@1
	ANDI R30,LOW(@2)
	STS  @0+@1,R30
	.ENDM

	.MACRO __ANDWMNN
	LDS  R30,@0+@1
	ANDI R30,LOW(@2)
	STS  @0+@1,R30
	LDS  R30,@0+@1+1
	ANDI R30,HIGH(@2)
	STS  @0+@1+1,R30
	.ENDM

	.MACRO __ANDD1N
	ANDI R30,LOW(@0)
	ANDI R31,HIGH(@0)
	ANDI R22,BYTE3(@0)
	ANDI R23,BYTE4(@0)
	.ENDM

	.MACRO __ORBMNN
	LDS  R30,@0+@1
	ORI  R30,LOW(@2)
	STS  @0+@1,R30
	.ENDM

	.MACRO __ORWMNN
	LDS  R30,@0+@1
	ORI  R30,LOW(@2)
	STS  @0+@1,R30
	LDS  R30,@0+@1+1
	ORI  R30,HIGH(@2)
	STS  @0+@1+1,R30
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
	LDI  R30,0
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

	.MACRO __PUTD2S
	STD  Y+@0,R26
	STD  Y+@0+1,R27
	STD  Y+@0+2,R24
	STD  Y+@0+3,R25
	.ENDM

	.MACRO __POINTB1MN
	LDI  R30,LOW(@0+@1)
	.ENDM

	.MACRO __POINTW1MN
	LDI  R30,LOW(@0+@1)
	LDI  R31,HIGH(@0+@1)
	.ENDM

	.MACRO __POINTD1M
	LDI  R30,LOW(@0)
	LDI  R31,HIGH(@0)
	LDI  R22,BYTE3(@0)
	.ENDM

	.MACRO __POINTW1FN
	LDI  R30,LOW(2*@0+@1)
	LDI  R31,HIGH(2*@0+@1)
	.ENDM

	.MACRO __POINTD1FN
	LDI  R30,LOW(2*@0+@1)
	LDI  R31,HIGH(2*@0+@1)
	LDI  R22,BYTE3(2*@0+@1)
	.ENDM

	.MACRO __POINTB2MN
	LDI  R26,LOW(@0+@1)
	.ENDM

	.MACRO __POINTW2MN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	.ENDM

	.MACRO __POINTBRM
	LDI  R@0,LOW(@1)
	.ENDM

	.MACRO __POINTWRM
	LDI  R@0,LOW(@2)
	LDI  R@1,HIGH(@2)
	.ENDM

	.MACRO __POINTBRMN
	LDI  R@0,LOW(@1+@2)
	.ENDM

	.MACRO __POINTWRMN
	LDI  R@0,LOW(@2+@3)
	LDI  R@1,HIGH(@2+@3)
	.ENDM

	.MACRO __POINTWRFN
	LDI  R@0,LOW(@2*2+@3)
	LDI  R@1,HIGH(@2*2+@3)
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

	.MACRO __GETB1HMN
	LDS  R31,@0+@1
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
	LDS  R@0,@1+@2
	.ENDM

	.MACRO __GETWRMN
	LDS  R@0,@2+@3
	LDS  R@1,@2+@3+1
	.ENDM

	.MACRO __GETWRZ
	LDD  R@0,Z+@2
	LDD  R@1,Z+@2+1
	.ENDM

	.MACRO __GETD2Z
	LDD  R26,Z+@0
	LDD  R27,Z+@0+1
	LDD  R24,Z+@0+2
	LDD  R25,Z+@0+3
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

	.MACRO __PUTBR0MN
	STS  @0+@1,R0
	.ENDM

	.MACRO __PUTDZ2
	STD  Z+@0,R26
	STD  Z+@0+1,R27
	STD  Z+@0+2,R24
	STD  Z+@0+3,R25
	.ENDM

	.MACRO __PUTBMRN
	STS  @0+@1,R@2
	.ENDM

	.MACRO __PUTWMRN
	STS  @0+@1,R@2
	STS  @0+@1+1,R@3
	.ENDM

	.MACRO __PUTBZR
	STD  Z+@1,R@0
	.ENDM

	.MACRO __PUTWZR
	STD  Z+@2,R@0
	STD  Z+@2+1,R@1
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

	.MACRO __CALL1FN
	LDI  R30,LOW(2*@0+@1)
	LDI  R31,HIGH(2*@0+@1)
	CALL __GETW1PF
	ICALL
	.ENDM

	.MACRO __CALL2EN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	CALL __EEPROMRDW
	ICALL
	.ENDM

	.MACRO __GETW1STACK
	IN   R26,SPL
	IN   R27,SPH
	ADIW R26,@0+1
	LD   R30,X+
	LD   R31,X
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

	.MACRO __PUTB1RN
	MOVW R26,R@0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RN
	MOVW R26,R@0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RN
	MOVW R26,R@0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	CALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RNS
	MOVW R26,R@0
	ADIW R26,@1
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RNS
	MOVW R26,R@0
	ADIW R26,@1
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RNS
	MOVW R26,R@0
	ADIW R26,@1
	CALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RON
	MOV  R26,R@0
	MOV  R27,R@1
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RON
	MOV  R26,R@0
	MOV  R27,R@1
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RON
	MOV  R26,R@0
	MOV  R27,R@1
	SUBI R26,LOW(-@2)
	SBCI R27,HIGH(-@2)
	CALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RONS
	MOV  R26,R@0
	MOV  R27,R@1
	ADIW R26,@2
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RONS
	MOV  R26,R@0
	MOV  R27,R@1
	ADIW R26,@2
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RONS
	MOV  R26,R@0
	MOV  R27,R@1
	ADIW R26,@2
	CALL __PUTDP1
	.ENDM


	.MACRO __GETB1SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R30,Z
	.ENDM

	.MACRO __GETB1HSX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R31,Z
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

	.MACRO __PUTD2SX
	MOVW R30,R28
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z+,R26
	ST   Z+,R27
	ST   Z+,R24
	ST   Z,R25
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
	MOVW R30,R0
	.ENDM

	.MACRO __MULBRRU
	MUL  R@0,R@1
	MOVW R30,R0
	.ENDM

	.MACRO __MULBRR0
	MULS R@0,R@1
	.ENDM

	.MACRO __MULBRRU0
	MUL  R@0,R@1
	.ENDM

	.MACRO __MULBNWRU
	LDI  R26,@2
	MUL  R26,R@0
	MOVW R30,R0
	MUL  R26,R@1
	ADD  R31,R0
	.ENDM

	.CSEG
	.ORG 0

	.INCLUDE "t.vec"
	.INCLUDE "t.inc"

__RESET:
	CLI
	CLR  R30
	OUT  EECR,R30

;INTERRUPT VECTORS ARE PLACED
;AT THE START OF FLASH
	LDI  R31,1
	OUT  GICR,R31
	OUT  GICR,R30
	OUT  MCUCR,R30

;DISABLE WATCHDOG
	LDI  R31,0x18
	OUT  WDTCR,R31
	OUT  WDTCR,R30

;CLEAR R2-R14
	LDI  R24,13
	LDI  R26,2
	CLR  R27
__CLEAR_REG:
	ST   X+,R30
	DEC  R24
	BRNE __CLEAR_REG

;CLEAR SRAM
	LDI  R24,LOW(0x800)
	LDI  R25,HIGH(0x800)
	LDI  R26,0x60
__CLEAR_SRAM:
	ST   X+,R30
	SBIW R24,1
	BRNE __CLEAR_SRAM

;GLOBAL VARIABLES INITIALIZATION
	LDI  R30,LOW(__GLOBAL_INI_TBL*2)
	LDI  R31,HIGH(__GLOBAL_INI_TBL*2)
__GLOBAL_INI_NEXT:
	LPM  R24,Z+
	LPM  R25,Z+
	SBIW R24,0
	BREQ __GLOBAL_INI_END
	LPM  R26,Z+
	LPM  R27,Z+
	LPM  R0,Z+
	LPM  R1,Z+
	MOVW R22,R30
	MOVW R30,R0
__GLOBAL_INI_LOOP:
	LPM  R0,Z+
	ST   X+,R0
	SBIW R24,1
	BRNE __GLOBAL_INI_LOOP
	MOVW R30,R22
	RJMP __GLOBAL_INI_NEXT
__GLOBAL_INI_END:

;STACK POINTER INITIALIZATION
	LDI  R30,LOW(0x85F)
	OUT  SPL,R30
	LDI  R30,HIGH(0x85F)
	OUT  SPH,R30

;DATA STACK POINTER INITIALIZATION
	LDI  R28,LOW(0x260)
	LDI  R29,HIGH(0x260)

	JMP  _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0x260
;       1 /*****************************************************
;       2 This program was produced by the
;       3 CodeWizardAVR V1.25.3 Standard
;       4 Automatic Program Generator
;       5 © Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.
;       6 http://www.hpinfotech.com
;       7 
;       8 Project :
;       9 Version :
;      10 Date    : 12/14/2008
;      11 Author  : F4CG
;      12 Company : F4CG
;      13 Comments:
;      14 
;      15 
;      16 Chip type           : ATmega32
;      17 Program type        : Application
;      18 Clock frequency     : 8.000000 MHz
;      19 Memory model        : Small
;      20 External SRAM size  : 0
;      21 Data Stack size     : 512
;      22 *****************************************************/
;      23 
;      24 #include <mega32.h>
;      25 
;      26 // Alphanumeric LCD Module functions
;      27 #asm
;      28    .equ __lcd_port=0x15 ;PORTC
   .equ __lcd_port=0x15 ;PORTC
;      29 #endasm
;      30 #include <lcd.h>
;      31 #include <delay.h>
;      32 unsigned char RxData[5],ComaCounter,SCount;
_RxData:
	.BYTE 0x5
;      33 
;      34 
;      35 interrupt [USART_RXC] void usart_rx_isr(void)
;      36 {

	.CSEG
_usart_rx_isr:
	ST   -Y,R0
	ST   -Y,R1
	ST   -Y,R15
	ST   -Y,R22
	ST   -Y,R23
	ST   -Y,R24
	ST   -Y,R25
	ST   -Y,R26
	ST   -Y,R27
	ST   -Y,R30
	ST   -Y,R31
	IN   R30,SREG
	ST   -Y,R30
;      37     char Rx,iser;
;      38     //status=UCSR0A;
;      39     Rx=UDR;
	ST   -Y,R17
	ST   -Y,R16
;	Rx -> R17
;	iser -> R16
	IN   R17,12
;      40     lcd_putchar(Rx);
	ST   -Y,R17
	CALL _lcd_putchar
;      41     if(Rx=='S')
	CPI  R17,83
	BRNE _0x3
;      42     {
;      43         SCount++;
	INC  R4
;      44         while(SCount==8);
_0x4:
	LDI  R30,LOW(8)
	CP   R30,R4
	BREQ _0x4
;      45     }
;      46     else
	RJMP _0x7
_0x3:
;      47     {
;      48         SCount=0;
	CLR  R4
;      49     }
_0x7:
;      50     switch (Rx)
	MOV  R30,R17
;      51     {
;      52             case '$':
	CPI  R30,LOW(0x24)
	BRNE _0xB
;      53                     for(iser=0;iser<5;iser++)
	LDI  R16,LOW(0)
_0xD:
	CPI  R16,5
	BRSH _0xE
;      54                             {RxData[iser]=0;}
	MOV  R30,R16
	LDI  R31,0
	SUBI R30,LOW(-_RxData)
	SBCI R31,HIGH(-_RxData)
	LDI  R26,LOW(0)
	STD  Z+0,R26
	SUBI R16,-1
	RJMP _0xD
_0xE:
;      55                     ComaCounter = 0;
	CLR  R5
;      56                     break;
	JMP  _0xA
;      57             case ',':
_0xB:
	CPI  R30,LOW(0x2C)
	BRNE _0xF
;      58                     ComaCounter++;
	INC  R5
;      59                     break;
	JMP  _0xA
;      60             case '*':
_0xF:
	CPI  R30,LOW(0x2A)
	BRNE _0x18
;      61 	                #asm("WDR");   //}{
	WDR
;      62 
;      63                     switch (RxData[0])      //10..99 for write, //110..199 Read //210..255 control
	LDS  R30,_RxData
;      64                     {
;      65                             case 9:
	CPI  R30,LOW(0x9)
	BRNE _0x13
;      66                                 while(1);
_0x15:
	RJMP _0x15
;      67                                 break;
;      68                     }
_0x13:
;      69                    //UCSR0B=0xD8;
;      70                    //UDR0 = '$';
;      71            	break;
	RJMP _0xA
;      72             default:
_0x18:
;      73                     if ( ComaCounter<5 && ComaCounter >=1 )
	LDI  R30,LOW(5)
	CP   R5,R30
	BRSH _0x1A
	LDI  R30,LOW(1)
	CP   R5,R30
	BRSH _0x1B
_0x1A:
	RJMP _0x19
_0x1B:
;      74                             RxData[ComaCounter-1]=10*RxData[ComaCounter-1] + Rx - '0';
	MOV  R30,R5
	SUBI R30,LOW(1)
	LDI  R31,0
	SUBI R30,LOW(-_RxData)
	SBCI R31,HIGH(-_RxData)
	MOVW R22,R30
	LD   R30,Z
	LDI  R26,LOW(10)
	MUL  R30,R26
	MOVW R30,R0
	ADD  R30,R17
	SUBI R30,LOW(48)
	MOVW R26,R22
	ST   X,R30
;      75                     break;
_0x19:
;      76         }
_0xA:
;      77 }
	LD   R16,Y+
	LD   R17,Y+
	LD   R30,Y+
	OUT  SREG,R30
	LD   R31,Y+
	LD   R30,Y+
	LD   R27,Y+
	LD   R26,Y+
	LD   R25,Y+
	LD   R24,Y+
	LD   R23,Y+
	LD   R22,Y+
	LD   R15,Y+
	LD   R1,Y+
	LD   R0,Y+
	RETI
;      78 
;      79 // Declare your global variables here
;      80 
;      81 void main(void)
;      82 {
_main:
;      83 // Declare your local variables here
;      84 
;      85 // Input/Output Ports initialization
;      86 // Port A initialization
;      87 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
;      88 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
;      89 PORTA=0x00;
	LDI  R30,LOW(0)
	OUT  0x1B,R30
;      90 DDRA=0x00;
	OUT  0x1A,R30
;      91 
;      92 // Port B initialization
;      93 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
;      94 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
;      95 PORTB=0x00;
	OUT  0x18,R30
;      96 DDRB=0x00;
	OUT  0x17,R30
;      97 
;      98 // Port C initialization
;      99 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
;     100 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
;     101 PORTC=0x00;
	OUT  0x15,R30
;     102 DDRC=0x00;
	OUT  0x14,R30
;     103 
;     104 // Port D initialization
;     105 // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
;     106 // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
;     107 PORTD=0x00;
	OUT  0x12,R30
;     108 DDRD=0x00;
	OUT  0x11,R30
;     109 
;     110 // Timer/Counter 0 initialization
;     111 // Clock source: System Clock
;     112 // Clock value: Timer 0 Stopped
;     113 // Mode: Normal top=FFh
;     114 // OC0 output: Disconnected
;     115 TCCR0=0x00;
	OUT  0x33,R30
;     116 TCNT0=0x00;
	OUT  0x32,R30
;     117 OCR0=0x00;
	OUT  0x3C,R30
;     118 
;     119 // Timer/Counter 1 initialization
;     120 // Clock source: System Clock
;     121 // Clock value: Timer 1 Stopped
;     122 // Mode: Normal top=FFFFh
;     123 // OC1A output: Discon.
;     124 // OC1B output: Discon.
;     125 // Noise Canceler: Off
;     126 // Input Capture on Falling Edge
;     127 // Timer 1 Overflow Interrupt: Off
;     128 // Input Capture Interrupt: Off
;     129 // Compare A Match Interrupt: Off
;     130 // Compare B Match Interrupt: Off
;     131 TCCR1A=0x00;
	OUT  0x2F,R30
;     132 TCCR1B=0x00;
	OUT  0x2E,R30
;     133 TCNT1H=0x00;
	OUT  0x2D,R30
;     134 TCNT1L=0x00;
	OUT  0x2C,R30
;     135 ICR1H=0x00;
	OUT  0x27,R30
;     136 ICR1L=0x00;
	OUT  0x26,R30
;     137 OCR1AH=0x00;
	OUT  0x2B,R30
;     138 OCR1AL=0x00;
	OUT  0x2A,R30
;     139 OCR1BH=0x00;
	OUT  0x29,R30
;     140 OCR1BL=0x00;
	OUT  0x28,R30
;     141 
;     142 // Timer/Counter 2 initialization
;     143 // Clock source: System Clock
;     144 // Clock value: Timer 2 Stopped
;     145 // Mode: Normal top=FFh
;     146 // OC2 output: Disconnected
;     147 ASSR=0x00;
	OUT  0x22,R30
;     148 TCCR2=0x00;
	OUT  0x25,R30
;     149 TCNT2=0x00;
	OUT  0x24,R30
;     150 OCR2=0x00;
	OUT  0x23,R30
;     151 
;     152 // External Interrupt(s) initialization
;     153 // INT0: Off
;     154 // INT1: Off
;     155 // INT2: Off
;     156 MCUCR=0x00;
	OUT  0x35,R30
;     157 MCUCSR=0x00;
	OUT  0x34,R30
;     158 
;     159 // Timer(s)/Counter(s) Interrupt(s) initialization
;     160 TIMSK=0x00;
	OUT  0x39,R30
;     161 // USART initialization
;     162 // Communication Parameters: 8 Data, 1 Stop, No Parity
;     163 // USART Receiver: On
;     164 // USART Transmitter: On
;     165 // USART Mode: Asynchronous
;     166 // USART Baud rate: 9600
;     167 UCSRA=0x00;
	OUT  0xB,R30
;     168 UCSRB=0x98;
	LDI  R30,LOW(152)
	OUT  0xA,R30
;     169 UCSRC=0x86;
	LDI  R30,LOW(134)
	OUT  0x20,R30
;     170 UBRRH=0x00;
	LDI  R30,LOW(0)
	OUT  0x20,R30
;     171 UBRRL=0x33;
	LDI  R30,LOW(51)
	OUT  0x9,R30
;     172 
;     173 // Analog Comparator initialization
;     174 // Analog Comparator: Off
;     175 // Analog Comparator Input Capture by Timer/Counter 1: Off
;     176 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     177 SFIOR=0x00;
	LDI  R30,LOW(0)
	OUT  0x30,R30
;     178 
;     179 // Watchdog Timer initialization
;     180 // Watchdog Timer Prescaler: OSC/64k
;     181 #asm("WDR");
	WDR
;     182 WDTCR=0x0D;
	LDI  R30,LOW(13)
	OUT  0x21,R30
;     183 
;     184 // Global enable interrupts
;     185 #asm("sei")
	sei
;     186 
;     187 // LCD module initialization
;     188 lcd_init(20);
	LDI  R30,LOW(20)
	ST   -Y,R30
	CALL _lcd_init
;     189 
;     190 while (1)
_0x1C:
;     191       {
;     192       // Place your code here
;     193       #asm("wdr");
	wdr
;     194       lcd_putsf("Ramin. ");
	__POINTW1FN _0,0
	ST   -Y,R31
	ST   -Y,R30
	CALL _lcd_putsf
;     195       delay_ms(500);
	LDI  R30,LOW(500)
	LDI  R31,HIGH(500)
	ST   -Y,R31
	ST   -Y,R30
	CALL _delay_ms
;     196       };
	JMP  _0x1C
;     197 }
_0x1F:
	RJMP _0x1F

    .equ __lcd_direction=__lcd_port-1
    .equ __lcd_pin=__lcd_port-2
    .equ __lcd_rs=0
    .equ __lcd_rd=1
    .equ __lcd_enable=2
    .equ __lcd_busy_flag=7

	.DSEG
__base_y_G2:
	.BYTE 0x4

	.CSEG
__lcd_delay_G2:
    ldi   r31,100
__lcd_delay0:
    dec   r31
    brne  __lcd_delay0
	RET
__lcd_ready:
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
    cbi   __lcd_port,__lcd_rs     ;RS=0
__lcd_busy:
	CALL SUBOPT_0x0
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL SUBOPT_0x0
    in    r26,__lcd_pin
    cbi   __lcd_port,__lcd_enable ;EN=0
	CALL SUBOPT_0x0
	CALL __lcd_delay_G2
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL SUBOPT_0x0
	CALL SUBOPT_0x0
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL SUBOPT_0x0
	CALL SUBOPT_0x0
    cbi   __lcd_port,__lcd_enable ;EN=0
    sbrc  r26,__lcd_busy_flag
    cbi   __lcd_port,__lcd_enable ;EN=0
    ;rjmp  __lcd_busy
	CALL SUBOPT_0x0
	CALL SUBOPT_0x0
	RET
__lcd_write_nibble_G2:
    andi  r26,0xf0
    or    r26,r27
    out   __lcd_port,r26          ;write
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL __lcd_delay_G2
    cbi   __lcd_port,__lcd_enable ;EN=0
	CALL __lcd_delay_G2
	RET
__lcd_write_data:
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf0 | (1<<__lcd_rs) | (1<<__lcd_rd) | (1<<__lcd_enable) ;set as output    
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
	CALL __lcd_write_nibble_G2
    ld    r26,y
    swap  r26
	CALL __lcd_write_nibble_G2
    sbi   __lcd_port,__lcd_rd     ;RD=1
	ADIW R28,1
	RET
__lcd_read_nibble_G2:
    sbi   __lcd_port,__lcd_enable ;EN=1
	CALL __lcd_delay_G2
    in    r30,__lcd_pin           ;read
    cbi   __lcd_port,__lcd_enable ;EN=0
	CALL __lcd_delay_G2
    andi  r30,0xf0
	RET
_lcd_read_byte0_G2:
	CALL __lcd_delay_G2
	CALL __lcd_read_nibble_G2
    mov   r26,r30
	CALL __lcd_read_nibble_G2
    cbi   __lcd_port,__lcd_rd     ;RD=0
    swap  r30
    or    r30,r26
	RET
_lcd_gotoxy:
	CALL __lcd_ready
	LD   R30,Y
	LDI  R31,0
	SUBI R30,LOW(-__base_y_G2)
	SBCI R31,HIGH(-__base_y_G2)
	LD   R30,Z
	LDD  R26,Y+1
	ADD  R30,R26
	ST   -Y,R30
	CALL __lcd_write_data
	LDD  R7,Y+1
	LDD  R6,Y+0
	ADIW R28,2
	RET
_lcd_clear:
	CALL __lcd_ready
	LDI  R30,LOW(2)
	ST   -Y,R30
	CALL __lcd_write_data
	CALL __lcd_ready
	LDI  R30,LOW(12)
	ST   -Y,R30
	CALL __lcd_write_data
	CALL __lcd_ready
	LDI  R30,LOW(1)
	ST   -Y,R30
	CALL __lcd_write_data
	LDI  R30,LOW(0)
	MOV  R6,R30
	MOV  R7,R30
	RET
_lcd_putchar:
    push r30
    push r31
    ld   r26,y
    set
    cpi  r26,10
    breq __lcd_putchar1
    clt
	INC  R7
	CP   R9,R7
	BRSH _0x21
	__lcd_putchar1:
	INC  R6
	LDI  R30,LOW(0)
	ST   -Y,R30
	ST   -Y,R6
	CALL _lcd_gotoxy
	brts __lcd_putchar0
_0x21:
    rcall __lcd_ready
    sbi  __lcd_port,__lcd_rs ;RS=1
    ld   r26,y
    st   -y,r26
    rcall __lcd_write_data
__lcd_putchar0:
    pop  r31
    pop  r30
	ADIW R28,1
	RET
_lcd_putsf:
	ST   -Y,R17
_0x25:
	LDD  R30,Y+1
	LDD  R31,Y+1+1
	ADIW R30,1
	STD  Y+1,R30
	STD  Y+1+1,R31
	SBIW R30,1
	LPM  R30,Z
	MOV  R17,R30
	CPI  R30,0
	BREQ _0x27
	ST   -Y,R17
	CALL _lcd_putchar
	RJMP _0x25
_0x27:
	LDD  R17,Y+0
	ADIW R28,3
	RET
__long_delay_G2:
    clr   r26
    clr   r27
__long_delay0:
    sbiw  r26,1         ;2 cycles
    brne  __long_delay0 ;2 cycles
	RET
__lcd_init_write_G2:
    cbi  __lcd_port,__lcd_rd 	  ;RD=0
    in    r26,__lcd_direction
    ori   r26,0xf7                ;set as output
    out   __lcd_direction,r26
    in    r27,__lcd_port
    andi  r27,0xf
    ld    r26,y
	CALL __lcd_write_nibble_G2
    sbi   __lcd_port,__lcd_rd     ;RD=1
	ADIW R28,1
	RET
_lcd_init:
    cbi   __lcd_port,__lcd_enable ;EN=0
    cbi   __lcd_port,__lcd_rs     ;RS=0
	LDD  R9,Y+0
	LD   R30,Y
	SUBI R30,-LOW(128)
	__PUTB1MN __base_y_G2,2
	LD   R30,Y
	SUBI R30,-LOW(192)
	__PUTB1MN __base_y_G2,3
	CALL SUBOPT_0x1
	CALL SUBOPT_0x1
	CALL SUBOPT_0x1
	CALL __long_delay_G2
	LDI  R30,LOW(32)
	ST   -Y,R30
	CALL __lcd_init_write_G2
	CALL __long_delay_G2
	LDI  R30,LOW(40)
	CALL SUBOPT_0x2
	LDI  R30,LOW(4)
	CALL SUBOPT_0x2
	LDI  R30,LOW(133)
	CALL SUBOPT_0x2
    in    r26,__lcd_direction
    andi  r26,0xf                 ;set as input
    out   __lcd_direction,r26
    sbi   __lcd_port,__lcd_rd     ;RD=1
	CALL _lcd_read_byte0_G2
	CPI  R30,LOW(0x5)
	BREQ _0x28
	LDI  R30,LOW(0)
	RJMP _0x29
_0x28:
	CALL __lcd_ready
	LDI  R30,LOW(6)
	ST   -Y,R30
	CALL __lcd_write_data
	CALL _lcd_clear
	LDI  R30,LOW(1)
_0x29:
	ADIW R28,1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 9 TIMES, CODE SIZE REDUCTION:13 WORDS
SUBOPT_0x0:
	CALL __lcd_delay_G2
	JMP  __lcd_delay_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:5 WORDS
SUBOPT_0x1:
	CALL __long_delay_G2
	LDI  R30,LOW(48)
	ST   -Y,R30
	JMP  __lcd_init_write_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES, CODE SIZE REDUCTION:3 WORDS
SUBOPT_0x2:
	ST   -Y,R30
	CALL __lcd_write_data
	JMP  __long_delay_G2

_delay_ms:
	ld   r30,y+
	ld   r31,y+
	adiw r30,0
	breq __delay_ms1
__delay_ms0:
	__DELAY_USW 0x7D0
	wdr
	sbiw r30,1
	brne __delay_ms0
__delay_ms1:
	ret

;END OF CODE MARKER
__END_OF_CODE:

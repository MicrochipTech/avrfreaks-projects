
;CodeVisionAVR C Compiler V2.04.4 Standard
;(C) Copyright 1998-2009 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Chip type                : ATtiny5
;Program type             : Application
;Clock frequency          : 12.000000 MHz
;Memory model             : Tiny
;Optimize for             : Speed
;(s)printf features       : int, width
;(s)scanf features        : int, width
;External RAM size        : 0
;Data Stack size          : 12 byte(s)
;Heap size                : 0 byte(s)
;Promote 'char' to 'int'  : No
;'char' is unsigned       : Yes
;8 bit enums              : Yes
;global 'const' stored in FLASH: No
;'auto' var. watch in AVR Studio: Off

	#pragma AVRPART ADMIN PART_NAME ATtiny5
	#pragma AVRPART CORE CORE_VERSION AVR8L_0
	#pragma AVRPART MEMORY PROG_FLASH 512
	#pragma AVRPART MEMORY EEPROM 0
	#pragma AVRPART MEMORY INT_SRAM SIZE 32
	#pragma AVRPART MEMORY INT_SRAM START_ADDR 0x40

	.LISTMAC
	.EQU WDTCSR=0x31
	.EQU SMCR=0x3A
	.EQU RSTFLR=0x3B
	.EQU CCP=0x3C
	.EQU SPL=0x3D
	.EQU SPH=0x3E
	.EQU SREG=0x3F

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
	LDS  R30,@0+(@1)
	ANDI R30,LOW(@2)
	STS  @0+(@1),R30
	.ENDM

	.MACRO __ANDWMNN
	LDS  R30,@0+(@1)
	ANDI R30,LOW(@2)
	STS  @0+(@1),R30
	LDS  R30,@0+(@1)+1
	ANDI R30,HIGH(@2)
	STS  @0+(@1)+1,R30
	.ENDM

	.MACRO __ANDD1N
	ANDI R30,LOW(@0)
	ANDI R31,HIGH(@0)
	ANDI R22,BYTE3(@0)
	ANDI R23,BYTE4(@0)
	.ENDM

	.MACRO __ANDD2N
	ANDI R26,LOW(@0)
	ANDI R27,HIGH(@0)
	ANDI R24,BYTE3(@0)
	ANDI R25,BYTE4(@0)
	.ENDM

	.MACRO __ORBMNN
	LDS  R30,@0+(@1)
	ORI  R30,LOW(@2)
	STS  @0+(@1),R30
	.ENDM

	.MACRO __ORWMNN
	LDS  R30,@0+(@1)
	ORI  R30,LOW(@2)
	STS  @0+(@1),R30
	LDS  R30,@0+(@1)+1
	ORI  R30,HIGH(@2)
	STS  @0+(@1)+1,R30
	.ENDM

	.MACRO __ORD1N
	ORI  R30,LOW(@0)
	ORI  R31,HIGH(@0)
	ORI  R22,BYTE3(@0)
	ORI  R23,BYTE4(@0)
	.ENDM

	.MACRO __ORD2N
	ORI  R26,LOW(@0)
	ORI  R27,HIGH(@0)
	ORI  R24,BYTE3(@0)
	ORI  R25,BYTE4(@0)
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
	SUBI R24,1
	SBCI R25,0
	BRNE __DELAY_USW_LOOP
	.ENDM

	.MACRO __POINTB1MN
	LDI  R30,LOW(@0+(@1))
	.ENDM

	.MACRO __POINTW1MN
	LDI  R30,LOW(@0+(@1))
	LDI  R31,HIGH(@0+(@1))
	.ENDM

	.MACRO __POINTD1M
	LDI  R30,LOW(@0)
	LDI  R31,HIGH(@0)
	LDI  R22,BYTE3(@0)
	LDI  R23,BYTE4(@0)
	.ENDM

	.MACRO __POINTW1FN
	LDI  R30,LOW(2*@0+(@1))
	LDI  R31,HIGH(2*@0+(@1))
	.ENDM

	.MACRO __POINTD1FN
	LDI  R30,LOW(2*@0+(@1))
	LDI  R31,HIGH(2*@0+(@1))
	LDI  R22,BYTE3(2*@0+(@1))
	LDI  R23,BYTE4(2*@0+(@1))
	.ENDM

	.MACRO __POINTB2MN
	LDI  R26,LOW(@0+(@1))
	.ENDM

	.MACRO __POINTW2MN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	.ENDM

	.MACRO __POINTBRM
	LDI  R@0,LOW(@1)
	.ENDM

	.MACRO __POINTWRM
	LDI  R@0,LOW(@2)
	LDI  R@1,HIGH(@2)
	.ENDM

	.MACRO __POINTBRMN
	LDI  R@0,LOW(@1+(@2))
	.ENDM

	.MACRO __POINTWRMN
	LDI  R@0,LOW(@2+(@3))
	LDI  R@1,HIGH(@2+(@3))
	.ENDM

	.MACRO __POINTWRFN
	LDI  R@0,LOW(@2*2+(@3))
	LDI  R@1,HIGH(@2*2+(@3))
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

	.MACRO __GETB1MN
	LDS  R30,@0+(@1)
	.ENDM

	.MACRO __GETB1HMN
	LDS  R31,@0+(@1)
	.ENDM

	.MACRO __GETW1MN
	LDS  R30,@0+(@1)
	LDS  R31,@0+(@1)+1
	.ENDM

	.MACRO __GETD1MN
	LDS  R30,@0+(@1)
	LDS  R31,@0+(@1)+1
	LDS  R22,@0+(@1)+2
	LDS  R23,@0+(@1)+3
	.ENDM

	.MACRO __GETBRMN
	LDS  R@0,@1+(@2)
	.ENDM

	.MACRO __GETWRMN
	LDS  R@0,@2+(@3)
	LDS  R@1,@2+(@3)+1
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
	LDS  R26,@0+(@1)
	.ENDM

	.MACRO __GETW2MN
	LDS  R26,@0+(@1)
	LDS  R27,@0+(@1)+1
	.ENDM

	.MACRO __GETD2MN
	LDS  R26,@0+(@1)
	LDS  R27,@0+(@1)+1
	LDS  R24,@0+(@1)+2
	LDS  R25,@0+(@1)+3
	.ENDM

	.MACRO __PUTB1MN
	STS  @0+(@1),R30
	.ENDM

	.MACRO __PUTW1MN
	STS  @0+(@1),R30
	STS  @0+(@1)+1,R31
	.ENDM

	.MACRO __PUTD1MN
	STS  @0+(@1),R30
	STS  @0+(@1)+1,R31
	STS  @0+(@1)+2,R22
	STS  @0+(@1)+3,R23
	.ENDM

	.MACRO __PUTB1EN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	RCALL __EEPROMWRB
	.ENDM

	.MACRO __PUTW1EN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	RCALL __EEPROMWRW
	.ENDM

	.MACRO __PUTD1EN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	RCALL __EEPROMWRD
	.ENDM

	.MACRO __PUTBR0MN
	STS  @0+(@1),R0
	.ENDM

	.MACRO __PUTBMRN
	STS  @0+(@1),R@2
	.ENDM

	.MACRO __PUTWMRN
	STS  @0+(@1),R@2
	STS  @0+(@1)+1,R@3
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

	.MACRO __GETBRS
	SUBI R28,-@1
	LD   R@0,Y
	.ENDM

	.MACRO __PUTBSR
	SUBI R28,-@1
	ST   Y,R@0
	.ENDM

	.MACRO __GETWRS
	SUBI R28,-@2
	LD   R@0,Y+
	LD   R@1,Y
	.ENDM

	.MACRO __PUTWSR
	SUBI R28,-@2
	ST   Y+,R@0
	ST   Y,R@1
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
	LDS  R30,@0+(@1)
	LDS  R31,@0+(@1)+1
	ICALL
	.ENDM

	.MACRO __CALL1FN
	LDI  R30,LOW(2*@0+(@1))
	LDI  R31,HIGH(2*@0+(@1))
	RCALL __GETW1PF
	ICALL
	.ENDM

	.MACRO __CALL2EN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	RCALL __EEPROMRDW
	ICALL
	.ENDM

	.MACRO __GETW1STACK
	IN   R26,SPL
	SUBI R26,LOW(-@0-1)
	LDI  R27,0
	LD   R30,X+
	LD   R31,X
	.ENDM

	.MACRO __GETD1STACK
	IN   R26,SPL
	IN   R27,SPH
	ADIW R26,@0+1
	LD   R30,X+
	LD   R31,X+
	LD   R22,X
	.ENDM

	.MACRO __NBST
	BST  R@0,@1
	IN   R30,SREG
	LDI  R31,0x40
	EOR  R30,R31
	OUT  SREG,R30
	.ENDM


	.MACRO __PUTB1SN
	LD   R26,Y
	SUBI R26,-@0
	LDI  R27,0
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SN
	LD   R26,Y
	SUBI R26,-@0
	LDI  R27,0
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SN
	LD   R26,Y
	SUBI R26,-@0
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1SNS
	LD   R26,Y
	SUBI R26,-@0
	LDI  R27,0
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SNS
	LD   R26,Y
	SUBI R26,-@0
	LDI  R27,0
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SNS
	LD   R26,Y
	SUBI R26,-@0
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RN
	MOV  R26,R@0
	SUBI R26,-@1
	LDI  R27,0
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RN
	MOV  R26,R@0
	SUBI R26,-@1
	LDI  R27,0
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RN
	MOV  R26,R@0
	SUBI R26,-@1
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RNS
	MOV  R26,R@0
	SUBI R26,-@1
	LDI  R27,0
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RNS
	MOV  R26,R@0
	SUBI R26,-@1
	LDI  R27,0
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RNS
	MOV  R26,R@0
	SUBI R26,-@1
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1PMN
	LDS  R26,@0
	SUBI R26,-@1
	LDI  R27,0
	ST   X,R30
	.ENDM

	.MACRO __PUTW1PMN
	LDS  R26,@0
	SUBI R26,-@1
	LDI  R27,0
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1PMN
	LDS  R26,@0
	SUBI R26,-@1
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1PMNS
	LDS  R26,@0
	SUBI R26,-@1
	LDI  R27,0
	ST   X,R30
	.ENDM

	.MACRO __PUTW1PMNS
	LDS  R26,@0
	SUBI R26,-@1
	LDI  R27,0
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1PMNS
	LDS  R26,@0
	SUBI R26,-@1
	RCALL __PUTDP1
	.ENDM

	.CSEG
	.ORG 0x00

;INTERRUPT VECTORS
	RJMP __RESET
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP _timer0_compa_isr
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP _adc_isr

__RESET:
	CLI
	CLR  R30

;DISABLE WATCHDOG
	LDI  R31,0xD8
	WDR
	IN   R26,RSTFLR
	CBR  R26,8
	OUT  RSTFLR,R26
	OUT  CCP,R31
	OUT  WDTCSR,R30

;CLEAR SRAM
	LDI  R24,LOW(0x20)
	LDI  R26,LOW(0x40)
	LDI  R27,HIGH(0x40)
__CLEAR_SRAM:
	ST   X+,R30
	DEC  R24
	BRNE __CLEAR_SRAM

;STACK POINTER INITIALIZATION
	LDI  R30,LOW(0x5F)
	OUT  SPL,R30
	LDI  R30,HIGH(0x5F)
	OUT  SPH,R30

;DATA STACK POINTER INITIALIZATION
	LDI  R28,LOW(0x4C)
	LDI  R29,HIGH(0x4C)

	RJMP _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0x4C

	.CSEG
;/*****************************************************
;This program was produced by the
;CodeWizardAVR V2.04.3a Standard
;Automatic Program Generator
;© Copyright 1998-2009 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com
;
;Project : Tiny5 Level Meter
;Version : 0.001
;Date    : 8/31/2009
;Author  : Brian Hammill
;Company : Atmel Corporation
;Comments:
;Simple project for the ADC on the Tiny5
;
;
;Chip type               : ATtiny5
;AVR Core Clock frequency: 12.000000 MHz
;Memory model            : Tiny
;External RAM size       : 0
;Data Stack size         : 8
;*****************************************************/
;
;#include <tiny5.h>
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x01
	.EQU __sm_mask=0xe0
	.EQU __sm_adc_noise_red=0x02
	.EQU __sm_powerdown=0x04
    .EQU __sm_standby=0x08
	.SET power_ctrl_reg=smcr
	#endif
;
;#include <delay.h>
;
;#define THRESHOLD_LOW   102     // (Low is 0 to 2 volts
;#define THRESHOLD_HIGH  153     // (High is 3 to 5 volts
;// Between 2 and 3 is MID.  Assume 5 volt Vcc for reference voltage.
;
;// Define 3 Active Low LED
;#define LED_LOW     PORTB1
;#define LED_MID     PORTB2
;#define LED_HIGH    PORTB3
;
;#define TRUE    1
;#define FALSE   0
;
;
;
;void update_leds(void);     // Function Prototype
;
;unsigned char new_data;     // Flag to tell us if the ADC has updated.
;
;
;// Timer 0 output compare A interrupt service routine
;interrupt [TIM0_COMPA] void timer0_compa_isr(void)
; 0000 0031 {

	.CSEG
_timer0_compa_isr:
	ST   -Y,R16
	ST   -Y,R17
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
; 0000 0032 // Place your code here
; 0000 0033 update_leds();
	RCALL _update_leds
; 0000 0034 new_data = FALSE;
	LDI  R30,LOW(0)
	STS  _new_data,R30
; 0000 0035 
; 0000 0036 }
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
	LD   R17,Y+
	LD   R16,Y+
	RETI
;
;#define FIRST_ADC_INPUT 0
;//#define LAST_ADC_INPUT 0
;unsigned char adc_data;
;
;
;// ADC interrupt service routine
;// with auto input scanning
;interrupt [ADC_INT] void adc_isr(void)
; 0000 0040 {
_adc_isr:
	ST   -Y,R24
	ST   -Y,R30
	IN   R30,SREG
	ST   -Y,R30
; 0000 0041 // static unsigned char input_index=0;
; 0000 0042 // Read the A/D conversion result
; 0000 0043 adc_data=ADCL;
	IN   R30,0x19
	STS  _adc_data,R30
; 0000 0044 new_data = TRUE;    // Access control to be sure ADC data is written.
	LDI  R30,LOW(1)
	STS  _new_data,R30
; 0000 0045 
; 0000 0046 // Delay needed for the stabilization of the ADC input voltage
; 0000 0047 delay_us(10);
	__DELAY_USB 40
; 0000 0048 // Start the A/D conversion
; 0000 0049 ADCSRA|=0x40;
	SBI  0x1D,6
; 0000 004A }
	LD   R30,Y+
	OUT  SREG,R30
	LD   R30,Y+
	LD   R24,Y+
	RETI
;
;void update_leds()
; 0000 004D {
_update_leds:
; 0000 004E     // We have 3 LEDs on PB1, PB2, and PB3.
; 0000 004F     // ADC0 is on PB0.
; 0000 0050     // There are no more pins...
; 0000 0051     // Based on the ADC reading, we want to light up 1 of 3 LEDs
; 0000 0052 
; 0000 0053     if (adc_data <= THRESHOLD_LOW )
	LDS  R26,_adc_data
	CPI  R26,LOW(0x67)
	BRLO PC+2
	RJMP _0x3
; 0000 0054     {   // Want to light up LED on PB1 (LOW)
; 0000 0055         PORTB = ~(1<<LED_LOW);
	LDI  R30,LOW(253)
	OUT  0x2,R30
; 0000 0056         }
; 0000 0057 
; 0000 0058     else if ( adc_data >= THRESHOLD_HIGH )
	RJMP _0x4
_0x3:
	LDS  R26,_adc_data
	CPI  R26,LOW(0x99)
	BRSH PC+2
	RJMP _0x5
; 0000 0059     {
; 0000 005A         // Want to light up LED on PB3 (HIGH)
; 0000 005B         PORTB = ~(1<<LED_HIGH);
	LDI  R30,LOW(247)
	OUT  0x2,R30
; 0000 005C         }
; 0000 005D 
; 0000 005E     else // Voltage on ADC is between the two thresholds.
	RJMP _0x6
_0x5:
; 0000 005F     {
; 0000 0060         // Want to light up LED on PB2 (MID)
; 0000 0061         PORTB = ~(1<<LED_MID);
	LDI  R30,LOW(251)
	OUT  0x2,R30
; 0000 0062         }
_0x6:
_0x4:
; 0000 0063 
; 0000 0064 
; 0000 0065 }
	RET
;
;// Declare your global variables here
;
;void main(void)
; 0000 006A {
_main:
; 0000 006B // Declare your local variables here
; 0000 006C 
; 0000 006D // Clock Prescaler division factor: 1
; 0000 006E #pragma optsize-
; 0000 006F CCP=0xD8;
	LDI  R30,LOW(216)
	OUT  0x3C,R30
; 0000 0070 CLKPSR=0x00;
	LDI  R30,LOW(0)
	OUT  0x36,R30
; 0000 0071 #ifdef _OPTIMIZE_SIZE_
; 0000 0072 #pragma optsize+
; 0000 0073 #endif
; 0000 0074 
; 0000 0075 // Voltage Level Monitor
; 0000 0076 // Trigger Level: 2.5V -> Power-On Reset
; 0000 0077 // Interrupt: Off
; 0000 0078 VLMCSR=0x02;
	LDI  R30,LOW(2)
	OUT  0x34,R30
; 0000 0079 
; 0000 007A // Input/Output Ports initialization
; 0000 007B // Port B initialization
; 0000 007C PUEB=0x00;
	LDI  R30,LOW(0)
	OUT  0x3,R30
; 0000 007D // Break Before Make Mode: Off
; 0000 007E PORTCR=0x00;
	OUT  0xC,R30
; 0000 007F // Func3=Out Func2=Out Func1=Out Func0=In
; 0000 0080 // State3=1 State2=1 State1=1 State0=T
; 0000 0081 PORTB=0x0E;
	LDI  R30,LOW(14)
	OUT  0x2,R30
; 0000 0082 DDRB=0x0E;
	OUT  0x1,R30
; 0000 0083 
; 0000 0084 // Timer/Counter 0 initialization
; 0000 0085 // Clock source: System Clock
; 0000 0086 // Clock value: 1500.000 kHz
; 0000 0087 // Mode: Normal top=FFFFh
; 0000 0088 // Input Capture on Falling Edge
; 0000 0089 // Input Capture Noise Canceler: Off
; 0000 008A // OC0A output: Disconnected
; 0000 008B // OC0B output: Disconnected
; 0000 008C TCCR0A=0x00;
	LDI  R30,LOW(0)
	OUT  0x2E,R30
; 0000 008D TCCR0B=0x02;
	LDI  R30,LOW(2)
	OUT  0x2D,R30
; 0000 008E TCNT0=0x0000;
	LDI  R30,LOW(0)
	LDI  R31,HIGH(0)
	OUT  0x28+1,R31
	OUT  0x28,R30
; 0000 008F ICR0=0x0000;
	OUT  0x22+1,R31
	OUT  0x22,R30
; 0000 0090 OCR0A=0x0400;   // Was 0x1500.
	LDI  R30,LOW(1024)
	LDI  R31,HIGH(1024)
	OUT  0x26+1,R31
	OUT  0x26,R30
; 0000 0091 OCR0B=0x0000;
	LDI  R30,LOW(0)
	LDI  R31,HIGH(0)
	OUT  0x24+1,R31
	OUT  0x24,R30
; 0000 0092 
; 0000 0093 // External Interrupt(s) initialization
; 0000 0094 // INT0: Off
; 0000 0095 // Interrupt on any change on pins PCINT0-3: Off
; 0000 0096 EICRA=0x00;
	OUT  0x15,R30
; 0000 0097 EIMSK=0x00;
	OUT  0x13,R30
; 0000 0098 PCICR=0x00;
	OUT  0x12,R30
; 0000 0099 
; 0000 009A // Timer/Counter 0 Interrupt(s) initialization
; 0000 009B TIMSK0=0x02;
	LDI  R30,LOW(2)
	OUT  0x2B,R30
; 0000 009C 
; 0000 009D // Analog Comparator initialization
; 0000 009E // Analog Comparator: Off
; 0000 009F // Analog Comparator Input Capture by Timer/Counter 0: Off
; 0000 00A0 ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x1F,R30
; 0000 00A1 
; 0000 00A2 // ADC initialization
; 0000 00A3 // ADC Clock frequency: 93.750 kHz
; 0000 00A4 // ADC Auto Trigger Source: Free Running
; 0000 00A5 // Digital input buffers on ADC0: Off, ADC1: On, ADC2: On, ADC3: On
; 0000 00A6 DIDR0=0x01;
	LDI  R30,LOW(1)
	OUT  0x17,R30
; 0000 00A7 ADMUX=FIRST_ADC_INPUT;
	LDI  R30,LOW(0)
	OUT  0x1B,R30
; 0000 00A8 ADCSRA=0xEF;
	LDI  R30,LOW(239)
	OUT  0x1D,R30
; 0000 00A9 ADCSRB=0x00;
	LDI  R30,LOW(0)
	OUT  0x1C,R30
; 0000 00AA 
; 0000 00AB // Global enable interrupts
; 0000 00AC #asm("sei")
	sei
; 0000 00AD 
; 0000 00AE while (1)
_0x7:
; 0000 00AF       {
; 0000 00B0       // Place your code here
; 0000 00B1 
; 0000 00B2       };
	RJMP _0x7
_0x9:
; 0000 00B3 }
_0xA:
	RJMP _0xA

	.DSEG
_new_data:
	.BYTE 0x1
_adc_data:
	.BYTE 0x1

	.CSEG

	.CSEG
;END OF CODE MARKER
__END_OF_CODE:

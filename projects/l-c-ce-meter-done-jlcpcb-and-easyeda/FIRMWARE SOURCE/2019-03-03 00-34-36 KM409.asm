//------------------------------------------------------------
//---            L/C meter firmware for AVR                ---
//---          ATmega328P / ATmega8A version               ---
//---                   2019-03-02                         ---
//------------------------------------------------------------


#define m328p

#ifdef m328p
.include "m328Pdef.inc"
#else
.include "m8Adef.inc"
#endif

//---------------------
// Lcd = PORTC.3,2,1,0
// RS=PORTC.5
// EN=PORTC.4
// LCD_TYPE = 16 * 2
//---------------------

//------------------------
//---  for ATMega 8A  ----
//------------------------
//        LO   HI   LOCK -
// fuses: 0x7D 0xDF 0x3F -
//------------------------

//------------------------------
//---    for ATMega 328P    ----
//------------------------------
//        LO   HI   EXT   LOCK -
// fuses: 0xFD 0xDF 0xFC  0x3F -
//------------------------------


//--------------------------
//--- our private deines ---
//--------------------------
#ifdef m328p
.equ     RAMSTART   = 0x0100
#else
.equ     RAMSTART   = 0x60
#endif
//-


//---------------------------------------------
//--- controll for change microcontroller's ---
//---------------------------------------------



//----------------------------------------------------------
.equ     DDRB_START_VALUE     = 0b00100011
.equ     PORTB_START_VALUE    = 0b00001101

.equ     ACSR_START_VALUE    = (1<<ACD)  // start value for ACSR

.equ     DIRECTION_TC1_PORT_START_VALUE    = 0b10000100  // initial inout/output value of PORTD
.equ     DIRECT_TC1_PORT = DDRD


.equ     TC1_PORT_START_VALUE   = 0b10101000  // initial inout/output value of PORTD
.equ     TC1_PORT = PORTD

#ifdef m328p
.equ     TIMSK1_START_VALUE  = (1<<TOIE1)  // IRQ TC1 OVERFLOW enable value 
.EQU     TIMSK0_START_VALUE  = (1<<TOIE0)  // IRQ TC0 OVERFLOW enable value 
#else
.EQU     TIMSK_START_VALUE  = (1<<TOIE0) | (1<<TOIE1)
#endif

.equ     TIMER1_START_VALUE  = (1<<CS12) | (1<<CS11) | (1<<CS10) // COUNT EXTERNAL TICKS ON RISING EDGE
.EQU     TIMER0_START_VALUE  = (1<<CS00)  //NO PRESCALE
//---------------------------------------------------------

.EQU    EE_C_OWN = 0x10
.EQU    EE_L_OWN = 0x12
.EQU    EE_C_REF = 0x14
.EQU    EE_F1    = 0x16
.EQU    EE_F2    = 0x1A
.EQU    EE_R1    = 0x1E
.EQU    EE_R2    = 0x20

//---------------------------------------------------------
.def     mod10         =     r16  // helps variable for divide | multiply
.def     cntr_1        =     r17  // helps variable for divide | multiply
.def     divisor       =     r18  // disor for divide proc
//--------

//--------
.EQU    RX_ADD = 0x01
//------
.def	RX0	 =	r1
.def	RX1	 =	r2
.def	RX2	 =	r3
.def	RX3	 =	r4
.def	RX4	 =	r5
.def	RX5	 =	r6
.def	RX6	 =	r7
.def	RX7	 =	r8

.EQU RY_ADD = 0x09
//-------
.def	RY0	 =	r9
.def	RY1	 =	r10
.def	RY2	 =	r11
.def	RY3	 =	r12
.def	RY4	 =	r13
.def	RY5	 =	r14

.def	RM0	 =	r15
.def	RM1	 =	r16
.def	RM2	 =	r17
.def	RM3	 =	r18
.def	RM4	 =	r19
.def	RM5	 =	r20
//--------
        

//-------------------------------------------------
//=================================================
		.DSEG
.ORG     RAMSTART
//--------
VAR_START:			.BYTE 1     // UNUSED
//--------

OUT_ASCII_BUFF:		.BYTE 10    // 10 bytes for ASCII decimal string 32bit unsigned value
LEN_ASCII_BUFF:		.BYTE 1     // length of ASCII digits
MY_LONG_INT:		.BYTE 4     // 4 bytes LONG INTEGER - 32bit unsigned value 
MY_JOB:				.BYTE 1     // 0 = Cx MEASSURE | 1 = Lx MEASSURE | 2 = CE MEASSURE
RING_LONG:			.BYTE 1     // INDICATOR OF BUZZER

FREQ1:				.BYTE 4     // FREQ OF L/C CIRCUIT
FREQ2:				.BYTE 4     // FREQ OF CALIBRATION
FREQ3:				.BYTE 4     // FREQ OF L MEASSURE
C_REF:				.BYTE 2     // REF CAP VALUE
C_OWN:				.BYTE 2     // OWN CAP VALUE
L_OWN:				.BYTE 2     // OWN CAP VALUE
REZ1:				.BYTE 2     // 3000 OHM
REZ2:				.BYTE 2     // 330 OHM
REZ3:				.BYTE 2     // RZ =100 * R1*R2/(R1+R2) = 29399
FREQ1X:				.BYTE 4
SIGN_F:				.BYTE 1
FR3_HI:				.BYTE 1
DSCH_SEC:			.BYTE 2
CALC_PLACE:			.BYTE 4     // BCD DATA TO CALC AND CONVERT
POINT_PLACE:		.BYTE 1
BIG_CAP:			.BYTE 4
//--------
VAR_END:			.BYTE 1     // UNUSED

//--------
//////////////////////////////////////////////////////////////
//				cp	  R16, R20
//				cpc	  R17, R21
//				cpc	  R18, R22
//				CPC   R19, R23
//
//				R16-19 < R20-23 C=1
//				R16-19 = R20-23 C=0 | Z=1
//				R16-19 > R20-23 C=0 | Z=0
//////////////////////////////////////////////////////////////
          .ESEG
.ORG     0x0010
	.db		0xd4, 0x03
	.db		0x27, 0x98
	.db		0xe8, 0x02
	.db		0x8e, 0xb4, 0x07, 0x00
	.db		0xce, 0x6b, 0x05, 0x00
	.DB     0xB3, 0x0B             // R1 = 2995 OHM
	.DB     0x01, 0x46             // R2 = 326  OHM

          .CSEG
.ORG     0x0000

//------------------------------
//-- vectors of interrupts -----
//------------------------------
#ifdef m328p
               jmp     _RESET_PROC // 0x0000 jmp     _RESET_PROC
               jmp     _RESET_PROC // 0x0002 jmp INT0 ; IRQ0
               jmp     _EXT_INT1   // 0x0004 jmp INT1 ; IRQ1
               jmp     _RESET_PROC // 0x0006 jmp PCINT0 ; PCINT0
               jmp     _RESET_PROC // 0x0008 jmp PCINT1 ; PCINT1
               jmp     _RESET_PROC // 0x000A jmp PCINT2 ; PCINT2
               jmp     _RESET_PROC // 0x000C jmp WDT ; Watchdog Timeout
               jmp     _RESET_PROC // 0x000E jmp TIM2_COMPA ; Timer2 CompareA
               jmp     _RESET_PROC // 0x0010 jmp TIM2_COMPB ; Timer2 CompareB
               jmp     _RESET_PROC // 0x0012 jmp TIM2_OVF ; Timer2 Overflow
               jmp     _RESET_PROC // 0x0014 jmp TIM1_CAPT ; Timer1 Capture
               jmp     _RESET_PROC // 0x0016 jmp TIM1_COMPA ; Timer1 CompareA
               jmp     _RESET_PROC // 0x0018 jmp TIM1_COMPB ; Timer1 CompareB
               jmp     TIM1_OVF    // 0x001A jmp TIM1_OVF ; Timer1 Overflow
               jmp     _RESET_PROC // 0x001C jmp TIM0_COMPA ; Timer0 CompareA
               jmp     _RESET_PROC // 0x001E jmp TIM0_COMPB ; Timer0 CompareB
               jmp     TIM0_OVF    // 0x0020 jmp TIM0_OVF ; Timer0 Overflow
#else
				rjmp     _RESET_PROC     // $000 rjmp RESET ; Reset Handler
				rjmp     _RESET_PROC     // $001 rjmp EXT_INT0 ; IRQ0 Handler
				rjmp     _EXT_INT1        // $002 rjmp EXT_INT1 ; IRQ1 Handler
				rjmp     _RESET_PROC     // $003 rjmp TIM2_COMP ; Timer2 Compare Handler
				rjmp     _RESET_PROC     // $004 rjmp TIM2_OVF ; Timer2 Overflow Handler
				rjmp     _RESET_PROC     // $005 rjmp TIM1_CAPT ; Timer1 Capture Handler
				rjmp     _RESET_PROC     // $006 rjmp TIM1_COMPA ; Timer1 CompareA Handler
				rjmp     _RESET_PROC     // $007 rjmp TIM1_COMPB ; Timer1 CompareB Handler
				rjmp    TIM1_OVF     // $008 rjmp TIM1_OVF ; Timer1 Overflow Handler
				rjmp    TIM0_OVF     // $009 rjmp TIM0_OVF ; Timer0 Overflow Handler
#endif
//==============================================================

//=======================================
_RESET_PROC:

				cli      // DISABLE IRQ

				ldi      yh,high(RAMEND)
				out      SPH,yh  // stack init
				ldi      yl,low(RAMEND)
				out      SPL,yl  // stack init
				dec      yh
//====================================================

				rcall    stop_meassure  //

				LDI      R20, DDRB_START_VALUE
				OUT      DDRB, R20
				LDI      R20, PORTB_START_VALUE
				OUT      PORTB, R20

				ldi      ZL, DIRECTION_TC1_PORT_START_VALUE
				out      DIRECT_TC1_PORT,ZL
				ldi      ZL, TC1_PORT_START_VALUE
				out      TC1_PORT,ZL


				RCALL    CLEAR_JOB
				RCALL    RING
				RCALL    WAIT50MS
				RCALL    RING
				rcall    LcdIni

				ldi      zl, low(welcome * 2)
				ldi      zh, high(welcome * 2) 
				rcall    WriteLCDString
				RCALL    _from_line2
				ldi      zl, low(welcome2 * 2)
				ldi      zh, high(welcome2 * 2) 
				rcall    WriteLCDString
				ldi      zl, low(RAMSTART+1)
				ldi      zh, high(RAMSTART+1)

//---
				ldi      r16, LOW(VAR_END - VAR_START)   // clear data ram 32 bytes of variables, string etc...
				clr      xh
				st       Z+, XH
				dec      r16
				brne     PC-2
				RCALL    COPY_VARIABLES

				RCALL    READ_EEPROM_DATA
				RCALL    R3_CALC             // RZ =100 * R1*R2/(R1+R2) = 29399

				RCALL    DO_MEASS
				RCALL    DO_MEASS

				RCALL    READ_EE_C_OWN
				BRNE     COMPENSATE
				RJMP     RUN_CALIBRATE     // !!! EEPROM IS CLEAR NEED CALIBRATION
COMPENSATE:
				rcall    LcdIni
				ldi      zl, low(welcome3 * 2)
				ldi      zh, high(welcome3 * 2) 
				rcall    WriteLCDString
				RCALL    _from_line2
				RCALL    PRINT_L_OWN
				RCALL    PRINT_C_OWN
				RCALL    DO_MEASS
				RCALL    DO_MEASS
//=====================================
//==========   repeat main loop =======
//=====================================
starter:

				LDS      R16, MY_JOB
				CPI      R16,2
				BRNE     PC+2
				RJMP     CE_MEASSURE
				rcall    start_meassure  // OK - let's go rocket :)
//====================================
selfee:
				SBIS     PINB,2     // SW2 PRESSED?
				RJMP     MENU
				SBIS     PINB,3     // SW1 PRESSED?
				RCALL    KEYBOARD_SERVICE
				TST      R11
				BREQ     selfee
				CLI
				RCALL    COPY_REG_TO_FREQ3

				rcall    LcdIni
				RCALL    print_f

				LDS      R16, MY_JOB
				CPI      R16, 1
				BREQ     LX_MEASS

				CBI      PORTD,2
				RCALL    CX_CALC
				rcall    PRINT_PF
				RJMP     starter
				
LX_MEASS:
				SBI      PORTD,2
				RCALL    LX_CALC
				RCALL    CHECK_FREQ
				BREQ     starter
				rcall    PRINT_LX
/////////////////////////////////////////////////////////////////////
				rjmp     starter   // OK repeat over and over again
/////////////////////////////////////////////////////////////////////

//==================================================================
//------------------------
//------ IRQ SERVICE -----
//------------------------
_EXT_INT1:

				PUSH    R12
				IN      R12, SREG
#ifdef m328p
				LDS     R1, TCNT1L
				LDS     R2, TCNT1H
#else
				IN      R1, TCNT1L
				IN      R2, TCNT1H
#endif
				RCALL   stop_meassure
				INC     R10

				OUT     SREG, R12
				POP     R12
				RETI
//------------------------
//------ IRQ SERVICE -----
//------------------------
TIM1_OVF:
				PUSH    R20
				IN      R20, SREG

				INC     R3
				BRNE    TIM1_EX
				INC     R4
TIM1_EX:

				OUT SREG, R20
				POP R20
				reti
//------------------------
//------ IRQ SERVICE -----
//------------------------
TIM0_OVF:
//-----------------------------------------------
//-----      read captured TIMER1 value     -----
//-----      and store her to variable      -----
//-----------------------------------------------

				PUSH    R12
				IN      R12, SREG
#ifdef m328p
				LDS     R1, TCNT1L
				LDS     R2, TCNT1H
#else
				IN      R1, TCNT1L
				IN      R2, TCNT1H
#endif
				INC     R9
				BRNE    T0_OV1
				INC     R10
;---

T0_OV1:			PUSH    R20
				MOV     R20, R10
				CPI     R20,0x7A
				BRNE    IRQ_NO

				MOV     R20, R9
				CPI     R20,0x12
				BRNE    IRQ_NO

				RCALL   stop_meassure
				INC     R11

IRQ_NO:			POP     R20
				OUT     SREG, R12
				POP     R12
				RETI
//-----------------------------------

//----------------------------
//------ IRQ SERVICE END -----
//----------------------------
//=================================================================
//---
PRINT_LX:
				LDI      R24, 'L'
				RCALL    _Lch

				RCALL    SIGN_PRT
								
				RCALL    get_string_pointers
				CPI      R17, 1
				BRNE     TEST_2_DIGITS
				RCALL    print_0_dot
				RCALL    print_0
				RCALL    print_last_digit
				RJMP     UH_PRINT
//---
TEST_2_DIGITS:  CPI      R17, 2
				BRNE     TEST_mH
				RCALL    print_0_dot
				RCALL    print_two_last_digit
				RJMP     UH_PRINT
TEST_mH:
				CPI      R17,6
				BRCS     PRINT_UH
				SUBI     R17, 5
				CLR      R18
				RCALL    print_from_buff
				RCALL    print_dot
				LDI      R17,3
				CLR      R18
				RCALL    print_from_buff
				LDI      R24, 'm'
				RJMP     MH_PRINT
PRINT_UH:
				RCALL    print_meassure_2digits
UH_PRINT:
				LDI      R24, 'u'
MH_PRINT:
				RCALL    _Lch
				LDI      R24, 'H'
				RJMP    _Lch
//---



//---
PRINT_NF:
				LDI      R18,4
				RCALL    PRINT_R18_LONG
				rcall    print_dot
				LDI      R18,1
				RCALL    PRINT_R18_LONG	
BIK:
				LDI      R24, 'n'
				RJMP     FARADS_PRINT
PRINT_R18_LONG:

				ld       r24, Z+
				rcall    _Lch
				dec      r17
				cp       r17,r18
				brne     PC-4
				RET
SIGN_PRT:
				LDI      R24, 'x'
				RCALL    _Lch
				LDI      R24, '='
				RCALL    _Lch
				LDS      R24, SIGN_F
				TST      R24
				BREQ     PC+3
				LDI      R24, '-'
				RCALL    _Lch
				RET
//---
PRINT_PF:
				LDI      R24, 'C'
				RCALL    _Lch


				RCALL    SIGN_PRT

				RCALL    get_string_pointers
				CPI      R17,1
				BRNE     MORE_DIGITS
				RCALL    print_0_dot
				RCALL    print_last_digit
				RJMP     LP01
MORE_DIGITS:
				CPI      R17,6
				BRCC     PRINT_NF
				DEC      R17
				CLR      R18
				RCALL    print_from_buff
				RCALL    print_dot
				RCALL    print_last_digit
LP01:
				LDI      R24, 'p'
FARADS_PRINT:
				PUSH     R24
				RCALL    ONE_SPACE
				POP      R24
				RCALL    _Lch
				LDI      R24, 'F'
				RJMP    _Lch

//---
PRINT_C_REF:
				LDS      R1, C_REF
				LDS      R2, C_REF+1
				RJMP     MAKE_PRT
PRINT_C_OWN:
				RCALL    ONE_SPACE
				LDS      R1, C_OWN
				LDS      R2, C_OWN+1
MAKE_PRT:
				RCALL    CONVERT_16BIT
				RCALL    print_meassure_0digits
				RJMP     LP01
//---
PRINT_L_OWN:
				LDS      R1, L_OWN
				LDS      R2, L_OWN+1
				RCALL    CONVERT_16BIT
				RJMP     PRINT_UH
//---
CONVERT_16BIT:
				rcall    CLEAR_MY_LONG_INT
				
				STS      MY_LONG_INT, R1
				STS      MY_LONG_INT+1, R2

				RJMP    get_string_pointers 
//-----------------------------------------
print_f:
				RCALL    FREQ3_TO_LONG
				LDS      R0, SIGN_F
				TST      R0
				BREQ     PC + 2
				RCALL    FREQ1X_TO_LONG
				rcall    get_string_pointers 

				rcall    _from_line2

				LDI      R24, 'F'
				RCALL    _Lch
				LDI      R24, '='
				RCALL    _Lch
PRINT_F_HZ:
				RCALL    print_meassure_0digits

				RCALL    ONE_SPACE
				LDI      R24, 'H'
				RCALL    _Lch
				LDI      R24, 'z'
				RCALL    _Lch
				RJMP     _from_line1
//---
get_string_pointers:
				rcall    BIN32u_2_ASCII   // first make the properly string

				lds      r17, LEN_ASCII_BUFF
				ldi      zl,low(OUT_ASCII_BUFF+10)
				ldi      zh,high(OUT_ASCII_BUFF+10)
				sub      zl, r17
				clr      r0
				sbc      zh, r0
				ret
//=====================================
//---
print_three_last_digit:
				cpi      r18, 2
				breq     print_two_last_digit

				lds      r24, (OUT_ASCII_BUFF+7)
				rcall    _Lch
print_two_last_digit:
				lds      r24, (OUT_ASCII_BUFF+8)
				rcall    _Lch
print_last_digit:
				lds      r24, (OUT_ASCII_BUFF+9)
				rcall    _Lch
				RET
//---
print_0_dot:
				rcall    print_0
print_dot:     
				ldi      r24, '.'
				rjmp     _Lch
//-
print_0:
				ldi      r24, 0x30
				rjmp     _Lch
//-------------------------


//-----------------------------
//- R18 MANY DIGITS AFTER DOT -
//- R19 LENGTH                -
//-----------------------------
print_meassure_0digits:
				ldi      r18, 0 // MANY DIGITS AFTER DOT
				MOV      R19, R17
				rjmp     print_fixed_point
print_meassure_1digits:
	
				ldi      r18, 1 // MANY DIGITS AFTER DOT
				LDI      r19, 4
				rjmp     print_fixed_point
print_meassure_2digits:
				ldi      r18, 2 // MANY DIGITS AFTER DOT
				ldi      r19, 3
				rjmp     print_fixed_point
print_meassure_3digits:
				ldi      r18, 3 // MANY DIGITS AFTER DOT
				ldi      r19, 4
print_fixed_point:
				cp       r17, r19 //r17=LEN_ASCII_BUFF
				brcc     print_from_buff
				rcall    print_0_dot
				cp       r17, r18
				breq     print_three_last_digit
				rcall    print_0
				cpi      r17, 2
				breq     print_two_last_digit
				rcall    print_0
				rjmp     print_last_digit
//---
print_from_buff:
				ld       r24, Z+
				rcall    _Lch
				dec      r17
				cp       r17,r18
				brne     print_from_buff
				TST      R18
				BRNE     PC+2
				RET
				rcall    print_dot
				rjmp     print_three_last_digit
//-------------------------

//================================================
stop_meassure:
//--------------------------
//--- disable interrupts ---
//---    stop   TIMER1   ---
//--------------------------
				CBI      PORTB,1   //stop ringing
				PUSH     ZL
				clr      zl
#ifdef m328p
				sts      TIMSK1, zl
				sts      TCCR1B, zl
				sts      TCCR1A, zl
				STS      TIMSK0, ZL
				OUT      TCCR0B, zl
				OUT      TCCR0A, zl
				STS      EICRA, ZL
				OUT      EIMSK, ZL
				sts      TCNT1H, ZL
				sts      TCNT1L, ZL
				OUT      TCNT0, ZL
#else
				out      TIMSK, zl
				out      TCCR1B, zl
				out      TCCR1A, zl
				OUT      TCCR0, zl
				OUT      MCUCR, ZL
				OUT      GICR, ZL
				out      TCNT1H, ZL
				out      TCNT1L, ZL
				OUT      TCCR0, ZL
				OUT      TCNT0, ZL
#endif
				ldi      zl, ACSR_START_VALUE
				out      ACSR, zl
				RCALL    CLEAR_IRQ_FLAGS
				RCALL    DISCHARGE
				POP      ZL
				ret
//------------------------------------------------
start_meassure:
//------------------------
//--- block interrupts ---
//------------------------
				cli
				rcall    stop_meassure
				RCALL    FREQ1_TO_FREQ1X
				rcall    CLEAR_MY_LONG_INT
				RCALL    copy_long_to_registers
				CLR      R9
				CLR      R10
				CLR      R11
//---------------------------------
//-----        Timer1 = 0    ------
//---------------------------------
#ifdef m328p
//--------------------------------------------------------
				ldi      ZL, TIMSK1_START_VALUE
				sts      TIMSK1, ZL
				ldi      ZL, TIMSK0_START_VALUE
				sts      TIMSK0, ZL

				LDI      R24, TIMER0_START_VALUE
				ldi      ZL,TIMER1_START_VALUE      
				sts      TCCR1B, ZL                 // START TIMER1
				OUT      TCCR0B, R24
#else
				ldi      ZL, TIMSK_START_VALUE
				out      TIMSK,  ZL

				LDI      R24, TIMER0_START_VALUE
				ldi      ZL,TIMER1_START_VALUE      
				out      TCCR1B, ZL                 // START TIMER1
				OUT      TCCR0, R24
#endif
//-----------------------
//--- clear IRQ flags ---
//-----------------------
				RCALL    CLEAR_IRQ_FLAGS
//---------
//-------------------------
//--- unlock interrupts ---
//-------------------------
				sei  // ok - wait for TC1
				ret
//***********************************************
CLEAR_IRQ_FLAGS:
				
				PUSH     R24
#ifdef m328p
				in       R24,TIFR1
				out      TIFR1, R24
				in       R24,TIFR0
				out      TIFR0, R24
				in       R24,EIFR
				out      EIFR, R24
#else
				in       R24,TIFR
				out      TIFR, R24
				in       R24,GIFR
				out      GIFR, R24
#endif
				POP      R24
				RET
//-----------------------------------------------------------------------------:
// 32bit Unsigned convert BIN to ASCII
//
// Register Variables
//  Call:  
//         var[1:4]           = 32bit unsigned value (0x00000000..0xffffffff)
//         mod10              = <don't care> (high register must be allocated)
//         cntr_1             = <don't care> (high register must be allocated)
//
//
//  Result:
//         OUT_ASCII_BUFF[0:9] = 10 bytes will be overwrite as ASCII decimal bin representation
//         var[1:4]            = (0x00000000)
//         mod10               = length of ASCII numbers
//         cntr_1              = 0
//         xl,xh               = @OUT_ASCII_BUFF first ASCII number
//
//---------------
BIN32u_2_ASCII:
				rcall   copy_long_to_registers

				ldi     xl, low(OUT_ASCII_BUFF+10)
				ldi     xh, high(OUT_ASCII_BUFF+10)

				ldi     divisor, 10   //divide by 10

BIN_TO_ASCII:
				rcall   div32u_by_8u  //make divide by 10
//---------------------------
				ori     mod10, 0x30   // O_o HERE we gotta our the rest of div10 :] o_O
				st      -x,mod10      // store as ASCII NUMBER
				cpi     xl, low(OUT_ASCII_BUFF)
				brne    BIN_TO_ASCII

				ldi     mod10,9

clr_lead:      
				ld      cntr_1, X
				cpi     cntr_1, 0x30
				brne    stop_it
				ldi     cntr_1, 0x20
				st      x+, cntr_1
				dec     mod10
				brne    clr_lead
stop_it:
				inc     mod10
				sts     LEN_ASCII_BUFF, mod10  // save how many bytes ASCII NUMBERS strig is here
				ret
//---
//------------------------------------------------------------------
// ---------- like above - prepare our fixed point value  ----------
//------------------------------------------------------------------

//-------------
//------------------------------------------------------------
//--- divide unsigned 32bit value by 8bit unsigned divisor ---
//------------------------------------------------------------
//--- input :                                              ---
//--- RX 0, 1,2,3 = 32 bit unsigned value                  ---
//--- divisor: our 8bit unsigned value of divide           ---
//------------------------------------------------------------
//--- output:                                              ---
//--- mod10 is touched, rest of divide                     ---
//--- var 1,2,3,4 = RESULT                                 ---
//--- cntr_1 is touched = 0                                ---
//------------------------------------------------------------
div32u_by_8u:
				clr     mod10          // initialize mod10 = 0

				ldi     cntr_1, 32     // lc = 32 (times bits to shift)
divloop:                              // simple 32bit_u divide by const 8bit_u
				lsl     RX0         
				rol     RX1
				rol     RX2
				rol     RX3
				rol     mod10
				cp      mod10, divisor     // divisor value
				brcs    divlp2
				inc     RX0
				sub     mod10, divisor     // divisor value
divlp2:
				dec     cntr_1
				brne    divloop
				ret
//---
//-----------------------------------------------------------------------
//--- copy from STATIC RAM our 32bit value into registers var 1,2,3,4 ---
//-----------------------------------------------------------------------
//--- input : don't care                                              ---
//--- output :                                                        ---
//--- var 1,2,3,4 = 32bit value                                       ---
//--- XL, XH, ZL, ZH, cntr_1 is touched                               ---
//--- cntr_1 = 0                                                      ---
//--- XL,XH  = @(var4 + 1)                                            ---
//--- ZL,ZH  = @(MY_LONG_INT + 4)                                     ---
//-----------------------------------------------------------------------
copy_long_to_registers:
				rcall   set_addr_var
COPY_1:
				ld      R0, Z+
				st      x+, R0
				dec     cntr_1
				brne    COPY_1
				ret
//---
set_addr_var:
				ldi     xl, 1
				clr     xh

set_my_long_addr:

				ldi     zl, low(MY_LONG_INT)
				ldi     zh, high(MY_LONG_INT)
				ldi     cntr_1,4
				ret
//-------------
COPY_BIGC_TO_LONG:
				ldi     XL, low(BIG_CAP)
				ldi     XH, high(BIG_CAP)
				RCALL   set_my_long_addr
				RJMP    COPY_2
//---
COPY_REG_TO_BIGC:
				LDI     XL,1
				CLR     XH
				ldi     zl, low(BIG_CAP)
				ldi     zh, high(BIG_CAP)
				ldi     cntr_1,4
				RJMP    COPY_2
//---
copy_registers_to_long:
				rcall   set_addr_var
COPY_2:
				ld      R0, X+
				st      Z+, R0
				dec     cntr_1
				brne    COPY_2
				ret
//-------------
CLEAR_MY_LONG_INT:
				rcall   set_my_long_addr
				clr     r0
COPY_4:
				st      Z+, r0
				dec     cntr_1
				brne    COPY_4
				ret
//-------------
COPY_REG_TO_FREQ2:
				ldi     zl, low(FREQ2)
				ldi     zh, high(FREQ2)
REG_TO_FREQ:
				ldi     xl, 1
				clr     xh
				ldi     cntr_1,4
				RJMP    COPY_2
//-----------------
COPY_REG_TO_FREQ1:
				ldi     zl, low(FREQ1)
				ldi     zh, high(FREQ1)
				RCALL   REG_TO_FREQ
				
COPY_REG_TO_FREQ3:
				LDS     ZL, FREQ3+2
				TST     ZL
				BRNE    STORE_FR3_HI
				LDS     ZL, FREQ3+1
STORE_FR3_HI:   STS     FR3_HI, ZL  // IF LX MEASSURE CHECK IS CONNECTED INDUCTANCE

				ldi     zl, low(FREQ3)
				ldi     zh, high(FREQ3)
				RCALL   REG_TO_FREQ
				RCALL   FREQ1X_TO_RY

				cp	  RY0, RX0
				cpc	  RY1, RX1
				cpc	  RY2, RX2
				CPC   RY3, RX3

////////////////////////////////////////////////////
//
//				RY = FREQ1 | RX = FREQ 3 
//				IF RY0-3 < RX0-3 C=1
//				IF RY0-3 = RX0-3 C=0 | Z=1
//				IF RY0-3 > RX0-3 C=0 | Z=0
//
/////////////////////////////////////////////////////

				CLR    R0
				CLR    RX7
				ADC    R0, RX7
				STS    SIGN_F, R0
				BRNE   PC+2
				RET
				RCALL  FREQ3_TO_FREQ1X
				RCALL  FREQ1_TO_FREQ3
//--
FREQ1X_TO_LONG:
				ldi     XL, low(FREQ1X)
				ldi     XH, high(FREQ1X)
				RJMP    STORE_TO_LONG
FREQ3_TO_LONG:
				ldi     XL, low(FREQ3)
				ldi     XH, high(FREQ3)
STORE_TO_LONG:
				RCALL   set_my_long_addr
				RJMP    COPY_2
//-----------------------------
//-------------------------
//---  init LCD display ---
//-------------------------




//---------------------
// Lcd = PORTC.3,2,1,0
// RS=PORTC.5
// EN=PORTC.4
// LCD_TYPE = 16 * 2
//---------------------


.EQU	SYS_FREQ	= 8		; czêstotliwoœæ pracy w MHz


.EQU K_LCD		= DDRC	; rejestr kierunku portu wyœwietlacza
.EQU O_LCD		= PORTC	; rejestr wyjœciowy portu wyœwietlacza
.EQU LCD_RS		= 5		; nr linii dla sygna³u RS
.EQU LCD_EN		= 4		; nr linii dla sygna³u EN
.EQU LCD_DB4	= 3		; nr linii dla sygna³u DB4
.EQU LCD_DB5	= 2		; nr linii dla sygna³u DB5
.EQU LCD_DB6	= 1		; nr linii dla sygna³u DB6
.EQU LCD_DB7	= 0		; nr linii dla sygna³u DB7



//--------

//---------------------------------------------------------

//---------------------------------------------------------

        

//---------------------------------------------------------
; PODPROGRAMY OBS£UGI WYŒWIETLACZA LCD W TRYBIE 4-BITOWYM
; Rafa³ Baranowski (C) 2004
;
; Etykiety procedur:
; 	Ini_LCD		- inicjalizacja wyœwietlacza
;	Instrukcja_LCD	- przesy³anie instrukcji do wyœwietlacza
;	Zapisz_LCD		- przesy³anie danych do wyœwietlacza
;	Zapisz_CGRAM	- kopiowanie tablicy do pamiêci CGRAM
;	Czysc_LCD		- czyszczenie wyœwietlacza
;	Wroc_LCD		- powrót kursora do pozycji pocz¹tkowej
;	Spacja_LCD		- generacja odstêpu
;	Pokaz_bajt_LCD	- wyœwietlanie wartoœci bajtu
;	Pokaz_ciag_LCD	- wyœwietlanie sta³ego ³añcucha znaków
;
; Wymagane sta³e:
;	K_LCD		- rejestr kierunku portu wyswietlacza
;	O_LCD		- rejestr wyjœciowy portu wyswietlacza
;	LCD_LCD_RS	- nr linii dla sygna³u LCD_RS
;	LCD_LCD_EN	- nr linii dla sygna³u LCD_EN
;	LCD_LCD_DB4	- nr linii dla sygna³u LCD_DB4
;	LCD_LCD_DB5	- nr linii dla sygna³u LCD_DB5
;	LCD_LCD_DB6	- nr linii dla sygna³u LCD_DB6
;	LCD_LCD_DB7	- nr linii dla sygna³u LCD_DB7
;
; Wymagane podprogramy dodatkowe:
;	Czekaj_us	- z pliku czekaj_us.inc
;	Czekaj_ms	- z pliku czekaj_ms.inc


;****************************************************************
;		PROCEDURA INICJALIZUJ¥CA WYŒWIETLACZ LCD
;
LCDIni:
				push	R16
				push	R17

				sbi	K_LCD, LCD_RS	; porty wyœwietlacza pracuj¹ jako wyjœcia
				sbi	K_LCD, LCD_EN
				sbi	K_LCD, LCD_DB4
				sbi	K_LCD, LCD_DB5
				sbi	K_LCD, LCD_DB6
				sbi	K_LCD, LCD_DB7
	
				cbi	O_LCD, LCD_RS	; przesy³ane wartoœci bêd¹ instrukcjami
				cbi	O_LCD, LCD_EN

				ldi	R16, 45
				ldi	R17, 1
				rcall	Czekaj_ms		; oczekiwanie przez 45 ms na wypadek, gdyby
								;   zasilanie dopiero zosta³o w³¹czone

				sbi	O_LCD, LCD_EN
				sbi	O_LCD, LCD_DB4
				sbi	O_LCD, LCD_DB5
				cbi	O_LCD, LCD_DB6
				cbi	O_LCD, LCD_DB7
				nop
				nop
				cbi	O_LCD, LCD_EN	; ustawienie DB7..4=0011 i potwierdzenie
								;   zboczem opadaj¹cym na linii EN
				ldi	R16, 5
				ldi	R17, 1
				rcall	Czekaj_ms		; oczekiwanie przez 5 ms

				ldi	R17, 2
Ini_LCD_1:
				sbi	O_LCD, LCD_EN
				ldi	R16, 1
				rcall	Czekaj_us
				cbi	O_LCD, LCD_EN	
				ldi	R16, 20
				rcall	Czekaj_us
				dec	R17
				brne	Ini_LCD_1		; dwukrotne powtórzenie transmisji tej samej
								;   wartoœci (DB7..4=0011) z opóŸnieniem 200 us
								;   (programowa inicjalizacja wyœwietlacza)

				sbi	O_LCD, LCD_EN
				cbi	O_LCD, LCD_DB4
				ldi	R16, 1
				rcall	Czekaj_us
				cbi	O_LCD, LCD_EN	
				ldi	R16, 10
				rcall	Czekaj_us		; transmisja wartoœci DB7..4=0010
								;   (przejœcie w tryb interfejsu 4-bitowego)

				ldi	R16, (1<<LCD_USTAW)|(1<<LCD_U_4BIT)|(1<<LCD_U_2LIN)|(1<<LCD_U_5x8)
				rcall	Instrukcja_LCD	; interfejs 4-bitowy, 2 linie, 5x8 pkt

				ldi	R16, (1<<LCD_TRYB)|(1<<LCD_T_INC)
				rcall	Instrukcja_LCD	; tryb zwiêkszania adresu przy zapisie

				ldi	R16, (1<<LCD_STER)|(1<<LCD_SW_ON)|(1<<LCD_SK_OFF)|(1<<LCD_SM_OFF)
				rcall	Instrukcja_LCD	; wyœwietlanie w³¹czone, kursor aktywny i miga


				sbi	O_LCD, LCD_RS	; przesy³ane wartoœci bêd¹ danymi

				rcall	Czysc_LCD

				pop	R17
				pop	R16
				ret


;****************************************************************
;	PROCEDURA PRZESY£AJ¥CA INSTRUKCJÊ DO WYŒWIETLACZA
;
; Parametry:
;	R16	- kod instrukcji
;
; Przyk³ad:
;				; w³¹czanie kursora
;	ldi	R16, (1<<LCD_STER)|(1<<LCD_SW_ON)|(1<<LCD_SK_ON)|(1<<LCD_SM_OFF)
;	rcall	Instrukcja_LCD
;

				; etykiety symboliczne bitów tworz¹cych
				;   kody instrukcji
.EQU	LCD_CZYSC	= 0	; czyszczenie wyœwietlacza i powrót kursora
.EQU	LCD_WROC	= 1	; powrót kursora do pozycji pocz¹tkowej

.EQU	LCD_TRYB	= 2	; zmiana trybu zapisu pamiêci
.EQU	LCD_T_INC	= 1	;   adres inkrementowany przy ka¿dym zapisie
.EQU	LCD_T_DEC	= 2	;   adres dekrementowany przy ka¿dym zapisie
.EQU	LCD_T_PRZES	= 0	;   okno przesuwane przy ka¿dym zapisie

.EQU	LCD_STER	= 3	; sterowanie wyœwietlaczem
.EQU	LCD_SW_ON	= 2	;   wyœwietlanie w³¹czone
.EQU	LCD_SW_OFF	= 3	;   wyœwietlanie wy³¹czone
.EQU	LCD_SK_ON	= 1	;   kursor w³¹czony
.EQU	LCD_SK_OFF	= 3	;   kursor wy³¹czony
.EQU	LCD_SM_ON	= 0	;   miganie kursora w³¹czone
.EQU	LCD_SM_OFF	= 3	;   miganie kursora wy³¹czone

.EQU	LCD_PRZES	= 4	; przesuwanie
.EQU	LCD_P_KUR	= 4	;   przesuwanie kursora
.EQU	LCD_P_WYS	= 3	;   przesuwanie okna
.EQU	LCD_LEWO	= 4	;   przesuwanie w lewo
.EQU	LCD_PRAWO	= 2	;   przesuwanie w prawo

.EQU	LCD_USTAW	= 5	; ustawienia interfejsu i wyœwietlacza
.EQU	LCD_U_8BIT	= 4	;   interfejs 8-bitowy
.EQU	LCD_U_4BIT	= 5	;   interfejs 4-bitowy
.EQU	LCD_U_2LIN	= 3	;   wyœwietlacz 2-liniowy
.EQU	LCD_U_1LIN	= 5	;   wyœwietlacz 1-liniowy
.EQU	LCD_U_5x10	= 2	;   znaki o rozmiarach 5x10 punktów
.EQU	LCD_U_5x8	= 5	;   znaki o rozmiarach 5x8 punktów

.EQU	LCD_CGADR	= 6	; zmiana adresu CGRAM
.EQU	LCD_DDADR	= 7	; zmiana adresu DDRAM

Instrukcja_LCD:
				cbi	O_LCD, LCD_RS	; przesy³ane wartoœci bêd¹ instrukcjami
				rcall	Zapisz_LCD		; przeœlij instrukcjê zawart¹ w R16
IN_END:
				sbi	O_LCD, LCD_RS	; przesy³ane wartoœci bêd¹ danymi

				ret
//---
_from_line2:
				LDI     r24, 0xC0
				RJMP    MAKE_SET
_from_line1:
				LDI     r24, 0x80
//---
MAKE_SET:
				cbi     O_LCD, LCD_RS
				rcall   _Lch
				rjmp    IN_END
;***************************************************************
;			PROCEDURA PRZESYLAJ¥CA BAJT
;
; Parametry:
;	R16	- dane do wys³ania (np. kod wyœwietlanego znaku)
_Lch:
				push R16
				mov  r16,r24
				rcall Zapisz_LCD
				pop  r16
				ret
;
Zapisz_LCD:
				sbi	O_LCD, LCD_EN	; linia EN w stanie wysokim
				cbi	O_LCD, LCD_DB4
				cbi	O_LCD, LCD_DB5
				cbi	O_LCD, LCD_DB6
				cbi	O_LCD, LCD_DB7	; linie DB7..4 w stanie niskim
				sbrc	R16, 7
				sbi	O_LCD, LCD_DB7 
				sbrc	R16, 6
				sbi	O_LCD, LCD_DB6 
				sbrc	R16, 5
				sbi	O_LCD, LCD_DB5
				sbrc	R16, 4
				sbi	O_LCD, LCD_DB4	; ustawianie tylko tych linii DB7..4,
				nop				;   którym odpowiadaj¹ wysokie wartoœci
							;   bitów 7..4 z rejestru R16
				cbi	O_LCD, LCD_EN	; linia EN w stanie niskim - potwierdzenie
							;   (przes³anie starszego pó³bajtu)

				nop
				nop				; opóŸnienie o dwa takty zegarowe

				sbi	O_LCD, LCD_EN
				cbi	O_LCD, LCD_DB4
				cbi	O_LCD, LCD_DB5
				cbi	O_LCD, LCD_DB6
				cbi	O_LCD, LCD_DB7
				sbrc	R16, 3
				sbi	O_LCD, LCD_DB7 
				sbrc	R16, 2
				sbi	O_LCD, LCD_DB6
				sbrc	R16, 1
				sbi	O_LCD, LCD_DB5
				sbrc	R16, 0
				sbi	O_LCD, LCD_DB4	; ustawianie tylko tych linii DB7..4,
				nop				;   którym odpowiadaj¹ wysokie wartoœci
							;   bitów 3..0 z rejestru R16
				cbi	O_LCD, LCD_EN	; linia EN w stanie niskim - potwierdzenie
							;   (przes³anie m³odszego pó³bajtu)

				ldi	R16, 5
				rcall	Czekaj_us		; opóŸnienie o 50 us

				ret
;***************************************************************
;		PROCEDURA CZYSZCZ¥CA WYŒWIETLACZ LCD
;
Czysc_LCD:
				push	R16
				ldi	R16, (1<<LCD_CZYSC)
WYKONAJ_INSTR:
				rcall	Instrukcja_LCD	; czyszczenie wyœwietlacza i
								;   powrót kursora do pozycji
								;   pocz¹tkowej
				ldi     R16, 180
				rcall   Czekaj_us		; opóŸnienie o 1,8 ms

				pop	R16
				ret


;***************************************************************
;			PROCEDURA POWROTU KURSORA
;
Wroc_LCD:
				push	R16

				ldi	R16, (1<<LCD_WROC)
				RJMP WYKONAJ_INSTR


;***************************************************************
;			PROCEDURA WPROWADZAJ¥CA ODSTÊP
;
; Parametry:
;	R16	- liczba spacji do wyœwietlenia
;
ONE_SPACE:
				LDI R16, 1
Spacja_LCD:
				push	R17

				mov	R17, R16	; kopiuj liczbê wymaganych spacji do R17
								;   (inicjalizacja licznika pêtli)
Spacja_LCD_1:
				ldi	R16, ' '	; R16 zawiera kod ASCII pustego pola (spacji)
				rcall	Zapisz_LCD	; wyœwietlenie pustego pola (znaku z R16)
				dec	R17		; zmniejszenie licznika pêtli
				brne	Spacja_LCD_1; iteracyjne zapêtlenie

				pop	R17
				ret



;***************************************************************
;	PROCEDURA WYŒWIETLAJ¥CA STA£Y £AÑCUCH ZNAKÓW
;
; Parametry:
;	Z	- adres pierwszego znaku ³añcucha w pamiêci programu

.EQU ZNAK_KONCA	= 255 ; etykieta symboliczna znaku koñca ³añcucha

WriteLCDString:
				push 	R0
				push	R16
				push	R17

Pok_c_LCD_1:		; pêtla wyœwietlaj¹ca kolejne znaki ³añcucha
				lpm	
				adiw	zl,1
				mov		R16,R0
					; odczytaj znak z pamiêci programu, spod adresu
							;   wskazywanego przez rejestr indeksowy Z
				tst	R16
				breq	Pok_c_LCD_2	; jeœli znak jest znakiem koñca, to zakoñcz,
				rcall	Zapisz_LCD	;   w przeciwnym razie wyœwietl go na LCD
				rjmp	Pok_c_LCD_1	;   i przejdŸ do odczytywania kolejnego znaku

Pok_c_LCD_2:
				pop	R17
				pop	R16
				pop	R0
				ret
//=============================================
;************************************************************
;		MIKROSEKUNDOWA PROCEDURA OPÓNIAJ¥CA
;
; 						Rafa³ Baranowski (C) 2004
;
; Parametry:
;	R16 - wartoœæ opóŸnienia w dziesi¹tkach us (dla zera - 2560 us)
;
; Wymagane sta³e:
;	SYS_FREQ - czêstotliwoœæ pracy w MHz
;
Czekaj_us:
				push	R17
				push	R18

				mov	R18, R16
				ldi	R17, SYS_FREQ

Czekaj_us_0:
				mov	R16, R18		;______
									;      \
Czekaj_us_1:						;       |
				nop					;__     |
				nop					;  \    |
				nop					;   |   |
				nop					;   |   |
				nop					;   p   p
				nop					;   1   0
				nop					;   |   |
				dec	R16				;   |   |
		brne	Czekaj_us_1			;__/    |
									;       |
				dec	R17				;       |
		brne	Czekaj_us_0			;______/

				pop	R18
				pop	R17
				ret
//-------------------
;************************************************************
;		MILISEKUNDOWA PROCEDURA OPÓNIAJ¥CA
;
; 						Rafa³ Baranowski (C) 2004
;
; Parametry:
;	R16 - wartoœæ opóŸnienia w ms (dla zera - 256 ms)
;	R17 - mno¿nik opóŸnienia (dla zera - 256x)
;
; Wymagane sta³e:
;	SYS_FREQ - czêstotliwoœæ pracy w MHz
;
Czekaj_ms:
				push	R18
				push	R19
				push	R20
				push	R21

				mov	R20, R16
				mov	R21, R17
				ldi	R18, SYS_FREQ

Czekaj_ms_0:
				mov	R17, R21		;______________
								;              \
Czekaj_ms_1:			;              				 |
				mov	R16, R20		;__________     |
								;          \    |
Czekaj_ms_2:					;           |   |
				ldi	R19, 249		;______     |   |
								;      \    |   |
				nop				;       |   |   |
Czekaj_ms_3:					;__     |   |   |
				nop				;  \    p   p   p
				dec	R19			;  p3   2   1   0
				brne	Czekaj_ms_3		;__/    |   |   |
								;       |   |   |
				dec	R16			;       |   |   |
				brne	Czekaj_ms_2		;______/    |   |
								;           |   |
				dec	R17			;           |   |
				brne	Czekaj_ms_1		;__________/    |
								;               |
				dec	R18			;               |
				brne	Czekaj_ms_0		;______________/

				pop	R21
				pop	R20
				pop	R19
				pop	R18
				ret

//==============================================================================
//------------------------------------------------------
RUN_CALIBRATE:
				RCALL    stop_meassure
				rcall    LcdIni
				RCALL    UNPRESS_WAIT
//===
//========= MEASS F1
//===
				ldi      zl, low(CALIB_START * 2)
				ldi      zh, high(CALIB_START * 2) 
				rcall    WriteLCDString 

				RCALL    SW2_INFOPRINT
				RCALL    SW2_WAIT
				rcall    LcdIni
				ldi      zl, low(F1_MEAS * 2)
				ldi      zh, high(F1_MEAS * 2) 
				rcall    WriteLCDString 
				RCALL    UNPRESS_WAIT
				RCALL    CLEAR_JOB
				RCALL    DO_MEASS
				RCALL    DO_MEASS
				RCALL    COPY_REG_TO_FREQ1
				RCALL    FREQ1_TO_FREQ1X

				rcall    LcdIni
				ldi      zl, low(F1_EQU * 2)
				ldi      zh, high(F1_EQU * 2) 
				rcall    WriteLCDString 

				RCALL    FREQ1X_TO_LONG
				rcall    get_string_pointers 
				RCALL    PRINT_F_HZ

				RCALL    SW2_INFOPRINT
				RCALL    SW2_WAIT
				RCALL    UNPRESS_WAIT
//===
//============ MEASS F2
//===
				rcall    LcdIni
				ldi      zl, low(AD_CREF * 2)
				ldi      zh, high(AD_CREF * 2) 
				rcall    WriteLCDString 
				RCALL    PRINT_C_REF

				RCALL    SW2_INFOPRINT
				RCALL    SW2_WAIT
				rcall    LcdIni
				ldi      zl, low(F2_MEAS * 2)
				ldi      zh, high(F2_MEAS * 2) 
				rcall    WriteLCDString 
				RCALL    UNPRESS_WAIT
				RCALL    DO_MEASS
				RCALL    DO_MEASS
				RCALL    COPY_REG_TO_FREQ2

				rcall    LcdIni
				ldi      zl, low(F2_EQU * 2)
				ldi      zh, high(F2_EQU * 2) 
				rcall    WriteLCDString 

				RCALL    FREQ2_TO_RY
				RCALL    TRANSFER_RY32_TO_RX
				RCALL    copy_registers_to_long
				rcall    get_string_pointers 
				RCALL    PRINT_F_HZ

				RCALL    SW2_INFOPRINT
				RCALL    SW2_WAIT
				RCALL    UNPRESS_WAIT

/////////////////////////////////////////////////
				RCALL    CAPP_CALIBRATE
				RCALL    COIL_CALIBRATE

				RCALL    WRITE_EE_C_REF
				RCALL    WRITE_EE_F1
				RCALL    WRITE_EE_F2
				RCALL    WRITE_EE_C_OWN
				RCALL    WRITE_EE_L_OWN

				RCALL    LcdIni

				RCALL    PRINT_L_OWN
				RCALL    PRINT_C_OWN


				RCALL    _from_line2
				ldi      zl, low(CAL_OK * 2)
				ldi      zh, high(CAL_OK * 2) 
				rcall    WriteLCDString



				RCALL    SW2_WAIT
				RCALL    UNPRESS_WAIT
				RCALL    DO_MEASS
				RJMP     _RESET_PROC
//-----------------
UNPRESS_WAIT:
				IN      R16, PINB
				ANDI    R16, 0x04
				BREQ    PC-2
				RCALL   RING
				RET
SW2_WAIT:
				IN      R16, PINB
				ANDI    R16, 0x04
				BRNE    PC-2
				RJMP    LcdIni
DO_MEASS:
				rcall   start_meassure 
				TST     R11
				BREQ    PC-1
				RET
//-----------------
SW2_INFOPRINT:
				RCALL   _from_line2
				ldi     zl, low(SW2_PRESS * 2)
				ldi     zh, high(SW2_PRESS * 2) 
				RJMP    WriteLCDString  // print welcome info from 1st line
//------------------------------------------------------

///////////////////////////////////////////////////////
//                                                   //
//   CAPPACITOR CALCULATE AFTER ADD C_REF 1000 pF    //
//                                                   //
//         C = C_REF * F2^2 / (F1^2 - F2^2)          //
//                                                   //
///////////////////////////////////////////////////////
CAPP_CALIBRATE:
				RCALL FREQ1_TO_FREQ1X
				RCALL FREQ1X_TO_RY     // CALCULATE C IN CIRCUIT BY ADD C_REF 
				RCALL SQUARE_CALC      // RX = F1^2
				RCALL SEND_RX_TO_RM    // STORE F1^2 TO REMAINDER RM = RX => RM = F1^2
				RCALL FREQ2_TO_RY      // RY = F2
				RCALL SQUARE_CALC      // RX = F2^2
				RCALL SUBSTRACT_RM_RX  // RM = RM - RX => RM = F1^2 - F2^2 

				RCALL TRANSFER_RY32_TO_RX // RX = RY = F2
				RCALL CLEAR_RY
				LDS   RY0, C_REF       // 0x3E8 = 1000 pF - CALIBRATION CAPPACITOR VALUE
				LDS   RY1, C_REF+1     // RY = C_REF VALUE | RX = FREQ2
				RCALL MULU32           // RX = FREQ2 * 1000 = (F2*C_REF)
				RCALL FREQ2_TO_RY      // RY = FREQ2
				RCALL MULU32           // RX = FREQ2 * 1000 * FREQ2 = (F2*C_REF*F2) = F2^2 * C_REF
				RCALL SEND_RM_TO_RY    // RM ALL THE TIME = F1^2 - F2^2
				RCALL DIV64            // RX = F2^2 * C_RE / (F1^2 - F2^2) => RX = VALUE OF pF CAPP IN THE CIRCUIT
				STS   C_OWN, RX0
				STS   C_OWN+1, RX1
				RET

///////////////////////////
// END OF CAPP CALIBRATE //
///////////////////////////



///////////////////////////////////////////////////////
//                                                   //
//      CALCULATE INDUCTANCE OF CIRCUIT COIL         //
//                                                   //
//              L = 1/ 4*Pi^2*C*F1^2                 //
//                                                   //
//              L = 1/4/Pi/Pi/C/F1/F1                //
//                                                   // 
///////////////////////////////////////////////////////
//              THE TRICK OF MAGIC_NUMBER            //
//                                                   //
//        MAGIC_NUMBER = 2533029591058444286 =       //
//                                                   //
//      100 000 000 000 000 000 000/4/Pi/Pi          //
//                                                   //
//              = 0x232720FE14976FFE                 //
//                                                   //
///////////////////////////////////////////////////////
COIL_CALIBRATE:
				RCALL FREQ1_TO_FREQ1X
				RCALL SET_RX_MAGIC_NUMBER  // RX = 0x232720FE14976FFE 
				RCALL FREQ1X_TO_RY         // RY = F1
				RCALL DIV64                // RX = RX/F1
				RCALL DIV64                // RX = RX/F1/F1
				RCALL CLEAR_RY             // RY = 0

				LDS   RY0, C_OWN           // 0x3D2 = 978 pF
				LDS   RY1, C_OWN+1
				RCALL DIV64                // RX = RX/F1/F1/C
				STS   L_OWN, RX0
				STS   L_OWN+1, RX1
				RET
///////////////////////////
// END OF COIL CALIBRATE //
///////////////////////////

///////////////////////////////
// COIL CALCULATE INDUCTANCE //
///////////////////////////////
LX_CALC:
//---------------------------
				RCALL FIRST_CALC

				LDS   RY0, L_OWN      // 0x279B = 101,13 uH - CALIBRATION CAPPACITOR VALUE
				LDS   RY1, L_OWN+1    // RY = 10113 RX = (F1^2 - F3^2)/F3
SECOND_CALC:
				RCALL MULU32          // RX = [(F1^2 - F3^2)/F3] * 10113 |  [L=101,13 uH]
				RCALL FREQ3_TO_RY     // RY = FREQ3
				RCALL DIV64           // RX = [(F1^2 - F3^2)/F3] * 10113 / F3 => RX = VALUE OF uH ADDED TO THE CIRCUIT
				RJMP  copy_registers_to_long
FIRST_CALC:
				RCALL FREQ1X_TO_RY      // CALCULATE C IN CIRCUIT BY ADD C_REF 
				RCALL SQUARE_CALC      // RX = F1^2
				RCALL SEND_RX_TO_RM    // STORE F1^2 TO REMAINDER RM = RX => RM = F1^2
				RCALL FREQ3_TO_RY      // RY = FREQ3 |[L]
				RCALL SQUARE_CALC      // RX = F2^2
				RCALL SUBSTRACT_RM_RX  // RM = RM - RX => RM = F1^2 - F3^2 |[L}
				RCALL TRANSFER_RM_TO_RX // RX = RM =  F1^2 - F3^2 |[L]
				RCALL DIV64            // RX = (F1^2 - F3^2)/F3
				RJMP  CLEAR_RY
//====================================================
CX_CALC:
				RCALL FIRST_CALC             // RX = (F1^2 - F3^2)/F3

                LDI   RM5, 10
				MOV   RY0, RM5
				RCALL MULU32 

				LDS   RY0, C_OWN      // RY = 978 pF - CALIBRATION CAPPACITOR VALUE
				LDS   RY1, C_OWN+1
				RCALL MULU32 
				RCALL FREQ3_TO_RY
				RCALL DIV64 
				RJMP  copy_registers_to_long

///////////////////////////////////////////
//             SQUARE 32BIT              //
///////////////////////////////////////////
SQUARE_CALC:
				RCALL TRANSFER_RY32_TO_RX
//////////////////////////////////////////////////////////
//           MULT32
//////////////////////////////////////////////////////////
MULU32:
				RCALL CLEAR_HIGH_RX
				PUSH  R25
				LDI   R25,33
				MOV   R0, R25
				POP   R25
				SUB   RX4,RX4
				brcc  PC+5
				add   RX4,RY0
				adc   RX5,RY1
				adc   RX6,RY2
				adc   RX7,RY3
				ror   RX7
				ror   RX6
				ror   RX5
				ror   RX4
				ror   RX3
				ror   RX2
				ror   RX1
				ror   RX0
				dec   R0
				brne  PC-14
				ret
///////////////////////////////////////////////////////////////////////////
//				Division 64 / 48 bit
//-------------------------------------------------------------------------
DIV64:	 		

				clr	  RM0
				clr	  RM1
				clr	  RM2
				CLR   RM3
				ldi	  RM4, 64
				MOV   R0, RM4 //COUNTER
				CLR   RM4
				CLR   RM5
//PC-30
				lsl	  RX0
				rol	  RX1
				rol   RX2
				rol	  RX3
				rol	  RX4
				rol	  RX5
				rol	  RX6
				ROL   RX7

				rol	  RM0
				rol	  RM1
				rol	  RM2
				ROL   RM3
				ROL   RM4
				ROL   RM5
				brcs	PC+8

				cp	  RM0, RY0
				cpc	  RM1, RY1
				cpc	  RM2, RY2
				CPC   RM3, RY3
				CPC   RM4, RY4
				CPC   RM5, RY5
				brcs	PC+8
//PC+8
				sub	  RM0, RY0
				sbc	  RM1, RY1
				sbc	  RM2, RY2
				SBC   RM3, RY3
				SBC   RM4, RY4
				SBC   RM5, RY5
				inc	  RX0
//PC+8
				dec	  R0
				brne   PC-30
				ret
;------------------------------------------------------------------------
CLEAR_HIGH_RX:
				CLR RX7
				CLR RX6
				CLR RX5
				CLR RX4
				RET
//-----------------
CLEAR_RY:
				CLR RY0
				CLR RY1
				CLR RY2
				CLR RY3
				CLR RY4
				CLR RY5
				RET
//-----------------
SEND_RX_TO_RM:

				MOV RM0, RX0
				MOV RM1, RX1
				MOV RM2, RX2
				MOV RM3, RX3
				MOV RM4, RX4
				MOV RM5, RX5
				RET
//-----------------
SEND_RM_TO_RY:

				MOV RY0, RM0
				MOV RY1, RM1
				MOV RY2, RM2
				MOV RY3, RM3
				MOV RY4, RM4
				MOV RY5, RM5
				RET
//-----------------
TRANSFER_RM_TO_RX:
				RCALL CLEAR_HIGH_RX
				MOV RX0, RM0
				MOV RX1, RM1
				MOV RX2, RM2
				MOV RX3, RM3
				MOV RX4, RM4
				MOV RX5, RM5
				RET
//-----------------
SUBSTRACT_RM_RX:
				sub	  RM0, RX0
				sbc	  RM1, RX1
				sbc	  RM2, RX2
				SBC   RM3, RX3
				SBC   RM4, RX4
				SBC   RM5, RX5
				RET
//-----------------
TRANSFER_RY32_TO_RX:
				MOV   RX0, RY0
				MOV   RX1, RY1
				MOV   RX2, RY2
				MOV   RX3, RY3
				RET
//---------------------
SET_RY_8M:
				ldi     ZL, low(STATIC8M * 2)
				ldi     ZH, high(STATIC8M * 2) 
				PUSH    R16
				LDI     R16,6
				ldi     XL, LOW(RY_ADD)
				CLR     XH
				RJMP    SAVE_TO_REGS
SET_RX_MAGIC_NUMBER:
				ldi     ZL, low(MAGIC_NO * 2)
				ldi     ZH, high(MAGIC_NO * 2) 
				PUSH    R16
				LDI     R16,8
				ldi     XL, LOW(RX_ADD)
				CLR     XH
SAVE_TO_REGS:
				lpm
				adiw	zl,1
				st      X+, R0
				DEC     R16
				BRNE    PC-4
				POP     R16
				RET
//------------------
COPY_VARIABLES:
				ldi     ZL, low(FREQX * 2)
				ldi     ZH, high(FREQX * 2) 
				PUSH    R16
				LDI     R16, LOW(DAT_END*2 - FREQX*2)
				ldi     xl, LOW(FREQ1)
				ldi     XH, HIGH(FREQ1)	
				RJMP    SAVE_TO_REGS
//------------------
FREQ1_TO_FREQ3:
				ldi    	ZL, low(FREQ1)
				ldi     ZH, high(FREQ1)
				ldi    	XL, low(FREQ3)
				ldi     XH, high(FREQ3)
				RJMP    STORE4BYTES
FREQ3_TO_FREQ1X:
				ldi    	ZL, low(FREQ3)
				ldi     ZH, high(FREQ3)
				RJMP    STORE_FREQ1X
FREQ1_TO_FREQ1X:
				ldi    	ZL, low(FREQ1)
				ldi     ZH, high(FREQ1)
STORE_FREQ1X:
				ldi    	XL, low(FREQ1X)
				ldi     XH, high(FREQ1X)
				RJMP    STORE4BYTES
FREQ1X_TO_RY:
				ldi    	zl, low(FREQ1X)
				ldi     zh, high(FREQ1X)
STORE_FREQ: 
				RCALL   CLEAR_RY
				ldi     XL, LOW(RY_ADD)
				CLR     XH
STORE4BYTES:
				PUSH    R16
				LDI     R16,4

				LD      R0, Z+
				ST      X+, R0
				DEC     R16
				BRNE    PC-3
				POP     R16
				RET
//------------------
FREQ2_TO_RY:
				ldi    	zl, low(FREQ2)
				ldi     zh, high(FREQ2) 
				RJMP    STORE_FREQ
//-------------------------------
FREQ3_TO_RY:
				ldi    	zl, low(FREQ3)
				ldi     zh, high(FREQ3) 
				RJMP    STORE_FREQ
//-----------------------------------
READ_EEPROM_DATA:
				RCALL   READ_EE_C_OWN
READ_EE_L_OWN:	
				LDI     R17, LOW(EE_L_OWN)
				RCALL   GET_EE_2BYTES
				BREQ    READ_EE_C_REF
				STS     L_OWN, R19
				STS     L_OWN+1, R16
READ_EE_C_REF:  LDI     R17, LOW(EE_C_REF)
				RCALL   GET_EE_2BYTES
				BREQ    READ_EE_F1
				STS     C_REF, R19
				STS     C_REF+1, R16
READ_EE_F1:		LDI     R17, LOW(EE_F1+2)
				RCALL   GET_EE_2BYTES
				TST     R16
				BRNE    READ_EE_F2
				STS     FREQ1+2, R19
				STS     FREQ1+3, R16				
				LDI     R17, LOW(EE_F1)
				RCALL   GET_EE_2BYTES
				STS     FREQ1, R19
				STS     FREQ1+1, R16
READ_EE_F2:		LDI     R17, LOW(EE_F2+2)
				RCALL   GET_EE_2BYTES
				TST     R16
				BRNE    READ_EE_R1
				STS     FREQ2+2, R19
				STS     FREQ2+3, R16				
				LDI     R17, LOW(EE_F2)
				RCALL   GET_EE_2BYTES
				STS     FREQ2, R19
				STS     FREQ2+1, R16

READ_EE_R1:     LDI     R17, LOW(EE_R1)
				RCALL   GET_EE_2BYTES
				BREQ    READ_EE_R2
				STS     REZ1, R19
				STS     REZ1+1, R16
READ_EE_R2:     LDI     R17, LOW(EE_R2)
				RCALL   GET_EE_2BYTES
				BREQ    END_EE_READ
				STS     REZ2, R19
				STS     REZ2+1, R16
END_EE_READ:	RET
//---
READ_EE_C_OWN:
				LDI     R17, LOW(EE_C_OWN)
				RCALL   GET_EE_2BYTES
				BREQ    NO_STORE
				STS     C_OWN, R19
				STS     C_OWN+1, R16
NO_STORE:		RET
//---
GET_EE_2BYTES:  RCALL   EEPROM_READ
				MOV     R19, R16
				INC     R17
				RCALL   EEPROM_READ
				CPI     R19, 0xFF
				BREQ    PC+2
				RET
				CPI     R16, 0xFF
				RET
//====================================
EEPROM_READ:
				RCALL   EEPROM_WAIT
				CLR     R18
				out     EEARH, r18
				out     EEARL, r17
				sbi     EECR,EERE
				in      r16,EEDR
				ret
EEPROM_WAIT:
#ifdef m328p
				sbic    EECR,EEPE
#else
				sbic    EECR,EEWE
#endif
				rjmp    PC-1
				RET
//===================================
WRITE_EE_R1:
                LDI     R17, LOW(EE_R1)
				LDI     ZL,  LOW(REZ1)
				LDI     ZH,  HIGH(REZ1)
				RJMP    STORE_EE_2BYTES
WRITE_EE_R2:
                LDI     R17, LOW(EE_R2)
				LDI     ZL,  LOW(REZ2)
				LDI     ZH,  HIGH(REZ2)
				RJMP    STORE_EE_2BYTES

WRITE_EE_F2:
                LDI     R17, LOW(EE_F2)
				LDI     ZL, LOW(FREQ2)
				LDI     ZH, HIGH(FREQ2)
				RCALL   STORE_EE_2BYTES
				RJMP    STORE_EE_2BYTES
WRITE_EE_F1:
                LDI     R17, LOW(EE_F1)
				LDI     ZL, LOW(FREQ1)
				LDI     ZH, HIGH(FREQ1)
				RCALL   STORE_EE_2BYTES
				RJMP    STORE_EE_2BYTES
WRITE_EE_C_REF:
                LDI     R17, LOW(EE_C_REF)
				LDI     ZL, LOW(C_REF)
				LDI     ZH, HIGH(C_REF)
				RJMP    STORE_EE_2BYTES
WRITE_EE_L_OWN:
                LDI     R17, LOW(EE_L_OWN)
				LDI     ZL, LOW(L_OWN)
				LDI     ZH, HIGH(L_OWN)
				RJMP    STORE_EE_2BYTES

WRITE_EE_C_OWN:
                LDI     R17, LOW(EE_C_OWN)
				LDI     ZL, LOW(C_OWN)
				LDI     ZH, HIGH(C_OWN)
STORE_EE_2BYTES:
				LD      R16, Z+
				RCALL   EEPROM_WRITE
				LD      R16, Z+
EEPROM_WRITE:
				RCALL   EEPROM_WAIT
				CLR     R18
				out     EEARH, r18
				out     EEARL, r17
				INC     R17
				out     EEDR,r16
#ifdef m328p
				sbi     EECR,EEMPE
				sbi     EECR,EEPE
#else
				sbi     EECR,EEMWE
				sbi     EECR,EEWE
#endif
				RJMP    EEPROM_WAIT
//=================
SET_C_OWN:
				LDS      R1, C_OWN
				LDS      R2, C_OWN+1
				RCALL    COPY_ASCII_TO_CALC_PLACE

SET_C_OWN2:

				RCALL    PRINT_INFO_SET
				RCALL    COWN_OVERWRITE

				RCALL    KEYBOARD_WAIT
				CPI      R16, 4
				BRNE     SET_C_OWN2

				RCALL    ASCII_TO_BIN
				STS      C_OWN, R1
				STS      C_OWN+1, R2
				RCALL    WRITE_EE_C_OWN
				RCALL    READ_EE_C_OWN

///// RECALCULATE L_OWN VALUE /////

				RCALL    COIL_CALIBRATE
				RCALL    WRITE_EE_L_OWN

				RCALL    PRINT_INFO_SET
				RCALL    COWN_OVERWRITE

				RJMP    FINISH_SET
//---
COWN_OVERWRITE:
				RCALL    _from_line2
				ldi      zl, low(SET_COWN * 2)
				ldi      zh, high(SET_COWN * 2) 
				rcall    WriteLCDString
				RJMP     CURSOR_EN
//=================
SET_RES2:
				LDS      R1, REZ2
				LDS      R2, REZ2+1
				RCALL    COPY_ASCII_TO_CALC_PLACE
SET_REZ02:
				RCALL    PRI7
				RCALL    KEYBOARD_WAIT
				CPI      R16, 4
				BRNE     SET_REZ02
				RCALL    ASCII_TO_BIN
				STS      REZ2, R1
				STS      REZ2+1, R2
				RCALL    WRITE_EE_R2
				RCALL    READ_EEPROM_DATA
				RCALL    PRI7
				RJMP     FINISH_SET
//-----------------------------------
SET_RES1:
				LDS      R1, REZ1
				LDS      R2, REZ1+1
				RCALL    COPY_ASCII_TO_CALC_PLACE
SET_REZ01:
				RCALL    PRI5
				RCALL    KEYBOARD_WAIT
				CPI      R16, 4
				BRNE     SET_REZ01
				RCALL    ASCII_TO_BIN
				STS      REZ1, R1
				STS      REZ1+1, R2
				RCALL    WRITE_EE_R1
				RCALL    READ_EEPROM_DATA
				RCALL    PRI5
				RJMP     FINISH_SET
//---
PRI7:
				RCALL    PRI2

				ldi      ZL, low(SET_REZ002 * 2)
				ldi      ZH, high(SET_REZ002 * 2) 
				rcall    WriteLCDString
				RJMP     PRI5A
//---
PRI5:
				RCALL    PRI2

				ldi      ZL, low(SET_REZ001 * 2)
				ldi      ZH, high(SET_REZ001 * 2) 
				rcall    WriteLCDString
//---
PRI5A:
				RCALL    PRI3A
				RCALL    PRI6
				RJMP     PRI4
//--------------
PRI6:
				ldi      ZL, low(OHM_TXT * 2)
				ldi      ZH, high(OHM_TXT * 2) 
				RJMP     WriteLCDString
//--------------------------------
SET_C_REF:
				LDS      R1, C_REF
				LDS      R2, C_REF+1
				RCALL    COPY_ASCII_TO_CALC_PLACE
SET_C_REF2:
				RCALL    PRINT_INFO_SET
				RCALL    KEYBOARD_WAIT
				CPI      R16, 4
				BRNE     SET_C_REF2
				RCALL    ASCII_TO_BIN
				STS      C_REF, R1
				STS      C_REF+1, R2
				RCALL    WRITE_EE_C_REF
				RCALL    READ_EEPROM_DATA
				RCALL    PRINT_INFO_SET
FINISH_SET:
				//SET CURSOR FF
				ldi	R16, (1<<LCD_STER)|(1<<LCD_SW_ON)|(1<<LCD_SK_OFF)|(1<<LCD_SM_OFF)
				rcall	Instrukcja_LCD
				RCALL    _from_line1
				ldi      zl, low(DONE_INFO * 2)
				ldi      zh, high(DONE_INFO * 2) 
				rcall    WriteLCDString
				RCALL    GET_KEY_STATE
				RJMP     _RESET_PROC
//---
ASCII_TO_BIN:
				LDI      ZL, LOW(CALC_PLACE)
				LDI      ZH, HIGH(CALC_PLACE)
				LDI      R16, 4
				CLR      R0
				CLR      R1
				CLR      R2
MUL_10:
				LD       R17, Z+
				ANDI     R17, 0x0F
				ADD      R1, R17
				ADC      R2, R0
				DEC      R16
				BREQ     EXIT_MUL_10
				MOV      R3, R1
				MOV      R4, R2

				LSL      R1
				ROL      R2
				LSL      R1
				ROL      R2
				ADD      R1, R3
				ADC      R2, R4
				LSL      R1
				ROL      R2

				RJMP     MUL_10
EXIT_MUL_10:
				RET
//---
KEYBOARD_WAIT:
				RCALL    GET_KEY_STATE
				ANDI     R16, 0x04
				BRNE     DIGIT_UP

NEXT_DIGIT:		LDS      R16, POINT_PLACE
				INC      R16
				STS      POINT_PLACE, R16
				RET
DIGIT_UP:
				LDS      R16, POINT_PLACE

				CLR      R17

				LDI      ZL, LOW(CALC_PLACE)
				LDI      ZH, HIGH(CALC_PLACE)
				ADD      ZL, R16
				ADC      ZH, R17
				LD       R17, Z
				INC      R17
				CPI      R17, 0x3A
				BRNE     PC+2
				LDI      R17, 0x30
				ST       Z, R17
				RET
//---
GET_KEY_STATE:
				
				RCALL   WAIT50MS
				IN      R16, PINB
				ANDI    R16, 0b00001100
				CPI     R16, 0b00001100
				BREQ    GET_KEY_STATE
GET_KEY_STATE3:
				PUSH    R16

GET_KEY_STATE2:
				RCALL   WAIT50MS
				IN      R16, PINB
				ANDI    R16, 0b00001100
				CPI     R16, 0b00001100
				BRNE    GET_KEY_STATE2
				RCALL   RING
				POP     R16
				RET
//---
RING:
				PUSH R16
				PUSH R17
				SBI PORTB, 1
				RCALL   WAIT50MS
				CBI PORTB, 1
				POP R17
				POP R16
				RET

//---
WAIT250MS:
				LDI     R16, 250
				RJMP    STILL_WAIT
//---
WAIT50MS:
				ldi	    R16, 50
STILL_WAIT:
				ldi	    R17, 1
				RJMP	Czekaj_ms
//---
COPY_ASCII_TO_CALC_PLACE:

				RCALL    CONVERT_16BIT
				LDI      ZL, LOW(OUT_ASCII_BUFF+6)
				LDI      ZH, HIGH(OUT_ASCII_BUFF+6)

				LDI      XL, LOW(CALC_PLACE)
				LDI      XH, HIGH(CALC_PLACE)

				LDI      R16,4

				LD       R24, Z+
				ORI      R24, 0x30
				ST       X+, R24
				DEC      R16
				BRNE     PC-4
				STS      POINT_PLACE, R16
				RET
//---
PRI2:
				rcall    LcdIni
				ldi      zl, low(SET_INFO * 2)
				ldi      zh, high(SET_INFO * 2) 
				rcall    WriteLCDString
				RJMP    _from_line2
//---
PRI3:
				ldi      zl, low(SET_CREF * 2)
				ldi      zh, high(SET_CREF * 2) 
				rcall    WriteLCDString
//---
PRI3A:
				ldi      zl, low(CALC_PLACE)
				ldi      zh, high(CALC_PLACE) 
				LDI      R16, 4
				LD       R24, Z+
				RCALL    _Lch
				DEC      R16
				BRNE     PC-3
				RET
//---
PRI4:
				LDS     R0, POINT_PLACE
				LDI     r24, 0xC7
				ADD     R24, R0
				RCALL   MAKE_SET

				//SET CURSOR ON
				ldi	    R16, (1<<LCD_STER)|(1<<LCD_SW_ON)|(1<<LCD_SK_ON)|(1<<LCD_SM_ON)
				RJMP	Instrukcja_LCD
//---
PRINT_INFO_SET:
				RCALL    PRI2
				RCALL    PRI3

				RCALL    LP01  // PRINT " pF"

CURSOR_EN:
				RJMP     PRI4
//------------------------------------
CHECK_FREQ:
				LDS     R0, FR3_HI
				TST     R0
				BRNE    CHECK_FREQ_EXIT
				rcall    LcdIni
				RCALL    _from_line2
				ldi      zl, low(NO_LX * 2)
				ldi      zh, high(NO_LX * 2) 
				rcall    WriteLCDString	
				CLR      R0			
CHECK_FREQ_EXIT:
				RET
//------------------------------------
DO_COMPENSATE:
				rcall    LcdIni
				ldi      zl, low(MS_WAIT * 2)
				ldi      zh, high(MS_WAIT * 2) 
				rcall    WriteLCDString	
				RCALL    DO_MEASS
				RCALL    DO_MEASS
				RCALL    COPY_REG_TO_FREQ1
				RCALL    FREQ1_TO_FREQ1X

				RCALL    COIL_CALIBRATE

				RCALL    LcdIni
				RJMP     COMPENSATE
KEYBOARD_SERVICE:

				RCALL    GET_KEY_STATE3   // WAIT FOR UNPRESS KEY AND RINGING
				LDS      R16, MY_JOB
				INC      R16
				ANDI     R16, 3
				CPI      R16, 3
				BRNE     PC+2
CLEAR_JOB:
				CLR      R16
				STS      MY_JOB, R16    // CHANGE MY_JOB
				RET
//------------------------------------
READ_KEY:
				RCALL    GET_KEY_STATE
				ANDI     R16, 0x08
				RET
//---
PRINT_MENU_CHOICE:

				rcall    LcdIni
				ldi      zl, low(MENU_CHOICE * 2)
				ldi      zh, high(MENU_CHOICE * 2) 
				rcall    WriteLCDString	
				RJMP     _from_line2
//---
MENU:
				RCALL    stop_meassure
				RCALL    GET_KEY_STATE3
MENU_LOOP:
// MENU COMPENSATE
				RCALL    PRINT_MENU_CHOICE
				ldi      zl, low(PR_COM * 2)
				ldi      zh, high(PR_COM * 2) 
				rcall    WriteLCDString	

				RCALL    READ_KEY
				BRNE     PC+2
				RJMP     DO_COMPENSATE
// MENU RESET
				RCALL    PRINT_MENU_CHOICE
				ldi      zl, low(PR_MEASS * 2)
				ldi      zh, high(PR_MEASS * 2) 
				rcall    WriteLCDString	

				RCALL    READ_KEY
				BRNE     PC+2
				RJMP     _RESET_PROC
// MENU DO CALIBRATE
				RCALL    PRINT_MENU_CHOICE
				ldi      zl, low(PR_CAL * 2)
				ldi      zh, high(PR_CAL * 2) 
				rcall    WriteLCDString	

				RCALL    READ_KEY
				BRNE     PC+2
				RJMP     RUN_CALIBRATE
// MENU SET C REF
				RCALL    PRINT_MENU_CHOICE
				ldi      zl, low(PR_REF * 2)
				ldi      zh, high(PR_REF * 2) 
				rcall    WriteLCDString	

				RCALL    READ_KEY
				BRNE     PC+2
				RJMP     SET_C_REF
// MENU SET C OWN
				RCALL    PRINT_MENU_CHOICE
				ldi      zl, low(PR_OWN * 2)
				ldi      zh, high(PR_OWN * 2) 
				rcall    WriteLCDString	

				RCALL    READ_KEY
				BRNE     PC+2
				RJMP     SET_C_OWN
// MENU SET RESISTOR 1
				RCALL    PRINT_MENU_CHOICE
				ldi      zl, low(PR_R1 * 2)
				ldi      zh, high(PR_R1 * 2) 
				rcall    WriteLCDString	

				RCALL    READ_KEY
				BRNE     PC+2
				RJMP     SET_RES1
// MENU SET RESISTOR 2
				RCALL    PRINT_MENU_CHOICE
				ldi      zl, low(PR_R2 * 2)
				ldi      zh, high(PR_R2 * 2) 
				rcall    WriteLCDString	

				RCALL    READ_KEY
				BRNE     PC+2
				RJMP     SET_RES2
// NEXT MENU
				RJMP     MENU_LOOP
//=================================
DIS100MS:
				CLI
				RCALL    stop_meassure
				RCALL    WAIT50MS
				RJMP     WAIT50MS				
//====================================
CE_MEASSURE:
				CBI      PORTD,2
				RCALL    DIS100MS
				RCALL    _from_line2
				LDI      R16, 16
				RCALL    Spacja_LCD

				RCALL    SMALL_CHARGE
WAIT_SMALL:
				MOV      R24, R3
				CPI      R24, 0x36   // 0.5 SEC NOT CHARGE ?
				BRCS     WSM2
				RCALL    DIS100MS    // TO BIG CAPP. GO TO BIG CHARGE
				RJMP     BIG_CX_MEASS
WSM2:
//--------------------
//- KEYBOARD SERVICE -
//--------------------
				SBIS     PINB,2     // SW2 PRESSED?
				RJMP     MENU
				SBIS     PINB,3     // SW1 PRESSED?
				RCALL    KEYBOARD_SERVICE
//--------------------
				TST      R10
				BREQ     WAIT_SMALL

				RCALL    SMALL_CX_PRINT
				RJMP     starter

BIG_CX_MEASS:
				RCALL    BIG_CHARGE
WAIT_BIG:
//--------------------
//- KEYBOARD SERVICE -
//--------------------
				SBIS     PINB,2     // SW2 PRESSED?
				RJMP     MENU
				SBIS     PINB,3     // SW1 PRESSED?
				RCALL    KEYBOARD_SERVICE
//--------------------
				TST      R10
				BREQ     WAIT_BIG

				RCALL    COPY_REG_TO_BIGC
				RCALL    BIG_CX_PRINT


DSCHG_LOOP:
				RCALL    DSCHG_PR

				RCALL    WAIT250MS
//--------------------
//- KEYBOARD SERVICE -
//--------------------
				SBIS     PINB,2     // SW2 PRESSED?
				RJMP     MENU
				SBIS     PINB,3     // SW1 PRESSED?
				RCALL    KEYBOARD_SERVICE
//--------------------
				RCALL    WAIT250MS
//--------------------
//- KEYBOARD SERVICE -
//--------------------
				SBIS     PINB,2     // SW2 PRESSED?
				RJMP     MENU
				SBIS     PINB,3     // SW1 PRESSED?
				RCALL    KEYBOARD_SERVICE
//--------------------
				RCALL    WAIT250MS
//--------------------
//- KEYBOARD SERVICE -
//--------------------
				SBIS     PINB,2     // SW2 PRESSED?
				RJMP     MENU
				SBIS     PINB,3     // SW1 PRESSED?
				RCALL    KEYBOARD_SERVICE
//--------------------
				RCALL    WAIT250MS
//--------------------
//- KEYBOARD SERVICE -
//--------------------
				SBIS     PINB,2     // SW2 PRESSED?
				RJMP     MENU
				SBIS     PINB,3     // SW1 PRESSED?
				RCALL    KEYBOARD_SERVICE
//--------------------
				LDS      R20,DSCH_SEC
				LDS      R21,DSCH_SEC +1
				TST      R20
				BRNE     PC+2
				DEC      R21
				DEC      R20
				STS      DSCH_SEC, R20
				STS      DSCH_SEC +1, R21
				CPI      R21, 0xFF
				BRNE     DSCHG_LOOP
				CPI      R20, 0xFF
				BRNE     DSCHG_LOOP

				RJMP     starter
//=================================================
//==================================================
//---

//---
BIG_CX_PRINT:
				RCALL    CLEAR_RY
				LDI      R24, 125
				MOV      RY0, R24
				RCALL    MULU32
				LDS      RY0, REZ3
				LDS      RY1, REZ3+1
				RCALL    DIV64
				CLR      RY1
				LDI      R24, 10
				MOV      RY0, R24
				RCALL    DIV64

				RCALL    copy_registers_to_long
				RCALL    CE_M_PR
				RCALL    get_string_pointers

				CPI      R17,4
				BRCS     MEDIUM_UF
				SUBI     R17, 3
				CLR      R18
				RCALL    print_from_buff
				RCALL    print_dot
				LDI      R17,3
				CLR      R18
				RCALL    print_from_buff
				RCALL    CE_MF_PR
				RJMP     DSCHG_CALC
//---
MEDIUM_UF:
				RCALL    print_meassure_0digits
				RCALL    CE_UF_PR
//------------------------------
// CALCULATE DELAY FOR DISCHARGE
//------------------------------
DSCHG_CALC:
				RCALL    COPY_BIGC_TO_LONG
				rcall    copy_long_to_registers
				RCALL    CLEAR_HIGH_RX
				RCALL    SET_RY_8M
				RCALL    DIV64
//------------------------------------
// STORE DELAY OF DISCHARGE IN SECONDS
//------------------------------------
				STS      DSCH_SEC, RX0
				STS      DSCH_SEC +1, RX1
				RET
//------------------

//==============================================
SMALL_CX_PRINT:
				RCALL    CLEAR_RY
				LDI      R24, 125
				MOV      RY0, R24
				RCALL    MULU32
				LDS      RY0, REZ1
				LDS      RY1, REZ1+1
				RCALL    DIV64
				CLR      RY1
				LDI      R24, 10
				MOV      RY0, R24
				RCALL    DIV64

				RCALL    copy_registers_to_long
				RCALL    CE_M_PR
				RCALL    get_string_pointers

				CPI      R17, 1
				BRNE     TEST_2_DG
				RCALL    print_0_dot
				RCALL    print_0
				RCALL    print_last_digit
				RJMP     CE_UF_PR
//---
TEST_2_DG:		CPI      R17, 2
				BRNE     TEST_3_DG
				RCALL    print_0_dot
				RCALL    print_two_last_digit
				RJMP     CE_UF_PR
TEST_3_DG:
				RCALL    print_meassure_2digits
				RJMP     CE_UF_PR
//================================================
CE_M_PR:
				rcall    LcdIni
				ldi      ZL, low(CE_M * 2)
				ldi      ZH, high(CE_M * 2) 
				RJMP     WriteLCDString
//---
CE_UF_PR:
				ldi      ZL, low(CE_UF * 2)
				ldi      ZH, high(CE_UF * 2)
				RJMP     WriteLCDString
CE_MF_PR:
				ldi      ZL, low(CE_MF * 2)
				ldi      ZH, high(CE_MF * 2)
				RJMP     WriteLCDString
//---
DSCHG_PR:
				RCALL    _from_line2
				LDI      R16, 16
				RCALL    Spacja_LCD
				RCALL    _from_line2
				ldi      ZL, low(DISCH * 2)
				ldi      ZH, high(DISCH * 2)
				RCALL    WriteLCDString

				rcall    CLEAR_MY_LONG_INT
				LDS      RX1,DSCH_SEC
				LDS      RX2,DSCH_SEC+1
				STS      MY_LONG_INT, RX1
				STS      MY_LONG_INT+1, RX2

				RCALL    get_string_pointers
				RJMP     print_meassure_0digits
//----------------------------------
// RZ =100 * R1*R2/(R1+R2) = 29399 -
//----------------------------------
R3_CALC:
				RCALL    CLEAR_RY

				LDS      RY0, REZ1
				LDS      RY1, REZ1+1
				RCALL    TRANSFER_RY32_TO_RX
				RCALL    SEND_RX_TO_RM
				LDS      RY0, REZ2
				LDS      RY1, REZ2+1
				ADD      RM0, RY0
				ADC      RM1, RY1
				RCALL    MULU32
				CLR      RY1
				LDI      R24,100
				MOV      RY0, R24
				RCALL    MULU32
				MOV      RY0, RM0
				MOV      RY1, RM1
				RCALL    DIV64
				STS      REZ3, RX0
				STS      REZ3+1, RX1
				RET
//------------------------
//--- block interrupts ---
//------------------------
CE_START:
				rcall    stop_meassure
				rcall    CLEAR_MY_LONG_INT
				RCALL    copy_long_to_registers
				CLI
//---------------------------------
//--------------------------------------------------------

//--------------------------------------------------------

				LDI      ZL, (1<<ISC11) | (0<<ISC10)
#ifdef m328p
				sts      EICRA, ZL                 // ENABLE INT1 FALLING EDGE
				LDI      ZL, (1<<INTF1)
				out      EIFR, ZL                 // CLR IRQ FLAG
				ldi      ZL, TIMSK1_START_VALUE
				sts      TIMSK1, ZL
				ldi      ZL, (1<<INT1)
				out      EIMSK, ZL
#else
				out      MCUCR, ZL                  // ENABLE INT1 FALLING EDGE
				LDI      ZL, (1<<INTF1)				
				out      GIFR, ZL                   // CLR IRQ FLAGS
				ldi      ZL, (1<<TOIE1)
				out      TIMSK,  ZL
				ldi      ZL, (1<<INT1)
				OUT      GICR, ZL
#endif
				ret
//--------------
DISCHARGE:
				CBI      DDRB,3
				SBI      PORTD,3

				SBI      DDRB,0
				SBI      DDRD,7

				SBI      PORTB,0
				SBI      PORTD,7
				RET
//=================================
SMALL_CHARGE:
				RCALL    CE_START
				CLR      R10
				RCALL    CLEAR_IRQ_FLAGS
#ifdef m328p
				ldi      ZL,(1<<CS10)
				sts      TCCR1B, ZL                 // START TIMER1
#else
				ldi      ZL,(1<<CS10)
				out      TCCR1B, ZL                 // START TIMER1
#endif
				SEI
				SBI      PORTD,7
				CBI      PORTB,0
				RET
//=================================
BIG_CHARGE:
				RCALL    CE_START
				CLR      R10
				RCALL    CLEAR_IRQ_FLAGS
#ifdef m328p
				ldi      ZL,(1<<CS10)
				sts      TCCR1B, ZL                 // START TIMER1
#else
				ldi      ZL,(1<<CS10)
				out      TCCR1B, ZL                 // START TIMER1
#endif
				CBI      PORTB,0
				CBI      PORTD,7
				SEI
				RET
//***********************************************
//--------------------------
//-- a start info string ---
//--------------------------
welcome:
				.DB "WEGI L / C METER" ,0,0
welcome2:
				.DB "  2019-03-02 ",0
welcome3:
				.DB "KM409 - REMARK ",0
//-------------------
CALIB_START:
				.DB " OPEN CIRCUIT" , 0
SW2_PRESS:
				.DB "AND PRESS SW2", 0
F1_MEAS:
				.DB " MEASSURE F1..." , 0
AD_CREF:
				.DB "ADD CREF=" , 0
F2_MEAS:
				.DB " MEASSURE F2..." , 0
CAL_OK:
				.DB " CALIBRATED", 0

F1_EQU:			.DB "F1=",0

F2_EQU:			.DB "F2=",0

DONE_INFO:		.DB "     DONE !     ",0,0

SET_INFO:		.DB "SET         NEXT",0,0

SET_CREF:		.DB "CREF = ",0
SET_COWN:		.DB "COWN ",0
SET_REZ001:		.DB "RES1 = ",0
SET_REZ002:		.DB "RES2 = ",0
OHM_TXT:        .DB " OHM",0,0

NO_LX:			.DB "Lx = 0.00 uH",0,0

MENU_CHOICE:	.DB "ENTER       NEXT",0,0
PR_COM:			.DB "DO COMPENSATE",0
PR_REF:			.DB "SET C REF VALUE",0
PR_OWN:			.DB "SET C OWN VALUE",0
PR_R1:			.DB "SET RES 1 VALUE",0
PR_R2:			.DB "SET RES 2 VALUE",0

PR_CAL:			.DB "DO  CALIBRATE",0
PR_MEASS:		.DB "   DO RESET",0
MS_WAIT:		.DB "    WAIT...",0
CE_M:			.DB "CE = ",0
CE_UF:          .DB " uF",0
CE_MF:          .DB " mF",0
DISCH:          .DB "DISCHARGE ",0,0

//==========================================
STATIC8M:
    .DB 0x00, 0x12, 0x7A, 0x00, 0x00, 0x00  // CLOCK FREQ 8 000 000 VALUE
MAGIC_NO:
	.DB 0xFE, 0x6F, 0x97, 0x14, 0xFE, 0x20, 0x27, 0x23
FREQX:
	.DB 0x38, 0xB6, 0x07, 0x00 // FREQ1 505 400 Hz
	.DB 0x48, 0x6C, 0x05, 0x00 // FREQ2 355 400 Hz
	.DB 0xEE, 0x06, 0x07, 0x00 // FREQ3 460 526 Hz
	.DB 0xE8, 0x03             // C_REF 1000 pF CALIBRATE VALUE
	.DB 0xD2, 0x03             // C_OWN 978  pF CIRCUIT   VALUE
	.DB 0x9B, 0x27             // L_OWN 101.39 uH (10139)
	.DB 0xB3, 0x0B             // R1 = 2995 OHM
	.DB 0x46, 0x01             // R2 = 326  OHM
DAT_END:
//==========================================

//-------
//- EOF -
//-------

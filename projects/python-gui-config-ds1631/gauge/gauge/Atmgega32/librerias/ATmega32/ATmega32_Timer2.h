/*Archivo de configuración del timer/Counter2 del ATmega32*/


/* Configuración del puerto sobre el que actua el Timer*/

#define Timer2_Direction_Port DDRD
#define Timer2_Data_Port      PORTD
#define Timer2_Pins_Port      PIND
#define OC2                   7

/*
Timer/Counter2 is a general purpose, single compare unit, 8-bit Timer/Counter module.

The main features are:
	• Single Compare unit Counter
	• Clear Timer on Compare Match (Auto Reload)
	• Glitch-free(Libre de errores de pulsos), Phase Correct Pulse Width Modulator (PWM)
	• Frequency Generator
	• 10-bit Clock Prescaler
	• Overflow and Compare Match Interrupt Sources (TOV2 and OCF2)
	• Allows clocking from External 32 kHz Watch Crystal Independent of the I/O Clock
*/

/*
Timer/Counter Control
Register – TCCR2       Bit              7      6        5       4       3        2       1       0
                                      FOC2   WGM20   COM21   COM20    WGM21    CS22     CS21   CS20    TCCR2
                       Read/Write       W     R/W     R/W     R/W      R/W     R/W      R/W     R/W
                       Initial Value    0      0        0       0       0        0       0       0

• Bit 7 – FOC2: Force Output Compare
The FOC2 bit is only active when the WGM bits specify a non-PWM mode. However, for
ensuring compatibility with future devices, this bit must be set to zero when TCCR2 is
written when operating in PWM mode. When writing a logical one to the FOC2 bit, an
immediate compare match is forced on the waveform generation unit. The OC2 output is
changed according to its COM21:0 bits setting. Note that the FOC2 bit is implemented
as a strobe. Therefore it is the value present in the COM21:0 bits that determines the
effect of the forced compare.
A FOC2 strobe will not generate any interrupt, nor will it clear the timer in CTC mode
using OCR2 as TOP.
The FOC2 bit is always read as zero.
*/

#define Timer2_Force_Output_Compare_SetOn TCCR2  |=  _BV( FOC2)
#define Timer2_Force_Output_Compare_SetOff TCCR2 &= ~_BV( FOC2)
		
/*
• Bit 6, 3 – WGM21:0: Waveform Generation Mode
These bits control the counting sequence of the counter, the source for the maximum
(TOP) counter value, and what type of waveform generation to be used. Modes of oper-
ation supported by the Timer/Counter unit are: Normal mode, Clear Timer on Compare
match (CTC) mode, and two types of Pulse Width Modulation (PWM) modes. See Table
50 and “Modes of Operation” on page 118.
Table 50. Waveform Generation Mode Bit Description(1)
          WGM21       WGM20      Timer/Counter Mode of                Update of   TOV2 Flag
 Mode     (CTC2)      (PWM2)     Operation                   TOP      OCR2        Set on
    0         0          0       Normal                      0xFF     Immediate   MAX*/
#define Timer2_Waveform_Generation_Mode_Normal_Operation TCCR2 &= ~_BV( WGM20) & ~_BV( WGM21)
/*  
    1         0          1       PWM, Phase Correct          0xFF     TOP         BOTTOM*/
#define Timer2_Waveform_Generation_Mode_Phase_Correct    TCCR2 |= _BV( WGM20)
/*    
    2         1          0       CTC                         OCR2     Immediate   MAX*/
#define Timer2_Waveform_Generation_Mode_CTC              TCCR2 |= _BV( WGM21)
/*    
    3         1          1       Fast PWM                    0xFF     BOTTOM      MAX*/
#define Timer2_Waveform_Generation_Mode_Fast_PWM         TCCR2 |= _BV( WGM20) | _BV( WGM21)
/*    
Note:   1. The CTC2 and PWM2 bit definition names are now obsolete. Use the WGM21:0 def-
            initions. However, the functionality and location of these bits are compatible with
            previous versions of the timer.
*/

/*

 Bit 5:4 – COM21:0: Compare Match Output Mode
These bits control the Output Compare pin (OC2) behavior. If one or both of the
COM21:0 bits are set, the OC2 output overrides the normal port functionality of the I/O
pin it is connected to. However, note that the Data Direction Register (DDR) bit corre-
sponding to the OC2 pin must be set in order to enable the output driver.
*/
#define Timer2_OutPut_Compare_Pin_Set_Out Timer2_Direction_Port |= _BV( OC2)
/*
When OC0 is connected to the pin, the function of the COM21:0 bits depends on the
WGM01:0 bit setting. Table 39 shows the COM21:0 bit functionality when the WGM01:0
bits are set to a normal or CTC mode (non-PWM).

Table 39. Compare Output Mode, non-PWM Mode
    COM21          COM20      Description
       0              0       Normal port operation, OC0 disconnected.
*/
#define Timer2_Compare_Output_Mode_Set_Off  TCCR2 &= ~_BV( COM20) & ~_BV( COM21)
/*
       0              1       Toggle OC0 on compare match*/
#define Timer2_Compare_Output_Mode_Non_PWM_Toggle_OC2_On_Compare_Match TCCR2 |= _BV( COM20)
/*
       1              0       Clear OC0 on compare match*/
#define Timer2_Compare_Output_Mode_Non_PWM_Clear_OC2_On_Compare_Match TCCR2 |= _BV( COM21)
/*
       1              1       Set OC0 on compare match*/
#define Timer2_Compare_Output_Mode_Non_PWM_Set_OC2_On_Compare_Match TCCR2 |= _BV( COM20) | _BV( COM21)

/*
Table 40 shows the COM21:0 bit functionality when the WGM01:0 bits are set to fast
PWM mode.

Table 40. Compare Output Mode, Fast PWM Mode(1)
   COM21        COM20       Description
      0            0        Normal port operation, OC0 disconnected.
      0            1        Reserved
      1            0        Clear OC0 on compare match, set OC0 at BOTTOM,
                            (nin-inverting mode)*/
#define Timer2_Compare_Output_Mode_Fast_PWM_Clear_OC2_On_Compare_Match TCCR2 |= _BV( COM21)
/*
                            
      1            1        Set OC0 on compare match, clear OC0 at BOTTOM,
                            (inverting mode)*/
#define Timer2_Compare_Output_Mode_Fast_PWM_Set_OC2_On_Compare_Match TCCR2 |= _BV( COM20) | _BV( COM21)
/*                            
Note:   1. A special case occurs when OCR0 equals TOP and COM21 is set. In this case, the
           compare match is ignored, but the set or clear is done at BOTTOM. See “Fast PWM
           Mode” on page 75 for more details.

Table 41 shows the COM21:0 bit functionality when the WGM01:0 bits are set to phase
correct PWM mode.
Table 41. Compare Output Mode, Phase Correct PWM Mode(1)
 COM21     COM20     Description
    0         0      Normal port operation, OC0 disconnected.
    0         1      Reserved
    1         0      Clear OC0 on compare match when up-counting. Set OC0 on compare
                     match when downcounting.*/
#define Timer2_OutPut_Compare_Mode_Phase_Correct_PWM_Clear_OC2_On_Compare_Match TCCR2 |= _BV( COM21)
/*
    1         1      Set OC0 on compare match when up-counting. Clear OC0 on compare
                     match when downcounting.*/
#define Timer2_OutPut_Compare_Mode_Phase_Correct_PWM_SetClear_OC2_On_Compare_Match TCCR2 |= _BV( COM20) | _BV( COM21)
/*                     
Note:   1. A special case occurs when OCR0 equals TOP and COM21 is set. In this case, the
           compare match is ignored, but the set or clear is done at TOP. See “Phase Correct
           PWM Mode” on page 76 for more details.

*/

/*
• Bit 2:0 – CS22:0: Clock Select
The three Clock Select bits select the clock source to be used by the Timer/Counter, see
Table 54.
Table 54. Clock Select Bit Description
  CS22       CS21       CS20       Description
     0         0           0       No clock source (Timer/Counter stopped).*/
#define Timer2_Stop_Timer TCCR2 &= ~_BV( CS20) & ~_BV( CS21) & ~_BV( CS22)/*
     0         0           1	clkT2S/(No prescaling)*/
#define Timer2_Set_Clock_Preescaling_1 TCCR2 |= _BV( CS20)/*
     0         1           0	clkT2S/8 (From prescaler)*/
#define Timer2_Set_Clock_Preescaling_8 TCCR2 |= _BV( CS21)/*
     0         1           1	clkT2S/32 (From prescaler)*/
#define Timer2_Set_Clock_Preescaling_32 TCCR2 |= _BV( CS20) | _BV( CS21)/*
     1         0           0	clkT2S/64 (From prescaler)*/
#define Timer2_Set_Clock_Preescaling_64 TCCR2 |= _BV( CS22)/*
     1         0           1	clkT2S/128 (From prescaler)*/
#define Timer2_Set_Clock_Preescaling_128 TCCR2 |= _BV( CS20) | _BV( CS22)/*
     1         1           0	clkT2S/256 (From prescaler)*/
#define Timer2_Set_Clock_Preescaling_1024 TCCR2 |= _BV( CS20) | _BV( CS21) | _BV( CS22)/*
     1         1           1	clkT2S/1024 (From prescaler)
*/

/*
Asynchronous Status
Register – ASSR       Bit           7       6      5       4       3       2       1        0
                                    –       –      –       –      AS2   TCN2UB   OCR2UB  TCR2UB       ASSR
                      Read/Write    R       R      R       R      R/W      R       R        R
                      Initial Value 0       0      0       0       0       0       0        0

                    • Bit 3 – AS2: Asynchronous Timer/Counter2
                    When AS2 is written to zero, Timer/Counter 2 is clocked from the I/O clock, clkI/O. When
                    AS2 is written to one, Timer/Counter2 is clocked from a Crystal Oscillator connected to
                    the Timer Oscillator 1 (TOSC1) pin. When the value of AS2 is changed, the contents of
                    TCNT2, OCR2, and TCCR2 might be corrupted.*/
#define Timer2_32Khz_External_Clock_Set_On  ASSR |=  _BV( AS2)
#define Timer2_32Khz_External_Clock_Set_Off ASSR &= ~_BV( AS2)
/*
                    • Bit 2 – TCN2UB: Timer/Counter2 Update Busy
                    When Timer/Counter2 operates asynchronously and TCNT2 is written, this bit becomes
                    set. When TCNT2 has been updated from the temporary storage register, this bit is
                    cleared by hardware. A logical zero in this bit indicates that TCNT2 is ready to be
                    updated with a new value.
                    • Bit 1 – OCR2UB: Output Compare Register2 Update Busy
                    When Timer/Counter2 operates asynchronously and OCR2 is written, this bit becomes
                    set. When OCR2 has been updated from the temporary storage register, this bit is
                    cleared by hardware. A logical zero in this bit indicates that OCR2 is ready to be
                    updated with a new value.
                    • Bit 0 – TCR2UB: Timer/Counter Control Register2 Update Busy
                    When Timer/Counter2 operates asynchronously and TCCR2 is written, this bit becomes
                    set. When TCCR2 has been updated from the temporary storage register, this bit is
                    cleared by hardware. A logical zero in this bit indicates that TCCR2 is ready to be
                    updated with a new value.
                    If a write is performed to any of the three Timer/Counter2 Registers while its update
                    busy flag is set, the updated value might get corrupted and cause an unintentional inter-
                    rupt to occur.
                    The mechanisms for reading TCNT2, OCR2, and TCCR2 are different. When reading
                    TCNT2, the actual timer value is read. When reading OCR2 or TCCR2, the value in the
                    temporary storage register is read.



Timer/Counter Interrupt Mask
Register – TIMSK              Bit             7      6        5      4         3         2      1        0
                                            OCIE2  TOIE2   TICIE1  OCIE1A   OCIE1B     TOIE1  OCIE0    TOIE0     TIMSK
                              Read/Write     R/W    R/W     R/W     R/W      R/W        R/W    R/W      R/W
                              Initial Value   0      0        0      0         0         0      0        0

                             • Bit 7 – OCIE2: Timer/Counter2 Output Compare Match Interrupt Enable
                             When the OCIE2 bit is written to one and the I-bit in the Status Register is set (one), the
                             Timer/Counter2 Compare Match interrupt is enabled. The corresponding interrupt is
                             executed if a compare match in Timer/Counter2 occurs, i.e., when the OCF2 bit is set in
                             the Timer/Counter Interrupt Flag Register – TIFR.*/
#define Timer2_Interrupt_Enable_Output_Compare_Match TIMSK |= _BV( OCIE2)
/*

                             • Bit 6 – TOIE2: Timer/Counter2 Overflow Interrupt Enable
                             When the TOIE2 bit is written to one and the I-bit in the Status Register is set (one), the
                             Timer/Counter2 Overflow interrupt is enabled. The corresponding interrupt is executed if
                             an overflow in Timer/Counter2 occurs, i.e., when the TOV2 bit is set in the
                             Timer/Counter Interrupt Flag Register – TIFR.*/
#define Timer2_Interrupt_Enable_Overflow TIMSK |= _BV( TOIE2)                           

/*
Timer/Counter Interrupt Flag
Register – TIFR               Bit             7        6        5     4         3      2       1       0
                                            OCF2     TOV2     ICF1  OCF1A    OCF1B   TOV1     OCF0   TOV0      TIFR
                              Read/Write     R/W     R/W      R/W    R/W       R/W    R/W     R/W     R/W
                              Initial Value   0        0        0     0         0      0       0       0

                             • Bit 7 – OCF2: Output Compare Flag 2
                             The OCF2 bit is set (one) when a compare match occurs between the Timer/Counter2
                             and the data in OCR2 – Output Compare Register2. OCF2 is cleared by hardware when
                             executing the corresponding interrupt handling vector. Alternatively, OCF2 is cleared by
                             writing a logic one to the flag. When the I-bit in SREG, OCIE2 (Timer/Counter2 Com-
                             pare match Interrupt Enable), and OCF2 are set (one), the Timer/Counter2 Compare
                             match Interrupt is executed.

                             • Bit 6 – TOV2: Timer/Counter2 Overflow Flag
                             The TOV2 bit is set (one) when an overflow occurs in Timer/Counter2. TOV2 is cleared
                             by hardware when executing the corresponding interrupt handling vector. 
                             Alternatively,TOV2 is cleared by writing a logic one to the flag. When the SREG I-bit, TOIE2 (Timer/Counter2 Overflow Interrupt Enable), and TOV2 are set (one), the Timer/Counter2 Overflow interrupt is executed. In PWM mode, this bit is set when Timer/Counter2 changes counting direction at $00.
*/


/*Set TCNT2 to zero*/

#define Timer2_Set_TCNT2_To_0 TCNT2 = 0x00


void Init_RTC()
{

Timer2_OutPut_Compare_Pin_Set_Out;
Timer2_Stop_Timer;
Timer2_Waveform_Generation_Mode_CTC;
Timer2_32Khz_External_Clock_Set_On;
Timer2_Set_Clock_Preescaling_1024;
Timer2_Compare_Output_Mode_Non_PWM_Toggle_OC2_On_Compare_Match;
Timer2_Interrupt_Enable_Output_Compare_Match;
Timer2_Set_TCNT2_To_0;
OCR2= 16;// 1 pulso por segundo

}

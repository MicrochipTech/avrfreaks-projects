/* Archivo de configuración del temporizador Timer/Counter0 del ATmega32*/

/* Configuración del puerto sobre el que actua el Timer*/

#define Timer0_Direction_Port DDRB
#define Timer0_Data_Port      PORTB
#define Timer0_Pins_Port      PINB


/* Configuración del Timer/Counter0 Register Control TCCR0*/

/*

 Bit 7 -> FOC0 => Force Output Compare

The FOC0 bit is only active when the WGM00 bit specifies a non-PWM mode. However,
for ensuring compatibility with future devices, this bit must be set to zero when TCCR0 is
written when operating in PWM mode. When writing a logical one to the FOC0 bit, an
immediate compare match is forced on the Waveform Generation unit. The OC0 output
is changed according to its COM01:0 bits setting. Note that the FOC0 bit is implemented
as a strobe. Therefore it is the value present in the COM01:0 bits that determines the
effect of the forced compare.
A FOC0 strobe will not generate any interrupt, nor will it clear the timer in CTC mode
using OCR0 as TOP.
The FOC0 bit is always read as zero.
*/

#define Timer0_Force_Output_Compare_Set_On  TCCR0  |= _BV( FOC0)
#define Timer0_Force_Output_Compare_Set_Off TCCR0  &= ~_BV( FOC0)

/*
 Bit 6, 3 – WGM01:0: Waveform Generation Mode
These bits control the counting sequence of the counter, the source for the maximum
(TOP) counter value, and what type of Waveform Generation to be used. Modes of
operation supported by the Timer/Counter unit are: Normal mode, Clear Timer on Com-
pare Match (CTC) mode, and two types of Pulse Width Modulation (PWM) modes. See
Table 38 and “Modes of Operation” on page 73.

Table 38. Waveform Generation Mode Bit Description(1)
           WGM01        WGM00      Timer/Counter Mode                 Update of    TOV0 Flag
  Mode      (CTC0)      (PWM0)     of Operation              TOP      OCR0         Set-on
    0          0           0       Normal                    0xFF     Immediate    MAX
*/
#define Timer0_WaveForm_Generation_Mode_Normal           TCCR0 &= ~_BV( WGM01) & ~_BV( WGM00)
/*
    1          0           1       PWM, Phase Correct        0xFF     TOP          BOTTOM
*/
#define Timer0_WaveForm_Generation_Mode_PWM_Fase_Correct TCCR0 |=  _BV( WGM00)
/*
    2          1           0       CTC                       OCR0     Immediate    MAX
*/
#define Timer0_WaveForm_Generation_Mode_CTC              TCCR0 |=  _BV( WGM01)
/*
    3          1           1       Fast PWM                  0xFF     BOTTOM       MAX
*/
#define Timer0_WaveForm_Generation_Mode_Fast_PWM         TCCR0 |=  _BV( WGM01) | _BV( WGM00)
/*
Note:    1. The CTC0 and PWM0 bit definition names are now obsolete. Use the WGM01:0 def-
             initions. However, the functionality and location of these bits are compatible with
             previous versions of the timer.
*/

/*

 Bit 5:4 – COM01:0: Compare Match Output Mode
These bits control the Output Compare pin (OC0) behavior. If one or both of the
COM01:0 bits are set, the OC0 output overrides the normal port functionality of the I/O
pin it is connected to. However, note that the Data Direction Register (DDR) bit corre-
sponding to the OC0 pin must be set in order to enable the output driver.
*/
#define Timer0_OutPut_Compare_Pin_Set_Out Timer0_Direction_Port |= _BV( OC0)
/*
When OC0 is connected to the pin, the function of the COM01:0 bits depends on the
WGM01:0 bit setting. Table 39 shows the COM01:0 bit functionality when the WGM01:0
bits are set to a normal or CTC mode (non-PWM).

Table 39. Compare Output Mode, non-PWM Mode
    COM01          COM00      Description
       0              0       Normal port operation, OC0 disconnected.

       0              1       Toggle OC0 on compare match*/
#define Timer0_Compare_Output_Mode_Non_PWM_Toggle_OC0_On_Compare_Match TCCR0 |= _BV( COM00)
/*
       1              0       Clear OC0 on compare match*/
#define Timer0_Compare_Output_Mode_Non_PWM_Clear_OC0_On_Compare_Match TCCR0 |= _BV( COM01)
/*
       1              1       Set OC0 on compare match*/
#define Timer0_Compare_Output_Mode_Non_PWM_Set_OC0_On_Compare_Match TCCR0 |= _BV( COM00) | _BV( COM01)

/*
Table 40 shows the COM01:0 bit functionality when the WGM01:0 bits are set to fast
PWM mode.

Table 40. Compare Output Mode, Fast PWM Mode(1)
   COM01        COM00       Description
      0            0        Normal port operation, OC0 disconnected.
      0            1        Reserved
      1            0        Clear OC0 on compare match, set OC0 at BOTTOM,
                            (nin-inverting mode)*/
#define Timer0_Compare_Output_Mode_Fast_PWM_Clear_OC0_On_Compare_Match TCCR0 |= _BV( COM01)
/*
                            
      1            1        Set OC0 on compare match, clear OC0 at BOTTOM,
                            (inverting mode)*/
#define Timer0_Compare_Output_Mode_Fast_PWM_Set_OC0_On_Compare_Match TCCR0 |= _BV( COM00) | _BV( COM01)
/*                            
Note:   1. A special case occurs when OCR0 equals TOP and COM01 is set. In this case, the
           compare match is ignored, but the set or clear is done at BOTTOM. See “Fast PWM
           Mode” on page 75 for more details.

Table 41 shows the COM01:0 bit functionality when the WGM01:0 bits are set to phase
correct PWM mode.
Table 41. Compare Output Mode, Phase Correct PWM Mode(1)
 COM01     COM00     Description
    0         0      Normal port operation, OC0 disconnected.
    0         1      Reserved
    1         0      Clear OC0 on compare match when up-counting. Set OC0 on compare
                     match when downcounting.*/
#define Timer0_OutPut_Compare_Mode_Phase_Correct_PWM_Clear_OC0_On_Compare_Match TCCR0 |= _BV( COM01)
/*
    1         1      Set OC0 on compare match when up-counting. Clear OC0 on compare
                     match when downcounting.*/
#define Timer0_OutPut_Compare_Mode_Phase_Correct_PWM_SetClear_OC0_On_Compare_Match TCCR0 |= _BV( COM00) | _BV( COM01)
/*                     
Note:   1. A special case occurs when OCR0 equals TOP and COM01 is set. In this case, the
           compare match is ignored, but the set or clear is done at TOP. See “Phase Correct
           PWM Mode” on page 76 for more details.

*/

/*
  Bit 2:0 – CS02:0: Clock Select
The three Clock Select bits select the clock source to be used by the Timer/Counter.
Table 42. Clock Select Bit Description
   CS02      CS01     CS00    Description
     0        0        0      No clock source (Timer/Counter stopped).*/
#define Timer0_Stop_Timer TCCR0 &= ~_BV( CS00) & ~_BV( CS01) & ~_BV( CS02)
/*
     0        0        1      clkI/O/(No prescaling)*/
#define Timer0_Set_Clock_No_Preescaling TCCR0 |= _BV( CS00)     
/*
     0        1        0      clkI/O/8 (From prescaler)*/
#define Timer0_Set_Clock_Preescaling_8 TCCR0 |= _BV( CS01)     
/*
     0        1        1      clkI/O/64 (From prescaler)*/
#define Timer0_Set_Clock_Preescaling_64 TCCR0 |= _BV( CS00) | _BV( CS01)     
/*
     1        0        0      clkI/O/256 (From prescaler)*/
#define Timer0_Set_Clock_Preescaling_256 TCCR0 |= _BV( CS02)     
/*
     1        0        1      clkI/O/1024 (From prescaler)*/
#define Timer0_Set_Clock_Preescaling_1024 TCCR0 |= _BV( CS00) | _BV( CS02)
/*
     1        1        0      External clock source on T0 pin. Clock on falling edge.*/
#define Timer0_Set_Clock_External_Clock_Falling_Edge TCCR0 |= _BV( CS01) | _BV( CS02)
/*
     1        1        1      External clock source on T0 pin. Clock on rising edge.*/
#define Timer0_Set_Clock_External_Clock_Rising_Edge TCCR0 |=  _BV( CS00) | _BV( CS01) | _BV( CS02) 
/*
If external pin modes are used for the Timer/Counter0, transitions on the T0 pin will
clock the counter even if the pin is configured as an output. This feature allows software
control of the counting.
*/



/*
Timer/Counter Interrupt Mask
Register – TIMSK              Bit             7     6         5       4        3         2       1        0
                                            OCIE2 TOIE2    TICIE1   OCIE1A  OCIE1B     TOIE1   OCIE0    TOIE0     TIMSK
                              Read/Write     R/W   R/W      R/W      R/W      R/W       R/W     R/W      R/W
                              Initial Value   0     0         0       0        0         0       0        0
                              
                              
                              
                             • Bit 1 – OCIE0: Timer/Counter0 Output Compare Match Interrupt Enable
                             When the OCIE0 bit is written to one, and the I-bit in the Status Register is set (one), the
                             Timer/Counter0 Compare Match interrupt is enabled. The corresponding interrupt is
                             executed if a compare match in Timer/Counter0 occurs, i.e., when the OCF0 bit is set in
                             the Timer/Counter Interrupt Flag Register – TIFR.
*/
#define Timer0_Interrupt_Enable_Output_Compare_Match TIMSK |= _BV( OCIE0)                             
/*
                             • Bit 0 – TOIE0: Timer/Counter0 Overflow Interrupt Enable
                             When the TOIE0 bit is written to one, and the I-bit in the Status Register is set (one), the
                             Timer/Counter0 Overflow interrupt is enabled. The corresponding interrupt is executed if
                             an overflow in Timer/Counter0 occurs, i.e., when the TOV0 bit is set in the
                             Timer/Counter Interrupt Flag Register – TIFR.
*/
#define Timer0_Interrupt_Enable_Overflow TIMSK |= _BV( TOIE0)
/*

Timer/Counter Interrupt Flag
Register – TIFR               Bit             7        6        5     4         3        2       1        0
                                            OCF2     TOV2     ICF1  OCF1A    OCF1B     TOV1    OCF0     TOV0      TIFR
                              Read/Write     R/W     R/W      R/W    R/W       R/W      R/W     R/W      R/W
                              Initial Value   0        0        0     0         0        0       0        0
  
                             • Bit 1 – OCF0: Output Compare Flag 0
                             The OCF0 bit is set (one) when a compare match occurs between the Timer/Counter0
                             and the data in OCR0 – Output Compare Register0. OCF0 is cleared by hardware when
                             executing the corresponding interrupt handling vector. Alternatively, OCF0 is cleared by
                             writing a logic one to the flag. When the I-bit in SREG, OCIE0 (Timer/Counter0 Com-
                             pare Match Interrupt Enable), and OCF0 are set (one), the Timer/Counter0 Compare
                             Match Interrupt is executed.

                             • Bit 0 – TOV0: Timer/Counter0 Overflow Flag
                             The bit TOV0 is set (one) when an overflow occurs in Timer/Counter0. TOV0 is cleared
                             by hardware when executing the corresponding interrupt handling vector. Alternatively,
                             TOV0 is cleared by writing a logic one to the flag. When the SREG I-bit, TOIE0
                             (Timer/Counter0 Overflow Interrupt Enable), and TOV0 are set (one), the
                             Timer/Counter0 Overflow interrupt is executed. In phase correct PWM mode, this bit is
                             set when Timer/Counter0 changes counting direction at $00.
*/

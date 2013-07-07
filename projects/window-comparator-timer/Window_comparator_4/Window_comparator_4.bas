
' window comparator program, for switching solar inverter
' includes variable thresholds depending on "net" tariff (clock)
' version 1.0e, 2013-07-03
' changed ADC filtering to average 1sec of samples (750).
' changed LOW threshold during DAY "low" tariff
'
' PB0 - relay / LED - active HIGH
' PB1 - test LED -> winter/summer switch
' PB2 - (ADC 1) - voltage divider
' PB3 - XTAL
' PB4 - XTAL
' PB5 - RESET
'
' timer - 12MHz /8 (clkdiv), /8 (prescale), /250 (count) = 750... Hz not natural number else :(
'
Const Devel_board = 0                                       ' 0 - production, 1 - dev - inverse IO levels
'
Const Is_crystal = 1                                        ' 0 - 8MHz RC, 1 - 12MHz Xtal
'
Const Rc_freq = 7950000 / 8                                 ' from table, 5V, 20degC.
Const Crystal_freq = 12000000 / 8                           'crystal

Const Timer_0_count = 250

$crystal = Crystal_freq
Const Timer0_prescale = 8
Const Timer_0_freq =(_xtal /(timer0_prescale * Timer_0_count))       ' for crystal - 750

Const Timer_0_10_duty = Timer_0_freq / 10
Const Timer_0_90_duty = Timer_0_freq / 10 * 9

Dim Herce As Word , 1s_flag As Bit , Day_low As Bit
Dim Maly_timer1 As Word , Tariff As Bit , Season As Bit
Dim Maly_timer2 As Word , Stan As Byte , Adc_rdy_flag As Bit , Voltage As Word
Dim Voltage_tmp As Word
Dim Auto_continue As Bit , Tmp As Byte , Tmp2 As Word , Tmp3 As Word
'Dim Adc_filter(4) As Word
Dim Adc_flt_pointer As Word
Dim Adc_flt_sum As Long
Dim Sekunda As Byte , Minuta As Byte , Godzina As Byte
'
Const V_thr_high_1 = 767                                    ' 25V
'Const V_thr_high_1 = 799                                    ' 26V - corrected
Const V_thr_high_2 = 850                                    ' 27.8V

Const V_thr_low_1 = 707                                     ' 23V - corrected
Const V_thr_low_2 = 739                                     ' 24V
Const V_thr_low_3 = 767                                     ' 25V -> DAY "low" tariff

Const Voltage_init =(v_thr_high_1 - V_thr_low_2) / 2
Voltage = Voltage_init                                      ' init at 24.5V

'----------------------------------------------------------------------------------------
Godzina = 15
Minuta = 00

'----------------------------------------------------------------------------------------
Relay_out Alias Portb.0
Test_out Alias Portb.1
Voltage_in Alias Portb.2

'----------------------------------------------------------------------------------------
' Herce = 0

Adc_flt_pointer = 0

Config Relay_out = Output
Config Test_out = Output

'--------------------------------------------------------------------------
'
Timer0 = 0
' remember 0! (advice from AVR Freaks)
Compare0a = Timer_0_count - 1
Config Timer0 = Timer , Prescale = Timer0_prescale , Clear Timer = 1
Start Timer0
On Compare0a Timer_0_int:
Enable Compare0a
'
'--------------------------------------------------------------------------
'
' with free program hangs (probably in interrupt), must look at assembler code
Config Adc = Single , Prescaler = Auto , Reference = Avcc

Set Adc1d                                                   ' ADC1 digital input inhibit
Set Admux.0                                                 ' channel 1

Enable Adc

On Adc Adc_int:

'--------------------------------------------------------------------------

Enable Interrupts

'**************************************************************************
Petla1:

         Power Idle

         If 1s_flag = 0 Then
            Goto Petla1:
         End If

         If 1s_flag = 1 Then
            Reset 1s_flag
         End If

Goto Petla1

'**************************************************************************
'
' ADC interrupt routine
Adc_int:

            Tmp = Adcl
            Tmp2 = 256 * Adch
            Tmp2 = Tmp2 + Tmp

            Voltage_tmp = Tmp2
            Set Adc_rdy_flag
Return

'-------------------------------------------------------------------------
' timer interrupt routine,
'
Timer_0_int:

' average Voltage over 1sec period
         If Adc_rdy_flag = 1 Then
            Adc_flt_sum = Adc_flt_sum + Voltage_tmp
            Adc_flt_pointer = Adc_flt_pointer + 1
            Reset Adc_rdy_flag
         End If
         If Adc_flt_pointer = Timer_0_freq Then
             Adc_flt_pointer = 0
            Adc_flt_sum = Adc_flt_sum / Timer_0_freq
            Voltage = Adc_flt_sum
            Adc_flt_sum = 0
         End If

' "START ADC"
$asm
         in r16, &h06                                       ' ADCSRA
         ori r16, &h40                                      ' ADSC
         Out &H06 , R16
$end Asm

         Herce = Herce + 1
         If Herce = Timer_0_freq Then
            Herce = 0
            Set 1s_flag
            Sekunda = Sekunda + 1

'            Adc_flt_pointer = 0
'            Voltage = Adc_flt_sum / Timer_0_freq
'            Adc_flt_sum = 0
         End If

'-------------------------------------------------------------------------
' real time clock
         If Sekunda = 60 Then
            Sekunda = 0
            Minuta = Minuta + 1
            If Minuta = 60 Then
               Minuta = 0
               Godzina = Godzina + 1
               If Godzina = 24 Then
                       Godzina = 0
               End If
            End If
         End If

'-------------------------------------------------------------------------
' setting tariff according to clock

Reset Tariff
Reset Day_low

If Season = 0 Then                                          ' summer
      If Godzina >= 15 Then                                 ' II tariff (cheap) - 15-17 and 22-06
         If Godzina < 17 Then
            Set Tariff
            Set Day_low
         End If
      End If
Else                                                        ' winter
      If Godzina >= 13 Then
         If Godzina < 15 Then
            Set Tariff
            Set Day_low
         End If
      End If
End If

If Godzina >= 22 Then
    Set Tariff
End If
If Godzina < 6 Then
    Set Tariff
End If

'-------------------------------------------------------------------------
' timer test - LED on when on cheap
         If Tariff = 1 Then
            If Herce > Timer_0_10_duty Then
               Set Test_out
            Else
               Reset Test_out
           End If
         Else
            If Herce > Timer_0_10_duty Then
               Reset Test_out
            Else
               Set Test_out
            End If
         End If

'-------------------------------------------------------------------------
'main control
         If Relay_out = 0 Then                              'switched OFF, on@
            If Tariff = 0 Then
               If Voltage > V_thr_high_1 Then               ' 25V
                  Set Relay_out
               End If
            Else
               If Voltage > V_thr_high_2 Then               ' 27V
                  Set Relay_out
               End If
            End If
         Else                                               'switched ON, off@
            If Tariff = 0 Then
               If Voltage < V_thr_low_1 Then                ' 23V
                  Reset Relay_out
               End If
            Else
               If Day_low = 0 Then
                  If Voltage < V_thr_low_2 Then             ' 24V
                     Reset Relay_out
                  End If
               Else
                  If Voltage < V_thr_low_3 Then             ' 25V
                     Reset Relay_out
                  End If
               End If
            End If
         End If

Return

'-------------------------------------------------------------------------
' END *
'-------------------------------------------------------------------------
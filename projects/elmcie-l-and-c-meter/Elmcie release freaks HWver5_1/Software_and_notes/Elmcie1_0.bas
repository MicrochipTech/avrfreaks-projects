'###################################
'######## Elmcie1_0 release ########
'###################################

'Elmcie is a stand-alone inductance and capacitance measurement device
'Its name is a tribute to the original designs/designers

'Elmcie is based on Elsie (or Elcie), as showed here:
'http://www.amqrp.org/kits/elsie/
'but it can be found on several other webpages as well.
'With thanks to the (unknown) original designer

'The second part of Elmcie is based on the capacitance-meter as created by Elm-Chan:
'http://elm-chan.org/works/cmc/report.html

'All rights are reserved for the genuine designers

'I merged both methods into the Elmcie design:
'Elsie's mechanism is great for small capacitances and inductors,
'where Elm-Chan's capacitance meter is more suitable for large capacitors,
'specifically electrolytes, upto 2200 uF.


'Implementation of algorythms are all genuine Plonswerk :-)
'This software is published under GNU General Public License (enclosed in the package)

'Plons on AVRfreaks.net (Nard Awater), August 27, 2007


'  Copyright (C) 2007 Nard Awater <elmcie@aplomb.nl>.

'  This program is free software; you can redistribute it and/or
'  modify it under the terms of the GNU General Public License
'  as published by the Free Software Foundation; either version 2
'  of the License, or (at your option) any later version.
'
'  This program is distributed in the hope that it will be useful,
'  but WITHOUT ANY WARRANTY; without even the implied warranty of
'  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
'  GNU General Public License for more details.
'
'  You should have received a copy of the GNU General Public License
'  along with this program; if not, write to the Free Software Foundation,
'  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

')

'Notes:

'Set the Bascom-compiler Optimize flag in Options - Compiler - Chip - Output-tab

'Bascom generated fuse- and lockbit-setting. Uncomment to use it.
'$prog &HFF , &H3F , &HD1 , &H00

'Or in words:

'Set fuses for 16 MHz external crystal
'Preserve EEprom when chip is erased
'Set BOD-level to 4 Volt, and enable it
'Set Reset-vector to $0000

'Calibration-values are stored in - and retrieved from EEprom
'A virgin AVR has all cells set to $FF
'For first time run: check the comments at label Main:


$baud = 38400
$crystal = 16000000
$regfile = "m8def.dat"

'Elm's cal cap : Ossi's 100 nF large red Wima CalCap
Const C_cal1 = 97449                                        'in pF's
'Elcie's cal cap : Ossi's 2000pF 1% silver-mica CalCap
Const C_cal2 = 19968                                        'in 0.1 pF's !!!

Const Factor2 = 2533                                        '10^5/4*pi*pi

Dim Tmp_byte As Byte
Dim Tmp_word As Word
Dim Tmp_long As Long
Dim Tmp_single As Single

Dim Cnt_4ms As Byte

'Here we do the fancy trick again: overlay !
'Used for Elsie and Elm Low treshold values
Dim Cnts1_b1 As Byte
Dim Cnts1_b2 As Byte
Dim Cnts1_b3 As Byte
Dim Cnts1_b4 As Byte
Dim Cnts1_long As Long At Cnts1_b1 Overlay

'Used for Elm High treshold values
Dim Cnts2_b1 As Byte
Dim Cnts2_b2 As Byte
Dim Cnts2_b3 As Byte
Dim Cnts2_b4 As Byte
Dim Cnts2_long As Long At Cnts2_b1 Overlay

Dim Elm_cnt As Long

'Elm Calibration values in counts / pF
Dim Elm_high_range_cal As Single
Dim Ee_elm_high_range_cal As Eram Single
Dim Elm_low_range_cal As Single
Dim Ee_elm_low_range_cal As Eram Single

Dim Status As Byte
Dim Status_copy As Byte

Elm_selected Alias Status.7
Elm_large_caps Alias Status.6
Do_nulling Alias Status.5
Do_calibration Alias Status.4
Elcie_cap Alias Status.3
Elm_missed_it Alias Status.2
Elm_timeout Alias Status.1
Elm_ready Alias Status.0

Dim Elm_null_cnts As Word
Dim Ee_elm_null_cnts As Eram Word
Dim Elm_result As Single

Dim Teh_result As Single
Dim Teh_result_str As String * 16
Dim Unit_str As String * 16
Dim Empty_line_str As String * 16

Dim Elcie_f_null As Long
Dim Ee_elcie_f_null As Eram Long
Dim Elcie_f_dut As Long

Dim L_elcie_dut As Single
Dim C_elcie_dut As Single
Dim C_elcie_null As Single
Dim Ee_c_elcie_null As Eram Single
Dim L_elcie_null As Single
Dim Ee_l_elcie_null As Eram Single

Sw1_n Alias Pinc.3
Sw2_n Alias Pinc.2
Backlight_hi Alias Portb.1
Spkr Alias Portc.4
Do_l Alias Portb.0
Elm_disch Alias Ddrd.6
Elm_low_treshold Alias Ddrd.4
N_elm3k3_lm311on Alias Portd.3
Power_hold Alias Portd.2
Sw3_null Alias Pind.2

'setup i/o
Ddrc = &B00010011
Portc = &B00001100

Ddrb = &B00111111
Portb = &B00000000

'Pull -up On D.2 Enabled - - > Holds Power After Power -up
Ddrd = &B01011010
Portd = &B00000100


Config Lcdpin = Pin , Db4 = Portb.2 , Db5 = Portb.3 , Db6 = Portb.4 , Db7 = Portb.5 , E = Portc.0 , Rs = Portc.1
Config Lcd = 16 * 2

Config Adc = Single , Prescaler = Auto , Reference = Internal
Start Adc


'Initialize

'Timer2 is the time-base for Elcie, and is used as time-out for Elm
'setup to generate a compare int every 4 / 16 ms
'Setup CTC, OCR2 is top
Tccr2 = &B00001000                                          'CTC mode
Ocr2 = 249
On Oc2 Ms_int Nosave                                        '4 / 16 ms int handler
Enable Oc2
'For 1 second timing we use 4 ms interrupts:
'To start timer2, tccr2=tccr2 or &B00000110
'To stop Timer2,  tccr2=tccr2 and &B11111001
'For 4 second timing we use 16 ms interrupts:
'To start timer2, tccr2=tccr2 or &B00000111
'To stop Timer2,  tccr2=tccr2 and &B11111000

'Timer 1
'Elsie: counts the pulses from the LM311 on T1
'Tccr1a=0  'for reference only
'Tccr1b=0
'tccr1b=7 starts the counting of pulses on T1

'Elm:
'set tccr1b.cs10 to start counting with I/O-clock (16 MHz)

On Timer1 Timer1_ovf Nosave                                 'this int routine takes care of overflows
Enable Ovf1

'Analog Comparator used in Elm
On Aci Treshold_passed Nosave

Enable Interrupts

Empty_line_str = "                "

'###################################################################################
'Main:


'**** Uncomment these lines once calibration has been done
Elm_high_range_cal = Ee_elm_high_range_cal
Elm_low_range_cal = Ee_elm_low_range_cal
C_elcie_null = Ee_c_elcie_null
L_elcie_null = Ee_l_elcie_null
Elcie_f_null = Ee_elcie_f_null

'**** Comment-out these lines once calibration has been done
'Elm_high_range_cal = 0.028789
'Elm_low_range_cal = 27.334
'C_elcie_null = 10184                                        'in 0.1 pF's
'L_elcie_null = 84.8118                                      'in uH
'Elcie_f_null = 541548

Elm_null_cnts = Ee_elm_null_cnts                            'restore from eeprom

'Wake up call
Gosub Do_okay_sound

Cls
Cursor Off Noblink

Lcd "ElmCie rev 1_0"
Home Lower


'Battery check
'Under full-load, the minimum allowed batteryvoltage is 7.35V = 490 ADC counts
'Less means that the 7805 will drop out of regulation

'set for min. current consumption
Set N_elm3k3_lm311on
Reset Backlight_hi
Reset Do_l

Tmp_word = Getadc(5)                                        'dummy read as it's the first
Waitms 1
Tmp_word = Getadc(5)                                        'ADC5= 0.18 * Ubatt

If Tmp_word < 490 Then
   Sound Spkr , 75 , 3000
   Lcd "Battery too LOW"
Else
   'max. current
   Reset N_elm3k3_lm311on
   Set Backlight_hi
   Set Do_l
   Waitms 200
   Tmp_word = Getadc(5)                                     'we want to check the battery level under max. current draw
   'restore min. current consumption
   Set N_elm3k3_lm311on
   Reset Backlight_hi
   Reset Do_l
   Tmp_word = Tmp_word - 450                                'to get a nice readout like a score in school
   Shift Tmp_word , Right , 4                               'divide by 16
   Lcd "Battery Q = " ; Tmp_word
End If

Wait 2

'Default selected
Set Elm_selected

'Default select for Elcie
Set Elcie_cap

Cls
Cursor Off Noblink

Do

   'This block handles the buttons.
   'All it does is setting status-bits according to the required function

   'Switch off request by user
   If Sw1_n = 0 And Sw2_n = 0 And Sw3_null = 0 Then
      Sound Spkr , 25 , 5000
      Waitms 20
      Sound Spkr , 100 , 5000
      Power_hold = 0                                        '.... and go in power down
   End If

   'Nulling or Calibration request by user
   If Sw3_null = 1 Then
      If Sw2_n = 1 Then
         Set Do_nulling
      Else
         Set Do_calibration
      End If

   End If

   'left button only
   If Sw1_n = 0 And Sw2_n = 1 And Sw3_null = 0 Then         'toggle between Elm and Elcie-inductive
      If Elm_selected = 1 Then
         Elm_selected = 0
         Elcie_cap = 0
      Else
         If Elcie_cap = 1 Then                              'when in Elcie-mode, switch to inductance measurement
            Elcie_cap = 0
         Else
            Elm_selected = 1                                'otherwise switch back to Elm-mode
            Elcie_cap = 1
         End If
      End If
      Gosub Do_okay_sound
      Waitms 500
   End If

   'Center button only
   If Sw1_n = 1 And Sw2_n = 0 And Sw3_null = 0 Then         'toggle between Elm and Elcie-capacitive
      If Elm_selected = 1 Then
         Elm_selected = 0
         Elcie_cap = 1
      Else
         If Elcie_cap = 0 Then                              'when in Elcie-mode, switch to capacitive measurement
            Elcie_cap = 1
         Else
            Elm_selected = 1                                'otherwise switch back to Elm-mode
            Elcie_cap = 1
         End If
      End If
      Gosub Do_okay_sound
      Waitms 500
   End If

   'Activate the Elcie-relay for inductance if necessary
   If Elm_selected = 0 And Elcie_cap = 0 Then
      Set Do_l
   Else
      Reset Do_l
   End If

'---------------------------------------------------------------------------------------------

'Elm-mode

   If Elm_selected = 1 Then

      'User wants zero picofarad on the display :-)
      If Do_nulling = 1 Then
         Cls
         Gosub Do_command_sound
         Lcd "Remove DUT"
         Home Lower
         Lcd "Press Null"
         While Sw3_null = 1
         Wend
         While Sw3_null = 0
         Wend
         Gosub Do_command_sound
         Waitms 200
         Cls
         Lcd "Nulling .."
         Elm_null_cnts = 0
         Home Lower
         For Tmp_byte = 1 To 8
            Gosub Do_elm
            Elm_cnt = Cnts1_long - Cnts2_long
            Elm_null_cnts = Elm_null_cnts + Elm_cnt
            Lcd ".";
            Waitms 200
         Next Tmp_byte
         Shift Elm_null_cnts , Right , 3                    'divide by 8
         If Elm_null_cnts > 1400 Or Elm_null_cnts < 800 Then
            Gosub Do_error_sound_and_display
            Elm_null_cnts = Ee_elm_null_cnts                'get latest OK-known value from EEprom
         Else
            Ee_elm_null_cnts = Elm_null_cnts                'and store in EEprom
            Gosub Do_okay_sound
            Lcd "  done"
         End If
         Reset Do_nulling
         Wait 1
         Cls
      End If

      If Do_calibration = 1 Then
         Teh_result_str = "Connect Cal_Elm"
         Gosub Do_cal_display

         Cls
         Tmp_long = 0
         For Tmp_byte = 1 To 8
            Gosub Do_elm
            Elm_cnt = Cnts1_long - Cnts2_long
            Elm_cnt = Elm_cnt - Elm_null_cnts
            Tmp_long = Tmp_long + Elm_cnt
            Lcd ".";
            Waitms 200
         Next Tmp_byte
         Shift Tmp_long , Right , 3                         'divide by 8
         Elm_low_range_cal = Tmp_long / C_cal1
         If Elm_low_range_cal < 25 Or Elm_low_range_cal > 29 Then
            Gosub Do_error_sound_and_display
            Elm_low_range_cal = Ee_elm_low_range_cal        'get latest OK-known value from EEprom
         Else
            Ee_elm_low_range_cal = Elm_low_range_cal        'and store the result
            Gosub Do_okay_sound
            Lcd "low & ."
         End If

         Home Lower
         Set Elm_large_caps                                 'second cal.phase: setup for large cap's (3k3 pull-up)
         Tmp_long = 0
         For Tmp_byte = 1 To 8
            Gosub Do_elm
            Elm_cnt = Cnts1_long - Cnts2_long
            Tmp_long = Tmp_long + Elm_cnt
            Lcd ".";
            Waitms 250
         Next Tmp_byte
         Shift Tmp_long , Right , 3                         'divide by 8
         Elm_high_range_cal = Tmp_long / C_cal1
         If Elm_high_range_cal < 0.024 Or Elm_high_range_cal > 0.028 Then
            Gosub Do_error_sound_and_display
            Elm_high_range_cal = Ee_elm_high_range_cal      'get latest OK-known value from EEprom
         Else
            Ee_elm_high_range_cal = Elm_high_range_cal      'and store the result
            Gosub Do_okay_sound
            Lcd "high OK"
         End If
         Reset Elm_large_caps
         Reset Do_calibration
         Wait 1
         Cls
      End If

      Gosub Do_elm

      If Elm_timeout = 0 Then

         Elm_cnt = Cnts1_long - Cnts2_long                  'calculate the number of clocks between low treshold and high treshold

         If Elm_large_caps = 0 Then
            'subtract null-counts in low range
            Elm_cnt = Elm_cnt - Elm_null_cnts
            Elm_result = Elm_cnt / Elm_low_range_cal        'from counts to picoFarads
         Else
            'no null-counts in high range
            Elm_result = Elm_cnt / Elm_high_range_cal       'from counts to picoFarads
            'if the counts are very low when in high range mode, switch back to low range mode
            If Elm_cnt < 30000 Then
               Elm_large_caps = 0
            End If
         End If

         Select Case Elm_result
            Case Is < 1000
               Teh_result = Elm_result
               Unit_str = " pF"
            Case 1000 To 400000
               Teh_result = Elm_result / 1000
               Unit_str = " nF"
            Case Is > 400000
               Teh_result = Elm_result / 1000000
               Unit_str = " uF"
         End Select

      Else
         'timeout is set so let's see if the high range mode suits better
         If Elm_large_caps = 0 Then
            Elm_large_caps = 1
         Else
            'timeout in spite of the high range mode .... so sorry, out of range
            'upto 2200 uF is possible .... not bad at all !
            Gosub Do_error_sound_and_display
            Teh_result = -10000
            Unit_str = ""
            Elm_large_caps = 0                              'may look silly, but we want to retry with all options open
            Waitms 200
         End If

      End If

   Else

'Elcie-mode

      Reset N_elm3k3_lm311on                                'Turn on the LM311

      'User wants zero picofarad / zero micro-henry resp. on the display :)

      If Do_nulling = 1 Then
         Cls
         Gosub Do_command_sound
         If Elcie_cap = 1 Then
            Lcd "Remove DUT"
         Else
            Lcd "Bridge DUT"
         End If
         Home Lower
         Lcd "Press Null"
         While Sw3_null = 1
         Wend
         While Sw3_null = 0
         Wend
         Gosub Do_command_sound
         Waitms 200
         Cls
         Lcd "Nulling .."
         Home Lower
         Elcie_f_null = 0
         For Tmp_byte = 1 To 4
            Gosub Do_elcie
            Elcie_f_null = Elcie_f_null + Cnts1_long
            Lcd "..";
            Waitms 100
         Next Tmp_byte
         Shift Elcie_f_null , Right , 2                     'divide by 4
         Reset Do_nulling
         If Elcie_f_null > 550000 Or Elcie_f_null < 530000 Then
            Gosub Do_error_sound_and_display
            Elcie_f_null = Ee_elcie_f_null                  'get latest OK-known value from EEprom
         Else
            Ee_elcie_f_null = Elcie_f_null                  'store
            Gosub Do_okay_sound
         End If
         Wait 1
         Cls
      End If


      If Do_calibration = 1 Then
         Teh_result_str = "Conn. Cal_Elcie"
         Gosub Do_cal_display
         Cls
         Elcie_f_dut = 0
         Lcd "Calibrating .. "
         Home Lower
         For Tmp_byte = 1 To 4
            Gosub Do_elcie
            Elcie_f_dut = Elcie_f_dut + Cnts1_long
            Lcd "..";
            Waitms 100
         Next Tmp_byte
         Shift Elcie_f_dut , Right , 2                      'divide by 4

         'and now the math ...
         'We have Elcie_f_null, Elcie_f_dut (= Elcie_f_cal),
         'and we know the calibrated value of C_cal2
         'teh_term is an equasion we need more often, so it is put in a sub
         Gosub Teh_term
         Tmp_long = 1e9 / Elcie_f_null
         Tmp_long = Tmp_long * Tmp_long
         L_elcie_null = Tmp_single * Tmp_long
         L_elcie_null = L_elcie_null * Factor2              'Factor2 =10^5/4pi^2
         L_elcie_null = L_elcie_null / C_cal2
         L_elcie_null = L_elcie_null / 10000                'correction for units
         Tmp_byte = L_elcie_null
         If Tmp_byte > 87 Or Tmp_byte < 81 Then
            Gosub Do_error_sound_and_display
            L_elcie_null = Ee_l_elcie_null                  'get latest OK-known value from EEprom
         Else
            Ee_l_elcie_null = L_elcie_null                  'and store
            Gosub Do_okay_sound
         End If
         C_elcie_null = C_cal2 / Tmp_single
         C_elcie_null = C_elcie_null / 10
         Tmp_word = C_elcie_null
         If Tmp_word > 1036 Or Tmp_word < 1000 Then
            Gosub Do_error_sound_and_display
            C_elcie_null = Ee_c_elcie_null                  'get latest OK-known value from EEprom
         Else
            Ee_c_elcie_null = C_elcie_null                  'and store
            Gosub Do_okay_sound
            Lcd "  done"
         End If
         Reset Do_calibration
         Wait 1
         Cls

      End If

      Gosub Do_elcie

      If Elcie_cap = 1 Then                                 'capacitive
         Elcie_f_dut = Cnts1_long
         Gosub Teh_term
         C_elcie_dut = Tmp_single * C_elcie_null

         Select Case C_elcie_dut
            Case Is < 1000
               Teh_result = C_elcie_dut
               Unit_str = " pF"
            Case Is > 1000
               Teh_result = C_elcie_dut / 1000
               Unit_str = " nF"
         End Select

      Else                                                  'inductive

         If Cnts1_long < 10 Then                            'when there is no Ldut connected, display questionmarks
            Teh_result = -10000
            Unit_str = ""
         Else
            Elcie_f_dut = Cnts1_long
            Gosub Teh_term
            L_elcie_dut = Tmp_single * L_elcie_null
            Select Case L_elcie_dut
               Case Is < 1000
                  Teh_result = L_elcie_dut
                  Unit_str = " uH"
               Case Is > 1000
                  Teh_result = L_elcie_dut / 1000
                  Unit_str = " mH"
            End Select
         End If

      End If

   End If

   Gosub Do_display

Loop


End

'The End _ The End _ The End _ The End _ The End _ The End _ The End _ The End _ The End _ The End _


'####################################   Sub's      #####################################

Do_cal_display:

   Cls
   Gosub Do_command_sound
   Lcd "Calibration"
   Home Lower
   Lcd "release buttons"
   While Sw3_null = 1 Or Sw2_n = 0                          'wait for button release
   Wend
   Waitms 200
   Cls
   Gosub Do_command_sound
   Lcd Teh_result_str                                       'holds the instruction
   Home Lower
   Lcd "and press Null"
   While Sw3_null = 0                                       'wait for Null-button pressed
   Wend
   Waitms 200

   'Note:
   'No return here .... it will bleep the Okay-sound, and will return on that sub's return

Do_okay_sound:

   Sound Spkr , 10 , 5000
   Waitms 100
   Sound Spkr , 30 , 2500

   Return


Do_error_sound_and_display:

   Sound Spkr , 100 , 10000
   Lcd " error"
   Wait 1

   Return

Do_command_sound:

   Sound Spkr , 100 , 1000

   Return

Teh_term:

   'This part of the program is needed twice; by making it a sub I freed some flash

   'To prevent an overflow on the square of the freq's, we need to do some tricks
   'For the real math, see end of this listing

   Tmp_long = Elcie_f_null
   Shift Tmp_long , Right , 4                               'tmp_long is a copy of Elcie_f_null/16
   Shift Elcie_f_dut , Right , 4
   'now it's safe to do a square
   Tmp_long = Tmp_long * Tmp_long
   Elcie_f_dut = Elcie_f_dut * Elcie_f_dut
   Tmp_single = Tmp_long / Elcie_f_dut
   Tmp_single = Tmp_single - 1                              'this term we keep as it's needed elsewhere

   Return

Do_display:

   'Enter this sub with the known teh_result and unit_str

   'A different handling for the pF- and uH-range is required
   If Unit_str = " pF" Or Unit_str = " uH" Then
      Teh_result_str = Fusing(teh_result , "#.#")
   Else
      Select Case Teh_result
         Case -10000
            Teh_result_str = "??? ...."
         Case Is < 10
            Teh_result_str = Fusing(teh_result , "#.###")
         Case 10 To 100
            Teh_result_str = Fusing(teh_result , "#.##")
         Case 100 To 1000
            Teh_result_str = Fusing(teh_result , "#.#")
         Case Is > 1000
            Teh_result = Int(teh_result)
            Teh_result_str = Str(teh_result)
      End Select
   End If

   Cls
   Lcd Teh_result_str ; Unit_str

   Locate 1 , 12
   If Elm_selected = 1 Then
      Lcd "Elm" ;
      If Elm_large_caps = 0 Then
         Lcd "Lo"
      Else
         Lcd "Hi"
      End If
   Else
      Lcd "LC" ;
      If Elcie_cap = 0 Then
         Lcd "ind"
      Else
         Lcd "cap"
      End If
   End If

   Waitms 500

   Return



'-------------------------------------    Do Elm   -----------------------------------

Do_elm:

   'Discharge DUT
   Set Elm_low_treshold

   Status_copy = Status                                     'remember the current setting
   Set N_elm3k3_lm311on                                     'disable the pull-up
   While Acsr.aco = 1                                       'by waiting for ACO to go low
      Set Elm_disch                                         'discharge the DUT
      Waitms 10
      Reset Elm_disch                                       'stop the discharge to check
      Waitus 5
   Wend
   Status = Status_copy                                     'restore the current setting
   Set Elm_disch

   'set the pull-up according to the requested range, and add some more to the discharge-time
   'to make sure we are defenitely under the low treshold (important for large cap's)
   If Elm_large_caps = 0 Then
      Set N_elm3k3_lm311on                                  '3M3 pull-up for small caps
      Waitms 10
   Else
      Reset N_elm3k3_lm311on                                '3k3 pull-up for large caps
      Waitms 100
   End If

   ' ACSR
   'Bit 7 – ACD: Analog Comparator Disable
   'Bit 6 – Acbg : Analog Comparator Bandgap Select
   'Bit 5 – ACO: Analog Comparator Output
   'Bit 4 – ACI: Analog Comparator Interrupt Flag
   'Bit 3 – ACIE: Analog Comparator Interrupt Enable
   'Bit 2 – ACIC: Analog Comparator Input Capture Enable
   'Bits 1,0 – ACIS1, ACIS0: Analog Comparator Interrupt Mode Select
   Acsr = &B00011011                                        'positive edge
   Waitms 5
   'This was a hard one to find: for larger caps ( >10 uF),
   'the first treshold was met BEFORE it should
   'The ICP noisecanceller was tested for this purpose, but "No deal"
   'I have to sort that out some day :-(
   Set Acsr.aci                                             're-clear a possible flag

   'Prepare Timer2 to act as timeout
   Cnt_4ms = 250
   Timer2 = 0
   Set Sfior.psr2                                           'clear the pre-scaler of timer2
   'start timer2
   Tccr2 = Tccr2 Or &B00000111                              'higher!! 4 seconds

   'Reset variables
   Reset Elm_ready
   Reset Elm_timeout
   Reset Elm_missed_it
   Cnts1_long = 0
   Cnts2_long = 0
   Timer1 = 0

   'Start Timer1 in Elm-mode
   Set Tccr1b.cs10
   'start the measurement by releasing the discharge
   Reset Elm_disch

   While Elm_ready = 0                                      'wait for measurement to finish or ...
      If Cnt_4ms = 0 Then                                   '... leave this loop if a timeout occured
         Elm_timeout = 1
         Elm_ready = 1
      End If
      If Acsr.aco = 1 Then
         Elm_missed_it = 1
      End If
   Wend

   'stop timer1
   Tccr1b = 0
   'and stop timer2
   Tccr2 = Tccr2 And &B11111000

   'And we return with .....
   'the Elm-statusbits: Ready, Timeout and Missed_it (= missed high treshold)
   'and the counter-values Cnts1_long and Cnts2_long

   Return

'-------------------------------------    End Do Elm   -----------------------------------

Do_elcie:

   Cnt_4ms = 250
   Timer2 = 0
   Cnts1_long = 0
   Timer1 = 0
   Set Sfior.psr2                                           'clear the pre-scaler of timer2
   Tccr2 = Tccr2 Or &B00000110                              'start timer2
   Tccr1b = 7                                               'count external pulses on T1

   'Wait for 1 second int ; interrupt routine stops Timer1 and Timer2 when Cnt_4ms = 0
   While Cnt_4ms <> 0
   Wend
   'Timer1_overflow has been taken care of by the int routine
   'add contents of Timer1 to cnts1_long
   Cnts1_long = Cnts1_long + Timer1

   'And we return with .....
   'Cnts1_long as the frequncy of the LM311

   Return



'###################################     Int's     ########################################

Ms_int:

   push xl
   in  xl,sreg
   push xl

   lds xl,{Cnt_4ms}
   dec xl
   sts {Cnt_4ms},xl

   brne ms_int_exit                                         'if Cnt_4ms=0 then stop the timers HERE !
   ldi xl,0
   !out tccr1b,xl
   in xl,tccr2
   andi xl, &HF9
   !out tccr2,xl

   Ms_int_exit:
   pop xl
   !Out Sreg , Xl
   pop xl

   Return

'----------------------------------------------------------------------------------------------

Timer1_ovf:                                                 'now for 32 bit !

   push xl
   in  xl,sreg
   push xl

   lds xl,{Cnts1_b3}
   inc xl
   sts {Cnts1_b3},xl
   brne Timer1_ovf_exit                                     'done
   lds xl,{Cnts1_b4}
   inc xl
   sts {Cnts1_b4},xl

   Timer1_ovf_exit:
   pop xl
   !Out Sreg , Xl
   pop xl

   Return

'----------------------------------------------------------------------------------------------

Treshold_passed:

   push xl
   in  xl,sreg
   push xl

   'if ddrd.4=1 treshold is low, else we have the high one
   'Can also be done with SBIS or SBIC .... no need for T-bit-use
   in xl,ddrd
   bst xl,4                                                 'ddrd.4
   brtc high_tresh
   cbi ddrd,4                                               'set treshold high asap to prevent a second occurance
   in xl,tcnt1l                                             'the low treshold has been passed
   sts {cnts2_b1},xl                                        'so we store the 32 bit result in cnts2
   in xl,tcnt1h
   sts {cnts2_b2},xl
   lds xl, {Cnts1_b3}
   sts {cnts2_b3},xl
   lds xl, {Cnts1_b4}
   sts {cnts2_b4},xl                                        'cnts2_long holds now the 32 bit result of the first capture
   sbi Acsr,aci                                             'clear a possible flag
   rjmp Treshold_passed_exit

   High_tresh:
   in xl,tcnt1l
   sts {cnts1_b1},xl                                        'so we store the 32 bit result in cnts1
   in xl,tcnt1h
   sts {cnts1_b2},xl                                        'cnts1_long holds now the 32 bit result of the first capture
   lds xl,{status}
   sbr xl,1                                                 '!!! sets bit0
   sts {status},xl
   sbi ddrd,4                                               'to see end of measurement on scope
   cbi acsr,acie                                            'disable further interrupts
   sbi Acsr,aci                                             'clear a possible flag

   Treshold_passed_exit:
   pop xl
   !Out Sreg , Xl
   pop xl

   Return



'##############################   Some Elcie math    #########################################


'
'               / Elcie_f_null ^ 2     \
'Teh_term is = | ----------------- - 1 |
'               \ Elcie_f_dut ^ 2      /
'
'
'
'
'
'The Math
'
'
'                / Elcie_f_null ^ 2     \                      1
'L_elcie_null = | ----------------- - 1 | X --------------------------------------
'                \ Elcie_f_dut ^ 2      /   4 X Pi ^ 2 X C_cal2 X Elcie_f_null ^ 2
'
'
'Note that Elcie_f_dut is used for Elcie_f_cal
'
'
'                      C_cal2
'C_elcie_null = -----------------------
'                Elcie_f_null ^ 2
'                ----------------  -  1
'                Elcie_f_dut ^ 2
'


'############################## The End #####################################################
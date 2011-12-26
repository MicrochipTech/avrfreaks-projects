01_program_title:

'Transmitter V15_0.bas written for BASCOM-AVR V 2.0.7.3 build 001
'by Jan Huygh (jan.huygh@gmail.com) in July 2012


02__program_documentation:

02_01_copyright:

'Copyright (c) 2012 Jan Huygh jan.huygh@gmail.com

'The author would appreciate it if enhancements to this software or it's
'documentation would be communicated to him. Send a mail to jan.huygh@gmail.com

'In an effort to encourage the development of enhancements to this software
'permission is hereby granted, free of charge, to any person obtaining a copy of
'this software, to deal in the software without restriction, including without
'limitation the right to use, copy, modify, merge, publish, distribute,
'sublicense, and/or sell copies of this software, and to permit persons to whom
'the software is furnished to do so if the above copyright notice and this
'permission notice is included in all copies of this software or a substantial
'portion of this software.

'THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
'IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 'MERCHANTABILITY,
'FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
'AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
'LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
'OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
'SOFTWARE.


02_02_version_numbering_system:

'When I start with a completely new program that will be the version 1_0.
'As more functionality is introduced sucessive versions will be named 1_1, 1_2,
'At a certain point in time I will have the first supposed usable version that
'will be version 2_0
'When bug fixes and minor functionality changes are introduced the version
'numbers will be 2_1, 2_2, ...
'When I start developing major functionality changes that version will be
'version 3_0, 3_1, 3_2, ...
'The first usable version of that code will be version 4_0.
'So in summary :
'
'  - Versions starting with an odd number (1,3,5,...) are development versions
'    and not for "production use
'  - Versions starting with an even number (2,4,6,...) are supposed to be usable


02_03_ver15_0_functionalities:

'Version 15_0 :The three channel 9 model transmitter

'Reads a selector switch that selects what model is used

'Displays the name of the current model on the LCD

'  Internally there is support for 20 models but with my selector switch
'  connected to an 8-bit port I can only select 9 models via the selector switch
'  In a later version I plan to support 20 models via a menu button selection.

'Reads the battery voltage and displays it on an LCD.

'Includes a menu system that enables the user to program into the EEPROM (for
'each of the 9 models supported) the values for :

'  - Model_Name

'  - Steer_subtrim
'  - Steer_low_endpoint
'  - Steer_high_endpoint
'  - Steer_exponential
'  - Steer_normal_reverse

'  - Speed_subtrim
'  - Speed_low_endpoint
'  - Speed_high_endpoint
'  - Speed_exponential
'  - Speed_normal_reverse

'  - Ch3_subtrim
'  - CH3_low_endpoint
'  - CH3_high_endpoint
'  - Ch3_exponential
'  - CH3_Norma/reverse

'Reads the position of potentiometers on the transmitter used for :

'  - Steer
'  - Steer Trim
'  - Speed
'  - Speed Trim
'  - Ch3

'Calculates the Ch1 (Steer), Ch2(Speed) and Ch3 pulselenghts based on the
'position of the steering sticks, trims and all EEPROM stored parameters.

'Generates the Pulse Position Modulation signal on the PPM_outup_pin


02_04_changes_over_prev_version:

'Extra channel Ch3
'The menu system has been completely reworked. To me it is much simpler to
'understand and maintain.
'The modal name can now be 16 (instead of 10) characters long
'The model name can no longer contain any character in the ASCII-character set
'     Scrolling trough over 250 possible characters proved too slow in practice
'     The set is now limited to -*/_0 to 9 and A to Z (in total 41 characters)
'Outside the menu system the model name and the battery voltage are only every
'0,5 secconds written to the LCD. This offers better contrast then rewriting
'every 15 ms (as we did till now)


02_05_future_version_planning:

'Ideas for improvements on current version
' - Make the EEPROM initialisation automatically detect if an EEPROM
'   initialisation is needed.
' - In this version we do the calculation for Steer (CH1), Speed (CH2) and CH3
'   in 3 bloks of code. It must be doable to calculate them with one block that
'   gets different parameters as input like a big function.

'Version 16_0 to 18_0 :The three channel 9 model transmitter
'Skipped on purpose to accomodate for eventual changes in the above numbering
'and still be able to start with the first version with a timer as version 20_0

'Version 20_0 : The three channel 9 model transmitter with lap timer
'Same as previous version PLUS
'A timer/lap counter

'Version 22_0 to 28_0
'Skipped on purpose to accomodate for versions that have different functionality
'of the lab counter and still be able to start with the first version with an
'automatic sequence as version 30_0

'Version 30_0
'Same as previous version PLUS
'By pushing a button the user can trigger a sequence of events.
'The purpose of this functionality would be to aim for a perfect combination of
'actions for a specific manouvre.


02_06_using_transmitter_modules:

'If you want to build your own RC transmitter then using a commercial HF module
'such as the Graupner/JR Sender-HF-Modul Part Nr 4057 or the Graupner/JR
'PLL-SYNTHESIZER-Sender HF-Modul Part Nr 4059 is a good idea.
'Another alternative is a 2.4GHz module like the Jeti Duplex TU2 EX
'http://jetimodel.com/index.php?page=product&id=197
'Using such a module you will not have to worry about a transmitter license nor
'will you have to worry about building the HF-portion of the transmitter.
'Such modules take a Pulse Position Modulation signal as input.
'I can buy such a Graupner module for about 75 Euro, the Jeti Module costs me
'about 110 Euro.
'If somebody can tell me where I can buy a comparable module for less please
'send me a mail. jan.huygh@gmail.com


02_07_the_positive_ppm_signal:

'The PPM-signal is a digital 5V/0V signal.
'The line goes low for 0,4ms then high for 1 to 2 ms then low again for 0,4 ms.
'The 1 to 2 ms pulse represents channel-1.
'You can now add channel-2 to channel-8 by bringing the line high for 1 to 2 ms
'and then low again for 0,4 ms for each channel.
'After channel 8 you have to end with a 0,4 ms low. This pulsetrain is repeated
'every 20 ms. Since all the channels might want to transmit a 2 ms pulse and we
'need to separate the channels with a 0,4 ms pulse we can transmit 8 channels
'within 20 ms since 0,4 + 8 * (0,4 + 2) = 19.6 ms.
'You do not need to transmit 8 channels. You can stop after any channel just
'don't forget to end with a 0,4ms low.

'A Jeti module measures the channel-X-pulses from the start (=the falling edge)
'of the 400 µs separation pulse to the end (= the falling edge of the CH-X
'pulse. So it will send pulses to the transmitter that are 400 µS too long if
'you use a standard PPM signal.
'When using a Jeti module you need to correct for that in the PPM renerator.
'Use 64376 instead of 63576
'The advantage of Jeti's implementation is that you only need maximum 2ms for
'each channel instead of 2,4 ms.(20-0,4)/2 = 9,8.


02_08_graupner_hf_module:

'If you put the HF module in front of you with the connector (5 small holes) in
'the lower left corner the connections are as indicated here below.

'  |
'  |  ° PPM           PPM is the Positive Pulse Position Signal
'  |  ° +5V           + 5V is a stabelized 5V tention
'  |  ° +8V           + 8V to this you connect your main battery (12V is OK too)
'  |  ° GND           GND is the ground connection (the - from the battery)
'  |  ° To Antenna
'  |______________


02_09_jeti_duplex_module:

'That can not be simpler :
'  - 5V to 5V
'  - Ground to ground
'  - PPM output from your AVR to PPM input of the module

02_10_table_for_16_bit_timer:

'Prescale      Maximum CPU   Precision       Maximim time    Highest possible
'  Factor    cycles before    [cycles]    before overflow           precision
'           timer overflow                   @16MHz clock        @16MHz clock

'      1            65 536           1           4 096 µs           0,0625 µs
'      8           524 288           8          32 768 µs              0,5 µs
'     64         4 194 304          64         262 144 µs                4 µs
'    256        16 777 216         256       1 048 576 µs               16 µs

02_11_table_for__8_bit_timer:

'Prescale      Maximum CPU   Precision       Maximim time    Highest possible
'  Factor    cycles before    [cycles]    before overflow           precision
'           timer overflow                   @16MHz clock        @16MHz clock

'      1              256            1              16 µs           0,0625 µs
'      8            2 048            8             128 µs              0,5 µs
'     64           16 384           64           1 024 µs                4 µs
'    256           65 536          256           4 096 µs               16 µs
'  1 024          262 144        1 024          16 384 µs               64 µs


02_12_general_setup_of_program:

'I would have liked it if Bascom would have an easy way to bundle files that
'make a project. You could do that by using a lot of $include files and keep
'all the files in one directory. I tried that approach and went back to one
'big file.
'To structure the program and help me find the right portion of the program
'back fast I have included a series of labels. In the Bascom Integrated
'Development Environment (IDE) you can easyly jump to a label by selecting
'the label from a drop down box called "Label"

'In general I try to stick to the program structure outlined here below.
'However it is not so that I follow that outline strickly. e.g. when it makes
'very much sence to define an alias in the hardware initialisation section
'instead of defining the alias in the aliases definition section I will do so.
'I did that in this program for the alis of the PPM-signal-output-pin.

'Program title
'Program documentation
'     including many subtitles like copyright and version information
'Compiler directives
'Harware initialisation
'Aliases definition
'Constants definition
'Dimension variables
'Macro definition
'Declare subroutines
'Initialise variables
'Main
'Interrupt service routines
'Subroutines

'This program has essentially 4 parts
'The main program loop
'     Display the current model name and the battery voltage if we are not in
'     the menu system
'     Calculate the pulselenghts needed for the PPM signal generator
'The Menu_polling_interupt routine
'     When it detects that a menu key has been pressed it passes on to execution
'     to the menu system
'The Pulse Place Modulation signal generator implemented as an irs
'The menu system that is actually a large set of subroutines.

03_compiler_directives:

'We are using a ATmega644P with a 16 MHz external crystal
$regfile "m644Pdef.dat" : $crystal = 16000000
'Set Hardware stack, Software stack and Frame size. Don't use an $include file!
$hwstack = 80 : $swstack = 200 : $framesize = 500


04_hardware_initialisation:

'We use TIMER0 to generate a "Menu Button Pressed ?" polling interrupt
'This code will generate the interrupt every 16 384 µs
Config Timer0 = Timer , Prescale = 1024
On Timer0 Menu_button_isr
Enable Timer0
Stop Timer0 :

'We use TIMER1 to generate the Pulse Place Modulation Signal
Config Timer1 = Timer , Prescale = 8
On Timer1 Ppm_isr
Enable Timer1
Stop Timer1

'Define from what pin the PPM signal is output and set that pin to output
Ppm_output_pin Alias Portc.6
Config Ppm_output_pin = Output

'The menu buttons are connected to PORTD
Config Portd = Input

'We use a 2 lines, 20 characters per line LCD
Config Lcd = 20 * 2
Config Lcdpin = Pin , Db4 = Portc.0 , Db5 = Portc.1 , Db6 = Portc.2 , Db7 = Portc.3 , E = Portc.5 , Rs = Portc.4

'The model selector switch is connected to Port B .
Config Portb = Input

'We use the ADC to measure the position of the steering stick and trims
'We are OK with 8-bit precision that is why prescaler is not auto
'Auto would come up with longer conversion times than with the 64 we use here
Config Adc = Single , Prescaler = 64 , Reference = Avcc : Start Adc


05_aliases_definition:

Get_steerstick_position Alias Getadc(0)
Get_steertrim_position Alias Getadc(1)
Get_speedstick_position Alias Getadc(2)
Get_speedtrim_position Alias Getadc(3)
Get_ch3_position Alias Getadc(5)
Get_battery_voltage Alias Getadc(6)


06_constants_definition:

Const Menu_nothing = 63
Const Menu_up = 47
Const Menu_dwn = 55
Const Menu_lft = 59
Const Menu_rgt = 61

'Define the constants that are linked to the mechanics of the transmitter
Const Steerstick_lowest = 108
Const Steerstick_neutral = 518
Const Steerstick_maximum = 904
Const Steertrim_lowest = 144
Const Steertrim_neutral = 507
Const Steertrim_maximum = 896

Const Speedstick_lowest = 126
Const Speedstick_neutral = 378
Const Speedstick_maximum = 893
Const Speedtrim_lowest = 167
Const Speedtrim_neutral = 510
Const Speedtrim_maximum = 897

Const Ch3_lowest = 10
Const Ch3_neutral = 512
Const Ch3_maximum = 1014

'Define/calculate values that are linked to the mechanics of the transmitter
Const Steerstick_lower_range = Steerstick_neutral - Steerstick_lowest
Const Steerstick_upper_range = Steerstick_maximum - Steerstick_neutral
Const Speedstick_lower_range = Speedstick_neutral - Speedstick_lowest
Const Speedstick_upper_range = Speedstick_maximum - Speedstick_neutral
Const Steertrim_lower_range = Steertrim_neutral - Steertrim_lowest
Const Steertrim_upper_range = Steertrim_maximum - Steertrim_neutral
Const Speedtrim_lower_range = Speedtrim_neutral - Speedtrim_lowest
Const Speedtrim_upper_range = Speedtrim_maximum - Speedtrim_neutral
Const Ch3_lower_range = Ch3_neutral - Ch3_lowest
Const Ch3_upper_range = Ch3_maximum - Ch3_neutral

07_dimension_variables:

'To avoid that general purpose variables get mixed up between different
'interrupt service routines we dedicate a set of general purpose variables
'to a specific interrupt service routine.
'For the menu system we add _menu to the name of variable.
'For the main program we add _main_program to the name of the variable
Dim Current_model_nr As Byte

Dim Model_name(20) As Eram String * 16

Dim Steer_subtrim(20) As Eram Integer
Dim Steer_low_endpoint(20) As Eram Integer
Dim Steer_high_endpoint(20) As Eram Integer
Dim Steer_exponential(20) As Eram Integer
Dim Steer_normal_reverse(20) As Eram Byte

Dim Speed_subtrim(20) As Eram Integer
Dim Speed_low_endpoint(20) As Eram Integer
Dim Speed_high_endpoint(20) As Eram Integer
Dim Speed_exponential(20) As Eram Integer
Dim Speed_normal_reverse(20) As Eram Byte

Dim Ch3_subtrim(20) As Eram Integer
Dim Ch3_low_endpoint(20) As Eram Integer
Dim Ch3_high_endpoint(20) As Eram Integer
Dim Ch3_exponential(20) As Eram Integer
Dim Ch3_normal_reverse(20) As Eram Byte

Dim Menu_position As Word
Dim Menu_button_pressed As Byte
Dim Menu_push_counter As Byte
Dim Menu_previous_button_pressed As Byte
Dim Copy_of_model_name As String * 16
Dim Pos_in_model_name As Byte
Dim Position_in_character_selection As Byte
Dim Possible_characters(41) As String * 1
Dim Position_in_possible_characters As Byte


Dim Battery_voltage As Word
Dim Main_loop_counter As Byte
Dim Ch(3) As Word
Dim S1_main_program As String * 16

Dim I1 As Integer
Dim L1 As Long
Dim L2 As Long
Dim L3 As Long
Dim L4 As Long
Dim Xbp As Integer
Dim Ybp As Integer

Dim B1_menu As Byte
Dim B2_menu As Byte
Dim B3_menu As Byte
Dim B4_menu As Byte
Dim I1_menu As Integer
Dim I2_menu As Integer
Dim S1_menu As String * 7
Dim S2_menu As String * 16
Dim S3_menu As String * 21


'Used only in Timer1 interrupt service routine
Dim Ppm_cntr As Byte
Dim B1_ppm_isr As Byte
Dim W1_ppm_isr As Word


08_macro_definitions:

Macro Macro_read_selector_switch_and_set_current_model_nr
   If Pinb = 1 Then Current_model_nr = 1
   If Pinb = 2 Then Current_model_nr = 2
   If Pinb = 4 Then Current_model_nr = 3
   If Pinb = 8 Then Current_model_nr = 4
   If Pinb = 16 Then Current_model_nr = 5
   If Pinb = 32 Then Current_model_nr = 6
   If Pinb = 64 Then Current_model_nr = 7
   If Pinb = 128 Then Current_model_nr = 8
   If Pinb = 0 Then Current_model_nr = 9
End Macro


09_declare_subroutines:

Declare Sub Menu_system

Declare Sub M_000
Declare Sub M_100
Declare Sub M_110
Declare Sub M_111
Declare Sub M_120
Declare Sub M_121
Declare Sub M_130
Declare Sub M_131
Declare Sub M_140
Declare Sub M_141
Declare Sub M_200
Declare Sub M_210
Declare Sub M_211
Declare Sub M_220
Declare Sub M_221
Declare Sub M_230
Declare Sub M_231
Declare Sub M_240
Declare Sub M_241
Declare Sub M_300
Declare Sub M_310
Declare Sub M_311
Declare Sub M_320
Declare Sub M_321
Declare Sub M_330
Declare Sub M_331
Declare Sub M_340
Declare Sub M_341
Declare Sub M_400
Declare Sub M_410
Declare Sub M_411

Declare Sub Enter_000
Declare Sub Enter_100
Declare Sub Enter_110
Declare Sub Enter_111
Declare Sub Enter_120
Declare Sub Enter_121
Declare Sub Enter_130
Declare Sub Enter_131
Declare Sub Enter_140
Declare Sub Enter_141
Declare Sub Enter_200
Declare Sub Enter_210
Declare Sub Enter_211
Declare Sub Enter_220
Declare Sub Enter_221
Declare Sub Enter_230
Declare Sub Enter_231
Declare Sub Enter_240
Declare Sub Enter_241
Declare Sub Enter_300
Declare Sub Enter_310
Declare Sub Enter_311
Declare Sub Enter_320
Declare Sub Enter_321
Declare Sub Enter_330
Declare Sub Enter_331
Declare Sub Enter_340
Declare Sub Enter_341
Declare Sub Enter_400
Declare Sub Enter_410
Declare Sub Enter_411


10_initialise_variables:

'EEPROM Initialisation
'For B1_menu = 1 To 20
'   S1_main_program = Str(B1_menu)
'   S1_main_program = Format(S1_main_program , "00")
'   S1_main_program = "MODEL_________" + S1_main_program
'   Model_name(B1_menu) = S1_main_program

'   Steer_subtrim(B1_menu) = 0
'   Steer_low_endpoint(B1_menu) = 100
'   Steer_high_endpoint(B1_menu) = 100
'   Steer_exponential(B1_menu) = 0
'   Steer_normal_reverse(B1_menu) = 0

'   Speed_subtrim(B1_menu) = 0
'   Speed_low_endpoint(B1_menu) = 100
'   Speed_high_endpoint(B1_menu) = 100
'   Speed_exponential(B1_menu) = 0
'   Speed_normal_reverse(B1_menu) = 0

'   Ch3_subtrim(B1_menu) = 0
'   Ch3_low_endpoint(B1_menu) = 100
'   Ch3_high_endpoint(B1_menu) = 100
'   Ch3_exponential(B1_menu) = 0
'   Ch3_normal_reverse(B1_menu) = 0
'Next B1_menu

Menu_position = 0
Pos_in_model_name = 1

Possible_characters(1) = " "
For B1_menu = 2 To 14
   B2_menu = 43 + B1_menu
   Possible_characters(b1_menu) = Chr(b2_menu)
Next
For B1_menu = 15 To 40
   B2_menu = 50 + B1_menu
   Possible_characters(b1_menu) = Chr(b2_menu)
Next
Possible_characters(41) = "_"

Ch(1) = 500
Ch(2) = 500
Ch(3) = 500
Main_loop_counter = 0


11__main:


11_01_before_the_big_loop_starts:

Start Timer0
Start Timer1
Enable Interrupts
Cls
Cursor Off


11_02_start_of_main_loop:

Do
   Macro_read_selector_switch_and_set_current_model_nr
   If Menu_position = 0 Then
      If Main_loop_counter = 50 Then
         Main_loop_counter = 0
         S1_main_program = Model_name(current_model_nr)
         Stop Timer0
         Locate 1 , 1 : Lcd S1_main_program
         Start Timer0
         Battery_voltage = Get_battery_voltage
         Battery_voltage = Battery_voltage * 10
         Battery_voltage = Battery_voltage / 70
         S1_main_program = Str(battery_voltage)
         Stop Timer0
         Locate 1 , 18 : Lcd Format(s1_main_program , "0.0")
         Locate 1 , 19 : Lcd "V"
         Start Timer0
      End If
      Incr Main_loop_counter
   End If
   Waitms 10


11_03_calculating_ch1:

'CH(1) is Steer

'Read the value  from the steer stick potentiometer and convert the position
'to -500 to + 500 (with 0 representing neutral).
'     The mechanical construction of the transmitter reduces the movement of the
'     potentiometers of the sticks so we do not read values between 0 and 1024
'     with 512 as the neutral position. Since we want to use the full range with
'     precision we convert the actual values to a value of -500 to 500 with 0 as
'     the neutral position.
'We base that calulation on the constants Steerstick_lowest, Steerstick_neutral
'and Steerstick_maximum that we define at the start of the code.
   L1 = Get_steerstick_position
   If L1 < Steerstick_lowest Then
      L1 = Steerstick_lowest
   End If
   If L1 > Steerstick_maximum Then
      L1 = Steerstick_maximum
   End If
   L1 = L1 - Steerstick_neutral
   L1 = L1 * 500
   If L1 < 0 Then
      L1 = L1 / Steerstick_lower_range
   Else
      L1 = L1 / Steerstick_upper_range
   End If

'At this point in the code the value of L1 is
'- 500 less than or equal to L1 less than or equal to 500  AND
'L1=0 represents the neutral position of the stick

'EXPONENTIAL
'Normally the position of the steer stick converts in a linear way to the value
'of the CH(1) pulse. In the advanced trasmitter systems they offer you the
'possibility to alter the linear conversion to a non linear conversion.
'They will offer a positive exponential curve and a negative exponential curve.
'The positive exponential curve will overreact in the beginning of the curve and
'underreact at the end of the curve. In other words, a small movement arround
'the neutral position results in more change of the pulselenght but on the other
'side near the end point of the steer stick movement you get less change in the
'pulse length for the movement of the steer stick.
'So the total spread remains from -500 to +500 and 0 represents still neutral.
'For the negative exponential we have less effect on the pulse lenght for the
'movements close to the neutral position and more near the end points.
'In RC race boats its is classic to use a negative curve on the steer. It will
'improve the precision you have on keeping a straight line and still enable you
'to take fast sharp curves.
'The system we have used to implement exponetial behaviour is by using a break
'point (defined by XBP and YBP).
'We store in the Steer_exponetial(X) the value that deviates XBP from 250.
'We have decided to limit the delta to -200 to +200. I guarantee that this still
'delivers much more agressive curves than you will ever want to use.
'The code actually breaks the total curve in 3 zones that each have their own
'equantion Y = a * X + b.
'X here is the position of the steer stick. Y is the convered value for the
'stear stick taking the exponential into account.
'We first calculate the coordinates for Xbp and Ybp from Steer_exponential(X)
'for the active model
'Depending on the actual value of X (= the position of the steer stick) we can
'evaluate in what part of curve we are and by consequence what equation is valid

   If L1 > 0 Then
      Xbp = 250 - Steer_exponential(current_model_nr)
      Ybp = 250 + Steer_exponential(current_model_nr)
      If L1 < Xbp Then       ' Y = a1 * X
         L2 = Ybp * L1
         L1 = L2 / Xbp
      Else       ' Y = a2 * X + b2
         L2 = Ybp - 500
         L3 = Xbp - 500
         L4 = L2 * L1
         L4 = L4 / L3
         L4 = L4 + 500
         L2 = L2 * 500
         L2 = L2 / L3
         L1 = L4 - L2
      End If
   Else       'If L1 =< 0
      Xbp = -250 + Steer_exponential(current_model_nr)
      Ybp = -250 - Steer_exponential(current_model_nr)
      If L1 > Xbp Then       'Y = a1 * X
         L2 = Ybp * L1
         L1 = L2 / Xbp
      Else       'Y = a3 * X + b3
         L2 = Ybp + 500
         L3 = Xbp + 500
         L4 = L2 * L1
         L4 = L4 / L3
         L4 = L4 - 500
         L2 = L2 * 500
         L2 = L2 / L3
         L1 = L4 + L2
      End If
   End If       ' L1
'At this point in the code the value of L1 is
'- 500 less than or equal to L1 less than or equal to 500. AND
'L1=0 represents the neutral position of the stick

'TRIM and END POINT
'We start by reading the position of the Steer Trim by reading the value from
'the Steer Trim Potentiometer.
'This time we convert to a value between -200 and +200 thus limiting the
'influnce of the trim to 200/500 = 40%

   L2 = Get_steertrim_position
   If L2 < Steertrim_lowest Then
      L2 = Steertrim_lowest
   End If
   If L2 > Steertrim_maximum Then
      L2 = Steertrim_maximum
   End If
   L2 = L2 - Steertrim_neutral
   L2 = L2 * 200
   If L2 < 0 Then
      L2 = L2 / Steertrim_lower_range
   Else
      L2 = L2 / Steertrim_upper_range
   End If
'At this point in the code the value of L2 is
'-200 less than or equal to L2 less than or equal to 200. AND
'L2=0 represents the neutral position of the trim

'We add the value we got from the Steer trim potentiometer to the value we get
'from the EEPROM for Steer_subtrim
   I1 = Steer_subtrim(current_model_nr)
   L2 = L2 + I1       'L2 = L2 + Steer_subtrim(1) gives a type conversion error
'To avoid that we would get too high value for the trim correction we limit the
'influence to -200 to +200
   If L2 > 200 Then
      L2 = 200
   End If
   If L2 < -200 Then
      L2 = -200
   End If
'If we are in the right side of the Steer stick movement (L1>0)
'  The total range = 500 - the_total_trim_adjustment - the_high_endpoint
'    (so this handles also the end point adjustment)
'If we are in the left side of the Steer stick movement (L1<0)
'  The total range = 500 + the_total_trim_adjustment - the_low_endpoint
'    (so this handles also the end point adjustment)
   If L1 > 0 Then
      L3 = 500 - L2
      I1 = Steer_high_endpoint(current_model_nr)
      L3 = L3 - I1       'L3 = L3 - Steer_low_endpoint(1) gives type conv error
   Else
      L3 = 500 + L2
      I1 = Steer_low_endpoint(current_model_nr)
      L3 = L3 - I1       'L3 = L3 - Steer_low_endpoint(1) gives type conv error
   End If
'The stick position value =
'  [(stick_position * Range ) / 500] + total_trim_adjustment
   L1 = L3 * L1
   L1 = L1 / 500
   L1 = L1 + L2
'At this point in the code the value of L1 is
'- 500 less than or equal to L1 less than or equal to 500. AND
'L1=0 + TOTAL_TRIM_ADJUSTMENT = the neutral position

'We take the Normal/reverse setting into account
   If Steer_normal_reverse(current_model_nr) = 0 Then
      Ch(1) = L1 + 500
   Else
      Ch(1) = 500 - L1
   End If


11_04_calculating_ch2:

'CH(2) is Speed

   L1 = Get_speedstick_position
   If L1 < Speedstick_lowest Then
      L1 = Speedstick_lowest
   End If
   If L1 > Speedstick_maximum Then
      L1 = Speedstick_maximum
   End If
   L1 = L1 - Speedstick_neutral
   L1 = L1 * 500
   If L1 < 0 Then
      L1 = L1 / Speedstick_lower_range
   Else
      L1 = L1 / Speedstick_upper_range
   End If

'EXPONENTIAL
   If L1 > 0 Then
      Xbp = 250 - Speed_exponential(current_model_nr)
      Ybp = 250 + Speed_exponential(current_model_nr)
      If L1 < Xbp Then       ' Y = A1 * X
         L2 = Ybp * L1
         L1 = L2 / Xbp
      Else
         L2 = Ybp - 500
         L3 = Xbp - 500
         L4 = L2 * L1
         L4 = L4 / L3
         L4 = L4 + 500
         L2 = L2 * 500
         L2 = L2 / L3
         L1 = L4 - L2
      End If
   Else       'If L1 =< 0
      Xbp = -250 + Speed_exponential(current_model_nr)
      Ybp = -250 - Speed_exponential(current_model_nr)
      If L1 > Xbp Then       'Y = A1 * X
         L2 = Ybp * L1
         L1 = L2 / Xbp
      Else
         L2 = Ybp + 500
         L3 = Xbp + 500
         L4 = L2 * L1
         L4 = L4 / L3
         L4 = L4 - 500
         L2 = L2 * 500
         L2 = L2 / L3
         L1 = L4 + L2
      End If
   End If       ' L1

'TRIM and END POINT adjustment
   L2 = Get_speedtrim_position
   If L2 < Speedtrim_lowest Then
      L2 = Speedtrim_lowest
   End If
   If L2 > Speedtrim_maximum Then
      L2 = Speedtrim_maximum
   End If
   L2 = L2 - Speedtrim_neutral
   L2 = L2 * 200
   If L2 < 0 Then
      L2 = L2 / Speedtrim_lower_range
   Else
      L2 = L2 / Speedtrim_upper_range
   End If

   I1 = Speed_subtrim(current_model_nr)
   L2 = L2 + I1
   If L2 > 200 Then
      L2 = 200
   End If
   If L2 < -200 Then
      L2 = -200
   End If
   If L1 > 0 Then
      L3 = 500 - L2
      I1 = Speed_high_endpoint(current_model_nr)
      L3 = L3 - I1
   Else
      L3 = 500 + L2
      I1 = Speed_low_endpoint(current_model_nr)
      L3 = L3 - I1
   End If
   I1 = Speed_low_endpoint(current_model_nr)
   L3 = L3 - I1
   L1 = L3 * L1
   L1 = L1 / 500
   L1 = L1 + L2
'At this point in the code the value of L1 is
'- 500 less than or equal to L1 less than or equal to 500. AND
'L1=0 + TOTAL_TRIM_ADJUSTMENT = the neutral position

'Normal/Reverse
   If Speed_normal_reverse(current_model_nr) = 0 Then
      Ch(2) = L1 + 500
   Else
      Ch(2) = 500 - L1
   End If
'At this point in the code the value of CH(x) is
'0 less than or equal to L1 less than or equal to 1000


11_05_calculating_ch3:

'CH(3)is Function3

   L1 = Get_ch3_position
   If L1 < Ch3_lowest Then
      L1 = Ch3_lowest
   End If
   If L1 > Ch3_maximum Then
      L1 = Ch3_maximum
   End If
   L1 = L1 - Ch3_neutral
   L1 = L1 * 500
   If L1 < 0 Then
      L1 = L1 / Ch3_lower_range
   Else
      L1 = L1 / Ch3_upper_range
   End If
'At this point in the code the value of L1 is
'- 500 less than or equal to L1 less than or equal to 500.

'EXPONENTIAL
   If L1 > 0 Then
      Xbp = 250 - Ch3_exponential(current_model_nr)
      Ybp = 250 + Ch3_exponential(current_model_nr)
      If L1 < Xbp Then       ' Y = A1 * X
         L2 = Ybp * L1
         L1 = L2 / Xbp
      Else
         L2 = Ybp - 500
         L3 = Xbp - 500
         L4 = L2 * L1
         L4 = L4 / L3
         L4 = L4 + 500
         L2 = L2 * 500
         L2 = L2 / L3
         L1 = L4 - L2
      End If
   Else       'If L1 =< 0
      Xbp = -250 + Ch3_exponential(current_model_nr)
      Ybp = -250 - Ch3_exponential(current_model_nr)
      If L1 > Xbp Then       'Y = A1 * X
         L2 = Ybp * L1
         L1 = L2 / Xbp
      Else
         L2 = Ybp + 500
         L3 = Xbp + 500
         L4 = L2 * L1
         L4 = L4 / L3
         L4 = L4 - 500
         L2 = L2 * 500
         L2 = L2 / L3
         L1 = L4 + L2
      End If
   End If       ' L1

'TRIM and END POINT adjustment
   L2 = 0       'There is no CH3 trim potmeter,only a subtrim

   I1 = Ch3_subtrim(current_model_nr)
   L2 = L2 + I1
   If L2 > 200 Then
      L2 = 200
   End If
   If L2 < -200 Then
      L2 = -200
   End If
   If L1 > 0 Then
      L3 = 500 - L2
      I1 = Ch3_high_endpoint(current_model_nr)
      L3 = L3 - I1
   Else
      L3 = 500 + L2
      I1 = Ch3_low_endpoint(current_model_nr)
      L3 = L3 - I1
   End If
   L1 = L3 * L1
   L1 = L1 / 500
   L1 = L1 + L2
'At this point in the code the value of L1 is
'- 500 less than or equal to L1 less than or equal to 500. AND
'L1=0 + TOTAL_TRIM_ADJUSTMENT = the neutral position

'Normal/Reverse
   If Ch3_normal_reverse(current_model_nr) = 0 Then
      Ch(3) = L1 + 500
   Else
      Ch(3) = 500 - L1
   End If


Loop


12_interrupt_service_routines:

Menu_button_isr:
'We enebale interrupts to mke sure the Ppm_generator continues even when we
'are in the menu system
Enable Interrupts
Menu_button_pressed = Pind And &H7F
If Menu_button_pressed <> Menu_nothing Then
   If Menu_previous_button_pressed = Menu_button_pressed Then
      Incr Menu_push_counter
      If Menu_push_counter = 1 Then
         Call Menu_system
      Elseif Menu_push_counter = 255 Then
         Menu_push_counter = 254
      End If
   Else
      Menu_previous_button_pressed = Menu_button_pressed
      Menu_push_counter = 0
   End If
Else
   Menu_previous_button_pressed = Menu_nothing
   Menu_push_counter = 0
End If
Return


'*******************************************************************************
'*       The PPM signal generator
'The action to take by the PMM signal generator is based on the value of
'Ppm_progress_counter.
'The Ppm_cntr is incremented when leaving the interrupt service
'routine.
'For the 3 channel version :
'When Ppm_cntr is 1, 3, 5 or 7 the 400 µs low separation pulse
'needs to be transmitted. 'That would mean we need to load the timer with
'65536 - (400 * 2) =  64736. In practice the value delivering 400µS is 64752
'I think this is caused by losing time when pushing all the registers on the
'stack when entering the interrupt service routine.
'When Ppm_cntr is 2,4 or 6 the channel 1, channel 2 or channel 3
'pulse must be transmitted.
'In this code we start by converting the value in CH(X) that is expressed in
'microsecconds to clock ticks (so X 2).
'We now reload the counter with the number needed for a 1000µs pulse + the
'number of clock ticks just calculated
'For a 1000µs pulse we would expect 63536 but we use 63576 (again due to pushing
'registers on stack now plus some code in the isr)
'For a 3 channel version when Ppm_cntr is 8 the timer needs to
'restart after 20-(4X0,4)-(3*1)-(CH(1)+CH(2)+CH(3)) ms.
'At the end of each isr call we increment Ppm_cntr. When it reaches
'9 we reset to 1 to start the next pulse
Ppm_isr:
   If Ppm_cntr = 1 Or Ppm_cntr = 3 Or Ppm_cntr = 5 Or Ppm_cntr = 7 Then
      Reset Ppm_output_pin
      Timer1 = 64752
   Elseif Ppm_cntr = 2 Or Ppm_cntr = 4 Or Ppm_cntr = 6 Then
      Set Ppm_output_pin
      B1_ppm_isr = Ppm_cntr / 2
      W1_ppm_isr = Ch(b1_ppm_isr) * 2
      Timer1 = 64376 - W1_ppm_isr
   Elseif Ppm_cntr = 8 Then
      Set Ppm_output_pin
      W1_ppm_isr = Ch(1) + Ch(2)
      W1_ppm_isr = W1_ppm_isr + Ch(3)
      W1_ppm_isr = W1_ppm_isr * 2 : Timer1 = 29273 + W1_ppm_isr
   End If
   Incr Ppm_cntr
   If Ppm_cntr = 9 Then Ppm_cntr = 1
Return


13_subroutines:

'This is the concept of the menu system
'Each position in the menu system has a number. That number is kept in the
'variable Menu_position.
'A polling routine is called every 16 ms to check if a menu button has been
'pressed.
'When a menu button has been pressed the sub Menu_system is called.
'The sub menu system will call another subroutine depending on the current
'menu poisition. (e.g. it will call the subroutine M_100 if we are in the
'menu position 100.
'That routine will implement what needs to happen when UP DOWN LEFT or RIGHT
'is pushed.
'When entering a new menu position (or when re-entering the current position
'after an update the sub routine Enter_XXX for that menu position will be
'called to update the display.
'Here is an effort to give a graphical representation of this
'
'Main Program
' Do
' -----
' -----   -----> Menu_button_isr
' -----  <----    -----
' -----       |   -----
' -----       |   -----   -----> Sub Menu_system
' Loop        |   -----  <----    -----
'             |   -----       |   -----
'              - Return       |   -----   -----> Sub Menu_XXX
'                             |   -----  <----    -----
'                             |   -----       |   -----
'                              - End Sub      |   -----   -----> Sub Enter_YYY
'                                             |   -----  <----     -----
'                                             |   -----       |    -----
'                                             |   -----       |    -----
'                                              - End Sub       - End Sub
Sub Menu_system
   Select Case Menu_position
   Case 000 : Call M_000

   Case 100 : Call M_100
   Case 110 : Call M_110
   Case 111 : Call M_111
   Case 120 : Call M_120
   Case 121 : Call M_121
   Case 130 : Call M_130
   Case 131 : Call M_131
   Case 140 : Call M_140

   Case 200 : Call M_200
   Case 210 : Call M_210
   Case 211 : Call M_211
   Case 220 : Call M_220
   Case 221 : Call M_221
   Case 230 : Call M_230
   Case 231 : Call M_231
   Case 240 : Call M_240

   Case 300 : Call M_300
   Case 310 : Call M_310
   Case 311 : Call M_311
   Case 320 : Call M_320
   Case 321 : Call M_321
   Case 330 : Call M_330
   Case 331 : Call M_331
   Case 340 : Call M_340

   Case 400 : Call M_400
   Case 410 : Call M_410
   Case 411 : Call M_411
   End Select
End Sub

Sub M_000
   Select Case Menu_button_pressed
   Case Menu_lft
      Cursor Blink
      Call Enter_400
   Case Menu_rgt
      Cursor Blink
      Cursor Blink
      Call Enter_100
   End Select
End Sub

Sub Enter_000
   Menu_position = 000
   Cls
   Cursor Off
   Cursor Noblink
   Main_loop_counter = 50
End Sub

Sub M_100
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_000
   Case Menu_dwn
      Call Enter_110
   Case Menu_lft
      Call Enter_000
   Case Menu_rgt
      Call Enter_200
   End Select
End Sub

Sub Enter_100
   Menu_position = 100
   Cls
   Locate 1 , 1
   Lcd "Menu:Steer"
   Locate 2 , 1
   Lcd "Steer Speed Ch3 Mdl_Name"
   Locate 2 , 1
End Sub

Sub M_110
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_100
   Case Menu_dwn
      Call Enter_111
   Case Menu_lft
      Call Enter_140
   Case Menu_rgt
      Call Enter_120
   End Select
End Sub

Sub Enter_110
   Menu_position = 110
   Cls
   Locate 1 , 1
   Lcd "Menu:Steer Sub_trim"
   Locate 2 , 1
   Lcd "Sub_t End_p Exp N/R"
   Locate 2 , 1
End Sub

Sub M_111
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_110
   Case Menu_lft
      I1_menu = Steer_subtrim(current_model_nr) - 1
      Steer_subtrim(current_model_nr) = I1_menu
      Call Enter_111
   Case Menu_rgt
      I1_menu = Steer_subtrim(current_model_nr) + 1
      Steer_subtrim(current_model_nr) = I1_menu
      Call Enter_111
   End Select
End Sub

Sub Enter_111
   Menu_position = 111
   Cls
   Locate 1 , 1
   Lcd "Menu:Steer Sub_trim"
   Locate 2 , 1
   I1_menu = Steer_subtrim(current_model_nr)
   If Steer_normal_reverse(current_model_nr) = 0 Then
      I2_menu = 1500 + I1_menu
   Else
      I2_menu = 1500 - I1_menu
   End If
   If I1_menu > 0 Then
      Lcd "R"
   Elseif I1_menu = 0 Then
      Lcd "N"
   Else
      Lcd "L"
   End If
   I1_menu = Abs(i1_menu)
   S1_menu = Str(i1_menu)
   Lcd Format(s1_menu , "000")
   Lcd " => "
   S1_menu = Str(i2_menu)
   Lcd Format(s1_menu , "0000")
   Lcd " mS     "
   Locate 2 , 21
End Sub

Sub M_120
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_100
   Case Menu_dwn
      Call Enter_121
   Case Menu_lft
      Call Enter_110
   Case Menu_rgt
      Call Enter_130
   End Select
End Sub

Sub Enter_120
   Menu_position = 120
   Cls
   Locate 1 , 1
   Lcd "Menu:Steer End_point"
   Locate 2 , 1
   Lcd "Sub_t End_p Exp N/R"
   Locate 2 , 7
End Sub

Sub M_121
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_120
   Case Menu_lft
      I1_menu = Ch(1) - 500
      I2_menu = Abs(i1_menu)
      If I2_menu < 300 Then
         I1_menu = Steer_low_endpoint(current_model_nr) + 1
         Steer_low_endpoint(current_model_nr) = I1_menu
         I1_menu = Steer_high_endpoint(current_model_nr) + 1
         Steer_high_endpoint(current_model_nr) = I1_menu
      Elseif I1_menu > 300 Then
         I1_menu = Steer_high_endpoint(current_model_nr) + 1
         Steer_high_endpoint(current_model_nr) = I1_menu
      Elseif I1_menu < -300 Then
         I1_menu = Steer_low_endpoint(current_model_nr) + 1
         Steer_low_endpoint(current_model_nr) = I1_menu
      End If
      Call Enter_121
   Case Menu_rgt
      I1_menu = Ch(1) - 500
      I2_menu = Abs(i1_menu)
      If I2_menu < 300 Then
         I1_menu = Steer_low_endpoint(current_model_nr) - 1
         Steer_low_endpoint(current_model_nr) = I1_menu
         I1_menu = Steer_high_endpoint(current_model_nr) - 1
         Steer_high_endpoint(current_model_nr) = I1_menu
      Elseif I1_menu > 300 Then
         I1_menu = Steer_high_endpoint(current_model_nr) - 1
         Steer_high_endpoint(current_model_nr) = I1_menu
      Elseif I1_menu < -300 Then
         I1_menu = Steer_low_endpoint(current_model_nr) - 1
         Steer_low_endpoint(current_model_nr) = I1_menu
      End If
      Call Enter_121
   End Select
End Sub

Sub Enter_121
   Menu_position = 121
   Cls
   Locate 1 , 1
   Lcd "Menu:Steer End_point"
   Locate 2 , 1
   If Steer_normal_reverse(current_model_nr) = 0 Then
      I1_menu = Steer_low_endpoint(current_model_nr)
      I2_menu = Steer_high_endpoint(current_model_nr)
   Else
      I1_menu = Steer_high_endpoint(current_model_nr)
      I2_menu = Steer_low_endpoint(current_model_nr)
   End If
   Locate 2 , 1
   Lcd "L(-"
   S1_menu = Str(i1_menu)
   Lcd Format(s1_menu , "000")
   Lcd ")      (-"
   S1_menu = Str(i2_menu)
   Lcd Format(s1_menu , "000")
   Lcd ")R"
End Sub

Sub M_130
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_100
   Case Menu_dwn
      Call Enter_131
   Case Menu_lft
      Call Enter_120
   Case Menu_rgt
      Call Enter_140
   End Select
End Sub

Sub Enter_130
   Menu_position = 130
   Cls
   Locate 1 , 1
   Lcd "Menu:Steer Exponent."
   Locate 2 , 1
   Lcd "Sub_t End_p Exp N/R"
   Locate 2 , 13
End Sub

Sub M_131
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_130
   Case Menu_lft
      I1_menu = Steer_exponential(current_model_nr) - 1
      Steer_exponential(current_model_nr) = I1_menu
      Call Enter_131
   Case Menu_rgt
      I1_menu = Steer_exponential(current_model_nr) + 1
      Steer_exponential(current_model_nr) = I1_menu
      Call Enter_131
   End Select
End Sub

Sub Enter_131
   Menu_position = 131
   Cls
   Locate 1 , 1
   Lcd "Menu:Steer Exponent."
   Locate 2 , 1
   I1_menu = Steer_exponential(current_model_nr)
   S1_menu = Str(i1_menu)
   Lcd Format(s1_menu , "+000")
   Locate 2 , 21
End Sub

Sub M_140
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_100
   Case Menu_dwn
      If Steer_normal_reverse(current_model_nr) = 0 Then
         Steer_normal_reverse(current_model_nr) = 1
      Else
         Steer_normal_reverse(current_model_nr) = 0
      End If
      Call Enter_140
   Case Menu_lft
      Call Enter_130
   Case Menu_rgt
      Call Enter_110
   End Select
End Sub

Sub Enter_140
   Menu_position = 140
   Cls
   Locate 1 , 1
   Lcd "Menu:Steer "
   If Steer_normal_reverse(current_model_nr) = 0 Then
      Lcd "Normal"
   Else
      Lcd "Reverse"
   End If
   Locate 2 , 1
   Lcd "Sub_t End_p Exp N/R"
   Locate 2 , 17
End Sub Menu_system

Sub M_200
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_000
   Case Menu_dwn
      Call Enter_210
   Case Menu_lft
      Call Enter_100
   Case Menu_rgt
      Call Enter_300
   End Select
End Sub

Sub Enter_200
   Menu_position = 200
   Cls
   Locate 1 , 1
   Lcd "Menu:Speed"
   Locate 2 , 1
   Lcd "Steer Speed Ch3 Mdl"
   Locate 2 , 7
End Sub

Sub M_210
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_200
   Case Menu_dwn
      Call Enter_211
   Case Menu_lft
      Call Enter_240
   Case Menu_rgt
      Call Enter_220
   End Select
End Sub

Sub Enter_210
   Menu_position = 210
   Cls
   Locate 1 , 1
   Lcd "Menu:Spêed Sub_trim"
   Locate 2 , 1
   Lcd "Subt_t End_p Exp N/R"
   Locate 2 , 1
End Sub

Sub M_211
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_210
   Case Menu_lft
      I1_menu = Speed_subtrim(current_model_nr) - 1
      Speed_subtrim(current_model_nr) = I1_menu
      Call Enter_211
   Case Menu_rgt
      I1_menu = Speed_subtrim(current_model_nr) + 1
      Speed_subtrim(current_model_nr) = I1_menu
      Call Enter_211
   End Select
End Sub

Sub Enter_211
   Menu_position = 211
   Cls
   Locate 1 , 1
   Lcd "M:Speed Sub_trim"
   Locate 2 , 1
   I1_menu = Speed_subtrim(current_model_nr)
   If Speed_normal_reverse(current_model_nr) = 0 Then
      I2_menu = 1500 + I1_menu
   Else
      I2_menu = 1500 - I1_menu
   End If
   If I1_menu > 0 Then
      Lcd "R"
   Elseif I1_menu = 0 Then
      Lcd "N"
   Else
      Lcd "L"
   End If
   I1_menu = Abs(i1_menu)
   S1_menu = Str(i1_menu)
   Lcd Format(s1_menu , "000")
   Lcd " => "
   S1_menu = Str(i2_menu)
   Lcd Format(s1_menu , "0000")
   Lcd " mS     "
   Locate 2 , 21
End Sub

Sub M_220
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_200
   Case Menu_dwn
      Call Enter_221
   Case Menu_lft
      Call Enter_210
   Case Menu_rgt
      Call Enter_230
   End Select
End Sub

Sub Enter_220
   Menu_position = 220
   Cls
   Locate 1 , 1
   Lcd "Menu:Speed End_point"
   Locate 2 , 1
   Lcd "Sub_t End_p Exp N/R"
   Locate 2 , 7
End Sub

Sub M_221
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_220
   Case Menu_lft
      I1_menu = Ch(2) - 500
      I2_menu = Abs(i1_menu)
      If I2_menu < 300 Then
         I1_menu = Speed_low_endpoint(current_model_nr) + 1
         Speed_low_endpoint(current_model_nr) = I1_menu
         I1_menu = Speed_high_endpoint(current_model_nr) + 1
         Speed_high_endpoint(current_model_nr) = I1_menu
      Elseif I1_menu > 300 Then
         I1_menu = Speed_high_endpoint(current_model_nr) + 1
         Speed_high_endpoint(current_model_nr) = I1_menu
      Elseif I1_menu < -300 Then
         I1_menu = Speed_low_endpoint(current_model_nr) + 1
         Speed_low_endpoint(current_model_nr) = I1_menu
      End If
      Call Enter_221
   Case Menu_rgt
      I1_menu = Ch(2) - 500
      I2_menu = Abs(i1_menu)
      If I2_menu < 300 Then
         I1_menu = Speed_low_endpoint(current_model_nr) - 1
         Speed_low_endpoint(current_model_nr) = I1_menu
         I1_menu = Speed_high_endpoint(current_model_nr) - 1
         Speed_high_endpoint(current_model_nr) = I1_menu
      Elseif I1_menu > 300 Then
         I1_menu = Speed_high_endpoint(current_model_nr) - 1
         Speed_high_endpoint(current_model_nr) = I1_menu
      Elseif I1_menu < -300 Then
         I1_menu = Speed_low_endpoint(current_model_nr) - 1
         Speed_low_endpoint(current_model_nr) = I1_menu
      End If
      Call Enter_221
   End Select
End Sub

Sub Enter_221
   Menu_position = 221
   Cls
   Locate 1 , 1
   Lcd "Menu:Speed End_point"
   Locate 2 , 1
   If Speed_normal_reverse(current_model_nr) = 0 Then
      I1_menu = Speed_low_endpoint(current_model_nr)
      I2_menu = Speed_high_endpoint(current_model_nr)
   Else
      I1_menu = Speed_high_endpoint(current_model_nr)
      I2_menu = Speed_low_endpoint(current_model_nr)
   End If
   Locate 2 , 1
   Lcd "L(-"
   S1_menu = Str(i1_menu)
   Lcd Format(s1_menu , "000")
   Lcd ")      (-"
   S1_menu = Str(i2_menu)
   Lcd Format(s1_menu , "000")
   Lcd ")H"
End Sub

Sub M_230
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_200
   Case Menu_dwn
      Call Enter_231
   Case Menu_lft
      Call Enter_220
   Case Menu_rgt
      Call Enter_240
   End Select
End Sub

Sub Enter_230
   Menu_position = 230
   Cls
   Locate 1 , 1
   Lcd "Menu:Speed Exponent."
   Locate 2 , 1
   Lcd "Sub_t End_p Exp N/R"
   Locate 2 , 13
End Sub

Sub M_231
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_230
   Case Menu_lft
      I1_menu = Speed_exponential(current_model_nr) - 1
      Speed_exponential(current_model_nr) = I1_menu
      Call Enter_231
   Case Menu_rgt
      I1_menu = Speed_exponential(current_model_nr) + 1
      Speed_exponential(current_model_nr) = I1_menu
      Call Enter_231
   End Select
End Sub

Sub Enter_231
   Menu_position = 231
   Cls
   Locate 1 , 1
   Lcd "Menu:Speed Exponent."
   Locate 2 , 1
   I1_menu = Speed_exponential(current_model_nr)
   S1_menu = Str(i1_menu)
   Lcd Format(s1_menu , "+000")
   Locate 2 , 21
End Sub

Sub M_240
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_200
   Case Menu_dwn
      If Speed_normal_reverse(current_model_nr) = 0 Then
         Speed_normal_reverse(current_model_nr) = 1
      Else
         Speed_normal_reverse(current_model_nr) = 0
      End If
      Call Enter_240
   Case Menu_lft
      Call Enter_230
   Case Menu_rgt
      Call Enter_210
   End Select
End Sub

Sub Enter_240
   Menu_position = 240
   Cls
   Locate 1 , 1
   Lcd "Menu:Speed "
   If Speed_normal_reverse(current_model_nr) = 0 Then
      Lcd "Normal"
   Else
      Lcd "Reverse"
   End If
   Locate 2 , 1
   Lcd "Sub_t End_p Exp N/R"
   Locate 2 , 17
End Sub Menu_system

Sub M_300
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_000
   Case Menu_dwn
      Call Enter_310
   Case Menu_lft
      Call Enter_200
   Case Menu_rgt
      Call Enter_400
   End Select
End Sub

Sub Enter_300
   Menu_position = 300
   Cls
   Locate 1 , 1
   Lcd "Menu:Ch3"
   Locate 2 , 1
   Lcd "Steer Speed Ch3 Mdl"
   Locate 2 , 13
End Sub

Sub M_310
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_300
   Case Menu_dwn
      Call Enter_311
   Case Menu_lft
      Call Enter_340
   Case Menu_rgt
      Call Enter_320
   End Select
End Sub

Sub Enter_310
   Menu_position = 310
   Cls
   Locate 1 , 1
   Lcd "Menu:Ch3 Sub_trim"
   Locate 2 , 1
   Lcd "Subt_t End_p Exp N/R"
   Locate 2 , 1
End Sub

Sub M_311
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_310
   Case Menu_lft
      I1_menu = Ch3_subtrim(current_model_nr) - 1
      Ch3_subtrim(current_model_nr) = I1_menu
      Call Enter_311
   Case Menu_rgt
      I1_menu = Ch3_subtrim(current_model_nr) + 1
      Ch3_subtrim(current_model_nr) = I1_menu
      Call Enter_311
   End Select
End Sub

Sub Enter_311
   Menu_position = 311
   Cls
   Locate 1 , 1
   Lcd "Menu:Ch3 Sub_trim"
   Locate 2 , 1
   I1_menu = Ch3_subtrim(current_model_nr)
   If Ch3_normal_reverse(current_model_nr) = 0 Then
      I2_menu = 1500 + I1_menu
   Else
      I2_menu = 1500 - I1_menu
   End If
   If I1_menu > 0 Then
      Lcd "R"
   Elseif I1_menu = 0 Then
      Lcd "N"
   Else
      Lcd "L"
   End If
   I1_menu = Abs(i1_menu)
   S1_menu = Str(i1_menu)
   Lcd Format(s1_menu , "000")
   Lcd " => "
   S1_menu = Str(i2_menu)
   Lcd Format(s1_menu , "0000")
   Lcd " mS     "
   Locate 2 , 21
End Sub

Sub M_320
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_300
   Case Menu_dwn
      Call Enter_321
   Case Menu_lft
      Call Enter_310
   Case Menu_rgt
      Call Enter_330
   End Select
End Sub

Sub Enter_320
   Menu_position = 320
   Cls
   Locate 1 , 1
   Lcd "Menu:Ch3 End_point"
   Locate 2 , 1
   Lcd "Sub_t End_p Exp N/R"
   Locate 2 , 7
End Sub

Sub M_321
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_320
   Case Menu_lft
      I1_menu = Ch(3) - 500
      I2_menu = Abs(i1_menu)
      If I2_menu < 300 Then
         I1_menu = Ch3_low_endpoint(current_model_nr) + 1
         Ch3_low_endpoint(current_model_nr) = I1_menu
         I1_menu = Ch3_high_endpoint(current_model_nr) + 1
         Ch3_high_endpoint(current_model_nr) = I1_menu
      Elseif I1_menu > 300 Then
         I1_menu = Ch3_high_endpoint(current_model_nr) + 1
         Ch3_high_endpoint(current_model_nr) = I1_menu
      Elseif I1_menu < -300 Then
         I1_menu = Ch3_low_endpoint(current_model_nr) + 1
         Ch3_low_endpoint(current_model_nr) = I1_menu
      End If
      Call Enter_321
   Case Menu_rgt
      I1_menu = Ch(3) - 500
      I2_menu = Abs(i1_menu)
      If I2_menu < 300 Then
         I1_menu = Ch3_low_endpoint(current_model_nr) - 1
         Ch3_low_endpoint(current_model_nr) = I1_menu
         I1_menu = Ch3_high_endpoint(current_model_nr) - 1
         Ch3_high_endpoint(current_model_nr) = I1_menu
      Elseif I1_menu > 300 Then
         I1_menu = Ch3_high_endpoint(current_model_nr) - 1
         Ch3_high_endpoint(current_model_nr) = I1_menu
      Elseif I1_menu < -300 Then
         I1_menu = Ch3_low_endpoint(current_model_nr) - 1
         Ch3_low_endpoint(current_model_nr) = I1_menu
      End If
      Call Enter_321
   End Select
End Sub

Sub Enter_321
   Menu_position = 321
   Cls
   Locate 1 , 1
   Lcd "Menu:Ch3 End_point"
   Locate 2 , 1
   If Ch3_normal_reverse(current_model_nr) = 0 Then
      I1_menu = Ch3_low_endpoint(current_model_nr)
      I2_menu = Ch3_high_endpoint(current_model_nr)
   Else
      I1_menu = Ch3_high_endpoint(current_model_nr)
      I2_menu = Ch3_low_endpoint(current_model_nr)
   End If
   Locate 2 , 1
   Lcd "L(-"
   S1_menu = Str(i1_menu)
   Lcd Format(s1_menu , "000")
   Lcd ")      (-"
   S1_menu = Str(i2_menu)
   Lcd Format(s1_menu , "000")
   Lcd ")H"
End Sub

Sub M_330
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_300
   Case Menu_dwn
      Call Enter_331
   Case Menu_lft
      Call Enter_320
   Case Menu_rgt
      Call Enter_340
   End Select
End Sub

Sub Enter_330
   Menu_position = 330
   Cls
   Locate 1 , 1
   Lcd "Menu:Ch3 Exponent."
   Locate 2 , 1
   Lcd "Sub_t End_p Exp N/R"
   Locate 2 , 13
End Sub

Sub M_331
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_330
   Case Menu_lft
      I1_menu = Ch3_exponential(current_model_nr) - 1
      Ch3_exponential(current_model_nr) = I1_menu
      Call Enter_331
   Case Menu_rgt
      I1_menu = Ch3_exponential(current_model_nr) + 1
      Ch3_exponential(current_model_nr) = I1_menu
      Call Enter_331
   End Select
End Sub

Sub Enter_331
   Menu_position = 331
   Cls
   Locate 1 , 1
   Lcd "Menu:Ch3 Exponent."
   Locate 2 , 1
   I1_menu = Ch3_exponential(current_model_nr)
   S1_menu = Str(i1_menu)
   Lcd Format(s1_menu , "+000")
   Locate 2 , 21
End Sub

Sub M_340
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_300
   Case Menu_dwn
      If Ch3_normal_reverse(current_model_nr) = 0 Then
         Ch3_normal_reverse(current_model_nr) = 1
      Else
         Ch3_normal_reverse(current_model_nr) = 0
      End If
      Call Enter_340
   Case Menu_lft
      Call Enter_330
   Case Menu_rgt
      Call Enter_310
   End Select
End Sub

Sub Enter_340
   Menu_position = 340
   Cls
   Locate 1 , 1
   Lcd "Menu:Ch3 "
   If Ch3_normal_reverse(current_model_nr) = 0 Then
      Lcd "Normal"
   Else
      Lcd "Reverse"
   End If
   Locate 2 , 1
   Lcd "Sub_t End_p Exp N/R"
   Locate 2 , 17
End Sub Menu_system

Sub M_400
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_000
   Case Menu_dwn
      Copy_of_model_name = Model_name(current_model_nr)
      Call Enter_410
   Case Menu_lft
      Call Enter_300
   Case Menu_rgt
      Call Enter_000
   End Select
End Sub

Sub Enter_400
   Menu_position = 400
   Cls
   Locate 1 , 1
   Lcd "Menu:Model_name"
   Locate 2 , 1
   Lcd "Steer Speed Ch3 Mdl"
   Locate 2 , 17
End Sub

Sub M_410
   Select Case Menu_button_pressed
   Case Menu_up
      Model_name(current_model_nr) = Copy_of_model_name
      Call Enter_400
   Case Menu_dwn
      S2_menu = Mid(copy_of_model_name , Pos_in_model_name , 1)
      Call Enter_411
   Case Menu_lft
      Pos_in_model_name = Pos_in_model_name - 1
      If Pos_in_model_name = 0 Then Pos_in_model_name = 1
      Call Enter_410
   Case Menu_rgt
      Pos_in_model_name = Pos_in_model_name + 1
      If Pos_in_model_name = 17 Then Pos_in_model_name = 16
      Call Enter_410
   End Select
End Sub

Sub Enter_410
   Menu_position = 410
   Cls
   Locate 1 , 1
   Lcd "Mn: "
   Lcd Copy_of_model_name
   Locate 2 , 1 : Lcd "                     "
   B1_menu = 4 + Pos_in_model_name
   Locate 1 , B1_menu
End Sub

Sub M_411
   Select Case Menu_button_pressed
   Case Menu_up
      Call Enter_410
   Case Menu_lft
      If Position_in_possible_characters > 1 Then
         Position_in_possible_characters = Position_in_possible_characters - 1
         S2_menu = Possible_characters(position_in_possible_characters)
      End If
      S3_menu = ""
      If Pos_in_model_name > 1 Then
         B1_menu = Pos_in_model_name - 1
         S3_menu = Left(copy_of_model_name , B1_menu )
      End If
      S3_menu = S3_menu + S2_menu
      If Pos_in_model_name < 16 Then
         B1_menu = 16 - Pos_in_model_name
         S3_menu = S3_menu + Right(copy_of_model_name , B1_menu )
      End If
      Copy_of_model_name = S3_menu
      Locate 1 , 5
      Lcd Copy_of_model_name
      Call Enter_411
   Case Menu_rgt
      If Position_in_possible_characters < 41 Then
         Position_in_possible_characters = Position_in_possible_characters + 1
         S2_menu = Possible_characters(position_in_possible_characters)
      End If
      S3_menu = ""
      If Pos_in_model_name > 1 Then
         B1_menu = Pos_in_model_name - 1
         S3_menu = Left(copy_of_model_name , B1_menu )
      End If
      S3_menu = S3_menu + S2_menu
      If Pos_in_model_name < 16 Then
         B1_menu = 16 - Pos_in_model_name
         S3_menu = S3_menu + Right(copy_of_model_name , B1_menu )
      End If
      Copy_of_model_name = S3_menu
      Locate 1 , 5
      Lcd Copy_of_model_name
      Call Enter_411
   End Select
End Sub

Sub Enter_411
'This routine assumes that we have:
'- a copy of the model name in Copy_of_model_name
'- the position of the chraracter that we want to replace in Pos_in_model_name
'- the character that we want to replace in S2_menu
   Menu_position = 411
   For Position_in_possible_characters = 1 To 41
      If Possible_characters(position_in_possible_characters) = S2_menu Then
         Exit For
      End If
   Next
   I2_menu = Position_in_possible_characters - 10
   B2_menu = Position_in_possible_characters + 9
   If I2_menu < 0 Then
      S3_menu = ""
      For B4_menu = 1 To 20
         S1_menu = Possible_characters(b4_menu)
         S3_menu = S3_menu + S1_menu
      Next
      Position_in_character_selection = 10 + I2_menu
   Elseif B2_menu > 41 Then
      S3_menu = ""
      For B4_menu = 22 To 41
         S1_menu = Possible_characters(b4_menu)
         S3_menu = S3_menu + S1_menu
      Next
      B2_menu = B2_menu - 41
      Position_in_character_selection = 11 + B2_menu
   Else
      S3_menu = ""
      B2_menu = Position_in_possible_characters - 10
      B3_menu = Position_in_possible_characters + 9
      For B4_menu = B2_menu To B3_menu
         S1_menu = Possible_characters(b4_menu)
         S3_menu = S3_menu + S1_menu
      Next
      Position_in_character_selection = 11
   End If
   Locate 2 , 1
   Lcd S3_menu
   Locate 2 , Position_in_character_selection
End Sub
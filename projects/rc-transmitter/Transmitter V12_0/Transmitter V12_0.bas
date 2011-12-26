'(*********************************************************************************************************************************
                 Transmitter V12_0.bas
written for    : BASCOM-AVR Version 2.0.7.3 build 001
by             : Jan Huygh (jan.huygh@gmail.com)
in             : December 2011

-----[Copyright (c) 2011 Jan Huygh jan.huygh@gmail.com]----------------------------------------------------------------------------

The author would appreciate it if enhancements to this software or it's documentation would be communicated to him.

In an effort to encourage the development of enhancements to this software permission is hereby granted, free of charge, to any
person obtaining a copy of this software, to deal in the software without restriction, including without limitation the right to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of this software, and to permit persons to whom the
software is furnished to do so if the above copyright notice and this permission notice is included in all copies of this software
or a substantial portion of this software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

-----[Introduction]----------------------------------------------------------------------------------------------------------------

If you want to build your own RC transmitter then using a commercial HF module such as the Graupner/JR Sender-HF-Modul
Part Nr 4057 or the Graupner/JR PLL-SYNTHESIZER-Sender HF-Modul Part Nr 4059 is a good idea. Another alternative is a 2.4GHz
module like the Jeti Duplex TU2 EX (http://jetimodel.com/index.php?page=product&id=197)
Using such a module you will not have to worry about a transmitter license nor will you have to worry about building the HF-portion
of the transmitter. Such modules take a Pulse Position Modulation signal as input.
I can buy such a module for about 75 Euro. If somebody can tell me where I can buy a comparable module for less please send me a
mail. jan.huygh@gmail.com

-----[Description of the positive Pulse Position Modulation signal]----------------------------------------------------------------

The line goes low for 0,4ms then high for 1 to 2 ms then low again for 0,4 ms. The 1 to 2 ms pulse represents channel-1.
You can now add channel-2 to channel-8 by bringing the line high for 1 to 2 ms and then low again for 0,4 ms for each channel.
After channel 8 you have to end with a 0,4 ms low. This pulsetrain is repeated every 20 ms. Since all the channels might want to
transmit a 2 ms pulse and we need to separate the channels with a 0,4 ms pulse we can transmit 8 channels within 20 ms since
0,4 + 8 * (0,4 + 2) = 19.6 ms. You do not need to transmit 8 channels. You can stop after any channel just don't forget to end with
a 0,4ms low.

-----[Connecting to a Graupner/JR Sender-HF-Modul]---------------------------------------------------------------------------------

  |                    If you put the HF module in front of you with the connector (5 small holes) in the lower left corner
  |  ° PPM             the connections are as indicated here on the left.
  |  ° +5V
  |  ° +8V                PPM is the Positive Pulse Position Signal
  |  ° GND                + 5V is a stabelized 5V tention
  |  ° To Antenna         + 8V to this you connect your main battery (8 * 1.5 = 12V is OK too ).
  |______________         GND is the ground connection (the - from the battery)

-----[Connecting to a Jeti Duplex TU2 EX modul]------------------------------------------------------------------------------------

That can not be simpler : 5V to 5V, Ground to ground, PPM output from your AVR to PPM input of the module. DONE !

-----[My version numbering system]-------------------------------------------------------------------------------------------------

When I start with a completely new program that will be the version 1_0.
As more functionality is introduced sucessive versions will be named 1_1, 1_2, ...
At a certain point in time I will have the first supposed usable version that will be version 2_0
When bug fixes and minor functionality changes are introduced the version numbers will be 2_1, 2_2, ...
When I start developing major functionality changes that version will be version 3_0, 3_1, 3_2, ...
The first usable version of that code will be version 4_0.
So in summary : versions starting with an odd number (1,3,5,...) are development versions not for use
                versions strating with an even number (2,4,6,...) are supposed to be usable.

-----[Version planning for Transmitter]--------------------------------------------------------------------------------------------

Version 12_0 :The two channel multiple model transmitter
Reads the battery voltage and displays it on an LCD.
Reads a selector switch that selects what model is used (9 models supported)
Menu system that enables the user to program into the EEPROM (for each of the 9 models supported) the values for
- Steer_normal_reverse, Steer_subtrim, Steer_high_max, Steer_low_max, Steer_exponential
- Speed_normal_reverse, Speed_subtrim, Speed_high_max, Speed_low_max, Speed_exponential
Reads the potentiometer for the speed and the steering stick on the transmitter.
Reads the potentiometer for the speed-trim and the steering-trim on the transmitter.
Reads the EEPROM value for Sub-trim setting for steer and speed.
Reads the EEPROM value for values for steer maximum, steer minimum, speed maximum, speed minimuml. (Neutral = subtrim)
Reads the EEPROM value for Steer_reverse (yes or no) and Speed_reverse (yes or no) setting.
Reads the EEPROM value for values for steer and speed exponential.
Calculates the pulse lengths.
Generates the Pulse Position Modulation signal.


Version 14_0
Same as version 12_0 PLUS
The menu system enables you to change the name for the models 1 to 9

Version 16_0 to 18_0 :The two channel multiple model transmitter
Skipped on purpose to accomodate for eventual changes in the above numbering and still be able to start with the first version
with a timer as version 20_0

Version 20_0
Same as previous version PLUS
A timer/lap counter

Version 22_0 to 28_0
Skipped on purpose to accomodate for versions that have different functionality of the lab counter and still be able to start with
the first version with an automatic sequence as version 30_0

Version 30_0
Same as previous version PLUS
By pushing a button the user can trigger a sequence of events.
The purpose of this functionality would be to aim for a perfect combination of actions for a specific manouvre.

-----[CPU cycle table and timing table for 16 bit timer Timer1 when running with a 16Mhz external crystal]-------------------------

Prescale  Maximum CPU Cycles before       Precision           With a 16MHz external crystal     With 16Mhz external crystal
  Factor            Timer1 overflow     CPU Cyclse]     Maxumum time before Timer1 Overflow      Highest possible precision

      1         65 536 =     65 536               1        65 536/16 000 000 =     4 096 µs                       0,0625 µs
      8     8 X 65 536 =    524 288               8       524 288/16 000 000 =    32 768 µs                       0,5 µs
     64    64 X 65 536 =  4 194 304              64     4 194 304/16 000 000 =   262 144 µs                       4 µs
    256   256 X 65 536 = 16 777 216             256    16 777 216/16 000 000 = 1 048 576 µs                      16 µs

-----------------------------------------------------------------------------------------------------------------------------------
'**********************************************************************************************************************************
')

'I started by using an ATmega16. Then I intended to use Pin Change Interrupt (PCI) for the menu system. Since the ATmega16 does not
'support PCI on the pins of port D (to whom I connect the menu navigation buttons) I ordered an ATmega164A and put that in my
'transmitter. In the end I did not use the PCI so this code will also work on an ATmega16.
'During the development I reached the 16k limit of the ATmega16. So I got myself an ATmega644p.
'Probably I can still find a way to make my code fot a ATmega16. For sure it will be possibel to make it fit an ATmega324.
'But at the price difference betwaen an ATmega644p and an ATmega324... it might take forever before I start doing the effort
'to write my code less space consuming.
'So we are now using an ATmega644p running at 16MHz
$regfile = "m644pdef.dat" : $crystal = 16000000

'We will need the Analog to Digital converter to read the position of the sticks and trims on the transmitter
Config Adc = Single , Prescaler = Auto , Reference = Avcc : Start Adc

'We use TIMER1 to generate the Pulse Place Modulation Signal
Config Timer1 = Timer , Prescale = 8 : On Timer1 Timer1_isr : Stop Timer1 : Enable Timer1 : Enable Interrupts

'We will use a 20 character per line, 2 line Liquid Cristal Display connected as indicated in the statements here below
Config Lcdpin = Pin , Db4 = Portc.0 , Db5 = Portc.1 , Db6 = Portc.2 , Db7 = Portc.3 , E = Portc.5 , Rs = Portc.4
Config Lcd = 20 * 2 : Initlcd

'The model selector switch is connected to Port B .
Config Portb = Input

'The menu navigation buttons and the trigger button (that we intend to use for the lab counter) are connected to Port D.
Config Portd = Input

'Here we indicate what transmitter controll is connected to what AVR-pin
'Define from what pin the PPM signal is output and set that pin to output
Ppm_output_pin Alias Portc.6 : Config Ppm_output_pin = Output
'Define to what pin the steer stick, steer trim speed stick and speed trim is connected.
Get_steerstick_position Alias Getadc(0)
Get_speedstick_position Alias Getadc(2)
Get_steertrim_position Alias Getadc(1)
Get_speedtrim_position Alias Getadc(3)
Get_battery_voltage Alias Getadc(6)
'Just to make the code easy to read we define constants for the menu buttons
Const M_nothing = 63
Const M_esc = 31
Const M_up = 47
Const M_down = 55
Const M_left = 59
Const M_right = 61
Const M_enter = 62
Const M_trigger_button = 127

'Here we enter the values that are linked to the mechanics of the transmitter
Const Steerstick_lowest = 108
Const Steerstick_neutral = 518
Const Steerstick_maximum = 904
Const Speedstick_lowest = 126
Const Speedstick_neutral = 378
Const Speedstick_maximum = 893
Const Steertrim_lowest = 144
Const Steertrim_neutral = 507
Const Steertrim_maximum = 896
Const Speedtrim_lowest = 167
Const Speedtrim_neutral = 510
Const Speedtrim_maximum = 897
'Here we calculate values that are linked to the mechanics of the transmitter
Const Steerstick_lower_range = Steerstick_neutral - Steerstick_lowest
Const Steerstick_upper_range = Steerstick_maximum - Steerstick_neutral
Const Speedstick_lower_range = Speedstick_neutral - Speedstick_lowest
Const Speedstick_upper_range = Speedstick_maximum - Speedstick_neutral
Const Steertrim_lower_range = Steertrim_neutral - Steertrim_lowest
Const Steertrim_upper_range = Steertrim_maximum - Steertrim_neutral
Const Speedtrim_lower_range = Speedtrim_neutral - Speedtrim_lowest
Const Speedtrim_upper_range = Speedtrim_maximum - Speedtrim_neutral

'All variables are global meaning the main program and the interrupt service routines use the same variables.
'Some variables are intended for temporary storage of intermediate results. You can re-use them in the main code but you need
'a separate set of temporary/intermediate variables in the interrupt service routine(s) because otherwise the interrupt service
'routine might change the content of such a variable in the main program.

'Truly global variables: They have the same meaning in the main program and all the Timer1 interrupt service routine
Dim Ch(3) As Word
Dim Ppm_progress_counter As Byte

'Used only in Timer1 interrupt service routine
Dim B1_tisr As Byte
Dim W1_tisr As Word

'Used only in the main program (that includes the complete menu system)
Dim Current_model_nr As Byte

Dim Model_name(9) As Eram String * 10
Dim Steer_r_setting(9) As Eram Byte
Dim Speed_r_setting(9) As Eram Byte
Dim Steer_subtrim(9) As Eram Integer
Dim Speed_subtrim(9) As Eram Integer
Dim Steer_low_reduction(9) As Eram Integer
Dim Steer_high_reduction(9) As Eram Integer
Dim Speed_low_reduction(9) As Eram Integer
Dim Speed_high_reduction(9) As Eram Integer
Dim Steer_exponential(9) As Eram Integer
Dim Speed_exponential(9) As Eram Integer

Dim Ml1 As Byte , Ml2 As Byte , Ml3 As Byte
Dim M_button_was As Byte
Dim M_press_counter As Byte
Dim M_button_pressed As Byte

Dim B1 As Byte
Dim W1 As Word
Dim L1 As Long
Dim L2 As Long
Dim L3 As Long
Dim L4 As Long
Dim I1 As Integer
Dim I2 As Integer

Dim Xbp As Integer
Dim Ybp As Integer

Dim S1 As String * 10

'EEPROM initialisation
'For B1 = 1 To 9
'   S1 = "Model    " + Str(b1)
'   Model_name(b1) = S1
'   Steer_r_setting(b1) = 0
'   Speed_r_setting(b1) = 0
'   Steer_subtrim(b1) = 0
'   Speed_subtrim(b1) = 0
'   Steer_low_reduction(b1) = 100
'   Steer_high_reduction(b1) = 100
'   Speed_low_reduction(b1) = 100
'   Speed_high_reduction(b1) = 100
'   Steer_exponential(b1) = 0
'   Steer_exponential(b1) = 0
'Next B1

'Variable initialisation
Ppm_progress_counter = 1
Ch(1) = 500 : Ch(2) = 500
Ml1 = 0 : Ml2 = 0 : Ml3 = 0

Start Timer1

'For some reason constantly switching cursor on and off mixed with locate statements confused my LCD. So i set it here
Cursor On Blink

Do
'This code is polling for the menu navigation buttons.
'Save PIND in M_button_pressed so that successive tests of the value of PIND do not give inconsistant results due to bouncing
'  If there is a key pressed
'     If the key pressed is the same as the one that was pressed 20ms ago
'        Increment the M_press counter so we can avoid executing the action more then once for one long pressed button by
'        checking the value of M_press_counter
'        If M_press_counter = 1
'           this is the first consistant value and we go with that value to the menu system
'        If M_press_counter = 255
'           M_Press_counter = 100 to avoid that a really long press would make the counter overrun.
'           We use 100 instead of 2 to enable us to later use M_Press_counter to detect a long key press
'     If the key that is pressed is NOT the same as the previous poll then stor this key as "the new previous key"
'  If there was no button pressed that could also be an intermediate result in a bouncing key
'     If we have twice "no key pressed" in two measurements that are 20 ms separated we are sure the button is released and
'        We reset the M_Press_counter to zero so now a next key can come in and get executed
'     If we had just once a "no key pressed" we store the "No key pressed" in M_button_was
  M_button_pressed = Pind
   If M_button_pressed <> M_nothing Then
      If M_button_pressed = M_button_was Then
         Incr M_press_counter
         If M_press_counter = 1 Then
            Gosub Menu_system
         Elseif M_press_counter = 255 Then
            M_press_counter = 100
         End If
      Else       'If the key we detected as press was another one as the one we detected before
         M_button_was = M_button_pressed
      End If
   Else       'If no button is pressed
      If M_button_pressed = M_button_was Then
         M_press_counter = 0
      Else
         M_button_was = M_nothing
      End If
   End If

'Set Current_model_nr by reading the position of the selector switch
   B1 = Pinb
   If B1 = 1 Then Current_model_nr = 1
   If B1 = 2 Then Current_model_nr = 2
   If B1 = 4 Then Current_model_nr = 3
   If B1 = 8 Then Current_model_nr = 4
   If B1 = 16 Then Current_model_nr = 5
   If B1 = 32 Then Current_model_nr = 6
   If B1 = 64 Then Current_model_nr = 7
   If B1 = 128 Then Current_model_nr = 8
   If B1 = 0 Then Current_model_nr = 9

'This code does display model name and battery voltage on the LCD
'Since we do not want this to interfere with the menu system we only print this info on the LCD when we are outside the menu system
'If ML1 = 0 we are outside the menu system
'  Print the model name on the LCD
'  Get the battery voltage from the adc (a value from 0 to 1024)
'  Convert that value to a voltage
'  Print that voltage on the LCD with one decimal digit
   If Ml1 = 0 Then
      Locate 1 , 1 : S1 = Model_name(current_model_nr) : Lcd S1
      W1 = Get_battery_voltage : W1 = W1 * 10 : W1 = W1 / 70 : S1 = Str(w1)
      Locate 1 , 15 : Lcd Format(s1 , "  0.0") : Lcd "V"
   End If

'**********************************************************************************************************************************
'*    Here we start calculating the Ch(X) values.
'**********************************************************************************************************************************
'*       CH(1): Steer
'OPPORTUNITY FOR IMPROVEMENT
'Here we do the calculation for Steer and Speed in two bloks of code. It must be doable to calculate both with one block that
'gets different parameters as input like a big function.
'Read the value  from the steer stick potentiometer and convert the position to -500 to + 500 (with 0 representing neutral).
'The mechanical construction of the transmitter reduces the movement of the potentiometers of the sticks so we do not read
'values between 0 and 1024 with 512 as the neutral position. Since we want to use the full range with precision we convert the
'actual values to a value of -500 to 500 with 0 as the neutral position.
'We base that calulation on the constants Steerstick_lowest, Steerstick_neutral and Steerstick_maximum that we have defined at
'the start of the code.
   L1 = Get_steerstick_position
   If L1 < Steerstick_lowest Then L1 = Steerstick_lowest
   If L1 > Steerstick_maximum Then L1 = Steerstick_maximum
   L1 = L1 - Steerstick_neutral
   L1 = L1 * 500
   If L1 < 0 Then L1 = L1 / Steerstick_lower_range Else L1 = L1 / Steerstick_upper_range
'At this point in the code the value of L1 is
'- 500 less than or equal to L1 less than or equal to 500. AND L1=0 represents the neutral position of the stick

'EXPONENTIAL
'Normally the position of the steer stick converts in a linear way to the value of the CH(1) pulse.
'In the advanced trasmitter systems they offer you the possibility to alter the linear conversion to a non linear conversion
'They will offer a positive exponential curve and a negative exponential curve.
'The positive exponential curve will overreact in the beginning of the curve and underreact at the end of the curve.
'In other words, a small movement arround the neutral position results in more change of the pulselenght but on the other side
'near the end point of the steer stick movement you get less change in the pulse length for the movement of the steer stick
'So the ytotal spread remains from -500 to +500 and 0 represents still neutral.
'For the negative exponential we have less effect on the pulse lenght for the movements close to the neutral position and more
'near the end points.
'In RC race boats its is classic to use a negative curve on the steer. It will improve the precision you have on keeping
'a straight line and still enable you to take fast sharp curves.
'The system we have used to implement exponetila is by using a break point (defined by XBP and YBP)
'We store in the Steer_exponetial(X) the value that deviates XBP from 250.
'We have decided to limit the delta to -200 to +200. I guarantee that that gives much more agressive curves than you will ever
'want to use.
'The code actually breaks the total curve in 3 zones that each have an own equantion Y = a * X + b.
'X here is the position of the steer stick. Y is the convered value for the stear stick taking the exponential into account
'We first calculate the coordinates for Xbp and Ybp from the Steer_exponential(X) value for the active model
'Depending on the actual value of X (= the position of the steer stick) we can evaluate in what part of curve we are
'and by consequence what equation is valid.
'OPPORTUNITY FOR IMPOVEMENT
' A) Somehow it must be possible to calculate the values with ABS(L1) and keep that for L1>0 and use a simple conversion for L1<0
' B) Since I use this code twice (for Steer and Speed) and it takes a considerable amount of steps it's wort rewoking to a function
   If L1 > 0 Then
      Xbp = 250 - Steer_exponential(current_model_nr)
      Ybp = 250 + Steer_exponential(current_model_nr)
      If L1 < Xbp Then       ' Y = a1 * X
         L2 = Ybp * L1 : L1 = L2 / Xbp
      Else       ' Y = a2 * X + b2
         L2 = Ybp - 500
         L3 = Xbp - 500
         L4 = L2 * L1 : L4 = L4 / L3 : L4 = L4 + 500 : L2 = L2 * 500 : L2 = L2 / L3 : L1 = L4 - L2
      End If
   Else       'If L1 =< 0
      Xbp = -250 + Steer_exponential(current_model_nr)
      Ybp = -250 - Steer_exponential(current_model_nr)
      If L1 > Xbp Then       'Y = a1 * X
         L2 = Ybp * L1 : L1 = L2 / Xbp
      Else       'Y = a3 * X + b3
         L2 = Ybp + 500
         L3 = Xbp + 500
         L4 = L2 * L1 : L4 = L4 / L3 : L4 = L4 - 500 : L2 = L2 * 500 : L2 = L2 / L3 : L1 = L4 + L2
      End If
   End If       ' L1
'At this point in the code the value of L1 is
'- 500 less than or equal to L1 less than or equal to 500. AND L1=0 represents the neutral position of the stick



'TRIM and END POINT
'We start by reading the position of the Steer Trim by reading the value from the Steer Trim Potentiometer
'This time we convert to a value between -200 and +200 thus limiting the influnce of the trim to 200/500 = 40%
   L2 = Get_steertrim_position
   If L2 < Steertrim_lowest Then L2 = Steertrim_lowest
   If L2 > Steertrim_maximum Then L2 = Steertrim_maximum
   L2 = L2 - Steertrim_neutral
   L2 = L2 * 200
   If L2 < 0 Then L2 = L2 / Steertrim_lower_range Else L2 = L2 / Steertrim_upper_range
'At this point in the code the value of L2 is
'-200 less than or equal to L2 less than or equal to 200. AND L2=0 represents the neutral position of the trim

'We add the value we got from the Steer trim potentiometer to the value we get from the EEPROM for Steer_subtrim
'To avoid that we would get too high value for the trim correction we limit the influence to -200 to +200
'If we are in the right side of the Steer stick movement (L1>0)
'  The total range = 500 - the_total_trim_adjustment - the_high_reduction (so this handles also the end point adjustment)
'If we are in the left side of the Steer stick movement (L1<0)
'  The total range = 500 + the_total_trim_adjustment - the_low_reduction (so this handles also the end point adjustment)
'The stick position value = [(stick_position * Range ) / 500] + total_trim_adjustment
   I1 = Steer_subtrim(current_model_nr) : L2 = L2 + I1       'L2 = L2 + Steer_subtrim(1) gives a type conversion error
   If L2 > 200 Then L2 = 200 : If L2 < -200 Then L2 = -200
   If L1 > 0 Then
      L3 = 500 - L2
   Else
      L3 = 500 + L2
   End If
   I1 = Steer_low_reduction(current_model_nr) : L3 = L3 - I1       'L3 = L3 - Steer_low_reduction(1) gives a type conv error
   L1 = L3 * L1 : L1 = L1 / 500 : L1 = L1 + L2
'At this point in the code the value of L1 is
'- 500 less than or equal to L1 less than or equal to 500. AND L1=0 + TOTAL_TRIM_ADJUSTMENT = the neutral position

'We take the Normal/reverse setting into account
   If Steer_r_setting(current_model_nr) = 0 Then Ch(1) = L1 + 500 Else Ch(1) = 500 - L1


'**********************************************************************************************************************************
'*       CH(1): Steer
   L1 = Get_speedstick_position
   If L1 < Speedstick_lowest Then L1 = Speedstick_lowest
   If L1 > Speedstick_maximum Then L1 = Speedstick_maximum
   L1 = L1 - Speedstick_neutral
   L1 = L1 * 500
   If L1 < 0 Then L1 = L1 / Speedstick_lower_range Else L1 = L1 / Speedstick_upper_range

'EXPONENTIAL
   If L1 > 0 Then
      Xbp = 250 - Speed_exponential(current_model_nr)
      Ybp = 250 + Speed_exponential(current_model_nr)
      If L1 < Xbp Then       ' Y = A1 * X
         L2 = Ybp * L1 : L1 = L2 / Xbp
      Else
         L2 = Ybp - 500
         L3 = Xbp - 500
         L4 = L2 * L1 : L4 = L4 / L3 : L4 = L4 + 500 : L2 = L2 * 500 : L2 = L2 / L3 : L1 = L4 - L2
      End If
   Else       'If L1 =< 0
      Xbp = -250 + Speed_exponential(current_model_nr)
      Ybp = -250 - Speed_exponential(current_model_nr)
      If L1 > Xbp Then       'Y = A1 * X
         L2 = Ybp * L1 : L1 = L2 / Xbp
      Else
         L2 = Ybp + 500
         L3 = Xbp + 500
         L4 = L2 * L1 : L4 = L4 / L3 : L4 = L4 - 500 : L2 = L2 * 500 : L2 = L2 / L3 : L1 = L4 + L2
      End If
   End If       ' L1

'TRIM and END POINT adjustment
   L2 = Get_speedtrim_position
   If L2 < Speedtrim_lowest Then L2 = Speedtrim_lowest
   If L2 > Speedtrim_maximum Then L2 = Speedtrim_maximum
   L2 = L2 - Speedtrim_neutral
   L2 = L2 * 200
   If L2 < 0 Then L2 = L2 / Speedtrim_lower_range Else L2 = L2 / Speedtrim_upper_range

   I1 = Speed_subtrim(current_model_nr)
   L2 = L2 + I1 : If L2 > 200 Then L2 = 200 : If L2 < -200 Then L2 = -200
   If L1 > 0 Then
      L3 = 500 - L2
   Else
      L3 = 500 + L2
   End If
   I1 = Speed_low_reduction(current_model_nr) : L3 = L3 - I1
   L1 = L3 * L1 : L1 = L1 / 500 : L1 = L1 + L2
'At this point in the code the value of L1 is
'- 500 less than or equal to L1 less than or equal to 500. AND L1=0 + TOTAL_TRIM_ADJUSTMENT = the neutral position


'Normal/Reverse
   If Speed_r_setting(current_model_nr) = 0 Then Ch(2) = L1 + 500 Else Ch(2) = 500 - L1
'At this point in the code the value of CH(x) is
'0 less than or equal to L1 less than or equal to 1000


Loop

End

'**********************************************************************************************************************************
'*       The PPM signal generator
'The action to take by the PMM signal generator is based on the value of Ppm_progress_counter.
'The Ppm_progress_counter is incemented before leaving the interrupt service routine.
'When Ppm_progress_counter = 7 it is  reset to 1 at the start of the do-loop in the main program.
'For a 2 channel version when Ppm_progress_counter is 1, 3 or 5 the 400 µs low separation pulse  needs to be transmitted
'That would mean we need to load the timer with  65536 - (400 * 2) =  64736 In practice the value delivering 400µS is 64752
'I think this is caused by losing time when pushing all the registers on the stack when entering the isr.
'For a 2 channel version when Ppm_progress_counter is 2 or 4 the channel 1 and channel 2 pulse must be transmitted.
'In this code we start by converting the value in CH(X) that is expressed in microsecconds to clock ticks (so X 2).
'We now reload the counter with the number needed for a 1000µs pulse + the number of clock ticks just calculated
'For a 1000µs pulse we would expect 63536 but we use 63576 (again due to pushing registers on stack now plus some code in the isr)
'For a 2 channel version when Ppm_progress_counter is 6 the timer needs to restart after 20-(3X0,4)-(2*1)-(CH(1)+CH(2)) ms.
'At the end of each isr call we increùment Ppm_progress_counter. When it reaches 7 we reset to 1 to start the next pulse
Timer1_isr:
   If Ppm_progress_counter = 1 Or Ppm_progress_counter = 3 Or Ppm_progress_counter = 5 Then
      Reset Ppm_output_pin
      Timer1 = 64752
   Elseif Ppm_progress_counter = 2 Or Ppm_progress_counter = 4 Then
      Set Ppm_output_pin
      B1_tisr = Ppm_progress_counter / 2 : W1_tisr = Ch(b1_tisr) * 2 : Timer1 = 63576 - W1_tisr
   Elseif Ppm_progress_counter = 6 Then
      Set Ppm_output_pin
      W1_tisr = Ch(1) + Ch(2)
      W1_tisr = W1_tisr * 2 : Timer1 = 31935 + W1_tisr
   End If
   Incr Ppm_progress_counter
   If Ppm_progress_counter = 7 Then Ppm_progress_counter = 1
Return


'The menu system
Macro Enter000
   Cls
End Macro

Macro Enter100
   Locate 1 , 1 : Lcd "Menu:Steer          "
   Locate 2 , 1 : Lcd "Steer Speed Mdl_Name"
   Locate 2 , 1
End Macro

Macro Enter200
   Locate 1 , 1 : Lcd "Menu:Speed          "
   Locate 2 , 1 : Lcd "Steer Speed Mdl_Name"
   Locate 2 , 7
End Macro

Macro Enter300
   Locate 1 , 1 : Lcd "Menu:Model_Name     "
   Locate 2 , 1 : Lcd "Steer Speed Mdl_Name"
   Locate 2 , 13
End Macro

Macro Enter110
   Locate 1 , 1 : Lcd "Menu:Steer Sub_Trim "
   Locate 2 , 1 : Lcd "Sub_T End_P Exp N/R "
   Locate 2 , 1
End Macro

Macro Enter120
   Locate 1 , 1 : Lcd "Menu:Steer End_Point"
   Locate 2 , 1 : Lcd "Sub_T End_P Exp N/R "
   Locate 2 , 7
End Macro

Macro Enter130
   Locate 1 , 1 : Lcd "Menu:Steer Exponent."
   Locate 2 , 1 : Lcd "Sub_T End_P Exp N/R "
   Locate 2 , 13
End Macro

Macro Enter140
   Locate 1 , 1 : Lcd "Menu:Steer Nrml/Rev "
   Locate 2 , 1 : Lcd "Sub_T End_P Exp N/R "
   Locate 2 , 17
End Macro

Macro Enter210
   Locate 1 , 1 : Lcd "Menu:Speed Sub_Trim "
   Locate 2 , 1 : Lcd "Sub_T End_P Exp N/R "
   Locate 2 , 1
End Macro

Macro Enter220
   Locate 1 , 1 : Lcd "Menu:Speed End_Point"
   Locate 2 , 1 : Lcd "Sub_T End_P Exp N/R "
   Locate 2 , 7
End Macro

Macro Enter230
   Locate 1 , 1 : Lcd "Menu:Speed Exponent."
   Locate 2 , 1 : Lcd "Sub_T End_P Exp N/R "
   Locate 2 , 13
End Macro

Macro Enter240
   Locate 1 , 1 : Lcd "Menu:Speed Nrml/Rev "
   Locate 2 , 1 : Lcd "Sub_T End_P Exp N/R "
   Locate 2 , 17
End Macro

Macro Enter310
   Locate 1 , 1
   Lcd "Menu:["
   S1 = Model_name(current_model_nr)
   Lcd S1
   Lcd "]"
   Locate 2 , 1 : Lcd "                     "
   Locate 1 , 7
End Macro

'Steer Subtrim
Macro Enter111
   Locate 2 , 1
   I1 = Steer_subtrim(current_model_nr)
   If Steer_r_setting(current_model_nr) = 0 Then
      I2 = 1500 + I1
   Else
      I2 = 1500 - I1
   End If
   If I1 > 0 Then
      Lcd "R"
   Elseif I1 = 0 Then
      Lcd "N"
   Else
      Lcd "L"
   End If
   I1 = Abs(i1)
   S1 = Str(i1)
   Lcd Format(s1 , "000")
   Lcd " => "
   S1 = Str(i2)
   Lcd Format(s1 , "0000")
   Lcd " mS     "
   Locate 2 , 21
End Macro

'Steer Endpoint
Macro Enter121
   If Steer_r_setting(current_model_nr) = 0 Then
      I1 = Steer_low_reduction(current_model_nr)
      I2 = Steer_high_reduction(current_model_nr)
   Else
      I1 = Steer_high_reduction(current_model_nr)
      I2 = Steer_low_reduction(current_model_nr)
   End If
   Locate 2 , 1
   Lcd "L-"
   S1 = Str(i1)
   Lcd Format(s1 , "000")
   Lcd "          -"
   S1 = Str(i2)
   Lcd Format(s1 , "000")
   Lcd "R"
End Macro

'Steer Exponential
Macro Enter131
   Locate 2 , 1
   I1 = Steer_exponential(current_model_nr)
   S1 = Str(i1)
   Lcd Format(s1 , "+000")
   Lcd "                "
End Macro

'Steer Normal/Reverse
Macro Enter141
   Locate 2 , 1 :
   If Steer_r_setting(current_model_nr) = 0 Then
      Lcd "NORMAL              "
   Else
      Lcd "REVERSE             "
   End If
End Macro

'Speed Subtrim
Macro Enter211
   Locate 2 , 1
   I1 = Speed_subtrim(current_model_nr)
   If Speed_r_setting(current_model_nr) = 0 Then
      I2 = 1500 + I1
   Else
      I2 = 1500 - I1
   End If
   If I1 > 0 Then
      Lcd "F"
   Elseif I1 = 0 Then
      Lcd "N"
   Else
      Lcd "B"
   End If
   I1 = Abs(i1)
   S1 = Str(i1)
   Lcd Format(s1 , "000")
   Lcd " => "
   S1 = Str(i2)
   Lcd Format(s1 , "0000")
   Lcd " mS     "
   Locate 2 , 21
End Macro

'Speed Endpoint
Macro Enter221
   If Speed_r_setting(current_model_nr) = 0 Then
      I1 = Speed_low_reduction(current_model_nr)
      I2 = Speed_high_reduction(current_model_nr)
   Else
      I1 = Speed_high_reduction(current_model_nr)
      I2 = Speed_low_reduction(current_model_nr)
   End If
   Locate 2 , 1
   Lcd "B-"
   S1 = Str(i1)
   Lcd Format(s1 , "000")
   Lcd "          -"
   S1 = Str(i2)
   Lcd Format(s1 , "000")
   Lcd "F"
End Macro

'Speed Exponential
Macro Enter231
   Locate 2 , 1
   I1 = Speed_exponential(current_model_nr)
   S1 = Str(i1)
   Lcd Format(s1 , "+000")
   Lcd "                    "
End Macro

'Speed Normal/Reverse
Macro Enter241
   Locate 2 , 1 :
   If Speed_r_setting(current_model_nr) = 0 Then
      Lcd "NORMAL              "
   Else
      Lcd "REVERSE             "
   End If
End Macro

'Model Name
Macro Enter311
   Locate 2 , 1 : Lcd "Menu311    "
End Macro

'This is the code that defines for each menu block what code needs to be executed when the menu button is pressed that has been
'pressed.
Macro Menu000
   If M_button_pressed = M_right Then
      Ml1 = 1
      Enter100
   Elseif M_button_pressed = M_left Then
      Ml1 = 3
      Enter300
   End If
End Macro

Macro Menu100
   If M_button_pressed = M_left Or M_button_pressed = M_up Then
      Ml1 = 0
      Enter000
   Elseif M_button_pressed = M_right Then
      Ml1 = 2
      Enter200
   Elseif M_button_pressed = M_down Then
      Ml2 = 1
      Enter110
   End If
End Macro

Macro Menu200
   If M_button_pressed = M_left Then
      Ml1 = 1
      Enter100
   Elseif M_button_pressed = M_right Then
      Ml1 = 3
      Enter300
   Elseif M_button_pressed = M_down Then
      Ml2 = 1
      Enter210
   Elseif M_button_pressed = M_up Then
      Ml1 = 0
      Enter000
   End If
End Macro

Macro Menu300
   If M_button_pressed = M_left Then
      Ml1 = 2
      Enter200
   Elseif M_button_pressed = M_right Or M_button_pressed = M_up Then
      Ml1 = 0
      Enter000
   Elseif M_button_pressed = M_down Then
      Ml2 = 1
      Enter310
   End If
End Macro

Macro Menu110
   If M_button_pressed = M_left Then
      Ml2 = 4
      Enter140
   Elseif M_button_pressed = M_right Then
      Ml2 = 2
      Enter120
   Elseif M_button_pressed = M_up Then
      Ml2 = 0
      Enter100
   Elseif M_button_pressed = M_down Then
      Ml3 = 1
      Enter111
   End If
End Macro

Macro Menu120
   If M_button_pressed = M_left Then
      Ml2 = 1
      Enter110
   Elseif M_button_pressed = M_right Then
      Ml2 = 3
      Enter130
   Elseif M_button_pressed = M_up Then
      Ml2 = 0
      Enter100
   Elseif M_button_pressed = M_down Then
      Ml3 = 1
      Enter121
   End If
End Macro

Macro Menu130
   If M_button_pressed = M_left Then
      Ml2 = 2
      Enter120
   Elseif M_button_pressed = M_right Then
      Ml2 = 4
      Enter140
   Elseif M_button_pressed = M_up Then
      Ml2 = 0
      Enter100
   Elseif M_button_pressed = M_down Then
      Ml3 = 1
      Enter131
   End If
End Macro

Macro Menu140
   If M_button_pressed = M_left Then
      Ml2 = 3
      Enter130
   Elseif M_button_pressed = M_right Then
      Ml2 = 1
      Enter110
   Elseif M_button_pressed = M_up Then
      Ml2 = 0
      Enter100
   Elseif M_button_pressed = M_down Then
      Ml3 = 1
      Enter141
   End If
End Macro

Macro Menu210
   If M_button_pressed = M_left Then
      Ml2 = 4
      Enter240
   Elseif M_button_pressed = M_right Then
      Ml2 = 2
      Enter220
   Elseif M_button_pressed = M_up Then
      Ml2 = 0
      Enter200
   Elseif M_button_pressed = M_down Then
      Ml3 = 1
      Enter211
   End If
End Macro

Macro Menu220
   If M_button_pressed = M_left Then
      Ml2 = 1
      Enter210
   Elseif M_button_pressed = M_right Then
      Ml2 = 3
      Enter230
   Elseif M_button_pressed = M_up Then
      Ml2 = 0
      Enter200
   Elseif M_button_pressed = M_down Then
      Ml3 = 1
      Enter221
   End If
End Macro

Macro Menu230
   If M_button_pressed = M_left Then
      Ml2 = 2
      Enter220
   Elseif M_button_pressed = M_right Then
      Ml2 = 4
      Enter240
   Elseif M_button_pressed = M_up Then
      Ml2 = 0
      Enter200
   Elseif M_button_pressed = M_down Then
      Ml3 = 1
      Enter231
   End If
End Macro

Macro Menu240
   If M_button_pressed = M_left Then
      Ml2 = 3
      Enter230
   Elseif M_button_pressed = M_right Then
      Ml2 = 1
      Enter210
   Elseif M_button_pressed = M_up Then
      Ml2 = 0
      Enter200
   Elseif M_button_pressed = M_down Then
      Ml3 = 1
      Enter241
   End If
End Macro

Macro Menu310
   If M_button_pressed = M_left Then
      Ml2 = 1
      Enter310
   Elseif M_button_pressed = M_right Then
      Ml2 = 1
      Enter310
   Elseif M_button_pressed = M_up Then
      Ml2 = 0
      Enter300
   Elseif M_button_pressed = M_down Then
      Ml3 = 1
      Enter311
   End If
End Macro

Macro Menu111
   If M_button_pressed = M_up Then
      Ml3 = 0
      Enter110
   Elseif M_button_pressed = M_right Then
      I1 = Steer_subtrim(current_model_nr) + 1
      Steer_subtrim(current_model_nr) = I1
      Enter111
   Elseif M_button_pressed = M_left Then
      I1 = Steer_subtrim(current_model_nr) - 1
      Steer_subtrim(current_model_nr) = I1
      Enter111
   End If
End Macro

Macro Menu121
   I1 = Ch(1) - 500
   I2 = Abs(i1)
   If M_button_pressed = M_up Then
      Ml3 = 0
      Enter120
   Elseif M_button_pressed = M_right Then
      If I2 < 300 Then
         I1 = Steer_low_reduction(current_model_nr) - 1
         Steer_low_reduction(current_model_nr) = I1
         I1 = Steer_high_reduction(current_model_nr) - 1
         Steer_high_reduction(current_model_nr) = I1
      Elseif I1 > 300 Then
         I1 = Steer_high_reduction(current_model_nr) - 1
         Steer_high_reduction(current_model_nr) = I1
      Elseif I1 < -300 Then
         I1 = Steer_low_reduction(current_model_nr) - 1
         Steer_low_reduction(current_model_nr) = I1
      End If
      Enter121
   Elseif M_button_pressed = M_left Then
      If I2 < 300 Then
         I1 = Steer_low_reduction(current_model_nr) + 1
         Steer_low_reduction(current_model_nr) = I1
         I1 = Steer_high_reduction(current_model_nr) + 1
         Steer_high_reduction(current_model_nr) = I1
      Elseif I1 > 300 Then
         I1 = Steer_high_reduction(current_model_nr) + 1
         Steer_high_reduction(current_model_nr) = I1
      Elseif I1 < -300 Then
         I1 = Steer_low_reduction(current_model_nr) + 1
         Steer_low_reduction(current_model_nr) = I1
      End If
      Enter121
   End If
End Macro

'Steer Exponential
Macro Menu131
   If M_button_pressed = M_up Then
      Ml3 = 0
      Enter130
   Elseif M_button_pressed = M_right Then
      I1 = Steer_exponential(current_model_nr) + 1
      Steer_exponential(current_model_nr) = I1
      Enter131
   Elseif M_button_pressed = M_left Then
      I1 = Steer_exponential(current_model_nr) - 1
      Steer_exponential(current_model_nr) = I1
      Enter131
   End If
End Macro

Macro Menu141
   If M_button_pressed = M_up Then
      Ml3 = 0
      Enter140
   Elseif M_button_pressed = M_left Or M_button_pressed = M_right Then
      If Steer_r_setting(current_model_nr) = 0 Then
         Steer_r_setting(current_model_nr) = 1
      Else
         Steer_r_setting(current_model_nr) = 0
      End If
      Enter141
   End If
End Macro

'Speed Subtrim
Macro Menu211
   If M_button_pressed = M_up Then
      Ml3 = 0
      Enter110
   Elseif M_button_pressed = M_right Then
      I1 = Speed_subtrim(current_model_nr) + 1
      Speed_subtrim(current_model_nr) = I1
      Enter211
   Elseif M_button_pressed = M_left Then
      I1 = Speed_subtrim(current_model_nr) - 1
      Speed_subtrim(current_model_nr) = I1
      Enter211
   End If
End Macro

'Speed Endpoint
Macro Menu221
   I1 = Ch(2) - 500
   I2 = Abs(i1)
   If M_button_pressed = M_up Then
      Ml3 = 0
      Enter220
   Elseif M_button_pressed = M_right Then
      If I2 < 300 Then
         I1 = Speed_low_reduction(current_model_nr) - 1
         Speed_low_reduction(current_model_nr) = I1
         I1 = Speed_high_reduction(current_model_nr) - 1
         Speed_high_reduction(current_model_nr) = I1
      Elseif I1 > 300 Then
         I1 = Speed_high_reduction(current_model_nr) - 1
         Speed_high_reduction(current_model_nr) = I1
      Elseif I1 < -300 Then
         I1 = Speed_low_reduction(current_model_nr) - 1
         Speed_low_reduction(current_model_nr) = I1
      End If
      Enter221
   Elseif M_button_pressed = M_left Then
      If I2 < 300 Then
         I1 = Speed_low_reduction(current_model_nr) + 1
         Speed_low_reduction(current_model_nr) = I1
         I1 = Speed_high_reduction(current_model_nr) + 1
         Speed_high_reduction(current_model_nr) = I1
      Elseif I1 > 300 Then
         I1 = Speed_high_reduction(current_model_nr) + 1
         Speed_high_reduction(current_model_nr) = I1
      Elseif I1 < -300 Then
         I1 = Speed_low_reduction(current_model_nr) + 1
         Speed_low_reduction(current_model_nr) = I1
      End If
      Enter221
   End If
End Macro

'Speed Exponential
Macro Menu231
   If M_button_pressed = M_up Then
      Ml3 = 0
      Enter230
   Elseif M_button_pressed = M_right Then
      I1 = Speed_exponential(current_model_nr) + 1
      Speed_exponential(current_model_nr) = I1
      Enter231
   Elseif M_button_pressed = M_left Then
      I1 = Speed_exponential(current_model_nr) - 1
      Speed_exponential(current_model_nr) = I1
      Enter231
   End If
End Macro

'Speed Normal/reverse
Macro Menu241
   If M_button_pressed = M_up Then
      Ml3 = 0
      Enter240
   Elseif M_button_pressed = M_left Or M_button_pressed = M_right Then
      If Speed_r_setting(current_model_nr) = 0 Then
         Speed_r_setting(current_model_nr) = 1
      Else
         Speed_r_setting(current_model_nr) = 0
      End If
      Enter241
   End If
End Macro

Macro Menu311
   If M_button_pressed = M_up Then
      Ml3 = 0
      Enter310
   End If
End Macro

'The Menu system here below is where you come when the menu navigation button has been pressed
'It decides based on where you are in te menu (= what the values for Ml1, Ml2 and Ml3 are) what menu blok is active
'It the takes us to that menu block and in that menu block we decide based on what menu button was pressed waht action is needed

Menu_system:
   If Ml1 = 0 Then
      Menu000
   Elseif Ml1 = 1 Then
      If Ml2 = 0 Then
         If Ml3 = 0 Then
            Menu100
         End If
      Elseif Ml2 = 1 Then
         If Ml3 = 0 Then
            Menu110
         Elseif Ml3 = 1 Then
            Menu111
         End If
      Elseif Ml2 = 2 Then
         If Ml3 = 0 Then
            Menu120
         Elseif Ml3 = 1 Then
            Menu121
         End If
      Elseif Ml2 = 3 Then
         If Ml3 = 0 Then
            Menu130
         Elseif Ml3 = 1 Then
            Menu131
         End If
      Elseif Ml2 = 4 Then
         If Ml3 = 0 Then
            Menu140
         Elseif Ml3 = 1 Then
            Menu141
         End If
      End If
   Elseif Ml1 = 2 Then
      If Ml2 = 0 Then
         If Ml3 = 0 Then
            Menu200
         End If
      Elseif Ml2 = 1 Then
         If Ml3 = 0 Then
            Menu210
         Elseif Ml3 = 1 Then
            Menu211
         End If
      Elseif Ml2 = 2 Then
         If Ml3 = 0 Then
            Menu220
         Elseif Ml3 = 1 Then
            Menu221
         End If
      Elseif Ml2 = 3 Then
         If Ml3 = 0 Then
            Menu230
         Elseif Ml3 = 1 Then
            Menu231
         End If
      Elseif Ml2 = 4 Then
         If Ml3 = 0 Then
            Menu240
         Elseif Ml3 = 1 Then
            Menu241
         End If
      End If
   Elseif Ml1 = 3 Then
      If Ml2 = 0 Then
         If Ml3 = 0 Then
            Menu300
         End If
      Elseif Ml2 = 1 Then
         If Ml3 = 0 Then
            Menu310
         Elseif Ml3 = 1 Then
            Menu311
         End If
      End If
   End If
   Return
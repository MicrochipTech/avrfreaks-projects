'(*********************************************************************************************************************************
                 TRANSMITTER V4_0.BAS
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
  |  � PPM             the connections are as indicated here on the left.
  |  � +5V
  |  � +8V                PPM is the Positive Pulse Position Signal
  |  � GND                + 5V is a stabelized 5V tention
  |  � To Antenna         + 8V to this you connect your main battery (8 * 1.5 = 12V is OK too ).
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

Version 4_0 (The elementary two channel transmitter with Normal/Reverse)
Reads the potentiometer for the speed and the steering stick on the transmitter.
Reads the fixed programmed Steer_reverse (yes or no) and Speed_reverse (yes or no) setting.
Calculates the pulse lengths.
Generates the Pulse Position Modulation signal.

Version 6_0 (The elementary two channel transmitter with Normal/Reverse and Trim)
Reads the potentiometer for the speed and the steering stick on the transmitter.
Reads the potentiometer for the speed trim and the steering trim on the transmitter.
Reads the fixed programmed Steer_reverse (yes or no) and Speed_reverse (yes or no) setting.
Calculates the pulse lengths.
Generates the Pulse Position Modulation signal.

Version 8_0 (The elementary two channel transmitter with Normal/Reverse, Trim and fixed programmed maximum and Sub Trim adjustment)
Reads the potentiometer for the speed and the steering stick on the transmitter.
Reads the potentiometer for the speed trim and the steering trim on the transmitter.
Reads the fixed programmed values for steer maximum, steer minimum, speed maximum, speed minimuml. (Neutral = subtrim)
Reads the fixed programmed Steer_reverse (yes or no) and Speed_reverse (yes or no) setting.
Calculates the pulse lengths.
Generates the Pulse Position Modulation signal.

Version 10_0 (The two channel NOT in the field programmable single model transmitter)
Reads the potentiometer for the speed and the steering stick on the transmitter.
Reads the potentiometer for the speed trim and the steering trim on the transmitter.
Reads the fixed programmed values for steer maximum, steer minimum, speed maximum, speed minimuml. (Neutral = subtrim)
Reads the fixed programmed values for steer and speed exponentials.
Reads the fixed programmed Steer_reverse (yes or no) and Speed_reverse (yes or no) setting.
Calculates the pulse lengths.
Generates the Pulse Position Modulation signal.

Version 12_0 (The two channel field programmable single model transmitter)
Reads the potentiometer for the speed and the steering stick on the transmitter.
Reads the potentiometer for the speed trim and the steering trim on the transmitter.
Reads the fixed programmed values for steer maximum, steer minimum, speed maximum, speed minimuml. (Neutral = subtrim)
Reads the fixed programmed values for steer and speed exponentials.
Reads the fixed programmed Steer_reverse (yes or no) and Speed_reverse (yes or no) setting.
Calculates the pulse lengths.
Generates the Pulse Position Modulation signal.
Menu system that enables the user program in the field the values for
- Turn_normal_reverse, Turn_subtrim, Turn_high_max, Turn_low_max, Turn_exponential
- Speed_normal_reverse, Speed_subtrim, Speed_high_max, Speed_low_max, Speed_exponential

Version 14_0 (The two channel field programmable multiple model transmitter)
Same as version 12 PLUS
Selector switch that enables the user to select between 9 models that all have their own settings.
But the names of the models are fixed programmed in the code : Model 1, Model 2, ... Model 9

Version 16_0 (The two channel field programmable multiple model transmitter)
Same as version 14 PLUS
The names for the models can be field programmed by the user to any 10 character name

Version 18_0
Skipped on purpose to accomodate for eventual changes in the above numbering and still be able to start with the first version
with a timer as version 20_0

Version 20_0
Same as previous version PLUS
A lap counter

Version 22_0 to 28_0
Skipped on purpose to accomodate for versions that have different functionality of the lab counter and still be able to start with
the first version with an automatic sequence as version 30_0

Version 30_0
Same as previous version PLUS
By pushing a button the user can trigger a sequence of events.
The purpose of this functionality would be to aim for a perfect combination of actions for a specific manouvre.

-----[CPU cycle table and timing table for 8 bit timer Timer0 when running with a 16Mhz external crystal]--------------------------

Prescale  Maximum CPU Cycles before       Precision        With a 16MHz external crystal   With 16Mhz external crystal
  Factor            Timer0 overflow     CPU Cyclse]  Maxumum time before Timer0 Overflow    Highest possible precision

      1         65 536 =     65 536               1        65 536/16 000 000 =     4 096 탎                  0,0625 탎
      8     8 X 65 536 =    524 288               8       524 288/16 000 000 =    32 768 탎                  0,5 탎
     64    64 X 65 536 =  4 194 304              64     4 194 304/16 000 000 =   262 144 탎                  4 탎
    256   256 X 65 536 = 16 777 216             256    16 777 216/16 000 000 = 1 048 576 탎                 16 탎

-----------------------------------------------------------------------------------------------------------------------------------
')

'We are using an ATmega16 with a 16MHz crystal
$regfile = "m16def.dat" : $crystal = 16000000

Config Adc = Single , Prescaler = Auto , Reference = Avcc : Start Adc

'We use TIMER1 to generate the Pulse Place Modulation Signal
Config Timer1 = Timer , Prescale = 8 : On Timer1 Timer1_isr : Stop Timer1
Enable Timer1 : Enable Interrupts

'Here we indicate what transmitter controll is connecetd to what AVR-pin
Ppm_output_pin Alias Portc.6 : Config Ppm_output_pin = Output
Get_steerstick_position Alias Getadc(0)
Get_speedstick_position Alias Getadc(2)
'Here we enter the values that are linked to the mechanics of the transmitter
Const Steerstick_lowest = 108
Const Steerstick_neutral = 518
Const Steerstick_maximum = 904
Const Speedstick_lowest = 126
Const Speedstick_neutral = 378
Const Speedstick_maximum = 893
'Here we calculate values that are linked to the mechanics of the transmitter
Const Steerstick_lower_range = Steerstick_neutral - Steerstick_lowest
Const Steerstick_upper_range = Steerstick_maximum - Steerstick_neutral
Const Speedstick_lower_range = Speedstick_neutral - Speedstick_lowest
Const Speedstick_upper_range = Speedstick_maximum - Speedstick_neutral
'Here we set values that later on will need to come from the EEPROM
Const Steer_r_setting = 1
Const Speed_r_setting = 1

Dim B1 As Byte , Count As Byte , Ch(3) As Word , W1 As Word , L1 As Long , Steer_r As Bit , Speed_r As Bit

Steer_r = Steer_r_setting
Speed_r = Speed_r_setting
Count = 1
For B1 = 1 To 2
   Ch(b1) = 500
Next
Start Timer1

Do
   L1 = Get_steerstick_position
   If L1 < Steerstick_lowest Then L1 = Steerstick_lowest
   If L1 > Steerstick_maximum Then L1 = Steerstick_maximum
   L1 = L1 - Steerstick_neutral
   L1 = L1 * 500
   If L1 < 0 Then L1 = L1 / Steerstick_lower_range Else L1 = L1 / Steerstick_upper_range
   If Steer_r = 0 Then Ch(1) = L1 + 500 Else Ch(1) = 500 - L1

   L1 = Get_speedstick_position
   If L1 < Speedstick_lowest Then L1 = Speedstick_lowest
   If L1 > Speedstick_maximum Then L1 = Speedstick_maximum
   L1 = L1 - Speedstick_neutral
   L1 = L1 * 500
   If L1 < 0 Then L1 = L1 / Speedstick_lower_range Else L1 = L1 / Speedstick_upper_range
   If Speed_r = 0 Then Ch(2) = L1 + 500 Else Ch(2) = 500 - L1
Loop

End

Timer1_isr:
   If Count = 1 Or Count = 3 Or Count = 5 Then
      Reset Ppm_output_pin
      Timer1 = 64752
   Elseif Count = 2 Or Count = 4 Then
      Set Ppm_output_pin
      B1 = Count / 2 : W1 = Ch(b1) * 2 : Timer1 = 63576 - W1
   Elseif Count = 6 Then
      Set Ppm_output_pin
      W1 = 0
      For B1 = 1 To 2
         W1 = W1 + Ch(b1)
      Next
      W1 = 15800 - W1 : W1 = W1 * 2 : Timer1 = 63536 - W1
   End If
   Incr Count : If Count = 7 Then Count = 1
Return
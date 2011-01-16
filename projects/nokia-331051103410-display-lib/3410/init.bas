'===============================================================================
'Library for the Nokia 3410 display for BASCOM-AVR

'This is v2.0 with support for 96*65 bitmap images.

'Library creation and various modifications, additions, and bugfixes by Niclas Arndt in March 2007, modified by Joey in januari 2011.
'including a revised and extended character set and bitmap image printing.

'Included in the .zip file is also an MS Excel 2003-based character generator.
'Simply write "1" to each active pixel and copy/paste the
'resulting "Data &Hxx , &Hxx , &Hxx , &Hxx , &Hxx" line to the end section of 3410end.bas
'and add a corresponding "Case "x" : Restore x" line in the middle section.
'Please read the instruction for Excel toolpak installation in the Excel document.
'The installation package is automatically installed from some MS download site over the internet.

'Please send any comments to user name Kimmeridgien at www.avrfreaks.net forum or www.mcselec.com forum.

'Based on code written by Michael K�nig
'http://www.mkprojekt.de, in June 2006

'My thanks to Michael for publishing the base code on his web site!

'===============================================================================

'Library developed on 1.11.8.3 demo version.
'With soft SPI on an ATMega8 it requires almost 4 kB of program code space.
'You may want to comment out those "Case "x" : Restore x" lines that you don't need to make room for your own code.

'===============================================================================

'It is essential that you include 3410init.bas at the beginning of your main program
'and 3410end.bas at the end, as the data rows must be at the end or it will be executed
'as program code.

'It is possible to use the EEPROM to store the character data instead of the flash ROM.
'Please consult the BASCOM-AVR Help menu for this.

'-------------------------------------------------------------------------------

'This library requires that you add the following (uncommented) lines to
'the BEGINNING of your main program:

'$framesize = 54              'This library needs at least a 54 bytes framesize. Increase further for your main program.

'$include "3410init.bas"      'Please note: At the BEGINNING!

'Config Portd.7 = Output      '(Replace by your actual port.pin)
'D3410Dc Alias Portd.7

'Config Portb.0 = Output      '(Replace by your actual port.pin)
'D3410Ce Alias Portb.0

'Config Portb.7 = Output      '(Replace by your actual port.pin)
'D3410Re Alias Portb.7

'Configure SPI in soft mode   (Replace by your actual ports.pins)
'SPI hard mode also works of course
'Config Spi = Soft , Din = Portb.1 , Dout = Portd.6 , Ss = None , Clock = Portd.5

'Initialise Spi
'Spiinit

'You also must call these initialisation subs at the beginning of your main program:
'Call D3410reset
'Call D3410initialisation
'Call D3410clear

'-------------------------------------------------------------------------------

'At the END of your program you must add the following line:

'$include "3410end.bas"        'Please note: At the END!
'$include "3410bmp1.bas"       'This is only if you are printing a bitmap image.

'The files 3410init.bas and 3410end.bas must be placed in the same folder as your
'main program .bas file.

'===============================================================================

'Example of how you talk to the display:
'It has 7 rows and 15 character positions.

'Call D3410position(0 , 0)                   'Top left corner
'Call D3410print("HELLO WORLD")

'Call D3410position(0 , 1)                   'Second row, leftmost position
'Call D3410print("HELLO SWEDEN")

'Call D3410position(16 , 7)                  'Last row, rightmost position
'Call D3410print(".")

'To clear the display:
'Call D3410clear

'To invert the display:
'Call D3410invert

'To uninvert the display:
'Call D3410uninvert

'-------------------------------------------------------------------------------

'Command listing for controlling the display:
'Please note that this is nothing you should normally worry about.
'It has been included for reference and improved understanding.

' &H20 Normal command mode, horizontal addressing
' &H22 Normal command mode, vertical adressing

' &H80 Set RAM to X address 0 (For a certain position, add 0 to 83 in increments of 6, i.e. 0,6,12,18,24,30,36,42,48,54,60,66,72,78)
' &H40 Set RAM to Y address 0 (For a certain position, add 0 to 6)

' &H0C Uninverted display
' &H0D Inverted display
' &H08 All pixels off
' &H09 All pixels on

' &H21 Extended command mode, horizontal addressing
' &H23 Extended command mode, vertical adressing

' &HC8 Middle contrast (only in extended mode)
' &H06 Temperature selection (only in extended mode)
' &H13 Bias 1:48 (optimal) (only in extended mode)

' &H24 LCD off (power-down mode)
' &H25 LCD off (power-down mode)
' &H26 LCD off (power-down mode)
' &H27 LCD off (power-down mode)

'===============================================================================

'The connections on the Nokia 3410 display are:
'Looking at the back. Connector up. From left to right.
'1. VCC     Connect this to the 2.7 - 3.3 V power supply
'2. SCK     Connect this to the SPI SCK line
'3. SDIN    Connect this to the SPI MOSI line
'4. D/C!    Connect this to any pin and refer to it as D3410dc
'5. SCE!    Connect this to any pin and refer to it as D3410ce
'6. GND     Connect this to the power supply GND
'7. VLCD    Connect a capacitor of about 4.7uF between this pin and GND
'8. RES!    Connect this to any pin and refer to it as D3410re

'===============================================================================

'Define subs
Declare Sub D3410reset
Declare Sub D3410init
Declare Sub D3410clear
Declare Sub D3410invert
Declare Sub D3410uninvert
Declare Sub D3410commandout(byval D3410value As Byte)
Declare Sub D3410dataout(byval D3410value As Byte)
Declare Sub D3410bmpout
Declare Sub D3410print(byval D3410string As String)
Declare Sub D3410position(byval D3410x As Byte , Byval D3410y As Byte)

'Define a variable for the quotation mark "
Dim D3410quotation As String * 1
D3410quotation = Chr(34)
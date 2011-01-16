'===============================================================================
'Library for the Nokia 3310 display for BASCOM-AVR


'Library creation and various modifications, additions, and bugfixes by Niclas Arndt in March 2007,
'including a revised and extended character set.

'Included in the .zip file is also an MS Excel 2003-based character generator.
'Simply write "1" to each active pixel and copy/paste the
'resulting "Data &Hxx , &Hxx , &Hxx , &Hxx , &Hxx" line to the end section of 3310end.bas
'and add a corresponding "Case "x" : Restore x" line in the middle section.
'Please read the instruction for Excel toolpak installation in the Excel document.
'The installation package is automatically installed from some MS download site over the internet.

'Please send any comments to user name Kimmeridgien at www.avrfreaks.net forum or www.mcselec.com forum.

'Based on code written by Michael König
'http://www.mkprojekt.de/3310.php, in June 2006

'My thanks to Michael for publishing the base code on his web site!

'===============================================================================

'Library developed on 1.11.8.3 demo version.
'With soft SPI on an ATMega8 it requires almost 4 kB of program code space.
'You may want to comment out those "Case "x" : Restore x" lines that you don't need to make room for your own code.

'===============================================================================

'It is essential that you include 3310init.bas at the beginning of your main program
'and 3310end.bas at the end, as the data rows must be at the end or it will be executed
'as program code.

'It is possible to use the EEPROM to store the character data instead of the flash ROM.
'Please consult the BASCOM-AVR Help menu for this.

'-------------------------------------------------------------------------------

'This library requires that you add the following (uncommented) lines to
'the BEGINNING of your main program:

'$framesize = 52              'This library needs at least a 52 bytes framesize. Increase further for your main program.

'$include "3310init.bas"      'Please note: At the BEGINNING!

'Config Portd.7 = Output      '(Replace by your actual port.pin)
'D3310Dc Alias Portd.7

'Config Portb.0 = Output      '(Replace by your actual port.pin)
'D3310Ce Alias Portb.0

'Config Portb.7 = Output      '(Replace by your actual port.pin)
'D3310Re Alias Portb.7

'Configure SPI in soft mode   (Replace by your actual ports.pins)
'SPI hard mode also works of course
'Config Spi = Soft , Din = Portb.1 , Dout = Portd.6 , Ss = None , Clock = Portd.5

'Initialise Spi
'Spiinit

'You also must call these initialisation subs at the beginning of your main program:
'Call D3310reset
'Call D3310initialisation
'Call D3310clear

'-------------------------------------------------------------------------------

'At the END of your program you must add the following line:

'$include "3310end.bas"        'Please note: At the END!

'The files 3310init.bas and 3310end.bas must be placed in the same folder as your
'main program .bas file.

'===============================================================================

'Example of how you talk to the display:
'It has 6 rows and 14 character positions.

'Call D3310position(0 , 0)                   'Top left corner
'Call D3310print("HELLO WORLD")

'Call D3310position(0 , 1)                   'Second row, leftmost position
'Call D3310print("HELLO SWEDEN")

'Call D3310position(13 , 5)                  'Last row, rightmost position
'Call D3310print(".")

'To clear the display:
'Call D3310clear

'To invert the display:
'Call D3310invert

'To uninvert the display:
'Call D3310uninvert

'-------------------------------------------------------------------------------

'Command listing for controlling the display:
'Please note that this is nothing you should normally worry about.
'It has been included for reference and improved understanding.

' &H20 Normal command mode, horizontal addressing
' &H22 Normal command mode, vertical adressing

' &H80 Set RAM to X address 0 (For a certain position, add 0 to 83 in increments of 6, i.e. 0,6,12,18,24,30,36,42,48,54,60,66,72,78)
' &H40 Set RAM to Y address 0 (For a certain position, add 0 to 5)

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

'The connections on the Nokia 3310 display are:
'Looking at the back. Connector up. From left to right.
'1. VCC     Connect this to the 2.7 - 3.3 V power supply
'2. SCK     Connect this to the SPI SCK line
'3. SDIN    Connect this to the SPI MOSI line
'4. D/C!    Connect this to any pin and refer to it as D3310dc
'5. SCE!    Connect this to any pin and refer to it as D3310ce
'6. GND     Connect this to the power supply GND
'7. VLCD    Connect a capacitor of about 4.7uF between this pin and GND
'8. RES!    Connect this to any pin and refer to it as D3310re

'===============================================================================

'Define subs
Declare Sub D3310reset
Declare Sub D3310init
Declare Sub D3310clear
Declare Sub D3310invert
Declare Sub D3310uninvert
Declare Sub D3310commandout(byval D3310value As Byte)
Declare Sub D3310dataout(byval D3310value As Byte)
Declare Sub D3310print(byval D3310string As String)
Declare Sub D3310position(byval D3310x As Byte , Byval D3310y As Byte)

'Define a variable for the quotation mark "
Dim D3310quotation As String * 1
D3310quotation = Chr(34)
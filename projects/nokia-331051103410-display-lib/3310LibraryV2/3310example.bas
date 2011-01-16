$regfile = "m8def.dat"

'This is v2.0 with support for printing bitmap images.

$crystal = 8000000
$hwstack = 32                                               ' default
$swstack = 8                                                ' default
$framesize = 52                                             ' minimum for this library. Increase for your application needs!

Dim Strstring As String * 14                                'Just used for this demo

'Define I/O and name them
Config Portd.7 = Output
D3310dc Alias Portd.7

Config Portb.0 = Output
D3310ce Alias Portb.0

Config Portb.7 = Output
D3310re Alias Portb.7

'Configure SPI in soft mode
Config Spi = Soft , Din = Portb.1 , Dout = Portd.6 , Ss = None , Clock = Portd.5

'Initialise Spi
Spiinit

$include "3310init.bas"

'Initialise the display
Call D3310reset
Call D3310init
Call D3310clear

'Main program loop
Do

   Call D3310position(0 , 0)
   Call D3310print( "Hello World")

   Call D3310position(0 , 1)
   Call D3310print( "Hello Sweden")

   Call D3310position(0 , 2)
   Strstring = D3310quotation + "Quote"
   Strstring = Strstring + D3310quotation
   Call D3310print(strstring)

   'Call D3310position(0 , 3)
   'Call D3310print(strstring)

   'Call D3310position(0 , 4)
   'Call D3310print(strstring)

   Call D3310position(11 , 5)
   Call D3310print( "END")

   Wait 1

   Restore D3310bmp1                                        'D3310Bmp1 is the label name for the first bmp
   Call D3310bmpout

   Wait 1

   'Restore D3310bmp2                                         'D3310Bmp2 is the label name for the second bmp
   'Call D3310bmpout

   'Wait 1

   Call D3310invert

   Wait 1

   Call D3310clear

   Wait 1

   Call D3310uninvert

   Wait 1

Loop

Stop

$include "3310end.bas"
$include "3310bmp1.bas"
'$include "3310bmp2.bas"
'... and so on
$regfile = "m8def.dat"

'This is v2.0 with support for printing bitmap images.

$crystal = 4000000
$hwstack = 32                                               ' default
$swstack = 8                                                ' default
$framesize = 54                                             'minimum for this library. Increase for your application needs!

Dim Strstring As String * 14                                'Just used for this demo

'Define I/O and name them
Config Portd.2 = Output
D3410dc Alias Portd.2

Config Portd.1 = Output
D3410ce Alias Portd.1

Config Portd.0 = Output
D3410re Alias Portd.0

'Configure SPI in soft mode
Config Spi = Soft , Din = Portb.1 , Dout = Portd.3 , Ss = None , Clock = Portd.4

'Initialise Spi
Spiinit

$include "3410init.bas"

'Initialise the display
Call D3410reset
Call D3410init
Call D3410clear



'Main program loop
Do

   Restore D3410bmp1                                        'D3410Bmp1 is the label name for the first bmp
   Call D3410bmpout

   Wait 1

   Call D3410invert
   Wait 1

   Call D3410uninvert
   Wait 1

   Call D3410clear

   Call D3410position(0 , 0)
   Call D3410print( "Lijn 0 1234567ab")
   Wait 1
   Call D3410position(10 , 7)
   Call D3410print( "Lijn 7")
   Wait 1

Loop

Stop

$include "3410end.bas"
$include "3410bmp1.bas"
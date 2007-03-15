$regfile = "m16def.dat"

$crystal = 8000000
$hwstack = 32                                               'Default
$swstack = 8                                                'Default
$framesize = 36                                             'Minimum for this library. Increase for your application needs!
                                                             'For the 8-character displays you must increase this value
Dim Strstring As String * 14                                'Only used for this demo
Dim Testloop As Byte                                        'Only used for this demo

'Define I/O and name them
Config Portd.1 = Output
Hcmsrs Alias Portd.1

Config Portd.3 = Output
Hcmsce Alias Portd.3

Config Portd.4 = Output
Hcmsre Alias Portd.4

'Configure SPI in soft mode
Config Spi = Soft , Din = Portd.5 , Dout = Portd.0 , Ss = None , Clock = Portd.2

'Initialise Spi
Spiinit

$include "HCMSinit.bas"

'Initialise the display
Call Hcmsreset
Call Hcmsconfigure(&H0)                                     'Initiate the display and set the LEDs to 0% brightness. (Basically what the reset does too.)
Call Hcmsclear                                              'Shift in 160 blanks. (This could also be done by Call HCMSprint("    ").
                                                            'Not really necessary here since the next Statement Loads All 4 Characters.
'Main program

Call Hcmsprint( "LOVE")                                     'Send 4 characters in one transmission for concurrent ignition

Do                                                          'Dim from 0% to 100% brightness
   For Testloop = 0 To 15
      Call Hcmsconfigure(testloop)
      Waitms 250
   Next Testloop

   Call Hcmsprint( "    ")                                  'Empty the display
   Waitms 250
   Call Hcmsprint( "L")                                     'Shift in single characters for scroll effect
   Waitms 250
   Call Hcmsprint( "O")
   Waitms 250
   Call Hcmsprint( "V")
   Waitms 250
   Call Hcmsprint( "E")
   Waitms 250

   Call Hcmssleep                                           'Go to sleep mode
   Wait 1
   Call Hcmsconfigure(&Hf)                                  'Return from sleep mode
                                                             'Dim from 100% to 0% brightness
   For Testloop = 15 To 0 Step -1
      Call Hcmsconfigure(testloop)
      Waitms 250
   Next Testloop
                                                             'This is how you print the quotation mark "
   'Strstring = Hcmsquotation + "Quote"
   'Strstring = Strstring + Hcmsquotation
   'Call Hcmsprint( Strstring)

Loop

Stop

$include "HCMSend.bas"
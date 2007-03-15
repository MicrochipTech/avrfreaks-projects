'===============================================================================
'Library for the HP/Agilent/Avogo HCMS-29xx display for BASCOM-AVR


'Library creation and various modifications, additions, and bugfixes by Niclas Arndt in March 2007,
'including a revised and extended character set.

'Included in the .zip file is also an MS Excel 2003-based character generator.
'Simply write "1" to each active pixel and copy/paste the
'resulting "Data &Hxx , &Hxx , &Hxx , &Hxx , &Hxx" line to the end section of 3310end.bas
'and add a corresponding "Case "x" : Restore x" line in the middle section.
'Please read the instruction for Excel toolpak installation in the Excel document.
'The installation package is automatically installed from some MS download site over the internet.

'Please send any comments to user name Kimmeridgien at www.avrfreaks.net forum or www.mcselec.com forum.

'Based on code for the Nokia 3310 display written by Michael König
'http://www.mkprojekt.de/3310.php, in June 2006

'My thanks to Michael for publishing the base code on his web site!

'===============================================================================

'Library developed on 1.11.8.3 demo version.
'With soft SPI on an ATMega16 it requires about 4.6 kB of program code space.
'You may want to comment out those "Case "x" : Restore x" lines that you don't need to make room for your own code.

'===============================================================================

'It is essential that you include HCMSinit.bas at the beginning of your main program
'and HCMSend.bas at the end, as the data rows must be at the end or it will be executed
'as program code.

'It is possible to use the EEPROM to store the character data instead of the flash ROM.
'Please consult the BASCOM-AVR Help menu for this.

'-------------------------------------------------------------------------------

'This library requires that you add the following (uncommented) lines to
'the BEGINNING of your main program:

'$framesize = 36              'This library needs at least a 36 bytes framesize. Increase further for your main program.
                              'For 8-character displays you must also increase this value!

'$include "HCMSinit.bas"      'Please note: At the BEGINNING!

'Config Portd.7 = Output      '(Replace by your actual port.pin)
'HCMSRs Alias Portd.7

'Config Portb.0 = Output      '(Replace by your actual port.pin)
'HCMSCe Alias Portb.0

'Config Portb.7 = Output      '(Replace by your actual port.pin)
'HCMSRe Alias Portb.7

'Configure SPI in soft mode   (Replace by your actual ports.pins)
'SPI hard mode also works of course
'Config Spi = Soft , Din = Portb.1 , Dout = Portd.6 , Ss = None , Clock = Portd.5

'Initialise Spi
'Spiinit

'You also must call these initialisation subs at the beginning of your main program:
'Call HCMSreset
'Call HCMSconfigure(&Hx)    'Where the hex number is a value between 0 and F for brightness,
                            'typically 0 until you have cleared the display
'Call HCMSclear

'-------------------------------------------------------------------------------

'At the END of your program you must add the following line:

'$include "HCMSend.bas"        'Please note: At the END!

'The files HCMSinit.bas and HCMSend.bas must be placed in the same folder as your
'main program .bas file.

'===============================================================================

'Example of how you talk to the display:
'The display in the example has 4 character positions.

'Call HCMSprint("LOVE")       'Shifts in all 4 characters before the LEDs are ignited
'Call HCMSprint("L")          'Shifts in 1 character at a time, creating a scroll effect
'Call HCMSprint("O")
'Call HCMSprint("V")
'Call HCMSprint("E")

'To clear the display: (Both alternatives give the same result
'Call HCMSclear
'Call HCMSprint("    ")       'Or 8 space signs for an 8-character display

'Call HCMSsleep               'Power-down mode
'Call HCMSconfigure(&Hx)      'Wake up after power-down mode. (x >0)
'===============================================================================


'Define subs
Declare Sub Hcmsreset
Declare Sub Hcmsconfigure(byval Hcmsbrightness As Byte)
Declare Sub Hcmsclear
Declare Sub Hcmssleep
Declare Sub Hcmscommandout(byval Hcmsvalue As Byte)
Declare Sub Hcmsprint(byval Hcmsstring As String)

'Define a variable for the quotation mark "
Dim Hcmsquotation As String * 1
Hcmsquotation = Chr(34)
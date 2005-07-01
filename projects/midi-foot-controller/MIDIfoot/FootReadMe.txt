  This is a MIDI foot switch controller.  It allows the MIDI program value to be changed without moving hands away
from the piano keyboard.  Any of the 128 values can be selected with a maximum of seven foot taps.

  This controller is based on the REX MIDIbuddy MP128P controller.  I bought one on eBay that was sold as broken.
Instead of a single broken wire the entire Z80 microprocessor was burned out.  I rewrote the firmware according to
the instruction manual for the Atmel AVR.  Then I found that I could order a Z80 replacement processor from the 
REX factory for only $10 and saved many hours of programming and debugging.

  There are twelve foot switches, a bank of six DIP switches, and a dual seven-segment LED display with 100's segment.
The program values are organized in banks of ten.  The tens digit value determines the bank.  There are twelve banks,
holding all Program values from 01 to 128.  The first and last banks have only nine values: 01-09 and 120-128.  

  Pressing any of the pedals marked 0 to 9 will change the MIDI program value in the current bank to the selected digit.
Pressing the 10Up or 10Down pedal will change the bank number.  The bank numbers roll over from 12>0 and 0>12.
A press on the 10Up or 10Down pedal will display a dash in the 10s digit place and will not send a program change
MIDI message until a digit pedal number is pressed.

  Selecting Program #129 sends MIDI stop (0xFC) and displays 'SP'.   Selecting Program #0 sends MIDI start (0xFA)
and displays 'Sr'.  There is no MIDI program change sent with these commands.

  There are six DIP switches.   Each switch is pulled high with a 22k ohm resistor when open and is connected to ground 
when closed.    DIP switches 1 to 4 select the MIDI channel.  They are encoded as a binary value and put reverse logic
onto the AVR port pins.  MIDI channel one will read as '1 1 1 1' and channel 16 will read as '0 0 0 0'.   DIP Switch 5
determines whether the controller will send MIDI program change only or MIDI program change and Song Select 
messages.  The MIDI song select command is 0xF3 followed by 0-127.  The new Program Change value gets sent as the
new song select data.  An open (up) DIP switch 5 will send MIDI program change only.  
   DIP switch 6 selects whether the LED display will show MIDI program values 01 to 128 (default; switch 
closed), or 00 to 127 (switch open).  In the current firmware, the pedal selection will be one value higher than the 
display when the 127 display option is active.   Pressing pedal one will display '(bank#) 0'.

  The pedal switches are multiplexed with the LED segment outputs.  The pedals are read about six times a second and
the LED digits are multiplexed about 60 times a second to avoid flicker.  The code waits about 1/6 second after detecting
a pedal down before testing for a pedal up.  This debounces the switch presses.

  The schematic file is in GIF format.  It was done in an old DOS OrCAD Schematic Capture version from 1990.  To get
the GIF file,  I ran OrCAD in Windows DOS from the command line.  Then I suspended the DOS session with Alt-Enter.
Alt-PrintScreen copied the window into the clipboard.  Then a graphics image program is opened, and the clipboard pasted
(Control-V) as a new image.  The colors are reduced to black and white and the entire image reversed.  This leaves
a section of the OrCAD image as a white background image that is saved as a GIF.  It takes several loops to get the 
entire schematic with this technique, but simple printing is often impossible because the old OrCAD printer drivers
rarely work with modern printers.

  The original MIDIbuddy design has 10K resistors between U2 outputs and the CPU port pins.  This worked OK with
the original Z80 processor.   However the AVR I/O ports could not read a logic low on the pins.  When I changed the 
value to 1K ohms,  the circuit worked and the AVR ports could read the logic low from the 74HC147 output pins.  
It took a long time to figure this out.


Alan Probandt
Portland, Oregon USA    June 2005
alan_probandt@yahoo.com
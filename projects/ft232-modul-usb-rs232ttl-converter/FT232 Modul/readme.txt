Modul with FT232 USB<->RS232(TTL) converter

Features:
- single layer board (easy for self-etching), but with 6 wire bridges on top
- bus-powered or self powered configuration via jumper
- EEPROM for configuring your own product description message strings/vendor id etc.
- most important signals available on 14pol connector
- TTL/CMOS signals for easy interfacing with microcontrollers
- in Germany, you can buy nearly all parts by Reichelt Elektronik (www.reichelt.de).
  Partlists with ordering numbers for Reichelt in Excel/Open Office Calc format
  are included in the project directory

For viewing/printing the schematic and layoutfile you need the freeware version of 
Eagle 4.1 from www.cadsoft.de. Press the Ratsnest-Button before printing the Layout
to fill the polygons.

Notes:
Powering over USB: JP1 closed, JP2 2-3
Externel power: JP1 open, JP2 1-2

Simple loopbacktest:
By putting 3 jumpers on the 14pol connector a simple loopback test is possible. 
Short VCC with VCCIO (pin 1-2), short RTS with CTS (pin 5-6), short TXD with RXD (pin 3-4).
Install VCP-drivers from www.ftdichip.com for easy using this device with
a simple virtual COM-port under windows. I recommend the freeware Bray's Terminal
from http://bray.velenje.cx/avr/terminal for playing around with the virtual COM-port.

For simple operation at 5V (only RxD, TxD and GND) connect RTS with CTS and VCC with VCCIO. 



Disclaimer:
I declare this project under the Public Domain, so you can do whatever you want to do
with it, even using it for commercial purposes without having paying me for it. 
This project is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
PARTICULAR PURPOSE. If you kill your usb-controller or burn down your PC using this project
I am not responsible for that. You should have at least basic knowledge in electronics when
you are building and hooking up your own hardware to your PC. I can't guarantee any support
for this project.

However, feedback is welcome. If you successfully build this project, feel free to let me
know about it. I build and tested the modul and it worked as expected for me. If you have
trouble to get your converter running or when you find any bugs in it you can contact me
too.

EMail at lutz. lisseck at gmx. de (write me in german or english).

Lutz Lisseck, 2005.





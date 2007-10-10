Here is some working code for use with the RF-24G modules.
It comes from many different fragments of code I downloaded on the internet,
pieced together to make it work and a few routines added.

You can buy the modules here:

in Canada
http://store.qkits.com/moreinfo.cfm/TXRX24G
(ONLY $12.95!)

or here:
IN USA
http://www.sparkfun.com/commerce/product_info.php?products_id=151

Included is a schematic (please forgive my schematic making skills)
I have yet to find a decent schematic editor for windows.
I left out the power supply circuit. The entire circuit runs on 3.3V.
(see the parts list for part numbers)

Also included is a schematic for a programming cable using avreal in
straight DOS or dos window in win98. There is a version for WIN XP
but you need a special I/O driver.

The circuit uses an atmega8515L at 3.3Volts. If you connect 5V to the RF24G, you 
can damage the device. Please read the datasheet before attempting to build this project.
(not my problem if you smoke your RF24G module)

Just program two atmega8515's with the TX.hex code. Connect the device to a terminal 
running @ 9600 baud. Simply type any character in a terminal
to invoke transmission. RF24G will transmit "0123456789" to the other RF24G module, 
and print to terminal. RF24G will return to receive mode after transmitting.

Why did I do this?
Because I couldn't find decent working code that would easily switch from TX back to RX. 
Most of what I downloaded didn't work, or would never compile without 
100 errors! This should give someone a head start if they would like to learn RF.

Disclaimer:
I do not provide any kind of warranty, use AS IS!

Good LUCK!


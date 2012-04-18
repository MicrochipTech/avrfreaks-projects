# Atmega8 small datalogger

Uploaded by domenico on 2012-04-18 08:16:00 (rating 0 out of 5)

## Summary

This is a simple datalogger developed mainly to collect thermal data in laboratory use, interfacing with LM35 sensors; normal voltage acquisition up to 20V is possible.Input selection between 20V, 2V, 0.2V can be done via jumper setting. A 7segment display let you control temperature in progress, while saving on internal eeprom let you have a log of the test. Eeprom content can be downloaded via a serial connection, the channel is a 5V-level RS232 to be connected to a usb to serial converter based on FT232 chip. Internal eeprom is used, but it's possible to add a 24c256eeprom ( yet not software supported ) Power supply can be from USB or external 5V.  

Some pushbutton permit simple selectng of input and mode explained briefly in .c file. Some text and variable name are in italian.... sorry.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)

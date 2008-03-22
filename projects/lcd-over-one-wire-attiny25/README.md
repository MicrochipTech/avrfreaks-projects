# LCD over one wire with ATtiny25

Uploaded by danni on 2008-03-21 22:29:00 (rating 0 out of 5)

## Summary

A standard LCD need at least 6 wires to the micro.  

If not enough wires available or a LCD should be controlled over RS232, then only an ATtiny25 was sufficient to control the LCD.


The command interface was quite simple:


For autobauding the byte 0x0D must be sent.  

Then a 'c' followed by the command byte send commands to the LCD. Alternatively a 'C' followed by two hex digits ('0..1,a..f,A..F') can be used.


To write data, send the data stream after 'd'. The stream can be finished by a 0 or 0x0A or 0x0D byte.  

Alternatively a 'D' followed by two hex digits ('0..1,a..f,A..F') per data byte can be used. The stream can be finished by any non hex digit.


The UART input pin was shared with a LCD output pin over a resistor.  

You can select direct (TTL) or inverted (RS-232) input.


For testing this program it should be recommended to istall my bootloader at first, since all 6 IO pins must be used. Then the bootloader can still reprogram the ATtiny25, even if reset was disabled.


Peter

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)

# midget board

Uploaded by andrewbenson on 2007-03-15 16:16:00 (rating 0 out of 5)

## Summary

The MIDget board is a simple project to provide 8-channels of ADC-->MIDI conversion to control real-time media projects. It uses the ATTiny26, which has a bunch of ADC channels, but no UART, so the UART/MIDI functions were implemented using example code from the Atmel USI\_UART Application Note, but ported to work with GCC. The port was fairly simple, mostly involving interrupt syntax tweaks. The rest of the code is just a basic ADC setup and conversion and then formatting of MIDI CC messages. For my uses, I set it up to send the full 10-bit value by using 2 separate CC messages. This is then put back together in Software on my computer.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)

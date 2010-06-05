# LCD MIDI-IN data display

Uploaded by Simonetta on 2010-06-04 18:13:00 (rating 0 out of 5)

## Summary

 Received MIDI is placed in the SRAM input buffer. MIDI clock (0xf8)and Active Sense (0xfe) are filtered out. All MIDI input bytes are immediately re-transmitted to MIDI-Out without filtering. New MIDI values displayed on fourth LCD line. Each new byte has two hex-char display of the buffer offset for the data, followed by colon and six data bytes (two hex chars and space). After six bytes, lines 1-3 scroll up to lines 0-2 with same values. Line 3 is cleared andupdated buffer offset displayed at left.


When a potentiometer is moved to <+2.6v of +5v, the LCD screen is filled with previously received MIDI bytes. These bytes are displayed as two hex ASCII characters, six per LCD line, 24 bytes total, are displayed on the 20x4 LCD. The SRAM buffer is 240 bytes in size. This is 40 rows of 6 bytes each. The value of the ADC (less than 37) is the row number displayed at the LCD top left. For example, ADC setting of 0x1a (0x3f max value) would display 24 bytes from the SRAM MIDI input buffer starting at 156 (26 * 6). 


 There are three buttons: one halts the display of new MIDI data. One restarts it. One resets the buffer to zero.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler

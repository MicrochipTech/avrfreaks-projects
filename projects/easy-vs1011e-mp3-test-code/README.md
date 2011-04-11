# Easy vs1011e MP3 Test Code

Uploaded by newbie123 on 2011-04-11 02:42:00 (rating 0 out of 5)

## Summary

This project includes all of the code you will need to test a vs1011e mp3 decoder. The included hex file contains the MP3 file of a woman saying "hi" repeatedly every 2 seconds.


I am using a Sparkfun vs1011e breakout board with an atmega8515L running on the internal 8MHz clock. The atmega is running at 3V so there is no need for a logic level conversion circuit.


The circuit does not need external memory because the MP3 file is stored in the flash memory of the atmega8515 making it easy to breadboard and test.


You can hook up a set of headphones or a computer speaker with a built in amplifier to hear the sound. (See the vs1011e datasheet.)


The vs1011e datasheet is here:  

[http://www.vlsi.fi/datasheets/vs...](http://www.vlsi.fi/datasheets/vs1011.pdf)


The Sparkfun board I am using is here:  

[http://www.sparkfun.com/products...](http://www.sparkfun.com/products/8819)


I also included a small utility to convert MP3 files (or any bin file) to asm format as in an include file for avrasm. Please read the readthis.txt file for all details.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler

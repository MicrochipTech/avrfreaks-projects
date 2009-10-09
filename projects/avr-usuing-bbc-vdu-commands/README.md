# AVR usuing BBC VDU Commands

Uploaded by joeyAVR on 2009-10-09 09:36:00 (rating 0 out of 5)

## Summary

This isn't BBC Basic - the VDU command set on the old BBC micros was part of the O/S and common to all languages. Stick an RS232 cable from a PC to a BBC micro and with a little code you could use the BBC as your I/O terminal (though why you should want to...)


BUT


It provides a dead simple byte-stream way of controlling graphic I/O, and is well suited to microcontroller apps with relatively modest displays. This is set up to use the common twin KS0801 driven displays.


There's a demo at: 


[http://www.youtube.com/watch?v=q...](http://www.youtube.com/watch?v=q_Seq9-Oz7s) 


You can do lines, filled and outline circles,triangles and rectangles, have separate graphic and text windows, with windowed text scrolling, there are three native font sizes. You can print text at the graphic cursor or the text cursor, read characters from the screen, define your own characters etc. etc.


At the moment the code is set up to receive and transmit any responses via SPI, but the tweaks to use RS232 should be obvious.


Hope the documentation is self-replanatory.


Joey

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4

I named this an STK505 clone, but it's really just an adapter board that plugs into
an STK500 to use the ATtiny24/44/84 devices in the 14-SOIC package.  See the
STK505 user guide for instructions on how to use the adapter for High Voltage
Serial Programming.

Build hints

1) This board is made using the toner transfer procedure and therefore does not
have plated through holes.  Because of this many components need to be soldered
on both sides of the board.

2) Begin by installing the XTAL2 header.  Solder all three pins on the bottom of the
board.  Gently lift the black plastic spacer up to the top of the pins and sparingly
solder pin 1 on the top.

3) Following the same procedure mount the XTAL1 and RESET headers.

4) Using the same procedure as in #1, mount the ISP header, then finally the HV
programming header

5) Mount teh bypass cap on the bottom of the board and solder both top and bottom
sides.  Clip the leads on the top very close to the board because the SOIC socket
needs to fit over these solder points.

6) Use turned pin IC socket headers, like in the photo of the bottom of the board.
This facilitates soldering on both sides of the board.  Before loading onto the board,
clip pins 7, 8, 13, 14, 27, and 28 off the top side of the header.  This will allow
the SOIC socket to fit flush.

7) Place the headers in the board and load the board into a 40 pin IC socket to
help align the pins (the socket on the STK500 works fine if you solder quickly).
Solder the headers to the top side of the board.

8) Solder the pins on the bottom of the board where there is a trace connection
(pins 1, 2, 3, 9, 32, 34, 35, 36, 37, 38, and 39).

10) Clip pins 33 and 34 very close on the to of the board so the SOIC socket can
seat correctly.

11) The socket I used, an Enplas OTC-16-03, needs a litte modification before mounting.
I could not find a 14 pin socket so I disassembled the 16 pin socket and removed the
contact fingers for pins 8 and 9 in a 16 pin socket.  Reassemble the socket and
cut the two locating pins off the bottom.  File the remaining protrusions flush with
the bottom of the socket and file the standoff boss that will end up over pins
33 and 34 off flush as well.

12) Finally solder the socket, clean the board and push the plastic pin header
strips back down to the board.

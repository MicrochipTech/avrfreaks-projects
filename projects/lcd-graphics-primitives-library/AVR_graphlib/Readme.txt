 Graphics Primitives Functions Library for ATMEL AVR

 Target: AVRs with hardware multiplier & internal RAM

 Graphics controller hardware: 2x SED1520, described in "G_LCD_Swap"
 Note: Routines are easily adaptable to similar controllers (cf. below)

 The routines are primarily optimized for speed, especially the "draw
 character" and "filled primitive" (circle, rectangle, ellipse etc.)
 functions are typically 3 to 10 times faster than compiler generated code.

 The functions preserve all registers used, so they can easily be embedded
 as asm-inline in 'high-level language' sources.

 Note:	maximum display (screen) size currently is 255x255. The functions
	assume a 'vertically aligned' and 'paged' video/graphics memory
	organisation (Epson SED136x, SED15xx family, Toshiba P90xx,
	HD61202/KS0107/KS0108, KS0713 etc.):

 * List of functions:

 Line (with clear/set/invert)
 Rectangle (outline/filled w. clear/set/invert)
 Circle (outline/filled w. clear/set/invert)
 Ellipse (outline/filled w. clear/set/invert)
 Polygon/Area Seed Fill(s) (pixel set only,
 	fastest implementation with "checkered fill" option)
 Bitmap (Icon/Picture w. clear/set/invert)
 Single Pixel R/W (w. clear/set/invert)
 Single Character (w. clear/set/invert)
 ASCIIZ String (w. clear/set/invert)
 Print BCD as ASCII (w. clear/set/invert)
 Print HEX as ASCII (w. clear/set/invert)
 Clear/Fill/Invert Screen
 Write Frame Buffer to Display (device driver for 2xSED1520, 120x32 pix)

 Three fonts (8x8 bold, 8x8 thin, 5x6 small, full IBM enxtended character
 set (code page 437) included.

 This library is an enhanced re-arrangement and somewhat 'generalized'
 compilation of modules I've written over the past two years for our
 designs.

 I've included and adapted some code (Bresenham line, print BCD/Hex) from
 Mr. Rolf Kalle's "Nokia 7110 LCD graphics library V2"; I've also
 used some of his labels and other naming conventions as Mr. Kalle's
 library actually provided the incentive to start this. By the way: in
 "Nokia 7110 ..." you'll also find a device driver for an 8-page
 96x64 organized serial display (SED 1565, used by the aforementioned
 Nokia 7110).

 I would highly appreciate any feedback from people who use the routines
 presented here.

 I'd also like to gather any additional graphics routines, bug fixes and
 especially further device drivers and code adaptions for different HW
 architectures you might happen to come up with into this archive.


 Andreas Lenze
 EDV-Systeme Lenze
 Munich, Germany 2003, 2005
 andreas.lenze@t-online.de


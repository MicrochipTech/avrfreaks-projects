/------------------------------------------\
| TV Image Generation                      |
|------------------------------------------|
| Application / API to generate images     |
|   on NTSC (American) Television          |
| Writen in Assembly intended for ATMEGA16 |
|   running at 8MHz, others later          |
|                                          |
|------------------------------------------|
| John Haskins                             |
|------------------------------------------|
| v0.3 05.02.2005                          |
|  - initial release                       |
| v0.4 06.02.2005                          |
|  - changed order of signals              |
|  - added basic bitmap support            |
|     created basic character set          |
|  - still has known bugs, working on 'em  |
| v0.4 06.02.2005                          |
|  - changed memory location, pixel 0,0    |
|     is now the top left insted of bottom |
|  - added a second example app, TVDraw    |
|     it allows you to move a flashing     |
|     pixel arround the field to draw or   |
|     erase lines, uses port d             |
|  - still has known bugs, working on 'em  |
\------------------------------------------/


TV Gen Readme

Notes on the hardware setup:
	The output signal is generated from 2 pins, these are 
	configurable in the 8MHzM16TVMacs.inc file with the following lines
		.SET VIDPORT=PORTB	; port rca jack is on
		.SET WHITEBIT=2		; bit that indicates white
		.SET BLACKBIT=0		; bit that indicates black
	Default setup is set to what i use on the STK500 Dev board,
	if you are using this leave the setup like this and take
	one of the 10 pin jumper cables, connect to PortB, then jump
	to another breadboard.  Put in a 5 pin header for the
	connection pin 1 of the cable is PortB.0 (BLACKBIT) pin 3
	(second as you move lengthwise) is PortB.2 (WHITEBIT) the
	last pin on this side of the cable, pin 9, is Ground.
	To make this work simply put a pin header in a breaboard,
	connect pin 5 (cable's pin 9) to ground, then connect a
	470 omh resister to pin 1, a 1K ohm resister to pin 2.
	Leave pin 3&4 blank or use them as ground for status led.
	The reisitors should both be jumped to another row on the
	board, that row should also include the inner RCA jack.
	The outer connection on the RCA jack should be connected to
	gound.  This external board need not be powered, so if you
	noticed pin 5 of the header is ground, so if you prefer not
	to use the rail simply connect pin 5 with RCA outter.  That's
	about it for hardware to get started.
	
	The resistor values are not exact, if you did the calculations
	you'll notice they are a little off, ideally they would be
	450 and 900 ohms respectively, the use of standard, available
	components was more important to me than a "true" black/white.
	As they stand 470/1K seems to make a pretty darn good image for
	me.  But play with them if you like.

	If you aren't using the STK500 i would immagine all you would
	need to do is set the VIDPORT, WHITEBIT, & BLACKBIT to desired
	values and connect as described above to the RCA jack.  I have
	yet to do this but plan to attempt it soon.

Notes on use:
	The image is generated based on Timer 0 compare match.  There
	is a single service routine (ISR_COMP0) that handles the
	interrupt, you'll notice that in the example I also tie this
	isr to timer0 overflow, this is a safegard in case, for some
	reason, the timer cycles; it should never do that but just in
	case.
	
	The documentation i read, a lot of it, says the HSync signals
	should be spaced 63.5-64 us appart, i had this coded and found
	the image a bit jiddery, so i have increased to 66 us to
	stabalize it, it works well for me but your tv may differ.
	
	At present i'm counting down linectr, thus pixel 0,0 is actually
	the bottom left, not the top left.  I plan on changing this soon.
	
	In this latest version there is a bug that causes the top grey
	bounding box to shift to the left.  As far as i can tell this
	is caused by the isr being called 1 ck too late, it wasn't present
	in the previous version, and only cropped up now since i changed
	the routine to give the host application more clock time.  I plan
	on fixing it but decided to releast this first.
	
	If you look through the code deeply you will also notice i'm doing
	someting rather stupid, i'm ignoring the interlacing of the video
	completely!  So you might ask if you aren't interlacing your video
	how is the television displaying it correctly, wouldn't it be all
	funky with half displaying somewhere and half somewhere else?
	Well it's a funny thing, i'm not entirely sure: my current theory
	is that the 2 lines are just matching up, so I'm writing each line,
	the same line, twice per real frame.  The first time it's writing
	the even lines, the seccond the odd ones, no special code needed as
	long as we don't want to have a vertical resolution less than 2 scan
	lines, as it stands i'm currently using 2 of these dual scan lines
	as one pixel so unless i quaduple the vertical resolution i don't
	need to worry about it, pretty cool hu?

References
	I read a lot about TV signal generation before i was able to get
	anything on the screen.  If you are porting this or want to make
	your own you might find these sources helpful, i did.


	http://www.rickard.gunee.com/projects/video/pic/howto.php
	http://instruct1.cit.cornell.edu/courses/ee476/video/
	http://www.stanford.edu/class/ee281/handouts/lab4.pdf

Still to come
	I will likely migrate to the Mega8515 from Mega16 for the external
	memory support, might also go for 16MHz crystal for better resolution.
	
	Add better bitmap support, draw it at any x coordinate instead of
	only on the bit boundries.  That could be interesting...
	
	Add drawing functions like line and rectange, we'll see how easy
	that will be.
	

This may be the lates version as is, possibly move on to video overlay project.

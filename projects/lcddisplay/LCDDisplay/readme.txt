Hi programmers,

These are my first C files i wrote for ATMEL controllers...
If there are any problems using it, write me an E-Mail and report me your problems.
I hope I can help with the most problems...
I used this files on a STK 500 board with an AT90S8515 and it works fine...
If you want updates of these file look on AVRFreaks or write me an E-Mail...

Happy programming,

Florian Hirschberg (fhirschberg@fhelectronic.de)


My website: www.fhelectronic.de


UPDATED !!

01.05.2002 :	Use "LCD_sendconststring" to display strings stored in program memory.
		The original function "LCD_sendstring" uses the SRAM for the string
		parameter. This can make your SRAM memory gets full, and you get compiler
		errors if the string parameters gets too big..

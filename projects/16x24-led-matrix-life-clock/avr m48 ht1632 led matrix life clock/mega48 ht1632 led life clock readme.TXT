copyright (c) 2008 H. Carl Ott hcarlott@gmail.com
 Now what do we have here?

 This is a simple dot matrix (16x24) led clock with some added bells and whistles.
It's based on an Atmel atmega48 processor and an LED matrix display driven by a 
HT1632 led driver chip manufactured by Holtec. Built in RTC functions, 
rotary enoder to configure options, & supercap backup. 

 The display itself is a DE-DP016 led display module from Sure Electronics 
http://www.sure-electronics.com and is/was available on Ebay as of the summer
of 2008 for less then 20 USD. If the display is unavailable, you may be able to design one around 
the HT1632 chip itself. 

 The clock has a number of standard time related features, but will also optionally
run a version of the Life game.

 The Game of Life is a cellular automaton devised by the British mathematician 
John Horton Conway in 1970. It is the best-known example of a cellular automaton.
Additional information is available @ wikipedia and online.

 Most features and behaviors can be modified by editing variables using a built in editor 
and a rotary encoder.  
 
 Please reference separate schematics and source code to complete assembly of the unit.   
 
  Documentation is a little sparse on this one. I only made the one clock and originally
did not plan on sharing the design. You may have to do a some of your own parts selection,
as I used whatever surplus parts I had available in the shop. Sorry, no detailed BOM
exists. If you have any specific questions, please feel free to email me. 
hcarlott@gmail.com
 
 Construction notes.
  
 I used the LT1176-5 switching regulator because I had them and I wanted the efficiency. 
 You could substitute a linear regulator, just be sure to properly heatsink it.
 
 The Rotary encoder is an alps EC11B series with a push button. 
 You can  probably substitute anything similar.
 
 I used a .47F 5.5 volt supercap to provide backup power for a few hours.
 
 The resistors in line with the display control signals are to minimize 
the possibility of latchup because the display is operating at a slightly higher voltage 
then the MCU. I did not have any problems with this design, and you could probably 
leave them out 

 
  Timing is derived from the mains ac signal, but the clock does have a 32khz xtal
to provide a timebase when running without power or from a dc source. 
Accuracy of the xtal is not as good as the mains signal. There is a calibration 
variable that can be adjusted to make minor tweaks in the time keeping accuracy.
 
 I made a pcb carrier board for the Sure display module and housed the whole thing in
'Espresso Display' cube purchased from the 'Container Store'.  http://containerstore.com
Some red plastic on the front enhances the led contrast.
  
  It actually came out quite nice for one of my projects :)
 
 The code:
  The code is not well written or commented, but it does work, and fits into the mega48 with 5% to spare.
There may still be some debugging code still resident. It assembles under the free Atmel AVRstudio. 
Hex files are included to just burn and go. It's important to program the eeprom hex data as well as
the code flash, strings and operational data are in the eeprom.

Fuses: The only critical one is to turn off the divide by 8 sys clock divisor.

Life: The simulation runs every n minutes (programmed behavior). It continues to run
for n number of seconds, or until the program determines that the display is stagnant (zero changes)
or if we just have a two cycle blinker running. We calculate a simple 8 bit crc for each 
generation and keep track of the last 16 generations. This is used to track changes.   

   
Options.
 One the clock is running, the rotary encoder can be used to set the time by
just spinning it. To set the date and other clock options you must enter the config mode by 
holding down the select button for approx 3 seconds. 
 
Rotate through the variables to select the one to be changed, hit select and modify the 
variables value. Select to exit. Play around for a while to get the hang of it.

Control variable definitions:


HOUR > Sets the hour
MIN  > Sets the minutes
SEC  > Sets the seconds. If you change the seconds, the clock stops till you exit the
       cfg mode (hacking). You can use this to match the time exactly to an external clock.
DAY  > Day of the week       
DATE > Day of the month (clock should handle leap years)
MNTH > Month
YEAR > Year
24HR > 12 hour or 24 hour (zulu) display mode. 1 = 24 hour mode.
AMPM > 0-8 enable display of a single led to indicate AM/PM status
       0=off, 1-8 pick which corner to display and if LED on = AM or PM
BSEC > Enable or disable display of seconds in binary. 0/1 0 = off 

CAL +> Experimental calibration technique, adds very small correction factor to timebase
CAL -> 0-99 + speeds up, - slows down (default 0 and 0 for no correction)

DLIM > Delimiter char. Sets the character to use as the separator between day/date
COLC > Colon char. sets the character to use as the separator between hour/minutes
SSPD > Scroll speed. Sets the speed which alpha data scrolls across display.
       Lower = faster. 1-99 default = 18.  
DLAY > Long delay value. Sets long delay value.  Long delays are used primarily
       when switching display modes. 1-99 default = 12.
DDSP > Date display. How often to display the date. 
       0-60. 0 = never 1 = every minute, 5 = every 5 minutes, 10 = every 10 minutes etc...
LSPD > Life speed. How fast to run the life generations. 1-99 default 6. Lower = faster.
LIFE > Life display. How often to run the Life game. 0=never, 1 = every minute, etc...
LRUN > Life run time. How long in seconds to run the life game. 2-99 seconds.
RNDM > Random seed mode. 0-2.  0 = use time display as seed. 1 = use random seed.
       2= xor random seed with current time display as seed.
EXIT > Select to exit the config mode                        

===================================================

  A Port of the ATMEL AVR Butterfly Application 
  Code to avr-gcc/avr-libc
  
  Development Diary of the gcc-port

  by Martin Thomas, Kaiserslautern, Germany
  mthomas@rhrk.uni-kl.de 
  eversmith@heizung-thomas.de

===================================================


Disclaimer:
-----------
I'm not working for ATMEL.
The port is based on REV_06 of the ATMEL-Code (for IAR-EWAVR) with fixes
from REV_07.
Initially I marked my changes with "// mt" or enclosed them with "// mtA" and 
"// mtE" but forgot this for some changes esp. during debugging. 'diff' against 
the original code to see everything that has been changed.


2. July 2009

Tested with avr-gcc 4.3.2, avr-libc 1.6.6 (WinAVR 3/2009)

- main.c - added attribute OS_main to main()
- lcd_functions.c - scrollmode = scrollmode; to avoid warnings with -Wextra
- adc.c - ADC_periphery(), typecasts to avoid signed/unsigned warnings
- botton.c - init key with 0 instead of NULL (Craig Arno)
- main.h - define NULL with ((void*)0) (Craig Arno)
- main.h - version to 0.7.4

12. Mar. 2009

Tested with avr-gcc 4.3.0, avr-libc 1.6.2 (WinAVR 6/2008)

- sound.c - added song, just for fun
- dataflash.c - translation of comments. Thanks to Eirik Tveiten
- main.h - avoid redefintion of NULL, version to 0.7.3
- RTC.c/ShowData - moved DATE_FORMAT_NR from RAM to FLASH, 
  modified output-routine
- AVRStudio gcc config-options and Makefile: 
  Compiler and linker-options for smaller binary
  Older versions of gcc and/or binutils may produce incorrrect
  code when using these settings so use at least the
  versions used for tests (see above)

17. May 2007

- vcard.c/vCard: moved enter=0 do avoid redundant writes
- vcard.c/RS232: fixed possible buffer-overrun
- vcard.c/EnterName: fixed possible buffer-overrun, local helper c
  rewrote copy name to display, saves some flash-memory,
  removed unneeded statics
- LCD_functions.c/LCD_clear: gTextBuffer[0] to 0-char
- main.h: version to 0.7.2
- uart.h: added #define to enabled/disable transmit
- uart.c: used #ifdef to enable transmit and provide tx-function
- test.c: removed the #if 0 sections replaced by TestWaitEnter

31. Jan. 2007

THIS VERSION HAS BEEN TESTED WITH avr-gcc 4.1.1 AND avr-libc 1.4.5.
USE WinAVR 20070122 OR NEWER ON MS-WINDOWS PLATFORMS

update for gcc 4.1.1/avr-libc 1.4.5 (WinAVR_20070122) and small
improvements:
- all interrupt-handlers: migrated SIGNAL(SIG_...) to ISR(..._vect)
- removed includes of avr/signal.h (fully replaced by avr/interrupt.h)
- include stdint.h instead of inttypes.h
- renamed eeprom.h to bfeeprom.h and eeprom.c to bfeeprom.c to avoid
  confusion with avr-libc's avr/eeprom.h
- use of avr-libc's eeprom_read/write (avr/eeprom.h) in bfeeprom.c 
  and vcard.c. The avr-libc supports the ATmega169 eeprom-registers 
  since several versions now. Replaces the functions from eeprom169.c/.h 
  in older releases of the gcc-port (files have been removed).
- main.c: use of avr-libc's sleep-functions/macros (avr/sleep.h). 
  New versions of the avr-libc support the ATmega169 sleep-register 
  correctly. The old code is still available in comments as reference.
- main.c: check version of avr-libc using defines from avr/version.h 
  and write warnings if an old version is in use.
- updated Makefile with sample/template from WinAVR 20070122
- LCD_driver.c: made gLCD_Start_Scroll_Timer volatile (used in LCD-ISR)
- LCD_functions.c: made gLCD_Start_Scroll_Timer extern volatile (used in LCD-ISR)
- main.h: version info: 0.7.1
updates from Atmel's REV07-code for IAR (available from atmel.com):
- adc.c : fixed table-lookup-index for light-sensor: 
  else if(i > 21) -> else if(i >= 21)
- lcd_driver.c: init added LCDCCR |= (1<<LCDDC2)|(1<<LCDDC1)|(1<<LCDDC0);
- lcd_driver.h: updated LCD_CONTRAST_LEVEL macro
=> 6th public release. Version 0.7.1 (timestamp.20070131)

7. Jan. 2006

Summary: AVR-Studio project-workspace, small fixes and changes

- added volatile for data, buffer and timer in lcd_driver
- exchanged cbi/sbiBF(PORTB, 5) in sound.c/PlayClick()
- added volatile sound.c/gPlaying (PlayTune called by timer0-isr) 
- moved gPlaying from main.c to sound.c
- moved gUart to vcard.c
- moved gPowerSaveTimer to RTC.c
- removed extern ... LCD_charcter_table from main.c
- moved gAutoPressJoystick from main.c to LCD_driver.c, 
  changed type from bool to char 
- prevent unneeded LCD refreshs in main.c/KeyClick()
- gKeyClickStatus volatile since used in pinchange-ISR
- added local helper function AutoPowerShowMin(); in main.c
- updated/corrected some comments
- main.c/Init. corrected DIDR1 to DIDR0
- moved button definitions from main.c to button.h
- added volatile to time and date global variables (rtc.h/rtc.c)
- corrections/additions in this readme
- added an AVRStudio "project-file", compile with -Os and 
  link with -lm since the binary will not fit into the 14kBytes 
  application-section with -O0 and the standard gcc FP-functions,
  tested with AVRStudio V4.12SP1
- fixed length-check in vcard.c/vCard() ( repl. | by || )
- fixed pause if-condition in sound.c/Play_Tune() ( repl. | by || )
- gimmick in name-input: initial-value for an appended char is the
  same as the previous char. flipped plus/minus
- gcc-port of Test() now tested, added TestWaitEnter() to 
  save some flash-space
-> 5th public release Version 0.6.6 (timestamp 20060107)

27. Aug. 2004

THIS VERSION WILL NOT BUILD WITH AVR-LIBC VERSIONS BELOW 1.0.4.
USE WinAVR 20040720 OR NEWER ON MS-WINDOWS PLATFORMS

Most of this has been found out during the development of a data-logger 
application for the Butterfly using the Dataflash. 

In the iom169.h-file from avr-libc 1.0.4 as included in 
WinAVR 20040720 the special-function-register-names match those in the
datasheet for the ATmega169 (12/2003). While some old names have been kept
for compatibility this has not been done for all. This version of the port
should only use "official" SFR- and bit-labels now.

* usart.c/USART_INIT - corrected comment: only receiver is enabled (power-save)
* dataflash.c/.h - new functions Page_Erase and verify, not accessed by the 
  App Code but might be useful. From the logger application.
* dataflash.c/DF_SPI_init - comment near spi-int freq setting
* menu.h: commented out "browse segments" - not implemented in Atmel code
* main.c: init of state and nextstate separated. Had problems with this in 
  the logger application
* dataflash.c: added comment about dataflash chip select and powersaving;
  added comment about PageBits/PageSize near DF_status
* sound.c: added a local variable to save some pgm_space accesses, as proposed
  by [TODO: namen raussuchen] in the avr-gcc mailing-list
* main.c/OSCCAL_calibration: two "&&" where not correct. Reported by
  Steve Hippisley-Cox. Changed to "&". A test on the Butterfly did now show
  any difference in the calibration. "Wrong" and "correct" code result in the
  same OSCCAL value.
* Information: Colin O'Flynn measured the current draw of V 0.6.4. Current
  draw is now the same (a little less in some functions) as with the original
  binary created with IAR EWAVR.
* Added batch-file ("shell-script") for flash upload through bootloader with avrdude.
* all: adapted register-names as defined in the avr-libc iom169.h file based
  on the ATmega169 datasheet 12/2003
  (avr-libc-tag: $Id: iom169.h,v 1.13.2.2 2004/04/20 23:54:56 troth Exp $) 
* sound.c: song-name pointer-array in flash only (sound.c), saves 16 
  bytes of RAM (wow), added local "view song-name function" to save 
  same flash-space
* main.c/sound.c/menu.h: added keyclick-function from version6.net
* menu.h: Gimmick - let "GCC"-flash/blink in the welcome message
* all: changed sbi/cbi makro-names to sbiBF/cbiBF in main.h and everywhere 
  else in the code to keep compatiblity with future versions of the avr-libc
* makefile based on template from WinAVR 20040720, used avrdude programmer
  option butterfly, if avrdude does not work for you try the 
  version available at www.siwawi.arubi.uni-kl.de/avr_projects
* Programmers-Notepad project file
* made eeprom169.c a separate module (not inline any more) - saves flash space
-> 4th public release Version 0.6.5 (timestamp 20040827)

4./5. Dec. 2003

- Fixed a small error in the vcard-module ("NAME") when downloading via RS232.
- Report from Randy Ott in the avrfreaks forum. BF with GCC port current
  drain is much higher than the original application which cuts battery life. 
  There seems to be a problem/incompatibility with avr-libc sleep-functions and the 
  ATmega169. Fixed by using the original ATMEL approach and "sleep" via inline-
  assembler. Current drain of version 0.6.4 is much lower now (the old code never 
  entered power-save-mode) Needs testing and better current measuring.
  (update 1/2007: the avr-libc supports power-save of ATmega169 now,
  avr-lib-functions used from version 0.7.1)
- Made powersavetimer volatile because of interaction with 
  rtc-timer-interrupt-handler.
-> Third public release Version 0.6.4 (timestamp 20031205)

15. Oct. 2003

Second public release. Since ATMEL accepted to distribute the gcc-port
as long as "the ported code call[s] attention to that this is ported Atmel
code", the ported dataflash-sources are included now. This is 
Version 0.6.3 (timestamp 20031016).
Build hints: 
- the path in the makefile may have to be changed.
- tested with WINAVR Sept. 2003-Edition, avr-gcc older than 3.3.1 or
  avr-libc older than 1.0 may not be able to compile and link this code.

14. Oct. 2003 (had a wrong date)

First public release of the Butterfly application code 
port to avr-gcc/avr-libc. Version number is 0.6.2-Datecode 20031015. 
The files dataflash.h and dataflash.c are not included because 
of possible copyright violations. Patch for this files is
available on request.

14. Oct. 2003

No Message from ATMEL. The only copyright-notice is in
dataflash.h/.c which is only used in the hardware-test
routine. Started to rewrite this code to get a clean 
implementation. Stopped the rewrite at some point - 
it's too silly doing a cleanup or rewrite of code which 
I do not fully understand. 

About 12 requests for the code via e-mail. Will wait one more 
day for response from ATMEL.

The modified code leaves 8 words of free space in the program-
flash. Since the possibility to use the 2nd buffer on the 
dataflash has been enabled. Shortened some flash-text constants
to make the binary fit into the memory. (1/2006: this has been
reverted in all of the newer releases)
For own extension the files test.c and dataflash.c can be removed from 
the makefile. And the files test.c/test.h and dataflash.c/dataflash.h 
should be deleted or moved. After a compiler-attempt the output will 
show the locations of references to functions in test/dataflash, 
they can be removed without loss of functionality compared to the 
original application without hardware extension (see comment in code 
how test-subroutine is activated)

(update 1/2006: check firmware-usage with avr-size it changes
from version to version of compiler and application)

Will prepared a source-package to work without test and dataflash 
just in case.

10. Oct. 2003

Some code-cleanup, fixed a small bug. Still 
some bytes stored in RAM which could be flash-
constants - will keep it like this.

Enough of playing around - spent too
much time with this. The datalogger application
will have to wait.

Still no message from Atmel. The Atmel-man at
the seminar today told me to ask ATMEL/Norway.
(The ATMEL ARMs are very "nice" - but prototyping
or producing small series with these devices is 
too difficult for a one-man-show.)

(update 1/2006: Since 10/03 I have developed 
different dataloggers, some based on the 
Butterfly-hardware. I'm now also using 
ARM7 controllers from Atmel and Philips)


9.Oct.2003

Wrote an e-mail to ATMEL today, hopefully they do 
not have any copyright concerns. They should not,
but since I've sent the message I have to wait.

I can't stop when I should, the song playback
has to work too...

Should be ** finished **  (software is never finished
before Version 3.14159...)
Song playback does work now - activated all
Songs ;-). 

The whole libc-progmem "thing" is a pain,
hopefully the avr-gcc/libc people can implement
the IAR/codevision FLASH method sometime.
(Although Joerg Wunsch writes that it is inefficient
in some forum-threads.)

I call this Version 0.6.1 the Butterfly welcome
Message is "AVR Butterfly GCC", Version is "REV061",
Initial date is 8. Oct. 2003.

The "joystick" on my BF is nearly broken ("up"-Direction) 
not the best quality.
If someone from Atmel will ever read this: another
supplier my have parts of better quality. 
I should get a new BF sometime after the ATMEL seminar.

(May 2004: although the joystick is not that good 
any more the main reason for the "slow" response to
the joystick was the missing sleep-mode. See comment
for V 0.6.4.)


8.Oct.2003

Sent mail to the avr-gcc list.
Reply from "Desi" (AVR Butterfly+gcc = Desi ;-) ). 
He suggests to take his code and start from there. Desi-
code is based on an older Atmel-package. I made too many 
changes and don't want to throw them away - no more 
replies - on my own. 


*** OK, done. *** (most of it). Foolish mistakes. 
I don't write them down here - does not put me in a good light.
(translated word by word from a German saying)

Separated some code to stand-alone projects during debugging.

New special eeprom-handler since ATmega169 is not
supported by the avr-libc via eeprom.h, main code is from
the ATmega169 databook with small changes to  make them 
compatible to the usual avr-libc interface. A lot of
other things had to be changed.

(update 1/2006: Newer versions of the avr-libc support
the ATmega169 so functions from eeprom.h could be
used. But I will leave the eeprom-code as it is)
(update 1/2007: migrated to avr-libc functions)

The following does work:
- LCD
- Menu-System
- Welcome-Text / Revision-Number
- Time Clock with adjust and format
- Time Date with adjust and format
- Music - selection and start tune (see below), Volume control
- Name - input via Joystick and RS232 
- Temperature - seems reasonable (at least Celsius ;-),
  it is getting cold in Germany
- Voltage - shows 0,0 if no voltage source attached - no more tests done
- Light - seems to be ok
- Options - Display contrast works, Jump to bootloader too
- Sleep mode-settings should work 
 
No "go" for sound playback - although the Butterfly produces some
funny noise - I think I'll keep it like this...
Will publish a fist hex-File on the web-Page. Version 0.2

7.Oct.2003

more modifications - marked some variables 
as volatile, changed counter in main/delay 
from int to uint8_t because it did not
work correctly in the Studio4 simulator, 
more changes not documented here,
uploaded the code to the Butterfly -
no-go. Simulator crashes (resets) for 
unknown reasons. Tried to find out the 
difference between "DESI" code (the LCD does 
work in his code) - no success. Found the
Codevision port but its pretty much
like the IAR code. Nevertheless all the "dirty" 
work for the avr-gcc should be done... (esp. progmem
stuff) - will send a mail-for-help to the avr-gcc list.

3.Oct.2003

Progspace-Variable-Handling improved. Na, not improved:
changed the complete accessing. Simply replacing 
__FLASH with PROGMEM isn't enough (by far).
A lot of hints found in a message
on the avr-gcc ML from Joerg Wunsch.
- but Joerg's changes (patch) are not 
based on the Atmel-Code but on the code from
"DESI"/Mexiko who has sent Joerg a version of the ATMEL
source which he had already modified. So the 
"state-machine"/Menu does not work. To say it correctly: 
nothing beside of the LCD works as it should. 
Joerg patched a lot but obviously not everything that 
is needed. Credits to Joerg.
New function prg_read_float_hlp is an improved 
(?) version of the routine found in the jw-
patch. Still not tested with the hardware,
but some simple tests with the simulator.
More changes not documented here.

1.Oct.2003

Created Web-Page:

This is a completely untested port 
of the ATMEL AVR Butterfly application code
(Rev. 06) to gcc. I've used the WINAVR Sept. 
2003 edition.

I hope this does not violate any copyrights.
But I could not find a copyright notice in ATMEL's
code. (1/2006 - this is wrong, so above)

There exists some code from "DESI, Mexico"
which I found in the AVRFREAKS gcc forum.
(18AGO2003.zip) This code produced a lot 
of warnings and did not cover all functions 
of the original Butterfly application. 
So I restarted from ATMEL's code. 
Credits to DESI.

Changes:
- __flash directive to const ... PROGMEM
- Interrupt -> Signal declarations
- __disable_interrupt/__enable to cli(), sei()
- changed some char to uint8_t to avoid compiler 
  warnings.
- PORT constants to iom169.h style
- other constants OCxxx, WDxxx
- ...

The original code is still in the source
(commented) I marked my changes with
//mt or // mt.
By the time of writing this I don't have
my Butterfly board at hand. When I get it 
back I have to solder a connection for
an external power source first since the 
battery drains very fast if the board is 
accessed via RS232. 
Well, since "someone" said: "publish early,
quick" in open source - here it is. I hope 
someone will pick up this, does some debugging
and sends back an improved version. Of cause 
any kind of feedback is welcome.

This is my first attempt to use the
GNU avr toolchain (and C on microcontrollers
in general). So you might see some strange 
things - I simply don't know better. Feel free
to tell me what's wrong and how it can be done 
better.


Cheers,

Martin - mthomas@rhrk.uni-kl.de





---
o:
	10ae:	87 e0       	ldi	r24, 0x07	; 7
    10b0:	83 bf       	out	0x33, r24	; 51
    10b2:	88 95       	sleep

n:
    10b2:	83 b7       	in	r24, 0x33	; 51
    10b4:	81 7f       	andi	r24, 0xF1	; 241
    10b6:	86 60       	ori	r24, 0x06	; 6
    10b8:	83 bf       	out	0x33, r24	; 51
    10ba:	83 b7       	in	r24, 0x33	; 51
    10bc:	81 60       	ori	r24, 0x01	; 1
    10be:	83 bf       	out	0x33, r24	; 51
    10c0:	88 95       	sleep
    10c2:	83 b7       	in	r24, 0x33	; 51
    10c4:	8e 7f       	andi	r24, 0xFE	; 254
    10c6:	83 bf       	out	0x33, r24	; 51


o:
	10de:	81 e0       	ldi	r24, 0x01	; 1
    10e0:	83 bf       	out	0x33, r24	; 51
    10e2:	88 95       	sleep

n: 	    
	10f4:	83 b7       	in	r24, 0x33	; 51
    10f6:	81 7f       	andi	r24, 0xF1	; 241
    10f8:	83 bf       	out	0x33, r24	; 51
    10fa:	83 b7       	in	r24, 0x33	; 51
    10fc:	81 60       	ori	r24, 0x01	; 1
    10fe:	83 bf       	out	0x33, r24	; 51
    1100:	88 95       	sleep
    1102:	83 b7       	in	r24, 0x33	; 51
    1104:	8e 7f       	andi	r24, 0xFE	; 254
    1106:	83 bf       	out	0x33, r24	; 51

2007/1, 4.1.1, 1.4.5 - r/o: 13862  
2008/6, 4.3.0, 1.6.2 - r/o: 14306

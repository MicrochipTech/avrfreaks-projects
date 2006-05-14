# TV Image Generation

Uploaded by johnhaskins on 2006-05-13 17:31:00 (rating 0 out of 5)

## Summary

This is an api / codebase that will generate an image on the tv screen based on the bits of sram. It usues virtually all of the avaialable sram, leaving only 39 bytes for the stack. This yeilds a field 96x82 that looks pretty good sitting on the screen.


Comments, Questions, and Suggestions welcome.


2006.05.13: Finally got everything up on sourceforge, take a look at [https://sourceforge.net/projects...](https://sourceforge.net/projects/avrtv/) for the project details and latest version.


2005.02.09: New version 0.5 includes some upgrades, changed the pixel numbering so that 0,0 is \_top\_ left instead of bottom. Now included 2 example application, one similar to previouse and a drawing program that uses 6 of the 8 pins on port d. Major enhancments still left to do but may have to wait a while for them.


2005.02.06: New version 0.4 includes basic bitmap support, and a character set stored in EEPROM. Example app writes the character set to the screen. It also includes a Readme talking about the hardware settup, sorry about that for you that downloaded already and don't know how to connect it to the tv.


-Removed version 0.3-


Enjoy


-john

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4

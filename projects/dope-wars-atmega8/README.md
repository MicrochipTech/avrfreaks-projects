# Dope Wars ATmega8

Uploaded by k0balt on 2009-11-01 06:22:00 (rating 0 out of 5)

## Summary

Uart interfaced text based trading game.


License - - in the public domain, no warranty, should not be used by  

anyone, but if you make it better you have to send me a copy (please).


Playable alpha, no major bugs found (but lots of little ones)


Cant upload.....no worky worky.


[http://rapidshare.com/files/3007...](http://rapidshare.com/files/300794472/M8_Dopewars.zip)  

 for archive with source, hex, and screenshot


++++more info


Ok, dopewars fans....


First, this is an alpha - version....works with known bugs.


Bugs that I remember off the top of my head:


Everything coded in single variable type, sometimes giving strange  

decimals....Integers wouldn't work. Not sure why.  

Sometimes items become unavailable and are displayed with a zero  

price. This loosely mimics intentional game behavior, so i left it in.


None of these seriously affect playability.


I spent an hour and a half jotting this out to refresh myself in  

basic, and plus I wanted to play dopewars. I do not intend to  

(although I Might) finish or improve the program.


That said, it works for me.


Here's the zip file with the hex, obj and bas files.


It was written with bascom-avr but it is more than 4k so if you want  

to recompile with that you will need the regged version, which I  

recommend buying if you can. There  

are some minor bugs in the code, and if anyone does take the time to  

sort them out, I'd be stoked to get a copy of the fixed code...


The chip (atmega8)has to be set up in the fuse bits for 8mhz  

(internal, long startup time is what I used on my ATmega8-16PU)


Schematic...  

Connect your MAX232 or other ttl-rs232 level converter to the uart on  

pin d0(rx)(pin2) and d1(tx)(pin3)  

Power (+5)on pin 7, and pin 8 to gnd, 1uf cap (not critical) across  

vcc to gnd, a 100k resistor to hold reset (pin1) to vcc.  

---End Schematic


Connect your RS232 port to your computer and connect using a terminal  

program at 19200baud, 8data bits, no parity, 1stop bit, no flow  

control. I included a HT setup file for windoze hyperterm, but you  

will probably have to change the com port (mine is 6)


Turn on the AVR and you should see "press any key to start" , the  

random seed generator dialog. At that point you are on your way.


If it doesn't work, write a simple Hello World for your UART to check  

your hardware setup.


At least that's what worked for me.


Good Luck, and watch out for officer hardass!


Hint :Use CAPS LOCK. vinnie takes liberties with your debt. This is  

not a bug. You have to give him a little extra for the vig.


k0balt

## Compilers

- BASCOM-AVR

## Tags

- Complete code
- BASCOM-AVR

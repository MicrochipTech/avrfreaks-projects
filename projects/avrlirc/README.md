# avrlirc

Uploaded by paulfox on 2009-02-10 15:22:00 (rating 0 out of 5)

## Summary

There are two parts to the current project:


The first is a simple program (avrlirc.c) that runs on an ATTiny2313 AVR microcontroller, or probably many others, without much work -- an onboard hardware-based UART is the main requirement. (The 16-bit timer and the input capture register are used as well.) The code monitors the pulse stream from a standard IR receiver (similar to the Panasonic PNA4602M or Sharp GP1UD261XK0F), and sends the pulse timing information to a host computer via a 115200 baud serial link. The data format is designed to closely match that of the LIRC ([www.lirc.org](http://www.lirc.org)) UDP driver.


The second part of the project is not AVR code, but a small program (avrlirc2udp.c) which reads the avrlirc data stream from the serial port and relays it (via a UDP socket connection) to a listening lircd daemon.


Advantages of this setup are several:


- Unlike the traditional LIRC "home-brew" serial port IR dongle, or even newer USB-based dongles, there is no requirement here for a special kernel driver. Any serial port capable of 115.2Kbaud can be used, including USB-to-serial converters (which can also provide a convenient source of 5V power for the AVR micro).


- Unlike some other AVR-based solutions, there is no need for a new type of config -- the same lircd.conf file used for the home-brew receivers will work with this.


- Thanks to the UDP network connection the serial port to which the avrlirc device is connected need not be on the same machine as the lircd process that is interpreting its data. In my case, I have IR receivers attached to several workstations, all of which control processes (for home automation, and home audio) which run on a central server. Using avrlirc, the lircd daemons can all run on the the central server (each listening on a different port) which simplifies configuration changes.


[Edit]  

The code is now a) happy running at 38400 baud, using the internal RC oscillator on the ATTiny2313 at 8Mhz, and b) no longer necessarily requires a MAX232 line driver. Two gpio pins are used as a "software inverter" to flip the TX signal, in order to make the level polarity match that of RS232. Most consumer-grade serial ports accept TTL levels. This makes avrlirc a "one chip" serial port solution to IR reception. (Well, not counting the IR receiver itself, I guess.)


Issues of the upper level data transfer daemon getting out of phase with the pairs of bytes being sent by the micro have also been addressed. May 9, 2007


[Edit #2]  

I've put up a "public" page describing this project, including pictures of my receivers, here:  

[http://www.foxharp.boston.ma.us/...](http://www.foxharp.boston.ma.us/avrlirc/)


[Edit #3]  

I've released a new source ball (090209) that fixes a race condition in the character transmit routine. I believe the race is benign, and avrlirc won't hit it, but good to fix it nonetheless. I can't upload the tarball to avrfreaks for some reason, so I'm referencing it by URL. I'll leave the old file in place just in case the linked version goes away someday.  

-------------------  

Paul Fox, April 26, 2007, updated Febrary 2009.

## Links

- [avrlirc-090209-0835.tar.gz](http://home.comcast.net/~foxharp/avrlirc/avrlirc-090209-0835.tar.gz)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain

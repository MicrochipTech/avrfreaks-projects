# Simple M168 Based Logic Analyzer

Uploaded by tymm on 2008-02-26 19:54:00 (rating 0 out of 5)

## Summary

This is a simple logic analyzer application requiring a minimal M168 setup (originally developed to run on Arduino clones, but does not require the Arduino libraries/software). It can either work in clocked mode or capture based on state change (it has no free-running capture mode). With each sample it also gets a relative time-stamp.


I put this together because I was fighting with a BitScope too much when working on debugging serial communications at different rates.


Hardware required is very simple; you just a 168 with power connected (decoupling capacitor recommended) and a 10K resistor from VCC to RST, and a way to get serial data to a PC (I use an FTDI USB-to-TTL serial cable; you can also use a Max232 or similar, or even use an Arduino clone). As I don't need super-accurate timing, I've configured my '168 to use the internal clock at 8mhz.


I've also put together a basic (pre-alpha level) gui based on Open Frameworks, available at  

[http://timothytwillman.com/?page...](http://timothytwillman.com/?page_id=162) -- the executable available from the link currently only runs on OSX, and only has been tested on Leopard. I don't know how much more work I'll put into it at this point since it does most of what I need, but I'd be happy to help if anyone wishes to work on it.

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain

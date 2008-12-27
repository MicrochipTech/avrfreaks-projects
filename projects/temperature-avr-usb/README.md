# Temperature via AVR-USB

Uploaded by dstahlke on 2008-12-26 20:16:00 (rating 0 out of 5)

## Summary

This is a simple USB temperature probe that uses the AVR USB library by Objective Development (<http://www.obdev.at/avrusb/>). It is in large part built upon the EasyLogger example by Christian Starkjohann. Whereas EasyLogger would send the data values over a keyboard interface, this project uses a custom device class and reads values using the ruby-usb library. As such, it is primarily intended to be used in Linux, although it is possible to get it to work in XP with a bit of hassle involved (I was not able to get it working in Vista, although it may be possible). The included ruby script reads the temperature and optionally logs it using RRDTool.


Now I have some tangible numbers to give when telling building services that my office is too hot!

## Links

- [Project web page](http://www.stahlke.org/dan/usb-temperature/)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain

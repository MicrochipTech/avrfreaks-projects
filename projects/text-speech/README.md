# Text to Speech

Uploaded by joegr on 2002-07-02 13:40:00 (rating 0 out of 5)

## Summary

This is some quick and dirty code that I did to test a hardware design that I did (talking bank ATM). It uses a new Winbond text-to-speech chip (WTS701, about $15). The design uses the four SPI comm lines only, with no connection to the interrupt and busy lines from the part. Most of the real work involved figuring out how to get around not having those lines, without just adding dumb delays. I did not use the AVR's built in SPI port, but it should work for this application. Please note that the hardware I used inverted all four of the SPI signals. The software also inverts, to correct for this. The prelim data sheet for the WTS701 did not indicate 5V tolerant IO (WTS is 3V part), but the lastest parts are. You should be able to connect the part directly and adjust the software as needed.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4

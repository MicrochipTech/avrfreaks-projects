# NMEA Sentence Parsing Library

Uploaded by penquissciguy on 2009-05-19 02:37:00 (rating 0 out of 5)

## Summary

I found Mikal Hart's TinyGPS library for the Arduino when I was looking for a NMEA parsing library for a project. This library processes the NMEA string (gpgga and gprmc) character by character, resulting in a low memory footprint library that is well-suited to use with serial communications. I liked the library so much that I rewrote the  

function definitions and added some #defines so that the code would compile as c code under avr-gcc. The actual logic of the code is all Mikal's, and I appreciate his permission to post my pretty trivial port. Any errors you run into are (probably) due to my hackish coding style and were (probably) not part of the code I started with. The one piece of functionality from the original I did disable was a millisecond-resolution "age of fix" parameter. It used the "millis()" function on the Arduino, and as I did not need the functionality I did not write a replacement. In it's place, I have a dummy millis() function that always returns 0.


The included files are a proof-of-concept project I wrote for the AVR-MT-128 board by Olimex. The TinyGPS code is platform-neutral, however, and is located in TinyGPS.c and TinyGPS.h.


Since the file upload is not working here for some reason, I've put the zip archive up on my own webspace at [http://www.kenworster.com/tinygp...](http://www.kenworster.com/tinygps/testgpscode.zip)

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)

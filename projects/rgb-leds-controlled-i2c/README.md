# rgb leds controlled by I2C

Uploaded by il-teo on 2008-08-13 07:50:00 (rating 0 out of 5)

## Summary

BlinkM is a Smart LED produced by ThingM:


 BlinkM is a “Smart LED”, a networkable and programmable full-color RGB LED for hobbyists, industrial designers, prototypers, and experimenters. 


Just connect it to an I2C bus and start sending commands like "show rgb color" or "fade to hsv value" and so on and so forth (it does much more; see the human-readable data sheet).


A bit of explanation: I liked the idea to do a few more things with the leds and decided to modify the firmware. The original firmware is actually closed source (due to legal issues) so I had to start writing a new one from scratch. This first release is very basic but hopefully adding new features won't be that hard.


The new firmware has two interesting additional features in the works: the ability to act as a master on the I2C bus and support for dallas 1-wire protocol, to leave one pin free for your creativity. 


For full details, source code and downloads, please refer to [http://code.google.com/p/codalyz...](http://code.google.com/p/codalyze/wiki/CyzRgb)


Matteo

## Links

- [cyz_rgb sources](http://codalyze.googlecode.com/files/cyz_rgb-alpha1-2008_08_13-sources.zip)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain

# AVR SuperMario

Uploaded by jery9871 on 2008-08-16 20:09:00 (rating 0 out of 5)

## Summary

Finally, i found some time to write a collision detector and to form a nice sidescrolling game engine on top of the AVGA driver which fits into ATMEGA168. 


[http://www.youtube.com/watch?v=-...](http://www.youtube.com/watch?v=-mMdc-rNNtU) 


Few fords about the AVGA: AVR, clocked 19.6608MHz will generate 4bit pixel stream and SYNC signal. The stream is converted to R,G,B with simple, 9 rezistor video DAC. Resulting signals can be connected via SCART connector to a PAL device like TV, VCR etc. resolution is 192x144, 16 colors per pixel. 


EAGLE 4.11 schematic and PCB layout are included. Note that this is the alpha-version, it's not a game yet, it's just a demonstration of what's also possible to do with theese awesome MCUs :-) 


Well, now it becomes a matter of editing levels rather than exhaustively optimizing all the subroutines... So i'm looking for someone enthusiastic who would help me with this project. A level editor probably needs to be written, to generate special formatted-levels with special compression to fit in an AVR, i suggest mega644...


I would be also very happy if someone adjusts the driver to generate NTSC signal. I can't do that since i don't have any device that accepts NTSC in RGB-SYNC form. The AVR probably must be bit overclocked to handle this... 


Soon, i'm goin to release AVGA 0.2 source with sound support, double buffering, bug fixes etc.


If you're interested, contact me at [md@unicode.cz](mailto:md@unicode.cz) or ICQ# 206-779-315.

## Compilers

- WinAVR (GNU GCC)

## Tags

- WinAVR (GNU GCC)

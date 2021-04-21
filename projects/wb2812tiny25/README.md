# WB2812_Tiny25

Uploaded by Simonetta on 2021-04-21 15:46:00 (rating 0 out of 5)

## Summary

This programs lights a row of RGB LEDs (WB2812B ICs, known as NeoPixels) in multi-colored patterns.  It is a crosswalk pedestrian signaler for people who are walking across streets in the early evening when it is dark and there are a lot of cars still on the road.   It is meant to fit in a clear or translucent plastic tube about 18-22mm inner diameter (the width of a 18650 lithium battery).  


 


Pressing a button completes the path from the battery to the Tiny25, which lights 9 LEDs in a blinking pattern.  With each blink the center color (#4 of 9 LEDs) is copied and shifted.  If white is the center LED color on the first blink with the other colors of the sequence being=   purple:red: green: blue: white :blue: green: red:purple,  then at the second blink, the color sequence is=  red, green, blue, white, pink, white, blue, green, red.  The colors move from the center to the ends of the LED string.   The RGB color values of the pseudo-random color selected for the middle LED comes from reading the flash values of the binary program code.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler

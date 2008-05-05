# 4x4x1

Uploaded by Scroungre on 2008-05-04 18:00:00 (rating 0 out of 5)

## Summary

Summary: 


Using a combination of resistor ladders and an AVR ADC input pin, you can couple a 4x4 matrix keypad to an AVR using only one microprocessor input. Noise is minimized by heavy averaging of the ADC input, and exact wiring made (nearly) irrelevant by using a lookup table for the actual keypad results. A serial line to a dumb terminal (or any PC running a dumb terminal program – I used a Dell laptop with Win2k and TeraTerm) receives the output. 


Includes:  

Schematic (PDF)  

Commented AVR Assembler code (ZIPped up)  

AVR project files (in ZIPfile)  

PCB layout (Express PCB)


Works with:  

 Any AVR with ADC inputs. I used an ATmega168 in a 28-pin skinny (0.3”) DIP package, but pretty much any one with an ADC and SRAM would work. Some code editing could be necessary.


Requires:  

Some sort of RS-232 receive and display unit. I used a Dell laptop with Win2k (SP4) and TeraTerm.


Comments:


 In short, what we are doing is using resistor ladders to construct a very crude DAC (Digital to Analog Convertor). Then we feed an analog signal level into the AVR’s ADC (Analog to Digital Convertor) input, and convert back to digital. While this may seem redundant, the significant saving in wiring makes it a fascinating procedure. After all, it’s what modems did for many years before everyone got DSL and cable broadband Internet connections.


 The idea is that each key pressed will result in a different analog voltage level by tapping into the various dividers, the resistor ladders, at different points, and the ADC then tries to discern which key’s been pressed, returning an eight-bit value of the analog voltage. 


The ADC input values are then averaged over 256 samples. 256 samples is a neat averaging trick, because you don’t have to preload the loop counter (just clear it, then decrement before testing for zero) and, much more so, because the division by 256 is trivial.


 Each eight-bit sample is added to a 16-bit accumulator, then to divide by 256, you just drop the low byte of the accumulator. Pow – Instant division by 256! It’s a zero-cycle divide!


 Granted, this trick truncates rather than rounds off, throwing away the remainder, but given the lookup table later, it doesn’t matter. I used the ADC in an eight-bit mode, again because the lowest bits are likely to be noisy, and I don’t like noise, and it would make the averaging accumulator math less cute.


 It’s possible to precalculate all the values beforehand, and thus ‘just know’ what analog voltage any given keypress will result in, but I’m lazy. I didn’t even care what row I wired to which resistor, nor which column (although you do have to keep the rows and columns separate). Then I pushed all the buttons, and watched the terminal display the values for each button. Some dithered, so I assumed the value was ‘somewhere in between’.


 Then once I had a value for each button, and they were acceptably widely separated (they could be better…), I just wrote a lookup table (only 256 entries, and most of them blank. It didn’t take very long) for every input ADC value, and assigned some to the closest character value. I also assigned some adjacent values to the same character, just to give even more noise immunity.


 Note that I enabled the AVR’s internal pullup resistor on that input pin. This makes a significant difference! And the specification for the AVR pullups is atrocious; “Between 20k and 100k”. That’s quite the variance! Thus your lookup tables will almost certainly differ from mine, and therefore also this design is probably not really well-suited to mass production. 


 I did use resistor networks instead of discrete resistors in the circuit board layout, because they, although not being perfectly accurate themselves, tend to internally match each other very well.


 Any ADC value over 0xE0 is presumed to be ‘No Key Pressed’. But they’re still in the lookup table to prevent weird things happening when the lookup value overruns the table and winds up in other program space. This can be changed as ‘NO\_PUSH’ in the code.


How to use it:


 Compile and run. There should be no errors and no warnings (If you’re using an ATmega168. If not, fix whatever’s different with your chip). Apply a serial cable, get the terminal behaving (9600 8N1), and power up the card. Program the chip. It should send a little message to the terminal saying, basically, “Welcome”. If you get this far, the rest is pretty easy. Skip to ‘To get the keypad working’. Else, keep reading…


 If the AVR didn’t program, you may have a more serious problem outside the scope of this article. If it didn’t say anything, or anything legible, to the RS-232 port of the terminal, then continue reading.


 Now would be a good time to check out the voltages on the MAX232A. Consult the spec sheet, and put a voltmeter on the +10V and -10V pins. I’ve had bad caps around them cause these RS-232 voltages to not appear and thus the chip ‘has a failure to communicate’.


 My ATmega168 had an internal RC oscillator running at 8.6MHz out of the box, rather faster than the nominal 8MHz. I had to adjust the Serial Port Timing Value quite a bit to get it to talk to me legibly at 9600 8-n-1. AVRs can be programmed via the ‘Fuses’ dialog box tab to output their clock signal on an external pin where it can be measured by a frequency counter (be sure there’s nothing else connected to that specific pin when you do). Then you can calculate the Serial Port Timing Value according to the equations in the spec sheet. 


If you don’t have a frequency counter, you can get some results by just incrementing (or decrementing) the “Serial Port Timing Value” until you get something legible. This is tedious – Raise it by one, recompile, reprogram, grunt, try again. And again. And again. Then decide you’re going the wrong way, start back at the original value, and start going down. By one, each time. Recompile. Reprogram. Test. Grunt. Try again. Sometimes you can read some of the characters, then you know you’re closing in… 


It’s difficult (although not impossible) to get the keypad working without the serial port, but not recommended. Expert programmers only.


To get the keypad working:


 Now, once that’s done, to get the keypad working, we have to create a lookup table.


 First, we run the loop test. It should loop incessantly, dumping FF (or close to it, I’ve seen FE now and then) to the terminal. Then push a key on the keypad, and hold it down. Expect something strange on the start and end of the keypress, because the averaging will do weird things when you contact or release the key halfway through the averaging process. Once you have a stable (or mostly stable – Doesn’t hafta always be the same thing, but should only bounce between two adjacent hexadecimal numbers (eg. C6 C6 C6 C6 C7 C7 C6 C7 C6 C6 C6 C6…) For example, I got 'A7' when I pushed the button labeled '9'.


 Then write down the hex value(s), and what key you were pressing. Repeat for all fifteen other keys. Now you can build your lookup table.


 Start with the blank lookup table (KEY\_LOOKUP\_BLANK). Locate the hex value by looking at the comment lines for the first digit of the hex value to get the line, then look at the column indicators for the second hex digit. Once you’ve found it, put into that location whatever key you were pressing. If it bounced between two values, put the character on the key into both locations. Again, I put '9' in location 'A7'.


Hopefully all the different key label characters will be in different places, with some blank space in between. If not, you’ve either blundered in your wiring (shorts? opens? Bad solder joints?) or have seriously off resistor values, or something else is wrong. Fix the hardware, then try again. Notice my own unit isn’t that great in this respect, but it is good enough.


Then add, on both sides of every character, a couple more of the same character, just to widen the range of ‘acceptable values’. Don’t overrun the space of a neighboring character – That’d just cause confusion. Some may wind up butting up against each other, that’s okay. Mine do. Try to give each character equal bi-directional space. I don’t recommend filling in all the blank spaces – There ought to be known invalid values – But you can if you want to.


 Now you have constructed a lookup table. Comment out (put a semicolon before) the ‘rjmp LOOP\_TEST’ line under MAIN:. Rename your lookup table to KEY\_LOOKUP and rename mine KEY\_LOOKUP\_OLD (or something like that).


 Then push some buttons on your keypad, and see if they work. If they do, you should get whatever you put into the lookup table for each location printed on your terminal. If they’re reliable and consistent, feel free to integrate them with your own program. Have Fun!!


Fun things for others to do (Let me know if you do!):


 Make it do something useful. My design is part of a LynxMotion arm controller, and uses interrupts and clever motion control algorithms to (hopefully eventually – it doesn’t dance yet) make it play chess. Most of the weirdness on the PCB layout has to do with that.


 [LynxMotion is a trademark of someone other than me, and used without permission. I have no affiliation with LynxMotion nor anyone else with legal rights to that trademark except as a happy customer (I bought one. It’s pretty cool).]


 Rewrite it in C. I used assembler code throughout.


 Get the resistor ladder balance better. I wound up with an awful lot of keys in the $Cx range, and very few down near the ‘B’. Not quite sure how that happened, but it doesn’t matter for now.


 Fix my ‘zero-cycle divider’ to round off properly. Without using too many cycles. And without getting weird results on a key release.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4

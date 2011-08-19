# Input Event Library

Uploaded by alank2 on 2011-08-19 11:55:00 (rating 5 out of 5)

## Summary

Title : Input Event Library  

Author : SA Development  

Version: 1.00


FEATURES:  

---------


Event Based - If the user pressed B1 (button 1), you will get a single event indicating that B1 was pressed. This is much easier to handle than checking the state of an input and trying to determine if you have already responded to it or not. If they press B1 twice, you get two events.


Two Functions Per Input - It allows you to assign two functions to a single input. Press and release B1 for one function, or HOLD B1 for X seconds for a second function. These will be reported as two different events so you can easily process them separately and one will not mistakenly invoke the other.


Multiple Input Combinations - It recognizes combinations of inputs as unique events. So you can also use any combination of inputs to perform different tasks: Press and release B1+B2 for a third function, and HOLDING B1+B2 can execute a forth function. With the combination and hold features combined, you can make X inputs perform ((2^X)-1)*2 distinct functions! 2 inputs -> 6 functions, 3 inputs -> 14 functions, 4 inputs -> 30 functions, and so on. This could allow you to implement less inputs and/or reduce your component count.


Circular Event Buffer - This allows the user to press buttons ahead of time which are stored in the event buffer so your code can process them when available. This is especially helpful for navigating menus. If your user knows they have 4 button presses to get to the menu they want, it will work consistently every time. This is much better than making them wait for each menu before they can press a button to continue. For a critical prompt where you don't want any buffered events to effect it, simply clear the buffer before the propmpt.


Hold Repeating - This allows a user to hold an input down and it will repeat at a certain time such as 1 second and then continue repeating at a different interval such as 10 per second. This allows the user to change settings or values very quickly and efficiently.


Debounces with Low CPU Usage - Debouncing often involves delaying and reading the input multiple times to debounce it. This library is designed to debounce without any delays that would waste CPU time. It requires a timer ISR that runs on a fixed period that calls ProcessInput() with the current input status each time it fires. It may take a number of these fires to debounce, but it never wastes cycles by delaying. At the same time, as long as this ISR fires quickly enough, input will never be missed. I usually already have a timer like this to keep track of time implemented anyway.


Low Memory Footprint - The code is optimized to use smaller data types when possible to save flash and sram.


Multiple Input Types Supported - It supports any type of input system you might have. It will work with inputs each on their own pin, a resistor ladder/ADC, or even a matrix. You can convert any of these to send it in the call to InputProcess().


EXAMPLE:  

--------


I just finished a project that only has two input buttons. Here is how I am using them:


Press and release B1 to subtract one from the value.  

Press and release B2 to add one to the value.  

Press and hold B1 for 3 seconds to enter setup.  

Press and hold B2 for 3 seconds to rotate through all statistics on the display.  

Press and release B1 and B2 together to instantly display the value no matter what was on the screen.


5 distinct functions from only 2 inputs!. I still have one available if I need another.


INSTALLATION:  

-------------


Three files are provided:


inputevent.c - Main code file, you must add this to your project under "Source Files".


inputevent.h - Main include file, you must include this in any files you wish to use the library.


inputevent\_settings\_example.h - This is an example of the inputevent\_settings.h file that the library requires (and will try to include). The settings that are intended to be customized for each project are located in this file.


The advantage to this is that the main C/H files are unmodified and can be updated to a new version without losing custom per project settings. Another advantage is that since they are unmodified, you can put them in a shared or library directory and use them in multiple separate projects. Then you only have one place to update them instead of multiple project directories.


Two ways you can implement this:


Non-shared method:


1. Copy these files into your project directory.  

2. Rename "inputevent\_settings\_example.h" to "inputevent\_settings.h".  

3. Set the values appropriate to your project in "inputevent\_settings.h".  

4. Add the inputevent.c to your project.  

5. Put "#include "inputevent.h" in any of your C files that need to use the functions.


Shared method:


1. Create a shared directory.  

2. Copy these files into this directory.


To use it with a project:


1. Copy "inputevent\_settings\_example.h" to your project directory as "inputevent\_settings.h". NOTE THE "\_example" was dropped from the filename.  

2. Set the values appropriate to your project in "inputevent\_settings.h".  

3. Add the inputevent.c to your project.  

4. Put "#include "..\shared\inputevent.h" in any of your C files that need to use the functions. You may have to modify this to point to your shared directory.  

5. Project -> Configuration Options -> Include Directories -> New -> Add your project directory. It should put a ".\" in the list. This step is necessary because when the library tries to include "inputevent\_settings.h", it will look in your project directory and grab the one customized for that particular project. This is why it is important NOT to have a inputevent\_settings.h in your shared directory and why I have this file named inputevent\_settings\_example.h instead. You can leave the example file in the shared directory as a file to copy and rename when starting a new project.


HOW TO USE:  

-----------


This library is very easy to use and does nearly all the work for you. It debounces all of the inputs and generates events based on them. Each event represents one or more inputs (possibly all of them) and a flag that indicates if they were just pressed and released or held down for a period of time. Without the event design, you would have to watch an inputs status and keep track of whether you have already responded to it or not. You can change the size of the buffer that holds these events in the settings header (INPUT\_EVENT\_BUFFER\_SIZE).


How an event is created:


All of the inputs are debounced based on the INPUT\_DEBOUNCE\_CYCLES. It is important to understand that this is the number of cycles and not necessarily the time. For example if you have INPUT\_DEBOUNCE\_CYCLES set to 6 and your ISR fires every 5 milliseconds, then you have a 6*5=30 millisecond debounce time. The debounced inputs are stored in v\_InputState.


When no inputs are activated, it is at an inactive state.


As soon as one or more inputs are activated, it signals the beginning of an event. It resets a counter that tracks the number of ISR cycles and also remembers which inputs were activated.


If the inputs change (but aren't all released), it will remember any newly added inputs and reset the counter. It will not forget inputs that were released.


If the inputs are all released before the v\_InputHoldCycles number of cycles, it will add the event into the event buffer.


If the inputs are not all released before the v\_InputHoldCycles number of cycles, it will add the event into the event buffer with the INPUT\_HOLD flag set. Once this event has been reached, it will no longer allow new inputs to be added as it did before. As long as any buttons are still pressed and the InputHoldRepeatCycles is not zero, it will repeat this event every InputHoldRepeatCycles cycles. When all inputs are released it will reset.


If there is no room in the event buffer, the event will be lost.


To implement this you need to have an ISR that fires fast enough to poll your inputs and report them using the InputProcess function. I usually use CTC mode and a timer that runs from 1 to 5 milliseconds. The InputProcess function accepts a parameter that is the raw inputs. Depending on the INPUT\_NO\_OF\_INPUTS value, the parameter may be 8, 16, or 32 bits. You will want one input per bit and you can use all bits except the top bit which is reserved for the INPUT\_HOLD flag. If you aren't using the hold features (INPUT\_ENABLE\_HOLD\_FEATURES set to 0) then you can also use the top bit as well. This is all that you need to do to feed the library.


At this point you just need to check the v\_InputEventCount to see if any events are available and then call InputGetEvent() to grab them. Each event can be a combinations or one or more inputs and it may or may not have the INPUT\_HOLD flag set.


If you call InputGetEvent() and no events are available, it will WAIT for one.


Here is a common loop I use to process input:



```
<br />
while (v\_InputEventCount) //the while allows us to process all the events that are waiting<br />
  {<br />
    switch (InputGetEvent())<br />
      {<br />
        case \_BV(MINUS\_BUTTON)                    : //only minus pressed<br />
                                                    break;<br />
        case \_BV(MINUS\_BUTTON) | \_BV(PLUS\_BUTTON) : //both plus and minus pressed<br />
                                                    break;<br />
        case INPUT\_HOLD | \_BV(MINUS\_BUTTON)       : //minus was held<br />
                                                    break;<br />
      }<br />
  }<br />

```



This function will wait until there is no inputs pressed (it simply loops while v\_InputState!=0):


WaitNoInput();


This function will clear the event buffer:


ClearEventBuffer();


You can set these two variables by using an atomic block:



```
<br />
ATOMIC\_BLOCK(ATOMIC\_FORCEON)<br />
  {<br />
    v\_InputHoldCycles=3000;<br />
    v\_InputHoldRepeatCycles=0;<br />
  }<br />

```



In this example, the hold delay is set to 3000 (remember ISR cycles). Since my timer ISR runs at 1ms, this is 3000ms or 3 seconds. If you hold a button or group of buttons for 3 seconds, an event will be generated with the INPUT\_HOLD flag set. Note that the v\_InputHoldRepeatCycles is set to zero! This means that after the initial INPUT\_HOLD event is generated, there will be no repeating INPUT\_HOLD events after it! If v\_InputHoldRepeatCycles was set to 100 cycles for example, that would be 100ms based on my timer ISR speed which means it would generate repeating INPUT\_HOLD events 10 times per second until all inputs are released.


The above example of 3000 and 0 is probably ideal for dual use buttons where pressing/releasing a button does one thing, but holding a button does a secondary thing.


If you wanted to use the button to set a value, you might try using 1000 for the hold and 100 for the repeat. This would mean holding a button would have it generate its first event at 1 second and then 10 times per second until released.


Please take the time to read through each of the source files as I tried to comment them all very well and hopefully they are clear and make sense. If you have any questions or discover something that doesn't work as you would expect, drop me a PM here at avrfreaks!


VERSION HISTORY:  

----------------


1.00 - Initial version.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)

STK500 Control alpha 1.01, july 10 2002, PM 9:52 CET
----------------------------------------------------


Description
-----------
STK500 Control is a small delphi application that utilizes the STK500 command
line software to access STK500 / AVRISP.
Once set up it can be set in small mode and always on top, for easy access
to the PROGRAM button while not in the way of any other windows.
The program applies to users of compilers without a programming capablility of
the STK500, which usually have to open AVR Studio to program the AVR.


Future plans
------------
I had plans to redirect the output from STK500 to the status field. However,
that was not as easy as it first seemed. If anyone knows how to do this I
would apprechiate it if you told me (peter_griffin_@hotmail.com).


History
-------
alpha 1.01   Fixed fuses/lock bits bug. FFFF/FF was written regardless of the 
             fuses/lock bits fields.
             Added ability to program oscillator calibration byte.
             
alpha 1      Initial release.


Feel free to hack the code or give me some clues.

Peter.
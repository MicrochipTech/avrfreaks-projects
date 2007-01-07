# gcc module setup wizard

Uploaded by outer_space on 2007-01-06 18:27:00 (rating 0 out of 5)

## Summary

This only works with T26 for certain things I use frequently. Its probably not what you are looking for.


For the pulldown tool menu. Configure modules easily by selecting menu options. Opens wordpad with commented module setup code. This is different from other wizards, it does not generate an entire template file. It only generates code for 1 module at a time for copy pasting into your file or just for a quicker reference than the datasheet.


Right now this is only for the tiny26 and only for modules timer0 timer1, ADC, sleep modes, and watchdog. This can be expanded for other modules, tasks, or chips. Revised to be much clearer using ostream and string arrays instead of many repetetive cout statements.


Compile the code with this command  

g++ avr\_regset.cpp -o avr\_regset.exe


Then place avr\_regset.exe and avr\_regset.bat together somewhere.  

Add avr\_regset.bat to the avr studio tools menu. There must be 2 arguements, the first processor ex t26 the second MHz chip clock.  

Ex. 'avr\_regset.bat t26 2'


avr\_regset.exe puts the generated code into a text file, avr\_regset.bat opens this text file with wordpad. The command prompt will stay open until wordpad is closed.

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)

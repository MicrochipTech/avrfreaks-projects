# Accurate Delay Library 1.02

Uploaded by alank2 on 2011-08-19 11:45:00 (rating 5 out of 5)

## Summary

Title : Accurate Delay Library  

Author : SA Develpoment  

Version: 1.02


INSTALLATION:  

-------------


Two files are provided:


adelay.s - Main ASM code file, you must add this to your project under "Source Files".


adelay.h - Main include file, you must include this in any files you wish to use the library.


adelay\_settings\_example.h - This is an example of the adelay\_settings.h file that the library requires (and will try to include). The settings that are intended to be customized for each project are located in this file.


The advantage to this is that the main S/H files are unmodified and can be updated to a new version without losing custom per project settings. Another advantage is that since they are unmodified, you can put them in a shared or library directory and use them in multiple separate projects. Then you only have one place to update them instead of multiple project directories.


Two ways you can implement this:


Non-shared method:


1. Copy these files into your project directory.  

2. Rename "adelay\_settings\_example.h" to "adelay\_settings.h".  

3. Set the values appropriate to your project in "adelay\_settings.h".  

4. Add the adelay.s to your project.  

5. Put "#include "adelay.h" in any of your C files that need to use the functions.


Shared method:


1. Create a shared directory.  

2. Copy these files into this directory.


To use it with a project:


1. Copy "adelay\_settings\_example.h" to your project directory as "adelay\_settings.h". NOTE THE "\_example" was dropped from the filename.  

2. Set the values appropriate to your project in "adelay\_settings.h".  

3. Add the adelay.s to your project.  

4. Put "#include "..\shared\adelay.h" in any of your C files that need to use the functions. You may have to modify this to point to your shared directory.  

5. Project -> Configuration Options -> Include Directories -> New -> Add your project directory. It should put a ".\" in the list. This step is necessary because when the library tries to include "adelay\_settings.h", it will look in your project directory and grab the one customized for that particular project. This is why it is important NOT to have a adelay\_settings.h in your shared directory and why I have this file named adelay\_settings\_example.h instead. You can leave the example file in the shared directory as a file to copy and rename when starting a new project.


HOW TO USE:  

-----------


I required a smaller delay function than the standard \_delay\_us which does very poorly in the ns area despite that it can take fractional parameters such as 0.1us for 100ns. My other complaint about the standard delay functions is that they don't always wait enough time and may fall slightly short of the requested time. It is certainly understandable that the delay might take a little longer, for example if you are running 4mhz and ask for a 100ns delay, since one cycle is 250ns, you should get a 250ns delay. The standard \_delay\_us yields 4 instructions for a 1us delay when asked for 100ns which is overshooting by 3 instructions.


My first goal was to obtain cycle accurate delays. This is much more difficult than I expected because of the optimizing nature of GCC. I found that I had to put my delay cycles in assembly. This is the ONLY way I found to get GCC to reliably produce the same exact code each time (so the exact number of cycles can be predicted).


My functions will never undershoot a delay. You will always get at least that much delay.


The DelayCycles(unsigned long) function can properly delay for 0 to 4294967295 cycles with 100% accuracy. They will accomplish this in 0-7 instructions.


The Delay\_ns(double), Delay\_us(double), Delay\_ms(double), and Delay\_s(double) defines will accept a non-integer value such as: Delay\_s(3.5) for 3.5 seconds. They use the F\_CPU define to calculate the number of cycles required so the floating point calculations are all done at compile time and so it does not need to link in the floating point library.


ONE important thing about DelayCycles, Delay\_ns, Delay\_us, Delay\_ms, and Delay\_s is that these EXPECT A CONSTANT VALUE supplied to them. This is because they are timed to be optimized fully by GCC and this won't happen if you supply a variable or some other value unknown to the compiler at the time it is compiled.


IF you have a need to specify a variable or some value not known to the compiler at compile time you need to put a "Variable" in front of the above function names to call the versions that expect not to be fully optimized, such as VariableDelayCycles or VariableDelay\_s(3.5). These functions are not 100% accurate because the code to make them be fully accurate itself causes inaccuracy. They will overshoot by typically up to 4 or 8 cycles depending on whether the number of cycles is above 65535 or not, but can overshoot by as much as 12 cycles in some situations. Not as accurate as the non-variable functions, but much more accurate than using a non variable function with a variable. Since their values are not known at compile time, we can't use floating point to calculate the number of cycles, so the VariableDelay\_[ns,us,ms,s] functions are more limited in the largest amount they can accept. See the note listed with them on how to calculate the largest amount.


These will work properly on any optimize setting from -O1 to -Os. On -O0 inline functions are disabled which will increase the delay time, but this would only be very noticeable in smaller delays.


VERSION HISTORY:  

----------------


1.00 - Initial version.


1.01 - Reorganized all code to follow the standard C and H file techniques.


1.02 - Bug fix because the previous implementation did not optimize the functions as inline properly.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)

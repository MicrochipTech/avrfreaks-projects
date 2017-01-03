# AVR Visual Programming

Uploaded by milgo on 2017-01-02 21:35:00 (rating 5 out of 5)

## Summary

 


What I've created is a environment containing open source software to build applications for AVR microcontroler (atmega8 in this example) using diagrams in DIA (Microsoft Windows only).  

This environment consists of software like: imagemagick, doxygen, saxon xslt, dia diagram editor, java, WinAVR.


Basecally it works like this:


1. Modify functions.c which contain function definitions
2. Modify functions.h which contain function declaration
3. Generate functions blocks for DIA using generate.bat script (as administrator)
4. Open DIA to draw your program using previously generated function block (drag blocks from side bar)
5. Connect blocks with each other.
6. Save and compile program using compile.bat script (writing "compile.bat program\_name.dia" - program\_name.dia must be in the same directory as compile.bat).
7. Download binary to device (you can use download.bat script but you must modify MAKEFILE in src directory to use with your programmer)

 


So as you can see there are to scripts generate.bat and compile.bat. What they do is they transform doxygen and dia files (which are XML files) using special XSLT transformations and they output them as other files.  

This project was tested on atmega8. You can try for other devices it should work but you must modify makefile and setup.c in src directory.


How to install:


1. Install newest Java JRE  (add it's directory to PATH environment variable)
2. Install imagemagick  (add it's directory to PATH environment variable)
3. Install Doxygen (add it's directory to PATH environment variable)
4. Install SAXON XSLT (set SAXONDIR variable in setup.bat to point directory to saxon main directory)
5. Install DIA diagram editor (set DIADIR variable in setup.bat to point directory to DIA main directory)
6. Install WinAVR (add it's directory to PATH environment variable)

 


![](https://3.bp.blogspot.com/-I74eq8HZnjY/WC-K_quh0hI/AAAAAAAAAOI/26TMCylIgQIaL0-jqEnUgkD1r3K6uwujgCLcB/s1600/dia-screenshot.png)


 


![](https://3.bp.blogspot.com/-CDDJKcVK0qU/WC-T2YGXZjI/AAAAAAAAAOk/ekqYGSCGiFAnC8lVMJabvc3Q5N36K9RngCLcB/s1600/IMG_1834.JPG)


For more information visit develimgo.blogspot.com

## Links

- [dia-visual-programming-avr](https://bitbucket.org/limgo/dia-plc/src/a4e341e1a79d2558a13f6573ba99f1a96052bf9a?at=avr)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)

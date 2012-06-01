# Text Editor using AVR

Uploaded by visakhanc on 2012-06-01 13:24:00 (rating 5 out of 5)

## Summary

TYPE IT! - A small text editor on AVR using PC keyboard and LCD  

---------------------------------------------------------------


(see Video demo: <http://youtu.be/a2SDs7H5KDE>


(see Pictures:


[http://s19.postimage.org/6aytqzp...](http://s19.postimage.org/6aytqzpgj/Pict1.jpg)  

[http://s19.postimage.org/7ska2ju...](http://s19.postimage.org/7ska2ju77/Pict3.jpg)


Introducion and Usage  

---------------------------


 This text editor is made up of a PS/2 keyboard, a 16x2 LCD and the AVR microcontroller. Anything typed with the keyboard is displayed on the LCD and can be saved in the flash memory of the AVR. The text is saved with a file ID from 1 to 80. So, maximum number of such files is 80. The size of each is limited to 80 characters. (The maximum number of files depends on the AVR device; I used ATmega16)


 The INTERFACE of 'TYPE IT!' is a MENU with three options: NEW, OPEN and EDIT. It can be navigated using LEFT/RIGHT arrow keys and selected by ENTER key. 


NEW:  

 Brings blank space in the LCD for the user to type in the text. Basic text entry, editing, and navigation using arrow keys are available. Upon completion of entering text, it can be saved by hitting TAB key. The prompt asks for a file ID. Any valid file ID(from 1 to 80) can be given and upon pressing ENTER key the text is saved into the flash ROM. The editor can be exited using ESC key; but, if the text is not saved or any change is made to the saved text, a prompt asks whether to save or not. 


OPEN:  

 Opens a saved text for viewing only. It asks the user to enter the file ID of the text previously saved. When a valid file ID is given, the text is displayed on the screen. It can be scrolled by LEFT/RIGHT arrow keys. To exit to the menu press ESC.


EDIT:  

 A saved text can be edited using this option. When a file ID is given the corresponding text is displayed on the LCD for editing. When exiting, if the file is changed, a prompt asks whether to save or not. 


Hardware Components  

---------------------------


1. AVR ATMega16 (Or any other with enough flash and self programming(SPM instruction), but requires modifications to source)  

2. Any old PS/2 Keyboard  

3. 16x2 character LCD  

4. PS/2 socket.  

5. 5v dc power


Construction  

------------------  

This can be set up on an ordinary line PCB. After wiring up and soldering the circuit, flash the AVR with hex file and set fuses matching the frequency. That's all. 


Compiler : AVR-GCC  

-------------------  

WinAVR is easy. The frequency is set as 8MHz in Makefile.


ACKNOWLEDGMENTS  

-----------------------  

The PS/2 driver for the keyboard is taken from AVR app. note: AVR313


----------------- LICENCE --------------------  

Use it for anything anyway you want.


feel free to contact me for any help!


 [visakhanc@gmail.com](mailto:visakhanc@gmail.com)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)

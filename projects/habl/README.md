# HABL

Uploaded by xehpuk on 2007-06-27 17:47:00 (rating 0 out of 5)

## Summary

Henriks AVR Boot Loader


At reset this boot loader will give a short help text.  

Then it will wait 3 seconds for the user to cancel auto boot. 


Auto boot is canceled by pressing ESC exactly 5 times during the 3 seconds,  

this to avoid entering download mode due to noise or other input.


In download mode it will expect an application to be set using xmodem.


I have reused code from some other projects here at avrfreaks,  

adapted it for atmega88, added the auto boot part and a little  

bit of documentation.


I have tested it on atmega8 and atmega88. Others have used it  

with atmega32, one bug found and now corrected (thanks Jerry).


<http://www.eit.se/habl/>

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)

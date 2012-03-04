# USB password generator

Uploaded by jokkebk on 2012-03-04 08:33:00 (rating 0 out of 5)

## Summary

By utilizing the V-USB library, I made a device that stores a previously generated password in EEPROM and inputs it to PC every time the device is connected. 


The device implements the USB boot HID keyboard protocol so once plugged in, the device can receive keyboard LED state changes (caps lock, num lock, scroll lock), which are used in the firmware to trigger new password generation (four LED toggle messages, i.e. four caps lock presses). 


Randomness for the generated password is also taken from LED state change timings - ATtiny85 timer is left running and every time an additional state change happens, lower 6 bits in TCNT0 are used to add one character to the password - so for a new 10-character password (default setting), a total of 4+10 caps lock presses generate a new password, which is stored in EEPROM.


Full source code is attached, and more detailed explanation along with a Youtube video showing the device in action is available in my blog:


[http://codeandlife.com/2012/03/0...](http://codeandlife.com/2012/03/03/diy-usb-password-generator/)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)

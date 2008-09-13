# AVR Color LCD 320x240

Uploaded by pjwl on 2008-09-13 13:50:00 (rating 0 out of 5)

## Summary

This is a project with the Hitachi TX14D11VM1CAA is controled with the LCD controler S1D13705 and the ATmega162.


Version 1.0: Release  

- Mod: Function CF\_Info disabled for memory programm space  

- Mod: Added touchscreen driver  

- Mod: Added touchscreen calibration  

- Mod: Added vertical and horizontal line drawing S1D13705  

- Mod: Added LTC1298(ADC) driver for touchscreen control  

- Mod: When pressed on the touchscreen a cross is drawed  

- Mod: Added storing calibration coeffecients and touchscreen flag register in eeprom


Version 0,6: (Aplication tool version is update to 1.1)  

- Mod: Increased the receiving speed a lot!  

- Mod: Changed Blok to Page  

- Mod: Changed calculation of the sector of the CF  

- Mod: Changed Show\_BMP '1 to 32' to '0 to 32'  

- Bug: Only Page 1 was written


Version 0,5:  

- Compact flash driver added  

- CF info present on LCD(When choosed in Terminal menu)  

- Application tool avaible to send BMP to the board  

- Draw picture from CF data to S1D13705  

- Added Terminal menu for settings  

- Link menu present(When choosded in Terminal menu)  

- Baudrate increased to 57600, this the max of 16Mhz  

- Make a define option in S1D13705.h for more info of the S1D13705

## Links

- [AVR Color LCD site](http://www.lompec.nl/avrcolorlcd.html)
- [Lompec Webshop](http://www.lompec.nl/WebShop)

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code with HW
- CodeVisionAVR-Standard

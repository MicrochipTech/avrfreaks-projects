# Open Source AVRISP-MKII

Uploaded by abcminiuser on 2010-11-22 11:02:00 (rating 0 out of 5)

## Summary

After first creating my LUFA USB stack for the USB AVRs, I decided as a side project to create a new firmware for the entire USB AVR range, which would emulate an AVRISP-MKII. This project is a clean-room copy of the AVRISP-MKII functionality, supporting the programming of both the standard AVR range via ISP as well as the newer XMEGA AVRs via PDI and 6-pin TINY AVRs via TPI. It is entirely open source and has a wide range of configuration options to adjust the feature set and pinout.


When compiled for the XPLAIN board target, the code will automatically function as a PDI programmer for the XPLAIN board using the optimal settings. This will allow the XPLAIN board to be programmed from within AVRStudio without additional hardware.


For device pinout information, build the Doxygen documentation from the source by executing "make doxygen". The resulting documentation will be located in the /Documentation/html/ directory of the source folder.


Features:  

 * ISP, PDI and TPI protocols supported  

 * 4MHz ISP Rescue Clock (for incorrectly set system clock fuses)  

 * Fast, native USB operation  

 * Works on any USB AVR model  

 * Works with anything the official AVRISP-MKII works with (e.g. AVRStudio)


NOTE: AVRFreaks is currently broken and I cannot post a link to the AVRISP-MKII specific project page ([http://www.fourwalledcubicle.com...](http://www.fourwalledcubicle.com/AVRISP.php)). The download link below links to the LUFA project page, which contains the AVRISP-MKII project source as part of the library download package.

## Links

- [Project Webpage, with download link](http://www.fourwalledcubicle.com/LUFA.php)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)

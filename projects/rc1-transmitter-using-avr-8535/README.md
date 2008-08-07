# RC1 transmitter using AVR 8535

Uploaded by Savanth on 2008-08-07 16:12:00 (rating 0 out of 5)

## Summary

Canon cameras are provided with IR remotes for remote-click operation. The circuit duplicates this RC1 IR code. SAA3010 is a commonly used RC5 transmitter chip used in TV remotes and the circuit is designed to monitor the code output from this chip. When the center key is pressed the RC1 (instead of RC5) format transmitted so that the camera operates. For all other keys the corresponding RC5 format is transmitted. This feature allows other custom built RC5 receivers to operate other actuators (for zoom, pan etc) with the center key configured for 'click' operation.  

Atmega8535 interrupt0 is used to detect key codes from SAA3010. A diode network disables the transmission from SAA3010 on center key press and RC1 is sent via PD7 of the AVR8535. Note that the default setting of the internal oscillator is used. Hence an external crystal is not needed reducing component count. The .zip file contains all necessary files including schematic, asm code, tools used for downloading to target and the pictures showing the RC1 format. Please ensure that path is included while extracting.  

As an improvement the thoughts are to activate sleep modes in the controller that will save valuable battery power. Any related suggestions or comments are Welcome.


The 40pin Uc is wasteful. Will soon put up a 8pin ATtiny version.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)

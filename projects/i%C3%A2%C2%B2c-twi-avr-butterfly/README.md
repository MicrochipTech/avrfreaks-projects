# IÂ²C, TWI on the AVR Butterfly

Uploaded by topazx2 on 2009-09-30 17:52:00 (rating 0 out of 5)

## Summary

The AVR Butterfly does not have a traditional hardware TWI (IÂ²C) interface, it does however have a USI-TWI interface which is a bit harder to use, but still better than a bit-banged version.


 In this project, I have added IÂ²C protocol to the main menu of the AVR Butterfly. This code is written specifically for the IÂ²C-IT sensors that HVWtech sells.  

 Here's a direct link to the sensor:  

[HVWTech IÂ²C-IT Sensor](http://www.hvwtech.com/products_view.asp?ProductID=665)  

The code must be installed through the ISP port using AVR Studio and the GNU GCC IDE. It cannot be installed through the bootloader since it is actually the code that contains the bootloader.


 To use the IÂ²C-IT on the butterfly, you will need to add pull-up resistors to both the SCL and the SDA lines, anything from 1.8k to 10k should work. Also, the IÂ²C-IT sensor requires 5v while the max voltage fro the Butterfly is only 4v, so I modified an Ecros AVR carrier board to allow for both voltages.


 Once the code is installed, you can select the IÂ²C function by clicking the up arrow once and then right once. This takes you to the readout of the sensor in inches. If you would rather have the results in cm, simple use the up or down arrows which will toggle between inches and cm. If you have more than one sensor attached and you have set the addresses differently (see IÂ²C-IT manual) then you can select which sensor you want by clicking to the right once more and then use the up or down arrows to choose between three different addresses. (I may update this with more selections later, but for now, this was all I needed). Once you have selected the address you want. click the select button by pressing down on the joystick.


 Clitches:  

 There is one known glitch at this time that seems to freeze up the system. If you select an address that is not on the line, and there is some other device on the line, the system freezes and the only way I know to fix it is by loading the OLD menu code and then reloading the IÂ²C code. It's a pain and I would like to know what is causing this, but I haven't found it yet.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)

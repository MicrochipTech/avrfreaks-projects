# ATtiny26 Driving S6B0755 GLCD.

Uploaded by leroy157 on 2012-03-14 08:25:00 (rating 0 out of 5)

## Summary

This is a program I have compiled using mikroC to display text on an old Electronic Gold Mine GLCD 79294 SDI. These displayâ€™s have been sold out but I had a number of them and wanted to find a use for them. They use a S6B0755 driver controller, I have modified the original header and c files from the site (Outguessing the machine) where the original files had been written for a Pic, they are now useable with an AVR chip. The font header file was created with MikroElectronic Font Creator as an mikroC GLCD lib. Then copy and paste to header file . The output to GLCD pin assignment is located in the s6b0755 header file and can easily be reconfigured for different pin arrangements. This all fit into an ATtiny26. It will display text strings, and put ascll characters on the screen, it will also reverse print direction, and screen black on white can be changed to white on black. In order to keep the file small I omitted all graphic functions as I did not need them. I compiled the files for a Tiny26 using the 8mhz internal osc.  

Included are the c and h files, the hex file, and the Mikroc proj file.  

I used the STK-500 board and a jumper cable I describe how to make in the PDF for the ATtiny 26 voltmeter

## Tags

- Complete code

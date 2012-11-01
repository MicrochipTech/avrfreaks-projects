# ATxmega programmer for $0.50

Uploaded by Electronic_Project on 2012-11-01 08:58:00 (rating 0 out of 5)

## Summary

I found this project on web. I don't it really works or not! But seems ok.






  



[![A simple interface enables USBASP to program ATxmega16A4 (PDI protocol)](https://lh3.ggpht.com/-zsJUyHApD7U/UCwPZZR5QsI/AAAAAAAAAg4/uSP9PSF89vA/s640/connected.jpg "A simple interface enables USBASP to program ATxmega16A4 (PDI protocol)")](http://1.bp.blogspot.com/-zsJUyHApD7U/UCwPZZR5QsI/AAAAAAAAAg4/uSP9PSF89vA/s1600/connected.jpg)  



## 
ATxmega programmer for $0.50 or how to make your USBASP support PDI



[![](https://lh3.ggpht.com/-nBHhlefAhfM/UCwPtz9a80I/AAAAAAAAAhA/DpWXyA01qRI/s200/xmega.jpg)](http://3.bp.blogspot.com/-nBHhlefAhfM/UCwPtz9a80I/AAAAAAAAAhA/DpWXyA01qRI/s1600/xmega.jpg)
Do you know [XMEGA](http://www.atmel.com/products/microcontrollers/avr/avr_xmega.aspx), the new family of AVR microcontrollers? They look really sweet, especially for someone who already mastered the AVR architecture. More speed (32MHz!), more peripherals (5 USARTs! 16 PWM channels!), modular design, and you can still use the same well known tools. And then you read about the programming interface and... what?! What happened to ISP? No way I would purchase a completely new programmer model just to play with their new MCU!  
  

  





|  |
| V-USB interrupt disturbing the PDI clock
 - but not breaking it |


Fortunately, implementing the new programming protocol in [USBASP](http://www.fischl.de/usbasp/) was not that hard. Initially I thought it would be almost impossible, because PDI requires a constantly clocking signal and disconnects if the clock rate drops below 10 kHz. Strict timing is a problem for [V-USB](http://www.obdev.at/vusb/) (USB protocol implementation used in USBASP) as it handles all USB transmissions in software and cannot do both things at the same time (like a guy who lacks brain power to walk and talk at the same time). The tests showed that the interrupt handling a basic control transfer only steals 40-50 us, and this is short enough to allow for the reliable PDI bit banging. Sending more payload (more than a regular control message) would break the clocking but the programmer can be scheduled to work in batches, so it gathers all data from the usb and only then begins the PDI transmission (hoping that there will be no new USB transmission for a while).  





  





|  |
| Bit-banging clock (to the left)
and timer clock (to the right) |


My implementation switches to the hardware timer clocking to keep the clock running during the interrupt-intense USB transmissions. It is not really necessary because the programmer could also disconnect and reconnect after each transmission, but I think the PDI might like the continuous clock better ;-)  



### 


### 
Physical connection



[![](https://lh3.ggpht.com/-1-YkWfpvvHY/UCwkSFgtTzI/AAAAAAAAAhQ/MgM_IOnGFF0/s200/interface.jpg)](http://3.bp.blogspot.com/-1-YkWfpvvHY/UCwkSFgtTzI/AAAAAAAAAhQ/MgM_IOnGFF0/s1600/interface.jpg)[![](https://lh3.ggpht.com/-A6Tv2B1nmHo/UCwLqlLI1nI/AAAAAAAAAgg/sbIBGnwXzEo/s320/usbasp_pdi_schem.png)](http://2.bp.blogspot.com/-A6Tv2B1nmHo/UCwLqlLI1nI/AAAAAAAAAgg/sbIBGnwXzEo/s1600/usbasp_pdi_schem.png)

If you have a regular USBASP then it is 5V, while the PDI operates at 3,3V. The simple way to reduce the voltage is just adding zener diodes and some resistors. As you can see, SCK becomes PDI DATA and MOSI becomes PDI CLK.
R1 and R2 seem redundant but I put them here after the [official Atmel PDI documentation](http://www.atmel.com/Images/doc8282.pdf). The thing is, the DATA line is bidirectional and the physical layer should be able to detect collisions. My implementation does not do it, but it could, thanks to the MISO line (maybe someone will implement it some day...). Another weird thing is RESET connection, it looks like it should be GND instead, and yes, this is GND, that can be disconnected at will. Permenent GND connection breaks the PDI bus-keeper so it must be disabled before receiving anything from PDI.  
  

If your USBASP is 3,3V then it would be more simple:  




[![](https://lh3.ggpht.com/-Z29VWzYD6mw/UCwO6_1XhKI/AAAAAAAAAgw/nUlaM8B9FWQ/s320/usbasp_pdi_schem_33.png)](http://3.bp.blogspot.com/-Z29VWzYD6mw/UCwO6_1XhKI/AAAAAAAAAgw/nUlaM8B9FWQ/s1600/usbasp_pdi_schem_33.png)
### 


### 
Power supply



If you want to power your circuit from a 5V USBASP then of course you need a 3,3V regulator - but this is not a part of the project.
### 


### 
Software



Obviously, you will need 2 pieces of software:  
  

  

#### 
1. USBASP Firmware supporting PDI



Download USBASP source: <http://www.fischl.de/usbasp/usbasp.2011-05-28.tar.gz>

Patch file: <http://sz.toyspring.com/usbasp-pdi-usbaspfirmware-20120816.diff>

  

Unpack, apply the patch, compile:  
  

  




tar xvzf usbasp.2011-05-28.tar.gz

cd usbasp.2011-05-28/firmware

patch <full/path/of/usbasp-pdi-usbaspfirmware-20120816.diff

make main.hex

  


...and install in your USBASP

  

  
  

Note: ATmega48 is not supported because its flash area is to small for the increased program size. I only tested it on ATmega8 because that's what in my USBASP. ATmega88 should work, too.  



#### 
2. AVRDUDE supporting USBASP with PDI



Get the AVRDUDE source and apply the patch against revision 1092:

<http://sz.toyspring.com/usbasp-pdi-avrdude2091-20120816.diff>


  


svn co svn://svn.sv.gnu.org/avrdude/trunk -r 1092


cd trunk/avrdude

patch <full/path/of/usbasp-pdi-avrdude2091-20120816.diff

./bootstrap

./configure

make

  


  


./avrdude -C avrdude.conf -c usbasp -p x16a4 -U flash:w:../../../xmega/xmegatest.hex  -E noreset
  

  

  




  

  

  

  





avrdude: AVR device initialized and ready to accept instructions

  


Reading | ################################################## | 100% 0.02s

  


avrdude: Device signature = 0x1e9441

avrdude: NOTE: Programmer supports page erase for Xmega devices.

         Each page will be erased before programming it, but no chip erase is performed.

         To disable page erases, specify the -D option; for a chip-erase, use the -e option.

avrdude: reading input file "../../../xmega/xmegatest.hex"

avrdude: input file ../../../xmega/xmegatest.hex auto detected as Intel Hex

avrdude: writing flash (440 bytes):

  


Writing | ################################################## | 100% 0.29s

  


avrdude: 440 bytes of flash written

avrdude: verifying flash memory against ../../../xmega/xmegatest.hex:

avrdude: load data flash data from input file ../../../xmega/xmegatest.hex:

avrdude: input file ../../../xmega/xmegatest.hex auto detected as Intel Hex

avrdude: input file ../../../xmega/xmegatest.hex contains 440 bytes

avrdude: reading on-chip flash data:

  


Reading | ################################################## | 100% 0.27s

  


avrdude: verifying ...

avrdude: 440 bytes of flash verified

  


avrdude done.  Thank you.

  

  

  




  

  

  

  




  

  





And the result:


|  |
| The canonical flashing LED! |


### 


### 
What now?



It works for my ATxmega16A4 + Linux but it would be nice to know if it also works on anyone else hardware (especially that the PDI connection reliability depends on the V-USB interrupts). So... download it, test it (and maybe fix it), and if everything goes well, it could be probably included in the official USBASP and AVRDUDE releases.  

  

### 
Summary


  



* Add a simple interface to USBASP and you can program ATxmega
* If you already have USBASP then the interface can be made in minutes almost for free (but if you have to buy USBASP, it is still the least expensive way to program ATxmega)
* Upgraded USBASP can still program old AVRs (without the interface)
* There is a catch: you have to install a new firmware in USBASP using another programmer

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)

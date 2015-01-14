avrasm2 -fI  %1.asm  -l %1.lst -o %1.hex  
avrdude -p m8 -c usbasp -P usb -U flash:w:%1.hex
rem avrdude -p m8 -c usbasp -P usb -U flash:r:wtc_M8.bin:r

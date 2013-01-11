# CH340 USB-Serial AVR Programmer

Uploaded by eventhorizon on 2013-01-11 12:47:00 (rating 0 out of 5)

## Summary

CH340 Based USB-Serial compatible ICSP programmer. Simply add this programmer to the avrdude.conf:


#CH340 based USB-Serial/TTL adapters.  

programmer  

 id = "ch340";  

 desc = "ttl port banging, reset=!txd sck=!rts mosi=!dtr miso=!cts (invert signals using 74ls04)";  

 type = serbb;  

 reset = 3;  

 sck = 4;  

 mosi = 7;  

 miso = 8;  

;


The reset pin is somehow optional (jump the reset pin of the AVR to ground instead). The pin values on the description are inverted, but the pin configuration is not actually inverted, instead, invert the pins using a NOT gate such as 74LS04. Inverting the pin values through the configuration doesn't help. And since CH340 is a TTL, it has inverted logic. There are times when the programmer can't read the proper device ID, simply redo the command.


*Changing AVR Fuse bits using this programmer is NOT recommended.*


Advantage:  

- Way cheaper than PL2303 based USB-Serial Adapters, FTDI, and USB ICSP programmers.  

- Easy to make which only needs 2 components, the CH340 adapter and a 74LS04 TTL NOT Gate.  

- Doesn't require level-shifting since it is TTL. (+5 and 0v logic)


Disadvantages:  

- Sometimes fails to read the device ID properly.  

- Flashing sometimes takes a few tries.  

- Unreliable for short.

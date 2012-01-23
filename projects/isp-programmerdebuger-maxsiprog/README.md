# ISP programmer/debuger MAXSIprog

Uploaded by admin on 2012-01-22 22:26:00 (rating 0 out of 5)

## Summary

ISP programmer/debuger MAXSIprog


-3..5V Vcc operation (according to MAX3232 datasheet) 3,2V tested (2,6V trusted)  

-No more zener diodes, MAX3232 is the correct HW for connecting to RS232.  

-Remaining Rx channel in MAX3232 is used fo rxing debug printf\_Ps into PC.  

-no need to switch programmer and RS232 convereter during debugging  

-PonnyProg compatible (select SIprog and invert all but reset)  

-AVRdude compatible  

-BRD fits into DSUB connector cover (click-type no screw-type)  

-9pin DSUB female!!!  

-BRD simply used as singlesided (replace bottom layer by wires)


AVRdude setting:


programmer  

 id = "maxsiprog";  

 desc = "Ponyprog like serial using max3232, reset=!txd sck=!rts mosi=!dtr miso=!cts";  

 type = serbb;  

 reset = ~3;  

 sck = ~7;  

 mosi = ~4;  

 miso = ~8;  

 ;

## Tags

- Complete code with HW

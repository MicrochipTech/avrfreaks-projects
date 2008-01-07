# Ethernet Bootloader

Uploaded by ysaad on 2008-01-06 17:11:00 (rating 0 out of 5)

## Summary

Bootloading through standard ethernet connection based on Tuxgraphics's Article06061.  

Both Bootloader code and loader program can be downloaded.


The bootloader program do not support ping and ARP, due to a very small size of bootloading space in ATMega88.  

So static arp will automatically be added to arp cache as the loader program starts.  

The precompiled hex file has the IP of 192.168.1.5 .

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4

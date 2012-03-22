# USI I2C SLAVE for ATtiny CPUs

Uploaded by jim116 on 2012-03-22 11:04:00 (rating 5 out of 5)

## Summary

COMPLETE ASSEMBLER CODE FOR USI I2C SLAVE for ATtiny CPUs


USE external pullups for SDA,SCL pins (4.7k to V+)


USAGE:


I2C WRITE DATA TO SLAVE  

1byte: ADDRESS (=0xAC)  

2byte: SUBADDRESS (=< SRAM SIZE-STACK; from 0 to 120 for ATtiny2313)  

3byte: DATA (will be written to SRAM position =SRAM\_START+SUBADDRESS)  

4byte: DATA (will be written to SRAM position =SRAM\_START+SUBADDRESS+1)  

5byte: DATA (will be written to SRAM position =SRAM\_START+SUBADDRESS+2)  

...  

etc.


I2C READ DATA FROM SLAVE  

1byte: ADDRESS+1 (=0xAC+1)  

2byte: DATA (read from SRAM position =SRAM\_START+SUBADDRESS)  

3byte: DATA (read from SRAM position =SRAM\_START+SUBADDRESS+1)  

...  

etc.


I2C WRITE SUBADDRESS TO SLAVE, READ DATA FROM SLAVE  

1byte: ADDRESS (=0xAC)  

2byte: SUBADDRESS (=< SRAM SIZE-STACK)  

Repeated Start  

1byte: ADDRESS+1 (=0xAC+1)  

2byte: DATA (read from SRAM position =SRAM\_START+SUBADDRESS)  

3byte: DATA (read from SRAM position =SRAM\_START+SUBADDRESS+1)  

...  

etc.


I HAVE SPENDED A LOT OF TIME AND THEN FIND BUG IN ALL ATMEL DOCS:  

SCL DDR and PORT MUST BE ALWAYS =1 !!!!!!!!!!!!!!!!! *  

SDA PORT MUST BE ALWAYS =1 !!!!!!!!!!!!!!!!! *


ATtiny2313 memory use summary [bytes]:  

Segment Begin End Code Data Used Size Use%  

---------------------------------------------------------------  

[.cseg] 0x000000 0x000132 278 0 278 2048 13.6%  

[.dseg] 0x000060 0x000061 0 1 1 128 0.8%  

[.eseg] 0x000000 0x000000 0 0 0 128 0.0%

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4

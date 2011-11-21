# SDRSCHD

Uploaded by gogamoga on 2011-11-20 20:17:00 (rating 0 out of 5)

## Summary

Assembler include file. Documentation inside.  

Simle, quick and small dual task scheduler for ATTiny2313  

Takes only 98 bytes, shares CPU time 1/3 - 1/3 - 1/3 between 2 processes  

and the scheduler itself.


Can provide precise short delays +-1 Clock tick (depends where interrupt kicks in)


only 1 process can request delay, if delay isnt finished and other request  

for delay comes - previous delay will be over immediately


Memory is shared 50/50 between both processes (minus 2 bytes at the end of  

RAM for scheduler use and 2 reserved bytes) each process gets 62 bytes of RAM and its own stack.


Intended to use on ATTiny2313, with internal 1 mhz osc. Minimal delay - 45 clocks, maximal - 300.


Look inside the file before using it.

## Compilers

- AVR Assembler

## Tags

- Part-specific Lib.functions
- AVR Assembler

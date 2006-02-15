# PORTB toggling

Uploaded by J_D on 2006-02-15 07:12:00 (rating 0 out of 5)

## Summary

This small assembler code changes PIN state on PORTB.  

Just for practice I wanted to use interrupt from TCNT0.  

The time for PORTB toggling is set for 0,999sek. This can be changed using PRESCALER and OVF\_NR values.  

In my case Timer/Counter is increased each 32us <1/(CLK/256)>, that causes timer overflow each 8,192ms (32us*256). Each overflow is counted.  

Having exactly 1sec for LEDs state changing, the overflow must happened 122times (0x7A). It gives exactly 0,999424sec.


This is my first AVR assembler code, so please for understanding  

Would be appreciate to receive any comments on this or suggestions  

to improve. For instance how to write this code more effectively.  

=================  

uC: AT90S2313  

CLK: 8MHz  

=================  

e-mail: [jdebs@tlen.pl](mailto:jdebs@tlen.pl)

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4

# ATmega32 alarm

Uploaded by SDPfalcon on 2006-03-28 18:45:00 (rating 0 out of 5)

## Summary

I am trying to write some code for an ATmega32. I need it to be checking inputs B0-6 & D0-7 continuously and as soon as there is a change on the input change the output which will be on A0-7 & C0-7. I created some quick tester code that checks half of my i/o port B in D out that works but it uses a wait function wich will not work once in the full alarm. I need to be polling all of the inputs at once and therfore am quite sure that I will need to use for loops with interupts or something like it. I have added the quick test code I created and would appreciate any help.  

PS. this is going to be used on a remote controle car with sensors that sense a line under the car and warns the "driver" if they are deviating from the "lane".

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4

# 1st project - light meter

Uploaded by admin on 2004-02-05 03:44:00 (rating 0 out of 5)

## Summary

First exploration project: 555 timer set up as a monostable with a photoresistor; 8515 code all interrupt driven to:  

- poke the trigger on the circuit  

- run a timer, keep track of overflows to extend the timer beyond 16 bits  

- sample the timer it when the 555 output goes low (using caputure mode)  

- output the formatted timer sample to the UART along with a couple of other pieces of data  

- keep feeding characters to the UART via interrupts until end of the string


It's a digital light meter :)


 Also has STDIO or hand-rolled output formatting. 1.5K w/o STDIO; 4.5K with it. I guess I am going to recode this in assembly - I bet I can squeeze it into 500 bytes...

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)

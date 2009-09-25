# RF communication between AVRs

Uploaded by ashuinfy on 2009-09-25 10:02:00 (rating 0 out of 5)

## Summary

Hi friends there is a sureshot, phodu, simple C program to communicate wirelessly through RF modules


Radio transmission is a bit more complicated than wired communications because you never know what radio signals are present on air. So all matters how transmitted signal is encoded. And this is a part where you have many choices: use hardware encoding like USART or write your own based on one of many ending methods like NRZ, Manchester etc. In my example I have used AVR USART module to form data packs. Using hardware encoders solves many problems like synchronization, start and stop, various signal checks. But as long as I was practicing you cannot rely on plain USART signal. Here you can actually improvise by adding various checks and so on.  

I decided to form 4 byte data packages in order to send one byte information. These include:  

one dummy synchronization byte (10101010);  

one address byte â€“ in case there are more receivers(or transmitters);  

one data byte;  

and checksum which is actually a sum of address and data(address+data).  

Why did I use a dummy byte at the beginning of package. Simply I noticed, that when transmitter doesn't transmit any data â€“ receiver catches various noises that come from power supply or other sources because receiver likes adjust its input gain depending on input signal level. First byte tunes receiver to accept normal signal after then address byte, data and checksum can be read more reliably. Probably with different transmission modules you may exclude this dummy byte.


[http://robolabz.multiply.com/jou...](http://robolabz.multiply.com/journal/item/42/Make_ATMEGA8_microcontrollers_to_work_like_Encoder_Decoder_with_RF_modules_)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)

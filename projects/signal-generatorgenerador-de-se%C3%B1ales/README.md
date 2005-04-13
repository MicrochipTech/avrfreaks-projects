# Signal Generator???||Generador de señales

Uploaded by daedrobu87 on 2005-04-13 15:38:00 (rating 0 out of 5)

## Summary

This code generates diferent signals: Sine, Sawtooth, square and a ramdon signal based on the logistic equation... The Output is the port D, and it should be connected to an DAC(I test it with DAC 0808). The ADC ables to make the signal more or less "bigger" (Amplitude). I used the ADC mux 0, so the entry should be connected to a voltage divisor, and it ables to vary the frequency of operation... The problem of this code is that it makes the conversion just on the begining of the program, so in fact you can´t vary the frequency "live", it must be reseted. 


Dont be optimistic about the use of this generator... its maximum frequency its about 20 Hz... with a crystal of 1MHz. 


I used an ATMEL ATMEGA8535... I woul apreciate to know if someone makes some improvements or make a better generator than mine...:-)


Atte: Daedrobu87  

e-mail: [daedrobu87@gmail.com](mailto:daedrobu87@gmail.com)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain

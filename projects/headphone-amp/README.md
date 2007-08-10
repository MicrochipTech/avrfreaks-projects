# Headphone Amp

Uploaded by Simonetta on 2007-08-10 12:10:00 (rating 0 out of 5)

## Summary

Headphone amp using National LM4832 audio processor that is controlled through I2C from AVR Tiny13. Unique design has one potentiometer and a single pushswitch. On power-up, the pot controls volume. On first switch press, pot controls treble. Next press, pot controls bass level. On third switch press, the pot goes back to controlling volume. The amp retains the parameter settings set by the previous switch modes. AVR sleeps between 1/15 second intervals.  

 An AVR Tiny13 reads the potentiometer 15 times a second and creates the I2C clock and data signals needed to change the settings of the LM4832.  

 I needed a small but loud headphone amp and was given these LM4832 samples at work. None of the old-school engineers could figure out how to control or program an I2C connection. The code shows a simple but effective way to use I2C with AVR assembler.

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler

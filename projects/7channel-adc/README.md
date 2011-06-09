# 7channel ADC

Uploaded by hzrnbgy on 2011-06-09 07:56:00 (rating 0 out of 5)

## Summary

This is a simple firmware for the Attiny861 to read ADC channels 0 to 7 and store it in SRAM. As it stands right now, readings from these channels are stored in SRAM. I haven't included the SPI slave routine to read these data points externally from another MCU. I will work on that in the next few days.


Channels ADC0, ADC1, and ADC2 are reflected on the three PWM channels for visual debugging.


A lower ADC readings would mean a higher duty cycle for each of the PWM channels. I included this to aid in debugging. You can hook up a LED on each of the PWM outputs (PB1, PB3, PB5) and see visually whats read on channels 0, 1, and 2 on the ADC.


ADC channels 0 to 7 are sequentially read every 16ms. You can alter this by changing the prescaler for timer0. Make sure though that the sampling rate not exceed 200khz and leave enough time for some SPI read routine if you intend to read the data off SRAM. They are stored from 0x60 to 0x67.


Reference for the ADC is Vcc. I have included the ASM code if you intend to modify it for your own needs. I used comments extensively to help out.


If you have questions, shoot me an email and I will gladly get back to you as soon as I possibly can.


Attiny861 Setting  

1Mhz clock  

No external component needed

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler

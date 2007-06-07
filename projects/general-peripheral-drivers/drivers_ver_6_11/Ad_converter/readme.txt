

SOFTWARE ADC FUNCTIONS FOR GCC
Written: by Chris Efstathiou
Date:    03/Nov/2002 
Target:  All AVR MPU's
Info:    This code is FREE. There are no copyright issues with this code.
     
This code is an AD Converter driver for the AVR MPU family in GCC.
Configuration is done inside the adc.h file.

IMPORTANT:

1) 
Make sure that you edit the F_CPU field in the makefile to match
the used MCU clock frequency.
Also make sure that you edit the AVRGCC_VERSION field in the makefile
to match the avrgcc version that you use to compile the code.
330=winavr, 320=version 3.20, 302=version 3.02 etc.
This is essential in order to include the correct files.
From version 3.02 (302) and below you can use 302 as the version number.

2)
WHEN MEGA128 IS USED CHECK THE JTAG FUSE!
IF THE JTAG INTERFACE IS ENABLED CHANNELS 4,5,7 WILL MALFUNCTION! 
The same applies in other devices where the JTAG interface or other peripherals
shares the adc pins.

3) 
It is my understanding that if you dont want to burn the AD Converter
never ever connect the ADC inputs to VCC. 
Also do not connect AREF or VCC directly to the ADC inputs.
Use a resistor in series at least 1 Kohm, otherwise you might burn the ADC.
This is particulary true when "ADC_NOISE_REDUCTION" == 1
When "ADC_NOISE_REDUCTION" is set to 1 adc noise is greatly reduced by allowing only the 
current adc channel pin to be an input. All other pins are set as outputs and at 0 state.
This way i am creating a ground shield near the active adc pin.
I REALLY DONT KNOW IF THE AVR PINS ARE OVERCURRENT PROTECTED IF YOU APPLY 5 VOLTS
TO A GROUNDED PIN.
DURING TESTING IT SHOWS THAT THERE IS NO PROBLEM BUT I AM NOT SURE
IF THIS APPLIES IN THE LONG RUN ALSO.
So always use a resistor in series at least 1 Kohm.
You can make a lowpass filter by using a resistor in series followed by a capacitor
connected directly to the adc pin and the AGND.
A good network would be a 8,2 Kohm resistor and 1 nf capacitor which makes a lowpass filter
with a 3db point at ~20 Khz. 

4)
After 2 days of fault finding i decided to let all other bits except the first 3 in ADMUX
intact so you can modify them as you please. The driver won't mess with those bits that
set varius things like the reference source etc. 


Feel free to modify any part you wish but please
give me a note so i can get feedback.



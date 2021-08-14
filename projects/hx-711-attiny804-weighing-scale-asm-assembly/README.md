# HX-711 , Attiny804 weighing scale ,ASM , assembly

Uploaded by darth vader on 2021-08-14 10:50:00 (rating 0 out of 5)

## Summary

HX711 weighing scale


0-5kg load cell 


Attiny 804 MCU


AS7 project with video and schematics attached


language - assembly


;prototype was 5kg Ali express loadcell with HX711 board costing 2-3 us$  

;the load cell reading drifts even with slight change in temperature 40deg - 30deg is 12-15gm drift  

;always re zero by pressing/shorting PA3 to ground for 1 -2 second with weighing tray empty for correct reading  

;as current offset value of the sensor based on the ambient temperature is subtracted from sensor reading  

;to get correct weight.


;to find out how much ADC count is 1 gm I placed car wheel balancing sticker weights which are available in 5gm increments and are fairly accurate


;I got 2000 counts increment on the raw ADC value for each 5 gram weight placed on the sensor. 2000adc count/5gm = 400adc count for 1 gm


;multiplying the RAW ADC COUNT - sensor offset with 1/400 gives the weight. (ACD - offset)x 1/400 = weight measured (1/400 = 0.0025)


; offset handling is automatically done on pressing the re-zero switch for 1 second , remove the weight from the sensor and press re zero switch to calibrate the sensor to zero.


;On doing the above the current ADC value without weight is stored in the SRAM for offset calculation. If abnormal values are displayed due to temperature drift always re zero.


;more modifications can be done on the code to print messages, disable underflow when sensor drifts etc which is not included


;finally thanks for the conversion & math code snippets taken from AVR freaks website.


;I have also posted another code named HX711\_raw\_data\_reader that displays raw data from HX711 to LCD which can be used to find the raw ADC counts for a known weight


;which can be used to find the ADC count per 1 gram. 


;HX711.zip is full weighing scale code


;HX711\_raw\_data\_reader is for finding the ADC counts for 1 gm on a new load cell, adc count calculated is used as divisor in HX711.zip program to run the weighing machine calculations


Thanks


Darth vader

## Compilers

- Atmel Studio 7

## Tags

- Complete code
- Atmel Studio 7
- HX-711
- Attiny804 weighing scale
- ASM
- assembly
- asm
- assembly
- Home Appliances
- HX-711
- Attiny804 weighing scale

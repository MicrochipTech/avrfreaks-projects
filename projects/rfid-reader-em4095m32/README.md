# RFID reader (EM4095+M32...)

Uploaded by dzairo on 2010-08-06 13:52:00 (rating 0 out of 5)

## Summary

Hi all.  

Here is my code for read RFID card.  

Use EM4095 IO and card with standard EM4102, EM4100... and clone.  

Card use manchester code with 64clk/bit.


On Internet is more free code but some don't work good.  

MCU is used M16,M32 or M8 (make some changes then can use all what have T0 input).


Code is fast and can work in real-time if use 16Mhz crystal.  

1. Every 32clock pulse read data pin, end real time decode manchester code to normal data.  

2. All data save in RFbuffer and rotate this data for one bit .  

 But don't rotate for 8bit per byt (there is it) , only 5bit per byt.  

 See RollData function.  

3. Separate all half byt of data with parity .  

 Data are in 4,3,2,1 bit position and parity is 0bitposition  

4. Check header and stop bit , if OK then can control parity.  

5. Calculate line (horizontal) parity.  

 Read all half data byt with parity and compare with some constants.  

 if Data with parity is equal, then data are OK ,if not then some  

 wrong. It's easy ,work fast and is good for understand.  

6. Calculate column (vertical) parity.  

 This is very primitive and easy too. XOR is magic function.  

 Read all data from buffer and make XOR with next data from buffer.  

 On finish make XOR with parity and if result is 0 that's mean so data  

 are OK (before must clear last bit - this columns are parity ), if  

 result some number that's mean so have corrupt data and can read  

 again data.  

7. IF data on RFbuffer are good then clean header  

8. Copy data from RFbuffer in to user defined buffer (with out of parity  

 bit).


That all.  

If use crystal minimal 16MHz than don't need stop T0 interupt and all work on real time.


PLS. test it and write my your suggestion or if find any bug write too .  

regards


PS: thanks clawson for help.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4

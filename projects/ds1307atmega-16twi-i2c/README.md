# DS1307_atmega 16_TWI (I2c)

Uploaded by BBh on 2012-08-06 14:14:00 (rating 0 out of 5)

## Summary

Description : RTC DS1307  

 Input : Pc0=SDA Pc1 =SDA TWI PROTOCOL  

 Output : SQW OUT =32.768 Khz PIN 7 RTC 


0xd0 equ ds1307 is in write mode. (TWI master\_transmitte\_mode)  

0xd1 equ ds1307 is in read mode. (TWI master\_receiving\_mode)


application of power to the device the time and date registers are  

typically reset to 01/01/00 01 00:00:00  

Please note that the initial power-on state of all registers is not defined.  

Therefore it is important to enable the oscillator (CH bit = 0)  

 during initial configuration 


TESTFUNKTIONS:


test of sqw output,setup frq out 32Khz ok use to check ds1307 is a live.  

test of clok halt osc bit 7 register adress 00h work when set to off or on. (USE A SCOP TO CHECK AND SE IT WORKS)


hardware connect:  

RTC ON PORT C (TWI)  

1=X-TAL  

2=X-TAL  

3=Vbat SET TO GND dont use backup bat ((importen))  

4=GND  

5=SDA Pc1 TWI note 10k pull up to Vcc  

6=SCL Pc0 TWI note 10k pul up to Vcc  

7=SQW OUT  

8=Vcc +5V

## Compilers

- AVR Studio 4

## Tags

- General Lib.functions
- AVR Studio 4

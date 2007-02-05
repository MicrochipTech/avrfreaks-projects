# EM4095 RFID Reader using Atmega8

Uploaded by admin on 2007-02-05 11:06:00 (rating 0 out of 5)

## Summary

I was porting EM4095 source code from other EM4095 source.  

I was found much error in their source code.  

Ofcourse I have been complete debugging after modified it.  

Must be correct running in AVR ATMeag8 MPU.


Hardware circuit is included in attached file by jpg image file.  

How to make antenna coil?  

I am using urethan coated copper wire of 0.24mm diameter  

110 turns in 70 x 70 mm square shape. inductance is 1400uH.  

But, If you like less than 40 turns coil (about 180 ~ 200uH),  

Ofcourse you can do it,In this case,  

You able to R1 change to 20 ohm, and C-res change to 10nF.  

If you have an oscilloscope, Make sure antenna voltage under 100 Vp-p .


Have a funny weekend !!!  

wrote leeky from Republic of Korea.  

Regards 


project : EM4095 RFID Reader V2.0  

Target : MEGA8-16AU  

Crystal: 14.7456 Mhz  

Input : EM4095 RFID 125KHz  

Output : RS232C 115200,N,8,1  

Compiler : ICC-AVR V6.26C  

Author. : leeky  

Email : [info@ledshop.co.kr](mailto:info@ledshop.co.kr)


Pin configuration for ATMEGA8-16AU TQFP32  

#30 PD0/RXD <--- RXD (RS232C)  

#31 PD1/TXD ---> TXD (RS232C)  

# 9 PD5/T1 <--- TIMER1 EXT-T1 <--- RDY-CLK (EM4095)  

#10 PD6 ---> SHD (EM4095)  

#11 PD7 ---> MOD (EM4095)  

#12 PB0/ICAP <--- TIMER1 ICAP <--- DEMOD (EM4095)  

#13 PB1 ---> LED (READY)  

#14 PB2 ---> BUZZER (BEEP)  

#15 PB3 ---> RELAY (DOOR OPEN)


Read only Transmitter Data Format (ASCII)  

LF+ 10 of Nibbles HEXA + CR  

HEXA are 10 digit of '0' to 'F' by ASCII  

Example : LF+0461167A89+CR (My ID card)


EM type (Read only RFID Card) output data format  

1111 11111 = Header = 9 bit  

\_\_\_\_\_\_--\_\_--\_\_--\_\_--\_\_--\_\_--\_\_--\_\_--\_\_-- (wave)  

0-0 0-1 0-1 0-1 0-1 0-1 0-1 0-1 0-1 0-1 (edge)  

(x)<>(1)=(1)=(1)=(1)=(1)=(1)=(1)=(1)=(1) (code)


Custom-ID (10 bits)  

--\_\_--\_\_--\_\_--\_\_--\_\_--\_\_\_\_----\_\_--\_\_\_\_-- (wave)  

1-0 1-0 1-0 1-0 1-0 1-0 0-1 1-0 1-0 0-1 (edge)  

(0)=(0)=(0)=(0)=(0)=(0)<>(1)<>(0)=(0)<>(1) (code)  

 D3 = D2 = D1 = D0 = Pr = D3 <> D2 <> D1 = D0 <> Pr (Pr =parity)


Notes :  

This source able to using AVR MPU study or personal project.  

Please do not use for commercial products.  

If you have to development more improved RFID system,  

or if you have any question, Please email me Name is 'leeky'  

Email to : [info@ledshop.co.kr](mailto:info@ledshop.co.kr)

## Compilers

- ImageCraft ICCAVR V6

## Tags

- Complete code with HW
- ImageCraft ICCAVR V6
